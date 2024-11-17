# Troubleshooting CMake Builds on Linux

This guide provides solutions for common issues encountered when compiling projects with CMake on Linux, helping to ensure a smooth and successful build process.

---

## Table of Contents

1. [Issue 1: Missing `<asm/errno.h> File` File](#issue-1-missing-asmerrnoh-file)
   - [Solution: Create a Symbolic Link](#solution-create-a-symbolic-link)
   - [Solution 2: Install Missing Libraries](#solution-2-install-missing-libraries)
   - [Solution 3: Check Toolchain and Cross-Compilation Settings](#solution-3-check-toolchain-and-cross-compilation-settings)
2. [Issue 2: Failure to Cross-Compile with Extended Objects Enabled](#issue-3-failure-to-cross-compile-with-extended-objects-enabled)
   - [Solution 1: Use Target-Compatible Libraries](#solution-1-use-target-compatible-libraries)
   - [Solution 2: Pre-Build Required Libraries](#solution*2-pre-build-required-libraries)
   - [Solution 3: Disable Extended Objects](#solution-3-disable-extended-objects)

---

## Issue 1: Missing `<asm/errno.h>` File

If you encounter an error during compilation that states `<asm/errno.h>` could not be found, this typically indicates that certain header files are either missing or not properly linked in the system's include directories.

### Solution 1: Create a Symbolic Link

> [!NOTE]
> Administrator (root) permissions are required for this solution.

To resolve this issue, create a symbolic link from the `asm-generic` directory to the `asm` directory in `/usr/include`. This ensures the compiler can locate the required header files.

1. **Open Terminal and Run the Command**:
   ```bash
   sudo ln -s /usr/include/asm-generic/ /usr/include/asm
   ```
   This command creates a symbolic link named `asm` in `/usr/include`, pointing to the `asm-generic` directory.

2. **Retry Compilation**: After creating the symbolic link, rerun the compilation command. The error should be resolved.

### Solution 2: Install Missing Libraries

In some cases, the `asm/errno.h` file is provided by specific system packages. Installing the appropriate package can resolve the issue. For example, on Debian-based systems:
```bash
sudo apt-get install -y linux-libc-dev
```

### Solution 3: Check Toolchain and Cross-Compilation Settings

If you are cross-compiling, verify that the toolchain being used includes the necessary headers for the target architecture. You may need to install a compatible `libc` development package for your target platform. For example, for ARM targets:
```bash
sudo apt-get install -y libc6-dev-armhf-cross
```

---

## Issue 2: Failure to Cross-Compile with Extended Objects Enabled

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
