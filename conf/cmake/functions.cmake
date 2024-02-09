# AREG CMake functions

# ---------------------------------------------------------------------------
# Description : Sets the compiler and the linker options of the executable applications.
#               Adds libraries to link. The AREG library is automatically added.
# Function ...: setAppOptions
# Parameters .: ${item}         -- the name of executable to apply options.
#               ${library_list} -- the list of libraries to link with the executable.
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
endfunction(setAppOptions)

# ---------------------------------------------------------------------------
# Description : Adds executable, sets the sources and the options of executable,
#               links with the passed list of libraries.
#               No need to specify the AREG library, it is automatically added.
# Function ...: addExecutableEx
# Parameters .: ${target_name}  -- The name of the executable to build.
#               ${source_list}  -- The list of the sources to build the executable.
#               ${library_list} -- The list of libraries to link executable.
# usage ......: addExecutableEx( <name of executable> <list of sources> <list of libraries> ) 
# ---------------------------------------------------------------------------
function(addExecutableEx target_name source_list library_list)
    set(exList "${ARGN}")
    foreach(item IN LISTS exList)
        list(APPEND library_list "${item}")
    endforeach()
    add_executable(${target_name} ${source_list})
    setAppOptions(${target_name} "${library_list}")
    target_include_directories(${target_name}  BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})    
endfunction(addExecutableEx)

# ---------------------------------------------------------------------------
# Description : Adds executable, sets the sources and the options of executable.
#               The AREG library is automatically added.
# Function ...: addExecutable
# Parameters .: ${target_name}  -- The name of the executable to build.
#               ${source_list}  -- The list of the sources to build the executable.
# usage ......: addExecutable( <name of executable> <list of sources> ) 
# ---------------------------------------------------------------------------
function(addExecutable target_name source_list)
    addExecutableEx(${target_name} "${source_list}" "")
endfunction(addExecutable)


# ---------------------------------------------------------------------------
# Description : Sets the compiler and the linker options of the static library.
#               Adds libraries to link. The AREG library is automatically added.
# Function ...: setStaticLibOptions
# Parameters .: ${item}         -- the name of static library to apply options.
#               ${library_list} -- the list of libraries to link with the library.
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

endfunction(setStaticLibOptions)

# ---------------------------------------------------------------------------
# Description : Adds static library, sets the sources and the options of library,
#               links with the passed list of libraries.
#               No need to specify the AREG library, it is automatically added.
# Function ...: addStaticLibEx
# Parameters .: ${target_name}  -- The name of the static library to build.
#               ${source_list}  -- The list of the sources to build the static library.
#               ${library_list} -- The list of libraries to link the static library.
# usage ......: addStaticLibEx( <name of static library> <list of sources> <list of libraries> ) 
# ---------------------------------------------------------------------------
function(addStaticLibEx target_name source_list library_list)
    set(exList "${ARGN}")
    foreach(item IN LISTS exList)
        list(APPEND library_list "${item}")
    endforeach()
    add_library(${target_name} STATIC ${source_list})
    setStaticLibOptions(${target_name} "${library_list}")
    target_include_directories(${target_name}  BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})    
endfunction(addStaticLibEx)

# ---------------------------------------------------------------------------
# Description : Adds static library, sets the sources and the options of library.
#               The AREG library is automatically added.
# Function ...: addStaticLib
# Parameters .: ${target_name}  -- The name of the static library to build.
#               ${source_list}  -- The list of the sources to build the static library.
# usage ......: addStaticLib( <name of static library> <list of sources> ) 
# ---------------------------------------------------------------------------
function(addStaticLib target_name source_list)
    addStaticLibEx(${target_name} "${source_list}" "")
endfunction(addStaticLib)

# ---------------------------------------------------------------------------
# Description : Sets the compiler and the linker options of the shared library.
#               Adds libraries to link. The AREG library is automatically added.
# Function ...: setSharedLibOptions
# Parameters .: ${item}         -- the name of shared library to apply options.
#               ${library_list} -- the list of libraries to link with the library.
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

