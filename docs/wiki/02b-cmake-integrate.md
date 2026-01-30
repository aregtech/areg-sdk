# Integrating Areg Framework with CMake

This guide covers integrating Areg Framework into your project using CMake on Windows and Linux systems.

> [!TIP]
> For practical integration and cross-compilation examples, see the [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) repository.

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Prerequisites](#prerequisites)
3. [Integration Methods](#integration-methods)
4. [Advanced Integration](#advanced-integration)
5. [Troubleshooting](#troubleshooting)

---

## Quick Start

### Fastest Integration Method

**FetchContent (recommended for new projects):**

Create `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Fetch Areg SDK
include(FetchContent)
FetchContent_Declare(areg-sdk
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG "master")
FetchContent_MakeAvailable(areg-sdk)

# Create executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

Create `main.cpp`:
```cpp
#include "areg/base/String.hpp"
#include <iostream>

int main() {
    String greeting("Hello from Areg SDK!");
    std::cout << greeting.getData() << std::endl;
    return 0;
}
```

**Build and run:**
```bash
cmake -B ./build
cmake --build ./build
./build/myapp
```

**Expected output:**
```
Hello from Areg SDK!
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Prerequisites

### Required Tools

- **CMake 3.20+** - Build system generator
- **Git** - Version control
- **C++17 Compiler** - GCC, Clang/LLVM, or MSVC
- **Java 17+** - For code generation tools

### Supported Platforms

- **Linux** - GCC, Clang
- **Windows** - MSVC, MinGW, Clang
- **macOS** - Clang, GCC

**Verify installation:**

```bash
cmake --version    # Should be 3.20+
git --version      # Any recent version
java -version      # Should be 17+
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Integration Methods

Three methods to integrate Areg SDK:

| Method                                             | Best For          | Complexity   | Build Time            |
|----------------------------------------------------|-------------------|--------------|-----------------------|
| [Project Setup Tool](./02a-quick-project-setup.md) | New projects      | Very Low     | 30 sec                |
| [FetchContent](#method-1-fetchcontent)             | New projects      | Low          | First build: 3-8 min  |
| [vcpkg](#method-2-vcpkg-package)                   | Existing projects | Medium       | Pre-built, instant    |
| [Git Submodule](#method-3-git-submodule)           | Source control    | Medium       | First build: 3-8 min  |


### Method 1: Project Setup Tool (Fastest - 30 seconds)


```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
./tools/project-setup.sh
# Follow prompts, then:
cd areg_hello
cmake -B ./build && cmake --build ./build
./build/areg_hello
```

**See:** [Quick Project Setup Guide](./wiki/02a-quick-project-setup.md)

### Option 2: Manual Integration

See [CMake Integration Guide](./wiki/02b-cmake-integrate.md)

---

### Method 2: FetchContent

**Advantages:**
- ✅ Simple CMake-native approach
- ✅ Automatic source fetching
- ✅ No external dependencies
- ✅ Version control via Git tag

**Disadvantages:**
- ❌ Downloads sources every clean build
- ❌ Slower first build

#### Step 1: Create Project Structure

```bash
mkdir myproject
cd myproject
touch CMakeLists.txt main.cpp
```

#### Step 2: Write Application Code

Create `main.cpp`:
```cpp
#include "areg/base/String.hpp"
#include <iostream>

int main() {
    String greeting("Hello from Areg SDK!");
    std::cout << greeting.getData() << std::endl;
    return 0;
}
```

#### Step 3: Create CMakeLists.txt

**Basic version:**
```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Fetch Areg SDK
include(FetchContent)
FetchContent_Declare(
    areg-sdk
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG "master"
)
FetchContent_MakeAvailable(areg-sdk)

# Create executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

**Optimized version (faster builds):**
```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Disable examples and tests for faster builds
option(AREG_BUILD_EXAMPLES "Build examples" OFF)
option(AREG_BUILD_TESTS "Build tests" OFF)
option(AREG_ENABLE_OUTPUTS "Use Areg output dirs" OFF)

# Fetch Areg SDK
include(FetchContent)
FetchContent_Declare(
    areg-sdk
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG "master"
)
FetchContent_MakeAvailable(areg-sdk)

# Create executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

#### Step 4: Build and Run

```bash
cmake -B ./build
cmake --build ./build -j
./build/myapp
```

**Expected output:**
```
Hello from Areg SDK!
```

**Build times:**
- First build: 3-8 minutes (compiles Areg SDK)
- Subsequent builds: Seconds (only your code)

<div align="right"><kbd><a href="#integration-methods">↑ Back to methods ↑</a></kbd></div>

---

### Method 3: vcpkg Package

**Advantages:**
- ✅ Pre-built binaries (fast)
- ✅ Version management
- ✅ Multi-project sharing
- ✅ Automatic dependency resolution

**Disadvantages:**
- ❌ Requires vcpkg installation
- ❌ Additional setup step

> [!NOTE]
> vcpkg support available starting with Areg SDK 2.0.

#### Step 1: Install vcpkg

Follow [vcpkg installation guide](https://github.com/microsoft/vcpkg#quick-start):

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh    # Linux/macOS
# or
.\bootstrap-vcpkg.bat   # Windows
```

#### Step 2: Install Areg SDK Package

```bash
vcpkg install areg
```

**Platform-specific:**
```bash
vcpkg install areg:x64-linux      # 64-bit Linux
vcpkg install areg:x64-windows    # 64-bit Windows
vcpkg install areg:x86-windows    # 32-bit Windows
```

#### Step 3: Integrate vcpkg

```bash
vcpkg integrate install
```

**Save the CMake toolchain path** shown in output:
```
CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

#### Step 4: Create CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Find Areg SDK package
find_package(areg CONFIG REQUIRED)

# Create executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

#### Step 5: Build with Toolchain

Replace `<vcpkg-root>` with your vcpkg installation path:

```bash
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
cmake --build ./build
./build/myapp
```

**Build time:** ~10 seconds (using pre-built binaries)

> [!TIP]
> For detailed vcpkg setup, see [Installing Areg SDK with vcpkg](./02e-cmake-vcpkg.md).

<div align="right"><kbd><a href="#integration-methods">↑ Back to methods ↑</a></kbd></div>

---

### Method 4: Git Submodule

**Advantages:**
- ✅ Source code in your repository
- ✅ Version pinning via Git
- ✅ Full source visibility
- ✅ Offline builds possible

**Disadvantages:**
- ❌ Repository size increase
- ❌ Manual submodule management
- ❌ Slower first build

#### Step 1: Initialize Git Repository

```bash
git init myproject
cd myproject
```

#### Step 2: Add Areg SDK as Submodule

**Option A: Using .gitmodules file**

Create `.gitmodules`:
```ini
[submodule "areg-sdk"]
    path = areg-sdk
    url = https://github.com/aregtech/areg-sdk.git
```

Then run:
```bash
git submodule update --init --recursive
```

**Option B: Using git command**

```bash
git submodule add https://github.com/aregtech/areg-sdk.git areg-sdk
git submodule update --init --recursive
```

#### Step 3: Create CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Set Areg SDK root
set(AREG_SDK_ROOT "${CMAKE_SOURCE_DIR}/areg-sdk")

# Include Areg SDK
add_subdirectory(areg-sdk)

# Create executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

#### Step 4: Build and Run

```bash
cmake -B ./build
cmake --build ./build -j
./build/myapp
```

#### Step 5: Update Submodule (When Needed)

```bash
git submodule update --remote --recursive
```

<div align="right"><kbd><a href="#integration-methods">↑ Back to methods ↑</a></kbd></div>

---

## Advanced Integration

### Areg SDK Components

Understanding what gets included:

| Component | Type | Purpose | Required |
|-----------|------|---------|----------|
| **areg** | Library | Core framework (ORPC, threading, IPC) | ✅ Yes |
| **aregextend** | Library | Extended utilities (optional) | ❌ No |
| **areglogger** | Library | Log observer API | ❌ No |
| **mtrouter** | Executable | Message router for IPC/networking | ❌ No* |
| **logcollector** | Executable | Remote log collection service | ❌ No |
| **codegen.jar** | Tool | Service interface code generator | ✅ For ORPC |

*Required only for inter-process or networked communication.

---

### CMake Configuration Options

Configure Areg SDK build behavior:

```cmake
# Disable examples and tests (faster builds)
option(AREG_BUILD_EXAMPLES "Build examples" OFF)
option(AREG_BUILD_TESTS "Build unit tests" OFF)

# Disable Areg-specific output directories
option(AREG_ENABLE_OUTPUTS "Use Areg output structure" OFF)

# Build extended library
option(AREG_EXTENDED "Build aregextend library" ON)

# Disable logging (smaller binaries)
option(AREG_LOGS "Enable logging" OFF)

# Build static libraries instead of shared
option(AREG_BINARY "areg library type" "static")
option(AREG_LOGGER_BINARY "areglogger library type" "static")
```

**Complete example:**
```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Configure Areg SDK
option(AREG_BUILD_EXAMPLES "Build examples" OFF)
option(AREG_BUILD_TESTS "Build tests" OFF)
option(AREG_EXTENDED "Build extended" ON)

# Fetch Areg SDK
include(FetchContent)
FetchContent_Declare(areg-sdk
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG "master")
FetchContent_MakeAvailable(areg-sdk)

# Your application
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

> [!IMPORTANT]
> Set options **before** fetching or including Areg SDK.

For complete option list, see [CMake Configuration Options](./02d-cmake-config.md).

---

### Using Areg CMake Functions

Areg SDK provides helper functions for common tasks.

**Include Areg utilities:**
```cmake
set(AREG_SDK_ROOT "${areg-sdk_SOURCE_DIR}")
include("${AREG_SDK_ROOT}/areg.cmake")
```

**Simple executable declaration:**
```cmake
# Instead of:
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)

# Use:
macro_declare_executable(myapp main.cpp)
```

**Generate service interface code:**
```cmake
# Generate code from .siml file and create library
addServiceInterface(myservice "services/MyService.siml")

# Use generated service in application
macro_declare_executable(myapp myservice main.cpp)
```

**Example with service interface:**

Project structure:
```
myproject/
├── CMakeLists.txt
├── main.cpp
└── services/
    └── GreetingService.siml
```

`CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Fetch Areg SDK
include(FetchContent)
FetchContent_Declare(areg-sdk
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG "master")
FetchContent_MakeAvailable(areg-sdk)

# Include Areg utilities
set(AREG_SDK_ROOT "${areg-sdk_SOURCE_DIR}")
include("${AREG_SDK_ROOT}/areg.cmake")

# Generate service interface
addServiceInterface(greetingservice "services/GreetingService.siml")

# Create application
macro_declare_executable(myapp greetingservice main.cpp)
```

For complete function reference, see [CMake Functions and Macros](./02e-cmake-functions.md).

---

### Cross-Compilation

Cross-compile for different architectures using CMake toolchain files.

**Example toolchain file** (`arm-linux.cmake`):
```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

**Build with toolchain:**
```bash
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=arm-linux.cmake
cmake --build ./build -j
```

**Example toolchain files available:**
- [Areg SDK toolchains](../../conf/exports/example/toolchains/)
- [Areg SDK Demo toolchains](https://github.com/aregtech/areg-sdk-demo/tree/main/toolchains)

For detailed cross-compilation guide, see [CMake Build Guide - Cross-Compilation](./02b-cmake-build.md#cross-compilation).

---

### Complete Integration Example

**Hybrid approach** (tries vcpkg, falls back to FetchContent):

```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Try to find Areg SDK package (vcpkg or system installation)
find_package(areg CONFIG)

if(NOT areg_FOUND)
    # Package not found, fetch from source
    message(STATUS "Areg SDK not found, fetching from GitHub...")
    
    # Configure build
    option(AREG_BUILD_EXAMPLES "Build examples" OFF)
    option(AREG_BUILD_TESTS "Build tests" OFF)
    set(AREG_BUILD_ROOT "${CMAKE_SOURCE_DIR}/product")
    
    # Fetch sources
    include(FetchContent)
    FetchContent_Declare(areg-sdk
        GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
        GIT_TAG "master")
    FetchContent_MakeAvailable(areg-sdk)
    
    # Set paths
    set(AREG_SDK_ROOT "${areg-sdk_SOURCE_DIR}")
    include_directories("${AREG_SDK_ROOT}/framework")
endif()

# Include Areg utilities
include("${AREG_SDK_ROOT}/areg.cmake")

# Create application using Areg macro
macro_declare_executable(myapp main.cpp)
```

**Benefits:**
- Uses pre-built package if available (fast)
- Falls back to source build if needed (reliable)
- Configures optimal build settings
- Uses Areg helper macros

> [!NOTE]
> When using `macro_declare_executable`, no need to explicitly link with `areg::areg` - it's done automatically.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### CMake Cannot Find Areg Package

**Problem:** `find_package(areg CONFIG REQUIRED)` fails.  
**Solution:**  
*For vcpkg:*
```bash
# Ensure toolchain file is set
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
```
*For system installation:*
```bash
# Specify installation prefix
cmake -B ./build -DCMAKE_PREFIX_PATH=/opt/areg
```

---

### FetchContent Download Fails

**Problem:** Cannot download from GitHub.  
**Solution:**  
*Check network:*
```bash
ping github.com
```
*Use local copy:*
```cmake
FetchContent_Declare(areg-sdk
    SOURCE_DIR "/path/to/local/areg-sdk")
FetchContent_MakeAvailable(areg-sdk)
```

---

### Submodule Not Updating

**Problem:** Git submodule shows outdated code.  
**Solution:**
```bash
git submodule update --remote --recursive
git submodule foreach git pull origin master
```

---

### Build Fails with "Java not found"

**Problem:** Code generator requires Java but it's not found.  
**Solution:**  
*Install Java 17+:*
```bash
# Ubuntu/Debian
sudo apt-get install openjdk-17-jre

# Or download from https://adoptium.net/
```
*Verify:*
```bash
java -version  # Should show version 17+
```

---

### Linking Errors

**Problem:** Undefined reference to Areg symbols.  
**Solution:**  
*Ensure proper linking:*
```cmake
target_link_libraries(myapp PRIVATE areg::areg)
```
*Not this:*
```cmake
target_link_libraries(myapp areg)  # Wrong - missing namespace
```

---

### Wrong Architecture Built

**Problem:** Built for x64 when x86 was needed.  
**Solution:**
```bash
# Explicitly specify architecture
cmake -B ./build -DAREG_PROCESSOR=x86
```
**Or use toolchain file for cross-compilation.**

---

### Options Not Applied

**Problem:** `AREG_BUILD_EXAMPLES=OFF` but examples still build.  
**Solution:**  
*Set options BEFORE fetching:*
```cmake
# Correct order
option(AREG_BUILD_EXAMPLES "Build examples" OFF)
FetchContent_Declare(areg-sdk ...)

# Wrong order - options ignored
FetchContent_Declare(areg-sdk ...)
option(AREG_BUILD_EXAMPLES "Build examples" OFF)  # Too late
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Integration Guides:**
- [Project Setup Tool](./02a-quick-project-setup.md) - Create new project in 30 sec
- [vcpkg Installation](./02e-cmake-vcpkg.md) - Detailed vcpkg setup
- [CMake Configuration](./02d-cmake-config.md) - All configuration options
- [CMake Functions](./02e-cmake-functions.md) - Helper functions reference
- [CMake Build Guide](./02b-cmake-build.md) - Building from source

**Examples:**
- [Areg SDK Examples](../../examples/README.md) - Sample applications
- [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) - Integration examples

**Build Guides:**
- [Building with CMake](./02b-cmake-build.md)
- [Building with MSBuild](./02c-msbuild-build.md)
- [Building on WSL](./02d-wsl-build.md)

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
