#include "core/state.hpp"

std::unique_ptr<AppState> AppState::sm_instance;

AppState::AppState(void* dllHandle) : m_dllHandle{dllHandle} {}

AppState& AppState::create(void* dllHandle) {
    sm_instance = std::make_unique<AppState>(dllHandle);
    return *sm_instance;
}

void* AppState::getDllHandle() const noexcept {
    return m_dllHandle;
}

void AppState::setOnGameDllLoadedHandler(std::function<void()> handler) {
    m_onGameDllLoaded = handler;
}

void AppState::notifyOnGameDllLoaded() {
    if (m_onGameDllLoaded) {
        m_onGameDllLoaded();
    }
}

AppState& AppState::get() noexcept {
    return *sm_instance;
}
