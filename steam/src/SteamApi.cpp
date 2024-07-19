#include "steam/SteamApi.hpp"
#include "steam/ScopedSteamApiInit.hpp"

#include <steam/steam_api.h>

class SteamApi::Impl {
public:
    Impl(unsigned int appId) : m_scope{appId} {
        m_steamApps = ::SteamApps();
    }

    static bool isSteamRunning() noexcept {
        return ::SteamAPI_IsSteamRunning();
    }

    bool isOk() const noexcept {
        return m_scope.isOk();
    }

    bool isAppInstalled(unsigned int appId) const noexcept {
        return m_steamApps->BIsAppInstalled(appId);
    }

    std::filesystem::path getAppInstallDir(unsigned int appId) const noexcept {
        char cBuffer[1024]{};
        const auto cActualLength{m_steamApps->GetAppInstallDir(appId, cBuffer, sizeof(cBuffer))};

        if (cActualLength == 0) {
            return {};
        }

        char* cBufferEnd = cBuffer + cActualLength;
        while (cBufferEnd > cBuffer && *cBufferEnd == 0) {
            --cBufferEnd;
        }

        ++cBufferEnd;

        return std::filesystem::u8path(cBuffer, cBufferEnd);
    }

private:
    ScopedSteamApiInit m_scope;
    ISteamApps* m_steamApps{};
};

SteamApi::SteamApi(unsigned int appId) : m_impl{std::make_unique<Impl>(appId)} {}
SteamApi::~SteamApi() {}

bool SteamApi::isSteamRunning() noexcept {
    return Impl::isSteamRunning();
}

bool SteamApi::isOk() const noexcept {
    return m_impl->isOk();
}

bool SteamApi::isAppInstalled(unsigned int appId) const noexcept {
    return m_impl->isAppInstalled(appId);
}

std::filesystem::path SteamApi::getAppInstallDir(unsigned int appId) const noexcept {
    return m_impl->getAppInstallDir(appId);
}
