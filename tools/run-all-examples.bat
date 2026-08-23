@echo off
rem ===========================================================================
rem  This file is part of the Areg SDK
rem  Copyright (c) 2017-2026, Aregtech (Artak Avetyan)
rem  Contact: info[at]areg.tech
rem  Website: https://www.areg.tech
rem ---------------------------------------------------------------------------
rem  Areg SDK -- run the example applications as a test suite (Windows).
rem
rem  Thin wrapper around 'tools\run-all-examples.py'. Every argument is passed
rem  through, so the options of the driver are available here as well:
rem
rem      tools\run-all-examples.bat                       :: default set
rem      tools\run-all-examples.bat --tier smoke          :: single process only
rem      tools\run-all-examples.bat --only 12_svcmulti --repeat 20
rem      tools\run-all-examples.bat --list
rem      tools\run-all-examples.bat --bin-dir path\to\bin :: explicit build to test,
rem                                                        :: e.g. to pick between an
rem                                                        :: MSVC and a Ninja build
rem
rem  Run it after the build and before the unit tests. Exit code 0 means every
rem  scenario passed.
rem ===========================================================================
setlocal EnableExtensions

set "SCRIPT_DIR=%~dp0"
set "DRIVER=%SCRIPT_DIR%run-all-examples.py"

if not exist "%DRIVER%" (
    echo [error] driver not found: %DRIVER% 1>&2
    exit /b 1
)

set "PYTHON="
for %%P in (python.exe python3.exe py.exe) do (
    if not defined PYTHON (
        for /f "delims=" %%I in ('where %%P 2^>nul') do (
            if not defined PYTHON set "PYTHON=%%I"
        )
    )
)

if not defined PYTHON (
    echo [error] Python 3.6 or newer is required to run the example test suite. 1>&2
    exit /b 1
)

"%PYTHON%" "%DRIVER%" %*
exit /b %ERRORLEVEL%
