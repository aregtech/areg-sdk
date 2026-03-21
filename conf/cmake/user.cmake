# ###########################################################################
# User specific settings
# Copyright 2022-2026 Aregtech
# ###########################################################################

# ---------------------------------------------------------------------------
# This CMake file allows configuration of various Areg Framework build options.
# You can pass these options via the command line, or the system will use default values.
#
# Available Options:
#   1. AREG_LIB_TYPE        -- Specifies the library type for the Areg Framework ('shared' or 'static'). Defaults to 'shared'.
#   2. AREG_COMPILER        -- Sets a specific compiler for both C++ and C projects.
#   3. AREG_COMPILER_FAMILY -- A quick way to set the C++ and C compilers (CMAKE_CXX_COMPILER and CMAKE_C_COMPILER).
#   4. AREG_EXAMPLES        -- Enables or disables building examples for the Areg Framework.
#   5. AREG_EXTENDED        -- Enables or disables extended Areg Framework features. May require additional dependencies.
#   6. AREG_LOGGING         -- Enables or disables logging during compilation. Defaults to 'enabled'.
#   7. AREG_TESTS           -- Enables or disables building unit tests for the Areg Framework.
#   8. AREG_INSTALL         -- Enables or disables installation of Areg SDK binaries, headers and dependencies like 'sqlite3' and 'ncurses'.
#   9. AREG_INSTALL_DIR     -- Location where Areg SDK binaries, headers, and tools are installed. Defaults to the user's home directory
#  10. AREG_SYSTEM_PACKAGES -- Enables or disables using installed packages. Controls other package options like SQLite and GTest.
#  11. AREG_SYSTEM_GTEST    -- Determines if the system's GTest package should be used or compiled from source.
#  12. AREG_SYSTEM_SQLITE   -- Determines if the system's SQLite3 package should be used or compiled from source.
#  13. AREG_BUILD_DIR       -- Specifies the root directory for build files. Defaults to './product' within the Areg SDK root.
#  14. AREG_OUTPUT_DIR      -- Directory where build outputs are placed.
#  15. AREG_BIN_DIR         -- Directory for output binaries (executable and shared libraries).
#  16. AREG_LIB_DIR         -- Directory for output static libraries.
#  17. AREG_DEPS_DIR        -- Location for fetching third-party packages such as GTest.
#  18. AREG_OUTPUT_LAYOUT   -- If disabled, output directories will match the CMake binary directory.
#  19. AREG_LOGGER_LIB_TYPE -- Specifies the type of the Log Observer API library ('shared' or 'static'). Defaults to 'shared'.
#  20. AREG_ARCH            -- The processor architect. Ignore if need to use system default.
#  21. AREG_TARGET          -- Specifies the compiler and library architecture target. Defaults to the system-defined compiler and architecture.
#
# Default Values:
#   1. AREG_LIB_TYPE        = shared    (possible values: shared, static)
#   2. AREG_COMPILER        = <default> (possible values: g++, gcc, c++, cc, clang++, clang, clang-cl, cl)
#   3. AREG_COMPILER_FAMILY = <default> (possible values: gnu, cygwin, mingw, llvm, msvc)
#   4. AREG_EXAMPLES        = ON        (possible values: ON, OFF)
#   5. AREG_EXTENDED        = OFF       (possible values: ON, OFF)
#   6. AREG_LOGGING         = ON        (possible values: ON, OFF)
#   7. AREG_TESTS           = ON        (possible values: ON, OFF)
#   8. AREG_INSTALL         = ON        (possible values: ON, OFF)
#   9. AREG_INSTALL_DIR     = '${HOME}/areg-sdk' (or '${USERPROFILE}' on Windows, defaults to current directory if unset
#  10. AREG_SYSTEM_PACKAGES = ON        (possible values: ON, OFF)
#  11. AREG_SYSTEM_GTEST    = ON        (possible values: ON, OFF)
#  12. AREG_SYSTEM_SQLITE   = ON        (possible values: ON, OFF)
#  13. AREG_BUILD_DIR       = '<areg-sdk>/product'    (path for output and generated files)
#  14. AREG_OUTPUT_DIR      = '<areg-sdk>/product/build/<default-compiler family-name>/<os>-<bitness>-<cpu>-release-<areg-lib>'
#  15. AREG_BIN_DIR         = '<areg-sdk>/product/build/<default-compiler family-name>/<os>-<bitness>-<cpu>-release-<areg-lib>/bin'
#  16. AREG_LIB_DIR         = '<areg-sdk>/product/build/<default-compiler family-name>/<os>-<bitness>-<cpu>-release-<areg-lib>/lib'
#  17. AREG_DEPS_DIR        = '${CMAKE_BINARY_DIR}/packages'
#  18. AREG_OUTPUT_LAYOUT   = ON        (possible values: ON, OFF)
#  19. AREG_LOGGER_LIB_TYPE = shared    (possible values: shared, static)
#  20. AREG_ARCH            = System    (possible values: x86 (i386, i486), x64 (x86_64, x86-64, amd64, ia64), arm (arm32, armv7), aarch64 (arm64))
#  21. AREG_TARGET          = <default> (possible values: 'i386-linux-gnu', 'x86_64-linux-gnu', 'arm-linux-gnueabihf', 'aarch64-linux-gnu')
#
# Hints:
#   - AREG_COMPILER_FAMILY is an easy way to set compilers:
#       - 'gnu' sets g++ and gcc for C++ and C.
#       - 'cygwin' sets g++ and gcc for C++ and C under Cygwin.
#       - 'mingw' sets g++ and gcc for C++ and C under MinGW.
#       - 'llvm' sets clang++, clang, and clang-cl (on Windows) for C++ and C.
#       - 'msvc' sets Microsoft Visual C++ (cl) for C++ and C.
#
# Example Command:
# $ cmake -B ./build -DCMAKE_BUILD_TYPE=Debug -DAREG_COMPILER_FAMILY=llvm -DAREG_TESTS=ON -DAREG_EXAMPLES=ON
#
# Notes:
#   - To integrate manually build binaries of Areg SDK into another project, you can simply set `AREG_BUILD_DIR` to the project's root directory.
#
# Example Command with AREG_BUILD_DIR to output binaries in custom directory:
# $ cmake -B ./build -DAREG_BUILD_DIR="/home/developer/projects/my_project/product"
#
# Example of installing Areg SDK to custom location:
# $ cmake -B ./build -DAREG_INSTALL_DIR="/custom/install/path"
# $ cmake --build ./build -j16 --target install
# or if permission is required:
# $ sudo cmake --build ./build -j16 --target install
#
# Hint:
# To use Areg SDK cmake options in other project, the 'user.cmake' file should be included before first call of "project()".
# Otherwise, Areg SDK settings uses cmake options CMAKE_CXX_COMPILER and CMAKE_BUILD_TYPE options.
#
# Integration:
#   - Visit https://github.com/aregtech/areg-sdk-demo repository to see various ways of Areg Framework integration.
# ---------------------------------------------------------------------------

