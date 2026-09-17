@echo off
rem Creates a ready-to-build Areg project. setup-project.ps1 beside this file does
rem the work and takes the same options: --name, --root, --mode, --sdk-root, --tag, --force.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0setup-project.ps1" %*
exit /b %errorlevel%
