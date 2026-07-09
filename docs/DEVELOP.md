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
| [framework/aregextend](../framework/aregextend/) | Extended features (may require additional dependencies) |
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

Compiled binaries are in `<areg-sdk>/product/build/<compiler>/<platform>/bin/`.

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
