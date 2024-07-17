@echo off

set "game_root_dir=%ProgramFiles(x86)%\Steam\steamapps\common\Dying Light 2"
set "game_bin_dir=%game_root_dir%\ph\work\bin\x64"
set "dev_tools_bin_dir=%game_root_dir%\DevTools\bin"
set "dev_tools_exe=%dev_tools_bin_dir%\dyinglightgame_x64_rwe.exe"

if not exist "%dev_tools_exe%" (
    echo DevTools is required.
    exit /b 1
)

start "" "%dev_tools_exe%"