# Set variables for C++ and C compilers and their short name
set(AREG_CXX_COMPILER)
set(AREG_C_COMPILER)
set(AREG_COMPILER_SHORT)

# Specify CPU platform here, the system CPU platform is detected in 'commmon.cmake'
if (DEFINED AREG_ARCH AND NOT "${AREG_ARCH}" STREQUAL "")
    macro_get_processor(${AREG_ARCH} AREG_ARCH AREG_BITNESS _found_proc)
    set(CMAKE_SYSTEM_PROCESSOR ${AREG_ARCH})
endif()

if (DEFINED CMAKE_CXX_COMPILER_TARGET AND NOT "${CMAKE_CXX_COMPILER_TARGET}" STREQUAL "")
    set(AREG_TARGET ${CMAKE_CXX_COMPILER_TARGET})
endif()

# If CMake compilers are specified, use them
if ((DEFINED CMAKE_CXX_COMPILER OR DEFINED CMAKE_C_COMPILER) AND (NOT "${CMAKE_CXX_COMPILER}" STREQUAL "" OR NOT "${CMAKE_C_COMPILER}" STREQUAL ""))

    # Determine the system compiler based on the available CMake variables
    if (NOT "${CMAKE_CXX_COMPILER}" STREQUAL "")
        set(_sys_compiler "${CMAKE_CXX_COMPILER}")
    else()
        set(_sys_compiler "${CMAKE_C_COMPILER}")
    endif()

    message(STATUS "Areg: >>> Using CMake specified C++ compiler '${_sys_compiler}'")

    if (DEFINED AREG_ARCH AND NOT "${AREG_ARCH}" STREQUAL "")
        set(_sys_process ${AREG_ARCH})
    elseif(DEFINED CMAKE_SYSTEM_PROCESSOR AND NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "")
        set(_sys_process ${CMAKE_SYSTEM_PROCESSOR})
    endif()

    # Setup compiler details based on the identified system compiler
    macro_setup_compilers_data("${_sys_compiler}" _compiler_family _compiler_short _cxx_compiler _c_compiler _sys_target _sys_process _sys_bitness _compiler_found)

    if (_compiler_found)
        # Check for existing compiler family or specific compiler and issue warnings if necessary
        if (NOT "${AREG_COMPILER_FAMILY}" STREQUAL "" AND NOT "${AREG_COMPILER_FAMILY}" STREQUAL "${_compiler_family}")
            message(WARNING "Areg: >>> Selected compiler family '${AREG_COMPILER_FAMILY}' is ignored; using '${_compiler_family}'")
        endif()

        # Only if AREG_COMPILER is defined, compare the short compiler name to the full path
        if (NOT "${AREG_COMPILER}" STREQUAL "")
            string(FIND "${AREG_COMPILER}" "${_compiler_short}" _found_pos)
            if (_found_pos LESS 0)
                message(WARNING "Areg: >>> Selected compiler '${AREG_COMPILER}' is ignored; using '${_compiler_short}'")
            endif()
        endif()

        # Set the relevant variables for the compiler
        set(AREG_COMPILER_FAMILY    "${_compiler_family}")
        set(AREG_COMPILER           "${_sys_compiler}")
        set(AREG_COMPILER_SHORT     "${_compiler_short}")
        set(AREG_CXX_COMPILER       "${_sys_compiler}")
        set(AREG_C_COMPILER         "${_c_compiler}")
        if("${AREG_TARGET}" STREQUAL "")
            set(AREG_TARGET         "${_sys_target}")
        endif()
        if (NOT "${_sys_process}" STREQUAL "")
            set(AREG_ARCH ${_sys_process})
            set(AREG_BITNESS   ${_sys_bitness})
            set(CMAKE_SYSTEM_PROCESSOR ${AREG_ARCH})
        endif()
    else()
        message(WARNING "Areg: >>> Unknown C++ compiler '${_sys_compiler}'; results may be unpredictable")
    endif()

    unset(_sys_compiler)
    unset(_sys_process)
    unset(_sys_bitness)
    unset(_sys_target)
    unset(_compiler_found)

