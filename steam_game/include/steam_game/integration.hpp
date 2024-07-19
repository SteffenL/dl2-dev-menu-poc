#pragma once

#include <filesystem>
#include <optional>

std::optional<std::filesystem::path> getSteamInstallDir();
std::optional<std::filesystem::path> getGameInstallDir();
std::optional<std::filesystem::path> getGameBinDir();
std::optional<std::filesystem::path> getGameDevToolsBinDir();
std::optional<std::filesystem::path> getGameDevToolsExePath();
