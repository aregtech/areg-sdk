macro_add_source(mcrouter_SRC "${AREG_FRAMEWORK}"
	mcrouter/app/private/MulticastRouter.cpp
	mcrouter/app/private/NEMulticastRouterSettings.cpp
    mcrouter/app/private/RouterConsoleService.cpp
    mcrouter/app/private/posix/MulticastRouterPosix.cpp
    mcrouter/app/private/win32/MulticastRouterWin32.cpp
)
