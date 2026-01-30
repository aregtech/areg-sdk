# Integration Troubleshooting

This guide provides solutions for common issues encountered when integrating the Areg SDK into projects, ensuring a smooth and successful build process.

---

## Table of Contents

1. [Issue 1: Values of `AREG_XXX` Variables are Ignored](#issue-1-values-of-areg_xxx-variables-are-ignored)
2. [Issue 2: Error Message `Relocations in generic ELF (EM: 62)`](#issue-2-error-message-relocations-in-generic-elf-em-62)
3. [Issue 3: Error Message `File wrong format`](#issue-3-error-message-file-wrong-format)
4. [Summary](#summary)

---

## Issue 1: Values of `AREG_XXX` Variables are Ignored

### Problem Description

During the build process, values of Areg-specific variables such as `AREG_COMPILER_FAMILY`, `AREG_COMPILER`, `AREG_BUILD_TYPE`, and `AREG_PROCESSOR` might be ignored.

**Root Cause**: This typically happens when the Areg SDK is integrated *after* the first call to `project()`, causing these variables to be overwritten by CMake's standard variables (`CMAKE_CXX_COMPILER`, `CMAKE_C_COMPILER`, `CMAKE_BUILD_TYPE`, etc.).

### Solution 1: Integrate Areg SDK Before Project Declaration

Ensure the Areg SDK is integrated into your project *before* the first call to `project()`. Additionally, set the values of Areg-specific CMake variables before fetching the SDK sources to avoid conflicts.

**Example CMakeLists.txt:**

```cmake
cmake_minimum_required(VERSION 3.20)

# Set Areg-specific variables BEFORE project()
set(AREG_COMPILER "arm-linux-gnueabihf-g++")
set(AREG_BUILD_TYPE "Release")

# Fetch Areg SDK
include(FetchContent)
FetchContent_Declare(
    areg-sdk
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(areg-sdk)

# Now declare project
project(MyProject)
```

### Solution 2: Use Standard CMake Variables

If integrating the Areg SDK before the `project()` call is not possible, rely on standard CMake variables to configure the build. Pass these variables via the command line when running `cmake`:

**Key Variables:**
- **`CMAKE_CXX_COMPILER`**: Explicitly define the C++ compiler
- **`CMAKE_C_COMPILER`**: Explicitly define the C compiler
- **`CMAKE_BUILD_TYPE`**: Specify the build configuration (`Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel`)

These standard variables are fully compatible with the Areg SDK and help avoid potential conflicts.

**Example: Cross-Compiler Setup**

```bash
cmake -B ./product/cache/gnu-arm32 \
  -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ \
  -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
  -DCMAKE_BUILD_TYPE=Release

cmake --build ./product/cache/gnu-arm32 -j
```

**Alternative: Using Areg-Specific Variables (Before `project()`)**

```bash
cmake -B ./product/cache/gnu-arm32 \
  -DAREG_COMPILER=arm-linux-gnueabihf-g++ \
  -DAREG_BUILD_TYPE=Release
```

> [!IMPORTANT]
> **Critical for SQLite3 Builds**:
>
> When fetching Areg SDK sources after the first `project()` call, set both `CMAKE_CXX_COMPILER` and `CMAKE_C_COMPILER`. This is especially critical if you are building the `sqlite3` library from sources in the `thirdparty` directory of Areg SDK instead of using a pre-built `sqlite3` package.
>
> **Not setting both compilers may result in build errors** when compiling SQLite3 or other C-based dependencies.

### Solution 3: Manually Include `<areg-sdk>/areg.cmake`

If using the Areg SDK as a submodule, explicitly include `<areg-sdk>/areg.cmake` in your `CMakeLists.txt` file *before* the `project()` call. This ensures Areg-specific variables are properly initialized:

```cmake
cmake_minimum_required(VERSION 3.20)

# Include Areg SDK BEFORE project()
include(<path-to-areg-sdk>/areg.cmake)

# Now declare project
project(MyProject)
```

**Example with Submodule:**

```cmake
# Assuming areg-sdk is in thirdparty/areg-sdk/
include(${CMAKE_SOURCE_DIR}/thirdparty/areg-sdk/areg.cmake)
project(MyProject VERSION 1.0.0)
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Issue 2: Error Message `Relocations in generic ELF (EM: 62)`

### Problem Description

When cross-compiling, you might encounter the error:

```
error: Relocations in generic ELF (EM: 62)
```

**Example Context:**
```
/usr/bin/ld: build/sqlite3.o: error: Relocations in generic ELF (EM: 62)
/usr/bin/ld: build/sqlite3.o: error adding symbols: file format not recognized
```

### Root Cause

This issue typically arises when source files are compiled using **mismatched compilers** or for **different target platforms**. Common scenarios:

- The `sqlite3` library is compiled with `gcc` (native x86_64)
- The rest of the project is compiled with `arm-linux-gnueabihf-g++` (ARM target)
- Linker attempts to link x86_64 object files with ARM binaries

### Diagnosing the Issue

Use the `printAregConfigStatus()` function from `functions.cmake` to verify compiler configuration:

```cmake
# Add to your CMakeLists.txt
include(<areg-sdk>/conf/cmake/functions.cmake)
printAregConfigStatus()
```

This function provides a configuration report showing:
- Compiler family and versions
- Target architecture
- Build type
- Detected system information

### Solution 1: Clear the CMake Cache

Residual settings in the CMake cache can cause this issue, especially when switching from a native build to cross-compiling.

**Steps:**

1. **Clear the Cache**:
   ```bash
   rm -rf ./product/cache/*
   ```

2. **Clean Build Directory** (alternative):
   ```bash
   rm -rf build/
   mkdir build
   ```

3. **Reconfigure and Build**:
   ```bash
   cmake -B build \
     -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ \
     -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc
   
   cmake --build build -j
   ```

### Solution 2: Explicitly Specify C and C++ Compilers

Ensure both the C and C++ compilers are explicitly specified for the target platform. Pass these compilers directly via the command line:

```bash
cmake -B ./product/cache/gnu-arm32 \
  -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ \
  -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
  -DCMAKE_BUILD_TYPE=Release

cmake --build ./product/cache/gnu-arm32 -j
```

This ensures consistency across all source files, preventing mismatched binaries.

**Verify Compiler Usage:**

```bash
# Check which compilers are being used
cmake -B build -DCMAKE_VERBOSE_MAKEFILE=ON
```

### Solution 3: Use Prebuilt Toolchain-Compatible Libraries

If rebuilding third-party dependencies like `sqlite3` is not feasible, use prebuilt versions compatible with your target platform and toolchain.

**Using vcpkg:**

1. **Install vcpkg** (if not already installed):
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh
   ```

2. **Install SQLite3 for target platform**:
   ```bash
   ./vcpkg install sqlite3:arm-linux
   ```

3. **Configure project to use prebuilt packages**:
   ```bash
   cmake -B ./build \
     -DAREG_USE_PACKAGES:BOOL=TRUE \
     -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake \
     -DVCPKG_TARGET_TRIPLET=arm-linux \
     -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++
   
   cmake --build ./build -j
   ```

**Using Conan:**

```bash
conan install . --build=missing --settings arch=armv7 --settings os=Linux
cmake -B build -DAREG_USE_PACKAGES:BOOL=TRUE
cmake --build build -j
```

### Solution 4: Cross-Compilation Toolchain File

Define a cross-compilation environment using a dedicated CMake toolchain file. This provides a consistent environment and minimizes errors.

**Example Toolchain File (`toolchain-arm.cmake`):**

```cmake
# Specify target system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Specify compilers
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_C_COMPILER_TARGET arm-linux-gnueabihf)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
set(CMAKE_CXX_COMPILER_TARGET arm-linux-gnueabihf)

# Specify sysroot (adjust path to your environment)
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

# Search paths configuration
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Specify library architecture
set(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf)
```

**Using the Toolchain File:**

```bash
cmake -B ./product/cache/gnu-arm32 \
  -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake

cmake --build ./product/cache/gnu-arm32 -j
```

> [!TIP]
> **Example Toolchain Files Available**:
> - Working examples: [`toolchains`](../../conf/exports/example/toolchains/) directory
> - More examples: [`areg-sdk-demo`](https://github.com/aregtech/areg-sdk-demo) repository

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Issue 3: Error Message `File wrong format`

### Problem Description

This error occurs when `find_package()` or `find_library()` successfully locates a library, but the library is incompatible with the target platform.

**Example Error:**
```
error: /usr/lib/x86_64-linux-gnu/libsqlite3.so: file format not recognized; treating as linker script
```

**Common Scenario**: Build target is set for the `i386` processor architecture, but the located library is built for `x86_64`.

### Root Cause

CMake's library search mechanisms (`find_package()`, `find_library()`) can locate libraries that are incompatible with the target architecture when:

- System has multiple library versions for different architectures
- `CMAKE_LIBRARY_ARCHITECTURE` is not set correctly
- Library search paths include incompatible architectures
- Cross-compilation environment is not properly configured

### Solution 1: Set the Correct Compiler Target

Ensure that the `CMAKE_CXX_COMPILER_TARGET` or `AREG_TARGET` variables are configured to match the desired processor architecture:

**Using AREG_TARGET:**
```bash
cmake -B build \
  -DAREG_TARGET=i386-linux-gnu \
  <source-dir>
```

**Using CMAKE Variables:**
```bash
cmake -B build \
  -DCMAKE_CXX_COMPILER_TARGET=i386-linux-gnu \
  -DCMAKE_C_COMPILER_TARGET=i386-linux-gnu \
  <source-dir>
```

**Common Target Triplets:**
- `i386-linux-gnu` - 32-bit x86
- `x86_64-linux-gnu` - 64-bit x86
- `arm-linux-gnueabihf` - ARM 32-bit hard-float
- `aarch64-linux-gnu` - ARM 64-bit

### Solution 2: Use `macro_check_module_architect` to Verify Compatibility

In your CMake script, use the `macro_check_module_architect` macro from the `functions.cmake` file to verify the architecture of the located library before linking:

```cmake
# Include Areg SDK functions
include(<areg-sdk>/conf/cmake/functions.cmake)

# Find the library
find_library(SQLITE3_LIB sqlite3)

if(SQLITE3_LIB)
    # Verify architecture compatibility
    macro_check_module_architect(
        "${SQLITE3_LIB}"        # Library path
        ${AREG_TARGET}           # Target architecture
        ${AREG_PROCESSOR}        # Processor type
        _is_compatible           # Output variable
    )
    
    if(_is_compatible)
        message(STATUS "Library is compatible: ${SQLITE3_LIB}")
        target_link_libraries(my_target ${SQLITE3_LIB})
    else()
        message(FATAL_ERROR "Incompatible library detected for target architecture: ${SQLITE3_LIB}")
    endif()
else()
    message(FATAL_ERROR "Library not found")
endif()
```

### Solution 3: Control Library Search Order

Restrict the library search scope using the `NO_DEFAULT_PATH` option in `find_package()` or `find_library()` to ensure only compatible libraries are located:

**Example: Restrict to Specific Path**

```cmake
# Search only in the specified path
find_library(MyLib mylib 
    PATHS /usr/lib/i386-linux-gnu
    NO_DEFAULT_PATH
)
```

**Example: Multiple Paths with Priority**

```cmake
# Search in order of preference
find_library(SQLITE3_LIB sqlite3
    PATHS
        /usr/local/lib/i386-linux-gnu  # Custom build (priority)
        /usr/lib/i386-linux-gnu        # System library (fallback)
    NO_DEFAULT_PATH
)
```

### Solution 4: Verify the Library Path

Ensure that the correct version of the library is being linked by explicitly specifying the library path using the `CMAKE_PREFIX_PATH` variable:

**Set Prefix Path:**

```bash
cmake -B build \
  -DCMAKE_PREFIX_PATH=/usr/lib/i386-linux-gnu \
  <source-dir>
```

**Multiple Prefix Paths:**

```bash
cmake -B build \
  -DCMAKE_PREFIX_PATH="/opt/mylibs;/usr/local/lib/i386-linux-gnu" \
  <source-dir>
```

**Verify Library Architecture:**

Use `file` command to check library architecture:

```bash
file /usr/lib/i386-linux-gnu/libsqlite3.so
# Expected: ELF 32-bit LSB shared object, Intel 80386

file /usr/lib/x86_64-linux-gnu/libsqlite3.so
# Expected: ELF 64-bit LSB shared object, x86-64
```

### Solution 5: Use Toolchain Files

When cross-compiling, leverage toolchain files to enforce the correct target architecture. These files help define essential compiler and architecture settings.

**Example Toolchain File with Library Paths (`toolchain-i386.cmake`):**

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR i386)

# Compilers
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_COMPILER_TARGET i386-linux-gnu)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_COMPILER_TARGET i386-linux-gnu)

# Library architecture
set(CMAKE_LIBRARY_ARCHITECTURE i386-linux-gnu)

# Sysroot and library paths
set(CMAKE_FIND_ROOT_PATH /usr/lib/i386-linux-gnu)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

**Using the Toolchain:**

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=toolchain-i386.cmake
```

> [!TIP]
> **Reference Examples**:
> - Example toolchain files: [`toolchains`](../../conf/exports/example/toolchains/) directory
> - Complete examples: [`areg-sdk-demo`](https://github.com/aregtech/areg-sdk-demo) repository

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Summary

This guide covered three main categories of integration issues when working with Areg SDK:

1. **Variable Configuration**: Areg-specific variables being overwritten by CMake defaults
2. **Compiler Mismatch**: Cross-compilation with inconsistent compilers causing linking errors
3. **Architecture Incompatibility**: Libraries for wrong target architecture being linked

### Key Takeaways

- ✅ Integrate Areg SDK before `project()` declaration when possible
- ✅ Always set both C and C++ compilers explicitly for cross-compilation
- ✅ Use toolchain files for consistent cross-compilation environments
- ✅ Verify library architecture compatibility before linking
- ✅ Clear CMake cache when switching between build configurations

### Best Practices

**Integration:**
- Include `areg.cmake` before `project()` call
- Set Areg-specific variables before SDK integration
- Use standard CMake variables when late integration is necessary

**Cross-Compilation:**
- Create and use toolchain files for each target platform
- Set both `CMAKE_C_COMPILER` and `CMAKE_CXX_COMPILER`
- Verify compiler configuration with `printAregConfigStatus()`
- Clear CMake cache when changing target platforms

**Dependency Management:**
- Use package managers (vcpkg, Conan) for target-compatible libraries
- Verify library architecture with `macro_check_module_architect`
- Control library search paths with `CMAKE_PREFIX_PATH`
- Use `NO_DEFAULT_PATH` to restrict library search scope

### Troubleshooting Checklist

**Before Building:**
1. ☑ Verify Areg SDK is integrated before `project()` (if possible)
2. ☑ Check `CMAKE_CXX_COMPILER` and `CMAKE_C_COMPILER` are set correctly
3. ☑ Confirm `CMAKE_BUILD_TYPE` matches intended build configuration
4. ☑ Verify toolchain file (if cross-compiling) has correct settings

**When Errors Occur:**
1. ☑ Clear CMake cache: `rm -rf build/`
2. ☑ Run `printAregConfigStatus()` to verify configuration
3. ☑ Check library architecture with `file` command
4. ☑ Verify compiler is being used correctly (`CMAKE_VERBOSE_MAKEFILE=ON`)

### Additional Resources

For more information on Areg SDK integration and CMake configuration:
- [Areg SDK CMake Functions](./02e-cmake-functions.md)
- [CMake Toolchains Documentation](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)
- [Areg SDK Demo Repository](https://github.com/aregtech/areg-sdk-demo)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
