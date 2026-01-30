# Building Lusan with CMake or Qt Creator

This guide explains how to build **Lusan**, the graphical development and diagnostics tool for the Areg SDK ecosystem. Lusan provides visual service interface design, centralized log collection, and advanced log analysis capabilities for distributed C++ applications.

Lusan is developed in the [`areg-sdk-tools`](https://github.com/aregtech/areg-sdk-tools) repository and supports two build workflows: **CMake** (command-line) and **Qt Creator** (IDE-based).

---

## Table of Contents

- [Overview](#overview)
- [Architecture and Dependencies](#architecture-and-dependencies)
- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
- [Building with CMake](#building-with-cmake)
- [Building with Qt Creator](#building-with-qt-creator)
- [Build Output](#build-output)
- [Running Lusan](#running-lusan)
- [Troubleshooting](#troubleshooting)

---

## Overview

**Lusan** is a professional-grade graphical tool designed for day-to-day development workflows with Areg SDK applications. It provides:

- **Visual Service Designer**: Create and edit `.siml` service interface definitions graphically
- **Centralized Log Collection**: Aggregate logs from distributed applications
- **Live Log Monitoring**: Real-time log visualization with filtering and search
- **Offline Log Analysis**: Load and analyze saved log files
- **Runtime Control**: Dynamically adjust logging scopes and priorities

Lusan is hosted separately from the core Areg SDK:  
📦 [github.com/aregtech/areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Architecture and Dependencies

Lusan is built on top of the **Areg Framework** and **Qt Framework**, combining Areg's distributed computing capabilities with Qt's cross-platform GUI toolkit.

### Core Components

**Areg Framework (`areg`)**  
Core middleware providing:
- Service-oriented communication
- Multi-threading and IPC infrastructure
- Logging system integration
- Message routing

**Areg Extend Library (`aregextend`)**  
Extended functionality layer providing:
- Advanced logging control
- Log persistence (save/load operations)
- Log parsing and data transformation
- Visualization support APIs

**Qt Framework**  
Cross-platform GUI toolkit providing:
- User interface components (Qt Widgets)
- Core utilities (Qt Core)
- Platform abstraction

### Automatic Dependency Management

If Areg SDK is not installed locally, the build system automatically:
1. Clones the Areg SDK repository from GitHub
2. Builds required Areg libraries (`areg`, `aregextend`)
3. Links them with Lusan
4. Copies runtime dependencies to the output directory

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Prerequisites

### Required Software

| Component | Version | Notes |
|-----------|---------|-------|
| **C++ Compiler** | C++17 compatible | GCC, Clang, MSVC, or MinGW |
| **CMake** | 3.20 or newer | Build system generator |
| **Qt Framework** | 5.x or 6.x | GUI toolkit (Qt 6 recommended) |
| **Git** | Any recent version | For cloning repositories |

### System Requirements

- **Internet Access**: Required for initial build if Areg SDK is not present locally
- **Disk Space**: ~500 MB for source code, build artifacts, and dependencies
- **RAM**: 4 GB minimum, 8 GB recommended for parallel builds

### Platform Support

- **Linux**: Tested on Ubuntu 20.04+, Fedora, Debian
- **Windows**: Windows 10/11 with MSVC 2019+ or MinGW
- **macOS**: macOS 11+ with Xcode Command Line Tools

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Getting Started

### Clone the Repository

```bash
git clone https://github.com/aregtech/areg-sdk-tools.git
cd areg-sdk-tools
```

### Repository Structure

```
areg-sdk-tools/
├── CMakeLists.txt           # Top-level build configuration
├── sources/                 # Lusan application source code
├── thirdparty/              # Third-party dependencies (if any)
└── README.md                # Project documentation
```

The repository contains all necessary build configuration and dependency integration logic. No additional setup is required before building.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Building with CMake

### Standard Build (Release)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Debug Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

### Build Process

During the build, CMake automatically:
1. **Checks for Areg SDK**: Searches for local Areg installation
2. **Fetches Dependencies**: Clones Areg SDK from GitHub if not found
3. **Builds Areg Libraries**: Compiles `areg` and `aregextend` static libraries
4. **Builds Lusan**: Compiles the Lusan application
5. **Builds Tools**: Compiles `mtrouter` and `logcollector` executables
6. **Copies Runtime Libraries**: Deploys Qt libraries to output directory

### Build Output Location

After successful build:
```
build/
└── bin/
    ├── lusan               # Main executable
    ├── mtrouter            # Message router service
    ├── logcollector        # Log collection service
    ├── libareg.*           # Areg Framework library
    ├── libaregextend.*     # Areg Extend library
    └── Qt*.dll / Qt*.so    # Qt runtime libraries
```

### Build Time

- **First build**: 5-10 minutes (includes Areg SDK compilation)
- **Incremental builds**: 30-60 seconds (Lusan changes only)

> [!TIP]
> Use `-j` without a number to let CMake automatically determine optimal parallelism based on available CPU cores.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Building with Qt Creator

Qt Creator is recommended when:
- Qt is not installed system-wide
- You prefer an IDE-based workflow
- You need integrated debugging and profiling

### Build Steps

**1. Launch Qt Creator**

**2. Open Project**
   - Select **File → Open File or Project**
   - Navigate to `areg-sdk-tools/`
   - Open `CMakeLists.txt`

**3. Configure Kit**
   - Qt Creator detects available Qt installations
   - Select or configure a **Qt Kit** matching your Qt version
   - Choose compiler toolchain (GCC, Clang, MSVC, or MinGW)

**4. Configure Build Settings**
   - Select build type: **Debug** or **Release**
   - Review CMake configuration arguments (optional)
   - Click **Configure Project**

**5. Build**
   - Click the **Build** button (hammer icon)
   - Or press `Ctrl+B` (Linux/Windows) / `Cmd+B` (macOS)

**6. Run**
   - Click the **Run** button (green play icon)
   - Or press `Ctrl+R` (Linux/Windows) / `Cmd+R` (macOS)

### Advantages of Qt Creator

- **Automatic Qt Resolution**: Detects and configures Qt libraries automatically
- **Integrated Debugging**: Set breakpoints, inspect variables, step through code
- **Code Navigation**: Fast symbol lookup and cross-referencing
- **CMake Integration**: Live CMake output and configuration editing

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Build Output

### Executable and Libraries

The build process generates a self-contained distribution in `build/bin/`:

| File | Description |
|------|-------------|
| `lusan` | Main Lusan executable |
| `mtrouter` | Message router service (IPC backbone) |
| `logcollector` | Log collection service (aggregates logs from applications) |
| `libareg.*` | Areg Framework runtime library |
| `libaregextend.*` | Areg Extend library (logging utilities) |
| `Qt*.dll` / `Qt*.so` / `Qt*.dylib` | Qt runtime libraries (Core, Widgets) |

### Configuration Files

Default configuration files may be generated in:
- `build/bin/areg.init` - Areg SDK configuration
- `build/bin/lusan.conf` - Lusan-specific settings (if applicable)

### Qt Runtime Libraries

The build system automatically copies required Qt libraries:
- **Qt Core**: Base utilities and event loop
- **Qt Widgets**: GUI components
- Platform-specific plugins (if needed)

This allows Lusan to run without a system-wide Qt installation.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Running Lusan

### Standalone Execution

```bash
cd build/bin
./lusan
```

Lusan can run independently for:
- Offline log file analysis
- Service interface design
- Configuration editing

### With Log Collector

For live log monitoring, run `logcollector` alongside Lusan:

**Terminal 1 (Log Collector)**:
```bash
cd build/bin
./logcollector
```

**Terminal 2 (Lusan)**:
```bash
cd build/bin
./lusan
```

**Terminal 3 (Your Application)**:
```bash
cd your-application/build
./your-app
```

Lusan connects to `logcollector`, which aggregates logs from all Areg-based applications on the network.

### Configuration

Edit `areg.init` to configure:
- Log collector IP address and port (default: `localhost:8282`)
- Message router IP address and port (default: `localhost:8181`)
- Logging scopes and priorities

See [Key-Value Data Persistence](./key-value-persistence.md) for configuration syntax.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Troubleshooting

### CMake Cannot Find Qt

**Error**: `CMake Error: Could not find Qt5 or Qt6`

**Solution**:
```bash
# Specify Qt installation path
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
```

Or set environment variable:
```bash
export CMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64
cmake -B build
```

### Missing Qt Runtime Libraries

**Error**: Application fails to start with "Qt library not found"

**Solution**: CMake should copy Qt libraries automatically. If missing, manually copy from Qt installation:
```bash
cp /path/to/Qt/6.x/gcc_64/lib/libQt*.so* build/bin/
```

### Areg SDK Clone Fails

**Error**: `Failed to clone Areg SDK repository`

**Solution**: Check internet connection or clone manually:
```bash
git clone https://github.com/aregtech/areg-sdk.git
cmake -B build -DAREG_SDK_ROOT=/path/to/areg-sdk
```

### Build Fails on Windows (MinGW)

**Error**: Linker errors or missing symbols

**Solution**: Ensure MinGW version matches Qt compilation:
- If using Qt built with MinGW 11.2, use the same MinGW version
- Download matching MinGW from Qt Maintenance Tool

### Qt Creator Cannot Configure Project

**Error**: "No valid kits found"

**Solution**:
1. Go to **Tools → Options → Kits**
2. Auto-detect compilers and Qt versions
3. Create a manual kit if needed
4. Ensure CMake is configured in **Tools → Options → CMake**

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Summary

Lusan is a self-contained graphical tool built on Areg and Qt frameworks. The build process is fully automated:

- **CMake Workflow**: Command-line build for CI/CD and headless systems
- **Qt Creator Workflow**: IDE-based development with integrated debugging
- **Automatic Dependencies**: Fetches and builds Areg SDK if not present
- **Self-Contained Output**: All runtime libraries included in `build/bin/`

### Build Checklist

- ✅ C++17 compiler installed
- ✅ CMake 3.20+ available
- ✅ Qt 5.x or 6.x installed
- ✅ Internet access for initial build
- ✅ 4+ GB RAM for parallel builds

### Next Steps

After building Lusan:
1. Run `logcollector` for live log monitoring
2. Launch Lusan and connect to log collector
3. Start your Areg applications
4. Monitor logs in real-time or design service interfaces

For detailed usage instructions, refer to the Lusan user guide (coming soon).

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech