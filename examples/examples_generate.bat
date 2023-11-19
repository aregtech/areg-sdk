echo off
cls

set EXAMPLE_BATCH_ROOT=%~dp0

:: set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=%EXAMPLE_BATCH_ROOT%..

cd %EXAMPLE_BATCH_ROOT%\00_helloservice\
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\10_locsvc
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\11_locmesh
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\12_pubsvc
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\13_pubmesh
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\14_pubtraffic
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\15_pubworker
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\16_pubfsm
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\17_winchat
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\18_locwatchdog
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\19_pubwatchdog
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\20_pubdatarate
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\21_pubunblock
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\22_pubsub
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\23_pubsubmix
call .\res\generate.bat

cd %EXAMPLE_BATCH_ROOT%\24_pubsubmulti
call .\res\generate.bat

@echo Completed to generate files...
