# ###########################################################################
# Areg CMake functions
# Copyright 2022-2026 Aregtech (Artak Avetyan)
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
            message(WARNING "Areg: >>> Areg requires C++${AREG_CXX_STANDARD} or higher, \
                            current version is C++${CMAKE_CXX_STANDARD}. \
                            To avoid compilation errors, set 'CMAKE_CXX_STANDARD' to ${AREG_CXX_STANDARD}. \
                            Example: 'set(CMAKE_CXX_STANDARD ${AREG_CXX_STANDARD})'")
        endif()
    else()
        message(WARNING "Areg: >>> Cannot check and set C++ standard, variable 'AREG_CXX_STANDARD' is not defined.")
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
        unset(_normalized_path)
    else()
        set(${normal_path} "${os_path}")
    endif()
endmacro(macro_normalize_path)

# ---------------------------------------------------------------------------
# Macro ......: macro_cmake_path
# Purpose ....: The counterpart of 'macro_normalize_path'. Converts a path back into the
#               form CMake itself works with.
# Note .......: Under Cygwin a path that starts with a drive letter is understood by the
#               compilers and by the Windows tools, but not by CMake: file(GLOB) and the
#               other commands that take a path apart read it as a relative path and look
#               for it under the current directory, where it is not. Anything that a CMake
#               command has to open or match must go through this macro first. Only a drive
#               letter path is converted, so a path that is already the right one costs
#               nothing here and on the other platforms nothing is converted at all.
# Parameters .: ${cmake_path} [out] -- Name of variable to hold the converted path.
#               ${any_path}   [in]  -- The path to convert.
# Usage ......: macro_cmake_path(<out-var> <path>)
# ---------------------------------------------------------------------------
macro(macro_cmake_path cmake_path any_path)
    if (CYGWIN AND "${any_path}" MATCHES "^[A-Za-z]:")
        execute_process(COMMAND cygpath.exe -u "${any_path}" OUTPUT_VARIABLE _converted_path OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(${cmake_path} "${_converted_path}")
        unset(_converted_path)
    else()
        set(${cmake_path} "${any_path}")
    endif()
endmacro(macro_cmake_path)

# ---------------------------------------------------------------------------
# Macro ......: macro_absolute_path
# Purpose ....: Makes a path absolute and leaves one that already is alone. A relative path is
#               taken from ${base_dir}.
# Note .......: The code generator takes every path either absolute or relative to the project
#               root, while CMake takes a relative path from the directory being configured.
#               The two disagree on what a relative path means, so a path is made absolute
#               before it is handed over and the question does not arise.
# Parameters .: ${abs_path} [out] -- Name of variable to hold the absolute path.
#               ${any_path} [in]  -- The path to convert.
#               ${base_dir} [in]  -- The directory a relative path is taken from.
# Usage ......: macro_absolute_path(<out-var> <path> <base-dir>)
# ---------------------------------------------------------------------------
macro(macro_absolute_path abs_path any_path base_dir)
    set(${abs_path} "${any_path}")
    cmake_path(IS_ABSOLUTE ${abs_path} _path_is_absolute)
    if (NOT _path_is_absolute)
        cmake_path(ABSOLUTE_PATH ${abs_path} BASE_DIRECTORY "${base_dir}" NORMALIZE)
    endif()
    unset(_path_is_absolute)
endmacro(macro_absolute_path)

# ---------------------------------------------------------------------------
# Macro ......: macro_document_key
# Purpose ....: Turns the path of a model document into a stable key, so that the same
#               document reached by two different calls -- once because the user named it,
#               once because another document imports it -- is recognized as one document.
#               Used only by 'macro_add_generated_document' and only as the tail of a global
#               property name, never as a path.
# Note .......: Compared case-insensitively on Windows, where two spellings of one path name
#               one file, and case-sensitively everywhere else.
# Parameters .: ${doc_key}  [out] -- Name of variable to hold the key.
#               ${doc_path} [in]  -- Path of the model document.
# Usage ......: macro_document_key(<out-var> <document-path>)
# ---------------------------------------------------------------------------
macro(macro_document_key doc_key doc_path)
    file(TO_CMAKE_PATH "${doc_path}" _doc_key_path)
    if (WIN32)
        string(TOLOWER "${_doc_key_path}" _doc_key_path)
    endif()
    string(MAKE_C_IDENTIFIER "${_doc_key_path}" ${doc_key})
    unset(_doc_key_path)
endmacro(macro_document_key)

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
#   macro_get_processor("arm64" AREG_ARCH AREG_BITNESS _entry_found)
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
    message(STATUS "Areg: >>> Validating binary '${path_module}' for compatibility with processor '${target_proc}'")

    # Determine the appropriate objdump command
    if (NOT "${CMAKE_OBJDUMP}" STREQUAL "")
        set(_objdump "${CMAKE_OBJDUMP}")
    else()
        set(_objdump "${target_name}-objdump")
    endif()

    set(${var_compatible} FALSE)
    # Check existence of the binary and objdump tool
    if (EXISTS "${path_module}")

        set(_tool_exists FALSE)
        set(_data "")
        if (APPLE)
            # Check if it's a .tbd file (text-based stub) - these are multi-arch by design
            set(_module_path "${path_module}")
            cmake_path(GET _module_path EXTENSION _file_ext)
            if ("${_file_ext}" STREQUAL ".tbd")
                # .tbd files are text-based stubs provided by Apple's SDK.
                set(${var_compatible} TRUE)
                message(STATUS "Areg: >>> File '${path_module}' is a macOS stub library (.tbd), assuming compatible")
            else()
                # Use lipo for actual Mach-O binaries
                # lipo output: "Non-fat file: /path is architecture: arm64" or
                #              "Architectures in the fat file: /path are: x86_64 arm64"
                execute_process(
                    COMMAND lipo -info "${path_module}"
                    OUTPUT_VARIABLE _data
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET
                    RESULT_VARIABLE _lipo_result
                )
                if (_lipo_result EQUAL 0)
                    set(_tool_exists TRUE)
                endif()
            endif()

        elseif (EXISTS "${_objdump}")
            set(_tool_exists TRUE)
            execute_process(
                COMMAND bash -c "${_objdump} -f ${path_module} | grep ^architecture | cut -d' ' -f2 | sort -u"
                OUTPUT_VARIABLE _data
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
            )
        endif()

        if (_tool_exists)
            macro_get_processor(${target_proc} _proc _bitness _found)
            set(_pos -1)
            # Match the processor type with extracted architecture
            if (${_proc} STREQUAL ${_proc_x86})
                string(FIND "${_data}" "x86_64" _pos)
                if (_pos EQUAL -1)
                    string(FIND "${_data}" "x86-64" _pos)
                endif()
                if (_pos EQUAL -1)
                    string(FIND "${_data}" "i386" _pos)
                endif()
                # For x86 (32-bit), exclude if x86_64 is found
                if (_pos GREATER -1)
                    string(FIND "${_data}" "x86_64" _pos64)
                    string(FIND "${_data}" "x86-64" _pos64_alt)
                    if ((_pos64 GREATER -1) OR (_pos64_alt GREATER -1))
                        set(_pos -1)
                    endif()
                endif()
            elseif (${_proc} STREQUAL ${_proc_x64})
                string(FIND "${_data}" "x86_64" _pos)
                if (_pos EQUAL -1)
                    string(FIND "${_data}" "x86-64" _pos)
                endif()
            elseif (${_proc} STREQUAL ${_proc_arm32})
                string(FIND "${_data}" "arm" _pos)
                # Exclude arm64
                if (_pos GREATER -1)
                    string(FIND "${_data}" "arm64" _pos64)
                    if (_pos64 GREATER -1)
                        set(_pos -1)
                    endif()
                endif()
            elseif (${_proc} STREQUAL ${_proc_arm64})
                # macOS uses "arm64", Linux uses "AARCH64" or "aarch64"
                string(FIND "${_data}" "arm64" _pos)
                if (_pos EQUAL -1)
                    string(FIND "${_data}" "AARCH64" _pos)
                endif()
                if (_pos EQUAL -1)
                    string(FIND "${_data}" "aarch64" _pos)
                endif()
            else()
                string(FIND "${_data}" "${_proc}" _pos)
            endif()

            # Set compatibility flag based on architecture match
            if (_pos GREATER -1)
                set(${var_compatible} TRUE)
            else()
                message(WARNING "Areg: >>> Binary '${path_module}' is NOT compatible with target processor '${target_proc}'. Detected: '${_data}'")
            endif()
        elseif (AREG_PLATFORM_WINDOWS)
            set(${var_compatible} TRUE)
        endif()

    elseif (AREG_PLATFORM_WINDOWS)
        set(${var_compatible} TRUE)
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
        macro_check_module_architect("${${var_library}}" ${AREG_TARGET} ${AREG_ARCH} ${var_found})
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
        if (NOT EXISTS "${GTEST_LIBRARIES}")
            get_target_property(_gtest_location GTest::gtest LOCATION)
            if (NOT _gtest_location)
                get_target_property(_gtest_location GTest::gtest IMPORTED_LOCATION)
                if (NOT _gtest_location)
                    get_target_property(_gtest_location GTest::GTest LOCATION)
                    if (NOT _gtest_location)
                        get_target_property(_gtest_location GTest::GTest IMPORTED_LOCATION)
                    endif()
                endif()
            endif()
        else()
            set(_gtest_location "${GTEST_LIBRARIES}")
        endif()

        if (EXISTS "${_gtest_location}")
            macro_check_module_architect("${_gtest_location}" ${AREG_TARGET} ${AREG_ARCH} ${var_found})
        else()
            set(${var_found} TRUE)
        endif()

        unset(_gtest_location)

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
        macro_check_module_architect("${SQLite3_LIBRARY}" ${AREG_TARGET} ${AREG_ARCH} ${var_found})
    endif()
endmacro(macro_find_sqlite_package)

# ---------------------------------------------------------------------------
# Macro ......: macro_create_option
# Purpose ....: Creates a boolean cache variable with a default value.
# Parameters .: ${var_name}     [out] -- Name of the boolean variable.
#               ${var_value}    [in]  -- Default value if the variable is not yet defined.
#               ${var_describe} [in]  -- Description of the variable for the CMake cache.
# Usage ......: macro_create_option(<name-var> <default-value> <describe>)
# Example ....: macro_create_option(AREG_LOGGING ON "Compile with logs")
# ---------------------------------------------------------------------------
macro(macro_create_option var_name var_value var_describe)
    if (NOT DEFINED ${var_name})
        option(${var_name} "${var_describe}" ${var_value})
    else()
        option(${var_name} "${${var_name}}" ${var_value})
    endif()
endmacro(macro_create_option)

# ---------------------------------------------------------------------------
# Macro ......: macro_check_sse42
# Purpose ....: Verifies that the build host can compile AND execute SSE4.2
#               CRC32C intrinsics when compiled with '-msse4.2'. Used by the
#               GNU and Clang Release configurations to select a portable
#               hardware-accelerated ISA baseline when 'AREG_ARCH_NATIVE' is disabled.
#               The check runs once, the result is cached.
# Parameters .: ${var_name} [out] -- Name of the cached result variable (TRUE/FALSE).
# Usage ......: macro_check_sse42(<out-var>)
# Example ....: macro_check_sse42(AREG_SSE42_HW)
# ---------------------------------------------------------------------------
macro(macro_check_sse42 var_name)
    if (NOT DEFINED ${var_name})
        include(CheckCXXSourceRuns)
        set(CMAKE_REQUIRED_FLAGS "-msse4.2")
        check_cxx_source_runs("
            #include <nmmintrin.h>
            int main()
            {
                volatile unsigned char byte = 42;
                volatile unsigned int crc = _mm_crc32_u8(0xFFFFFFFFu, byte);
                return (crc == 0u) ? 1 : 0;
            }
        " ${var_name})
        unset(CMAKE_REQUIRED_FLAGS)
    endif()
endmacro(macro_check_sse42)

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
            message(FATAL_ERROR "Areg: >>> The item '${_item}' does not exist in '${src_base_dir}'")
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
            if (_ext STREQUAL ".rc")
                list(APPEND ${res_resources} "${_full_path}")
            endif()
        else()
            message(FATAL_ERROR "Areg: >>> File \'${_item}\' does not exist, stopping.")
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
    elseif (UNIX AND NOT APPLE)
        if (${_proc} MATCHES "${_proc_x64}")
            set(${var_name_target} x86_64-linux-gnu)
        elseif (${_proc} MATCHES "${_proc_x86}")
            set(${var_name_target} i386-linux-gnu)
        elseif (${_proc} MATCHES "${_proc_arm64}")
            set(${var_name_target} aarch64-linux-gnu)
        elseif (${_proc} MATCHES "${_proc_arm32}")
            set(${var_name_target} arm-linux-gnueabihf)
        endif()
    elseif (APPLE)
        if (${_proc} MATCHES "${_proc_x64}")
            set(${var_name_target} x86_64-apple-darwin)
        elseif (${_proc} MATCHES "${_proc_arm64}")
            set(${var_name_target} arm64-apple-darwin)
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
    elseif(MINGW)
        if (${_proc} MATCHES "${_proc_x64}")
            set(${var_name_target} "x86_64-w64-mingw32")
        elseif (${_proc} MATCHES "${_proc_x86}")
            set(${var_name_target} "i686-w64-mingw32")
        endif()
    else()
        set(${var_name_target})
    endif()
endmacro(macro_default_target)

# ---------------------------------------------------------------------------
# Macro .......: macro_setup_compilers_data
# Purpose .....: Detects and configures compiler attributes including family, short name, paths, 
#                target, processor architecture, and application bitness. 
# Note ........: In addition to "gnu", "llvm", and "msvc", GNU compilers used in CYGWIN are 
#                categorized under the "cygwin" family, and MINGW is "mingw".
# Parameters ...: ${compiler_path}    [in]       -- Path to the C++ compiler.
#                 ${var_name_family}  [out]      -- Variable to store the compiler family (e.g., "gnu", "msvc", "llvm", "cygwin", "mingw").
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
#                                             AREG_ARCH 
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
    if (NOT "${${var_name_arch}}" STREQUAL "")
        macro_get_processor("${${var_name_arch}}" ${var_name_arch} ${var_name_bitness} _ignore)
    else()
        macro_system_bitness(${var_name_bitness})
    endif()
    
    # Iterate over known compilers to identify the compiler type
    # Note: "c++" and "cc" use "wrapper" family - they need runtime detection since they
    # can be symlinks/wrappers for either GCC or Clang depending on the system
    foreach(_entry "clang-cl;llvm;clang-cl" "appleclang++;llvm;appleclang" "clang++;llvm;clang" "clang;llvm;clang" "g++;gnu;gcc" "gcc;gnu;gcc" "c++;wrapper;cc" "cc;wrapper;cc" "cl;msvc;cl")
        list(GET _entry 0 _cxx_comp)

        # Check if the provided compiler matches the known C++ compiler
        string(TOLOWER "${compiler_path}" _comp_path)
        string(FIND "${_comp_path}" "${_cxx_comp}" _found_pos REVERSE)
        if (_found_pos GREATER -1)
            list(GET _entry 1 _family)
            list(GET _entry 2 _cc_comp)

            # Handle cc/c++ wrapper compilers - detect actual compiler by running --version
            if (${_family} STREQUAL wrapper)
                execute_process(
                    COMMAND "${compiler_path}" --version
                    OUTPUT_VARIABLE _version_output
                    ERROR_QUIET
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    RESULT_VARIABLE _version_result
                )
                string(TOLOWER "${_version_output}" _version_lower)

                if ("${_version_lower}" MATCHES "clang")
                    # cc/c++ is actually Clang (common on macOS and some Linux distros)
                    set(_family "llvm")
                else()
                    # Assume GNU/GCC
                    set(_family "gnu")
                endif()
            endif()

            # Handle special case for CYGWIN and GNU family compilers
            if (${_family} STREQUAL gnu)
                if (CYGWIN)
                    set(${var_name_family} "cygwin")
                    macro_default_target("${${var_name_arch}}" ${var_name_target})
                elseif(MINGW)
                    set(${var_name_family} "mingw")
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
    unset(_version_output)
    unset(_version_lower)
    unset(_version_result)

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
    foreach(_entry "clang++;llvm;clang" "g++;gnu;gcc" "cl;msvc;cl" "g++;cygwin;gcc" "g++;mingw;gcc")
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
                macro_default_target("${AREG_ARCH}" ${var_name_target})
            elseif ("${AREG_ARCH}" STREQUAL "${_proc_arm32}" AND "${_family}" STREQUAL "gnu")
                set(${var_name_short}  g++)
                set(${var_name_cxx}    arm-linux-gnueabihf-g++)
                set(${var_name_c}      arm-linux-gnueabihf-gcc)
                set(${var_name_target} arm-linux-gnueabihf)
            elseif ("${AREG_ARCH}" STREQUAL "${_proc_arm64}" AND "${_family}" STREQUAL "gnu")
                set(${var_name_short}  g++)
                set(${var_name_cxx}    aarch64-linux-gnu-g++)
                set(${var_name_c}      aarch64-linux-gnu-gcc)
                set(${var_name_target} aarch64-linux-gnu)
            else()
                set(${var_name_short} "${_cxx_comp}")
                set(${var_name_cxx}   "${_cxx_comp}")
                set(${var_name_c}     "${_cc_comp}")
                macro_default_target("${AREG_ARCH}" ${var_name_target})
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
#               Automatically links the Areg library, along with any additional libraries specified.
# Parameters .: ${target_name}  -- Name of the executable to apply options to.
#               ${library_list} -- List of additional libraries to link.
# Usage ......: setAppOptions(<target-name> <library-list>)
# ---------------------------------------------------------------------------
function(setAppOptions target_name library_list)

    # Set common compile definitions for the executable
    target_compile_definitions(${target_name} PRIVATE ${COMMON_COMPILE_DEF})

    # Apply common compiler options, such as disabling certain warnings
    target_compile_options(${target_name} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")

    # Link areg library, additional specified libraries, and any extended or extra libraries.
    # Wrapping the archive group in --start-group/--end-group forces the linker to make
    # multiple passes until all inter-archive symbol references are resolved.
    if (NOT MSVC AND NOT APPLE AND "${AREG_LIB_TYPE}" STREQUAL "static")
        target_link_libraries(${target_name} PRIVATE
            -Wl,--start-group
            ${AREG_PACKAGE_NAME}::aregextend
            ${library_list}
            ${AREG_PACKAGE_NAME}::areg
            ${AREG_EXTENDED_LIBS}
            -Wl,--end-group
            ${AREG_LDFLAGS}
            )
    else()
        target_link_libraries(${target_name} PRIVATE
            ${AREG_PACKAGE_NAME}::aregextend   # Areg extended library
            ${library_list}                    # Custom libraries to link
            ${AREG_PACKAGE_NAME}::areg         # Core Areg library
            ${AREG_EXTENDED_LIBS}              # Extended libraries, if any
            ${AREG_LDFLAGS}                    # Linker flags (stdc++, pthread, etc.)
            )
    endif()

endfunction(setAppOptions)

# ---------------------------------------------------------------------------
# Purpose ....: Creates an executable, sets its source files, applies necessary options, 
#               and links it with the provided list of libraries. 
#               The Areg library is automatically linked, no need to specify it.
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
        message(FATAL_ERROR "Areg: >>> Source list for executable \'${target_name}\' is empty")
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

    # Apply compiler and linker options, including linking with Areg and additional libraries
    setAppOptions(${target_name} "${library_list}")

    # Set the include directories for the executable
    target_include_directories(${target_name} BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})

endfunction(addExecutableEx)

# ---------------------------------------------------------------------------
# Function ...: addExecutable
# Purpose ....: Wrapper for addExecutableEx, assuming there is not list of libraries to link with.
#               The Areg library is automatically linked, no need to specify it.
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
#               automatically linking the Areg Framework library along with any 
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

    # Position-independent code is required on non-Windows platforms so that static
    # library object files can be linked into shared libraries or position-independent executables.
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32" OR MINGW)
        target_compile_options(${target_name} PRIVATE -fPIC)
    endif()

    # On Cygwin with a shared areg, AREG_API expands to __attribute__((dllimport)),
    # GNU ld must encounter the areg import library during the same archive.
    # On other platforms areg is intentionally omitted.
    if (CYGWIN AND NOT "${AREG_LIB_TYPE}" STREQUAL "static")
        target_link_libraries(${target_name} PRIVATE
                              ${library_list}
                              ${AREG_PACKAGE_NAME}::areg)
    else()
        target_link_libraries(${target_name} PRIVATE ${library_list})
    endif()

endfunction(setStaticLibOptions)

# ---------------------------------------------------------------------------
# Function ...: addStaticLibEx
# Purpose ....: Creates a static library with specified source files and options,
#               importing and auto-linking the Areg Framework library along with
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

    # Apply compiler and linker options, including linking with Areg and additional libraries
    setStaticLibOptions(${target_name} "${library_list}")

    # Set the include directories for the static library
    target_include_directories(${target_name} BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})

endfunction(addStaticLibEx)

# ---------------------------------------------------------------------------
# Function ...: addStaticLib
# Purpose ....: Wrapper of addStaticLibEx, assuming there is no list of libraries to link.
#               Creates a static library, setting sources and options, importing, 
#               and auto-linking the Areg Framework library.
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
#               importing, and auto-linking the Areg Framework library along 
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

    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32" OR MINGW)
        target_compile_options(${target_name} PRIVATE "-Bstatic")
        target_compile_options(${target_name} PRIVATE -fPIC)
    endif()

    # Cygwin shared builds, record areg as a direct dep.
    # On other platforms, omit areg to avoid duplicate entries on macOS ld.
    if (CYGWIN AND NOT "${AREG_LIB_TYPE}" STREQUAL "static")
        target_link_libraries(${target_name} PRIVATE
                             ${library_list}
                             ${AREG_PACKAGE_NAME}::areg)
    else()
        target_link_libraries(${target_name} PRIVATE ${library_list})
    endif()
endfunction(addStaticLibEx_C)

# ---------------------------------------------------------------------------
# Function ...: addStaticLib_C
# Purpose ....: Wrapper for addStaticLibEx_C, assuming there is no aliasing and 
#               list of libraries for linking. Creates a static library compiled with C,
#               setting sources, importing, and auto-linking the Areg Framework library.
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
#               the Areg Framework library and any additional specified libraries.
# Parameters .: ${target_name}  -- Name of the shared library to apply options to.
#               ${library_list} -- List of libraries for linking.
# Usage ......: setSharedLibOptions(<target-name> <library-list>)
# ---------------------------------------------------------------------------
function(setSharedLibOptions target_name library_list)

    # Apply common compile definitions for shared libraries
    target_compile_definitions(${target_name} PRIVATE ${COMMON_COMPILE_DEF} _USRDLL)
    target_compile_options(${target_name} PRIVATE "${AREG_OPT_DISABLE_WARN_COMMON}")

    # Link the shared library with provided libraries and Areg framework
    target_link_libraries(${target_name} PRIVATE
                          ${AREG_PACKAGE_NAME}::aregextend
                          ${library_list}
                          ${AREG_PACKAGE_NAME}::areg
                          ${AREG_EXTENDED_LIBS}
                          ${AREG_LDFLAGS}
    )

    # Additional compile options for non-Windows platforms
    if (NOT ${AREG_DEVELOP_ENV} MATCHES "Win32" OR MINGW)
        target_compile_options(${target_name} PRIVATE "-Bdynamic") # Ensure dynamic linking
        target_compile_options(${target_name} PRIVATE -fPIC)       # Position-independent code for shared libraries
    endif()

endfunction(setSharedLibOptions)

# ---------------------------------------------------------------------------
# Function ...: addSharedLibEx
# Purpose ....: Creates a shared library with specified source files and options,
#               importing and auto-linking the Areg Framework library along with
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

    # Apply compiler and linker options, including linking with Areg and additional libraries
    setSharedLibOptions(${target_name} "${library_list}")

    # Set the include directories for the shared library
    target_include_directories(${target_name} BEFORE PRIVATE ${CMAKE_CURRENT_LIST_DIR})

endfunction(addSharedLibEx)

# ---------------------------------------------------------------------------
# Function ...: addSharedLib
# Purpose ....: Wrapper for addSharedLibEx, assuming there is no aliasing and no list for linking.
#               Creates a shared library with specified sources, options, imports,
#               and auto-linking the Areg Framework library.
# Parameters .: ${target_name}  -- Name of the shared library to build.
#               ${source_list}  -- List of source files to build the shared library.
# Usage ......: addSharedLib(<target-name> <source-list>)
# ---------------------------------------------------------------------------
function(addSharedLib target_name target_source_list)
    addSharedLibEx(${target_name} "" "${target_source_list}" "")
endfunction(addSharedLib)

# ---------------------------------------------------------------------------
# Macro ......: macro_add_generated_document
# Purpose ....: Runs the code generator on a single model document and adds the produced
#               files to a library. Any document type is accepted, Service Interface
#               (.siml), State Machine (.fsml) or Data Type (.dtml), because the tool picks
#               the generator by file extension. The named wrappers below
#               (addServiceInterface, addStateMachine, addDataType,
#               macro_add_service_interface) only forward to this macro.
#
#               The list of generated files comes from the manifest, not from this file.
#               The generator writes '<Name>.<kind>.files' next to the output, one tagged
#               entry per line: 'out:' for a generated file, 'in:' for a document that was
#               read. A missing manifest aborts the configuration, since it means the tool
#               did not run or rejected the document.
#
#               Put the generated files of one project in one library. Calling this macro
#               several times with the same target is the normal usage, and CMake drops
#               repeated file paths on its own.
#
#               A document that is imported by another document is generated together with
#               its host. Naming such an import in a separate call to the same library is
#               therefore skipped: the generator has already written its files. A call for a
#               different library is a different program, so it gets its own copy.
#
#               Every document a run touches is placed by its own location: the generate root
#               plus the document's parent path under the project root. Two documents that
#               import a third one therefore produce one copy of it, at one path.
#
# Parameters .: ${lib_name}         -- Library to receive the generated files. Created if it does not exist.
#               ${model_doc}        -- Path to the model document (.siml, .fsml, .dtml), in the form
#                                      CMake uses, absolute or relative to ${source_root}. The
#                                      conversion for the generator happens here.
#               ${source_root}      -- The project root the document belongs to, absolute or relative to
#                                      the directory being configured. The generator resolves every path
#                                      against it, including the imports a document names.
#               ${codegen_root}     -- The generate root: the single folder the generated files of the
#                                      whole project are written under, absolute or relative to
#                                      ${source_root}.
#               ${output_path}      -- The document's own parent path, relative to ${source_root}. That is
#                                      where the generator writes this document's files and its manifest.
#               ${codegen_tool}     -- Full path to codegen.jar.
#               ${lib_type}         -- Optional: 'static' (default) or 'shared'. Ignored, with a note, if the target already exists.
#               ${export_keyword}   -- Optional: passed to the tool as --export=<KEYWORD>, and only when non-empty.
#
# Usage ......: macro_add_generated_document(<name-lib> <full-path-doc> <source-root> <root-gen> <doc-parent-path> <codegen-tool> [<lib-type>] [<export>])
# Example ....:
#   macro_add_generated_document(funlib "/home/dev/fun/src/service/HelloWorld.siml" "/home/dev/fun/src" "/home/dev/fun/product/generate" "service" /tools/areg/codegen.jar)
#   macro_add_generated_document(funlib "/home/dev/fun/src/fsm/TrafficLight.fsml"   "/home/dev/fun/src" "/home/dev/fun/product/generate" "fsm"     /tools/areg/codegen.jar)
# ---------------------------------------------------------------------------
macro(macro_add_generated_document lib_name model_doc source_root codegen_root output_path codegen_tool)

    # return() in a macro exits the calling function, not the macro. The guards below
    # rely on FATAL_ERROR to stop the configuration. Do not lower them to WARNING.
    if (NOT ${Java_FOUND})
        message(FATAL_ERROR "Areg Setup: Java not found! Install Java 17 or higher to run the code generator.")
        return()
    endif()

    # Two forms of the same path, differing only under Cygwin. The generator runs there as
    # a Windows program and gets the drive letter form, while the manifest lookup, the
    # source list and the configure dependency keep the form CMake can open.
    macro_cmake_path(_doc_path "${model_doc}")
    macro_cmake_path(_src_root "${source_root}")
    macro_cmake_path(_gen_root "${codegen_root}")
    file(TO_CMAKE_PATH "${output_path}" _gen_target)

    # Absolute or relative is the caller's choice on all three. The project root is taken from
    # the directory being configured, the document and the generate root from the project root,
    # which is the rule the generator states for its own arguments.
    macro_absolute_path(_src_root "${_src_root}" "${CMAKE_CURRENT_SOURCE_DIR}")
    macro_absolute_path(_doc_path "${_doc_path}" "${_src_root}")
    macro_absolute_path(_gen_root "${_gen_root}" "${_src_root}")

    if (NOT EXISTS "${_doc_path}")
        message(FATAL_ERROR "Areg Setup: The model document \'${model_doc}\' does not exist. Cannot generate files.")
        return()
    endif()

    cmake_path(GET _doc_path STEM _doc_name)
    if ("${_doc_name}" STREQUAL "")
        message(FATAL_ERROR "Areg Setup: The path \'${model_doc}\' has no file name. Cannot generate files.")
        return()
    endif()

    set(_lib_type "static")
    if (${ARGC} GREATER 6 AND NOT "${ARGV6}" STREQUAL "")
        set(_lib_type "${ARGV6}")
    endif()

    set(_export_keyword "")
    if (${ARGC} GREATER 7)
        set(_export_keyword "${ARGV7}")
    endif()

    # Nothing to do if an earlier call already generated this document into the same
    # library as an import. A different library gets its own copy.
    macro_document_key(_doc_id "${_doc_path}")
    get_property(_doc_owner GLOBAL PROPERTY AREG_GENDOC_${_doc_id}_LIB)
    set(_doc_skip FALSE)
    if (_doc_owner)
        if ("${_doc_owner}" STREQUAL "${lib_name}")
            set(_doc_skip TRUE)
            message(STATUS "Areg Setup: \'${model_doc}\' is already generated into \'${lib_name}\' by the document that imports it. Nothing to do.")
        else()
            message(WARNING "Areg Setup: \'${model_doc}\' is generated into \'${_doc_owner}\' and now into \'${lib_name}\' as well. Linking both libraries into one program is a duplicate symbol error. All generated documents of a project belong in one library.")
        endif()
    endif()

    if (NOT _doc_skip)

        # Where this document's own files and its manifest land.
        set(_generate "${_gen_root}/${_gen_target}")

        # Run the code generator. The export keyword is passed only when requested: a document
        # used inside its own library needs none, and a static library must never get one.
        #
        # '--root' is the project root and not the generate folder: an import is named by a path
        # anchored there, and the tool cannot find it otherwise. '--target' is the generate root
        # for the whole project, absolute, because an out-of-source build can put it on another
        # drive. The tool gives each document the folder its own path names underneath it.
        macro_normalize_path(_tool_doc    "${_doc_path}")
        macro_normalize_path(_tool_root   "${_src_root}")
        macro_normalize_path(_tool_target "${_gen_root}")
        macro_normalize_path(_tool_jar    "${codegen_tool}")

        set(_codegen_args --doc=${_tool_doc} --root=${_tool_root} --target=${_tool_target})
        if (NOT "${_export_keyword}" STREQUAL "")
            list(APPEND _codegen_args --export=${_export_keyword})
        endif()

        # Check the exit code here. A rejected document that reaches the compiler shows up as
        # a missing header, naming neither the document nor the reason.
        execute_process(COMMAND ${Java_JAVA_EXECUTABLE} -jar ${_tool_jar} ${_codegen_args}
                        RESULT_VARIABLE _codegen_result)
        if (NOT _codegen_result EQUAL 0)
            message(FATAL_ERROR "Areg Setup: The code generator refused or failed on \'${model_doc}\' (exit ${_codegen_result}). See the output above for the rule and the element.")
            return()
        endif()

        # Find the manifest this run wrote. It is named after the document's declared name and
        # not after the file, and the two are allowed to differ, so the file name cannot be the
        # search key. Read the manifests of the folder instead and let each one say what it is.
        #
        # A manifest lists the documents its run read in 'in:' lines, the imports first and the
        # named document last. That last entry tells this run's manifest from the manifest of
        # another document sharing the folder, and from the manifest of a run that read this
        # document only as an import. The kind in the name ('.si', '.fsm', '.dt') is what lets
        # 'HelloWorld.siml' and 'HelloWorld.fsml' live side by side.
        file(GLOB _manifests "${_generate}/*.files")
        set(_manifests_kept)
        foreach(_manifest IN LISTS _manifests)
            file(STRINGS "${_manifest}" _in_lines REGEX "^in:")
            list(LENGTH _in_lines _in_count)
            if (_in_count GREATER 0)
                math(EXPR _in_count "${_in_count} - 1")
                list(GET _in_lines ${_in_count} _line)
                string(REGEX REPLACE "^in:" "" _line "${_line}")
                macro_cmake_path(_in_path "${_line}")
                macro_document_key(_in_id "${_in_path}")
                if ("${_in_id}" STREQUAL "${_doc_id}")
                    list(APPEND _manifests_kept "${_manifest}")
                endif()
            endif()
        endforeach()

        if (_manifests_kept)
            set(_manifests "${_manifests_kept}")
        else()
            # No manifest claimed the document. Fall back to the file name, which is the
            # manifest name whenever the author did not give the document a different one.
            file(GLOB _manifests "${_generate}/${_doc_name}.*.files")
        endif()
        unset(_manifests_kept)
        unset(_in_count)

        if (NOT _manifests)
            message(FATAL_ERROR "Areg Setup: The code generator wrote no manifest for \'${model_doc}\'. Expected a \'<name>.<kind>.files\' file under \'${_generate}\'.")
            return()
        endif()

        set(_sources)
        set(_inputs)
        foreach(_manifest IN LISTS _manifests)
            file(STRINGS "${_manifest}" _out_lines REGEX "^out:")
            foreach(_line IN LISTS _out_lines)
                string(REGEX REPLACE "^out:" "" _line "${_line}")
                list(APPEND _sources "${_gen_root}/${_line}")
            endforeach()

            file(STRINGS "${_manifest}" _in_lines REGEX "^in:")
            foreach(_line IN LISTS _in_lines)
                string(REGEX REPLACE "^in:" "" _line "${_line}")
                macro_cmake_path(_in_path "${_line}")
                list(APPEND _inputs "${_in_path}")
            endforeach()
        endforeach()

        if (NOT _sources)
            message(FATAL_ERROR "Areg Setup: The manifest for \'${model_doc}\' lists no generated file.")
            return()
        endif()

        # Remember every document this run read, so that a later call naming one of them,
        # normally an import, knows it has nothing to do. The folder of a document follows the
        # document and not the caller, so two calls that reach one import agree on where its
        # files are. They disagree only if the two calls gave different project roots, and that
        # does put two copies of the same classes in one library. Report it here.
        foreach(_input IN LISTS _inputs)
            cmake_path(RELATIVE_PATH _input BASE_DIRECTORY "${_src_root}" OUTPUT_VARIABLE _input_rel)
            cmake_path(GET _input_rel PARENT_PATH _input_gen)
            # A document above the project root keeps its files inside the generate root.
            string(REGEX REPLACE "^(\\.\\./)+" "" _input_gen "${_input_gen}")
            set(_input_gen "${_gen_root}/${_input_gen}")

            macro_document_key(_input_id "${_input}")
            get_property(_input_lib GLOBAL PROPERTY AREG_GENDOC_${_input_id}_LIB)
            get_property(_input_dir GLOBAL PROPERTY AREG_GENDOC_${_input_id}_DIR)
            if (_input_lib AND "${_input_lib}" STREQUAL "${lib_name}" AND NOT "${_input_dir}" STREQUAL "${_input_gen}")
                message(WARNING "Areg Setup: \'${_input}\' is generated into \'${lib_name}\' twice, under \'${_input_dir}\' and under \'${_input_gen}\'. That is two copies of the same classes in one library. The two calls that reach this document gave different project roots; give them one root so the document keeps one place.")
            endif()
            set_property(GLOBAL PROPERTY AREG_GENDOC_${_input_id}_LIB "${lib_name}")
            set_property(GLOBAL PROPERTY AREG_GENDOC_${_input_id}_DIR "${_input_gen}")
        endforeach()

        # Re-run CMake when any document the generator read is edited, not only the one this
        # call named. Otherwise editing an import regenerates nothing and the build quietly
        # compiles the previous code.
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${_inputs})

        # Add to build targets if in generate-only mode
        if (AREG_GENERATE_ONLY)
            message(STATUS "Areg: >>> AREG_GENERATE_ONLY=ON: Skipping library '${lib_name}'")
            if (NOT TARGET ${lib_name})
                add_library(${lib_name} INTERFACE)
            endif()
        elseif (TARGET ${lib_name})
            # The target already exists, so the generated sources join it and the requested
            # library type does not apply. Report it instead of ignoring it quietly.
            if (NOT "${_lib_type}" STREQUAL "static")
                message(STATUS "Areg Setup: Target '${lib_name}' already exists; the requested '${_lib_type}' library type is ignored.")
            endif()

            target_sources(${lib_name} PRIVATE "${_sources}")

            # Set on the sources and not on the target. Warnings in generated code cannot be
            # fixed by the developer, while the rest of the target keeps its own warning level.
            set_source_files_properties(${_sources} PROPERTIES COMPILE_OPTIONS "${AREG_OPT_DISABLE_WARN_CODEGEN}")
        elseif ("${_lib_type}" STREQUAL "shared")
            message(STATUS "Areg Setup: Adding new generated shared library ${lib_name}")
            addSharedLib(${lib_name} "${_sources}")
            target_compile_options(${lib_name} PRIVATE "${AREG_OPT_DISABLE_WARN_CODEGEN}")
        else()
            message(STATUS "Areg Setup: Adding new generated static library ${lib_name}")
            addStaticLib(${lib_name} "${_sources}")
            target_compile_options(${lib_name} PRIVATE "${AREG_OPT_DISABLE_WARN_CODEGEN}")
        endif()

    endif()

    unset(_doc_id)
    unset(_doc_owner)
    unset(_doc_skip)
    unset(_input)
    unset(_input_id)
    unset(_input_lib)
    unset(_input_dir)
    unset(_input_rel)
    unset(_input_gen)
    unset(_doc_path)
    unset(_doc_name)
    unset(_src_root)
    unset(_gen_root)
    unset(_gen_target)
    unset(_lib_type)
    unset(_export_keyword)
    unset(_generate)
    unset(_tool_doc)
    unset(_tool_root)
    unset(_tool_target)
    unset(_tool_jar)
    unset(_codegen_args)
    unset(_codegen_result)
    unset(_manifests)
    unset(_manifest)
    unset(_out_lines)
    unset(_in_lines)
    unset(_line)
    unset(_in_path)
    unset(_in_id)
    unset(_sources)
    unset(_inputs)

