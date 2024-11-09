# Installing and Using AREG SDK with vcpkg Package Manager

This document provides a comprehensive guide for installing the `vcpkg` package manager, setting up the `areg` package, and creating projects with CMake or Visual Studio to effectively use the AREG SDK.

## Table of Contents
- [AREG SDK General Requirements](#areg-sdk-general-requirements)
- [Install vcpkg](#install-vcpkg)
- [Install and Integrate the areg Package](#install-and-integrate-the-areg-package)
- [Setting Up a CMake Project to Use areg](#setting-up-a-cmake-project-to-use-areg)
- [Creating a Microsoft Visual Studio Project to Use areg](#creating-a-microsoft-visual-studio-project-to-use-areg)

---

## AREG SDK General Requirements

Ensure the following dependencies are installed to integrate the AREG SDK:

- **CMake** version 3.20 or higher
- **Git** for repository management
- **Supported Compilers**: GCC, LLVM, or MSVC (Windows only) with **C++17** or newer
- **Java** version 17+ for code generation tools

---

## Install vcpkg

The [`vcpkg` package manager](https://github.com/microsoft/vcpkg) helps manage C++ libraries across multiple platforms. Follow these steps to install `vcpkg`:

1. **Clone the `vcpkg` repository**:
   Open a terminal (Command Prompt, PowerShell, or Terminal on macOS/Linux) and run:

   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   ```

2. **Bootstrap vcpkg**:
   Build the `vcpkg` executable by running the bootstrap script.

   - **On Windows**:
     ```powershell
     .\bootstrap-vcpkg.bat
     ```
   - **On Linux/macOS**:
     ```bash
     ./bootstrap-vcpkg.sh
     ```

3. **Add vcpkg to PATH (Optional)**:
   Adding `vcpkg` to your PATH allows you to run it from any terminal session.

   - **On Windows**:
     ```powershell
     set PATH=%PATH%;<path_to_vcpkg>
     ```
   - **On Linux/macOS**:
     ```bash
     export PATH=$PATH:<path_to_vcpkg>
     ```

4. **Integrate `vcpkg` with your system**:
   To enable seamless use with Visual Studio, run:

   ```bash
   vcpkg integrate install
   ```
   This command configures Visual Studio to automatically detect and use libraries installed with `vcpkg`. This also displays the full path to the CMake toolchain file to set `CMAKE_TOOLCHAIN_FILE` option.

---

## Install and Integrate the areg Package

After installing `vcpkg`, you can install the `areg` package, which provides the AREG SDK libraries, header files, CMake configurations and tools.

1. **Install the `areg` Package**:
   Run the following command to download, build, and install the AREG SDK development components.

   ```bash
   vcpkg install areg
   ```

2. **Verify Integration**:
   If using Visual Studio, ensure `vcpkg integrate install` was run as described in the previous section. This integration makes `areg` available to all Visual Studio projects on the system. For CMake projects, the CMake toolchain file provided by `vcpkg` must be set (option `CMAKE_TOOLCHAIN_FILE`) when configuring the project.

---

## Setting Up a CMake Project to Use areg

Once `vcpkg` and the `areg` package are installed, you can create a CMake project that links with the AREG SDK libraries.

Steps to Create a CMake Project:

1. **Create a Project Directory**:
   ```bash
   mkdir example
   cd example
   ```

2. **Add Source Code**:
   Create a `main.cpp` file that uses components from the areg framework:

   ```cpp
   #include "areg/base/String.hpp"

   int main() {
       String str("Hello from AREG SDK!");
       std::cout << str.getData() << std::endl;
       return 0;
   }
   ```

3. **Create a `CMakeLists.txt` File**:
   In the project directory, create a `CMakeLists.txt` file:

   ```cmake
   cmake_minimum_required(VERSION 3.20)
   project(example)

   # Enable vcpkg
   find_package(areg CONFIG REQUIRED)
   
   add_executable(example main.cpp)
   target_link_libraries(example PRIVATE areg::areg)
   ```

4. **Build the Project**:
   Run the following commands to configure and build your project. Replace `<path-to-vcpkg-root>` with the path to your `vcpkg` installation.

   ```bash
   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE="<path-to-vcpkg-root>/scripts/buildsystems/vcpkg.cmake"
   cmake --build ./build
   ```

This will compile the `example` project and link it with the `areg` library.

---

## Creating a Microsoft Visual Studio Project to Use areg

`vcpkg` also integrates seamlessly with Visual Studio, making it easy to add packages like `areg` to C++ projects.

Steps to Set Up a Visual Studio Project:

1. **Open Visual Studio**:
   Start a new project and select **C++ Console Application**.

2. **Add Source Code**:
   Create a simple `main.cpp` file in your project with the following content:

   ```cpp
   #include "areg/base/String.hpp"
   #include <iostream>

   int main() {
       String str("Hello from AREG SDK!");
       std::cout << str.getData() << std::endl;
       return 0;
   }
   ```

3. **Configure and Build**:
   - If you ran `vcpkg integrate install`, Visual Studio should automatically detect and use packages installed through `vcpkg`, including `areg`.
   - Build and run your project. Visual Studio will handle linking with `areg` and other configurations through `vcpkg`.

   If `vcpkg` is not detected, verify the integration by checking that `vcpkg integrate install` was executed and restart Visual Studio if needed.
