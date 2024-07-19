#pragma once

#include <filesystem>
#include <memory>

class SteamApi {
public:
    SteamApi(unsigned int appId = 480);
    ~SteamApi();
    static bool isSteamRunning() noexcept;
    bool isOk() const noexcept;
    bool isAppInstalled(unsigned int appId) const noexcept;
    std::filesystem::path getAppInstallDir(unsigned int appId) const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};