endmacro(macro_add_generated_document)

# ---------------------------------------------------------------------------
# Macro ......: macro_add_service_interface
# Purpose ....: A thin forwarder to macro_add_generated_document, kept because projects call
#               it directly. It adds nothing of its own; see that macro for what happens.
# Parameters .: The same, with ${interface_doc} being a Service Interface document (.siml).
# Usage ......: macro_add_service_interface(<name-lib> <full-path-siml> <source-root> <root-gen> <doc-parent-path> <codegen-tool> [<lib-type>] [<export>])
# ---------------------------------------------------------------------------
macro(macro_add_service_interface lib_name interface_doc source_root codegen_root output_path codegen_tool)
    macro_add_generated_document(${lib_name} "${interface_doc}" "${source_root}" "${codegen_root}" "${output_path}" "${codegen_tool}" ${ARGN})
endmacro(macro_add_service_interface)

# ---------------------------------------------------------------------------
# Function ...: addGeneratedDocumentImpl
# Purpose ....: The shared body of addServiceInterfaceEx, addStateMachineEx and addDataTypeEx:
#               take the document's own parent path as the output path and call
#               macro_add_generated_document. It is separate only so the public functions
#               cannot drift apart -- there is nothing document-type specific in here, which
#               is the point.
#
#               A FUNCTION and not a macro, deliberately: 'cmake_path(GET <var> ...)' needs a
#               real variable to read, and a macro parameter is a text substitution rather
#               than a variable.
# Parameters .: As addServiceInterfaceEx / addStateMachineEx / addDataTypeEx, with the optional
#               <lib-type> and <export> arriving in ${ARGN}.
# Usage ......: addGeneratedDocumentImpl(<library-name> <source-root> <doc-relative-path> [<lib-type>] [<export>])
# ---------------------------------------------------------------------------
function(addGeneratedDocumentImpl lib_name source_root doc_path)

    # The Ex functions used to take a generate path in this position. The output folder now
    # follows the document, so a fourth argument can only be the library type. Catch the old
    # call shape here instead of letting a path be read as a library type.
    set(_first_option "")
    if (${ARGC} GREATER 3)
        set(_first_option "${ARGV3}")
    endif()
    if (NOT "${_first_option}" STREQUAL "" AND NOT "${_first_option}" STREQUAL "static" AND NOT "${_first_option}" STREQUAL "shared")
        message(FATAL_ERROR "Areg Setup: \'${_first_option}\' is not a library type. The generate path argument of addServiceInterfaceEx, addStateMachineEx and addDataTypeEx was removed, because a document's generated files now go under the document's own path. Drop that argument. The call reads: addServiceInterfaceEx(<library> <source-root> <document> [static|shared] [<export>]).")
        return()
    endif()

    # The document is named relative to the source root, or by an absolute path. Both reach
    # the same generated files: the output folder mirrors where the document sits under the
    # source root either way.
    macro_absolute_path(_src_root "${source_root}" "${CMAKE_CURRENT_SOURCE_DIR}")
    macro_absolute_path(_doc_full "${doc_path}" "${_src_root}")
    cmake_path(RELATIVE_PATH _doc_full BASE_DIRECTORY "${_src_root}" OUTPUT_VARIABLE _doc_rel)
    cmake_path(GET _doc_rel PARENT_PATH _gen_path)

    # A document above the source root keeps its generated files inside the generate root.
    string(REGEX REPLACE "^(\\.\\./)+" "" _gen_path "${_gen_path}")

    # The paths are handed over as CMake knows them. Converting them for the generator is
    # the job of the macro, which is the only place that talks to it.
    macro_add_generated_document(${lib_name}
                                 "${_doc_full}"
                                 "${_src_root}"
                                 "${AREG_GENERATE_DIR}"
                                 "${_gen_path}"
                                 "${AREG_SDK_TOOLS}/codegen.jar"
                                 ${ARGN})
