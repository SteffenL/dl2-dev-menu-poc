#pragma once

#include <filesystem>
#include <span>

std::filesystem::path getMainBinDir();
void setupCheats();
bool isGameDllLoaded();
void setGameWorkingDirectory();
