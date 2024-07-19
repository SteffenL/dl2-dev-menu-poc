#include "steam_game/integration.hpp"

#include "core/registry.hpp"
#include "core/system.hpp"
#include "steam/SteamApi.hpp"

#include <string>

constexpr const unsigned int gameSteamAppId{534380};

std::optional<std::filesystem::path> getSteamInstallDir() {
    RegKey reg{HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_READ | KEY_WOW64_32KEY};
    if (auto installPath{reg.queryString(L"InstallPath")}) {
        return std::make_optional(*installPath);
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> getSteamAppsDir() {
    if (auto appsDir{getSteamInstallDir()}) {
        return std::make_optional(*appsDir / "steamapps" / "common");
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> getGameInstallDirFromRegistry(unsigned int appId) {
    const auto subKey{L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App " + std::to_wstring(appId)};
    RegKey reg{HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ | KEY_WOW64_32KEY};
    if (auto installPath{reg.queryString(L"InstallPath")}) {
        return std::make_optional(*installPath);
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> getGameInstallDir() {
    if (SteamApi::isSteamRunning()) {
        SteamApi steam;
        if (steam.isOk()) {
            if (steam.isAppInstalled(gameSteamAppId)) {
                return {steam.getAppInstallDir(gameSteamAppId)};
            }
        }
    }

    if (auto installDirFromReg{getGameInstallDirFromRegistry(gameSteamAppId)}) {
        return installDirFromReg;
    }

    if (auto steamDir{getSteamAppsDir()}) {
        return std::make_optional(*steamDir / "Dying Light 2");
    }

    return std::nullopt;
}

std::optional<std::filesystem::path> getGameBinDir() {
    if (auto installDir{getGameInstallDir()}) {
        return std::make_optional(*installDir / "ph" / "work" / "bin" / "x64");
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> getGameDevToolsBinDir() {
    if (auto gameDir{getGameInstallDir()}) {
        return std::make_optional(*gameDir / "DevTools" / "bin");
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> getGameDevToolsExePath() {
    if (auto binDir{getGameDevToolsBinDir()}) {
        return std::make_optional(*binDir / "dyinglightgame_x64_rwe.exe");
    }
    return std::nullopt;
}