# ###########################################################################
#               CMake options of AREG SDK
# Copyright 2022-2023 Aregtech
# ###########################################################################

cmake_minimum_required(VERSION 3.20.0)

# Areg root directory
set(AREG_SDK_ROOT           "${CMAKE_CURRENT_LIST_DIR}")
set(AREG_CMAKE_CONFIG_DIR   "${AREG_SDK_ROOT}/conf/cmake")

# Set 'AREG_SDK_ROOT' and optional 'AREG_CMAKE_CONFIG_DIR' variables
#  before 'setup.cmake', and include 'setup.cmake'
# before 'project' call.
include(${AREG_CMAKE_CONFIG_DIR}/setup.cmake)

# AREG SDK project name and version
set(AREG_PROJECT_NAME "areg-sdk")
set(AREG_PROJECT_VERSION "1.9.99.111")

# Project's properties
set(PROJECT_NAME ${AREG_PROJECT_NAME})
set(PROJECT_VERSION ${AREG_PROJECT_VERSION})
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
if (NOT AREG_SQLITE_FOUND)
    add_dependencies(sqlite3 areg-dummy)
endif()

# build AREG Framework software
include(${AREG_BASE}/CMakeLists.txt)
add_dependencies(areg areg-dummy)

# build optional AREG project examples, if required
if(AREG_BUILD_EXAMPLES)
    include(${AREG_EXAMPLES}/CMakeLists.txt)
endif()

# build optional AREG Framework unit tests, if required
if(AREG_BUILD_TESTS)
    include(${AREG_TESTS}/CMakeLists.txt)
    if (NOT AREG_GTEST_FOUND)
        add_dependencies(areg-unit-tests areg-dummy)
    endif()
endif()



if (AREG_INSTALL)
    include(${AREG_CMAKE_CONFIG_DIR}/install.cmake)
endif(AREG_INSTALL)

# Print the configuration status
message(STATUS "=======================================================================================")
message(STATUS "----------------------> AREG project CMake Status Report Begin <-----------------------")
message(STATUS "=======================================================================================")
message(STATUS "AREG: >>> CMAKE_SOURCE_DIR = \'${CMAKE_SOURCE_DIR}\', build type \'${CMAKE_BUILD_TYPE}\'")
message(STATUS "AREG: >>> Build ...........: \'${CMAKE_SYSTEM_NAME}\' system, \'${AREG_BITNESS}\'-bit platform, \'${AREG_PROCESSOR}\' CPU")
message(STATUS "AREG: >>> Compiler ........: \'${CMAKE_CXX_COMPILER}\', ID \'${AREG_COMPILER_FAMILY}\'")
message(STATUS "AREG: >>> Binary output ...: \'${AREG_OUTPUT_BIN}\', extension '${CMAKE_EXECUTABLE_SUFFIX}'")
message(STATUS "AREG: >>> Generated files .: \'${AREG_GENERATE_DIR}\' directory")
message(STATUS "AREG: >>> Packages ........: \'${FETCHCONTENT_BASE_DIR}\' directory")
message(STATUS "AREG: >>> Java version ....: \'${Java_VERSION_STRING}\' of version \'${Java_JAVA_EXECUTABLE}\'. Minimum should be 17")
message(STATUS "AREG: >>> Use of packages .: SQLite3 package use is \'${AREG_SQLITE_PACKAGE}\', GTest package use is \'${AREG_GTEST_PACKAGE}\' ")
message(STATUS "AREG: >>> Other options ...: Examples = \'${AREG_BUILD_EXAMPLES}\', Unit Tests = \'${AREG_BUILD_TESTS}\', AREG Extended = \'${AREG_EXTENDED}\', Logs = \'${AREG_LOGS}\'")
message(STATUS "AREG: >>> Installation ....: is '${AREG_INSTALL}', location \'${CMAKE_INSTALL_PREFIX}\'")
message(STATUS "=======================================================================================")
message(STATUS "-----------------------> AREG project CMake Status Report End <------------------------")
message(STATUS "=======================================================================================")
