list(APPEND logobserverapp_SRC 
	${logobserverapp_BASE}/private/LogObserver.cpp
	${logobserverapp_BASE}/private/NELogObserverSettings.cpp
    ${logobserverapp_BASE}/private/posix/LogObserverPosix.cpp
    ${logobserverapp_BASE}/private/win32/LogObserverWin32.cpp
)
