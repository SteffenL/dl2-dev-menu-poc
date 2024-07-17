@echo off

set "script_dir=%~dp0."
set "game_root_dir=%ProgramFiles(x86)%\Steam\steamapps\common\Dying Light 2"
set "dev_tools_bin_dir=%game_root_dir%\DevTools\bin"
set "dev_tools_exe=%dev_tools_bin_dir%\dyinglightgame_x64_rwe.exe"

if not exist "%dev_tools_exe%" (
    echo Please install DevTools first.
    exit /b 1
)

call :install_file "%script_dir%\xinput1_3.dll" "%dev_tools_bin_dir%"

goto :eof

:install_file
    echo Installing "%~dpf1" into "%~dpf2".
    xcopy /E /Q "%~dpf1" "%~dpf2"
    goto :eof
