# ###########################################################################
# User sepcific settings
# ###########################################################################

# ---------------------------------------------------------------------------
# Pass the compiler options in command line or use the defaults:
#   1. AREG_CXX_COMPILER    -- C++ compiler, full or relative path
#   2. AREG_C_COMPILER      -- C compiler, full or relative path
#   3. AREG_BUILD_TYPE      -- build configurations ('Debug' or 'Release')
#   4. AREG_BINARY          -- AREG engine binary type ('shared' or 'static')
#   5. AREG_BUILD_TESTS     -- Build AREG engine unit tests
#   6. AREG_BUILD_EXAMPLES  -- Build AREG engine examples
#   7. AREG_COMPILER_FAMILY -- Simple way to set AREG_CXX_COMPILER and AREG_C_COMPILER compilers
#   8. AREG_COMPILER        -- Specifies one compiler to set for CXX and CC compilers
#   8. AREG_BUILD_ALL       -- Bulds the framework, examples and tests.
#   9. AREG_OUTPUT_DIR      -- The output directory of build binaries
#  10. AREG_ENABLE_EXT      -- Enables or disables the extensions, which might require additional libraries.
#
# The default values are:
#   1. AREG_CXX_COMPILER    := g++      (possible values: g++, clang++-13, cl)
#   2. AREG_C_COMPILER      := gcc      (possible values: gcc, clang-13, cl)
#   3. AREG_BUILD_TYPE      := Release  (possible values: Release, Debug
#   4. AREG_BINARY          := shared   (possible values: shared, static)
#   5. AREG_BUILD_TESTS     := ON       (possible values: ON, OFF)
#   6. AREG_BUILD_EXAMPLES  := ON       (possible values: ON, OFF)
#   7. AREG_COMPILER_FAMILY := <value>  (possible values: gnu, cygwin, clang, msvc)
#   8. AREG_COMPILER        := <value>  (possible values: g++, gcc, clang++, clang, cl)
#   9. AREG_BUILD_ALL       := ON       (possible values: ON, OFF)
#  10. AREG_OUTPUT_DIR      := <path>   (possible values: valid path where creates 'bin' and 'lib' subfolders)
#  11. AREG_ENABLE_EXT      := ON       (possible values: ON, OFF)
#
# Hint:
#   AREG_COMPILER_FAMILY is a simple and short way to specify the compiler.
#       - The value 'gnu' will set g++ and gcc compilers for C++ and C.
#       - The value 'cygwin' will set g++ and gcc compilers for C++ and C.
#       - The value 'clang' will set clang++-13 and clang-13 compilers for C++ and C.
#       - The value 'msvc' will set Microsoft Visual C++ compiler for C++ and C.
#
# Example:
# $ cmake -B build -DAREG_COMPILER_FAMILY=clang -DAREG_BINARY=Release -DAREG_BUILD_TESTS:BOOL=ON -DAREG_BUILD_EXAMPLES:BOOL=PFF
# 
# NOTE: if in command line specify AREG_CXX_COMPILER, the AREG_C_COMPILER
#       must be specified as well. The both options must be specified
#       either together or none should be specified to use defaults.
# ---------------------------------------------------------------------------

# If any compiler is set, both C and C++ should be specified.
# Otherwise, either none should exist or outputs error
if ((DEFINED AREG_CXX_COMPILER AND NOT DEFINED AREG_C_COMPILER) OR (DEFINED AREG_C_COMPILER AND NOT DEFINED AREG_CXX_COMPILER))
    message(FATAL_ERROR "You should specify both AREG_CXX_COMPILER and AREG_C_COMPILER variables in command line")
endif()

# Check the compiler option.
# Set the compiler here. Can set compiler full path.
# Set the C++ and C compiler type either automatically or manual in command line
# by specifying AREG_CXX_COMPILER and AREG_C_COMPILER
if (NOT DEFINED AREG_CXX_COMPILER OR AREG_CXX_COMPILER STREQUAL "")
    # Possible settings: "gcc", "g++", "cl", "clang++"
    set(AREG_CXX_COMPILER "g++")
else()
    message(STATUS ">>> User selected C++ compiler ${AREG_CXX_COMPILER}")
endif()

if (NOT DEFINED AREG_C_COMPILER OR AREG_C_COMPILER STREQUAL "")
    # Possible settings: "gcc", "cl", "clang"
    set(AREG_C_COMPILER "gcc")
else()
    message(STATUS ">>> User selected C compiler ${AREG_C_COMPILER}")
endif()

