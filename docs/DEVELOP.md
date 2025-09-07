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
1. The [`framework/areg`](./../framework/areg/) directory contains _AREG communication engine_ (`areg` library), which is the core library used by all applications.
2. The [framework/aregextend](./../framework/aregextend/) directory contains extended objects of _AREG framework_ and may require extra library dependency (configurable).
3. The [framework/areglogger](./../framework/areglogger/) directory contains implementation of a Log Observer library to use in applications to receive logs.
4. The [framework/logcollector](./../framework/logcollector/) directory contains implementation of Log Collector (`logcollector`), which runs either as console application or OS-managed Service.
5. The [framework/logobserver](./../framework/logobserver/) directory contains implementation of Log Observer (`logobserver`), which runs as a console application.
6. The [framework/mtrouter](./../framework/mtrouter/) directory contains implementation of Multitarget Router (`mtrouter`), which runs either as console application or OS-managed Service.
7. The [thirdparty](./../thirdparty) directory contains third-party modules that SDK has dependencies.
8. The [examples](./../examples/) directory contains working examples to demonstrate _AREG_ features.
9. The [tests](./../tests/) directory contains _Unit Tests_ (planned to have benchmark tests).
10. The [tools](./../tools/) directory contains code generator (planed to have design tools).
11. The [conf](./../conf/) directory contains build configuration with `cmake` and `msbuild` tools.
12. The [docs](./../docs/) directory contains documentation.
13. The detailed documentations are in [Wiki](./../docs/wiki).

---

## Service Interface prototype

For a comprehensive guide on the definition and creation of a Service Interface, please refer to the document titled [`ServiceInterface.md`](./ServiceInterface.md).

---

## Hello Service!

[Hello Service!"](./HelloService.md) is a tutorial demonstrating how to create service provider and consumer objects that can operate within a single thread, a single application, or across a network using multiprocessing.

---

## Testing examples

Firstly, build all projects using [CMake](./../docs/wiki/01b-cmake-build.md) or [MSBuild](./../docs/wiki/01c-msvc-build.md).

**Example of quick build using `cmake`**:
```bash
cmake -B ./build
cmake --build ./build -j20
```

**Example of quick build using `msbuild`**:
```bash
msbuild .
```

All compiled AREG Framework and example binaries are located in `<areg-sdk>/product/build/..../bin` directory. See the [description of examples](./../examples/README.md). If the example is marked as **IPC** (Inter-process communication), the application contains a **Public Service** Provider and Consumer, and it requires start of `mtrouter` to run in the multiprocessing environment. Otherwise, the application contains **Local Service** Provider and Consumer, and it can run without `mtrouter` in a multithreading environment. Some applications may create logs in the `logs` subdirectory to analyze.

---

## Creating Your Own Project

You can setup your own project and easily integrate AREG SDK builds in your project. To integrate using CMake, refer to the [Integrating AREG Framework with CMake](./../docs/wiki/02c-cmake-integrate.md) document. To integrate using MSBuild (Microsoft Visual Studio), refer to the [Integrating AREG Framework with Microsoft Visual Studio](./../docs/wiki/02d-msvc-integrate.md) document. As a working example of integrated project, see [AREG SDK Demo](https://github.com/aregtech/areg-sdk-demo) repository.

---

## Contact us!

Contact us at info[at]aregtech.com if you need help, training or more information.
If you linked the project and the idea behind, please ![star the AREG SDK project](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=star%20the%20AREG%20SDK%20project) at GitHub to thank the contributors and help us to enlarge the community.
