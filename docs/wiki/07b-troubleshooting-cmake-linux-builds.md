# Troubleshooting CMake Builds on Linux

This guide provides solutions for common issues encountered when compiling projects with CMake on Linux, helping to ensure a smooth and successful build process for Areg SDK applications.

---

## Table of Contents

1. [Issue 1: Missing `<asm/errno.h>` File](#issue-1-missing-asmerrnoh-file)
2. [Issue 2: Failure to Cross-Compile with the System SQLite3](#issue-2-failure-to-cross-compile-with-the-system-sqlite3)
3. [Summary](#summary)

---

## Issue 1: Missing `<asm/errno.h>` File

### Problem Description

If you encounter an error during compilation that states `<asm/errno.h>` could not be found, this typically indicates that certain header files are either missing or not properly linked in the system's include directories.

**Example error message:**
```
fatal error: asm/errno.h: No such file or directory
```

### Solution 1: Create a Symbolic Link

> [!NOTE]
> Administrator (root) permissions are required for this solution.

To resolve this issue, create a symbolic link from the `asm-generic` directory to the `asm` directory in `/usr/include`. This ensures the compiler can locate the required header files.

**Steps:**

1. **Open Terminal and Run the Command**:
   ```bash
   sudo ln -s /usr/include/asm-generic/ /usr/include/asm
   ```
   This command creates a symbolic link named `asm` in `/usr/include`, pointing to the `asm-generic` directory.

2. **Verify the Link**:
   ```bash
   ls -l /usr/include/asm
   ```
   Expected output: `lrwxrwxrwx ... /usr/include/asm -> /usr/include/asm-generic/`

3. **Retry Compilation**: Rerun the CMake build command. The error should be resolved.

### Solution 2: Install Missing Libraries

In some cases, the `asm/errno.h` file is provided by specific system packages. Installing the appropriate package can resolve the issue.

**For Debian/Ubuntu-based systems:**
```bash
sudo apt-get update
sudo apt-get install -y linux-libc-dev
```

**For Red Hat/Fedora-based systems:**
```bash
sudo dnf install -y kernel-headers
```

**For Arch Linux:**
```bash
sudo pacman -S linux-headers
```

After installation, verify the headers are present:
```bash
ls /usr/include/asm/errno.h
```

### Solution 3: Check Toolchain and Cross-Compilation Settings

If you are cross-compiling, verify that the toolchain being used includes the necessary headers for the target architecture.

**For ARM Cross-Compilation:**

Install compatible `libc` development package for your target platform:

```bash
# For ARM 32-bit hard-float
sudo apt-get install -y libc6-dev-armhf-cross

# For ARM 64-bit
sudo apt-get install -y libc6-dev-arm64-cross
```

**Verify Toolchain Configuration:**

Ensure your CMake toolchain file specifies the correct sysroot and include paths:

```cmake
set(CMAKE_SYSROOT /usr/arm-linux-gnueabihf)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Issue 2: Failure to Cross-Compile with the System SQLite3

### Problem Description

The build process fails during cross-compilation when it links the `sqlite3` library installed on the host.

**Common error message:**
```
cannot find -lsqlite3: No such file or directory
```

This error appears even when the library is installed on the host system.

### Root Cause

This issue occurs when the system lacks a **target-compatible library**. For example:
- Host system runs on `x86_64` platform
- Cross-compiling for 32-bit ARM or ARM64
- Only the x86_64 version of `sqlite3` is installed

The linker cannot use an x86_64 library when building for ARM targets.

### Solution 1: Use the Bundled SQLite3 Sources

Areg SDK carries the SQLite3 sources in the `thirdparty` subdirectory. Build them for the target instead of
linking the host library:

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake \
  -DAREG_SYSTEM_SQLITE=OFF
```

This configuration compiles successfully without additional target libraries.

### Solution 2: Use a Target-Compatible Library

Install `sqlite3` for the target platform, for example with `vcpkg`:

1. **Install vcpkg** (if not already installed):
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh
   ```

2. **Install the library for the target platform**:
   ```bash
   # For ARM Linux target
   ./vcpkg install sqlite3:arm-linux

   # For ARM64 Linux target
   ./vcpkg install sqlite3:arm64-linux
   ```

3. **Configure CMake with vcpkg toolchain**:
   ```bash
   cmake -B build \
     -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake \
     -DVCPKG_TARGET_TRIPLET=arm-linux
   ```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Summary

This guide covered two main categories of CMake build issues on Linux:

1. **Missing Header Files**: System header file location or installation issues resolved by creating symbolic links or installing kernel/libc development packages

2. **Cross-Compilation Library Issues**: A target-incompatible `sqlite3` resolved by building the bundled sources or using a target-compatible library

### Key Takeaways

- ✅ Verify system headers are properly installed and linked
- ✅ Ensure cross-compilation toolchains include target-specific headers
- ✅ Use target-compatible libraries when cross-compiling
- ✅ Consider static linking to avoid runtime dependency issues
- ✅ Build the bundled SQLite3 sources when the target has no `sqlite3`

### Troubleshooting Checklist

**For Header File Issues:**
1. Check if header files exist: `ls /usr/include/asm/errno.h`
2. Verify symbolic links: `ls -l /usr/include/asm`
3. Install missing packages: `linux-libc-dev`, `kernel-headers`
4. Verify toolchain sysroot configuration

**For Cross-Compilation Library Issues:**
1. Identify target architecture: `arm-linux-gnueabihf`, `aarch64-linux-gnu`, etc.
2. Verify library availability for target: `find /usr -name libsqlite3.a`
3. Install or build target-compatible libraries
4. Update CMake `CMAKE_PREFIX_PATH` to include library locations
5. Consider using `vcpkg` for dependency management

### Additional Resources

For more information on CMake cross-compilation and toolchain configuration:
- [CMake Cross Compiling](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#cross-compiling)
- [vcpkg Package Manager](https://vcpkg.io/)
- [Building Static Libraries](https://cmake.org/cmake/help/latest/command/add_library.html)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech (Artak Avetyan), www.areg.tech, email: info[at]areg.tech
