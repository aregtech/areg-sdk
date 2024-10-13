# ###########################################################################
# AREG CMake functions
# Copyright 2022-2023 Aregtech
# ###########################################################################

# ---------------------------------------------------------------------------
# Description : Checks and sets the C++ standard for the project.
#               The variable 'AREG_CXX_STANDARD' must be defined before calling this macro.
#               If 'CMAKE_CXX_STANDARD' is not set, it will be assigned the value of 'AREG_CXX_STANDARD'.
#               If 'CMAKE_CXX_STANDARD' is lower than 'AREG_CXX_STANDARD', a warning is displayed.
# Macro    ...: macro_check_fix_areg_cxx_standard
# Usage ......: macro_check_fix_areg_cxx_standard()
# ---------------------------------------------------------------------------
macro(macro_check_fix_areg_cxx_standard)

    # Ensure that the required 'AREG_CXX_STANDARD' variable is defined.
    if (NOT DEFINED AREG_CXX_STANDARD)
        message(WARNING "AREG: >>> Cannot check and set C++ standard, variable 'AREG_CXX_STANDARD' is not defined.")
        return()
    endif()

    # If 'CMAKE_CXX_STANDARD' is not set, assign 'AREG_CXX_STANDARD' to it.
    if (NOT DEFINED CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD          ${AREG_CXX_STANDARD})
        set(CMAKE_CXX_EXTENSIONS        OFF)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)    
    # If the current C++ standard is less than the required 'AREG_CXX_STANDARD', issue a warning.
    elseif(${CMAKE_CXX_STANDARD} LESS ${AREG_CXX_STANDARD})
        message(WARNING "AREG: >>> AREG requires C++${AREG_CXX_STANDARD} or higher, \
                        current version is C++${CMAKE_CXX_STANDARD}. \
                        To avoid compilation errors, set 'CMAKE_CXX_STANDARD' to ${AREG_CXX_STANDARD}. \
                        Example: 'set(CMAKE_CXX_STANDARD ${AREG_CXX_STANDARD})'")
    endif()

endmacro(macro_check_fix_areg_cxx_standard)

# ---------------------------------------------------------------------------
# Description : Configures the compiler and linker options for executable applications.
#               Automatically links the AREG library, along with any additional libraries specified.
# Function ...: setAppOptions
# Parameters .: ${item}         -- The name of the executable to apply options to.
#               ${library_list} -- The list of additional libraries to link with the executable.
# Usage ......: setAppOptions(<name of executable> <list of libraries>)
# ---------------------------------------------------------------------------
function(setAppOptions item library_list)

    # Set common compile definitions for the executable
    target_compile_definitions(${item} PRIVATE ${COMMON_COMPILE_DEF})

    # Apply common compiler options, such as disabling certain warnings
    target_compile_options(${item} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")

    # Link the AREG library, additional specified libraries, and any extended or extra libraries
    target_link_libraries(${item} 
        ${AREG_PACKAGE_NAME}::aregextend   # AREG extended library
        ${library_list}                    # Custom libraries to link
        ${AREG_PACKAGE_NAME}::areg         # Core AREG library
        ${AREG_EXTENDED_LIBS}              # Extended libraries, if any
        ${AREG_LDFLAGS}                    # Additional linker flags
    )

endfunction(setAppOptions)

# ---------------------------------------------------------------------------
# Description : Creates an executable, sets its source files, applies necessary options, 
#               and links it with the provided list of libraries. 
#               The AREG library is automatically linked, so no need to specify it.
# Function ...: addExecutableEx
# Parameters .: ${target_name}      -- The name of the executable to build.
#               ${target_namespace} -- The namespace of the executable, used for aliasing (optional).
#               ${source_list}      -- The list of source files used to build the executable.
#               ${library_list}     -- The list of libraries to link with the executable.
# Usage ......: addExecutableEx(<name of executable> <namespace (optional)> <list of sources> <list of libraries>)
# ---------------------------------------------------------------------------
function(addExecutableEx target_name target_namespace source_list library_list)

    # Ensure the source list is not empty
    if (NOT source_list)
        message(FATAL_ERROR "AREG: >>> Source list for executable \'${target_name}\' is empty")
    endif()

    # Gather any additional libraries passed as arguments (ARGN)
    set(exList "${ARGN}")
    foreach(_item IN LISTS exList)
        list(APPEND library_list "${_item}")
    endforeach()

    # Create the executable with the specified source files
    add_executable(${target_name} ${source_list})

    # Optionally create an alias for the executable using the specified namespace
    if (NOT "${target_namespace}" STREQUAL "")
        add_executable(${target_namespace}::${target_name} ALIAS ${target_name})
    endif()

    # Apply compiler and linker options, including linking with AREG and additional libraries
    setAppOptions(${target_name} "${library_list}")

    # Set the include directories for the executable
    target_include_directories(${target_name} BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})

endfunction(addExecutableEx)

# ---------------------------------------------------------------------------
# Description : Creates an executable, sets its source files, applies necessary options. 
#               The AREG library is automatically linked, so no need to specify it.
# Function ...: addExecutable
# Parameters .: ${target_name}      -- The name of the executable to build.
#               ${source_list}      -- The list of source files used to build the executable.
# Usage ......: addExecutable(<name of executable> <list of sources>)
# ---------------------------------------------------------------------------
function(addExecutable target_name source_list)
    addExecutableEx(${target_name} "" "${source_list}" "")
endfunction(addExecutable)


