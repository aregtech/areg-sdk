@echo off
REM ============================================================================
REM  check-env.bat
REM
REM  Reports whether this machine can build an AREG application.
REM  Needs no interpreter, so it also works when Python is the missing piece.
REM
REM  Usage:  tools\check-env.bat
REM  Exit:   0 = every requirement is present, 1 = at least one is missing.
REM ============================================================================

setlocal EnableDelayedExpansion
set "FAILED=0"

echo.
echo AREG environment check
echo ----------------------

REM --- CMake 3.20+ : required -------------------------------------------------
where cmake >nul 2>&1
if errorlevel 1 (
    echo   [MISSING]  cmake      - required 3.20+
    echo              get it from https://cmake.org/download/ or: winget install Kitware.CMake
    set "FAILED=1"
) else (
    for /f "tokens=3" %%v in ('cmake --version 2^>^&1 ^| findstr /r /c:"cmake version"') do set "CMAKE_VER=%%v"
    echo   [ok]       cmake      !CMAKE_VER!
)

REM --- Java 17+ : required, runs tools\codegen.jar ----------------------------
where java >nul 2>&1
if errorlevel 1 (
    echo   [MISSING]  java       - required 17+, runs tools\codegen.jar
    echo              get it from https://adoptium.net/ or: winget install EclipseAdoptium.Temurin.17.JRE
    set "FAILED=1"
) else (
    for /f "tokens=3" %%v in ('java -version 2^>^&1 ^| findstr /i "version"') do set "JAVA_VER=%%v"
    echo   [ok]       java       !JAVA_VER!
)

REM --- C++ compiler : required ------------------------------------------------
set "HAVE_CXX=0"
where cl >nul 2>&1        && set "HAVE_CXX=1" && set "CXX_NAME=cl (MSVC)"
if "!HAVE_CXX!"=="0" ( where g++ >nul 2>&1   && set "HAVE_CXX=1" && set "CXX_NAME=g++" )
if "!HAVE_CXX!"=="0" ( where clang++ >nul 2>&1 && set "HAVE_CXX=1" && set "CXX_NAME=clang++" )
if "!HAVE_CXX!"=="0" (
    echo   [MISSING]  c++        - required C++17 compiler
    echo              install Visual Studio Build Tools, or run this from a
    echo              "Developer Command Prompt" so that cl.exe is on PATH.
    set "FAILED=1"
) else (
    echo   [ok]       c++        !CXX_NAME!
)

REM --- Python 3 : optional ----------------------------------------------------
set "HAVE_PY=0"
python --version >nul 2>&1 && set "HAVE_PY=1"
if "!HAVE_PY!"=="0" (
    echo   [absent]   python     - OPTIONAL. Only the SDK helper scripts need it.
    echo              Without it, copy a recipe from docs\agent\recipes\ instead.
) else (
    for /f "tokens=2" %%v in ('python --version 2^>^&1') do set "PY_VER=%%v"
    echo   [ok]       python     !PY_VER!
)

echo.
if "%FAILED%"=="1" (
    echo RESULT: cannot build. Install the [MISSING] items above.
    endlocal & exit /b 1
) else (
    echo RESULT: ready to build.
    echo   next: cmake -B build  ^&^&  cmake --build build -j
    endlocal & exit /b 0
)