# If a specific compiler family is set, use that to determine compilers
elseif (DEFINED AREG_COMPILER_FAMILY AND NOT "${AREG_COMPILER_FAMILY}" STREQUAL "")

    message(STATUS "Areg: >>> Using user-specified C/C++ compiler family '${AREG_COMPILER_FAMILY}'")
    if ("${AREG_ARCH}" STREQUAL "")
        set(AREG_ARCH ${CMAKE_SYSTEM_PROCESSOR})
    endif()
    macro_setup_compilers_data_by_family("${AREG_COMPILER_FAMILY}" _compiler_short _cxx_compiler _c_compiler _compiler_target _compiler_found)

    if (_compiler_found)
        # Set the relevant variables for the chosen compiler family
        set(AREG_COMPILER           "${_cxx_compiler}")
        set(AREG_COMPILER_SHORT     "${_compiler_short}")
        set(AREG_CXX_COMPILER       "${_cxx_compiler}")
        set(AREG_C_COMPILER         "${_c_compiler}")
        if("${AREG_TARGET}" STREQUAL "")
            set(AREG_TARGET         "${_compiler_target}")
        endif()
    else()
        message(WARNING "Areg: >>> Unknown compiler family '${AREG_COMPILER_FAMILY}'; results may be unpredictable")
    endif()

    unset(_compiler_short)
    unset(_cxx_compiler)
    unset(_c_compiler)
    unset(_compiler_target)
    unset(_compiler_found)

