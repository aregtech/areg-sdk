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

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_EXAMPLES "Build examples" ON)

include_directories(framework/)
include(${AREG_BASE}/CMakeLists.txt)

if(BUILD_EXAMPLES)
    include(${AREG_EXAMPLES}/CMakeLists.txt)
endif()

if(BUILD_TESTS)
    set(GOOGLE_TEST_BASE "${AREG_SDK_ROOT}/thirdparty/googletest")
    if(EXISTS "${GOOGLE_TEST_BASE}/CMakeLists.txt")
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        include("${GOOGLE_TEST_BASE}/CMakeLists.txt")
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(TESTS_BASE "${AREG_SDK_ROOT}/tests")
        set(UNIT_TEST_BASE "${TESTS_BASE}/units")
        include(${TESTS_BASE}/CMakeLists.txt)
    else()
        message(SEND_ERROR "Please clone the submodules using command\n
        'git clone --recurse-submodules https://github.com/aregtech/areg-sdk.git'")
    endif()
endif()

# copying log and router init files to 'bin/config'
file(MAKE_DIRECTORY ${AREG_OUTPUT_BIN}/config)
file(COPY ${AREG_BASE}/areg/resources/log.init DESTINATION ${AREG_OUTPUT_BIN}/config)
file(COPY ${AREG_BASE}/areg/resources/router.init DESTINATION ${AREG_OUTPUT_BIN}/config)
