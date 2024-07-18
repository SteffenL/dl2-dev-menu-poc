#include "core/string.hpp"

#include <windows.h>

template<typename T>
std::wstring widenBasicString(const std::basic_string_view<T> input) {
    if (input.empty()) {
        return {};
    }
    const UINT cp{CP_UTF8};
    const DWORD flags{MB_ERR_INVALID_CHARS};
    const auto input_length{static_cast<int>(input.size())};
    const auto* data{reinterpret_cast<const char*>(input.data())};
    auto required_length{
        MultiByteToWideChar(cp, flags, data, input_length, nullptr, 0)};
    if (required_length > 0) {
        std::wstring output(static_cast<std::size_t>(required_length), L'\0');
        if (MultiByteToWideChar(cp, flags, data, input_length, &output[0],
                                required_length) > 0) {
            return output;
        }
    }
    // Failed to convert string from UTF-8 to UTF-16
    return {};
}

// Converts a narrow (UTF-8-encoded) string into a wide (UTF-16-encoded) string.
std::wstring widenString(const std::string_view input) {
    return widenBasicString(input);
}

template<typename T>
std::basic_string<T> narrowBasicString(const std::wstring_view input) {
    struct wc_flags {
        enum TYPE : unsigned int {
            // WC_ERR_INVALID_CHARS
            err_invalid_chars = 0x00000080U
        };
    };
    if (input.empty()) {
        return {};
    }
    const UINT cp{CP_UTF8};
    const DWORD flags{wc_flags::err_invalid_chars};
    const auto input_length{static_cast<int>(input.size())};
    const auto required_length{WideCharToMultiByte(
        cp, flags, input.data(), input_length, nullptr, 0, nullptr, nullptr)};
    if (required_length > 0) {
        std::basic_string<T> output(static_cast<std::size_t>(required_length),
                                    '\0');
        if (WideCharToMultiByte(cp, flags, input.data(), input_length,
                                reinterpret_cast<char*>(&output[0]),
                                required_length, nullptr, nullptr) > 0) {
            return output;
        }
    }
    // Failed to convert string from UTF-16 to UTF-8
    return {};
}

// Converts a wide (UTF-16-encoded) string into a narrow (UTF-8-encoded) string.
std::string narrowString(const std::wstring_view input) {
    return narrowBasicString<char>(input);
}
