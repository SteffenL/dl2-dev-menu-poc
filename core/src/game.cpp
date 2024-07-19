#include "core/game.hpp"

#include "core/hex.hpp"
#include "core/log.hpp"
#include "core/search.hpp"
#include "core/string.hpp"
#include "core/system.hpp"

#include <string>

std::filesystem::path getMainBinDir() {
    return getExePath().parent_path().parent_path().parent_path() / "ph" / "work" / "bin" / "x64";
}

bool* findCheatsEnabledVariable(const std::span<uint8_t> range) {
    /*
    .text:0000000181C63383 E8 68 17 13 00                                call    sub_181D94AF0
    .text:0000000181C63388 84 C0                                         test    al, al
    .text:0000000181C6338A 0F 95 C0                                      setnz   al
    .text:0000000181C6338D EB 0D                                         jmp     short loc_181C6339C
    .text:0000000181C6338F                               ; ---------------------------------------------------------------------------
    .text:0000000181C6338F
    .text:0000000181C6338F                               loc_181C6338F:                          ; CODE XREF: sub_181C62AB0+16F↑j
    .text:0000000181C6338F                                                                       ; sub_181C62AB0+188↑j
    .text:0000000181C6338F C6 05 64 4C 0C 02 01                          mov     cs:sl__var__bool__cheats_enabled, 1 ; Enable cheats via UseDeveloperKeys
    */
    auto it{search(range, "E868171300 84C0 0F95C0 EB0D C605????01")};
    if (it == range.end()) {
        log("\"Cheats enabled\" pattern not found.");
        return nullptr;
    }
    auto* patternAddress{range.data() + std::distance(range.begin(), it)};
    patternAddress += 12;
    log("\"Cheats enabled\" instruction found at " + hexNumber(std::bit_cast<uintptr_t>(patternAddress)));
    auto relAddr{*std::bit_cast<uint32_t*>(patternAddress + 2 /*offset of rel addr*/)};
    log("\"Cheats enabled\" variable relative address: " + hexNumber(relAddr));
    auto absAddr{std::bit_cast<uintptr_t>(patternAddress + 7 /*length of instruction*/) + relAddr};
    log("\"Cheats enabled\" variable absolute address: " + hexNumber(absAddr));
    return std::bit_cast<bool*>(absAddr);
}

void setupCheats() {
    log("Finding code of game DLL.");
    auto code{getImageCode("gamedll_ph_x64_rwe.dll")};
    if (code.empty()) {
        log("Code of game DLL not found.");
        return;
    }
    log("Base of code: ", hexNumber(std::bit_cast<uintptr_t>(code.data())));
    auto* cheatsEnabled{findCheatsEnabledVariable(code)};
    if (!cheatsEnabled) {
        log("\"Cheats enabled\" variable not found.");
        return;
    }
    log("\"Cheats enabled\" variable found at " + hexNumber(std::bit_cast<uintptr_t>(cheatsEnabled)));
    log("Enabling cheats.");
    *cheatsEnabled = true;
}

bool isGameDllLoaded() {
    return !!getModuleHandle("gamedll_ph_x64_rwe.dll");
}

void setGameWorkingDirectory() {
    auto workingDir{getMainBinDir()};
    log("Changing working directory: ", charStringFromChar8String(workingDir.u8string()));
    setWorkingDirectory(workingDir);
}
