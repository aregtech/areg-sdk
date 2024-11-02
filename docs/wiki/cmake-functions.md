# AREG SDK CMake Functions and Macro

The [functions.cmake](./../../conf/cmake/functions.cmake) ile in AREG SDK includes custom CMake functions and macros that streamline configuration for projects built on the AREG Framework. Below is a breakdown of each function, highlighting its purpose, usage, and parameters.

---

## Introduction

The [functions.cmake](./../../conf/cmake/functions.cmake)  contains reusable CMake utilities designed to simplify repetitive tasks, supporting a cleaner, more maintainable project setup.

> [!IMPORTANT]
> Include [setup.cmake](./../../conf/cmake/setup.cmake) (not functions.cmake) in `CMakeLists.txt` files to ensure proper AREG Framework integration, after setting `AREG_SDK_ROOT` to the `<areg-sdk>` installation directory. More details about integration are described in the [AREG Framework Integration](./cmake-integrate.md) document of AREG SDK Wiki.

---

## CMake Macro Overview

### macro_check_fix_areg_cxx_standard
- **Syntax**: `macro_check_fix_areg_cxx_standard()`
- **Purpose**: Validates and sets C++ standard compatibility. The variable 'AREG_CXX_STANDARD' must be defined before calling this macro.
- **Details**: Ensures `CMAKE_CXX_STANDARD` matches `AREG_CXX_STANDARD`. If incompatible, outputs a warning.
- **Usage**: `macro_check_fix_areg_cxx_standard()`

### macro_normalize_path
- **Syntax**: `macro_normalize_path(normal_path os_path)`
- **Purpose**: Normalizes Windows paths to Cygwin format if applicable.
- **Note**: This macro does not address OS-specific path separator issues.
- **Parameters**:
  - `normal_path` [out]: Path normalized.
  - `os_path` [in]: Windows specific path to normalize.
- **Usage**: `macro_normalize_path(<out-var> <windows-path>)`

### macro_add_service_interface
- **Syntax**: `macro_add_service_interface(interface_doc codegen_root output_path codegen_tool)`
- **Purpose**: From the given Service Interface document file (`*.siml`), generates and adds service specific files to a static library..
- **Parameters**:
  - `lib_name` [in]: Name of the static library.
  - `interface_doc` [in]:Full path to the Service Interface document file (`.siml`). 
  - `codegen_root` [in]: Root directory to generate files.
  - `output_path` [in]: Relative path to generate files.
  - `codegen_tool` [in]: Full path to the code generator tool.
- **Usage**: `macro_add_service_interface(<name-lib> <full-path-siml> <root-gen> <relative-path> <codegen-tool>)`
- **Example:
   ```cmkae
   macro_add_service_interface(funlib "~/project/fun/src/service/HelloWorld.siml" "~/project/fun/product" "generate/service" /tools/areg/codegen.jar)
   macro_add_service_interface(funlib "~/project/fun/src/service/WeHaveFun.siml"  "~/project/fun/product" "generate/service" /tools/areg/codegen.jar)
   ```

### macro_find_package
- **Syntax**: `macro_find_package(package_name package_found package_includes package_libraries)`
- **Purpose**: Finds a package, returning paths to includes and libraries if found.
- **Parameters**:
  - `package_name` [in]: package name to search.
  - `package_found` [out]: Variable to hold package found flag.
  - `package_includes` [out]: Variable to hold package's include directories.
  - `package_libraries` [out]: Variable to hold package's libraries.
- **Usage**: `macro_find_package(<package-name> <found-flag-var> <includes-var> <libraries-var>)`
- **Example**:
   ```cmake
   set(SQLITE_FOUND FALSE)
   macro_find_package(SQLite3 SQLITE_FOUND SQLITE_INCLUDE SQLITE_LIB)
   ```

