# ###########################################################################
# AREG Framework based project setups.
# - It sets AREG Framework specific variables defining the software structure.
# - Sets up user specific setting like compiler type and build type.
# - Sets up AREG Framework specific functions that can be used in any project.
#
# The variable 'AREG_SDK_ROOT' should be already set.
# 
# Copyright (c) 2022-2023 Aregtech
# ###########################################################################

if (NOT DEFINED AREG_SDK_ROOT OR "${AREG_SDK_ROOT}" STREQUAL "")
    # Make sure that AREG_SDK_ROOT is set before the 'setup.cmake' is included
    message(FATAL_ERROR "AREG: >>> Set \'AREG_SDK_ROOT\' before including \'setup.cmake\'. Stopping building the project.")
    return()
endif()

# The location of cmake configuration files.
if (NOT DEFINED AREG_CMAKE_CONFIG_DIR OR "${AREG_CMAKE_CONFIG_DIR}" STREQUAL "")
    if (EXISTS "${AREG_SDK_ROOT}/conf/cmake/")
        set(AREG_CMAKE_CONFIG_DIR   "${AREG_SDK_ROOT}/conf/cmake")
    else()
        message(FATAL_ERROR "AREG: >>> Cannot find \'conf\cmake\' sub-directory in the AREG SDK Root \'${AREG_SDK_ROOT}\'. Set \'AREG_CMAKE_CONFIG_DIR\' manually.")
    endif()
endif()

# The location of AREG Framework header files.
if (NOT DEFINED AREG_FRAMEWORK OR "${AREG_FRAMEWORK}" STREQUAL "")
    if (EXISTS "${AREG_SDK_ROOT}/framework/areg/")
        set(AREG_FRAMEWORK               "${AREG_SDK_ROOT}/framework")
    elseif (EXISTS "${AREG_SDK_ROOT}/include/areg/")
        set(AREG_FRAMEWORK               "${AREG_SDK_ROOT}/include")
    else()
        message(FATAL_ERROR "AREG: >>> Cannot find AREG Framework headers in the AREG SDK Root \'${AREG_SDK_ROOT}\'. Set \'AREG_FRAMEWORK\' manually.")
    endif()
endif()

# The location of AREG Framework examples
if (NOT DEFINED AREG_EXAMPLES OR "${AREG_EXAMPLES}" STREQUAL "")
    set(AREG_EXAMPLES           "${AREG_SDK_ROOT}/examples")
endif()

# The location of AREG Framework unit tests
if (NOT DEFINED AREG_TESTS OR "${AREG_TESTS}" STREQUAL "")
    set(AREG_TESTS              "${AREG_SDK_ROOT}/tests")
endif()

# The location of AREG Framework specific third-party projects / source codes.
if (NOT DEFINED AREG_THIRDPARTY OR "${AREG_THIRDPARTY}" STREQUAL "")
    set(AREG_THIRDPARTY         "${AREG_SDK_ROOT}/thirdparty")
endif()

# The location of AREG SDK tools
if (NOT DEFINED AREG_SDK_TOOLS OR "${AREG_SDK_TOOLS}" STREQUAL "")
    set(AREG_SDK_TOOLS			"${AREG_SDK_ROOT}/tools")
endif()

# setup functions
include(${AREG_CMAKE_CONFIG_DIR}/functions.cmake)
# setup user configurations
include(${AREG_CMAKE_CONFIG_DIR}/user.cmake)

# Check whether the CMake CXX-compiler is set
if (NOT "${AREG_CXX_COMPILER}" STREQUAL "")
    if ("${CMAKE_CXX_COMPILER}" STREQUAL "")
        set(CMAKE_CXX_COMPILER  "${AREG_CXX_COMPILER}")
    else()
        # Force to set existing compiler
        set(AREG_CXX_COMPILER   "${CMAKE_CXX_COMPILER}")
    endif()
endif()

# Check whether the CMake C-compiler is set
if (NOT "${AREG_C_COMPILER}" STREQUAL "")
    if ("${CMAKE_C_COMPILER}" STREQUAL "")
        set(CMAKE_C_COMPILER    "${AREG_C_COMPILER}")
    else()
        # Force to set existing compiler
        set(AREG_C_COMPILER     "${CMAKE_C_COMPILER}")
    endif()
endif()

if (WIN32)

    if ("${AREG_COMPILER_FAMILY}" STREQUAL "llvm")
        set(CMAKE_GENERATOR_TOOLSET ClangCL CACHE INTERNAL "Force ClangCL tool-set")
    endif()

    if (AREG_BITNESS EQUAL 32)
        set(CMAKE_GENERATOR_PLATFORM Win32 CACHE INTERNAL "Force 32-bit compilation")
    elseif (AREG_BITNESS EQUAL 64)
        set(CMAKE_GENERATOR_PLATFORM x64 CACHE INTERNAL "Force 64-bit compilation")
    endif()

endif()

# check and fix CXX standard for AREG Framework sources.
macro_check_fix_areg_cxx_standard()

if (NOT "${AREG_PACKAGES}" STREQUAL "")
    set(FETCHCONTENT_BASE_DIR   "${AREG_PACKAGES}" CACHE PATH "Location of AREG thirdparty packages")
else()
    set(AREG_PACKAGES "${FETCHCONTENT_BASE_DIR}")
endif()

if (NOT AREG_BUILD_TESTS)
    set(AREG_GTEST_PACKAGE OFF)
endif()
