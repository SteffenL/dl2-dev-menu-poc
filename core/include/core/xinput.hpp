#pragma once

#include "core/log.hpp"
#include "core/string.hpp"
#include "core/system.hpp"

#include <string>

#include <windows.h>

inline std::filesystem::path getXinput13Path() {
    return getSystemDir() / "xinput1_3.dll";
}

struct XINPUT_VIBRATION;
struct XINPUT_STATE;
struct XINPUT_CAPABILITIES;
struct XINPUT_BATTERY_INFORMATION;
struct XINPUT_KEYSTROKE;
using PXINPUT_KEYSTROKE = XINPUT_KEYSTROKE*;

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
        log("Loading xinput DLL: ", charStringFromChar8String(origDllPath.u8string()));
        auto module{::LoadLibraryExW(origDllPath.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)};
        if (!module) {
            log("Unable to load xinput DLL.");
        }
        return module;
    }

    HMODULE m_handle{};
};

struct XInput1_3;

inline XInput1_3& getXinput() {
    static std::unique_ptr<XInput1_3> instance;
    if (!instance) {
        instance = std::make_unique<XInput1_3>();
    }
    return *instance;
}
