# ###########################################################################
# AREG CMake functions
# Copyright 2022-2023 Aregtech
# ###########################################################################

# ---------------------------------------------------------------------------
# Macro    ...: macro_check_fix_areg_cxx_standard
# Purpose ....: Validates and sets C++ standard compatibility.
#               The variable 'AREG_CXX_STANDARD' must be defined before calling this macro.
# Details ....: Ensures `CMAKE_CXX_STANDARD` matches `AREG_CXX_STANDARD`. If incompatible, outputs a warning.
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

        # ###################################################################
        # Do not disable extensions if googletest compilation is included.
        # It causes googletest compilation due to using non-standard POSIX API use
        # like 'fileno', 'fdopen' and 'mkstemp' method calls.
        # ###################################################################
        # set(CMAKE_CXX_EXTENSIONS        OFF)
        # set(CMAKE_CXX_STANDARD_REQUIRED ON)    
        # ###################################################################

        # If the current C++ standard is less than the required 'AREG_CXX_STANDARD', issue a warning.
    elseif(${CMAKE_CXX_STANDARD} LESS ${AREG_CXX_STANDARD})
        message(WARNING "AREG: >>> AREG requires C++${AREG_CXX_STANDARD} or higher, \
                        current version is C++${CMAKE_CXX_STANDARD}. \
                        To avoid compilation errors, set 'CMAKE_CXX_STANDARD' to ${AREG_CXX_STANDARD}. \
                        Example: 'set(CMAKE_CXX_STANDARD ${AREG_CXX_STANDARD})'")
    endif()

endmacro(macro_check_fix_areg_cxx_standard)

# ---------------------------------------------------------------------------
# Macro ......: macro_normalize_path
# Purpose ....:  Normalizes Windows paths to Cygwin format if applicable.
# Note .......: This macro does not address OS-specific path separator issues.
# Parameters .: ${normal_path} [out] -- Name of variable to hold normalized path.
#               ${os_path}     [in]  -- The Windows-specific path to normalize.
# Usage ......: macro_normalize_path(<out-var> <windows-path>)
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
# Macro ......: macro_find_package
# Purpose ....: Finds a package and returns paths to its includes and libraries if found.
# Parameters .: ${package_name}      [in]   -- Name of the package to search for.
#               ${package_found}     [out]  -- Name of variable indicating if the package was found.
#               ${package_includes}  [out]  -- Variable holding the package's include directories.3
#               ${package_libraries} [out]  -- Variable holding the package's libraries.
# Usage ......: macro_find_package(<package-name> <found-flag-var> <includes-var> <libraries-var>)
# Example ....: 
#   set(SQLITE_FOUND FALSE)
#   macro_find_package(SQLite3 SQLITE_FOUND SQLITE_INCLUDE SQLITE_LIB)
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
# Macro ......: macro_create_option
# Purpose ....: Creates a boolean cache variable with a default value.
# Parameters .: ${var_name}     [out]   -- Name of the boolean variable.
#               ${var_value}    [in]    -- Default value if the variable is not yet defined.
#               ${var_describe} [in]    -- Description of the variable for the CMake cache.
# Usage ......: macro_create_option(<name-var> <default-value> <describe>)
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
# Macro ......: macro_add_source
# Purpose ....: Adds existing source files to a list based on a base directory. Checks file existence.
# Parameters .: ${result_list}  -- Name of variable that on output will contain the list of source files.
#               ${src_base_dir} -- Base directory of the source files.
#               ${ARGN}         -- List of source files, relative to the base directory.
# Usage ......: macro_add_source(<src-list-var> <base-dirpath> <files>)
# Example ....: 
#       set(aregextend_SRC)
#       macro_add_source(aregextend_SRC "${AREG_FRAMEWORK}" aregextend/db/private/LogSqliteDatabase.cpp ...)
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
# Macro ......: macro_parse_arguments
# Purpose ....: Parses files and libraries into separate lists for sources,
#               libraries, and resources. Library names must match known targets.
#               Resource files should have `.rc` extension (Windows-specific).
#
# Note .......: - Throws an error if file does not exist.
#               - List of resource files is Windows specific, it contains files with extension .rc.
#
# Parameters .: ${res_sources}   [out]  -- Name of variable that on output will contain the list of source files.
#               ${res_libs}      [out]  -- Name of variable that on output will contain the list of recognized CMake targets.
#               ${res_resources} [out]  -- Name of variable that on output will contain the List of resource files ('*.rc')
#               ${ARGN}          [in]   -- List of files, libraries, or resources to categorize.
#
# Usage ......: macro_parse_arguments(<sources-var> <libs-var> <resources-var> <sources-targets-resources>)
# Example ....: macro_parse_arguments(src_files lib_targets res_files my_lib src/main.cpp src/object.cpp res/resource.rc)
# ---------------------------------------------------------------------------
macro(macro_parse_arguments res_sources res_libs res_resources)
    set(_list "${ARGN}")
    foreach(_item IN LISTS _list)
        # Determine full path if file exists in the current directory
        set(_full_path "${_item}")
        if (NOT EXISTS "${_full_path}" AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_item}")
            set(_full_path "${CMAKE_CURRENT_LIST_DIR}/${_item}")
        endif()

        # Add to appropriate lists
        if (TARGET ${_item})
            list(APPEND ${res_libs} ${_item})
        elseif (EXISTS "${_full_path}")
            list(APPEND ${res_sources} "${_full_path}")
            # Check for resource file extension
            cmake_path(GET _full_path EXTENSION _ext)
            if (_ext STREQUAL "rc")
                list(APPEND ${res_resources} "${_full_path}")
            endif()
        else()
            message(FATAL_ERROR "AREG: >>> File \'${_item}\' does not exist, stopping.")
        endif()
    endforeach()
