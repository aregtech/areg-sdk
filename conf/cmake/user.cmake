# ###########################################################################
# User specific settings
# Copyright 2022-2023 Aregtech
# ###########################################################################

# ---------------------------------------------------------------------------
# This CMake file allows configuration of various AREG Framework build options.
# You can pass these options via the command line, or the system will use default values.
#
# Available Options:
#   1. AREG_COMPILER_FAMILY -- A quick way to set the C++ and C compilers (CMAKE_CXX_COMPILER and CMAKE_C_COMPILER).
#   2. AREG_COMPILER        -- Sets a specific compiler for both C++ and C projects.
#   3. AREG_BINARY          -- Specifies the library type for the AREG Framework ('shared' or 'static'). Defaults to 'shared'.
#   4. AREG_LOGGER_LIB      -- Specifies the type of the Log Observer API library ('shared' or 'static'). Defaults to 'shared'.
#   5. AREG_BUILD_TYPE      -- Specifies the build configuration ('Debug' or 'Release').
#   6. AREG_BUILD_TESTS     -- Enables or disables building unit tests for the AREG Framework.
#   7. AREG_BUILD_EXAMPLES  -- Enables or disables building examples for the AREG Framework.
#   8. AREG_EXTENDED        -- Enables or disables extended AREG Framework features. May require additional dependencies.
#   9. AREG_LOGS            -- Enables or disables logging during compilation. Defaults to 'enabled'.
#  10. AREG_INSTALL         -- Enables or disables the installation of the AREG SDK. Defaults to 'enabled'. If enabled, any dependent libraries such as 'sqlite3' and 'ncurses' must also be installed.
#  11. AREG_USE_PACKAGES    -- Enables or disables using installed packages. Controls other package options like SQLite and GTest.
#  12. AREG_SQLITE_PACKAGE  -- Determines if the system's SQLite3 package should be used or compiled from source.
#  13. AREG_GTEST_PACKAGE   -- Determines if the system's GTest package should be used or compiled from source.
#  14. AREG_BUILD_ROOT      -- Specifies the root directory for build files. Defaults to './product' within the AREG SDK root.
#  15. AREG_OUTPUT_DIR      -- Directory where build outputs are placed.
#  16. AREG_OUTPUT_BIN      -- Directory for output binaries (executables and shared libraries).
#  17. AREG_OUTPUT_LIB      -- Directory for output static libraries.
#  18. AREG_PACKAGES        -- Location for fetching third-party packages such as GTest.
#  19. AREG_INSTALL_PATH    -- Location where AREG SDK binaries, headers, and tools are installed. Defaults to the user’s home directory.
#  20. AREG_ENABLE_OUTPUTS  -- If disabled, output directories will match the CMake binary directory.
#
# Default Values:
#   1. AREG_COMPILER_FAMILY = <default> (possible values: gnu, cygwin, llvm, msvc)
#   2. AREG_COMPILER        = <default> (possible values: g++, gcc, c++, cc, clang++, clang, clang-cl, cl)
#   3. AREG_BINARY          = shared    (possible values: shared, static)
#   4. AREG_LOGGER_LIB      = shared    (possible values: shared, static)
#   5. AREG_BUILD_TYPE      = Release   (possible values: Release, Debug)
#   6. AREG_BUILD_TESTS     = OFF       (possible values: ON, OFF)
#   7. AREG_BUILD_EXAMPLES  = ON        (possible values: ON, OFF)
#   8. AREG_EXTENDED        = OFF       (possible values: ON, OFF)
#   9. AREG_LOGS            = ON        (possible values: ON, OFF)
#  10. AREG_INSTALL         = ON        (possible values: ON, OFF)
#  11. AREG_USE_PACKAGES    = ON        (possible values: ON, OFF)
#  12. AREG_SQLITE_PACKAGE  = ON        (possible values: ON, OFF)
#  13. AREG_GTEST_PACKAGE   = ON        (possible values: ON, OFF)
#  14. AREG_BUILD_ROOT      = '<areg-sdk>/product'    (path for output and generated files)
#  15. AREG_OUTPUT_DIR      = '<areg-sdk>/product/build/<default-compiler family-name>/<os>-<bitness>-<cpu>-release-<areg-lib>'
#  16. AREG_OUTPUT_BIN      = '<areg-sdk>/product/build/<default-compiler family-name>/<os>-<bitness>-<cpu>-release-<areg-lib>/bin'
#  17. AREG_OUTPUT_LIB      = '<areg-sdk>/product/build/<default-compiler family-name>/<os>-<bitness>-<cpu>-release-<areg-lib>/lib'
#  18. AREG_PACKAGES        = '${AREG_BUILD_ROOT}/packages'
#  19. AREG_INSTALL_PATH    = '${HOME}/areg-sdk' (or '${USERPROFILE}' on Windows, defaults to current directory if unset)
#  20. AREG_ENABLE_OUTPUTS  = ON        (possible values: ON, OFF)
#
# Hints:
#   - AREG_COMPILER_FAMILY is an easy way to set compilers:
#       - 'gnu' sets g++ and gcc for C++ and C.
#       - 'cygwin' sets g++ and gcc for C++ and C under Cygwin.
#       - 'llvm' sets clang++, clang, and clang-cl (on Windows) for C++ and C.
#       - 'msvc' sets Microsoft Visual C++ (cl) for C++ and C.
#
# Example Command:
# $ cmake -B ./build -DCMAKE_BUILD_TYPE=Debug -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TESTS=ON -DAREG_BUILD_EXAMPLES=ON
#
# Notes:
#   - To integrate manually build binaries of AREG SDK into another project, you can simply set `AREG_BUILD_ROOT` to the project's root directory.
#
# Example Command with AREG_BUILD_ROOT to output binaries in custom directory:
# $ cmake -B ./build -DAREG_BUILD_ROOT="/home/developer/projects/my_project/product"
#
# Integration:
#   - Visit https://github.com/aregtech/areg-sdk-demo repository to see various ways of AREG Framework integration.
# ---------------------------------------------------------------------------

