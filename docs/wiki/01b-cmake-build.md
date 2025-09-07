# Building AREG SDK with CMake

## Introduction
This guide provides step-by-step instructions for building the **AREG SDK** on Linux, Windows, and Cygwin platforms using **CMake**. It also covers IDE setups for **Microsoft Visual Studio** and **Visual Studio Code**. Follow these steps to set up your build environment and efficiently compile AREG SDK projects. For troubleshooting, refer to the [AREG SDK documentation](https://github.com/aregtech/areg-sdk).

## Table of Contents
1. [System Requirements](#1-system-requirements)
   - [General Requirements](#general-requirements)
   - [Platform-Specific Requirements](#platform-specific-requirements)
2. [Configuration and Build Steps](#2-configuration-and-build-steps)
   - [Step 1: Installing Dependencies](#step-1-installing-dependencies)
     - [Linux](#linux)
     - [Windows](#windows)
     - [Cygwin](#cygwin)
   - [Step 2: Cloning the AREG SDK Repository](#step-2-cloning-the-areg-sdk-repository)
   - [Step 3: Configuring the Build](#step-3-configuring-the-build)
   - [Step 4: Building the Project](#step-4-building-the-project)
   - [Step 5: Running Tests and Installing](#step-5-running-tests-and-installing)
     - [Run Unit Tests:](#run-unit-tests)
     - [Install AREG SDK:](#install-areg-sdk)
3. [Cross-Compiling AREG SDK](#3-cross-compiling-areg-sdk)
   - [Supported Compilers,[ Platforms, and Architectures](#supported-compilers-platforms-and-architectures)
   - [Cross-Compiling for 32-bit Systems](#cross-compiling-for-32-bit-systems)
   - [Cross-Compiling for ARM Processors](#cross-compiling-for-arm-processors)
   - [Cross-Compiling for AARCH64 (64-bit ARM)](#cross-compiling-for-aarch64-64-bit-arm)
4. [Additional IDE Configurations](#4-additional-ide-configurations)
5. [Troubleshooting](#5-Troubleshooting)

---

## 1. System Requirements

### General Requirements
Ensure your system includes the following:
- **CMake** (version 3.20+)
- **Git** for repository cloning
- **Compatible Compilers**: *GNU*, *LLVM*, or *MSVC* (Windows only) supporting **C++17** or newer
- **Java** (version 17+ for code generation tools)

### Platform-Specific Requirements
- **Linux**: Install **ncurses** (required by `aregextend` extended objects).
- **Windows**: Requires Microsoft Visual C++, including packages **CMake** and **CLang compiler for Windows**, and **MFC** for GUI examples.
- **Optional Libraries**:
  - **Google Test (GTest)** for unit tests (or build from sources).
  - **SQLite3** (optional, or use the version in AREG SDK's `thirdparty` directory).

If your system does not meet these requirements, proceed to [Step 1: Installing Dependencies](#step-1-installing-dependencies); otherwise, start from [Step 2: Cloning the AREG SDK Repository](#step-2-cloning-the-areg-sdk-repository).

---

## 2. Configuration and Build Steps

Follow these steps to configure and build the AREG SDK on your system.

---

### Step 1: Installing Dependencies

#### Linux
To install the necessary packages:
```bash
sudo apt-get update && sudo apt-get upgrade -y
sudo apt-get install -y git cmake build-essential clang libncurses-dev openjdk-17-jre
```
This command installs essential tools and libraries, including `ncurses` for Linux builds.

> [!NOTE]
> You may need additional dependencies if compile for different target (**cross-compiling**). For additional information, refer to [Cross-Compiling AREG SDK](#crosscompiling-areg-sdk) section.

#### Windows
1. Download and install [Visual Studio](https://visualstudio.microsoft.com/), including packages **CMake** and **CLang compiler for Windows**.
2. Install [Java](https://www.java.com/download/) and [Git](https://git-scm.com/download/win).

After installing these tools, Windows will be ready for AREG SDK builds.

#### Cygwin
Install required packages with the **[Cygwin installer](https://cygwin.com/install.html)** or by running:
```powershell
c:\cygwin\setup.exe -qgnO -s http://mirrors.kernel.org/sourceware/cygwin/ -l C:\cygwin\ -P cmake, dos2unix, flexdll, gcc-g++, make, git, ncurses, libncurses-devel
```

---

### Step 2: Cloning the AREG SDK Repository

Clone the AREG SDK repository to obtain the latest source code:
```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```
Navigate to the project directory to proceed with build commands.

---

### Step 3: Configuring the Build

Initialize build configurations with default settings:
```bash
cmake -B ./build
```
To customize the build, modify options as needed. Below is an example of configuring and building the AREG SDK sources in Debug mode, without Unit Tests and Examples:
```bash
cmake -B ./build -DAREG_BUILD_TYPE=Debug -DAREG_EXAMPLES=OFF -DAREG_TESTS=OFF
```
For additional configurations, refer to the [CMake Configuration Guide](./02a-cmake-config.md).

---

### Step 4: Building the Project

Compile the AREG SDK:
```bash
cmake --build ./build -j
```
This command utilizes available cores to speed up the build process.

---

### Step 5: Running Tests and Installing

#### Run Unit Tests:
To execute unit tests (if enabled):
```bash
ctest --test-dir ./build
```
To save test results to a file:
```bash
ctest --test-dir ./build --output-on-failure --output-junit test_results.xml
```

#### Install AREG SDK:

Install AREG SDK binaries and headers to develop multithreaded and multiprocessing applications based on the AREG Framework.

- **Linux**:
  ```bash
  sudo cmake --install ./build
  ```
- **Windows** (run as Administrator):
  ```powershell
  cmake --install ./build
  ```

---

## 3. Cross-Compiling AREG SDK

Cross-compiling enables building applications for architectures different from the native environment. Below are instructions for configuring AREG SDK to target 32-bit systems, as well as ARM and AARCH64 (64-bit ARM) processors.

### Supported Compilers, Platforms, and Architectures

| **Compiler** | **Platform**    | **API**        | **CPU Architecture**      |
|--------------|-----------------|----------------|---------------------------|
| GNU          | Linux, macOS    | POSIX          | x86, x86_64, arm, aarch64 |
| Clang        | Linux, Windows  | POSIX, Win32   | x86, x86_64, arm, aarch64 |
| MSVC         | Windows         | Win32          | x86, x86_64               |
| Cygwin GNU   | Windows         | POSIX          | x86, x86_64               |

> [!NOTE]
> Compilation with **Clang** compiler for all specified processors where tested only under Linux platform.

**Important Notes**

- **Dependencies:**
  While the core AREG SDK has no external dependencies, the extended library `aregextend` may require additional libraries. When cross-compiling, consider the following:
  - If unsure about the availability of required libraries on the target platform, set `AREG_EXTENDED` to `OFF`:
    ```bash
    cmake -DAREG_EXTENDED=OFF <source-dir>
    ```
  - Ensure that dependencies like `ncurses` or `sqlite3` (if SQLite3 is used) are available for the target platform. Missing dependencies will cause the build to fail.

- **Binary Compatibility:**
  Use the `macro_check_module_architect` macro in your CMake script to validate the compatibility of dependent libraries with the target processor architecture. Provide the full path to the library (static or shared), the target name, and the processor architecture as arguments. For detailed usage, see the [macro_check_module_architect documentation](./02b-cmake-functions.md#macro_check_module_architect).

> [!TIP]
> To simplify cross-compilation, create or use pre-existing toolchain files. Examples of toolchain files are available in the [toolchain directory](../../conf/export/example/toolchains/) for reference. These files help configure the compiler and architecture settings for the desired target platform.

### Cross-Compiling for 32-bit Systems

To compile AREG SDK for a 32-bit system, you need to specify the target processor and ensure you have the correct 32-bit libraries installed.

**Steps**

1. **Install Required Libraries**
  ```bash
  sudo apt-get install -y gcc-multilib g++-multilib
  ```
2. **Configure CMake for 32-bit x86 Architecture**
  Optionally select compiler (here it is `clang++`)
  ```bash
  cmake -B ./build -DAREG_PROCESSOR=x86 -DAREG_COMPILER_FAMILY=llvm
  ```
3. **Build AREG SDK**
  ```bash
  cmake --build ./build -j20
  ```

To verify that a binary is 32-bit, navigate to the build directory and run:
```bash
file ./mtrouter.elf
```
This command should output something like:
> ./mtrouter.elf: ELF **32-bit** LSB pie executable, **Intel 80386**, version 1 (GNU/Linux), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=3df1d5e3d1b90b9533b93a906cece6ff95fa816c, for GNU/Linux 3.2.0, not stripped

Alternatively, you can run:
```bash
od -t x1 -t c ./mtrouter | head -n 2
```
In the ELF Header, the 5th byte `001` indicates a 32-bit executable, while `002` indicates 64-bit.
> ```plaintext
> 0000000  7f  45  4c  46  01  01  01  00  00  00  00  00  00  00  00  00
>         177   E   L   F 001 001 001  \0  \0  \0  \0  \0  \0  \0  \0  \0
> ```
For further details, refer to the **[ELF Header](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#File_header)** Wikipedia page.

### Cross-Compiling for ARM Processors

Cross-compiling for ARM processors requires an ARM-compatible toolchain and configuring CMake for ARM architecture.

**Steps**

1. **Install ARM Toolchain**
  ```bash
  sudo apt-get install -y gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf binutils-arm-linux-gnueabihf
  ```
  > [!NOTE]
  > For most 32-bit modern ARM processors, `gcc-arm-linux-gnueabihf` is recommended over `gcc-arm-linux-gnueab`.

2. **Configure CMake for ARM Architecture**
  ```bash
  cmake -B ./build -DAREG_PROCESSOR=arm -DAREG_COMPILER_FAMILY=gnu
  ```
3. **Build AREG SDK**
  ```bash
  cmake --build ./build -j 20
  ```

Verify the architecture of the compiled binary with:
```bash
file ./mtrouter.elf
```
This should output message `ELF 32-bit LSB executable, ARM, ...`.

### Cross-Compiling for AARCH64 (64-bit ARM)

For AARCH64 architecture, install the 64-bit ARM toolchain and configure CMake accordingly.

**Steps**
1. **Install AARCH64 Toolchain**
  ```bash
  sudo apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu
  ```

2. **Configure CMake for AARCH64 Architecture**
  ```bash
  cmake -B ./build -DAREG_PROCESSOR=aarch64 -DAREG_COMPILER_FAMILY=gnu
  ```

3. **Build AREG SDK**
  ```bash
  cmake --build ./build -j 20
  ```

Verify the binary architecture:
```bash
file ./mtrouter.elf
```
This should output message `ELF 64-bit LSB executable, ARM aarch64, ...`.

---

## 4. Additional IDE Configurations

For **Microsoft Visual Studio** or **Visual Studio Code**:
1. Open the `<areg-sdk>` directory in your IDE.
2. Right-click `CMakeLists.txt` and select *Configure*.
3. Adjust [AREG SDK settings](./02a-cmake-config.md) in the CMake cache if necessary, then build the project directly in the IDE.

**Further Resources**:
For additional setup information, refer to [Visual Studio CMake Projects](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio) or [VS Code CMake Quickstart](https://code.visualstudio.com/docs/cpp/cmake-quickstart).

---

## 5. Troubleshooting

If you have difficulties to compile AREG SDK binaries or integrate in your project, refer to the [Troubleshooting CMake Builds on Linux](./07b-troubleshooting-cmake-linux-builds.md) or [Integration Troubleshooting](./07c-troubleshooting-integration.md) documents. If your problem is not listed or you could not solve the problem, open a [topic for discussion](https://github.com/aregtech/areg-sdk/discussions) at `areg-sdk` repository.

---
