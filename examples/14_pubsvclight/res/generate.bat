echo off
cls

rem set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=E:\Projects\aregtech\areg-sdk

rem .bat file directory
set BATCH_ROOT=%~dp0
rem In case of examples, one level up.
set PROJECT_ROOT=%BATCH_ROOT%\..
rem Specify the relative path of output folder
set CODE_GEN=generated\src
rem Include codegen.jar in CLASSPATH
set CLASSPATH=%CLASSPATH%;%AREG_SDK_ROOT%\tools;%AREG_SDK_ROOT%\tools\codegen.jar

@echo ......................................................
@echo Generating class of SimpleTrafficLight Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\SimpleTrafficLight.siml --target=%CODE_GEN%

@echo ......................................................
@echo Generating class of SimpleTrafficSwitch Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\SimpleTrafficSwitch.siml --target=%CODE_GEN%

@echo ......................................................
echo:
echo End of code generating

pause
