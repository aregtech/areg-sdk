# AREG SDK CMake Functions and Macro

The [functions.cmake](./../../conf/cmake/functions.cmake) file in the AREG SDK repository contains a set of custom CMake functions and macros that streamline project configuration tasks for building projects based on AREG Framework. Below is a detailed breakdown of each function and macro, including their purposes and usage.

---

## Introduction

This file offers reusable CMake functions and macros, designed to simplify repetitive tasks in CMake configuration. By including this file, developers can leverage pre-built utilities for complex configuration, ensuring a cleaner and more maintainable CMake setup.


The [functions.cmake](./../../conf/cmake/functions.cmake) file provides reusable CMake utilities to simplify and improve the configuration of projects, which use AREG Framework. The developers can use functions and macro declared in `functions.cmake` by including [setup.cmake](./../../conf/cmake/setup.cmake) file in their project in their project to ensure consistent and efficient project setup.

> [!IMPORTANT]
> It is recommended to include the file `setup.cmake` in the `CMakeLists.txt` files instead of including `functions.cmake` to reuse functions and marco, and integrate AREG Framework properly. Before including the file, set the `AREG_SDK_ROOT` CMake variable indicating the location of `<areg-sdk>` installation directory. More details about integration are described in the [AREG Framework Integration](./cmake-integrate.md) document of AREG SDK Wiki.

---

## AREG Framework CMake Macro

Following are the CMake macro with description that can be used by developers:

### macro_check_fix_areg_cxx_standard

- **Macro**: macro_check_fix_areg_cxx_standard
- **Description**: Checks and sets the C++ standard for the project. The variable 'AREG_CXX_STANDARD' must be defined before calling this macro. If 'CMAKE_CXX_STANDARD' is not set, it will be assigned the value of 'AREG_CXX_STANDARD'. If 'CMAKE_CXX_STANDARD' is lower than 'AREG_CXX_STANDARD', a warning is displayed.
- **Macro Usage**: macro_check_fix_areg_cxx_standard()

### macro_normalize_path
- **Macro**: macro_normalize_path(normal_path os_path)
- **Description**: Converts Windows-specific paths to Cygwin format if running under Cygwin. Otherwise, the path remains unchanged.
- **Note**: This macro does not address OS-specific path separator issues.
- **Parameters**:
  - *normal_path* [out]: The normalized path.
  - *os_path* [in]: The OS-specific path to normalize.
- **Macro Usage**: *macro_normalize_path(<result_variable> <OS specific path>)*

### macro_add_service_interface

