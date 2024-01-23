# AREG CMake functions

# ---------------------------------------------------------------------------
# Description : Sets the compiler and the linker options of the executable applications.
#               Adds libraries to link. The AREG library is automatically added.
# Function ...: setAppOptions
# usage ......: setAppOptions( <name of executable> <list of libraries>) 
# ---------------------------------------------------------------------------
function(setAppOptions item library_list)
    # Set common compile definition
    target_compile_definitions(${item} PRIVATE ${COMMON_COMPILE_DEF})

    # Linking flags
    target_link_libraries(${item} areg-extend ${library_list} areg ${AREG_EXTENDED_LIBS} ${AREG_LDFLAGS})
   
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
    set(exList "${ARGN}")
    foreach(item IN LISTS exList)
        list(APPEND library_list "${item}")
    endforeach()
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

    # Set common compile definition
    target_compile_definitions(${item} PRIVATE ${COMMON_COMPILE_DEF} _LIB)

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(${item} PRIVATE "-Bstatic")
        target_compile_options(${item} PRIVATE -fPIC)
    endif()

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
    set(exList "${ARGN}")
    foreach(item IN LISTS exList)
        list(APPEND library_list "${item}")
    endforeach()
    add_library(${target_name} STATIC ${target_source_list})
    setStaticLibOptions(${target_name} "${library_list}")
    target_include_directories(${target_name}  BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})    
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
    # Set common compile definition
    target_compile_definitions(${item} PRIVATE ${COMMON_COMPILE_DEF} _USRDLL)

    # Linking flags
    target_link_libraries(${item} areg-extend ${library_list} areg ${AREG_EXTENDED_LIBS} ${AREG_LDFLAGS})

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(${item} PRIVATE -fPIC)
        target_compile_options(${item} PRIVATE "-Bdynamic")
    endif()

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
    set(exList "${ARGN}")
    foreach(item IN LISTS exList)
        list(APPEND library_list "${item}")
    endforeach()
    add_library(${target_name} SHARED ${target_source_list})
    setSharedLibOptions(${target_name} "${library_list}")
    target_include_directories(${target_name}  BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})    
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
# Description : This function is called to set the compiler short name such
#               as 'g++', 'gcc', 'clang' or 'msvc', etc.
# Function ...: findCompilerShortName
# Usage ......: findCompilerShortName( <compiler path> <compiler short name option> ) 
# Example ....: findCompilerShortName("${CMAKE_CXX_COMPILER}" AREG_COMPILER_SHORT)
#               It sets in variable 'AREG_COMPILER_SHORT' the short name of the compiler.
# ---------------------------------------------------------------------------
function(findCompilerShortName compiler_path compiler_short_name_option)

    set(FOUND_POS)
    set(${compiler_short_name_option} "unknown" PARENT_SCOPE)

    string(FIND "${compiler_path}" "clang++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_short_name_option} "clang++" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "clang" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_short_name_option} "clang" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "gcc" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_short_name_option} "gcc" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "g++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_short_name_option} "g++" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "c++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_short_name_option} "c++" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "cc" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_short_name_option} "cc" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "cl" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_short_name_option} "cl" PARENT_SCOPE)
        return()
    endif()

endfunction(findCompilerShortName)

