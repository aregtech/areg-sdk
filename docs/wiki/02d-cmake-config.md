# CMake Configuration Options for Areg SDK

This guide describes all CMake configuration options for building Areg SDK, including compiler selection, build customization, output directories, and installation settings.

> [!IMPORTANT]
> Configuration options are set in [user.cmake](../../conf/cmake/user.cmake) or via command-line arguments. Command-line options take precedence.

---

## Table of Contents

1. [Quick Configuration Examples](#quick-configuration-examples)
2. [Complete Options Reference](#complete-options-reference)
3. [Options by Category](#options-by-category)
4. [Detailed Option Descriptions](#detailed-option-descriptions)
5. [Advanced Configuration](#advanced-configuration)
6. [Integration Examples](#integration-examples)

---

## Quick Configuration Examples

### Common Build Configurations

**1. Default build (GCC, Release, all features):**
```bash
cmake -B ./build
cmake --build ./build -j20
```

**2. Debug build with Clang:**
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TYPE=Debug
cmake --build ./build -j20
```

**3. Release build without examples or tests:**
```bash
cmake -B ./build -DAREG_BUILD_TYPE=Release -DAREG_BUILD_EXAMPLES=OFF -DAREG_BUILD_TESTS=OFF
cmake --build ./build -j20
```

**4. Static library build:**
```bash
cmake -B ./build -DAREG_BINARY=static -DAREG_LOGGER_BINARY=static
cmake --build ./build -j20
```

**5. Cross-compile for ARM:**
```bash
cmake -B ./build -DAREG_PROCESSOR=arm -DAREG_COMPILER_FAMILY=gnu
cmake --build ./build -j20
```

**6. Build and install to custom location:**
```bash
cmake -B ./build -DAREG_INSTALL=ON -DAREG_INSTALL_PATH=/opt/areg
cmake --build ./build -j20
sudo cmake --install ./build
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Complete Options Reference

All 22 CMake configuration options for Areg SDK:

| # | Option | Values | Default | Description |
|---|--------|--------|---------|-------------|
| 1 | [AREG_COMPILER_FAMILY](#1-areg_compiler_family) | `gnu`, `llvm`, `msvc`, `cygwin` | System default | Compiler family selection |
| 2 | [AREG_COMPILER](#2-areg_compiler) | Compiler name/path | System default | Specific compiler executable |
| 3 | [AREG_TARGET](#3-areg_target) | Architecture string | System default | Compiler architecture target |
| 4 | [AREG_PROCESSOR](#4-areg_processor) | `x86`, `x64`, `arm`, `aarch64` | System default | Target CPU architecture |
| 5 | [AREG_BINARY](#5-areg_binary) | `shared`, `static` | `shared` | Areg framework library type |
| 6 | [AREG_LOGGER_BINARY](#6-areg_logger_binary) | `shared`, `static` | `shared` | Logger library type |
| 7 | [AREG_BUILD_TYPE](#7-areg_build_type) | `Debug`, `Release` | `Release` | Build configuration |
| 8 | [AREG_BUILD_TESTS](#8-areg_build_tests) | `ON`, `OFF` | `ON` | Build unit tests |
| 9 | [AREG_BUILD_EXAMPLES](#9-areg_build_examples) | `ON`, `OFF` | `ON` | Build example applications |
| 10 | [AREG_EXTENDED](#10-areg_extended) | `ON`, `OFF` | `OFF` | Build extended library |
| 11 | [AREG_LOGS](#11-areg_logs) | `ON`, `OFF` | `ON` | Enable logging |
| 12 | [AREG_USE_PACKAGES](#12-areg_use_packages) | `ON`, `OFF` | `ON` | Use system packages |
| 13 | [AREG_SQLITE_PACKAGE](#13-areg_sqlite_package) | `ON`, `OFF` | `ON` | Use system SQLite3 |
| 14 | [AREG_GTEST_PACKAGE](#14-areg_gtest_package) | `ON`, `OFF` | `ON` | Use system Google Test |
| 15 | [AREG_ENABLE_OUTPUTS](#15-areg_enable_outputs) | `ON`, `OFF` | `ON` | Use Areg output paths |
| 16 | [AREG_BUILD_ROOT](#16-areg_build_root) | Directory path | `<areg-sdk>/product` | Root build directory |
| 17 | [AREG_OUTPUT_DIR](#17-areg_output_dir) | Directory path | Auto-generated | Output directory |
| 18 | [AREG_OUTPUT_BIN](#18-areg_output_bin) | Directory path | `{AREG_OUTPUT_DIR}/bin` | Executable output |
| 19 | [AREG_OUTPUT_LIB](#19-areg_output_lib) | Directory path | `{AREG_OUTPUT_DIR}/lib` | Library output |
| 20 | [AREG_PACKAGES](#20-areg_packages) | Directory path | `{CMAKE_BINARY_DIR}/packages` | Packages directory |
| 21 | [AREG_INSTALL](#21-areg_install) | `ON`, `OFF` | `ON` | Enable installation |
| 22 | [AREG_INSTALL_PATH](#22-areg_install_path) | Directory path | `<user-profile>/areg-sdk` | Installation directory |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Options by Category

For easier navigation, options are grouped by purpose:

### Compiler and Processor Options

| # | Option | Short Description |
|---|--------|-------------------|
| 1 | [AREG_COMPILER_FAMILY](#1-areg_compiler_family) | Select C++ compiler by family |
| 2 | [AREG_COMPILER](#2-areg_compiler) | Select C++ compiler by name |
| 3 | [AREG_TARGET](#3-areg_target) | Compiler architecture target |
| 4 | [AREG_PROCESSOR](#4-areg_processor) | Target processor and bitness |

### Build Configuration Options

| # | Option | Short Description |
|---|--------|-------------------|
| 5 | [AREG_BINARY](#5-areg_binary) | `areg` library type (shared/static) |
| 6 | [AREG_LOGGER_BINARY](#6-areg_logger_binary) | `areglogger` library type |
| 7 | [AREG_BUILD_TYPE](#7-areg_build_type) | Debug or Release build |
| 8 | [AREG_BUILD_TESTS](#8-areg_build_tests) | Build or skip unit tests |
| 9 | [AREG_BUILD_EXAMPLES](#9-areg_build_examples) | Build or skip examples |
| 10 | [AREG_EXTENDED](#10-areg_extended) | Build `aregextend` library |
| 11 | [AREG_LOGS](#11-areg_logs) | Build binaries with logs |

### Package Management Options

| # | Option | Short Description |
|---|--------|-------------------|
| 12 | [AREG_USE_PACKAGES](#12-areg_use_packages) | Use installed packages when possible |
| 13 | [AREG_SQLITE_PACKAGE](#13-areg_sqlite_package) | Try using installed SQLite3 |
| 14 | [AREG_GTEST_PACKAGE](#14-areg_gtest_package) | Try using installed Google Test |

### Output Directory Options

| # | Option | Short Description |
|---|--------|-------------------|
| 15 | [AREG_ENABLE_OUTPUTS](#15-areg_enable_outputs) | Allow Areg-specific output paths |
| 16 | [AREG_BUILD_ROOT](#16-areg_build_root) | Build root directory |
| 17 | [AREG_OUTPUT_DIR](#17-areg_output_dir) | Output directory for binaries |
| 18 | [AREG_OUTPUT_BIN](#18-areg_output_bin) | Executable output directory |
| 19 | [AREG_OUTPUT_LIB](#19-areg_output_lib) | Static library output directory |
| 20 | [AREG_PACKAGES](#20-areg_packages) | Third-party packages directory |

### Installation Options

| # | Option | Short Description |
|---|--------|-------------------|
| 21 | [AREG_INSTALL](#21-areg_install) | Enable install target |
| 22 | [AREG_INSTALL_PATH](#22-areg_install_path) | Installation location |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Detailed Option Descriptions

### 1. AREG_COMPILER_FAMILY

**Description:** Select compiler family for C++ and C sources.

**Possible Values:**
- `gnu` - GCC compiler family (`g++`, `gcc`)
- `llvm` - LLVM/Clang compiler family (`clang++`, `clang`)
- `msvc` - Microsoft Visual C++ (`cl`)
- `cygwin` - GCC for Cygwin on Windows

**Default:** `gnu` on Linux/Cygwin, `msvc` on Windows  
**Example:**
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=llvm
```

> [!NOTE]
> This option is ignored if `CMAKE_CXX_COMPILER` is set and `user.cmake` is included after the first `project()` call.

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 2. AREG_COMPILER

**Description:** Specify exact compiler executable by name or full path.  
**Possible Values:** `g++`, `gcc`, `clang++`, `clang`, `c++`, `cc`, `cl`, `clang-cl`, or full path  
**Default:** System default compiler  
**Example:**
```bash
cmake -B ./build -DAREG_COMPILER=/usr/bin/clang++-14
```

> [!NOTE]
> Setting `CMAKE_CXX_COMPILER` directly achieves the same effect. If `CMAKE_CXX_COMPILER` is set and `user.cmake` is included after `project()`, this option is ignored.

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 3. AREG_TARGET

**Description:** Specify compiler architecture target for cross-compilation.  
**Possible Values:** `i386-linux-gnu`, `x86_64-linux-gnu`, `arm-linux-gnueabihf`, `arm-linux-gnueabi`, `aarch64-linux-gnu`, and other compiler-specific targets  
**Default:** System-defined  
**Example:**
```bash
cmake -B ./build -DAREG_TARGET=arm-linux-gnueabihf -DAREG_PROCESSOR=arm
```

> [!NOTE]
> This value is identical to `CMAKE_CXX_COMPILER_TARGET` and affects package and library location paths.

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 4. AREG_PROCESSOR

**Description:** Target processor architecture and bitness for compiled applications.  
**Possible Values:**
- `x86` - 32-bit x86
- `x64` (or `x86_64`, `amd64`) - 64-bit x86
- `arm` (or `arm32`) - 32-bit ARM
- `aarch64` (or `arm64`) - 64-bit ARM

**Default:** System-defined  
**Example:**
```bash
cmake -B ./build -DAREG_PROCESSOR=x86
```

> [!NOTE]
> The value may be ignored depending on compiler selection and when `user.cmake` is included. For instance, setting `AREG_PROCESSOR=arm` with `AREG_COMPILER_FAMILY=gnu` when `user.cmake` is included after `project()` may build for the host processor using default compiler settings.

> [!TIP]
> To verify binary architecture, navigate to build directory and run:
> ```bash
> file ./mtrouter.elf
> ```
> Example output for 32-bit:
> ```
> ./mtrouter.elf: ELF 32-bit LSB pie executable, Intel 80386, version 1 (GNU/Linux), ...
> ```
> 
> Or check ELF header:
> ```bash
> od -t x1 -t c ./mtrouter | head -n 2
> ```
> 
> The 5th byte indicates bitness: `01` = 32-bit, `02` = 64-bit
> ```
> 0000000  7f  45  4c  46  01  01  01  00  00  00  00  00  00  00  00  00
>         177   E   L   F 001 001 001  \0  \0  \0  \0  \0  \0  \0  \0  \0
> ```
> 
> For more information, see [ELF Header documentation](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header).

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 5. AREG_BINARY

**Description:** Specify Areg framework library type.  
**Possible Values:**
- `shared` - Dynamic library (`.so`, `.dll`)
- `static` - Static library (`.a`, `.lib`)

**Default:** `shared`  
**Example:**
```bash
cmake -B ./build -DAREG_BINARY=static
```

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 6. AREG_LOGGER_BINARY

**Description:** Specify log observer API library type.  
**Possible Values:**
- `shared` - Dynamic library
- `static` - Static library

**Default:** `shared`  
**Example:**
```bash
cmake -B ./build -DAREG_LOGGER_BINARY=static
```

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 7. AREG_BUILD_TYPE

**Description:** Build configuration type, equivalent to `CMAKE_BUILD_TYPE`.  
**Possible Values:**
- `Release` - Optimized build, no debug symbols
- `Debug` - Debug symbols, no optimization

**Default:** `Release`  
**Example:**
```bash
cmake -B ./build -DAREG_BUILD_TYPE=Debug
```

> [!NOTE]
> If `CMAKE_BUILD_TYPE` is set and `user.cmake` is included after `project()`, this option is ignored and `CMAKE_BUILD_TYPE` takes precedence.

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 8. AREG_BUILD_TESTS

**Description:** Enable or disable building unit tests.  
**Possible Values:** `ON`, `OFF`  
**Default:** `ON`  
**Dependencies:** Requires Google Test (fetched automatically if `AREG_GTEST_PACKAGE=OFF`)  
**Example:**
```bash
cmake -B ./build -DAREG_BUILD_TESTS=OFF
```

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 9. AREG_BUILD_EXAMPLES

**Description:** Enable or disable building example applications.  
**Possible Values:** `ON`, `OFF`  
**Default:** `ON`  
**Dependencies:** Some examples require MFC (Microsoft Foundation Classes) on Windows  
**Example:**
```bash
cmake -B ./build -DAREG_BUILD_EXAMPLES=OFF
```

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 10. AREG_EXTENDED

**Description:** Build `aregextend` library with extended features.  
**Possible Values:** `ON`, `OFF`  
**Default:** `OFF`  
**Dependencies:** Requires `ncurses` library on Linux/Cygwin platforms  
**Example:**
```bash
cmake -B ./build -DAREG_EXTENDED=ON
```

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 11. AREG_LOGS

**Description:** Enable logging capabilities in Areg framework.  
**Possible Values:** `ON`, `OFF`  
**Default:** `ON`  
**Example:**
```bash
cmake -B ./build -DAREG_LOGS=OFF
```

**Use case:** Disable logging to reduce overhead in production builds where logging isn't needed.

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 12. AREG_USE_PACKAGES

**Description:** Enable using pre-installed system packages for dependencies.  
**Possible Values:** `ON`, `OFF`  
**Default:** `ON`  
**Example:**
```bash
cmake -B ./build -DAREG_USE_PACKAGES=OFF
```

**Behavior:**
- `ON` - Use system packages if found, otherwise fetch from source
- `OFF` - Always build dependencies from source

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 13. AREG_SQLITE_PACKAGE

**Description:** Use system-installed SQLite3 package.  
**Possible Values:** `ON`, `OFF`  
**Default:** `ON`  
**Example:**
```bash
cmake -B ./build -DAREG_SQLITE_PACKAGE=OFF
```

**Behavior:**
- `ON` - Try system package first, fall back to `thirdparty/` source
- `OFF` - Always use `thirdparty/` source

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 14. AREG_GTEST_PACKAGE

**Description:** Use system-installed Google Test package.  
**Possible Values:** `ON`, `OFF`  
**Default:** `ON`  
**Example:**
```bash
cmake -B ./build -DAREG_GTEST_PACKAGE=OFF
```

**Behavior:**
- `ON` - Try system package first, fetch from Google repository if not found
- `OFF` - Always fetch from Google repository

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 15. AREG_ENABLE_OUTPUTS

**Description:** Enable Areg-specific output directory structure.  
**Possible Values:** `ON`, `OFF`  
**Default:** `ON`  
**Example:**
```bash
cmake -B ./build -DAREG_ENABLE_OUTPUTS=OFF
```

**Behavior:**
- `ON` - Use Areg-specific output paths (organized by compiler/platform)
- `OFF` - Use CMake default output directories

**Use case:** Set to `OFF` when integrating Areg into third-party builds with existing output structure.

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 16. AREG_BUILD_ROOT

**Description:** Main directory for all build artifacts, third-party dependencies, and generated code.  
**Default:** `<areg-sdk>/product`  
**Example:**
```bash
cmake -B ./build -DAREG_BUILD_ROOT=/tmp/areg-build
```

**Note:** This directory and its contents can be safely deleted without affecting source code.

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 17. AREG_OUTPUT_DIR

**Description:** Base directory for all build outputs, including binaries and libraries.  
**Default:** `{AREG_BUILD_ROOT}/build/<compiler>/<os>-<bits>-<cpu>-<build-type>-<linkage>`  
**Example:**
```bash
cmake -B ./build -DAREG_OUTPUT_DIR=/custom/output
```

**Default structure example:**
```
product/build/gnu-g++/linux-64-x86_64-release-shared/
```

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 18. AREG_OUTPUT_BIN

**Description:** Output directory for runtime binaries and shared libraries.  
**Default:** `{AREG_OUTPUT_DIR}/bin`  
**Example:**
```bash
cmake -B ./build -DAREG_OUTPUT_BIN=/custom/bin
```

**Corresponds to:** `CMAKE_RUNTIME_OUTPUT_DIRECTORY` and `CMAKE_LIBRARY_OUTPUT_DIRECTORY`

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 19. AREG_OUTPUT_LIB

**Description:** Output directory for static libraries.  
**Default:** `{AREG_OUTPUT_DIR}/lib`  
**Example:**
```bash
cmake -B ./build -DAREG_OUTPUT_LIB=/custom/lib
```

**Corresponds to:** `CMAKE_ARCHIVE_OUTPUT_DIRECTORY`

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 20. AREG_PACKAGES

**Description:** Directory for fetched third-party packages.  
**Default:** `{CMAKE_BINARY_DIR}/packages`  
**Example:**
```bash
cmake -B ./build -DAREG_PACKAGES=/custom/packages
```

**Contains:** Google Test, SQLite3 (when fetched from source)

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 21. AREG_INSTALL

**Description:** Enable SDK installation process.  
**Possible Values:** `ON`, `OFF`  
**Default:** `ON`  
**Example - Disable installation:**
```bash
cmake -B ./build -DAREG_INSTALL=OFF
```

**Example - Custom location install:**
```bash
cmake -B ./build -DAREG_INSTALL=ON -DAREG_INSTALL_PATH=/opt/areg
cmake --build ./build -j20
sudo cmake --install ./build
```

**Example - Default location install:**
```bash
cmake -B ./build
cmake --build ./build -j20
sudo cmake --install ./build
```

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

### 22. AREG_INSTALL_PATH

**Description:** Directory where Areg SDK will be installed.  
**Default:** `<user-profile>/areg-sdk`  
**Example:**
```bash
cmake -B ./build -DAREG_INSTALL_PATH=/usr/local
cmake --build ./build -j20
sudo cmake --install ./build
```

**Installed components:**
- Headers → `{AREG_INSTALL_PATH}/include/`
- Libraries → `{AREG_INSTALL_PATH}/lib/`
- Executables → `{AREG_INSTALL_PATH}/bin/`
- CMake config → `{AREG_INSTALL_PATH}/lib/cmake/areg/`

<div align="right"><kbd><a href="#complete-options-reference">↑ Back to options ↑</a></kbd></div>

---

## Advanced Configuration

### Print Configuration Status

Use `printAregConfigStatus()` to display current configuration:

```cmake
include(<areg-sdk>/conf/cmake/setup.cmake)
printAregConfigStatus()
```

**Example output:**
```
-- =======================================================================================
-- ----------------------> AREG project CMake Status Report Begin <-----------------------
-- =======================================================================================
-- AREG: >>> CMAKE_SOURCE_DIR    = '/home/user/areg-sdk', build type 'Debug'
-- AREG: >>> Build Environment ..: System 'Linux', 64-bit 'x86_64' platform, Env 'Posix'
-- AREG: >>> Used CXX-Compiler ..: '/usr/bin/clang++'
-- AREG: >>> Used C-Compiler ....: '/usr/bin/clang'
-- AREG: >>> Compiler Version ...: C++ standard 'c++17', compiler family 'llvm'
-- AREG: >>> AREG SDK Root ......: '/home/user/areg-sdk'
-- AREG: >>> CMake Build Dir ....: '/home/user/areg-sdk/build'
-- AREG: >>> Binary Output Dir ..: '/home/user/areg-sdk/product/build/llvm-clang++/linux-64-x86_64-debug-shared/bin'
-- AREG: >>> Build Modules ......: areg = 'shared', aregextend = 'static', areglogger = 'shared'
-- AREG: >>> Java Version .......: '17.0.16', minimum version required = 17
-- AREG: >>> Packages Use .......: SQLite3 package = 'ON', GTest package = 'OFF'
-- AREG: >>> Other Options ......: Examples = 'ON', Unit Tests = 'ON', Extended = 'OFF', Logs = 'ON'
-- AREG: >>> Installation .......: Enabled = 'ON', location = '/usr/local'
-- =======================================================================================
-- -----------------------> AREG project CMake Status Report End <------------------------
-- =======================================================================================
```

### Integration Variables

When integrating Areg SDK into other projects:

**AREG_SDK_ROOT:**
- Set to Areg SDK root directory before including `user.cmake` or `setup.cmake`
- In Areg SDK project, defaults to `CMAKE_CURRENT_LIST_DIR`

**AREG_FRAMEWORK:**
- When using Areg as package (e.g., vcpkg), set to include directory
- Required for header file locations

**Example:**
```cmake
set(AREG_SDK_ROOT "/path/to/areg-sdk")
include(${AREG_SDK_ROOT}/conf/cmake/setup.cmake)
```

### Complete Configuration Example

Build and install with all options explicitly set:

```bash
cmake -B ./build \
  -DAREG_COMPILER_FAMILY=llvm \
  -DAREG_BUILD_TYPE=Release \
  -DAREG_BUILD_TESTS=OFF \
  -DAREG_BUILD_EXAMPLES=OFF \
  -DAREG_BINARY=shared \
  -DAREG_LOGGER_BINARY=shared \
  -DAREG_EXTENDED=OFF \
  -DAREG_LOGS=ON \
  -DAREG_USE_PACKAGES=ON \
  -DAREG_INSTALL=ON \
  -DAREG_INSTALL_PATH=/usr/local

cmake --build ./build -j20

sudo cmake --install ./build
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Integration Examples

### Using Areg SDK in Your Project

**Option 1: System Installation**

After installing Areg SDK system-wide:

```cmake
find_package(areg CONFIG REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

**Option 2: Source Integration**

Include Areg SDK sources directly:

```cmake
set(AREG_SDK_ROOT "/path/to/areg-sdk")
include(${AREG_SDK_ROOT}/conf/cmake/setup.cmake)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

**Option 3: FetchContent**

Fetch Areg SDK automatically:

```cmake
include(FetchContent)
FetchContent_Declare(
    areg
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG        v1.5.0
)
FetchContent_MakeAvailable(areg)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

### Additional Notes

**Root Directory:**
- To integrate Areg SDK in other projects, set `AREG_SDK_ROOT` before including `user.cmake` or `setup.cmake`
- In Areg SDK project, `AREG_SDK_ROOT` defaults to `CMAKE_CURRENT_LIST_DIR` (i.e., `<areg-sdk root>`)

**Integrating as Package:**
- When using Areg via package manager (e.g., vcpkg), set `AREG_SDK_ROOT` to package's `share` directory
- Set `AREG_FRAMEWORK` to `include` directory for headers

**Compiler Configuration:**
- `AREG_COMPILER_FAMILY` automatically selects default compilers (e.g., `llvm` → `clang++` and `clang`)

**Target Processor:**
- `AREG_PROCESSOR` enables cross-compilation
- See [Cross-Compiling Guide](./02b-cmake-build.md#cross-compilation) for details

**Custom Output Directories:**
- `AREG_BUILD_ROOT` sets location for all build artifacts
- Aids in project organization and build cleanup

**Disable Areg Outputs:**
- Set `AREG_ENABLE_OUTPUTS=OFF` to use CMake defaults or custom output locations

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Build Guides:**
- [CMake Build Guide](./02b-cmake-build.md) - Step-by-step build instructions
- [CMake Integration](./02b-cmake-integrate.md) - Using Areg in your projects
- [vcpkg Installation](./02e-cmake-vcpkg.md) - Package manager installation

**Configuration Files:**
- [user.cmake](../../conf/cmake/user.cmake) - Default configuration values
- [setup.cmake](../../conf/cmake/setup.cmake) - Setup utilities and functions

**Examples:**
- [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) - Integration examples
- [Examples Directory](../../examples/README.md) - Sample applications

**Help:**
For questions or issues, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech
