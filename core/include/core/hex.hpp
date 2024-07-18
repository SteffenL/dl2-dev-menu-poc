#pragma once

#include <sstream>
#include <stdexcept>
#include <string_view>

constexpr bool isValidNibble(char c) noexcept {
    if (c >= '0' && c <= '9') {
        return true;
    }
    if (c >= 'a' && c <= 'f') {
        return true;
    }
    if (c >= 'A' && c <= 'F') {
        return true;
    }
    return false;
}

constexpr uint8_t decodeNibble(char c) {
    if (c >= '0' && c <= '9') {
        return static_cast<uint8_t>(c - '0');
    }
    if (c >= 'a' && c <= 'f') {
        return static_cast<uint8_t>(c - 'a' + 10);
    }
    if (c >= 'A' && c <= 'F') {
        return static_cast<uint8_t>(c - 'A' + 10);
    }
    throw std::logic_error{"Invalid nibble"};
}

template<typename T>
std::string hexNumber(T value, const std::string_view prefix = "0x") noexcept {
    std::stringstream os;
    if (!prefix.empty()) {
        os.write(prefix.data(), prefix.size());
    }
    os << std::hex << value;
    return os.str();
}
