#pragma once

#include <functional>
#include <iosfwd>
#include <string>
#include <vector>

enum class YesNoChoice {
    no,
    yes
};

struct MenuEntry {
    int id{};
    std::string label;
    std::function<void(void* userData)> handler;
};

YesNoChoice promptYesNo(std::ostream& os, std::istream& is, const std::string& prompt, YesNoChoice defaultChoice) noexcept;
long promptNumber(std::ostream& os, std::istream& is, const std::string& prompt, long defaultValue) noexcept;
MenuEntry promptChoice(std::ostream& os, std::istream& is, const std::string& prompt, long defaultValue, const std::vector<MenuEntry>& entries) noexcept;
int cliMain(std::function<int()> fn);
