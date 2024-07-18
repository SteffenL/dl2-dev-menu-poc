#define LOG_TAG "[sl_dl2_dev_menu] "

struct XINPUT_VIBRATION;
struct XINPUT_STATE;
struct XINPUT_CAPABILITIES;
struct XINPUT_BATTERY_INFORMATION;
struct XINPUT_KEYSTROKE;
using PXINPUT_KEYSTROKE = XINPUT_KEYSTROKE*;

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <span>
#include <string>
#include <type_traits>

#include <MinHook.h>

#include <windows.h>
#include <dbghelp.h>

constexpr bool isValidNibble(char c) noexcept {
    if (c >= '0' && c <= '9') {
        return true;
    }
    if (c >= 'a' && c <= 'f') {
        return true;
    }
    if (c >= 'A' && c <= 'F') {
        return true;
    }
    return false;
}

constexpr uint8_t decodeNibble(char c) {
    if (c >= '0' && c <= '9') {
        return static_cast<uint8_t>(c - '0');
    }
    if (c >= 'a' && c <= 'f') {
        return static_cast<uint8_t>(c - 'a' + 10);
    }
    if (c >= 'A' && c <= 'F') {
        return static_cast<uint8_t>(c - 'A' + 10);
    }
    throw std::logic_error{"Invalid nibble"};
}

constexpr size_t getEffectiveCharCountInPattern(const std::string_view pattern) {
    size_t count{};
    for (auto it{pattern.begin()}; it != pattern.end(); ++it) {
        if (*it == '?') {
            ++count;
            continue;
        }
        if (*it == ' ') {
            continue;
        }
        if (!isValidNibble(*it)) {
            throw std::logic_error{"Invalid nibble"};
        }
        ++it;
        if (it == pattern.end()) {
            throw std::logic_error{"Incomplete pattern"};
        }
        if (!isValidNibble(*it)) {
            throw std::logic_error{"Invalid nibble"};
        }
        ++count;
    }
    return count;
}

template<typename T>
constexpr auto search(const std::span<T> range, const std::string_view pattern) -> typename decltype(range)::iterator {
    auto rbegin{range.begin()};
    const auto rend{range.end()};
    const auto pbegin{pattern.begin()};
    const auto pend(pattern.end());
    auto rit{rbegin};
    for (auto pit{pbegin}; pit != pattern.end();) {
        if (rit == rend) {
            return rend;
        }
        if (*pit == ' ') {
            ++pit;
            continue;
        }
        if (*pit == '?') {
            ++rit;
            ++pit;
            continue;
        }
        auto h{decodeNibble(*pit)};
        ++pit;
        if (pit == pend) {
            throw std::logic_error{"Incomplete pattern"};
        }
        auto l{decodeNibble(*pit)};
        auto byte{static_cast<uint8_t>(h << 4U | l)};
        if (*rit != byte) {
            rit = ++rbegin;
            pit = pbegin;
            continue;
        }
        ++rit;
        ++pit;
    }
    if (rbegin == rend) {
        return rend;
    }
    auto matchCount{std::distance(rbegin, rit)};
    if (matchCount != getEffectiveCharCountInPattern(pattern)) {
        return rend;
    }
    return rbegin;
}

HMODULE getModuleHandle(const std::filesystem::path& moduleName) {
    return ::GetModuleHandleW(moduleName.c_str());
}

std::filesystem::path getModulePath(HMODULE handle) {
    std::array<WCHAR, MAX_PATH + 1> exePath{};
    auto length{::GetModuleFileNameW(handle, exePath.data(), exePath.size())};
    return std::filesystem::path{exePath.begin(), exePath.begin() + length};
}

std::span<uint8_t> getImageCode(const std::filesystem::path& moduleName) {
    auto* imageBase{std::bit_cast<uint8_t*>(getModuleHandle(moduleName))};
    if (!imageBase) {
        throw std::runtime_error{"Unable to get handle of module by name: " + moduleName.string()};
    }
    auto* ntHeaders{::ImageNtHeader(imageBase)};
    auto* baseOfCode{imageBase + ntHeaders->OptionalHeader.BaseOfCode};
    auto sizeOfCode{ntHeaders->OptionalHeader.SizeOfCode};
    return std::span<uint8_t>{baseOfCode, baseOfCode + sizeOfCode};
}

