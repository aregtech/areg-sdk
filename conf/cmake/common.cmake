include("${CMAKE_CONFIG_DIR}/user.cmake")

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

# Checking Compiler for adding corresponded tweaks and flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # POSIX API
    add_definitions(-DPOSIX)
    set(AREG_DEVELOP_ENV "Posix")
    if (Config MATCHES "Release")
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
    if (Config MATCHES "Release")
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
    if(NOT Config MATCHES "Release")
        list(APPEND CompileOptions -Od -RTC1 -c)
    endif()
endif()

# CPP standard for the project
set(aregCXXStandard "17")

add_definitions(-DUNICODE)
if(Config MATCHES "Release")
    add_definitions(-DNDEBUG)
else()
    add_definitions(-DDEBUG)
endif()

# flags for bitness
if(Platform MATCHES "x86_64" AND NOT AREG_DEVELOP_ENV MATCHES "Windows")
    if(NOT DEFINED CrossCompile)
        if(bit MATCHES "32")
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