endfunction(addGeneratedDocumentImpl)

# ---------------------------------------------------------------------------
# Function ...: addServiceInterfaceEx
# Purpose ....: Generates the code of one Service Interface document (.siml) into a library.
#               The code generator tool is assumed to be at ${AREG_SDK_TOOLS}/codegen.jar and
#               the generated files are placed under ${AREG_GENERATE_DIR}, in the folder the
#               document's own path names under ${source_root}.
# Parameters .: ${lib_name}         -- The name of the library to receive the generated code.
#               ${source_root}      -- The root directory containing the project's source files.
#               ${siml_path}        -- Path to the Service Interface document (.siml), relative to ${source_root}.
#               ${lib_type}         -- Optional: 'static' (default) or 'shared'.
#               ${export_keyword}   -- Optional: the symbol export keyword. Empty by default for BOTH library
#                                      types, including shared: an interface used only inside its own library
#                                      needs no keyword, and a static library must never be given one.
# Usage ......: addServiceInterfaceEx(<library-name> <source-root> <siml-relative-path> [<lib-type>] [<export>])
# Example ....:
#   addServiceInterfaceEx(fun_library "/home/dev/project/fun/src" "fun/service/interfaces/FunService.siml")
# ---------------------------------------------------------------------------
function(addServiceInterfaceEx lib_name source_root siml_path)
    addGeneratedDocumentImpl("${lib_name}" "${source_root}" "${siml_path}" ${ARGN})
