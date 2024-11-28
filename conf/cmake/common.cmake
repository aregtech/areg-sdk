# ###########################################################################
# Common settings for all projects
# Copyright 2022-2023 Aregtech
# ###########################################################################

set(AREG_PACKAGE_NAME   "areg")

if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(AREG_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
endif()

# Set processor, if not identified yet.
if ("${AREG_PROCESSOR}" STREQUAL "")
    set(AREG_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
endif()

# Identify compiler short name
if ("${AREG_COMPILER_FAMILY}" STREQUAL "")

    macro_setup_compilers_data("${CMAKE_CXX_COMPILER}" AREG_COMPILER_FAMILY AREG_COMPILER_SHORT AREG_CXX_COMPILER AREG_C_COMPILER AREG_PROCESSOR AREG_BITNESS _compiler_found)
    if (_compiler_found)
        message(STATUS "AREG: >>> Use system default settings:")
        message(STATUS "AREG: ... Compiler family = \'${AREG_COMPILER_FAMILY}\'")
        message(STATUS "AREG: ... CXX compiler = \'${AREG_CXX_COMPILER}\'")
        message(STATUS "AREG: ... CC  compiler = \'${AREG_C_COMPILER}\'")
    else()
        message(WARNING "AREG: >>> The compiler \'${${CMAKE_CXX_COMPILER}}\' is unknown, the compilation result is unpredictable")
    endif()

elseif("${AREG_COMPILER_SHORT}" STREQUAL "")

    message(FATAL_ERROR "AREG: >>> The file \'${AREG_CMAKE_CONFIG_DIR}/setup.cmake\' should be included before \'common.cmake\', fix it and retry again.")

endif()

# Set identified OS
set(AREG_OS ${CMAKE_SYSTEM_NAME})
# Set bitness, if not identified yet.
if ("${AREG_BITNESS}" STREQUAL "" OR AREG_BITNESS EQUAL 0)
    macro_system_bitness(AREG_BITNESS)
endif()

# -----------------------------------------------------
# areg specific internal variable settings
# -----------------------------------------------------

# The development environment -- POSIX or Win32 API
set(AREG_DEVELOP_ENV)
# The linker flags
set(AREG_LDFLAGS)
set(AREG_LDFLAGS_STR "")
# set areg extended static library dependencies
set(AREG_EXTENDED_LIBS)
set(AREG_EXTENDED_LIBS_STR "")
# The compiler options
set(AREG_COMPILER_OPTIONS)
# set areg compiler version
set(AREG_COMPILER_VERSION)
set(AREG_TARGET_COMPILER_OPTIONS)
# Set the SQLite library reference
set(AREG_SQLITE_LIB_REF)
set(AREG_SQLITE_LIB     sqlite3)
option(AREG_SQLITE_FOUND "SQLite3 package found flag" FALSE)
option(AREG_GTEST_FOUND  "GTest package found flag"   FALSE)

# Adding common definition
add_definitions(-DUNICODE -D_UNICODE)
remove_definitions(-D_MBCS -DMBCS)

# ########################################################
# Warnings to disable
# ########################################################

# Disable common warnings for all projects
set(AREG_OPT_DISABLE_WARN_COMMON)
# Disable warnings only for areg framework
set(AREG_OPT_DISABLE_WARN_FRAMEWORK)
# Disable warnings only for tools 
set(AREG_OPT_DISABLE_WARN_TOOLS)
# Disable warnings only for examples
set(AREG_OPT_DISABLE_WARN_EXAMPLES)
# Disable warnings only for generated codes
set(AREG_OPT_DISABLE_WARN_CODEGEN)
# Disable warnings only for thirdparty projects
set(AREG_OPT_DISABLE_WARN_THIRDPARTY)

# Checking Compiler for adding corresponded tweaks and flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    include(${AREG_CMAKE_CONFIG_DIR}/clang.cmake)

elseif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)

    include(${AREG_CMAKE_CONFIG_DIR}/gnu.cmake)

elseif (MSVC)

    include(${AREG_CMAKE_CONFIG_DIR}/msvc.cmake)

else()

    message(WARNING "AREG: >>> Unsupported compiler type may cause unpredicted results, use default GNU compiler settings and POSIX API")
    include(${AREG_CMAKE_CONFIG_DIR}/gnu.cmake)

endif()

if (AREG_EXTENDED)
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        macro_find_ncurses_package(${AREG_PROCESSOR} ${AREG_BITNESS} _ncurses_includes _ncurses_lib _ncurses_found )
        if (_ncurses_found)
            add_definitions(-DAREG_EXTENDED=1)
            list(APPEND AREG_EXTENDED_LIBS ncurses)
            set(AREG_EXTENDED_LIBS_STR "-lncurses")
        else()
            message(STATUS "AREG: >>> No suitable 'ncurses' library found for '${AREG_PROCESSOR}' processor, force to disable extended objects.")
            set(AREG_EXTENDED OFF CACHE INTERNAL "Disable AREG Extended objects")
            add_definitions(-DAREG_EXTENDED=0)
        endif()
    else()
        add_definitions(-DAREG_EXTENDED=1)
    endif()
else()
    add_definitions(-DAREG_EXTENDED=0)
endif()

if (AREG_LOGS)
    add_definitions(-DAREG_LOGS=1)
else()
    add_definitions(-DAREG_LOGS=0)
endif()

if (AREG_BITNESS EQUAL 32)
    add_definitions(-DBIT32)
else()
    add_definitions(-DBIT64)
endif()

# -------------------------------------------------------
# Setup product paths
# -------------------------------------------------------

if (AREG_ENABLE_OUTPUTS)

    # The output directory
    if (NOT DEFINED AREG_OUTPUT_DIR OR "${AREG_OUTPUT_DIR}" STREQUAL "")
        # Relative path of the output folder for the builds
        set(_product_path "build/${AREG_COMPILER_FAMILY}-${AREG_COMPILER_SHORT}/${AREG_OS}-${AREG_BITNESS}-${AREG_PROCESSOR}-${CMAKE_BUILD_TYPE}-${AREG_BINARY}")
        string(TOLOWER "${_product_path}" _product_path)
        # The absolute path of 'AREG_OUTPUT_DIR' for builds if it is not set.
        set(AREG_OUTPUT_DIR "${AREG_BUILD_ROOT}/${_product_path}")
        unset(_product_path)
    endif()

else()

    # The output directory
    if (NOT DEFINED AREG_OUTPUT_DIR OR "${AREG_OUTPUT_DIR}" STREQUAL "")
        set(AREG_OUTPUT_DIR "${AREG_BUILD_ROOT}")
    endif()

endif()

# The directory to output static libraries
if (NOT DEFINED AREG_OUTPUT_LIB OR "${AREG_OUTPUT_LIB}" STREQUAL "")
    # set absolute path to AREG_OUTPUT_LIB if it is not manually set
    set(AREG_OUTPUT_LIB "${AREG_OUTPUT_DIR}/lib")
endif()

# The directory to output shared libraries and executables
if (NOT DEFINED AREG_OUTPUT_BIN OR "${AREG_OUTPUT_BIN}" STREQUAL "")
    # set absolute path to AREG_OUTPUT_BIN if it is not manually set
    set(AREG_OUTPUT_BIN "${AREG_OUTPUT_DIR}/bin")
endif()

# The absolute path for compiled object files.
set(AREG_OUTPUT_OBJ "${AREG_OUTPUT_DIR}/obj")

# Setting output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${AREG_OUTPUT_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${AREG_OUTPUT_BIN})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${AREG_OUTPUT_LIB})

# Make sure that the output directories are set for each configuration.
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} AREG_OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${AREG_OUTPUTCONFIG} ${AREG_OUTPUT_BIN} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${AREG_OUTPUTCONFIG} ${AREG_OUTPUT_BIN} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${AREG_OUTPUTCONFIG} ${AREG_OUTPUT_LIB} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# Adding compile options
add_compile_options(${AREG_COMPILER_OPTIONS})

# Adding areg/product directory for clean-up
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${AREG_OUTPUT_DIR})

# Add include search paths
include_directories(BEFORE "${AREG_FRAMEWORK}" "${AREG_BUILD_ROOT}" "${AREG_GENERATE_DIR}" "${AREG_THIRDPARTY}")

# Adding library search paths
link_directories(BEFORE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

# Only for Linux
if(UNIX AND NOT CYGWIN)
    set(CMAKE_EXECUTABLE_SUFFIX ".elf")
endif()

set(COMMON_COMPILE_DEF)
if(AREG_BINARY MATCHES "static")
    set(COMMON_COMPILE_DEF IMP_AREG_LIB)
else()
    set(COMMON_COMPILE_DEF IMP_AREG_DLL)
endif()

find_package(Java COMPONENTS Runtime)
if (NOT ${Java_FOUND})
    find_package(Java COMPONENTS Development)
endif()

# Check and setup variables for installation
if (AREG_INSTALL)
    option(INSTALL_GTEST "Disable Googletest installation" OFF)
    if (NOT "${AREG_INSTALL_PATH}" STREQUAL "")
        set(CMAKE_INSTALL_PREFIX "${AREG_INSTALL_PATH}")
    endif()
endif()
