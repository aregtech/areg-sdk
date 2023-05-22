
# How To
```
This file is part of AREG SDK
Copyright (c) 2021-2022, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document replies to several **howto** questions, which are listed in the _Table of content_.

---

## Table of content[![](./docs/img/pin.svg)](#table-of-contents)

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

## How to use preprocessor defines[![](./img/pin.svg)](#how-to-use-preprocessor-defines)

The AREG SDK components utilize several preprocessor defines during the code compilation process. Below is a comprehensive list of these defines with brief descriptions. For more details, please visit [Preprocessor-define-symbols](https://github.com/aregtech/areg-sdk/wiki/03.-Preprocessor-define-symbols) Wiki page of AREG SDK.

| Preprocessor | Description and Meaning |
| --- | --- |
| **POSIX** | The define instructs the compiler to compile the sources using the _POSIX API_. POSIX API is listed in the [POSIX.md](./POSIX.md) file. |
| **WINDOWS** | The define instructs the compiler to compile the sources using the _WIN32 API_. WIN32 API is listed in the [WIN32.md](./WIN32.md) file. |
| **DEBUG** | The define is used to specify the *Debug* configuration. |
| **NDEBUG** | The define is used to specify the *Release* configuration. |
| **EXP_AREG_LIB** | The define is set when AREG framework is built as a _static_ library. It is functionally equivalent to **EXPORT_STATIC_SYMBOLS**. |
| **EXP_AREG_DLL** | The define is set when AREG framework is built as a _shared_ library. It is functionally equivalent to **EXPORT_SHARED_SYMBOLS**. |
| **IMP_AREG_LIB** | The define is set when a module is linked with the _static_ AREG framework library. It is functionally equivalent to **IMPORT_STATIC_SYMBOLS**. |
| **IMP_AREG_DLL** | The define is set when a module is linked with the _shared_ AREG framework library. It is functionally equivalent to **IMPORT_SHARED_SYMBOLS**. |
| **ENABLE_TRACES** | The define enables the compilation of log statements within the source codes. |

The defines are automatically set based on options specified in the [user.props](https://github.com/aregtech/areg-sdk/blob/master/conf/msvc/user.props), [user.cmake](https://github.com/aregtech/areg-sdk/blob/master/conf/cmake/user.cmake), or [user.mk](https://github.com/aregtech/areg-sdk/blob/master/conf/make/user.mk) files when the project is compiled using the appropriate tool.

Bellow is a quick description of preprocessor defines and settings.

### `POSIX` and `WINDOWS` defines

The _POSIX_ and _WINDOWS_ defines play a crucial role in determining whether projects are compiled using the _POSIX_ or _Win32_ API. These defines are automatically set based on the build tool and compiler selection. 

For instance, when compile with `cmake` and the compiler family is selected any compiler, except `msvc` (for example compile with option `-DAREG_COMPILER_FAMILY=gnu`), the sources are compiled with the _POSIX API_. On the other hand, if `msvc` is set, the sources will be compiled with the _Win32 API_.

The [POSIX define](https://github.com/aregtech/areg-sdk/wiki/03.-Preprocessor-define-symbols#posix-define) and [WINDOWS define](https://github.com/aregtech/areg-sdk/wiki/03.-Preprocessor-define-symbols#windows-define) chapters of AREG SDK Wiki offer detailed explanations and examples to set preprocessor defines during compilation.

### `DEBUG` and `NDEBUG` defines

The _DEBUG_ and _NDEBUG_ defines are responsible for indicating the specific build configuration. You can set specific build configuration either by changing user configuration file like `user.cmake` or `user.mk`, or via the command line option.

The [DEBUG and NDEBUG defines](https://github.com/aregtech/areg-sdk/wiki/03.-Preprocessor-define-symbols#debug-and-ndebug-defines) chapter of AREG SDK Wiki provides detailed explanations and examples on how to set preprocessor defines during compilation.
 
### `EXP_AREG_LIB`, `EXP_AREG_DLL`, `IMP_AREG_LIB` and `IMP_AREG_DLL` defines

The **EXP_AREG_LIB**, **EXP_AREG_DLL**, **IMP_AREG_LIB**, and **IMP_AREG_DLL** preprocessor defines serve a specific purpose in determining the configuration of the `areg` library of the AREG Framework and its linkage with an application. These defines help determine whether the `areg` library is compiled as a shared library or a static library, as well as whether the application should be linked with the shared or static version of the "areg" library.

The configuration for these defines is set when in configuration file like [user.cmake](https://github.com/aregtech/areg-sdk/blob/master/conf/cmake/user.cmake) the library type is specified (`set(AREG_BINARY "shared")`). By default, the AREG Framework is compiled as shared library. By selecting the library type, these preprocessor defines are automatically set for AREG Framework and all other applications using the library. The library binary type can be as well selecting via the command line options during compilation.

The [EXP_AREG_DLL and EXP_AREG_LIB defines](https://github.com/aregtech/areg-sdk/wiki/03.-Preprocessor-define-symbols#exp_areg_dll-and-exp_areg_lib-defines) and [IMP_AREG_DLL and IMP_AREG_LIB defines](https://github.com/aregtech/areg-sdk/wiki/03.-Preprocessor-define-symbols#imp_areg_dll-and-imp_areg_lib-defines) chapters of the AREG SDK Wiki provide detailed explanation and examples on how to set preprocessor defines during compilation.

### `ENABLE_TRACES` define

This preprocessor define determines whether the compilation includes logging functionality. Enabling the define allows applications to log messages during runtime, while disabling it removes logging from the builds and prevents the application from logging messages.

The [ENABLE_TRACES define](https://github.com/aregtech/areg-sdk/wiki/03.-Preprocessor-define-symbols#enable_traces-define) chapter of the AREG SDK Wiki provides detailed explanation and examples on how to compile sources with and without logs.

---

## How to build

The AREG SDK source codes have a minimum requirement of **C++17**. They can be compiled using tools such as `cmake`, `make`, `cygwin`, or `msbuild`. The codes are compatible with **Windows**, **Linux**, and **macOS** environments, including compilation within **WSL** (_Windows Subsystem for Linux_).

The [Software Build](https://github.com/aregtech/areg-sdk/wiki/02.-Software-build) page of AREG SDK Wiki provides a detailed explanation and examples on how to compile sources and build applications using various tools and IDEs.

---

## How to create a project or integrate in project

The [Setup a project](https://github.com/aregtech/areg-sdk/wiki/07.-Setup-a-project) page of AREG SDK Wiki provides a detailed explanation and examples on how to create a project based on AREG framework, as well as how to include AREG framework in your build environment.

---

## How to use logging

The projects based on AREG SDK can be compiled with or without logs. If a projects are compiled with logs, the logs, log scopes and group of scopes can be activated or deactivated during runtime.

The [AREG Logging System](https://github.com/aregtech/areg-sdk/wiki/05.-AREG-Logging-System) page of AREG SDK Wiki provides a detailed explanation and examples on how to compile source codes with logs, as well as hot to create logging scopes, log messages and configure the logging.

---

## How to use multicast router

Multicast Router (MCR) is a part of AREG SDK and it is used for inter-process communication (IPC). The multicast router can run as a stand-alone application or as a service managed by operating system. The multicast should be configured and all applications should contain [router.init](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/router.init) configuration file, so that they can connect to router to send and receive messages in real-time mode.

The [Message-Router](https://github.com/aregtech/areg-sdk/wiki/06.-Message-Router) page of the AREG SDK Wiki page provides a detailed explanation and examples on how to configure the message router as well as how to initialize in the applications.

---
