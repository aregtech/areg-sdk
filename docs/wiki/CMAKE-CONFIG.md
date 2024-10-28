# CMake Configuration Options for Building AREG SDK

## CMake Configuration Overview

The AREG SDK offers flexible build configurations via CMake, allowing users to customize compilation settings, output directories, library types, and additional features for tailored development environments. These options can be adjusted to optimize the SDK for different systems, such as Linux and Windows, and to control features like testing, logging, and package usage.

> [!IMPORTANT]
> The configuration options listed above are set in the [user.cmake](./../../conf/cmake/user.cmake) file, enabling both quick setup and precise build control for various project requirements.

## Options Table

1. [AREG_COMPILER_FAMILY](#1-areg_compiler_family)
2. [AREG_COMPILER](#2-areg_compiler)
3. [AREG_BINARY](#3-areg_binary)
4. [AREG_LOGGER_LIB](#4-areg_logger_lib)
5. [AREG_BUILD_TYPE](#5-areg_build_type)
6. [AREG_BUILD_TESTS](#6-areg_build_tests)
7. [AREG_BUILD_EXAMPLES](#7-areg_build_examples)
8. [AREG_EXTENDED](#8-areg_extended)
9. [AREG_LOGS](#9-areg_logs)
10. [AREG_INSTALL](#10-areg_install)
11. [AREG_USE_PACKAGES](#11-areg_use_packages)
12. [AREG_SQLITE_PACKAGE](#12-areg_sqlite_package)
13. [AREG_GTEST_PACKAGE](#13-areg_gtest_package)
14. [AREG_BUILD_ROOT](#14-areg_build_root)
15. [AREG_OUTPUT_DIR](#15-areg_output_dir)
16. [AREG_OUTPUT_BIN](#16-areg_output_bin)
17. [AREG_OUTPUT_LIB](#17-areg_output_lib)
18. [AREG_PACKAGES](#18-areg_packages)
19. [AREG_INSTALL_PATH](#19-areg_install_path)
20. [AREG_ENABLE_OUTPUTS](#20-areg_enable_outputs)

---

## List of Options

The following are the available CMake options to configure the AREG SDK build. Options can be set directly in the command line or before including `user.cmake` (`setup.cmake`) file, and default values are used if none are specified.

### 1. **AREG_COMPILER_FAMILY**
   - **Description**: Provides an easy way to set the compiler family for both C++ and C sources.
   - **Possible Values**: `gnu`, `cygwin`, `llvm`, `msvc`
     - `gnu`: Defaults to `g++` (C++) and `gcc` (C)
     - `llvm`: Defaults to `clang++` (C++) and `clang` (C)
     - `cygwin`: Defaults to `g++` and `gcc` for Cygwin on Windows
     - `msvc`: Uses `cl` for both C++ and C on Windows
   - **Default**: `gnu` on Linux and Cygwin, `msvc` on Windows
   - **Example**: `cmake -B ./build -DAREG_COMPILER_FAMILY=llvm`
 
<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 2. **AREG_COMPILER**
- **Description**: Defines the compiler for both C++ and C projects, using either the full path or a shorthand name (make sure the compiler’s directory is in your PATH environment variable). Alternatively, you can set the `CMAKE_CXX_COMPILER` variable directly, achieving the same effect. This option can be skipped if `CMAKE_CXX_COMPILER` is already specified.
   - **Possible Values**: `g++`, `gcc`, `clang++`, `clang`, `c++`, `cc`, `cl`, `clang-cl`.
   - **Defaults**: System default compiler.
   - **Example**: `cmake -B ./build -DAREG_COMPILER=g++`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 3. **AREG_BINARY**
   - **Description**: Specifies the type of library for the AREG Framework.
   - **Possible Values**: `shared`, `static`
   - **Default**: `shared`
   - **Example**: `cmake -B ./build -DAREG_BINARY=static`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 4. **AREG_LOGGER_LIB**
   - **Description**: Sets the library type for the Log Observer API.
   - **Possible Values**: `shared`, `static`
   - **Default**: `shared`
   - **Example**: `cmake -B ./build -DAREG_LOGGER_LIB=static`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 5. **AREG_BUILD_TYPE**
   - **Description**: Defines the build configuration type, equivalent to the CMake option `CMAKE_BUILD_TYPE`. If `AREG_BUILD_TYPE` is set, it will override `CMAKE_BUILD_TYPE`.
   - **Possible Values**: `Release`, `Debug`
   - **Default**: `Release`
   - **Example**: `cmake -B ./build -DAREG_BUILD_TYPE=Debug`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 6. **AREG_BUILD_TESTS**
   - **Description**: Enables or disables building unit tests for the AREG SDK. If enabled (`ON`), the tests build will require an additional dependency on `GTest`.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `OFF`
   - **Example**: `cmake -B ./build -DAREG_BUILD_TESTS=ON`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 7. **AREG_BUILD_EXAMPLES**
   - **Description**: Controls whether to build example applications for the AREG SDK. Enabling this option (`ON`) may require additional dependencies, such as MFC (Microsoft Foundation Classes) for GUI projects on Windows.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_BUILD_EXAMPLES=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 8. **AREG_EXTENDED**
   - **Description**: Specifies if `aregextend` additional library should be build with extended features, adding specialized library support for advanced functionality. Enabling this option (`ON`) may introduce dependencies such as `ncurses` on Linux or Cygwin platforms, which are required to support these features.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `OFF`
   - **Example**: `cmake -B ./build -DAREG_EXTENDED=ON`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 9. **AREG_LOGS**
   - **Description**: Controls whether the AREG Framework is compiled with logging capabilities. Disabling logging (`OFF`) prevents applications from generating log messages, which may be useful for reducing overhead in production builds.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_LOGS=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 10. **AREG_INSTALL**
   - **Description**: Specifies if the SDK installation process should be enabled. When set to `ON`, all necessary components will be prepared for installation on the specified system.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_INSTALL=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 11. **AREG_USE_PACKAGES**
   - **Description**: Enables the use of pre-installed third-party packages, such as **Google Test** or **SQLite3**. If disabled (`OFF`), the build process will download or compile the required third-party libraries from source.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_USE_PACKAGES=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 12. **AREG_SQLITE_PACKAGE**
   - **Description**: Controls whether the build uses a system-installed SQLite3 package or compiles SQLite3 from the `thirdparty` directory. If set to `ON`, it first attempts to locate a system installation; if not found, it falls back to compiling from source.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_SQLITE_PACKAGE=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 13. **AREG_GTEST_PACKAGE**
   - **Description**: Determines whether the system-installed Google Test package is used or fetched and compiled directly from the Google repository. If enabled (`ON`), it will attempt to use the system package; otherwise, it fetches from the repository if the package isn’t found.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_GTEST_PACKAGE=OFF`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 14. **AREG_BUILD_ROOT**
   - - **Description**: Defines the main directory for all build binary files, which also manages third-party dependencies and generated code files. This directory with the content can be safely deleted as needed without affecting codes.
   - **Default**: `<areg-sdk>/product`
   - **Example**: `cmake -B ./build -DAREG_BUILD_ROOT=/path/to/custom/build`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 15. **AREG_OUTPUT_DIR**
   - **Description**: Sets the directory for all build outputs, including binaries and libraries. The subdirectories `bin` and `lib` are used for runtime binaries and libraries, respectively.
   - **Default**: `'<areg-sdk>/product/build/<compiler-family>/<os>-<bitness>-<cpu>-release-<areg-lib>'`
   - **Example**: `cmake -B ./build -DAREG_OUTPUT_DIR=/custom/output`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 16. **AREG_OUTPUT_BIN**
   - **Description**: Defines the output directory for runtime binaries and shared libraries, aligning with CMake's `CMAKE_RUNTIME_OUTPUT_DIRECTORY` and `CMAKE_LIBRARY_OUTPUT_DIRECTORY`.
   - **Default**: `AREG_OUTPUT_DIR/bin`
   - **Example**: `cmake -B ./build -DAREG_OUTPUT_BIN=/custom/output/bin`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 17. **AREG_OUTPUT_LIB**
   - **Description**: Specifies the output directory for static libraries, corresponding to the CMake setting `CMAKE_ARCHIVE_OUTPUT_DIRECTORY`.
   - **Default**: `AREG_OUTPUT_DIR/lib`
   - **Example**: `cmake -B ./build -DAREG_OUTPUT_LIB=/custom/output/lib`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 18. **AREG_PACKAGES**
   - **Description**: Defines where third-party packages are stored. By default, this is set to a `packages` subdirectory within `AREG_BUILD_ROOT`.
   - **Default**: `AREG_BUILD_ROOT/packages`
   - **Example**: `cmake -B ./build -DAREG_PACKAGES=/path/to/packages`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 19. **AREG_INSTALL_PATH**
   - **Description**: Specifies the directory for installing all AREG SDK build components.
   - **Default**: `<user-profile>/areg-sdk`
   - **Example**: `cmake -B ./build -DAREG_INSTALL_PATH=/install/location`

<div align="right"><kbd><a href="#options-table">↑ Back to top ↑</a></kbd></div>

---

### 20. **AREG_ENABLE_OUTPUTS**
   - **Description**: Controls whether output directories use the CMake binary directory or AREG SDK-specific defaults. When disabled (`OFF`), AREG-specific settings are ignored in favor of CMake’s default outputs, useful when integrating AREG into third-party builds.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_ENABLE_OUTPUTS=OFF`
 
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

- **Root Directory**: To integrate AREG SDK sources in other projects, set `AREG_SDK_ROOT` to the AREG SDK’s root directory before including `user.cmake` or `setup.cmake`. In the AREG SDK project, `AREG_SDK_ROOT` defaults to `CMAKE_CURRENT_LIST_DIR` in `<areg-sdk root>/CMakeLists.txt` (i.e., `AREG_SDK_ROOT` is `<areg-sdk root>`).

- **Integrating as a Package**: When using AREG SDK as a package (e.g., via `vcpkg`), set `AREG_SDK_ROOT` to the package's `share` directory and `AREG_FRAMEWORK` to the `include` directory for headers.

- **Compiler Configuration**: The `AREG_COMPILER_FAMILY` option allows you to specify a compiler family, such as `llvm`, which automatically selects `clang++` and `clang` as the default compilers.

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
  -- AREG: >>> Build Modules ......: areg = 'shared', aregextend = static, areglogger = 'shared', executable extension '.out'
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

For integration examples, visit the [AREG SDK Demo Repository](https://github.com/aregtech/areg-sdk-demo) for practical implementations across various projects.
