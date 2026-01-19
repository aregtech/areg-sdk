# Building Lusan with CMake or Qt Creator

This document explains how to build **Lusan**, the graphical development and diagnostics tool of the **Areg SDK** ecosystem. It covers prerequisites, supported build workflows, dependencies, Qt requirements, and build outputs.

Lusan is built from the [`areg-sdk-tools`](https://github.com/aregtech/areg-sdk-tools) repository and can be compiled using **CMake** from the command line or **Qt Creator**.

---

## Table of Contents

- [Overview](#overview)
- [Architecture and Dependencies](#architecture-and-dependencies)
- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
- [Building with CMake](#building-with-cmake)
- [Building with Qt Creator](#building-with-qt-creator)
- [Qt Requirements](#qt-requirements)
- [Build Output](#build-output)
- [Notes and Recommendations](#notes-and-recommendations)
- [Summary](#summary)

---

## Overview

**Lusan** is a graphical development and diagnostics application that complements **Areg SDK**. It is used for:
- Service interface design
- Centralized log collection
- Live log monitoring
- Offline log analysis
- Runtime logging control

Lusan is built independently from the core Areg SDK and is hosted at:  
👉 https://github.com/aregtech/areg-sdk-tools

---

## Architecture and Dependencies

Lusan is built on top of the **Areg Framework** and the **Qt Framework**, and uses the **extended Areg static library (`aregextend`)**.

### Core Components

🔹**Areg Framework (`areg`)**  
  Core middleware providing service communication, threading, IPC, and logging infrastructure.

🔹**Areg Extend Library (`aregextend`)**  
  Utility layer optimized for:
  - Logging control
  - Log persistence (save and load)
  - Log parsing and visualization support

🔹**Qt Framework**  
  Used exclusively for the graphical user interface.

If the Areg SDK is not installed on the system, Lusan automatically fetches the required Areg sources from GitHub and builds them as part of the build process.

---

## Prerequisites

Before building Lusan, ensure the following are available:
- C++17 compatible compiler (GCC, Clang, MSVC, or MinGW)
- CMake **3.20+**
- Qt **5 or newer**
- Internet access for the initial build if the Areg SDK is not present locally

---

## Getting Started

Clone the repository:

```bash
git clone https://github.com/aregtech/areg-sdk-tools.git
cd areg-sdk-tools
```

The repository contains:
- CMake build configuration
- Lusan application sources
- Dependency integration logic

---

## Building with CMake

To build Lusan using the command line:

```bash
cmake -B build
cmake --build build -j20
```

During the build:
- Areg SDK is automatically cloned and built if not found
- `areg` and `aregextend` are compiled as dependencies
- Qt libraries are dynamically linked
- Required runtime libraries are copied to the output directory

After a successful build, the Lusan executable is available in `./build/bin`

---

## Building with Qt Creator

Qt Creator is recommended when Qt is not installed system-wide or when an IDE-based workflow is preferred.

Steps:
1. Open **Qt Creator**
2. Select **Open Project**
3. Open `CMakeLists.txt` from the `areg-sdk-tools` directory
4. Configure:
   - Compiler toolchain
   - Build type (Debug or Release)
5. Select or configure a Qt Kit
6. Build the project

Qt Creator automatically resolves Qt dependencies. All required Areg SDK components are fetched and built if missing.

---

## Qt Requirements

Lusan uses only a minimal subset of Qt.

🔹Required modules:
- **Qt Core**
- **Qt Widgets**

After the build, Qt runtime libraries are automatically copied into the `build/bin` directory. This allows Lusan to run without a global Qt installation at runtime.

---

## Build Output

After a successful build, the output directory contains:
- `lusan` executable
- Areg Framework libraries
- `mtrouter` and `logcollector` executables
  (can run as console applications or system services)
- Required Qt runtime libraries
- Configuration files

This layout allows Lusan to be executed directly from the build output directory.

---

## Notes and Recommendations

- Prefer **Qt Creator** when Qt is not installed system-wide
- Ensure internet access for the initial dependency fetch
- Use **Release** builds for daily development and diagnostics
- Run `logcollector` alongside Lusan for live log visualization

---

## Summary

Lusan is a standalone graphical tool built on top of the **Areg** and **Qt** Frameworks. It can be built using CMake or Qt Creator and requires only a minimal Qt setup. The build process is fully automated, including dependency fetching and runtime library preparation.

Lusan is an essential tool for service interface design, logging, monitoring, and diagnostics in distributed C++ applications built with **Areg SDK**.
