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

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# copying log and router init files to 'bin/config'
file(MAKE_DIRECTORY ${AREG_OUTPUT_BIN}/config)
file(COPY ${AREG_BASE}/areg/resources/log.init DESTINATION ${AREG_OUTPUT_BIN}/config)
file(COPY ${AREG_BASE}/areg/resources/router.init DESTINATION ${AREG_OUTPUT_BIN}/config)