std::filesystem::path getSystemDir() {
    std::array<WCHAR, MAX_PATH + 1> systemDir{};
    auto length{::GetSystemDirectoryW(systemDir.data(), systemDir.size())};
    return std::filesystem::path{systemDir.begin(), systemDir.begin() + length};
}

std::filesystem::path getExePath() {
    return getModulePath(nullptr);
}

std::filesystem::path getMainBinDir() {
    return getExePath().parent_path().parent_path().parent_path() / "ph" / "work" / "bin" / "x64";
}

std::filesystem::path getXinput13Path() {
    return getSystemDir() / "xinput1_3.dll";
}

std::unique_ptr<std::ostream> g_logStream;
HMODULE g_dllHandle{};

template<typename... Args>
void log(Args&&... args) {
    if (!g_logStream) {
        auto logFilePath{getExePath().parent_path() / "sl_dl2_dev_menu.txt"};
        g_logStream = std::make_unique<std::ofstream>(logFilePath);
    }
    *g_logStream << LOG_TAG;
    ((*g_logStream << std::forward<Args>(args)), ...);
    *g_logStream << std::endl;
}

struct XInput1_3 {
    using XInputGetState_t = DWORD(WINAPI*)(DWORD dwUserIndex, XINPUT_STATE* pState);
    using XInputSetState_t = DWORD(WINAPI*)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
    using XInputGetCapabilities_t = DWORD(WINAPI*)(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
    using XInputEnable_t = void(WINAPI*)(BOOL enable);
    using XInputGetDSoundAudioDeviceGuids_t = DWORD(WINAPI*)(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
    using XInputGetBatteryInformation_t = DWORD(WINAPI*)(DWORD dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
    using XInputGetKeystroke_t = DWORD(WINAPI*)(DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke);
    using XInputUnknown100_t = DWORD(WINAPI*)(DWORD dwUserIndex, XINPUT_STATE* pState);
    using XInputUnknown101_t = DWORD(WINAPI*)(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid);
    using XInputUnknown102_t = DWORD(WINAPI*)(DWORD dwUserIndex);
    using XInputUnknown103_t = DWORD(WINAPI*)(DWORD dwUserIndex);

    explicit XInput1_3(HMODULE handle)
        : m_handle{handle},
          XInputGetState{getSymbol<XInputGetState_t>(handle, "XInputGetState")},
          XInputSetState{getSymbol<XInputSetState_t>(handle, "XInputSetState")},
          XInputGetCapabilities{getSymbol<XInputGetCapabilities_t>(handle, "XInputGetCapabilities")},
          XInputEnable{getSymbol<XInputEnable_t>(handle, "XInputEnable")},
          XInputGetDSoundAudioDeviceGuids{getSymbol<XInputGetDSoundAudioDeviceGuids_t>(handle, "XInputGetDSoundAudioDeviceGuids")},
          XInputGetBatteryInformation{getSymbol<XInputGetBatteryInformation_t>(handle, "XInputGetBatteryInformation")},
          XInputGetKeystroke{getSymbol<XInputGetKeystroke_t>(handle, "XInputGetKeystroke")},
          XInputUnknown100{getSymbol<XInputUnknown100_t>(handle, 100)},
          XInputUnknown101{getSymbol<XInputUnknown101_t>(handle, 101)},
          XInputUnknown102{getSymbol<XInputUnknown102_t>(handle, 102)},
          XInputUnknown103{getSymbol<XInputUnknown103_t>(handle, 103)} {}

    XInput1_3() : XInput1_3(loadXinput13Dll()) {}

    XInputSetState_t XInputSetState{};
    XInputGetState_t XInputGetState{};
    XInputGetCapabilities_t XInputGetCapabilities{};
    XInputEnable_t XInputEnable{};
    XInputGetDSoundAudioDeviceGuids_t XInputGetDSoundAudioDeviceGuids{};
    XInputGetBatteryInformation_t XInputGetBatteryInformation{};
    XInputGetKeystroke_t XInputGetKeystroke{};
    XInputUnknown100_t XInputUnknown100{};
    XInputUnknown101_t XInputUnknown101{};
    XInputUnknown102_t XInputUnknown102{};
    XInputUnknown103_t XInputUnknown103{};

private:
    template<typename T>
    static T getSymbol(HMODULE handle, const std::string& name) {
        if (!handle) {
            throw std::runtime_error{"XInput1_3::getSymbol: Module is null"};
        }
        auto fn{reinterpret_cast<T>(::GetProcAddress(handle, name.c_str()))};
        if (!fn) {
            throw std::runtime_error{"XInput1_3::getSymbol: Missing symbol: " + name};
        }
        return fn;
    }

    template<typename T>
    static T getSymbol(HMODULE handle, int ordinal) {
        if (!handle) {
            throw std::runtime_error{"XInput1_3::getSymbol: Module is null"};
        }
        auto fn{reinterpret_cast<T>(::GetProcAddress(handle, std::bit_cast<LPCSTR>(static_cast<intptr_t>(ordinal))))};
        if (!fn) {
            throw std::runtime_error{"XInput1_3::getSymbol: Missing symbol: @" + std::to_string(ordinal)};
        }
        return fn;
    }

    static HMODULE loadXinput13Dll() {
        auto origDllPath{getXinput13Path()};
        log("Loading xinput DLL: ", origDllPath.string());
        auto module{::LoadLibraryExW(origDllPath.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)};
        if (!module) {
            log("Unable to load xinput DLL.");
        }
        return module;
    }

    HMODULE m_handle{};
};

struct XInput1_3;

XInput1_3& getXinput() {
    static std::unique_ptr<XInput1_3> instance;
    if (!instance) {
        instance = std::make_unique<XInput1_3>();
    }
    return *instance;
}

template<typename T>
std::invoke_result_t<T> catch_guard(T fn) {
    try {
        return fn();
    } catch (const std::exception& ex) {
        log("Error: ", ex.what());
        std::abort();
    } catch (...) {
        log("Unknown error.");
        std::abort();
    }
}

template<typename T>
std::string hexNumber(T value, const std::string_view prefix = "0x") noexcept {
    std::stringstream os;
    if (!prefix.empty()) {
        os.write(prefix.data(), prefix.size());
    }
    os << std::hex << value;
    return os.str();
}

template<typename T>
bool* findCheatsEnabledVariable(const std::span<T> range) {
    /*
    .text:0000000181C63383 E8 68 17 13 00                                call    sub_181D94AF0
    .text:0000000181C63388 84 C0                                         test    al, al
    .text:0000000181C6338A 0F 95 C0                                      setnz   al
    .text:0000000181C6338D EB 0D                                         jmp     short loc_181C6339C
    .text:0000000181C6338F                               ; ---------------------------------------------------------------------------
    .text:0000000181C6338F
    .text:0000000181C6338F                               loc_181C6338F:                          ; CODE XREF: sub_181C62AB0+16F↑j
    .text:0000000181C6338F                                                                       ; sub_181C62AB0+188↑j
    .text:0000000181C6338F C6 05 64 4C 0C 02 01                          mov     cs:sl__var__bool__cheats_enabled, 1 ; Enable cheats via UseDeveloperKeys
    */
    auto it{search(range, "E868171300 84C0 0F95C0 EB0D C605????01")};
    if (it == range.end()) {
        log("\"Cheats enabled\" pattern not found.");
        return nullptr;
    }
    auto* patternAddress{range.data() + std::distance(range.begin(), it)};
    patternAddress += 12;
    log("\"Cheats enabled\" instruction found at " + hexNumber(std::bit_cast<uintptr_t>(patternAddress)));
    auto relAddr{*std::bit_cast<uint32_t*>(patternAddress + 2 /*offset of rel addr*/)};
    log("\"Cheats enabled\" variable relative address: " + hexNumber(relAddr));
    auto absAddr{std::bit_cast<uintptr_t>(patternAddress + 7 /*length of instruction*/) + relAddr};
    log("\"Cheats enabled\" variable absolute address: " + hexNumber(absAddr));
    return std::bit_cast<bool*>(absAddr);
}

void setupCheats() {
    log("Finding code of game DLL.");
    auto code{getImageCode("gamedll_ph_x64_rwe.dll")};
    if (code.empty()) {
        log("Code of game DLL not found.");
        return;
    }
    log("Base of code: ", hexNumber(std::bit_cast<uintptr_t>(code.data())));
    auto* cheatsEnabled{findCheatsEnabledVariable(code)};
    if (!cheatsEnabled) {
        log("\"Cheats enabled\" variable not found.");
        return;
    }
    log("\"Cheats enabled\" variable found at " + hexNumber(std::bit_cast<uintptr_t>(cheatsEnabled)));
    log("Enabling cheats.");
    *cheatsEnabled = true;
}

bool isGameDllLoaded() {
    return !!getModuleHandle("gamedll_ph_x64_rwe.dll");
}

void onGameDllLoaded() {
    setupCheats();
}

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
        onGameDllLoaded();
        return module;
    }
    return g_origLoadLibraryExW(lpLibFileName, hFile, dwFlags);
}

FARPROC WINAPI detourGetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
    auto modulePath{getModulePath(hModule)};
    if (hModule == g_dllHandle) {
        auto fixedModulePath{getXinput13Path()};
        log("Shenanigan: GetProcAddress was used on us. Redirecting to module: ", fixedModulePath.string());
        hModule = getModuleHandle(fixedModulePath);
        return g_origGetProcAddress(hModule, lpProcName);
    }
    return g_origGetProcAddress(hModule, lpProcName);
}

