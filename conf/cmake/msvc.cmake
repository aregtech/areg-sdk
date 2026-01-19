# ###########################################################################
# MSVC compiler and linker options for Win32 API
# Copyright 2022-2026 Aregtech
# ###########################################################################

message(STATUS "AREG: >>> Preparing settings for MSVC compiler under \'${AREG_OS}\' platform, WIN32 = \'${WIN32}\'")

# Visual Studio C++, Windows / Win32 API
set(AREG_DEVELOP_ENV "Win32")

add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32)
if (${AREG_BITNESS} EQUAL 64)
    add_definitions(-DWIN64 -D_WIN64)
endif()

if(NOT CMAKE_BUILD_TYPE MATCHES Release)
    list(APPEND AREG_COMPILER_OPTIONS -Od -RTC1 -c)
endif()

# Linker flags (-l is not necessary)
list(APPEND AREG_LDFLAGS advapi32   psapi   shell32   ws2_32)
set(AREG_LDFLAGS_STR  "-ladvapi32 -lpsapi -lshell32 -lws2_32")