### macro_create_option
- **Syntax**: `macro_create_option(var_name var_value var_describe)`
- **Purpose**: Creates a boolean cache variable with a default value.
- **Parameters**: 
  - `var_name` [out]: Name of the boolean variable.
  - `var_value` [in]: Default value if the variable is not yet defined.
  - `var_describe` [in]: A brief Purpose of the variable for CMake cache.
- **Usage**: `macro_create_option(<name-var> <default-value> <describe>)`
- **Example**:
   ```cmake
   macro_create_option(AREG_LOGS ON "Compile with logs")
   ```

### macro_add_source
- **Syntax**: `macro_add_source(result_list src_base_dir ...)`
- **Purpose**: Adds existing source files to a list based on a base directory. Checks the file existence.
- **Parameters**:
  - `result_list` [in, out]: List of source files.
  - `src_base_dir` [in]: Base directory of the source files.
  - `${ARGN}` [in]: List of source files (should be relative to the base directory).
- **Usage**: `macro_add_source(<src-list-var> <base-dirpath> <files>)`
- **Example**:
   ```cmake
   set(aregextend_SRC)
   macro_add_source(aregextend_SRC "${AREG_FRAMEWORK}" aregextend/db/private/LogSqliteDatabase.cpp ...)
   ```

### macro_parse_arguments
- **Syntax**: `macro_parse_arguments(res_sources res_libs res_resources)`
- **Purpose**: Parses files and libraries into separate lists for sources, libraries, and resources. The library names should match the name of any known targets. The resource files should have `.rc` extension (Windows specific), source files should have `.cpp`, `.cxx` or `.c` file extension, and should exist on file system.
- **Note**:
  - Throws an error if file with relative or path does not exist.
  - The resource file with extension `.rc` is relevant only to the Windows system. On other systems it will be considered as a source file.
- **Parameters**:
  - `res_sources` [out]: List of source files.
  - `res_libs` [out]: List containing the recognized CMake targets.
  - `res_resources` [out]: List of identified resource files (*.rc).
  - `${ARGN}` [in]: List of files with relative or full path, libraries, or resources to categorize.
- **Usage**: `macro_parse_arguments(<sources-var> <libs-var> <resources-var> <sources-targets-resources>)`
- **Example**:    
   ```cmake
   macro_parse_arguments(src_files lib_targets res_files my_lib src/main.cpp src/object.cpp res/resource.rc)
   ```

