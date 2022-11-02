# Information
```
This file is part of AREG SDK
Copyright (c) Aregtech, 2017-2022
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document describes the file system structure of **AREG SDK** and the location of documents.

## The structure of AREG SDK
```
areg-sdk  ......... --> the root folder of AREG SDK.
    |
    +- conf ....... --> the configurations of Makefile, CMake and MS Visual Studio specific files.
    |
    +- docs ....... --> the documents location of AREG SDK.
    |
    +- framework .. --> the source codes of AREG framework and Multicast Router.
    |   |
    |   +- areg  .. --> the source codes of AREG framework.
    |   |
    |   +- mcrouter --> the sources of Multicast Router (MCR).
    |   |
    |   +- system . --> the development platform specific common codes.
    |
    +- examples  .. --> examples of using AREG framework and services.
    |
    +- tools  ..... --> code generator and other tools
    |
    +- LICENSE.txt  --> the license. Please read before use or distribute.
    |
    +- README.md  . --> the project description ReadMe file.

```
After compiling using _CMake_, _Make_ or _MS Visual Studio_, the root folder _areg-sdk_ will contain a recoverable _product_ subfolder that contains build binaries.

## The structure of _the document folder_:

```
docs
  |
  +- /img ......... --> Folder of images used in documents.
  |
  +- DEVELOP.md ... --> The developer guide and technical explanations.
  |
  +- HOWTO.md ..... --> The instructions how to build software.
  |
  +- README.md .... --> This document file.
  |
  +- POSIX.md  .... --> The instructions to compile applications in Linux.
  |
  +- WIN32.md  .... --> The instructions to compile applications in Windows.
```

## Shortcuts

- The source codes of AREG SDK are located in the [&lt;areg-sdk&gt;/framework](../framework).
- The sources codes of examples are located in the [&lt;areg-sdk&gt;/examples](../examples).
- The CMake common and user specific configuration files are in [&lt;areg-sdk&gt;/conf/cmake](../conf/cmake).
- The Makefile common and user specific configuration files are in [&lt;areg-sdk&gt;/conf/make](../conf/make).
- The MS Visual Studio common and user specific property files are in [&lt;areg-sdk&gt;/conf/msvc](../conf/msvc).
- The [developer guide](./DEVELOP.md) gives technical details and help to develop distributed and service enabled application.
- The document to explain [HOWTO](./HOWTO.md) export projects and make builds.
- The **POSIX** related configuration, compilation and the list of used API are in [POSIX](./POSIX.md)
- The **Windows** related configuration, compilation and the list of used API are in [WIN32](./WIN32.md)
