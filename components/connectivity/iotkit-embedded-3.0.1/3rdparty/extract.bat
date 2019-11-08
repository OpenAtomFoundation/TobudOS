@echo off

SET SHELLDIR=tools\prebuilt\windows\shell.w32-ix86
SET SHELL=%SHELLDIR%/bash.exe
SET BASH=%SHELL%

SET PATH=%SHELLDIR%;%PATH%

"%SHELL%" extract.sh

pause