# ---------------------------------------------------------------------------
# Description : Configures the compiler and linker options for a static library.
#               Automatically links the AREG library and any other specified libraries.
# Function ...: setStaticLibOptions
# Parameters .: ${item}         -- The name of the static library to apply options to.
#               ${library_list} -- The list of libraries to link with the static library.
# Usage ......: setStaticLibOptions(<name of static library> <list of libraries>) 
# ---------------------------------------------------------------------------
function(setStaticLibOptions item library_list)

    # Apply common compile definitions and options for static libraries
    target_compile_definitions(${item} PRIVATE ${COMMON_COMPILE_DEF} _LIB)
    target_compile_options(${item} PRIVATE ${AREG_COMPILER_VERSION})
    target_compile_options(${item} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")

    # Additional compile options for non-Windows platforms
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(${item} PRIVATE "-Bstatic")  # Ensure static linking
        target_compile_options(${item} PRIVATE -fPIC)       # Position-independent code
    endif()

    # Link the static library with the provided libraries and AREG framework
    target_link_libraries(${item} ${library_list} ${AREG_PACKAGE_NAME}::areg ${AREG_LDFLAGS})

endfunction(setStaticLibOptions)

# ---------------------------------------------------------------------------
# Description : Creates a static library, sets its source files, applies necessary options, 
#               and links it with the provided list of libraries.
#               The AREG library is automatically linked, so no need to specify it.
# Function ...: addStaticLibEx
# Parameters .: ${target_name}      -- The name of the static library to build.
#               ${target_namespace} -- The namespace of the static library (optional for aliasing).
#               ${source_list}      -- The list of source files used to build the static library.
#               ${library_list}     -- The list of libraries to link with the static library.
# Usage ......: addStaticLibEx(<name of static library> <namespace (optional)> <list of sources> <list of libraries>)
# ---------------------------------------------------------------------------
function(addStaticLibEx target_name target_namespace source_list library_list)

    # Gather any additional libraries passed as arguments (ARGN)
    set(exList "${ARGN}")
    foreach(_item IN LISTS exList)
        list(APPEND library_list "${_item}")
    endforeach()

    # Create the static library with the specified source files
    add_library(${target_name} STATIC ${source_list})

    # Optionally create an alias for the static library using the specified namespace
    if (NOT "${target_namespace}" STREQUAL "")
        add_library(${target_namespace}::${target_name} ALIAS ${target_name})
    endif()

    # Apply compiler and linker options, including linking with AREG and additional libraries
    setStaticLibOptions(${target_name} "${library_list}")

    # Set the include directories for the static library
    target_include_directories(${target_name} BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})

endfunction(addStaticLibEx)

# ---------------------------------------------------------------------------
# Description : Creates a static library, sets its source files, applies necessary options.
#               The AREG library is automatically linked, so no need to specify it.
# Function ...: addStaticLib
# Parameters .: ${target_name}      -- The name of the static library to build.
#               ${source_list}      -- The list of source files used to build the static library.
# Usage ......: addStaticLib(<name of static library> <list of sources>)
# ---------------------------------------------------------------------------
function(addStaticLib target_name source_list)
    addStaticLibEx(${target_name} "" "${source_list}" "")
endfunction(addStaticLib)