void setSteamAppId() {
    ::SetEnvironmentVariableA("SteamAppId", "534380");
}

void setWorkingDirectory() {
    auto workingDir{getMainBinDir()};
    log("Changing working directory: ", workingDir.string());
    ::SetCurrentDirectoryW(workingDir.c_str());
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

void setup() {
    log("Disclaimer: This is only a proof of concept for using the developer menu in Dying Light 2 (tested v1.17.2).");
    log("Author: Steffen André Langnes - www.steffenl.com");
    log("Starting up.");

    const auto exePath{getExePath()};
    if (exePath.filename() != "dyinglightgame_x64_rwe.exe") {
        log("Unexpected game EXE: ", exePath.string());
    }

    setSteamAppId();
    setWorkingDirectory();
    setupGameDllLoadHook();

    if (isGameDllLoaded()) {
        onGameDllLoaded();
    }
}

void shutdown() { log("Shutting down."); }

extern "C" DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
    return getXinput().XInputGetState(dwUserIndex, pState);
}

extern "C" DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration) {
    return getXinput().XInputSetState(dwUserIndex, pVibration);
}

extern "C" DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities) {
    return getXinput().XInputGetCapabilities(dwUserIndex, dwFlags, pCapabilities);
}

extern "C" void WINAPI XInputEnable(BOOL enable) {
    return getXinput().XInputEnable(enable);
}

