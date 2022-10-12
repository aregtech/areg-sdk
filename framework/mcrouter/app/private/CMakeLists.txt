list(APPEND mcrouter_SRC 
	${mcrouter_BASE}/app/private/MulticastRouter.cpp
	${mcrouter_BASE}/app/private/NEMulticastRouterSettings.cpp
    ${mcrouter_BASE}/app/private/ConsoleService.cpp
)

include("${mcrouter_BASE}/app/private/posix/CMakeLists.txt")
include("${mcrouter_BASE}/app/private/win32/CMakeLists.txt")
