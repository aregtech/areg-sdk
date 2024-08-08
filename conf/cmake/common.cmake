# ###########################################################################
# Common settings for all projects
# ###########################################################################

if ("${AREG_COMPILER_FAMILY}" STREQUAL "")
    set(AREG_CXX_COMPILER "${CMAKE_CXX_COMPILER}")
    set(AREG_C_COMPILER   "${CMAKE_C_COMPILER}")
    findCompilerFamilyName("${CMAKE_CXX_COMPILER}" AREG_COMPILER_FAMILY)
    message(STATUS ">>> Using system default settings: Compiler family = \'${AREG_COMPILER_FAMILY}\', CXX compiler = \'${AREG_CXX_COMPILER}\', CC compiler = \'${AREG_C_COMPILER}\'")
endif()

# Identify compiler short name
findCompilerShortName("${CMAKE_CXX_COMPILER}" AREG_COMPILER_SHORT)

# Identify the OS
set(AREG_OS ${CMAKE_SYSTEM_NAME})
# Identify CPU platform
set(AREG_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})

# Detect and set bitness here
# 8 bytes ==> 64-bits (x64) and 4 bytes ==> 32-nit (x86)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(AREG_BITNESS "64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(AREG_BITNESS "32")
else()
    message(WARNING " >>< Undefined Bitness, use default!")
endif()

# -----------------------------------------------------
# areg specific internal variable settings
# -----------------------------------------------------
# The toolchain
set(AREG_CXX_TOOLCHAIN "${CMAKE_CXX_COMPILER}")
set(AREG_CC_TOOLCHAIN  "${CMAKE_CC_COMPILER}")

# The development environment -- POSIX or Win32 API
set(AREG_DEVELOP_ENV)
# The linker flags
set(AREG_LDFLAGS)
# The compiler options
set(AREG_COMPILER_OPTIONS)
# set areg extended static library dependencies
set(AREG_EXTENDED_LIBS)
# set areg compiler version
set(AREG_COMPILER_VERSION)
set(AREG_TARGET_COMPILER_OPTIONS)

# Adding common definition
add_definitions(-DUNICODE -D_UNICODE)
remove_definitions(-D_MBCS -DMBCS)

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DNDEBUG)
    remove_definitions(-DDEBUG -D_DEBUG)
else()
    add_definitions(-DDEBUG)
    remove_definitions(-DNDEBUG -D_NDEBUG)
endif()

set(AREG_OPT_DISABLE_WARN_COMMON)
set(AREG_OPT_DISABLE_WARN_FRAMEWORK)
set(AREG_OPT_DISABLE_WARN_TOOLS)
set(AREG_OPT_DISABLE_WARN_EXAMPLES)
set(AREG_OPT_DISABLE_WARN_CODEGEN)
set(AREG_OPT_DISABLE_WARN_THIRDPARTY)

# Checking Compiler for adding corresponded tweaks and flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    include(${AREG_CMAKE_CONFIG_DIR}/clang.cmake)

elseif (CMAKE_COMPILER_IS_GNUCXX )

    include(${AREG_CMAKE_CONFIG_DIR}/gnu.cmake)

elseif (MSVC)

    include(${AREG_CMAKE_CONFIG_DIR}/msvc.cmake)

else()

    message(WARNING ">>> Unsupported compiler type. The result is unpredictable, by default use GNU compiler settings and POSIX API")
    include(${AREG_CMAKE_CONFIG_DIR}/gnu.cmake)

endif()

if (AREG_EXTENDED)
    add_definitions(-DAREG_EXTENDED=1)
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        list(APPEND AREG_EXTENDED_LIBS ncurses)
    endif()
else()
    add_definitions(-DAREG_EXTENDED=0)
endif()

if (AREG_LOGS)
    add_definitions(-DAREG_LOGS=1)
else()
    add_definitions(-DAREG_LOGS=0)
endif()


# -------------------------------------------------------
# Setup product paths
# -------------------------------------------------------

# The output directory
if (NOT DEFINED AREG_OUTPUT_DIR OR "${AREG_OUTPUT_DIR}" STREQUAL "")
    # Relative path of the output folder for the builds
    set(AREG_PRODUCT_PATH "build/${AREG_COMPILER_FAMILY}-${AREG_COMPILER_SHORT}/${AREG_OS}-${AREG_BITNESS}-${AREG_PROCESSOR}-${CMAKE_BUILD_TYPE}-${AREG_BINARY}")
    string(TOLOWER "${AREG_PRODUCT_PATH}" AREG_PRODUCT_PATH)
    # The absolute path of 'AREG_OUTPUT_DIR' for builds if it is not set.
    set(AREG_OUTPUT_DIR "${AREG_BUILD_ROOT}/${AREG_PRODUCT_PATH}")
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
include_directories(BEFORE "${AREG_BASE}" "${AREG_BUILD_ROOT}" "${AREG_GENERATE_DIR}" "${AREG_THIRDPARTY}")

# Adding library search paths
link_directories(BEFORE "${AREG_OUTPUT_BIN}" "${AREG_OUTPUT_LIB}")

# Only for Linux
if(UNIX AND NOT CYGWIN)
    set(CMAKE_EXECUTABLE_SUFFIX ".out")
endif()

set(COMMON_COMPILE_DEF)
if(AREG_BINARY MATCHES "static")
    set(COMMON_COMPILE_DEF IMP_AREG_LIB)
else()
    set(COMMON_COMPILE_DEF IMP_AREG_DLL)
endif()

set(FETCHCONTENT_BASE_DIR "${AREG_PACKAGES}" CACHE PATH "Location of AREG thirdparty packages" FORCE)

find_package(Java COMPONENTS Runtime)
if (NOT ${Java_FOUND})
    find_package(Java COMPONENTS Development)
endif()

message(STATUS "-------------------- CMakeLists Status Report Begin --------------------")
message(STATUS ">>> Build '${CMAKE_SYSTEM_NAME}' '${AREG_BITNESS}'-bit platform '${AREG_PROCESSOR}', compiler '${CMAKE_CXX_COMPILER}', ID '${AREG_COMPILER_FAMILY}', build type '${CMAKE_BUILD_TYPE}'")
message(STATUS ">>> Binary output folder '${AREG_OUTPUT_BIN}', executable extensions '${CMAKE_EXECUTABLE_SUFFIX}'")
message(STATUS ">>> Generated files location '${AREG_GENERATE_DIR}', library output folder '${AREG_OUTPUT_LIB}'")
message(STATUS ">>> Build examples is '${AREG_BUILD_EXAMPLES}', build tests is '${AREG_BUILD_TESTS}', AREG extended features are '${AREG_EXTENDED}', compile with logs '${AREG_LOGS}'")
message(STATUS ">>> Java ${Java_VERSION_STRING} at location ${Java_JAVA_EXECUTABLE} is required by code generator. Minimum version 17")
message(STATUS "-------------------- CMakeLists Status Report End ----------------------")
message(STATUS CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR})