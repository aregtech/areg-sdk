```
This file is part of AREG IoT SDK.
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
============================================

# Information

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
  +- POSIX.md  .... --> the instructions to compile applications in Linux.
  |
  +- WIN32.md  .... --> the instructions to compile applications in Windows.
```

## Locations

- The source codes of _AREG IoT SDK_ are located in the [_framework_](./../framework/) subfolder of _areg-sdk_ root.
- The sources of _AREG IoT SDK_ examples are located in the [_examples_](./../examples) subfolder of _areg-sdk_ root.
- The MS Visual Studio property and Makefile user settings are located in the [_conf_](./../conf) subfolder of _areg-sdk_ root.
- The **POSIX** related configuration, compilation and the list of used API are in [POSIX.md](./POSIX.md)
- The **Windows** related configuration, compilation and the list of used API are in [WIN32.md](./WIN32.md)
