# Building Areg SDK with Microsoft Visual Studio and MSBuild

This guide covers building Areg SDK using Microsoft Visual Studio and MSBuild, including quick build steps and configuration options.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Quick Build](#quick-build)
3. [Build Configuration](#build-configuration)
4. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Required Tools

- **Microsoft Visual Studio 2019+** with:
  - Desktop development with C++ workload
  - CMake tools (optional, for CMake-based builds)
  - Clang compiler for Windows (optional)
- **Java 17+** - For code generation tools
- **Git** - Version control

### Verify Installation

**Check MSBuild:**
```powershell
MSBuild -version
```

Expected output:
```
Microsoft (R) Build Engine version 17.x.x
...
```

**Check Java:**
```powershell
java -version
```

Expected output:
```
openjdk version "17.x.x"
...
```

> [!NOTE]
> MSBuild is installed with Visual Studio and should be in your PATH. If not, launch **Developer Command Prompt for VS** instead of regular Command Prompt.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Quick Build

### Step 1: Clone Repository

```powershell
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```

### Step 2: Build with Default Settings

**Option A: Visual Studio IDE**

1. Open `areg-sdk.sln` in Visual Studio
2. Select build configuration (Debug/Release) from toolbar
3. **Build → Build Solution** (or press `Ctrl+Shift+B`)

**Option B: MSBuild Command Line**

```powershell
MSBuild ./areg-sdk.sln
```

**Expected output:**
```
Microsoft (R) Build Engine version 17.x.x
...
Build succeeded.
    0 Warning(s)
    0 Error(s)

Time Elapsed 00:02:15.34
```

**Build time:** 2-5 minutes depending on configuration.

**Binaries location:**
```
.\product\build\msvc-{compiler}\windows-{bits}-{arch}-{config}-{linkage}\bin\
```

**Example:**
```
.\product\build\msvc-cl\windows-64-amd64-release-shared\bin\
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Build Configuration

MSBuild supports various configuration options to customize your build.

### Configuration Properties

| Property | Values | Default | Description |
|----------|--------|---------|-------------|
| **Configuration** | `Debug`, `Release` | `Debug` | Build type (optimizations, debugging) |
| **Platform** | `Win32`, `x64` | `Win32` | Target architecture (32-bit or 64-bit) |
| **AregExtended** | `0`, `1` | `1` | Enable extended features in `aregextend` library |
| **AregLogs** | `0`, `1` | `1` | Enable logging infrastructure |

> [!TIP]
> Edit `msvc_setup.props` in the Areg SDK root to set default values for all projects.

### Build Examples

**Example 1: 64-bit Debug build with extended features, no logging**

```powershell
MSBuild /m /property:Configuration=Debug /property:Platform=x64 /property:AregExtended=1 /property:AregLogs=0 ./areg-sdk.sln
```

**Breakdown:**
- `/m` - Build using multiple processors (parallel build)
- `Configuration=Debug` - Build with debug symbols
- `Platform=x64` - Target 64-bit architecture
- `AregExtended=1` - Include extended library
- `AregLogs=0` - Disable logging system

**Example 2: 32-bit Release build with logging, no extended features**

```powershell
MSBuild /m /property:Configuration=Release /property:Platform=Win32 /property:AregExtended=0 /property:AregLogs=1 ./areg-sdk.sln
```

**Breakdown:**
- `Configuration=Release` - Optimized build
- `Platform=Win32` - Target 32-bit architecture
- `AregExtended=0` - Exclude extended library
- `AregLogs=1` - Enable logging system

**Example 3: Clean and rebuild**

```powershell
MSBuild /m /t:Clean,Build /property:Configuration=Release /property:Platform=x64 ./areg-sdk.sln
```

### Common MSBuild Options

| Option | Description |
|--------|-------------|
| `/m` | Multi-processor build (faster) |
| `/t:Build` | Build target (default) |
| `/t:Clean` | Remove build artifacts |
| `/t:Rebuild` | Clean + Build |
| `/v:minimal` | Reduce output verbosity |
| `/v:detailed` | Increase output verbosity |
| `/maxcpucount:4` | Use specific number of processors |

**Example with multiple options:**

```powershell
MSBuild /m /t:Rebuild /v:minimal /property:Configuration=Release /property:Platform=x64 ./areg-sdk.sln
```

### Configuration Files

**msvc_setup.props**

Contains default compilation settings and output paths. Located at:
```
areg-sdk/msvc_setup.props
```

**Key settings you can modify:**
- Output directories
- Compiler flags
- Warning levels
- Optimization settings

**nuget.config**

Specifies NuGet package sources for dependencies (Google Test Adapter for unit tests). Located at:
```
areg-sdk/nuget.config
```

> [!NOTE]
> Areg SDK uses Google Test Adapter NuGet package for unit tests. Package directories are defined in `nuget.config`.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### MSBuild Not Found

**Problem:** `'MSBuild' is not recognized as an internal or external command`.

**Solution:**

**Option 1:** Use Developer Command Prompt
- Start **Developer Command Prompt for VS** from Start menu
- Navigate to `areg-sdk` directory
- Run MSBuild commands

**Option 2:** Add MSBuild to PATH
```powershell
# Visual Studio 2022 example
set PATH=%PATH%;C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin
```

**Option 3:** Use full path
```powershell
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ./areg-sdk.sln
```

### Java Not Found

**Problem:** Build fails with "Java is required for code generation".

**Solution:**

1. Verify Java installation:
   ```powershell
   java -version
   ```

2. If not installed, download from [java.com/download](https://www.java.com/download/)

3. Ensure Java is in PATH:
   ```powershell
   set PATH=%PATH%;C:\Program Files\Java\jdk-17\bin
   ```

### Build Errors After Updates

**Problem:** Build fails after updating Areg SDK or Visual Studio.

**Solution:**

1. **Clean the solution:**
   ```powershell
   MSBuild /m /t:Clean ./areg-sdk.sln
   ```

2. **Delete build artifacts:**
   ```powershell
   rmdir /s /q .\product\build
   ```

3. **Rebuild:**
   ```powershell
   MSBuild /m /t:Build ./areg-sdk.sln
   ```

### NuGet Restore Fails

**Problem:** NuGet packages (Google Test) fail to restore.

**Solution:**

1. **Restore packages manually:**
   ```powershell
   nuget restore ./areg-sdk.sln
   ```

2. **Or use MSBuild restore:**
   ```powershell
   MSBuild /m /t:Restore ./areg-sdk.sln
   ```

3. **Check nuget.config** file points to valid package sources

### Wrong Platform Built

**Problem:** Build produces 32-bit binaries when 64-bit was expected.

**Solution:**

Explicitly specify platform:
```powershell
MSBuild /m /property:Platform=x64 ./areg-sdk.sln
```

Verify in output:
```
Project "areg-sdk.sln" (Build target(s)):
  ...
  Platform = x64
```

### Slow Build Times

**Problem:** Build takes too long.

**Solution:**

1. **Enable parallel build:**
   ```powershell
   MSBuild /m /maxcpucount:8 ./areg-sdk.sln
   ```

2. **Build only specific project:**
   ```powershell
   MSBuild /m /t:"areg:Build" ./areg-sdk.sln
   ```

3. **Use Release configuration** (faster than Debug):
   ```powershell
   MSBuild /m /property:Configuration=Release ./areg-sdk.sln
   ```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Advanced Topics

### Building Specific Projects

Build only the core framework:
```powershell
MSBuild /m /t:"areg:Build" /property:Configuration=Release /property:Platform=x64 ./areg-sdk.sln
```

Build only examples:
```powershell
MSBuild /m /t:"examples:Build" /property:Configuration=Debug /property:Platform=x64 ./areg-sdk.sln
```

### Automated Builds

For CI/CD pipelines, use:

```powershell
MSBuild /m /t:Rebuild /v:minimal /property:Configuration=Release /property:Platform=x64 /property:AregExtended=1 /property:AregLogs=0 ./areg-sdk.sln
```

**Explanation:**
- `/t:Rebuild` - Clean build (ensures fresh state)
- `/v:minimal` - Reduce log output (easier to parse)
- Configuration settings suitable for release artifacts

**Exit codes:**
- `0` - Success
- Non-zero - Build failed

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Integration Guides:**
- [Integrating Areg with Visual Studio](./02c-msvc-integrate.md) - Using Areg SDK in your projects
- [CMake Build Guide](./02b-cmake-build.md) - Alternative build method
- [vcpkg Installation](./02e-cmake-vcpkg.md) - Package manager installation

**Microsoft Documentation:**
- [MSBuild Command-Line Reference](https://learn.microsoft.com/en-us/visualstudio/msbuild/msbuild-command-line-reference)
- [Visual Studio CMake Projects](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio)

**Areg SDK Resources:**
- [GitHub Repository](https://github.com/aregtech/areg-sdk)
- [MSBuild Workflow](https://github.com/aregtech/areg-sdk/blob/master/.github/workflows/msbuild.yml) - CI/CD examples
- [Discussions](https://github.com/aregtech/areg-sdk/discussions) - Get help from community

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech
