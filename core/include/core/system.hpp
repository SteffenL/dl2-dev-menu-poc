#pragma once

#include <filesystem>
#include <span>
#include <string>

void* getModuleHandle(const std::filesystem::path& moduleName);
std::filesystem::path getModulePath(void* handle);
std::span<uint8_t> getImageCode(const std::filesystem::path& moduleName);
std::filesystem::path getSystemDir();
std::filesystem::path getExePath();
void setEnv(const std::string& name, const std::string& value);
void setWorkingDirectory(const std::filesystem::path& dir);
