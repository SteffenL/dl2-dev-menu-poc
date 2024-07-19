#include "steam/ScopedSteamApiInit.hpp"

#include <string>

#include <steam/steam_api.h>
#include <windows.h>

ScopedSteamApiInit::ScopedSteamApiInit(unsigned int appId) {
    const auto appIdString{std::to_wstring(appId)};
    ::SetEnvironmentVariableW(L"SteamAppId", appIdString.c_str());

    if (::SteamAPI_Init()) {
        m_ok = true;
    }
}

ScopedSteamApiInit::~ScopedSteamApiInit() {
    if (m_ok) {
        ::SteamAPI_Shutdown();
    }
}

bool ScopedSteamApiInit::isOk() const noexcept {
    return m_ok;
}
