# Integration Troubleshooting

This guide provides solutions for common issues encountered when integrating the AREG SDK into projects, ensuring a smooth and successful build process.

---

## Table of Contents

1. [Issue 1: Values of `AREG_XXX` Variables are Ignored](#issue-1-values-of-areg_xxx-variables-are-ignored)
   - [Solution 1: Integrate AREG SDK Before Project Declaration](#solution-1-integrate-areg-sdk-before-project-declaration)
   - [Solution 2: Use Standard CMake Variables](#solution-2-use-standard-cmake-variables)
   - [Solution 3: Manually Include `<areg-sdk>/areg.cmake`](#solution-3-manually-include-areg-sdkaregcmake)
2. [Issue 2: Error Message `Relocations in generic ELF (EM: 62)`](#issue-2-error-message-relocations-in-generic-elf-em-62)
   - [Solution 1: Clear the CMake Cache](#solution-1-clear-the-cmake-cache)
   - [Solution 2: Explicitly Specify C and C++ Compilers](#solution-2-explicitly-specify-c-and-c-compilers)
   - [Solution 3: Use Prebuilt Toolchain-Compatible Libraries](#solution-3-use-prebuilt-toolchain-compatible-libraries)
   - [Solution 4: Cross-Compilation Toolchain File](#solution-4-cross-compilation-toolchain-file)

---

## Issue 1: Values of `AREG_XXX` Variables are Ignored

During the build process, values of AREG-specific variables such as `AREG_COMPILER_FAMILY`, `AREG_COMPILER`, `AREG_BUILD_TYPE`, and `AREG_PROCESSOR` might be ignored. This typically happens when the AREG SDK is integrated *after* the first call to `project()`, causing these variables to be overwritten by CMake's standard variables (`CMAKE_CXX_COMPILER`, `CMAKE_C_COMPILER`, `CMAKE_BUILD_TYPE`, etc.).

### Solution 1: Integrate AREG SDK Before Project Declaration

Ensure the AREG SDK is integrated into your project *before* the first call to `project()`. Additionally, set the values of AREG-specific CMake variables before fetching the SDK sources to avoid conflicts.

### Solution 2: Use Standard CMake Variables

If integrating the AREG SDK before the `project()` call is not possible, rely on standard CMake variables to configure the build. Pass these variables via the command line when running `cmake`:

- **`CMAKE_CXX_COMPILER`** and **`CMAKE_C_COMPILER`**: Explicitly define the compilers for C++ and C.
- **`CMAKE_BUILD_TYPE`**: Specify the build configuration, such as `Debug` or `Release`.

These standard variables are fully compatible with the AREG SDK and help avoid potential conflicts.

**Example: Cross-Compiler Setup**
```bash
cmake -B ./product/cache/gnu-arm32 -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc
cmake --build ./product/cache/gnu-arm32 -j20
```

Alternatively, when integrating AREG SDK *before* the `project()` call, the `AREG_COMPILER` variable can be used similarly:
```bash
cmake -B ./product/cache/gnu-arm32 -DAREG_COMPILER=arm-linux-gnueabihf-g++
```

> [!IMPORTANT]
> When fetching AREG SDK sources after the first `project()` call, set both `CMAKE_CXX_COMPILER` and `CMAKE_C_COMPILER`. This is especially critical if you are building the `sqlite3` library from sources in the `thirdparty` directory of AREG SDK instead of using a pre-built `sqlite3` package. Not setting both compilers may result in build errors.

### Solution 3: Manually Include `<areg-sdk>/areg.cmake`

If using the AREG SDK as a submodule, explicitly include `<areg-sdk>/areg.cmake` in your `CMakeLists.txt` file *before* the `project()` call. This ensures AREG-specific variables are properly initialized:
```cmake
include(<path-to-areg-sdk>/areg.cmake)
project(MyProject)
```

---

## Issue 2: Error Message `Relocations in generic ELF (EM: 62)`

When cross-compiling, you might encounter the error `Relocations in generic ELF (EM: 62)` during the linking process. This issue typically arises when source files are compiled using mismatched compilers or for different target platforms. For instance, the `sqlite3` library might be compiled with `gcc` while the rest of the project is compiled with `arm-linux-gnueabihf-g++`.

To confirm whether mismatched compilers are being used, utilize the `printAregConfigStatus()` function in the `functions.cmake` file. This function provides a configuration report, including details of the compilers in use.

### Solution 1: Clear the CMake Cache

Residual settings in the CMake cache can cause this issue, especially when switching from a native build to cross-compiling.

1. **Clear the Cache**: Remove the contents of the build directory, including the `CMakeCache.txt` file:
   ```bash
   rm -rf ./product/cache/*
   ```
2. **Reconfigure and Build**: Reconfigure the project and retry the build.

### Solution 2: Explicitly Specify C and C++ Compilers

Ensure both the C and C++ compilers are explicitly specified for the target platform. Pass these compilers directly via the command line when running `cmake`:
```bash
cmake -B ./product/cache/gnu-arm32 -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc
cmake --build ./product/cache/gnu-arm32 -j20
```

This ensures consistency across all source files, preventing mismatched binaries.

### Solution 3: Use Prebuilt Toolchain-Compatible Libraries

If rebuilding third-party dependencies like `sqlite3` is not feasible, use prebuilt versions of these libraries compatible with your target platform and toolchain. For example, package managers such as `vcpkg` or `conan` can provide suitable binaries:
```bash
vcpkg install sqlite3:linux-arm
```

Configure the project to use prebuilt packages:
```bash
cmake ./build -DAREG_USE_PACKAGES:BOOL=TRUE -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++
cmake --build ./build -j20
```

### Solution 4: Cross-Compilation Toolchain File

Define a cross-compilation environment using a dedicated CMake toolchain file. Example (`toolchain-arm.cmake`):
```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
set(CMAKE_FIND_ROOT_PATH /path/to/sysroot)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

Use the toolchain file during configuration:
```bash
cmake -B ./product/cache/gnu-arm32 -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake
```

This approach provides a consistent cross-compilation environment and minimizes errors.

---
