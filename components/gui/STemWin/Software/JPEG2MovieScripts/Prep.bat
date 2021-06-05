@ECHO OFF
GOTO START

******************************************************************************
*
* File:        Prep.bat
*
* Parameters:  none
*
* Purpose:     This batch file is called by MakeMovie.bat and sets the default
*              values for working folder, FFmpeg and JPEG2MOVIE path. It needs
*              to be adapted before MakeMovie.bat can be used.
*
******************************************************************************

:START

REM **************************************************************************
REM
REM   Configuration: Default values for size, quality and framerate
REM
REM **************************************************************************

SET DEFAULT_SIZE=320x240
SET DEFAULT_QUALITY=5
SET DEFAULT_FRAMERATE=25

REM **************************************************************************
REM
REM   Configuration: Working folder
REM
REM **************************************************************************

SET OUTPUT=C:\Movie\Output\

REM **************************************************************************
REM
REM   Configuration: FFmpeg
REM
REM **************************************************************************

SET FFMPEG=C:\FFmpeg\ffmpeg.exe

REM **************************************************************************
REM
REM   Configuration: JPEG2Movie
REM
REM **************************************************************************

SET JPEG2MOVIE=C:\Movie\JPEG2Movie.exe
