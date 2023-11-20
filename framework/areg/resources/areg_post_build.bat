@echo on
SETLOCAL

:: initialize passed variables. add if needed

:: full path of project resource folder
set PROJECT_RESOUR_DIR=%1

:: full path of target folder -- compiled executable folder
set TARGET_DIR=%2

set OLDPATH=%PATH%
set PATH=%SystemRoot%\System32;%PATH%

:: copy dependencies

echo copying %PROJECT_RESOUR_DIR%\areg.init

IF NOT EXIST %TARGET_DIR%\config mkdir      %TARGET_DIR%\config
copy /D /y %PROJECT_RESOUR_DIR%\areg.init %TARGET_DIR%\config\areg.init

set PATH=%OLDPATH%

set TARGET_DIR=
set PROJECT_RESOUR_DIR=
