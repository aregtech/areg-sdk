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
    target_link_libraries(${item} areg-extensions ${library_list} areg ${AREG_EXTENDED_LIBS} ${AREG_LDFLAGS})
   
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
    target_include_directories(${target_name}  BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})    
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
    target_link_libraries(${item} ${library_list} areg ${AREG_LDFLAGS})

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
    # target_link_libraries(${item} ${library_list} areg ${AREG_LDFLAGS})

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

# ---------------------------------------------------------------------------
# Description : Sets a list of source files in the given list.
#               The list should be empty. If it is not, the previous values
#               will be lost.
# Function ...: setSources
# usage ......: setSources( <name of the list> <list of sources> ) 
# ---------------------------------------------------------------------------
function(setSources source_list)
    set(current_dir "${CMAKE_CURRENT_LIST_DIR}")
    set(temp_list)

    foreach(arg IN LISTS ARGN)
        if (EXISTS "${arg}")
            list(APPEND temp_list "${arg}")
        elseif(EXISTS "${current_dir}/${arg}")
            list(APPEND temp_list "${current_dir}/${arg}")
        else()
            status(WARNING " >>> Do not add item ${arg} to list, it does not exist")
        endif()
    endforeach()

    set(${source_list} "${temp_list}" PARENT_SCOPE)
endfunction(setSources)

# ---------------------------------------------------------------------------
# Description : This macro adds a list of source files at the end of the existing list.
#               The list should be declared and exist, the previous values
#               will not be lost.
# Function ...: ADD_SOURCES
# usage ......: ADD_SOURCES( <name of the list> <list of sources> ) 
# ---------------------------------------------------------------------------
macro(ADD_SOURCES source_list)
    
    if (NOT DEFINED ${source_list})
        set(${source_list})
    endif()

    set(current_dir "${CMAKE_CURRENT_LIST_DIR}")
    set(list_var "${ARGN}")

    foreach(arg IN LISTS list_var)
        if (EXISTS "${arg}")
            list(APPEND ${source_list} "${arg}")
        elseif(EXISTS "${current_dir}/${arg}")
            list(APPEND ${source_list} "${current_dir}/${arg}")
        else()
            message(warning " >>> Do not add item ${arg} to list, it does not exist")
        endif()
    endforeach()
endmacro(ADD_SOURCES)

# ---------------------------------------------------------------------------
# Description : This macro include the 'CMakeLists.txt' file of specified
#               sub-directory to include in the build. The name of
#               sub-directory should not include slash '/' at the
#               begin and at the end.
# Function ...: INCLUDE_DIR
# usage ......: INCLUDE_DIR( <sub-directory path> <list of sources> ) 
# ---------------------------------------------------------------------------
macro(INCLUDE_DIR sub_dir)
    if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/CMakeLists.txt")
        message(ERROR " >>> The file \'${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/CMakeLists.txt\' does not exist, cannot include")
    endif()
    include("${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/CMakeLists.txt")
endmacro(INCLUDE_DIR)

# ---------------------------------------------------------------------------
# Description : This macro include the 'CMakeLists.txt' file of specified
#               sub-directory, which will append the sources to the 'list_name'.
# Function ...: PROJECT_SOURCES_EX
# usage ......: PROJECT_SOURCES_EX( <name of the list> <name of sub-directory> ) 
# ---------------------------------------------------------------------------
macro(PROJECT_SOURCES_EX list_name project_dir)
    set(project_sources)

    INCLUDE_DIR(${project_dir})

    set(${list_name} "${project_sources}")
    unset(project_sources)
endmacro(PROJECT_SOURCES_EX)

# ---------------------------------------------------------------------------
# Description : This macro include the 'CMakeLists.txt' file of specified
#               sub-directory and append the sources to the  list with name
#               '${project_name}_src'. For example if the project name is
#               'my_project', the list of source files to compile will be
#               'my_project_src'. This macro is simplified version of
#               the macro PROJECT_SOURCES_EX.
# Function ...: PROJECT_SOURCES
# usage ......: PROJECT_SOURCES( <name of the project> ) 
# ---------------------------------------------------------------------------
macro(PROJECT_SOURCES project_name)
    set(src_name    "${project_dir}_src")
    PROJECT_SOURCES_EX("${src_name}" "${proj_name}")
    unset(src_name)
endmacro(PROJECT_SOURCES)

# ---------------------------------------------------------------------------
# Description : This macro declares a variable named 'project_${project_location}',
#               which value is the '${project_name}', as well as includes 
#               in the build the 'CMakeLists.txt' file specified in the
#               'project_dir' to create a list of sources named '${project_location}_src'.
#               For example, if the developer want to declare a project
#               'my_proj', which sources are in the sub-directory 'sources/foo',
#               where 'foo' is the location of the project, this macro declares 
#               a variable named 'project_foo', which value
#               is 'my_proj' and the list of sources to compile, which name is
#               'foo_src', so that after using this macro, the developer can
#               use both variables: the 'project_foo' and 'foo_src'.
# Function ...: DECLARE_PROJECT
# usage ......: DECLARE_PROJECT( <name of the project> <name of project sub-directory> ) 
# ---------------------------------------------------------------------------
macro(DECLARE_PROJECT_EX project_name project_location project_dir)
    set(pr_name     "project_${project_location}")
    set(src_name    "${project_location}_src")
    set(${pr_name} "${project_name}")

    PROJECT_SOURCES_EX(${src_name} ${project_dir})

    unset(pr_name)
    unset(src_name)
endmacro(DECLARE_PROJECT_EX)

# ---------------------------------------------------------------------------
# Description : This macro declares a variable named 'project_${project_dir}',
#               which value is the '${project_name}', as well as includes 
#               in the build the 'CMakeLists.txt' file specified in the
#               'project_dir' to create a list of sources named '${project_dir}_src'.
#               For example, if the developer want to declare a project
#               'my_proj', which sources are in the sub-directory 'foo',
#               This macro declares a variable named 'project_foo', which value
#               is 'my_proj' and the list of sources to compile, which name is
#               'foo_src', so that after using this macro, the developer can
#               use both variables: the 'project_foo' and 'foo_src'.
# Function ...: DECLARE_PROJECT
# usage ......: DECLARE_PROJECT( <name of the project> <name of project sub-directory> ) 
# ---------------------------------------------------------------------------
macro(DECLARE_PROJECT project_name project_dir)
    DECLARE_PROJECT_EX(${project_name} ${project_dir} ${project_dir})
endmacro(DECLARE_PROJECT)
