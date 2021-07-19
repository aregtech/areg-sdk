```
This file is part of AREG SDK, the multi-tasking application development platform.
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
```

Current sources of **AREG IoT SDK** (or **AREG SDK**) support _POSIX_ and _Win32_ API.

**AREG SDK** is _an event-driven asynchronous connectivity and communication cross-platform framework_ written in C++ that helps to implement multitasking service enabled applications running on embedded and/or high-end machines with 32- or 64-bits multitasking operating systems such as Linux (_POSIX API_) or Windows (_Win32 API_). The inter-process communication (_IPC_) of current version of framework is based on a multicast routing communication model, which helps to create network of meshed services being distributes in any node.

Materially, **AREG SDK** enables mist computing at extreme edge of the network, where devices connect to fog not only as traditional clients, but as ultra-small servers that provide services to execute either device specific or a narrow range of tasks. IoT service-enabled embedded applications make IoT devices an integral part of fog. The key importance of such a model are capabilities of embedded devices to provide and discover services used by applications in  the  network.

Traditionally, IoT devices are connected clients of cloud or fog servers to stream data from sensors for further processing. Since data is generated and collected at devices, it makes sense to provide network accessible (public) services directly on things. Such a concept changes the role of connected devices and primarily solves the following challenges:
    * It significantly increases data privacy, which is a very important factor for sensitive data. 
    * It decreases data streaming, which is a fundamental condition for optimizing network communications that has a direct impact on energy consumption. 
    * It renders devices more autonomous and more intelligent by providing network services directly in the environment of data origin. 

 
The current version of **AREG SDK** consists of following components:  
    -- a connectivity and communication framework that contains the multitasking and distributed communication business logic;
    -- logger, which is integrated in the framework and can be switched ON or OFF;
    -- a multicast router to route messages between applications in real-time communication mode;
    -- code generator to generate base servicing objects to extend and implement the application logic.


## The file system structure:
```
areg-sdk            --> the root folder of AREG SDK
    |
    +- dev          --> the source codes of AREG Framework and Multicasting Router
    |   |
    |   +- areg     --> the source codes of AREG Framework
    |   |
    |   +- mcrouter --> the sources of Multicasting Router
    |   |
    |   +- system   --> the development platform specific common codes
    |
    +- examples     --> examples and tests of AREG SDK
    |
    +- tools        --> code generator and other tools
    |
    +- LICENSE.txt  --> the license. Please read before use or distribute.
    |
    +- POSIX.md    --> the instruction to compile application in Linux.
    |
    +- WIN32.md    --> the instruction to compile application in Windows.
```
The compiled codes output in "areg-sdk/product/build/<platform info>" folder

## Preprocessor directives:

    1. To compile sources with _POSIX API_, specify **POSIX** (or **_POSIX**) preprocessor directive.
    2. To compile sources with _Win32 API_, specify **WINDOWS** (or **_WINDOWS**) preprocessor directive.

**Other preprocessor directives:**
- **DEBUG** to compile debug version
- **NDEBUG** to compile release version
- **ENABLE_TRACES** to enable tracing / logging
- **EXP_AREG_LIB** or **EXPORT_STATIC_SYMBOLS** to compile sources as static library
- **EXP_AREG_DLL** or **EXPORT_SHARED_SYMBOLS** to compile sources as shared / dynamic linked library
- **IMP_AREG_LIB** or **IMPORT_STATIC_SYMBOLS** to link static library
- **IMP_AREG_DLL** or **IMPORT_SHARED_SYMBOLS** to link shared / dynamic linked library
- **OUTPUT_DEBUG_LEVEL=N**, to output debugging message on console, where _N_ is one of following numbers to enable debug outputs: 
  - _0 no outputs_
  - _1 output only errors_
  - _2 output errors and warnings_
  - _3 output errors, warnings and information_
  - _4 or higher output all_

In source codes, all paths are relative to **"areg-sdk/dev"** folder.

To compile with Visual Studio, use _areg-sdk.sln_
To compile with Eclipse / GCC, include all projects in _"areg-sdk/dev/xxx"_ folders and, by wish, _"areg-sdk/examples/xxx"_ folders.

**AREG SDK** is dual-licensed under commercial and free open source licenses. The commercial license gives the full rights to create and distribute software on own terms without any open source license obligations. For more details of licensing see **_LICENSE.txt_** file.
