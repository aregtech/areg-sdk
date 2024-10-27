# CMake Configuration Options for Building AREG SDK

## CMake Configuration Overview

The AREG SDK offers flexible build configurations via CMake, allowing users to customize compilation settings, output directories, library types, and additional features for tailored development environments. These options can be adjusted to optimize the SDK for different systems, such as Linux and Windows, and to control features like testing, logging, and package usage.

> [!IMPORTANT]
> The configuration options listed above are set in the [user.cmake](./../../conf/cmake/user.cmake) file, enabling both quick setup and precise build control for various project requirements.

## Table of Options

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
 
<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 2. **AREG_COMPILER**
- **Description**: Defines the compiler for both C++ and C projects, using either the full path or a shorthand name (make sure the compiler’s directory is in your PATH environment variable). Alternatively, you can set the `CMAKE_CXX_COMPILER` variable directly, achieving the same effect. This option can be skipped if `CMAKE_CXX_COMPILER` is already specified.
   - **Possible Values**: `g++`, `gcc`, `clang++`, `clang`, `c++`, `cc`, `cl`, `clang-cl`.
   - **Defaults**: System default compiler.
   - **Example**: `cmake -B ./build -DAREG_COMPILER=g++`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 3. **AREG_BINARY**
   - **Description**: Specifies the type of library for the AREG Framework.
   - **Possible Values**: `shared`, `static`
   - **Default**: `shared`
   - **Example**: `cmake -B ./build -DAREG_BINARY=static`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 4. **AREG_LOGGER_LIB**
   - **Description**: Sets the library type for the Log Observer API.
   - **Possible Values**: `shared`, `static`
   - **Default**: `shared`
   - **Example**: `cmake -B ./build -DAREG_LOGGER_LIB=static`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 5. **AREG_BUILD_TYPE**
   - **Description**: Defines the build configuration type, equivalent to the CMake option `CMAKE_BUILD_TYPE`. If `AREG_BUILD_TYPE` is set, it will override `CMAKE_BUILD_TYPE`.
   - **Possible Values**: `Release`, `Debug`
   - **Default**: `Release`
   - **Example**: `cmake -B ./build -DAREG_BUILD_TYPE=Debug`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 6. **AREG_BUILD_TESTS**
   - **Description**: Enables or disables building unit tests for the AREG SDK. If enabled, the tests build will require an additional dependency on `GTest`.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `OFF`
   - **Example**: `cmake -B ./build -DAREG_BUILD_TESTS=ON`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 7. **AREG_BUILD_EXAMPLES**
   - **Description**: Controls whether to build example applications for the AREG SDK. Enabling this option may require additional dependencies, such as MFC (Microsoft Foundation Classes) for GUI projects on Windows.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_BUILD_EXAMPLES=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 8. **AREG_EXTENDED**
   - **Description**: Enables the compilation of the additional library of the AREG with extended features, which may introduce additional dependencies like `ncurses` on Linux and Cygwin platforms.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `OFF`
   - **Example**: `cmake -B ./build -DAREG_EXTENDED=ON`
   
