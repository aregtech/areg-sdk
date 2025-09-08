<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="https://github.com/aregtech/areg-sdk/raw/master/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:70%;height:70%"/></a>
  <br /><br /><strong>AREG SDK Technical Guidance</strong>
</h1>

[**AREG (Automated Real-time Event Grid) Framework**](https://github.com/aregtech/areg-sdk) is a cutting-edge interface-centric solution designed to facilitate real-time asynchronous communication in distributed and mist-computing environments. The communication engine powering the AREG Software Development Kit (SDK) supports **Inter-Process Communication (IPC)**, it is built upon the **Object Remote Procedure Call (Object RPC)** protocol, helping developers create scalable, multitasking applications across multiple platforms. At the heart of the AREG SDK lies the AREG Framework written in C++, which comprises various software components and tools to simplify the development of distributed, real-time, and asynchronous applications. This framework enables connected Things to interact and offer services, functioning like lightweight distributed micro-servers.

These pages serve as comprehensive resources, providing detailed descriptions of the AREG SDK framework, along with practical examples and a range of accompanying tools.

## Getting Started

The following sections guide you through installing, configuring, and using the AREG SDK, with links to more detailed documentation.

### 1. Installation and Build
   - **[Installing and Using AREG SDK with vcpkg Package Manager](./01a-areg-package.md)**: Guide to installing the AREG SDK with the `vcpkg` package manager.
   - **[Building AREG SDK with CMake](./01b-cmake-build.md)**: Instructions for building the AREG SDK using CMake.
   - **[Building the AREG SDK with Microsoft Visual Studio and MSBuild](./01c-msvc-build.md)**: Steps to set up and build the AREG SDK in Visual Studio.
   - **[Building AREG SDK on Windows Subsystem for Linux (WSL)](./01d-wsl-build.md)**: Building the AREG SDK on WSL for a Linux environment within Windows.

### 2. Build Options and Integrations
   - **[CMake Configuration Options for Building AREG SDK](./02a-cmake-config.md)**: Overview of CMake options for building and customizing AREG SDK.
   - **[AREG SDK CMake Functions and Macros](./02b-cmake-functions.md)**: Details on CMake functions and macros provided by AREG SDK.
   - **[Integrating AREG Framework with CMake](./02c-cmake-integrate.md)**: Steps to integrate AREG SDK in your CMake projects.
   - **[Integrating AREG Framework with Microsoft Visual Studio](./02d-msvc-integrate.md)**: How to link and use AREG SDK within Visual Studio projects.
   - **[AREG SDK Preprocessor Definitions Guide: Configuration and Usage Across Toolchains](./02e-preprocessor-definitions.md)**: Comprehensive guide to configuring preprocessor definitions for the AREG SDK across different development environments and toolchains.

### 3. Development Tools
   - **[AREG SDK Code Generator (`codegen.jar`)](./03a-code-generator.md)**: Guide for using AREG SDK's code generator to create service interfaces and streamline development.

### 4. Logging and Monitoring
   - **[AREG SDK Logging Configuration Guide](./04a-logging-config.md)**: Setting up and configuring logging within AREG SDK applications.
   - **[Developing with AREG Logging System](./04b-logging-develop.md)**: Instructions for integrating logging into your applications.
   - **[AREG SDK Log Observer Application](./04c-logobserver.md)**: A guide to observing logs in real time.
   - **[AREG SDK Log Collector Service](./04d-logcollector.md)**: Configuration and use of AREG SDK's log collector for centralized log management.

### 5. Networking and Communication
   - **[AREG SDK Multitarget Router](./05a-mtrouter.md)**: Information on setting up Multitarget communication for distributed applications.

### 6. Persistence
   - **[Key-Value Data Persistence Syntax in AREG SDK](./06a-persistence-syntax.md)**: Persistent data storage and management using AREG SDK's key-value storage options.

### 7. Troubleshooting
   - **[Troubleshooting WSL Updates](./07a-troubleshooting-wsl-update.md)**: Provides solutions for common issues encountered while updating the WSL.
   - **[Troubleshooting CMake Builds on Linux](./07b-troubleshooting-cmake-linux-builds.md)**: Provides solutions for common errors encountered during the CMake compilation process on Linux.
   - **[Integration Troubleshooting](./07c-troubshooting-integration.md)**: Provides solutions for common errors encountered during integration of AREG SDK sources or binaries in the projects.

### 8. Examples and Tests
   - **[Building and Running Examples and Tests](./08a-examples-and-tests.md)**: Instructions to exclude, build and run Examples and Unit Tests of AREG SDK.


```
Help us to make docs greater.
See something is wrong, unclear or need a help? 
Submit a change, open a discussion or ask AREG SDK community a question.

2023-2024 © Aregtech, www.aregtech.com, email: info[at]aregtech.com
```
