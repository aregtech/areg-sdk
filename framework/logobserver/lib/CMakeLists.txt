set(logobserverapi_BASE		"${logobserver_BASE}/lib")
set(logobserverapi_RESOURCE	"${logobserverapi_BASE}/resources")
set(logobserverapi_SRC)

include("${logobserverapi_BASE}/private/CMakeLists.txt")

# add .def file to the list of sources
if("${AREG_DEVELOP_ENV}" STREQUAL "Win32")
	list(APPEND logobserverapi_SRC ${logobserverapi_RESOURCE}/logobserverapi.def)
endif()

