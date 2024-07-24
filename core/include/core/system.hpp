#pragma once

#include <filesystem>
#include <span>
#include <string>
#include <vector>

void* getModuleHandle(const std::filesystem::path& moduleName);
std::filesystem::path getModulePath(void* handle);
std::span<uint8_t> getImageCode(const std::filesystem::path& moduleName);
std::filesystem::path getSystemDir();
std::filesystem::path getExePath();
std::filesystem::path getExeDir();
std::filesystem::path getDesktopDir();
std::wstring GetDocumentsFolderPath();
void setEnv(const std::string& name, const std::string& value);
void setWorkingDirectory(const std::filesystem::path& dir);
void createShellShortcut(const std::filesystem::path& targetPath, const std::filesystem::path& directory, const std::string& label, const std::string& args = {});
bool shellShortcutExists(const std::filesystem::path& directory, const std::string& label);
void removeShellShortcut(const std::filesystem::path& directory, const std::string& label);
