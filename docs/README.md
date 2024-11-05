# Information

```
This file is part of the AREG SDK
Copyright (c) Aregtech, 2021-2023
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document provides an overview of the file system structure of the **AREG SDK** and the location of various documents.

## AREG SDK File System Structure

```
areg-sdk  ......... --> The root folder of the AREG SDK.
    |
    +- conf ....... --> Configuration files for Makefile, CMake, and MS Visual Studio.
    |
    +- docs ....... --> Location of documentation files for the AREG SDK.
    |
    +- examples  .. --> Examples demonstrating the usage of the AREG framework and services.
    |
    +- framework .. --> Source code of the AREG framework and Multicast Router.
    |   |
    |   +- areg  .. --> Source code of the AREG framework.
    |   |
    |   +- mcrouter --> Source code of the Multicast Router (MCR).
    |   |
    |   +- system . --> Development platform-specific common code.
    |
    +- tests  ..... --> Unit tests for the AREG Framework.
    |
    +- thirdparty . --> External dependencies.
    |
    +- LICENSE.txt  --> License file. Please read before using or distributing.
    |
    +- README.md  . --> ReadMe file providing project description.

```

Classes defined within each subdirectory of the `framework/areg` folder can be used to develop applications. However, classes declared in the `private` subdirectory of each of them are intended for internal use and should not be used by developers.

After compiling using CMake, Make, or MS Visual Studio, the root folder `areg-sdk` will contain a recoverable `product` subfolder that contains the built binaries.

## Document Folder Structure:

```
docs
  |
  +- /img ................ --> Folder containing images used in the documents.
  |
  +- /templates .......... --> Folder containing useful templates to create a projects.
  |
  +- /wiki ............... --> AREG SDK Wiki pages.
  |
  +- DEVELOP.md .......... --> Developer guide and technical explanations.
  |
  +- HelloService.md ..... --> Explanation of the HelloService.siml Service Interface document.
  |
  +- HelloService.siml ... --> Practical example Service Interface document.
  |
  +- HOWTO.md ............ --> Instructions on how to build the software.
  |
  +- POSIX.md ............ --> Instructions to compile applications in Linux.
  |
  +- README.md ........... --> This document file.
  |
  +- Sample.siml ......... --> Sample Service Interface document with structures.
  |
  +- ServiceInterface.md . --> Detailed explanation of the Service Interface structure and usage.
  |
  +- USECASES.md ......... --> Use cases of the AREG SDK.
  |
  +- WIN32.md ............ --> Instructions to compile applications in Windows.
```

> 💡 Please visit [AREG SDK Wiki](https://github.com/aregtech/areg-sdk/wiki) pages for more detailed documentation, which is periodically updating. Any help to improve the Wiki pages are appreciated very much.

## Shortcuts

- The source code of the AREG SDK is located in the [`<areg-sdk>/framework`](https://github.com/aregtech/areg-sdk/tree/master/framework) directory.
- The source code of the examples is located in the [`<areg-sdk>/examples`](https://github.com/aregtech/areg-sdk/tree/master/examples) directory.
- The common and user-specific CMake configuration files are located in [`<areg-sdk>/conf/cmake`](https://github.com/aregtech/areg-sdk/tree/master/conf/cmake).
- The common and user-specific Makefile configuration files are located in [`<areg-sdk>/conf/make`](https://github.com/aregtech/areg-sdk/tree/master/conf/make).
- The common and user-specific property files for MS Visual Studio are located in [`<areg-sdk>/conf/msvc`](https://github.com/aregtech/areg-sdk/tree/master/conf/msvc).
- The [**developer guide**](./DEVELOP.md) provides technical details and guidance for developing distributed and service-enabled applications.
- The [HOWTO](./HOWTO.md) document explains how to export projects and perform builds.
- The [POSIX](./POSIX.md) document contains POSIX-related configuration, compilation instructions, and a list of used APIs.
- The [WIN32](./WIN32.md) document contains Windows-related configuration, compilation instructions, and a list of used APIs.
- The [ServiceInterface](./ServiceInterface.md) document provides an explanation of the Service Interface structure and usage.
- The [USECASES](./USECASES.md) document presents various use cases of the AREG SDK in real-world scenarios.
- The [AREG SDK Wiki](https://github.com/aregtech/areg-sdk/wiki) pages contains detailed documentation of the project.
