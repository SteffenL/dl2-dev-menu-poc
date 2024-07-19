#include "core/cli.hpp"
#include "core/game.hpp"
#include "core/string.hpp"
#include "core/system.hpp"
#include "steam/SteamApi.hpp"
#include "steam_game/integration.hpp"

#include <filesystem>
#include <iostream>
#include <vector>

#include <windows.h>

struct AppState {
    bool quit{};
    int exitCode{};
};

namespace fs = std::filesystem;

void installFile(const fs::path& filePath, const fs::path& targetPath) {
    const auto targetDir{targetPath.parent_path()};
    if (!fs::is_directory(targetDir)) {
        std::cout << "Creating directory: " << charStringFromChar8String(targetDir.u8string()) << "\n";
        fs::create_directories(targetDir);
    }
    std::cout << "Copying file: " << charStringFromChar8String(filePath.u8string()) << "\n";
    fs::copy_file(filePath, targetPath, fs::copy_options::overwrite_existing);
}

void uninstallFile(const fs::path& filePath) {
    if (fs::exists(filePath)) {
        std::cout << "Removing file: " << charStringFromChar8String(filePath.u8string()) << "\n";
        fs::remove(filePath);
    }
}

bool desktopShortcutExists(const std::string& label) {
    return shellShortcutExists(getDesktopDir(), label);
}

void installDesktopShortcut(const fs::path& filePath, const std::string& label, const std::string& args) {
    std::cout << "Creating desktop shortcut with label: " << label << "\n";
    createShellShortcut(filePath, getDesktopDir(), label, args);
}

void uninstallDesktopShortcut(const std::string& label) {
    if (shellShortcutExists(getDesktopDir(), label)) {
        std::cout << "Removing desktop shortcut with label: " << label << "\n";
        removeShellShortcut(getDesktopDir(), label);
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

constexpr std::string_view desktopShortcutLabel{"Dying Light 2 with Developer Menu (DevTools)"};

struct DesktopShortcutItem {
    std::string label;
};

std::vector<DesktopShortcutItem> getDesktopShortcutItems() {
    std::vector<DesktopShortcutItem> items;
    items.emplace_back(std::string{desktopShortcutLabel});
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
        if (!SteamApi::isSteamRunning()) {
            std::cout << "Warning: Steam should be running to enable accurate detection.\n\n";
        }
        std::cout << "DevTools were not found at the expected locations:\n\n"
                  << charStringFromChar8String(devToolsBinDir->u8string())
                  << "\n\n"
                  << "Please make sure that DevTools are installed.\n";
        state->exitCode = 1;
        return;
    }

    const auto devToolsExePath{getGameDevToolsExePath()};
    if (!devToolsExePath) {
        std::cout << "Unable to determine DevTools executable path.\n";
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
            const auto prompt{"File already exists:\n\n" + charStringFromChar8String(targetPath.u8string()) + "\n\nWould you like to replace it?"};
            const auto replace{promptYesNo(std::cout, std::cin, prompt, YesNoChoice::no) == YesNoChoice::yes};
            if (!replace) {
                std::cout << "Aborted.\n";
                state->exitCode = 1;
                return;
            }
        }
        installFile(exeDir / "files" / installItem.relPath, targetPath);
    }

    const auto shortcutPrompt{"Would you like to create a shortcut on the desktop?"};
    const auto makeShortcut{promptYesNo(std::cout, std::cin, shortcutPrompt, YesNoChoice::no) == YesNoChoice::yes};
    if (makeShortcut) {
        const std::string label{desktopShortcutLabel};
        if (desktopShortcutExists(label)) {
            const auto prompt{"Desktop shortcut already exists:\n\n" + label + "\n\nWould you like to replace it?"};
            const auto replace{promptYesNo(std::cout, std::cin, prompt, YesNoChoice::no) == YesNoChoice::yes};
            if (!replace) {
                std::cout << "Aborted.\n";
                state->exitCode = 1;
                return;
            }
        }
        installDesktopShortcut(*devToolsExePath, label, "-nologos");
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
        if (!SteamApi::isSteamRunning()) {
            std::cout << "Warning: Steam should be running to enable accurate detection.\n\n";
        }
        std::cout << "DevTools were not found at the expected locations:\n\n"
                  << charStringFromChar8String(devToolsBinDir->u8string())
                  << "\n\n"
                  << "Please make sure that DevTools are installed.\n";
        state->exitCode = 1;
        return;
    }

    for (const auto& shortcutItem : getDesktopShortcutItems()) {
        uninstallDesktopShortcut(shortcutItem.label);
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
        constexpr long defaultChoice{3};

        std::cout << "Dying Light 2 Developer Menu PoC Setup\n"
                  << "  by Steffen André Langnes\n"
                  << "     www.steffenl.com\n\n";

        AppState state;

        while (true) {
            const auto choice{promptChoice(std::cout, std::cin, "Enter a number depending on what you would like to do.", defaultChoice, menuEntries)};

            try {
                choice.handler(&state);
            } catch (const std::exception& ex) {
                std::cout << "Error: " << ex.what() << "\n";
            }

            if (state.quit) {
                return state.exitCode;
            }
        }
    });
}
