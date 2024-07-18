#include "core/main.hpp"

#include "core/log.hpp"
#include "core/state.hpp"

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return catch_guard([=] {
        if (fdwReason == DLL_PROCESS_ATTACH) {
            AppState::create(hinstDLL);
            ::DisableThreadLibraryCalls(hinstDLL);
            setup();
        } else if (fdwReason == DLL_PROCESS_DETACH) {
            shutdown();
        }
        return TRUE;
    });
}
