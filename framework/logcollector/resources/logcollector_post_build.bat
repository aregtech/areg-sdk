@echo on
SETLOCAL

:: full path of resource folder
set RESOURCE_DIR=%1
:: full path of target folder -- output folder
set TARGET_DIR=%2

:: set the 'system32' in the PATH to call 'copy' command
set OLDPATH=%PATH%
set PATH=%SystemRoot%\System32;%PATH%

:: create 'config' directory if does not exist and copy file
IF NOT EXIST "%TARGET_DIR%config" mkdir "%TARGET_DIR%config"
copy /D /y "%RESOURCE_DIR%logcollector.service.install.bat"   "%TARGET_DIR%config\"
copy /D /y "%RESOURCE_DIR%logcollector.service.uninstall.bat" "%TARGET_DIR%config\"

:: restore PATH
set PATH=%OLDPATH%
