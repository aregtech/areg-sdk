# ###########################################################################
#               CMake options of Areg SDK
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

cmake_minimum_required(VERSION 3.20.0)

# Areg root directory
set(AREG_SDK_ROOT           "${CMAKE_CURRENT_LIST_DIR}")
set(AREG_CMAKE_CONFIG_DIR   "${AREG_SDK_ROOT}/conf/cmake")

# Set 'AREG_SDK_ROOT' and optional 'AREG_CMAKE_CONFIG_DIR' variables
#  before 'setup.cmake', and include 'setup.cmake'
# before 'project' call.
include(${AREG_CMAKE_CONFIG_DIR}/setup.cmake)

# Areg SDK project name and version
set(AREG_PROJECT_NAME "areg-sdk")
set(AREG_PROJECT_VERSION "1.9.99.111")

# Project's properties
set(PROJECT_NAME ${AREG_PROJECT_NAME})
set(PROJECT_VERSION ${AREG_PROJECT_VERSION})
project("${PROJECT_NAME}" 
        VERSION      "${PROJECT_VERSION}" 
        DESCRIPTION  "Areg Communication Framework and Tools" 
        HOMEPAGE_URL "https://areg.tech" 
        LANGUAGES    CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)# The 'common.cmake' file should be included after 'project' call
include(${AREG_CMAKE_CONFIG_DIR}/common.cmake)

# Create output build directories at configure time so they exist before any
# compiler or archiver rule runs
file(MAKE_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
file(MAKE_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

# Dummy target used purely as a dependency anchor
add_custom_target(areg-dummy ALL)

include_directories(${AREG_FRAMEWORK})
include_directories(${AREG_THIRDPARTY})

# build Areg Framework thirdparty software
include(${AREG_THIRDPARTY}/CMakeLists.txt)
if (NOT AREG_SQLITE_FOUND)
    add_dependencies(aregsqlite3 areg-dummy)
endif()

# build Areg Framework software
include(${AREG_FRAMEWORK}/CMakeLists.txt)
add_dependencies(areg areg-dummy)

# build optional Areg project examples, if required
if(AREG_EXAMPLES)
    include(${AREG_EXAMPLES_DIR}/CMakeLists.txt)
endif()

# build optional Areg Framework unit tests, if required
if(AREG_TESTS)
    include(${AREG_TESTS_DIR}/CMakeLists.txt)
    if (NOT AREG_GTEST_FOUND)
        # Set dependency of 'areg-dummy' to make sure
        # the build directories are created before the
        # 'gtest' libraries are built
        add_dependencies(areg-unit-tests areg-dummy)
    endif()
else()
    option(AREG_SYSTEM_GTEST   "Use GTest package"         FALSE)
    option(AREG_GTEST_FOUND     "GTest package found flag"  FALSE)
endif()

if (AREG_INSTALL)
    include(${AREG_CMAKE_CONFIG_DIR}/install.cmake)
endif()

# Print the configuration status
printAregConfigStatus(
    TRUE
    "Areg"
    "----------------------> Areg project CMake Status Report Begin <-----------------------"
    "-----------------------> Areg project CMake Status Report End <------------------------"
)
