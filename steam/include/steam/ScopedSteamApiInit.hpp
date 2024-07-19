#pragma once

class ScopedSteamApiInit {
public:
    ScopedSteamApiInit(unsigned int appId);
    ~ScopedSteamApiInit();
    bool isOk() const noexcept;

private:
    bool m_ok{};
};
