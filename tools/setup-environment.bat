@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0setup-environment.ps1" %*
exit /b %errorlevel%
