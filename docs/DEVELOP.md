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
1. The framework of AREG SDK is in the [framework/areg](../framework/areg/) directory.
2. High level subdirectories in [framework/areg](../framework/areg/) have developer-usable classes.
3. All `private` subdirectories in framework contain code for internal use only.
4. The [framework/mcrouter](../framework/mcrouter/) directory is multicast router implementation.
5. The [framework/extensions](../framework/extensions/) directory has extended objects that may need extra libraries.
6. The [examples](../examples/) directory contains demonstration of AREG engine features.
7. The [tests](../tests/] directory contains _Unit Tests_ and later will contain benchmark tests.
8. The [tools](../tools/) directory contains programs like design (in progress) and code generator.
9. The [conf](../conf) directory contains build configuration with `cmake`, `make` and `msbuild`.
10. The [docs](../docs) directory contains documentation.
11. The detailed documentations are in [Wiki](https://github.com/aregtech/areg-sdk/wiki) (in progress).

---

## Service Interface prototype

For a comprehensive guide on the definition and creation of a Service Interface, please refer to the document titled [`ServiceInterface.md`](./ServiceInterface.md).

---

## Hello Service!

[Hello Service!"](./HelloService.md) is a tutorial demonstrating how to create service provider and consumer objects that can operate within a single thread, a single application, or across a network using multiprocessing.

---

## Testing

> 💡 More testing examples are listed in the [examples](../examples/) folder of `areg-sdk`.

1. To compile all projects, call `make all` in command line.
2. To compile example projects, call `make examples` in the command line.
3. All compiled files are located in `<areg-sdk>/product/build/<compiler-platform-path>/bin` folder.
4. If running an application with a _Public_ service (**IPC** projects), start `mcrouter`.
5. There is no need to start the _mcrouter_ if testing an application with no _Public_ service.
6. All compiled files are in the `bin` folder, switch there to start applications.
7. Some applications may create logs in the `logs` folder of binary location. Open to analyze logs.

---

## Contact us!

Contact us at info[at]aregtech.com if you need help, training or more information.
If you linked the project and the idea behind that, please ![star the AREG SDK project](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=star%20the%20AREG%20SDK%20project) at GitHub, so that contributors feel appreciated for the job.
