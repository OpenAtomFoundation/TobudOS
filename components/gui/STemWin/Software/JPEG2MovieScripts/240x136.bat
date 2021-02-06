@ECHO OFF
GOTO START

******************************************************************************
*
* File:        <X_SIZE>x<Y_SIZE>.bat
*
* Purpose:     This file simply calls MakeMovie.bat with its own file name
*              as parameter %2 which defines the resolution used by FFmpeg
*              to create the JPEG files.
*              By copying and renaming this file further resolutions can
*              be simply achieved.
*
*              Note: The file name of this file needs to match the size
*                    specification parameter '-s' for FFmpeg.exe.
*
******************************************************************************

:START

@%~dp0MakeMovie.bat %1 %~n0