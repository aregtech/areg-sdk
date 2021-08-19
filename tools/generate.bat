echo off
cls

rem set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=E:\Projects\aregtech\areg-sdk

rem .bat file directory
set BATCH_ROOT=%~dp0
rem Specify the root folder of your project here
set PROJECT_ROOT=%BATCH_ROOT%\..
rem Specify the relative path of output folder
set CODE_GEN=generated\src
rem Include codegen.jar in CLASSPATH
set CLASSPATH=%CLASSPATH%;%AREG_SDK_ROOT%\tools;%AREG_SDK_ROOT%\tools\codegen.jar

rem ==========================================================
rem -- root=<full path>                      - specify the root folder to ouptut generated codes.
rem --doc=<relative path>/<ServiceName>.siml - specify the relative path to the service interface file to parse.
rem --target=<relative path>                 - specify the relative path of the target folder to output codes.
rem ==========================================================
@echo ......................................................
@echo Generating class of <ServiceName> Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\<ServiceName>.siml --target=%CODE_GEN%

@echo ......................................................
echo:
echo End of code generating

pause