endmacro(macro_parse_arguments)

# ---------------------------------------------------------------------------
# Macro ......: macro_setup_compilers_data
# Purpose ....: Identifies and configures compiler family, short names, and paths.
# Note .......: Beside "gnu", "llvm", "msvc", the GNU compilers for CYGWIN are included as a "cygwin" family.
# Parameters .: - ${compiler_path}   -- Input:  Path to the C++ compiler.
#               - ${compiler_family} -- Output: Name of variable to hold compiler family (e.g., "gnu", "msvc", "llvm", "cygwin").
#               - ${compiler_short}  -- Output: Name of variable to hold short name of the compiler (e.g., "gcc", "clang", "cl").
#               - ${compiler_cxx}    -- Output: Name of variable to hold the C++ compiler path (usually same as ${compiler_path}).
#               - ${compiler_c}      -- Output: Name of variable to hold the corresponding C compiler name or path.
#               - ${is_identified}   -- Output: Name of variable to hold Boolean indicating successful identification.
# Usage ......: macro_setup_compilers_data(<compiler> <family-var> <short-var> <CXX-compiler-var> <C-compiler-var> <identified-var>)
# Example ....: macro_setup_compilers_data("${CMAKE_CXX_COMPILER}" 
#                                           AREG_COMPILER_FAMILY 
#                                           AREG_COMPILER_SHORT 
#                                           AREG_CXX_COMPILER 
#                                           AREG_C_COMPILER 
#                                           _compiler_supports
#                                         )
# ---------------------------------------------------------------------------
macro(macro_setup_compilers_data compiler_path compiler_family compiler_short compiler_cxx compiler_c is_identified)

    set(${is_identified} FALSE)
    
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
            set(${is_identified} TRUE)

            # break the loop, we have found
            break()
        endif()
    endforeach()

endmacro(macro_setup_compilers_data)

# ---------------------------------------------------------------------------
# Macro ......: macro_setup_compilers_data_by_family
# Purpose ....: Configures compiler names based on family (e.g., gnu, msvc, llvm, cygwin).
# Note .......: The "cygwin" family is supported for GNU compilers on the CYGWIN platform in Windows.
# Parameters .: - ${compiler_family} -- Input: Compiler family  name (e.g., "gnu", "msvc").
#               - ${compiler_short}  -- Output: Variable to hold the short name of the compiler (e.g., "gcc", "clang").
#               - ${compiler_cxx}    -- Output: Variable to hold the C++ compiler name.
#               - ${compiler_c}      -- Output: Variable to hold the corresponding C compiler name.
#               - ${is_identified}   -- Output: Name of variable to hold Boolean indicating successful identification..
# Usage ......: macro_setup_compilers_data(<compiler> <family-var> <short-var> <CXX-compiler-var> <C-compiler-var> <identified-var>)
# Example ....: macro_setup_compilers_data_by_family("gnu"
#                                                    AREG_COMPILER_SHORT 
#                                                    AREG_CXX_COMPILER 
#                                                    AREG_C_COMPILER 
#                                                    _compiler_supports
#                                                   )
# ---------------------------------------------------------------------------
macro(macro_setup_compilers_data_by_family compiler_family compiler_short compiler_cxx compiler_c is_identified)

    set(${is_identified} FALSE)
    
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
            set(${is_identified} TRUE)

            # break the loop, we have found
            break()
        endif()
    endforeach()