endfunction(addServiceInterfaceEx)

# ---------------------------------------------------------------------------
# Function ...: addServiceInterface
# Purpose ....: Simplified wrapper for addServiceInterfaceEx, taking the source root to be
#               ${PROJECT_SOURCE_DIR} and mirroring the document's parent path under
#               ${AREG_GENERATE_DIR}.
# Parameters .: ${lib_name}     -- The name of the library to receive the generated code.
#               ${siml_path}    -- Path to the Service Interface document (.siml), relative to PROJECT_SOURCE_DIR.
# Usage ......: addServiceInterface(<library-name> <siml-relative-path>)
# Example ....:
#   addServiceInterface(fun_library fun/service/interface/FunService.siml)
# ---------------------------------------------------------------------------
function(addServiceInterface lib_name siml_path)
    addServiceInterfaceEx(${lib_name} "${PROJECT_SOURCE_DIR}" "${siml_path}")
endfunction(addServiceInterface)

# ---------------------------------------------------------------------------
# Function ...: addStateMachineEx
# Purpose ....: Generates the code of one State Machine document (.fsml) into a library.
#               The same jar, the same generated tree and the same rules as
#               addServiceInterfaceEx -- only the document type differs, and the tool picks
#               its generator from the extension.
#
#               A document that IMPORTS other machines needs no more calls than one that does
#               not: the generator produces the whole closure and lists all of it in the
#               manifest. Naming an import in a second call is harmless and generates nothing
#               twice.
# Parameters .: ${lib_name}         -- The name of the library to receive the generated code.
#               ${source_root}      -- The root directory containing the project's source files.
#               ${fsml_path}        -- Path to the State Machine document (.fsml), relative to ${source_root}.
#               ${lib_type}         -- Optional: 'static' (default) or 'shared'.
#               ${export_keyword}   -- Optional: the symbol export keyword, empty by default.
# Usage ......: addStateMachineEx(<library-name> <source-root> <fsml-relative-path> [<lib-type>] [<export>])
# Example ....:
#   addStateMachineEx(fun_library "/home/dev/project/fun/src" "fun/fsm/TrafficLight.fsml")
# ---------------------------------------------------------------------------
function(addStateMachineEx lib_name source_root fsml_path)
    addGeneratedDocumentImpl("${lib_name}" "${source_root}" "${fsml_path}" ${ARGN})
