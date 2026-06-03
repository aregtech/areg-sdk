macro_add_source(aregextend_SRC "${AREG_FRAMEWORK}"
    aregextend/console/private/Console.cpp
    aregextend/console/private/OptionParser.cpp
    aregextend/console/private/SystemServiceConsole.cpp
    aregextend/console/private/ansi/ConsoleAnsi.cpp
)
if (WIN32)
    macro_add_source(aregextend_SRC "${AREG_FRAMEWORK}"
        aregextend/console/private/win32/ConsoleWin32.cpp
    )
else()
    macro_add_source(aregextend_SRC "${AREG_FRAMEWORK}"
        aregextend/console/private/ncurses/ConsoleNcurses.cpp
    )
endif()
