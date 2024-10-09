# ###########################################################################
# GNU compiler and linker options for POSIX API
# Copyright 2022-2023 Aregtech
# ###########################################################################

message(STATUS "AREG: >>> Preparing settings for GNU compiler and POSIX API, Cygwin = '${CYGWIN}'")

if (CYGWIN)
    set(AREG_COMPILER_FAMILY "cygwin")  
else()
    set(AREG_COMPILER_FAMILY "gnu")
endif()

# POSIX API
add_definitions(-DPOSIX)
set(AREG_DEVELOP_ENV "Posix")
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

if(${AREG_BITNESS} EQUAL 32)
    list(APPEND AREG_COMPILER_OPTIONS -m32)
else()
    list(APPEND AREG_COMPILER_OPTIONS -m64)
endif()

# Linker flags (-l is not necessary)
list(APPEND AREG_LDFLAGS stdc++ m pthread rt)
set(AREG_LDFLAGS_STR "-lstdc++ -lm -lpthread -lrt")

# disable SQLite warnings
list(APPEND AREG_OPT_DISABLE_WARN_THIRDPARTY
        -Wno-everything
        -Wno-unused-function
)
