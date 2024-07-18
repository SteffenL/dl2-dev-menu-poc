#include "core/hooks.hpp"

#include "core/log.hpp"
#include "core/state.hpp"
#include "core/xinput.hpp"

#include <windows.h>
#include <MinHook.h>

using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
LoadLibraryExW_t g_origLoadLibraryExW{};

using GetProcAddress_t = FARPROC(WINAPI*)(HMODULE hModule, LPCSTR lpProcName);
GetProcAddress_t g_origGetProcAddress{};

HMODULE WINAPI detourLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    std::filesystem::path libfileName{lpLibFileName};
    log("LoadLibraryExW: ", libfileName.string());
    if (libfileName.filename() == "gamedll_ph_x64_rwe.dll") {
        log("Game DLL is being loaded.");
        auto module{g_origLoadLibraryExW(lpLibFileName, hFile, dwFlags)};
        AppState::get().notifyOnGameDllLoaded();
        return module;
    }
    return g_origLoadLibraryExW(lpLibFileName, hFile, dwFlags);
}

FARPROC WINAPI detourGetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
    auto modulePath{getModulePath(hModule)};
    if (hModule == AppState::get().getDllHandle()) {
        auto fixedModulePath{getXinput13Path()};
        log("Shenanigan: GetProcAddress was used on us. Redirecting to module: ", fixedModulePath.string());
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
            if (::MH_CreateHook(std::bit_cast<LPVOID>(&::LoadLibraryExW), std::bit_cast<LPVOID>(&detourLoadLibraryExW), std::bit_cast<LPVOID*>(&g_origLoadLibraryExW)) == MH_OK) {
                if (::MH_EnableHook(std::bit_cast<LPVOID>(&::LoadLibraryExW)) == MH_OK) {
                    return true;
                }
            }
            return false;
        })()) {
        log("Unable to hook LoadLibraryW.");
    }
    if (!([]() -> bool {
            if (::MH_CreateHook(std::bit_cast<LPVOID>(&::GetProcAddress), std::bit_cast<LPVOID>(&detourGetProcAddress), std::bit_cast<LPVOID*>(&g_origGetProcAddress)) == MH_OK) {
                if (::MH_EnableHook(std::bit_cast<LPVOID>(&::GetProcAddress)) == MH_OK) {
                    return true;
                }
            }
            return false;
        })()) {
        log("Unable to hook LoadLibraryW.");
    }
}
