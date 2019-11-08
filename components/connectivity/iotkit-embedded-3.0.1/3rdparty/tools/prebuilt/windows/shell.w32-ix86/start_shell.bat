@echo off
REM Get the current directory 
REM
for /f "tokens=* delims=/" %%A in ('cd') do set CURRENT_DIR=%%A

SET SHELLDIR=%CURRENT_DIR%
SET SHELL=%SHELLDIR%/bash.exe
SET BASH=%SHELL%

SET PATH=%SHELLDIR%;%PATH%

"%SHELL%" %*
