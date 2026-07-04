# ###########################################################################
# Common settings for all projects
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

set(AREG_PACKAGE_NAME   "areg")

# Set processor, if not identified yet.
if ("${AREG_ARCH}" STREQUAL "")
    set(AREG_ARCH ${CMAKE_SYSTEM_PROCESSOR})
endif()

# Detect target platform
set(AREG_PLATFORM_LINUX     FALSE)
set(AREG_PLATFORM_MACOS     FALSE)
set(AREG_PLATFORM_WINDOWS   FALSE)
set(AREG_PLATFORM_POSIX     FALSE)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(AREG_PLATFORM_MACOS TRUE)
    set(AREG_PLATFORM_POSIX TRUE)
    message(STATUS "Areg: >>> Target platform = macOS")
    
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(AREG_PLATFORM_LINUX TRUE)
    set(AREG_PLATFORM_POSIX TRUE)
    message(STATUS "Areg: >>> Target platform = Linux")
    
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(AREG_PLATFORM_WINDOWS TRUE)
    message(STATUS "Areg: >>> Target platform = Windows")

elseif (CMAKE_SYSTEM_NAME STREQUAL "CYGWIN" OR CYGWIN)
    # The CYGWIN CMake variable is set automatically when CMake is run inside a Cygwin shell;
    # CMAKE_SYSTEM_NAME may also be reported as "CYGWIN" depending on the CMake and toolchain version.
    set(AREG_PLATFORM_POSIX TRUE)
    message(STATUS "Areg: >>> Target platform = Cygwin")

else()
    message(WARNING "Areg: >>> Unknown target platform '${CMAKE_SYSTEM_NAME}'")
endif()

# Cross-compilation detection
if (CMAKE_CROSSCOMPILING)
    message(STATUS "Areg: >>> Cross-compiling ${CMAKE_HOST_SYSTEM_NAME}(${CMAKE_HOST_SYSTEM_PROCESSOR}) -> ${CMAKE_SYSTEM_NAME}(${CMAKE_SYSTEM_PROCESSOR})")
endif()

# Identify compiler short name
if ("${AREG_COMPILER_FAMILY}" STREQUAL "")

    macro_setup_compilers_data("${CMAKE_CXX_COMPILER}" AREG_COMPILER_FAMILY AREG_COMPILER_SHORT AREG_CXX_COMPILER AREG_C_COMPILER AREG_TARGET AREG_ARCH AREG_BITNESS _compiler_found)
    if (_compiler_found)
        message(STATUS "Areg: >>> Use system default settings:")
        message(STATUS "Areg: ... Compiler family = '${AREG_COMPILER_FAMILY}'")
        message(STATUS "Areg: ... CXX compiler = '${AREG_CXX_COMPILER}'")
        message(STATUS "Areg: ... CC  compiler = '${AREG_C_COMPILER}'")
        message(STATUS "Areg: ... Compiler Target = '${AREG_TARGET}'")
    else()
        message(WARNING "Areg: >>> The compiler \'${${CMAKE_CXX_COMPILER}}\' is unknown, the compilation result is unpredictable")
    endif()

elseif("${AREG_COMPILER_SHORT}" STREQUAL "")

    message(FATAL_ERROR "Areg: >>> The file \'${AREG_CMAKE_CONFIG_DIR}/setup.cmake\' should be included before \'common.cmake\', fix it and retry again.")

endif()

# Set identified OS
set(AREG_OS ${CMAKE_SYSTEM_NAME})
# Set bitness, if not identified yet.
if ("${AREG_BITNESS}" STREQUAL "" OR AREG_BITNESS EQUAL 0)
    macro_system_bitness(AREG_BITNESS)
endif()

# Setup for find_xxx() calls
if ("${AREG_TARGET}" STREQUAL "")
    macro_default_target(${AREG_ARCH} AREG_TARGET)
endif()

set(CMAKE_FIND_PACKAGE_RESOLVE_SYMLINKS TRUE)
if ("${CMAKE_CXX_COMPILER_TARGET}" STREQUAL "")
    set(CMAKE_CXX_COMPILER_TARGET   ${AREG_TARGET})
    set(CMAKE_C_COMPILER_TARGET     ${AREG_TARGET})
    if (NOT MSVC)
        set(CMAKE_LIBRARY_ARCHITECTURE  ${AREG_TARGET})
    endif()
else()
    set(AREG_TARGET ${CMAKE_CXX_COMPILER_TARGET})
endif()
if (AREG_BITNESS EQUAL 32)
    set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
    set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS ON)
    set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIBX32_PATHS ON)
else()
    set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)
    set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS OFF)
    set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIBX32_PATHS OFF)
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
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    include(${AREG_CMAKE_CONFIG_DIR}/clang.cmake)

elseif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)

    include(${AREG_CMAKE_CONFIG_DIR}/gnu.cmake)

elseif (MSVC)

    include(${AREG_CMAKE_CONFIG_DIR}/msvc.cmake)

else()

    message(WARNING "Areg: >>> Unsupported compiler type may cause unpredicted results, use default GNU compiler settings and POSIX API")
    include(${AREG_CMAKE_CONFIG_DIR}/gnu.cmake)

endif()

