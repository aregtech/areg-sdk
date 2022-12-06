# Identify the OS
if(APPLE)
    set(OpSystem "macOS")
elseif(UNIX)
    set(OpSystem "linux")
else()
    set(OpSystem "windows")
endif()

# Determining bitness by size of void pointer
# 8 bytes ==> x64 and 4 bytes ==> x86
if(NOT ${CMAKE_SIZEOF_VOID_P} MATCHES "8")
    set(Platform "x86")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_BUILD_TYPE "Release")
    add_definitions(-DNDEBUG)
else()
    set(CMAKE_BUILD_TYPE "Debug")
    add_definitions(-DDEBUG)
endif()

# The toolset
set(Toolset "${CMAKE_CXX_COMPILER_ID}")

# Relative path of the output folder for the builds
set(ProjBuildPath "build/${CrossCompile}${Toolset}/${OpSystem}-${Platform}-${Config}")
# The absolute path for builds
set(ProjOutputDir "${AregBuildRoot}/${UserDefOutput}/${ProjBuildPath}")

message(STATUS ">>> Build for \'${CMAKE_SYSTEM_NAME}\' with compiler \'${CMAKE_CXX_COMPILER}\', ID \'${CMAKE_CXX_COMPILER_ID}\'")
message(STATUS ">>> Build output folder \'${ProjOutputDir}\'")

# The absolute path for generated files
set(ProjGenDir "${AregBuildRoot}/${UserDefOutput}/generate")

# The absolute path for obj files.
set(ProjObjDir "${ProjOutputDir}/obj")

# The absolute path for static libraries
set(ProjLibDir "${ProjOutputDir}/lib")

# The absolute path for all executables and shared libraries
set(ProjBinDir "${ProjOutputDir}/bin")

# Project inclues
set(ProjIncludes "${ProjIncludes} -I${AREG_BASE} -I${ProjGenDir} -I${UserDefIncludes}")

set(AREG_OUTPUT_OBJ  "${ProjObjDir}")
set(AREG_OUTPUT_LIB  "${ProjLibDir}")
set(AREG_OUTPUT_BIN  "${ProjBinDir}")
set(AREG_INCLUDES    "${ProjIncludes}")
set(AREG_TOOLCHAIN   "${CrossCompile}${Toolset}")
set(AREG_AR          "${CrossCompile}ar")
set(AREG_OS          "${OpSystem}")
set(AREG_DEVELOP_ENV)
set(AREG_STATIC_LIB)
set(AREG_EXAMPLES_LDFLAGS)

if(areg MATCHES "shared")
    set(AREG_BINARY "shared")
else()
    set(AREG_BINARY "static")
endif()

# CPP standard for the project
set(aregCXXStandard "17")

add_definitions(-DUNICODE)

# Checking Compiler for adding corresponded tweaks and flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

    # POSIX API
    add_definitions(-DPOSIX)
    set(AREG_DEVELOP_ENV "Posix")
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        list(APPEND CompileOptions -O2)
    else()
        list(APPEND CompileOptions -O0 -g3)
    endif()

    # Clang compile options
    list(APPEND CompileOptions -pthread -Wall -c -std=c++17 -fmessage-length=0 -MMD -stdlib=libc++ ${UserDefines})

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    # POSIX API
    add_definitions(-DPOSIX)
    set(AREG_DEVELOP_ENV "Posix")
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        list(APPEND CompileOptions -O2)
    else()
        list(APPEND CompileOptions -O0 -g3)
    endif()

    # GNU compile options
    list(APPEND CompileOptions -pthread -Wall -c -std=c++17 -fmessage-length=0 -MMD ${UserDefines})

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    # Visual Studio C++
    # Windows / Win32 API
    add_definitions(-DWINDOWS)
    set(AREG_DEVELOP_ENV "Windows")
    if(NOT CMAKE_BUILD_TYPE MATCHES "Release")
        list(APPEND CompileOptions -Od -RTC1 -c)
    endif()

endif()

# flags for bitness
if(Platform MATCHES "x86_64" AND NOT AREG_DEVELOP_ENV MATCHES "Windows")
    if(NOT DEFINED CrossCompile)
        if(Bitness MATCHES "32")
            list(APPEND CompileOptions -m32)
        else()
            list(APPEND CompileOptions -m64)
        endif()
    endif()
endif()

# Adding compile options
add_compile_options(${CompileOptions})

# Adding common definitions.
add_definitions(-DUNICODE)

# Examples Compile Options
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(ExampleCompileOptShared "-Wl,-rpath=${AREG_OUTPUT_BIN} -L ${AREG_OUTPUT_BIN}")
    set(ExampleCompileOptStatic "-L ${AREG_OUTPUT_LIB} -Wl,-Bstatic -Wl,-Bdynamic")
endif()

# Examples LD flags (-l is not necessary)
if(AREG_DEVELOP_ENV MATCHES "Windows")
    list(APPEND LDFLAGS advapi32 psapi shell32 ws2_32)
    list(APPEND AREG_EXAMPLES_LDFLAGS areg ${LDFLAGS})
else()
    list(APPEND LDFLAGS m stdc++ ncurses rt pthread)
    list(APPEND AREG_EXAMPLES_LDFLAGS areg ${LDFLAGS})
endif()


# Adding areg/product directory for clean-up
set_property(
    DIRECTORY
    APPEND
    PROPERTY ADDITIONAL_CLEAN_FILES ${ProjOutputDir}
)
# Setting output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ProjLibDir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ProjBinDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ProjBinDir})

# Only for Linux
if(UNIX AND NOT APPLE)
    set(CMAKE_EXECUTABLE_SUFFIX ".out")
endif()
