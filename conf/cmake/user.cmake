# ###########################################################################
# User specific settings
# ###########################################################################

# ---------------------------------------------------------------------------
# Pass the compiler options in command line or use the defaults:
#   1. AREG_COMPILER_FAMILY -- Simple way to set CMAKE_CXX_COMPILER and CMAKE_C_COMPILER compilers
#   2. AREG_COMPILER        -- Specifies one compiler to set for CXX and CC compilers
#   3. AREG_BINARY          -- AREG engine binary type ('shared' or 'static')
#   4. AREG_BUILD_TYPE      -- build configurations ('Debug' or 'Release')
#   5. AREG_BUILD_TESTS     -- Build AREG engine unit tests
#   6. AREG_BUILD_EXAMPLES  -- Build AREG engine examples
#   7. AREG_BUILD_ALL       -- Builds the framework, examples and tests.
#   8. AREG_EXTENSIONS      -- Enables or disables the extensions, which might require additional libraries.
#   9. AREG_LOGS            -- Enable or disable compilation with logs.
#  10. AREG_OUTPUT_DIR      -- The output directory of build binaries
#  11. AREG_OUTPUT_BIN      -- Set the path to folder to output compiled shared libraries and executables.
#  12. AREG_OUTPUT_LIB      -- Set the path to folder to output compiled static libraries.
#
# The default values are:
#   1. AREG_COMPILER_FAMILY := gnu      (possible values: gnu, cygwin, llvm, msvc)
#   2. AREG_COMPILER        := g++      (possible values: g++, gcc, clang++, clang, cl)
#   3. AREG_BINARY          := shared   (possible values: shared, static)
#   4. AREG_BUILD_TYPE      := Release  (possible values: Release, Debug
#   5. AREG_BUILD_TESTS     := ON       (possible values: ON, OFF)
#   6. AREG_BUILD_EXAMPLES  := ON       (possible values: ON, OFF)
#   7. AREG_BUILD_ALL       := ON       (possible values: ON, OFF)
#   8. AREG_EXTENSIONS      := ON       (possible values: ON, OFF)
#   9. AREG_LOGS            := ON       (possible values: ON, OFF)
#   9. AREG_OUTPUT_DIR      := <areg-sdk>/product/build/gnu-gcc/<os>-<bitness>-<cpu>-release      (possible values: any full path)
#  10. AREG_OUTPUT_BIN      := <areg-sdk>/product/build/gnu-gcc/<os>-<bitness>-<cpu>-release/bin  (possible values: any full path)
#  11. AREG_OUTPUT_LIB      := <areg-sdk>/product/build/gnu-gcc/<os>-<bitness>-<cpu>-release/lib  (possible values: any full path)
#
# Hint:
#   AREG_COMPILER_FAMILY is a simple and short way to specify the compiler.
#       - The value 'gnu' will set g++ and gcc compilers for C++ and C.
#       - The value 'cygwin' will set g++ and gcc compilers for C++ and C.
#       - The value 'llvm' will set clang++ and clang compilers for C++ and C.
#       - The value 'msvc' will set Microsoft Visual C++ compiler for C++ and C.
#
# Example:
# $ cmake -B build -DAREG_COMPILER_FAMILY=clang -DAREG_BINARY=Release -DAREG_BUILD_TESTS:BOOL=ON -DAREG_BUILD_EXAMPLES:BOOL=OFF
# 
# NOTE: if in command line specify AREG_CXX_COMPILER, the AREG_C_COMPILER
#       must be specified as well. The both options must be specified
#       either together or none should be specified to use defaults.
# ---------------------------------------------------------------------------

# CPP compiler, possible values: g++, gcc, clang++, clang, cl
set(AREG_CXX_COMPILER)

# C compiler, possible values: gcc, clang, cl
set(AREG_C_COMPILER)

