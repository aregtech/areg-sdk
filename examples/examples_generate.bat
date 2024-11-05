@echo off
cls

set EXAMPLE_ROOT=%~dp0

:: set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=%EXAMPLE_ROOT%..
set AREG_PRODUCT=%AREG_SDK_ROOT%\product

@echo "Generating service interface files of the AREG SDK examples"

:: Generate codes for example 00_helloservice
@echo "Generating service interface files of 00_helloservice example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\00_helloservice\res\HelloService.siml --target=generate\examples\00_helloservice

:: Generate codes for example 10_locsvc
@echo "Generating service interface files of 10_locsvc example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\10_locsvc\res\HelloWorld.siml --target=generate\examples\10_locsvc

:: Generate codes for example 11_locmesh
@echo "Generating service interface files of 11_locmesh example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\11_locmesh\res\HelloWorld.siml --target=generate\examples\11_locmesh

:: Generate codes for example 12_pubsvc
@echo "Generating service interface files of 12_pubsvc example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\12_pubsvc\res\HelloWorld.siml --target=generate\examples\12_pubsvc

:: Generate codes for example 13_pubmesh
@echo "Generating service interface files of 13_pubmesh example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\13_pubmesh\res\LocalHelloWorld.siml  --target=generate\examples\13_pubmesh
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\13_pubmesh\res\PublicHelloWorld.siml --target=generate\examples\13_pubmesh
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\13_pubmesh\res\SystemShutdown.siml   --target=generate\examples\13_pubmesh

:: Generate codes for example 14_pubtraffic
@echo "Generating service interface files of 14_pubtraffic example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\14_pubtraffic\res\SimpleTrafficLight.siml --target=generate\examples\14_pubtraffic

:: Generate codes for example 15_pubworker
@echo "Generating service interface files of 15_pubworker example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\15_pubworker\res\PatientInformation.siml --target=generate\examples\15_pubworker

:: Generate codes for example 16_pubfsm
@echo "Generating service interface files of 16_pubfsm example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\16_pubfsm\res\PowerManager.siml --target=generate\examples\16_pubfsm
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\16_pubfsm\res\TrafficController.siml --target=generate\examples\16_pubfsm

:: Generate codes for example 17_winchat
@echo "Generating service interface files of 17_winchat example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\17_winchat\res\CentralMessager.siml --target=generate\examples\17_winchat
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\17_winchat\res\ConnectionManager.siml --target=generate\examples\17_winchat
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\17_winchat\res\DirectMessager.siml --target=generate\examples\17_winchat
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\17_winchat\res\DirectConnection.siml --target=generate\examples\17_winchat

:: Generate codes for example 18_locwatchdog
@echo "Generating service interface files of 18_locwatchdog example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\18_locwatchdog\res\HelloWatchdog.siml --target=generate\examples\18_locwatchdog

:: Generate codes for example 19_pubwatchdog
@echo "Generating service interface files of 19_pubwatchdog example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\19_pubwatchdog\res\HelloWatchdog.siml --target=generate\examples\19_pubwatchdog

:: Generate codes for example 20_pubdatarate
@echo "Generating service interface files of 20_pubdatarate example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\20_pubdatarate\res\LargeData.siml --target=generate\examples\20_pubdatarate

:: Generate codes for example 21_pubunblock
@echo "Generating service interface files of 21_pubunblock example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\21_pubunblock\res\HelloUnblock.siml --target=generate\examples\21_pubunblock

:: Generate codes for example 22_pubsub
@echo "Generating service interface files of 22_pubsub example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\22_pubsub\res\PubSub.siml --target=generate\examples\22_pubsub

:: Generate codes for example 23_pubsubmix
@echo "Generating service interface files of 23_pubsubmix example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\23_pubsubmix\res\PubSubMix.siml --target=generate\examples\23_pubsubmix

:: Generate codes for example 24_pubsubmulti
@echo "Generating service interface files of 24_pubsubmulti example"
call java -jar %AREG_SDK_ROOT%/tools/codegen.jar --root=%AREG_PRODUCT% --doc=%EXAMPLE_ROOT%\24_pubsubmulti\res\PubSub.siml --target=generate\examples\24_pubsubmulti

@echo Completed to generate files...