# CPP compiler, possible values: g++, gcc, c++, cc, clang++, clang, clang-cl, cl
set(AREG_CXX_COMPILER)

# C compiler, possible values: gcc, clang, clang-cl, cl
set(AREG_C_COMPILER)

# Check the compiler option.
# Check and set the simple and shortest way to set the compiler
if(DEFINED AREG_COMPILER_FAMILY AND NOT ${AREG_COMPILER_FAMILY} STREQUAL "")

    message(STATUS "AREG: >>> User selected C/C++ compiler family \'${AREG_COMPILER_FAMILY}\'")
    if(${AREG_COMPILER_FAMILY} STREQUAL "gnu")
        set(AREG_CXX_COMPILER "g++")
        set(AREG_C_COMPILER   "gcc")
    elseif(${AREG_COMPILER_FAMILY} STREQUAL "cygwin")
        set(AREG_CXX_COMPILER "g++")
        set(AREG_C_COMPILER   "gcc")
    elseif(${AREG_COMPILER_FAMILY} STREQUAL "llvm")
        if (WIN32)
            set(AREG_CXX_COMPILER "clang-cl")
            set(AREG_C_COMPILER   "clang-cl")
        else()
            set(AREG_CXX_COMPILER "clang++")
            set(AREG_C_COMPILER   "clang")
        endif()
    elseif(${AREG_COMPILER_FAMILY} STREQUAL "msvc")
        set(AREG_CXX_COMPILER "cl")
        set(AREG_C_COMPILER   "cl")
    else()
        message(WARNING "AREG: >>> Unrecognized compiler family ${AREG_COMPILER_FAMILY}, supported values: \'gnu\', \'llvm\', \'cygwin\', \'msvc\'")
    endif()

elseif(DEFINED AREG_COMPILER AND NOT ${AREG_COMPILER} STREQUAL "")

    set(AREG_CXX_COMPILER   "${AREG_COMPILER}")
    set(AREG_C_COMPILER     "${AREG_COMPILER}")

    if (${AREG_COMPILER} STREQUAL "g++" OR ${AREG_COMPILER} STREQUAL "gcc")
        # GNU compiler

        # Make sure that C-compiler is properly set
        if (${AREG_COMPILER} STREQUAL "g++")
            set(AREG_C_COMPILER     "gcc")
        endif()

        if (CYGWIN)
            set(AREG_COMPILER_FAMILY "cygwin")
        else()
            set(AREG_COMPILER_FAMILY "gnu")
        endif()

    elseif (${AREG_COMPILER} STREQUAL "c++" OR ${AREG_COMPILER} STREQUAL "cc")
        # GNU compiler

        # Make sure that C-compiler is properly set
        if (${AREG_COMPILER} STREQUAL "c++")
            set(AREG_C_COMPILER     "cc")
        endif()

        if (CYGWIN)
            set(AREG_COMPILER_FAMILY "cygwin")
        else()
            set(AREG_COMPILER_FAMILY "gnu")
        endif()
        
    elseif (${AREG_COMPILER} STREQUAL "clang-cl" OR ${AREG_COMPILER} STREQUAL "clang++" OR ${AREG_COMPILER} STREQUAL "clang")
        # Clang compiler

        # Make sure that C-compiler is properly set
        if (${AREG_COMPILER} STREQUAL "clang-cl")
            set(AREG_C_COMPILER     "clang-cl")
        elseif (${AREG_COMPILER} STREQUAL "clang++")
            set(AREG_C_COMPILER     "clang")
        endif()

        set(AREG_COMPILER_FAMILY    "llvm")
        
    elseif (${AREG_COMPILER} STREQUAL "cl")
        set(AREG_COMPILER_FAMILY    "msvc")
    else()
        set(AREG_COMPILER_FAMILY)
        message(WARNING "AREG: >>> Unrecognized compiler ${AREG_COMPILER}, supported compilers: \'gcc\', \'g++\', 'cc', \'c++\',\'clang\', \'clang++\', \'clang-cl\', \'cl\'")
    endif()

