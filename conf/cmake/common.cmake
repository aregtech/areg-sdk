include("${CMAKE_CONFIG_DIR}/user.cmake")

set(AREG_OUTPUT_OBJ  "${ProjObjDir}")
set(AREG_OUTPUT_LIB  "${ProjLibDir}")
set(AREG_OUTPUT_BIN  "${ProjBinDir}")
set(AREG_INCLUDES    "${ProjIncludes}")
set(AREG_TOOLCHAIN   "${CrossCompile}${Toolset}")
set(AREG_AR          "${CrossCompile}ar")
set(AREG_OS          "${OpSystem}")
set(AREG_STATIC_LIB)

if(areg MATCHES "static")
    set(AREG_BINARY "static")
else()
    set(AREG_BINARY "shared")
endif()


# Add compiler flags here
# Checking Compiler for adding corresponded tweaks and flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang compile options
    add_compile_options(-g -pthread -Werror -stdlib=libc++)
    if (Config MATCHES "Release")
        add_compile_options(-O2)
    endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GNU compile options
    add_compile_options(-g -pthread -Werror -Wall ${UserDefines})
    if (Config MATCHES "Release")
        add_compile_options(-O2)
    endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Visual Studio C++
    if(Config MATCHES "Release")
        add_definitions(-DNDEBUG -DUNICODE)
    else()
        add_compile_options(-Od -RTC1)
        add_definitions(-DDEBUG -DUNICODE)
    endif()

endif()


if(Config MATCHES "Release")
    add_definitions(-DNDEBUG )
else()
    add_definitions(-DDEBUG)
endif()

# flags for bitness
if(Platform MATCHES "x86_64")
    if(NOT DEFINED CrossCompile)
        if(bit MATCHES "32")
            add_compile_options(-m32)
        else()
            add_compile_options(-m64)
        endif()
    endif()
endif()

if(AREG_OS MATCHES "Windows")
# Windows
    add_definitions(-DWINDOWS)
    set(OBJ_EXT "obj")
    set(AREG_BIN_EXT ".exe")
    set(AREG_LIB_PREFIX)
    set(AREG_STATIC_LIB ".lib")
    if(AREG_BINARY MATCHES "shared")
        set(AREG_LIB_EXT ".dll")
    else()
        set(AREG_LIB_EXT ".lib")
    endif()
else()
# Unix
    add_definitions(-DPOSIX)
    set(OBJ_EXT "o")
    set(AREG_BIN_EXT ".out")
    set(AREG_LIB_PREFIX "lib")
    set(AREG_STATIC_LIB ".a")
    if(AREG_BINARY MATCHES "shared")
        set(AREG_LIB_EXT ".so")
    else()
        set(AREG_LIB_EXT ".a")
    endif()
endif()

# Examples LD flags (-l is not necessary)
list(APPEND exampleLDFlags areg m  stdc++ rt pthread)
set(exampleCXXStandard "17")

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
