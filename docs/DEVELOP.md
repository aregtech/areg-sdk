# Developer guide
```
This file is part of AREG SDK
Copyright (c) 2021-2022, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document is a developer guide and describes how to develop a service enabled application.

---

## File structure

Brief description of AREG SDK file structure:
1. The framework of AREG SDK is in the [framework/areg](https://github.com/aregtech/areg-sdk/tree/master/framework/areg/) directory.
2. High level subdirectories in [framework/areg](https://github.com/aregtech/areg-sdk/tree/master/framework/areg/) have developer-usable classes.
3. All `private` subdirectories in framework contain code for internal use only.
4. The [framework/mcrouter](https://github.com/aregtech/areg-sdk/tree/master/framework/mcrouter/) directory is multicast router implementation.
5. The [framework/extensions](https://github.com/aregtech/areg-sdk/tree/master/framework/extensions/) directory has extended objects that may need extra libraries.
6. The [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/) directory contains demonstration of AREG engine features.
7. The [tests](https://github.com/aregtech/areg-sdk/tree/master/tests/) directory contains _Unit Tests_ and later will contain benchmark tests.
8. The [tools](https://github.com/aregtech/areg-sdk/tree/master/tools/) directory contains programs like design (in progress) and code generator.
9. The [conf](https://github.com/aregtech/areg-sdk/tree/master/conf/) directory contains build configuration with `cmake`, `make` and `msbuild`.
10. The [docs](https://github.com/aregtech/areg-sdk/tree/master/docs/) directory contains documentation.
11. The detailed documentations are in [Wiki](https://github.com/aregtech/areg-sdk/wiki) (in progress).

---

## Service Interface prototype

For a comprehensive guide on the definition and creation of a Service Interface, please refer to the document titled [`ServiceInterface.md`](https://github.com/aregtech/areg-sdk/blob/master/docs/ServiceInterface.md).

---

## Hello Service!

[Hello Service!"](https://github.com/aregtech/areg-sdk/blob/master/docs/HelloService.md) is a tutorial demonstrating how to create service provider and consumer objects that can operate within a single thread, a single application, or across a network using multiprocessing.

---

## Testing examples

> 💡 More testing examples are listed in the [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/) directory.

1. Firstly, compile all projects using [`cmake`](https://github.com/aregtech/areg-sdk/wiki/02.-Software-build#build-with-cmake-cmakeliststxt),  compile all projects, including example, call `make all` in command line.
2. To compile example projects, call `make examples` in the command line.
3. All compiled files are located in `<areg-sdk>/product/build/<compiler-platform-path>/bin` folder.
4. If running an application with a _Public_ service (**IPC** projects), start `mcrouter`.
5. There is no need to start the _mcrouter_ if testing an application with no _Public_ service.
6. All compiled files are in the `bin` folder, switch there to start applications.
7. Some applications may create logs in the `logs` folder of binary location. Open to analyze logs.

---

## Contact us!

Contact us at info[at]aregtech.com if you need help, training or more information.
If you linked the project and the idea behind, please ![star the AREG SDK project](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=star%20the%20AREG%20SDK%20project) at GitHub to thank the contributors and help us to enlarge the community.
