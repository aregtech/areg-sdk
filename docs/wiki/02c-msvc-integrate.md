# Integrating Areg Framework with Microsoft Visual Studio

This guide covers integrating Areg Framework into Microsoft Visual Studio projects on Windows.

> [!TIP]
> For practical integration examples, see the [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) repository.

> [!NOTE]
> Alternative integration method: [Integrating with CMake](./02b-cmake-integrate.md).

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [Prerequisites](#prerequisites)
3. [Integration Methods](#integration-methods)
4. [Advanced Integration](#advanced-integration)
5. [Troubleshooting](#troubleshooting)

---

## Quick Start

### Fastest Integration (vcpkg)

**Prerequisites:** Visual Studio 2019+ installed

**Step 1: Install vcpkg**

```powershell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

**Step 2: Install Areg SDK**

```powershell
.\vcpkg install areg:x64-windows
```

**Step 3: Create Visual Studio Project**

1. Open Visual Studio
2. Create new **Console App** project named `MyApp`
3. Replace `MyApp.cpp` content:

```cpp
#include "areg/base/String.hpp"
#include <iostream>

#pragma comment(lib, "areg")

int main() {
    String greeting("Hello from Areg SDK!");
    std::cout << greeting.getData() << std::endl;
    return 0;
}
```

**Step 4: Build and Run**

- **Build → Build Solution** (`Ctrl+Shift+B`)
- **Debug → Start Without Debugging** (`Ctrl+F5`)

**Expected output:**
```
Hello from Areg SDK!
```

**Build time:** ~5 seconds (using pre-built binaries)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Prerequisites

### Required Tools

- **Visual Studio 2019+** with C++ desktop development workload
- **Git** - Version control
- **Java 17+** - For code generation tools (if using ORPC services)

### Verify Installation

**Visual Studio:**
- Launch Visual Studio
- Check version: **Help → About Microsoft Visual Studio**
- Should show version 16.0 (VS 2019) or higher

**Git:**
```powershell
git --version
```

**Java (if using code generation):**
```powershell
java -version
```
Should output version 17 or higher.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Integration Methods

Two methods to integrate Areg SDK:

| Method | Best For | Complexity | Setup Time |
|--------|----------|------------|------------|
| [vcpkg](#method-1-vcpkg-package) | Quick setup | Low | 5-10 minutes |
| [Git Submodule](#method-2-git-submodule) | Source control | Medium | 15-20 minutes |

---

### Method 1: vcpkg Package

**Advantages:**
- ✅ Pre-built binaries (instant)
- ✅ Automatic updates via vcpkg
- ✅ Minimal configuration
- ✅ Zero project setup

**Disadvantages:**
- ❌ Requires vcpkg installation
- ❌ Less control over build options

> [!IMPORTANT]
> vcpkg support available starting with Areg SDK 2.0.

#### Step 1: Install vcpkg

**Clone and bootstrap:**

```powershell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

**Expected output:**
```
Downloading vcpkg-glibc...
Building vcpkg.exe...
vcpkg package management program version ...
```

**Installation time:** 2-3 minutes

#### Step 2: Integrate with Visual Studio

```powershell
.\vcpkg integrate install
```

**Expected output:**
```
Applied user-wide integration for this vcpkg root.

All MSBuild C++ projects can now #include any installed libraries.
Linking will be handled automatically.
Installing new libraries will make them instantly available.
```

> [!IMPORTANT]
> Close Visual Studio before running `vcpkg integrate install` if it's open. Restart Visual Studio after integration.

#### Step 3: Install Areg SDK

**For 64-bit Windows (recommended):**
```powershell
.\vcpkg install areg:x64-windows
```

**For 32-bit Windows:**
```powershell
.\vcpkg install areg:x86-windows
```

**Installation time:** 5-10 minutes (downloads and extracts pre-built binaries)

**Verify installation:**
```powershell
.\vcpkg list | findstr areg
```

**Expected output:**
```
areg:x64-windows    1.5.0    Areg SDK - Object RPC framework
```

#### Step 4: Create Visual Studio Project

**Create new project:**

1. Open Visual Studio
2. **File → New → Project**
3. Select **Console App**
4. Name: `MyAregApp`
5. Click **Create**

**Add Areg SDK code:**

Replace content of `MyAregApp.cpp`:

```cpp
#include "areg/base/String.hpp"
#include <iostream>

// Link with Areg library
#pragma comment(lib, "areg")

int main() {
    String greeting("Hello from Areg SDK!");
    std::cout << greeting.getData() << std::endl;
    return 0;
}
```

> [!NOTE]
> `#pragma comment(lib, "areg")` automatically links the Areg library. Without this pragma, you need to add `areg.lib` to **Linker → Input → Additional Dependencies**.

#### Step 5: Build and Run

**Build:**
- **Build → Build Solution** (or `Ctrl+Shift+B`)

**Expected build output:**
```
1>------ Build started: Project: MyAregApp, Configuration: Debug x64 ------
1>MyAregApp.cpp
1>MyAregApp.vcxproj -> C:\Projects\MyAregApp\x64\Debug\MyAregApp.exe
========== Build: 1 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========
```

**Run:**
- **Debug → Start Without Debugging** (or `Ctrl+F5`)

**Expected output:**
```
Hello from Areg SDK!
```

**Build time:** ~5 seconds (first build), ~2 seconds (rebuilds)

> [!TIP]
> For detailed vcpkg setup, see [Installing Areg SDK with vcpkg](./02e-cmake-vcpkg.md).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

### Method 2: Git Submodule

**Advantages:**
- ✅ Source code in your repository
- ✅ Full control over build
- ✅ Version pinning via Git
- ✅ Offline builds possible

**Disadvantages:**
- ❌ More complex setup
- ❌ Manual configuration required
- ❌ Longer first build time

#### Step 1: Create Git Repository

```powershell
mkdir MyAregProject
cd MyAregProject
git init
```

#### Step 2: Add Areg SDK Submodule

**Create `.gitmodules` file:**
```ini
[submodule "areg-sdk"]
    path = areg-sdk
    url = https://github.com/aregtech/areg-sdk.git
```

**Initialize submodule:**
```powershell
git submodule update --init --recursive
git submodule update --remote --recursive
```

**Verify:**
```powershell
dir areg-sdk
```

Should show Areg SDK directory structure.

#### Step 3: Configure Project Files

**Copy configuration file:**
```powershell
copy areg-sdk\msvc_setup.props .\msvc_setup.props
```

**Edit `msvc_setup.props`:**

Open in text editor and update `<AregSdkRoot>`:
```xml
<AregSdkRoot>$(SolutionDir)areg-sdk\</AregSdkRoot>
```

**Optionally adjust:**
- `AregOutputBin` - Binary output directory
- `AregOutputLib` - Library output directory
- `AregOutputObj` - Object files directory

#### Step 4: Create Visual Studio Solution

**Create new solution:**
1. Open Visual Studio
2. **File → New → Project**
3. Select **Console App**
4. Name: `MyAregApp`
5. Click **Create**

**Add Areg projects to solution:**
1. Right-click solution → **Add → Existing Project**
2. Navigate to `areg-sdk\framework\areg\`
3. Select `areg.vcxproj`
4. Click **Open**

**Optionally add:**
- `areg-sdk\framework\aregextend\aregextend.vcxproj`
- `areg-sdk\framework\areglogger\areglogger.vcxproj`

#### Step 5: Configure MyAregApp Project

**Add include directories:**

1. Right-click `MyAregApp` project → **Properties**
2. **Configuration:** All Configurations
3. **Platform:** All Platforms
4. Navigate to **C/C++ → General → Additional Include Directories**
5. Add: `$(AregFramework)`
6. Optionally add: `$(SolutionDir)areg-sdk\thirdparty`

**Add preprocessor definitions:**
1. Navigate to **C/C++ → Preprocessor → Preprocessor Definitions**
2. Add one of:
   - `IMPORT_SHARED_SYMBOLS` (for DLL - default)
   - `IMPORT_STATIC_SYMBOLS` (for static library)

**Link Areg library (if not using pragma):**

If not using `#pragma comment(lib, "areg")`:

1. Navigate to **Linker → Input → Additional Dependencies**
2. Add: `areg.lib`

**Set project dependency:**

1. Right-click `MyAregApp` → **Build Dependencies → Project Dependencies**
2. Check `areg`
3. Click **OK**

#### Step 6: Add Application Code

Replace `MyAregApp.cpp` content:

```cpp
#include "areg/base/String.hpp"
#include <iostream>

#pragma comment(lib, "areg")

int main() {
    String greeting("Hello from Areg SDK!");
    std::cout << greeting.getData() << std::endl;
    return 0;
}
```

#### Step 7: Build and Run

**Build:**
- **Build → Build Solution** (`Ctrl+Shift+B`)

**First build compiles Areg SDK:**
```
1>------ Build started: Project: areg, Configuration: Debug x64 ------
1>Building Areg Framework...
...
2>------ Build started: Project: MyAregApp, Configuration: Debug x64 ------
2>MyAregApp.cpp
========== Build: 2 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========
```

**Build time:**
- First build: 3-8 minutes (compiles Areg SDK)
- Subsequent builds: 2-5 seconds (only your code)

**Run:**
- **Debug → Start Without Debugging** (`Ctrl+F5`)

**Expected output:**
```
Hello from Areg SDK!
```

#### Step 8: Update Submodule (When Needed)

```powershell
git submodule update --remote --recursive
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Advanced Integration

### Areg SDK Components

Understanding what's available:

| Component | Type | Purpose | Required |
|-----------|------|---------|----------|
| **areg** | Library | Core framework (ORPC, threading) | ✅ Yes |
| **aregextend** | Library | Extended utilities | ❌ Optional |
| **areglogger** | Library | Log observer API | ❌ Optional |
| **mtrouter** | Executable | Message router for IPC/networking | ❌ No* |
| **logcollector** | Executable | Remote log collection | ❌ No |
| **codegen.jar** | Tool | Service interface generator | ✅ For ORPC |

*Required only for inter-process or networked communication.

---

### MSBuild Configuration Options

Build Areg SDK with specific options from command line.

**Available options:**

| Option | Values | Default | Description |
|--------|--------|---------|-------------|
| **Configuration** | `Debug`, `Release` | `Debug` | Build configuration |
| **Platform** | `Win32`, `x64` | `Win32` | Target architecture |
| **AregExtended** | `0`, `1` | `1` | Build extended library |
| **AregLogs** | `0`, `1` | `1` | Enable logging |

**Example - Release build without logging:**

```powershell
msbuild /m /p:Configuration=Release /p:Platform=x64 /p:AregLogs=0 MyAregApp.sln
```

**Example - Debug build without extended library:**

```powershell
msbuild /m /p:Configuration=Debug /p:Platform=x64 /p:AregExtended=0 MyAregApp.sln
```

**Example - Full build with package restore:**

```powershell
msbuild /m /p:Configuration=Release /p:Platform=x64 /p:AregExtended=1 /p:AregLogs=1 MyAregApp.sln -t:restore,build -p:RestorePackagesConfig=true
```

For detailed MSBuild options, see [Building with MSBuild](./02c-msbuild-build.md).

---

### Using Project Templates

Areg SDK provides Visual Studio project templates for consistent builds.

**Template location:** `<areg-sdk>\docs\templates\`

**Available templates:**
- `msvc-app.vcxproj` - Console application
- `msvc-lib-shared.vcxproj` - Shared library (DLL)
- `msvc-lib-static.vcxproj` - Static library

**Using templates:**

1. Copy desired template to your project directory
2. Rename file (e.g., `MyLibrary.vcxproj`)
3. Edit GUIDs (generate new ones)
4. Update project name
5. Add to solution
6. Configure sources

**Templates include:**
- Areg SDK property settings
- Compiler configurations
- Linker settings
- Output directories

---

### Service Interface Code Generation

For ORPC service development, generate code from Service Interface documents.

**Manual approach:**

1. Create Service Interface file (`.siml`)
2. Run code generator:
   ```powershell
   java -jar areg-sdk\tools\codegen.jar --root=. --doc=services\MyService.siml --target=generated
   ```
3. Add generated files to project
4. Build

**Automated approach (batch file):**

Create `generate_services.bat`:

```batch
@echo off
set CODEGEN=areg-sdk\tools\codegen.jar
set ROOT=%~dp0
set TARGET=generated

java -jar %CODEGEN% --root=%ROOT% --doc=services\Service1.siml --target=%TARGET%
java -jar %CODEGEN% --root=%ROOT% --doc=services\Service2.siml --target=%TARGET%

echo Service interfaces generated
```

**Integrate with build:**

1. Create `dummy` project in solution
2. Project properties → **Build Events → Pre-Build Event**
3. Command Line: `$(SolutionDir)generate_services.bat`
4. Make other projects depend on `dummy`

**Example reference:**
- Batch file: [examples_generate.bat](../../examples/examples_generate.bat)
- Dummy project: [examples/dummy](../../examples/dummy/)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### vcpkg Integration Not Working

**Problem:** Visual Studio doesn't find Areg SDK headers after vcpkg installation.  
**Solution:**  
*1. Verify integration:*
```powershell
vcpkg integrate install
```
*2. Restart Visual Studio*  
*3. Check integration status:*
```powershell
vcpkg integrate remove
vcpkg integrate install
```
*4. Verify package installation:*
```powershell
vcpkg list | findstr areg
```

---

### Build Fails with "Cannot open include file"

**Problem:** Error C1083: Cannot open include file: 'areg/base/String.hpp'  
**Solution:**  
*For vcpkg method:*
- Ensure vcpkg integration is active
- Restart Visual Studio
*For submodule method:*
1. Check **Additional Include Directories** contains `$(AregFramework)`
2. Verify `msvc_setup.props` is properly configured
3. Check `<AregSdkRoot>` path in `msvc_setup.props`

---

### Linker Error LNK2019

**Problem:** Unresolved external symbol errors.  
**Solution:**  
*1. Add library linkage:*  
Either use pragma in code:
```cpp
#pragma comment(lib, "areg")
```
Or add to project settings:
- **Linker → Input → Additional Dependencies**
- Add: `areg.lib`

*2. Check preprocessor definitions:*  
Ensure one of these is defined:
- `IMPORT_SHARED_SYMBOLS` (for DLL)
- `IMPORT_STATIC_SYMBOLS` (for static lib)
*3. Verify project dependency:*
- Right-click project → **Build Dependencies → Project Dependencies**
- Check `areg` is selected

---

### Wrong Platform Built

**Problem:** Built for x86 when x64 was needed.  
**Solution:**  
*In Visual Studio:*
1. Select platform from toolbar: **x64** or **Win32**
2. **Build → Rebuild Solution**
*With MSBuild:*
```powershell
msbuild /p:Platform=x64 MyAregApp.sln
```

---

### Java Not Found

**Problem:** Code generation fails with "Java is not recognized".  
**Solution:**  
*1. Install Java 17+:*
- Download from [Adoptium](https://adoptium.net/)
*2. Verify installation:*
```powershell
java -version
```
*3. Add to PATH if needed:*
```powershell
$env:PATH += ";C:\Program Files\Java\jdk-17\bin"
```

---

### Submodule Not Updating

**Problem:** Areg SDK submodule shows old version.  
**Solution:**  
```powershell
git submodule update --remote --recursive
git submodule foreach git pull origin master
```

---

### MSBuild Options Not Applied

**Problem:** `AregLogs=0` specified but logging still enabled.  
**Solution:**  
*Ensure correct syntax:*
```powershell
msbuild /m /p:AregLogs=0 MyAregApp.sln
```
*Not:*
```powershell
msbuild /m /property:AregLogs=0  # Wrong
```

**Check property file:**  
Verify `msvc_setup.props` doesn't override command-line options.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Integration Guides:**
- [vcpkg Installation](./02e-cmake-vcpkg.md) - Detailed vcpkg setup
- [CMake Integration](./02b-cmake-integrate.md) - Alternative integration method
- [MSBuild Build Guide](./02c-msbuild-build.md) - MSBuild options

**Build Guides:**
- [Building with MSBuild](./02c-msbuild-build.md)
- [Building with CMake](./02b-cmake-build.md)

**Examples:**
- [Areg SDK Examples](../../examples/README.md) - Sample applications
- [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) - Integration examples

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
