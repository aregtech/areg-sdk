Here’s an optimized and professional version of the **"Building AREG SDK with CMake"** guide:

---

# Building AREG SDK with CMake

## Introduction
This guide provides clear, step-by-step instructions for building the **AREG SDK** across Linux, Windows, and Cygwin platforms using **CMake**. It also covers IDE setups for **Microsoft Visual Studio** and **Visual Studio Code**. Follow these steps to set up the build environment and compile AREG SDK projects efficiently.

## Table of Contents
- [System Requirements](#system-requirements)
  - [General Requirements](#general-requirements)
  - [Platform-Specific Requirements](#platform-specific-requirements)
- [Configuration and Build Steps](#configuration-and-build)
  - [Step 1: Installing Dependencies](#step-1-installing-dependencies)
  - [Step 2: Cloning the AREG SDK Repository](#step-2-cloning-the-areg-sdk-repository)
  - [Step 3: Configuring the Build](#step-3-configuring-the-build)
  - [Step 4: Building the Project](#step-4-building-the-project)
  - [Step 5: Running Tests and Installing](#step-5-running-tests-and-installing)
- [Additional IDE Configurations](#additional-ide-configurations)

---

## System Requirements

### General Requirements
Ensure your system includes the following:
- **CMake** version 3.20+
- **Git** for cloning the repository
- **Compatible Compilers**: *GNU*, *LLVM*, or *MSVC* (Windows only) with **C++17** or newer
- **Java** (version 17+ for code generation tools)

### Platform-Specific Requirements
- **Linux**: Install **ncurses** (required by `aregextend`).
- **Windows**: Requires Microsoft Visual C++ and **MFC** for GUI examples.
- **Optional Libraries**:
  - **Google Test (GTest)** for unit testing (or build from sources).
  - **SQLite3** (if not building from sources, available in the `thirdparty` directory of AREG SDK).

If your system does not meet these requirements, proceed to [Step 1: Installing Dependencies](#step-1-installing-dependencies).

---

## Configuration and Build

Follow these instructions to configure and build AREG SDK on your system.

---

### Step 1: Installing Dependencies

#### Linux
To install required packages:
```bash
sudo apt update
sudo apt install -y git cmake build-essential clang openjdk-17-jre libncurses-dev
```
This command installs essential tools and libraries, including `ncurses` for Linux builds.

#### Windows
1. Download and install [Visual Studio](https://visualstudio.microsoft.com/) with CMake and Clang.
2. Install [Java](https://www.java.com/download/) and [Git](https://git-scm.com/download/win).

After installing these tools, Windows is ready for AREG SDK builds.

#### Cygwin
Install required packages with **[Cygwin installer](https://cygwin.com/install.html)** or via command:
```powershell
c:\cygwin\setup.exe -qgnO -s http://mirrors.kernel.org/sourceware/cygwin/ -l C:\cygwin\ -P cmake, dos2unix, flexdll, gcc-g++, make, git, ncurses, libncurses-devel
```

---

### Step 2: Cloning the AREG SDK Repository

Clone the AREG SDK repository to access the latest source code:
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
To customize the build, modify options:
```bash
cmake -B ./build -DAREG_BUILD_TYPE=Debug -DAREG_EXAMPLES=ON -DAREG_TESTS=ON
```
For additional configurations, refer to the [CMake Configuration Guide](./cmake-config.md).

---

### Step 4: Building the Project

Compile AREG SDK:
```bash
cmake --build ./build -j
```
This command utilizes available cores for a faster build process.

---

### Step 5: Running Tests and Installing

#### Run Unit Tests:
To execute unit tests (if enabled):
```bash
ctest --test-dir ./build
```
To output test results to a file:
```bash
ctest --test-dir ./build --output-on-failure --output-junit test_results.xml
```

#### Install AREG SDK:

Install AREG SDK binaries and header files required to develop AREG Framework based multithreaded and multiprocessing applications.

- **Linux**:
  ```bash
  sudo cmake --install ./build
  ```
- **Windows** (run as Administrator):
  ```powershell
  cmake --install ./build
  ```

---

## Additional IDE Configurations

For **Microsoft Visual Studio** or **Visual Studio Code** users:
1. Open the `<areg-sdk>` directory in your IDE.
2. Right-click `CMakeLists.txt` and select *Configure*.
3. Adjust [AREG SDK settings](./cmake-config.md) in the CMake cache if needed, then build the project directly in the IDE.

**Further Resources**:
For additional setup details, refer to [Visual Studio CMake Projects](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio) or [VS Code CMake Quickstart](https://code.visualstudio.com/docs/cpp/cmake-quickstart).

--- 

This guide provides comprehensive instructions to set up, build, and optionally test the AREG SDK on various platforms. For troubleshooting, refer to the [AREG SDK documentation](https://github.com/aregtech/areg-sdk).
