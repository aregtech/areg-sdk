# ###########################################################################
# User specific settings
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################

# ---------------------------------------------------------------------------
# The build options of the Areg Framework. Pass them with '-D<name>=<value>'.
#
#   AREG_LIB_TYPE        -- 'shared' (default) or 'static' library type of 'areg' and 'areglogger'.
#   AREG_COMPILER_FAMILY -- Selects the C and C++ compilers by family: 'gnu', 'llvm', 'msvc', 'cygwin', 'mingw'. Without it, CMAKE_CXX_COMPILER or the system default compiler is used.
#   AREG_LOGGING         -- ON (default): compile with logs.
#   AREG_EXAMPLES        -- Build the examples. ON when areg is the top-level project, OFF inside another project.
#   AREG_TESTS           -- Build the unit tests. ON when areg is the top-level project, OFF inside another project.
#   AREG_BENCHMARKS      -- OFF (default): build the performance benchmarks.
#   AREG_INSTALL         -- ON (default): create the install rules. The location is CMAKE_INSTALL_PREFIX.
#   AREG_SYSTEM_SQLITE   -- ON (default): use the installed SQLite3, else build the bundled one.
#   AREG_SYSTEM_GTEST    -- ON (default): use the installed GoogleTest, else fetch and build it.
#   AREG_NO_EXCEPTIONS   -- OFF (default): compile without exceptions and RTTI (GNU/Clang only).
#   AREG_OUTPUT_DIR      -- Root of the 'bin' and 'lib' output directories. Defaults to the build directory; a relative path is relative to the build directory.
#   AREG_ARCH            -- Target processor: 'x86', 'x64', 'arm', 'aarch64'. Defaults to the system processor.
#                           Without a compiler, it selects the host compiler family: 'gnu' on Linux, 'llvm' on macOS.
#
# Everything of a build goes to the build directory: the CMake cache, the objects, 'generate',
# 'bin' and 'lib'. The standard CMake variables apply as usual: CMAKE_BUILD_TYPE (default 'Release'),
# CMAKE_CXX_COMPILER, CMAKE_TOOLCHAIN_FILE, CMAKE_INSTALL_PREFIX, FETCHCONTENT_BASE_DIR, and
# CMAKE_RUNTIME_OUTPUT_DIRECTORY / CMAKE_ARCHIVE_OUTPUT_DIRECTORY, which win over AREG_OUTPUT_DIR.
#
# Example:
# $ cmake -B ./build -DCMAKE_BUILD_TYPE=Debug -DAREG_COMPILER_FAMILY=llvm
# $ cmake --build ./build -j
#
# A project that sets these options includes 'user.cmake' before its first 'project()' call.
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

# A target processor without a compiler or a toolchain file selects the default compiler family of the host.
if (NOT "${AREG_ARCH}" STREQUAL "" AND "${CMAKE_CXX_COMPILER}${CMAKE_C_COMPILER}${AREG_COMPILER_FAMILY}${CMAKE_TOOLCHAIN_FILE}" STREQUAL "")
    if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
        set(AREG_COMPILER_FAMILY "gnu")
    elseif (CMAKE_HOST_APPLE)
        set(AREG_COMPILER_FAMILY "llvm")
    endif()
endif()

# The compiler target follows CMAKE_CXX_COMPILER_TARGET when the caller sets it.
set(AREG_TARGET "${CMAKE_CXX_COMPILER_TARGET}")

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

        # Set the relevant variables for the compiler
        set(AREG_COMPILER_FAMILY    "${_compiler_family}")
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

# The log observer API library 'areglogger' has the library type of 'areg'.
set(AREG_LOGGER_LIB_TYPE "${AREG_LIB_TYPE}")

# areg as the top-level project builds its tests and examples; areg inside another project does not.
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${AREG_SDK_ROOT}")
    set(_areg_top_level ON)
else()
    set(_areg_top_level OFF)
endif()

macro_create_option(AREG_TESTS          ${_areg_top_level} "Build unit tests")
macro_create_option(AREG_EXAMPLES       ${_areg_top_level} "Build examples")
macro_create_option(AREG_BENCHMARKS     OFF "Build benchmarks")
macro_create_option(AREG_LOGGING        ON  "Compile with logs")
macro_create_option(AREG_INSTALL        ON  "Enable installation")
# The sources use neither exceptions nor RTTI; required by MISRA C++ and AUTOSAR C++14 projects.
macro_create_option(AREG_NO_EXCEPTIONS  OFF "Compile without exceptions and RTTI (GNU/Clang only)")
macro_create_option(AREG_SYSTEM_SQLITE  ON  "Use SQLite3 installed package")
macro_create_option(AREG_SYSTEM_GTEST   ON  "Use GTest installed package")

unset(_areg_top_level)
