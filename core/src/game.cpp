#include "core/game.hpp"

#include "core/hex.hpp"
#include "core/log.hpp"
#include "core/search.hpp"
#include "core/string.hpp"
#include "core/system.hpp"

#include <cstdint>
#include <string>

std::filesystem::path getMainBinDir() {
    return getExePath().parent_path().parent_path().parent_path() / "ph" / "work" / "bin" / "x64";
}

bool* findCheatsEnabledVariable(const std::span<uint8_t> range) {
    // From game v1.17.2+
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

std::span<uint8_t> findDeveloperMenuListBranch(std::span<uint8_t> code) {
    // From game v1.17.3
    /*
    .text:0000000181711FFC 41 3B D3                                      cmp     edx, r11d
    .text:0000000181711FFF 72 06                                         jb      short loc_181712007 ; Location of SDavidLee's patch to display "Developer Menu List" button
    .text:0000000181712001 0F 86 1D 03 00 00                             jbe     loc_181712324
    .text:0000000181712007
    .text:0000000181712007                               loc_181712007:                          ; CODE XREF: sub_181711EC0+128↑j
    .text:0000000181712007                                                                       ; sub_181711EC0+138↑j ...
    .text:0000000181712007 49 3B EF                                      cmp     rbp, r15
    .text:000000018171200A 73 1E                                         jnb     short loc_18171202A
    .text:000000018171200C 48 85 C9                                      test    rcx, rcx
    .text:000000018171200F 74 0D                                         jz      short loc_18171201E
    */
    auto it{search(code, "413BD3 7206 0F861D030000 493BEF 731E 4885C9 740D")};
    if (it == code.end()) {
        log("\"Developer Menu List\" pattern not found.");
        return {};
    }
    auto* patternAddress{code.data() + std::distance(code.begin(), it)};
    patternAddress += 3;
    log("\"Developer Menu List\" instruction found at " + hexNumber(std::bit_cast<uintptr_t>(patternAddress)));
    return {static_cast<uint8_t*>(patternAddress), static_cast<uint8_t*>(patternAddress) + 2 /*length of branch instruction*/};
}

void patchDeveloperMenuListBranch(std::span<uint8_t> code) {
    auto branch{findDeveloperMenuListBranch(code)};
    if (branch.empty()) {
        return;
    }
    log("Patching \"Developer Menu List\" branch.");
    // Change instruction from:
    // 72 06                                         jb      short loc_181712007
    // to:
    // EB 06                                         jmp     short loc_181712007
    makeProgramPatchScope(branch, [&] {
        branch[0] = 0xEB;
    });
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
    if (cheatsEnabled) {
        log("\"Cheats enabled\" variable found at " + hexNumber(std::bit_cast<uintptr_t>(cheatsEnabled)));
        log("Enabling cheats.");
        *cheatsEnabled = true;
    } else {
        log("\"Cheats enabled\" variable not found.");
    }

    if (cheatsEnabled && *cheatsEnabled) {
        patchDeveloperMenuListBranch(code);
    }
}

bool isGameDllLoaded() {
    return !!getModuleHandle("gamedll_ph_x64_rwe.dll");
}

void setGameWorkingDirectory() {
    auto workingDir{getMainBinDir()};
    log("Changing working directory: ", charStringFromChar8String(workingDir.u8string()));
    setWorkingDirectory(workingDir);
}
