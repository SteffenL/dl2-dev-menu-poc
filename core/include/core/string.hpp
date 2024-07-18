#pragma once

#include <string>
#include <string_view>

// Converts a narrow (UTF-8-encoded) string into a wide (UTF-16-encoded) string.
std::wstring widenString(const std::string_view input);

// Converts a wide (UTF-16-encoded) string into a narrow (UTF-8-encoded) string.
std::string narrowString(const std::wstring_view input);
