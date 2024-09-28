# ###########################################################################
# User specific settings
# Copyright 2022-2023 Aregtech
# ###########################################################################

# ---------------------------------------------------------------------------
# Pass the compiler options in command line or use the defaults:
#   1. AREG_COMPILER_FAMILY -- Simple way to set CMAKE_CXX_COMPILER and CMAKE_C_COMPILER compilers.
#   2. AREG_COMPILER        -- Specifies one compiler name to set for CXX and CC projects.
#   3. AREG_BINARY          -- Specifies the AREG Framework library type ('shared' or 'static'). By default it is 'shared'.
#   4. AREG_LOGOBSERVER_LIB -- Specifies the Log Observer API library type ('shared' or 'static'). By default it is 'shared'.
#   5. AREG_BUILD_TYPE      -- Build configurations ('Debug' or 'Release').
#   6. AREG_BUILD_TESTS     -- Flag to enable or disable AREG Framework unit tests.
#   7. AREG_BUILD_EXAMPLES  -- Flag to enable or disable AREG Framework examples.
#   8. AREG_EXTENDED        -- Flag to enable or disable to compile AREG SDK project with AREG Framework extended features. This may require additional dependencies.
#   9. AREG_LOGS            -- Flag to enable or disable compilation with logs. By default, it is enabled.
#  10. AREG_INSTALL         -- Flag to enable or disable AREG SDK installation. By default, it is enabled.
#  11. AREG_INSTALL_DEPENDS -- Flag to enable or disable AREG SDK to install thirdparty dependencies. The flag is ignored if AREG_INSTALL is 'OFF'. By default, it is disabled.
#  11. AREG_BUILD_ROOT      -- The root directory to build or create generated files. By default is the './product' directory of areg-sdk root.
#  12. AREG_OUTPUT_DIR      -- The output directory of build binaries.
#  13. AREG_OUTPUT_BIN      -- The output directory of build executables or runtime binaries (shared libraries).
#  14. AREG_OUTPUT_LIB      -- The output directory of static libraries.
#  15. AREG_PACKAGES        -- The location to install thirdparty packages.
#  16. AREG_INSTALL_PATH    -- The location to install AREG SDK binaries, headers, configuration files and tools. By default, it is created in the user home directory.
#
# The default values are:
#   1. AREG_COMPILER_FAMILY = <default> (possible values: gnu, cygwin, llvm, msvc)
#   2. AREG_COMPILER        = <default> (possible values: g++, gcc, c++, cc, clang++, clang, clang-cl, cl)
#   3. AREG_BINARY          = shared    (possible values: shared, static)
#   4. AREG_LOGOBSERVER_LIB = shared    (possible values: shared, static)
#   5. AREG_BUILD_TYPE      = Release   (possible values: Release, Debug)
#   6. AREG_BUILD_TESTS     = OFF       (possible values: ON, OFF)
#   7. AREG_BUILD_EXAMPLES  = ON        (possible values: ON, OFF)
#   8. AREG_EXTENDED        = OFF       (possible values: ON, OFF)
#   9. AREG_LOGS            = ON        (possible values: ON, OFF)
#  10. AREG_INSTALL         = ON        (possible values: ON, OFF)
#  11. AREG_INSTALL_DEPENDS = OFF       (possible values: ON, OFF)
#  11. AREG_BUILD_ROOT      = '<areg-sdk>/product'                                                              (possible values: any valid path for outputs for product and generated files)
#  12. AREG_OUTPUT_DIR      = '<areg-sdk>/product/build/gnu-gcc/<os>-<bitness>-<cpu>-release-<areg-lib>'        (possible values: any valid path for build outputs)
#  13. AREG_OUTPUT_BIN      = '<areg-sdk>/product/build/gnu-gcc/<os>-<bitness>-<cpu>-release-<areg-lib>/bin'    (possible values: any valid path for binary outputs)
#  14. AREG_OUTPUT_LIB      = '<areg-sdk>/product/build/gnu-gcc/<os>-<bitness>-<cpu>-release-<areg-lib>/lib'    (possible values: any valid path for library outputs)
#  15. AREG_PACKAGES        = '${AREG_BUILD_ROOT}/packages'                                                     (possible values: any valid path for package location)
#  16. AREG_INSTALL_PATH    = '${HOOME}/areg-sdk'; if '${HOOME}' is empty, use '${USERPROFILE}' or current dir. (possible values: any valid path to install AREG SDK outputs)   
#
# Hints:
#
#  Hint #1:
#   AREG_COMPILER_FAMILY is a simple and short way to specify the compiler.
#       - The value 'gnu' will set g++ and gcc compilers for C++ and C.
#       - The value 'cygwin' will set g++ and gcc compilers for C++ and C.
#       - The value 'llvm' will set clang++,  clang and clang-cl (under windows) compilers for C++ and C.
#       - The value 'msvc' will set Microsoft Visual C++ compiler for C++ and C.
#
# Example:
# $ cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TYPE=Release -DAREG_BUILD_TESTS=ON -DAREG_BUILD_EXAMPLES=ON
# 
# NOTE: if in command line specify AREG_CXX_COMPILER, the AREG_C_COMPILER
#       must be specified as well. The both options must be specified
#       either together or none should be specified to use defaults.
#
#  Hint #2:
#   To integrate the AREG SDK in any other project, it might be enough to specify
#   the AREG_BUILD_ROOT, which should indicate the root path of the project.
#   In this case, if the areg output paths are not directly pointed, then
#   they are build relative to the AREG_BUILD_ROOT variable.
#
# Example:
# $ cmake -B ./build -D=AREG_BUILD_ROOT="~/projects/my_project/product"
# ---------------------------------------------------------------------------

