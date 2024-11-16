# Integration Troubleshooting

This guide provides solutions for common issues encountered when integrating AREG SDK in the projects, helping to ensure a smooth and successful build process.

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

During the build process, the values of AREG-specific variables such as `AREG_COMPILER_FAMILY`, `AREG_COMPILER`, `AREG_BUILD_TYPE`, and `AREG_PROCESSOR` might be ignored. This issue typically arises if the AREG SDK is integrated *after* the first invocation of `project()`. In such cases, these variables are overwritten by CMake's standard variables (`CMAKE_CXX_COMPILER`, `CMAKE_C_COMPILER`, `CMAKE_BUILD_TYPE`, etc.).

### Solution 1: Integrate AREG SDK Before Project Declaration

To prevent this issue, ensure the AREG SDK is fetched and integrated into your project *before* the first call to `project()`. Additionally, set the values of AREG-specific CMake variables before fetching the AREG SDK sources to ensure they are recognized.

### Solution 2: Use Standard CMake Variables  

If you cannot integrate the AREG SDK *before* the `project()` call, you can rely on CMake's standard variables to configure the build. These variables can be specified via the command line when running `cmake`:  

- **`CMAKE_CXX_COMPILER`** and **`CMAKE_C_COMPILER`**: Explicitly define the C++ and C compilers.  
- **`CMAKE_BUILD_TYPE`**: Specify the desired build configuration, such as `Debug` or `Release`.  

These standard variables are fully compatible with the AREG SDK specific variables and help avoid potential conflicts or overrides.  

**Example: Setting up a cross-compiler**  
```bash  
cmake -B ./product/cache/gnu-arm32 -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc  
cmake --build ./product/cache/gnu-arm32 -j20  
```  

This approach is **equally effective** as specifying `AREG_COMPILER` variable when AREG SDK is integrated *before* the `project()` call:  
```bash  
cmake -B ./product/cache/gnu-arm32 -DAREG_COMPILER=arm-linux-gnueabihf-g++  
```  

> [!IMPORTANT]  
> If you include the AREG SDK sources in your project and they are fetched *after* the first call to `project()`, make sure to set both `CMAKE_CXX_COMPILER` and `CMAKE_C_COMPILER`. This is particularly important if you are building the `sqlite3` library from the sources in the `thirdparty` directory of the AREG SDK, instead of using a pre-built `sqlite3` package. Not setting the C compiler alongside the C++ compiler may result in build errors.

### Solution 3: Manually Include `<areg-sdk>/areg.cmake`

If you use AREG SDK repository as a submodule, ensure that `<areg-sdk>/areg.cmake` is explicitly included in your project's `CMakeLists.txt` *before* the `project()` call. This guarantees that AREG-specific variables are initialized and take precedence. Example:
```cmake
include(<path-to-areg-sdk>/areg.cmake)
project(MyProject)
```

---

Yes, here are additional suggestions to enhance the troubleshooting section for the issue `Relocations in generic ELF (EM: 62)`:

---

## Issue 2: Error Message `Relocations in generic ELF (EM: 62)`  

When cross-compiling, you may encounter the error `Relocations in generic ELF (EM: 62)` during the linking process. This issue typically arises when source files are compiled with mismatched compilers or for different target platforms. For instance, the `sqlite3` library might have been compiled with the `gcc` compiler while the rest of the project was compiled with `arm-linux-gnueabihf-g++`.  

To verify whether different compilers are being used for C and C++ sources, utilize the `printAregConfigStatus()` function, defined in the `functions.cmake` file. This function provides a detailed configuration report, including the compilers in use.

### Solution 1: Clear the CMake Cache

If you previously built the project with different settings and later switched to cross-compiling, residual data in the CMake cache may cause this error. To resolve the issue:  

1. **Clear the Cache**: Delete the contents of the build directory, including the `CMakeCache.txt` file.  
   ```bash  
   rm -rf ./product/cache/*  
   ```  
2. **Reconfigure and Build**: Reconfigure the project and retry the build process.  

### Solution 2: Explicitly Specify C and C++ Compilers  

Ensure that both C and C++ compilers are explicitly specified for the target platform. Instead of relying solely on AREG SDK-specific variables, pass the appropriate compilers directly via the CMake command line:  
```bash  
cmake -B ./product/cache/gnu-arm32 \
      -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ \
      -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc  
cmake --build ./product/cache/gnu-arm32 -j20  
```  

This ensures consistency in the compilers used for all source files, preventing mismatched binaries and linkage errors.

### Solution 3: Use Prebuilt Toolchain-Compatible Libraries  

If rebuilding dependencies is not feasible, check for prebuilt versions of the required libraries compatible with your target platform and toolchain. Package managers such as `vcpkg` or `conan` may provide suitable binaries. For example:  
```bash  
vcpkg install sqlite3:linux-arm  
```

Then you compile the sources giving instructions to use the pre-build package:
```bash
cmake ./build -DAREG_USE_PACKAGES:BOOL=TRUE -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++
cmake --build ./build -j20
```

### Solution 4: Cross-Compilation Toolchain File  

Create a dedicated CMake toolchain file to define the cross-compilation environment, including the target platform, compilers, and paths to dependencies. Example toolchain file (`toolchain-arm.cmake`):  
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
Use it during configuration:  
```bash  
cmake -B ./product/cache/gnu-arm32 -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake  
```  

This ensures a consistent environment for cross-compiling, minimizing errors.

---
