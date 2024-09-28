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

copy /D /y %PROJECT_RESOUR_DIR%\logger.service.install.bat   %TARGET_DIR%\logger.service.install.bat
copy /D /y %PROJECT_RESOUR_DIR%\logger.service.uninstall.bat %TARGET_DIR%\logger.service.uninstall.bat

set PATH=%OLDPATH%

set TARGET_DIR=
set PROJECT_RESOUR_DIR=
