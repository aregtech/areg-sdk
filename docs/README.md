# Information

```
This file is part of the Areg SDK
Copyright (c) Aregtech, 2021-2026
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

This document provides an overview of the file system structure of the **Areg SDK** and the location of various documents.

## Areg SDK File System Structure

```
areg-sdk .............. --> The root folder of the Areg SDK.
    |
    +- conf ........... --> Configuration files for Makefile, CMake, and MS Visual Studio.
    |
    +- docs ........... --> Location of documentation files for the Areg SDK.
    |   |
    |   +- img ........ --> Images and screen-shots used in documents.
    |   |
    |   +- templates .. --> MSVC project templates to use when create a project.
    |   |
    |   +- wiki ....... --> The Wiki pages of Areg SDK.
    |
    +- examples ....... --> Examples demonstrating the usage of the Areg framework and services.
    |
    +- framework ...... --> Source code of the Areg framework and Multitarget Router.
    |   |
    |   +- areg ....... --> Source code of the Areg framework.
    |   |
    |   +- aregextend . --> A static library with object of extended features
    |   |
    |   +- areglogger . --> A library, designed to facilitate log observation and management.
    |   |
    |   +- logcollector --> An application as a tool for collecting logs from multiple processes across a network.
    |   |
    |   +- logobserver  --> A console application designed to manage and analyze logs in real time.
    |   |
    |   +- mtrouter ... --> Source code of the Multitarget Router (MCR).
    |
    +- tests .......... --> Unit tests for the Areg Framework.
    |
    +- thirdparty ..... --> External dependencies.
    |
    +- LICENSE.txt .... --> License file. Please read before using or distributing.
    |
    +- README.md ...... --> ReadMe file providing project description.

```

Classes defined within each subdirectory of the `framework/areg` folder can be used to develop applications. However, classes declared in the `private` subdirectory of each of them are intended for internal use and should not be used by developers.

After compiling using CMake, Make, or MS Visual Studio, the root folder `areg-sdk` will contain a recoverable `product` subdirectory that contains the built binaries.

> [!TIP]
> Visit [Areg SDK Wiki](./wiki/README.md) page for details of available documentation, which is periodically updating. Any help to improve the Wiki pages are appreciated very much.

## Shortcuts

- The [**Developer Guide**](./DEVELOP.md) provides technical details and guidance for developing distributed and service-enabled applications.
- The [**HOWTO**](./HOWTO.md) document explains how to export projects and perform builds.
- The [**POSIX**](./POSIX.md) document contains POSIX-related configuration, compilation instructions, and a list of used APIs.
- The [**WIN32**](./WIN32.md) document contains Windows-related configuration, compilation instructions, and a list of used APIs.
- The [**ServiceInterface**](./ServiceInterface.md) document provides an explanation of the Service Interface structure and usage.
- The [**USECASES**](./USECASES.md) document presents various use cases of the Areg SDK in real-world scenarios.
- The [**Areg SDK Wiki**](./wiki/README.md) pages contains detailed documentation of the project.
