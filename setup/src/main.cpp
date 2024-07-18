#include "core/cli.hpp"
#include "core/game.hpp"
#include "core/system.hpp"

#include <filesystem>
#include <iostream>
#include <vector>

#include <windows.h>
#include <winnls.h>

struct AppState {
    bool quit{};
    int exitCode{};
};

namespace fs = std::filesystem;

void installFile(const fs::path& filePath, const fs::path& targetPath) {
    const auto targetDir{targetPath.parent_path()};
    if (!fs::is_directory(targetDir)) {
        std::cout << "Creating directory: " << targetDir.string() << "\n";
        fs::create_directories(targetDir);
    }
    std::cout << "Copying file: " << filePath.string() << "\n";
    fs::copy_file(filePath, targetPath, fs::copy_options::overwrite_existing);
}

void uninstallFile(const fs::path& filePath) {
    if (fs::exists(filePath)) {
        std::cout << "Removing file: " << filePath.string() << "\n";
        fs::remove(filePath);
    }
}

struct InstallItem {
    fs::path relPath;
    bool install{true};
    bool uninstall{true};
};

std::vector<InstallItem> getInstallItems() {
    std::vector<InstallItem> items;
    items.emplace_back("xinput1_3.dll", true, true);
    items.emplace_back("sl_dl2_dev_menu.txt", false, true);
    return items;
}

void cmdInstall(void* userData) {
    auto* state{static_cast<AppState*>(userData)};

    const auto devToolsBinDir{getGameDevToolsBinDir()};
    if (!devToolsBinDir) {
        std::cout << "Unable to determine DevTools directory.\n";
        state->exitCode = 1;
        return;
    }

    if (!fs::is_directory(*devToolsBinDir)) {
        std::cout << "DevTools was not found at the expected locations:\n\n"
                  << devToolsBinDir->string()
                  << "\n\n"
                  << "Please make sure to install DevTools.\n";
        state->exitCode = 1;
        return;
    }

    const auto& targetDir{*devToolsBinDir};
    const auto exeDir{getExeDir()};

    for (const auto& installItem : getInstallItems()) {
        if (!installItem.install) {
            continue;
        }
        const auto targetPath{targetDir / installItem.relPath};
        if (fs::exists(targetPath)) {
            const auto prompt{"File already exists:\n\n" + targetPath.string() + "\n\nWould you like to replace it?"};
            const auto replace{promptYesNo(std::cout, std::cin, prompt, YesNoChoice::no) == YesNoChoice::yes};
            if (!replace) {
                std::cout << "Aborted.\n";
                state->exitCode = 1;
                return;
            }
        }
        installFile(exeDir / installItem.relPath, targetPath);
    }

    std::cout << "Installation completed.\n";
}

void cmdUninstall(void* userData) {
    auto* state{static_cast<AppState*>(userData)};

    const auto devToolsBinDir{getGameDevToolsBinDir()};
    if (!devToolsBinDir) {
        std::cout << "Unable to determine DevTools directory.\n";
        state->exitCode = 1;
        return;
    }

    if (!fs::is_directory(*devToolsBinDir)) {
        std::cout << "DevTools was not found at the expected locations:\n\n"
                  << devToolsBinDir->string()
                  << "\n\n"
                  << "Please make sure to install DevTools.\n";
        state->exitCode = 1;
        return;
    }

    const auto& targetDir{*devToolsBinDir};

    for (const auto& installItem : getInstallItems()) {
        if (!installItem.uninstall) {
            continue;
        }
        const auto targetPath{targetDir / installItem.relPath};
        if (!fs::exists(targetPath)) {
            continue;
        }
        uninstallFile(targetPath);
    }

    std::cout << "Uninstallation completed.\n";
}

void cmdQuit(void* userData) {
    auto* state{static_cast<AppState*>(userData)};
    state->quit = true;
}

int main() {
    return cliMain([] {
        static const std::vector<MenuEntry> menuEntries{
            {1, "Install", cmdInstall},
            {2, "Uninstall", cmdUninstall},
            {3, "Quit", cmdQuit}};

        std::cout << "Dying Light 2 Developer Menu PoC Setup\n"
                  << "  by Steffen André Langnes\n"
                  << "     www.steffenl.com\n\n";

        AppState state;

        while (true) {
            const auto choice{promptChoice(std::cout, std::cin, "Enter a number depending on what you would like to do.", 3, menuEntries)};
            choice.handler(&state);
            if (state.quit) {
                return state.exitCode;
            }
        }
    });
}
