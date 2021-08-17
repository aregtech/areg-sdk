echo off
cls

set PATH=D:\Program Files\jdk12;D:\Program Files\jdk12\lib;D:\Program Files\jdk12\bin;%PATH%
set PROJECT_ROOT=E:\Projects\aregtech\areg-sdk\examples\messager
set CODE_GEN=shared\generated
set CLASSPATH=D:\Program Files\jdk12\lib;%CLASSPATH%;E:\Projects\aregtech\areg-sdk\tools;E:\Projects\aregtech\areg-sdk\tools\codegen.jar

set NEW_LINE=echo:

cd %PROJECT_ROOT%

@echo ......................................................
%NEW_LINE%
@echo Generating class of MessageOutput Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\CentralMessager.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
@echo Generating class of System Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\ConnectionManager.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
@echo Generating class of System Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\DirectConnection.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
@echo Generating class of System Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\DirectMessager.siml --target=%CODE_GEN%


@echo ......................................................
%NEW_LINE%
echo End of code generating

pause
