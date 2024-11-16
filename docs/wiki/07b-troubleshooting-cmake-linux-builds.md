# Troubleshooting CMake Builds on Linux

This guide provides solutions for common issues encountered when compiling projects with CMake on Linux, helping to ensure a smooth and successful build process.

---

## Table of Contents

1. [Issue 1: Missing `<asm/errno.h>` File](#issue-1-missing-asmerrnoh-file)
   - [Solution: Create a Symbolic Link](#solution-create-a-symbolic-link)
2. [Issue 2: Values of `AREG_XXX` Variables are Ignored](#issue-2-values-of-areg_xxx-variables-are-ignored)
   - [Solution 1: Fetch Sources Before Project Declaration](#solution-fetch-sources-before-project-declaration)
   - [Solution 2: Use Standard CMake Variables](#solution-2-use-standard-cmake-variables)
3. [Issue 3: Failure to Cross-Compile with Extended Objects Enabled](#issue-3-failure-to-cross-compile-with-extended-objects-enabled)
   - [Solution 1: Use Target-Compatible Libraries](#solution-1-use-target-compatible-libraries)
   - [Solution 2: Pre-Build Required Libraries](#solution*2-pre-build-required-libraries)
   - [Solution 3: Disable Extended Objects](#solution-3-disable-extended-objects)

---

## Issue 1: Missing `<asm/errno.h>` File

If you receive an error during compilation indicating that `<asm/errno.h>` could not be found, it often means certain header files are missing or not correctly linked in the system's include directories.

### Solution: Create a Symbolic Link

> [!NOTE]  
> You will need administrator (root) permissions for this solution.

To address this issue, create a symbolic link from the `asm-generic` directory to the `asm` directory in `/usr/include`. This enables the compiler to locate the required header files.

1. **Open Terminal**: Launch your terminal.
2. **Run the Command**:
   ```bash
   sudo ln -s /usr/include/asm-generic/ /usr/include/asm
   ```
   This command creates a symbolic link called `asm` in `/usr/include`, pointing to the `asm-generic` directory.

3. **Retry Compilation**: After creating the symbolic link, rerun the compilation command. This should resolve the error.

> [!TIP]  
> Alternatively, the `asm/errno.h` header file is included in some system packages. You can try installing it by running:
> ```bash
> sudo apt-get install -y linux-libc-dev
> ```

---

## Issue 2: Values of `AREG_XXX` Variables are Ignored

During the build process, the values of `AREG_COMPILER_FAMILY`, `AREG_COMPILER`, `AREG_BUILD_TYPE`, and even the target processor defined in `AREG_PROCESSOR` variable may be ignored. This typically occurs when the AREG SDK is integrated *after* the first invocation of `project()`, as these AREG-specific variables are overwritten by standard CMake values.

### Solution 1: Fetch Sources Before Project Declaration

Fetch the AREG SDK sources *before* calling `project()`. Additionally, you can set the values of AREG-specific CMake variables prior to fetching the SDK.

### Solution 2: Use Standard CMake Variables

Alternatively, use standard CMake variables such as `CMAKE_CXX_COMPILER` to specify the compiler and `CMAKE_BUILD_TYPE` to define the build type. These variables are compatible with AREG-specific CMake configurations and help avoid conflicts.

---

## Issue 3: Failure to Cross-Compile with Extended Objects Enabled

The build process fails during cross-compilation when AREG SDK extended objects are enabled (option `-DAREG_EXTENDED=ON`). The error message typically states: `cannot find -lncurses: No such file or directory`, even when `ncurses` library is installed. Similar may happend with `sqlite3` library, wich is not connected to extended objects.

This issue occurs when the system lacks target-compatible libraries. For example, if your host system runs on an `x86_64` platform but you are cross-compiling for 32-bit ARM, this error will appear due to missing ARM-compatible versions of the required libraries.  

### Solution 1: Use Target-Compatible Libraries

Ensure that the necessary libraries are available for your target platform. You can install them directly or use a package manager, such as `vcpkg`, to compile the dependencies for your target architecture. For example, to install `sqlite3` for an ARM Linux target, use the following command:  
```bash  
vcpkg install sqlite3:linux-arm  
```  
For `ncurses`, similarly ensure you compile or install the library for the target platform.  

### Solution 2: Pre-Build Required Libraries

Manually build and install the required libraries (`ncurses`, `sqlite3`, etc.) for the target platform using your cross-compilation toolchain. Ensure the paths to these libraries are correctly set in your build environment. For simplicity and portability, consider building statically linked versions of `ncurses` and `sqlite3` for your target platform. This approach avoids runtime dependency issues on the target system.

### Solution 3: Disable Extended Objects

If the extended features of the AREG SDK are not required for your application, you can disable them by setting `-DAREG_EXTENDED=OFF`. This removes dependencies on `ncurses` library. And for `sqlite3` library, use AREG SDK built-in SQLite3 sources available in the `thirdparty` subdirectory.  

---
