#include "core/system.hpp"
#include "core/string.hpp"

#include "ScopedCoInitialize.hpp"

#include <array>
#include <filesystem>
#include <stdexcept>

#include <windows.h>
#include <wrl.h>
#include <dbghelp.h>
#include <shlobj.h>
#include <wrl/client.h>

std::filesystem::path makeShortcutFileName(const std::string& label) {
    auto normalizedLabel{std::filesystem::u8path(label).filename()};
    normalizedLabel += ".lnk";
    return normalizedLabel;
}

void* getModuleHandle(const std::filesystem::path& moduleName) {
    return ::GetModuleHandleW(moduleName.c_str());
}

std::filesystem::path getModulePath(void* handle) {
    std::array<WCHAR, MAX_PATH + 1> exePath{};
    auto length{::GetModuleFileNameW(std::bit_cast<HMODULE>(handle), exePath.data(), exePath.size())};
    return std::filesystem::path{exePath.begin(), exePath.begin() + length};
}

std::span<uint8_t> getImageCode(const std::filesystem::path& moduleName) {
    auto* imageBase{std::bit_cast<uint8_t*>(getModuleHandle(moduleName))};
    if (!imageBase) {
        throw std::runtime_error{"Unable to get handle of module by name: " + charStringFromChar8String(moduleName.u8string())};
    }
    auto* ntHeaders{::ImageNtHeader(imageBase)};
    auto* baseOfCode{imageBase + ntHeaders->OptionalHeader.BaseOfCode};
    auto sizeOfCode{ntHeaders->OptionalHeader.SizeOfCode};
    return std::span<uint8_t>{baseOfCode, baseOfCode + sizeOfCode};
}

std::filesystem::path getSystemDir() {
    std::array<WCHAR, MAX_PATH + 1> systemDir{};
    auto length{::GetSystemDirectoryW(systemDir.data(), systemDir.size())};
    return std::filesystem::path{systemDir.begin(), systemDir.begin() + length};
}

std::filesystem::path getExePath() {
    return getModulePath(nullptr);
}

std::filesystem::path getExeDir() {
    return getExePath().parent_path();
}

std::filesystem::path getDesktopDir() {
    PWSTR desktopPath{};
    if (::SHGetKnownFolderPath(FOLDERID_Desktop, KF_FLAG_DEFAULT, NULL, &desktopPath) != S_OK) {
        throw std::runtime_error{"Unable to get desktop path"};
    }
    std::filesystem::path result{desktopPath};
    ::CoTaskMemFree(desktopPath);
    return result;
}

std::wstring GetDocumentsFolderPath() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path))) {
        return std::wstring(path);
    } else {
        return std::wstring();
    }
}

bool ReplaceRendererMode(const std::wstring& filePath, const std::wstring& from, const std::wstring& to) {
    std::wifstream fileIn(filePath);
    if (!fileIn.is_open()) {
        return false;
    }

    std::wstringstream buffer;
    buffer << fileIn.rdbuf();
    std::wstring content = buffer.str();
    fileIn.close();

    size_t startPos = content.find(from);
    if (startPos == std::wstring::npos) {
        return false;
    }

    while (startPos != std::wstring::npos) {
        content.replace(startPos, from.length(), to);
        startPos = content.find(from, startPos + to.length());
    }

    std::wofstream fileOut(filePath);
    if (!fileOut.is_open()) {
        return false;
    }

    fileOut << content;
    fileOut.close();
    return true;
}

void setEnv(const std::string& name, const std::string& value) {
    const auto wname{widenString(name)};
    const auto wvalue{widenString(value)};
    ::SetEnvironmentVariableW(wname.c_str(), wvalue.c_str());
}

void setWorkingDirectory(const std::filesystem::path& dir) {
    ::SetCurrentDirectoryW(dir.c_str());
}

void createShellShortcut(const std::filesystem::path& targetPath, const std::filesystem::path& directory, const std::string& label, const std::string& args) {
    ScopedCoInitialize coinit{COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE};

    IShellLinkW* shellLink_{};
    auto hr{CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, std::bit_cast<LPVOID*>(&shellLink_))};
    if (FAILED(hr)) {
        throw std::runtime_error{"CoCreateInstance failed"};
    }

    Microsoft::WRL::ComPtr<IShellLinkW> shellLink{shellLink_};
    shellLink_->Release();

    hr = shellLink->SetPath(targetPath.c_str());
    if (FAILED(hr)) {
        throw std::runtime_error{"IShellLink::SetPath failed"};
    }

    auto wargs{widenString(args)};
    hr = shellLink->SetArguments(wargs.c_str());
    if (FAILED(hr)) {
        throw std::runtime_error{"IShellLink::SetArguments failed"};
    }

    Microsoft::WRL::ComPtr<IPersistFile> persistFile;

    hr = shellLink.As(&persistFile);
    if (FAILED(hr)) {
        throw std::runtime_error{"ComPtr<IShellLink>::As<IPersistFile> failed"};
    }

    const auto linkName{directory / makeShortcutFileName(label)};
    hr = persistFile->Save(linkName.c_str(), TRUE);
    if (FAILED(hr)) {
        throw std::runtime_error{"IPersistFile::Save failed"};
    }
}

bool shellShortcutExists(const std::filesystem::path& directory, const std::string& label) {
    return std::filesystem::exists(directory / makeShortcutFileName(label));
}

void removeShellShortcut(const std::filesystem::path& directory, const std::string& label) {
    std::filesystem::remove(directory / makeShortcutFileName(label));
}
