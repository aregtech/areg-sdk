# Building AREG SDK with CMake

## Introduction
This guide details how to build the **AREG SDK** on Linux, Windows, or Cygwin using **CMake**. Follow these instructions to ensure a seamless build environment setup and project compilation. While the focus is on command-line builds, it also briefly covers building with **Microsoft Visual Studio** and **Visual Studio Code** using **CMake** build tool.

## Table of Contents
- [Building AREG SDK with CMake](#building-areg-sdk-with-cmake)
  - [Introduction](#introduction)
  - [Table of Contents](#table-of-contents)
  - [System Requirement](#system-requirement)
    - [General Requirements](#general-requirements)
    - [Platform-Specific Requirements](#platform-specific-requirements)
  - [Configuration and Build](#configuration-and-build)
    - [Step 1: Installing Dependencies](#step-1-installing-dependencies)
      - [Linux](#linux)
      - [Windows](#windows)
      - [Cygwin](#cygwin)
    - [Step 2: Clone AREG SDK Repository](#step-2-clone-areg-sdk-repository)
    - [Step 3: Configuring the Build](#step-3-configuring-the-build)
    - [Step 4: Building the Project](#step-4-building-the-project)
    - [Step 5 (Optional): Running Tests and Installation](#step-5-optional-running-tests-and-installation)
  - [Additional IDE Configurations](#additional-ide-configurations)

---

## System Requirement

### General Requirements
Before building, ensure your system meets these prerequisites:
- **CMake** version 3.20+
- **Git** for source code cloning
- **Compatible Compilers**: *GNU*, *LLVM*, or *MSVC* (Windows only) supporting standard **C++17** or newer
- **Java** (version 17+ for code generation tools)

### Platform-Specific Requirements
- **Linux**: Install **ncurses** for extended builds (extended objects of `aregextend` library).
- **Windows**: Microsoft Visual C++ and **MFC** for example builds.
- **Optional**: 
   - **Google Test (GTest)** package if going to build unit tests and if not going to build from sources;
   - **SQLite3** package if not going to build from sources, located in the `thirdparty` directory of AREG SDK.

Follow [Step 1: Installing Dependencies](#step-1-installing-dependencies) if you system does not match the requirement.

---

## Configuration and Build

The followings are step-by-step explanations how to configure and build AREG SDK projects.

### Step 1: Installing Dependencies

Escape this step if your system matches all prerequisites. Otherwise, follow the instructions to install all dependencies in your system. Here is brief instructions to prepare the system.

#### Linux
Update and install required tools:
```bash
sudo apt update
sudo apt install -y cmake build-essential clang openjdk-17-jre libncurses-dev git
```

#### Windows
1. [Install Visual Studio](https://visualstudio.microsoft.com/) with CMake and Clang components.
2. Install [Java](https://www.java.com/download/) and [Git](https://git-scm.com/download/win).

#### Cygwin
Use the **[Cygwin installer](https://cygwin.com/install.html)** or the following command to install required packages:
```powershell
c:\cygwin\setup.exe -qgnO -s http://mirrors.kernel.org/sourceware/cygwin/ -l C:\cygwin\ -P cmake, dos2unix, flexdll, gcc-g++, make, git, ncurses, libncurses-devel
```

---

### Step 2: Clone AREG SDK Repository
Clone the AREG SDK repository to your system:
```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```

---

### Step 3: Configuring the Build

Run CMake to initialize build configuration with default options:
```bash
cmake -B ./build
```

To customize the build, modify options such as:
```bash
cmake -B ./build -DAREG_BUILD_TYPE=Debug -DAREG_EXAMPLES=ON -DAREG_TESTS=ON
```
For further customization, refer to the [CMake Configuration Guide](./cmake-config.md).

---

### Step 4: Building the Project

Compile the AREG SDK sources:
```bash
cmake --build ./build -j
```
This utilizes all available cores for faster builds.

---

### Step 5 (Optional): Running Tests and Installation

**Run Unit Tests**:

To run unit tests (if enabled), execute:
```bash
ctest --test-dir ./build
```
To run unit tests and to save test results to a file:
```bash
ctest --test-dir ./build --output-on-failure --output-junit test_results.xml
```

**Install AREG SDK**:
- **Linux**:
  ```bash
  sudo cmake --install ./build
  ```
- **Windows**: Run with Administrator privileges:
  ```powershell
  cmake --install ./build
  ```

---

## Additional IDE Configurations

For **Microsoft Visual Studio** or **Visual Studio Code** users:
1. Open `<areg-sdk>` directory in your IDE.
2. Right-click `CMakeLists.txt` and select *Configure*.
3. Adjust [AREG SDK settings](./cmake-config.md) in the CMake cache if needed, then build the project from the IDE.

**For further guidance** on Visual Studio setup, check [Visual Studio CMake Projects](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio), and for VS Code, see [VS Code CMake Quickstart](https://code.visualstudio.com/docs/cpp/cmake-quickstart).

---

This guide provides a comprehensive overview of setting up, building, and optionally testing AREG SDK on multiple platforms. For troubleshooting, consult the [AREG SDK documentation](https://github.com/aregtech/areg-sdk).

