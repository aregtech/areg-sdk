# Building Areg SDK with CMake

This guide covers building Areg SDK on Linux, Windows, and Cygwin using CMake, including cross-compilation for different architectures and IDE setup for Visual Studio and VS Code.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Quick Start](#quick-start)
3. [Step-by-Step Build Instructions](#step-by-step-build-instructions)
4. [Cross-Compilation](#cross-compilation)
5. [IDE Configuration](#ide-configuration)
6. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Required Tools

- **CMake 3.20+** - Build system generator
- **Git** - Version control
- **C++17 Compiler** - GCC, Clang/LLVM, or MSVC
- **Java 17+** - For code generation tools

### Platform-Specific Requirements

**Linux:**
- `ncurses` library (required by `aregextend`)
- GCC or Clang toolchain

**Windows:**
- Visual Studio with C++ workload
- CMake tools for Visual Studio
- Clang compiler for Windows (optional)
- MFC libraries (for GUI examples)

**Optional:**
- **Google Test** - For unit tests (or built from sources)
- **SQLite3** - For database features (or use bundled version in `thirdparty/`)

> [!TIP]
> If dependencies are already installed, skip to [Quick Start](#quick-start).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Quick Start

**Build Areg SDK in 4 commands:**

```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
cmake -B ./build
cmake --build ./build -j20
```

**Binaries location:**
- **Linux:** `./product/build/<compiler>/<platform>-<arch>-release-shared/bin/`
- **Windows:** `.\product\build\<compiler>\<platform>-<arch>-release-shared\bin\`

**Example:** `./product/build/gnu-g++/linux-64-x86_64-release-shared/bin/`

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Step-by-Step Build Instructions

### Step 1: Install Dependencies

#### Linux (Ubuntu/Debian)

```bash
sudo apt-get update && sudo apt-get upgrade -y
sudo apt-get install -y git cmake build-essential clang libncurses-dev openjdk-17-jre
```

**What gets installed:**
- `git` - Version control
- `cmake` - Build system
- `build-essential` - GCC compiler and tools
- `clang` - Alternative C++ compiler
- `libncurses-dev` - Terminal UI library (required by `aregextend`)
- `openjdk-17-jre` - Java runtime for code generator

> [!NOTE]
> For cross-compilation, additional toolchains may be required. See [Cross-Compilation](#cross-compilation).

#### Windows

1. **Install Visual Studio:**
   - Download from [visualstudio.microsoft.com](https://visualstudio.microsoft.com/)
   - Select **Desktop development with C++** workload
   - Include **CMake tools** and **Clang compiler for Windows**

2. **Install Java:**
   - Download from [java.com/download](https://www.java.com/download/)
   - Verify installation: `java -version`

3. **Install Git:**
   - Download from [git-scm.com/download/win](https://git-scm.com/download/win)
   - Verify installation: `git --version`

#### Cygwin

Install packages via [Cygwin installer](https://cygwin.com/install.html) or command line:

```powershell
c:\cygwin\setup.exe -qgnO -s http://mirrors.kernel.org/sourceware/cygwin/ -l C:\cygwin\ -P cmake,dos2unix,flexdll,gcc-g++,make,git,ncurses,libncurses-devel
```

**Packages installed:**
- `cmake`, `make` - Build tools
- `gcc-g++` - C++ compiler
- `git` - Version control
- `ncurses`, `libncurses-devel` - Terminal library

### Step 2: Clone Repository

```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```

**Expected output:**
```
Cloning into 'areg-sdk'...
remote: Enumerating objects: 12345, done.
remote: Counting objects: 100% (1234/1234), done.
...
```

### Step 3: Configure Build

**Basic configuration (uses defaults):**

```bash
cmake -B ./build
```

**Custom configuration example:**

```bash
cmake -B ./build \
  -DAREG_BUILD_TYPE=Debug \
  -DAREG_EXAMPLES=OFF \
  -DAREG_TESTS=OFF
```

**Common configuration options:**

| Option | Values | Description |
|--------|--------|-------------|
| `AREG_BUILD_TYPE` | `Release`, `Debug` | Build configuration |
| `AREG_EXAMPLES` | `ON`, `OFF` | Build example projects |
| `AREG_TESTS` | `ON`, `OFF` | Build unit tests |
| `AREG_EXTENDED` | `ON`, `OFF` | Build extended library (requires ncurses) |
| `AREG_LOGGER` | `ON`, `OFF` | Enable logging infrastructure |

> [!TIP]
> For complete configuration options, see [CMake Configuration Guide](./02d-cmake-config.md).

**Expected output:**
```
-- The CXX compiler identification is GNU 11.4.0
-- Detecting CXX compiler ABI info
-- Configuring done
-- Generating done
-- Build files written to: /path/to/areg-sdk/build
```

### Step 4: Build Project

**Build with all available CPU cores:**

```bash
cmake --build ./build -j20
```

**Build with specific number of cores:**

```bash
cmake --build ./build -j20
```

**Build specific target:**

```bash
cmake --build ./build --target areg
```

**Expected output:**
```
[  1%] Building CXX object framework/areg/CMakeFiles/areg.dir/base/File.cpp.o
[  2%] Building CXX object framework/areg/CMakeFiles/areg.dir/base/Process.cpp.o
...
[100%] Built target areg
```

**Build time:** 2-10 minutes depending on configuration and CPU cores.

### Step 5: Run Tests (Optional)

**Run all unit tests:**

```bash
ctest --test-dir ./build
```

**Run tests with detailed output:**

```bash
ctest --test-dir ./build --output-on-failure
```

**Save test results to JUnit XML:**

```bash
ctest --test-dir ./build --output-on-failure --output-junit test_results.xml
```

**Expected output:**
```
Test project /path/to/areg-sdk/build
    Start 1: NEMath.StringTest
1/5 Test #1: NEMath.StringTest ................   Passed    0.12 sec
    Start 2: NEMath.BufferTest
2/5 Test #2: NEMath.BufferTest ................   Passed    0.08 sec
...
100% tests passed, 0 tests failed out of 5
```

### Step 6: Install (Optional)

Install Areg SDK libraries and headers system-wide for use in other projects.

**Linux:**
```bash
sudo cmake --install ./build
```

**Windows (run as Administrator):**
```powershell
cmake --install ./build
```

**Default installation locations:**
- **Linux:** `/usr/local/lib/`, `/usr/local/include/`
- **Windows:** `C:\Program Files (x86)\areg\`

**Custom installation prefix:**
```bash
cmake --install ./build --prefix /opt/areg
```

> [!NOTE]
> System-wide installation is optional. You can use Areg SDK directly from the build directory or via [CMake FetchContent](./02b-cmake-integrate.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Cross-Compilation

Cross-compilation enables building Areg SDK for architectures different from your host system.

### Supported Platforms and Architectures

| Compiler | Platform | API | CPU Architecture |
|----------|----------|-----|------------------|
| **GCC** | Linux, macOS | POSIX | x86, x86_64, arm, aarch64 |
| **Clang** | Linux, Windows | POSIX, Win32 | x86, x86_64, arm, aarch64 |
| **MSVC** | Windows | Win32 | x86, x86_64 |
| **Cygwin GCC** | Windows | POSIX | x86, x86_64 |

> [!NOTE]
> Clang cross-compilation for ARM processors has been tested only on Linux.

### Important Considerations

**Dependencies:**

The core Areg Framework has no external dependencies, but the extended library (`aregextend`) requires:
- `ncurses` (Linux/Cygwin)
- `sqlite3` (optional, or use bundled version)

When cross-compiling, ensure target platform has these libraries or disable `aregextend`:

```bash
cmake -B ./build -DAREG_EXTENDED=OFF -DAREG_PROCESSOR=arm
```

**Binary Compatibility:**

Use `macro_check_module_architect` in your CMake scripts to validate library compatibility with target architecture. See [CMake Functions Guide](./02e-cmake-functions.md#macro_check_module_architect).

**Toolchain Files:**

For complex cross-compilation, use CMake toolchain files. Examples available in [conf/export/example/toolchains/](../../conf/export/example/toolchains/).

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

The 5th byte `01` indicates 32-bit, `02` indicates 64-bit. See [ELF Header format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header).


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

## IDE Configuration

### Visual Studio

**1. Open Areg SDK in Visual Studio:**
   - **File → Open → Folder**
   - Navigate to `areg-sdk` directory

**2. Configure CMake:**
   - Right-click `CMakeLists.txt` → **CMake Settings**
   - Adjust configuration options as needed
   - Visual Studio auto-detects CMake projects

**3. Build:**
   - **Build → Build All** (or `Ctrl+Shift+B`)

**4. Run examples:**
   - Select target from dropdown (e.g., `01_minimalrpc`)
   - **Debug → Start Without Debugging** (`Ctrl+F5`)

> [!TIP]
> For detailed Visual Studio CMake setup, see [Visual Studio CMake Projects](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio).


### Visual Studio Code

**1. Install extensions:**
   - **C/C++ Extension Pack** (Microsoft)
   - **CMake Tools** (Microsoft)

**2. Open Areg SDK:**
   - **File → Open Folder**
   - Select `areg-sdk` directory

**3. Configure CMake:**
   - Press `Ctrl+Shift+P` → **CMake: Configure**
   - Select compiler kit (GCC, Clang, or MSVC)

**4. Build:**
   - Press `F7` or click **Build** in status bar

**5. Debug:**
   - Set breakpoints in source files
   - Press `F5` to start debugging

> [!TIP]
> For VS Code CMake setup, see [VS Code CMake Quickstart](https://code.visualstudio.com/docs/cpp/cmake-quickstart).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### Build Configuration Issues

**Problem:** CMake cannot find compiler.

**Solution:** Ensure compiler is in PATH or specify explicitly:

```bash
cmake -B ./build -DCMAKE_CXX_COMPILER=/usr/bin/g++-11
```

### Missing Dependencies

**Problem:** `ncurses` not found during configuration.

**Solution:** Install ncurses development package:

**Linux:**
```bash
sudo apt-get install libncurses-dev
```

**Or disable extended library:**
```bash
cmake -B ./build -DAREG_EXTENDED=OFF
```


### Java Not Found

**Problem:** Code generator fails with "Java not found".

**Solution:** Install Java 17+ and verify:

```bash
java -version
```

Should output Java version 17 or higher. Install from [Adoptium](https://adoptium.net/) or package manager.


### Cross-Compilation Errors

**Problem:** Build fails when cross-compiling.

**Solution:**
1. Verify toolchain is installed:
   ```bash
   arm-linux-gnueabihf-g++ --version
   ```
2. Check target dependencies are available
3. Use toolchain file for complex setups:
   ```bash
   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=conf/export/example/toolchains/arm-linux.cmake
   ```

### Windows Build Issues

**Problem:** Build fails with "MSVC not found".

**Solution:**
1. Launch **Developer Command Prompt for VS**
2. Navigate to `areg-sdk` directory
3. Run CMake commands from this prompt

**Or specify generator explicitly:**
```powershell
cmake -B ./build -G "Visual Studio 17 2022"
```

### Unit Test Failures

**Problem:** Some unit tests fail.

**Solution:**
1. Run tests with verbose output:
   ```bash
   ctest --test-dir ./build --output-on-failure --verbose
   ```
2. Check test logs in `./build/Testing/Temporary/LastTest.log`
3. Report issues at [areg-sdk discussions](https://github.com/aregtech/areg-sdk/discussions)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Build Guides:**
- [CMake Configuration Options](./02d-cmake-config.md) - Complete configuration reference
- [CMake Integration Guide](./02b-cmake-integrate.md) - Using Areg SDK in your projects
- [vcpkg Installation](./02d-cmake-vcpkg.md) - Alternative installation method

**Troubleshooting:**
- [Linux Build Troubleshooting](./07b-troubleshooting-cmake-linux-builds.md)
- [Integration Troubleshooting](./07c-troubleshooting-integration.md)

**Help:**
If you encounter issues not covered here, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
