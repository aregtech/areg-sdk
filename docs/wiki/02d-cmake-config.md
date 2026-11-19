# CMake Configuration Options for Areg SDK

This guide describes the CMake options of Areg SDK: library type, compiler selection, logging, examples and tests, packages, and installation. Everything else is set with the standard CMake variables.

> [!IMPORTANT]
> Options are passed on the command line with `-D<name>=<value>`. Their defaults are in [user.cmake](../../conf/cmake/user.cmake).

---

## Table of Contents

1. [Quick Configuration Examples](#quick-configuration-examples)
2. [Options Reference](#options-reference)
3. [Standard CMake Variables](#standard-cmake-variables)
4. [Detailed Option Descriptions](#detailed-option-descriptions)
5. [Advanced Configuration](#advanced-configuration)
6. [Integration Examples](#integration-examples)

---

## Quick Configuration Examples

**1. Default build (system compiler, Release, examples and tests):**
```bash
cmake -B ./build
cmake --build ./build -j20
```
Executables and shared libraries are in `./build/bin`, static libraries in `./build/lib`.

**2. Debug build with Clang:**
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=llvm -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build -j20
```

**3. Release build without examples or tests:**
```bash
cmake -B ./build -DAREG_EXAMPLES=OFF -DAREG_TESTS=OFF
cmake --build ./build -j20
```

**4. Static library build:**
```bash
cmake -B ./build -DAREG_LIB_TYPE=static
cmake --build ./build -j20
```

**5. Cross-compile for 32-bit ARM:**
```bash
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/gnu-linux-arm32.cmake
cmake --build ./build -j20
```

**6. Build and install to a custom location:**
```bash
cmake -B ./build -DCMAKE_INSTALL_PREFIX=/opt/areg
cmake --build ./build -j20
sudo cmake --install ./build
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Options Reference

| Option | Values | Default | Description |
|--------|--------|---------|-------------|
| [AREG_LIB_TYPE](#areg_lib_type) | `shared`, `static` | `shared` | `areg` and `areglogger` library type |
| [AREG_COMPILER_FAMILY](#areg_compiler_family) | `gnu`, `llvm`, `msvc`, `cygwin`, `mingw` | System default | Compiler family |
| [AREG_LOGGING](#areg_logging) | `ON`, `OFF` | `ON` | Compile with logs |
| [AREG_EXAMPLES](#areg_examples) | `ON`, `OFF` | `ON` top level, `OFF` in another project | Build the examples |
| [AREG_TESTS](#areg_tests) | `ON`, `OFF` | `ON` top level, `OFF` in another project | Build the unit tests |
| [AREG_BENCHMARKS](#areg_benchmarks) | `ON`, `OFF` | `OFF` | Build the performance benchmarks |
| [AREG_INSTALL](#areg_install) | `ON`, `OFF` | `ON` | Create the install rules |
| [AREG_SYSTEM_SQLITE](#areg_system_sqlite) | `ON`, `OFF` | `OFF` | Use the installed SQLite3 |
| [AREG_SYSTEM_GTEST](#areg_system_gtest) | `ON`, `OFF` | `ON` | Use the installed GoogleTest |
| [AREG_NO_EXCEPTIONS](#areg_no_exceptions) | `ON`, `OFF` | `OFF` | Compile without exceptions and RTTI (GNU/Clang) |
| [AREG_OUTPUT_DIR](#areg_output_dir) | Directory path | The build directory | Root of `bin/` and `lib/` |
| [AREG_ARCH](#areg_arch) | `x86`, `x64`, `arm`, `aarch64` | System default | Target processor |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Standard CMake Variables

Areg SDK follows these variables and adds no option of its own for them:

| Variable | Default | Purpose |
|----------|---------|---------|
| `CMAKE_BUILD_TYPE` | `Release` | `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel` |
| `CMAKE_CXX_COMPILER` | System default | C++ compiler by name or path; the C compiler is derived from it |
| `CMAKE_TOOLCHAIN_FILE` | none | Cross-compilation; ready files are in [conf/toolchains](../../conf/toolchains/) |
| `CMAKE_CXX_COMPILER_TARGET` | Derived from the processor | Compiler target triple, such as `arm-linux-gnueabihf` |
| `CMAKE_RUNTIME_OUTPUT_DIRECTORY` | `<AREG_OUTPUT_DIR>/bin` | Executables and shared libraries |
| `CMAKE_ARCHIVE_OUTPUT_DIRECTORY` | `<AREG_OUTPUT_DIR>/lib` | Static libraries |
| `CMAKE_INSTALL_PREFIX` | `/usr/local`, `C:/Program Files/areg-sdk` | Installation directory; `cmake --install <build> --prefix <dir>` overrides it |
| `FETCHCONTENT_BASE_DIR` | `<build>/_deps` | Where fetched sources, such as GoogleTest, are placed |

Everything of a build goes to the directory given to `cmake -B`: the CMake cache, the objects, the generated code in `<build>/generate`, and the binaries. A build in the Areg SDK root itself is refused. The configure step runs the code generator on every `.siml` and `.fsml` document and rewrites `<build>/generate`; `cmake --build` compiles.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Detailed Option Descriptions

### AREG_LIB_TYPE

**Description:** Library type of the `areg` framework and of the log observer API library `areglogger`. The two always have the same type, so a process never holds two copies of the framework.
**Possible Values:**
- `shared` - Dynamic library (`.so`, `.dll`, `.dylib`)
- `static` - Static library (`.a`, `.lib`)

**Default:** `shared`
**Example:**
```bash
cmake -B ./build -DAREG_LIB_TYPE=static
```

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_COMPILER_FAMILY

**Description:** Selects the C++ and C compilers by family in one word.
**Possible Values:**
- `gnu` - GCC (`g++`, `gcc`)
- `llvm` - Clang (`clang++`, `clang`); `clang-cl` and the `ClangCL` toolset with Visual Studio
- `msvc` - Microsoft Visual C++ (`cl`)
- `cygwin` - GCC under Cygwin
- `mingw` - GCC under MinGW

**Default:** The system default compiler
**Example:**
```bash
cmake -B ./build -DAREG_COMPILER_FAMILY=llvm
```

To name one compiler by path or version, use the standard variable instead:
```bash
cmake -B ./build -DCMAKE_CXX_COMPILER=/usr/bin/clang++-18
```

> [!NOTE]
> `CMAKE_CXX_COMPILER` wins over `AREG_COMPILER_FAMILY` when both are set.

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_LOGGING

**Description:** Compiles the framework and the applications with logs.
**Possible Values:** `ON`, `OFF`
**Default:** `ON`
**Example:**
```bash
cmake -B ./build -DAREG_LOGGING=OFF
```

**Use case:** Disable logging in builds where no log is needed and every cycle counts.

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_EXAMPLES

**Description:** Builds the example applications.
**Possible Values:** `ON`, `OFF`
**Default:** `ON` when Areg SDK is the top-level project; `OFF` when it is part of another project (FetchContent, `add_subdirectory`)
**Dependencies:** Some examples require MFC (Microsoft Foundation Classes) on Windows
**Example:**
```bash
cmake -B ./build -DAREG_EXAMPLES=OFF
```

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_TESTS

**Description:** Builds the unit tests.
**Possible Values:** `ON`, `OFF`
**Default:** `ON` when Areg SDK is the top-level project; `OFF` when it is part of another project
**Dependencies:** GoogleTest, installed or fetched (see [AREG_SYSTEM_GTEST](#areg_system_gtest))
**Example:**
```bash
cmake -B ./build -DAREG_TESTS=OFF
```

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_BENCHMARKS

**Description:** Builds the performance benchmarks in [tests/benchmarks](../../tests/benchmarks/).
**Possible Values:** `ON`, `OFF`
**Default:** `OFF`
**Example:**
```bash
cmake -B ./build -DAREG_BENCHMARKS=ON
cmake --build ./build --target areg-benchmarks -j20
```

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_INSTALL

**Description:** Creates the install rules of the SDK: headers, libraries, tools and the CMake package.
**Possible Values:** `ON`, `OFF`
**Default:** `ON`
**Example - Disable installation:**
```bash
cmake -B ./build -DAREG_INSTALL=OFF
```

**Example - Install to a custom location:**
```bash
cmake -B ./build
cmake --build ./build -j20
cmake --install ./build --prefix /opt/areg
```

**Installed components:**
- Headers → `<prefix>/include/`
- Libraries → `<prefix>/lib/`
- Executables → `<prefix>/bin/`
- CMake package → `<prefix>/share/areg/`

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_SYSTEM_SQLITE

**Description:** Uses the SQLite3 installed in the system instead of the bundled sources.
**Possible Values:** `ON`, `OFF`
**Default:** `OFF`
**Example:**
```bash
cmake -B ./build -DAREG_SYSTEM_SQLITE=ON
```

**Behavior:**
- `OFF` - The sources in `thirdparty/sqlite3`, built as `aregsqlite3` on every platform. The log database is verified with these sources
- `ON` - The installed SQLite3 when found for the target architecture, else the sources in `thirdparty/sqlite3`. A vendor build may behave differently: Apple's system SQLite leaves the `-shm` file beside a closed `.sqlog`, and an application that links `aregsqlite3` (Lusan) then has no such target

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_SYSTEM_GTEST

**Description:** Uses the GoogleTest installed in the system for the unit tests.
**Possible Values:** `ON`, `OFF`
**Default:** `ON`
**Example:**
```bash
cmake -B ./build -DAREG_SYSTEM_GTEST=OFF
```

**Behavior:**
- `ON` - The installed GoogleTest when found, else fetched from its repository
- `OFF` - Always fetched from its repository

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_NO_EXCEPTIONS

**Description:** Compiles without C++ exceptions and RTTI (`-fno-exceptions -fno-rtti`). The sources use neither, so the option only removes the code and the tables the compiler emits for them. Required by projects that forbid both, such as MISRA C++ and AUTOSAR C++14.
**Possible Values:** `ON`, `OFF`
**Default:** `OFF`
**Example:**
```bash
cmake -B ./build -DAREG_NO_EXCEPTIONS=ON
```

> [!NOTE]
> Ignored by MSVC: its standard library requires an exception handling model.

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_ARCH

**Description:** Target processor and bitness of the compiled binaries.
**Possible Values:**
- `x86` - 32-bit x86
- `x64` (or `x86_64`, `amd64`) - 64-bit x86
- `arm` (or `arm32`) - 32-bit ARM
- `aarch64` (or `arm64`) - 64-bit ARM

**Default:** The system processor
**Example:**
```bash
cmake -B ./build -DAREG_ARCH=x86
```

Without `CMAKE_CXX_COMPILER`, `AREG_COMPILER_FAMILY` or a toolchain file, `AREG_ARCH` selects the default compiler family of the host: `gnu` on Linux, `llvm` on macOS. With `gnu`, `arm` builds with `arm-linux-gnueabihf-g++` and `aarch64` with `aarch64-linux-gnu-g++`.

x86 and x64 Release builds with GNU or Clang use the portable SSE4.2 baseline (`-msse4.2`) when the build host supports it, for the hardware CRC32C. To tune the binaries for the build machine instead, pass `-DCMAKE_CXX_FLAGS=-march=native`; such binaries may not run on another CPU.

> [!TIP]
> To cross-compile, prefer a toolchain file from [conf/toolchains](../../conf/toolchains/); it sets the processor, the compilers and the search paths together.

> [!TIP]
> To verify the architecture of a binary:
> ```bash
> file ./build/bin/mtrouter.elf
> ```
> Example output for 32-bit:
> ```
> ./build/bin/mtrouter.elf: ELF 32-bit LSB pie executable, Intel 80386, version 1 (GNU/Linux), ...
> ```

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

### AREG_OUTPUT_DIR

**Description:** Root directory of the binaries: executables and shared libraries go to `<dir>/bin`, static libraries to `<dir>/lib`. The CMake cache, the object files and the generated code stay in the build directory.
**Default:** The build directory, so `cmake -B build` puts the binaries in `build/bin`
**Example:**
```bash
cmake -B ./build -DAREG_OUTPUT_DIR=/home/user/my_project/areg
```

> [!NOTE]
> A relative path is relative to the build directory. `CMAKE_RUNTIME_OUTPUT_DIRECTORY` and `CMAKE_ARCHIVE_OUTPUT_DIRECTORY` win when they are set. Do not point two builds at the same directory: libraries of the same name replace each other.

<div align="right"><kbd><a href="#options-reference">↑ Back to options ↑</a></kbd></div>

---

## Advanced Configuration

### Print Configuration Status

Every configure of Areg SDK ends with a status report:

```
-- =======================================================================================
-- ----------------------> Areg project CMake Status Report Begin <-----------------------
-- =======================================================================================
-- Areg: >>> CMAKE_SOURCE_DIR    = '/home/user/areg-sdk', build type 'Debug'
-- Areg: >>> Build Environment ..: System 'Linux', 64-bit 'x86_64' platform, Env 'Posix'
-- Areg: >>> Used CXX-Compiler ..: '/usr/bin/clang++'
-- Areg: >>> Used C-Compiler ....: '/usr/bin/clang'
-- Areg: >>> Compiler Version ...: C++ standard 'c++17', compiler family 'llvm'
-- Areg: >>> Areg SDK Root ......: '/home/user/areg-sdk'
-- Areg: >>> CMake Build Dir ....: '/home/user/areg-sdk/build'
-- Areg: >>> Binary Output Dir ..: '/home/user/areg-sdk/build/bin'
-- Areg: >>> Build Modules ......: areg = 'shared', aregextend = 'static', areglogger = 'shared'
-- Areg: >>> Java Version .......: '17.0.16', minimum version required = 17
-- Areg: >>> Packages Use .......: SQLite3 package use = 'ON', GTest package use = 'OFF'
-- Areg: >>> Feature Options ....: Logs = 'ON', No exceptions = 'OFF'
-- Areg: >>> Other Options ......: Examples = 'ON', Unit Tests = 'ON', Benchmarks = 'OFF'
-- Areg: >>> Installation .......: Enabled = 'ON', location = '/usr/local'
-- =======================================================================================
-- -----------------------> Areg project CMake Status Report End <------------------------
-- =======================================================================================
```

### Integration Variables

When integrating Areg SDK into other projects:

**AREG_SDK_ROOT:**
- Set to the Areg SDK root directory before including `areg.cmake`
- In the Areg SDK project, it is the directory of the top-level `CMakeLists.txt`

**AREG_FRAMEWORK:**
- The directory of the framework headers, set by `setup.cmake` or by the installed package

**Example:**
```cmake
set(AREG_SDK_ROOT "/path/to/areg-sdk")
include(${AREG_SDK_ROOT}/areg.cmake)
```

### Complete Configuration Example

Build and install with every common option set explicitly:

```bash
cmake -B ./build \
  -DAREG_COMPILER_FAMILY=llvm \
  -DCMAKE_BUILD_TYPE=Release \
  -DAREG_TESTS=OFF \
  -DAREG_EXAMPLES=OFF \
  -DAREG_LIB_TYPE=shared \
  -DAREG_LOGGING=ON \
  -DAREG_INSTALL=ON \
  -DCMAKE_INSTALL_PREFIX=/usr/local

cmake --build ./build -j20

sudo cmake --install ./build
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Integration Examples

### Using Areg SDK in Your Project

**Option 1: Installed package**

After installing Areg SDK:

```cmake
find_package(areg CONFIG REQUIRED)
include(${AREG_CMAKE})

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

**Option 2: FetchContent**

```cmake
include(FetchContent)
FetchContent_Declare(
    areg
    GIT_REPOSITORY https://github.com/aregtech/areg-sdk.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(areg)
include(${areg_SOURCE_DIR}/areg.cmake)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

A fetched Areg SDK builds neither its examples nor its tests, and its binaries land in the `bin` directory of your build beside your own.

**Option 3: A local copy of the sources**

```cmake
set(AREG_SDK_ROOT "/path/to/areg-sdk")
include(${AREG_SDK_ROOT}/areg.cmake)
```

See [CMake Integration](./02b-cmake-integrate.md) for complete projects.

### Additional Notes

**Compiler Configuration:**
- `AREG_COMPILER_FAMILY` selects both compilers by family (e.g., `llvm` → `clang++` and `clang`)

**Target Processor:**
- A toolchain file or `AREG_ARCH` selects the target processor
- See [Cross-Compiling Guide](./01b-cmake-build.md#cross-compilation) for details

**Output Directories:**
- Set `AREG_OUTPUT_DIR` to put `bin/` and `lib/` elsewhere, for example into the directory your project collects its binaries in
- Several builds side by side: see [Several Builds Side by Side](../DEVELOP.md#several-builds-side-by-side)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Additional Resources

**Build Guides:**
- [CMake Build Guide](./01b-cmake-build.md) - Step-by-step build instructions
- [CMake Integration](./02b-cmake-integrate.md) - Using Areg in your projects
- [Installing as a Package](./01a-areg-package.md) - Install once, use with `find_package`

**Configuration Files:**
- [user.cmake](../../conf/cmake/user.cmake) - The options and their defaults
- [setup.cmake](../../conf/cmake/setup.cmake) - Setup utilities and functions

**Examples:**
- [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) - Integration examples
- [Examples Directory](../../examples/README.md) - Sample applications

**Help:**
For questions or issues, open a [discussion](https://github.com/aregtech/areg-sdk/discussions) or [issue](https://github.com/aregtech/areg-sdk/issues) on GitHub.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech (Artak Avetyan), www.areg.tech, email: info[at]areg.tech
