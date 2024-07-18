#include "core/xinput.hpp"

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
