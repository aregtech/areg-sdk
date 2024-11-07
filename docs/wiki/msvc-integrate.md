# Integrating AREG Framework with Microsoft Visual Studio

This guide covers how to integrate the **AREG Framework** into an existing project or create a new project **using Microsoft Visual Studio** under Windows system. There are two main methods for integration:

1. **Install via `vcpkg`** as a prebuilt package.
2. **Add AREG SDK as a Git submodule** to your project.

> [!TIP]  
> For a practical example of integrating AREG SDK libraries and tools, see the **[AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo)** repository.

> [!NOTE]
> Alternatively, learn how to [integrate AREG Framework in a project with CMake](./cmake-integrate.md).

## Table of Contents
- [AREG SDK General Requirements](#areg-sdk-general-requirements)
- [General Information](#general-information)
- [Example Code](#example-code)
- [Integration Methods](#integration-methods)
  - [Integrate Using `vcpkg` Package](#integrate-using-vcpkg-package)
  - [Integrate as Git Submodule](#integrate-as-git-submodule)
- [Advanced Integration](#advanced-integration)
  - [Advanced MSBuild Options](#advanced-msbuild-options)
  - [Advanced Project Settings](#advanced-project-settings)

---

## AREG SDK General Requirements

Ensure the following dependencies are installed for AREG SDK integration:

- **Git** for repository management
- **Supported Compilers**: Microsoft Visual Studio 2019 or newer
- **Java** version 17+ for code generation tools

---

## General Information

AREG SDK consists of several components:

1. **AREG Framework (`areg` library)**: Core library for automation and Object RPC communication.
2. **AREG Framework Extension (`aregextend` library)**: Optional library with extended utilities.
3. **AREG Log Observer API (`areglogger` library)**: Enables applications to receive and manage log messages.
4. **AREG Multicast Router (`mcrouter` executable)**: OS-managed service to route RPC messages.
5. **AREG Log Collector (`logger` executable)**: OS-managed service for remote log collection.
6. **AREG Log Observer (`logobserver` executable)**: Stand-alone application for managing logs.
7. **AREG Code Generator (`codegen.jar` runnable)**: Java tool for generating Service Interface source code.

These components provide a robust infrastructure for integrating AREG SDK in projects, ensuring streamlined development.

---

## Example Code

Use Microsoft Visual Studio and create a new solution with a console project `example`. Create a `example.cpp` file with following content.

```cpp
#include "areg/base/String.hpp"

// To keep simple, we link areg library here, not in the project property.
#pragma comment(lib, "areg")

int main() {
    String str("Hello from AREG SDK!");
    std::cout << str.getData() << std::endl;
    return 0;
}
```

This project and the source file we'll use in all our examples.

---

## Integration Methods

### Integrate Using `vcpkg` Package

> [!IMPORTANT]  
> Starting with AREG SDK 2.0, integration via `vcpkg` is available.

1. **Install `vcpkg`**:  
   Follow instructions on the [vcpkg GitHub page](https://github.com/microsoft/vcpkg). Make sure to update `vcpkg` for the latest package data. If you are going to install `vcpkg` now and have Microsoft Visual Studio opened, close it letting `vcpkg` to install and integrate in MSVS.

2. **Install AREG SDK Package for Windows**:  
   Use these command to install `areg` package for Windows (64-bit)
     ```bash
     vcpkg install areg:x64-windows
     ```

3. **Integrate installed package**:  
   Use the following to integrate AREG SDK binaries:
   ```bash
   vcpkg integrate install
   ```
 
4. **Build `example` Project**:  
   Since in the example we have already linked the sources with `areg` library (`#pragma comment(lib, "areg")`), there is no need for additional actions like link library using Project Property page. Now build the `example` project and run.

### Integrate as Git Submodule

1. **Define Submodule**:  
   Add `areg-sdk` as a submodule by creating a `.gitmodules` file:
   ```plaintext
   [submodule "areg-sdk"]
     path = areg-sdk
     url = https://github.com/aregtech/areg-sdk.git
   ```

2. **Get AREG SDK Sources**:  
   Run these commands:

   ```bash
   git submodule update --init --recursive
   git submodule update --remote --recursive
   ```

3. **Integrate in Visual Studio Solution**:
   - Copy `msvc_setup.props` file located in `<areg-sdk>` directory to the root directory of your project (same location with `example.sln`)
   - Open `msvc_setup.props` file and minimum change this `<AregSdkRoot>$(SolutionDir)</AregSdkRoot>` to indicate the path of `areg-sdk` submodule. For example:
     ```xml
     <AregSdkRoot>$(SolutionDir)areg-sdk/</AregSdkRoot>
     ```
     If needed you may change other settings like `AregOutputXXX` in `msvc_setup.props` to fit your project build structure.
   - Add at least `areg.vcxproj` project, located in the `<areg-sdk>/framework` directory to your `example` solution.
   - Select `example` project, go to **Project** => **Properties** => **C/C++** => **General** => **Additional Include Directories** and add there `$(AregFramework)`.
   - Optionally, add in `<areg-sdk>\thirdparty\` to the list of **Additional Include Directories**.
   - In the same **C/C++** category, select **Preprocessor Definitions** and add `IMPORT_SHARED_SYMBOLS` if you compile `areg` library as DLL or `IMPORT_STATIC_SYMBOLS` if you compile `areg` as static library.
   - Either in `example.cpp` file add `#pragma comment(lib, "areg")`, or select `example` project, go to **Project** => **Properties** => **Link** => **Input** => **Additional Dependencies** and add `areg.lib`. If needed, update **Additional Library Directories** in **General**.
4. **Build Commands**:  
   If all settings were properly changed, you should be able to build all projects.

---

## Advanced Integration

### Advanced MSBuild Options

AREG SDK project defined following options that can be used in MSBuild command line:
- **AregExtended**: Relevant to `aregextend` project. Enables (value `1`) or disables (value `0`) to build the objects with extended features;
- **AregLogs**: Relevant to all projects. Enables (value `1`) or disables (value `0`) to compile projects with logs.

If you compile your solution file using **MSBuild**, you can use these options. For example, the following command will build projects without extended objects and without logs:
```powershell
msbuild /m /property:Configuration=Release /property:Platform=Win32 /property:AregExtended=0 /property:AregLogs=0 . -t:restore,build -p:RestorePackagesConfig=true
```

### Advanced Project Settings

If you like the directory structure which offers AREG SDK project and want to adapt your projects to this structure, you can use `msvc-XXX.vcxproj` project files located in `<areg-sdk>\docs\templates` directory. This project files include all required properties and compiler settings. Copy the files you need, rename, open the file in text editor to change GUID and add to your solution.

Since Microsoft Visual Studio is not as advanced and dynamic as CMake, it requires additional effort if you use the code generator:
  - Create a `.bat` file and call commands to generate files from the Service Interface document files;
  - Generate the Service Interface codes;
  - Create a project to build static library and include generated files there;
  - Create a `dummy` project and call that `.bat` file are a pre-build action;
  - Make all your projects dependent on `dummy` to make sure that it is build first.

The working example of `.bat` file is [<areg-sdk>\examples\examples_generate.bat](./../../examples/examples_generate.bat)
The working example of `dummy` project which calls `examples_generate.bat` as pre-build action is [<areg-sdk>\examples\dummy](./../../examples/dummy/).
