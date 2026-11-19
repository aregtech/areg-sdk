cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME    "observability")
set(PROJECT_VERSION "1.0.0")
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

find_package(areg CONFIG)

if (NOT areg_FOUND)
    include(FetchContent)
    set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/packages")
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
