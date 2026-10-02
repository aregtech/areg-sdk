# ###########################################################################
# MSVC compiler and linker options for Win32 API
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

message(STATUS "Areg: >>> Preparing settings for MSVC compiler under \'${AREG_OS}\' platform, WIN32 = \'${WIN32}\'")

# Visual Studio C++, Windows / Win32 API
set(AREG_DEVELOP_ENV "Win32")

add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32)
if (AREG_BITNESS EQUAL 64)
    add_definitions(-DWIN64 -D_WIN64)
endif()

# Match the conformance mode of the Visual Studio projects, so that both build systems
# accept and reject the same code.
list(APPEND AREG_COMPILER_OPTIONS /permissive-)

get_property(_areg_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
# Debug is the only unoptimized configuration. MinSizeRel and RelWithDebInfo keep the code
# generation and link time settings of Release, and take their own optimization level.
if (_areg_multi_config)
    # Multi-config generator (Visual Studio): scope flags per-configuration using
    # generator expressions so an optimization level never bleeds into a Debug build.
    foreach(_areg_cfg IN ITEMS Release RelWithDebInfo MinSizeRel Debug)
        macro_optimization_option("${_areg_cfg}" _areg_cfg_opt)
        if (NOT "${_areg_cfg_opt}" STREQUAL "")
            list(APPEND AREG_COMPILER_OPTIONS $<$<CONFIG:${_areg_cfg}>:${_areg_cfg_opt}>)
        endif()
        string(TOUPPER "${_areg_cfg}" _areg_cfg_name)
        if (NOT "${_areg_cfg}" STREQUAL "Debug")
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_${_areg_cfg_name} TRUE)
            set(CMAKE_EXE_LINKER_FLAGS_${_areg_cfg_name}    "${CMAKE_EXE_LINKER_FLAGS_${_areg_cfg_name}} /OPT:REF /OPT:ICF")
            set(CMAKE_SHARED_LINKER_FLAGS_${_areg_cfg_name} "${CMAKE_SHARED_LINKER_FLAGS_${_areg_cfg_name}} /OPT:REF /OPT:ICF")
        endif()
    endforeach()
    unset(_areg_cfg)
    unset(_areg_cfg_opt)
    unset(_areg_cfg_name)
    list(APPEND AREG_COMPILER_OPTIONS
        $<$<NOT:$<CONFIG:Debug>>:/GL>
        $<$<NOT:$<CONFIG:Debug>>:/Gy>
        $<$<NOT:$<CONFIG:Debug>>:/fp:fast>
        $<$<CONFIG:Debug>:/RTC1>
        /c
    )
else()
    # Single-config generator (Ninja, NMake): CMAKE_BUILD_TYPE is reliable.
    string(TOUPPER "${CMAKE_BUILD_TYPE}" _areg_cfg_name)
    if (AREG_BUILD_OPTIMIZED)
        list(APPEND AREG_COMPILER_OPTIONS ${AREG_OPTIMIZATION} /GL /Gy /fp:fast /c)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        set(CMAKE_EXE_LINKER_FLAGS_${_areg_cfg_name}    "${CMAKE_EXE_LINKER_FLAGS_${_areg_cfg_name}} /OPT:REF /OPT:ICF")
        set(CMAKE_SHARED_LINKER_FLAGS_${_areg_cfg_name} "${CMAKE_SHARED_LINKER_FLAGS_${_areg_cfg_name}} /OPT:REF /OPT:ICF")
    else()
        list(APPEND AREG_COMPILER_OPTIONS ${AREG_OPTIMIZATION} /RTC1 /c)
    endif()
    unset(_areg_cfg_name)
endif()

# Linker flags (-l is not necessary)
list(APPEND AREG_LDFLAGS advapi32   psapi   shell32   ws2_32   Synchronization)
set(AREG_LDFLAGS_STR  "-ladvapi32 -lpsapi -lshell32 -lws2_32 -lSynchronization")