### macro_declare_static_library
- **Syntax**: `macro_declare_static_library(lib_name ...)`
- **Purpose**: Declares a static library with categorized sources, libraries, and resources. It splits the list of files using **[macro_parse_arguments](#macro_parse_arguments)** macro and declares new static library target. The detected targets are used to link the libraries.
- **Parameters**:
  - `lib_name` [in]: Name of the static library to declare.
  - `${ARGN}` [in]: List of source files, libraries, and resource files.
- **Usage**: `macro_declare_static_library(<lib-name> <sources-targets-resources>)`
- **Example**:
   ```cmake
   macro_declare_static_library(myStaticLib src/main.cpp src/resource.rc libSomeDependency)
   ```

### macro_declare_shared_library
- **Syntax**: `macro_declare_shared_library(lib_name ...)`
- **Purpose**: Similar to **[macro_declare_static_library](#macro_declare_static_library)**, for shared libraries. The detected targets are used to link the libraries.
- **Parameters**:
  - `lib_name` [in]: Name of the shared library to be declared.
  - `${ARGN}` [in]: List of source files, libraries, and resource files. Files can be specified with full or relative paths.
- **Usage**: `macro_declare_shared_library(<lib-name> <sources-targets-resources>)`
- **Example**:
   ```cmake
   macro_declare_shared_library(mySharedLib src/main.cpp src/resource.rc libSomeDependency)
   ```

### macro_declare_executable
- **Syntax**: `macro_declare_executable(exe_name ...)`
- **Purpose**: Declares an executable with categorized sources, libraries, and resources. It splits the list of files using **[macro_parse_arguments](#macro_parse_arguments)** macro and declares new executable target. The list of detected targets is used to link the libraries.
- **Parameters**:
  - `exe_name` [in]: Name of the executable to be declared.
  - `${ARGN}` [in]: List of source files, libraries, and resource files. Files can be specified with full or relative paths.
- **Usage**: `macro_declare_executable(<executable-name> <sources-targets-resources>)`
- **Example**:
   ```cmake
   macro_declare_executable(myApplication src/main.cpp src/resource.rc libSomeDependency)
   ```

### macro_setup_compilers_data
- **Syntax**: `macro_setup_compilers_data(compiler_path compiler_family compiler_short compiler_cxx compiler_c is_identified)`
- **Purpose**: Identifies and sets up compiler family, short names and paths, `compiler_supports` variable stores the flag if compiler is supported by AREG SDK.
- **Note**: Beside "gnu", "llvm", "msvc", the GNU compilers for CYGWIN are included as a "cygwin" family.
- **Parameters**: 
  - `compiler_path` [in]: Name or full path of the C++ compiler.
  - `compiler_family` [out]: Variable to hold identified compiler family (e.g., "gnu", "msvc", "llvm", "cygwin").
  - `compiler_short` [out]: Variable to hold short name of the compiler (e.g., "gcc", "clang").
  - `compiler_cxx` [out]: Variable to hold C++ compiler name or path (usually same as ${compiler_path}).
  - `compiler_c` [out]: Variable to hold corresponding C compiler name or path.
  - `is_identified` [out]: Variable to hold Boolean flag indicating if the compiler was successfully identified ("TRUE" or "FALSE").
- **Usage**: `macro_setup_compilers_data(<compiler> <compiler-family-var> <compiler-short-var> <CXX-compiler-var> <C-compiler-var> <identified-var>)`
- **Example**:
   ```cmakr
   macro_setup_compilers_data("${CMAKE_CXX_COMPILER}" AREG_COMPILER_FAMILY AREG_COMPILER_SHORT AREG_CXX_COMPILER AREG_C_COMPILER _is_identified)
   ```

### macro_setup_compilers_data_by_family
- **Syntax**: `macro_setup_compilers_data_by_family(compiler_family compiler_short compiler_cxx compiler_c is_identified)`
- **Purpose**: Configures compiler names based on family (e.g., gnu, msvc, llvm, cygwin). Make sure that the compiler path is included in PATH environment variable.
- **Note**: The "cygwin" family is supported for GNU compilers on the CYGWIN platform in Windows.
- **Parameters**: 
  - `compiler_family` [in]: Name of the compiler family (e.g., "gnu", "msvc", "llvm", "cygwin").
  - `compiler_short` [out]: Variable to hold the short name of the compiler (e.g., "gcc", "clang").
  - `compiler_cxx` [out]: Variable to hold the C++ compiler name or path.
  - `compiler_c` [out]: Variable to hold the corresponding C compiler name.
  - `is_identified` [out]: Variable to hold Boolean flag indicating whether the compiler was successfully identified ("TRUE" or "FALSE").
- **Usage**: `macro_setup_compilers_data_by_family(<compiler-family> <compiler-short-var> <CXX-compiler-vat> <C-compiler-vat> <identified-var>)`
- **Example**:
   ```cmake
   macro_setup_compilers_data_by_family("gnu" AREG_COMPILER_SHORT AREG_CXX_COMPILER AREG_C_COMPILER _is_identified)
   ```

---

## CMake Functions Overview

### setAppOptions
- **Syntax**: `setAppOptions(target_name library_list)`
- **Purpose**: Configures compiler and linker settings for an application target, imports and auto-linking the AREG Framework library and any additional libraries.
- **Parameters**:
  - `target_name` [in]: Name of the target application to apply options to.
  - `library_list` [in]: List of additional libraries to link with the target application.
- **Usage**: `setAppOptions(<target-name> <library-list>)`

### addExecutableEx
- **Syntax**: `addExecutableEx(target_name target_namespace source_list library_list)`
- **Purpose**: Creates an executable target, setting sources, options, imports and auto-linking the AREG Framework library and any additional libraries.
- **Parameters**:
  - `target_name` [in]: Name of the target executable to build.
  - `target_namespace` [in]: Namespace of the target, used for aliasing. Pass empty string if has no aliasing.
  - `source_list` [in]: List of source files used to build the target executable.
  - `library_list` [in]: List of libraries to link with the executable.
- **Usage**: `addExecutableEx(<target-name> <namespace-opt> <sources-list> <libraries-list>)`

### addExecutable
- **Syntax**: `addExecutable(target_name source_list)`
- **Purpose**: Creates an executable target, setting sources, options, imports and auto-linking the AREG library.
- **Parameters**:
  - `target_name` [in]: Name of the executable to build.
  - `source_list` [in]: List of source files used to build the executable.
- **Usage**: `addExecutable(<target-name> <sources-list>)`

### setStaticLibOptions
- **Syntax**: `setStaticLibOptions(target_name library_list)`
- **Purpose**: Configures compiler and linker settings for a static library, imports and auto-linking the AREG Framework library and any additional libraries.
- **Parameters**:
  - `target_name` [in]: Name of the static library to apply options to.
  - `library_list` [in]: List of libraries to link with the static library.
- **Usage **: `setStaticLibOptions(<target-name> <libraries-list>)` 

### addStaticLibEx
- **Syntax**: `addStaticLibEx(target_name target_namespace source_list library_list)`
- **Purpose**: Creates a static library, applying specified sources and options, imports and auto-linking the AREG Framework library and any additional libraries.
- **Parameters**
  - `target_name` [in]: Name of the static library to build.
  - `target_namespace` [in]: Namespace of the target, used for aliasing. Pass empty string if has no aliasing.
  - `source_list` [in]: List of source files to build the static library.
  - `library_list` [in]: List of libraries for linking.
- **Usage**: `addStaticLibEx(<target-name> <namespace-opt> <sources-list> <libraries-list>)`

### addStaticLib
- **Syntax**: `addStaticLib(target_name source_list)`
- **Purpose**: Creates a static library, applying specified sources and options, imports and auto-linking the AREG Framework library.
- **Parameters**
  - `target_name` [in]: Name of the static library to build.
  - `source_list` [in]: List of source files to build the static library.
- **Usage**: `addStaticLib(<target-name> <sources-list>)`

### addStaticLibEx_C
- **Syntax**: `addStaticLibEx_C(target_name target_namespace source_list library_list)`
- **Purpose**: Creates a static library compiled with C, setting sources, imports and auto-linking the AREG Framework library and any additional libraries.
- **Parameters**:
  - `target_name` [in]: Name of the static library to build.
  - `target_namespace` [in]: Namespace of the target, used for aliasing. Pass empty string if has no aliasing.
  - `source_list` [in]: List of C-code source files to build the static library.
  - `library_list` [in]: List of libraries to link with the static library.
- **Usage**: `addStaticLibEx_C(<target-name> <namespace-opt> <C-sources-list> <libraries-list>)`

### addStaticLib_C
- **Syntax**: `addStaticLib_C(target_name source_list)`
- **Purpose**: Creates a static library compiled with C, setting sources, imports and auto-linking the AREG Framework library.
- **Parameters**:
  - `target_name` [in]: Name of the static library to build.
  - `source_list` [in]: List of C-code source files to build the static library.
- **Usage**: `addStaticLib_C(<target-name> <C-sources-list>)`

### setSharedLibOptions
- **Syntax**: `setSharedLibOptions(item_name library_list)`
- **Purpose**: Configures settings for a shared library, imports and auto-linking the AREG Framework library and any additional libraries.
- **Parameters**:
  - `target_name` [in]: Name of the shared library to apply options to.
  - `library_list` [in]: List of libraries for linking.
- **Usage**: `setSharedLibOptions(<target-name> <libraries-list>)`

### addSharedLibEx
- **Syntax**: `addSharedLibEx(target_name target_namespace source_list library_list)`
- **Purpose**: Creates a shared library, setting sources, options, imports and auto-linking the AREG Framework library and any additional libraries.
- **Parameters**:
  - `target_name` [in]: Name of the shared library to build.
  - `target_namespace` [in]: Namespace of the target, used for aliasing. Pass empty string if has no aliasing.
  - `source_list` [in]: List of source files to build the shared library.
  - `library_list` [in]: List of libraries for linking.
- **Usage**: `addSharedLibEx(<target-name> <namespace-opt> <sources-list> <libraries-list>)`

### addSharedLib
- **Syntax**: `addSharedLib(target_name source_list)`
- **Purpose**: Creates a shared library, setting sources, options, imports and auto-linking the AREG Framework library.
- **Parameters**:
  - `target_name` [in]: Name of the shared library to build.
  - `source_list` [in]: List of source files to build shared library.
- **Usage**: `addSharedLib(<target-name> <sources-list>)`

### addServiceInterfaceEx
- **Syntax**: `addServiceInterfaceEx(lib_name source_root relative_path sub_dir interface_name)`
- **Purpose**: From given Service Interface document file (`.siml`), generates codes and includes files in a static library.
- **Parameters**
  - `lib_name` [in]: Static library name.
  - `source_root` [in]: Source root directory.
  - `relative_path` [in]: Service Interface relative file path.
  - `sub_dir` [in]: Optional sub-directory within the `relative_path`.
  - `interface_name` [in]: Service Interface name (without `.siml` extension).
- **Usage**: `addServiceInterfaceEx(<library-name> <source-root> <relative-path> <sub-dir-opt> <service-interface-name>)`
- **Example**:
   ```cmake
   addServiceInterfaceEx("fun_library" "/home/develop/project/my-fun/sources" "my/service/interfaces" "" FunInterface)
   ```

### addServiceInterface
- **Syntax**: `addServiceInterface(lib_name sub_dir interface_name)`
- **Purpose**: A wrapper for addServiceInterfaceEx, assuming the source root as `CMAKE_SOURCE_DIR` and relative path as `CMAKE_CURRENT_LIST_DIR`.
- **Parameters**:
  - `lib_name` [in]: Static library name.
  - `sub_dir` [in]: Optional sub-directory. Can by empty.
  - `interface_name` [in]: Service Interface name (without `.siml` extension).
- **Usage**: `addServiceInterface(<library-name> <sub-dir-opt> <service-interface-name>)`

### removeEmptyDirs
- **Syntax**: `removeEmptyDirs(dir_name)`
- **Purpose**: Recursively removes empty directories.
- **Parameters**:
  - `dir_name` [in]: Directory path to check and potentially remove.
- **Usage**: `removeEmptyDirs(<dir-path>)`

### printAregConfigStatus
- **Syntax**: `printAregConfigStatus(var_make_print var_prefix var_header var_footer)`
- **Purpose**: Prints a detailed status of AREG's CMake configuration, including build environment details.
- **Parameters**: 
  - `var_make_print` [in]: Boolean flag indicating whether to print the status message. If FALSE, the function exits without printing.
  - `var_prefix` [in]: A prefix added to each line of the status message (e.g., project name or custom label).
  - `var_header` [in]: A custom header message displayed at the beginning of the status report.
  - `var_footer` [in]: A custom footer message displayed at the end of the status report.
- **Usage**: `printAregConfigStatus(<flag-to-print> <prefix> <header-output> <footer-output>)`
- **Example**: 
   ```cmake
   printAregConfigStatus(TRUE "AREG"
                        "----------------------> AREG project CMake Status Report Begin <-----------------------"
                        "-----------------------> AREG project CMake Status Report End <------------------------"
                        )
   ```
