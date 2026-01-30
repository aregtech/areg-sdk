# Building Areg SDK on Windows Subsystem for Linux (WSL)

This guide covers setting up WSL, building Areg SDK on Linux distributions within Windows, and troubleshooting common WSL issues.

---

## Table of Contents

1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [WSL Setup](#wsl-setup)
4. [Build Instructions](#build-instructions)
5. [Cross-Compilation](#cross-compilation)
6. [Running Applications](#running-applications)
7. [Troubleshooting](#troubleshooting)

---

## Overview

Windows Subsystem for Linux (WSL) enables running Linux distributions natively on Windows 10 (version 2004+) and Windows 11. WSL provides:

✅ **Native Linux development** on Windows  
✅ **Full Linux toolchain** (GCC, Clang, CMake)  
✅ **No dual-boot required** - seamless integration  
✅ **File system access** between Windows and Linux

**Supported distributions:** Ubuntu, Debian, openSUSE, Kali Linux, and more.

**Recommended:** WSL 2 for better performance and full system call compatibility.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Quick Start

**Build Areg SDK on WSL in 5 commands:**

```bash
# Install WSL with Ubuntu (if not already installed)
wsl --install -d ubuntu

# Inside WSL terminal
sudo apt-get update && sudo apt-get install -y git cmake build-essential clang libncurses-dev openjdk-17-jre
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
cmake -B ./build && cmake --build ./build -j20
```

**Binaries location:**
```
./product/build/<compiler>/<platform>-<arch>-<config>-shared/bin/
```

**Example:** `./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/`

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## WSL Setup

### Step 1: Install WSL

**Check if WSL is installed:**

```powershell
wsl --version
```

**Expected output:**
```
WSL version: 2.0.x
Kernel version: 5.15.x
...
```

**If not installed:**

```powershell
wsl --install -d ubuntu
```

**This command:**
- Enables WSL feature on Windows
- Installs WSL 2
- Downloads and installs Ubuntu
- Creates a user account

**Installation time:** 5-15 minutes depending on internet speed.

> [!NOTE]
> Requires Windows 10 version 2004+ or Windows 11. Administrator privileges required.

### Step 2: Set WSL Version to WSL 2

**WSL 2 offers:**
- Better performance
- Full Linux kernel
- Better compatibility

**Set default version:**

```powershell
wsl --set-default-version 2
```

**Verify WSL version:**

```powershell
wsl --list --verbose
```

**Expected output:**
```
  NAME      STATE           VERSION
* Ubuntu    Running         2
```

### Step 3: Update Ubuntu

**Inside WSL terminal:**

```bash
sudo apt-get update && sudo apt-get upgrade -y
```

**Expected output:**
```
Hit:1 http://archive.ubuntu.com/ubuntu jammy InRelease
Get:2 http://archive.ubuntu.com/ubuntu jammy-updates InRelease [119 kB]
...
Reading package lists... Done
```

**Update time:** 2-5 minutes for first-time setup.

### Step 4: Install Build Tools

**Install essential development tools:**

```bash
sudo apt-get install -y git cmake build-essential clang libncurses-dev openjdk-17-jre
```

**What gets installed:**
- `git` - Version control
- `cmake` - Build system (3.20+)
- `build-essential` - GCC compiler and tools
- `clang` - Alternative C++ compiler
- `libncurses-dev` - Terminal UI library (for `aregextend`)
- `openjdk-17-jre` - Java runtime for code generator

**Verify installation:**

```bash
gcc --version
cmake --version
java -version
```

**Expected outputs:**
```
gcc (Ubuntu 11.4.0) 11.4.0
cmake version 3.22.1
openjdk version "17.0.x"
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Build Instructions

### Step 1: Clone Repository

**Option A: Clone directly in WSL (recommended)**

```bash
mkdir ~/projects && cd ~/projects
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```

**Option B: Use existing Windows clone**

If Areg SDK is already cloned in Windows (e.g., `C:\projects\areg-sdk\`):

```bash
cd /mnt/c/projects/areg-sdk
```

> [!TIP]
> WSL mounts Windows drives under `/mnt/`. Access `C:\` as `/mnt/c/`, `D:\` as `/mnt/d/`, etc.

### Step 2: Build with Default Configuration

**Build with GCC (default):**

```bash
cmake -B ./build
cmake --build ./build -j20
```

**Expected output:**
```
-- The CXX compiler identification is GNU 11.4.0
-- Configuring done
-- Generating done
-- Build files written to: /home/user/projects/areg-sdk/build
[  1%] Building CXX object framework/areg/CMakeFiles/areg.dir/base/File.cpp.o
...
[100%] Built target areg
```

**Build time:** 3-8 minutes depending on CPU cores.

### Step 3: Build with Clang (Optional)

**Configure and build with Clang:**

```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DAREG_BUILD_TYPE=Release
cmake --build ./build -j20
```

**Verify compiler used:**

```bash
grep "The CXX compiler" ./build/CMakeCache.txt
```

**Expected output:**
```
CMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++
```

### Step 4: Verify Build

**Check built binaries:**

```bash
ls ./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/
```

**Expected output:**
```
01_minimalrpc  02_minimalipc  ...  mtrouter  logcollector  ...
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Cross-Compilation

WSL supports cross-compilation for different architectures.

### Cross-Compile for 32-bit x86

**1. Install 32-bit development libraries:**

```bash
sudo apt-get install -y gcc-multilib g++-multilib
```

**2. Configure for 32-bit:**

```bash
cmake -B ./build -DAREG_PROCESSOR=x86 -DAREG_COMPILER_FAMILY=llvm
```

**3. Build:**

```bash
cmake --build ./build -j20
```

**4. Verify binary architecture:**

```bash
file ./product/build/llvm-clang/linux-32-x86-release-shared/bin/mtrouter
```

**Expected output:**
```
mtrouter: ELF 32-bit LSB pie executable, Intel 80386, version 1 (GNU/Linux), ...
```

**Alternative verification:**

```bash
od -t x1 -t c ./product/build/llvm-clang/linux-32-x86-release-shared/bin/mtrouter | head -n 2
```

**Expected output:**
```
0000000  7f  45  4c  46  01  01  01  00  00  00  00  00  00  00  00  00
        177   E   L   F 001 001 001  \0  \0  \0  \0  \0  \0  \0  \0  \0
```

The 5th byte `01` indicates 32-bit (`02` would indicate 64-bit). See [ELF Header format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header).

> [!IMPORTANT]
> If you encounter "could not find `<asm/errno.h>`" error, create a symbolic link:
> ```bash
> sudo ln -s /usr/include/asm-generic/ /usr/include/asm
> ```

### Cross-Compile for ARM (32-bit)

**1. Install ARM toolchain:**

```bash
sudo apt-get install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf binutils-arm-linux-gnueabihf
```

> [!TIP]
> Use `gcc-arm-linux-gnueabihf` for modern ARM processors with hardware floating-point. For older processors, use `gcc-arm-linux-gnueabi`.

**2. Configure for ARM:**

```bash
cmake -B ./build -DAREG_PROCESSOR=arm -DAREG_COMPILER_FAMILY=gnu
```

**3. Build:**

```bash
cmake --build ./build -j20
```

**4. Verify:**

```bash
file ./product/build/gnu-g++/linux-32-arm-release-shared/bin/mtrouter
```

**Expected output:**
```
mtrouter: ELF 32-bit LSB executable, ARM, EABI5 version 1 (GNU/Linux), ...
```

### Cross-Compile for AArch64 (64-bit ARM)

**1. Install AArch64 toolchain:**

```bash
sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu
```

**2. Configure for AArch64:**

```bash
cmake -B ./build -DAREG_PROCESSOR=aarch64 -DAREG_COMPILER_FAMILY=gnu
```

**3. Build:**

```bash
cmake --build ./build -j20
```

**4. Verify:**

```bash
file ./product/build/gnu-g++/linux-64-aarch64-release-shared/bin/mtrouter
```

**Expected output:**
```
mtrouter: ELF 64-bit LSB executable, ARM aarch64, version 1 (GNU/Linux), ...
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running Applications

### Step 1: Navigate to Areg SDK Directory

```bash
cd ~/projects/areg-sdk
```

Or if using Windows clone:

```bash
cd /mnt/c/projects/areg-sdk
```

### Step 2: Run Example Application

**Run the minimal RPC example:**

```bash
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/01_minimalrpc
```

**Expected output:**
```
'Hello Service!'
Exiting application ...
```

**Run the local service example:**

```bash
./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/10_locservice
```

**Expected output:**
```
A Demo to demonstrate simple request, response, and broadcast ...
"Hello client [ TestServiceClient ]!", remaining [ 36 ] to process.
...
Exit application; check logs for details.
```

> [!NOTE]
> For complete list of examples and detailed instructions, see [Examples README](../../examples/README.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### WSL Installation Issues

**Problem:** `wsl --install` command not recognized.

**Solution:**

Ensure Windows is updated to version 2004 or higher:

1. **Windows + R** → type `winver` → press Enter
2. Check version number
3. If below 2004, update Windows via **Settings → Update & Security**

### WSL Update Errors

**Problem:** Error code `Wsl/Service/CreateInstance/0x80040326`.

**Solution:**

Update WSL:

```powershell
wsl --update
```

**Then restart WSL:**

```powershell
wsl --shutdown
wsl
```

### DNS Resolution Failures

**Problem:** `apt-get update` fails with:
```
W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/jammy/InRelease
Temporary failure resolving 'archive.ubuntu.com'
```

**Solution:**

**Check current DNS:**

```bash
cat /etc/resolv.conf
```

**If showing incorrect DNS (e.g., `nameserver 172.23.112.1`):**

**Option 1: Edit manually**

```bash
sudo vim /etc/resolv.conf
```

Change to Google DNS:
```
nameserver 8.8.8.8
```

Save (`Esc` → `:wq` → `Enter`)

**Option 2: Use command**

```bash
sudo sh -c "echo nameserver 8.8.8.8 > /etc/resolv.conf"
```

**Verify:**

```bash
ping -c 3 google.com
```

**Expected output:**
```
PING google.com (142.250.x.x) 56(84) bytes of data.
64 bytes from ...
```

### Build Tool Not Found

**Problem:** `cmake: command not found` or `gcc: command not found`.

**Solution:**

Reinstall build tools:

```bash
sudo apt-get update
sudo apt-get install -y git cmake build-essential clang libncurses-dev openjdk-17-jre
```

Verify installation:

```bash
which cmake gcc clang java
```

**Expected output:**
```
/usr/bin/cmake
/usr/bin/gcc
/usr/bin/clang
/usr/bin/java
```

### ncurses Not Found

**Problem:** CMake configuration fails with "ncurses not found".

**Solution:**

Install ncurses development package:

```bash
sudo apt-get install -y libncurses-dev
```

**Or disable extended library:**

```bash
cmake -B ./build -DAREG_EXTENDED=OFF
```

### Cross-Compilation Toolchain Missing

**Problem:** Build fails for ARM with "arm-linux-gnueabihf-g++: not found".

**Solution:**

Install the appropriate cross-compilation toolchain:

**For 32-bit ARM:**
```bash
sudo apt-get install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
```

**For 64-bit ARM:**
```bash
sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

**Verify toolchain:**

```bash
arm-linux-gnueabihf-g++ --version
```

or

```bash
aarch64-linux-gnu-g++ --version
```

### Permission Denied Errors

**Problem:** Cannot write to `/mnt/c/` directories.

**Solution:**

**Check Windows folder permissions:**
1. Right-click folder in Windows Explorer
2. **Properties → Security**
3. Ensure your user has Write permission

**Or work in WSL home directory:**

```bash
mkdir ~/projects
cd ~/projects
git clone https://github.com/aregtech/areg-sdk.git
```

### Slow Build Performance

**Problem:** Build takes excessively long (>15 minutes).

**Solution:**

**1. Ensure using WSL 2:**
```powershell
wsl --list --verbose
```

If VERSION shows `1`, convert to WSL 2:
```powershell
wsl --set-version Ubuntu 2
```

**2. Build from WSL filesystem:**

Use `~/projects/` instead of `/mnt/c/` for better performance.

**3. Increase WSL memory:**

Create/edit `C:\Users\<YourUsername>\.wslconfig`:

```ini
[wsl2]
memory=8GB
processors=4
```

Restart WSL:
```powershell
wsl --shutdown
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**WSL Documentation:**
- [Microsoft WSL Install Guide](https://learn.microsoft.com/en-us/windows/wsl/install)
- [WSL Best Practices](https://learn.microsoft.com/en-us/windows/wsl/setup/environment)
- [WSL Troubleshooting](https://learn.microsoft.com/en-us/windows/wsl/troubleshooting)

**Areg SDK Build Guides:**
- [CMake Build Guide](./02b-cmake-build.md) - Complete CMake build instructions
- [CMake Configuration](./02d-cmake-config.md) - Configuration options
- [Linux Build Troubleshooting](./07b-troubleshooting-cmake-linux-builds.md)

**Examples:**
- [Areg SDK Examples](../../examples/README.md) - Sample applications and usage

**Help:**
If you encounter issues not covered here, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
