# Developer guide
```
This file is part of AREG SDK
Copyright (c) 2021-2023, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document is a developer guide and describes how to develop a service enabled application.

---

## File structure

Brief description of AREG SDK file structure:
1. The [framework/areg](https://github.com/aregtech/areg-sdk/tree/master/framework/areg/) directory contains _AREG engine_ (`areg` library).
2. The high level subdirectories of [framework/areg](https://github.com/aregtech/areg-sdk/tree/master/framework/areg/) contain declarations of classes, structures and types that can be used by developers to create an application.
3. All objects in `private` subdirectories contain codes for internal use and should not be used  by developers.
4. The [framework/mcrouter](https://github.com/aregtech/areg-sdk/tree/master/framework/mcrouter/) directory contains implementation of Multicast Router (or _mcrouter_).
5. The [framework/extend](https://github.com/aregtech/areg-sdk/tree/master/framework/extend/) directory contains extended objects of _AREG framework_ and may require extra library dependency (configurable).
6. The [thirdparty](https://github.com/aregtech/areg-sdk/tree/master/thirdparty) directory contains third-party modules that SDK has dependencies.
7 The [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/) directory contains working examples to demonstrate _AREG_ features.
8. The [tests](https://github.com/aregtech/areg-sdk/tree/master/tests/) directory contains _Unit Tests_ (planned to have benchmark tests).
9. The [tools](https://github.com/aregtech/areg-sdk/tree/master/tools/) directory contains code generator (planed to have design tools).
10. The [conf](https://github.com/aregtech/areg-sdk/tree/master/conf/) directory contains build configuration with `cmake`, `make` and `msbuild` tools.
11. The [docs](https://github.com/aregtech/areg-sdk/tree/master/docs/) directory contains documentation.
12. The detailed documentations are in [Wiki](https://github.com/aregtech/areg-sdk/wiki).

---

## Service Interface prototype

For a comprehensive guide on the definition and creation of a Service Interface, please refer to the document titled [`ServiceInterface.md`](https://github.com/aregtech/areg-sdk/blob/master/docs/ServiceInterface.md).

---

## Hello Service!

[Hello Service!"](https://github.com/aregtech/areg-sdk/blob/master/docs/HelloService.md) is a tutorial demonstrating how to create service provider and consumer objects that can operate within a single thread, a single application, or across a network using multiprocessing.

---

## Testing examples

> 💡 More testing examples are listed in the [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/) directory.

Firstly, [build](https://github.com/aregtech/areg-sdk/wiki/03.-Software-build) all projects using [`cmake`](https://github.com/aregtech/areg-sdk/wiki/03.-Software-build#build-with-cmake-cmakeliststxt), [`make`](https://github.com/aregtech/areg-sdk/wiki/03.-Software-build#build-with-make-makefile) or [`MSBuild`](https://github.com/aregtech/areg-sdk/wiki/03.-Software-build#build-with-msbuild-areg-sdksln).

_Example of quick build using `cmake`:_
```bash
$ cmake -B ./build
$ cmake --build build -j 8
```

_Example of quick build using `make`:_
```bash
$ make all -j 8
```

_Example of quick build using `msbuild`:_
```bash
> msbuild .
```

All compiled binaries are located in `<areg-sdk>/product/build/<compiler-platform-path>/bin` directory.

If the [listed example](https://github.com/aregtech/areg-sdk/blob/master/examples/README.md) is marked as an **IPC** (inter-process communication), the application contains a _Public_ services and requires start of `mcrouter`. Otherwise, the application contains _Local_ service and can run without `mcrouter` as a multithreading application. Some applications may create logs in the `logs` subdirectory to analyze.

---

## Creating Your Own Project

The [Setup a Project](https://github.com/aregtech/areg-sdk/wiki/08.-Setup-a-project) Wiki page offers detailed instructions and steps to help you set up your project effectively. It provides comprehensive guidance on how to integrate the AREG SDK sources into your build process and utilize the functionalities of the AREG Framework.

By referring to the [Setup a Project](https://github.com/aregtech/areg-sdk/wiki/08.-Setup-a-project) Wiki page, you will find valuable information on configuring your project, incorporating the AREG SDK, and leveraging the features offered by the AREG Framework to build robust and efficient applications.

---

## Contact us!

Contact us at info[at]aregtech.com if you need help, training or more information.
If you linked the project and the idea behind, please ![star the AREG SDK project](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=star%20the%20AREG%20SDK%20project) at GitHub to thank the contributors and help us to enlarge the community.
