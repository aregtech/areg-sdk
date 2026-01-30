@echo off
cls

set EXAMPLE_ROOT=%~dp0

:: set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=%EXAMPLE_ROOT%..
set AREG_PRODUCT=%AREG_SDK_ROOT%\product\generate

@echo "Generating service interface files of the Areg SDK examples"

:: Generate codes for example 01_minimalrpc
@echo "Generating service interface files of 01_minimalrpc example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\01_minimalrpc\services\HelloService.siml --target=examples\01_minimalrpc\services

:: Generate codes for example 02_minimalipc
@echo "Generating service interface files of 02_minimalipc example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\02_minimalipc\services\HelloService.siml --target=examples\02_minimalipc\services

:: Generate codes for example 03_helloservice
@echo "Generating service interface files of 03_helloservice example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\03_helloservice\services\HelloService.siml --target=examples\03_helloservice\services

:: Generate codes for example 13_locsvc
@echo "Generating service interface files of 13_locsvc example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\13_locsvc\services\HelloWorld.siml --target=examples\13_locsvc\services

:: Generate codes for example 14_locmesh
@echo "Generating service interface files of 14_locmesh example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\14_locmesh\services\HelloWorld.siml --target=examples\14_locmesh\services

:: Generate codes for example 15_pubsvc
@echo "Generating service interface files of 15_pubsvc example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\15_pubsvc\services\HelloWorld.siml --target=examples\15_pubsvc\services

:: Generate codes for example 16_pubmesh
@echo "Generating service interface files of 16_pubmesh example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\16_pubmesh\services\LocalHelloWorld.siml  --target=examples\16_pubmesh\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\16_pubmesh\services\PublicHelloWorld.siml --target=examples\16_pubmesh\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\16_pubmesh\services\SystemShutdown.siml   --target=examples\16_pubmesh\services

:: Generate codes for example 17_pubtraffic
@echo "Generating service interface files of 17_pubtraffic example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\17_pubtraffic\services\SimpleTrafficLight.siml --target=examples\17_pubtraffic\services

:: Generate codes for example 18_pubworker
@echo "Generating service interface files of 18_pubworker example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\18_pubworker\services\PatientInformation.siml --target=examples\18_pubworker\services

:: Generate codes for example 19_pubfsm
@echo "Generating service interface files of 19_pubfsm example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\19_pubfsm\services\PowerManager.siml        --target=examples\19_pubfsm\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\19_pubfsm\services\TrafficController.siml   --target=examples\19_pubfsm\services

:: Generate codes for example 20_winchat
@echo "Generating service interface files of 20_winchat example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\20_winchat\services\CentralMessager.siml    --target=examples\20_winchat\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\20_winchat\services\ConnectionManager.siml  --target=examples\20_winchat\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\20_winchat\services\DirectMessager.siml     --target=examples\20_winchat\services
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\20_winchat\services\DirectConnection.siml   --target=examples\20_winchat\services

:: Generate codes for example 21_locwatchdog
@echo "Generating service interface files of 21_locwatchdog example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\21_locwatchdog\services\HelloWatchdog.siml --target=examples\21_locwatchdog\services

:: Generate codes for example 22_pubwatchdog
@echo "Generating service interface files of 22_pubwatchdog example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\22_pubwatchdog\services\HelloWatchdog.siml --target=examples\22_pubwatchdog\services

:: Generate codes for example 23_pubdatarate
@echo "Generating service interface files of 23_pubdatarate example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\23_pubdatarate\services\LargeData.siml --target=examples\23_pubdatarate\services

:: Generate codes for example 24_pubunblock
@echo "Generating service interface files of 24_pubunblock example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\24_pubunblock\services\HelloUnblock.siml --target=examples\24_pubunblock\services

:: Generate codes for example 25_pubsub
@echo "Generating service interface files of 25_pubsub example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\25_pubsub\services\PubSub.siml --target=examples\25_pubsub\services

:: Generate codes for example 26_pubsubmix
@echo "Generating service interface files of 26_pubsubmix example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\26_pubsubmix\services\PubSubMix.siml --target=examples\26_pubsubmix\services

:: Generate codes for example 27_pubsubmulti
@echo "Generating service interface files of 27_pubsubmulti example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\27_pubsubmulti\services\PubSub.siml --target=examples\27_pubsubmulti\services

@echo Completed to generate files...
