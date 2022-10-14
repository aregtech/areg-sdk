include("${CMAKE_CONFIG_DIR}/user.cmake")

set(AREG_OUTPUT_OBJ  "${ProjObjDir}")
set(AREG_OUTPUT_LIB  "${ProjLibDir}")
set(AREG_OUTPUT_BIN  "${ProjBinDir}")
set(AREG_INCLUDES    "${ProjIncludes}")
set(AREG_TOOLCHAIN   "${CrossCompile}${Toolset}")
set(AREG_AR          "${CrossCompile}ar")
set(AREG_OS          "${OpSystem}")
set(AREG_STATIC_LIB)

if(areg MATCHES "shared")
    set(AREG_BINARY "shared")
else()
    set(AREG_BINARY "static")
endif()

# Checking Compiler for adding corresponded tweaks and flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang compile options
    list(APPEND CompileOptions -g -pthread -Werror -stdlib=libc++ ${UserDefines})
    if (Config MATCHES "Release")
        list(APPEND CompileOptions -O2)
    endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GNU compile options
    list(APPEND CompileOptions -g -pthread -Werror -Wall ${UserDefines})
    if (Config MATCHES "Release")
        list(APPEND CompileOptions -O2)
    endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Visual Studio C++
    if(Config MATCHES "Release")
    else()
        list(APPEND CompileOptions -Od -RTC1)
    endif()
    add_definitions(-DUNICODE)
endif()

# CPP standard for the project
set(aregCXXStandard "17")

if(Config MATCHES "Release")
    add_definitions(-DNDEBUG)
else()
    add_definitions(-DDEBUG)
endif()

# flags for bitness
if(Platform MATCHES "x86_64")
    if(NOT DEFINED CrossCompile AND NOT AREG_OS MATCHES "Windows")
        if(bit MATCHES "32")
            list(APPEND CompileOptions -m32)
        else()
            list(APPEND CompileOptions -m64)
        endif()
    endif()
endif()

if(AREG_OS MATCHES "Windows")
# Windows
    add_definitions(-DWINDOWS)
else()
# Unix
    add_definitions(-DPOSIX)
endif()

# Adding compile options
add_compile_options(${CompileOptions})

# Examples Compile Options
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(ExampleCompileOptShared "-Wl,-rpath=${AREG_OUTPUT_BIN} -L ${AREG_OUTPUT_BIN}")
    set(ExampleCompileOptStatic "-L ${AREG_OUTPUT_LIB} -Wl,-Bstatic -Wl,-Bdynamic")
endif()

# Examples LD flags (-l is not necessary)
if(AREG_OS MATCHES "Windows")
    list(APPEND LDFLAGS ws2_32 psapi)
    list(APPEND exampleLDFlags areg ${LDFLAGS})
else()
    list(APPEND LDFLAGS m  stdc++ rt pthread ncurses)
    list(APPEND exampleLDFlags areg ${LDFLAGS})
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
