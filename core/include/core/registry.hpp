#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <windows.h>

class RegKey {
public:
    RegKey(HKEY rootKey, const std::wstring& subKey, DWORD options, REGSAM samDesired);
    ~RegKey();

    RegKey(const RegKey& other) = delete;
    RegKey& operator=(const RegKey& other) = delete;
    RegKey(RegKey&& other) = delete;
    RegKey& operator=(RegKey&& other) = delete;

    bool isOpen() const;

    std::optional<std::vector<uint8_t>> queryBytes(const std::wstring& name) const;
    std::optional<std::wstring> queryString(const std::wstring& name) const;
    std::optional<unsigned int> queryUint(const std::wstring& name, unsigned int defaultValue) const;

private:
    HKEY m_handle{};
};