endfunction(setSharedLibOptions)

# ---------------------------------------------------------------------------
# Description : Adds shared library, sets the sources and the options of library,
#               links with the passed list of libraries.
#               No need to specify the AREG library, it is automatically added.
# Function ...: addSharedLibEx
# Parameters .: ${target_name}  -- The name of the shared library to build.
#               ${source_list}  -- The list of the sources to build the shared library.
#               ${library_list} -- The list of libraries to link the shared library.
# usage ......: addSharedLibEx( <name of shared library> <list of sources> <list of libraries> ) 
# ---------------------------------------------------------------------------
function(addSharedLibEx target_name source_list library_list)
    set(exList "${ARGN}")
    foreach(item IN LISTS exList)
        list(APPEND library_list "${item}")
    endforeach()
    add_library(${target_name} SHARED ${source_list})
    setSharedLibOptions(${target_name} "${library_list}")
    target_include_directories(${target_name}  BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})    
endfunction(addSharedLibEx)

# ---------------------------------------------------------------------------
# Description : Adds shared library, sets the sources and the options of library.
#               The AREG library is automatically added.
# Function ...: addSharedLib
# Parameters .: ${target_name}  -- The name of the shared library to build.
#               ${source_list}  -- The list of the sources to build the shared library.
# usage ......: addSharedLib( <name of shared library> <list of sources> ) 
# ---------------------------------------------------------------------------
function(addSharedLib target_name target_source_list)
    addSharedLibEx(${target_name} "${target_source_list}" "")
endfunction(addSharedLib)

# ---------------------------------------------------------------------------
# Description : Sets a list of source files in the given list.
#               The list should be empty. If it is not, the previous values
#               will be lost.
# Function ...: setSources
# Parameters .: ${source_list}  -- The name of the source list variable to set the list.
#               ARGN            -- The list of the sources to set, followed the ${source_list} parameter.
#                                  Each entry in the list can have either absolute or relative path to current directory.
# usage ......: setSources( <name of the list variable> <list of sources> ) 
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
            message(WARNING " >>> Do not add item ${arg} to list, it does not exist")
        endif()
    endforeach()

    set(${source_list} "${temp_list}" PARENT_SCOPE)
endfunction(setSources)

