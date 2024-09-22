# ###########################################################################
#               CMake opions of AREG SDK
# Copyright 2022-2023 Aregtech
# ###########################################################################

cmake_minimum_required(VERSION 3.20.0)

# Areg root directory
set(AREG_SDK_ROOT           "${CMAKE_CURRENT_LIST_DIR}")
set(AREG_CMAKE_CONFIG_DIR   "${AREG_SDK_ROOT}/conf/cmake")

# Set 'AREG_SDK_ROOT' and optional 'AREG_SDK_ROOT' variables
#  before 'setup.cmake', and include 'setup.cmake'
# before 'project' call.
include(${AREG_CMAKE_CONFIG_DIR}/setup.cmake)

# Project's properties
set(PROJECT_NAME "areg-sdk")
set(PROJECT_VERSION "1.5.0")
project("${PROJECT_NAME}" 
        VERSION "${PROJECT_VERSION}" 
        DESCRIPTION "AREG Communication Framework and Tools" 
        HOMEPAGE_URL "https://aregtech.com" 
        LANGUAGES CXX C)

# The 'common.cmake' file should be included after 'project' call
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
# build AREG Framework thirdparty software
include(${AREG_THIRDPARTY}/CMakeLists.txt)
# build AREG Framework software
include(${AREG_BASE}/CMakeLists.txt)

# build optional AREG project examples, if required
if(AREG_BUILD_EXAMPLES)
    include(${AREG_EXAMPLES}/CMakeLists.txt)
endif()

# build optional AREG Framework unit tests, if required
if(AREG_BUILD_TESTS)
    include(${AREG_TESTS}/CMakeLists.txt)
endif()
