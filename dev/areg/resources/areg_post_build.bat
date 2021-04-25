@echo on
SETLOCAL

:: initialize passed variables. add if needed

:: full path of target folder -- compiled executable folder
set TARGET_DIR=%1

:: configuration name -- 'Debug' or 'Release'
set CONFIG_NAME=%2

:: full path of project directory
set PROJECT_DIR=%3

:: the name of project
set PROJECT_NAME=%4

:: full path of project resource folder
set PROJECT_RESOURCE_DIR=%5

:: full path of user resource folder
set USER_RESOURCE_DIR=%6

set OLDPATH=%PATH%
set PATH=%SystemRoot%\System32;%PATH%

::copy dependent DLLs

:: copy additional dependencies

echo TARGET_DIR = %TARGET_DIR%
echo CONFIG_NAME = %CONFIG_NAME%
echo PROJECT_DIR = %PROJECT_DIR%
echo PROJECT_NAME = %PROJECT_NAME%
echo PROJECT_RESOURCE_DIR = %PROJECT_RESOURCE_DIR%
echo USER_RESOURCE_DIR = %USER_RESOURCE_DIR%

echo copy /D /y %PROJECT_RESOURCE_DIR%\log.init      %TARGET_DIR%\config\log.init
echo copy /D /y %PROJECT_RESOURCE_DIR%\router.init   %TARGET_DIR%\config\router.init

IF NOT EXIST %TARGET_DIR%\config mkdir      	%TARGET_DIR%\config
copy /D /y %PROJECT_RESOURCE_DIR%\log.init      %TARGET_DIR%\config\log.init
copy /D /y %PROJECT_RESOURCE_DIR%\router.init   %TARGET_DIR%\config\router.init

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

set TARGET_DIR=
set CONFIG_NAME=
set PROJECT_DIR=
set PROJECT_NAME=
set PROJECT_RESOURCE_DIR=
set USER_RESOURCE_DIR=
