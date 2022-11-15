list(APPEND areg_SRC 
    ${areg_BASE}/appbase/private/Application.cpp
	${areg_BASE}/appbase/private/configure.cpp
	${areg_BASE}/appbase/private/Console.cpp
    ${areg_BASE}/appbase/private/NEApplication.cpp 
)

include("${areg_BASE}/appbase/private/posix/CMakeLists.txt")
include("${areg_BASE}/appbase/private/win32/CMakeLists.txt")
