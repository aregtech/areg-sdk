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
3. [Issue 3: Error Message `File wrong format`](#issue-3-error-message-file-wrong-format)
   - [Solution 1: Set the Correct Compiler Target](#solution-1-set-the-correct-compiler-target)
   - [Solution 2: Use `macro_check_module_architect` to Verify Compatibility](#solution-2-use-macro_check_module_architect-to-verify-compatibility)
   - [Solution 3: Control Library Search Order](#solution-3-control-library-search-order)
   - [Solution 4: Verify the Library Path](#solution-4-verify-the-library-path)
   - [Solution 5: Use Toolchain Files](#solution-5-use-toolchain-files)

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
set(CMAKE_SYSTEM_NAME Linux)                        # Specify platform
set(CMAKE_SYSTEM_PROCESSOR arm)                     # Specify processor architecture
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)       # Specify C-compiler
set(CMAKE_C_COMPILER_TARGET arm-linux-gnueabihf)    # Specify C-target
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)     # Specify C++-compiler
set(CMAKE_CXX_COMPILER_TARGET arm-linux-gnueabihf)  # Specify C++-target
set(CMAKE_FIND_ROOT_PATH /path/to/sysroot)          # This is normally path "/usr"
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)        # Find modes, see CMake docs for details.
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf) # Specify the library architecture to search packages
```

Use the toolchain file during configuration:
```bash
cmake -B ./product/cache/gnu-arm32 -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake
```

This approach provides a consistent cross-compilation environment and minimizes errors.

> [!TIP]
> Working examples of toolchain files you can find in the [`toolchains`](../../conf/exports/example/toolchains/) directory or visit [`areg-sdk-demo`](https://github.com/aregtech/areg-sdk-demo) repository.

---

## Issue 3: Error Message `File wrong format`

This error occurs when `find_package()` or `find_library()` successfully locates a library, but the library is incompatible with the target platform. For example, this issue arises when the build target is set for the `i386` processor architecture, but the located library is built for `x86_64`.

By applying these solutions, you can resolve the `File wrong format` error and ensure your build uses libraries that are compatible with the specified target platform.

### Solution 1: Set the Correct Compiler Target

Ensure that the `CMAKE_CXX_COMPILER_TARGET` or `AREG_TARGET` variables are configured to match the desired processor architecture:
```bash
cmake -DAREG_TARGET=i386-linux-gnu <source-dir>
```

### Solution 2: Use `macro_check_module_architect` to Verify Compatibility

In your CMake script, use the `macro_check_module_architect` macro from the `functions.cmake` file to verify the architecture of the located library before linking:
```cmake
macro_check_module_architect("/path/to/library.so" ${AREG_TARGET} ${AREG_PROCESSOR} _is_compatible)
if (_is_compatible)
    target_link_libraries(my_target "/path/to/library.so")
else()
    message(FATAL_ERROR "Incompatible library detected for target architecture")
endif()
```

### Solution 3: Control Library Search Order

Restrict the library search scope using the `NO_DEFAULT_PATH` option in `find_package()` or `find_library()` to ensure only compatible libraries are located:
```cmake
find_library(MyLib mylib PATHS /path/to/compatible/libs NO_DEFAULT_PATH)
```

### Solution 4: Verify the Library Path

Ensure that the correct version of the library is being linked by explicitly specifying the library path.
Use the `CMAKE_PREFIX_PATH` variable to define the directory containing the compatible libraries:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/compatible/libs <source-dir>
```

### Solution 5: Use Toolchain Files

When cross-compiling, leverage toolchain files to enforce the correct target architecture. These files help define essential compiler and architecture settings. Refer to example [toolchain files](../../conf/exports/example/toolchains/) or visit the [`areg-sdk-demo`](https://github.com/aregtech/areg-sdk-demo) repository for guidance.

---
