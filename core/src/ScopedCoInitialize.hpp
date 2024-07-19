#pragma once

#include <stdexcept>
#include <utility>

#include <windows.h>

/**
 * A wrapper around COM library initialization. Calls CoInitializeEx in the
 * constructor and CoUninitialize in the destructor.
 *
 * @exception exception Thrown if CoInitializeEx has already been called with a
 * different concurrency model.
 */
class ScopedCoInitialize {
public:
    ScopedCoInitialize(DWORD dwCoInit) {
        // We can safely continue as long as COM was either successfully
        // initialized or already initialized.
        // RPC_E_CHANGED_MODE means that CoInitializeEx was already called with
        // a different concurrency model.
        switch (CoInitializeEx(nullptr, dwCoInit)) {
            case S_OK:
            case S_FALSE:
                m_initialized = true;
                break;
            case RPC_E_CHANGED_MODE:
                throw std::runtime_error{"CoInitializeEx already called with a different concurrency model"};
            default:
                throw std::runtime_error{"Unexpected result from CoInitializeEx"};
        }
    }

    ScopedCoInitialize() : ScopedCoInitialize{COINIT_MULTITHREADED} {}

    ~ScopedCoInitialize() {
        if (m_initialized) {
            CoUninitialize();
            m_initialized = false;
        }
    }

    ScopedCoInitialize(const ScopedCoInitialize& other) = delete;
    ScopedCoInitialize& operator=(const ScopedCoInitialize& other) = delete;
    ScopedCoInitialize(ScopedCoInitialize&& other) { *this = std::move(other); }

    ScopedCoInitialize& operator=(ScopedCoInitialize&& other) {
        if (this == &other) {
            return *this;
        }
        m_initialized = std::exchange(other.m_initialized, false);
        return *this;
    }

private:
    bool m_initialized{};
};