# ---------------------------------------------------------------------------
# Description : This function is called to set the compiler short name such
#               as 'g++', 'gcc', 'clang' or 'msvc', etc.
# Function ...: findCompilerShortName
# Parameters .: ${compiler_path}    -- The path to the compiler to guess the short name
#               ${short_name_var}   -- The name of the variable to set the short name.
# Usage ......: findCompilerShortName( <compiler path> <compiler short name option> ) 
# Example ....: findCompilerShortName("${CMAKE_CXX_COMPILER}" AREG_COMPILER_SHORT)
#               It sets in variable 'AREG_COMPILER_SHORT' the short name of the compiler.
# ---------------------------------------------------------------------------
function(findCompilerShortName compiler_path short_name_var)

    set(FOUND_POS)
    set(${short_name_var} "unknown" PARENT_SCOPE)

    string(FIND "${compiler_path}" "clang++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${short_name_var} "clang++" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "clang" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${short_name_var} "clang" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "gcc" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${short_name_var} "gcc" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "g++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${short_name_var} "g++" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "c++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${short_name_var} "c++" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "cc" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${short_name_var} "cc" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "cl" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${short_name_var} "cl" PARENT_SCOPE)
        return()
    endif()

endfunction(findCompilerShortName)

# ---------------------------------------------------------------------------
# Description : This function is called to set the compiler family name such
#               as 'gnu', 'llvm', 'msvc' or 'cygwin'.
# Function ...: findCompilerFamilyName
# Parameters .: ${compiler_path}    -- The path of compiler to guess family.
#               ${family_var}       -- The name of variable to set the compiler family name.
# Usage ......: findCompilerFamilyName( <compiler path> <compiler family name option> ) 
# Example ....: findCompilerFamilyName("${CMAKE_CXX_COMPILER}" AREG_COMPILER_FAMILY)
#               It sets in variable 'AREG_COMPILER_FAMILY' the family of the compiler.
# ---------------------------------------------------------------------------
function(findCompilerFamilyName compiler_path family_var)

    set(FOUND_POS)

    string(FIND "${compiler_path}" "clang++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${family_var} "clang" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "clang" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${family_var} "clang" PARENT_SCOPE)
        return()
    endif()

    string(FIND "${compiler_path}" "gcc" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        if (CYGWIN)
            set(${family_var} "cygwin" PARENT_SCOPE)
        else()
            set(${family_var} "gnu" PARENT_SCOPE)
        endif()
        return()
    endif()

    string(FIND "${compiler_path}" "g++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        if (CYGWIN)
            set(${family_var} "cygwin" PARENT_SCOPE)
        else()
            set(${family_var} "gnu" PARENT_SCOPE)
        endif()
        return()
    endif()

    string(FIND "${compiler_path}" "c++" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        if (CYGWIN)
            set(${family_var} "cygwin" PARENT_SCOPE)
        else()
            set(${family_var} "gnu" PARENT_SCOPE)
        endif()
        return()
    endif()

    string(FIND "${compiler_path}" "cc" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        if (CYGWIN)
            set(${family_var} "cygwin" PARENT_SCOPE)
        else()
            set(${family_var} "gnu" PARENT_SCOPE)
        endif()
        return()
    endif()

    string(FIND "${compiler_path}" "cl" FOUND_POS REVERSE)
    if (${FOUND_POS} GREATER -1)
        set(${family_var} "msvc" PARENT_SCOPE)
        return()
    endif()

    set(${family_var} "unknown" PARENT_SCOPE)

endfunction(findCompilerFamilyName)

# ---------------------------------------------------------------------------
# Description : This macro adds a list of source files at the end of the existing list.
#               The list should be declared and exist, the previous values
#               will not be lost.
# Macro ......: macro_add_sources
# Parameters .: ${source_list}  -- The name of the source list variable to set the list.
#               ARGN            -- The list of the sources to set, followed the ${source_list} parameter.
#                                  Each entry in the list can have either absolute or relative path to current directory.
# Usage ......: macro_add_sources( <name of the list> <list of sources> ) 
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
# Macro ......: macro_include_dir
# Parameters .: ${sub_dir}      -- the name of the sub-directory that contains 'CMakeLists.txt' file.
# Usage ......: macro_include_dir( <sub-directory path> ) 
# ---------------------------------------------------------------------------
macro(macro_include_dir sub_dir)
    if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/CMakeLists.txt")
        message(ERROR " >>> The file \'${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/CMakeLists.txt\' does not exist, cannot include")
    endif()
    include("${CMAKE_CURRENT_LIST_DIR}/${sub_dir}/CMakeLists.txt")
endmacro(macro_include_dir)

# ---------------------------------------------------------------------------
# Description : This macro includes the 'CMakeLists.txt' file of specified
#               sub-directory, which appends the sources to the 'list_name'.
# Macro ......: macro_project_sources_ex
# Parameters .: ${list_name}    -- The name of the variable to add sources from sub-directory.
#               ${sub_dir}      -- The name of sub-directory that contains 'CMakeList.txt' file.
# Usage ......: macro_project_sources_ex( <name of the list> <name of sub-directory> ) 
# ---------------------------------------------------------------------------
macro(macro_project_sources_ex list_name sub_dir)
    set(project_sources)

    macro_include_dir(${sub_dir})

    set(${list_name} "${project_sources}")
    unset(project_sources)
endmacro(macro_project_sources_ex)

# ---------------------------------------------------------------------------
# Description : This macro includes the 'CMakeLists.txt' file of specified
#               sub-directory and appends the sources to the  list with name
#               '${project_name}_src'. For example if the project name is
#               'my_project', the list of source files to compile will be
#               'my_project_src'. This macro is simplified version of
#               the macro macro_project_sources_ex.
# Macro ......: macro_project_sources
# Parameters .: ${project_name}     -- The name of the project, which has same name od a sub-directory
# Usage ......: macro_project_sources( <name of the project> ) 
# ---------------------------------------------------------------------------
macro(macro_project_sources project_name)
    set(src_name    "${project_name}_src")
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
# Macro ......: macro_declare_project_ex
# Parameters .: ${project_name}     -- The name of the project to declare.
#               ${project_alias}    -- The alias of the project, which is used to create variable name with prefix 'project_'.
#                                      Can be same as ${project_name}.
#               ${project_dir}      -- The sub-directory name of the project.
# Usage ......: macro_declare_project_ex( <name of the project> <project alias> <name of project sub-directory> ) 
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
# Parameters .: ${project_name}     -- The name of the project to declare.
#               ${project_alias}    -- The alias of the project, which is used to create variable name with prefix 'project_'.
#                                      Can be same as ${project_name}. As well it assumes that there is a sub-directory with the same name.
# Macro ......: macro_declare_project
# Usage ......: macro_declare_project( <name of the project> <project alias> ) 
# ---------------------------------------------------------------------------
macro(macro_declare_project project_name project_alias)
    macro_declare_project_ex(${project_name} ${project_alias} ${project_alias})
endmacro(macro_declare_project)

# ---------------------------------------------------------------------------
# Description : This function calls service interface code generator to
#               generate codes, includes the generated codes either in the
#               new static library, if it does not exist, or adds to the
#               list to the static library, if the library already exists.
#               As parameters, the function receives the name of the static library,
#               the root folder of the source codes, the relative path to the
#               project or service interface files, which is included in the path
#               of generate files, the sub-directory relative to "relative path",
#               where the service interface files are located, but the path will not
#               included in file paths of generate files, and the name of service interface,
#               which should be the same as the file name of the service interface.
#               For example if developer creates a service interface files of
#               HelloWorld.siml and WeHaveFun.siml service interfaces, by first call of 
#               addServiceInterfaceEx with project name 'fun_library', by first call of
#               addServiceInterfaceEx(fun_library ... HelloWorld), this generates files of HelloWorld
#               service interface. In second call addServiceInterfaceEx(fun_library ... WeHaveFun),
#               it generates the source files and include them in the list of existing 'fun_library' static library.
# Function ...: addServiceInterfaceEx
# Parameters .: ${gen_project_name} -- The name of the generated static library.
#               ${source_root}      -- The root directory of the sources.
#                                      All relative paths, inclusive generated, are relative to this parameter.
#               ${relative_path}    -- The relative path to the Service Interface.
#                                      Same path is used to generate files.
#               ${sub_dir}          -- The sub-directory of Service Interface file location.
#                                      The path is excluded from generated files.
#               ${interface_name}   -- The name of the Service Interface, which is the name of the Service Interface file without '.siml' extension.
# Usage ......: addServiceInterfaceEx(<static library name> <root of sources like project root> <relative path> <sub-directory like 'res'> <Service Interface name>)
# Example ....: addServiceInterfaceEx("fun_library" "~/project/my-fun/sources" "my/service/interfaces" "" FunInterface)
# ---------------------------------------------------------------------------
function(addServiceInterfaceEx gen_project_name source_root relative_path sub_dir interface_name)

    set(interface_doc)
    if (${sub_dir} STREQUAL "")
        set(interface_doc "${source_root}/${relative_path}/${interface_name}.siml")
    else()
        set(interface_doc "${source_root}/${relative_path}/${sub_dir}/${interface_name}.siml")
    endif()

    set(interface_out "${AREG_GENERATE}/${relative_path}")
    set(generate_dir "${AREG_GENERATE_DIR}/${relative_path}")
    set(succeeded 0)
    execute_process(COMMAND java -jar ${AREG_SDK_TOOLS}/codegen.jar --root=${AREG_BUILD_ROOT} --doc=${interface_doc} --target=${interface_out} RESULT_VARIABLE succeeded)
    if (NOT succeeded EQUAL 0)
        message(FATAL_ERROR "Failed to generate files for service interface ${interface_doc}, ignoring adding library. Either Java is not installed or the Service Interface does not exit.")
        return ()
    endif()

    set(proj_src)
    list(APPEND proj_src
        ${generate_dir}/private/${interface_name}ClientBase.cpp 
        ${generate_dir}/private/${interface_name}Events.cpp 
        ${generate_dir}/private/${interface_name}Proxy.cpp 
        ${generate_dir}/private/${interface_name}Stub.cpp 
        ${generate_dir}/private/NE${interface_name}.cpp 
        )

    if (TARGET ${gen_project_name})
        target_sources(${gen_project_name} PRIVATE "${proj_src}")
    else()
        message(STATUS "Adding service interface library ${gen_project_name}")
        addStaticLib(${gen_project_name} "${proj_src}")
    endif()

endfunction(addServiceInterfaceEx)

# ---------------------------------------------------------------------------
# Description : This function calls service interface code generator to
#               generate codes, includes the generated codes either in the
#               new static library, if it does not exist, or adds to the
#               list to the static library, if the library already exists.
#               As parameters, the function receives the name of the static library,
#               the sub-directory relative to the 'CMakeLists.txt' file,
#               where the service interface files are located, but the path will not
#               included in file paths of generate files, and the name of service interface,
#               which should be the same as the file name of the service interface.
#               Unlike 'addServiceInterfaceEx' function, here we assume that the root directory
#               of sources is ${CMAKE_SOURCE_DIR} and the relative path is the location
#               of current 'CMakeLists.txt' file, i.e. ${CMAKE_CURRENT_LIST_DIR}.
# Function ...: addServiceInterface
# Parameters .: ${gen_project_name} -- The name of the generated static library.
#               ${sub_dir}          -- The sub-directory of Service Interface file location.
#                                      The path is excluded from generated files and relative to the location of the current 'CMakeLists.txt' file
#               ${interface_name}   -- The name of the Service Interface, which is the name of the Service Interface file without '.siml' extension.
# Usage ......: addServiceInterface(<static library name> <sub-directory like 'res'> <Service Interface name>)
# Example ....: addServiceInterface("fun_library" "resources" FunInterface)
# ---------------------------------------------------------------------------
function(addServiceInterface gen_project_name sub_dir interface_name)
    file(RELATIVE_PATH relative ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    addServiceInterfaceEx(${gen_project_name} ${CMAKE_SOURCE_DIR} ${relative} ${sub_dir} ${interface_name})
endfunction(addServiceInterface)

# ---------------------------------------------------------------------------
# Description : This macro generates the codes and adds generated files to
#               the list of sources to build 'gen_project_name' static library.
#               It assumes that the location of the Service Interface file is
#               same as the location of current 'CMakeLists.txt' and the root directory
#               of sources is ${CMAKE_SOURCE_DIR}.
# Macro ......: macro_add_service_interface
# Parameters .: ${gen_project_name} -- The name of the generated static library.
#               ${interface_name}   -- The name of the Service Interface, which is the name of the Service Interface file without '.siml' extension.
# Example ....: macro_add_service_interface(fun_library FunInterface)
# ---------------------------------------------------------------------------
macro(macro_add_service_interface gen_project_name interface_name)
    addServiceInterface(${gen_project_name} "" ${interface_name})
endmacro(macro_add_service_interface)
