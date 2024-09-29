# ###########################################################################
# AREG Framework based project setups.
# - It sets AREG Framework specific variables defining the software structure.
# - Sets up user specific setting like compiler type and build type.
# - Sets up AREG Framework specific functions that can be used in any project.
#
# The variable 'AREG_SDK_ROOT' should be already set.
# 
# Copyright © 2022-2023 Aregtech
# ###########################################################################

option(SET_AREG_COMMON OFF)
if (NOT DEFINED AREG_SDK_ROOT OR "${AREG_SDK_ROOT}" STREQUAL "")
    if (DEFINED areg-sdk_SOURCE_DIR AND NOT "${areg-sdk_SOURCE_DIR}" STREQUAL "")
        # AREG SDK source was fetched by an external application and 'areg-sdk_SOURCE_DIR' is the root of 'areg-sdk'
        # Set AREG_SDK_ROOT if it was not set before
        set(AREG_SDK_ROOT       "${areg-sdk_SOURCE_DIR}")
        option(SET_AREG_COMMON ON)
    elseif (DEFINED areg-sdk_DIR AND NOT "${areg-sdk_DIR}" STREQUAL "")
        # AREG SDK is included as an external package and 'areg-sdk_DIR' is the root of 'areg-sdk'
        # Set AREG_SDK_ROOT if it was not set before
        set(AREG_SDK_ROOT       "${areg-sdk_DIR}")
        option(SET_AREG_COMMON TRUE)
    else()
        # Make sure that AREG_SDK_ROOT is set before the 'setup.cmake' is included
        message(FATAL_ERROR "AREG: >>> Set AREG_SDK_ROOT before including \'setup.cmake\'. Stopping building the project.")
    endif()
endif()

# The location of cmake configuration files.
if (NOT DEFINED AREG_CMAKE_CONFIG_DIR OR "${AREG_CMAKE_CONFIG_DIR}" STREQUAL "")
    set(AREG_CMAKE_CONFIG_DIR   "${AREG_SDK_ROOT}/conf/cmake")
endif()

# The location of AREG Framework source codes.
if (NOT DEFINED AREG_BASE OR "${AREG_BASE}" STREQUAL "")
    set(AREG_BASE               "${AREG_SDK_ROOT}/framework")
endif()

# The location of AREG Framework examples
if (NOT DEFINED AREG_EXAMPLES OR "${AREG_EXAMPLES}" STREQUAL "")
    set(AREG_EXAMPLES           "${AREG_SDK_ROOT}/examples")
endif()

# The location of AREG Framework unit tests
if (NOT DEFINED AREG_TESTS OR "${AREG_TESTS}" STREQUAL "")
    set(AREG_TESTS              "${AREG_SDK_ROOT}/tests")
endif()

# The location of AREG Framework specific third-party projects / source codes.
if (NOT DEFINED AREG_THIRDPARTY OR "${AREG_THIRDPARTY}" STREQUAL "")
    set(AREG_THIRDPARTY         "${AREG_SDK_ROOT}/thirdparty")
endif()

# The location of AREG SDK tools
if (NOT DEFINED AREG_SDK_TOOLS OR "${AREG_SDK_TOOLS}" STREQUAL "")
    set(AREG_SDK_TOOLS			"${AREG_SDK_ROOT}/tools")
endif()

# setup functions
include(${AREG_CMAKE_CONFIG_DIR}/functions.cmake)
# setup user configurations
include(${AREG_CMAKE_CONFIG_DIR}/user.cmake)

# If compiler family name is not empty.
# Otherwise, use system default compiler.
# See in 'common.cmake'
if (NOT "${AREG_COMPILER_FAMILY}" STREQUAL "")
    set(CMAKE_CXX_COMPILER  "${AREG_CXX_COMPILER}")
    set(CMAKE_C_COMPILER    "${AREG_C_COMPILER}")
endif()

# set CMake tool settings
set(CMAKE_BUILD_TYPE        ${AREG_BUILD_TYPE})
set(CMAKE_BUILD_TYPE        ${AREG_BUILD_TYPE} CACHE STRING "Configuration Type" FORCE)
set(CXX_STANDARD            ${AREG_CXX_STANDARD})
set(FETCHCONTENT_BASE_DIR   "${AREG_PACKAGES}" CACHE PATH "Location of AREG thirdparty packages" FORCE)

message(STATUS "<<< SET_AREG_COMMON = ${SET_AREG_COMMON}, PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR} >>>")
if (SET_AREG_COMMON)
    if (DEFINED PROJECT_SOURCE_DIR AND NOT "${PROJECT_SOURCE_DIR}" STREQUAL "")
        # include automatically 'common.cmake' because 'areg-sdk' sources where either fetched or included as a package.
        include(${AREG_CMAKE_CONFIG_DIR}/common.cmake)
    endif()
endif()