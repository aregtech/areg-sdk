# Areg SDK CMake Functions and Macros Reference

This document provides a comprehensive reference for CMake functions and macros defined in [functions.cmake](../../conf/cmake/functions.cmake) for projects built with Areg Framework.

> [!IMPORTANT]
> Include [areg.cmake](../../conf/cmake/areg.cmake) (not functions.cmake directly) in your `CMakeLists.txt` after setting `AREG_SDK_ROOT` to the Areg SDK installation directory. See [CMake Integration Guide](./02b-cmake-integrate.md) for details.

---

## Table of Contents

1. [Quick Reference](#quick-reference)
2. [CMake Macros](#cmake-macros)
3. [CMake Functions](#cmake-functions)

---

## Quick Reference

### When to Use Macros vs Functions

**Macros:**
- Compiler and architecture detection
- Path normalization
- Package discovery
- Source file management
- Low-level configuration

**Functions:**
- Creating executables and libraries
- Setting compilation options
- Service interface generation
- High-level project setup

### Most Commonly Used Functions

| Function | Purpose | Typical Use Case |
|----------|---------|------------------|
| [addExecutable](#addexecutable) | Create executable | Building applications |
| [addStaticLib](#addstaticlib) | Create static library | Reusable code modules |
| [addSharedLib](#addsharedlib) | Create shared library | Dynamic libraries |
| [addServiceInterface](#addserviceinterface) | Generate service code | ORPC service implementation |
| [printAregConfigStatus](#printaregconfigstatus) | Print build config | Debugging build issues |

### All Macros Quick Reference

| Macro | One-Line Purpose |
|-------|------------------|
| [macro_check_fix_areg_cxx_standard](#macro_check_fix_areg_cxx_standard) | Validate C++ standard compatibility |
| [macro_normalize_path](#macro_normalize_path) | Convert Windows paths to Cygwin format |
| [macro_get_processor](#macro_get_processor) | Detect processor architecture |
| [macro_check_module_architect](#macro_check_module_architect) | Validate binary architecture compatibility |
| [macro_find_ncurses_package](#macro_find_ncurses_package) | Locate ncurses library |
| [macro_find_gtest_package](#macro_find_gtest_package) | Locate Google Test package |
| [macro_find_sqlite_package](#macro_find_sqlite_package) | Locate SQLite3 package |
| [macro_create_option](#macro_create_option) | Create CMake boolean option |
| [macro_add_source](#macro_add_source) | Add source files to list |
| [macro_parse_arguments](#macro_parse_arguments) | Parse sources/libraries/resources |
| [macro_guess_processor_architecture](#macro_guess_processor_architecture) | Detect architecture from compiler |
| [macro_system_bitness](#macro_system_bitness) | Get system bitness (32/64) |
| [macro_default_target](#macro_default_target) | Configure compiler target |
| [macro_setup_compilers_data](#macro_setup_compilers_data) | Identify compiler family |
| [macro_setup_compilers_data_by_family](#macro_setup_compilers_data_by_family) | Configure compiler by family |
| [macro_add_service_interface](#macro_add_service_interface) | Generate service interface code |
| [macro_declare_static_library](#macro_declare_static_library) | Declare static library |
| [macro_declare_shared_library](#macro_declare_shared_library) | Declare shared library |
| [macro_declare_executable](#macro_declare_executable) | Declare executable |

### All Functions Quick Reference

| Function | One-Line Purpose |
|----------|------------------|
| [setAppOptions](#setappoptions) | Configure application compilation |
| [addExecutableEx](#addexecutableex) | Create executable with options |
| [addExecutable](#addexecutable) | Create simple executable |
| [setStaticLibOptions](#setstaticliboptions) | Configure static library |
| [addStaticLibEx](#addstaticlibex) | Create static library with options |
| [addStaticLib](#addstaticlib) | Create simple static library |
| [addStaticLibEx_C](#addstaticlibex_c) | Create C static library with options |
| [addStaticLib_C](#addstaticlib_c) | Create simple C static library |
| [setSharedLibOptions](#setsharedliboptions) | Configure shared library |
| [addSharedLibEx](#addsharedlibex) | Create shared library with options |
| [addSharedLib](#addsharedlib) | Create simple shared library |
| [addServiceInterfaceEx](#addserviceinterfaceex) | Generate service interface (extended) |
| [addServiceInterface](#addserviceinterface) | Generate service interface (simple) |
| [removeEmptyDirs](#removeemptydirs) | Remove empty directories |
| [printAregConfigStatus](#printaregconfigstatus) | Print configuration status |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## CMake Macros

### macro_check_fix_areg_cxx_standard

**Syntax:** `macro_check_fix_areg_cxx_standard()`

**Purpose:** Validates and sets C++ standard compatibility.

**Details:** Ensures `CMAKE_CXX_STANDARD` matches `AREG_CXX_STANDARD`. Outputs warning if incompatible. Variable `AREG_CXX_STANDARD` must be defined before calling this macro.

**Usage:**
```cmake
macro_check_fix_areg_cxx_standard()
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_normalize_path

**Syntax:** `macro_normalize_path(normal_path os_path)`

**Purpose:** Normalizes Windows paths to Cygwin format if applicable.

**Parameters:**
- `normal_path` [out] - Variable to hold normalized path
- `os_path` [in] - Windows-specific path to normalize

> [!NOTE]
> This macro does not address OS-specific path separator issues.

**Usage:**
```cmake
macro_normalize_path(_norm_path "c:\\path\\to\\my\\directory")
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_get_processor

**Syntax:** `macro_get_processor(processor_name var_processor var_bitness var_found)`

**Purpose:** Detects and validates processor architecture from input name.

**Parameters:**
- `processor_name` [in] - Input processor architecture name
- `var_processor` [out] - Canonical processor architecture name
- `var_bitness` [out] - Processor bitness (32 or 64)
- `var_found` [out] - Boolean indicating if processor is supported

**Usage:**
```cmake
macro_get_processor("arm64" AREG_PROCESSOR AREG_BITNESS _entry_found)
if(_entry_found)
    message(STATUS "Processor: ${AREG_PROCESSOR}, Bitness: ${AREG_BITNESS}")
endif()
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_check_module_architect

**Syntax:** `macro_check_module_architect(path_module target_name target_proc var_compatible)`

**Purpose:** Validates whether a binary module matches specified processor architecture.

**Parameters:**
- `path_module` [in] - Full path to binary (executable, shared, or static library)
- `target_name` [in] - Target name (typically same as AREG_TARGET)
- `target_proc` [in] - Target processor architecture
- `var_compatible` [out] - TRUE if binary is compatible, FALSE otherwise

**Usage:**
```cmake
find_library(_library NAMES ncurses)
if(_library)
    macro_check_module_architect("${_library}" 
                                ${AREG_TARGET} 
                                ${AREG_PROCESSOR} 
                                _is_compatible)
    if(_is_compatible)
        message(STATUS "ncurses library is compatible")
    endif()
endif()
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_find_ncurses_package

**Syntax:** `macro_find_ncurses_package(var_include var_library var_found)`

**Purpose:** Locates ncurses library and header files.

**Parameters:**
- `var_include` [out] - Path to directory containing ncurses.h
- `var_library` [out] - Full path to ncurses library
- `var_found` [out] - TRUE if found, FALSE otherwise

**Usage:**
```cmake
macro_find_ncurses_package(NCURSES_INCLUDE NCURSES_LIB NCURSES_FOUND)
if(NCURSES_FOUND)
    message(STATUS "ncurses found: ${NCURSES_LIB}")
endif()
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_find_gtest_package

**Syntax:** `macro_find_gtest_package(var_include var_library var_found)`

**Purpose:** Locates Google Test package, headers, and libraries.

**Parameters:**
- `var_include` [out] - Path to GTest header directory
- `var_library` [out] - Full path to GTest libraries
- `var_found` [out] - TRUE if found, FALSE otherwise

**Usage:**
```cmake
macro_find_gtest_package(GTEST_INCLUDE GTEST_LIB GTEST_FOUND)
if(GTEST_FOUND)
    message(STATUS "Google Test found: ${GTEST_LIB}")
endif()
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_find_sqlite_package

**Syntax:** `macro_find_sqlite_package(var_include var_library var_found)`

**Purpose:** Locates SQLite3 package, headers, and libraries.

**Parameters:**
- `var_include` [out] - Path to SQLite3 header directory
- `var_library` [out] - Full path to SQLite3 library
- `var_found` [out] - TRUE if found, FALSE otherwise

**Usage:**
```cmake
macro_find_sqlite_package(SQLITE_INCLUDE SQLITE_LIB SQLITE_FOUND)
if(SQLITE_FOUND)
    message(STATUS "SQLite3 found: ${SQLITE_LIB}")
endif()
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_create_option

**Syntax:** `macro_create_option(var_name var_value var_describe)`

**Purpose:** Creates boolean cache variable with default value.

**Parameters:**
- `var_name` [out] - Name of boolean variable
- `var_value` [in] - Default value if not yet defined
- `var_describe` [in] - Description for CMake cache

**Usage:**
```cmake
macro_create_option(ENABLE_LOGGING ON "Enable application logging")
macro_create_option(BUILD_TESTS OFF "Build unit tests")
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_add_source

**Syntax:** `macro_add_source(result_list src_base_dir ...)`

**Purpose:** Adds existing source files to a list based on base directory.

**Parameters:**
- `result_list` [in, out] - Variable containing list of source files
- `src_base_dir` [in] - Base directory of source files
- `${ARGN}` [in] - List of source files relative to base directory

**Usage:**
```cmake
set(MY_SOURCES)
macro_add_source(MY_SOURCES 
                "${PROJECT_SOURCE_DIR}/src"
                main.cpp
                util.cpp
                handler.cpp)
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_parse_arguments

**Syntax:** `macro_parse_arguments(res_sources res_libs res_resources)`

**Purpose:** Parses files and libraries into separate lists for sources, libraries, and resources.

**Parameters:**
- `res_sources` [out] - Variable for source files list
- `res_libs` [out] - Variable for recognized CMake targets
- `res_resources` [out] - Variable for resource files (`*.rc`)
- `${ARGN}` [in] - List of files, libraries, or resources

> [!NOTE]
> Throws error if file doesn't exist. Resource files (`.rc`) are Windows-specific. Library names must match known CMake targets.

**Usage:**
```cmake
macro_parse_arguments(SRC_FILES LIB_TARGETS RES_FILES 
                     mylib 
                     src/main.cpp 
                     src/object.cpp 
                     res/resource.rc)
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_guess_processor_architecture

**Syntax:** `macro_guess_processor_architecture(compiler_path target_processor target_bitness)`

**Purpose:** Detects processor architecture and bitness from compiler path.

**Parameters:**
- `compiler_path` [in] - Compiler path
- `target_processor` [out] - Variable for CPU architecture
- `target_bitness` [out] - Variable for CPU bitness

**Usage:**
```cmake
macro_guess_processor_architecture("arm-linux-gnueabihf-g++" 
                                   CPU_ARCH 
                                   CPU_BITS)
message(STATUS "Architecture: ${CPU_ARCH}, Bitness: ${CPU_BITS}")
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_system_bitness

**Syntax:** `macro_system_bitness(var_bitness)`

**Purpose:** Extracts system default bitness (32 or 64).

**Parameters:**
- `var_bitness` [out] - Variable for system bitness

**Usage:**
```cmake
macro_system_bitness(_sys_bitness)
message(STATUS "System bitness: ${_sys_bitness}")
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_default_target

**Syntax:** `macro_default_target(target_processor var_name_target)`

**Purpose:** Configures default compiler target based on processor architecture.

**Parameters:**
- `target_processor` [in] - Target processor architecture
- `var_name_target` [out] - Variable for determined compiler target

**Usage:**
```cmake
macro_default_target(AARCH64 AREG_TARGET)
message(STATUS "Compiler target: ${AREG_TARGET}")
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_setup_compilers_data

**Syntax:** `macro_setup_compilers_data(compiler_path compiler_family compiler_short compiler_cxx compiler_c var_name_target var_name_arch var_name_bitness is_identified)`

**Purpose:** Identifies and configures compiler family, short names, and paths.

**Parameters:**
- `compiler_path` [in] - Path to C++ compiler
- `compiler_family` [out] - Variable for compiler family (`gnu`, `msvc`, `llvm`, `cygwin`)
- `compiler_short` [out] - Variable for compiler short name (`gcc`, `clang`, `cl`)
- `compiler_cxx` [out] - Variable for C++ compiler name
- `compiler_c` [out] - Variable for C compiler path
- `var_name_target` [out] - Variable for compiler target value
- `var_name_arch` [in, out] - Variable for processor architecture
- `var_name_bitness` [out] - Variable for application bitness
- `is_identified` [out] - Variable for Boolean success flag

> [!NOTE]
> Compiler families include `gnu`, `llvm`, `msvc`, and `cygwin` (GNU compilers for Cygwin on Windows).

**Usage:**
```cmake
macro_setup_compilers_data("${CMAKE_CXX_COMPILER}"
                          AREG_COMPILER_FAMILY 
                          AREG_COMPILER_SHORT 
                          AREG_CXX_COMPILER 
                          AREG_C_COMPILER 
                          AREG_TARGET 
                          AREG_PROCESSOR 
                          AREG_BITNESS 
                          _compiler_supports)
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_setup_compilers_data_by_family

**Syntax:** `macro_setup_compilers_data_by_family(compiler_family compiler_short compiler_cxx compiler_c var_name_target is_identified)`

**Purpose:** Configures compiler names based on family.

**Parameters:**
- `compiler_family` [in] - Compiler family (`gnu`, `msvc`, `llvm`, `cygwin`)
- `compiler_short` [out] - Variable for compiler short name
- `compiler_cxx` [out] - Variable for C++ compiler path
- `compiler_c` [out] - Variable for C compiler path
- `var_name_target` [out] - Variable for compiler default target
- `is_identified` [out] - Variable for Boolean success flag

**Usage:**
```cmake
macro_setup_compilers_data_by_family("gnu" 
                                    AREG_COMPILER_SHORT 
                                    AREG_CXX_COMPILER 
                                    AREG_C_COMPILER 
                                    AREG_TARGET 
                                    _is_identified)
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_add_service_interface

**Syntax:** `macro_add_service_interface(lib_name interface_doc codegen_root output_path codegen_tool)`

**Purpose:** Generates and adds service-specific files to static library from Service Interface document (`.siml`).

**Parameters:**
- `lib_name` [in] - Name of static library
- `interface_doc` [in] - Full path to Service Interface document (`.siml`)
- `codegen_root` [in] - Root directory for file generation
- `output_path` [in] - Relative path from `${codegen_root}` for generated files
- `codegen_tool` [in] - Full path to code generator (`codegen.jar`)

**Usage:**
```cmake
macro_add_service_interface(funlib 
                           "/home/dev/fun/src/service/HelloWorld.siml" 
                           "/home/dev/fun/product" 
                           "generate/service" 
                           "/tools/areg/codegen.jar")
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_declare_static_library

**Syntax:** `macro_declare_static_library(lib_name ...)`

**Purpose:** Declares static library with categorized sources, libraries, and resources.

**Parameters:**
- `lib_name` [in] - Name of static library
- `${ARGN}` [in] - List of source files, libraries, and resources

**Usage:**
```cmake
macro_declare_static_library(myStaticLib 
                            src/main.cpp 
                            src/util.cpp 
                            src/resource.rc 
                            someDependency)
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_declare_shared_library

**Syntax:** `macro_declare_shared_library(lib_name ...)`

**Purpose:** Declares shared library with categorized sources, libraries, and resources.

**Parameters:**
- `lib_name` [in] - Name of shared library
- `${ARGN}` [in] - List of source files, libraries, and resources

**Usage:**
```cmake
macro_declare_shared_library(mySharedLib 
                            src/main.cpp 
                            src/util.cpp 
                            src/resource.rc 
                            someDependency)
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

### macro_declare_executable

**Syntax:** `macro_declare_executable(exe_name ...)`

**Purpose:** Declares executable target with categorized sources, libraries, and resources.

**Parameters:**
- `exe_name` [in] - Name of executable
- `${ARGN}` [in] - List of source files, libraries, and resources

**Usage:**
```cmake
macro_declare_executable(myApplication 
                        src/main.cpp 
                        src/handler.cpp 
                        src/resource.rc 
                        someDependency)
```

<div align="right"><kbd><a href="#cmake-macros">↑ Back to macros ↑</a></kbd></div>

---

## CMake Functions

### setAppOptions

**Syntax:** `setAppOptions(target_name library_list)`

**Purpose:** Configures compiler and linker settings for application target, automatically linking Areg Framework library.

**Parameters:**
- `target_name` [in] - Target application
- `library_list` [in] - Additional libraries to link

**Usage:**
```cmake
setAppOptions(myapp "pthread;dl")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addExecutableEx

**Syntax:** `addExecutableEx(target_name target_namespace source_list library_list)`

**Purpose:** Creates executable target with specified source files and libraries.

**Parameters:**
- `target_name` [in] - Name of executable
- `target_namespace` [in] - Namespace for aliasing (can be empty)
- `source_list` [in] - List of source files
- `library_list` [in] - Libraries to link

**Usage:**
```cmake
addExecutableEx(myapp "myproject" "${SOURCES}" "areg::areg;pthread")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addExecutable

**Syntax:** `addExecutable(target_name source_list)`

**Purpose:** Wrapper for `addExecutableEx` with no aliasing and automatic Areg Framework linking.

**Parameters:**
- `target_name` [in] - Name of executable
- `source_list` [in] - List of source files

**Usage:**
```cmake
set(SOURCES main.cpp util.cpp)
addExecutable(myapp "${SOURCES}")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### setStaticLibOptions

**Syntax:** `setStaticLibOptions(target_name library_list)`

**Purpose:** Configures compiler and linker settings for static library, automatically linking Areg Framework.

**Parameters:**
- `target_name` [in] - Name of static library
- `library_list` [in] - List of libraries to link

**Usage:**
```cmake
setStaticLibOptions(mylib "pthread")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addStaticLibEx

**Syntax:** `addStaticLibEx(target_name target_namespace source_list library_list)`

**Purpose:** Creates static library with specified source files and options.

**Parameters:**
- `target_name` [in] - Name of static library
- `target_namespace` [in] - Namespace for aliasing (can be empty)
- `source_list` [in] - List of source files
- `library_list` [in] - Libraries to link

**Usage:**
```cmake
addStaticLibEx(mylib "myproject" "${SOURCES}" "areg::areg")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addStaticLib

**Syntax:** `addStaticLib(target_name source_list)`

**Purpose:** Wrapper for `addStaticLibEx` with no aliasing and automatic Areg Framework linking.

**Parameters:**
- `target_name` [in] - Name of static library
- `source_list` [in] - List of source files

**Usage:**
```cmake
set(LIB_SOURCES service.cpp handler.cpp)
addStaticLib(mylib "${LIB_SOURCES}")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addStaticLibEx_C

**Syntax:** `addStaticLibEx_C(target_name target_namespace source_list library_list)`

**Purpose:** Creates static library compiled with C compiler.

**Parameters:**
- `target_name` [in] - Name of static library
- `target_namespace` [in] - Namespace for aliasing (can be empty)
- `source_list` [in] - List of C source files
- `library_list` [in] - Libraries to link

**Usage:**
```cmake
addStaticLibEx_C(mylib "myproject" "${C_SOURCES}" "m")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addStaticLib_C

**Syntax:** `addStaticLib_C(target_name source_list)`

**Purpose:** Wrapper for `addStaticLibEx_C` with no aliasing and automatic Areg Framework linking.

**Parameters:**
- `target_name` [in] - Name of static library
- `source_list` [in] - List of C source files

**Usage:**
```cmake
set(C_SOURCES utils.c helpers.c)
addStaticLib_C(mylib "${C_SOURCES}")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### setSharedLibOptions

**Syntax:** `setSharedLibOptions(target_name library_list)`

**Purpose:** Configures settings for shared library, automatically linking Areg Framework.

**Parameters:**
- `target_name` [in] - Name of shared library
- `library_list` [in] - List of libraries to link

**Usage:**
```cmake
setSharedLibOptions(mysharedlib "pthread;dl")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addSharedLibEx

**Syntax:** `addSharedLibEx(target_name target_namespace source_list library_list)`

**Purpose:** Creates shared library with specified source files and options.

**Parameters:**
- `target_name` [in] - Name of shared library
- `target_namespace` [in] - Namespace for aliasing (can be empty)
- `source_list` [in] - List of source files
- `library_list` [in] - Libraries to link

**Usage:**
```cmake
addSharedLibEx(mysharedlib "myproject" "${SOURCES}" "areg::areg;pthread")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addSharedLib

**Syntax:** `addSharedLib(target_name source_list)`

**Purpose:** Wrapper for `addSharedLibEx` with no aliasing and automatic Areg Framework linking.

**Parameters:**
- `target_name` [in] - Name of shared library
- `source_list` [in] - List of source files

**Usage:**
```cmake
set(LIB_SOURCES service.cpp handler.cpp)
addSharedLib(mysharedlib "${LIB_SOURCES}")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addServiceInterfaceEx

**Syntax:** `addServiceInterfaceEx(lib_name source_root siml_path generate_path)`

**Purpose:** Wrapper for `macro_add_service_interface`, generates code for Service Interface document (`.siml`) within static library.

**Parameters:**
- `lib_name` [in] - Name of static library
- `source_root` [in] - Root directory containing source files
- `siml_path` [in] - Path to `.siml` file relative to `${source_root}`
- `generate_path` [in] - Subdirectory within `${AREG_GENERATE_DIR}` for generated files

> [!NOTE]
> Assumes code generator is at `${AREG_SDK_TOOLS}/codegen.jar`.

**Usage:**
```cmake
addServiceInterfaceEx(fun_library 
                     "/home/dev/project/fun/src" 
                     "fun/service/interfaces/FunService.siml" 
                     "fun/service/interfaces")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### addServiceInterface

**Syntax:** `addServiceInterface(lib_name siml_path)`

**Purpose:** Simplified wrapper for `addServiceInterfaceEx`, uses `PROJECT_SOURCE_DIR` as source root.

**Parameters:**
- `lib_name` [in] - Name of static library
- `siml_path` [in] - Path to `.siml` file relative to `${PROJECT_SOURCE_DIR}`

**Usage:**
```cmake
addServiceInterface(fun_library "fun/service/interface/FunService.siml")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### removeEmptyDirs

**Syntax:** `removeEmptyDirs(dir_name)`

**Purpose:** Recursively removes empty directories within specified path.

**Parameters:**
- `dir_name` [in] - Directory path to check and remove

**Usage:**
```cmake
removeEmptyDirs("${CMAKE_BINARY_DIR}/temp")
```

<div align="right"><kbd><a href="#cmake-functions">↑ Back to functions ↑</a></kbd></div>

---

### printAregConfigStatus

**Syntax:** `printAregConfigStatus(var_make_print var_prefix var_header var_footer)`

**Purpose:** Prints detailed status of Areg's CMake configuration.

**Parameters:**
- `var_make_print` [in] - Boolean flag to print (FALSE skips printing)
- `var_prefix` [in] - Prefix for each line
- `var_header` [in] - Custom header message
- `var_footer` [in] - Custom footer message

**Usage:**
```cmake
printAregConfigStatus(TRUE 
                     "AREG"
                     "--- AREG Configuration Begin ---"
                     "--- AREG Configuration End ---")
```

**Example output:**
```
--- AREG Configuration Begin ---
AREG: >>> CMAKE_SOURCE_DIR    = '/home/user/project', build type 'Debug'
AREG: >>> Build Environment ..: System 'Linux', 64-bit 'x86_64' platform
AREG: >>> Used CXX-Compiler ..: '/usr/bin/clang++'
AREG: >>> AREG SDK Root ......: '/opt/areg-sdk'
AREG: >>> Binary Output Dir ..: '/home/user/project/build/bin'
AREG: >>> Build Modules ......: areg = 'shared', areglogger = 'shared'
--- AREG Configuration End ---
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Integration Guides:**
- [CMake Integration](./02b-cmake-integrate.md) - Using Areg Framework in your projects
- [CMake Configuration](./02d-cmake-config.md) - Configuration options
- [CMake Build Guide](./02b-cmake-build.md) - Building Areg SDK

**Configuration Files:**
- [functions.cmake](../../conf/cmake/functions.cmake) - Source file
- [areg.cmake](../../conf/cmake/areg.cmake) - Main include file
- [setup.cmake](../../conf/cmake/setup.cmake) - Setup utilities

**Examples:**
- [Areg SDK Examples](../../examples/README.md) - Sample applications
- [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) - Integration examples

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
