<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.areg.tech"><img align="center" src="https://github.com/aregtech/areg-sdk/raw/master/docs/img/areg-sdk-1280x360px-logo.png" alt="Areg SDK Home" style="width:70%;height:70%"/></a>
  <br /><br /><strong>Areg SDK Technical Guidance</strong>
</h1>

[**Areg Framework**](https://github.com/aregtech/areg-sdk) is an interface-centric C++ framework designed for building **real-time, asynchronous, and distributed systems** in embedded, desktop, cloud, and mist-computing environments.

At its core, the Areg SDK provides a high-performance **Inter-Process Communication (IPC)** engine based on the **Object Remote Procedure Call (Object RPC)** protocol. It enables applications to expose and consume services across process and machine boundaries with minimal coupling and strong runtime guarantees.

The Areg SDK includes:
- A modern C++ framework for service-oriented and event-driven architectures
- A code generation toolchain for strongly typed service interfaces
- Built-in logging, monitoring, and persistence infrastructure
- Developer tools for system design, debugging, and diagnostics

Areg-based applications behave as lightweight distributed micro-servers that dynamically discover, connect, and communicate with each other at runtime.

This wiki serves as the **authoritative technical reference and learning hub** for Areg SDK. It contains installation guides, architecture explanations, developer tutorials, troubleshooting guides, and real-world examples.

---

## Table of Contents

- [Getting Started](#getting-started)
- [1. Installation and Build](#1-installation-and-build)
- [2. Build Options and Integrations](#2-build-options-and-integrations)
- [3. Networking and Communication](#3-networking-and-communication)
- [4. Logging and Monitoring](#4-logging-and-monitoring)
- [5. Persistence](#5-persistence)
- [6. Development and Testing Tools](#6-development-and-testing-tools)
- [7. Troubleshooting](#7-troubleshooting)
- [8. Examples and Tests](#8-examples-and-tests)
- [Contribute and Improve](#contribute-and-improve)

---

## Getting Started

The following sections are organized to help you progress from **installation** to **advanced system design**, with each document focusing on a specific aspect of the Areg SDK ecosystem.

---

## 1. Installation and Build

Guides for installing and building the Areg SDK across supported platforms and toolchains.

- **[Installing and Using Areg SDK with vcpkg Package Manager](./01a-areg-package.md)**  
  Install Areg SDK using the `vcpkg` package manager for fast and repeatable dependency management.

- **[Building Areg SDK with CMake](./01b-cmake-build.md)**  
  Step-by-step instructions for building Areg SDK using CMake on Linux, macOS, and Windows.

- **[Building the Areg SDK with Microsoft Visual Studio and MSBuild](./01c-msvc-build.md)**  
  Build and configure Areg SDK using native Microsoft Visual Studio tooling.

- **[Building Areg SDK on Windows Subsystem for Linux (WSL)](./01d-wsl-build.md)**  
  Run and build Areg SDK in a Linux environment hosted on Windows via WSL.

---

## 2. Build Options and Integrations

Configuration, customization, and integration of Areg SDK into existing projects.

- **[CMake Configuration Options for Building Areg SDK](./02a-cmake-config.md)**  
  Complete reference of CMake options controlling features, modules, and build behavior.

- **[Areg SDK CMake Functions and Macros](./02b-cmake-functions.md)**  
  Description of helper CMake functions and macros provided by Areg SDK.

- **[Integrating Areg Framework with CMake](./02c-cmake-integrate.md)**  
  Best practices for consuming Areg SDK from external CMake-based projects.

- **[Integrating Areg Framework with Microsoft Visual Studio](./02d-msvc-integrate.md)**  
  How to integrate Areg SDK binaries or sources into Visual Studio solutions.

- **[Areg SDK Preprocessor Definitions Guide: Configuration and Usage Across Toolchains](./02e-preprocessor-definitions.md)**  
  Detailed guide to preprocessor switches affecting logging, IPC, threading, and runtime behavior.

---

## 3. Networking and Communication

Advanced communication routing for distributed deployments.

- **[Areg SDK Multitarget Router](./03a-mtrouter.md)**  
  Configure and deploy multitarget routing for cross-network service communication.

---

## 4. Logging and Monitoring

Centralized logging and diagnostics for distributed and multi-process systems.

- **[Areg SDK Logging Configuration Guide](./04a-logging-config.md)**  
  Configure logging scopes, priorities, and outputs.

- **[Developing with Areg Logging System](./04b-logging-develop.md)**  
  Best practices for instrumenting applications with logging.

- **[Areg SDK Log Observer Application](./04c-logobserver.md)**  
  Observe logs in real time using a lightweight console-based tool.

- **[Areg SDK Log Collector Service](./04d-logcollector.md)**  
  Aggregate logs from multiple applications into a centralized logging backend.

---

## 5. Persistence

Built-in persistent storage support.

- **[Key-Value Data Persistence Syntax in Areg SDK](./05a-persistence-syntax.md)**  
  Define and manage persistent key-value data across application restarts.

---

## 6. Development and Testing Tools

Tools that accelerate development and enforce architectural consistency.

- **[Areg SDK Tools](./06a-areg-sdk-tools.md)**  
  Overview of all tools shipped with Areg SDK.

- **[Areg SDK Code Generator (`codegen.jar`)](./06b-code-generator.md)**  
  Generate service interfaces, stubs, and proxies from interface definition files.

- **[Building Lusan with CMake or Qt Creator](./06c-build-lusan.md)**  
  Build the Lusan GUI application.

- **[Setup and Using Lusan](./06d-setup-lusan.md)**  
  Initial setup and core workflows in Lusan.

- **[Creating Service Interface Documents with Lusan](./06e-lusan-service-interface.md)**  
  Design service interfaces visually using Lusan.

- **[Lusan Live Log Viewer](./06f-lusan-live-logging.md)**  
  Monitor and analyze logs from running systems in real time.

- **[Lusan Offline Log Viewer](./06g-lusan-offline-logging.md)**  
  Perform post-mortem analysis using recorded log sessions.

---

## 7. Troubleshooting

Solutions for common build, integration, and runtime issues.

- **[Troubleshooting WSL Updates](./07a-troubleshooting-wsl-update.md)**  
  Fix issues related to WSL updates and environment changes.

- **[Troubleshooting CMake Builds on Linux](./07b-troubleshooting-cmake-linux-builds.md)**  
  Diagnose and resolve common Linux build problems.

- **[Integration Troubleshooting](./07c-troubshooting-integration.md)**  
  Resolve issues when integrating Areg SDK into external projects.

---

## 8. Examples and Tests

Hands-on examples and real-world demos.

- **[Building and Running Examples and Tests](./08a-examples-and-tests.md)**  
  Build, run, and customize Areg SDK examples and unit tests.

- **[Demo with Areg SDK](https://github.com/aregtech/areg-sdk-demo)**  
  Demonstration projects showcasing Areg SDK capabilities.

- **[Areg SDK and Edge AI agent](https://github.com/aregtech/areg-edgeai)**  
  Edge AI service provider and consumer architecture powered by Areg SDK.

- **[Qt Integration with Areg SDK](https://github.com/aregtech/areg-sdk-tools)**  
  Lusan: a control plane for designing, debugging, and monitoring Areg-based systems.

---

### Contribute and Improve

Help us make the documentation better.

If you find something unclear, incomplete, or incorrect:
- Submit a pull request
- Open a discussion
- Ask a question in the Areg SDK community

Your feedback directly improves the ecosystem.

---
```
Help us to make docs greater.
See something is wrong, unclear or need a help? 
Submit a change, open a discussion or ask Areg SDK community a question.
```

© 2023–2026 Aregtech  
https://www.areg.tech  
Contact: info[at]areg.tech
