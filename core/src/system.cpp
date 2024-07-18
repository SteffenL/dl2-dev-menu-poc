#include "core/system.hpp"

#include <array>
#include <stdexcept>

#include <windows.h>
#include <dbghelp.h>

void* getModuleHandle(const std::filesystem::path& moduleName) {
    return ::GetModuleHandleW(moduleName.c_str());
}

std::filesystem::path getModulePath(void* handle) {
    std::array<WCHAR, MAX_PATH + 1> exePath{};
    auto length{::GetModuleFileNameW(std::bit_cast<HMODULE>(handle), exePath.data(), exePath.size())};
    return std::filesystem::path{exePath.begin(), exePath.begin() + length};
}

std::span<uint8_t> getImageCode(const std::filesystem::path& moduleName) {
    auto* imageBase{std::bit_cast<uint8_t*>(getModuleHandle(moduleName))};
    if (!imageBase) {
        throw std::runtime_error{"Unable to get handle of module by name: " + moduleName.string()};
    }
    auto* ntHeaders{::ImageNtHeader(imageBase)};
    auto* baseOfCode{imageBase + ntHeaders->OptionalHeader.BaseOfCode};
    auto sizeOfCode{ntHeaders->OptionalHeader.SizeOfCode};
    return std::span<uint8_t>{baseOfCode, baseOfCode + sizeOfCode};
}

std::filesystem::path getSystemDir() {
    std::array<WCHAR, MAX_PATH + 1> systemDir{};
    auto length{::GetSystemDirectoryW(systemDir.data(), systemDir.size())};
    return std::filesystem::path{systemDir.begin(), systemDir.begin() + length};
}

std::filesystem::path getExePath() {
    return getModulePath(nullptr);
}

std::filesystem::path getExeDir() {
    return getExePath().parent_path();
}

void setEnv(const std::string& name, const std::string& value) {
    ::SetEnvironmentVariableA(name.c_str(), value.c_str());
}

void setWorkingDirectory(const std::filesystem::path& dir) {
    ::SetCurrentDirectoryW(dir.c_str());
}
