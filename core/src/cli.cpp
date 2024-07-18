#include "core/cli.hpp"

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#define NOMINMAX
#include <windows.h>

constexpr long countNumberCharsInternal(long n) noexcept {
    return 1 + (n < 10 ? 0 : countNumberCharsInternal(n / 10));
}

constexpr long countNumberChars(long n) noexcept {
    if (n == 0) {
        return 1;
    }
    if (n < 0) {
        n = -n;
        return 1 + countNumberCharsInternal(n);
    }
    return countNumberCharsInternal(n);
}

constexpr long getGreatestMenuEntryIdCharCount(const std::vector<MenuEntry>& entries) {
    long lowestId{};
    long greatestId{};
    for (const auto& entry : entries) {
        if (entry.id < lowestId) {
            lowestId = entry.id;
        } else if (entry.id > greatestId) {
            greatestId = entry.id;
        }
    }
    const auto idLength{std::max(countNumberChars(lowestId), countNumberChars(greatestId))};
    return idLength;
}

YesNoChoice promptYesNo(std::ostream& os, std::istream& is, const std::string& prompt, YesNoChoice defaultChoice) noexcept {
    os << prompt
       << " ["
       << (defaultChoice == YesNoChoice::yes ? "Y" : "y")
       << "/"
       << (defaultChoice == YesNoChoice::no ? "N" : "n")
       << "] ";
    while (true) {
        std::string input;
        std::getline(is, input);
        if (input.empty()) {
            os << (defaultChoice == YesNoChoice::yes ? "y" : "n") << "\n";
            return defaultChoice;
        }
        if (input == "y" || input == "Y") {
            return YesNoChoice::yes;
        }
        if (input == "n" || input == "N") {
            return YesNoChoice::no;
        }
        os << "Invalid answer.\n";
    }
}

long promptNumber(std::ostream& os, std::istream& is, const std::string& prompt, long defaultValue) noexcept {
    os << prompt << " [" << defaultValue << "] ";
    while (true) {
        std::string input;
        std::getline(is, input);
        if (input.empty()) {
            os << defaultValue << "\n";
            return defaultValue;
        }
        try {
            return std::stol(input);
        } catch (const std::invalid_argument&) {
            os << "Invalid number.\n";
        } catch (const std::out_of_range&) {
            os << "Invalid number range.\n";
        }
    }
}

MenuEntry promptChoice(std::ostream& os, std::istream& is, const std::string& prompt, long defaultValue, const std::vector<MenuEntry>& entries) noexcept {
    std::unordered_map<int, const MenuEntry*> entryMap;
    for (const auto& entry : entries) {
        entryMap[entry.id] = &entry;
    }
    const auto idLength{getGreatestMenuEntryIdCharCount(entries)};

    os << prompt << "\n\n";

    for (const auto& entry : entries) {
        os << "  " << std::setfill(' ') << std::setw(idLength) << entry.id << ". " << entry.label << "\n";
    }

    os << "\n";

    while (true) {
        try {
            return *entryMap.at(promptNumber(os, is, "Choice:", defaultValue));
        } catch (const std::out_of_range&) {
            os << "Invalid choice.\n";
        }
    }
}
int cliMain(std::function<int()> fn) {
    ::SetConsoleOutputCP(CP_UTF8);
    try {
        return fn();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        std::exit(1);
    }
}