# Check and set the simple and shortest way to set the compiler
if (DEFINED AREG_COMPILER_FAMILY)
    message(STATUS ">>> User selected C/C++ compiler family ${AREG_COMPILER_FAMILY}")
    if (${AREG_COMPILER_FAMILY} STREQUAL "gnu")
        set(AREG_CXX_COMPILER "g++")
        set(AREG_C_COMPILER   "gcc")
    elseif (${AREG_COMPILER_FAMILY} STREQUAL "cygwin")
        set(AREG_CXX_COMPILER "g++")
        set(AREG_C_COMPILER   "gcc")
    elseif(${AREG_COMPILER_FAMILY} STREQUAL "clang")
        set(AREG_CXX_COMPILER "clang++")
        set(AREG_C_COMPILER   "clang")
    elseif(${AREG_COMPILER_FAMILY} STREQUAL "msvc")
        set(AREG_CXX_COMPILER "cl")
        set(AREG_C_COMPILER   "cl")
    elseif(NOT ${AREG_COMPILER_FAMILY} STREQUAL "")
        message(WARNING ">>> Unrecognized compiler family ${AREG_COMPILER_FAMILY}, supported value are: \'gnu\', \'clang\', \'cygwin\', \'msvc\', setting default GNU compilers")
        set(AREG_CXX_COMPILER "g++")
        set(AREG_C_COMPILER   "gcc")
    endif()

elseif(DEFINED AREG_COMPILER AND NOT AREG_COMPILER STREQUAL "")

    set(AREG_CXX_COMPILER, ${AREG_COMPILER})
    set(AREG_C_COMPILER, ${AREG_COMPILER})

    if (${AREG_COMPILER} STREQUAL "g++" OR ${AREG_COMPILER} STREQUAL "gcc")
        if (CYGWIN)
            set(AREG_COMPILER_FAMILY, "cygwin")
        else()
            set(AREG_COMPILER_FAMILY, "gnu")
        endif()
    elseif (${AREG_COMPILER} STREQUAL "clang++" OR ${AREG_COMPILER} STREQUAL "clang")
        set(AREG_COMPILER_FAMILY, "gnu")
    elseif (${AREG_COMPILER} STREQUAL "cl")
        set(AREG_COMPILER_FAMILY, "msvc")
    else()
        message(WARNING ">>> Unrecognized compiler ${AREG_COMPILER} is selected")
        set(AREG_COMPILER_FAMILY, "Unknown")
    endif()

endif()

# Set build configuration here.
# Set the build configuration either automatically or manually in command line by specifying AREG_BUILD_TYPE
if (NOT DEFINED AREG_BUILD_TYPE OR AREG_BUILD_TYPE STREQUAL "")
    # Set "Debug" for debug and "Release" for release build
    # set(AREG_BUILD_TYPE "Release")
    set(AREG_BUILD_TYPE "Release")
endif()

# Set the AREG binary library type to compile.
# Set the library type either anutmatically or manually in command line
if (NOT DEFINED AREG_BINARY OR AREG_BINARY STREQUAL "")
    # Set "static" for static and "shared" for shared library
    set(AREG_BINARY "shared")
endif()

# Build tests. By default it is enabled. To disable, set OFF
if (NOT DEFINED AREG_BUILD_TESTS OR AREG_BUILD_TESTS)
    option(AREG_BUILD_TESTS     "Build unit tests" ON)
else()
    option(AREG_BUILD_TESTS     "Build unit tests" OFF)
endif()

# Build examples. By default it is enabled. To disable, set OFF
if (NOT DEFINED AREG_BUILD_EXAMPLES OR AREG_BUILD_EXAMPLES)
    option(AREG_BUILD_EXAMPLES  "Build examples"   ON)
else()
    option(AREG_BUILD_EXAMPLES  "Build examples"   OFF)
endif()

# Check and set the builds projects. If ON, force to compile examples and tests
if (DEFINED AREG_BUILD_ALL)
    option(AREG_BUILD_TESTS     "Build unit tests" ${AREG_BUILD_ALL})
    option(AREG_BUILD_EXAMPLES  "Build examples"   ${AREG_BUILD_ALL})
endif()

# Set AREG extensions enable or disable flag to compiler additional optional features.
if (NOT DEFINED AREG_ENABLE_EXT)
    option(AREG_ENABLE_EXT      "Enable extensions" OFF)
endif()

# CPP standard for the projects
set(AREG_CXX_STANDARD 17)

# Set bitless here
set(AREG_BITNESS "64")

# Specify CPU platform here
set(AREG_PROCESSOR "x86_64")

# Set the areg-sdk build root folder to output files.
set(AREG_BUILD_ROOT "${AREG_SDK_ROOT}")

# Set user or project specific defines here or keep empty.
# By default it is compile with enabled logging --> "-DENABLE_TRACES"
set(AREG_USER_DEFINES "-DENABLE_TRACES=1")

# Set user specific includes here or keep empty
set(AREG_USER_DEF_INCLUDES)

# Set user specific library paths here or keep empty
set(AREG_USER_DEF_LIB_PATHS)

# Set user specific library list here or keep empty
set(AREG_USER_DEF_LIBS)

# Set user specific root folder here
set(AREG_USER_DEF_OUTPUT_DIR "product")