# ---------------------------------------------------------------------------
# Description : Creates a static library compiled with C-compiler,
#               sets its source files, applies necessary options, and links it
#               with the provided list of libraries.
#               The AREG library is automatically linked, so no need to specify it.
# Function ...: addStaticLibEx_C
# Parameters .: ${target_name}      -- The name of the static library to build.
#               ${target_namespace} -- The namespace of the static library (optional for aliasing).
#               ${source_list}      -- The list of C-code source files used to build the static library.
#               ${library_list}     -- The list of libraries to link with the static library.
# Usage ......: addStaticLibEx_C(<name of static library> <namespace (optional)> <list of C-code sources> <list of libraries>)
# ---------------------------------------------------------------------------
function(addStaticLibEx_C target_name target_namespace source_list library_list)
    set(exList "${ARGN}")
    foreach(_item IN LISTS exList)
        list(APPEND library_list "${_item}")
    endforeach()
    add_library(${target_name} STATIC ${source_list})
    if (NOT "${target_namespace}" STREQUAL "")
        add_library(${target_namespace}::${target_name} ALIAS ${target_name})
    endif()

    target_compile_options(${target_name} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")

    # Set common compile definition
    target_compile_definitions(${target_name} PRIVATE ${COMMON_COMPILE_DEF} _LIB)

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(${target_name} PRIVATE "-Bstatic")
        target_compile_options(${target_name} PRIVATE -fPIC)
    endif()

    target_link_libraries(${target_name} ${library_list} ${AREG_PACKAGE_NAME}::areg ${AREG_LDFLAGS})
endfunction(addStaticLibEx_C)

# ---------------------------------------------------------------------------
# Description : Adds static library compiled with C-compiler, sets the sources and
#               the options of library. The AREG library is automatically added.
# Function ...: addStaticLib_C
# Parameters .: ${target_name}  -- The name of the static library to build.
#               ${source_list}  -- The list of the sources to build the static library.
# usage ......: addStaticLib_C( <name of static library> <list of C-sources> ) 
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Description : Creates a static library compiled with C-compiler,
#               sets its source files, applies necessary options.
#               The AREG library is automatically linked, so no need to specify it.
# Function ...: addStaticLib_C
# Parameters .: ${target_name}      -- The name of the static library to build.
#               ${source_list}      -- The list of C-code source files used to build the static library.
# Usage ......: addStaticLib_C(<name of static library> <list of C-code sources>)
# ---------------------------------------------------------------------------
function(addStaticLib_C target_name source_list)
    addStaticLibEx_C(${target_name} "" "${source_list}" "")
endfunction(addStaticLib_C)

# ---------------------------------------------------------------------------
# Description : Configures the compiler and linker options for a shared library.
#               Automatically links the AREG library and any other specified libraries.
# Function ...: setSharedLibOptions
# Parameters .: ${item}         -- The name of the shared library to apply options to.
#               ${library_list} -- The list of libraries to link with the shared library.
# Usage ......: setSharedLibOptions(<name of shared library> <list of libraries>) 
# ---------------------------------------------------------------------------
function(setSharedLibOptions item library_list)

    # Apply common compile definitions for shared libraries
    target_compile_definitions(${item} PRIVATE ${COMMON_COMPILE_DEF} _USRDLL)
    target_compile_options(${item} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")

    # Link the shared library with provided libraries and AREG framework
    target_link_libraries(${item} ${AREG_PACKAGE_NAME}::aregextend ${library_list} ${AREG_PACKAGE_NAME}::areg ${AREG_EXTENDED_LIBS} ${AREG_LDFLAGS})

    # Additional compile options for non-Windows platforms
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(${item} PRIVATE -fPIC)       # Position-independent code for shared libraries
        target_compile_options(${item} PRIVATE "-Bdynamic") # Ensure dynamic linking
    endif()

endfunction(setSharedLibOptions)

# ---------------------------------------------------------------------------
# Description : Creates a shared library, sets its source files, applies necessary options, 
#               and links it with the provided list of libraries.
#               The AREG library is automatically linked, so no need to specify it.
# Function ...: addSharedLibEx
# Parameters .: ${target_name}      -- The name of the shared library to build.
#               ${target_namespace} -- The namespace of the shared library (optional for aliasing).
#               ${source_list}      -- The list of source files used to build the shared library.
#               ${library_list}     -- The list of libraries to link with the shared library.
# Usage ......: addSharedLibEx(<name of shared library> <namespace (optional)> <list of sources> <list of libraries>)
# ---------------------------------------------------------------------------
function(addSharedLibEx target_name target_namespace source_list library_list)

    # Gather any additional libraries passed as arguments (ARGN)
    set(exList "${ARGN}")
    foreach(_item IN LISTS exList)
        list(APPEND library_list "${_item}")
    endforeach()

    # Create the shared library with the specified source files
    add_library(${target_name} SHARED ${source_list})

    # Optionally create an alias for the shared library using the specified namespace
    if (NOT "${target_namespace}" STREQUAL "")
        add_library(${target_namespace}::${target_name} ALIAS ${target_name})
    endif()

    # Apply compiler and linker options, including linking with AREG and additional libraries
    setSharedLibOptions(${target_name} "${library_list}")

    # Set the include directories for the shared library
    target_include_directories(${target_name} BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})

endfunction(addSharedLibEx)

# ---------------------------------------------------------------------------
# Description : Creates a shared library, sets its source files, applies necessary options.
#               The AREG library is automatically linked, so no need to specify it.
# Function ...: addSharedLib
# Parameters .: ${target_name}      -- The name of the shared library to build.
#               ${source_list}      -- The list of source files used to build the shared library.
# Usage ......: addSharedLib(<name of shared library> <list of sources>)
# ---------------------------------------------------------------------------
function(addSharedLib target_name target_source_list)
    addSharedLibEx(${target_name} "" "${target_source_list}" "")
endfunction(addSharedLib)

# ---------------------------------------------------------------------------
# Description : Converts Windows-specific paths to Cygwin format if running under Cygwin.
#               Otherwise, the path remains unchanged.
# Note .......: This macro does not address OS-specific path separator issues.
# Parameters .: ${normal_path} -- The normalized path (output).
#               ${os_path}     -- The OS-specific path to normalize.
# Macro ......: macro_normalize_path
# Usage ......: macro_normalize_path(<result_variable> <OS specific path>)
# ---------------------------------------------------------------------------
macro(macro_normalize_path normal_path os_path)
    if (CYGWIN)
        execute_process(COMMAND cygpath.exe -m "${os_path}" OUTPUT_VARIABLE _normalized_path OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(${normal_path} "${_normalized_path}")
    else()
        set(${normal_path} "${os_path}")
    endif()
endmacro(macro_normalize_path)

# ---------------------------------------------------------------------------
# Description : This function invokes the service interface code generator 
#               to produce source files and either adds them to a new static 
#               library (if it doesn't exist) or includes them in an existing 
#               one. It accepts the static library name, full paths to 
#               Service Interface file, Service Interface name and paths for generated files.
#               The service interface name must match the file name (without the .siml extension).
#               
#               Example: 
#               For a project 'fun_library' with 'HelloWorld.siml' and 'WeHaveFun.siml' interfaces, 
#               calling macro_add_service_interface("fun_library" ... HelloWorld ...) generates 
#               source files for HelloWorld and includes them in the static library. 
#               A subsequent call for WeHaveFun adds it to the same library.
# 
# Macro ......: macro_add_service_interface
# Parameters .: ${lib_name}         -- Name of the static library.
#               ${interface_doc}    -- Full path to the Service Interface document file. 
#               ${interface_name}   -- Name of the Service Interface (without '.siml').
#               ${codegen_root}     -- Root directory for generating files.
#               ${output_path}      -- Relative path for generated files.
#               ${codegen_tool}     -- Full path to the code generator tool.
# 
# Usage ......: macro_add_service_interface(<library name> <documend dir><interface name>.siml <interface name> <codegen root> <sub-path> <codegen tool>)
# Example ....: macro_add_service_interface("fun_library" "~/project/my-fun/sources/service/interfaces/FunInterface.siml" FunInterface "~/project/my-fun/" "generate/service/interfaces" /tools/areg/codegen.jar)
# ---------------------------------------------------------------------------
macro(macro_add_service_interface lib_name interface_doc interface_name codegen_root output_path codegen_tool)

    if (NOT ${Java_FOUND})
        message(FATAL_ERROR "AREG Setup: Java not found! Install Java 17 or higher to run the code generator.")
        return()
    endif()

    # Run the code generator tool
    execute_process(COMMAND ${Java_JAVA_EXECUTABLE} -jar ${codegen_tool} --root=${codegen_root} --doc=${interface_doc} --target=${output_path})

    # Set path for generated files
    set(_generate "${codegen_root}/${output_path}")
    
    # List of generated source and header files
    list(APPEND _sources
        ${_generate}/private/${interface_name}ClientBase.cpp
        ${_generate}/private/${interface_name}Events.cpp
        ${_generate}/private/${interface_name}Proxy.cpp
        ${_generate}/private/${interface_name}Stub.cpp
        ${_generate}/private/NE${interface_name}.cpp
        ${_generate}/private/${interface_name}Events.hpp
        ${_generate}/private/${interface_name}Proxy.hpp
        ${_generate}/${interface_name}ClientBase.hpp
        ${_generate}/${interface_name}Stub.hpp
        ${_generate}/NE${interface_name}.hpp
    )

    # Add generated files to an existing or new static library
    if (TARGET ${lib_name})
        target_sources(${lib_name} PRIVATE "${_sources}")
    else()
        message(STATUS "AREG Setup: Adding new service interface library ${lib_name}")
        addStaticLib(${lib_name} "${_sources}")
        target_compile_options(${lib_name} PRIVATE "${AREG_OPT_DISABLE_WARN_CODEGEN}")
    endif()

    unset(_generate)
    unset(_sources)

endmacro(macro_add_service_interface)

# ---------------------------------------------------------------------------
# Description : Generates code for a service interface using a code generator and includes
#               the generated files in a static library.
# Function ...: addServiceInterfaceEx
# Parameters .: ${lib_name}       -- The static library name.
#               ${source_root}    -- The root directory of the source files.
#               ${relative_path}  -- The relative path to the Service Interface files.
#               ${sub_dir}        -- Optional sub-directory within relative_path (can be empty).
#               ${interface_name} -- The name of the Service Interface (without the '.siml' extension).
# Usage ......: addServiceInterfaceEx(<static library> <source root> <relative path> <sub-directory> <interface name>)
# Example ....: addServiceInterfaceEx("fun_library" "/home/develop/project/my-fun/sources" "my/service/interfaces" "" FunInterface)
# ---------------------------------------------------------------------------
function(addServiceInterfaceEx lib_name source_root relative_path sub_dir interface_name)

    macro_normalize_path(codegen_root   "${AREG_BUILD_ROOT}")
    macro_normalize_path(output_path    "${AREG_GENERATE}/${relative_path}")
    macro_normalize_path(codegen_tool   "${AREG_SDK_TOOLS}/codegen.jar")

    if (sub_dir STREQUAL "")
        macro_normalize_path(interface_doc "${source_root}/${relative_path}/${interface_name}.siml")
    else()
        macro_normalize_path(interface_doc "${source_root}/${relative_path}/${sub_dir}/${interface_name}.siml")
    endif()

    macro_add_service_interface(${lib_name}
                                ${interface_doc}
                                ${interface_name}
                                "${codegen_root}"
                                "${output_path}"
                                "${codegen_tool}")
endfunction(addServiceInterfaceEx)

# ---------------------------------------------------------------------------
# Description : Wrapper for addServiceInterfaceEx, with the source root assumed
#               to be ${CMAKE_SOURCE_DIR} and the relative path ${CMAKE_CURRENT_LIST_DIR}.
# Function ...: addServiceInterface
# Parameters .: ${lib_name}       -- The name of the static library.
#               ${sub_dir}        -- The sub-directory of the service interface files.
#               ${interface_name} -- The name of the Service Interface (without the '.siml' extension).
# Usage ......: addServiceInterface(<static library> <sub-directory> <Service Interface name>)
# ---------------------------------------------------------------------------
function(addServiceInterface lib_name sub_dir interface_name)
    file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    addServiceInterfaceEx(${lib_name} ${CMAKE_SOURCE_DIR} ${relative_path} ${sub_dir} ${interface_name})
endfunction(addServiceInterface)

# ---------------------------------------------------------------------------
# Description : Searches for a package and sets output variables to indicate the package's
#               include directories and libraries if found.
# Macro ......: macro_find_package
# Parameters .: ${package_name}      -- The package name to search.
#               ${package_found}     -- Output variable, set to TRUE if the package is found.
#               ${package_includes}  -- Output variable, set to the package's include directories (if any).
#               ${package_libraries} -- Output variable, set to the package's libraries (if any).
# Usage ......: macro_find_package(<package> <found flag> <includes> <libraries>)
# Example ....: macro_find_package(SQLite3 SQLITE_FOUND SQLITE_INCLUDE SQLITE_LIB)
# ---------------------------------------------------------------------------
macro(macro_find_package package_name package_found package_includes package_libraries)
    find_package(${package_name})
    if (${package_name}_FOUND)
        set(${package_found} TRUE)
        set(${package_includes}  "${${package_name}_INCLUDE_DIRS}")
        set(${package_libraries} "${${package_name}_LIBRARIES}")
    else()
        set(${package_found} FALSE)
        set(${package_includes}  "")
        set(${package_libraries} "")
    endif()
endmacro(macro_find_package)

# ---------------------------------------------------------------------------
# Description : Creates or updates a boolean cache variable in CMake, ensuring it is defined and set correctly.
# Macro ......: macro_create_option
# Parameters .: ${var_name}     -- The name of the boolean variable.
#               ${var_value}    -- The default value if the variable is not yet defined.
#               ${var_describe} -- A brief description of the variable for CMake cache.
# Usage ......: macro_create_option(<var_name> <default_value> <description>)
# Example ....: macro_create_option(AREG_LOGS ON "Compile with logs")
# ---------------------------------------------------------------------------
macro(macro_create_option var_name var_value var_describe)
    if (NOT DEFINED ${var_name})
        set(${var_name} ${var_value} CACHE BOOL "${var_describe}" FORCE)
    else()
        set(${var_name} ${${var_name}} CACHE BOOL "${var_describe}" FORCE)
    endif()
endmacro(macro_create_option)

# ---------------------------------------------------------------------------
# Description : Recursively removes empty directories.
# Function ...: removeEmptyDirs
# Parameters .: ${dir_name} -- The directory path to check and potentially remove.
# Usage ......: removeEmptyDirs(<directory path>)
# ---------------------------------------------------------------------------
function(removeEmptyDirs dir_name)
    if (EXISTS "${dir_name}" AND IS_DIRECTORY "${dir_name}")
        file(GLOB entries "${dir_name}/*")
        if ("${entries}" STREQUAL "")
            file(REMOVE_RECURSE "${dir_name}")
        else()
            foreach(entry IN LISTS entries)
                removeEmptyDirs("${entry}")
            endforeach()
            
            # Check again if the directory is empty after recursion.
            file(GLOB entries "${dir_name}/*")
            if ("${entries}" STREQUAL "")
                file(REMOVE_RECURSE "${dir_name}")
            endif()
        endif()
    endif()
endfunction(removeEmptyDirs)

# ---------------------------------------------------------------------------
# Description : Adds source files to a list, checking if the files exist relative to the base directory.
# Macro ......: macro_add_source
# Parameters .: ${result_list}  -- The output list of source files.
#               ${src_base_dir} -- The base directory for the source files.
#               ${ARGN}         -- The list of source files (relative to the base directory).
# Usage ......: set(aregextend_SRC)
#               macro_add_source(aregextend_SRC "${AREG_FRAMEWORK}" aregextend/db/private/LogSqliteDatabase.cpp ...)
# ---------------------------------------------------------------------------
macro(macro_add_source result_list src_base_dir)
    set(_list "${ARGN}")
    foreach(_item IN LISTS _list)
        set(_src "${src_base_dir}/${_item}")
        if (EXISTS "${_src}")
            list(APPEND ${result_list} "${_src}")
        else()
            message(FATAL_ERROR "AREG: >>> The item '${_item}' does not exist in '${src_base_dir}'")
        endif()
    endforeach()
    unset(_list)
endmacro(macro_add_source)


# ---------------------------------------------------------------------------
# Description : This macro processes a list of input arguments that can include:
#                   1. **CMake targets** (predefined libraries or targets).
#                   2. **Source files**: Files provided with either absolute paths or 
#                      paths relative to the current directory.
#                   3. **Resource files** (*.rc): Windows-specific resource files.
#               The macro categorizes the input arguments into three separate lists:
#                   1. **Libraries list**: Contains any known CMake targets.
#                   2. **Source files list**: Includes valid source files (.cpp, .c) 
#                      from either the provided absolute paths or relative paths.
#                   3. **Resource files list**: Filters out .rc files, specifically for Windows,
#                      and stores them in a separate list.
#
# Behavior ...: 
#               - If a file does not exist, either as a full path or relative to the current directory, 
#                 the macro throws a fatal error.
#               - On Windows, it specifically identifies and appends resource files (*.rc) to the 
#                 resources list.
#
# Macro ......: macro_parse_arguments
# Parameters .: ${res_sources}   -- Output: A list containing the parsed source files.
#               ${res_libs}      -- Output: A list containing the recognized CMake targets (libraries).
#               ${res_resources} -- Output: A list containing the identified resource files (*.rc).
#               ${ARGN}          -- Input: A list of files, libraries, or resources to be categorized.
#
# Usage ......: macro_parse_arguments(<var sources> <var libs> <var resources> my_target my/app/main.cpp my/lib/object.cpp my/resource/resource.rc)
#               Example: macro_parse_arguments(src_files lib_targets res_files my_lib src/main.cpp src/object.cpp res/resource.rc)
# ---------------------------------------------------------------------------
macro(macro_parse_arguments res_sources res_libs res_resources)
    set(_list "${ARGN}")
    foreach(_item IN LISTS _list)
        # Check if the _item is a known CMake target
        if (TARGET ${_item})
            list(APPEND ${res_libs} ${_item})
        # Check if the _item is an existing file, relative or full path
        elseif (EXISTS "${_item}")
            list(APPEND ${res_sources} "${_item}")
        elseif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_item}")
            list(APPEND ${res_sources} "${CMAKE_CURRENT_LIST_DIR}/${_item}")
        else()
            message(FATAL_ERROR "AREG: >>> File \'${_item}\' does not exist, ignoring")
        endif()
    endforeach()

    # Separate out resource files (*.rc) for setting their properties on Windows
    foreach(_file IN LISTS ${res_sources})
        cmake_path(GET _file EXTENSION _ext)
        if (_ext STREQUAL "rc")
            list(APPEND ${res_resources} "${_file}")
        endif()
    endforeach()

endmacro(macro_parse_arguments)

# ---------------------------------------------------------------------------
# Description : This macro declares a static library by processing the provided list of
#               arguments and categorizing them into three main groups:
#                   1. **Source Files**: These are the .cpp or .c files used to build the library.
#                      The macro supports both absolute paths and paths relative to the current directory.
#                   2. **Libraries**: These are existing CMake targets (predefined libraries) 
#                      that the static library depends on.
#                   3. **Resource Files**: These are .rc files (Windows resource files). 
#                      On Windows systems, the macro ensures they are processed using the appropriate RC language settings.
#               The macro declares a static library target using the collected source files and linked libraries.
#               It also handles resource file configuration on Windows platforms.
#
# Notes ......: 
#               - Throws a fatal error if no source files are provided.
#               - On Windows, resource files (*.rc) are set to use the RC language automatically.
# 
# Macro ......: macro_declare_static_library
# Parameters .: ${lib_name}  -- The name of the static library to be declared.
#               ${ARGN}      -- The list of source files, libraries, and resource files.
#                               The files can be specified with full or relative paths.
#
# Usage ......: macro_declare_static_library(<lib_name> <list_of_sources_libraries_and_resources>)
#               Example: macro_declare_static_library(myStaticLib src/main.cpp src/resource.rc libSomeDependency)
# ---------------------------------------------------------------------------
macro(macro_declare_static_library lib_name)

    # Parse arguments to get sources, libraries, and resources
    macro_parse_arguments(_sources _libs _resources "${ARGN}")

    # Ensure the source list is not empty
    if (NOT _sources)
        message(FATAL_ERROR "AREG: >>> Source list for executable \'${exe_name}\' is empty")
    endif()

    # Declare the static library using gathered sources and libraries
    addStaticLibEx(${lib_name} "" "${_sources}" "${_libs}")

    # If on Windows, set the RC files' language property
    if (AREG_DEVELOP_ENV MATCHES "Win32" AND _resources)
        set_source_files_properties(${_resources} PROPERTIES LANGUAGE RC)
    endif()

    # Clean up temporary variables
    unset(_sources)
    unset(_libs)
    unset(_resources)

endmacro(macro_declare_static_library)

# ---------------------------------------------------------------------------
# Description : This macro declares a shared library by processing the provided list of
#               arguments and categorizing them into three main groups:
#                   1. **Source Files**: These are the .cpp or .c files used to build the library.
#                      The macro supports both absolute paths and paths relative to the current directory.
#                   2. **Libraries**: These are existing CMake targets (predefined libraries) 
#                      that the shared library depends on.
#                   3. **Resource Files**: These are .rc files (Windows resource files). 
#                      On Windows systems, the macro ensures they are processed using the appropriate RC language settings.
#               The macro declares a shared library target using the collected source files and linked libraries.
#               It also handles resource file configuration on Windows platforms.
#
# Notes ......: 
#               - Throws a fatal error if no source files are provided.
#               - On Windows, resource files (*.rc) are set to use the RC language automatically.
# 
# Macro ......: macro_declare_shared_library
# Parameters .: ${lib_name}  -- The name of the shared library to be declared.
#               ${ARGN}      -- The list of source files, libraries, and resource files.
#                               The files can be specified with full or relative paths.
#
# Usage ......: macro_declare_static_library(<lib_name> <list_of_sources_libraries_and_resources>)
#               Example: macro_declare_static_library(myStaticLib src/main.cpp src/resource.rc libSomeDependency)
# ---------------------------------------------------------------------------
macro(macro_declare_shared_library lib_name)

    # Parse arguments to get sources, libraries, and resources
    macro_parse_arguments(_sources _libs _resources "${ARGN}")

    # Ensure the source list is not empty
    if (NOT _sources)
        message(FATAL_ERROR "AREG: >>> Source list for executable \'${exe_name}\' is empty")
    endif()

    # Declare the shared library using gathered sources and libraries
    addSharedLibEx(${lib_name} "" "${_sources}" "${_libs}")

    # If on Windows, set the RC files' language property
    if (AREG_DEVELOP_ENV MATCHES "Win32" AND _resources)
        set_source_files_properties(${_resources} PROPERTIES LANGUAGE RC)
    endif()

    # Clean up temporary variables
    unset(_sources)
    unset(_libs)
    unset(_resources)

endmacro(macro_declare_shared_library)

# ---------------------------------------------------------------------------
# Description : This macro declares an executable by processing the provided list of
#               arguments and categorizing them into three main groups:
#                   1. **Source Files**: These are the .cpp or .c files used to build the library.
#                      The macro supports both absolute paths and paths relative to the current directory.
#                   2. **Libraries**: These are existing CMake targets (predefined libraries) 
#                      that the executable depends on.
#                   3. **Resource Files**: These are .rc files (Windows resource files). 
#                      On Windows systems, the macro ensures they are processed using the appropriate RC language settings.
#               The macro declares a executable target using the collected source files and linked libraries.
#               It also handles resource file configuration on Windows platforms.
#
# Notes ......: 
#               - Throws a fatal error if no source files are provided.
#               - On Windows, resource files (*.rc) are set to use the RC language automatically.
# 
# Macro ......: macro_declare_executable
# Parameters  : ${lib_name}  -- The name of the executable to be declared.
#               ${ARGN}      -- The list of source files, libraries, and resource files.
#                               The files can be specified with full or relative paths.
#
# Usage ......: macro_declare_static_library(<lib_name> <list_of_sources_libraries_and_resources>)
#               Example: macro_declare_static_library(myStaticLib src/main.cpp src/resource.rc libSomeDependency)
# ---------------------------------------------------------------------------
macro(macro_declare_executable exe_name)

    # Parse arguments to get sources, libraries, and resources
    macro_parse_arguments(_sources _libs _resources "${ARGN}")

    # Ensure the source list is not empty
    if (NOT _sources)
        message(FATAL_ERROR "AREG: >>> Source list for executable \'${exe_name}\' is empty")
    endif()

    # Declare the executable using gathered sources and libraries
    addExecutableEx(${exe_name} "" "${_sources}" "${_libs}")

    # If on Windows, set the RC files' language property
    if (AREG_DEVELOP_ENV MATCHES "Win32" AND _resources)
        set_source_files_properties(${_resources} PROPERTIES LANGUAGE RC)
    endif()

    # Clean up temporary variables
    unset(_sources)
    unset(_libs)
    unset(_resources)

endmacro(macro_declare_executable)

# ---------------------------------------------------------------------------
# Description : This macro identifies the compiler family (e.g., GNU, Clang, MSVC),
#               extracts a short name for the compiler, and attempts to guess the corresponding
#               C compiler based on the provided C++ compiler path or name. It handles cases
#               where only the compiler name is provided (assuming it is available in the system's PATH).
#               If a match is found, it sets the output variables for the compiler family, short name,
#               C++ compiler path, and C compiler path, and marks the process as successful.
#               Otherwise, the output flag indicates failure.
#
# Note .......: Beside "gnu", "llvm", "msvc", the compilers for CYGWIN are included in the family "cygwin"
#               to identify that the application is compiled in Windows under CYGWIN platform with GNU compilers.
# Macro ......: macro_declare_executable
# Parameters .: 
#               - ${compiler_path}   -- Input: The name or full path of the C++ compiler executable.
#               - ${compiler_family} -- Output: Variable to hold the identified compiler family (e.g., "gnu", "msvc").
#               - ${compiler_short}  -- Output: Variable to hold the short name of the compiler (e.g., "gcc", "clang").
#               - ${compiler_cxx}    -- Output: Variable to hold the C++ compiler path (usually same as ${compiler_path}).
#               - ${compiler_c}      -- Output: Variable to hold the corresponding C compiler name or path.
#               - ${compiler_found}  -- Output: Boolean flag indicating if the compiler was successfully identified ("TRUE" or "FALSE").
#
# Usage ......: macro_setup_compilers_data("${CMAKE_CXX_COMPILER}" 
#                                           AREG_COMPILER_FAMILY 
#                                           AREG_COMPILER_SHORT 
#                                           AREG_CXX_COMPILER 
#                                           AREG_C_COMPILER 
#                                           _compiler_found
#                                         )
# ---------------------------------------------------------------------------
macro(macro_setup_compilers_data compiler_path compiler_family compiler_short compiler_cxx compiler_c compiler_found)

    set(${compiler_found} FALSE)
    
    # Iterate over known compilers to identify the compiler type
    foreach(_entry "clang-cl;llvm;clang-cl" "clang++;llvm;clang" "clang;llvm;clang" "g++;gnu;gcc" "gcc;gnu;gcc" "c++;gnu;cc" "cc;gnu;cc" "cl;msvc;cl")
        list(GET _entry 0 _cxx_compiler)
        list(GET _entry 1 _family)
        list(GET _entry 2 _c_compiler)

        # Check if the provided compiler matches the known C++ compiler
        string(FIND "${compiler_path}" "${_cxx_compiler}" _found_pos REVERSE)
        if (_found_pos GREATER -1)
            # Handle special case for CYGWIN and GNU family compilers
            if (CYGWIN AND ("${_family}" STREQUAL "gnu"))
                set(${compiler_family} "cygwin")
            else()
                set(${compiler_family} "${_family}")
            endif()

            set(${compiler_short} "${_cxx_compiler}")
            set(${compiler_cxx}   "${compiler_path}")

            # Determine the corresponding C compiler path or name
            if ("${_cxx_compiler}" STREQUAL "${_c_compiler}")
                set(${compiler_c} "${compiler_path}")
            else()
                string(REPLACE "${_cxx_compiler}" "${_c_compiler}" ${compiler_c} "${compiler_path}")
            endif()

            # Mark compiler as found
            set(${compiler_found} TRUE)

            # break the loop, we have found
            break()
        endif()
    endforeach()

endmacro(macro_setup_compilers_data)

# ---------------------------------------------------------------------------
# Description : This macro identifies the short name of the C++ and C compilers
#               based on the provided compiler family (e.g., "gnu", "msvc", "llvm", "cygwin").
#               It assumes that the compiler executables are available in the system's PATH.
#               Once a match is found for the compiler family, it sets the output variables 
#               for the short name, C++ compiler name, and C compiler name, and marks the process as successful.
#               If no match is found, it sets the output flag to "FALSE".
#
# Note .......: The "cygwin" family is supported for GNU compilers on the CYGWIN platform in Windows.
#
# Macro ......: macro_setup_compilers_data_by_family
# Parameters .: 
#               - ${compiler_family} -- Input: The name of the compiler family (e.g., "gnu", "msvc").
#               - ${compiler_short}  -- Output: Variable to hold the short name of the compiler (e.g., "gcc", "clang").
#               - ${compiler_cxx}    -- Output: Variable to hold the C++ compiler name.
#               - ${compiler_c}      -- Output: Variable to hold the corresponding C compiler name.
#               - ${compiler_found}  -- Output: Boolean flag indicating whether the compiler was successfully identified ("TRUE" or "FALSE").
#
# Usage ......: macro_setup_compilers_data_by_family("gnu"
#                                                    AREG_COMPILER_SHORT 
#                                                    AREG_CXX_COMPILER 
#                                                    AREG_C_COMPILER 
#                                                    _compiler_found
#                                                   )
# ---------------------------------------------------------------------------
macro(macro_setup_compilers_data_by_family compiler_family compiler_short compiler_cxx compiler_c compiler_found)

    set(${compiler_found} FALSE)
    
    # Iterate over known compilers and match the family
    foreach(_entry "clang++;llvm;clang" "g++;gnu;gcc" "cl;msvc;cl" "g++;cygwin;gcc")
        list(GET _entry 0 _compiler)
        list(GET _entry 1 _family)
        list(GET _entry 2 _c_compiler)

        if ("${_family}" STREQUAL "${compiler_family}")
            # Special case for Windows
            if (WIN32 AND "${_family}" STREQUAL "llvm")
                set(${compiler_short} "clang-cl")
                set(${compiler_cxx}   "clang-cl")
                set(${compiler_c}     "clang-cl")
            else()
                set(${compiler_short} "${_compiler}")
                set(${compiler_cxx}   "${_compiler}")
                set(${compiler_c}     "${_c_compiler}")
            endif()

            # Mark compiler as found
            set(${compiler_found} TRUE)

            # break the loop, we have found
            break()
        endif()
    endforeach()

endmacro(macro_setup_compilers_data_by_family)

# ---------------------------------------------------------------------------
# Description : This function prints a detailed status report for the AREG project's CMake configuration.
#               It provides an overview of the build environment, compiler, output directories, 
#               and relevant configuration options such as installed packages and libraries.
#
# Function ...: printAregConfigStatus
# Parameters .: 
#               - ${var_make_print} -- Boolean flag indicating whether to print the status message. 
#                                      If FALSE, the function exits without printing.
#               - ${var_prefix}     -- A prefix added to each line of the status message (e.g., project name or custom label).
#               - ${var_header}     -- A custom header message displayed at the beginning of the status report.
#               - ${var_footer}     -- A custom footer message displayed at the end of the status report.
#
# Usage ......: printAregConfigStatus(
#                                   TRUE
#                                   "AREG"
#                                   "----------------------> AREG project CMake Status Report Begin <-----------------------"
#                                   "-----------------------> AREG project CMake Status Report End <------------------------"
#                                   )
# ---------------------------------------------------------------------------
function(printAregConfigStatus var_make_print var_prefix var_header var_footer)
    # If the flag is false, skip printing the status
    if (NOT ${var_make_print})
        return()
    endif()

    # Print the header section with separators
    message(STATUS "=======================================================================================")
    message(STATUS "${var_header}")
    message(STATUS "=======================================================================================")

    # Print detailed configuration status, each with the defined prefix
    message(STATUS "${var_prefix}: >>> CMAKE_SOURCE_DIR    = '${CMAKE_SOURCE_DIR}', build type '${CMAKE_BUILD_TYPE}'")
    message(STATUS "${var_prefix}: >>> Build Environment ..: System '${CMAKE_SYSTEM_NAME}', ${AREG_BITNESS}-bit platform, '${AREG_PROCESSOR}' CPU")
    message(STATUS "${var_prefix}: >>> Compiler ...........: '${CMAKE_CXX_COMPILER}'")
    message(STATUS "${var_prefix}: >>> Compiler Version ...: C++ standard 'c++${CMAKE_CXX_STANDARD}', compiler family '${AREG_COMPILER_FAMILY}'")
    message(STATUS "${var_prefix}: >>> Binary Output Dir ..: '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'")
    message(STATUS "${var_prefix}: >>> Generated Files Dir : '${AREG_GENERATE_DIR}'")
    message(STATUS "${var_prefix}: >>> Packages Dir .......: '${FETCHCONTENT_BASE_DIR}'")
    message(STATUS "${var_prefix}: >>> Build Modules ......: areg = '${AREG_BINARY}', aregextend = static, areglogger = '${AREG_LOGGER_LIB}', executable extension '${CMAKE_EXECUTABLE_SUFFIX}'")
    message(STATUS "${var_prefix}: >>> Java Version .......: '${Java_VERSION_STRING}', Java executable = '${Java_JAVA_EXECUTABLE}', minimum version required = 17")
    message(STATUS "${var_prefix}: >>> Packages Use .......: SQLite3 package use = '${AREG_SQLITE_PACKAGE}', GTest package use = '${AREG_GTEST_PACKAGE}'")
    message(STATUS "${var_prefix}: >>> Other Options ......: Examples = '${AREG_BUILD_EXAMPLES}', Unit Tests = '${AREG_BUILD_TESTS}', AREG Extended = '${AREG_EXTENDED}', Logs = '${AREG_LOGS}'")
    message(STATUS "${var_prefix}: >>> Installation .......: Enabled = '${AREG_INSTALL}', location = '${CMAKE_INSTALL_PREFIX}'")

    # Print the footer section with separators
    message(STATUS "=======================================================================================")
    message(STATUS "${var_footer}")
    message(STATUS "=======================================================================================")

endfunction(printAregConfigStatus)
