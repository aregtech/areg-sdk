@echo off
SETLOCAL

:: initialize passed variables. add if needed

:: full path of target folder -- compiled executable folder
set TARGET_DIR=%1

:: configuration name -- 'Debug' or 'Release'
set CONFIG_NAME=%2

:: platform name -- 'Win32' or 'x64'
set PLATFORM_NAME=%3

:: full path of project directory
set PROJECT_DIR=%4

:: the name of project
set PROJECT_NAME=%5

:: full path of project resource folder
set PROJECT_RESOURCE_DIR=%6

:: full path of user resource folder
set USER_RESOURCE_DIR=%7

:: project resource directory, common and application
set RESOURCE_PRJ_DIR=%PROJECT_DIR%resources\%PROJECT_NAME%
set RESOURCE_CMN_DIR=%PROJECT_DIR%resources\common

set OLDPATH=%PATH%
set PATH=%SystemRoot%\System32;%PATH%

::copy dependent DLLs

:: copy additional dependencies

copy /D /y %RESOURCE_PRJ_DIR%\broker_install.bat    %TARGET_DIR%\broker_install.bat
copy /D /y %RESOURCE_PRJ_DIR%\broker_uninstall.bat  %TARGET_DIR%\broker_uninstall.bat


:: check whether should copy debug or relase versions of libs
IF /i "%CONFIG_NAME%" EQU "Debug" GOTO DEBUG_LIBS
IF /i "%CONFIG_NAME%" NEQ "Debug" GOTO RELEASE_LIBS   

:: copy debug versions of DLLs
:DEBUG_LIBS

:: finished copying, go to end
GOTO END_SCRIPT

:: copy release versions of DLLs
:RELEASE_LIBS

:: finished copying, go to end
GOTO END_SCRIPT

:: finished copying, go to end
GOTO END_SCRIPT

:: end, reset environment variables
:END_SCRIPT
set PATH=%OLDPATH%
set RESOURCE_PRJ_DIR=
set RESOURCE_CMN_DIR=
set TARGET_DIR=
set CONFIG_NAME=
set PLATFORM_NAME=
set PROJECT_DIR=
set PROJECT_NAME=
set PROJECT_RESOURCE_DIR=
set USER_RESOURCE_DIR=
