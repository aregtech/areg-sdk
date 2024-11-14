macro_add_source(logger_SRC "${AREG_FRAMEWORK}"
	logcollector/app/private/LogCollector.cpp
    logcollector/app/private/LogCollectorConsoleService.cpp
	logcollector/app/private/NELogCollectorSettings.cpp
    logcollector/app/private/posix/LogCollectorPosix.cpp
    logcollector/app/private/win32/LogCollectorWin32.cpp
)
