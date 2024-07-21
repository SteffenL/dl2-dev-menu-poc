#include "core/hooks.hpp"
#include "core/game.hpp"
#include "core/string.hpp"

#include "core/log.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "core/xinput.hpp"

#include <filesystem>

#include <windows.h>
#include <MinHook.h>

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    _Field_size_bytes_part_opt_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

using LdrLoadDll_t = NTSTATUS(NTAPI*)(
    _In_opt_ PWSTR DllPath,
    _In_opt_ PULONG DllCharacteristics,
    _In_ PUNICODE_STRING DllName,
    _Out_ PVOID* DllHandle);

LdrLoadDll_t g_origLdrLoadDll{};

using GetProcAddress_t = FARPROC(WINAPI*)(HMODULE hModule, LPCSTR lpProcName);
GetProcAddress_t g_origGetProcAddress{};

NTSTATUS NTAPI detourLdrLoadDll(PWSTR DllPath, PULONG DllCharacteristics, PUNICODE_STRING DllName, PVOID* DllHandle) {
    const std::wstring_view libName{DllName->Buffer, DllName->Length / sizeof(DllName->Buffer[0])};
    auto libPath{std::filesystem::path{libName}.lexically_normal()};
    log("LdrLoadDll: ", charStringFromChar8String(libPath.u8string()));
    if (libPath.filename() == "gamedll_ph_x64_rwe.dll") {
        log("Game DLL is being loaded.");
        auto result{g_origLdrLoadDll(DllPath, DllCharacteristics, DllName, DllHandle)};
        AppState::get().notifyOnGameDllLoaded();
        return result;
    }
    // Try to load DLL with provided path
    auto result{g_origLdrLoadDll(DllPath, DllCharacteristics, DllName, DllHandle)};
    if (result == 0) {
        return result;
    }
    log("DLL failed to load: ", charStringFromChar8String(libPath.u8string()));
    if (libPath.has_root_path()) {
        // Might be rooted in the wrong directory
        try {
            const auto canonicalLibPath{std::filesystem::weakly_canonical(libPath)};
            const auto relPath{std::filesystem::relative(libPath, getExeDir())};
            libPath = getMainBinDir() / relPath;
        } catch (const std::filesystem::filesystem_error& ex) {
            log("Filesystem error: ", ex.what());
            return result;
        }
    } else {
        // Non-rooted might be in main game dir
        libPath = getMainBinDir() / libPath;
    }
    log("Attempting to redirect to DLL: ", charStringFromChar8String(libPath.u8string()));
    const auto newPath{(getMainBinDir() / libPath.filename()).wstring()};
    const auto newLengthInBytes{static_cast<USHORT>(newPath.size() * sizeof(DllName->Buffer[0]))};
    UNICODE_STRING newPathUnicode{
        .Length = newLengthInBytes,
        .MaximumLength = newLengthInBytes,
        .Buffer = const_cast<wchar_t*>(newPath.c_str())};
    result = g_origLdrLoadDll(DllPath, DllCharacteristics, &newPathUnicode, DllHandle);
    if (result == 0) {
        return result;
    }
    log("DLL still failed to load: ", charStringFromChar8String(libPath.u8string()));
    return result;
}

FARPROC WINAPI detourGetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
    auto modulePath{getModulePath(hModule)};
    if (hModule == AppState::get().getDllHandle()) {
        auto fixedModulePath{getXinput13Path()};
        // Temporarily disabled due to spam
        //log("Shenanigan: GetProcAddress was used on us. Redirecting to module: ", charStringFromChar8String(fixedModulePath.u8string()));
        hModule = std::bit_cast<HMODULE>(getModuleHandle(fixedModulePath));
        return g_origGetProcAddress(hModule, lpProcName);
    }
    return g_origGetProcAddress(hModule, lpProcName);
}

void setupGameDllLoadHook() {
    if (::MH_Initialize() != MH_OK) {
        log("Unable to initialize MinHook.");
        return;
    }
    if (!([]() -> bool {
            const auto ntdllPath{getSystemDir() / "ntdll.dll"};
            if (auto ntdllHandle{::GetModuleHandleW(ntdllPath.c_str())}) {
                if (auto fnLdrLoadDll{g_origGetProcAddress ? g_origGetProcAddress(ntdllHandle, "LdrLoadDll") : ::GetProcAddress(ntdllHandle, "LdrLoadDll")}) {
                    if (::MH_CreateHook(fnLdrLoadDll, &detourLdrLoadDll, std::bit_cast<LPVOID*>(&g_origLdrLoadDll)) == MH_OK) {
                        if (::MH_EnableHook(fnLdrLoadDll) == MH_OK) {
                            return true;
                        }
                    }
                }
            }
            return false;
        })()) {
        log("Unable to hook LdrLoadDll.");
    }
    if (!([]() -> bool {
            if (::MH_CreateHook(&::GetProcAddress, &detourGetProcAddress, std::bit_cast<LPVOID*>(&g_origGetProcAddress)) == MH_OK) {
                if (::MH_EnableHook(std::bit_cast<LPVOID>(&::GetProcAddress)) == MH_OK) {
                    return true;
                }
            }
            return false;
        })()) {
        log("Unable to hook GetProcAddress.");
    }
}
