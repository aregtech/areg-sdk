# Areg SDK Development Tools

Areg SDK provides a comprehensive suite of development tools supporting the complete lifecycle of service-oriented C++ applications—from initial project setup and code generation to runtime logging, monitoring, and diagnostics. Tools are categorized as **core utilities** (built with the SDK) and **optional tools** (standalone or GUI-based).

This document provides an overview of available tools, their purpose, and basic usage information. Detailed documentation for each tool is available in dedicated guides.

---

## Table of Contents
- [Overview of Tools](#overview-of-tools)
- [1. Project Setup Tool](#1-project-setup-tool)
- [2. Code Generator (`codegen.jar`)](#2-code-generator-codegenjar)
- [3. Log Collector (`logcollector`)](#3-log-collector-logcollector)
- [4. Log Observer (`logobserver`)](#4-log-observer-logobserver)
- [5. Lusan (GUI Tool)](#5-lusan-gui-tool)
- [Tool Ecosystem](#tool-ecosystem)
- [Summary](#summary)

---

## Overview of Tools

| Tool             | Type        | Purpose                                                              |
|------------------|-------------|----------------------------------------------------------------------|
| `project-setup`  | Script      | Interactive project generator for rapid Areg SDK project creation    |
| `codegen.jar`    | Build-time  | Service interface code generator (C++ stub/skeleton generation)      |
| `logcollector`   | Runtime     | Centralized log aggregation service for distributed applications     |
| `logobserver`    | Runtime     | Console-based logging control and inspection tool                    |
| `lusan`          | GUI Tool    | Visual service design, log visualization, and system diagnostics     |

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 1. Project Setup Tool

**Scripts:**
- Linux/macOS: `tools/project-setup.sh`
- Windows: `tools/project-setup.bat`

### Purpose

Automated project scaffold generator that creates production-ready Areg SDK projects in under 30 seconds through an interactive command-line interface.

### Features

- **Interactive Configuration**: Guided prompts for project name, location, and architecture
- **Architecture Selection**: Choose multithreading or multiprocessing model
- **Automatic Generation**: CMake build files, service interfaces, and source templates
- **Ready-to-Build**: Generated projects compile immediately without modifications

### Project Architectures

1. **Multithreading**: Service provider and consumer in the same process, separate threads
2. **Multiprocessing**: Service provider and consumer as independent processes (IPC-based)

### Generated Artifacts

- `CMakeLists.txt` - Top-level project configuration
- `src/services/HelloService.siml` - Sample service interface definition
- Provider/Consumer source files with basic implementation
- Configured build system ready for code generation and compilation

### Usage

```bash
# Linux/macOS
./areg-sdk/tools/project-setup.sh

# Windows
.\areg-sdk\tools\project-setup.bat
```

**Detailed Guide**: [Quick Project Setup](./02a-quick-project-setup.md)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 2. Code Generator (`codegen.jar`)

### Purpose

The **service code generator** transforms service interface definitions (`.siml` files) into production-ready C++ code, eliminating manual boilerplate and ensuring interface consistency across service providers and consumers.

### Generated Code Components

- Service provider base classes and stubs
- Service consumer proxy classes
- RPC method signatures and dispatch logic
- Event notification handlers
- Attribute access methods
- Serialization/deserialization code

### Integration Model

- **Seamless CMake Integration**: Automatically invoked during project builds
- **Zero Manual Intervention**: Runs transparently as part of the build pipeline
- **Dependency Tracking**: Regenerates code only when interface definitions change

### Build and Availability

- **Build Requirement**: Java 17 or newer
- **Build Process**: Automatically compiled when building Areg SDK
- **Location**: Output directory depends on build configuration (typically `product/build/<config>/bin`)

> [!NOTE]
> Developers do not need to invoke `codegen.jar` manually. The CMake build system handles code generation automatically.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 3. Log Collector (`logcollector`)

### Purpose

A **runtime log aggregation service** that collects and forwards log messages from distributed Areg applications running across multiple threads, processes, or physical devices.

### Key Capabilities

- **Centralized Logging**: Single collection point for multi-instance systems
- **Distributed Support**: Collects logs over TCP/IP from remote applications
- **Integration Point**: Required backend for GUI tools (e.g., Lusan)
- **Scalable Architecture**: Handles high-throughput logging from numerous sources

### Deployment Modes

- **Console Application**: Interactive debugging and development
- **System Service**: Production deployments and long-running systems

### Build Instructions

Built as part of the Areg SDK build process:

```bash
cmake -B ./build
cmake --build ./build -j
```

Executable location: `<build-output>/bin/logcollector`

### Configuration

Configured via `areg.init` file. Key settings:
- TCP/IP address and port (default: `localhost:8282`)
- Log targets (file, database, debug output)
- Message queue size and buffering behavior

**Reference**: [Key-Value Data Persistence](./key-value-persistence.md)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 4. Log Observer (`logobserver`)

### Purpose

A **lightweight console-based logging controller** that enables runtime inspection and manipulation of logging behavior without restarting applications or modifying configuration files.

### Core Functions

- **Scope Control**: Enable/disable individual log scopes or scope groups
- **Priority Adjustment**: Change logging priorities dynamically (DEBUG, INFO, WARN, ERROR, FATAL)
- **Log Recording**: Capture log streams to files for offline analysis
- **Runtime Inspection**: Query current logging configuration and active scopes

> [!IMPORTANT]
> `logobserver` **controls and observes** logging behavior but does not collect logs. Use `logcollector` for log aggregation.

### Use Cases

- Debugging in headless or embedded environments
- Fine-grained runtime diagnostics without GUI tools
- Production troubleshooting with minimal overhead
- Automated testing and log validation

### Build Instructions

Built with Areg SDK:

```bash
cmake -B ./build
cmake --build ./build -j
```

Executable location: `<build-output>/bin/logobserver`

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## 5. Lusan (GUI Tool)

### Purpose

**Lusan** (Areg SDK Tools) is a professional-grade graphical development and diagnostics suite designed for day-to-day development workflows and advanced system analysis.

### Core Features

- **Visual Service Designer**: Graphical interface for creating and editing `.siml` service definitions
- **Centralized Log Viewer**: Real-time and offline log visualization with advanced filtering
- **Performance Analysis**: Log correlation, timing analysis, and bottleneck identification
- **Runtime Control**: Interactive scope and priority management
- **Multi-Source Support**: Aggregate logs from distributed applications

### Repository

Lusan is developed and maintained separately:  
📦 [github.com/aregtech/areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

### Build Instructions

```bash
git clone https://github.com/aregtech/areg-sdk-tools.git
cd areg-sdk-tools
cmake -B ./build
cmake --build ./build -j
```

### Requirements

- **Qt Framework**: Qt 5.x or Qt 6.x
- **Log Collector**: Lusan connects to `logcollector` for log aggregation
- **Platform Support**: Windows, Linux, macOS

> [!TIP]
> Lusan significantly improves developer productivity through visual service design and powerful log analysis capabilities.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Tool Ecosystem

The Areg SDK tools form an integrated development and diagnostics pipeline:

```text
                   Project Setup Tool
                           ↓
              Service Interface Definition (.siml)
                           ↓
                     codegen.jar
                           ↓
           Generated Service Providers/Consumers
                           ↓
              Areg-based Applications (Runtime)
                           ↓
                    logcollector ←→ logobserver
                           ↓
                        Lusan
                    (Visualization & Analysis)
```

### Workflow Integration

1. **Project Creation**: Use `project-setup` to generate initial project structure
2. **Service Design**: Define service interfaces in `.siml` files (manually or via Lusan)
3. **Code Generation**: `codegen.jar` generates C++ code during build
4. **Development**: Implement business logic in generated base classes
5. **Debugging**: Use `logcollector` + `logobserver` or Lusan for runtime diagnostics

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Summary

Areg SDK provides a complete toolchain for distributed C++ application development:

| Development Phase | Tools |
|-------------------|-------|
| **Project Bootstrap** | `project-setup` script |
| **Service Design** | `.siml` files, Lusan GUI |
| **Code Generation** | `codegen.jar` (automated) |
| **Runtime Logging** | `logcollector`, `logobserver` |
| **Analysis & Debug** | Lusan, `logobserver` |

### Key Takeaways

- **`project-setup`**: Rapid project scaffolding (< 30 seconds)
- **`codegen.jar`**: Automated service code generation (zero manual effort)
- **`logcollector`**: Centralized log aggregation for distributed systems
- **`logobserver`**: Console-based runtime logging control
- **`lusan`**: Professional GUI for service design and log analysis

Together, these tools provide a **professional-grade development ecosystem** for building, testing, debugging, and deploying distributed service-oriented applications with Areg SDK.

---

For detailed documentation on specific tools, refer to their dedicated guides in the Areg SDK documentation repository.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