endfunction(addStateMachineEx)

# ---------------------------------------------------------------------------
# Function ...: addStateMachine
# Purpose ....: Simplified wrapper for addStateMachineEx, taking the source root to be
#               ${PROJECT_SOURCE_DIR} and mirroring the document's parent path under
#               ${AREG_GENERATE_DIR}. The .fsml counterpart of addServiceInterface, and
#               deliberately the same call shape.
# Parameters .: ${lib_name}     -- The name of the library to receive the generated code.
#               ${fsml_path}    -- Path to the State Machine document (.fsml), relative to PROJECT_SOURCE_DIR.
# Usage ......: addStateMachine(<library-name> <fsml-relative-path>)
# Example ....:
#   addStateMachine(fun_library fun/fsm/TrafficLight.fsml)
# ---------------------------------------------------------------------------
function(addStateMachine lib_name fsml_path)
    addStateMachineEx(${lib_name} "${PROJECT_SOURCE_DIR}" "${fsml_path}")
endfunction(addStateMachine)

# ---------------------------------------------------------------------------
# Function ...: addDataTypeEx
# Purpose ....: Generates the code of one Data Type document (.dtml) into a library.
#               The same jar, the same generated tree and the same rules as
#               addServiceInterfaceEx -- only the document type differs.
#
#               A .dtml that is INCLUDED by a service interface or a state machine needs no
#               call of its own: the generator produces it together with the document that
#               includes it and lists it in the manifest. Call this only for a data type
#               document that nothing includes, or to place one in the build before the
#               documents that use it.
# Parameters .: ${lib_name}         -- The name of the library to receive the generated code.
#               ${source_root}      -- The root directory containing the project's source files.
#               ${dtml_path}        -- Path to the Data Type document (.dtml), relative to ${source_root}.
#               ${lib_type}         -- Optional: 'static' (default) or 'shared'.
#               ${export_keyword}   -- Optional: the symbol export keyword, empty by default.
# Usage ......: addDataTypeEx(<library-name> <source-root> <dtml-relative-path> [<lib-type>] [<export>])
# Example ....:
#   addDataTypeEx(fun_library "/home/dev/project/fun/src" "fun/common/SharedTypes.dtml")
# ---------------------------------------------------------------------------
function(addDataTypeEx lib_name source_root dtml_path)
    addGeneratedDocumentImpl("${lib_name}" "${source_root}" "${dtml_path}" ${ARGN})
