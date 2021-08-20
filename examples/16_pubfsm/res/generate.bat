echo off
cls

set AREG_SDK_ROOT=E:\Projects\aregtech\areg-sdk

set BATCH_ROOT=%~dp0
set PROJECT_ROOT=%BATCH_ROOT%\..
set CODE_GEN=generated\src
set CLASSPATH=%CLASSPATH%;%AREG_SDK_ROOT%\tools;%AREG_SDK_ROOT%\tools\codegen.jar

@echo ......................................................
@echo Generating class of PowerManager Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\PowerManager.siml --target=%CODE_GEN%

@echo ......................................................
@echo Generating class of TrafficController Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\TrafficController.siml --target=%CODE_GEN%

@echo ......................................................
echo:
echo End of code generating

pause
