macro_add_source(mcrouter_SRC "${AREG_FRAMEWORK}"
	mcrouter/app/private/MultitargetRouter.cpp
	mcrouter/app/private/NEMultitargetRouterSettings.cpp
    mcrouter/app/private/RouterConsoleService.cpp
    mcrouter/app/private/posix/MultitargetRouterPosix.cpp
    mcrouter/app/private/win32/MultitargetRouterWin32.cpp
)
