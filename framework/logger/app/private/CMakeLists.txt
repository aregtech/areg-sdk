macro_add_source(logger_SRC "${AREG_FRAMEWORK}"
	logger/app/private/Logger.cpp
    logger/app/private/LoggerConsoleService.cpp
	logger/app/private/NELoggerSettings.cpp
    logger/app/private/posix/LoggerPosix.cpp
    logger/app/private/win32/LoggerWin32.cpp
)
