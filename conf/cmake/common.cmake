set(AREG_OUTPUT_OBJ  "${ProjObjDir}")
set(AREG_OUTPUT_LIB  "${ProjLibDir}")
set(AREG_OUTPUT_BIN  "${ProjBinDir}")
set(AREG_INCLUDES    "${ProjIncludes}")
set(AREG_TOOLCHAIN   "${CrossCompile}${Toolset}")
set(AREG_AR          "${CrossCompile}ar")
set(AREG_OS          "${OpSystem}")
set(AREG_STATIC_LIB)


include("${CMAKE_CONFIG_DIR}/user.cmake") 

if(areg MATCHES "static")
    set(AREG_BINARY "static")
else()
    set(AREG_BINARY "shared")
endif()


# Add compiler flags here
add_compile_options(-g -pthread -Werror)

add_library(Libs SHARED IMPORTED)
target_link_libraries(Libs INTERFACE -lm -lstdc++ -lrt -lpthread)

# Checking Compiler for adding corresponded tweaks and flags
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang compile options
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GNU compile options
    add_compile_options(-Wall ${UserDefines})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Visual Studio C++
endif()


if(Config MATCHES "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    add_definitions(-DNDEBUG)
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

