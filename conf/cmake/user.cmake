# ###########################################################################
# User sepcific settings
# ###########################################################################

# ###########################################################################
# Pass the compiler options in command line or use the defaults:
#   1. AREG_CXX_COMPILER    -- C++ compiler, full or relative path
#   2. AREG_C_COMPILER      -- C compiler, full or relative path
#   2. AREG_BUILD_TYPE      -- build configurations ('Debug' or 'Release')
#   3. AREG_BINARY          -- AREG engine binary type ('shared' or 'static')
# 
# NOTE: if in command line specify AREG_CXX_COMPILER, the AREG_C_COMPILER
#       must be specified as well. The both options must be specified
#       either together or none should be specified to use defaults.
# ###########################################################################

# If any compiler is set, both C and C++ should be specified.
# Otherwise, either none should exist or outputs error
if (DEFINED AREG_CXX_COMPILER AND NOT DEFINED AREG_C_COMPILER)
    message(FATAL_ERROR "You should specify both AREG_CXX_COMPILER and AREG_C_COMPILER variables in command line")
endif()
if (DEFINED AREG_C_COMPILER AND NOT DEFINED AREG_CXX_COMPILER)
    message(FATAL_ERROR "You should specify both AREG_CXX_COMPILER and AREG_C_COMPILER variables in command line")
endif()


# Check the compiler option.
# Set the compiler here. Can set compiler full path.
# Set the C++ and C compiler type either automatically or manual in command line
# by specifying AREG_CXX_COMPILER and AREG_C_COMPILER
if (NOT DEFINED AREG_CXX_COMPILER OR AREG_CXX_COMPILER STREQUAL "")
    # Possible settings: "gcc", "g++", "cl", "clang++-13"
    set(AREG_CXX_COMPILER "g++")
else()
    message(STATUS ">>> User selected C++ compiler ${AREG_CXX_COMPILER}")
endif()

if (NOT DEFINED AREG_C_COMPILER OR AREG_C_COMPILER STREQUAL "")
    # Possible settings: "gcc", "cl", "clang-13"
    set(AREG_C_COMPILER "gcc")
else()
    message(STATUS ">>> User selected C compiler ${AREG_C_COMPILER}")
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

# CPP standard for the projects
set(AREG_CXX_STANDARD 17)

if(${AREG_BUILD_TYPE} MATCHES "Debug")
    option(AREG_BUILD_TESTS    "Build unit tests" ON)
    option(AREG_BUILD_EXAMPLES "Build examples"   ON)
else()
    option(AREG_BUILD_TESTS    "Build unit tests" ON)
    option(AREG_BUILD_EXAMPLES "Build examples"   ON)
endif()

# Set bitless here
set(Bitness "64")

# Specify CPU platform here
set(AREG_PLATFORM "x86_64")

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
