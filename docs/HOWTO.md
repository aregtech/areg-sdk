
# How To
```
This file is part of AREG SDK
Copyright (c) 2021-2026, Aregtech
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

This document replies to several **howto** questions, which are listed in the _Table of content_.

---

## Table of contents

- [1. How to use preprocessor defines](#1-how-to-use-preprocessor-defines)
- [2. How to build](#2-how-to-build)
- [3. How to create a project or integrate in project](#3-how-to-create-a-project-or-integrate-in-project)
- [4. How to use logging](#4-how-to-use-logging)
- [5. How to use Multitarget router](#5-how-to-use-Multitarget-router)

---

## 1. How to use preprocessor defines

The AREG SDK provides a range of preprocessor defines that developers can utilize during the code compilation process. These defines enable developers to customize the SDK's behavior according to their specific requirements. The [AREG SDK Preprocessor Definitions Guide](./wiki/02e-preprocessor-definitions.md) page in the AREG SDK Wiki document is a comprehensive guide, offering detailed explanations and examples on how to modify these defines during compilation.

By modifying the preprocessor defines, developers can enable or disable features, configure behaviors, and customize the SDK to suit their application's needs. The Wiki page acts as a valuable reference, ensuring a clear understanding of each define's purpose.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## 2. How to build

The AREG SDK source codes have a minimum requirement of **C++17**, CMake 3.20 or Microsoft Visual Studio, and Java 17 or higher for the code generator. They can be compiled using tools such as `cmake` or `msbuild`. The codes are compatible with **Windows** and **Linux** platforms, including compilation within **WSL** (_Windows Subsystem for Linux_), and can be build for x86, x86_64, arm32 and aarch64 (arm64) processors.

- The [Building AREG SDK with CMake](./wiki/01b-cmake-build.md) page provides a detailed explanation and examples on how to compile sources and build applications using CMake.
- The [Building the AREG SDK with Microsoft Visual Studio and MSBuild](./wiki/01c-msvc-build.md) page provides a detailed explanation and examples on how to compile sources and build applications using Microsoft Visual Studio.
- The [Building AREG SDK on Windows Subsystem for Linux (WSL)](./wiki/01d-wsl-build.md) page provides a detailed explanation and examples on how to compile sources and build applications using CMake in Windows Subsystem for Linux (WSL).

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## 3. How to create a project or integrate in project

You can setup your own project and easily integrate AREG SDK builds in your project. To integrate using CMake, refer to the [Integrating AREG Framework with CMake](./../docs/wiki/02c-cmake-integrate.md) document. To integrate using MSBuild (Microsoft Visual Studio), refer to the [Integrating AREG Framework with Microsoft Visual Studio](./../docs/wiki/02d-msvc-integrate.md) document. As a working example of integrated project, see [AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo) repository.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## 4. How to use logging

The projects based on AREG SDK can be compiled with or without logs. If a projects are compiled with logs, the logs, log scopes and group of scopes can be activated or deactivated during runtime.

- The [AREG SDK Logging Configuration Guide](./wiki/04a-logging-config.md) page of AREG SDK Wiki provides a detailed explanation and examples on how to configure the logs.
- The [Developing with AREG Logging System](./wiki/04b-logging-develop.md) page of AREG SDK Wiki provides a detailed explanation and examples on how to develop with logs.
- Other logging related documents are:
  - [AREG SDK Log Observer Application](./wiki/04c-logobserver.md)
  - [AREG SDK Log Collector Service](./wiki/04d-logcollector.md)

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## 5. How to use Multitarget router

Multitarget Router (MCR) is a part of AREG SDK and it is used for inter-process communication (IPC). The Multitarget router can run as a stand-alone application or as a service managed by operating system. The Multitarget should be configured and all applications should contain [areg.init](./../framework/areg/resources/areg.init) configuration file, so that they can connect to router to send and receive messages in real-time mode.

The [AREG SDK Multitarget Router](./wiki/05a-mtrouter.md) page of the AREG SDK Wiki page provides a detailed explanation and examples on how to configure the message router as well as how to initialize in the applications.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---
