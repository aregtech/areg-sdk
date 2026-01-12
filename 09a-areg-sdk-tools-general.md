# Areg SDK Development Tools

Areg SDK provides a set of **core and optional tools** that support service-oriented development, code generation, logging, monitoring, and diagnostics for distributed C++ applications. Some tools are built and shipped as part of the SDK, while others are provided as standalone utilities or GUI-based tools.

This document describes the available tools, their purpose, and how to build or obtain them.

---

## Table of Contents
- [Overview of Tools](#overview-of-tools)
- [1. Code Generator (`codegen.jar`)](#1-code-generator-codegenjar)
  - [Purpose](#purpose)
  - [Usage Model](#usage-model)
  - [Build and Availability](#build-and-availability)
- [2. Log Collector (`logcollector`)](#2-log-collector-logcollector)
  - [Purpose](#purpose-1)
  - [Execution Modes](#execution-modes)
  - [Build Instructions](#build-instructions)
  - [Configuration](#configuration)
- [3. Log Observer (`logobserver`)](#3-log-observer-logobserver)
  - [Purpose](#purpose-2)
  - [Typical Use Cases](#typical-use-cases)
  - [Build Instructions](#build-instructions-1)
- [4. Lusan (GUI Tool)](#4-lusan-gui-tool)
  - [Purpose](#purpose-3)
  - [Repository](#repository)
  - [Build Instructions](#build-instructions-2)
- [Tool Relationships](#tool-relationships)
- [Summary](#summary)

---

## Overview of Tools

| Tool           | Type        | Purpose                                                                  |
|----------------|-------------|--------------------------------------------------------------------------|
| `codegen.jar`  | Build-time  | Generates service-related C++ code from service interface definitions    |
| `logcollector` | Runtime     | Collects logs from one or multiple Areg-based applications               |
| `logobserver`  | Runtime     | Controls and records logging behavior (scopes, priorities)               |
| `lusan`        | GUI Tool    | Visual service design, log collection, visualization, and analysis       |

---

## 1. Code Generator (`codegen.jar`)

### Purpose

`codegen.jar` is the **service code generator** used by Areg SDK. It processes **service interface definition files** and generates the C++ source code required to implement:

- **Service Providers**
- **Service Consumers**
- RPC method stubs
- Event and attribute bindings

The generated code guarantees interface consistency, removes manual boilerplate, and significantly reduces development errors and effort.

### Usage Model

- Integrated directly into the **CMake build process**
- Automatically executed during project builds
- No manual invocation required in normal workflows

### Build and Availability

- Built automatically when building **areg-sdk**
- Requires **Java 17 or newer**
- Output location depends on the selected build configuration

> [!NOTE]
> No separate build step is required for `codegen.jar` when building Areg SDK.

---

## 2. Log Collector (`logcollector`)

### Purpose

`logcollector` is a **runtime log aggregation service** responsible for collecting logs from one or multiple Areg-based applications running across threads, processes, or devices.

Key capabilities include:
- Centralized log collection
- Support for distributed and multi-instance systems
- Required backend for GUI-based log visualization tools (e.g. Lusan)

### Execution Modes

- Console application
- System service (recommended for production and long-running systems)

### Build Instructions

`logcollector` is built as part of the **areg-sdk** build process.

```bash
cmake -B build
cmake --build build
````

After building, the executable is located in the SDK output `bin` directory.

### Configuration

* Uses the `areg.init` configuration file
* Requires correct IP address and port settings for TCP/IP-based logging

---

## 3. Log Observer (`logobserver`)

### Purpose

`logobserver` is a **lightweight, console-based tool** used to control and inspect the logging system at runtime. It allows developers to:

* Enable or disable log priorities
* Enable or disable log scopes
* Adjust logging behavior dynamically
* Save logs to files for offline analysis

> [!IMPORTANT]
> `logobserver` does **not** collect logs. Its role is to **control**, **observe**, and **record** logging behavior.

### Typical Use Cases

* Debugging without GUI tools
* Fine-grained runtime control of logging scopes and priorities
* Embedded or headless environments

### Build Instructions

`logobserver` is built together with Areg SDK:

```bash
cmake -B build
cmake --build build
```

The executable is placed in the SDK `bin` output directory.

---

## 4. Lusan (GUI Tool)

### Purpose

**Lusan** is a graphical development and diagnostics tool that complements Areg SDK. It is designed for **day-to-day developer workflows** as well as advanced system analysis and debugging.

Key capabilities include:

* Visual **Service Interface Design**
* Centralized **log collection**
* **Live and offline** log visualization
* Log filtering, correlation, and performance analysis
* Runtime control of logging scopes and priorities

### Repository

Lusan is hosted in a separate repository:

👉 [https://github.com/aregtech/areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

### Build Instructions

Lusan is built independently from the main SDK.

```bash
git clone https://github.com/aregtech/areg-sdk-tools.git
cd areg-sdk-tools
cmake -B build
cmake --build build
```

> [!IMPORTANT]
> Lusan requires Qt libraries and operates together with `logcollector`.

---

## Tool Relationships

```text
Service Interfaces
        ↓
   codegen.jar
        ↓
 Service Providers / Consumers
        ↓
   Areg-based Applications
        ↓
  logcollector  ←→  logobserver
        ↓
        Lusan
```

---

## Summary

* **`codegen.jar`** automates service code generation at build time
* **`logcollector`** aggregates logs from distributed applications
* **`logobserver`** controls and records logging behavior
* **`lusan`** enables visual service design and advanced log analysis

Together, these tools form a **complete development and diagnostics ecosystem** for building, testing, debugging, and scaling distributed C++ applications with Areg SDK.
