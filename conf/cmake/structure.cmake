# ###########################################################################
# AREG Framework specific variables defining the software structure.
# The variable 'AREG_SDK_ROOT' should be already set.
# Copyright 2022-2023 Aregtech
# ###########################################################################

# Make sure that AREG_SDK_ROOT is set
if (NOT DEFINED AREG_SDK_ROOT)
    message(ERROR ">>> Set AREG_SDK_ROOT before including \'variable.cmake\' file")
endif()

# The location of cmake configuration files.
if (NOT DEFINED AREG_CMAKE_CONFIG_DIR OR "${AREG_CMAKE_CONFIG_DIR}" STREQUAL "")
    set(AREG_CMAKE_CONFIG_DIR   "${AREG_SDK_ROOT}/conf/cmake")
endif()

# The location of AREG Framework source codes.
if (NOT DEFINED AREG_BASE OR "${AREG_BASE}" STREQUAL "")
    set(AREG_BASE               "${AREG_SDK_ROOT}/framework")
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

# The location of building root directories.
# By default it is the root directory of the project, where the 'product' subfolder is created.
if (NOT DEFINED PROJECT_BUILD_ROOT OR "${PROJECT_BUILD_ROOT}" STREQUAL "")
    set(PROJECT_BUILD_ROOT         "${CMAKE_SOURCE_DIR}")
endif()
