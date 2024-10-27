# CMake Configuration Options for Building AREG SDK

## CMake Configuration Overview

The AREG SDK offers flexible build configurations via CMake, allowing users to customize compilation settings, output directories, library types, and additional features for tailored development environments. These options can be adjusted to optimize the SDK for different systems, such as Linux and Windows, and to control features like testing, logging, and package usage.

> [!IMPORTANT]
> The configuration options listed above are set in the [user.cmake](./../../conf/cmake/user.cmake) file, enabling both quick setup and precise build control for various project requirements.

## Table of Options

1. [AREG_COMPILER_FAMILY](#areg_compiler_family)
2. [AREG_COMPILER](#areg_compiler)
3. [AREG_BINARY](#areg_binary)
4. [AREG_LOGGER_LIB](#areg_logger_lib)
5. [AREG_BUILD_TYPE](#areg_build_type)
6. [AREG_BUILD_TESTS](#areg_build_tests)
7. [AREG_BUILD_EXAMPLES](#areg_build_examples)
8. [AREG_EXTENDED](#areg_extended)
9. [AREG_LOGS](#areg_logs)
10. [AREG_INSTALL](#areg_install)
11. [AREG_USE_PACKAGES](#areg_use_packages)
12. [AREG_SQLITE_PACKAGE](#areg_sqlite_package)
13. [AREG_GTEST_PACKAGE](#areg_gtest_package)
14. [AREG_BUILD_ROOT](#areg_build_root)
15. [AREG_OUTPUT_DIR](#areg_output_dir)
16. [AREG_OUTPUT_BIN](#areg_output_bin)
17. [AREG_OUTPUT_LIB](#areg_output_lib)
18. [AREG_PACKAGES](#areg_packages)
19. [AREG_INSTALL_PATH](#areg_install_path)
20. [AREG_ENABLE_OUTPUTS](#areg_enable_outputs)

---

The following are the available CMake options to configure the AREG SDK build. Options can be set directly in the command line, and default values are used if none are specified.

### 1. **AREG_COMPILER_FAMILY**
   - **Description**: Specifies a quick way to set the C++ and C compilers by family.
   - **Possible Values**: `gnu`, `cygwin`, `llvm`, `msvc`
   - **Example**: `-DAREG_COMPILER_FAMILY=llvm`
 
<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 2. **AREG_COMPILER**
   - **Description**: Sets a specific compiler for C++ and C projects.
   - **Possible Values**: `g++`, `gcc`, `clang++`, `clang`, `cl`, etc.
   - **Example**: `-DAREG_COMPILER=g++`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 3. **AREG_BINARY**
   - **Description**: Specifies the type of library for the AREG Framework.
   - **Default**: `shared`
   - **Possible Values**: `shared`, `static`
   - **Example**: `-DAREG_BINARY=static`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 4. **AREG_LOGGER_LIB**
   - **Description**: Sets the library type for the Log Observer API.
   - **Default**: `shared`
   - **Possible Values**: `shared`, `static`
   - **Example**: `-DAREG_LOGGER_LIB=static`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 5. **AREG_BUILD_TYPE**
   - **Description**: Specifies the build configuration.
   - **Default**: `Release`
   - **Possible Values**: `Release`, `Debug`
   - **Example**: `-DAREG_BUILD_TYPE=Debug`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 6. **AREG_BUILD_TESTS**
   - **Description**: Enables or disables building unit tests.
   - **Default**: `OFF`
   - **Possible Values**: `ON`, `OFF`
   - **Example**: `-DAREG_BUILD_TESTS=ON`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 7. **AREG_BUILD_EXAMPLES**
   - **Description**: Enables or disables building example applications.
   - **Default**: `ON`
   - **Possible Values**: `ON`, `OFF`
   - **Example**: `-DAREG_BUILD_EXAMPLES=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 8. **AREG_EXTENDED**
   - **Description**: Enables extended framework features, which may require additional dependencies.
   - **Default**: `OFF`
   - **Possible Values**: `ON`, `OFF`
   - **Example**: `-DAREG_EXTENDED=ON`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 9. **AREG_LOGS**
   - **Description**: Enables logging during compilation.
   - **Default**: `ON`
   - **Possible Values**: `ON`, `OFF`
   - **Example**: `-DAREG_LOGS=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 10. **AREG_INSTALL**
   - **Description**: Enables or disables the SDK installation process.
   - **Default**: `ON`
   - **Possible Values**: `ON`, `OFF`
   - **Example**: `-DAREG_INSTALL=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 11. **AREG_USE_PACKAGES**
   - **Description**: Enables or disables the use of system-installed packages.
   - **Default**: `ON`
   - **Possible Values**: `ON`, `OFF`
   - **Example**: `-DAREG_USE_PACKAGES=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 12. **AREG_SQLITE_PACKAGE**
   - **Description**: Determines if the system's SQLite3 package should be used.
   - **Default**: `ON`
   - **Possible Values**: `ON`, `OFF`
   - **Example**: `-DAREG_SQLITE_PACKAGE=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 13. **AREG_GTEST_PACKAGE**
   - **Description**: Determines if the system’s GTest package should be used.
   - **Default**: `ON`
   - **Possible Values**: `ON`, `OFF`
   - **Example**: `-DAREG_GTEST_PACKAGE=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 14. **AREG_BUILD_ROOT**
   - **Description**: Defines the root directory for build files.
   - **Default**: `<areg-sdk>/product`
   - **Example**: `-DAREG_BUILD_ROOT=/path/to/custom/build`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 15. **AREG_OUTPUT_DIR**
   - **Description**: Sets the directory for build outputs.
   - **Default**: `'<areg-sdk>/product/build/<default-compiler-family>/<os>-<bitness>-<cpu>-release-<areg-lib>'`
   - **Example**: `-DAREG_OUTPUT_DIR=/custom/output`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 16. **AREG_OUTPUT_BIN**
   - **Description**: Directory for output binaries (executables and shared libraries).
   - **Default**: `AREG_OUTPUT_DIR/bin`
   - **Example**: `-DAREG_OUTPUT_BIN=/custom/output/bin`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 17. **AREG_OUTPUT_LIB**
   - **Description**: Directory for output static libraries.
   - **Default**: `AREG_OUTPUT_DIR/lib`
   - **Example**: `-DAREG_OUTPUT_LIB=/custom/output/lib`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 18. **AREG_PACKAGES**
   - **Description**: Location for fetching third-party packages.
   - **Default**: `AREG_BUILD_ROOT/packages`
   - **Example**: `-DAREG_PACKAGES=/path/to/packages`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 19. **AREG_INSTALL_PATH**
   - **Description**: Directory for installing AREG SDK components.
   - **Default**: `~/areg-sdk`
   - **Example**: `-DAREG_INSTALL_PATH=/install/location`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### 20. **AREG_ENABLE_OUTPUTS**
   - **Description**: Controls whether output directories match the CMake binary directory.
   - **Default**: `ON`
   - **Possible Values**: `ON`, `OFF`
   - **Example**: `-DAREG_ENABLE_OUTPUTS=OFF`

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### Example Command for Configuring AREG SDK Build
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TESTS=ON -DAREG_BUILD_EXAMPLES=ON -DAREG_INSTALL=ON
```

<div align="right"><kbd><a href="#table-of-options">↑ Back to top ↑</a></kbd></div>

---

### Notes
- **Compiler Configuration**: `AREG_COMPILER_FAMILY` simplifies setting default compilers, e.g., `llvm` sets `clang++` and `clang`.
- **Custom Output Directories**: Use `AREG_BUILD_ROOT` to direct binaries to a specific location within a project.

These options allow you to customize the AREG SDK to fit project requirements and streamline integration.