endmacro(macro_setup_compilers_data_by_family)

# ---------------------------------------------------------------------------
# Function ...: setAppOptions
# Purpose ....: Configures the compiler and linker options for executable applications.
#               Automatically links the AREG library, along with any additional libraries specified.
# Parameters .: ${target_name}  -- Name of the executable to apply options to.
#               ${library_list} -- List of additional libraries to link.
# Usage ......: setAppOptions(<target-name> <library-list>)
# ---------------------------------------------------------------------------
function(setAppOptions target_name library_list)

    # Set common compile definitions for the executable
    target_compile_definitions(${target_name} PRIVATE ${COMMON_COMPILE_DEF})

    # Apply common compiler options, such as disabling certain warnings
    target_compile_options(${target_name} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")

    # Link the AREG library, additional specified libraries, and any extended or extra libraries
    target_link_libraries(${target_name}
        ${AREG_PACKAGE_NAME}::aregextend   # AREG extended library
        ${library_list}                    # Custom libraries to link
        ${AREG_PACKAGE_NAME}::areg         # Core AREG library
        ${AREG_EXTENDED_LIBS}              # Extended libraries, if any
        ${AREG_LDFLAGS}                    # Additional linker flags
    )

endfunction(setAppOptions)

# ---------------------------------------------------------------------------
# Purpose ....: Creates an executable, sets its source files, applies necessary options, 
#               and links it with the provided list of libraries. 
#               The AREG library is automatically linked, no need to specify it.
# Function ...: addExecutableEx
# Parameters .: ${target_name}      -- The name of the executable target.
#               ${target_namespace} -- Namespace for aliasing. Can be empty.
#               ${source_list}      -- List of source files used to build the target executable.
#               ${library_list}     -- Libraries to link with the executable.
# Usage ......: addExecutableEx(<target-name> <namespace-opt> <source-list> <library-list>)
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
# Function ...: addExecutable
# Purpose ....: Wrapper for addExecutableEx, assuming there is not list of libraries to link with.
#               The AREG library is automatically linked, no need to specify it.
# Parameters .: ${target_name}      -- Name of the executable to build.
#               ${source_list}      -- List of source files used to build the executable.
# Usage ......: addExecutable(<target-name> <source-list>)
# ---------------------------------------------------------------------------
function(addExecutable target_name source_list)
    addExecutableEx(${target_name} "" "${source_list}" "")
endfunction(addExecutable)


# ---------------------------------------------------------------------------
# Function ...: setStaticLibOptions
# Purpose ....: Configures compiler and linker settings for a static library,
#               automatically linking the AREG Framework library along with any 
#               additional specified libraries
# Parameters .: ${target_name}  -- Name of the static library to apply options to.
#               ${library_list} -- List of libraries to link with the static library.
# Usage ......: setStaticLibOptions(<target-name> <library-list>)
# ---------------------------------------------------------------------------
function(setStaticLibOptions target_name library_list)

    # Apply common compile definitions and options for static libraries
    target_compile_definitions(${target_name} PRIVATE ${COMMON_COMPILE_DEF} _LIB)
    target_compile_options(${target_name} PRIVATE ${AREG_COMPILER_VERSION})
    target_compile_options(${target_name} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")

    # Additional compile options for non-Windows platforms
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(${target_name} PRIVATE "-Bstatic")  # Ensure static linking
        target_compile_options(${target_name} PRIVATE -fPIC)       # Position-independent code
    endif()

    # Link the static library with the provided libraries and AREG framework
    target_link_libraries(${target_name} ${library_list} ${AREG_PACKAGE_NAME}::areg ${AREG_LDFLAGS})

endfunction(setStaticLibOptions)

# ---------------------------------------------------------------------------
# Function ...: addStaticLibEx
# Purpose ....: Creates a static library with specified source files and options,
#               importing and auto-linking the AREG Framework library along with
#               any additional libraries.
# Parameters .: ${target_name}      -- Name of the static library to build.
#               ${target_namespace} -- Namespace for aliasing. Can be empty string if no aliasing.
#               ${source_list}      -- List of source files to build the static library.
#               ${library_list}     -- List of libraries to link.
# Usage ......: addStaticLibEx(<target-name> <namespace-opt> <source-list> <library-list>)
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
# Function ...: addStaticLib
# Purpose ....: Wrapper of addStaticLibEx, assuming there is no list of libraries to link.
#               Creates a static library, setting sources and options, importing, 
#               and auto-linking the AREG Framework library.
# Parameters .: ${target_name}      -- Name of the static library to build.
#               ${source_list}      -- List of source files to build the static library.
# Usage ......: addStaticLib(<target-name> <source-list>)
# ---------------------------------------------------------------------------
function(addStaticLib target_name source_list)
    addStaticLibEx(${target_name} "" "${source_list}" "")
endfunction(addStaticLib)

# ---------------------------------------------------------------------------
# Function ...: addStaticLibEx_C
# Purpose ....: Creates a static library compiled with C, setting sources, 
#               importing, and auto-linking the AREG Framework library along 
#               with any additional libraries.
# Parameters .: ${target_name}      -- Name of the static library to build.
#               ${target_namespace} -- Namespace for aliasing. Pass empty string if no aliasing.
#               ${source_list}      -- List of C-source files to build the static library.
#               ${library_list}     -- Libraries to link with the static library.
# Usage ......: addStaticLibEx_C(<target-name> <namespace-opt> <C-source-list> <library-list>)
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
# Function ...: addStaticLib_C
# Purpose ....: Wrapper for addStaticLibEx_C, assuming there is no aliasing and 
#               list of libraries for linking. Creates a static library compiled with C,
#               setting sources, importing, and auto-linking the AREG Framework library.
# Parameters .: ${target_name}      -- Name of the static library to build.
#               ${source_list}      -- List of C-source files to build the static library.
# Usage ......: addStaticLib_C(<target-name> <C-source-list>)
# ---------------------------------------------------------------------------
function(addStaticLib_C target_name source_list)
    addStaticLibEx_C(${target_name} "" "${source_list}" "")
endfunction(addStaticLib_C)

# ---------------------------------------------------------------------------
# Function ...: setSharedLibOptions
# Purpose ....: Configures settings for a shared library, automatically linking
#               the AREG Framework library and any additional specified libraries.
# Parameters .: ${target_name}  -- Name of the shared library to apply options to.
#               ${library_list} -- List of libraries for linking.
# Usage ......: setSharedLibOptions(<target-name> <library-list>)
# ---------------------------------------------------------------------------
function(setSharedLibOptions target_name library_list)

    # Apply common compile definitions for shared libraries
    target_compile_definitions(${target_name} PRIVATE ${COMMON_COMPILE_DEF} _USRDLL)
    target_compile_options(${target_name} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")

    # Link the shared library with provided libraries and AREG framework
    target_link_libraries(${target_name} ${AREG_PACKAGE_NAME}::aregextend ${library_list} ${AREG_PACKAGE_NAME}::areg ${AREG_EXTENDED_LIBS} ${AREG_LDFLAGS})

    # Additional compile options for non-Windows platforms
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32")
        target_compile_options(${target_name} PRIVATE -fPIC)       # Position-independent code for shared libraries
        target_compile_options(${target_name} PRIVATE "-Bdynamic") # Ensure dynamic linking
    endif()

endfunction(setSharedLibOptions)

# ---------------------------------------------------------------------------
# Function ...: addSharedLibEx
# Purpose ....: Creates a shared library with specified source files and options,
#               importing and auto-linking the AREG Framework library along with
#               any additional libraries.
# Parameters .: ${target_name}      -- Name of the shared library to build.
#               ${target_namespace} -- Namespace for aliasing. Can be empty string if no aliasing.
#               ${source_list}      -- List of source files to build the shared library.
#               ${library_list}     -- Libraries for linking.
# Usage ......: addSharedLibEx(<target-name> <namespace-opt> <source-list> <library-list>)
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
# Function ...: addSharedLib
# Purpose ....: Wrapper for addSharedLibEx, assuming there is no aliasing and no list for linking.
#               Creates a shared library with specified sources, options, imports,
#               and auto-linking the AREG Framework library.
# Parameters .: ${target_name}      -- Name of the shared library to build.
#               ${source_list}      -- List of source files to build the shared library.
# Usage ......: addSharedLib(<target-name> <source-list>)
# ---------------------------------------------------------------------------
function(addSharedLib target_name target_source_list)
    addSharedLibEx(${target_name} "" "${target_source_list}" "")
endfunction(addSharedLib)

# ---------------------------------------------------------------------------
# Macro ......: macro_add_service_interface
# Purpose ....: Generates and adds service-specific files to a static library
#               based on a given Service Interface document ('*.siml').
# Parameters .: ${lib_name}         -- Name of the static library.
#               ${interface_doc}    -- Full path to the Service Interface document file ('.siml').
#               ${codegen_root}     -- Root directory for file generation.
#               ${output_path}      -- Relative path for generated files.
#               ${codegen_tool}     -- Full path to the code generator tool.
# 
# Usage ......: macro_add_service_interface(<name-lib> <full-path-siml> <root-gen> <relative-path> <codegen-tool>)
# Example ....: 
#   macro_add_service_interface(funlib "/home/dev/fun/src/service/HelloWorld.siml" "/home/dev/fun/product" "generate/service" /tools/areg/codegen.jar)
#   macro_add_service_interface(funlib "/home/dev/fun/src/service/WeHaveFun.siml"  "/home/dev/fun/product" "generate/service" /tools/areg/codegen.jar)
# ---------------------------------------------------------------------------
macro(macro_add_service_interface lib_name interface_doc codegen_root output_path codegen_tool)

    if (NOT ${Java_FOUND})
        message(FATAL_ERROR "AREG Setup: Java not found! Install Java 17 or higher to run the code generator.")
        return()
    endif()

    # Run the code generator tool
    execute_process(COMMAND ${Java_JAVA_EXECUTABLE} -jar ${codegen_tool} --root=${codegen_root} --doc=${interface_doc} --target=${output_path})

    # Set path for generated files
    set(_generate "${codegen_root}/${output_path}")
    set(_interface_name "")
    cmake_path(GET interface_doc STEM _interface_name)
    if ("${_interface_name}" STREQUAL "")
        message(FATAL_ERROR "AREG Setup: The path \'${interface_doc}\' has no file name. Cannot generate Service Interface files.")
        return()
    endif()
    
    # List of generated source and header files
    list(APPEND _sources
        ${_generate}/private/${_interface_name}ClientBase.cpp
        ${_generate}/private/${_interface_name}Events.cpp
        ${_generate}/private/${_interface_name}Proxy.cpp
        ${_generate}/private/${_interface_name}Stub.cpp
        ${_generate}/private/NE${_interface_name}.cpp
        ${_generate}/private/${_interface_name}Events.hpp
        ${_generate}/private/${_interface_name}Proxy.hpp
        ${_generate}/${_interface_name}ClientBase.hpp
        ${_generate}/${_interface_name}Stub.hpp
        ${_generate}/NE${_interface_name}.hpp
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
# Function ...: addServiceInterfaceEx
# Purpose ....: Wrapper for macro_add_service_interface, assuming that the code generator
#               tool is in the 'tools/codegen.jar', the root directory for file generation
#               is ${AREG_BUILD_ROOT} and the code is generated in the "${AREG_GENERATE}/${relative_path}". 
#               Generates code and includes files for a Service Interface document (.siml) in a static library.
# Parameters .: ${lib_name}       -- Static library name.
#               ${source_root}    -- Source root directory.
#               ${relative_path}  -- Relative path to the Service Interface file.
#               ${sub_dir}        -- Optional sub-directory within 'relative_path'. Can be empty string.
#               ${interface_name} -- Service Interface name (excluding `.siml` extension).
# Usage ......: addServiceInterfaceEx(<library-name> <source-root> <relative-path> <sub-dir-opt> <service-interface-name>)
# Example ....: addServiceInterfaceEx("fun_library" "/home/dev/project/fun/src" "my/service/interfaces" "" FunInterface)
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
                                "${interface_doc}"
                                "${codegen_root}"
                                "${output_path}"
                                "${codegen_tool}")
endfunction(addServiceInterfaceEx)

# ---------------------------------------------------------------------------
# Function ...: addServiceInterface
# Purpose ....: Wrapper for addServiceInterfaceEx, with the source root assumed
#               to be ${CMAKE_SOURCE_DIR} and the Service Interface document is located
#               relative to ${CMAKE_CURRENT_LIST_DIR}.
# Parameters .: ${lib_name}       -- Name of the static library.
#               ${sub_dir}        -- Optional sub-directory, where the Service Interface document is located. Can be empty.
#               ${interface_name} -- Service Interface name (excluding '.siml' extension).
# Usage ......: addServiceInterface(<library-name> <sub-dir-opt> <service-interface-name>)
# ---------------------------------------------------------------------------
function(addServiceInterface lib_name sub_dir interface_name)
    set(_cur_dir ${CMAKE_CURRENT_LIST_DIR})
    string(FIND "${_cur_dir}" "${AREG_EXAMPLES}" _pos)
    if (${_pos} EQUAL 0)
        file(RELATIVE_PATH relative_path ${AREG_SDK_ROOT} ${CMAKE_CURRENT_LIST_DIR})
    else()
        file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
    endif()

    addServiceInterfaceEx(${lib_name} ${CMAKE_SOURCE_DIR} ${relative_path} ${sub_dir} ${interface_name})
endfunction(addServiceInterface)

# ---------------------------------------------------------------------------
# Function ...: removeEmptyDirs
# Purpose ....: Recursively removes empty directories within the specified directory path.
# Parameters .: ${dir_name} -- Directory path to check and potentially remove.
# Usage ......: removeEmptyDirs(<dir-path>)
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
# Macro ......: macro_declare_static_library
# Purpose ....: Declares a static library with categorized sources, libraries, and resources using 'macro_parse_arguments'.
# Parameters .: ${lib_name}  -- Name of the static library.
#               ${ARGN}      -- List of source files, libraries, and resources.
# Usage ......: macro_declare_static_library(<lib-name> <sources-targets-resources>)
# Example ....: macro_declare_static_library(myStaticLib src/main.cpp src/resource.rc libSomeDependency)
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
# Macro ......: macro_declare_shared_library
# Purpose ....: Declares a shared library with categorized sources, libraries, and resources using macro_parse_arguments.
# Parameters .: ${lib_name}  -- Name of the shared library.
#               ${ARGN}      -- List of source files, libraries, and resources.
# Usage ......: macro_declare_shared_library(<lib-name> <sources-targets-resources>)
# Example ....: macro_declare_shared_library(mySharedLib src/main.cpp src/resource.rc libSomeDependency)
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
# Macro ......: macro_declare_executable
# Purpose ....: Declares an executable target with categorized sources, 
#               libraries, and resources using macro_parse_arguments.
# Parameters  : ${exe_name}  -- Name of the target executable.
#               ${ARGN}      -- List of source files, libraries, and resources.
# Usage ......: macro_declare_executable(<executable-name> <sources-targets-resources>)
# Example ....: macro_declare_executable(myApplication src/main.cpp src/resource.rc libSomeDependency)
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
# Function ...: printAregConfigStatus
# Purpose ....: Prints a detailed status of AREG's CMake configuration, including details of the build environment.
# Parameters .: - ${var_make_print} -- Boolean flag indicating whether to print the status message (if FALSE, the function exits without printing).
#               - ${var_prefix}     -- A prefix added to each line of the status message (e.g., project name or custom label).
#               - ${var_header}     -- A custom header message displayed at the beginning of the status report.
#               - ${var_footer}     -- A custom footer message displayed at the end of the status report.
# Usage ......: printAregConfigStatus(<flag-to-print> <prefix> <header-output> <footer-output>)
# Example ....: printAregConfigStatus(
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
