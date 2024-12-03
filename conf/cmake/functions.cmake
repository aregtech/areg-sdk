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
    if (DEFINED AREG_CXX_STANDARD)
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
    else()
        message(WARNING "AREG: >>> Cannot check and set C++ standard, variable 'AREG_CXX_STANDARD' is not defined.")
    endif()

endmacro(macro_check_fix_areg_cxx_standard)

# ---------------------------------------------------------------------------
# Macro ......: macro_normalize_path
# Purpose ....:  Normalizes Windows paths to Cygwin format if applicable.
# Note .......: This macro does not address OS-specific path separator issues.
# Parameters .: ${normal_path} [out] -- Name of variable to hold normalized path.
#               ${os_path}     [in]  -- The Windows-specific path to normalize.
# Usage ......: macro_normalize_path(<out-var> <windows-path>)
# Example ....: macro_normalize_path(_norm_path "c:\path\to\my\directory")
# ---------------------------------------------------------------------------
macro(macro_normalize_path normal_path os_path)
    if (CYGWIN)
        execute_process(COMMAND cygpath.exe -m "${os_path}" OUTPUT_VARIABLE _normalized_path OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(${normal_path} "${_normalized_path}")
    else()
        set(${normal_path} "${os_path}")
    endif()
endmacro(macro_normalize_path)

# Read-only variable of 32-bit 'x86' processor name
set(_proc_x86   "i386")
# Read-only variable of 64-bit 'x64' processor name
set(_proc_x64   "x86_64")
# Read-only variable of 32-bit 'arm' processor name
set(_proc_arm32 "ARM")
# Read-only variable of 64-bit 'aarch64' processor name
set(_proc_arm64 "AARCH64")

# ---------------------------------------------------------------------------
# Macro .......: macro_get_processor
# Purpose .....: Detects and validates a processor architecture from an input name.
#                Matches the provided name against a predefined list of supported
#                architectures and retrieves:
#                  - The canonical architecture name.
#                  - The bitness (32-bit or 64-bit).
# Parameters ...: ${processor_name} [in]   -- The input processor architecture name to validate.
#                 ${var_processor}  [out]  -- The canonical name of the processor architecture (if supported).
#                 ${var_bitness}    [out]  -- The bitness (32/64) of the processor.
#                 ${var_found}      [out]  -- Boolean flag indicating whether the processor is supported (TRUE/FALSE).
# Usage ........: macro_get_processor(<processor_name> <var_processor> <var_bitness> <var_found>)
# Example ......: 
#   macro_get_processor("arm64" AREG_PROCESSOR AREG_BITNESS _entry_found)
# ---------------------------------------------------------------------------
macro(macro_get_processor processor_name var_processor var_bitness var_found)
    set(${var_found} FALSE)
    string(TOLOWER "${processor_name}" _proc_name)
    set(_arch_list  "x86|${_proc_x86}|32"
                    "i386|${_proc_x86}|32"
                    "i486|${_proc_x86}|32"
                    "i686|${_proc_x86}|32"
                    "x64|${_proc_x64}|64"
                    "x86_64|${_proc_x64}|64"
                    "x86-64|${_proc_x64}|64"
                    "amd64|${_proc_x64}|64"
                    "ia64|${_proc_x64}|64"
                    "arm|${_proc_arm32}|32"
                    "arm32|${_proc_arm32}|32"
                    "armv7|${_proc_arm32}|32"
                    "arm64|${_proc_arm64}|64"
                    "aarch64|${_proc_arm64}|64"
    )

    foreach(_entry IN LISTS _arch_list)
        string(REPLACE "|" ";" _entry "${_entry}")
        list(GET _entry 0 _arch)
        if ("${_arch}" STREQUAL "${_proc_name}")
            # If a match is found, extract processor details
            list(GET _entry 1 ${var_processor})
            list(GET _entry 2 ${var_bitness})
            set(${var_found} TRUE)
            break()
        endif()
    endforeach()
endmacro(macro_get_processor)

# ---------------------------------------------------------------------------
# Macro ........: macro_check_module_architect
# Purpose ......: Validates whether a given binary module (executable or library) 
#                 matches the specified processor architecture. The validation 
#                 uses object dumping tools to analyze the module. Typically, 
#                 this macro is invoked after identifying a package or library.
#
# Parameters ...: ${path_module}    [in]  -- Full path to the binary module (executable, shared, or static library).
#                 ${target_name}    [in]  -- Target name, typically the same as AREG_TARGET.
#                 ${target_proc}    [in]  -- Target processor architecture to validate.
#                 ${var_compatible} [out] -- Boolean variable set to TRUE if the binary is compatible with the target processor, FALSE otherwise.
#
# Usage ........: macro_check_module_architect(<path_to_binary> <target_name> <target_processor> <output_compatibility_var>)
# Example ......: 
#   macro_check_module_architect("/usr/lib/i386-linux-gnu/sqlite3.so" "i386-linux-gnu" i386 _is_compatible)
# ---------------------------------------------------------------------------
macro(macro_check_module_architect path_module target_name target_proc var_compatible)
    message(STATUS "AREG: >>> Validating binary '${path_module}' for compatibility with processor '${target_proc}'")
    
    # Determine the appropriate objdump command
    if (NOT "${CMAKE_OBJDUMP}" STREQUAL "")
        set(_objdump "${CMAKE_OBJDUMP}")
    else()
        set(_objdump "${target_name}-objdump")
    endif()

    # Check existence of the binary and objdump tool
    if (EXISTS "${path_module}" AND EXISTS "${_objdump}")
        macro_get_processor(${target_proc} _proc _bitness _found)

        execute_process(
            COMMAND bash -c "${_objdump} -f ${path_module} | grep ^architecture | cut -d' ' -f2 | sort -u"
            OUTPUT_VARIABLE _data
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )

        # Match the processor type with extracted architecture
        if (${_proc} STREQUAL ${_proc_x86})
            string(FIND "${_data}" "x86-64" _pos)
            if (_pos EQUAL -1)
                string(FIND "${_data}" "i386" _pos)
			else()
				set(_pos -1)
            endif()
        elseif (${_proc} STREQUAL ${_proc_x64})
            string(FIND "${_data}" "x86-64" _pos)
        elseif (${_proc} STREQUAL ${_proc_arm32})
            string(FIND "${_data}" "ARM" _pos)
        elseif (${_proc} STREQUAL ${_proc_arm64})
            string(FIND "${_data}" "AARCH64" _pos)
        else()
            string(FIND "${_data}" "${_proc}" _pos)
        endif()

        # Set compatibility flag based on architecture match
        if (_pos GREATER -1)
            set(${var_compatible} TRUE)
        else()
            message(WARNING "AREG: >>> Binary '${path_module}' is NOT compatible with target processor '${target_proc}'")
            set(${var_compatible} FALSE)
        endif()
    elseif (${AREG_OS} STREQUAL Windows)
        set(${var_compatible} TRUE)
    else()
        set(${var_compatible} FALSE)
    endif()
endmacro(macro_check_module_architect)

# ---------------------------------------------------------------------------
# Macro .......: macro_find_ncurses_package
# Purpose .....: Locates the 'ncurses' library and its associated header files on the system.
#                Sets output variables with the include directory and library path if found.
# Parameters ...: ${var_include}  [out] -- Variable to store the path to the directory containing the 'ncurses.h' header file.
#                 ${var_library}  [out] -- Variable to store the full path to the 'ncurses' library file.
#                 ${var_found}    [out] -- Variable to indicate whether the 'ncurses' library and headers were successfully located (TRUE/FALSE).
# Usage ........: macro_find_ncurses_package(<ncurses-include-var> <ncurses-library-var> <found-flag-var>)
# Example ......: 
#   macro_find_ncurses_package(NCURSES_INCLUDE NCURSES_LIB NCURSES_FOUND)
# ---------------------------------------------------------------------------
macro(macro_find_ncurses_package var_include var_library var_found)
    set(${var_found} FALSE)
    set(${var_include})
    set(${var_library})

    find_path(${var_include}    NAMES ncurses.h)
    find_library(${var_library} NAMES ncurses)
    if (${var_include} AND ${var_library})
        macro_check_module_architect("${${var_library}}" ${AREG_TARGET} ${AREG_PROCESSOR} ${var_found})
    endif()
endmacro(macro_find_ncurses_package)

# ---------------------------------------------------------------------------
# Macro .......: macro_find_gtest_package
# Purpose .....: Locates the Google Test (GTest) package, including its header files and libraries.
#                Sets output variables with the include directory and library paths if found.
# Parameters ...: ${var_include} [out] -- Variable to store the path to the directory containing GTest header files.
#                 ${var_library} [out] -- Variable to store the full paths to the GTest libraries.
#                 ${var_found}   [out] -- Variable to indicate whether the GTest package was successfully located (TRUE/FALSE).
# Usage ........: macro_find_gtest_package(<gtest-include-var> <gtest-library-var> <gtest-found-flag-var>)
# Example ......: 
#   macro_find_gtest_package(GTEST_INCLUDE GTEST_LIB GTEST_FOUND)
# ---------------------------------------------------------------------------
macro(macro_find_gtest_package var_include var_library var_found)
    set(${var_found} FALSE)
    set(${var_include})
    set(${var_library})

    include(FindGTest)
    if (GTest_FOUND)
        set(${var_library} "${GTEST_LIBRARIES}")
        set(${var_include} "${GTEST_INCLUDE_DIRS}")
        if (EXISTS "${GTEST_LIBRARIES}")
            set(_gtest_location "${GTEST_LIBRARIES}")
        else()
            get_target_property(_gtest_location GTest::gtest LOCATION)
            if (NOT _gtest_location)
                get_target_property(_gtest_location GTest::GTest LOCATION)
            endif()
        endif()

        if (EXISTS "${_gtest_location}")
            message(STATUS "<<< AREG: Checking _gtest_location = ${_gtest_location}")
            macro_check_module_architect("${_gtest_location}" ${AREG_TARGET} ${AREG_PROCESSOR} ${var_found})
        else()
            message(WARNING "<<< AREG: Failed to detect the location of GTest, _gtest_location = ${_gtest_location}")
            set(${var_found} FALSE)
        endif()

    endif()
endmacro(macro_find_gtest_package)

# ---------------------------------------------------------------------------
# Macro .......: macro_find_sqlite_package
# Purpose .....: Locates the SQLite3 package, including its header files and libraries.
#                Sets output variables with the include directory and library paths if found.
# Parameters ...: ${var_include} [out] -- Variable to store the path to the directory containing SQLite3 header files.
#                 ${var_library} [out] -- Variable to store the full path to the SQLite3 library file(s).
#                 ${var_found}   [out] -- Variable to indicate whether the SQLite3 package was successfully located (TRUE/FALSE).
# Usage ........: macro_find_sqlite_package(<sqlite3-include-var> <sqlite3-library-var> <sqlite3-found-flag-var>)
# Example ......: 
#   macro_find_sqlite_package(SQLITE_INCLUDE SQLITE_LIB SQLITE_FOUND)
# ---------------------------------------------------------------------------
macro(macro_find_sqlite_package var_include var_library var_found)
    set(${var_found}    FALSE)
    set(${var_include})
    set(${var_library})

    include(FindSQLite3)
    if (SQLite3_FOUND)
        set(${var_library} "${SQLite3_LIBRARIES}")
        set(${var_include} "${SQLite3_INCLUDE_DIRS}")
        macro_check_module_architect("${SQLite3_LIBRARY}" ${AREG_TARGET} ${AREG_PROCESSOR} ${var_found})
    endif()
endmacro(macro_find_sqlite_package)

# ---------------------------------------------------------------------------
# Macro ......: macro_create_option
# Purpose ....: Creates a boolean cache variable with a default value.
# Parameters .: ${var_name}     [out] -- Name of the boolean variable.
#               ${var_value}    [in]  -- Default value if the variable is not yet defined.
#               ${var_describe} [in]  -- Description of the variable for the CMake cache.
# Usage ......: macro_create_option(<name-var> <default-value> <describe>)
# Example ....: macro_create_option(AREG_LOGS ON "Compile with logs")
# ---------------------------------------------------------------------------
macro(macro_create_option var_name var_value var_describe)
    if (NOT DEFINED ${var_name})
        option(${var_name} "${var_describe}" ${var_value})
    else()
        option(${var_name} "${${var_name}}" ${var_value})
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
# Example ....: 
#   macro_parse_arguments(src_files lib_targets res_files my_lib src/main.cpp src/object.cpp res/resource.rc)
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
# Macro ......: macro_guess_processor_architecture
# Purpose ....: If possible, detects the processor architecture and bitness by given compiler path.
# Parameters .: ${compiler_path}    [in]   -- Path of the cross-compiler.
#               ${target_processor} [out]  -- Name of variable to store the CPU type.
#               ${target_bitness}   [out]  -- Name of variable to store bitness.
#
# Usage ......: macro_guess_processor_architecture(<compiler-path> <processor-var> <bitness-var>)
# Example ....: 
#   macro_guess_processor_architecture("arm-linux-gnueabihf-g++" cpu_architect cpu_bitness)
# ---------------------------------------------------------------------------
macro(macro_guess_processor_architecture compiler_path target_processor target_bitness)
    foreach(_entry "arm;${_proc_arm32};32" "aarch64;${_proc_arm64};64")
        list(GET _entry 0 _proc)
        list(GET _entry 1 _arch)
        list(GET _entry 2 _bits)
        string(FIND "${compiler_path}" ${_proc} _proc_pos)
        if (_proc_pos GREATER -1)
            set(${target_processor} ${_arch})
            set(${target_bitness} ${_bits})
            break()
        endif()
    endforeach()

    unset(_entry)
    unset(_proc)
    unset(_arch)
    unset(_bits)
    unset(_proc_pos)

endmacro(macro_guess_processor_architecture)

# ---------------------------------------------------------------------------
# Macro ......: macro_system_bitness
# Purpose ....: Extracts the system default bitness.
# Parameters  : ${var_bitness} [out] -- The name of variable to set the bitness.
# Usage ......: macro_system_bitness(<var-name>)
# Example ....: 
#   macro_system_bitness(_sys_bitness)
# ---------------------------------------------------------------------------
macro(macro_system_bitness var_bitness)
    # Detect and set bitness here
    # 8 bytes ==> 64-bits (x64) and 4 bytes ==> 32-nit (x86)
    if (DEFINED CMAKE_SIZEOF_VOID_P)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(${var_bitness} 64)
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(${var_bitness} 32)
        else()
            set(${var_bitness} 0)
        endif()
    endif()
endmacro(macro_system_bitness)

# ---------------------------------------------------------------------------
# Macro .......: macro_default_target
# Purpose .....: Configures the default compiler target based on the specified processor architecture.
#                The configured target is also used to determine the library architecture for linking.
# Parameters ...: ${target_processor} [in]  -- The target processor architecture (e.g., AARCH64, X86_64).
#                 ${var_name_target}  [out] -- Variable to store the determined compiler target.
# Usage ........: macro_default_target(<target-processor> <compiler-target-var>)
# Example ......: 
#   macro_default_target(AARCH64 AREG_TARGET)
# ---------------------------------------------------------------------------
macro(macro_default_target target_processor var_name_target)
    macro_get_processor("${target_processor}" _proc _bitness _found)
    if ("${_proc}" STREQUAL "")
        set(${var_name_target})
    elseif (UNIX)
        if (${_proc} MATCHES "${_proc_x64}")
            set(${var_name_target} x86_64-linux-gnu)
        elseif (${_proc} MATCHES "${_proc_x86}")
            set(${var_name_target} i386-linux-gnu)
        elseif (${_proc} MATCHES "${_proc_arm64}")
            set(${var_name_target} aarch64-linux-gnu)
        elseif (${_proc} MATCHES "${_proc_arm32}")
            set(${var_name_target} arm-linux-gnueabihf)
        endif()
    elseif (MSVC)
        if (${_proc} MATCHES "${_proc_x64}")
            set(${var_name_target} x64)
        elseif (${_proc} MATCHES "${_proc_x86}")
            set(${var_name_target} "win32")
        endif()
    elseif(CYGWIN)
        if (${_proc} MATCHES "${_proc_x64}")
            set(${var_name_target} "x86_64-pc-cygwin")
        elseif (${_proc} MATCHES "${_proc_x86}")
            set(${var_name_target} "i386-pc-cygwin")
        endif()
    endif()
endmacro(macro_default_target)

# ---------------------------------------------------------------------------
# Macro .......: macro_setup_compilers_data
# Purpose .....: Detects and configures compiler attributes including family, short name, paths, 
#                target, processor architecture, and application bitness. 
# Note ........: In addition to "gnu", "llvm", and "msvc", GNU compilers used in CYGWIN are 
#                categorized under the "cygwin" family.
# Parameters ...: ${compiler_path}    [in]       -- Path to the C++ compiler.
#                 ${var_name_family}  [out]      -- Variable to store the compiler family (e.g., "gnu", "msvc", "llvm", "cygwin").
#                 ${var_name_short}   [out]      -- Variable to store the short name of the compiler (e.g., "gcc", "clang", "cl").
#                 ${var_name_cxx}     [out]      -- Variable to store the path to the C++ compiler (typically same as ${compiler_path}).
#                 ${var_name_c}       [out]      -- Variable to store the corresponding C compiler name or path.
#                 ${var_name_target}  [out]      -- Variable to store the compiler target value.
#                 ${var_name_arch}    [in, out]  -- Variable containing the processor architecture (input) and updated with the determined architecture (output).
#                 ${var_name_bitness} [out]      -- Variable to store the application bitness value (e.g., 32 or 64).
#                 ${var_name_found}   [out]      -- Variable to indicate whether the compiler was successfully identified (TRUE/FALSE).
# Usage ........: macro_setup_compilers_data(<compiler-path> <family-var> <short-var> 
#                                            <CXX-compiler-var> <C-compiler-var> 
#                                            <compiler-target-var> <processor-architecture-var> 
#                                            <target-bitness-var> <found-flag-var>)
# Example ......: macro_setup_compilers_data("${CMAKE_CXX_COMPILER}" 
#                                             AREG_COMPILER_FAMILY 
#                                             AREG_COMPILER_SHORT 
#                                             AREG_CXX_COMPILER 
#                                             AREG_C_COMPILER 
#                                             AREG_TARGET 
#                                             AREG_PROCESSOR 
#                                             AREG_BITNESS 
#                                             _compiler_supports)
# ---------------------------------------------------------------------------
macro(macro_setup_compilers_data 
        compiler_path 
        var_name_family 
        var_name_short 
        var_name_cxx 
        var_name_c 
        var_name_target 
        var_name_arch 
        var_name_bitness 
        var_name_found)

    set(${var_name_found} FALSE)
    if(NOT "${${var_name_arch}}" STREQUAL "")
        macro_get_processor("${${var_name_arch}}" ${var_name_arch} ${var_name_bitness} _ignore)
    elseif()
        macro_system_bitness(${var_name_bitness})
    endif()
    
    # Iterate over known compilers to identify the compiler type
    foreach(_entry "clang-cl;llvm;clang-cl" "clang++;llvm;clang" "clang;llvm;clang" "g++;gnu;gcc" "gcc;gnu;gcc" "c++;gnu;cc" "cc;gnu;cc" "cl;msvc;cl")
        list(GET _entry 0 _cxx_comp)

        # Check if the provided compiler matches the known C++ compiler
        string(TOLOWER "${compiler_path}" _comp_path)
        string(FIND "${_comp_path}" "${_cxx_comp}" _found_pos REVERSE)
        if (_found_pos GREATER -1)
            list(GET _entry 1 _family)
            list(GET _entry 2 _cc_comp)
            # Handle special case for CYGWIN and GNU family compilers
            if (${_family} STREQUAL gnu)
                if (CYGWIN)
                    set(${var_name_family} "cygwin")
                    macro_default_target("${${var_name_arch}}" ${var_name_target})
                else()
                    set(${var_name_family} "gnu")
                    macro_guess_processor_architecture("${_comp_path}" ${var_name_arch} ${var_name_bitness})
                    string(FIND "${_comp_path}" "-${_cxx_comp}" _found_pos REVERSE)
                    if (_found_pos GREATER -1)
                        cmake_path(GET _comp_path FILENAME _file_name)
                        string(REPLACE "-${_cxx_comp}" "" ${var_name_target} "${_file_name}")
                    else()
                        macro_default_target("${${var_name_arch}}" ${var_name_target})
                    endif()
                endif()
            elseif (${_family} STREQUAL llvm)
                set(${var_name_family} "llvm")
                macro_default_target("${${var_name_arch}}" ${var_name_target})
            elseif (${_family} STREQUAL msvc)
                set(${var_name_family} "msvc")
                macro_default_target("${${var_name_arch}}" ${var_name_target})
            else()
                set(${var_name_family} "${_family}")
                macro_default_target("${${var_name_arch}}" ${var_name_target})
            endif()

            set(${var_name_short} "${_cxx_comp}")
            set(${var_name_cxx}   "${compiler_path}")

            # Determine the corresponding C compiler path or name
            if ("${_cxx_comp}" STREQUAL "${_cc_comp}")
                set(${var_name_c} "${compiler_path}")
            else()
                string(REPLACE "${_cxx_comp}" "${_cc_comp}" ${var_name_c} "${compiler_path}")
            endif()

            # Mark compiler as found
            set(${var_name_found} TRUE)

            # break the loop, we have found
            break()
        endif()
    endforeach()

    unset(_entry)
    unset(_cxx_comp)
    unset(_family)
    unset(_cc_comp)
    unset(_found_pos)

endmacro(macro_setup_compilers_data)

# ---------------------------------------------------------------------------
# Macro ......: macro_setup_compilers_data_by_family
# Purpose ....: Configures compiler names based on family (e.g., gnu, msvc, llvm, cygwin), and compiler target.
# Note .......: The "cygwin" family is supported for GNU compilers on the CYGWIN platform in Windows.
# Parameters .: ${compiler_family} [in]  -- Compiler family  name (e.g., "gnu", "msvc").
#               ${var_name_short}  [out] -- Variable to hold the short name of the compiler (e.g., "gcc", "clang").
#               ${var_name_cxx}    [out] -- Variable to hold the C++ compiler name.
#               ${var_name_c}      [out] -- Variable to hold the corresponding C compiler name.
#               ${var_name_target} [out] -- Variable to hold the compiler default target name.
#               ${var_name_found}  [out] -- Name of variable to hold Boolean indicating successful identification.
# Usage ......: macro_setup_compilers_data_by_family(<compiler-family> <short-var> <CXX-compiler-var> <C-compiler-var> <identified-var>)
# Example ....: macro_setup_compilers_data_by_family("gnu"
#                                                    AREG_COMPILER_SHORT 
#                                                    AREG_CXX_COMPILER 
#                                                    AREG_C_COMPILER 
#                                                    AREG_TARGET
#                                                    _compiler_supports
#                                                   )
# ---------------------------------------------------------------------------
macro(macro_setup_compilers_data_by_family compiler_family var_name_short var_name_cxx var_name_c var_name_target var_name_found)

    set(${var_name_found} FALSE)
    
    # Iterate over known compilers and match the family
    foreach(_entry "clang++;llvm;clang" "g++;gnu;gcc" "cl;msvc;cl" "g++;cygwin;gcc")
        list(GET _entry 1 _family)

        if ("${_family}" STREQUAL "${compiler_family}")
            list(GET _entry 0 _cxx_comp)
            list(GET _entry 2 _cc_comp)
            # Special case for Windows
            if ("${_family}" STREQUAL "llvm")
                if (MSVC)
                    set(${var_name_short} "clang-cl")
                    set(${var_name_cxx}   "clang-cl")
                    set(${var_name_c}     "clang-cl")
                else()
                    set(${var_name_short} "${_cxx_comp}")
                    set(${var_name_cxx}   "${_cxx_comp}")
                    set(${var_name_c}     "${_cc_comp}")
                endif()
                macro_default_target("${AREG_PROCESSOR}" ${var_name_target})
            elseif ("${AREG_PROCESSOR}" STREQUAL "${_proc_arm32}" AND "${_family}" STREQUAL "gnu")
                set(${var_name_short}  g++)
                set(${var_name_cxx}    arm-linux-gnueabihf-g++)
                set(${var_name_c}      arm-linux-gnueabihf-gcc)
                set(${var_name_target} arm-linux-gnueabihf)
            elseif ("${AREG_PROCESSOR}" STREQUAL "${_proc_arm64}" AND "${_family}" STREQUAL "gnu")
                set(${var_name_short}  g++)
                set(${var_name_cxx}    aarch64-linux-gnu-g++)
                set(${var_name_c}      aarch64-linux-gnu-gcc)
                set(${var_name_target} aarch64-linux-gnu)
            else()
                set(${var_name_short} "${_cxx_comp}")
                set(${var_name_cxx}   "${_cxx_comp}")
                set(${var_name_c}     "${_cc_comp}")
                macro_default_target("${AREG_PROCESSOR}" ${var_name_target})
            endif()

            # Mark compiler as found
            set(${var_name_found} TRUE)

            # break the loop, we have found
            break()
        endif()
    endforeach()

    unset(_entry)
    unset(_cxx_comp)
    unset(_family)
    unset(_cc_comp)

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
# Parameters .: ${target_name}  -- Name of the executable to build.
#               ${source_list}  -- List of source files used to build the executable.
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
# Parameters .: ${target_name}  -- Name of the static library to build.
#               ${source_list}  -- List of source files to build the static library.
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
# Parameters .: ${target_name}  -- Name of the static library to build.
#               ${source_list}  -- List of C-source files to build the static library.
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
        target_compile_options(${target_name} PRIVATE "-Bdynamic") # Ensure dynamic linking
        target_compile_options(${target_name} PRIVATE -fPIC)       # Position-independent code for shared libraries
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
# Parameters .: ${target_name}  -- Name of the shared library to build.
#               ${source_list}  -- List of source files to build the shared library.
# Usage ......: addSharedLib(<target-name> <source-list>)
# ---------------------------------------------------------------------------
function(addSharedLib target_name target_source_list)
    addSharedLibEx(${target_name} "" "${target_source_list}" "")
endfunction(addSharedLib)

# ---------------------------------------------------------------------------
# Macro ......: macro_add_service_interface
# Purpose ....: The macro_add_service_interface function automates the generation and 
#               inclusion of service-specific source and header files for a static library,
#               based on a provided Service Interface document (*.siml).
# Parameters .: ${lib_name}         -- The name of the static library where generated files will be included.
#               ${interface_doc}    -- Full path to the Service Interface document file (.siml), which defines the service interface details.
#               ${codegen_root}     -- Root directory where generated files will be stored.
#               ${output_path}      -- Relative path from ${codegen_root} to the directory where generated files will be placed.
#               ${codegen_tool}     -- Full path to the code generation tool (codegen.jar) used to generate the necessary source and header files.
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

    if (NOT EXISTS "${interface_doc}")
        message(FATAL_ERROR "AREG Setup: The Service Interface file \'${interface_doc}\' does not exist. Cannot generate files.")
        return()
    endif()

    set(_si_doc "${interface_doc}")
    cmake_path(GET _si_doc STEM _interface_name)
    if ("${_interface_name}" STREQUAL "")
        message(FATAL_ERROR "AREG Setup: The path \'${interface_doc}\' has no file name. Cannot generate Service Interface files.")
        return()
    endif()

    # Run the code generator tool
    execute_process(COMMAND ${Java_JAVA_EXECUTABLE} -jar ${codegen_tool} --doc=${interface_doc} --root=${codegen_root} --target=${output_path})

    # Set path for generated files
    set(_generate "${codegen_root}/${output_path}")
    
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
# Purpose ....: The addServiceInterfaceEx function acts as a wrapper for
#               macro_add_service_interface, facilitating the generation of code
#               and header files for a Service Interface document (.siml file)
#               within a specified static library. This function assumes the
#               code generator tool is located at ${AREG_SDK_TOOLS}/codegen.jar.
#               The generated files are placed within the ${AREG_GENERATE_DIR},
#               with a subdirectory structure specified by ${generate_path}.
# Parameters .: ${lib_name}         -- The name of the static library to be created for the generated Service Interface code.
#               ${source_root}      -- The root directory containing the project's source files.
#               ${siml_path}        -- Path to the Service Interface document file (.siml), relative to the specified ${source_root}.
#               ${generate_path}    -- Subdirectory path within ${AREG_GENERATE_DIR} where the generated files will be stored.
# Usage ......: addServiceInterfaceEx(<library-name> <source-root> <service-interface-relative-path> <relative-path-to-generate-codes>)
# Example ....: 
#   addServiceInterfaceEx(fun_library "/home/dev/project/fun/src" "fun/service/interfaces/FunService.siml" "fun/service/interfaces")
# ---------------------------------------------------------------------------
function(addServiceInterfaceEx lib_name source_root siml_path generate_path)
    if ("${generate_path}" STREQUAL "")
        cmake_path(GET siml_path PARENT_PATH generate_path)
    endif()

    macro_normalize_path(_interface_doc  "${source_root}/${siml_path}")
    macro_normalize_path(_codegen_root   "${AREG_GENERATE_DIR}")
    macro_normalize_path(_output_path    "${generate_path}")
    macro_normalize_path(_codegen_tool   "${AREG_SDK_TOOLS}/codegen.jar")

    macro_add_service_interface(${lib_name}
                                "${_interface_doc}"
                                "${_codegen_root}"
                                "${_output_path}"
                                "${_codegen_tool}")
endfunction(addServiceInterfaceEx)

# ---------------------------------------------------------------------------
# Function ...: addServiceInterface
# Purpose ....: The addServiceInterface function provides a simplified wrapper for 
#               addServiceInterfaceEx, automatically setting the source root to
#               ${PROJECT_SOURCE_DIR}. It assumes that the Service Interface 
#               document (.siml file) is located relative to ${PROJECT_SOURCE_DIR},
#               and the generated source files are placed in ${AREG_GENERATE_DIR},
#               with a directory structure mirroring the parent path of ${siml_path}.
# Parameters .: ${lib_name}     -- The name of the static library to be created for the Service Interface.
#               ${siml_path}    -- The path to the Service Interface document file (.siml), relative to PROJECT_SOURCE_DIR.
# Usage ......: addServiceInterface(<library-name> <service-interface-file-relative-path>)
# Example ....: 
#   addServiceInterface(fun_library fun/service/interface/FunService.siml)
# ---------------------------------------------------------------------------
function(addServiceInterface lib_name siml_path)
    addServiceInterfaceEx(${lib_name} "${PROJECT_SOURCE_DIR}" "${siml_path}" "")
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
# Example ....: 
#   macro_declare_static_library(myStaticLib src/main.cpp src/resource.rc libSomeDependency)
# ---------------------------------------------------------------------------
macro(macro_declare_static_library lib_name)

    # Parse arguments to get sources, libraries, and resources
    macro_parse_arguments(_sources _libs _resources "${ARGN}")

    # Ensure the source list is not empty
    if (NOT _sources)
        message(FATAL_ERROR "AREG: >>> Source list to build static library \'${exe_name}\' is empty")
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
# Example ....: 
#   macro_declare_shared_library(mySharedLib src/main.cpp src/resource.rc libSomeDependency)
# ---------------------------------------------------------------------------
macro(macro_declare_shared_library lib_name)

    # Parse arguments to get sources, libraries, and resources
    macro_parse_arguments(_sources _libs _resources "${ARGN}")

    # Ensure the source list is not empty
    if (NOT _sources)
        message(FATAL_ERROR "AREG: >>> Source list to build shared library \'${exe_name}\' is empty")
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
# Example ....: 
#   macro_declare_executable(myApplication src/main.cpp src/resource.rc libSomeDependency)
# ---------------------------------------------------------------------------
macro(macro_declare_executable exe_name)

    # Parse arguments to get sources, libraries, and resources
    macro_parse_arguments(_sources _libs _resources "${ARGN}")

    # Ensure the source list is not empty
    if (NOT _sources)
        message(FATAL_ERROR "AREG: >>> Source list to build executable \'${exe_name}\' is empty")
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
    message(STATUS "${var_prefix}: >>> Build Environment ..: System '${CMAKE_SYSTEM_NAME}', ${AREG_BITNESS}-bit '${AREG_PROCESSOR}' platform, Env '${AREG_DEVELOP_ENV}'")
    message(STATUS "${var_prefix}: >>> Used CXX-Compiler ..: '${CMAKE_CXX_COMPILER}'")
    message(STATUS "${var_prefix}: >>> Used C-Compiler ....: '${CMAKE_C_COMPILER}'")
    message(STATUS "${var_prefix}: >>> Compiler Version ...: C++ standard 'c++${CMAKE_CXX_STANDARD}', compiler family '${AREG_COMPILER_FAMILY}', target '${CMAKE_CXX_COMPILER_TARGET}'")
    message(STATUS "${var_prefix}: >>> AREG SDK Root ......: '${AREG_SDK_ROOT}'")
    message(STATUS "${var_prefix}: >>> CMake Build Dir ....: '${CMAKE_BINARY_DIR}'")
    message(STATUS "${var_prefix}: >>> Binary Output Dir ..: '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'")
    message(STATUS "${var_prefix}: >>> Generated Files Dir : '${AREG_GENERATE_DIR}'")
    message(STATUS "${var_prefix}: >>> Packages Dir .......: '${FETCHCONTENT_BASE_DIR}'")
    message(STATUS "${var_prefix}: >>> Build Modules ......: areg = '${AREG_BINARY}', aregextend = static, areglogger = '${AREG_LOGGER_BINARY}', executable extension '${CMAKE_EXECUTABLE_SUFFIX}'")
    message(STATUS "${var_prefix}: >>> Java Version .......: '${Java_VERSION_STRING}', Java executable = '${Java_JAVA_EXECUTABLE}', minimum version required = 17")
    message(STATUS "${var_prefix}: >>> Packages Use .......: SQLite3 package use = '${AREG_SQLITE_PACKAGE}', GTest package use = '${AREG_GTEST_PACKAGE}'")
    message(STATUS "${var_prefix}: >>> Other Options ......: Examples = '${AREG_BUILD_EXAMPLES}', Unit Tests = '${AREG_BUILD_TESTS}', AREG Extended = '${AREG_EXTENDED}', Logs = '${AREG_LOGS}'")
    message(STATUS "${var_prefix}: >>> Installation .......: Enabled = '${AREG_INSTALL}', location = '${CMAKE_INSTALL_PREFIX}'")

    # Print the footer section with separators
    message(STATUS "=======================================================================================")
    message(STATUS "${var_footer}")
    message(STATUS "=======================================================================================")

endfunction(printAregConfigStatus)
