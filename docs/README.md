# Information
```
This file is part of AREG SDK
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document describes the file system structure of **AREG SDK** and the location of documents.

## The structure of AREG SDK
```
areg-sdk  ......... --> the root folder of AREG SDK
    |
    +- conf ....... --> the MS Visual Studio and Makefile configuration folder
    |
    +- docs ....... --> the documents location of AREG SDK
    |
    +- framework .. --> the source codes of AREG framework and Multicasting Router
    |   |
    |   +- areg  .. --> the source codes of AREG framework
    |   |
    |   +- mcrouter --> the sources of Multicast Router (MCR)
    |   |
    |   +- system . --> the development platform specific common codes
    |
    +- examples  .. --> examples and tests of AREG SDK
    |
    +- tools  ..... --> code generator and other tools
    |
    +- LICENSE.txt  --> the license. Please read before use or distribute.
    |
    +- README.md  . --> the project description readme file.

```
After compiling using _MS Visual Studio_ or _Makefile_, the root folder _areg-sdk_ will contain a recoverable _product_ subfolder that contains build binaries.

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

- The source codes of AREG SDK are located in the [/<areg-sdk>/framework](./../framework/).
- The sources codes of examples are located in the [/<areg-sdk>/examples](./../examples).
- The Makefile general and user specific configuration files are in [/<areg-sdk>/conf/make](./../conf/make).
- The MS Visual Studio general and user specific property files are in [/<areg-sdk>/conf/msvc](./../conf/msvc).
- The [developer guide](./DEVELOP.md) gives technical details and helpt to develop distributed and service enabled application.
- The document to explain [HOWTO](./HOWTO.md) export projects and make builds.
- The **POSIX** related configuration, compilation and the list of used API are in [POSIX](./POSIX.md)
- The **Windows** related configuration, compilation and the list of used API are in [WIN32](./WIN32.md)
