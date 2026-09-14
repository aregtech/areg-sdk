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
REM  A version is compared as two numbers, so 3.9 is older than 3.20. A version
REM  this cannot read is reported as present: the build is what finds out.
where cmake >nul 2>&1
if errorlevel 1 (
    echo   [MISSING]  cmake      - required 3.20+
    echo              get it from https://cmake.org/download/ or: winget install Kitware.CMake
    set "FAILED=1"
) else (
    for /f "tokens=3" %%v in ('cmake --version 2^>^&1 ^| findstr /r /c:"cmake version"') do set "CMAKE_VER=%%v"
    set "C_MAJ=0"
    set "C_MIN=0"
    set "C_OLD=0"
    for /f "tokens=1,2 delims=." %%a in ("!CMAKE_VER!") do (
        set /a "C_MAJ=%%a" >nul 2>&1
        set /a "C_MIN=%%b" >nul 2>&1
    )
    if !C_MAJ! GTR 0 (
        if !C_MAJ! LSS 3 set "C_OLD=1"
        if !C_MAJ! EQU 3 if !C_MIN! LSS 20 set "C_OLD=1"
    )
    if "!C_OLD!"=="1" (
        echo   [OLD]      cmake      !CMAKE_VER! - required 3.20+
        echo              get it from https://cmake.org/download/ or: winget install Kitware.CMake
        set "FAILED=1"
    ) else (
        echo   [ok]       cmake      !CMAKE_VER!
    )
)

REM --- Java 17+ : required, runs tools\codegen.jar ----------------------------
REM  Java 8 and older spell themselves 1.8.0_292, so the number that decides is
REM  the one after the 1.
where java >nul 2>&1
if errorlevel 1 (
    echo   [MISSING]  java       - required 17+, runs tools\codegen.jar
    echo              get it from https://adoptium.net/ or: winget install EclipseAdoptium.Temurin.17.JRE
    set "FAILED=1"
) else (
    for /f "tokens=3" %%v in ('java -version 2^>^&1 ^| findstr /i "version"') do set "JAVA_VER=%%v"
    set JAVA_VER=!JAVA_VER:"=!
    set "J_CMP=!JAVA_VER!"
    if "!J_CMP:~0,2!"=="1." set "J_CMP=!J_CMP:~2!"
    set "J_MAJ=0"
    set "J_OLD=0"
    for /f "tokens=1 delims=._-" %%a in ("!J_CMP!") do set /a "J_MAJ=%%a" >nul 2>&1
    if !J_MAJ! GTR 0 if !J_MAJ! LSS 17 set "J_OLD=1"
    if "!J_OLD!"=="1" (
        echo   [OLD]      java       !JAVA_VER! - required 17+, runs tools\codegen.jar
        echo              get it from https://adoptium.net/ or: winget install EclipseAdoptium.Temurin.17.JRE
        set "FAILED=1"
    ) else (
        echo   [ok]       java       !JAVA_VER!
    )
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
    echo   next: cmake -B build  ^&^&  cmake --build build -j8
    endlocal & exit /b 0
)
