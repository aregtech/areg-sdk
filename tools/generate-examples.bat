echo off
cls

:: set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=E:\Projects\aregtech\areg-sdk

:: .bat file directory
set BATCH_ROOT=%~dp0
:: Specify the root folder of your project here
set PROJECT_ROOT=%BATCH_ROOT%..
:: Specify example root folder
set EXAMPLE_ROOT=%PROJECT_ROOT%\examples

echo on
run %EXAMPLE_ROOT%\10_locsvc\res\generate.bat
run %EXAMPLE_ROOT%\11_localmixed\res\generate.bat
run %EXAMPLE_ROOT%\12_pubsvc\res\generate.bat
run %EXAMPLE_ROOT%\13_pubmesh\res\generate.bat
run %EXAMPLE_ROOT%\14_pubtraffic\res\generate.bat
run %EXAMPLE_ROOT%\15_pubworker\res\generate.bat
run %EXAMPLE_ROOT%\16_pubfsm\res\generate.bat
run %EXAMPLE_ROOT%\17_winchat\res\generate.bat

echo off
@echo ......................................................
echo:
echo End of example code generating

pause
