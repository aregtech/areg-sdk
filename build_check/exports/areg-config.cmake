
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################
include(CMakeFindDependencyMacro)

# ##################################################
# Set Areg SDK essentials
# ##################################################

# Set C and CXX compiler targets
if (NOT DEFINED CMAKE_CXX_COMPILER_TARGET OR "${CMAKE_CXX_COMPILER_TARGET}" STREQUAL "")
    set(CMAKE_CXX_COMPILER_TARGET x64)
endif()
if (NOT DEFINED CMAKE_C_COMPILER_TARGET OR "${CMAKE_C_COMPILER_TARGET}" STREQUAL "")
    set(CMAKE_C_COMPILER_TARGET   x64)
endif()

# Root folder of the SDK
set(AREG_SDK_ROOT           "C:/Program Files (x86)/areg-sdk")
# Location of 'areg', 'aregextend' and 'areglogger' header files
set(AREG_FRAMEWORK          "C:/Program Files (x86)/areg-sdk/include")
# Location of configuration, initialization and other shared files of Areg SDK
set(AREG_SHARE_DIR          "C:/Program Files (x86)/areg-sdk/share/areg")
# Location of initialization file to copy as a template in the processes
set(AREG_SHARE_INIT         "${AREG_SHARE_DIR}/areg.init")
# Location of cmake configuration directory
set(AREG_CMAKE_CONFIG_DIR   "${AREG_SHARE_DIR}/conf/cmake")
# Location of Areg SDK tools.
set(AREG_SDK_TOOLS          "C:/Program Files (x86)/areg-sdk/tools/areg")
# Location of the Areg SDK code generator
set(AREG_TOOL_CODEGEN       "${AREG_SDK_TOOLS}/codegen.jar")
# Full path to 'areg.cmake' to include in projects
set(AREG_CMAKE              "${AREG_SHARE_DIR}/areg.cmake")

# The library type of 'areg'. Static or dynamic
set(AREG_LIB_TYPE             shared)
# The library type of 'areglogger'. Static or dynamic.
set(AREG_LOGGER_LIB_TYPE      shared)

# set platform specific variables.
set(AREG_OS             Windows)
set(AREG_DEVELOP_ENV    Win32)
set(AREG_BITNESS        64)
set(AREG_ARCH      x86_64)

# target platforms
set(AREG_PLATFORM_LINUX     FALSE)
set(AREG_PLATFORM_MACOS     FALSE)
set(AREG_PLATFORM_WINDOWS   TRUE)
set(AREG_PLATFORM_POSIX     FALSE)

# Specify whether 'aregextend' is compiled with enabled extended objects flag
option(AREG_EXTENDED        "Areg Extended object flag"         OFF)
# Specify whether the libraries where compiled with enabled logging.
option(AREG_LOGGING            "Areg logging enable flag"          ON)
# Specify whether to use installed or compiled SQLite3 package
option(AREG_SYSTEM_SQLITE  "Use installed SQLite3 library"     ON)

# include the exported targets
include("${CMAKE_CURRENT_LIST_DIR}/areg-targets.cmake")
check_required_components("areg")

# Minimum C++ standard to use
set(AREG_CXX_STANDARD ${AREG_CXX_STANDARD})
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD  ${AREG_CXX_STANDARD})
elseif (${CMAKE_CXX_STANDARD} LESS ${AREG_CXX_STANDARD})
    message(WARNING "Areg: >>> Areg requires minimum C++${AREG_CXX_STANDARD}, current is C++${CMAKE_CXX_STANDARD}. Forcing to change to avoid compilation errors.")
    set(CMAKE_CXX_STANDARD  ${AREG_CXX_STANDARD})
endif()

# Unit tests are ignored
option(AREG_TESTS     "No Areg Unit Tests"    OFF)
# Examples are ignored
option(AREG_EXAMPLES  "No Areg Examples"      OFF)

# Change the build root directory if compiled fines are copied somewhere else.
# set to 'CMAKE_CURRENT_BINARY_DIR' if it should be in the compiled binaries directory
set(AREG_BUILD_DIR         "${CMAKE_SOURCE_DIR}/product")

# initialize SQLite3 library reference and type
set(AREG_SQLITE_LIB_REF areg::aregsqlite3)
set(AREG_SQLITE_LIB     aregsqlite3)
add_definitions(-DUSE_SQLITE_PACKAGE=0)
# Pass path to the 'areg.init' file as a preprocessor definition
add_definitions(-DAREG_SHARE_INIT="${AREG_SHARE_INIT}")

# Indicate the import flag of 'areg' library
if (AREG_LIB_TYPE MATCHES "shared")
    add_definitions(-DIMP_AREG_DLL)
else()
    add_definitions(-DIMP_AREG_LIB)
endif()

# Indicate the import flag of 'areg' library
if (AREG_LOGGER_LIB_TYPE MATCHES "shared")
    add_definitions(-DIMP_LOGGER_DLL)
else()
    add_definitions(-DIMP_LOGGER_LIB)
endif()

# Indicate the areg extended objects availability.
if (AREG_EXTENDED)
    add_definitions(-DAREG_EXTENDED=1)
else()
    add_definitions(-DAREG_EXTENDED=0)
endif()

# Indicate the areg logging availability.
if (AREG_LOGGING)
    add_definitions(-DAREG_LOGGING=1)
else()
    add_definitions(-DAREG_LOGGING=0)
endif()

# Set the Platform API and the platform bitness.
if (AREG_DEVELOP_ENV STREQUAL "Win32")
    add_definitions(-DWINDOWS -DWIN32)
    if (${AREG_BITNESS} EQUAL 64)
        add_definitions(-DWIN64)
    endif()
else()
    add_definitions(-DPOSIX)
endif()

if (AREG_BITNESS EQUAL 32)
    add_definitions(-DBIT32)
else()
    add_definitions(-DBIT64)
endif()

# Add '${AREG_FRAMEWORK}' path in the includes
include_directories(${AREG_FRAMEWORK})
