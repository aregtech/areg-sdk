<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.areg.tech"><img align="center" src="https://github.com/aregtech/areg-sdk/raw/master/docs/img/areg-sdk-1280x360px-logo.png" alt="Areg SDK Home" style="width:70%;height:70%"/></a>
  <br /><br /><strong>Areg SDK Technical Guidance</strong>
</h1>

[**Areg Framework**](https://github.com/aregtech/areg-sdk) is a cutting-edge interface-centric solution designed to facilitate real-time asynchronous communication in distributed and mist-computing environments. The communication engine powering the Areg Software Development Kit (SDK) supports **Inter-Process Communication (IPC)**, it is built upon the **Object Remote Procedure Call (Object RPC)** protocol, helping developers create scalable, multitasking applications across multiple platforms. At the heart of the Areg SDK lies the Areg Framework written in C++, which comprises various software components and tools to simplify the development of distributed, real-time, and asynchronous applications. This framework enables connected Things to interact and offer services, functioning like lightweight distributed micro-servers.

These pages serve as comprehensive resources, providing detailed descriptions of the Areg SDK framework, along with practical examples and a range of accompanying tools.

## Getting Started

The following sections guide you through installing, configuring, and using the Areg SDK, with links to more detailed documentation.

### 1. Installation and Build
   - **[Installing and Using Areg SDK with vcpkg Package Manager](./01a-areg-package.md)**: Guide to installing the Areg SDK with the `vcpkg` package manager.
   - **[Building Areg SDK with CMake](./01b-cmake-build.md)**: Instructions for building the Areg SDK using CMake.
   - **[Building the Areg SDK with Microsoft Visual Studio and MSBuild](./01c-msvc-build.md)**: Steps to set up and build the Areg SDK in Visual Studio.
   - **[Building Areg SDK on Windows Subsystem for Linux (WSL)](./01d-wsl-build.md)**: Building the Areg SDK on WSL for a Linux environment within Windows.

### 2. Build Options and Integrations
   - **[CMake Configuration Options for Building Areg SDK](./02a-cmake-config.md)**: Overview of CMake options for building and customizing Areg SDK.
   - **[Areg SDK CMake Functions and Macros](./02b-cmake-functions.md)**: Details on CMake functions and macros provided by Areg SDK.
   - **[Integrating Areg Framework with CMake](./02c-cmake-integrate.md)**: Steps to integrate Areg SDK in your CMake projects.
   - **[Integrating Areg Framework with Microsoft Visual Studio](./02d-msvc-integrate.md)**: How to link and use Areg SDK within Visual Studio projects.
   - **[Areg SDK Preprocessor Definitions Guide: Configuration and Usage Across Toolchains](./02e-preprocessor-definitions.md)**: Comprehensive guide to configuring preprocessor definitions for the Areg SDK across different development environments and toolchains.

### 3. Development Tools
   - **[Areg SDK Code Generator (`codegen.jar`)](./03a-code-generator.md)**: Guide for using Areg SDK's code generator to create service interfaces and streamline development.

### 4. Logging and Monitoring
   - **[Areg SDK Logging Configuration Guide](./04a-logging-config.md)**: Setting up and configuring logging within Areg SDK applications.
   - **[Developing with Areg Logging System](./04b-logging-develop.md)**: Instructions for integrating logging into your applications.
   - **[Areg SDK Log Observer Application](./04c-logobserver.md)**: A guide to observing logs in real time.
   - **[Areg SDK Log Collector Service](./04d-logcollector.md)**: Configuration and use of Areg SDK's log collector for centralized log management.

### 5. Networking and Communication
   - **[Areg SDK Multitarget Router](./05a-mtrouter.md)**: Information on setting up Multitarget communication for distributed applications.

### 6. Persistence
   - **[Key-Value Data Persistence Syntax in Areg SDK](./06a-persistence-syntax.md)**: Persistent data storage and management using Areg SDK's key-value storage options.

### 7. Troubleshooting
   - **[Troubleshooting WSL Updates](./07a-troubleshooting-wsl-update.md)**: Provides solutions for common issues encountered while updating the WSL.
   - **[Troubleshooting CMake Builds on Linux](./07b-troubleshooting-cmake-linux-builds.md)**: Provides solutions for common errors encountered during the CMake compilation process on Linux.
   - **[Integration Troubleshooting](./07c-troubshooting-integration.md)**: Provides solutions for common errors encountered during integration of Areg SDK sources or binaries in the projects.

### 8. Examples and Tests
   - **[Building and Running Examples and Tests](./08a-examples-and-tests.md)**: Instructions to exclude, build and run Examples and Unit Tests of Areg SDK.
   - **[Demo with Areg SDK]**(https://github.com/aregtech/areg-sdk-demo): A repository containing demos showcasing the capabilities of Areg SDK.
   - **[Areg SDK and Edge AI agent](https://github.com/aregtech/areg-edgeai)**: Edge AI agent service provider and consumer powered by Areg SDK.
   - **[Qt Integration with Areg SDK](https:github.com/aregtech/areg-sdk-tools)**: Lusan - Control Plane for Designing and Debugging Areg-Based Distributed Systems

### 9. Areg SDK Tools
   - **[Areg SDK Development Tools](./09a-areg-sdk-tools-general.md)**: Overview of development tools provided with Areg SDK.
   - **[Building Lusan with CMake or Qt Creator](./09b-build-lusan.md)**: Instructions for building the Lusan GUI tool for service design and log analysis.
   - **[Setup and Using Lusan](./09c-use-lusan.md)**: Guide to setting up and using Lusan for service interface design and log monitoring.
   - **[Creating Service Interface Documents with Lusan](./09d-create-service-interface.md)**: Instructions for creating service interface definition files using Lusan.
   - **[Lusan Live Log Viewer](./09e-lusan-live-log-viewer.md)**: Guide to using Lusan for real-time live log monitoring and analysis.
   - **[Lusan Offline Log Viewer](./09f-lusan-offline-log-viewer.md)**: Guide to using Lusan for offline log analysis.

```
Help us to make docs greater.
See something is wrong, unclear or need a help? 
Submit a change, open a discussion or ask Areg SDK community a question.

2023-2024 © Aregtech, www.areg.tech, email: info[at]areg.tech
```
