# Developer Guide

```
This file is part of Areg SDK
Copyright (c) 2021-2026, Aregtech (Artak Avetyan)
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

This guide provides technical information for developing service-enabled applications with Areg SDK.

---

## Project Structure

| Directory | Description |
|-----------|-------------|
| [framework/areg](../framework/areg/) | Core communication engine library |
| [framework/aregextend](../framework/aregextend/) | Optional objects that extend `areg`: console, SQLite database, OS-managed services |
| [framework/areglogger](../framework/areglogger/) | Log observer library for receiving logs |
| [framework/logcollector](../framework/logcollector/) | Log collection service (console or system service) |
| [framework/logobserver](../framework/logobserver/) | Real-time log monitoring console application |
| [framework/mtrouter](../framework/mtrouter/) | Multitarget Router for IPC (console or system service) |
| [thirdparty](../thirdparty/) | Third-party dependencies |
| [examples](../examples/) | Working examples demonstrating features |
| [tests](../tests/) | Unit tests |
| [tools](../tools/) | Code generator and project scaffolding |
| [conf](../conf/) | Build configuration (CMake, MSBuild) |
| [docs](../docs/) | Documentation |
| [docs/wiki](../docs/wiki/) | Detailed wiki pages |

---

## Service Interface

A Service Interface defines the API for communication between service providers and consumers. For a complete guide on creating service interfaces, see [ServiceInterface.md](./ServiceInterface.md).

---

## Hello Service Tutorial

The [Hello Service Tutorial](./HelloService.md) demonstrates how to create service provider and consumer components that work:

- Within a single thread
- Across multiple threads in one process
- Across multiple processes on a network

---

## Building and Running Examples

### Quick Build with CMake

```bash
cmake -B ./build
cmake --build ./build -j20
```

### Quick Build with MSBuild

```bash
msbuild .
```

### Output Location

Everything of a build goes to the directory given to `cmake -B`: the CMake cache, the object
files, the generated code in `generate/`, the binaries in `bin/` and the static libraries in
`lib/`. `cmake -B build` puts the binaries in `<areg-sdk>/build/bin/`. A build in the Areg SDK
root itself (`cmake .`) is refused.

`-DAREG_OUTPUT_DIR=<dir>` moves only `bin/` and `lib/` to `<dir>`; a relative path is relative to
the build directory. `generate/` stays in the build directory.

### Several Builds Side by Side

Keep every build in its own directory under `product/build/`, named
`<compiler-family>-<build-type>-<processor>[-static]` in lowercase. All builds can then be deleted
at once with `product/`, and each one is restored by configuring and building it again:

```bash
cmake -B product/build/gnu-release-x64   -DAREG_COMPILER_FAMILY=gnu  -DCMAKE_BUILD_TYPE=Release
cmake -B product/build/llvm-release-x64  -DAREG_COMPILER_FAMILY=llvm -DCMAKE_BUILD_TYPE=Release
cmake -B product/build/gnu-debug-x86     -DAREG_COMPILER_FAMILY=gnu  -DCMAKE_BUILD_TYPE=Debug -DAREG_ARCH=x86
cmake -B product/build/gnu-release-x64-static -DAREG_COMPILER_FAMILY=gnu -DAREG_LIB_TYPE=static
cmake --build product/build/llvm-release-x64 -j
```

`sh tools/build.sh` (`tools\build.bat` on Windows) names and fills the directory itself, for
example `sh tools/build.sh -c llvm -t debug` for `product/build/llvm-debug-x64`; `--help` lists
the options.

A binary built by one compiler can be run against a router built by another, for example
`product/build/llvm-release-x64/bin/mtrouter.elf` with the applications of
`product/build/gnu-release-x64/bin/`. Never point two builds at the same `AREG_OUTPUT_DIR`: libraries
of the same name replace each other.

On Windows, use the Ninja generator from a Visual Studio developer prompt, one directory per
configuration. The Visual Studio generator builds every configuration into the same `bin/`, so a
Debug build replaces the Release binaries:

```powershell
cmake -B product/build/msvc-debug-x64   -G Ninja -DAREG_COMPILER_FAMILY=msvc -DCMAKE_BUILD_TYPE=Debug
cmake -B product/build/msvc-release-x64 -G Ninja -DAREG_COMPILER_FAMILY=msvc -DCMAKE_BUILD_TYPE=Release
```

`tools\build.bat` keeps the Visual Studio generator safe the same way: it builds only one
configuration into each directory, `product/build/msvc-debug-x64` and `product/build/msvc-release-x64`.

### Example Types

| Type | Description | mtrouter Required |
|------|-------------|-------------------|
| Local | Multithreaded (single process) | No |
| IPC | Multiprocess (Public services) | Yes |

See the [examples README](../examples/README.md) for descriptions of each example. Some examples write logs to a `logs` subdirectory.

---

## Creating Your Own Project

### Integration Options

| Build System | Guide |
|--------------|-------|
| CMake | [Integrating with CMake](./wiki/02b-cmake-integrate.md) |
| Visual Studio | [Integrating with Visual Studio](./wiki/02c-msvc-integrate.md) |

### Reference Project

See the [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) repository for a working integrated project.

---

## Contact

For help, training, or more information, contact us at **info[at]areg.tech**.

If you find this project useful, please star the [Areg SDK repository](https://github.com/aregtech/areg-sdk) on GitHub to support the project and help grow the community.
