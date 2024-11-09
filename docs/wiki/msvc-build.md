# Building the AREG SDK with Microsoft Visual Studio and MSBuild

## Introduction

This guide offers a detailed process for building the **AREG SDK** using **Microsoft Visual Studio** and **MSBuild**. It provides quick build steps and customization options, ensuring the SDK is optimized for **multithreading**, **Inter-Process Communication (IPC)**, and **Object Remote Procedure Call (Object RPC)**.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Quick Build with MSBuild](#quick-build-with-msbuild)
  - [Step 1: Installing Dependencies](#step-1-installing-dependencies)
  - [Step 2: Cloning the AREG SDK Repository](#step-2-cloning-the-areg-sdk-repository)
  - [Step 3: Building the Project](#step-3-building-the-project)
- [Customizing Your Build with MSBuild](#customizing-your-build-with-msbuild)
  - [Examples of MSBuild Commands](#examples-of-msbuild-commands)
  - [Additional Resources](#additional-resources)

---

## Prerequisites

To build the AREG SDK with **MSBuild** in Microsoft Visual Studio, ensure the following tools are installed on your system:

1. **Microsoft Visual Studio** (with CMake and Clang).
2. **Java** (required for code generation tools).
3. **Git** for repository management.

> [!NOTE]
> Make sure `MSBuild` is installed and included in your system's PATH.

---

## Quick Build with MSBuild

### Step 1: Installing Dependencies

- Download and install **[Visual Studio](https://visualstudio.microsoft.com/)** with CMake and Clang.
- Install **[Java](https://www.java.com/download/)** and **[Git](https://git-scm.com/download/win)**.

This setup prepares your environment to build AREG SDK projects on Windows.

### Step 2: Cloning the AREG SDK Repository

Clone the AREG SDK repository to obtain the latest source code:
```powershell
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```

### Step 3: Building the Project

Open `areg-sdk.sln` in Microsoft Visual Studio to build with default settings, or use Terminal or PowerShell in the `<areg-sdk>` directory:
```powershell
MSBuild ./areg-sdk.sln
```

This command initiates the default configuration for AREG SDK projects.

---

## Customizing Your Build with MSBuild

`MSBuild` allows you to configure and optimize the build for different development needs. Below are key configurable properties:

| **Option**          | **Possible Values** | **Default** | **Description**                                                   |
|---------------------|---------------------|-------------|-------------------------------------------------------------------|
| **Configuration**   | `Debug`, `Release`  |  `Debug`    | Sets the build configuration (Debug or Release).                  |
| **Platform**        | `Win32`, `x64`      |  `Win32`    | Sets the target architecture: `Win32` (32-bit) or `x64` (64-bit). |
| **AregExtended**    | `0`, `1`            |  `1`        | Enables extended features in the `aregextend` library.            |
| **AregLogs**        | `0`, `1`            |  `1`        | Enables logging in the SDK for debugging.                         |

> [!IMPORTANT]
> Settings can also be modified in the `msvc_setup.props` file in the AREG SDK, containing essential compilation and output configurations. Refer to [msvc_setup.props](./../../msvc_setup.props) for further details.

> [!NOTE]
> The AREG SDK uses the *Google Test Adapter* NuGet package to compile AREG Framework unit tests. Package directories are specified in the [nuget.config](./../../nuget.config) file.

### Examples of MSBuild Commands

Here are examples of `MSBuild` commands for different configurations:

**Example 1:** 64-bit Debug build with extended features and no logging:
```powershell
MSBuild /m /property:Configuration=Debug /property:Platform=x64 /property:AregExtended=1 /property:AregLogs=0 ./areg-sdk.sln
```

**Example 2:** 32-bit Release build with standard features and logging enabled:
```powershell
MSBuild /m /property:Configuration=Release /property:Platform=Win32 /property:AregExtended=0 /property:AregLogs=1 ./areg-sdk.sln
```

For additional examples, see the [MSBuild Workflow](./../../.github/workflow/msbuild.yml) in the AREG SDK repository.

### Additional Resources

- For additional setup information, refer to [Visual Studio CMake Projects](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio) or [VS Code CMake Quickstart](https://code.visualstudio.com/docs/cpp/cmake-quickstart).
- Additionally, read [Integrating AREG Framework with Microsoft Visual Studio](./msvc-integrate.md) document.
- For more information on building the AREG SDK, troubleshooting tips, or advanced setup options, see the **AREG SDK documentation** on [GitHub](https://github.com/aregtech/areg-sdk).
