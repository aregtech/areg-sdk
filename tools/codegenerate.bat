echo on
cls

:: set the AREG_SDK_ROOT directory here
if (%AREG_SDK_ROOT%=="") (set AREG_SDK_ROOT="c:\path\to\areg-sdk")


echo %AREG_SDK_ROOT%

:: .bat file directory
set BATCH_ROOT=%~dp0
:: In case of examples, one level up.
set PROJECT_ROOT=%BATCH_ROOT%..
:: Specify the relative path of output folder
set CODE_GEN=generated\src
:: Include codegen.jar in CLASSPATH
set CLASSPATH=%CLASSPATH%;%AREG_SDK_ROOT%\tools;%AREG_SDK_ROOT%\tools\codegen.jar

:: ==========================================================
:: --root=<full path>                       - specify the root folder of the project.
:: --doc=<relative path>/<ServiceName>.siml - specify the relative path to the service interface file to parse.
:: --target=<relative path>                 - specify the relative path of the target folder to output codes.
:: ==========================================================
@echo ......................................................
@echo Generating class of <ServiceName> Service Interface in folder %CODE_GEN% .....
java -jar %AREG_SDK_ROOT%\tools\codegen.jar --root=%PROJECT_ROOT% --doc=services\<ServiceName>.siml --target=%CODE_GEN%

@echo ......................................................
echo:
echo End of code generating

pause
