macro_add_source(mtrouter_SRC "${AREG_FRAMEWORK}"
	mtrouter/app/private/MultitargetRouter.cpp
	mtrouter/app/private/NEMultitargetRouterSettings.cpp
    mtrouter/app/private/RouterConsoleService.cpp
    mtrouter/app/private/posix/MultitargetRouterPosix.cpp
    mtrouter/app/private/win32/MultitargetRouterWin32.cpp
)
