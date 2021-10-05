echo off
cls

:: set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=E:\Projects\aregtech\areg-sdk

:: .bat file directory
set BATCH_ROOT=%~dp0
:: In case of examples, one level up.
set PROJECT_ROOT=%BATCH_ROOT%\..
:: Specify the relative path of output folder
set CODE_GEN=generated
:: Include codegen.jar in CLASSPATH
set CLASSPATH=%CLASSPATH%;%AREG_SDK_ROOT%\tools;%AREG_SDK_ROOT%\tools\codegen.jar

@echo ......................................................
@echo Generating class of CentralMessager Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\CentralMessager.siml --target=%CODE_GEN%

@echo ......................................................
@echo Generating class of ConnectionManager Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\ConnectionManager.siml --target=%CODE_GEN%

@echo ......................................................
@echo Generating class of DirectConnection Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\DirectConnection.siml --target=%CODE_GEN%

@echo ......................................................
@echo Generating class of DirectMessager Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\DirectMessager.siml --target=%CODE_GEN%

@echo ......................................................
echo:
echo End of code generating

pause
