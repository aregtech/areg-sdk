@echo off
REM ============================================================================
REM  build.bat
REM
REM  Runs build.ps1 with the same arguments; see build.ps1 --help.
REM  The command line reaches build.ps1 unparsed, so -DNAME:TYPE=VALUE stays whole.
REM ============================================================================
setlocal
set "AREG_BUILD_ARGS=%*"
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build.ps1"
exit /b %errorlevel%
