cmake_minimum_required(VERSION 3.16.0)

# Project's properties
set(PROJECT_NAME "AREG-SDK")
set(PROJECT_VERSION "1.1.0")
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

# Areg root directory
set(CMAKE_ROOT_FILE "${CMAKE_SOURCE_DIR}")
set(AREG_SDK_ROOT "${CMAKE_ROOT_FILE}")
set(CMAKE_CONFIG_DIR "${AREG_SDK_ROOT}/conf/cmake")
set(AREG_BASE "${AREG_SDK_ROOT}/framework")
set(AREG_EXAMPLES "${AREG_SDK_ROOT}/examples")


include("${CMAKE_CONFIG_DIR}/common.cmake")


include("${AREG_BASE}/CMakeLists.txt")
include("${AREG_EXAMPLES}/CMakeLists.txt")


