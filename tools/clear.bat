@ECHO OFF
REM This file is a part of cdev project
REM https://github.com/adm244/cdev

ECHO: Cleaning bin folder...

RMDIR /S /Q "%bin%"
MKDIR "%bin%"

ECHO: Done!