<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 9. **AREG_LOGS**
   - **Description**: Enables or disables compilation with logs. If compiled without logs, the applications using AREG Framework will not be able to log messages.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_LOGS=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 10. **AREG_INSTALL**
   - **Description**: Enables or disables the SDK installation process.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_INSTALL=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 11. **AREG_USE_PACKAGES**
   - **Description**: Enables or disables the use of system-installed thirdparty packages like *google test* or *SQLite3*. If disabled, will try either to fetch the sources from remote repository or compile the thirdparty *SQLite3* library.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_USE_PACKAGES=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 12. **AREG_SQLITE_PACKAGE**
   - **Description**: Determines if the system's SQLite3 package should be used or compile the existing sources in *thirdparty* directory.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_SQLITE_PACKAGE=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 13. **AREG_GTEST_PACKAGE**
   - **Description**: Determines if the system’s GTest package should be used or fetch the sources from the official remote repository.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_GTEST_PACKAGE=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 14. **AREG_BUILD_ROOT**
   - **Description**: Defines the root directory for build binary files. The directory will be also used to fetch thirdparty dependencies.
   - **Default**: `<areg-sdk>/product`
   - **Example**: `cmake -B ./build -DAREG_BUILD_ROOT=/path/to/custom/build`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 15. **AREG_OUTPUT_DIR**
   - **Description**: Sets the directory for build binary outputs. By default, the sub-directories `bin` and `lib` will be used to output runtime binaries, shared and static libraries.
   - **Default**: `'<areg-sdk>/product/build/<default-compiler-family>/<os>-<bitness>-<cpu>-release-<areg-lib>'`
   - **Example**: `cmake -B ./build -DAREG_OUTPUT_DIR=/custom/output`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 16. **AREG_OUTPUT_BIN**
   - **Description**: Directory for output runtime and shared library binaries. This is equivalent to CMake `CMAKE_RUNTIME_OUTPUT_DIRECTORY` and `CMAKE_LIBRARY_OUTPUT_DIRECTORY` settings.
   - **Default**: `AREG_OUTPUT_DIR/bin`
   - **Example**: `cmake -B ./build -DAREG_OUTPUT_BIN=/custom/output/bin`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 17. **AREG_OUTPUT_LIB**
   - **Description**: Directory for output static libraries. This is equivalent to CMake `CMAKE_ARCHIVE_OUTPUT_DIRECTORY` settings.
   - **Default**: `AREG_OUTPUT_DIR/lib`
   - **Example**: `cmake -B ./build -DAREG_OUTPUT_LIB=/custom/output/lib`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 18. **AREG_PACKAGES**
   - **Description**: Location for fetching third-party packages. By default it is `packages` subdirectory of `AREG_BUILD_ROOT`.
   - **Default**: `AREG_BUILD_ROOT/packages`
   - **Example**: `cmake -B ./build -DAREG_PACKAGES=/path/to/packages`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 19. **AREG_INSTALL_PATH**
   - **Description**: Directory for installing AREG SDK build components.
   - **Default**: `<user-profile>/areg-sdk`
   - **Example**: `cmake -B ./build -DAREG_INSTALL_PATH=/install/location`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 20. **AREG_ENABLE_OUTPUTS**
   - **Description**: Controls whether output directories match the CMake binary directory or not. If disabled (`OFF`) the AREG SDK default settings `AREG_OUTPUT_XXX` are ignored and CMake default settings are used. This setting can be used if AREG Framework is compiled in the thirdparty project build, so that it will use the thirdparty settings, instead of defining own, AREG SDK specific settings.
   - **Possible Values**: `ON`, `OFF`
   - **Default**: `ON`
   - **Example**: `cmake -B ./build -DAREG_ENABLE_OUTPUTS=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### Example Command for Configuring, building and installing AREG SDK binaries

```bash
cmake -B ./product/cache/llvm -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TESTS=OFF -DAREG_BUILD_EXAMPLES=OFF -DAREG_USE_PACKAGES=ON -DAREG_INSTALL=ON -DAREG_INSTALL_PATH="/usr/local"
cmake --build ./product/cache/llvm -j 20
sudo cmake --install ./product/cache/llvm
```

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### Notes
- **Compiler Configuration**: `AREG_COMPILER_FAMILY` simplifies setting default compilers, e.g., `llvm` sets `clang++` and `clang`.
- **Custom Output Directories**: Use `AREG_BUILD_ROOT` to direct binaries to a specific location within a project.

These options allow you to customize the AREG SDK to fit project requirements and streamline integration.

---

## Integration

Visit https://github.com/aregtech/areg-sdk-demo repository to see practical examples of various ways to integrate AREG Framework in the projects.