# ---------------------------------------------------------------------------
# Description : This function is called to set the compiler family name such
#               as 'gnu', 'llvm', 'msvc' or 'cygwin'.
# Function ...: findCompilerFamilyName
# Usage ......: findCompilerFamilyName( <compiler path> <compiler family name option> ) 
# Example ....: findCompilerFamilyName("${CMAKE_CXX_COMPILER}" AREG_COMPILER_FAMILY)
#               It sets in variable 'AREG_COMPILER_FAMILY' the family of the compiler.
# ---------------------------------------------------------------------------
function(findCompilerFamilyName compiler_path compiler_family_option_name)

    set(FOUND_POS)

    string(FIND "${compiler_path}" "clang++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_family_option_name} "clang" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "clang" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_family_option_name} "clang" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "gcc" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        if (CYGWIN)
            set(${compiler_family_option_name} "cygwin" PARENT_SCOPE)
        else()
            set(${compiler_family_option_name} "gnu" PARENT_SCOPE)
        endif()
        return()
    endif()

    string(FIND "${compiler_path}" "g++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        if (CYGWIN)
            set(${compiler_family_option_name} "cygwin" PARENT_SCOPE)
        else()
            set(${compiler_family_option_name} "gnu" PARENT_SCOPE)
        endif()
        return()
    endif()

    string(FIND "${compiler_path}" "c++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        if (CYGWIN)
            set(${compiler_family_option_name} "cygwin" PARENT_SCOPE)
        else()
            set(${compiler_family_option_name} "gnu" PARENT_SCOPE)
        endif()
        return()
    endif()

    string(FIND "${compiler_path}" "cc" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        if (CYGWIN)
            set(${compiler_family_option_name} "cygwin" PARENT_SCOPE)
        else()
            set(${compiler_family_option_name} "gnu" PARENT_SCOPE)
        endif()
        return()
    endif()

    string(FIND "${compiler_path}" "cl" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${compiler_family_option_name} "msvc" PARENT_SCOPE)
        return()
    endif()

    set(${compiler_family_option_name} "unknown" PARENT_SCOPE)

endfunction(findCompilerFamilyName)

# ---------------------------------------------------------------------------
# Description : This function is called if use default compiler.
#               It sets CXX and CC compilers, as well as parses the compiler
#               names to set the compiler family name.
#               All argument should be passed by option name.
#               For example, macro_find_default_compiler(AREG_COMPILER_FAMILY, AREG_CXX_COMPILER, AREG_C_COMPILER)
#               This sets values for options AREG_COMPILER_FAMILY, AREG_CXX_COMPILER and AREG_C_COMPILER
# Function ...: macro_find_default_compiler
# usage ......: macro_find_default_compiler( <name of compiler family option> <compiler short name option> <name of cc compiler option> ) 
# ---------------------------------------------------------------------------
macro(delete_macro_find_default_compiler compiler_family_option_name)

    findCompiler("${CMAKE_CXX_COMPILER}" IS_GCC IS_CPP IS_CLANG IS_MSVC)

    if (${IS_GCC})
        if (CYGWIN)
            set(${compiler_family_option_name} "cygwin")
        else()
            set(${compiler_family_option_name} "gnu")
        endif()
    elseif(${IS_CLANG})
        set(${compiler_family_option_name} "llvm")
    elseif(${IS_MSVC})
        set(${compiler_family_option_name} "msvc")
    endif()
endmacro(delete_macro_find_default_compiler)

# ---------------------------------------------------------------------------
# Description : This macro adds a list of source files at the end of the existing list.
#               The list should be declared and exist, the previous values
#               will not be lost.
# macro ......: macro_add_sources
# usage ......: macro_add_sources( <name of the list> <list of sources> ) 
# ---------------------------------------------------------------------------
macro(macro_add_sources source_list)
    
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
endmacro(macro_add_sources)

# ---------------------------------------------------------------------------
# Description : This macro include the 'CMakeLists.txt' file of specified
#               sub-directory to include in the build. The name of
#               sub-directory should not include slash '/' at the
#               begin and at the end.
# macro ......: macro_include_dir
# usage ......: macro_include_dir( <sub-directory path> ) 
# ---------------------------------------------------------------------------
macro(macro_include_dir sub_dir)
    if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/CMakeLists.txt")
        message(ERROR " >>> The file \'${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/CMakeLists.txt\' does not exist, cannot include")
    endif()
    include("${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/CMakeLists.txt")
endmacro(macro_include_dir)

# ---------------------------------------------------------------------------
# Description : This macro include the 'CMakeLists.txt' file of specified
#               sub-directory, which will append the sources to the 'list_name'.
# macro ......: macro_project_sources_ex
# usage ......: macro_project_sources_ex( <name of the list> <name of sub-directory> ) 
# ---------------------------------------------------------------------------
macro(macro_project_sources_ex list_name project_dir)
    set(macro_project_sources)

    macro_include_dir(${project_dir})

    set(${list_name} "${macro_project_sources}")
    unset(macro_project_sources)
endmacro(macro_project_sources_ex)

# ---------------------------------------------------------------------------
# Description : This macro include the 'CMakeLists.txt' file of specified
#               sub-directory and append the sources to the  list with name
#               '${project_name}_src'. For example if the project name is
#               'my_project', the list of source files to compile will be
#               'my_project_src'. This macro is simplified version of
#               the macro macro_project_sources_ex.
# macro ......: macro_project_sources
# usage ......: macro_project_sources( <name of the project> ) 
# ---------------------------------------------------------------------------
macro(macro_project_sources project_name)
    set(src_name    "${project_dir}_src")
    macro_project_sources_ex("${src_name}" "${proj_name}")
    unset(src_name)
endmacro(macro_project_sources)

# ---------------------------------------------------------------------------
# Description : This macro declares a variable named 'project_${project_alias}',
#               which value is the '${project_name}', as well as includes 
#               in the build the 'CMakeLists.txt' file specified in the
#               'project_dir' to create a list of sources named '${project_alias}_src'.
#               For example, if the developer want to declare a project
#               'my_proj', which sources are in the sub-directory 'sources/foo',
#               where 'foo' is the location of the project, this macro declares 
#               a variable named 'project_foo', which value
#               is 'my_proj' and the list of sources to compile, which name is
#               'foo_src', so that after using this macro, the developer can
#               use both variables: the 'project_foo' and 'foo_src'.
# macro ......: macro_declare_project_ex
# usage ......: macro_declare_project_ex( <name of the project> <project alias> <name of project sub-directory> ) 
# ---------------------------------------------------------------------------
macro(macro_declare_project_ex project_name project_alias project_dir)
    set(pr_name      "project_${project_alias}")
    set(src_name     "${project_alias}_src")
    set(${pr_name}   "${project_name}")
    set(project_root "${CMAKE_CURRENT_LIST_DIR}")

    if (NOT ${project_dir} STREQUAL "")
        macro_project_sources_ex(${src_name} ${project_dir})
    endif()

    unset(pr_name)
    unset(src_name)
endmacro(macro_declare_project_ex)

# ---------------------------------------------------------------------------
# Description : This macro declares a variable named 'project_${project_alias}',
#               which value is the '${project_name}', as well as includes 
#               in the build the 'CMakeLists.txt' file specified in the
#               'project_alias' to create a list of sources named '${project_alias}_src'.
#               For example, if the developer want to declare a project
#               'my_proj', which sources are in the sub-directory 'foo',
#               This macro declares a variable named 'project_foo', which value
#               is 'my_proj' and the list of sources to compile, which name is
#               'foo_src', so that after using this macro, the developer can
#               use both variables: the 'project_foo' and 'foo_src'.
# macro ......: macro_declare_project
# usage ......: macro_declare_project( <name of the project> <project alias> ) 
# ---------------------------------------------------------------------------
macro(macro_declare_project project_name project_alias)
    macro_declare_project_ex(${project_name} ${project_alias} ${project_alias})
endmacro(macro_declare_project)

