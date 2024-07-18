#include "core/registry.hpp"

#include <optional>

template<typename Container>
bool queryBytes(HKEY handle, const std::wstring& name, Container& result) {
    DWORD bufLength{};
    auto status{::RegQueryValueExW(handle, name.c_str(), nullptr, nullptr, nullptr, &bufLength)};
    if (status != ERROR_SUCCESS && status != ERROR_MORE_DATA) {
        result.resize(0);
        return false;
    }
    result.resize(bufLength / sizeof(typename Container::value_type));
    auto* buf{reinterpret_cast<LPBYTE>(&result[0])};
    status = ::RegQueryValueExW(handle, name.c_str(), nullptr, nullptr, buf, &bufLength);
    if (status != ERROR_SUCCESS) {
        result.resize(0);
        return false;
    }
    return true;
}

RegKey::RegKey(HKEY rootKey, const std::wstring& subKey, DWORD options, REGSAM samDesired) {
    HKEY handle{};
    auto status{::RegOpenKeyExW(rootKey, subKey.c_str(), options, samDesired, &handle)};
    if (status == ERROR_SUCCESS) {
        m_handle = handle;
    }
}

RegKey::~RegKey() {
    if (m_handle) {
        ::RegCloseKey(m_handle);
        m_handle = nullptr;
    }
}

bool RegKey::isOpen() const { return !!m_handle; }

std::optional<std::vector<uint8_t>> RegKey::queryBytes(const std::wstring& name) const {
    std::vector<uint8_t> result;
    if (::queryBytes(m_handle, name, result)) {
        return std::make_optional(result);
    }
    return std::nullopt;
}

std::optional<std::wstring> RegKey::queryString(const std::wstring& name) const {
    std::wstring result;
    if (!::queryBytes(m_handle, name, result)) {
        return std::nullopt;
    }
    // Remove trailing null-characters.
    for (auto length{result.size()}; length > 0; --length) {
        if (result[length - 1] != 0) {
            result.resize(length);
            break;
        }
    }
    return result;
}

std::optional<unsigned int> RegKey::queryUint(const std::wstring& name, unsigned int defaultValue) const {
    std::vector<char> result;
    if (!::queryBytes(m_handle, name, result)) {
        return std::nullopt;
    }
    if (result.size() < sizeof(DWORD)) {
        return defaultValue;
    }
    return static_cast<unsigned int>(*reinterpret_cast<DWORD*>(result.data()));
}
