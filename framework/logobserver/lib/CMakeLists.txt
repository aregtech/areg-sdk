set(logobserverapi_BASE		"${logobserver_BASE}/lib")
set(logobserverapi_RESOURCE	"${logobserverapi_BASE}/resources")
set(logobserverapi_SRC)

if("${AREG_DEVELOP_ENV}" STREQUAL "Win32")
	list(APPEND logobserverapi_SRC ${logobserverapi_RESOURCE}/observerapi.def)
endif()

include("${logobserverapi_BASE}/private/CMakeLists.txt")
