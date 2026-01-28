# Troubleshooting CMake Builds on Linux

This guide provides solutions for common issues encountered when compiling projects with CMake on Linux, helping to ensure a smooth and successful build process for Areg SDK applications.

---

## Table of Contents

1. [Issue 1: Missing `<asm/errno.h>` File](#issue-1-missing-asmerrnoh-file)
2. [Issue 2: Failure to Cross-Compile with Extended Objects Enabled](#issue-2-failure-to-cross-compile-with-extended-objects-enabled)
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

## Issue 2: Failure to Cross-Compile with Extended Objects Enabled

### Problem Description

The build process fails during cross-compilation when Areg SDK extended objects are enabled (option `-DAREG_EXTENDED=ON`).

**Common error messages:**
```
cannot find -lncurses: No such file or directory
cannot find -lsqlite3: No such file or directory
```

These errors appear even when the libraries are installed on the host system.

### Root Cause

This issue occurs when the system lacks **target-compatible libraries**. For example:
- Host system runs on `x86_64` platform
- Cross-compiling for 32-bit ARM or ARM64
- Only x86_64 versions of `ncurses` and `sqlite3` are installed
- ARM-compatible versions are missing

The linker cannot use x86_64 libraries when building for ARM targets.

### Solution 1: Use Target-Compatible Libraries

Ensure that the necessary libraries are available for your target platform. You can install them using a package manager like `vcpkg` to compile dependencies for your target architecture.

**Using vcpkg:**

1. **Install vcpkg** (if not already installed):
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh
   ```

2. **Install libraries for target platform**:
   ```bash
   # For ARM Linux target
   ./vcpkg install sqlite3:arm-linux
   ./vcpkg install ncurses:arm-linux
   
   # For ARM64 Linux target
   ./vcpkg install sqlite3:arm64-linux
   ./vcpkg install ncurses:arm64-linux
   ```

3. **Configure CMake with vcpkg toolchain**:
   ```bash
   cmake -B build \
     -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake \
     -DVCPKG_TARGET_TRIPLET=arm-linux \
     -DAREG_EXTENDED=ON
   ```

### Solution 2: Pre-Build Required Libraries

Manually build and install the required libraries (`ncurses`, `sqlite3`, etc.) for the target platform using your cross-compilation toolchain.

**Building ncurses for ARM:**

1. **Download ncurses source**:
   ```bash
   wget https://ftp.gnu.org/gnu/ncurses/ncurses-6.4.tar.gz
   tar -xzf ncurses-6.4.tar.gz
   cd ncurses-6.4
   ```

2. **Configure for cross-compilation**:
   ```bash
   ./configure \
     --host=arm-linux-gnueabihf \
     --prefix=/usr/arm-linux-gnueabihf \
     --enable-static \
     --disable-shared
   ```

3. **Build and install**:
   ```bash
   make
   sudo make install
   ```

**Building SQLite3 for ARM:**

1. **Download SQLite3 source**:
   ```bash
   wget https://www.sqlite.org/2024/sqlite-autoconf-3450000.tar.gz
   tar -xzf sqlite-autoconf-3450000.tar.gz
   cd sqlite-autoconf-3450000
   ```

2. **Configure for cross-compilation**:
   ```bash
   ./configure \
     --host=arm-linux-gnueabihf \
     --prefix=/usr/arm-linux-gnueabihf \
     --enable-static \
     --disable-shared
   ```

3. **Build and install**:
   ```bash
   make
   sudo make install
   ```

**Update CMake configuration:**

Ensure the paths to these libraries are correctly set in your build environment:

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake \
  -DCMAKE_PREFIX_PATH=/usr/arm-linux-gnueabihf \
  -DAREG_EXTENDED=ON
```

> [!TIP]
> For simplicity and portability, consider building **statically linked** versions of `ncurses` and `sqlite3` for your target platform. This approach avoids runtime dependency issues on the target system.

### Solution 3: Disable Extended Objects

If the extended features of the Areg SDK are not required for your application, you can disable them by setting `-DAREG_EXTENDED=OFF`.

**Benefits:**
- Removes dependency on `ncurses` library
- Uses Areg SDK built-in SQLite3 sources available in the `thirdparty` subdirectory
- Simplifies cross-compilation process

**Configuration:**

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake \
  -DAREG_EXTENDED=OFF
```

This configuration:
- Removes `ncurses` dependency entirely
- Uses bundled SQLite3 implementation from `thirdparty/` directory
- Compiles successfully without additional target libraries

> [!NOTE]
> Disabling extended objects removes advanced logging and debugging features that depend on `ncurses`. Evaluate whether your application requires these features before disabling.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Summary

This guide covered two main categories of CMake build issues on Linux:

1. **Missing Header Files**: System header file location or installation issues resolved by creating symbolic links or installing kernel/libc development packages

2. **Cross-Compilation Library Issues**: Target-incompatible library dependencies resolved by using target-compatible libraries, pre-building dependencies, or disabling extended features

### Key Takeaways

- ✅ Verify system headers are properly installed and linked
- ✅ Ensure cross-compilation toolchains include target-specific headers
- ✅ Use target-compatible libraries when cross-compiling
- ✅ Consider static linking to avoid runtime dependency issues
- ✅ Disable extended features if not needed to simplify builds

### Troubleshooting Checklist

**For Header File Issues:**
1. Check if header files exist: `ls /usr/include/asm/errno.h`
2. Verify symbolic links: `ls -l /usr/include/asm`
3. Install missing packages: `linux-libc-dev`, `kernel-headers`
4. Verify toolchain sysroot configuration

**For Cross-Compilation Library Issues:**
1. Identify target architecture: `arm-linux-gnueabihf`, `aarch64-linux-gnu`, etc.
2. Verify library availability for target: `find /usr -name libncurses.a`
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

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
