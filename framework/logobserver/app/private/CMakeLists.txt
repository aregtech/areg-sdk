macro_add_source(logobserver_SRC "${AREG_BASE}"
	logobserver/app/private/LogObserver.cpp
	logobserver/app/private/NELogObserverSettings.cpp
    logobserver/app/private/posix/LogObserverPosix.cpp
    logobserver/app/private/win32/LogObserverWin32.cpp
)
