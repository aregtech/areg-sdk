Here’s a refined version with improved grammar, flow, and added clarity where helpful:

---

# Integrating AREG Framework with CMake

This guide covers how to integrate the AREG Framework into an existing project or create a new project using AREG Framework. There are three main methods for integration:

1. **Fetch the source code** directly from the **[AREG SDK GitHub repository](https://github.com/aregtech/areg-sdk)**.
2. **Install via `vcpkg`** as a prebuilt package.
3. **Add AREG SDK as a Git submodule** to your project.

> [!TIP]  
> For a practical example of integrating AREG SDK libraries and tools, see the **[AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo)** repository.

## Table of Contents
- [AREG SDK General Requirements](#areg-sdk-general-requirements)
- [General Information](#general-information)
- [Example Code](#example-code)
- [Integration Methods](#integration-methods)
  - [Integrate by Fetching Sources](#integrate-by-fetching-sources)
  - [Integrate Using `vcpkg` Package](#integrate-using-vcpkg-package)
  - [Integrate as Git Submodule](#integrate-as-git-submodule)
- [Advanced Integration](#advanced-integration)
  - [Advanced CMake Options](#advanced-cmake-options)
  - [Advanced CMake Functions](#advanced-cmake-functions)
- [Conclusion](#conclusion)

---

## AREG SDK General Requirements

Ensure the following dependencies are installed for AREG SDK integration:

- **CMake** version 3.20 or higher
- **Git** for repository management
- **Supported Compilers**: GCC, LLVM, or MSVC (Windows only) with **C++17** or newer
- **Java** version 17+ for code generation tools

---

## General Information

AREG SDK consists of several components:

1. **AREG Framework (`areg` library)**: Core library for automation and Object RPC communication.
2. **AREG Framework Extension (`aregextend` library)**: Optional library with extended utilities.
3. **AREG Log Observer API (`areglogger` library)**: Enables applications to receive and manage log messages.
4. **AREG Multicast Router (`mcrouter` executable)**: OS-managed service to route RPC messages.
5. **AREG Log Collector (`logger` executable)**: OS-managed service for remote log collection.
6. **AREG Log Observer (`logobserver` executable)**: Stand-alone application for managing logs.
7. **AREG Code Generator (`codegen.jar` runnable)**: Java tool for generating Service Interface source code.

These components provide a robust infrastructure for integrating AREG SDK in projects, ensuring streamlined development.

---

## Example Code

Use the following code for integration examples. Create a `main.cpp` file in a separate directory:

```cpp
#include "areg/base/String.hpp"

int main() {
    String str("Hello from AREG SDK!");
    std::cout << str.getData() << std::endl;
    return 0;
}
```

---

## Integration Methods

### Integrate by Fetching Sources

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

At the end, your `CMakeLists.txt`  file should loook like this:
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

### Integrate Using `vcpkg` Package

> [!IMPORTANT]  
> Starting with AREG SDK 2.0, integration via `vcpkg` is available.

1. **Install `vcpkg`**:  
   Follow instructions on the [vcpkg GitHub page](https://github.com/microsoft/vcpkg). Make sure to update `vcpkg` for the latest package data.

2. **Install AREG SDK Package**:  
   Use these commands:

   - **Windows (64-bit)**:
     ```bash
     vcpkg install areg:x64-windows
     ```
   - **Linux (64-bit)**:
     ```bash
     vcpkg install areg:x64-linux
     ```

3. **Get Toolchain File Path**:  
   Use the following to integrate AREG SDK binaries:

   ```bash
   vcpkg integrate install
   ```
 
This also displays the CMake option to use toolchain when build `areg` package. Make sure you copied the command with path to the toolchain.

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
   Replace `<vcpkg-root>` with the actual path, then run these command to configure, build and execute `example` application:

   ```bash
   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
   cmake --build ./build
   ./build/example
   ```

### Integrate as Git Submodule

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

## Advanced Integration

### Advanced CMake Options

The AREG SDK offers flexible configurations via CMake, allowing customization of compilation settings, output directories, and additional features. For the full list, refer to the [CMake Configuration Options for Building AREG SDK](./cmake-config.md) document.

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
addServiceInterface(coolservice "" CoolService)
macro_declare_executable(cool coolservice main.cpp)
```

See [AREG SDK CMake Functions and Macros](./cmake-functions.md) for details.

---

## Conclusion

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

If you don't want to use AREG SDK CMake macro and function, alternatively the last 2 lines can be replaced:
```cmake
add_executable(example main.cpp)
target_link_libraries(example PRIVATE areg::areg)
```
