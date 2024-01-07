list(APPEND logobserver_SRC 
	${logobserver_BASE}/app/private/LogObserver.cpp
	${logobserver_BASE}/app/private/NELogObserverSettings.cpp
    ${logobserver_BASE}/app/private/posix/LogObserverPosix.cpp
    ${logobserver_BASE}/app/private/win32/LogObserverWin32.cpp
)
