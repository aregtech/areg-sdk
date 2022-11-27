cmake_minimum_required(VERSION 3.16.0)

# Areg root directory
set(CMAKE_ROOT_FILE "${CMAKE_SOURCE_DIR}")
set(AREG_SDK_ROOT "${CMAKE_ROOT_FILE}")
set(CMAKE_CONFIG_DIR "${AREG_SDK_ROOT}/conf/cmake")
set(AREG_BASE "${AREG_SDK_ROOT}/framework")
set(AREG_EXAMPLES "${AREG_SDK_ROOT}/examples")

include(${CMAKE_CONFIG_DIR}/user.cmake)

# Project's properties
set(PROJECT_NAME "AREG-SDK")
set(PROJECT_VERSION "1.1.0")
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

include(${CMAKE_CONFIG_DIR}/common.cmake)

option(BUILD_TESTS "Build unit tests")
option(BUILD_EXAMPLES "Build examples" ON)

include_directories(framework/)
include(${AREG_BASE}/CMakeLists.txt)

if(BUILD_EXAMPLES)
    include(${AREG_EXAMPLES}/CMakeLists.txt)
endif()
