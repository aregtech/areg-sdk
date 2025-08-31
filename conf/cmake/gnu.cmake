# ###########################################################################
# GNU compiler and linker options for POSIX API
# Copyright 2022-2023 Aregtech
# ###########################################################################

message(STATUS "AREG: >>> Preparing settings for GNU compiler under \'${AREG_OS}\' platform, Cygwin = \'${CYGWIN}\', MINGW = \'${MINGW}\'")

if (CYGWIN)
    set(AREG_COMPILER_FAMILY "cygwin")
elseif (MINGW)
    set(AREG_COMPILER_FAMILY "mingw")
else()
    set(AREG_COMPILER_FAMILY "gnu")
endif()

if (MINGW)

    set(AREG_DEVELOP_ENV "Win32")
    add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32)
    set(AREG_COMPILER_VERSION -std=c++17)
    list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -municode -mwin32 -MMD ${AREG_USER_DEFINES})

    if (${AREG_PROCESSOR} STREQUAL ${_proc_x86} OR ${AREG_PROCESSOR} STREQUAL ${_proc_x64})
        if(${AREG_BITNESS} EQUAL 32)
            list(APPEND AREG_COMPILER_OPTIONS -m32)
            list(APPEND AREG_LDFLAGS -m32)
        else()
            list(APPEND AREG_COMPILER_OPTIONS -m64)
            list(APPEND AREG_LDFLAGS -m64)
        endif()
    endif()

    if (AREG_BUILD_TYPE MATCHES "Debug")
        list(APPEND AREG_COMPILER_OPTIONS -O2)
        list(APPEND AREG_LDFLAGS   stdc++   m   pthread   advapi32   psapi   shell32   ws2_32   dbghelp)
        set(AREG_LDFLAGS_STR    "-lstdc++ -lm -lpthread -ladvapi32 -lpsapi -lshell32 -lws2_32 -ldbghelp")
    else()
        list(APPEND AREG_COMPILER_OPTIONS -O0 -g3)
        list(APPEND AREG_LDFLAGS   stdc++   m   pthread   advapi32   psapi   shell32   ws2_32)
        set(AREG_LDFLAGS_STR    "-lstdc++ -lm -lpthread -ladvapi32 -lpsapi -lshell32 -lws2_32")
    endif()
else()

    # POSIX API
    set(AREG_DEVELOP_ENV "Posix")
    add_definitions(-DPOSIX)
    # GNU compile options
    if (CYGWIN)
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
        if("${AREG_BITNESS}" STREQUAL "32")
            list(APPEND AREG_COMPILER_OPTIONS -m32)
            list(APPEND AREG_LDFLAGS -m32)
        else()
            list(APPEND AREG_COMPILER_OPTIONS -m64)
            list(APPEND AREG_LDFLAGS -m64)
        endif()
    endif()

    # Linker flags (-l is not necessary)
    list(APPEND AREG_LDFLAGS stdc++ m pthread rt)
    set(AREG_LDFLAGS_STR "-lstdc++ -lm -lpthread -lrt")
endif()

# disable SQLite warnings
list(APPEND AREG_OPT_DISABLE_WARN_THIRDPARTY
        -Wno-everything
        -Wno-unused-function
        -Wno-unused-but-set-variable
)

list(APPEND AREG_OPT_DISABLE_WARN_COMMON
        -Wno-psabi
)
