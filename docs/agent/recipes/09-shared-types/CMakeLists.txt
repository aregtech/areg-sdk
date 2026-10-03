cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME    "shared_types")
set(PROJECT_VERSION "1.0.0")
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

find_package(areg CONFIG)

# build/bin whichever way areg was found; an installed package names product/ otherwise.
set(AREG_BUILD_DIR "${CMAKE_BINARY_DIR}")
option(AREG_OUTPUT_LAYOUT "Areg build structure" OFF)

if (NOT areg_FOUND)
    set(AREG_DEPS_DIR  "${CMAKE_BINARY_DIR}/packages")
    set(AREG_LIB_TYPE  shared)
    option(AREG_TESTS         "Build areg-sdk tests"    OFF)
    option(AREG_EXAMPLES      "Build areg-sdk examples" OFF)
    option(AREG_SYSTEM_GTEST  "Build GTest"             OFF)

    include(FetchContent)
    set(FETCHCONTENT_BASE_DIR "${AREG_DEPS_DIR}")
    FetchContent_Declare(areg
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master")
    FetchContent_MakeAvailable(areg)

    set(AREG_SDK_ROOT         "${areg_SOURCE_DIR}")
    set(AREG_CMAKE_CONFIG_DIR "${AREG_SDK_ROOT}/conf/cmake")
    set(AREG_CMAKE            "${AREG_SDK_ROOT}/areg.cmake")
endif()

include(${AREG_CMAKE})

add_subdirectory(src)
