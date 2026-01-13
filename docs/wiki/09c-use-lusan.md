# Building and Using Lusan

This document provides an overview of how to build and use **Lusan**, the graphical development and diagnostics tool of the **Areg SDK** ecosystem. It explains how to build the application, set up a workspace, configure folders and connections, adjust runtime settings, and use Lusan for everyday development and diagnostics tasks.

---

## Table of Contents

- [Overview](#overview)
- [What Is Lusan Used For](#what-is-lusan-used-for)
- [Prerequisites](#prerequisites)
- [Building Lusan](#building-lusan)
- [Starting Lusan](#starting-lusan)
- [Workspace Concept](#workspace-concept)
- [Setting Up a Workspace](#setting-up-a-workspace)
- [Configuring Folders and Paths](#configuring-folders-and-paths)
- [Configuration and Runtime Settings](#configuration-and-runtime-settings)
- [Core Workflows](#core-workflows)
  - [Service Interface Design](#service-interface-design)
  - [Live Log Monitoring](#live-log-monitoring)
  - [Offline Log Analysis](#offline-log-analysis)
- [Saving and Reusing Configuration](#saving-and-reusing-configuration)
- [Best Practices](#best-practices)
- [Summary](#summary)

---

## Overview

**Lusan** is a graphical tool that supports the full lifecycle of Areg-based distributed applications. It combines **visual service interface design**, **log collection**, **live monitoring**, and **offline analysis** in a single, developer-focused environment.

Lusan is part of the Areg SDK toolchain, for live log collection it works together with `logcollector`.

---

## What Is Lusan Used For

Lusan is commonly used to:

- Design and maintain **service interface documents** (`.siml` files)
- Monitor logs from distributed applications in real time
- Analyze recorded logs offline
- Control logging scopes and priorities at runtime

It is intended for **daily development**, **debugging**, **system analysis**, and **integration testing**.

---

## Prerequisites

Before using Lusan, ensure that:

- Lusan is built and installed  
  👉 https://github.com/aregtech/areg-sdk-tools
- Qt libraries are installed
- `logcollector` is built and accessible
- Network configuration (`lusan.init`) is properly prepared if logging is used

---

## Building Lusan

Lusan is hosted in a separate repository than Areg SDK sources:  
👉 [https://github.com/aregtech/areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools)

Lusan is built independently from the main SDK.

```bash
git clone https://github.com/aregtech/areg-sdk-tools.git
cd areg-sdk-tools
cmake -B build
cmake --build build
```

If Areg SDK package is not installed on the system, it will automatically fetch the sources from GitHub and build the dependent libraries of Areg SDK. The users should make sure that internet connection is available during the build.

> [!IMPORTANT]
> Lusan requires Qt libraries and operates together with `logcollector`. If Qt library package is not installed, it is recommended to use Qt-Creator to build the application:
>  - Open `areg-sdk-tools` directory in Qt-Creator;
>  - Setup compiler configuration (it supports GCC, Clang, MSVC, MinGW) and the build configuration (Debug or Release)
>  - Build the system. All required dependencies, including Areg Framework library, are automatically copied in the build directory

---

## Starting Lusan

Launch Lusan from the build output or installation directory.

On startup, if `Lusan` is started for the first time, it prompts to create new workspace:

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/lusan-workspace-setup.png"><img src="./docs/img/lusan-workspace-setup.png" alt="Lusan workspace setup" style="width:80%;height:80%"/></a></div>

If `Lusan` has been used before, initializes the list of workspaces and offers the last accessed workspace as the default to load.

---

## Workspace Concept

A **workspace** in Lusan defines the environment in which users work. It groups together:
- The project directory (workspace directory)
- Source code directory
- Include directory
- Delivery directory (delivery by thirdparty project)
- Log directory

The workspace concept allows Lusan to adapt and browse different structures of the project to visualize service interfaces and logs. Within the workspace, the user can configure folders to open **Service interface files**, used for interface design and code generation, and **Log file**, which by default are saved in the log storage directory.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/lusan-app.png"><img src="./docs/img/lusan-app.png" alt="Lusan Application" style="width:80%;height:80%"/></a></div>

All directories of the workspace, except the project (workspace) directory are optional, they are displayed in the `Workspace` navigation tab of `Lusan` and can be changed at any time by accessing menu `Tools` => `Options` => `Directories`:

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/lusan-option-workspace-dirs.png"><img src="./docs/img/lusan-option-workspace-dirs.png" alt="Lusan option workspace directories" style="width:80%;height:80%"/></a></div>

The developer can remove any workspace entry, except the currently opened one, from the list of available workspaces:

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/lusan-option-workspace-list.png"><img src="./docs/img/lusan-option-workspace-list.png" alt="Lusan option list of workspaces" style="width:80%;height:80%"/></a></div>

These paths can be updated at any time to match project layout. The updates are immediately saved in the workspace configuration.

---

## Logcollector Configuration

Beside setting and changing workspace settings, the users of `lusan` can set, change and test the connection to the `logcollector` service:

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/lusan-option-logcollector.png"><img src="./docs/img/lusan-option-logcollector.png" alt="Lusan Logcollector settings" style="width:80%;height:80%"/></a></div>

If change the `logcollector` connection settings, the user can test the connection by pressing the `Test` button. The result of connection success is displayed in the `Test Status` box. The connection settings are saved in the `lusan.init` configuration used when `lusan` starts.

---

## Core Workflows

### Service Interface Design

Within the workspace, the user can create or open service interface documents.  
Lusan provides a structured, visual editor to define data types, data attributes, methods, constants and additional include files specific for the service.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/lusan-service-if-general.png"><img src="./docs/img/lusan-service-if-general.png" alt="Lusan Service Interface" style="width:80%;height:80%"/></a></div>

These interfaces are validated and later consumed by the Areg code generator during build when calling `addServiceInterface()` (or `addServiceInterfaceEx()`) `CMake` function like explained in [Creating Service Interface Documents with Lusan](./09d-create-service-interface.md) document.

---

### Live Log Monitoring

When connected to a running `logcollector`, Lusan displays logs in real time from one or multiple applications.

Developers can:
- Filter logs
- Correlate events across components
- Trace request execution
- Control logging behavior at runtime

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/lusan-live-log-gen.png"><img src="./docs/img/lusan-live-log-gen.png" alt="Lusan Live Logging app" style="width:80%;height:80%"/></a></div>

---

### Offline Log Analysis

Previously recorded logs can be loaded directly into Lusan for offline analysis.

This mode is useful for:
- Post-mortem debugging
- Investigating intermittent issues
- Sharing logs with other team members

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/lusan-offline-log-gen.png"><img src="./docs/img/lusan-offline-log-gen.png" alt="Lusan Offline Logging app" style="width:80%;height:80%"/></a></div>

---

## Runtime Settings

Lusan allows users to modify configuration settings without restarting applications. Common configuration areas include:
- Logging scopes and priorities
- Display and filtering preferences

Changes take effect immediately and can be adjusted dynamically during development.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/lusan-live-log-scope-prio.png"><img src="./docs/img/lusan-live-log-scope-prio.png" alt="Lusan Log Scope Priority change" style="width:80%;height:80%"/></a></div>

When log priorities are updated, the user can save the current configuration to a file by pressing the `Save` button. The saved configuration file ensures that on next start applications will start with the specified log priority.

---

## Best Practices

- Use one workspace per project or system
- Keep service interfaces under version control
- Record logs for complex test scenarios
- Adjust log verbosity dynamically instead of restarting applications
- Reuse workspace configurations across team members when possible

---

## Summary

Lusan provides a **central workspace-driven environment** for designing, configuring, and analyzing Areg-based distributed systems. By combining service modeling, live observability, offline analysis, and runtime control in one tool, Lusan significantly improves developer productivity and system insight.

It is a core part of the Areg SDK workflow for building reliable, scalable distributed applications.
