#pragma once

#include <utility>

#include <windows.h>

class ScopedVirtualProtect {
public:
    ScopedVirtualProtect(LPVOID address, SIZE_T size, DWORD newProtect)
        : m_address{address},
          m_size{size} {
        m_ok = ::VirtualProtect(address, size, newProtect, &m_oldProtect) != 0;
    }

    ~ScopedVirtualProtect() {
        if (!m_ok) {
            return;
        }
        DWORD oldProtect{};
        ::VirtualProtect(m_address, m_size, m_oldProtect, &oldProtect);
    }

    ScopedVirtualProtect(const ScopedVirtualProtect& other) = delete;
    ScopedVirtualProtect& operator=(const ScopedVirtualProtect& other) = delete;
    ScopedVirtualProtect(ScopedVirtualProtect&& other) { *this = std::move(other); }

    ScopedVirtualProtect& operator=(ScopedVirtualProtect&& other) {
        if (this == &other) {
            return *this;
        }
        m_ok = std::exchange(other.m_ok, false);
        m_address = std::exchange(other.m_address, nullptr);
        m_size = std::exchange(other.m_size, 0);
        m_oldProtect = std::exchange(other.m_oldProtect, 0);
        return *this;
    }

private:
    bool m_ok{};
    LPVOID m_address{};
    SIZE_T m_size{};
    DWORD m_oldProtect{};
};
