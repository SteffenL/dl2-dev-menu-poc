# Changelog

All notable changes to this project will be documented in this file.

## Unreleased

### Fixed

- Issues with some settings causing crashes.

## 0.5.0 - 2024-07-21

### Changed

- Improved compatibility by attempting to redirect all DLLs that fail to load.

## 0.4.0 - 2024-07-19

### Changed

- Improved redirection of D3D12Core.dll by intercepting the low level function LdrLoadDll instead of LoadLibraryW.

## 0.3.0 - 2024-07-19

### Added

- Steam integration for the installer for more accurate detection of the game's install folder.
- Redirect loading of D3D12Core.dll to avoid a startup issue when using D3D12 rendering.

### Removed

- Disabled some spammy logging.

## 0.2.0 - 2024-07-18

### Added

- A new installer:
  - Batch scripts are not needed any more.
  - Detects game's installation directory based on Steam's installation directory.
  - Can also create a desktop shortcut.

### Fixed

- Potential stability issues with logging.
- Potential issues with internationalization.

## 0.1.0 - 2024-07-17

This is the first release.