else()

    message(STATUS "AREG: >>> No compiler is selected, will use system default")

endif()

# Set build configuration. Set "Debug" for debug build, and "Release" for release build.
if (NOT "${AREG_BUILD_TYPE}" STREQUAL "Debug")
    if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "")
        set(AREG_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
    else()
        set(AREG_BUILD_TYPE "Release")
    endif()
endif()

# Set the AREG binary library type to compile. Set "shared" if not "static"
if (NOT DEFINED AREG_BINARY)
    if (DEFINED VCPKG_LIBRARY_LINKAGE AND "${VCPKG_LIBRARY_LINKAGE}" STREQUAL "static")
        set(AREG_BINARY "static")
    else()
        set(AREG_BINARY "shared")
    endif()
elseif (NOT "${AREG_BINARY}" STREQUAL "static")
    set(AREG_BINARY "shared")
endif()

# Set the areg log observer API library type.
if (NOT DEFINED AREG_LOGGER_LIB)
    if (DEFINED VCPKG_LIBRARY_LINKAGE AND "${VCPKG_LIBRARY_LINKAGE}" STREQUAL "static")
        set(AREG_LOGGER_LIB "static")
    else()
        set(AREG_LOGGER_LIB "shared")
    endif()
elseif (NOT "${AREG_LOGGER_LIB}" STREQUAL "static")
    set(AREG_LOGGER_LIB "shared")
endif()

# Build tests. By default it is disabled. To enable, set ON
macro_create_option(AREG_BUILD_TESTS ON "Build unit tests")

# Build examples. By default it is disabled. To enable, set ON
macro_create_option(AREG_BUILD_EXAMPLES ON "Build examples")

# Set AREG extended features enable or disable flag to compiler additional optional features. By default, it is disabled.
macro_create_option(AREG_EXTENDED OFF "Enable extended feature")

# Modify 'AREG_LOGS' to enable or disable compilation with logs. By default, compile with logs
macro_create_option(AREG_LOGS ON "Compile with logs")

# Modify 'AREG_INSTALL' to enable or disable installation of AREG SDK
macro_create_option(AREG_INSTALL ON "Enable installation")

# Check the request of using installed packages
if (NOT DEFINED AREG_USE_PACKAGES)
    # Set default values
    macro_create_option(AREG_USE_PACKAGES      ON  "Enable using installed packages")
    macro_create_option(AREG_SQLITE_PACKAGE    ON  "Use SQLite3 installed package")
    macro_create_option(AREG_GTEST_PACKAGE     ON  "Use GTest installed package")
elseif(AREG_USE_PACKAGES)
    # Using packages is enabled by default, set values
    macro_create_option(AREG_USE_PACKAGES      ON  "Enable using installed packages")
    macro_create_option(AREG_SQLITE_PACKAGE    ON  "Use SQLite3 installed package")
    macro_create_option(AREG_GTEST_PACKAGE     ON  "Use GTest installed package")
else()
    # Using packages is disabled by default, set values
    macro_create_option(AREG_USE_PACKAGES      OFF "Enable using installed packages")
    macro_create_option(AREG_SQLITE_PACKAGE    OFF "Use SQLite3 installed package")
    macro_create_option(AREG_GTEST_PACKAGE     OFF "Use GTest installed package")
    macro_create_option(AREG_INSTALL           OFF "Enable installation")
endif()

# CPP standard for the projects
set(AREG_CXX_STANDARD 17)

# Specify default bitness, the system bitness is detected in 'common.cmake'
set(AREG_BITNESS "64")

# Specify CPU platform here, the system CPU platform is detected in 'commmon.cmake'
set(AREG_PROCESSOR "x86_64")


if (NOT DEFINED AREG_ENABLE_OUTPUTS OR AREG_ENABLE_OUTPUTS)
    option(AREG_ENABLE_OUTPUTS "Enable changing output directories" TRUE)
    # Set the areg-sdk build root folder to output files.
    if (NOT DEFINED AREG_BUILD_ROOT OR "${AREG_BUILD_ROOT}" STREQUAL "")
        set(AREG_BUILD_ROOT "${AREG_SDK_ROOT}/product")
    endif()

    if (NOT DEFINED AREG_PACKAGES OR "${AREG_PACKAGES}" STREQUAL "")
        set(AREG_PACKAGES "${AREG_BUILD_ROOT}/packages")
    endif()
else()
    option(AREG_ENABLE_OUTPUTS "Enable changing output directories" FALSE)
    if (NOT DEFINED AREG_BUILD_ROOT OR "${AREG_BUILD_ROOT}" STREQUAL "")
        set(AREG_BUILD_ROOT "${CMAKE_BINARY_DIR}")
    endif()
endif()

# The relative path for generated files
if ("${AREG_GENERATE}" STREQUAL "")
    set(AREG_GENERATE "generate")
endif()

if ("${AREG_GENERATE_DIR}" STREQUAL "")
    set(AREG_GENERATE_DIR "${AREG_BUILD_ROOT}/${AREG_GENERATE}")
endif()
