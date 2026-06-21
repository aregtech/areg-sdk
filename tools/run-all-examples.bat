@echo off
rem ===========================================================================
rem  Areg SDK - run all example applications as a smoke test (Windows).
rem  ---------------------------------------------------------------------------
rem  Launches every built example, makes sure it starts and does not crash, then
rem  runs the unit-test binary. Interactive examples that wait for the user to
rem  quit are stopped after a few seconds - a clean start is the pass criterion.
rem
rem    Rules:
rem      * Examples whose name contains 'pub' are IPC examples and need
rem        'mtrouter'; the router is started before the run and stopped after.
rem      * Other examples run standalone.
rem      * Example 20 (winchat) is a Windows GUI app and is skipped.
rem
rem    Usage:  tools\run-all-examples.bat [bin-dir]
rem      bin-dir   Directory holding the built binaries. Auto-detected under
rem                <root>\product\build\*\*\bin when omitted.
rem
rem    Env:
rem      RUN_SECONDS   Seconds to let an interactive example run before stopping
rem                    it (default: 3).
rem
rem    Exit code: 0 if everything passed, 1 otherwise.
rem ===========================================================================
setlocal EnableExtensions EnableDelayedExpansion

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "ROOT_DIR=%%~fI"
if "%RUN_SECONDS%"=="" set "RUN_SECONDS=3"

rem --- locate the binary output directory (first argument overrides) ---
set "BIN_DIR=%~1"
if "%BIN_DIR%"=="" (
    for /f "delims=" %%D in ('dir /b /ad /o-d "%ROOT_DIR%\product\build" 2^>nul') do (
        for /f "delims=" %%E in ('dir /b /ad /o-d "%ROOT_DIR%\product\build\%%D" 2^>nul') do (
            if not defined BIN_DIR if exist "%ROOT_DIR%\product\build\%%D\%%E\bin\" set "BIN_DIR=%ROOT_DIR%\product\build\%%D\%%E\bin"
        )
    )
)
if "%BIN_DIR%"=="" (
    echo [error] binary directory not found - build the examples or pass it as an argument.
    exit /b 1
)
if not exist "%BIN_DIR%\" (
    echo [error] binary directory does not exist: %BIN_DIR%
    exit /b 1
)
pushd "%BIN_DIR%"
echo ==^> Binaries: %BIN_DIR%

rem --- find router + unit-test binaries ---
set "ROUTER="
if exist "mtrouter.exe" set "ROUTER=mtrouter.exe"
set "UNIT="
if exist "areg-unit-tests.exe" set "UNIT=areg-unit-tests.exe"

set /a total=0, passed=0, failed=0, skipped=0
set "failed_list="

rem --- start the router if any IPC (pub) example is present ---
set "NEED_ROUTER=0"
for %%F in (??_*pub*.exe) do set "NEED_ROUTER=1"
set "ROUTER_STARTED=0"
if "%NEED_ROUTER%"=="1" (
    if defined ROUTER (
        start "areg-mtrouter" /min "%ROUTER%"
        set "ROUTER_STARTED=1"
        ping -n 2 127.0.0.1 >nul
        echo ==^> Started mtrouter
    ) else (
        echo [warn] mtrouter.exe not found - IPC examples run without a router.
    )
)

echo ==^> Running examples (%RUN_SECONDS%s each)
for %%F in (??_*.exe) do (
    set "base=%%~nF"
    if /i "!base:~0,3!"=="20_" (
        echo   SKIP  %%~nxF ^(Windows GUI^)
        set /a skipped+=1
    ) else (
        set /a total+=1
        call :run_app "%%~fF" RC
        if "!RC!"=="0" (
            echo   PASS  %%~nxF ^(clean exit^)
            set /a passed+=1
        ) else if "!RC!"=="124" (
            echo   PASS  %%~nxF ^(ran %RUN_SECONDS%s, stopped^)
            set /a passed+=1
        ) else (
            echo   FAIL  %%~nxF ^(exit !RC!^)
            set /a failed+=1
            set "failed_list=!failed_list! %%~nxF"
        )
    )
)

if "%ROUTER_STARTED%"=="1" taskkill /f /im mtrouter.exe >nul 2>&1

rem --- run the unit tests ---
if defined UNIT (
    echo.
    echo ==^> Running unit tests: %UNIT%
    set /a total+=1
    "%UNIT%"
    if errorlevel 1 (
        echo   FAIL  unit tests
        set /a failed+=1
        set "failed_list=!failed_list! areg-unit-tests"
    ) else (
        echo   PASS  unit tests
        set /a passed+=1
    )
) else (
    echo.
    echo [warn] areg-unit-tests.exe not found - skipped.
)

echo.
echo ==== Summary ====
echo   passed:  %passed%
echo   failed:  %failed%
echo   skipped: %skipped%
echo   total:   %total%
popd
if %failed% NEQ 0 (
    echo Failed:%failed_list%
    exit /b 1
)
echo All good.
exit /b 0

rem ---------------------------------------------------------------------------
rem  :run_app "<full path>" <out-var>
rem  Runs the app, stops it after RUN_SECONDS. Sets <out-var> to: 0 clean exit,
rem  124 still running when stopped (interactive OK), else the error exit code.
rem ---------------------------------------------------------------------------
:run_app
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='SilentlyContinue'; $p=Start-Process -FilePath '%~1' -PassThru -WindowStyle Hidden; if($null -eq $p){ exit 200 }; if(-not $p.WaitForExit(%RUN_SECONDS% * 1000)){ try{ $p.Kill($true) }catch{}; exit 124 } else { exit $p.ExitCode }"
endlocal & set "%~2=%ERRORLEVEL%"
goto :eof
