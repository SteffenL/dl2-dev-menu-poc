#include "core/log.hpp"

#include "core/system.hpp"

#include <memory>
#include <fstream>

const std::string_view getLogTag() noexcept {
    return "[sl_dl2_dev_menu] ";
}

std::ostream& getLogStream() {
    static std::unique_ptr<std::ostream> instance;
    if (!instance) {
        auto logFilePath{getExePath().parent_path() / "sl_dl2_dev_menu.txt"};
        instance = std::make_unique<std::ofstream>(logFilePath);
    }
    return *instance;
}
