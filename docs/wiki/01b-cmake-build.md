# Building AREG SDK with CMake

## Introduction
This guide provides step-by-step instructions for building the **AREG SDK** on Linux, Windows, and Cygwin platforms using **CMake**. It also covers IDE setups for **Microsoft Visual Studio** and **Visual Studio Code**. Follow these steps to set up your build environment and efficiently compile AREG SDK projects. For troubleshooting, refer to the [AREG SDK documentation](https://github.com/aregtech/areg-sdk).

## Table of Contents
- [Building AREG SDK with CMake](#building-areg-sdk-with-cmake)
  - [Introduction](#introduction)
  - [Table of Contents](#table-of-contents)
  - [System Requirements](#system-requirements)
    - [General Requirements](#general-requirements)
    - [Platform-Specific Requirements](#platform-specific-requirements)
  - [Configuration and Build Steps](#configuration-and-build-steps)
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
  - [Additional IDE Configurations](#additional-ide-configurations)

---

## System Requirements

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

## Configuration and Build Steps

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
To customize the build, modify options as needed:
```bash
cmake -B ./build -DAREG_BUILD_TYPE=Debug -DAREG_EXAMPLES=ON -DAREG_TESTS=ON
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

## Additional IDE Configurations

For **Microsoft Visual Studio** or **Visual Studio Code**:
1. Open the `<areg-sdk>` directory in your IDE.
2. Right-click `CMakeLists.txt` and select *Configure*.
3. Adjust [AREG SDK settings](./02a-cmake-config.md) in the CMake cache if necessary, then build the project directly in the IDE.

**Further Resources**:
For additional setup information, refer to [Visual Studio CMake Projects](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio) or [VS Code CMake Quickstart](https://code.visualstudio.com/docs/cpp/cmake-quickstart).
