# ###########################################################################
# MSVC compiler and linker options for Win32 API
# Copyright 2022-2026 Aregtech
# ###########################################################################

message(STATUS "Areg: >>> Preparing settings for MSVC compiler under \'${AREG_OS}\' platform, WIN32 = \'${WIN32}\'")

# Visual Studio C++, Windows / Win32 API
set(AREG_DEVELOP_ENV "Win32")

add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32)
if (${AREG_BITNESS} EQUAL 64)
    add_definitions(-DWIN64 -D_WIN64)
endif()

get_property(_areg_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (_areg_multi_config)
    # Multi-config generator (Visual Studio): scope flags per-configuration using
    # generator expressions so Release-only /O2 never bleeds into Debug builds.
    list(APPEND AREG_COMPILER_OPTIONS
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/GL>
        $<$<CONFIG:Release>:/Gy>
        $<$<CONFIG:Release>:/fp:fast>
        $<$<NOT:$<CONFIG:Release>>:/Od>
        $<$<NOT:$<CONFIG:Release>>:/RTC1>
        /c
    )
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
else()
    # Single-config generator (Ninja, NMake): CMAKE_BUILD_TYPE is reliable.
    if(${CMAKE_BUILD_TYPE} MATCHES "Release")
        list(APPEND AREG_COMPILER_OPTIONS /O2 /GL /Gy /fp:fast /c)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
    else()
        list(APPEND AREG_COMPILER_OPTIONS /Od /RTC1 /c)
    endif()
endif()

# Linker flags (-l is not necessary)
list(APPEND AREG_LDFLAGS advapi32   psapi   shell32   ws2_32)
set(AREG_LDFLAGS_STR  "-ladvapi32 -lpsapi -lshell32 -lws2_32")
