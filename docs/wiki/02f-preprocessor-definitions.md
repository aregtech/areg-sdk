# Areg SDK Preprocessor Definitions Reference

This guide provides a comprehensive reference for preprocessor definitions used in Areg SDK projects, detailing each definition's purpose and usage for configuring and compiling applications.

---

## Table of Contents

1. [Quick Reference](#quick-reference)
2. [Platform Defines](#platform-defines)
3. [Build Type Defines](#build-type-defines)
4. [Areg Framework Library Defines](#areg-framework-library-defines)
5. [Areg Logger Library Defines](#areg-logger-library-defines)
6. [Feature Defines](#feature-defines)
7. [Common Use Cases](#common-use-cases)

---

## Quick Reference

### All Preprocessor Definitions

| Define                            | Category   | Purpose                               | Auto-Set         |
|-----------------------------------|------------|---------------------------------------|-------------------|
| [POSIX](#posix)                   | Platform   | Enable POSIX API compilation          | ✅ Yes            |
| [WINDOWS](#windows)               | Platform   | Enable Windows API compilation        | ✅ Yes            |
| [DEBUG](#debug)                   | Build Type | Enable Debug build                    | ✅ Yes            |
| [NDEBUG](#ndebug)                 | Build Type | Enable Release build                  | ✅ Yes            |
| [EXP_AREG_DLL](#exp_areg_dll)     | Export     | Export areg shared library symbols    | ✅ Yes            |
| [EXP_AREG_LIB](#exp_areg_lib)     | Export     | Export areg static library symbols    | ✅ Yes            |
| [IMP_AREG_DLL](#imp_areg_dll)     | Import     | Import from areg shared library       | ✅ Yes*           |
| [IMP_AREG_LIB](#imp_areg_lib)     | Import     | Import from areg static library       | ✅ Yes*           |
| [EXP_LOGGER_DLL](#exp_logger_dll) | Export     | Export areglogger shared library      | ✅ Yes            |
| [EXP_LOGGER_LIB](#exp_logger_lib) | Export     | Export areglogger static library      | ✅ Yes            |
| [IMP_LOGGER_DLL](#imp_logger_dll) | Import     | Import from areglogger shared library | ❌ Manual         |
| [IMP_LOGGER_LIB](#imp_logger_lib) | Import     | Import from areglogger static library | ❌ Manual         |
| [AREG_EXTENDED](#areg_extended)   | Feature    | Enable extended library features      | ⚙️ Configurable   |
| [AREG_LOGS](#areg_logs)           | Feature    | Enable logging infrastructure         | ⚙️ Configurable   |
| [BIT32](#bit32)                   | Bitness    | Enable 32-bit compilation             | ✅ Yes            |
| [BIT64](#bit64)                   | Bitness    | Enable 64-bit compilation             | ✅ Yes            |

*Auto-set when using Areg CMake functions; manual otherwise

### Configuration Methods

Preprocessor definitions can be set in:

1. **CMake** - Command line or `CMakeLists.txt`
2. **Visual Studio** - `msvc_setup.props` property file
3. **Manual** - Project-specific preprocessor definitions

### Mutual Exclusivity

These define groups are mutually exclusive (only one can be set):

- **Platform**: `POSIX` ↔ `WINDOWS`
- **Build Type**: `DEBUG` ↔ `NDEBUG`
- **Areg Export**: `EXP_AREG_DLL` ↔ `EXP_AREG_LIB`
- **Areg Import**: `IMP_AREG_DLL` ↔ `IMP_AREG_LIB`
- **Logger Export**: `EXP_LOGGER_DLL` ↔ `EXP_LOGGER_LIB`
- **Logger Import**: `IMP_LOGGER_DLL` ↔ `IMP_LOGGER_LIB`
- **Bitness**: `BIT32` ↔ `BIT64`

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Platform Defines

### POSIX

**Purpose:** Enables Areg Framework compilation with POSIX APIs.  
**Auto-set:** Yes (when using GCC or Clang on Linux/macOS/Cygwin)  
**When to use:**
- Building on Linux
- Building on macOS
- Building on Cygwin (Windows POSIX environment)
**CMake example:**
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=gnu
```
**Compilers:**
- GCC (`g++`)
- Clang (`clang++`)
- Cygwin GCC

**Mutual exclusivity:** Cannot be used with `WINDOWS`

<div align="right"><kbd><a href="#platform-defines">↑ Back to platform ↑</a></kbd></div>

---

### WINDOWS

**Purpose:** Enables Areg Framework compilation with Windows APIs.  
**Auto-set:** Yes (when using MSVC or Clang for Windows)  
**When to use:**
- Building on Windows with Visual Studio
- Building with MSVC compiler
- Building with Clang for Windows (`clang-cl`)
**CMake example:**
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=msvc
```
**Visual Studio:** Automatically set when building with Visual Studio.  
**Compilers:**
- MSVC (`cl`)
- Clang for Windows (`clang-cl`)

**Mutual exclusivity:** Cannot be used with `POSIX`

<div align="right"><kbd><a href="#platform-defines">↑ Back to platform ↑</a></kbd></div>

---

## Build Type Defines

### DEBUG

**Purpose:** Enables Debug build configuration with debugging symbols and no optimization.  
**Auto-set:** Yes (when `CMAKE_BUILD_TYPE=Debug` or Visual Studio Debug configuration)  
**When to use:**
- Development and debugging
- Testing and diagnostics
- Analyzing issues
**CMake example:**
```bash
cmake -B ./build -DAREG_BUILD_TYPE=Debug
# or
cmake -B ./build -DCMAKE_BUILD_TYPE=Debug
```

**Visual Studio:** Select **Debug** configuration from toolbar.  
**Impact:**
- Debugging symbols included
- No optimization
- Assertions enabled
- Larger binary size

**Mutual exclusivity:** Cannot be used with `NDEBUG`

<div align="right"><kbd><a href="#build-type-defines">↑ Back to build type ↑</a></kbd></div>

---

### NDEBUG

**Purpose:** Enables Release build configuration with optimization and no debugging symbols.  
**Auto-set:** Yes (when `CMAKE_BUILD_TYPE=Release` or Visual Studio Release configuration)  
**When to use:**
- Production deployment
- Performance testing
- Final binary distribution
**CMake example:**
```bash
cmake -B ./build -DAREG_BUILD_TYPE=Release
# or
cmake -B ./build -DCMAKE_BUILD_TYPE=Release
```
**Visual Studio:** Select **Release** configuration from toolbar.  
**Impact:**
- No debugging symbols
- Full optimization
- Assertions disabled
- Smaller, faster binary

**Mutual exclusivity:** Cannot be used with `DEBUG`

<div align="right"><kbd><a href="#build-type-defines">↑ Back to build type ↑</a></kbd></div>

---

## Areg Framework Library Defines

### Export Defines (Building areg Library)

#### EXP_AREG_DLL

**Purpose:** Exports symbols when building `areg` as a shared library (DLL/SO).  
**Auto-set:** Yes (when building areg library as shared)  
**When to use:**
- Building areg shared library
- Creating dynamic library

**CMake example:**
```bash
cmake -B ./build -DAREG_BINARY=shared
```

**Visual Studio:** Default configuration for areg library.

**Mutual exclusivity:** Cannot be used with `EXP_AREG_LIB`

> [!NOTE]
> This define is only relevant when **building** the areg library itself, not when using it in applications.

<div align="right"><kbd><a href="#areg-framework-library-defines">↑ Back to areg defines ↑</a></kbd></div>

---

#### EXP_AREG_LIB

**Purpose:** Exports symbols when building `areg` as a static library.  
**Auto-set:** Yes (when building areg library as static)  
**When to use:**
- Building areg static library
- Creating standalone library

**CMake example:**
```bash
cmake -B ./build -DAREG_BINARY=static
```
**Visual Studio:** Manually set in areg project properties.

**Mutual exclusivity:** Cannot be used with `EXP_AREG_DLL`

> [!NOTE]
> This define is only relevant when **building** the areg library itself, not when using it in applications.

<div align="right"><kbd><a href="#areg-framework-library-defines">↑ Back to areg defines ↑</a></kbd></div>

---

### Import Defines (Using areg Library)

#### IMP_AREG_DLL

**Purpose:** Imports symbols from `areg` shared library (DLL/SO).  
**Auto-set:** Yes when using Areg CMake functions; manual otherwise  
**When to use:**
- Linking with areg shared library
- Importing from DLL

**CMake example (auto-set):**
```cmake
# Using Areg CMake functions
macro_declare_executable(myapp main.cpp)
```

**CMake example (manual):**
```cmake
add_executable(myapp main.cpp)
target_compile_definitions(myapp PRIVATE IMP_AREG_DLL)
target_link_libraries(myapp PRIVATE areg::areg)
```

**Visual Studio:**
1. Project → Properties
2. C/C++ → Preprocessor → Preprocessor Definitions
3. Add: `IMP_AREG_DLL`

**Mutual exclusivity:** Cannot be used with `IMP_AREG_LIB`

> [!IMPORTANT]
> Must match how areg library was built (shared vs static).

<div align="right"><kbd><a href="#areg-framework-library-defines">↑ Back to areg defines ↑</a></kbd></div>

---

#### IMP_AREG_LIB

**Purpose:** Imports symbols from `areg` static library.  
**Auto-set:** Yes when using Areg CMake functions; manual otherwise  
**When to use:**
- Linking with areg static library
- Embedding areg in executable

**CMake example (auto-set):**
```cmake
# Configure areg as static
option(AREG_BINARY "areg library type" "static")

# Using Areg CMake functions
macro_declare_executable(myapp main.cpp)
```

**CMake example (manual):**
```cmake
add_executable(myapp main.cpp)
target_compile_definitions(myapp PRIVATE IMP_AREG_LIB)
target_link_libraries(myapp PRIVATE areg::areg)
```

**Visual Studio:**
1. Project → Properties
2. C/C++ → Preprocessor → Preprocessor Definitions
3. Add: `IMP_AREG_LIB`

**Mutual exclusivity:** Cannot be used with `IMP_AREG_DLL`

> [!IMPORTANT]
> Must match how areg library was built (shared vs static).

<div align="right"><kbd><a href="#areg-framework-library-defines">↑ Back to areg defines ↑</a></kbd></div>

---

## Areg Logger Library Defines

### Export Defines (Building areglogger Library)

#### EXP_LOGGER_DLL

**Purpose:** Exports symbols when building `areglogger` as shared library.  
**Auto-set:** Yes (when building areglogger library as shared)  
**When to use:**
- Building areglogger shared library

**CMake example:**
```bash
cmake -B ./build -DAREG_LOGGER_BINARY=shared
```

**Visual Studio:** Default configuration for areglogger library.

**Mutual exclusivity:** Cannot be used with `EXP_LOGGER_LIB`

> [!NOTE]
> This define is only relevant when **building** the areglogger library itself.

<div align="right"><kbd><a href="#areg-logger-library-defines">↑ Back to logger defines ↑</a></kbd></div>

---

#### EXP_LOGGER_LIB

**Purpose:** Exports symbols when building `areglogger` as static library.  
**Auto-set:** Yes (when building areglogger library as static)  
**When to use:**
- Building areglogger static library

**CMake example:**
```bash
cmake -B ./build -DAREG_LOGGER_BINARY=static
```

**Visual Studio:** Manually set in areglogger project properties.

**Mutual exclusivity:** Cannot be used with `EXP_LOGGER_DLL`

> [!NOTE]
> This define is only relevant when **building** the areglogger library itself.

<div align="right"><kbd><a href="#areg-logger-library-defines">↑ Back to logger defines ↑</a></kbd></div>

---

### Import Defines (Using areglogger Library)

#### IMP_LOGGER_DLL

**Purpose:** Imports symbols from `areglogger` shared library.  
**Auto-set:** No (must be set manually)  
**When to use:**
- Linking with areglogger shared library
- Using log observer API

**CMake example:**
```cmake
add_executable(myapp main.cpp)
target_compile_definitions(myapp PRIVATE IMP_LOGGER_DLL)
target_link_libraries(myapp PRIVATE areglogger::areglogger)
```

**Visual Studio:**
1. Project → Properties
2. C/C++ → Preprocessor → Preprocessor Definitions
3. Add: `IMP_LOGGER_DLL`

**Mutual exclusivity:** Cannot be used with `IMP_LOGGER_LIB`

<div align="right"><kbd><a href="#areg-logger-library-defines">↑ Back to logger defines ↑</a></kbd></div>

---

#### IMP_LOGGER_LIB

**Purpose:** Imports symbols from `areglogger` static library.  
**Auto-set:** No (must be set manually)  
**When to use:**
- Linking with areglogger static library
- Embedding log observer in executable

**CMake example:**
```cmake
add_executable(myapp main.cpp)
target_compile_definitions(myapp PRIVATE IMP_LOGGER_LIB)
target_link_libraries(myapp PRIVATE areglogger::areglogger)
```

**Visual Studio:**
1. Project → Properties
2. C/C++ → Preprocessor → Preprocessor Definitions
3. Add: `IMP_LOGGER_LIB`

**Mutual exclusivity:** Cannot be used with `IMP_LOGGER_DLL`

<div align="right"><kbd><a href="#areg-logger-library-defines">↑ Back to logger defines ↑</a></kbd></div>

---

## Feature Defines

### AREG_EXTENDED

**Purpose:** Enables additional features in `aregextend` library.  
**Auto-set:** No (configurable, default is OFF)  
**When to use:**
- Need extended utility features
- Using database functionality
- Using advanced terminal features

**Dependencies:**
- **Linux/Cygwin:** Requires `ncurses` library

**CMake example:**
```bash
cmake -B ./build -DAREG_EXTENDED=ON
```

**Visual Studio:**
Edit `msvc_setup.props`:
```xml
<AregExtended>1</AregExtended>
```

**Impact:**
- Adds extended library features
- May require additional dependencies
- Slightly larger binary

<div align="right"><kbd><a href="#feature-defines">↑ Back to features ↑</a></kbd></div>

---

### AREG_LOGS

**Purpose:** Enables logging infrastructure in Areg Framework.  
**Auto-set:** No (configurable, default is ON)  
**When to use:**
- Need application logging
- Debugging and diagnostics
- Production monitoring

**CMake example:**
```bash
# Enable logging (default)
cmake -B ./build -DAREG_LOGS=ON

# Disable logging
cmake -B ./build -DAREG_LOGS=OFF
```

**Visual Studio:**
Edit `msvc_setup.props`:
```xml
<AregLogs>1</AregLogs>  <!-- Enable -->
<AregLogs>0</AregLogs>  <!-- Disable -->
```

**Impact:**
- **ON:** Logging infrastructure included (larger binary)
- **OFF:** No logging (smaller, faster binary)

> [!TIP]
> Disable logging in production for minimal overhead.

<div align="right"><kbd><a href="#feature-defines">↑ Back to features ↑</a></kbd></div>

---

### BIT32

**Purpose:** Configures build for 32-bit systems.  
**Auto-set:** Yes (based on compiler and platform selection)  
**When to use:**
- Building for 32-bit systems
- Legacy system support

**CMake example:**
```bash
cmake -B ./build -DAREG_PROCESSOR=x86
```

**Visual Studio:** Select **Win32** platform.

**Mutual exclusivity:** Cannot be used with `BIT64`

<div align="right"><kbd><a href="#feature-defines">↑ Back to features ↑</a></kbd></div>

---

### BIT64

**Purpose:** Configures build for 64-bit systems.  
**Auto-set:** Yes (based on compiler and platform selection)  
**When to use:**
- Building for 64-bit systems (recommended)
- Modern system deployment

**CMake example:**
```bash
cmake -B ./build -DAREG_PROCESSOR=x64
```

**Visual Studio:** Select **x64** platform.

**Mutual exclusivity:** Cannot be used with `BIT32`

<div align="right"><kbd><a href="#feature-defines">↑ Back to features ↑</a></kbd></div>

---

## Common Use Cases

### Use Case 1: Windows Application with Shared areg Library

**Scenario:** Building Windows application using areg DLL.

**Required defines:**
- `WINDOWS` (auto-set)
- `IMP_AREG_DLL`
- `DEBUG` or `NDEBUG` (auto-set)
- `BIT64` or `BIT32` (auto-set)

**CMake:**
```cmake
add_executable(myapp main.cpp)
target_compile_definitions(myapp PRIVATE IMP_AREG_DLL)
target_link_libraries(myapp PRIVATE areg::areg)
```

**Visual Studio:**
```cpp
// In source file or project preprocessor definitions
#define IMP_AREG_DLL
```

---

### Use Case 2: Linux Application with Static areg Library

**Scenario:** Building Linux application with areg statically linked.

**Required defines:**
- `POSIX` (auto-set)
- `IMP_AREG_LIB`
- `DEBUG` or `NDEBUG` (auto-set)
- `BIT64` or `BIT32` (auto-set)

**CMake:**
```cmake
# Configure areg as static
set(AREG_BINARY "static" CACHE STRING "areg library type")

add_executable(myapp main.cpp)
target_compile_definitions(myapp PRIVATE IMP_AREG_LIB)
target_link_libraries(myapp PRIVATE areg::areg)
```

---

### Use Case 3: Application with Logging Disabled

**Scenario:** Production application with minimal overhead (no logging).

**Required defines:**
- Platform define (auto-set)
- Import define for areg
- `AREG_LOGS` disabled
- `NDEBUG` (Release build)

**CMake:**
```bash
cmake -B ./build \
  -DAREG_BUILD_TYPE=Release \
  -DAREG_LOGS=OFF \
  -DAREG_BINARY=shared
```

**Impact:**
- Smaller binary size
- Faster execution
- No logging overhead

---

### Use Case 4: Application Using Extended Features

**Scenario:** Application using aregextend library features.

**Required defines:**
- Platform define (auto-set)
- Import define for areg
- `AREG_EXTENDED` enabled

**CMake:**
```bash
cmake -B ./build -DAREG_EXTENDED=ON
```

**Dependencies:**
- Linux/Cygwin: Install `ncurses` library

**Impact:**
- Extended features available
- Additional library dependencies

---

### Use Case 5: Cross-Platform Application

**Scenario:** Application building on both Windows and Linux.

**Strategy:** Use auto-set platform defines, manually set others.

**CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

# Platform-specific handling
if(WIN32)
    # Windows-specific settings
    set(PLATFORM_LIBS ws2_32)
elseif(UNIX)
    # Linux-specific settings
    set(PLATFORM_LIBS pthread)
endif()

# Application
add_executable(myapp main.cpp)
target_compile_definitions(myapp PRIVATE IMP_AREG_DLL)
target_link_libraries(myapp PRIVATE areg::areg ${PLATFORM_LIBS})
```

**Defines set:**
- Windows: `WINDOWS`, `IMP_AREG_DLL`, `BIT64`
- Linux: `POSIX`, `IMP_AREG_DLL`, `BIT64`

---

### Use Case 6: Building areg Library Itself

**Scenario:** Building areg framework from source.  
**Export defines (auto-set):**
- `EXP_AREG_DLL` (shared) or `EXP_AREG_LIB` (static)

**CMake:**
```bash
# Build areg as shared library
cmake -B ./build -DAREG_BINARY=shared

# Build areg as static library
cmake -B ./build -DAREG_BINARY=static
```

> [!NOTE]
> Export defines are only for building areg itself, not for applications using areg.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Configuration Guides:**
- [CMake Configuration Options](./02a-cmake-config.md) - Complete CMake options
- [CMake Integration](./02c-cmake-integrate.md) - Using Areg in projects
- [Visual Studio Integration](./02d-msvc-integrate.md) - MSVC setup

**Configuration Files:**
- [user.cmake](../../conf/cmake/user.cmake) - CMake configuration
- [msvc_setup.props](../../msvc_setup.props) - Visual Studio properties

**Build Guides:**
- [CMake Build Guide](./02b-cmake-build.md)
- [MSBuild Build Guide](./02c-msbuild-build.md)

**Help:**
For questions, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
