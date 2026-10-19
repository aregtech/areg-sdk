# Installing Areg SDK as a Package

This guide installs Areg SDK once on a machine, so that other CMake projects find it with
`find_package(areg)`.

> [!NOTE]
> **No vcpkg package yet.** A vcpkg port is in preparation and not published; `vcpkg install areg`
> does not work today. Until it is available, install Areg from source as shown below, or let
> each project fetch it with [CMake FetchContent](./02b-cmake-integrate.md#method-2-fetchcontent).

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Build and Install](#build-and-install)
3. [Use the Installed Package](#use-the-installed-package)
4. [Troubleshooting](#troubleshooting)
5. [See Also](#see-also)

---

## Prerequisites

- **CMake 3.20+**
- **C++17 compiler:** GCC, Clang/LLVM or MSVC
- **Java 17+:** runs the bundled code generator at build time

**Supported platforms:** Linux, macOS and Windows. **Architectures:** x86, x86_64, ARM, AArch64.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Build and Install

```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
cmake -B ./build -DAREG_EXAMPLES=OFF -DAREG_TESTS=OFF
cmake --build ./build -j
sudo cmake --install ./build                     # Linux and macOS
```

On Windows, run `cmake --install ./build` from a terminal started as Administrator. To
install into another directory, add `--prefix /opt/areg`.

Build options, default locations and cross-compilation:
[Building Areg SDK with CMake](./01b-cmake-build.md#step-6-install-optional).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Use the Installed Package

```cmake
cmake_minimum_required(VERSION 3.20)
project(myapp)

find_package(areg CONFIG REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE areg::areg)
```

If Areg is installed under a custom prefix, pass it when configuring:

```bash
cmake -B ./build -DCMAKE_PREFIX_PATH=/opt/areg
```

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### CMake Cannot Find the areg Package

`find_package(areg CONFIG REQUIRED)` fails when the install prefix is not on CMake's search
path. Pass `-DCMAKE_PREFIX_PATH=<prefix>` with the directory given to `--prefix`.

### Java Not Found During the Build

The code generator needs Java 17 or newer. Install it, check `java -version`, and configure
the build again.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## See Also

- [Quick Project Setup](./02a-quick-project-setup.md): a new project in one command
- [CMake Integration](./02b-cmake-integrate.md): FetchContent and Git submodule
- [MSVC Integration](./02c-msvc-integrate.md): Visual Studio projects
- [CMake Configuration Guide](./02d-cmake-config.md)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech (Artak Avetyan), www.areg.tech, email: info[at]areg.tech
