echo off
cls

set PATH=D:\Program Files\jdk12;D:\Program Files\jdk12\lib;D:\Program Files\jdk12\bin;%PATH%
set PROJECT_ROOT=E:\Projects\aregtech\areg-sdk-dev\examples\16_pubfsm
set CODE_GEN=generated\src
set CLASSPATH=D:\Program Files\jdk12\lib;%CLASSPATH%;E:\Projects\aregtech\areg-sdk\tools;E:\Projects\aregtech\areg-sdk\tools\codegen.jar

set NEW_LINE=echo:

cd %PROJECT_ROOT%

@echo ......................................................
%NEW_LINE%
@echo Generating class of PowerManager Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\PowerManager.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
@echo Generating class of TrafficController Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\TrafficController.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
echo End of code generating

pause