extern "C" DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid) {
    return getXinput().XInputGetDSoundAudioDeviceGuids(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
}

extern "C" DWORD WINAPI XInputGetBatteryInformation(DWORD dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation) {
    return getXinput().XInputGetBatteryInformation(dwUserIndex, devType, pBatteryInformation);
}

extern "C" DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke) {
    return getXinput().XInputGetKeystroke(dwUserIndex, dwReserved, pKeystroke);
}

extern "C" DWORD WINAPI XInputUnknown100(DWORD dwUserIndex, XINPUT_STATE* pState) {
    return getXinput().XInputUnknown100(dwUserIndex, pState);
}

extern "C" DWORD WINAPI XInputUnknown101(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid) {
    return getXinput().XInputUnknown101(dwUserIndex, dwFlag, pVoid);
}

extern "C" DWORD WINAPI XInputUnknown102(DWORD dwUserIndex) {
    return getXinput().XInputUnknown102(dwUserIndex);
}

extern "C" DWORD WINAPI XInputUnknown103(DWORD dwUserIndex) {
    return getXinput().XInputUnknown103(dwUserIndex);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    g_dllHandle = hinstDLL;
    return catch_guard([=] {
        if (fdwReason == DLL_PROCESS_ATTACH) {
            ::DisableThreadLibraryCalls(hinstDLL);
            setup();
        } else if (fdwReason == DLL_PROCESS_DETACH) {
            shutdown();
        }
        return TRUE;
    });
}
