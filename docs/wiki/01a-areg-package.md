# Installing Areg SDK with vcpkg Package Manager

This guide covers installing vcpkg, setting up the Areg SDK package, and creating projects with CMake or Visual Studio.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Why Use vcpkg](#why-use-vcpkg)
3. [Install vcpkg](#install-vcpkg)
4. [Install Areg SDK Package](#install-areg-sdk-package)
5. [Create a CMake Project](#create-a-cmake-project)
6. [Create a Visual Studio Project](#create-a-visual-studio-project)
7. [Troubleshooting](#troubleshooting)

---

## Prerequisites

Before installing Areg SDK via vcpkg, ensure you have:

- **CMake 3.20+** - Build system generator
- **Git** - Version control (required by vcpkg)
- **C++17 Compiler** - GCC, Clang/LLVM, or MSVC
- **Java 17+** - Required for Areg code generation tools

**Supported Platforms:** Windows, Linux, macOS  
**Supported Architectures:** x86, x86_64, ARM, AArch64

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Why Use vcpkg

[vcpkg](https://github.com/microsoft/vcpkg) is Microsoft's cross-platform C++ package manager that simplifies dependency management:

✅ **Cross-platform** - Works on Windows, Linux, and macOS  
✅ **CMake integration** - Automatic library detection  
✅ **Visual Studio integration** - Zero-configuration package usage  
✅ **Reproducible builds** - Consistent library versions across teams

**Alternative installation methods:** See [CMake FetchContent Integration](./02b-cmake-integrate.md) for direct source integration.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Install vcpkg

### Step 1: Clone vcpkg Repository

Open your terminal (Command Prompt, PowerShell, or Bash) and run:

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
```

### Step 2: Bootstrap vcpkg

Build the vcpkg executable:

**Windows:**
```powershell
.\bootstrap-vcpkg.bat
```

**Linux/macOS:**
```bash
./bootstrap-vcpkg.sh
```

### Step 3: Add vcpkg to PATH (Optional)

For convenient access from any directory, add vcpkg to your system PATH.

**Windows (PowerShell):**
```powershell
$env:PATH += ";<path-to-vcpkg-root>"
```

**Linux/macOS:**
```bash
export PATH=$PATH:<path-to-vcpkg-root>
```

Replace `<path-to-vcpkg-root>` with the actual vcpkg installation path.

> [!TIP]
> Add the PATH export to your shell configuration file (~/.bashrc, ~/.zshrc, or PowerShell profile) for persistence.

### Step 4: Integrate with Build Tools

For seamless Visual Studio integration, run:

```bash
vcpkg integrate install
```

**This command:**
- Configures Visual Studio to auto-detect vcpkg packages
- Displays the CMake toolchain file path (save this for CMake projects)
- Requires administrator/sudo privileges on some systems

**Example output:**
```
Applied user-wide integration for this vcpkg root.

All MSBuild C++ projects can now #include any installed libraries.
Linking will be handled automatically.
Installing new libraries will make them instantly available.

CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Install Areg SDK Package

Once vcpkg is installed and integrated, install the Areg SDK:

```bash
vcpkg install areg
```

**What gets installed:**
- Areg Framework libraries (`areg.lib` or `libareg.a`)
- C++ header files
- CMake configuration files
- Code generation tools (`codegen.jar`)

**Installation time:** 2-5 minutes depending on your system.

### Verify Installation

Check that Areg SDK is installed:

```bash
vcpkg list | grep areg
```

Expected output:
```
areg:x64-windows    1.5.0    Areg SDK - Object RPC framework
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Create a CMake Project

### Step 1: Create Project Directory

```bash
mkdir my-areg-project
cd my-areg-project
```

### Step 2: Write Application Code

Create `main.cpp`:

```cpp
#include "areg/base/String.hpp"
#include <iostream>

int main() {
    String greeting("Hello from Areg SDK!");
    std::cout << greeting.getData() << std::endl;
    return 0;
}
```

### Step 3: Create CMakeLists.txt

Create `CMakeLists.txt` in your project root:

```cmake
cmake_minimum_required(VERSION 3.20)
project(my-areg-project)

# Find Areg SDK package
find_package(areg CONFIG REQUIRED)

# Create executable
add_executable(my-areg-project main.cpp)

# Link with Areg library
target_link_libraries(my-areg-project PRIVATE areg::areg)
```

### Step 4: Configure and Build

**Configure CMake:**
```bash
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE="<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake"
```

**Build the project:**
```bash
cmake --build ./build
```

**Run the executable:**

**Windows:**
```powershell
.\build\Debug\my-areg-project.exe
```

**Linux/macOS:**
```bash
./build/my-areg-project
```

**Expected output:**
```
Hello from Areg SDK!
```

> [!NOTE]
> Replace `<path-to-vcpkg>` with your actual vcpkg installation path (shown during `vcpkg integrate install`).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Create a Visual Studio Project

Visual Studio automatically detects vcpkg packages after running `vcpkg integrate install`.

### Step 1: Create New Project

1. Open Visual Studio
2. **File → New → Project**
3. Select **Console App (C++)**
4. Name your project and click **Create**

### Step 2: Add Areg SDK Code

Replace the contents of `main.cpp`:

```cpp
#include "areg/base/String.hpp"
#include <iostream>

int main() {
    String greeting("Hello from Areg SDK!");
    std::cout << greeting.getData() << std::endl;
    return 0;
}
```

### Step 3: Build and Run

1. **Build → Build Solution** (or press `Ctrl+Shift+B`)
2. **Debug → Start Without Debugging** (or press `Ctrl+F5`)

Visual Studio automatically:
- Finds Areg SDK headers
- Links with Areg libraries
- Copies required DLLs (Windows)

**Expected output:**
```
Hello from Areg SDK!
```

> [!TIP]
> If headers are not found, verify `vcpkg integrate install` was executed and restart Visual Studio.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### vcpkg Command Not Found

**Problem:** Terminal doesn't recognize `vcpkg` command.

**Solution:** Either navigate to vcpkg directory or add it to PATH:

```bash
cd <path-to-vcpkg>
./vcpkg install areg
```

Or add vcpkg to PATH as described in [Step 3](#step-3-add-vcpkg-to-path-optional).

### CMake Cannot Find areg Package

**Problem:** CMake reports `Could not find a package configuration file provided by "areg"`.

**Solution:** Ensure CMAKE_TOOLCHAIN_FILE points to vcpkg:

```bash
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE="<path-to-vcpkg>/scripts/buildsystems/vcpkg.cmake"
```

Get the exact path from `vcpkg integrate install` output.

### Visual Studio Cannot Find Areg Headers

**Problem:** IntelliSense shows errors for `#include "areg/base/String.hpp"`.

**Solution:**
1. Verify `vcpkg integrate install` was executed
2. Restart Visual Studio
3. Check vcpkg integration status:
   ```bash
   vcpkg integrate install
   ```
4. If issues persist, remove and re-add integration:
   ```bash
   vcpkg integrate remove
   vcpkg integrate install
   ```

### Wrong Architecture or Platform

**Problem:** Build fails with architecture mismatch errors.

**Solution:** Specify triplet explicitly when installing:

**For 64-bit Windows:**
```bash
vcpkg install areg:x64-windows
```

**For 64-bit Linux:**
```bash
vcpkg install areg:x64-linux
```

**For macOS:**
```bash
vcpkg install areg:x64-osx
```

### Java Not Found During Installation

**Problem:** vcpkg reports Java is required but not found.

**Solution:** Install Java 17+ and ensure it's in PATH:

```bash
java -version
```

Should output Java version 17 or higher. If not, install from [Adoptium](https://adoptium.net/) or your system package manager.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Next Steps

After successfully installing Areg SDK with vcpkg:

1. **Explore Examples:** [Areg SDK Examples](https://github.com/aregtech/areg-sdk/tree/master/examples)
2. **Learn Service Design:** [Service Interface Guide](./06d-setup-lusan.md)
3. **Build Distributed Apps:** [IPC and Networking](./05a-mtrouter.md)
4. **Integration Options:** [CMake FetchContent](./02b-cmake-integrate.md) for source-based integration

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## See Also

- [vcpkg Official Documentation](https://vcpkg.io/en/getting-started.html)
- [Areg SDK GitHub Repository](https://github.com/aregtech/areg-sdk)
- [CMake Configuration Guide](./02a-cmake-config.md)
- [Building Areg SDK from Source](./02b-cmake-build.md)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech
