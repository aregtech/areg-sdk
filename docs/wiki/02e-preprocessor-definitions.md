# AREG SDK Preprocessor Definitions Guide: Configuration and Usage Across Toolchains

*This document provides an overview of preprocessor definitions used in AREG SDK projects, detailing each definition's purpose and usage to aid in configuring and compiling applications effectively.*

## Table of Contents
1. [General Information](#1-general-information)
2. [Platform-Specific Defines](#2-platform-specific-defines)
   - [POSIX](#posix)
   - [WINDOWS](#windows)
3. [Build Type Defines](#3-build-type-defines)
   - [DEBUG and NDEBUG](#debug-and-ndebug)
4. [AREG Framework Library Defines](#4-areg-framework-library-defines)
   - [Exporting Framework Library](#exporting-framework-library)
   - [Importing Framework Library](#importing-framework-library)
5. [AREG Log Observer API Library Defines](#5-areg-log-observer-api-library-defines)
   - [Exporting areglogger Library](#exporting-areglogger-library)
   - [Importing areglogger Library](#importing-areglogger-library)
6. [Other Definitions](#6-other-definitions)
   - [AREG_EXTENDED](#areg_extended)
   - [AREG_LOGS](#areg_logs)
   - [Application Bitness](#application-bitness)

---

## 1. General Information

The AREG SDK uses preprocessor-defined symbols to configure framework compilation, enabling customization and control over the build process. This guide provides details on each define and instructions for configuring them across various platforms and IDEs.

| Define                | Description                                           |
|-----------------------|-------------------------------------------------------|
| **POSIX**             | Enables compilation with POSIX API.                   |
| **WINDOWS**           | Enables compilation with Windows API.                 |
| **DEBUG**             | Enables Debug build.                                  |
| **NDEBUG**            | Enables Release build.                                |
| **EXP_AREG_DLL**      | Exports symbols for shared `areg` library.            |
| **EXP_AREG_LIB**      | Exports symbols for static `areg` library.            |
| **IMP_AREG_DLL**      | Imports symbols from shared `areg` library.           |
| **IMP_AREG_LIB**      | Imports symbols from static `areg` library.           |
| **AREG_EXTENDED**     | Enables extended AREG features.                       |
| **AREG_LOGS**         | Enables logging in AREG framework.                    |
| **EXP_LOGGER_DLL**    | Exports symbols for shared `areglogger` library.      |
| **EXP_LOGGER_LIB**    | Exports symbols for shared `areglogger` library.      |
| **IMP_LOGGER_DLL**    | Imports symbols for shared `areglogger` library.      |
| **IMP_LOGGER_LIB**    | Imports symbols for shared `areglogger` library.      |
| **BIT32**             | Enables compilation for 32-bit systems.               |
| **BIT64**             | Enables compilation for 64-bit systems.               |

### Configuration Methods

These symbols can be set in:
- `msvc_setup.props` property file for builds with Microsoft Visual Studio,
- `CMakeLists.txt` of your project when using AREG SDK libraries,
- or the command line when building with CMake.

---

## 2. Platform-Specific Defines

### POSIX

The `POSIX` define enables the AREG framework to compile with POSIX APIs. It is automatically set when compiling with compatible compilers (e.g., GCC, Clang) under Linux or Cygwin.

**CMake Example**:
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=gnu
```

### WINDOWS
The `WINDOWS` define enables compilation with the Windows API and is automatically set when using MSVC or Clang for Visual Studio (`clang-cl`).

**CMake Example**:
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=msvc
```

> [!NOTE]
> `POSIX` and `WINDOWS` are mutually exclusive.

---

## 3. Build Type Defines

### DEBUG and NDEBUG
These defines specify the build configuration (`DEBUG` for Debug, `NDEBUG` for Release) and are mutually exclusive.

**CMake Example**: Set the build type using the `AREG_BUILD_TYPE` parameter, which is equivalent to `CMAKE_BUILD_TYPE`:
```bash
cmake -B ./build -DAREG_BUILD_TYPE=Debug
```

**Microsoft Visual Studio**: Select the build configuration in the Visual Studio Configuration Manager.

> [!NOTE]
> `DEBUG` and `NDEBUG` are mutually exclusive.

---

## 4. AREG Framework Library Defines

### Exporting Framework Library
These symbols ensure proper export of classes and functions for `areg` shared or static libraries. Relevant only when building the `areg` library.

- **EXP_AREG_DLL**: Exports symbols for a shared `areg` library.
- **EXP_AREG_LIB**: Exports symbols for a static `areg` library.

**CMake Example**: Configure the library as shared
```bash
cmake -B ./build -DAREG_BINARY=shared
```

**Microsoft Visual Studio**: By default, `areg` is compiled as a shared library (DLL) and sets `EXP_AREG_DLL`. To change, manually select the library type and set `EXP_AREG_LIB` in the `areg` project property page.

> [!NOTE]
> `EXP_AREG_DLL` and `EXP_AREG_LIB` are mutually exclusive.

### Importing Framework Library
For projects using the `areg` library, these symbols control import of shared or static library symbols.

- **IMP_AREG_DLL**: Imports symbols from a shared `areg` library.
- **IMP_AREG_LIB**: Imports symbols from a static `areg` library.

**CMake Example**: Automatically sets the correct define when specifying the `areg` library type and adding executables using AREG CMake functions or macros. Otherwise, the define should be set manually
```bash
cmake -B ./build -DAREG_BINARY=shared
```

> [!NOTE]
> `IMP_AREG_DLL` and `IMP_AREG_LIB` are mutually exclusive.

---

## 5. AREG Log Observer API Library Defines

### Exporting areglogger Library
These symbols ensure proper export of classes and functions for shared or static `areglogger` libraries. Relevant only when building the `areglogger` library.

- **EXP_LOGGER_DLL**: Exports symbols for the `areglogger` library when building as a dynamic library (DLL).
- **EXP_LOGGER_LIB**: Exports symbols for the `areglogger` library when building as a static library.

**CMake Example**: Configure the library as static
```bash
cmake -B ./build -DAREG_LOGCOLLECTOR_LIB=static
```

**Microsoft Visual Studio**: By default, `areglogger` is compiled as a shared library (DLL) and sets `EXP_LOGGER_DLL`. To change, manually select the library type and set `EXP_LOGGER_LIB` in the `areglogger` project property page.

> [!NOTE]
> `EXP_LOGGER_DLL` and `EXP_LOGGER_LIB` are mutually exclusive.

### Importing areglogger Library
For projects using the `areglogger` library, these symbols control import of shared or static library symbols.

- **IMP_LOGGER_DLL**: Imports symbols from a shared `areglogger` library.
- **IMP_LOGGER_LIB**: Imports symbols from a static `areglogger` library.

**CMake Example**: Manually define in the command line or in the CMake scripts the type of library to link the project
```bash
cmake -B ./build -DIMP_LOGGER_LIB
```

**Microsoft Visual Studio**: Select your project property and manually set one of the defines.

> [!NOTE]
> `IMP_LOGGER_DLL` and `IMP_LOGGER_LIB` are mutually exclusive.

---

## 6. Other Definitions

### AREG_EXTENDED
The `AREG_EXTENDED` define enables additional features in the `aregextend` library. Set it in configuration files as needed. It may have additional dependencies. For example, it requires `ncurses` library dependency when compile with additional features using POSIX API.

**CMake Example**: Change the boolean value of the `AREG_EXTENDED` variable either before including AREG SDK CMake scripts or in the command line:
```
cmake -B ./build -DAREG_EXTENDED:BOOL=ON
```

**Microsoft Visual Studio**: Change the property value `AregExtended` in the property file `msvc_setup.props`

### AREG_LOGS
The `AREG_LOGS` define enables compilation sources with logging. Set it in configuration files as needed.

**CMake Example**: Change the boolean value of the `AREG_LOGS` variable either before including AREG SDK CMake scripts or in the command line:
```
cmake -B ./build -DAREG_LOGS:BOOL=OFF
```

**Microsoft Visual Studio**: Change the property value `AregLogs` in the property file `msvc_setup.props`

### Application Bitness

The defines `BIT32` and `BIT64` configures the build to target a 32- or 64-bit system. They are mutually exclusive and automatically set when choosing build platform.

---

For further details, see the [CMake Configuration Options for Building AREG SDK](./02a-cmake-config.md) document, the configuration file [`user.cmake`](./../../conf/cmake/user.cmake), or Visual Studio property file [`msvc_setup`](./../../msvc_setup).
