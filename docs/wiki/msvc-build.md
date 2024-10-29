# Building the AREG SDK with Microsoft Visual Studio and MSBuild

## Introduction
This guide covers the steps to build the **AREG SDK** using **Microsoft Visual Studio** and **MSBuild** for quick and customizable builds. It provides a clear overview of the `MSBuild` command options for streamlined compilation. Following this guide will ensure you have the AREG SDK ready for multithreading, IPC, and RPC-based application development.

## Quick Build with MSBuild

To quickly build the AREG SDK with `MSBuild`, follow these steps:

### Step 1: Installing Dependencies

1. Download and install [Visual Studio](https://visualstudio.microsoft.com/), including packages **CMake** and **CLang compiler for Windows**.
2. Install [Java](https://www.java.com/download/) and [Git](https://git-scm.com/download/win).

After installing these tools, Windows will be ready for AREG SDK builds.

### Step 2: Cloning the AREG SDK Repository

### Step 2: Cloning the AREG SDK Repository

Clone the AREG SDK repository to obtain the latest source code:
```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```
Navigate to the project directory to proceed with build commands.

### Step 3: Building the Project

Compile the AREG SDK:
```bash
MSBuild ./areg-sdk.sln
```
This command builds AREG SDK projects with default configuration.

> [!NOTE]
> *Ensure `MSBuild` is correctly installed and included in your system's PATH.*

## Customizing Your Build with MSBuild

Build with `MSBuild` allows customization and configuration configurations to optimize AREG SDK projects for different requirements. Below are the key `MSBuild` options:

| **Option**          | **Default** | **Description**                                                                 |
|---------------------|-------------|---------------------------------------------------------------------------------|
| **Configuration**   | `Debug`     | Defines the build type (Debug/Release) for the source code.                     |
| **Platform**        | `Win32`     | Specifies the target architecture (Win32 for 32-bit, x64 for 64-bit).           |
| **AregExtended**    | `0`         | Enables/disables extended features in the `areg-extend` library.                |
| **AregLogs**        | `1`         | Enables logging in the AREG SDK. Set to `0` to disable logging.                 |

### Examples of MSBuild Commands

**Example 1:** 64-bit Debug build with extended features and no logging.
```bash
MSBuild /m /property:Configuration=Debug /property:Platform=x64 /property:AregExtended=1 /property:AregLogs=0 ./areg-sdk.sln
```

**Example 2:** 32-bit Release build with basic features and with logging.
```bash
MSBuild /m /property:Configuration=Release /property:Platform=Win32 /property:AregExtended=0 /property:AregLogs=1 ./areg-sdk.sln
```

> [!NOTE]
> *An example of build with `MSBuild` can be found in the [MSBuild Workflow](../../.github/workflows/MSBuild.yml) file in the AREG SDK repository.*

For more details, refer to the [AREG SDK documentation](https://github.com/aregtech/areg-sdk).