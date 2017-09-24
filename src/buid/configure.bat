setlocal
@echo off

SET CURR_DIR=%cd%
SET THIS_SCRIPT_PATH=%~dp0
cd %THIS_SCRIPT_PATH%
cmake -G "Visual Studio 14 2015 Win64" ..
cd %CURR_DIR%

endlocal