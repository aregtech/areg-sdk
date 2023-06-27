list(APPEND logger_SRC 
	${logger_BASE}/app/private/Logger.cpp
    ${logger_BASE}/app/private/LoggerConsoleService.cpp
	${logger_BASE}/app/private/NELoggerSettings.cpp
    ${logger_BASE}/app/private/posix/LoggerPosix.cpp
    ${logger_BASE}/app/private/win32/LoggerWin32.cpp
)
