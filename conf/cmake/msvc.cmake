# ###########################################################################
# MSVC compiler and linker options for Win32 API
# Copyright 2022-2023 Aregtech
# ###########################################################################

message(STATUS "AREG: >>> Preparing compiler settings for `MSVC` and Win32 API")

# Visual Studio C++, Windows / Win32 API
set(AREG_DEVELOP_ENV "Win32")
add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32)

if(NOT CMAKE_BUILD_TYPE MATCHES Release)
    list(APPEND AREG_COMPILER_OPTIONS -Od -RTC1 -c)
endif()
        
if (${AREG_BITNESS} MATCHES "64")
    add_definitions(-DWIN64 -D_WIN64)
endif()

# MS Visual C++ compile options
list(APPEND AREG_COMPILER_OPTIONS)
# Linker flags (-l is not necessary)
list(APPEND AREG_LDFLAGS advapi32 psapi shell32 ws2_32)
