#pragma once

#include <functional>
#include <memory>

class AppState {
public:
    explicit AppState(void* dllHandle);
    void* getDllHandle() const noexcept;
    void setOnGameDllLoadedHandler(std::function<void()> handler);
    void notifyOnGameDllLoaded();
    static AppState& create(void* dllHandle);
    static AppState& get() noexcept;

private:
    static std::unique_ptr<AppState> sm_instance;
    void* m_dllHandle{};
    std::function<void()> m_onGameDllLoaded;
};
