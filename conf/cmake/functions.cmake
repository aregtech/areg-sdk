# AREG CMake functions

# ---------------------------------------------------------------------------
# Description : Sets the compiler and the linker options of the executable applications.
#               Adds libraries to link. The AREG library is automatically added.
# Function ...: setAppOptions
# usage ......: setAppOptions( <name of executable> <list of libraries>) 
# ---------------------------------------------------------------------------
function(setAppOptions item library_list)

    if(AREG_BINARY MATCHES "shared")
        target_compile_definitions(${item} PRIVATE IMP_AREG_DLL)
    else()
        target_compile_definitions(${item} PRIVATE IMP_AREG_LIB)
    endif()

    # Linking flags
    target_link_libraries(${item} areg ${library_list} ${AREG_LDFLAGS})
   
    # Adjusting CPP standard for target
    set_target_properties(${item} PROPERTIES CXX_STANDARD ${AREG_CXX_STANDARD} CXX_STANDARD_REQUIRED ON )
    set_property(TARGET ${item} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${AREG_OUTPUT_BIN})
endfunction(setAppOptions item library_list)

# ---------------------------------------------------------------------------
# Description : Adds executable, sets the sources and the options of executable,
#               links with the passed list of libraries.
#               No need to specify the AREG library, it is automatically added.
# Function ...: addExecutableEx
# usage ......: addExecutableEx( <name of executable> <list of sources> <list of libraries> ) 
# ---------------------------------------------------------------------------
function(addExecutableEx target_name target_source_list library_list)
    add_executable(${target_name} ${target_source_list})
    setAppOptions(${target_name} "${library_list}")
endfunction(addExecutableEx target_name target_source_list library_list)

# ---------------------------------------------------------------------------
# Description : Adds executable, sets the sources and the options of executable.
#               The AREG library is automatically added.
# Function ...: addExecutable
# usage ......: addExecutable( <name of executable> <list of sources> ) 
# ---------------------------------------------------------------------------
function(addExecutable target_name target_source_list)
    addExecutableEx(${target_name} "${target_source_list}" "")
endfunction(addExecutable target_name target_source_list)


# ---------------------------------------------------------------------------
# Description : Sets the compiler and the linker options of the static library.
#               Adds libraries to link. The AREG library is automatically added.
# Function ...: setStaticLibOptions
# usage ......: setStaticLibOptions( <name of static library> <list of libraries>) 
# ---------------------------------------------------------------------------
function(setStaticLibOptions item library_list)

    target_compile_definitions(${item} PRIVATE _LIB)

    # Setting libxxx properties (static or shared)
    if(AREG_BINARY MATCHES "shared")
        target_compile_definitions(${item} PRIVATE IMP_AREG_DLL)
    else()
        target_compile_definitions(${item} PRIVATE IMP_AREG_LIB)
    endif()

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(${item} PRIVATE "-Bstatic")
    endif()

    target_compile_definitions(${item} PRIVATE _LIB)
    target_link_libraries(${item} areg ${library_list} ${AREG_LDFLAGS})

    # Adjusting CPP standard for target
    set_target_properties(${item} PROPERTIES CXX_STANDARD ${AREG_CXX_STANDARD} CXX_STANDARD_REQUIRED ON )
    set_property(TARGET ${item} PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${AREG_OUTPUT_LIB})

endfunction(setStaticLibOptions item library_list)

# ---------------------------------------------------------------------------
# Description : Adds static library, sets the sources and the options of library,
#               links with the passed list of libraries.
#               No need to specify the AREG library, it is automatically added.
# Function ...: addStaticLibEx
# usage ......: addStaticLibEx( <name of static library> <list of sources> <list of libraries> ) 
# ---------------------------------------------------------------------------
function(addStaticLibEx target_name target_source_list library_list)
    add_library(${target_name} STATIC ${target_source_list})
    setStaticLibOptions(${target_name} "${library_list}")
endfunction(addStaticLibEx target_name target_source_list library_list)

# ---------------------------------------------------------------------------
# Description : Adds static library, sets the sources and the options of library.
#               The AREG library is automatically added.
# Function ...: addStaticLib
# usage ......: addStaticLib( <name of static library> <list of sources> ) 
# ---------------------------------------------------------------------------
function(addStaticLib target_name target_source_list)
    addStaticLibEx(${target_name} "${target_source_list}" "")
endfunction(addStaticLib target_name target_source_list)

# ---------------------------------------------------------------------------
# Description : Sets the compiler and the linker options of the shared library.
#               Adds libraries to link. The AREG library is automatically added.
# Function ...: setSharedLibOptions
# usage ......: setSharedLibOptions( <name of shared library> <list of libraries>) 
# ---------------------------------------------------------------------------
function(setSharedLibOptions item library_list)

    if(AREG_BINARY MATCHES "shared")
        target_compile_definitions(${item} PRIVATE IMP_AREG_DLL)
        target_compile_options(${item} PRIVATE ${AREG_SHARED_LIB_FLAGS})
    else()
        target_compile_definitions(${item} PRIVATE IMP_AREG_LIB)
    endif()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${item} PRIVATE -fPIC)
    endif()

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(${item} PRIVATE "-Bdynamic")
    endif()

    target_compile_definitions(${item} PRIVATE _USRDLL)
    target_link_libraries(${item} areg ${library_list} ${AREG_LDFLAGS})

    # Adjusting CPP standard for target
    set_target_properties(${item} PROPERTIES CXX_STANDARD ${AREG_CXX_STANDARD} CXX_STANDARD_REQUIRED ON )
    set_property(TARGET ${item} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${AREG_OUTPUT_BIN})

endfunction(setSharedLibOptions item library_list)

# ---------------------------------------------------------------------------
# Description : Adds shared library, sets the sources and the options of library,
#               links with the passed list of libraries.
#               No need to specify the AREG library, it is automatically added.
# Function ...: addSharedLibEx
# usage ......: addSharedLibEx( <name of shared library> <list of sources> <list of libraries> ) 
# ---------------------------------------------------------------------------
function(addSharedLibEx target_name target_source_list library_list)
    add_library(${target_name} SHARED ${target_source_list})
    setSharedLibOptions(${target_name} "${library_list}")
endfunction(addSharedLibEx target_name target_source_list library_list)

# ---------------------------------------------------------------------------
# Description : Adds shared library, sets the sources and the options of library.
#               The AREG library is automatically added.
# Function ...: addStaticLib
# usage ......: addStaticLib( <name of shared library> <list of sources> ) 
# ---------------------------------------------------------------------------
function(addSharedLib target_name target_source_list)
    addSharedLibEx(${target_name} "${target_source_list}" "")
endfunction(addSharedLib target_name target_source_list)
