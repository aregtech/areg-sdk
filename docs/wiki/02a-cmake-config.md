# CMake Configuration Options for Building AREG SDK

*This document describes the variables listed in `user.cmake` file.*

## CMake Configuration Overview

The AREG SDK offers flexible build configurations via CMake, allowing users to customize compilation settings, output directories, library types, and additional features for tailored development environments. These options can be adjusted to optimize the AREG SDK binaries for different systems, such as Linux and Windows, and to control features like testing, logging, and package usage.

> [!IMPORTANT]
> The configuration options listed above are set in the [user.cmake](./../../conf/cmake/user.cmake) file, enabling both quick setup and precise build control for various project requirements.

## Options Table

The following is the list of AREG SDK specific options that needs to be set before configuring AREG SDK.

1. [AREG_COMPILER_FAMILY](#1-areg_compiler_family)
2. [AREG_COMPILER](#2-areg_compiler)
3. [AREG_TARGET](#3-areg_target)
4. [AREG_PROCESSOR](#4-areg_processor) 
5. [AREG_BINARY](#5-areg_binary)
6. [AREG_LOGGER_BINARY](#6-areg_logger_binary)
7. [AREG_BUILD_TYPE](#7-areg_build_type)
8. [AREG_BUILD_TESTS](#8-areg_build_tests)
9. [AREG_BUILD_EXAMPLES](#9-areg_build_examples)
10. [AREG_EXTENDED](#10-areg_extended)
11. [AREG_LOGS](#11-areg_logs)
12. [AREG_USE_PACKAGES](#12-areg_use_packages)
13. [AREG_SQLITE_PACKAGE](#13-areg_sqlite_package)
14. [AREG_GTEST_PACKAGE](#14-areg_gtest_package)
15. [AREG_ENABLE_OUTPUTS](#15-areg_enable_outputs)
16. [AREG_BUILD_ROOT](#16-areg_build_root)
17. [AREG_OUTPUT_DIR](#17-areg_output_dir)
18. [AREG_OUTPUT_BIN](#18-areg_output_bin)
19. [AREG_OUTPUT_LIB](#19-areg_output_lib)
20. [AREG_PACKAGES](#20-areg_packages)
21. [AREG_INSTALL](#21-areg_install)
22. [AREG_INSTALL_PATH](#22-areg_install_path)

For the quick orientation, here are the lists of AREG SDK specific options grouped by categories. See description for details. 

### Options for Compiler and Processor

The following is the list of options to select compiler:
| Options                                               | Short description                                 |
|-------------------------------------------------------|---------------------------------------------------|
| 1. [AREG_COMPILER_FAMILY](#1-areg_compiler_family)    | Select C++ compiler by family.                    |
| 2. [AREG_COMPILER](#2-areg_compiler)                  | Select C++ compiler by name.                      |
| 3. [AREG_TARGET](#3-areg_target)                      | Specifies the compiler architecture target.       |
| 3. [AREG_PROCESSOR](#4-areg_processor)                | Select processor and bitness of target application|

### Options to Build Binaries

The following is the list of options that have impact on the build:

| Options                                               | Short description                                 |
|-------------------------------------------------------|---------------------------------------------------|
| 4.  [AREG_BINARY](#5-areg_binary)                     | `areg` framework shared or static library.        |
| 5.  [AREG_LOGGER_BINARY](#6-areg_logger_binary)       | `areglogger` library shared or static library.    |
| 6.  [AREG_BUILD_TYPE](#7-areg_build_type)             | `Debug` or `Release` build.                       |
| 7.  [AREG_BUILD_TESTS](#8-areg_build_tests)           | Build or escape unit tests.                       |
| 8.  [AREG_BUILD_EXAMPLES](#9-areg_build_examples)     | Build or escape example.                          |
| 9.  [AREG_EXTENDED](#10-areg_extended)                 | Build `aregextend` library with extended objects. |
| 10. [AREG_LOGS](#11-areg_logs)                         | Build binaries with logs.


### Options to Use Packages

| Options                                               | Short description                                 |
|-------------------------------------------------------|---------------------------------------------------|
| 11. [AREG_USE_PACKAGES](#12-areg_use_packages)        | Use installed packages whenever possible.         |
| 12. [AREG_SQLITE_PACKAGE](#13-areg_sqlite_package)    | Try using installed `SQLite3` package.            |
| 13. [AREG_GTEST_PACKAGE](#14-areg_gtest_package)      | Try using installed `GTest` package.              |


### Options to Change Output Directories

| Options                                               | Short description                                 |
|-------------------------------------------------------|---------------------------------------------------|
| 14. [AREG_ENABLE_OUTPUTS](#15-areg_enable_outputs)    | Allow AREG specific output directories.           |
| 15. [AREG_BUILD_ROOT](#16-areg_build_root)            | Build directory path (binaries, packages).        |
| 16. [AREG_OUTPUT_DIR](#17-areg_output_dir)            | Output directory path for binaries.               |
| 17. [AREG_OUTPUT_BIN](#18-areg_output_bin)            | Output directory path for executables.            |
| 18. [AREG_OUTPUT_LIB](#19-areg_output_lib)            | Output directory path for static libraries.       |
| 19. [AREG_PACKAGES](#20-areg_packages)                | Directory to fetch packages (like `Google Test`). |

### Options to Install AREG Binaries

| Options                                               | Short description                                 |
|-------------------------------------------------------|---------------------------------------------------|
| 20. [AREG_INSTALL](#21-areg_install)                  | Enable install (call `cmake --install`).          |
| 21. [AREG_INSTALL_PATH](#22-areg_install_path)        | Location to install AREG binaries.                |

---

## List of Options

The following are the available CMake options to configure the AREG SDK build. Options can be set directly in the command line or before including `user.cmake` (`setup.cmake`) file, and default values are used if none are specified.

### 1. **AREG_COMPILER_FAMILY**
   - **Description**: Provides an easy way to set the compiler family for both C++ and C sources.
   - **Possible Values**: `gnu`, `llvm`, `msvc`, `cygwin`
     - `gnu`: Defaults to `g++` (C++) and `gcc` (C)
     - `llvm`: Defaults to `clang++` (C++) and `clang` (C)
     - `cygwin`: Defaults to `g++` and `gcc` for Cygwin on Windows
     - `msvc`: Uses `cl` for both C++ and C on Windows
   - **Default**: `gnu` on Linux and Cygwin, `msvc` on Windows
   - **Example**: `cmake -B ./build -DAREG_COMPILER_FAMILY=llvm`
 
> [!NOTE]  
> The value of `AREG_COMPILER_FAMILY` variable is ignored and overridden by the value of the standard CMake variable `CMAKE_CXX_COMPILER` if the `<areg-sdk-config>/user.cmake` file is included in the project *after* the first invocation of `project()`.

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 2. **AREG_COMPILER**
- **Description**: Defines the compiler for both C++ and C projects, using either the full path or a shorthand name (make sure the compiler's directory is in your PATH environment variable). Alternatively, you can set the `CMAKE_CXX_COMPILER` variable directly, achieving the same effect. This option can be skipped if `CMAKE_CXX_COMPILER` is already specified.
   - **Possible Values**: `g++`, `gcc`, `clang++`, `clang`, `c++`, `cc`, `cl`, `clang-cl`.
   - **Defaults**: System default compiler.
   - **Example**: `cmake -B ./build -DAREG_COMPILER=g++`

> [!NOTE]
> The value of `AREG_COMPILER` variable is ignored and overridden by the value of the standard CMake variable `CMAKE_CXX_COMPILER` if the `<areg-sdk-config>/user.cmake` file is included in the project *after* the first invocation of `project()`.

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 3. **AREG_TARGET**

   - **Description**: Specifies the target compiler and library architecture. Defaults to the compiler and processor architecture defined by the system.
   - **Possible Values**: `i386-linux-gnu`, `x86_64-linux-gnu`, `arm-linux-gnueabihf`, `arm-linux-gnueabi`, `aarch64-linux-gnu`, and other compiler-specific targets.
   - **Default**: *System-Defined Compiler and Processor*
   - **Example**: `cmake -B ./build -DAREG_TARGET=arm-linux-gnueabi -DAREG_PROCESSOR=arm`

> [!NOTE]
> This target is also used to specify the architecture for locating packages and libraries. The `AREG_TARGET` value is identical to `CMAKE_CXX_COMPILER_TARGET`.

---

### 4. **AREG_PROCESSOR**

   - **Description**: Specifies the target processor architecture and bitness for the compiled application. By default, it uses the system's processor architecture and bitness.
   - **Possible Values**: `x86`, `x64` (`x86_x64`, `amd64`), `arm` (`arm32`), `aarch64` (`arm64`)
   - **Default**: *System-Defined*
   - **Example**: `cmake -B ./build -DAREG_PROCESSOR=x86`

> [!NOTE]  
> The value of `AREG_PROCESSOR` may be ignored depending on the selected compiler and the inclusion timing of `<areg-sdk-config>/user.cmake`. For instance, if `AREG_PROCESSOR` is set to `arm`, the `AREG_COMPILER_FAMILY` is `gnu` and `user.cmake` is included *after* the first invocation of `project()`, the sources will be built for the host processor (like `x86_x64` using the default compiler settings.

> [!TIP]  
> To check if an application has been compiled for a 32-bit or 64-bit system, navigate to the build binary directory and run:
> ```bash
> file ./mcrouter.elf
> ```
> This command displays the binary's architecture. For example, the output for a 32-bit `mcrouter` binary may appear as:
> > ./mcrouter.elf: ELF **32-bit** LSB pie executable, **Intel 80386**, version 1 (GNU/Linux), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=3df1d5e3d1b90b9533b93a906cece6ff95fa816c, for GNU/Linux 3.2.0, not stripped

> Alternatively, use the following command to check specific details in the ELF header:
> ```bash
> od -t x1 -t c ./mcrouter | head -n 2
> ```
> In the ELF header output, the 5th byte indicates the bitness: `001` represents a 32-bit executable, while `002` indicates a 64-bit executable.
> ```plaintext
> 0000000  7f  45  4c  46  01  01  01  00  00  00  00  00  00  00  00  00
>         177   E   L   F 001 001 001  \0  \0  \0  \0  \0  \0  \0  \0  \0
> ```
> For more information, refer to the **[ELF Header documentation](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header)**.

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 5. **AREG_BINARY**
   - **Description**: Specifies the type of library for the AREG Framework.
   - **Possible Values**: `shared`, `static`
   - **Default**: `shared`
   - **Example**: `cmake -B ./build -DAREG_BINARY=static`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 6. **AREG_LOGGER_BINARY**
   - **Description**: Sets the library type for the Log Observer API.
   - **Possible Values**: `shared`, `static`
   - **Default**: `shared`
   - **Example**: `cmake -B ./build -DAREG_LOGGER_BINARY=static`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 7. **AREG_BUILD_TYPE**
   - **Description**: Defines the build configuration type, equivalent to the CMake option `CMAKE_BUILD_TYPE`. If `AREG_BUILD_TYPE` is set, it will override `CMAKE_BUILD_TYPE`. Otherwise, it takes the value of `CMAKE_BUILD_TYPE` if set, or `Release` if not set.
   - **Possible Values**: `Release`, `Debug`
   - **Default**: `Release`
   - **Example**: `cmake -B ./build -DAREG_BUILD_TYPE=Debug`

> [!NOTE]
> The value of `AREG_BUILD_TYPE` variable is ignored and overridden by the value of the standard CMake variable `CMAKE_BUILD_TYPE` if the `<areg-sdk-config>/user.cmake` file is included in the project *after* the first invocation of `project()`.

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 8. **AREG_BUILD_TESTS**
   - **Description**: Enables or disables building unit tests for the AREG SDK. If enabled (`ON`), the tests build will require an additional dependency on `GTest`.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_BUILD_TESTS=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 9. **AREG_BUILD_EXAMPLES**
   - **Description**: Controls whether to build example applications for the AREG SDK. Enabling this option (`ON`) may require additional dependencies, such as MFC (Microsoft Foundation Classes) for GUI projects on Windows.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_BUILD_EXAMPLES=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 10. **AREG_EXTENDED**
   - **Description**: Specifies if `aregextend` additional library should be build with extended features, adding specialized library support for advanced functionality. Enabling this option (`ON`) may introduce dependencies such as `ncurses` on Linux or Cygwin platforms, which are required to support these features.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `OFF`
   - **Example**: `cmake -B ./build -DAREG_EXTENDED=ON`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 11. **AREG_LOGS**
   - **Description**: Controls whether the AREG Framework is compiled with logging capabilities. Disabling logging (`OFF`) prevents applications from generating log messages, which may be useful for reducing overhead in production builds.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_LOGS=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 12. **AREG_USE_PACKAGES**
   - **Description**: Enables the use of pre-installed third-party packages, such as **Google Test** or **SQLite3**. If disabled (`OFF`), the build process will download or compile the required third-party libraries from source.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_USE_PACKAGES=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 13. **AREG_SQLITE_PACKAGE**
   - **Description**: Controls whether the build uses a system-installed SQLite3 package or compiles SQLite3 from the `thirdparty` directory. If set to `ON`, it first attempts to locate a system installation; if not found, it falls back to compiling from source.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_SQLITE_PACKAGE=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 14. **AREG_GTEST_PACKAGE**
   - **Description**: Determines whether the system-installed Google Test package is used or fetched and compiled directly from the Google repository. If enabled (`ON`), it will attempt to use the system package; otherwise, it fetches from the repository if the package isn't found.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_GTEST_PACKAGE=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 15. **AREG_ENABLE_OUTPUTS**
   - **Description**: Controls whether output directories use the CMake binary directory or AREG SDK-specific defaults. When disabled (`OFF`), AREG-specific settings are ignored in favor of CMake's default outputs, useful when integrating AREG into third-party builds.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_ENABLE_OUTPUTS=OFF`
 
<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 16. **AREG_BUILD_ROOT**
   - - **Description**: Defines the main directory for all build binary files, which also manages third-party dependencies and generated code files. This directory with the content can be safely deleted as needed without affecting codes.
   - **Default**: `<areg-sdk>/product`
   - **Example**: `cmake -B ./build -DAREG_BUILD_ROOT=/path/to/custom/build`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 17. **AREG_OUTPUT_DIR**
   - **Description**: Sets the directory for all build outputs, including binaries and libraries. The subdirectories `bin` and `lib` are used for runtime binaries and libraries, respectively.
   - **Default**: `'{AREG_BUILD_ROOT}/build/<compiler-family>/<os>-<bitness>-<cpu>-<build-type><areg-lib-type>'`
   - **Example**: `cmake -B ./build -DAREG_OUTPUT_DIR=/custom/output`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 18. **AREG_OUTPUT_BIN**
   - **Description**: Defines the output directory for runtime binaries and shared libraries, aligning with CMake's `CMAKE_RUNTIME_OUTPUT_DIRECTORY` and `CMAKE_LIBRARY_OUTPUT_DIRECTORY`.
   - **Default**: `{AREG_OUTPUT_DIR}/bin`
   - **Example**: `cmake -B ./build -DAREG_OUTPUT_BIN=/custom/output/bin`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 19. **AREG_OUTPUT_LIB**
   - **Description**: Specifies the output directory for static libraries, corresponding to the CMake setting `CMAKE_ARCHIVE_OUTPUT_DIRECTORY`.
   - **Default**: `{AREG_OUTPUT_DIR}/lib`
   - **Example**: `cmake -B ./build -DAREG_OUTPUT_LIB=/custom/output/lib`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 20. **AREG_PACKAGES**
   - **Description**: Defines where third-party packages are stored. By default, this is set to a `packages` subdirectory within `CMAKE_BINARY_DIR`.
   - **Default**: `{CMAKE_BINARY_DIR}/packages`
   - **Example**: `cmake -B ./build -DAREG_PACKAGES=/path/to/packages`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 21. **AREG_INSTALL**
   - **Description**: Specifies if the SDK installation process should be enabled. When set to `ON`, all necessary components will be prepared for installation on the specified system.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_INSTALL=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 22. **AREG_INSTALL_PATH**
   - **Description**: Specifies the directory for installing all AREG SDK build components.
   - **Default**: `<user-profile>/areg-sdk`
   - **Example**: `cmake -B ./build -DAREG_INSTALL_PATH=/install/location`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### Example Command for Configuring, building and installing AREG SDK binaries

```bash
cmake -B ./product/cache/llvm -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TESTS=OFF -DAREG_BUILD_EXAMPLES=OFF -DAREG_INSTALL=ON -DAREG_INSTALL_PATH="/usr/local"
cmake --build ./product/cache/llvm -j 20
sudo cmake --install ./product/cache/llvm
```

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### Quick Notes

- **Root Directory**: To integrate AREG SDK sources in other projects, set `AREG_SDK_ROOT` to the AREG SDK's root directory before including `user.cmake` or `setup.cmake`. In the AREG SDK project, `AREG_SDK_ROOT` defaults to `CMAKE_CURRENT_LIST_DIR` in `<areg-sdk root>/CMakeLists.txt` (i.e., `AREG_SDK_ROOT` is `<areg-sdk root>`).

- **Integrating as a Package**: When using AREG SDK as a package (e.g., via `vcpkg`), set `AREG_SDK_ROOT` to the package's `share` directory and `AREG_FRAMEWORK` to the `include` directory for headers.

- **Compiler Configuration**: The `AREG_COMPILER_FAMILY` option allows you to specify a compiler family, such as `llvm`, which automatically selects `clang++` and `clang` as the default compilers.

- **Target Processor**: `AREG_PROCESSOR` can be set to cross-compile the AREG SDK binaries. For details, check the appropriate [Cross-Compiling AREG SDK](./01b-cmake-build.md#cross-compiling-areg-sdk) chapter.

- **Custom Output Directories**: `AREG_BUILD_ROOT` can be used to set a specific location for all build-related binaries, aiding in project organization.

- **Disable AREG SDK Outputs**: Set `AREG_ENABLE_OUTPUTS` to `OFF`, to use CMake defaults or custom locations for builds.

- **Print AREG Configuration Status**: Include `setup.cmake` CMake file in your `CMakeLists.txt` file and use `printAregConfigStatus()` function to print configuration status like this:
  ```txt
  -- =======================================================================================
  -- ----------------------> AREG project CMake Status Report Begin <-----------------------
  -- =======================================================================================
  -- AREG: >>> CMAKE_SOURCE_DIR    = '/mnt/c/projects/areg-sdk', build type 'Release'
  -- AREG: >>> Build Environment ..: System 'Linux', 64-bit platform, 'x86_64' CPU
  -- AREG: >>> Compiler ...........: '/usr/bin/clang++'
  -- AREG: >>> Compiler Version ...: C++ standard 'c++17', compiler family 'llvm'
  -- AREG: >>> Binary Output Dir ..: '/mnt/c/projects/areg-sdk/product/build/llvm-clang++/linux-64-x86_64-release-shared/bin'
  -- AREG: >>> Generated Files Dir : '/mnt/c/projects/areg-sdk/product/generate'
  -- AREG: >>> Packages Dir .......: '/mnt/c/projects/areg-sdk/product/packages'
  -- AREG: >>> Build Modules ......: areg = 'shared', aregextend = static, areglogger = 'shared', executable extension '.elf'
  -- AREG: >>> Java Version .......: '17.0.12', Java executable = '/usr/bin/java', minimum version required = 17
  -- AREG: >>> Packages Use .......: SQLite3 package use = 'ON', GTest package use = 'ON'
  -- AREG: >>> Other Options ......: Examples = 'OFF', Unit Tests = 'OFF', AREG Extended = 'OFF', Logs = 'ON'
  -- AREG: >>> Installation .......: Enabled = 'ON', location = '/usr/local'
  -- =======================================================================================
  -- -----------------------> AREG project CMake Status Report End <------------------------
  -- =======================================================================================
  ```

These options allow you to customize AREG SDK to align with specific project needs, simplifying integration.

---

## Integration Example

For integration and cross-compiling examples, visit the [AREG SDK Demo Repository](https://github.com/aregtech/areg-sdk-demo) for practical implementations across various projects. Additionally, read [Integrating AREG Framework with CMake](./cmake-integration.md) document.