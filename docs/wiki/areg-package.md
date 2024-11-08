# Installing and Using AREG SDK with vcpkg Package Manager

This document provides a comprehensive guide to installing the `vcpkg` package manager, setting up the `areg` package, and creating a CMake or Visual Studio project to use the AREG SDK effectively.

## Table of Contents
1. [Install vcpkg](#install-vcpkg)
2. [Install and Integrate the AREG Package](#install-and-integrate-the-areg-package)
3. [Setting Up a CMake Project to Use AREG](#setting-up-a-cmake-project-to-use-areg)
4. [Creating a Microsoft Visual Studio Project to Use AREG](#creating-a-microsoft-visual-studio-project-to-use-areg)

---

## AREG SDK General Requirements

Ensure the following dependencies are installed for AREG SDK integration:

- **CMake** version 3.20 or higher
- **Git** for repository management
- **Supported Compilers**: GCC, LLVM, or MSVC (Windows only) with **C++17** or newer
- **Java** version 17+ for code generation tools

## Install vcpkg

The `vcpkg` package manager is a tool for managing C++ libraries on multiple platforms. Here’s how to install `vcpkg` on your system.

1. **Clone the vcpkg repository**:
   Open a terminal (Command Prompt, PowerShell, or Terminal on macOS/Linux) and clone the repository with the following command:

   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   ```

2. **Bootstrap vcpkg**:
   Run the bootstrap script to build the `vcpkg` executable.

   - **On Windows**:
     ```powershell
     .\bootstrap-vcpkg.bat
     ```
   - **On Linux**:
     ```bash
     ./bootstrap-vcpkg.sh
     ```

3. **Add vcpkg to your PATH (optional)**:
   This allows you to run `vcpkg` from anywhere in the terminal.

   - **On Windows**:
     ```powershell
     set PATH=%PATH%;<path_to_vcpkg>
     ```
   - **On Linux**:
     ```bash
     export PATH=$PATH:<path_to_vcpkg>
     ```

4. **Integrate `vcpkg` with your system**:
   Run the following command to integrate `vcpkg`.
   ```bash
   vcpkg integrate install
   ```
   This also displays the full path to the CMake toolchain file to set `CMAKE_TOOLCHAIN_FILE` option.

---

## Install and Integrate the AREG Package

The `areg` package can be installed via `vcpkg` to easily manage dependencies and link with your projects.

1. **Install the `areg` package**:
   With `vcpkg` installed, run the following command to install `areg`.
   ```bash
   vcpkg install areg
   ```
   This downloads, builds, and makes `areg` package available for use.

2. **Integrate `areg`**:
   **CMake / Visual Studio integration**:
     ```bash
     vcpkg integrate install
     ```
     This command integrates with Visual Studio (Windows-only) and displays the full path to the CMake toolchain file to set `CMAKE_TOOLCHAIN_FILE` option.

---

## 3. Setting Up a CMake Project to Use AREG

Once `vcpkg` and the `areg` package are installed, you can set up a CMake project that uses the AREG Framework.

### Steps to Create a CMake Project

1. **Create a Project Directory**:
   ```bash
   mkdir example
   cd example
   ```
2. **Add Source Code**
   Create a simple `main.cpp` file that uses a component from areg framework:
  ```cpp
  #include "areg/base/String.hpp"

  int main() {
      String str("Hello from AREG SDK!");
      std::cout << str.getData() << std::endl;
      return 0;
  }
  ```

3. **Create a `CMakeLists.txt` File**:
   In the project directory, create a `CMakeLists.txt` file with the following content :

   ```cmake
   cmake_minimum_required(VERSION 3.20)
   project(example)
   find_package(areg CONFIG REQUIRED)
   add_executable(example main.cpp)
   target_link_libraries(example PRIVATE areg::areg)
   ```

4. **Build the Project**:
   From the command line, run the following commands to configure and build your project (replace `<path-to-vcpkg-root>` with full path):
   ```bash
   cmake -B ./build -DCMAKE_TOOLCHAIN_FILE="<path-to-vcpkg-root>/scripts/buildsystems/vcpkg.cmake"
   cmake --build ./build
   ```

   This compiles `example` project and link with `areg` library.

---

## 4. Creating a Microsoft Visual Studio Project to Use AREG

`vcpkg` also integrates smoothly with Visual Studio, making it easy to add packages like `areg` to your C++ projects.

### Steps to Set Up a Visual Studio Project

1. **Open Visual Studio**:
   Start a new project and select **C++ Console App**.

2. **Add Source Code**
   Create a simple `main.cpp` file that uses a component from areg framework:
  ```cpp
  #include "areg/base/String.hpp"
  #pragma commend(lib, "areg")

  int main() {
      String str("Hello from AREG SDK!");
      std::cout << str.getData() << std::endl;
      return 0;
  }
  ```
  If you don't want to have `#pragma commend(lib, "areg")` instruction, link with `areg` library by using **Project => Properties** dialog.

3. **Configure and build**:
   If you ran the `vcpkg integrate install` command, Visual Studio should already detect packages installed through `vcpkg`, including `areg`. Build and run your project. Visual Studio will handle linking with `areg` and any additional configurations needed via `vcpkg`.
