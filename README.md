```
This file is part of AREG SDK, the multi-tasking application development platform.
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
==========================================================================

Current sources of **AREG IoT SDK** (or **AREG SDK**) support _POSIX_ and _Win32_ API.


## Table of content
1. [Introduction](#introduction)
2. [The file system structure](#the-file-system-structure)
3. [How to compile](#how-to-compile)
4. [Preprocessor directives](#preprocessor-directives)
5. [Examples](#examples)
6. [Licensing](#licensing)


## Introduction

**AREG SDK** is _an event-driven asynchronous connectivity and communication cross-platform framework_ written in C++ that helps to implement multitasking service enabled applications running on embedded and/or high-end machines with 32- or 64-bits multitasking operating systems such as Linux (_POSIX API_) or Windows (_Win32 API_). The inter-process communication (_IPC_) of current version of framework is based on a multicast routing communication model, which helps to create network of meshed services being distributes in any node.

Materially, **AREG SDK** enables mist computing at extreme edge of the network, where devices connect to fog not only as traditional clients, but as ultra-small servers that provide services to execute either device specific or a narrow range of tasks. IoT service-enabled embedded applications make IoT devices an integral part of fog. The key importance of such a model are capabilities of embedded devices to provide and discover services used by applications in  the  network.

Traditionally, IoT devices are connected clients of cloud or fog servers to stream data from sensors for further processing. Since data is generated and collected at devices, it makes sense to provide network accessible (public) services directly on things. Such a concept changes the role of connected devices and primarily solves the following challenges:
* It significantly increases data privacy, which is a very important factor for sensitive data. 
* It decreases data streaming, which is a fundamental condition for optimizing network communications that has a direct impact on energy consumption. 
* It renders devices more autonomous and more intelligent by providing network services directly in the environment of data origin. 

 
The current version of **AREG SDK** consists of following components:  
- a connectivity and communication framework that contains the multitasking and distributed communication business logic;
- logger, which is integrated in the framework and can be switched ON or OFF;
- a multicast router to route messages between applications in real-time communication mode;
- code generator to generate base servicing objects to extend and implement the application logic.


## The file system structure
```
areg-sdk  ......... --> the root folder of AREG SDK
    |
    +- dev  ....... --> the source codes of AREG Framework and Multicasting Router
    |   |
    |   +- areg  .. --> the source codes of AREG Framework
    |   |
    |   +- mcrouter --> the sources of Multicast Router
    |   |
    |   +- system . --> the development platform specific common codes
    |
    +- examples  .. --> examples and tests of AREG SDK
    |
    +- tools  ..... --> code generator and other tools
    |
    +- LICENSE.txt  --> the license. Please read before use or distribute.
    |
    +- POSIX.md  .. --> the instruction to compile application in Linux.
    |
    +- WIN32.md  .. --> the instruction to compile application in Windows.