# CPP compiler, possible values: g++, gcc, c++, cc, clang++, clang, clang-cl, cl
set(AREG_CXX_COMPILER)

# C compiler, possible values: gcc, clang, clang-cl, cl
set(AREG_C_COMPILER)

# Check the compiler option.
# Check and set the simple and shortest way to set the compiler
if(DEFINED AREG_COMPILER_FAMILY AND NOT ${AREG_COMPILER_FAMILY} STREQUAL "")

    message(STATUS ">>> User selected C/C++ compiler family \'${AREG_COMPILER_FAMILY}\'")
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
        message(WARNING ">>> Unrecognized compiler family ${AREG_COMPILER_FAMILY}, supported values: \'gnu\', \'llvm\', \'cygwin\', \'msvc\''")
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
        message(WARNING ">>> Unrecognized compiler ${AREG_COMPILER}, supported compilers: \'gcc\', \'g++\', \'clang\', \'clang++\', \'clang-cl\', \'cl\'")
    endif()

else()

    message(STATUS ">>> No compiler is selected, will use system default")

endif()

# Set build configuration. Set "Debug" for debug build, and "Release" for release build.
if (NOT DEFINED AREG_BUILD_TYPE OR NOT ${AREG_BUILD_TYPE} STREQUAL "Debug")
    if (DEFINED CMAKE_BUILD_TYPE AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
        set(AREG_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
    else()
        set(AREG_BUILD_TYPE "Release")
    endif()
endif()

# Set the AREG binary library type to compile. Set "shared" if not "static"
if (NOT DEFINED AREG_BINARY OR NOT ${AREG_BINARY} STREQUAL "static")
    set(AREG_BINARY "shared")
endif()

# Build tests. By default it is disabled. To enable, set ON
if (NOT DEFINED AREG_BUILD_TESTS)
    option(AREG_BUILD_TESTS     "Build unit tests" OFF)
endif()

# Build examples. By default it is disabled. To enable, set ON
if (NOT DEFINED AREG_BUILD_EXAMPLES)
    option(AREG_BUILD_EXAMPLES  "Build examples"   ON)
endif()

# Set AREG extended features enable or disable flag to compiler additional optional features. By default, it is disabled.
if (NOT DEFINED AREG_EXTENDED)
    option(AREG_EXTENDED      "Enable extensions" OFF)
endif()

# Modify 'AREG_LOGS' to enable or disable compilation with logs. By default, compile with logs
if (NOT DEFINED AREG_LOGS)
    option(AREG_LOGS "Compile with logs" ON)
endif()

# Modify 'AREG_INSTALL' to enable or disable installation of AREG SDK
if (NOT DEFINED AREG_INSTALL)
    option(AREG_INSTALL "Enable installation" ON)
endif()

if (NOT DEFINED AREG_INSTALL_DEPENDS)
    option(AREG_INSTALL_DEPENDS "Disable installation of thirdparty dependencies" OFF)
endif()

# Set the areg-sdk build root folder to output files.
if (NOT DEFINED AREG_BUILD_ROOT OR "${AREG_BUILD_ROOT}" STREQUAL "")
    set(AREG_BUILD_ROOT "${AREG_SDK_ROOT}/product")
endif()

# Set the areg log observer API library type.
if (NOT DEFINED AREG_LOGOBSERVER_LIB OR NOT "${AREG_LOGOBSERVER_LIB}" STREQUAL "static")
    set(AREG_LOGOBSERVER_LIB "shared")
endif()

if (NOT DEFINED AREG_PACKAGES OR "${AREG_PACKAGES}" STREQUAL "")
    set(AREG_PACKAGES "${AREG_BUILD_ROOT}/packages")
endif()

if (NOT DEFINED AREG_INSTALL_PATH OR "${AREG_INSTALL_PATH}" STREQUAL "")
    set(INST_PATH "${AREG_BUILD_ROOT}")
    set(TEMP1_VAL "$ENV{HOME}")

    if (NOT "${TEMP1_VAL}" STREQUAL "")
        set(INST_PATH ${TEMP1_VAL})
    else()
        set(TEMP1_VAL "$ENV{USERPROFILE}")
        if (NOT "${TEMP1_VAL}" STREQUAL "")
            set(INST_PATH ${TEMP1_VAL})
        endif()
    endif()

    set(AREG_INSTALL_PATH "${INST_PATH}/areg-sdk")
endif()

# The absolute path for generated files
set(AREG_GENERATE "generate")
set(AREG_GENERATE_DIR "${AREG_BUILD_ROOT}/${AREG_GENERATE}")

# CPP standard for the projects
set(AREG_CXX_STANDARD 17)

# Specify default bitness, the system bitness is detected in 'common.cmake'
set(AREG_BITNESS "64")

# Specify CPU platform here, the system CPU platform is detected in 'commmon.cmake'
set(AREG_PROCESSOR "x86_64")