# If a specific compiler is set, use that to determine compilers
elseif (DEFINED AREG_COMPILER AND NOT "${AREG_COMPILER}" STREQUAL "")

    message(STATUS "Areg: >>> Using user-specified C/C++ compiler '${AREG_COMPILER}'")
    # Set both C and C++ compilers based on AREG_COMPILER
    macro_setup_compilers_data("${AREG_COMPILER}" _compiler_family _compiler_short _cxx_compiler _c_compiler _sys_target _sys_process _sys_bitness _compiler_found)

    if (_compiler_found)
        # Set the relevant variables for the chosen compiler
        set(AREG_COMPILER_FAMILY    "${_compiler_family}")
        set(AREG_COMPILER_SHORT     "${_compiler_short}")
        set(AREG_CXX_COMPILER       "${_cxx_compiler}")
        set(AREG_C_COMPILER         "${_c_compiler}")
        if("${AREG_TARGET}" STREQUAL "")
            set(AREG_TARGET         "${_sys_target}")
        endif()
        if (NOT "${_sys_process}" STREQUAL "")
            set(AREG_ARCH ${_sys_process})
            set(AREG_BITNESS   ${_sys_bitness})
            set(CMAKE_SYSTEM_PROCESSOR ${AREG_ARCH})
        endif()
    else()
        message(WARNING "Areg: >>> Unknown compiler '${AREG_COMPILER}'; results may be unpredictable")
    endif()

    unset(_sys_compiler)
    unset(_sys_process)
    unset(_sys_bitness)
    unset(_sys_target)
    unset(_compiler_found)

# If no specific compiler or family is set, use the system default
else()
    message(STATUS "Areg: >>> No compiler specified; using system default compilers.")
endif()

# Set build configuration. Set "Debug" for debug build, and "Release" for release build.
if (NOT DEFINED CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Set the Areg binary library type to compile. Set "shared" if not "static"
if (NOT DEFINED AREG_LIB_TYPE)
    if (DEFINED VCPKG_LIBRARY_LINKAGE AND "${VCPKG_LIBRARY_LINKAGE}" STREQUAL "static")
        set(AREG_LIB_TYPE "static")
    else()
        set(AREG_LIB_TYPE "shared")
    endif()
elseif (NOT "${AREG_LIB_TYPE}" STREQUAL "static")
    set(AREG_LIB_TYPE "shared")
endif()

# Set the areg log observer API library type.
if (NOT DEFINED AREG_LOGGER_LIB_TYPE)
    if (DEFINED VCPKG_LIBRARY_LINKAGE AND "${VCPKG_LIBRARY_LINKAGE}" STREQUAL "static")
        set(AREG_LOGGER_LIB_TYPE "static")
    else()
        set(AREG_LOGGER_LIB_TYPE "shared")
    endif()
elseif (NOT "${AREG_LOGGER_LIB_TYPE}" STREQUAL "static")
    set(AREG_LOGGER_LIB_TYPE "shared")
endif()

# Build tests. By default it is disabled. To enable, set ON
macro_create_option(AREG_TESTS ON "Build unit tests")

# Build examples. By default it is disabled. To enable, set ON
macro_create_option(AREG_EXAMPLES ON "Build examples")

# Generate only service files. By default it is disabled. To enable, set ON
macro_create_option(AREG_GENERATE_ONLY OFF "Generate only service files")

# Set Areg extended features enable or disable flag to compiler additional optional features. By default, it is disabled.
macro_create_option(AREG_EXTENDED OFF "Enable extended feature")

# Modify 'AREG_LOGGING' to enable or disable compilation with logs. By default, compile with logs
macro_create_option(AREG_LOGGING ON "Compile with logs")

# Modify 'AREG_INSTALL' to enable or disable installation of Areg SDK
macro_create_option(AREG_INSTALL ON "Enable installation")

# Check the request of using installed packages
if (NOT DEFINED AREG_SYSTEM_PACKAGES)
    # Set default values
    macro_create_option(AREG_SYSTEM_PACKAGES      ON  "Enable using installed packages")
    macro_create_option(AREG_SYSTEM_SQLITE    ON  "Use SQLite3 installed package")
    macro_create_option(AREG_SYSTEM_GTEST     ON  "Use GTest installed package")
elseif(AREG_SYSTEM_PACKAGES)
    # Using packages is enabled by default, set values
    macro_create_option(AREG_SYSTEM_PACKAGES      ON  "Enable using installed packages")
    macro_create_option(AREG_SYSTEM_SQLITE    ON  "Use SQLite3 installed package")
    macro_create_option(AREG_SYSTEM_GTEST     ON  "Use GTest installed package")
else()
    # Using packages is disabled by default, set values
    macro_create_option(AREG_SYSTEM_PACKAGES      OFF "Enable using installed packages")
    macro_create_option(AREG_SYSTEM_SQLITE    OFF "Use SQLite3 installed package")
    macro_create_option(AREG_SYSTEM_GTEST     OFF "Use GTest installed package")
    macro_create_option(AREG_INSTALL           OFF "Enable installation")
endif()

