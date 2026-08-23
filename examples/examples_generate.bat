@echo off
cls

set EXAMPLE_ROOT=%~dp0

:: set the AREG_SDK_ROOT directory here
for %%i in ("%EXAMPLE_ROOT%..") do set AREG_SDK_ROOT=%%~fi
set AREG_PRODUCT=%AREG_SDK_ROOT%\product\generate
set AREG_CODEGEN=%AREG_SDK_ROOT%\tools\codegen.jar
set AREG_DOCLIST=%AREG_PRODUCT%\examples_documents.lst

:: '--root'   the project root, imports are resolved against it.
:: '--target' the generate root, each document gets the folder its own path names under it.
:: '--docs'   a file listing the documents, one path per line, all generated in one run.

@echo "Generating service interface files of the Areg SDK examples"

if not exist "%AREG_PRODUCT%" mkdir "%AREG_PRODUCT%"
if exist "%AREG_DOCLIST%" del /q "%AREG_DOCLIST%"

:: The documents to generate. Add an example here to include it.
for %%d in (
    01_minimalrpc\services\HelloService.siml
    02_minimalipc\services\HelloService.siml
    03_helloservice\services\HelloService.siml
    13_locsvc\services\HelloWorld.siml
    14_locmesh\services\HelloWorld.siml
    15_pubsvc\services\HelloWorld.siml
    16_pubmesh\services\LocalHelloWorld.siml
    16_pubmesh\services\PublicHelloWorld.siml
    16_pubmesh\services\SystemShutdown.siml
    17_pubtraffic\services\SimpleTrafficLight.siml
    18_pubworker\services\PatientInformation.siml
    19_pubfsm\services\PowerManager.siml
    19_pubfsm\services\TrafficController.siml
    19_pubfsm\services\TrafficLight.fsml
    20_winchat\services\CentralMessager.siml
    20_winchat\services\ConnectionManager.siml
    20_winchat\services\DirectMessager.siml
    20_winchat\services\DirectConnection.siml
    21_locwatchdog\services\HelloWatchdog.siml
    22_pubwatchdog\services\HelloWatchdog.siml
    23_pubdatarate\services\LargeData.siml
    24_pubunblock\services\HelloUnblock.siml
    25_pubsub\services\PubSub.siml
    26_pubsubmix\services\PubSubMix.siml
    27_pubsubmulti\services\PubSub.siml
    30_publatency\services\Latency.siml
    31_loclatency\services\LocalLatency.siml
    32_pubmixed\services\MixedTraffic.siml
) do >>"%AREG_DOCLIST%" echo %EXAMPLE_ROOT%%%d

call java -jar "%AREG_CODEGEN%" --root="%AREG_SDK_ROOT%" --docs="%AREG_DOCLIST%" --target="%AREG_PRODUCT%"

:: A refused document reaches the compiler as a missing header, naming neither the document
:: nor the reason. Stop here instead.
if errorlevel 1 (
    @echo Failed to generate the service interface files, the build cannot continue.
    exit /b 1
)

@echo Completed to generate files...