if (AREG_EXTENDED)
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        macro_find_ncurses_package(_ncurses_includes _ncurses_lib _ncurses_found)
        if (_ncurses_found)
            add_definitions(-DAREG_EXTENDED=1)
            list(APPEND AREG_EXTENDED_LIBS ncurses)
            set(AREG_EXTENDED_LIBS_STR "-lncurses")
        else()
            message(STATUS "Areg: >>> No suitable 'ncurses' library found for '${AREG_ARCH}' processor, force to disable extended objects.")
            set(AREG_EXTENDED OFF CACHE INTERNAL "Disable Areg Extended objects")
            add_definitions(-DAREG_EXTENDED=0)
        endif()
    else()
        add_definitions(-DAREG_EXTENDED=1)
    endif()
else()
    add_definitions(-DAREG_EXTENDED=0)
endif()

if (AREG_LOGGING)
    add_definitions(-DAREG_LOGGING=1)
else()
    add_definitions(-DAREG_LOGGING=0)
endif()

if (AREG_BITNESS EQUAL 32)
    add_definitions(-DBIT32)
else()
    add_definitions(-DBIT64)
endif()

# -------------------------------------------------------
# Setup product paths
# -------------------------------------------------------

if (AREG_OUTPUT_LAYOUT)

    # The output directory
    if (NOT DEFINED AREG_OUTPUT_DIR OR "${AREG_OUTPUT_DIR}" STREQUAL "")
        # Relative path of the output folder for the builds
        set(_product_path "build/${AREG_COMPILER_FAMILY}-${AREG_COMPILER_SHORT}/${AREG_OS}-${AREG_BITNESS}-${AREG_ARCH}-${CMAKE_BUILD_TYPE}-${AREG_LIB_TYPE}")
        string(TOLOWER "${_product_path}" _product_path)
        # The absolute path of 'AREG_OUTPUT_DIR' for builds if it is not set.
        set(AREG_OUTPUT_DIR "${AREG_BUILD_DIR}/${_product_path}")
        unset(_product_path)
    endif()

else()

    # The build root directory
    if (NOT DEFINED AREG_BUILD_DIR OR "${AREG_BUILD_DIR}" STREQUAL "")
        set(AREG_BUILD_DIR "${CMAKE_BINARY_DIR}")
    endif()

    # The output directory
    if (NOT DEFINED AREG_OUTPUT_DIR OR "${AREG_OUTPUT_DIR}" STREQUAL "")
        set(AREG_OUTPUT_DIR "${AREG_BUILD_DIR}")
    endif()

endif()

# The directory to output generated files
if ("${AREG_GENERATE_DIR}" STREQUAL "")
    set(AREG_GENERATE_DIR "${AREG_BUILD_DIR}/${AREG_GENERATE}")
endif()

# The directory to output static libraries
if (NOT DEFINED AREG_LIB_DIR OR "${AREG_LIB_DIR}" STREQUAL "")
    # set absolute path to AREG_LIB_DIR if it is not manually set
    set(AREG_LIB_DIR "${AREG_OUTPUT_DIR}/lib")
endif()

# The directory to output shared libraries and executables
if (NOT DEFINED AREG_BIN_DIR OR "${AREG_BIN_DIR}" STREQUAL "")
    # set absolute path to AREG_BIN_DIR if it is not manually set
    set(AREG_BIN_DIR "${AREG_OUTPUT_DIR}/bin")
endif()

# The absolute path for compiled object files.
set(AREG_OUTPUT_OBJ "${AREG_OUTPUT_DIR}/obj")

# Setting output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${AREG_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${AREG_BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${AREG_LIB_DIR})

# Make sure that the output directories are set for each configuration.
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} AREG_OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${AREG_OUTPUTCONFIG} ${AREG_BIN_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${AREG_OUTPUTCONFIG} ${AREG_BIN_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${AREG_OUTPUTCONFIG} ${AREG_LIB_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

# Adding compile options
add_compile_options(${AREG_COMPILER_OPTIONS})

# Adding areg/product directory for clean-up
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${AREG_OUTPUT_DIR})

# Add include search paths
include_directories(BEFORE "${AREG_FRAMEWORK}" "${AREG_BUILD_DIR}" "${AREG_GENERATE_DIR}" "${AREG_THIRDPARTY}")

# Adding library search paths
link_directories(BEFORE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

# Pass -no_warning_for_no_symbols on macOS to suppress harmless warnings.
if (APPLE)
    set(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
    set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()

if (NOT DEFINED CMAKE_EXECUTABLE_SUFFIX OR "${CMAKE_EXECUTABLE_SUFFIX}" STREQUAL "")
    if (AREG_PLATFORM_MACOS)
        # Only for macOS
        set(CMAKE_EXECUTABLE_SUFFIX ".mac")
    elseif ((UNIX) AND (NOT CYGWIN) AND (NOT MINGW))
        # Only for Linux
        set(CMAKE_EXECUTABLE_SUFFIX ".elf")
    endif()
endif() 

set(COMMON_COMPILE_DEF)
if(AREG_LIB_TYPE MATCHES "static")
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
    if (NOT "${AREG_INSTALL_DIR}" STREQUAL "")
        set(CMAKE_INSTALL_PREFIX "${AREG_INSTALL_DIR}")
    endif()
endif()
