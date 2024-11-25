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

# set CMake tool settings
set(CMAKE_BUILD_TYPE        ${AREG_BUILD_TYPE} CACHE STRING "Configuration Type" FORCE)

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

set(CMAKE_FIND_PACKAGE_RESOLVE_SYMLINKS TRUE)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG    TRUE)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib;.a" CACHE INTERNAL "Suffix")
else()
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a;.so" CACHE INTERNAL "Suffix")
endif()


if (DEFINED AREG_BITNESS)
    if (${AREG_BITNESS} EQUAL 64)
        set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)
        set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS OFF)
    else()
        set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
        set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS ON)
    endif()
endif()

if (NOT MSVC)
    if ("${AREG_PROCESSOR}" STREQUAL "AARCH64")
        set(CMAKE_CXX_COMPILER_TARGET aarch64-linux-gnu)
        set(CMAKE_FIND_ROOT_PATH "/usr/aarch64-linux-gnu;/usr/lib/aarch64-linux-gnu" CACHE INTERNAL "Find path")
    elseif("${AREG_PROCESSOR}" STREQUAL "ARM")
        set(CMAKE_CXX_COMPILER_TARGET arm-linux-gnueabihf)
        set(CMAKE_FIND_ROOT_PATH "/usr/arm-linux-gnueabihf;/usr/lib/arm-linux-gnueabihf" CACHE INTERNAL "Find path")
    elseif("${AREG_PROCESSOR}" STREQUAL "i386")
        set(CMAKE_CXX_COMPILER_TARGET i386-linux-gnu)
        set(CMAKE_FIND_ROOT_PATH "/usr/lib/i386-linux-gnu;/usr/i386-linux-gnu;/usr/lib" CACHE INTERNAL "Find path")
    elseif("${AREG_PROCESSOR}" STREQUAL "x86_64")
        set(CMAKE_CXX_COMPILER_TARGET x86_64-linux-gnu)
        set(CMAKE_FIND_ROOT_PATH "/usr/lib/x86_64-linux-gnu;/usr/x86_64-linux-gnu;/usr/lib" CACHE INTERNAL "Find path")
    else()
        message(STATUS "AREG: <<< Undefined processor!")
    endif()
endif()

message("<<< CMAKE_CXX_COMPILER_TARGET = '${CMAKE_CXX_COMPILER_TARGET}', CMAKE_FIND_LIBRARY_SUFFIXES = '${CMAKE_FIND_LIBRARY_SUFFIXES}'")
message("<<< CMAKE_FIND_ROOT_PATH = '${CMAKE_FIND_ROOT_PATH}'")

# The 'common.cmake' file should be included after 'project' call
include(${AREG_CMAKE_CONFIG_DIR}/common.cmake)

# add a custom command to create output build folders if they are not existing.
# create a dummy project to add as a dependency in all other projects.
# this ensures that the commands to create directories are called first.
add_custom_target(areg-dummy ALL COMMAND ${CMAKE_COMMAND} VERBATIM)
add_custom_command( TARGET areg-dummy PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
                    VERBATIM)

include_directories(${AREG_FRAMEWORK})
include_directories(${AREG_THIRDPARTY})

# build AREG Framework thirdparty software
include(${AREG_THIRDPARTY}/CMakeLists.txt)
if (NOT ${AREG_SQLITE_FOUND})
    add_dependencies(aregsqlite3 areg-dummy)
endif()

# build AREG Framework software
include(${AREG_FRAMEWORK}/CMakeLists.txt)
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
else()
    option(AREG_GTEST_PACKAGE "Use google test package" OFF)
endif()

if (AREG_INSTALL AND AREG_SQLITE_FOUND)
    include(${AREG_CMAKE_CONFIG_DIR}/install.cmake)
elseif(AREG_INSTALL)
    option(AREG_INSTALL "Enable install AREG SDK" OFF)
endif()

# Print the configuration status
printAregConfigStatus(
    TRUE
    "AREG"
    "----------------------> AREG project CMake Status Report Begin <-----------------------"
    "-----------------------> AREG project CMake Status Report End <------------------------"
)
