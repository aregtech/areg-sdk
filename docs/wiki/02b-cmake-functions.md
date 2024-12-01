# AREG SDK CMake Functions and Macros

The [functions.cmake](./../../conf/cmake/functions.cmake) file in AREG SDK contains custom CMake functions and macros to streamline project configuration for those built on the AREG Framework. This document provides a detailed overview of each function, its purpose, syntax, parameters, and usage examples.

## Table of Contents

1. [Introduction](#1-introduction)
2. [CMake Macro Overview](#2-cmake-macro-overview)
   - [`macro_check_fix_areg_cxx_standard`](#macro_check_fix_areg_cxx_standard)
   - [`macro_normalize_path`](#macro_normalize_path)
   - [`macro_add_service_interface`](#macro_add_service_interface)
   - [`macro_create_option`](#macro_create_option)
   - [`macro_add_source`](#macro_add_source)
   - [`macro_parse_arguments`](#macro_parse_arguments)
   - [`macro_declare_static_library`](#macro_declare_static_library)
   - [`macro_declare_shared_library`](#macro_declare_shared_library)
   - [`macro_declare_executable`](#macro_declare_executable)
   - [`macro_setup_compilers_data`](#macro_setup_compilers_data)
   - [`macro_setup_compilers_data_by_family`](#macro_setup_compilers_data_by_family)
   - [`macro_guess_processor_architecture`](#macro_guess_processor_architecture)
   - [`macro_system_bitness`](#macro_system_bitness)
   - [`macro_get_processor`](#macro_get_processor)
3. [CMake Functions Overview](#3-cmake-functions-overview)
   - [`setAppOptions`](#setappoptions)
   - [`addExecutableEx`](#addexecutableex)
   - [`addExecutable`](#addexecutable)
   - [`setStaticLibOptions`](#setstaticliboptions)
   - [`addStaticLibEx`](#addstaticlibex)
   - [`addStaticLib`](#addstaticlib)
   - [`addStaticLibEx_C`](#addstaticlibex_c)
   - [`addStaticLib_C`](#addstaticlib_c)
   - [`setSharedLibOptions`](#setsharedliboptions)
   - [`addSharedLibEx`](#addsharedlibex)
   - [`addSharedLib`](#addsharedlib)
   - [`addServiceInterfaceEx`](#addserviceinterfaceex)
   - [`addServiceInterface`](#addserviceinterface)
   - [`removeEmptyDirs`](#removeemptydirs)
   - [`printAregConfigStatus`](#printaregconfigstatus)

---

## 1. Introduction

The [functions.cmake](./../../conf/cmake/functions.cmake) file includes reusable CMake utilities that simplify repetitive tasks, making project setup cleaner and more maintainable.

> [!NOTE]
> Include [areg.cmake](./../../conf/cmake/areg.cmake) (not functions.cmake) in `CMakeLists.txt` files to ensure proper AREG Framework integration after setting `AREG_SDK_ROOT` to the `<areg-sdk>` installation directory. More details can be found in the [AREG Framework Integration](./02c-cmake-integrate.md) document in the AREG SDK Wiki.

---

## 2. CMake Macro Overview

### `macro_check_fix_areg_cxx_standard`
- **Syntax**: `macro_check_fix_areg_cxx_standard()`
- **Purpose**: Validates and sets the C++ standard compatibility. The variable `AREG_CXX_STANDARD` must be defined before calling this macro.
- **Details**: Ensures `CMAKE_CXX_STANDARD` matches `AREG_CXX_STANDARD`. Outputs a warning if incompatible.
- **Usage**: `macro_check_fix_areg_cxx_standard()`

### `macro_normalize_path`
- **Syntax**: `macro_normalize_path(normal_path os_path)`
- **Purpose**: Normalizes Windows paths to Cygwin format, if applicable.
- **Note**: This macro does not address OS-specific path separator issues.
- **Parameters**:
  - `normal_path` [out]: Name of variable to hold normalized path.
  - `os_path` [in]: The Windows-specific path to normalize.
- **Usage**: `macro_normalize_path(<out-var> <windows-path>)`

### `macro_add_service_interface`
- **Syntax**: `macro_add_service_interface(lib_name interface_doc codegen_root output_path codegen_tool)`
- **Purpose**: Generates and adds service-specific files to a static library based on a given Service Interface document (`*.siml`).
- **Parameters**:
  - `lib_name` [in]: Name of the static library.
  - `interface_doc` [in]: Full path to the Service Interface document file (`.siml`).
  - `codegen_root` [in]: Root directory for file generation.
  - `output_path` [in]: Relative path from `${codegen_root}` to the directory where generated files will be placed.
  - `codegen_tool` [in]: Full path to the code generation tool (`codegen.jar`) used to generate the necessary source and header files.
- **Usage**: `macro_add_service_interface(<library-name> <full-path-to-siml> <codegen-root> <relative-output-path> <codegen-tool-path>)`
- **Example**:
   ```cmkae
   macro_add_service_interface(funlib "/home/dev/fun/src/service/HelloWorld.siml" "/home/dev/fun/product" "generate/service" /tools/areg/codegen.jar)
   macro_add_service_interface(funlib "/home/dev/fun/src/service/WeHaveFun.siml"  "/home/dev/fun/product" "generate/service" /tools/areg/codegen.jar)
   ```

### `macro_create_option`
- **Syntax**: `macro_create_option(var_name var_value var_describe)`
- **Purpose**: Creates a boolean cache variable with a default value.
- **Parameters**:
  - `var_name` [out]: Name of the boolean variable.
  - `var_value` [in]: Default value if the variable is not yet defined.
  - `var_describe` [in]: Description of the variable for the CMake cache.
- **Usage**: `macro_create_option(<name-var> <default-value> <describe>)`
- **Example**:
   ```cmake
   macro_create_option(AREG_LOGS ON "Compile with logs")
   ```

### `macro_add_source`
- **Syntax**: `macro_add_source(result_list src_base_dir ...)`
- **Purpose**: Adds existing source files to a list based on a base directory. Checks file existence.
- **Parameters**:
  - `result_list` [in, out]: Name of variable that on output will contain the list of source files.
  - `src_base_dir` [in]: Base directory of the source files.
  - `${ARGN}` [in]: List of source files, relative to the base directory.
- **Usage**: `macro_add_source(<src-list-var> <base-dirpath> <files>)`
- **Example**:
   ```cmake
   set(aregextend_SRC)
   macro_add_source(aregextend_SRC "${AREG_FRAMEWORK}" aregextend/db/private/LogSqliteDatabase.cpp ...)
   ```

### `macro_parse_arguments`
- **Syntax**: `macro_parse_arguments(res_sources res_libs res_resources)`
- **Purpose**: Parses files and libraries into separate lists for sources, libraries, and resources. Library names must match known targets. Resource files should have `.rc` extension (Windows-specific).
- **Note**:
  - Throws an error if file does not exist.
  - List of resource files is Windows specific, it contains files with extension `.rc`.
- **Parameters**:
  - `res_sources` [out]: Name of variable that on output will contain the list of source files.
  - `res_libs` [out]: Name of variable that on output will contain the list of recognized CMake targets.
  - `res_resources` [out]: Name of variable that on output will contain the List of resource files (`*.rc`).
  - `${ARGN}` [in]: List of files, libraries, or resources to categorize.
- **Usage**: `macro_parse_arguments(<sources-var> <libs-var> <resources-var> <sources-targets-resources>)`
- **Example**:    
   ```cmake
   macro_parse_arguments(src_files lib_targets res_files my_lib src/main.cpp src/object.cpp res/resource.rc)
   ```

### `macro_declare_static_library`
- **Syntax**: `macro_declare_static_library(lib_name ...)`
- **Purpose**: Declares a static library with categorized sources, libraries, and resources using **[macro_parse_arguments](#macro_parse_arguments)**.
- **Parameters**:
  - `lib_name` [in]: Name of the static library.
  - `${ARGN}` [in]: List of source files, libraries, and resources.
- **Usage**: `macro_declare_static_library(<lib-name> <sources-targets-resources>)`
- **Example**:
   ```cmake
   macro_declare_static_library(myStaticLib src/main.cpp src/resource.rc libSomeDependency)
   ```

### `macro_declare_shared_library`
- **Syntax**: `macro_declare_shared_library(lib_name ...)`
- **Purpose**: Declares a shared library with categorized sources, libraries, and resources using **[macro_parse_arguments](#macro_parse_arguments)**.
- **Parameters**:
  - `lib_name` [in]: Name of the shared library.
  - `${ARGN}` [in]: List of source files, libraries, and resources.
- **Usage**: `macro_declare_shared_library(<lib-name> <sources-targets-resources>)`
- **Example**:
   ```cmake
   macro_declare_shared_library(mySharedLib src/main.cpp src/resource.rc libSomeDependency)
   ```

### `macro_declare_executable`
- **Syntax**: `macro_declare_executable(exe_name ...)`
- **Purpose**: Declares an executable target with categorized sources, libraries, and resources using **[macro_parse_arguments](#macro_parse_arguments)**.
- **Parameters**:
  - `exe_name` [in]: Name of the target executable.
  - `${ARGN}` [in]: List of source files, libraries, and resources.
- **Usage**: `macro_declare_executable(<executable-name> <sources-targets-resources>)`
- **Example**:
   ```cmake
   macro_declare_executable(myApplication src/main.cpp src/resource.rc libSomeDependency)
   ```

### `macro_setup_compilers_data`
- **Syntax**: `macro_setup_compilers_data(compiler_path compiler_family compiler_short compiler_cxx compiler_c is_identified)`
- **Purpose**: Identifies and configures compiler family, short names, and paths.
- **Note**: Beside "gnu", "llvm", "msvc", the GNU compilers for CYGWIN are included as a "cygwin" family.
- **Parameters**:
  - `compiler_path` [in]: Path to the C++ compiler.
  - `compiler_family` [out]: Name of variable to hold compiler family (e.g., "gnu", "msvc", "llvm", "cygwin").
  - `compiler_short` [out]: Name of variable to hold short name of the compiler (e.g., "gcc", "clang", "cl").
  - `compiler_cxx` [out]: Name of variable to hold C++ compiler name, usually same as `compiler_path`.
  - `compiler_c` [out]: Name of variable to hold C compiler path.
  - `is_identified` [out]: Name of variable to hold Boolean indicating successful identification.
- **Usage**: `macro_setup_compilers_data(<compiler> <family-var> <short-var> <CXX-compiler-var> <C-compiler-var> <identified-var>)`
- **Example**:
   ```cmakr
   macro_setup_compilers_data("${CMAKE_CXX_COMPILER}" AREG_COMPILER_FAMILY AREG_COMPILER_SHORT AREG_CXX_COMPILER AREG_C_COMPILER _is_identified)
   ```

### `macro_setup_compilers_data_by_family`
- **Syntax**: `macro_setup_compilers_data_by_family(compiler_family compiler_short compiler_cxx compiler_c is_identified)`
- **Purpose**: Configures compiler names based on family (e.g., gnu, msvc, llvm, cygwin).
- **Note**: The "cygwin" family is supported for GNU compilers on the CYGWIN platform in Windows.
- **Parameters**:
  - `compiler_family` [in]: Compiler family name (e.g., "gnu", "msvc").
  - `compiler_short` [out]: Name of variable to hold short name of the compiler (e.g., "gcc", "clang", "cl").
  - `compiler_cxx` [out]: Name of variable to hold C++ compiler path.
  - `compiler_c` [out]: Name of variable to hold C compiler path.
  - `is_identified` [out]: Name of variable to hold Boolean indicating successful identification.
- **Usage**: `macro_setup_compilers_data_by_family(<compiler-family> <short-var> <CXX-compiler-var> <C-compiler-var> <identified-var>)`
- **Example**:
   ```cmake
   macro_setup_compilers_data_by_family("gnu" AREG_COMPILER_SHORT AREG_CXX_COMPILER AREG_C_COMPILER _is_identified)
   ```

### `macro_guess_processor_architecture`
- **Syntax**: `macro_guess_processor_architecture(compiler_path target_processor target_bitness)`
- **Purpose**: If possible, detects the processor architecture and bitness by given compiler path.
- **Parameters**:
  - `compiler_path` [in] : Compiler path.
  - `target_processor` [out]: Name of variable to set the CPU architecture value.
  - `target_bitness` [out]: Name of variable to set the CPU bitness value.
- **Usage**: `macro_guess_processor_architecture(<compiler-path> <processor-var> <bitness-var>)`
- **Example**:
   ```cmake
   macro_guess_processor_architecture("arm-linux-gnueabihf-g++" cpu_architect cpu_bitness)
   ```

### `macro_system_bitness`
- **Syntax**: `macro_system_bitness(var_bitness)`
- **Purpose**: Extracts the system default bitness. Sets in variable value `32` for 32-bit, or `64` for 64-bit system.
- **Parameters**:
  - `var_bitness` [out]: Name of variable to set the system bitness.
- **Usage**: `macro_system_bitness(<var-name>)`
- **Example**:
   ```cmake
   macro_system_bitness(_sys_bitness)
   ```

### `macro_get_processor`
- **Syntax**: `macro_get_processor(processor_name var_processor var_bitness var_found)`
- **Purpose**: Identifies and validates the processor architecture based on a provided name. If a match is found in the supported processor list, it extracts:
  - The canonical architecture name.
  - The bitness (e.g., 32 or 64 bits).
- **Parameters**:
  - `processor_name` [in]: Input processor architecture name to search for.
  - `var_processor` [out]: Variable to store the canonical processor architecture name.
  - `var_bitness`   [out]: Variable to store the bitness (32/64) of the processor.
  - `var_found`     [out]: Variable to indicate if the processor is supported (TRUE/FALSE).
- **Usage**: `macro_get_processor(<processor-name> <var_processor> <var_bitness> <var_entry_found>)`
- **Example**:
   ```cmake
   macro_get_processor("arm64" AREG_PROCESSOR AREG_BITNESS _entry_found)
   ```

---

## 3. CMake Functions Overview

### `setAppOptions`
- **Syntax**: `setAppOptions(target_name library_list)`
- **Purpose**: Configures compiler and linker settings for an application target, automatically linking the AREG Framework library along with additional specified libraries.
- **Parameters**:
  - `target_name` [in]: The target application.
  - `library_list` [in]: Additional libraries to link.
- **Usage**: `setAppOptions(<target-name> <library-list>)`

### `addExecutableEx`
- **Syntax**: `addExecutableEx(target_name target_namespace source_list library_list)`
- **Purpose**: Creates an executable target with specified source files and libraries.
- **Parameters**:
  - `target_name` [in]: The name of the executable target.
  - `target_namespace` [in]: Namespace for aliasing. Can be empty.
  - `source_list` [in]: List of source files used to build the target executable.
  - `library_list` [in]: Libraries to link with the executable.
- **Usage**: `addExecutableEx(<target-name> <namespace-opt> <source-list> <library-list>)`

### `addExecutable`
- **Syntax**: `addExecutable(target_name source_list)`
- **Purpose**: Wrapper for [`addExecutableEx`](#addexecutableex), assuming there is no aliasing and list of libraries to link. Creates an executable target, setting up sources, options, and imports, and auto-linking the AREG Framework library.
- **Parameters**:
  - `target_name` [in]: Name of the executable to build.
  - `source_list` [in]: List of source files used to build the executable.
- **Usage**: `addExecutable(<target-name> <source-list>)`

### `setStaticLibOptions`
- **Syntax**: `setStaticLibOptions(target_name library_list)`
- **Purpose**: Configures compiler and linker settings for a static library, automatically linking the AREG Framework library along with any additional specified libraries.
- **Parameters**:
  - `target_name` [in]: Name of the static library to apply options to.
  - `library_list` [in]: List of libraries to link with the static library.
- **Usage**: `setStaticLibOptions(<target-name> <library-list>)`

### `addStaticLibEx`
- **Syntax**: `addStaticLibEx(target_name target_namespace source_list library_list)`
- **Purpose**: Creates a static library with specified source files and options, importing and auto-linking the AREG Framework library along with any additional libraries.
- **Parameters**:
  - `target_name` [in]: Name of the static library to build.
  - `target_namespace` [in]: Namespace for aliasing. Can be empty string if no aliasing.
  - `source_list` [in]: List of source files to build the static library.
  - `library_list` [in]: List of libraries to link.
- **Usage**: `addStaticLibEx(<target-name> <namespace-opt> <source-list> <library-list>)`

### `addStaticLib`
- **Syntax**: `addStaticLib(target_name source_list)`
- **Purpose**: Wrapper for [`addStaticLibEx`](#addstaticlibex), assuming there is no aliasing and no list of libraries to link. Creates a static library, setting sources and options, importing, and auto-linking the AREG Framework library.
- **Parameters**:
  - `target_name` [in]: Name of the static library to build.
  - `source_list` [in]: List of source files to build the static library.
- **Usage**: `addStaticLib(<target-name> <source-list>)`

### `addStaticLibEx_C`
- **Syntax**: `addStaticLibEx_C(target_name target_namespace source_list library_list)`
- **Purpose**: Creates a static library compiled with C, setting sources, importing, and auto-linking the AREG Framework library along with any additional libraries.
- **Parameters**:
  - `target_name` [in]: Name of the static library to build.
  - `target_namespace` [in]: Namespace for aliasing. Pass empty string if no aliasing.
  - `source_list` [in]: List of C-source files to build the static library.
  - `library_list` [in]: Libraries to link with the static library.
- **Usage**: `addStaticLibEx_C(<target-name> <namespace-opt> <C-source-list> <library-list>)`

### `addStaticLib_C`
- **Syntax**: `addStaticLib_C(target_name source_list)`
- **Purpose**: Wrapper for [`addStaticLibEx_C`](#addstaticlibex_c), assuming there is no aliasing and list of libraries for linking. Creates a static library compiled with C, setting sources, importing, and auto-linking the AREG Framework library.
- **Parameters**:
  - `target_name` [in]: Name of the static library to build.
  - `source_list` [in]: List of C-source files to build the static library.
- **Usage**: `addStaticLib_C(<target-name> <C-source-list>)`

### `setSharedLibOptions`
- **Syntax**: `setSharedLibOptions(target_name library_list)`
- **Purpose**: Configures settings for a shared library, automatically linking the AREG Framework library and any additional specified libraries.
- **Parameters**:
  - `target_name` [in]: Name of the shared library to apply options to.
  - `library_list` [in]: List of libraries for linking.
- **Usage**: `setSharedLibOptions(<target-name> <library-list>)`

### `addSharedLibEx`
- **Syntax**: `addSharedLibEx(target_name target_namespace source_list library_list)`
- **Purpose**: Creates a shared library with specified source files and options, importing and auto-linking the AREG Framework library along with any additional libraries.
- **Parameters**: 
  - `target_name` [in]: Name of the shared library to build.
  - `target_namespace` [in]: Namespace for aliasing. Can be empty string if no aliasing.
  - `source_list` [in]: List of source files to build the shared library.
  - `library_list` [in]: Libraries for linking.
- **Usage**: `addSharedLibEx(<target-name> <namespace-opt> <source-list> <library-list>)`

### `addSharedLib`
- **Syntax**: `addSharedLib(target_name source_list)`
- **Purpose**: Wrapper for [`addSharedLibEx`](#addsharedlibex), assuming there is no aliasing and no list for linking. Creates a shared library with specified sources, options, imports, and auto-linking the AREG Framework library.
- **Parameters**:
  - `target_name` [in]: Name of the shared library to build.
  - `source_list` [in]: List of source files to build the shared library.
- **Usage**: `addSharedLib(<target-name> <source-list>)`

### `addServiceInterfaceEx`
- **Syntax**: `addServiceInterfaceEx(lib_name source_root siml_path generate_path)`
- **Purpose**: A wrapper for `macro_add_service_interface`, facilitating the generation of code and header files for a Service Interface document (`.siml` file) within a specified static library. This function assumes the code generator tool is located at `${AREG_SDK_TOOLS}/codegen.jar`. The generated files are placed within the `${AREG_GENERATE_DIR}`, with a subdirectory structure specified by `${generate_path}`.
- **Parameters**:
  - `lib_name` [in]: The name of the static library to be created for the generated Service Interface code.
  - `source_root` [in]: The root directory containing the project's source files.
  - `siml_path` [in]: Path to the Service Interface document file (.siml), relative to the specified `${source_root}`.
  - `generate_path` [in]: Subdirectory path within `${AREG_GENERATE_DIR}` where the generated files will be stored.
- **Usage**: `addServiceInterfaceEx(<library-name> <source-root> <service-interface-relative-path> <relative-path-to-generate-codes>)`
- **Example**:
   ```cmake
   addServiceInterfaceEx(fun_library "/home/dev/project/fun/src" "fun/service/interfaces/FunService.siml" "fun/service/interfaces")
   ```

### `addServiceInterface`
- **Syntax**: `addServiceInterface(lib_name siml_path)`
- **Purpose**: A simplified wrapper for [`addServiceInterfaceEx`](#addserviceinterfaceex), automatically setting the source root to `PROJECT_SOURCE_DIR`. It assumes that the Service Interface document (.siml file) is located relative to `PROJECT_SOURCE_DIR`, and the generated source files are placed in `AREG_GENERATE_DIR`, with a directory structure mirroring the parent path of `siml_path`.
- **Parameters**:
  - `lib_name` [in]: The name of the static library to be created for the Service Interface.
  - `siml_path` [in]: The path to the Service Interface document file (.siml), relative to `${PROJECT_SOURCE_DIR}`.
- **Usage**: `addServiceInterface(<library-name> <service-interface-file-relative-path>)`
- **Example**:
  ```cmake
  addServiceInterface(fun_library fun/service/interface/FunService.siml)
  ```

### `removeEmptyDirs`
- **Syntax**: `removeEmptyDirs(dir_name)`
- **Purpose**: Recursively removes empty directories within the specified directory path.
- **Parameters**:
  - `dir_name` [in]: Directory path to check and potentially remove.
- **Usage**: `removeEmptyDirs(<dir-path>)`

### `printAregConfigStatus`
- **Syntax**: `printAregConfigStatus(var_make_print var_prefix var_header var_footer)`
- **Purpose**: Prints a detailed status of AREG's CMake configuration, including details of the build environment.
- **Parameters**: 
  - `var_make_print` [in]: Boolean flag indicating whether to print the status message (if FALSE, the function exits without printing).
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
