# Integrating AREG Framework with CMake

This guide details how to integrate the AREG Framework into an existing project or to build a new project using AREG Framework. There are three primary methods for integration:

1. **Fetch the source code** directly from the **[AREG SDK GitHub repository](https://github.com/aregtech/areg-sdk)**;
2. **Install via `vcpkg`** as a prebuilt package;
3. **Add AREG SDK as a Git submodule** to your project.

> [!TIP]:
> For a practical example of integrating AREG SDK libraries and tools, refer to the **[AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo)** repository.

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

Ensure the following dependencies are installed to integrate AREG SDK sources or binaries into your project:

- **CMake** version 3.20 or higher
- **Git** for repository management
- **Compatible Compilers**: GCC, LLVM, or MSVC (Windows only) supporting **C++17** or newer
- **Java** version 17+ for code generation tools

---

## General Information

AREG SDK comprises several components:

1. **AREG Framework (`areg` library)**: Core library for automations and Object RPC communication.
2. **AREG Framework Extension (`aregextend` library)**: An optional library with extended utilities and features.
3. **AREG Log Observer API (`areglogger` library)**: Enables applications to receive and manage log messages.
4. **AREG Multicast Router (`mcrouter` executable)**: A console and OS-managed service to route RPC messages.
5. **AREG Log Collector (`logger` executable)**: A console and OS-managed service to remotely collect logs.
6. **AREG Log Observer (`logobserver` executable)**: Stand-alone console application for managing logs.
7. **AREG Code Generator (`codegen.jar` runnable)**: A Java-based tool to generate Service Interface source code.

These components provide a robust infrastructure for integrating AREG SDK in projects, ensuring streamlined development.

---

## Example Code

Use the following code for all integration examples. Create a `main.cpp` file in a separate directory:

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
   Create a `CMakeLists.txt` file with the following content, ensuring the minimum CMake version is set to 3.20:
   ```cmake
   cmake_minimum_required(VERSION 3.20.0)
   project(example)
   ```

2. **(Optional) Disable Examples and Tests**:
   To speed up builds, disable examples and tests by adding these options, and additionally disable AREG SDK specific output directories:
   ```cmake
   option(AREG_BUILD_EXAMPLES "Disable examples" OFF)   # Disable build of examples
   option(AREG_BUILD_TESTS "Disable tests" OFF)         # Disable build or unit tests
   option(AREG_ENABLE_OUTPUTS "Disable outputs" OFF)    # Disable AREG SDK specific output directories
   ```

3. **Fetch AREG SDK Sources**:
   Add this script to `CMakeLists.txt` to fetch the sources from `master` branch:
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
   Define executable's sources and link `areg` library and build:
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
   Run these commands in *Terminal* or *PowerShell* to configure, build and run `example` console application:
   ```bash
   cmake -B ./build
   cmake --build ./build
   ./build/example
   ```
 
### Integrate Using `vcpkg` Package

> [!IMPORTANT]
> Starting with AREG SDK 2.0, you can integrate it using the `vcpkg` package manager. Before that, the AREG SDK is not available in `vcpkg`.

1. **Install `vcpkg`**:
   Install `vcpkg` and ensure it’s updated. Follow instructions at the [vcpkg GitHub page](https://github.com/microsoft/vcpkg). If you already have `vcpkg` installed on your machine, make sure that you have latest data (*pull the data*).

2. **Install AREG SDK Package**:
   Use the following commands:
   - **Windows (64-bit)**:
     ```bash
     vcpkg install areg:x64-windows
     ```
   - **Linux (64-bit)**:
     ```bash
     vcpkg install areg:x64-linux
     ```
3. **Get Toolchain File Path**:
   Use the following command to integrate AREG SDK binaries and get the path of toolchain.
   ```bash
   vcpkg integrate install
   ```
   This will display a message with information to include toolchain file in your build `-DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake` where <vcpkg-root> is the path of `vcpkg` packet management directory.

3. **Integrate in CMake script**:
   Create a `CMakeLists.txt` file with the following content, ensuring that CMake will either find the `areg` package or the build will fail:
   ```cmake
   cmake_minimum_required(VERSION 3.20.0)
   project(example)
   find_package(areg CONFIG REQUIRED)       # Find package with option 'REQUIRED'
   include_directories(${AREG_FRAMEWORK})
   add_executable(example main.cpp)
   target_link_libraries(example PRIVATE areg::areg)
   ```

4. **Build with Toolchain File**:
   Run these commands in *Terminal* or *PowerShell* to configure, build and run `example` console application. To build, **replace** `<vcpkg-root>` with the actual path:
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

3. **Integrate in CMake script**:
   Create a `CMakeLists.txt` file with the following content to build all projects and use AREG Framework:
   ```cmake
   cmake_minimum_required(VERSION 3.20.0)
   project(example)
   set(AREG_SDK_ROOT "${CMAKE_SOURCE_DIR}/areg-sdk")
   include("${AREG_SDK_ROOT}/CMakeLists.txt")
   add_executable(example main.cpp)
   target_link_libraries(example PRIVATE areg::areg)
   ```

4. **Build Commands**:
   Run these commands in *Terminal* or *PowerShell* to configure, build and run `example` console application:
   ```bash
   cmake -B ./build
   cmake --build ./build
   ./build/example
   ```

In all cases, your should be able to run `./build/example` executable, which will output `Hello from AREG SDK!` message on console.

---

## Advanced Integration

### Advanced CMake Options

AREG SDK has

The AREG SDK offers flexible build configurations via CMake, allowing users to customize compilation settings, output directories, library types, and additional features for tailored development environments. These options can be adjusted to optimize the AREG SDK binaries for different systems and to control features like testing, logging, and package usage. For the complete list of CMake options, refer to the [CMake Configuration Options for Building AREG SDK](./cmake-config.md) document.

> [!IMPORTANT]
> Set the settings before including `<areg-sdk>/areg.cmake` file in CMakeLists.txt file or before fetching the sources.

For example, if the size of binary is actual and you want to compile your projects to use AREG Framework advanced features, set these options before `areg.cmake` is included
```cmake
option(AREG_LOGS "Disable logs" OFF)
option(AREG_EXTENDED "Build extended" ON)
include(${AREG_SDK_ROOT}/areg.cmake)
```

### Advanced CMake Functions
The `<areg-sdk>/areg.cmake` file refers to the `functions.cmake`, which contains set of CMake functions and macro to simplify build commands:
```cmake
macro_declare_executable(example main.cpp)
```

The following script triggers code generator to generate C++ source files from the Service Interface document `CoolServices.siml`. The sources are compiled in a static library `coolservice` to use by other projects:
```cmake
addServiceInterface(coolservice "" CoolService)
macro_declare_executable(cool coolservice main.cpp)
```

See [AREG SDK CMake Functions and Macros](./cmake-functions.md) document for the list of reusable CMake functions and macro.

---

## Conclusion

For integrating AREG SDK, a comprehensive `CMakeLists.txt` might look like this:
```cmake
# Step 1: Declare the project
cmake_minimum_required(VERSION 3.20.0)
project(example)

# Step 2: Find 'areg' package. If not found, fetch sources.
find_package(areg CONFIG)
if (NOT areg_FOUND)

    # Step 3 (optional): Disable examples and tests, set binary directory
    option(AREG_BUILD_EXAMPLES "Disable examples" OFF)  # Disable build of examples
    option(AREG_BUILD_TESTS "Disable tests" OFF)        # Disable build or unit tests
    set(AREG_BUILD_ROOT "${CMAKE_SOURCE_DIR}/product")  # Set the binary build output directory

    # Step 4: Fetch AREG SDK sources from master branch
    include(FetchContent)
    FetchContent_Declare(
        areg-sdk
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master"
        )
    FetchContent_MakeAvailable(areg-sdk)
    set(AREG_SDK_ROOT "${areg-sdk_SOURCE_DIR}")
endif()

# Step 5: include 'areg.cmake' in configuration process
include("${AREG_SDK_ROOT}/areg.cmake")

# Step 6: Use macro to build the project
macro_declare_executable(example main.cpp)
```

> [!NOTE]
> In this example, there is no need to link the project with `areg` library, because it is automatically done in the macro.

In this example:
- we try to find `areg` package
- if not available, fetch and build sources
- include `areg.cmake` file to benefit predefined CMake macro and functions, and declare `example` executable.
