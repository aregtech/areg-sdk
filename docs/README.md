```
This file is part of AREG IoT SDK
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
============================================

# Information

This file describes file system structure and the location of documents.

## The structure of AREG SDK
```
areg-sdk  ......... --> the root folder of AREG SDK
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
After compiling with Visual Studio, the root _areg-sdk_ will contain recoverable _product_ subfolder that contains build binaries.

## The structure of documents

```
docs
  |
  +- POSIX.md  .... --> the instruction to compile application in Linux.
  |
  +- WIN32.md  .... --> the instruction to compile application in Windows.
```

## Locations

- The source codes of _AREG IoT SDK_ are located in [_framework_](./../framework/) subfolder of _areg-sdk_ root.
- The sources of _AREG IoT SDK_ examples are located in [_examples_](./../examples) subfolder of _areg-sdk_ root.
- The **POSIX** related configuration, compilation and the list of used API are in [POSIX.md](./POSIX.md)
- The **Windows** related configuration, compilation and the list of used API are in [WIN32.md](./WIN32.md)
