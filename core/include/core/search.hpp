#pragma once

#include "core/hex.hpp"

#include <stdexcept>
#include <span>
#include <string_view>

constexpr size_t getEffectiveCharCountInPattern(const std::string_view pattern) {
    size_t count{};
    for (auto it{pattern.begin()}; it != pattern.end(); ++it) {
        if (*it == '?') {
            ++count;
            continue;
        }
        if (*it == ' ') {
            continue;
        }
        if (!isValidNibble(*it)) {
            throw std::logic_error{"Invalid nibble"};
        }
        ++it;
        if (it == pattern.end()) {
            throw std::logic_error{"Incomplete pattern"};
        }
        if (!isValidNibble(*it)) {
            throw std::logic_error{"Invalid nibble"};
        }
        ++count;
    }
    return count;
}

template<typename T>
constexpr auto search(const std::span<T> range, const std::string_view pattern) -> typename decltype(range)::iterator {
    auto rbegin{range.begin()};
    const auto rend{range.end()};
    const auto pbegin{pattern.begin()};
    const auto pend(pattern.end());
    auto rit{rbegin};
    for (auto pit{pbegin}; pit != pattern.end();) {
        if (rit == rend) {
            return rend;
        }
        if (*pit == ' ') {
            ++pit;
            continue;
        }
        if (*pit == '?') {
            ++rit;
            ++pit;
            continue;
        }
        auto h{decodeNibble(*pit)};
        ++pit;
        if (pit == pend) {
            throw std::logic_error{"Incomplete pattern"};
        }
        auto l{decodeNibble(*pit)};
        auto byte{static_cast<uint8_t>(h << 4U | l)};
        if (*rit != byte) {
            rit = ++rbegin;
            pit = pbegin;
            continue;
        }
        ++rit;
        ++pit;
    }
    if (rbegin == rend) {
        return rend;
    }
    auto matchCount{std::distance(rbegin, rit)};
    if (matchCount != getEffectiveCharCountInPattern(pattern)) {
        return rend;
    }
    return rbegin;
}