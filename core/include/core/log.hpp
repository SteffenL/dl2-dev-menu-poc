#pragma once

#include <cstdlib>
#include <functional>
#include <ostream>

void makeLogScope(std::function<void(std::ostream& os)> cb);

template<typename... Args>
void log(Args&&... args) {
    makeLogScope([&] (std::ostream& os) {
        ((os << std::forward<Args>(args)), ...);
    });
}

template<typename T>
std::invoke_result_t<T> catch_guard(T fn) {
    try {
        return fn();
    } catch (const std::exception& ex) {
        log("Error: ", ex.what());
        std::abort();
    } catch (...) {
        log("Unknown error.");
        std::abort();
    }
}
