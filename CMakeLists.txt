# ###########################################################################
#               CMake opions of AREG SDK
# Copyright 2022-2023 Aregtech
# ###########################################################################

cmake_minimum_required(VERSION 3.20.0)

# Areg root directory
set(AREG_SDK_ROOT           "${CMAKE_CURRENT_LIST_DIR}")
set(AREG_CMAKE_CONFIG_DIR   "${AREG_SDK_ROOT}/conf/cmake")

# set(AREG_BASE               "${AREG_SDK_ROOT}/framework")
# set(AREG_EXAMPLES           "${AREG_SDK_ROOT}/examples")
# set(AREG_TESTS              "${AREG_SDK_ROOT}/tests")
# set(AREG_THIRDPARTY         "${AREG_SDK_ROOT}/thirdparty")
# set(AREG_SDK_TOOLS			"${AREG_SDK_ROOT}/tools")

include(${AREG_CMAKE_CONFIG_DIR}/structure.cmake)
include(${AREG_CMAKE_CONFIG_DIR}/functions.cmake)
include(${AREG_CMAKE_CONFIG_DIR}/user.cmake)

# If compiler family name is not empty.
# Otherwise, use system default compiler.
# See in 'common.cmake'
if (NOT "${AREG_COMPILER_FAMILY}" STREQUAL "")
    set(CMAKE_CXX_COMPILER  "${AREG_CXX_COMPILER}")
    set(CMAKE_C_COMPILER    "${AREG_C_COMPILER}")
    message(STATUS ">>> Set CMAKE_CXX_COMPILER compiler \'${CMAKE_CXX_COMPILER}\'")
else()
    message(STATUS ">>> Use system default CMAKE_CXX_COMPILER compiler \'${CMAKE_CXX_COMPILER}\'")
endif()

set(CMAKE_BUILD_TYPE    ${AREG_BUILD_TYPE})
set(CMAKE_BUILD_TYPE    ${AREG_BUILD_TYPE} CACHE STRING "Configuration Type" FORCE)
set(CXX_STANDARD        ${AREG_CXX_STANDARD})

# Project's properties
set(PROJECT_NAME "areg-sdk")
set(PROJECT_VERSION "1.5.0")
project("${PROJECT_NAME}" 
        VERSION "${PROJECT_VERSION}" 
        DESCRIPTION "AREG Communication Framework and Tools" 
        HOMEPAGE_URL "https://aregtech.com" 
        LANGUAGES CXX C)

include(${AREG_CMAKE_CONFIG_DIR}/common.cmake)

# add a custom command to create output build folders if they are not existing.
# create a dummy project to add as a dependency in all other projects.
# this ensures that the commands to create directories are called first.
add_custom_target(areg-dummy ALL COMMAND ${CMAKE_COMMAND} VERBATIM)
add_custom_command( TARGET areg-dummy PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${AREG_OUTPUT_DIR}"
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${AREG_OUTPUT_BIN}"
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${AREG_OUTPUT_LIB}"
                    VERBATIM)

include_directories(${AREG_BASE})
include_directories(${AREG_THIRDPARTY})
include(${AREG_THIRDPARTY}/CMakeLists.txt)
include(${AREG_BASE}/CMakeLists.txt)

if(AREG_BUILD_EXAMPLES)
    include(${AREG_EXAMPLES}/CMakeLists.txt)
endif()

if(AREG_BUILD_TESTS)
    include(${AREG_TESTS}/CMakeLists.txt)
endif()
