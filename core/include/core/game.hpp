#pragma once

#include <filesystem>
#include <span>

std::filesystem::path getMainBinDir();

bool* findCheatsEnabledVariable(const std::span<uint8_t> range);
void setupCheats();
bool isGameDllLoaded();
void CheckVideoSetting();
void setGameWorkingDirectory();
