#pragma once

#include <filesystem>
#include <optional>
#include <span>

std::filesystem::path getMainBinDir();

bool* findCheatsEnabledVariable(const std::span<uint8_t> range);
void setupCheats();
bool isGameDllLoaded();
void setGameSteamAppId();
void setGameWorkingDirectory();
std::optional<std::filesystem::path> getSteamInstallDir();
std::optional<std::filesystem::path> getGameInstallDir();
std::optional<std::filesystem::path> getGameBinDir();
std::optional<std::filesystem::path> getGameDevToolsBinDir();
std::optional<std::filesystem::path> getGameDevToolsExePath();
