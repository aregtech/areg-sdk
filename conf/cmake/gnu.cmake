# ###########################################################################
# GNU compiler and linker options for POSIX API
# Copyright 2022-2023 Aregtech
# ###########################################################################

message(STATUS "AREG: >>> Preparing settings for GNU compiler under \'${AREG_OS}\' platform, Cygwin = \'${CYGWIN}\', MinGW = \'${MINGW}\'")

if (MINGW)
    set(AREG_COMPILER_FAMILY "mingw")
elseif (CYGWIN)
    set(AREG_COMPILER_FAMILY "cygwin")  
else()
    set(AREG_COMPILER_FAMILY "gnu")
endif()

if (NOT MINGW)
    # POSIX API
    add_definitions(-DPOSIX)
else()
    add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32 -D_UCRT -D__USE_MINGW_ACCESS -D_NO_CRT_STDIO_INLINE -D__USE_MINGW_ANSI_STDIO=1)
    if (${AREG_BITNESS} EQUAL 64)
        add_definitions(-DWIN64 -D_WIN64)
    endif()
endif()

set(AREG_DEVELOP_ENV "Posix")
# GNU compile options
if (MINGW)
    set(AREG_DEVELOP_ENV "Win32")
    list(APPEND AREG_COMPILER_OPTIONS -Wall -c -fmessage-length=0 -municode -mwin32 -MMD ${AREG_USER_DEFINES})
    set(AREG_COMPILER_VERSION  -std=c++17)
elseif (CYGWIN)
    list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -MMD ${AREG_USER_DEFINES})
    set(AREG_COMPILER_VERSION  -std=gnu++17)
else()
    list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -MMD ${AREG_USER_DEFINES})
    set(AREG_COMPILER_VERSION -std=c++17)
endif()

if(CMAKE_BUILD_TYPE MATCHES Release)
    list(APPEND AREG_COMPILER_OPTIONS -O2)
else()
    list(APPEND AREG_COMPILER_OPTIONS -O0 -g3)
endif()

if (${AREG_PROCESSOR} STREQUAL ${_proc_x86} OR ${AREG_PROCESSOR} STREQUAL ${_proc_x64})
    if(${AREG_BITNESS} EQUAL 32)
        list(APPEND AREG_COMPILER_OPTIONS -m32)
        list(APPEND AREG_LDFLAGS -m32)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -m64)
        list(APPEND AREG_LDFLAGS -m64)
    endif()
endif()

# Linker flags (-l is not necessary)
if (MINGW)
    if (AREG_BUILD_TYPE MATCHES "Debug")
        list(APPEND AREG_LDFLAGS  stdc++   ucrt   advapi32   psapi   shell32   ws2_32   dbghelp)
        set(AREG_LDFLAGS_STR   "-lstdc++ -lucrt -ladvapi32 -lpsapi -lshell32 -lws2_32 -ldbghelp")
    else()
        list(APPEND AREG_LDFLAGS  stdc++   ucrt   advapi32   psapi   shell32   ws2_32)
        set(AREG_LDFLAGS_STR   "-lstdc++ -lucrt -ladvapi32 -lpsapi -lshell32 -lws2_32")
    endif()
else()
    list(APPEND AREG_LDFLAGS stdc++ m pthread rt)
    set(AREG_LDFLAGS_STR "-lstdc++ -lpthread -lrt")
endif()

# disable SQLite warnings
list(APPEND AREG_OPT_DISABLE_WARN_THIRDPARTY
        -Wno-everything
        -Wno-unused-function
)

list(APPEND AREG_OPT_DISABLE_WARN_COMMON
        -Wno-psabi
        -Wno-unused-value
        -Wno-unused-but-set-variable
)
