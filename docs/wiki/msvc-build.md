# Building the AREG SDK with Microsoft Visual Studio and MSBuild

## Introduction

This guide provides a comprehensive overview of building the **AREG SDK** using **Microsoft Visual Studio** and **MSBuild**. It includes both quick build instructions and options for customizing your build, ensuring that AREG SDK is set up for efficient **multithreading**, **Inter-Process Communication (IPC)**, and **Object Remote Procedure Call (Object RPC)** functionality.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Quick Build with MSBuild](#quick-build-with-msbuild)
  - [Step 1: Installing Dependencies](#step-1-installing-dependencies)
  - [Step 2: Cloning the AREG SDK Repository](#step-2-cloning-the-areg-sdk-repository)
  - [Step 3: Building the Project](#step-3-building-the-project)
- [Customizing Your Build with MSBuild](#customizing-your-build-with-msbuild)
  - [Examples of MSBuild Commands](#examples-of-msbuild-commands)

---

## Prerequisites

To build AREG SDK with **MSBuild** in Microsoft Visual Studio, ensure the following tools are installed on your system:

1. **Microsoft Visual Studio** for Windows.
2. **Java** (required for code generation tools).
3. **Git** for repository management.

> [!NOTE]
> *Ensure that MSBuild is properly installed and included in your system's PATH.*

---

## Quick Build with MSBuild

### Step 1: Installing Dependencies

- Download and install **[Visual Studio](https://visualstudio.microsoft.com/)** with CMake and Clang.
- Install **[Java](https://www.java.com/download/)** and **[Git](https://git-scm.com/download/win)**.

These steps will prepare your environment to build AREG SDK projects on Windows.

### Step 2: Cloning the AREG SDK Repository

Clone the AREG SDK repository to access the latest source code:
```powershell
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```

### Step 3: Building the Project

You can open `areg-sdk.sln` file in Microsoft Visual Studio and compile all projects with default settings. Alternatively, you can open Terminal or Powershell in `<areg-sdk>` directory and compile AREG SDK projects using `MSBuild`:
```powershell
MSBuild ./areg-sdk.sln
```

This command initiates the default build configuration for AREG SDK projects.

> [!NOTE]
> *For quick access, ensure `MSBuild` is included in your system's PATH.*

---

## Customizing Your Build with MSBuild

`MSBuild` provides options to customize and optimize your build according to different development needs. Below are some configurable properties:

| **Option**          | **Possible Values** | **Default** | **Description**                                                           |
|---------------------|---------------------|-------------|---------------------------------------------------------------------------|
| **Configuration**   | `Debug`, `Release`  |  `Debug`    | Specifies the build configuration (`Debug` or `Release`).                 |
| **Platform**        | `Win32`, `x64`      |  `Win32`    | Sets the target architecture: `Win32` (32-bit) or `x64` (64-bit).         |
| **AregExtended**    | `0`, `1`            |  `1`        | Enables or disables extended objects in the `aregextend` library.         |
| **AregLogs**        | `0`, `1`            |  `1`        | Enables (`1`) or disables (`0`) logging in the SDK, useful for debugging. |

> [!IMPORTANT]
> You can also change the settings in [msvc_setup.props](./../../msvc_setup.props) property file of AREG SDK, which contains the basic settings to compile sources and output binaries. See the description in `msvc_setup.props` property file.

> [!NOTE]
> AREG SDK uses *Google Test Adapter* NuGet package to compile AREG Framework unit tests. The output directory for packages is configured in [nuget.config](./../../nuget.config) file.

### Examples of MSBuild Commands

Below are some examples that showcase how to use `MSBuild` with different configurations:

**Example 1:** 64-bit Debug build with extended features and no logging:
```powershell
MSBuild /m /property:Configuration=Debug /property:Platform=x64 /property:AregExtended=1 /property:AregLogs=0 ./areg-sdk.sln
```

**Example 2:** 32-bit Release build with standard features and logging enabled:
```powershell
MSBuild /m /property:Configuration=Release /property:Platform=Win32 /property:AregExtended=0 /property:AregLogs=1 ./areg-sdk.sln
```

For additional build configuration examples, refer to the [MSBuild Workflow](./../../.github/workflow/msbuild.yml) file in the AREG SDK repository.

---

## Additional Resources

For further guidance on building AREG SDK, troubleshooting tips, or advanced setup options, refer to the **AREG SDK documentation** on [GitHub](https://github.com/aregtech/areg-sdk).
