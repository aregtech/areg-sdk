echo off
cls

set PATH=D:\Program Files\jdk12;D:\Program Files\jdk12\lib;D:\Program Files\jdk12\bin;%PATH%
set PROJECT_ROOT=E:\Projects\aregtech\areg-sdk\examples\messager
set CODE_GEN=shared\generated
rem set CLASSPATH=D:\Program Files\jdk12\lib;%CLASSPATH%;E:\Projects\tools\sidesign\trunk\codegen\dist;E:\Projects\tools\sidesign\trunk\codegen\dist\codegen.jar;.\dist;.\dist\codegen.jar
set CLASSPATH=D:\Program Files\jdk12\lib;%CLASSPATH%;E:\Projects\aregtech\areg-sdk\tools;E:\Projects\aregtech\areg-sdk\tools\codegen.jar

set NEW_LINE=echo:

rem cd %CODE_GEN_ROOT%

@echo ......................................................
%NEW_LINE%
@echo Generating class of MessageOutput Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=resources\CentralMessager.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
@echo Generating class of System Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=resources\ConnectionManager.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
@echo Generating class of System Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=resources\DirectConnection.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
@echo Generating class of System Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=resources\DirectMessager.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
rem @echo Generating class of DriectMessager Service Interface in folder %CODE_GEN% .....
rem java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=resources\DirectMessager.siml --target=%CODE_GEN%



@echo ......................................................
%NEW_LINE%
echo End of code generating

pause