endfunction(addDataTypeEx)

# ---------------------------------------------------------------------------
# Function ...: addDataType
# Purpose ....: Simplified wrapper for addDataTypeEx, taking the source root to be
#               ${PROJECT_SOURCE_DIR}. The .dtml counterpart of addServiceInterface, and
#               deliberately the same call shape.
# Parameters .: ${lib_name}     -- The name of the library to receive the generated code.
#               ${dtml_path}    -- Path to the Data Type document (.dtml), relative to PROJECT_SOURCE_DIR.
# Usage ......: addDataType(<library-name> <dtml-relative-path>)
# Example ....:
#   addDataType(fun_library fun/common/SharedTypes.dtml)
# ---------------------------------------------------------------------------
function(addDataType lib_name dtml_path)
    addDataTypeEx(${lib_name} "${PROJECT_SOURCE_DIR}" "${dtml_path}")
endfunction(addDataType)

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
        message(FATAL_ERROR "Areg: >>> Source list to build static library \'${exe_name}\' is empty")
    endif()

    if (AREG_GENERATE_ONLY)
        message(STATUS "Areg Setup: AREG_GENERATE_ONLY is ON. Skipping static library compilation for ${lib_name}")
        if (NOT TARGET ${lib_name})
            add_library(${lib_name} INTERFACE)
        endif()
    else()
        # Declare the static library using gathered sources and libraries
        addStaticLibEx(${lib_name} "" "${_sources}" "${_libs}")

        # If on Windows, set the RC files' language property
        if ((AREG_DEVELOP_ENV MATCHES "Win32") AND (NOT MINGW) AND _resources)
            set_source_files_properties(${_resources} PROPERTIES LANGUAGE RC)
        endif()
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
        message(FATAL_ERROR "Areg: >>> Source list to build shared library \'${exe_name}\' is empty")
    endif()

    if (AREG_GENERATE_ONLY)
        message(STATUS "Areg Setup: AREG_GENERATE_ONLY is ON. Skipping shared library compilation for ${lib_name}")
        if (NOT TARGET ${lib_name})
            add_library(${lib_name} INTERFACE)
        endif()
    else()
        # Declare the shared library using gathered sources and libraries
        addSharedLibEx(${lib_name} "" "${_sources}" "${_libs}")

        # If on Windows, set the RC files' language property
        if ((AREG_DEVELOP_ENV MATCHES "Win32") AND (NOT MINGW) AND _resources)
            set_source_files_properties(${_resources} PROPERTIES LANGUAGE RC)
        endif()
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
        message(FATAL_ERROR "Areg: >>> Source list to build executable \'${exe_name}\' is empty")
    endif()

    # Determine if executable should be built or skipped due to generate-only mode
    if (AREG_GENERATE_ONLY)
        message(STATUS "Areg Setup: AREG_GENERATE_ONLY is ON. Skipping executable compilation for ${exe_name}")

        if (NOT TARGET ${exe_name})
            add_executable(${exe_name} "dummy.cpp")
            set_source_files_properties("dummy.cpp" PROPERTIES GENERATED TRUE)
            set_target_properties(${exe_name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
        endif()
    else()
        # Declare the executable using gathered sources and libraries
        addExecutableEx(${exe_name} "" "${_sources}" "${_libs}")
    
        # If on Windows, set the RC files' language property
        if ((AREG_DEVELOP_ENV MATCHES "Win32") AND (NOT MINGW) AND _resources)
            set_source_files_properties(${_resources} PROPERTIES LANGUAGE RC)
        endif()
    endif()

    # Clean up temporary variables
    unset(_sources)
    unset(_libs)
    unset(_resources)

endmacro(macro_declare_executable)

# ---------------------------------------------------------------------------
# Function ...: printAregConfigStatus
# Purpose ....: Prints a detailed status of Areg's CMake configuration, including details of the build environment.
# Parameters .: - ${var_make_print} -- Boolean flag indicating whether to print the status message (if FALSE, the function exits without printing).
#               - ${var_prefix}     -- A prefix added to each line of the status message (e.g., project name or custom label).
#               - ${var_header}     -- A custom header message displayed at the beginning of the status report.
#               - ${var_footer}     -- A custom footer message displayed at the end of the status report.
# Usage ......: printAregConfigStatus(<flag-to-print> <prefix> <header-output> <footer-output>)
# Example ....: printAregConfigStatus(
#                                   TRUE
#                                   "Areg"
#                                   "----------------------> Areg project CMake Status Report Begin <-----------------------"
#                                   "-----------------------> Areg project CMake Status Report End <------------------------"
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
    message(STATUS "${var_prefix}: >>> Build Environment ..: System '${CMAKE_SYSTEM_NAME}', ${AREG_BITNESS}-bit '${AREG_ARCH}' platform, Env '${AREG_DEVELOP_ENV}'")
    message(STATUS "${var_prefix}: >>> Used CXX-Compiler ..: '${CMAKE_CXX_COMPILER}'")
    message(STATUS "${var_prefix}: >>> Used C-Compiler ....: '${CMAKE_C_COMPILER}'")
    message(STATUS "${var_prefix}: >>> Compiler Version ...: C++ standard 'c++${CMAKE_CXX_STANDARD}', compiler family '${AREG_COMPILER_FAMILY}', target '${CMAKE_CXX_COMPILER_TARGET}'")
    message(STATUS "${var_prefix}: >>> Areg SDK Root ......: '${AREG_SDK_ROOT}'")
    message(STATUS "${var_prefix}: >>> CMake Build Dir ....: '${CMAKE_BINARY_DIR}'")
    message(STATUS "${var_prefix}: >>> Binary Output Dir ..: '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'")
    message(STATUS "${var_prefix}: >>> Generated Files Dir : '${AREG_GENERATE_DIR}'")
    message(STATUS "${var_prefix}: >>> Packages Dir .......: '${FETCHCONTENT_BASE_DIR}'")
    message(STATUS "${var_prefix}: >>> Build Modules ......: areg = '${AREG_LIB_TYPE}', aregextend = static, areglogger = '${AREG_LOGGER_LIB_TYPE}', executable extension '${CMAKE_EXECUTABLE_SUFFIX}'")
    message(STATUS "${var_prefix}: >>> Java Version .......: '${Java_VERSION_STRING}', Java executable = '${Java_JAVA_EXECUTABLE}', minimum version required = 17")
    message(STATUS "${var_prefix}: >>> Packages Use .......: SQLite3 package use = '${AREG_SYSTEM_SQLITE}', GTest package use = '${AREG_SYSTEM_GTEST}'")
    message(STATUS "${var_prefix}: >>> Feature Options ....: Logs = '${AREG_LOGGING}', Extended = '${AREG_EXTENDED}'")
    message(STATUS "${var_prefix}: >>> Other Options ......: Examples = '${AREG_EXAMPLES}', Unit Tests = '${AREG_TESTS}'")
    message(STATUS "${var_prefix}: >>> Installation .......: Enabled = '${AREG_INSTALL}', location = '${CMAKE_INSTALL_PREFIX}'")

    # Print the footer section with separators
    message(STATUS "=======================================================================================")
    message(STATUS "${var_footer}")
    message(STATUS "=======================================================================================")

endfunction(printAregConfigStatus)
