@echo off
cls

set EXAMPLE_ROOT=%~dp0

:: set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=%EXAMPLE_ROOT%..
set AREG_PRODUCT=%AREG_SDK_ROOT%\product\generate

@echo "Generating service interface files of the AREG SDK examples"

:: Generate codes for example 00_helloservice
@echo "Generating service interface files of 00_helloservice example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\00_helloservice\services\HelloService.siml --target=examples\00_helloservice\services

:: Generate codes for example 10_locsvc
@echo "Generating service interface files of 10_locsvc example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\10_locsvc\services\HelloWorld.siml --target=examples\10_locsvc\services

:: Generate codes for example 11_locmesh
@echo "Generating service interface files of 11_locmesh example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\11_locmesh\services\HelloWorld.siml --target=examples\11_locmesh\services

:: Generate codes for example 12_pubsvc
@echo "Generating service interface files of 12_pubsvc example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\12_pubsvc\services\HelloWorld.siml --target=examples\12_pubsvc\services

:: Generate codes for example 13_pubmesh
@echo "Generating service interface files of 13_pubmesh example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\13_pubmesh\services\LocalHelloWorld.siml  --target=examples\13_pubmesh\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\13_pubmesh\services\PublicHelloWorld.siml --target=examples\13_pubmesh\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\13_pubmesh\services\SystemShutdown.siml   --target=examples\13_pubmesh\services

:: Generate codes for example 14_pubtraffic
@echo "Generating service interface files of 14_pubtraffic example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\14_pubtraffic\services\SimpleTrafficLight.siml --target=examples\14_pubtraffic\services

:: Generate codes for example 15_pubworker
@echo "Generating service interface files of 15_pubworker example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\15_pubworker\services\PatientInformation.siml --target=examples\15_pubworker\services

:: Generate codes for example 16_pubfsm
@echo "Generating service interface files of 16_pubfsm example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\16_pubfsm\services\PowerManager.siml --target=examples\16_pubfsm\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\16_pubfsm\services\TrafficController.siml --target=examples\16_pubfsm\services

:: Generate codes for example 17_winchat
@echo "Generating service interface files of 17_winchat example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\17_winchat\services\CentralMessager.siml --target=examples\17_winchat\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\17_winchat\services\ConnectionManager.siml --target=examples\17_winchat\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\17_winchat\services\DirectMessager.siml --target=examples\17_winchat\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\17_winchat\services\DirectConnection.siml --target=examples\17_winchat\services

:: Generate codes for example 18_locwatchdog
@echo "Generating service interface files of 18_locwatchdog example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\18_locwatchdog\services\HelloWatchdog.siml --target=examples\18_locwatchdog\services

:: Generate codes for example 19_pubwatchdog
@echo "Generating service interface files of 19_pubwatchdog example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\19_pubwatchdog\services\HelloWatchdog.siml --target=examples\19_pubwatchdog\services

:: Generate codes for example 20_pubdatarate
@echo "Generating service interface files of 20_pubdatarate example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\20_pubdatarate\services\LargeData.siml --target=examples\20_pubdatarate\services

:: Generate codes for example 21_pubunblock
@echo "Generating service interface files of 21_pubunblock example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\21_pubunblock\services\HelloUnblock.siml --target=examples\21_pubunblock\services

:: Generate codes for example 22_pubsub
@echo "Generating service interface files of 22_pubsub example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\22_pubsub\services\PubSub.siml --target=examples\22_pubsub\services

:: Generate codes for example 23_pubsubmix
@echo "Generating service interface files of 23_pubsubmix example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\23_pubsubmix\services\PubSubMix.siml --target=examples\23_pubsubmix\services

:: Generate codes for example 24_pubsubmulti
@echo "Generating service interface files of 24_pubsubmulti example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\24_pubsubmulti\services\PubSub.siml --target=examples\24_pubsubmulti\services

@echo Completed to generate files...
