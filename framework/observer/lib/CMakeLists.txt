set(observer_lib_BASE "${observer_BASE}/lib")
set(observer_lib_RESOURCE "${observer_lib_BASE}/resources")
set(observer_lib_SRC)

if("${AREG_DEVELOP_ENV}" STREQUAL "Win32")
	list(APPEND observer_lib_SRC 
		${observer_lib_RESOURCE}/observerapi.def
	)
endif()

include("${observer_lib_BASE}/private/CMakeLists.txt")
