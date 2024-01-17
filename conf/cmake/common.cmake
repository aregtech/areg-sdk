# ###########################################################################
# Common settings for all projects
# ###########################################################################

if ("${AREG_COMPILER_FAMILY}" STREQUAL "")
    set(AREG_CXX_COMPILER "${CMAKE_CXX_COMPILER}")
    set(AREG_C_COMPILER   "${CMAKE_C_COMPILER}")
    findCompilerFamilyName("${CMAKE_CXX_COMPILER}" AREG_COMPILER_FAMILY)
    message(STATUS ">>> Using system default settings: Compiler family = \'${AREG_COMPILER_FAMILY}\', CXX compiler = \'${AREG_CXX_COMPILER}\', CC compiler = \'${AREG_C_COMPILER}\'")
endif()

findCompilerShortName("${CMAKE_CXX_COMPILER}" AREG_COMPILER_SHORT)

# Identify the OS
set(AREG_OS ${CMAKE_SYSTEM_NAME})
set(AREG_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})

# Determining bitness by size of void pointer
# 8 bytes ==> 64-bits (x64) and 4 bytes ==> 32-nit (x86)
if(NOT ${CMAKE_SIZEOF_VOID_P} MATCHES "8")
    set(AREG_BITNESS "32")
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
#set areg extended static library dependencies
set(AREG_EXTENDED_LIBS)

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

# Checking Compiler for adding corresponded tweaks and flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    message(STATUS ">>> Preparing compiler settings for `clang`")

    # POSIX API
    add_definitions(-DPOSIX)
    set(AREG_DEVELOP_ENV "Posix")

    if (CMAKE_BUILD_TYPE MATCHES "Release")
        list(APPEND AREG_COMPILER_OPTIONS -O2)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -O0 -g3)
    endif()

    if(AREG_BITNESS MATCHES "32")
        list(APPEND AREG_COMPILER_OPTIONS -m32)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -m64)
    endif()

    # Clang compile options
    list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -stdlib=libstdc++ ${AREG_USER_DEFINES})
    # Linker flags (-l is not necessary)
    list(APPEND AREG_LDFLAGS stdc++ m pthread rt)

elseif (CMAKE_COMPILER_IS_GNUCXX )

    message(STATUS ">>> Preparing compiler settings for `GNU`")

    # POSIX API
    add_definitions(-DPOSIX)
    set(AREG_DEVELOP_ENV "Posix")

    if (CMAKE_BUILD_TYPE MATCHES "Release")
        list(APPEND AREG_COMPILER_OPTIONS -O2)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -O0 -g3)
    endif()

    if(AREG_BITNESS MATCHES "32")
        list(APPEND AREG_COMPILER_OPTIONS -m32)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -m64)
    endif()

    # GNU compile options
    if (CYGWIN)
        message(STATUS ">>> CYGWIN is detected")
        list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -MMD -std=gnu++17 ${AREG_USER_DEFINES})
    else()
        list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -MMD -std=c++17 ${AREG_USER_DEFINES})
    endif()
    # Linker flags (-l is not necessary)
    list(APPEND AREG_LDFLAGS stdc++ m pthread rt)

elseif (MSVC)

    message(STATUS ">>> Preparing compiler settings for `MSVC`")

    # Visual Studio C++
    # Windows / Win32 API
    set(AREG_DEVELOP_ENV "Win32")
    add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32)

    if(NOT CMAKE_BUILD_TYPE MATCHES "Release")
        list(APPEND AREG_COMPILER_OPTIONS -Od -RTC1 -c)
    endif()
        
    if (${AREG_BITNESS} MATCHES "64")
        add_definitions(-DWIN64 -D_WIN64)
    endif()

    # MS Visual C++ compile options
    list(APPEND AREG_COMPILER_OPTIONS)
    # Linker flags (-l is not necessary)
    list(APPEND AREG_LDFLAGS advapi32 psapi shell32 ws2_32)

else()

    message(WARNING ">>> Unsupported compiler type. The result is unpredictable, by default use GNU compiler settings and POSIX API")
    # POSIX API
    add_definitions(-DPOSIX)
    set(AREG_DEVELOP_ENV "Posix")

    if (CMAKE_BUILD_TYPE MATCHES "Release")
        list(APPEND AREG_COMPILER_OPTIONS -O2)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -O0 -g3)
    endif()

    if(AREG_BITNESS MATCHES "32")
        list(APPEND AREG_COMPILER_OPTIONS -m32)
    else()
        list(APPEND AREG_COMPILER_OPTIONS -m64)
    endif()

    # Compile options
    list(APPEND AREG_COMPILER_OPTIONS -pthread -Wall -c -fmessage-length=0 -MMD -std=c++17 ${AREG_USER_DEFINES})
    # Linker flags (-l is not necessary)
    list(APPEND AREG_LDFLAGS stdc++ m pthread rt)

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
include_directories(BEFORE "${AREG_BASE}" "${AREG_GENERATE_DIR}")

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


message(STATUS "-------------------- CMakeLists Status Report Begin --------------------")
message(STATUS ">>> Build for '${CMAKE_SYSTEM_NAME}' '${AREG_BITNESS}'-bit platform '${AREG_PROCESSOR}' with compiler '${CMAKE_CXX_COMPILER}', ID '${AREG_COMPILER_FAMILY}', and build type '${CMAKE_BUILD_TYPE}'")
message(STATUS ">>> Binary output folder '${AREG_OUTPUT_BIN}', executable extensions '${CMAKE_EXECUTABLE_SUFFIX}'")
message(STATUS ">>> Generated files are in '${AREG_GENERATE_DIR}', library output folder '${AREG_OUTPUT_LIB}'")
message(STATUS ">>> Build examples is '${AREG_BUILD_EXAMPLES}', build tests is '${AREG_BUILD_TESTS}', AREG extended features are '${AREG_EXTENDED}', compile with logs '${AREG_LOGS}'")
message(STATUS "-------------------- CMakeLists Status Report End ----------------------")
