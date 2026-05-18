# How-To Guide

```
This file is part of Areg SDK
Copyright (c) 2021-2026, Aregtech
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

This guide answers common questions about building, configuring, and using Areg SDK.

---

## Table of Contents

- [1. Preprocessor Defines](#1-preprocessor-defines)
- [2. Building the SDK](#2-building-the-sdk)
- [3. Creating or Integrating Projects](#3-creating-or-integrating-projects)
- [4. Using Logging](#4-using-logging)
- [5. Using the Multitarget Router](#5-using-the-multitarget-router)

---

## 1. Preprocessor Defines

Areg SDK provides preprocessor defines that customize behavior during compilation. You can enable or disable features, configure behaviors, and tailor the SDK to your application's requirements.

For a complete list of defines and usage examples, see the [Preprocessor Definitions Guide](./wiki/02f-preprocessor-definitions.md).

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## 2. Building the SDK

### Requirements

- **C++ Standard**: C++17 or later
- **Build Tools**: CMake 3.20+ or Microsoft Visual Studio
- **Code Generator**: Java 17+ (for generating service interface code)

### Supported Platforms

| Platform | Compilers                | Architectures             |
| -------- | ------------------------ | ------------------------- |
| Linux    | GCC 9+, Clang 10+        | x86, x86_64, ARM32, ARM64 |
| Windows  | MSVC 2019+, MinGW, Clang | x86, x86_64               |
| macOS    | Apple Clang              | x86_64, ARM64             |
| WSL      | GCC, Clang               | x86, x86_64               |

### Build Guides

- [Building with CMake](./wiki/01b-cmake-build.md) - Cross-platform builds using CMake
- [Building with Visual Studio](./wiki/01c-msvc-build.md) - Windows builds using MSBuild
- [Building on WSL](./wiki/01d-wsl-build.md) - Linux builds in Windows Subsystem for Linux

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## 3. Creating or Integrating Projects

You can create a new project or integrate Areg SDK into an existing one.

### Integration Options

| Method        | Guide                                                          |
| ------------- | -------------------------------------------------------------- |
| CMake         | [Integrating with CMake](./wiki/02b-cmake-integrate.md)        |
| Visual Studio | [Integrating with Visual Studio](./wiki/02c-msvc-integrate.md) |

### Quick Start

Use the project setup tool to scaffold a new project:

```bash
# Linux/macOS
./tools/setup-project.sh

# Windows
tools\setup-project.bat
```

### Working Example

See the [Areg SDK Demo](https://github.com/aregtech/areg-sdk-demo) repository for a complete integrated project example.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## 4. Using Logging

Projects built with Areg SDK can include or exclude logging at compile time. When logging is enabled, individual log scopes and scope groups can be activated or deactivated at runtime.

### Logging Documentation

| Document                                                 | Description                              |
| -------------------------------------------------------- | ---------------------------------------- |
| [Logging Configuration](./wiki/04a-logging-config.md)    | Configure log output, levels, and scopes |
| [Developing with Logging](./wiki/04b-logging-develop.md) | Add logging to your application code     |
| [Log Observer Application](./wiki/04c-logobserver.md)    | Monitor logs in real time                |
| [Log Collector Service](./wiki/04d-logcollector.md)      | Aggregate logs from multiple processes   |

### Quick Example

```cpp
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(myapp_main);

int main()
{
    Application::setup();
    TRACE_SCOPE(myapp_main);
    TRACE_INFO("Application started");
    // ... application code ...
    return 0;
}
```

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## 5. Using the Multitarget Router

The Multitarget Router (`mtrouter`) enables inter-process communication (IPC) between Areg-based applications. It can run as a standalone console application or as an operating system service.

### When You Need mtrouter

- Applications with **Public** services that communicate across processes
- Distributed systems with services on multiple devices
- Any multiprocess scenario using Areg IPC

### Configuration

All applications connecting to `mtrouter` need the [areg.init](../framework/areg/resources/areg.init) configuration file to specify connection settings.

### Documentation

See the [Multitarget Router Guide](./wiki/03a-mtrouter.md) for:
- Configuration options
- Running as a console application
- Installing as a system service (Linux, macOS, Windows)
- Network topology setup

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---
