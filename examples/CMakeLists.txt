# ###########################################################################
# AREG SDK Examples with example specific functions
# Copyright 2022-2023 Aregtech
# ###########################################################################

# ###########################################################################
# CMake functions for examples
# ###########################################################################

# ---------------------------------------------------------------------------
# Description : Collects sources, dependent libraries and declares a static
#               library to use in examples.
# Parameters .: ${lib_name} -- Name of the static library to create.
#               ${ARGN}     -- The list of libraries or source code files.
# Macro ......: macro_example_declare_lib
# Usage ......: macro_example_declare_lib( <library name> <optional list of depdencies> <list of source codes with full or relative path>)
# Example ....: macro_example_declare_lib(13_common 13_generated common/src/HelloWorldClient.cpp common/src/HelloWorldService.cpp)
# See ........: as a use, see examples #13 and 23
# ---------------------------------------------------------------------------
macro(macro_example_declare_lib lib_name)
    set(_sources)
    set(_libs)
    set(_list "${ARGN}")
    foreach(item IN LISTS _list)
        if (TARGET ${item})
            list(APPEND _libs ${item})
        elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${item}")
            list(APPEND _sources "${CMAKE_CURRENT_LIST_DIR}/${item}")
        elseif(EXISTS "${item}")
            list(APPEND _sources "${item}")
        else()
            message(WARNING "AREG Examples: >>> The item \'${item}\' of target \'${example_name}\' has unknown type, ignored.")
        endif()
    endforeach()
    addStaticLibEx(${lib_name} "" "${_sources}" "${_libs}")
    unset(_list)
    unset(_sources)
    unset(_libs)
endmacro(macro_example_declare_lib)

# ---------------------------------------------------------------------------
# Description : Collects sources, dependent libraries and declares an executable.
# Parameters .: ${example_name} -- Name of the executable to create.
#               ${ARGN}         -- The list of libraries or source code files.
# Macro ......: macro_example_declare_runnable
# Usage ......: macro_example_declare_runnable( <executable name> <optional list of dependencies> <list of source codes with full or relative path>)
# Example ....: macro_example_declare_runnable(01_hello src/main.cpp)
# ---------------------------------------------------------------------------
macro(macro_example_declare_runnable example_name)
    set(_sources)
    set(_libs)
    set(_list "${ARGN}")
    foreach(item IN LISTS _list)
        if (TARGET ${item})
            list(APPEND _libs ${item})
        elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${item}")
            list(APPEND _sources "${CMAKE_CURRENT_LIST_DIR}/${item}")
        elseif(EXISTS "${item}")
            list(APPEND _sources "${item}")
        else()
            message(WARNING "AREG Examples: >>> The item \'${item}\' of target \'${example_name}\' has unknown type, ignored.")
        endif()
    endforeach()
    addExecutableEx(${example_name} "" "${_sources}" "${_libs}")
    unset(_list)
    unset(_sources)
    unset(_libs)
endmacro(macro_example_declare_runnable)

# ###########################################################################
# List of examples to build
# ###########################################################################

# Examples with specific design must be included one by one.
include(${AREG_EXAMPLES}/00_helloservice/CMakeLists.txt)
include(${AREG_EXAMPLES}/01_hello/CMakeLists.txt)
include(${AREG_EXAMPLES}/02_buffer/CMakeLists.txt)
include(${AREG_EXAMPLES}/03_file/CMakeLists.txt)
include(${AREG_EXAMPLES}/04_trace/CMakeLists.txt)
include(${AREG_EXAMPLES}/05_timer/CMakeLists.txt)
include(${AREG_EXAMPLES}/06_threads/CMakeLists.txt)
include(${AREG_EXAMPLES}/07_synch/CMakeLists.txt)
include(${AREG_EXAMPLES}/08_service/CMakeLists.txt)
include(${AREG_EXAMPLES}/09_svcmulti/CMakeLists.txt)
include(${AREG_EXAMPLES}/10_locsvc/CMakeLists.txt)
include(${AREG_EXAMPLES}/11_locmesh/CMakeLists.txt)
include(${AREG_EXAMPLES}/12_pubsvc/CMakeLists.txt)
include(${AREG_EXAMPLES}/13_pubmesh/CMakeLists.txt)
include(${AREG_EXAMPLES}/14_pubtraffic/CMakeLists.txt)
include(${AREG_EXAMPLES}/15_pubworker/CMakeLists.txt)
include(${AREG_EXAMPLES}/16_pubfsm/CMakeLists.txt)
include(${AREG_EXAMPLES}/18_locwatchdog/CMakeLists.txt)
include(${AREG_EXAMPLES}/19_pubwatchdog/CMakeLists.txt)
include(${AREG_EXAMPLES}/20_pubdatarate/CMakeLists.txt)
include(${AREG_EXAMPLES}/21_pubunblock/CMakeLists.txt)
include(${AREG_EXAMPLES}/22_pubsub/CMakeLists.txt)
include(${AREG_EXAMPLES}/23_pubsubmix/CMakeLists.txt)
include(${AREG_EXAMPLES}/24_pubsubmulti/CMakeLists.txt)

if (AREG_DEVELOP_ENV MATCHES "Win32")
    # This projects are based either on MFC (Microsoft Foundation Classes)
    # or are compiled only under Windows. So that, they are available
    # only when compile with Microsoft Visual Studio.

    find_package(MFC)
    if (MFC_FOUND)
        # MFC specific projects. Can be compiled only if target machine has installed MFC.
        # Otherwise, output warning message and exclude from the build.
        include(${AREG_EXAMPLES}/17_winchat/CMakeLists.txt)
    else(MFC_FOUND)
        message(NOTICE " AREG Examples: >>> Due to missing installed MFC library, escaping compile projects:")
        message(NOTICE " AREG Examples: >>> ${AREG_EXAMPLES}/17_winchat")
    endif(MFC_FOUND)

endif()
