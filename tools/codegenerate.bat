@echo off
:: ==========================================================================
:: Template to generate code from an Areg document on Windows. Copy it into
:: your project, set AREG_SDK_ROOT and DOCUMENT below, and run it.
::
:: The generator takes three paths:
::   --root=<path>   the project root. Absolute.
::   --doc=<path>    the .siml, .fsml or .dtml document to generate from.
::                   Absolute, or relative to --root.
::   --target=<path> the generate root: the one folder the project's generated
::                   sources go under. Absolute, or relative to --root. Each
::                   document gets the folder its own path names inside it.
::
:: A document that includes another one is generated together with it. Name
:: the .siml or the .fsml here and its includes come with it.
:: ==========================================================================

:: The Areg SDK root. Set it here, or in the environment before running.
if "%AREG_SDK_ROOT%"=="" (set "AREG_SDK_ROOT=c:\path\to\areg-sdk")

:: The document to generate from, relative to the project root.
set "DOCUMENT=services\ServiceName.siml"

:: The folder of this file, and the project root one level above it.
set "BATCH_ROOT=%~dp0"
set "PROJECT_ROOT=%BATCH_ROOT%.."

:: The generate root, relative to the project root.
set "CODE_GEN=generated\src"

echo AREG_SDK_ROOT = %AREG_SDK_ROOT%
echo ......................................................
echo Generating the code of %DOCUMENT% into %CODE_GEN% .....

java -jar "%AREG_SDK_ROOT%\tools\codegen.jar" "--root=%PROJECT_ROOT%" "--doc=%DOCUMENT%" "--target=%CODE_GEN%"
if errorlevel 1 (
    echo.
    echo The code generator failed. See the messages above.
    pause
    exit /b 1
)

echo ......................................................
echo.
echo End of code generating
pause
