# Integrating AREG Framework with CMake

This guide covers how to integrate the **AREG Framework** into an existing project or create a new project **using CMake** under **Windows** or **Linux** systems. There are three main methods for integration:

1. **Fetch the source code** directly from the **[AREG SDK GitHub repository](https://github.com/aregtech/areg-sdk)**.
2. **Install via `vcpkg`** as a pre-built package.
3. **Add AREG SDK as a Git submodule** to your project.

> [!TIP]
> For a practical example of integrating and cross-compiling AREG SDK libraries and tools, see the **[AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo)** repository.

## Table of Contents
1. [AREG SDK General Requirements](#1-areg-sdk-general-requirements)
2. [General Information](#2-general-information)
3. [Example Code](#3-example-code)
4. [Integration Methods](#4-integration-methods)
   - [Method 1: Integrate by Fetching Sources](#method-1-integrate-by-fetching-sources)
   - [Method 2: Integrate Using `vcpkg` Package](#method-2-integrate-using-vcpkg-package)
   - [Method 3: Integrate as Git Submodule](#method-3-integrate-as-git-submodule)
5. [Advanced Integration](#5-advanced-integration)
   - [Advanced CMake Options](#advanced-cmake-options)
   - [Advanced CMake Functions](#advanced-cmake-functions)
   - [Cross-compiling with CMake](#cross-compiling-with-cmake)
6. [Conclusion](#6-conclusion)

---

## 1. AREG SDK General Requirements

Ensure the following dependencies are installed for AREG SDK integration:

- **CMake** version 3.20 or higher
- **Git** for repository management
- **Supported Compilers**: GCC, LLVM, or MSVC (Windows only) with **C++17** or newer
- **Java** version 17+ for code generation tools

---

## 2. General Information

The AREG SDK includes several key components:

1. **AREG Framework (`areg` library)**: The core library for automation and Object RPC communication.
2. **AREG Framework Extension (`aregextend` library)**: An optional library with additional utilities.
3. **AREG Log Observer API (`areglogger` library)**: Allows applications to receive and manage log messages.
4. **AREG Multicast Router (`mcrouter` executable)**: An OS-managed service that routes RPC messages.
5. **AREG Log Collector (`logcollector` executable)**: An OS-managed service for remote log collection.
6. **AREG Log Observer (`logobserver` executable)**: A stand-alone application for managing logs.
7. **AREG Code Generator (`codegen.jar` runnable)**: A Java tool for generating Service Interface source code.

These components provide a robust foundation for integrating the AREG SDK, streamlining the development process.

---

## 3. Example Code

Use the following code for integration examples. Create a `main.cpp` file in a separate `example` directory:

```cpp
#include "areg/base/String.hpp"

int main() {
    String str("Hello from AREG SDK!");
    std::cout << str.getData() << std::endl;
    return 0;
}
```

---

## 4. Integration Methods

### Method 1: Integrate by Fetching Sources

1. **Declare the Project**:
   Create a `CMakeLists.txt` file, ensuring the minimum CMake version is 3.20:

   ```cmake
   cmake_minimum_required(VERSION 3.20.0)
   project(example)
   ```

2. **(Optional) Disable Examples and Tests**:
   Speed up builds by disabling examples, tests, and specific output directories:

   ```cmake
   option(AREG_BUILD_EXAMPLES "Disable examples" OFF)   # Disable build of examples
   option(AREG_BUILD_TESTS "Disable tests" OFF)         # Disable build or unit tests
   option(AREG_ENABLE_OUTPUTS "Disable outputs" OFF)    # Disable AREG SDK specific output directories
   ```

3. **Fetch AREG SDK Sources**:
   Add this script to `CMakeLists.txt` to fetch the sources from the `master` branch:

   ```cmake
   include(FetchContent)
   FetchContent_Declare(
       areg-sdk
       GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
       GIT_TAG "master"
   )
   FetchContent_MakeAvailable(areg-sdk)
   set(AREG_SDK_ROOT "${areg-sdk_SOURCE_DIR}")
   include_directories("${AREG_SDK_ROOT}/framework")
   ```

4. **Build the Project**:
   Define the executable sources, link the `areg` library, and build:

   ```cmake
   add_executable(example main.cpp)
   target_link_libraries(example PRIVATE areg::areg)
   ```

At the end, your `CMakeLists.txt` file should loook like this:
```cmake

# Step 1: Declare the project
cmake_minimum_required(VERSION 3.20.0)
project(example)

# Step 2: (Optional) Disable Examples and Tests
option(AREG_BUILD_EXAMPLES "Disable examples" OFF)   # Disable build of examples
option(AREG_BUILD_TESTS "Disable tests" OFF)         # Disable build or unit tests
option(AREG_ENABLE_OUTPUTS "Disable outputs" OFF)    # Disable AREG SDK specific output directories

# Step 3: Fetch AREG SDK Sources
include(FetchContent)
FetchContent_Declare(
    areg-sdk
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG "master"
)
FetchContent_MakeAvailable(areg-sdk)
set(AREG_SDK_ROOT "${areg-sdk_SOURCE_DIR}")
include_directories("${AREG_SDK_ROOT}/framework")

# Step 4: Build the Project
add_executable (example main.cpp)
target_link_libraries(example PRIVATE areg::areg)
```

5. **Build Commands**:
   Run these commands in *Terminal* or *PowerShell* to configure, build, and run the `example` console application:

   ```bash
   cmake -B ./build
   cmake --build ./build
   ./build/example
   ```

### Method 2: Integrate Using `vcpkg` Package

> [!IMPORTANT]
> Starting with AREG SDK 2.0, integration via `vcpkg` is available.

1. **Install `vcpkg`**:
   Follow instructions on the [vcpkg GitHub page](https://github.com/microsoft/vcpkg). Make sure to update `vcpkg` for the latest package data.

2. **Install AREG SDK Package**:
   Use these commands:
     ```bash
     vcpkg install areg
     ```
   Also you can install by running commands with *triplets* like `vcpkg install areg:x64-linux` for 64-bit Linux or `vcpkg install areg:x86-windows` for 32-bit Windows. 

3. **Retrieve Toolchain File Path**:
   Run the following command to integrate `vcpkg` binaries with your system:
   ```bash
   vcpkg integrate install
   ```
   This command displays the full path to set `CMAKE_TOOLCHAIN_FILE`, which is required to build projects with `vcpkg` packages. Copy this toolchain file path for later use during the build configuration process.

4. **Integrate in CMake Script**:
   Create a `CMakeLists.txt` file with the following content:

   ```cmake
   cmake_minimum_required(VERSION 3.20.0)
   project(example)
   find_package(areg CONFIG REQUIRED)
   add_executable(example main.cpp)
   target_link_libraries(example PRIVATE areg::areg)
   ```

5. **Build with Toolchain File**:
   Replace `<path-to-vcpkg-root>` with the actual path, then run these command to configure, build and execute `example` application:

   ```bash
   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=<path-to-vcpkg-root>/scripts/buildsystems/vcpkg.cmake
   cmake --build ./build
   ./build/example
   ```

Also see [Installing and Using AREG SDK with vcpkg Package Manager](./01a-areg-package.md) for more details.

### Method 3: Integrate as Git Submodule

1. **Define Submodule**:
   Add `areg-sdk` as a submodule by creating a `.gitmodules` file:
   ```plaintext
   [submodule "areg-sdk"]
     path = areg-sdk
     url = https://github.com/aregtech/areg-sdk.git
   ```

2. **Get AREG SDK Sources**:
   Run these commands:

   ```bash
   git submodule update --init --recursive
   git submodule update --remote --recursive
   ```

3. **Integrate in CMake Script**:
   Create a `CMakeLists.txt` file to build all projects and use AREG Framework:

   ```cmake
   cmake_minimum_required(VERSION 3.20.0)
   project(example)
   set(AREG_SDK_ROOT "${CMAKE_SOURCE_DIR}/areg-sdk")
   include("${AREG_SDK_ROOT}/CMakeLists.txt")
   add_executable(example main.cpp)
   target_link_libraries(example PRIVATE areg::areg)
   ```

4. **Build Commands**:
   Run these commands to configure, build and execute `example` application:

   ```bash
   cmake -B ./build
   cmake --build ./build
   ./build/example
   ```

---

## 5. Advanced Integration

> [!IMPORTANT]
> To utilize AREG SDK-specific CMake variables such as `AREG_COMPILER`, `AREG_COMPILER_FAMILY`, or `AREG_BUILD_TYPE`, ensure that `<areg-sdk>/areg.cmake` is included in your project's `CMakeLists.txt` file *before* the first invocation of `project()`. Otherwise, the values of these variables are ignored and overwritten by values of CMake variables `CMAKE_CXX_COMPILER` and `CMAKE_BUILD_TYPE`. Alternatively, you can directly use the `CMAKE_XXX` variables to specify the compiler or build type.

### Advanced CMake Options

The AREG SDK offers flexible configurations via CMake, allowing customization of compilation settings, output directories, and additional features. For the full list, refer to the [CMake Configuration Options for Building AREG SDK](./02a-cmake-config.md) document.

> [!IMPORTANT]
> Set options before including the `<areg-sdk>/areg.cmake` file.

For example:

```cmake
option(AREG_LOGS "Disable logs" OFF)
option(AREG_EXTENDED "Build extended" ON)
include(${AREG_SDK_ROOT}/areg.cmake)
```

### Advanced CMake Functions

The `<areg-sdk>/areg.cmake` file includes `functions.cmake`, simplifying build commands:

```cmake
macro_declare_executable(example main.cpp)
```

For example, generating source files from `CoolServices.siml` and compiling a static library `coolservice` to link with `cool` application:

```cmake
addServiceInterface(coolservice "service/CoolService.siml")
macro_declare_executable(cool coolservice main.cpp)
```

See [AREG SDK CMake Functions and Macros](./02b-cmake-functions.md) for details.

### Cross-Compiling with CMake

To cross-compile sources with AREG SDK, you can use CMake toolchain files. These files define CMake-specific values that influence AREG SDK's compile settings. For instance, specifying `CMAKE_CXX_COMPILER` or `CMAKE_CXX_COMPILER_TARGET` will automatically set the corresponding `AREG_COMPILER` and `AREG_TARGET` values.

You can find example toolchain files in the [`toolchains`](../../conf/exports/example/toolchains/) directory of in the [`areg-sdk-demo`](https://github.com/aregtech/areg-sdk-demo) repository. An example of using toolchain file:
```bash
cmake -B ./product/cache/gnu-linux-arm32 -DCMAKE_TOOLCHAIN_FILE=/full/path/to/gnu-linux-arm32.cmake
cmake --build ./product/cache/gnu-linux-arm32 -j20
```

---

## 6. Conclusion

A complete `CMakeLists.txt` might look like this:

```cmake
cmake_minimum_required(VERSION 3.20.0)
project(example)

find_package(areg CONFIG)
if (NOT areg_FOUND)
    option(AREG_BUILD_EXAMPLES "Disable examples" OFF)
    option(AREG_BUILD_TESTS "Disable tests" OFF)
    set(AREG_BUILD_ROOT "${CMAKE_SOURCE_DIR}/product")
    include(FetchContent)
    FetchContent_Declare(
        areg-sdk
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master"
    )
    FetchContent_MakeAvailable(areg-sdk)

    set(AREG_SDK_ROOT "${areg-sdk_SOURCE_DIR}")
    include_directories("${AREG_SDK_ROOT}/framework")
endif()

include("${AREG_SDK_ROOT}/areg.cmake")
macro_declare_executable(example main.cpp)
```

This file lets you set up an `example` project with AREG SDK and link necessary libraries, accommodating the different integration methods:
- try to find `areg` package
- if not available, fetch and build sources
- include `areg.cmake` and declare `example` executable.

> [!NOTE]
> In this script, there is no need to link the `example` project with the `areg` library. It is automatically done when call `macro_declare_executable`. 

If you don't want to use AREG SDK CMake macro and functions, alternatively the last 2 lines can be replaced:
```cmake
add_executable(example main.cpp)
target_link_libraries(example PRIVATE areg::areg)
```
