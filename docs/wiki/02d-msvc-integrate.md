# Integrating AREG Framework with Microsoft Visual Studio

This guide provides instructions for integrating the **AREG Framework** into an existing or new Microsoft Visual Studio project on Windows. You can integrate the AREG Framework using either of the following methods:

1. **Install via `vcpkg`** as a prebuilt package.
2. **Add AREG SDK as a Git submodule** in your project.

> [!TIP]  
> For a practical example of integrating AREG SDK libraries and tools, refer to the **[AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo)** repository.

> [!NOTE]  
> Alternatively, you can learn how to [integrate AREG Framework in a project with CMake](./02c-cmake-integrate.md).

## Table of Contents

- [AREG SDK General Requirements](#areg-sdk-general-requirements)
- [General Information](#general-information)
- [Example Code](#example-code)
- [Integration Methods](#integration-methods)
  - [Method 1: Integrate Using `vcpkg` Package](#method-1-integrate-using-vcpkg-package)
  - [Method 2: Integrate as Git Submodule](#method-2-integrate-as-git-submodule)
- [Advanced Integration](#advanced-integration)
  - [Advanced MSBuild Options](#advanced-msbuild-options)
  - [Advanced Project Settings](#advanced-project-settings)

---

## AREG SDK General Requirements

To integrate the AREG SDK, ensure the following prerequisites are met:

- **Git** for repository management
- **Supported Compilers**: Microsoft Visual Studio 2019 or newer
- **Java** version 17+ for code generation tools

---

## General Information

The AREG SDK includes several key components:

1. **AREG Framework (`areg` library)**: The core library for automation and Object RPC communication.
2. **AREG Framework Extension (`aregextend` library)**: An optional library with additional utilities.
3. **AREG Log Observer API (`areglogger` library)**: Allows applications to receive and manage log messages.
4. **AREG Multicast Router (`mcrouter` executable)**: An OS-managed service that routes RPC messages.
5. **AREG Log Collector (`logger` executable)**: An OS-managed service for remote log collection.
6. **AREG Log Observer (`logobserver` executable)**: A stand-alone application for managing logs.
7. **AREG Code Generator (`codegen.jar` runnable)**: A Java tool for generating Service Interface source code.

These components provide a robust foundation for integrating the AREG SDK, streamlining the development process.

---

## Example Code

Create a new Microsoft Visual Studio solution with a console project named `example`. Add a file named `example.cpp` with the following content:
```cpp
#include "areg/base/String.hpp"

// (Optional) Link the areg library in the source code for simplicity
#pragma comment(lib, "areg")

int main() {
    String str("Hello from AREG SDK!");
    std::cout << str.getData() << std::endl;
    return 0;
}
```
This example project and source file will be used throughout the integration steps.

---

## Integration Methods

### Method 1: Integrate Using `vcpkg` Package

> [!IMPORTANT]  
> As of AREG SDK 2.0, integration via `vcpkg` is supported.

1. **Install `vcpkg`**:  
   Follow the instructions on the [vcpkg GitHub page](https://github.com/microsoft/vcpkg). Ensure `vcpkg` is updated to the latest package version. If you are installing `vcpkg` for the first time, close Microsoft Visual Studio to allow proper integration.

2. **Install the AREG SDK Package for Windows**:  
   Run the following command in PowerShell to install the AREG SDK package for Windows:
   ```bash
   vcpkg install areg
   ```

3. **Integrate Installed Package**:  
   Run the following command to integrate the AREG SDK binaries and headers with Visual Studio:
   ```bash
   vcpkg integrate install
   ```

4. **Build the `example` Project**:  
   Since the example code includes a direct link to the `areg` library (`#pragma comment(lib, "areg")`), no additional configuration is needed. Build the `example` project and run it.

Also see [Installing and Using AREG SDK with vcpkg Package Manager](./01a-areg-package.md) for more details.

### Method 2: Integrate as Git Submodule

1. **Define Submodule**:  
   Add the `areg-sdk` as a submodule by creating a `.gitmodules` file in your project:
   ```plaintext
   [submodule "areg-sdk"]
     path = areg-sdk
     url = https://github.com/aregtech/areg-sdk.git
   ```

2. **Download AREG SDK Sources**:  
   Run these commands to initialize and update the submodule:
   ```bash
   git submodule update --init --recursive
   git submodule update --remote --recursive
   ```

3. **Integrate into Visual Studio Solution**:
   - Copy the `msvc_setup.props` file located in the `<areg-sdk>` directory to the root of your project (where your `example.sln` is located).
   - Open `msvc_setup.props` and set the `<AregSdkRoot>` property to the path of the `areg-sdk` submodule. For example:
     ```xml
     <AregSdkRoot>$(SolutionDir)areg-sdk/</AregSdkRoot>
     ```
     Adjust other settings, such as `AregOutputXXX`, in `msvc_setup.props` if needed to fit your project's build structure.
   
   - Add the `areg.vcxproj` project and optionally other `*.vcxproj` files located in `<areg-sdk>/framework` to your `example` solution.
   - In **Project** -> **Properties** for the `example` project, navigate to **C/C++** -> **General** -> **Additional Include Directories**, and add `$(AregFramework)`.
   - Optionally, add `$(SolutionDir)areg-sdk/thirdparty` to **Additional Include Directories**.
   - Under **C/C++** -> **Preprocessor Definitions**, add either `IMPORT_SHARED_SYMBOLS` (for DLL) or `IMPORT_STATIC_SYMBOLS` (for static library).
   - Since the `example.cpp` contains a linker instruction `#pragma comment(lib, "areg")`, there is no need to link `areg.lib` anymore. But if you don't like using `pragma` in your code, in **Project** -> **Properties** for the `example` project navigate to **Linker** -> **Input** -> **Additional Dependencies**, add `areg.lib`. Update **Additional Library Directories** in **Linker** -> **General** if necessary.

4. **Build the Solution**:  
   After configuring these settings, you should be able to build all projects in the solution.

---

## Advanced Integration

### Advanced MSBuild Options

The AREG SDK project defines the following options, which can be used with MSBuild from the command line:

- **AregExtended**: Enables (value `1`) or disables (value `0`) building with extended features in the `aregextend` project.
- **AregLogs**: Enables (value `1`) or disables (value `0`) compiling projects with logging capabilities.

To build your solution with MSBuild, you can specify these options. For example, to disable extended features and logging, use:

```powershell
msbuild /m /property:Configuration=Release /property:Platform=Win32 /property:AregExtended=0 /property:AregLogs=0 . -t:restore,build -p:RestorePackagesConfig=true
```

### Advanced Project Settings

If you want to adopt the AREG SDK's directory structure for your projects, use the `msvc-XXX.vcxproj` template files in `<areg-sdk>\docs\templates`. These templates include all necessary properties and compiler settings. Copy the required files, rename them, edit the GUIDs, and add them to your solution.

Since Microsoft Visual Studio does not offer the same flexibility as CMake for dynamic builds, additional steps are needed if using the code generator:

1. Create a `.bat` file with commands to generate files from Service Interface documents.
2. Generate the Service Interface code.
3. Create a project to compile the generated files into a static library.
4. Create a `dummy` project to execute the `.bat` file as a pre-build step.
5. Set other projects to depend on `dummy` to ensure it builds first.

For a working example, refer to the `.bat` file in [<areg-sdk>\examples\examples_generate.bat](./../../examples/examples_generate.bat) and the `dummy` project in [<areg-sdk>\examples\dummy](./../../examples/dummy/).
