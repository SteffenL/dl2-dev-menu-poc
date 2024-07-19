#include "core/game.hpp"

#include "core/hooks.hpp"
#include "core/log.hpp"
#include "core/state.hpp"
#include "core/string.hpp"
#include "core/system.hpp"

#include <windows.h>
#include <dbghelp.h>

void setup() {
    log("Disclaimer: This is only a proof of concept for using the developer menu in Dying Light 2 (tested v1.17.2).");
    log("Author: Steffen André Langnes - www.steffenl.com");
    log("Starting up.");

    const auto exePath{getExePath()};
    if (exePath.filename() != "dyinglightgame_x64_rwe.exe") {
        log("Unexpected game EXE: ", charStringFromChar8String(exePath.u8string()));
    }

    setGameSteamAppId();
	CheckVideoSetting();
    setGameWorkingDirectory();

    AppState::get().setOnGameDllLoadedHandler([] {
        setupCheats();
    });

    setupGameDllLoadHook();

    if (isGameDllLoaded()) {
        AppState::get().notifyOnGameDllLoaded();
    }
}

void shutdown() { log("Shutting down."); }
