echo off
cls

set PATH=D:\Program Files\jdk12;D:\Program Files\jdk12\lib;D:\Program Files\jdk12\bin;%PATH%
set PROJECT_ROOT=E:\Projects\aregtech\areg-sdk\examples\12_ipchello
set CODE_GEN=shripchello\src
set CLASSPATH=D:\Program Files\jdk12\lib;%CLASSPATH%;E:\Projects\aregtech\areg-sdk\tools;E:\Projects\aregtech\areg-sdk\tools\codegen.jar

set NEW_LINE=echo:

rem cd %CODE_GEN_ROOT%

@echo ......................................................
%NEW_LINE%
@echo Generating class of HelloWorld Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\HelloWorld.siml --target=%CODE_GEN%

@echo ......................................................
%NEW_LINE%
echo End of code generating

pause
