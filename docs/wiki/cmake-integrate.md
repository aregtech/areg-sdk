# Integrate AREG Framework with CMake

This document describes how to integrate AREG Framework into the existing project or how to build a new project using AREG Framework. The document describes 2 methods to integrate:
1. Integrate by fetching the sources from **[AREG SDK](https://github.com/aregtech/areg-sdk)** repository;
2. Integrate by using installed package with `vcpkg` tool.
3. Integrate by creating `areg-sdk` submodule.

> [!TIP]
> For the practical working example of integrating AREG SDK libraries and tools, see the existing **[AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo)**.

## Table of Contents


## AREG SDK General Requirements

To be able to integrate AREG SDK sources or binaries in the project, ensure your system includes following:
- **CMake** (version 3.20+)
- **Git** for repository cloning
- **Compatible Compilers**: *GNU*, *LLVM*, or *MSVC* (Windows only) supporting **C++17** or newer
- **Java** (version 17+ for code generation tools)

## General Information

AREG SDK consists of several development projects:
1. AREG Framework (`areg` library): the communication engine based on Object RPC.
2. AREG Framework extension (`aregextend` library): An optional library with objects with extended utilities and features, which may cause additional dependencies.
3. AREG Lob Observer API library (`areglogger` library): A library that log observer applications can use to receive log messages.
4. AREG Multicast Router (`mcrouter` executable): A stand-alone application that runs as console application or OS-managed service to route RPC messages.
5. AREG Log Collector (`logger` executable): A stand-alone application that runs as console application or OS-managed service to collect logs remotely.
6. AREG Log Observer (`logobserver` executable): A stand-alone application to manage and store logs.
7. AREG Code Generator (`codegen.jar` runnable): A stand-alone Java-based tool to generate Service Interface source codes.

To build a projects or use tools, developers need to integrate AREG SDK in their projects. AREG SDK contains several utilities to help developers to integrate AREG SDK libraries and tools in the project with minimum efforts. Here in this document there is detailed description of integration.

## Example Code

In this document, during entire example, we are going to use following `main.cpp` source code which you should create in a separate directory:
```cpp
#include "areg/base/String.hpp"

int main(void)
{
    String str("Hello from AREG SDK!");
    std::cout << str.getData() << std::endl;
    return 0;
}
```

The next are examples to integrate AREG SDK libraries in your project.

## Integrate by fetching

Developers can integrate AREG SDK libraries in the project by fetching the sources and including them in the build process. To do so, follow these steps:

### Quick Setup

**_Step 1_: Declare project**:

In the same location of `main.cpp` create `CMakeLists.txt` file with initial content. Pay attention that the CMake minimum required version should be `3.20`:
```cmake
cmake_minimum_required(VERSION 3.20.0)
project(example)
```

**_Step 2_ (Optional): Disable examples and tests**:

To disable AREG SDK specific output paths, to disable building AREG SDK **examples** and **unit tests** to save build time, set following variables before fetching sources:
```cmake
option(AREG_ENABLE_OUTPUTS  "Disable outputs"  OFF) # No AREG SDK build outputs
option(AREG_BUILD_EXAMPLES  "Disable examples" OFF) # No examples build
option(AREG_BUILD_TESTS     "Disable tests"    OFF) # No unit tests build
```

These are optional settings. If you want to build all projects of AREG SDK and use specific output paths, ignore *Step 1*.


**_Step 3_: Fetch AREG SDK sources**:

Change your `CMakeLists.txt` and add following script:
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

**_Step 4_: Make a build of project**:

Now you are ready to use AREG SDK libraries. The following is an example of building `example` binary and linking with `areg` library:
```cmake
add_executable (example main.cpp)
target_link_libraries(example PRIVATE areg::areg)
```

In total, the content of `CMakeLists.txt` should be following:
```cmake

# Step 1: Declare project
cmake_minimum_required(VERSION 3.20.0)
project(example)

# Step 2 (optional): Disable examples and tests, disable change binary directory
option(AREG_ENABLE_OUTPUTS  "Disable outputs"  OFF) # No AREG SDK build outputs
option(AREG_BUILD_EXAMPLES  "Disable examples" OFF) # No examples build
option(AREG_BUILD_TESTS     "Disable tests"    OFF) # No unit tests build

# Step 3: Fetch AREG SDK sources
include(FetchContent)
FetchContent_Declare(
    areg-sdk
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG "master"
)
FetchContent_MakeAvailable(areg-sdk)
set(AREG_SDK_ROOT "${areg-sdk_SOURCE_DIR}")
include_directories("${AREG_SDK_ROOT}/framework")

# Step 4: Make a build of project
add_executable (example main.cpp)
target_link_libraries(example PRIVATE areg::areg)
```

### Quick project build

Now you are ready to make a build. Open a *Terminal* or *PowerShell* (for Windows) to make a build.

The following commands will fetch AREG SDK source, configure `example` project:
```bash
cmake -B ./build
```

The following builds `example` project:
```bash
cmake --build ./build
```

When build is completed, you can run `example` executable:
```bash
./build/example
```

And this run will make output:
```plaintext
Hello from AREG SDK!
```

## Integrate `areg` package

### Setup `vcpkg` on system

If you don't want to compile the AREG SDK sources and want to use binaries instead, you can use `vcpkg` tool to install AREG SDK binaries and dependencies in your system. Here there is a step-by-step explanation to do that.

> [!IMPORTANT]
> Starting with AREG SDK 2.0, you can integrate it using the `vcpkg` package manager. Before that the AREG SDK will not be available in `vcpkg`.

**_Step 1_: Install `vcpkg`**:

If you already have installed `vcpkg` package management tool on your system, you can skip this step, but make sure that you have latest content from `vcpkg` repository. If you don't have installed `vcpkg` package manager tool, clone, build and install `vcpkg` follow the instructions in the [official vcpkg repository](https://github.com/microsoft/vcpkg).

**_Step 2_: Install AREG SDK package**:

After installing `vcpkg` in your system, you should install AREG SDK binaries to make it available for the builds. Install the AREG SDK package using the following commands:

**Windows (64-bit):**
```bash
vcpkg install areg:x64-windows
```

**Linux (64-bit):**
```bash
vcpkg install areg:x64-linux
```

After installing the package, add the `vcpkg` toolchain to your project displayed after running this command. Copy the toolchain path to use in your `CMakeLists.txt`:
```bash
vcpkg integrate install
```

This command will display a path like this `-DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake`, where `<vcpkg-root>` indicates the `vcpkg` package manager directory path.

### Integrate `areg` package in your build

**_Step 3_: Integrate in CMake build**:

Now create or open the `CMakeLists.txt` file located with `main.cpp` file and type following:
```cmake
# Should be not less than 3.20
cmake_minimum_required(VERSION 3.20.0)
project(example)

# This will search the 'areg' package or the configuration will fail
find_package(areg CONFIG REQUIRED)
include_directories(${AREG_FRAMEWORK})

# Ready to build and link
add_executable (example main.cpp)
target_link_libraries(example PRIVATE areg::areg)
```

Now you are ready to compile and run `example` executable. Open *Terminal* or PowerShell in the project directory and run command (don't forget to replace the `<vcpkg-root>` with the real path):
```bash
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
cmake --build ./build
```

When your project is compiled, you can run `example`:
```bash
./build/example
```

The result is:
```plaintext
Hello from AREG SDK!
```

## Integrate `areg-sdk` submodule

### Define submodule

Another method is to integrate `areg-sdk` as a submodule. For this, first of all your `example` project should be available on the Git. Then, you need to define `.gitmodules` file and declare this submodule:
```plaintext
[submodule "areg-sdk"]
  path = areg-sdk
  url = https://github.com/aregtech/areg-sdk.git
```

Open a *Terminal* or *PowerShell* in your project and run these commands:
```bash
git submodule update --init --recursive
git submodule update --remote --recursive
```

This creates a `areg-sdk` submodule in the root directory of your `example` project.

### Include build

In the root directory of `example` create or modify `CMakeLists.txt` file to have following:
```cmake
# Step 1: Set CMkae version 3.20+
cmake_minimum_required(VERSION 3.20.0)
project(example)

# Step 2 (optional): Disable examples and tests, disable change output directory
option(AREG_ENABLE_OUTPUTS  "Disable outputs"  OFF) # No AREG SDK build outputs
option(AREG_BUILD_EXAMPLES  "Disable examples" OFF) # No examples build
option(AREG_BUILD_TESTS     "Disable tests"    OFF) # No unit tests build

# Step 3: Include AREG SDK sources in the build
set(AREG_SDK_ROOT "${CMAKE_SOURCE_DIR}/areg-sdk")
include("${AREG_SDK_ROOT}/CMakeLists.txt")

# Step 4: Compile and build your project
add_executable (example main.cpp)
target_link_libraries(example PRIVATE areg::areg)
```

Now your project is ready to be compiled:
```bash
cmake -B ./build
cmake --build ./build
```

When your project is compiled, you can run `example`:
```bash
./build/example
```

The result is:
```plaintext
Hello from AREG SDK!
```

## Advanced Integration

### Advanced CMake Options

In case if you want to use the advantages of AREG SDK settings and CMake functions, you can integrate and use advanced settings in your project by including `areg.cmake`, located in the `<areg-sdk>` root, to your `CMakeLists.txt` build configuration file. Change AREG SDK settings before including `areg.cmake`. For example, if you don't want to compiler the sources with logs or want to compile `aregextend` library with advanced features, you should do following:
```cmake
option(AREG_LOGS "Disable logs" OFF)
option(AREG_EXTENDED "Build extended" ON)
include(${AREG_SDK_ROOT}/areg.cmake)
```

See the list of AREG SDK settings in the [CMake Configuration Options for Building AREG SDK](./cmake-config.md) document.

### Advanced CMake Functions

When include `areg.cmake` in the project, it automatically includes the `functions.cmake` making AREG SDK CMake functions and macro available in the projects. The `functions.cmake` simplifies building projects based on `areg` library. For example, there is no need anymore to link the target with `areg` library or think about importing functions. The previous example of `CMakeLists.txt` when adding an executable and linking with `areg` library, could be done in one line:
```cmake
macro_declare_executable(example main.cpp)
```

Additionally, it contains a function to generate Service Interface files and including them in the library. For example, if a project would have a Service Interface file named `CoolServices.siml`, it could be used in following way:
```cmake
# Step 1: Generate codes from 'CoolService' service interface document and create 'coolservice' static library
addServiceInterface(coolservice "" CoolService)
# Create 'cool' executable by compiling 'main.cpp' and linking with 'coolservice' static library
macro_declare_executable(cool coolservice main.cpp)
```
In this example, on *"Step 1"* we generate files from Service Interface document and automatically include them in the build to compile a static library. On *"Step 2"*, we declare an executable, which is compiled with project sources and linked with the static library of Service Interface.

For the complete list of available AREG SDK functions and macro, see [AREG SDK CMake Functions and Macros](./cmake-functions.md) Wiki document.

## Conclusion

We tried to keep simple to integrate AREG SDK libraries and headers in other projects. Considering the possibility to use CMake options, macro and functions, also considering that on some machines developers may have installed package managing tool like `vcpkg` and on the others machines it could be not installed, the final version of `CMakeLists.txt` to build out example project should be following.
```
# Step 1: Declare project
cmake_minimum_required(VERSION 3.20.0)
project(example)

# Step 2: Search 'areg' package. If found, ready to use. Otherwise, fetch sources
find_package(areg CONFIG)
if (NOT areg_FOUND)

    # Step 3 (optional): Disable examples and tests, set binary directory
    option(AREG_BUILD_EXAMPLES  "Disable examples" OFF) # No examples build
    option(AREG_BUILD_TESTS     "Disable tests"    OFF) # No unit tests build
    set(AREG_BUILD_ROOT "${CMAKE_SOURCE_DIR}/product")  # Set the binary build directory

    # Step 4: Fetch AREG SDK sources
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

# Step 6: Use defined macro to build a project
macro_declare_executable(example main.cpp)
```