```
After compiling with Visual Studio, the folder _areg-sdk/product_ is created. This folder can be deleted at any time. It is restored after compilation.


## How to compile

### Compilation with Eclipse IDE and GCC compiler under Linux

The compilation with Eclipse requires _Eclipse IDE for C/C++ Developers_ and _GCC_ compiler version 98 or higher.

* Download **AREG SDK** sources to the local folder;
* Open _Eclipse IDE for C/C++ Developers_ application;
* Create a new workspace and specify the workflow root folder the _are-sdk root_;
* In Eclipse IDE select _Import_ menu (either right clicking on _Project Explorer_ or select in _File_ menu) to open _Import_ wizard;
* In _Select_ dialog find _General_ and select option _Existing Projects into Workspace_;
* Click _Next_ button of dialog box, this opens _Import Projects_ dialog;
* In _Import Projects_ dialog select radio button _Select root directory_ and click _Browse_ button on the right side of radio;
* After scanning all folders of _areg-sdk root_, the _Projects_ list box will contain the list of all projects;
* Minimum select projects **areg** and **mcrouter**, or select all projects;
* Click _Finish_ button.

By default, there is no need to make additional changes. The _Debug_ build output in Debug subfolder and the _Release_ build in Release subfolder of the project. If changing settings of the project, make sure POSIX (or __POSIX_) preprocessor directive is set. To run IPC examples, make sure **mcrouter** process is started.

### Compilaton with Visual Studio IDE

The sources can be opened at least _Visual Studio 2012_ or higher version, and the sources can be compiled with compiler of _Visual Studio 2010_ (MSVC v.100) or higher. To avoid any troubles, compile sources with the your current compiler of Visual Studio. To run project in Visual Studio:

* Download AREG SDK soures to the local folder;
* Open your preferred Visual Sutio IDE (should be VS2012 or higher version);
* Open _areg-sdk.sln_ file located in _areg-sdk root_ folder, this loads all projects;
* Select appropriate _Solution Cofigurations_ and _Solution Platforms_, and compile solution

For example, to compile sources with _Visual Studio 2019_ compiler, select _dbg_v142_ (for _Debug_ build) or _rls_v142_ (for _Release_ build) in _Soution Configurations_, and select either _Win32_ (for 32-bit build) or _x64_ (for 64-bit build) and compile solution. When test IPC examples, make sure **mcrouter** process (or service) is started. All projects are output in **product** subfolder located in _areg-sdk root_.

### Compilation with any other IDE or from command line

Currently **AREG SDK** does not contain project files of any other IDE. 
At the moment, **AREG SDK** neither contains make files to run make, so that, the developers need manually to generate. This commes soon.


## Preprocessor directives

1. To compile sources with _POSIX API_, specify **POSIX** (or **_POSIX**) preprocessor directive. For more details see [POSIX.md](./POSIX.md)
1. To compile sources with _Win32 API_, specify **WINDOWS** (or **_WINDOWS**) preprocessor directive. For more details see [WIN32.md](./WIN32.md)

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

## Examples

**AREG SDK** contains various examples to demonstrate features of framework and cross-platform compilation. The examples are numerated based on complication. The examples will beperiodically updated, so that, it is recommended periodically check the examples. Currently, there are following examples available:

1.  [01_hello](./examples/01_hello/)         -- multithreading console application to output "Hello World" message.
2.  [02_buffer](./examples/02_buffer/)       -- multithreading console application to demonstrate work with shared buffer (stream).
3.  [03_file](./examples/03_file/)           -- console application to demonstrate work with test/binary files and masks in file paths.
4.  [04_trace](./examples/04_trace/)         -- multithreading console application to start and use logging.
5.  [05_timer](./examples/05_timer/)         -- multithreading console application to start Timer Service and use various timers.
6.  [06_threads](./examples/06_threads/)     -- multithreading console application to start and stop various threads.
7.  [07_synch](./examples/07_synch/)         -- multithreading console application to use various combined synchronizaton objects.
8.  [08_service](./examples/08_service/)     -- multithreading console application to start empty servicing component.
9.  [09_duplicate](./examples/09_duplicate/) -- multithreading console application to duplicate instances of same servicing component.
10. [10_svchello](./examples/10_svchello/)   -- multithreading console application to demonstrate _Local_ servicing component.
11. [11_svcmix](./examples/11_svcmix/)       -- multithreading console application to demonstrate multiple instances of same _Local_ service in the process.
12. [12_ipchello](./examples/12_ipchello/)   -- multiprocessing (IPC) application to demonstrate _Remote_ servicing component and client.
13. [13_ipcmix](./examples/13_ipcmix/)       -- multiprocessing (IPC) application to demonstrate service meshing and mixture of _Local_ and _Remote_ services.
14. [messager](./examples/messager/)         -- Windows GUI application to demonstrate dynamic modeling, fault tollerant environment and mist network.

For more details of every project, read _ReadMe.txt_ file of each example project.


## Licensing
 
**AREG SDK** is dual-licensed under commercial and free open source licenses. The commercial license gives the full rights to create and distribute software on own terms without any open source license obligations. For more details of licensing see [LICENSE.txt](./LICENSE.txt) file.
 
For license, support or any additional information, please visit [Aregtech](https://www.aregtech.com/) to contact us.