# Check the compiler option.
# Check and set the simple and shortest way to set the compiler
if (DEFINED AREG_COMPILER_FAMILY AND NOT ${AREG_COMPILER_FAMILY} STREQUAL "")
    message(STATUS ">>> User selected C/C++ compiler family ${AREG_COMPILER_FAMILY}")
    if (${AREG_COMPILER_FAMILY} STREQUAL "gnu")
        set(AREG_CXX_COMPILER "g++")
        set(AREG_C_COMPILER   "gcc")
    elseif (${AREG_COMPILER_FAMILY} STREQUAL "cygwin")
        set(AREG_CXX_COMPILER "g++")
        set(AREG_C_COMPILER   "gcc")
    elseif(${AREG_COMPILER_FAMILY} STREQUAL "llvm")
        set(AREG_CXX_COMPILER "clang++")
        set(AREG_C_COMPILER   "clang")
    elseif(${AREG_COMPILER_FAMILY} STREQUAL "msvc")
        set(AREG_CXX_COMPILER "cl")
        set(AREG_C_COMPILER   "cl")
    else()
        message(WARNING ">>> Unrecognized compiler family ${AREG_COMPILER_FAMILY}, supported: \'gnu\', \'llvm\', \'cygwin\', \'msvc\', set for \'gnu\'")
        set(AREG_CXX_COMPILER       "g++")
        set(AREG_C_COMPILER         "gcc")
    endif()

elseif(DEFINED AREG_COMPILER AND NOT AREG_COMPILER STREQUAL "")

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

    elseif (${AREG_COMPILER} STREQUAL "clang++" OR ${AREG_COMPILER} STREQUAL "clang")
        # Clang compiler

        # Make sure that C-compiler is properly set
        if (${AREG_COMPILER} STREQUAL "clang++")
            set(AREG_C_COMPILER     "clang")
        endif()

        set(AREG_COMPILER_FAMILY    "llvm")

    elseif (${AREG_COMPILER} STREQUAL "cl")
        set(AREG_COMPILER_FAMILY    "msvc")
    else()
        message(WARNING ">>> Unrecognized compiler ${AREG_COMPILER} is selected")
        set(AREG_COMPILER_FAMILY    "Unknown")
    endif()

else()

    # Possible values: "gnu", "llvm", "msvc", "cygwin"
    set(AREG_COMPILER_FAMILY    "gnu")
    
    set(AREG_CXX_COMPILER       "g++")
    set(AREG_C_COMPILER         "gcc")

    message(STATUS ">>> Compile using default settings: Compiler family = \'${AREG_COMPILER_FAMILY}\', CXX compiler = \'${AREG_CXX_COMPILER}\', CC compiler = \'${AREG_C_COMPILER}\'")

endif()

# Set build configuration here.
# Set the build configuration either automatically or manually in command line by specifying AREG_BUILD_TYPE
if (NOT DEFINED AREG_BUILD_TYPE OR AREG_BUILD_TYPE STREQUAL "")
    # Set "Debug" for debug and "Release" for release build
    # set(AREG_BUILD_TYPE "Release")
    set(AREG_BUILD_TYPE "Release")
endif()

# Set the AREG binary library type to compile.
# Set the library type either automatically or manually in command line
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
if (NOT DEFINED AREG_EXTENSIONS)
    option(AREG_EXTENSIONS      "Enable extensions" ON)
endif()

# Modify 'AREG_LOGS' to enable or disable compilation with logs.
# By default, the applications are compiled with logs.
if (NOT DEFINED AREG_LOGS)
    option(AREG_LOGS "Compile with logs" ON)
endif()

# CPP standard for the projects
set(AREG_CXX_STANDARD 17)

# Set bitness here
set(AREG_BITNESS "64")

# Specify CPU platform here
set(AREG_PROCESSOR "x86_64")

# Set the areg-sdk build root folder to output files.
set(AREG_BUILD_ROOT "${AREG_SDK_ROOT}")

# Set user specific root folder here
set(AREG_USER_PRODUCTS "product")
