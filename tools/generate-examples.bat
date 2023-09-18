echo off
cls

:: .bat file directory
set BATCH_ROOT=%~dp0

:: set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=%BATCH_ROOT%\..

:: .bat file directory
set BATCH_ROOT=%~dp0
:: Specify the root folder of your project here
set PROJECT_ROOT=%BATCH_ROOT%\..
:: Specify example root folder
set EXAMPLE_ROOT=%PROJECT_ROOT%\examples

echo on
run %EXAMPLE_ROOT%\00_helloservice\res\generate.bat
run %EXAMPLE_ROOT%\10_locsvc\res\generate.bat
run %EXAMPLE_ROOT%\11_locmesh\res\generate.bat
run %EXAMPLE_ROOT%\12_pubsvc\res\generate.bat
run %EXAMPLE_ROOT%\13_pubmesh\res\generate.bat
run %EXAMPLE_ROOT%\14_pubtraffic\res\generate.bat
run %EXAMPLE_ROOT%\15_pubworker\res\generate.bat
run %EXAMPLE_ROOT%\16_pubfsm\res\generate.bat
run %EXAMPLE_ROOT%\17_winchat\res\generate.bat
run %EXAMPLE_ROOT%\18_locwatchdog\res\generate.bat
run %EXAMPLE_ROOT%\19_pubwatchdog\res\generate.bat
run %EXAMPLE_ROOT%\20_pubdatarate\res\generate.bat
run %EXAMPLE_ROOT%\21_pubunblock\res\generate.bat
run %EXAMPLE_ROOT%\22_pubsub\res\generate.bat
run %EXAMPLE_ROOT%\23_pubsubmix\res\generate.bat
run %EXAMPLE_ROOT%\24_pubsubmulti\res\generate.bat

echo off
@echo ......................................................
echo:
echo End of example code generating

pause
