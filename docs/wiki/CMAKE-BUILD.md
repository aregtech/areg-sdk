# Building AREG SDK Sources with CMake

## Note

This document is created to share and help the developers to build AREG SDK project using CMake tool in command line in Linux, Cygwin or Windows systems. The document also contains a brief information of building the project with CMake using Microsoft Visual Studio or Visual Studio Code. The other IDEs, operating systems or other tools are currently out the AREG SDK scope. But despite this, we invite developers to update this document and share more information of using other operating systems or tools to build the projects.

## Overview
The following instructions will guide you through cloning, configuring, and building the AREG SDK sources using CMake on both Linux and Windows platforms. Ensure you have installed the necessary dependencies and have a compatible compiler on your system.

### Prerequisites
- **CMake** version 3.20 or higher
- **Git** for cloning the repository
- **Compiler**: GNU or LLVM compilers for Linux or Cygwin, or LLVM or MSVC compilers for Windows
- **C++ Standard**: C++17
- **Additional Dependencies**:
  - **ncurses** under Linux build if build AREG extended objects;
  - **MFC** under Windows if build examples;
- **Optional Dependencies**:
  - **Google Test / GTest**: if build Unit Tests
  - **SQLite3**: if need to escape build from sources and use installed package.

---

## Build Options

The AREG SDK build supports Linux (POSIX API) and Windows (POSIX and Win32 API) platforms. For POSIX on Windows, use [_Cygwin_](https://www.cygwin.com/). Supported compilers include GNU `g++`, LLVM `clang++`/`clang-cl`, and MSVC `cl`.

### Supported Configurations:

| Compiler                      | Platform                  | API          | Notes                             |
| ----------------------------- | ------------------------- | ------------ | --------------------------------- |
| GNU `gcc/g++/cc/c++`          | Linux, Cygwin (Windows)   | POSIX        | Cygwin required for Windows       |
| LLVM `clang/clang++/clang-cl` | Linux                     | POSIX, Win32 | Linux, Windows (LLVM for Windows) |
| MSVC `cl`                     | Windows                   | Win32        | Windows only                      |

---

## Step 1: Update and Prepare the System

> [!NOTE]
> The code generator tool of AREG SDK requires Java installed on your system. The supported minimum version should be Java17.

### Install Dependencies for Linux

If you compile under Linux:
- Ensure you have CMake, GNU or LLVM compilers minimum supported standard C++17, Java 17 or higher, the `ncurses` library if you plan to compile AREG SDK with extended features and other essential build tools installed:
  ```bash
  sudo apt update
  sudo apt install -y cmake build-essential clang openjdk-17-jre libncurses-dev git
  ```
- **Optional**: you can download and install [Visual Studio Code](https://code.visualstudio.com/) if you prefer this IDE or want to debug using Visual Studio Code:
  - Open Visual Studio Code [download page](https://code.visualstudio.com/Download)
  - Select the installer for your system. For example, for Ubuntu, select `.deb` package and install by calling in command line:
    ```bash
    sudo apt install ./<package-file>.deb
    ```
    For more details, see the official [Visual Studio Code on Linux](https://code.visualstudio.com/docs/setup/linux) description.

### Install Dependencies for Windows

If you compile under Windows:
- Download and install [Microsoft Visual Studio](https://visualstudio.microsoft.com/). This will install Microsoft Visual C++ compiler. Additionally, install CMake and CLang packages if you want to compile with CLang;
- Download and install [Java](https://www.java.com/download/);
- **Optiponal**: Download and install [CMake](https://cmake.org/download/) if you did not install with *Microsoft Visual Studio*;
- **Optional**: you can download and install [Visual Studio Code](https://code.visualstudio.com/) if you prefer VSCode IDE to edit and compile. Additionally, install CMake and C++ extensions;
- **Optional**: if your system does not have Git, you can download and install [Git](https://git-scm.com/downloads/win).

### Install Dependencies for Cygwin

If you compile under Cygwin:
- Download and install [Cygwin](https://cygwin.com/install.html);
- Run the installer, folow instructions and install following packages: *cmake, make, dos2unix, flexdll, gcc-g++, git, ncurses, libncurses-devel*;
- **Optional**: alternatively you can also install Cygwin packages from command line:
  ```powershell
  c:\cygwin\setup.exe -qgnO -s http://mirrors.kernel.org/sourceware/cygwin/ -l C:\cygwin\ -P cmake, dos2unix, flexdll, gcc-g++, make, git, ncurses, libncurses-devel
  ```
  This assumes that the cygwin installer `setup.exe` is located in the `C:\cygwin` directory where you are going to install the packages.

### Install Dependencies for Windows Subsystem for Linux (WSL)

If you want to use Windows Subsystem for Linux (WSL) to compile sources for Linux system, follow the separate the detailed instruction [Build AREG SDK with Windows Subsystem for Linux (WSL)](./CMAKE-WSL.md) to update, install and prepare your system. If you have already installed and running WSL, make sure you have all above mentioned packages for your Linux system.

## Step 2: Clone the AREG SDK Repository

Now, when you have all tools, open a terminal and clone the AREG SDK repository using the following command:
```bash
git clone https://github.com/aregtech/areg-sdk.git
```

Navigate into the project directory:
```bash
cd areg-sdk
```

---

## Step 3: Configure and build AREG SDK sources with CMake

> [!IMPORTANT]
> Read [CMake Configuration Options for Building AREG SDK](./CMAKE-CONFIG.md) document to know more about AREG CMake settings, default values and options to change.

### Configure the Project with CMake

Run CMake to configure the project with default options*:
```bash
cmake -B ./build
```
This configures the AREG SDK build with default options. To know more about options and possible options, see [CMake Configuration Options for Building AREG SDK](./CMAKE-CONFIG.md) document.

### Optional: Select preferred options

If you want to build the AREG SDK with Debug version of binaries, examples and tests, you can use this command:
```bash
cmake -B ./build -DAREG_EXAMPLES:BOOL=ON -DAREG_TESTS:BOOL=ON -DAREG_BUILD_TYPE=Debug
```

If you want to build sources with particular compiler family like LLVM, you can use this command:
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=llvm
```

If you want to build source with particular compiler like `clang-cl` (on WIndows), you can use this command
```bash
cmake -B ./build -DREG_COMPILER=clang-cl
```
Make sure that `clang-cl` is accessible from command line. Otherwise, specify the compiler with the full path

### Build the Project

Use the following command to build the AREG SDK:
```bash
cmake --build ./build -j
```
This command utilizes all available CPU cores to speed up the build process.

### Optional: Run Tests

If you compiled the sources with enabled tests (option `-DAREG_TESTS:BOOL=ON`), you can run unit tests from command line by calling
```bash
ctest --test-dir ./build
```

If you need to output the test result in the file, for example `test_results.xml`, run following command:
```bash
ctest --test-dir ./build --output-on-failure --output-junit test_results.xml
```
Where the file `./build/test_results.xml` will contain the test results.

### Optional: Install the AREG SDK

To install the AREG SDK to your system (optional):
- For Linux:
   ```bash
   sudo cmake --install ./build
   ```
- For Windows: Open a command prompt or PowerShell with **Administrator rights**, navigate to the `areg-sdk` directory and run:
   ```powershell
   cmake --install ./build
   ```

## Optional: Configure and build with Microsoft Visual Studio or Visual Studio Code

If you prefer to configure and build with CMake using Microsoft Visual Studio or Visual Studio Code, follow this steps:
1. Open `<areg-sdk>` folder in preferred IDE;
2. Select the `CMakeLists.txt` located in the root directory of AREG_SDK and right click;
3. In the pop-up menu, select *Configure All Projects* (in VSCode) or *Configure Cache* (in MSVS);
4. Make changes by editing CMake Cache. Use VSCode or MSVS extension tools to set preferred settings;
5. Select again `CMakeLists.txt` file, right click and select *Build All Projects*.

For more information see the appropriate documentations for [Microsoft Visual Studio](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio) or for [Visual Studio Code](https://code.visualstudio.com/docs/cpp/cmake-quickstart).

## Optional: Configure and build using other tools and systems

At the moment, other IDEs or operating systems are out of the AREG SDK scope.

---

## Summary of Key Commands

| Platform    | Step                          | Command |
|-------------|-------------------------------|---------|
| **Linux**   | Install Dependencies          | `sudo apt install -y cmake build-essential clang openjdk-17-jre libncurses-dev git` |
| **Windows** | Install Dependencies          | Install Visual Studio with CLangCL compiler, CMake, Git and Java 17 or higher|
| *Cygwin*    | Install Dependencies          | `.\setup.exe -qgnO -s http://mirrors.kernel.org/sourceware/cygwin/ -l C:\cygwin\ -P cmake, dos2unix, flexdll, gcc-g++, make, git, ncurses, libncurses-devel` |
| **All**     | Clone Repository              | `git clone https://github.com/aregtech/areg-sdk.git` |
| **All**     | Configure Project             | `cmake -B ./build`                  |
| **All**     | Build Project                 | `cmake --build ./build -j`          |
| **All**     | Run Tests (*Optional*)        | `ctest --test-dir ./build`          |
| **Linux**   | Install AREG SDK (*Optional*) | `sudo cmake --install ./build`      |
| **Windows** | Install AREG SDK (*Optional*) | `cmake --install ./build`           |

---

These steps should guide you through successfully building and installing the AREG SDK on Linux and Windows. If any errors occur during the process, ensure that all dependencies are properly installed, and check the [official documentation](https://github.com/aregtech/areg-sdk) for further support.