#include "core/log.hpp"

#include "core/system.hpp"

#include <fstream>
#include <memory>
#include <mutex>
#include <string_view>

namespace {
constexpr std::string_view logTag{"[sl_dl2_dev_menu] "};
constexpr std::string_view logFileName{"sl_dl2_dev_menu.txt"};
}

std::ostream& getLogStream() {
    static std::unique_ptr<std::ostream> instance;
    if (!instance) {
        auto logFilePath{getExePath().parent_path() / logFileName};
        instance = std::make_unique<std::ofstream>(logFilePath);
    }
    return *instance;
}

void makeLogScope(std::function<void(std::ostream& os)> cb) {
    static std::mutex mutex;
    std::scoped_lock lock{mutex};
    auto& os{getLogStream()};
    os << logTag;
    cb(os);
    os << std::endl;
}