- **Macro**: macro_add_service_interface(lib_name interface_doc interface_name codegen_root output_path codegen_tool)
- **Description**: This function invokes the service interface code generator to produce source files and either adds them to a new static library (if it doesn't exist) or includes them in an existing one. It accepts the static library name, full paths to Service Interface file, Service Interface name and paths for generated files. The service interface name must match the file name (without the .siml extension).
- **Parameters**:
  - *lib_name* [in]: Name of the static library.
  - *interface_doc* [in]:Full path to the Service Interface document file. 
  - *interface_name* [in]: Name of the Service Interface (without '.siml').
  - *codegen_root* [in]: Root directory for generating files.
  - *output_path* [in]: Relative path for generated files.
  - *codegen_tool* [in]: Full path to the code generator tool.
- **Macro Usage**: *macro_add_service_interface(<library name> <documend dir><interface name>.siml <interface name> <codegen root> <sub-path> <codegen tool>)*
- **Example: For a project 'fun_library' with 'HelloWorld.siml' and 'WeHaveFun.siml' interfaces calling macro_add_service_interface("fun_library" ... HelloWorld ...) generates source files for HelloWorld and includes them in the static library. A subsequent call for WeHaveFun adds it to the same library.
   ```cmkae
   macro_add_service_interface("fun_library" "~/project/my-fun/sources/service/interfaces/FunInterface.siml" FunInterface "~/project/my-fun/" "generate/service/interfaces" /tools/areg/codegen.jar)
   ```

### macro_find_package

- **Macro**: macro_find_package(package_name package_found package_includes package_libraries)
- **Description**: Searches for a package and sets output variables to indicate the package's include directories and libraries if found.
- **Parameters**:
  - *package_name* [in]: The package name to search.
  - *package_found* [out]: Pass the name of variable. On output the variable is TRUE if the package is found.
  - *package_includes* [out]: Pass the name of variable. On output the variable indicates the package's include directories (if any).
  - *package_libraries* [out]: Pass the name of variable. On output the variable indicates the package's libraries (if any).
- **Macro Usage**: *macro_find_package(<package> <found flag> <includes> <libraries>)*
- **Example**:
   ```cmake
   macro_find_package(SQLite3 SQLITE_FOUND SQLITE_INCLUDE SQLITE_LIB)
   ```

### macro_create_option

- **Macro**: macro_create_option(var_name var_value var_description)
- **Description**: Creates or updates a boolean cache variable in CMake, ensuring it is defined and set correctly.
- **Parameters**: 
  - *var_name* [in, out]: The name of the boolean variable.
  - *var_value* [in]: The default value if the variable is not yet defined.
  - *var_describe* [in]: A brief description of the variable for CMake cache.
- **Macro Usage**: *macro_create_option(<var_name> <default_value> <description>)*
- **Example**:
   ```cmake
   macro_create_option(AREG_LOGS ON "Compile with logs")
   ```

### macro_add_source

- **Macro**: macro_add_source(result_list src_base_dir ...)
- **Description**: Adds source files to a list, checking if the files exist relative to the base directory.
- **Parameters**:
  - *result_list* [in, out]: The output list of source files.
  - *src_base_dir* [in]: The base directory for the source files.
  - *ARGN* [in]: The list of source files (relative to the base directory).
- **Macro Usage**: macro_add_source(<src list variable> <base directory path> <list of source files with relative path>)
- **Example**:
   ```cmake
   set(aregextend_SRC)
   macro_add_source(aregextend_SRC "${AREG_FRAMEWORK}" aregextend/db/private/LogSqliteDatabase.cpp ...)
   ```

### macro_parse_arguments

- **Macro**: macro_parse_arguments(res_sources res_libs res_resources ...)
- **Description**: This macro processes a list of input arguments that can include:
          1. **CMake targets** (predefined libraries or targets).
          2. **Source files**: Files provided with either absolute paths or paths relative to the current directory.
          3. **Resource files** (*.rc): Windows-specific resource files.    
       The macro categorizes the input arguments into three separate lists:
          1. **Libraries list**: Contains any known CMake targets.
          2. **Source files list**: Includes valid source files (.cpp, .c) from either the provided absolute paths or relative paths.
          3. **Resource files list**: Filters out .rc files, specifically for Windows, and stores them in a separate list.
       **Behavior**: 
          - If a file does not exist, either as a full path or relative to the current directory, the macro throws a fatal error.
          - On Windows, it specifically identifies and appends resource files (*.rc) to the resources list.
- **Parameters**:
  - *res_sources* [out]: A list containing the parsed source files.
  - *res_libs* [out]: Output: A list containing the recognized CMake targets (libraries).
  - *res_resources* [out]: A list containing the identified resource files (*.rc).
  - *${ARGN}* [in]: A list of files, libraries, or resources to be categorized.
- **Macro Usage**: *macro_parse_arguments(<var sources> <var libs> <var resources> my_target my/app/main.cpp my/lib/object.cpp my/resource/resource.rc)*
- **Example**:    
   ```cmake
   macro_parse_arguments(src_files lib_targets res_files my_lib src/main.cpp src/object.cpp res/resource.rc)
   ```

### macro_declare_static_library

- **Macro**: macro_declare_static_library(lib_name ...)
- **Description**: This macro declares a static library by processing the provided list of arguments and categorizing them into three main groups:
            1. **Source Files**: These are the .cpp or .c files used to build the library. The macro supports both absolute paths and paths relative to the current directory.
            2. **Libraries**: These are existing CMake targets (predefined libraries) that the static library depends on.
            3. **Resource Files**: These are .rc files (Windows resource files). On Windows systems, the macro ensures they are processed using the appropriate RC language settings.
       The macro declares a static library target using the collected source files and linked libraries. It also handles resource file configuration on Windows platforms.
- **Notes**: 
   - Throws a fatal error if no source files are provided.
   - On Windows, resource files (*.rc) are set to use the RC language automatically.
- **Parameters**:
  - **lib_name** [in]: The name of the static library to be declared.
  - **${ARGN}** [in]: The list of source files, libraries, and resource files. The files can be specified with full or relative paths.
- **Macro Usage**: *macro_declare_static_library(<lib_name> <list_of_sources_libraries_and_resources>)*
- **Example**:
   ```cmake
   macro_declare_static_library(myStaticLib src/main.cpp src/resource.rc libSomeDependency)
   ```

### macro_declare_shared_library

- **Macro**: macro_declare_shared_library(lib_name ...)
- **Description**: This macro declares a shared library by processing the provided list of arguments and categorizing them into three main groups:
            1. **Source Files**: These are the .cpp or .c files used to build the library. The macro supports both absolute paths and paths relative to the current directory.
            2. **Libraries**: These are existing CMake targets (predefined libraries) that the shared library depends on.
            3. **Resource Files**: These are .rc files (Windows resource files). On Windows systems, the macro ensures they are processed using the appropriate RC language settings.
       The macro declares a shared library target using the collected source files and linked libraries. It also handles resource file configuration on Windows platforms.
- **Notes**: 
   - Throws a fatal error if no source files are provided.
   - On Windows, resource files (*.rc) are set to use the RC language automatically.
- **Parameters**:
  - *lib_name* [in]: The name of the shared library to be declared.
  - *${ARGN}* [in]: The list of source files, libraries, and resource files. The files can be specified with full or relative paths.
- **Macro Usage**: *macro_declare_shared_library(<lib_name> <list_of_sources_libraries_and_resources>)*
- **Example**:
   ```cmake
   macro_declare_shared_library(mySharedLib src/main.cpp src/resource.rc libSomeDependency)
   ```

### macro_declare_executable

- **Macro**: macro_declare_executable(exe_name ...)
- **Description**: This macro declares an executable by processing the provided list of arguments and categorizing them into three main groups:
            1. **Source Files**: These are the .cpp or .c files used to build the library. The macro supports both absolute paths and paths relative to the current directory.
            2. **Libraries**: These are existing CMake targets (predefined libraries) that the executable depends on.
            3. **Resource Files**: These are .rc files (Windows resource files). On Windows systems, the macro ensures they are processed using the appropriate RC language settings.
       The macro declares a executable target using the collected source files and linked libraries. It also handles resource file configuration on Windows platforms.
- **Notes**: 
   - Throws a fatal error if no source files are provided.
   - On Windows, resource files (*.rc) are set to use the RC language automatically.
- **Parameters**:
  - *exe_name* [in]: The name of the executable to be declared.
  - *${ARGN}* [in]: The list of source files, libraries, and resource files. The files can be specified with full or relative paths.
- **Macro Usage**: *macro_declare_executable(<lib_name> <list_of_sources_libraries_and_resources>)*
- **Example**:
   ```cmake
   macro_declare_executable(myApplication src/main.cpp src/resource.rc libSomeDependency)
   ```

### macro_setup_compilers_data

- **Macro**: macro_setup_compilers_data(compiler_path compiler_family compiler_short compiler_cxx compiler_c compiler_found)
- **Description**: This macro identifies the compiler family (e.g., GNU, Clang, MSVC), extracts a short name for the compiler, and attempts to guess the corresponding C compiler based on the provided C++ compiler path or name. It handles cases where only the compiler name is provided (assuming it is available in the system's PATH). If a match is found, it sets the output variables for the compiler family, short name, C++ compiler path, and C compiler path, and marks the process as successful. Otherwise, the output flag indicates failure.
- **Note**: Beside "gnu", "llvm", "msvc", the compilers for CYGWIN are included in the family "cygwin" to identify that the application is compiled in Windows under CYGWIN platform with GNU compilers.
- **Parameters**: 
  - *compiler_path* [in]: The name or full path of the C++ compiler executable.
  - *compiler_family* [out]: Variable to hold the identified compiler family (e.g., "gnu", "msvc").
  - *compiler_short* [out]: Variable to hold the short name of the compiler (e.g., "gcc", "clang").
  - *compiler_cxx* [out]: Variable to hold the C++ compiler path (usually same as ${compiler_path}).
  - *compiler_c* [out]: Variable to hold the corresponding C compiler name or path.
  - *compiler_supports* [out]: Boolean flag indicating if the compiler was successfully identified ("TRUE" or "FALSE").
- **Macro Usage**: macro_setup_compilers_data(<compiler with path> <var compiler family> <var compiler short> <var CXX compiler> <var C compiler> <flag compiler supports>)
- **Example**:
   ```cmake
   macro_setup_compilers_data("${CMAKE_CXX_COMPILER}" AREG_COMPILER_FAMILY AREG_COMPILER_SHORT AREG_CXX_COMPILER AREG_C_COMPILER _compiler_found)
   ```

### macro_setup_compilers_data_by_family

- **Macro**: macro_setup_compilers_data_by_family(compiler_family compiler_short compiler_cxx compiler_c compiler_supports)
- **Description**: This macro identifies the short name of the C++ and C compilers based on the provided compiler family (e.g., "gnu", "msvc", "llvm", "cygwin"). It assumes that the compiler executables are available in the system's PATH. Once a match is found for the compiler family, it sets the output variables for the short name, C++ compiler name, and C compiler name, and marks the process as successful. If no match is found, it sets the output flag to "FALSE".
- **Note**: The "cygwin" family is supported for GNU compilers on the CYGWIN platform in Windows.
- **Parameters**: 
  - *compiler_family* [in]: The name of the compiler family (e.g., "gnu", "msvc").
  - *compiler_short* [out]: Variable to hold the short name of the compiler (e.g., "gcc", "clang").
  - *compiler_cxx* [out]: Variable to hold the C++ compiler name.
  - *compiler_c* [out]: Variable to hold the corresponding C compiler name.
  - *compiler_supports* [out]: Boolean flag indicating whether the compiler was successfully identified ("TRUE" or "FALSE").
- **Macro Usage**: macro_setup_compilers_data_by_family(<compiler family name> <var compiler short> <var CXX compiler> <var C compiler> <flag compiler supports>)
- **Example**:
   ```cmake
   macro_setup_compilers_data_by_family("gnu" AREG_COMPILER_SHORT AREG_CXX_COMPILER AREG_C_COMPILER _compiler_supports)
   ```

---

## AREG Framework CMake Functions

Following are the CMake functions with description that can be used by developers:

### setAppOptions

- **Function**: *setAppOptions(target_name, library_list)*
- **Description**: Configures the compiler and linker options for target applications. Automatically links the AREG library, along with any additional libraries specified.
- **Parameters**:
  - *target_name* [in]: The name of the target application to apply options to.
  - *library_list* [in]: The list of additional libraries to link with the target application.
- **Function Usage**: *setAppOptions(<name of target> <list of libraries>)*

### addExecutableEx

- **Function**: *addExecutableEx(target_name target_namespace source_list library_list)*
- **Description**: Creates an target executable, sets its source files, applies necessary options, and links it with the provided list of libraries. The AREG library is automatically linked, so no need to specify it.
- **Parameters**:
  - *target_name* [in]: The name of the target executable to build.
  - *target_namespace* [in]: The namespace of the target, used for aliasing. Pass empty string if has no namespace.
  - *source_list* [in]: The list of source files used to build the target executable.
  - *library_list* [in]: The list of libraries to link with the executable.
- **Function Usage**: *addExecutableEx(<name of target executable> <namespace (optional)> <list of sources> <list of libraries>)*

### addExecutable

- **Function**: *addExecutable(target_name source_list)*
- **Description**: Creates an executable, sets its source files, applies necessary options. The AREG library is automatically linked, so no need to specify it.
- **Parameters**:
  - *target_name* [in]: The name of the executable to build.
  - *source_list* [in]: The list of source files used to build the executable.
- **Function Usage**: *addExecutable(<name of executable> <list of sources>)*

### setStaticLibOptions

- **Function**: setStaticLibOptions(target_name library_list)
- **Description**: Configures the compiler and linker options for a static library. Automatically links the AREG library and any other specified libraries.
- **Parameters**:
  - *target_name* [in]: The name of the static library to apply options to.
  - *library_list* [in]: The list of libraries to link with the static library.
- **Function Usage **: *setStaticLibOptions(<name of static library> <list of libraries>)* 

### addStaticLibEx

- **Function**: addStaticLibEx(target_name target_namespace source_list library_list)
- **Description**: Creates a static library, sets its source files, applies necessary options, and links it with the provided list of libraries. The AREG library is automatically linked, so no need to specify it.
- **Parameters**
  - *target_name* [in]: The name of the static library to build.
  - *target_namespace* [in]: (**optional**) The namespace of the static library. Pass empty string if has no namespace.
  - *source_list* [in]: The list of source files used to build the static library.
  - *library_list* [in]: The list of libraries to link with the static library.
- **Function Usage**: *addStaticLibEx(<name of static library> <namespace (optional)> <list of sources> <list of libraries>)*

### addStaticLib

- **Function**: addStaticLib(target_name source_list)
- **Description**: Creates a static library, sets its source files, applies necessary options. The AREG library is automatically linked, so no need to specify it.
- **Parameters**
  - *target_name* [in]: The name of the static library to build.
  - *source_list* [in]: The list of source files used to build the static library.
- **Function Usage**: *addStaticLib(<name of static library> <list of sources>)*

### addStaticLibEx_C

- **Function**: addStaticLibEx_C(target_name target_namespace source_list library_list)
- **Description**: Creates a static library compiled with C-compiler, sets its source files, applies necessary options, and links it with the provided list of libraries. The AREG library is automatically linked, so no need to specify it.
- **Parameters**:
  - *target_name* [in]: The name of the static library to build.
  - *target_namespace* [in]: (**optional**) The namespace of the static library. Pass empty string if has no namespace.
  - *source_list* [in]: The list of C-code source files used to build the static library.
  - *library_list* [in]: The list of libraries to link with the static library.
- **Function Usage**: *addStaticLibEx_C(<name of static library> <namespace (optional)> <list of C-code sources> <list of libraries>)*

### addStaticLib_C

- **Function**: addStaticLib_C(target_name source_list)
- **Description**: Creates a static library compiled with C-compiler, sets its source files, applies necessary options. The AREG library is automatically linked, so no need to specify it.
- **Parameters**:
  - *target_name* [in]: The name of the static library to build.
  - *source_list* [in]: The list of C-code source files used to build the static library.
- **Function Usage**: *addStaticLib_C(<name of static library> <list of C-code sources>)*

### setSharedLibOptions

- **Function**: setSharedLibOptions(item_name library_list)
- **Description**: Configures the compiler and linker options for a shared library. Automatically links the AREG library and any other specified libraries.
- **Parameters**:
  - target_name [in]: The name of the shared library to apply options to.
  - *library_list* [in]: The list of libraries to link with the shared library.
- **Function Usage**: *setSharedLibOptions(<name of shared library> <list of libraries>)* 

### addSharedLibEx

- **Function**: addSharedLibEx(target_name target_namespace source_list library_list)
- **Description**: Creates a shared library, sets its source files, applies necessary options, and links it with the provided list of libraries. The AREG library is automatically linked, so no need to specify it.
- **Parameters**:
  - *target_name* [in]: The name of the shared library to build.
  - *target_namespace* [in]: The namespace of the shared library (optional for aliasing).
  - *source_list* [in]: The list of source files used to build the shared library.
  - *library_list* [in]: The list of libraries to link with the shared library.
- **Function Usage**: *addSharedLibEx(<name of shared library> <namespace (optional)> <list of sources> <list of libraries>)*

### addSharedLib
- **Function**: addSharedLib(target_name source_list)
- **Description**: Creates a shared library, sets its source files, applies necessary options. The AREG library is automatically linked, so no need to specify it.
- **Parameters**:
  - *target_name* [in]: The name of the shared library to build.
  - *source_list* [in]: The list of source files used to build the shared library.
- **Function Usage**: *addSharedLib(<name of shared library> <list of sources>)*

### addServiceInterfaceEx

- **Function**: addServiceInterfaceEx(lib_name source_root relative_path sub_dir interface_name)
- **Description**: Generates code for a service interface using a code generator and includes the generated files in a static library.
- **Parameters**
  - *lib_name* [in]: The static library name.
  - *source_root* [in]: The root directory of the source files.
  - *relative_path* [in]: The relative path to the Service Interface files.
  - *sub_dir* [in]: Optional sub-directory within relative_path (can be empty).
  - *interface_name* [in]: The name of the Service Interface (without the '.siml' extension).
- **Function Usage**: *addServiceInterfaceEx(<static library> <source root> <relative path> <sub-directory> <interface name>)*
- **Example**:
   ```cmake
   addServiceInterfaceEx("fun_library" "/home/develop/project/my-fun/sources" "my/service/interfaces" "" FunInterface)
   ```

### addServiceInterface

- **Function**: addServiceInterface(lib_name sub_dir interface_name)
- **Description**: Wrapper for addServiceInterfaceEx, with the source root assumed to be `CMAKE_SOURCE_DIR` and the relative path `CMAKE_CURRENT_LIST_DIR`.
- **Parameters**:
  - *lib_name* [in]: The name of the static library.
  - *sub_dir* [in]: The sub-directory of the service interface files.
  - *interface_name* [in]: The name of the Service Interface (without the '.siml' extension).
- **Function Usage**: *addServiceInterface(<static library> <sub-directory> <Service Interface name>)*

### removeEmptyDirs

- **Function**: removeEmptyDirs(dir_name)
- **Description**: Recursively removes empty directories.
- **Parameters**:
  - dir_name [in]: The directory path to check and potentially remove.
- **Function Usage**: *removeEmptyDirs(<directory path>)*

### printAregConfigStatus

- **Function**: printAregConfigStatus(var_make_print var_prefix var_header var_footer)
- **Description**: This function prints a detailed status report for the AREG project's CMake configuration. It provides an overview of the build environment, compiler, output directories, and relevant configuration options such as installed packages and libraries.
- **Parameters**: 
  - *var_make_print* [in]: Boolean flag indicating whether to print the status message. If FALSE, the function exits without printing.
  - *var_prefix* [in]: A prefix added to each line of the status message (e.g., project name or custom label).
  - *var_header* [in]: A custom header message displayed at the beginning of the status report.
  - *var_footer* [in]: A custom footer message displayed at the end of the status report.
- **Function Usage**: printAregConfigStatus(<flag to print> <prefix name> <header to output> <footer to output>)
- **Example**: 
   ```cmake
   printAregConfigStatus(TRUE "AREG"
                        "----------------------> AREG project CMake Status Report Begin <-----------------------"
                        "-----------------------> AREG project CMake Status Report End <------------------------"
                        )
   ```
