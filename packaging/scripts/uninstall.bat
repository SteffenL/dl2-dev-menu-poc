@echo off

set "script_dir=%~dp0."
set "game_root_dir=%ProgramFiles(x86)%\Steam\steamapps\common\Dying Light 2"
set "dev_tools_bin_dir=%game_root_dir%\DevTools\bin"
set "dev_tools_exe=%dev_tools_bin_dir%\dyinglightgame_x64_rwe.exe"

call :uninstall_file "%dev_tools_bin_dir%\xinput1_3.dll"
call :uninstall_file "%dev_tools_bin_dir%\sl_dl2_dev_menu.txt"

goto :eof

:uninstall_file
    if exist "%~dpf1" (
        echo Uninstalling "%~dpf1".
        del /F /S /Q "%~dpf1" > nul
    )
    goto :eof
