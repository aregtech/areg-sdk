
# How To
```
This file is part of AREG SDK
Copyright (c) 2021-2022, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document replies to several **howto** questions, which are listed in the _Table of content_.

---

## Table of contents

1. [How to use preprocessor defines](#how-to-use-preprocessor-defines)
2. [How to build](#how-to-build)
3. [How to debug in IDE](#how-to-debug-in-ide)
    - [Debug with Visual Studio Code](#debug-with-visual-studio-code)
    - [Debug with Microsoft Visual Studio](#debug-with-microsoft-visual-studio)
4. [How to create a new project](#how-to-create-a-new-project)
5. [How to integrate in a project](#how-to-integrate-in-a-project)
6. [How to use logging](#how-to-use-logging)
    - [Enable logging](#enable-logging)
    - [Configure logging](#configure-logging)
7. [How to use multicast router](#how-to-use-multicast-router)

---

## How to use preprocessor defines

The AREG SDK components make use of various preprocessor defines during the code compilation process. These preprocessor defines allow developers to customize the behavior of the SDK according to their specific requirements. The [Preprocessor-define-symbols](https://github.com/aregtech/areg-sdk/wiki/02.-Preprocessor-define-symbols) page in the AREG SDK Wiki provides a comprehensive list of these defines, along with detailed explanations and examples on how to modify them during compilation.

By modifying the preprocessor defines, developers can enable or disable certain features, configure specific behaviors, or customize the SDK to meet their application's needs. The Wiki page serves as a reference guide, providing a clear understanding of each define and its purpose.

To utilize the preprocessor defines, developers can follow these steps:

1. Refer to the [Preprocessor-define-symbols](https://github.com/aregtech/areg-sdk/wiki/02.-Preprocessor-define-symbols) page in the AREG SDK Wiki to familiarize themselves with the available defines and their functionalities.

2. Identify the specific preprocessor define(s) that need to be modified for the desired behavior or feature configuration.

3. Depending on the build system being used (such as CMake, Make, Microsoft Visual Studio, or Visual Studio Code), update the corresponding configuration files or build commands to set the desired values for the preprocessor defines.

4. Rebuild the AREG SDK components using the updated preprocessor defines.

By utilizing the preprocessor defines effectively, developers can tailor the AREG SDK to their specific project requirements and achieve the desired functionality and behavior.

It is recommended to refer to the [Preprocessor-define-symbols](https://github.com/aregtech/areg-sdk/wiki/02.-Preprocessor-define-symbols) page in the AREG SDK Wiki for a comprehensive understanding of each define and its usage in order to leverage the full potential of the SDK.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## How to build

The AREG SDK source codes have a minimum requirement of **C++17**. They can be compiled using tools such as `cmake`, `make`, `cygwin`, or `msbuild`. The codes are compatible with **Windows**, **Linux**, and **macOS** environments, including compilation within **WSL** (_Windows Subsystem for Linux_).

The [Software Build](https://github.com/aregtech/areg-sdk/wiki/03.-Software-build) page of AREG SDK Wiki provides a detailed explanation and examples on how to compile sources and build applications using various tools and IDEs.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## How to create a project or integrate in project

The [Setup a project](https://github.com/aregtech/areg-sdk/wiki/07.-Setup-a-project) page of AREG SDK Wiki provides a detailed explanation and examples on how to create a project based on AREG framework, as well as how to include AREG framework in your build environment.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## How to use logging

The projects based on AREG SDK can be compiled with or without logs. If a projects are compiled with logs, the logs, log scopes and group of scopes can be activated or deactivated during runtime.

The [AREG Logging System](https://github.com/aregtech/areg-sdk/wiki/05.-AREG-Logging-System) page of AREG SDK Wiki provides a detailed explanation and examples on how to compile source codes with logs, as well as hot to create logging scopes, log messages and configure the logging.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---

## How to use multicast router

Multicast Router (MCR) is a part of AREG SDK and it is used for inter-process communication (IPC). The multicast router can run as a stand-alone application or as a service managed by operating system. The multicast should be configured and all applications should contain [router.init](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/router.init) configuration file, so that they can connect to router to send and receive messages in real-time mode.

The [Message-Router](https://github.com/aregtech/areg-sdk/wiki/06.-Message-Router) page of the AREG SDK Wiki page provides a detailed explanation and examples on how to configure the message router as well as how to initialize in the applications.

<div align="right">[ <a href="#table-of-contents">↑ Back to top ↑</a> ]</div>

---
