#pragma once

#include <cstdlib>
#include <string_view>
#include <ostream>

const std::string_view getLogTag() noexcept;
std::ostream& getLogStream();

template<typename... Args>
void log(Args&&... args) {
    auto& os{getLogStream()};
    os << getLogTag();
    ((os << std::forward<Args>(args)), ...);
    os << std::endl;
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
