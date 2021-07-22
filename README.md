```
This file is part of AREG SDK.
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
========================================================================

**_Bring your product to live service enabled_**


# AREG IoT SDK to simplify multitasking programming

**AREG IoT SDK** (or **AREG SDK**) consists of a _framework_ and _tools_ that simplify multitasking and service enabled applications development for embedded and/or high-end machines. 

AREG SDK can be used to develop a single embedded application (process), as well as to develop complex multi-process embedded or desktop applications running on one or several machines. AREG SDK is event-driven, asynchronous, and fault tolerant. It combines features of client-server and publish-subscribe communication model. It automatically provides modularity, forms a mesh of distributed services and keeps the service discovery procedure transparent. So that, in the complex multi-processing projects the architects may easily distribute service components in various processes running on one or different machines, and this remains transparent for software developers as they would write software for a single process or even a single thread.

The project _AREG framework_ has been started as a simple asynchronous multithreading framework to control a machine in a digital twin project, and it has grown as a distributed-computing framework with tools. Our agenda is to make AREG SDK powerful and flexible, but an easy and simple to use in embedded and IoT edge device application development by being lightweight framework that supports various operating systems and communication channels. We focus on technology to reduce software development time and help software developers to focus on application business logic.


## Table of content
1. [Introduction](#introduction)
2. [More than embedded](#more-than-embedded)
3. [Intelligent edge device](#intelligent-edge-device)
4. [The file system structure](#the-file-system-structure)
5. [How to compile](#how-to-compile)
6. [Preprocessor directives](#preprocessor-directives)
7. [Log and IPC enabling](#log-and-ipc-enabling)
8. [System requirement](#system-requirement)
9. [Examples](#examples)
10. [Licensing](#licensing)


## Introduction

### General

AREG SDK is _an event-driven asynchronous communication framework_ written in C++ that helps to implement service enabled and multitasking applications running on embedded and/or high-end machines with 32- or 64-bits operating systems such as Linux (_POSIX API_) or Windows (_Win32 API_). The inter-process communication (_IPC_) of the current version is based on a multicast routing communication model, which helps to form a network of meshed services distributed in any physical or virtual node.

Materially, AREG SDK enables _mist computing_ at the extreme edge of the network, where devices connect to fog not only as traditional clients, but as ultra-small logical servers that provide services to execute either device specific or a narrow range of tasks. _Service enabled_ embedded applications make IoT devices an integral part of IoT fog. The key importance of such a model are capabilities of embedded devices to provide and discover services used by applications in the network.

### Challenges and solutions

Traditionally, IoT devices are connected clients of cloud or fog servers to stream data from sensors for further processing. Since data is generated and collected at devices, it makes sense to provide network accessible (public) services directly on things. Such a concept changes the role of connected devices and primarily solves the following challenges:
* It significantly _increases data privacy_, which is a very important factor for sensitive data. 
* It _decreases data streaming_, which is a fundamental condition for optimizing network communications that has a direct impact on energy consumption. 
* It renders devices _more autonomous and more intelligent_ by providing network services directly in the environment of data origin. 
 
### Framework and tools

The current version of AREG SDK consists of following components:  
- A _connectivity and communication framework_ that contains the multitasking and distributed communication business logic;
- _Logger / Tracer_, which is integrated in the framework and can be switched ON or OFF;
- A _multicast router_ to route messages between applications in real-time communication mode;
- _Code generator_ to generate base servicing objects to extend and implement the application logic.

The other tools and features, such as _remote logging service_, _design tools_, support of other _embedded RTOS_ (freeRTOS, for example) are in the development phase. The other tools and features such as log viewer, testing tool, multiple communication channels and protocols, further extension supported OS, etc., are in development pipeline.

## More than embedded

With the rapid growth of the **Internet of Things** (_IoT_) technology market, many corporates are now faced with pulling together several technological solutions. However, potentially solutions that are not well chosen or thought through can create additional engineering, application development, and quality assurance difficulties in order to achieve their IoT goals. Technology change during any development phase is proving to be more difficult, more expensive, and sometimes impossible. Thus, there is a critical need to choosing the right technology, architecture, and application frameworks in line with business needs.

Billions of commercial smart devices with embedded operating systems are connected in both small and large networks. This will require robust solutions in the areas of network communications, device management, interoperability, data streaming, and communication protocols.

AREG SDK focuses not only on embedded software development, but it focuses on improvement of the communication channels and communication protocols. It is an embedded and IoT application development tool, where applications do not need to integrate additional IoT enabling software layers in order to increase the visibility of the device in the network. The current version of AREG framework provides mist-computing where it forms the network of ultra-small servers that provide services. Without any additional layer or adapter, the asynchronous and event-driven framework helps developers easily develop embedded and IoT mesh-computing applications, keeping the same architecture and the same logic for the entire application.


## Intelligent edge device

In traditional cloud-based application development, data is required to be sent and stored in a centralized cloud data center where all data are analyzed and decisions are made. Due to bandwidth constraints, moving big data from IoT devices to the cloud might be ineffective in some cases. Moreover, sending data to the cloud is not a feasible solution in some applications due to privacy concerns. In addition, the cloud is not able to provide ultra-low latency or location-aware services to develop time-sensitive and location-aware applications. In summary, the cloud is simply not enough to provide self-sufficient solutions.

To solve the issues of network communication latency and high bandwidth, data privacy and dispersion, there is a need to move computing power closer to the edge of the network, where data originate. The industry proposes edge-centric computing paradigms to address these issues, and to bridge the gap between cloud and connected devices by enabling and distributing computing, storage, device and data management applications on the network nodes closer to IoT devices. Where previously devices mainly collected and sent data for further computation, today smart devices are able to perform complex computations on-side. This extends cloud computing power by bringing and distributing services closer to the edge of the network.

The mist computing paradigm is the distributing computing mechanism to the extreme edge where IoT devices are located (i.e. IoT devices themselves) in order to minimize the communication latency between IoT devices in milliseconds. It is the first computing location in the IoT-fog-cloud continuum. The proposal is to grant the IoT devices the capability of self-awareness in terms of self-organizing, self-managing and several self-* mechanisms so that IoT devices become fully autonomous and are able to operate continuously, even in the event of no available internet or an unstable connection. In mist computing, IoT devices may act not only as “thin clients”, but also as “thin servers” by enabling services accessed within the network where the data can be processed directly on the device itself.

The guiding principles of mist computing are:
* The network of connected devices provides information, instead of simply data.
* The network delivers only information that has been requested, and when it has been requested.
* Devices create a dynamic system that works together using a subscriber-provider model.
* Device applications are service providers that are accessible in the network to execute device specific or a narrow range of tasks.
* Devices adapt to network configuration and information and dynamically discover data providers and available services before executing an application.

This concept of system architecture sufficiently reduces the amount of streaming data in the network. The devices use up to five times more energy for wireless communication rather than for computing. By processing data directly on an IoT device, mist computing helps to reduce power consumption. In mist computing, connected things can directly communicate with each other, bypassing any other network node, remaining autonomous and becoming an extension of the fog network. The latency in mist computing is ultra-low.


## The file system structure

Bellow is a file system structure of AREG SDK

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
    +- POSIX.md  .. --> the instruction to compile applications in Linux.
    |
    +- WIN32.md  .. --> the instruction to compile applications in Windows.
```


## How to compile

To compile with Visual Studio, use _areg-sdk.sln_

To compile with Eclipse / GCC, include all projects in _"areg-sdk/dev/xxx"_ folders and, by wish, _"areg-sdk/examples/xxx"_ folders.

In source codes, all paths are relative to **"areg-sdk/dev"** folder. In examples, in addition, the paths are relevant either to the project root or to _'src'_ source folder.

### Compilation with Eclipse IDE and GCC compiler under Linux

The compilation with Eclipse requires _Eclipse IDE for C/C++ Developers_ and _GCC_ compiler version 98 or higher.

* Download AREG SDK sources to the local folder;
* Open _Eclipse IDE for C/C++ Developers_ application;
* Create a new workspace and specify the workfspace root folder the _areg-sdk root_;
* In Eclipse IDE select _Import_ menu (either right clicking on _Project Explorer_ or select in _File_ menu) to open _Import_ wizard;
* In _Select_ dialog find _General_ and select option _Existing Projects into Workspace_;
* Click _Next_ button of dialog box, this opens _Import Projects_ dialog;
* In _Import Projects_ dialog select radio button _Select root directory_ and click _Browse_ button on the right side of radio;
* After scanning all folders of _areg-sdk root_, the _Projects_ list box will contain the list of all projects;
* Minimum select projects **areg** and **mcrouter**, or select all projects;
* Click _Finish_ button.

By default, there is no need to make additional changes. The _Debug_ build output in Debug subfolder and the _Release_ build in Release subfolder of the project. If changing settings of the project, make sure POSIX (or __POSIX_) preprocessor directive is set. To run IPC examples, make sure the **mcrouter** process has started.

### Compilation with Visual Studio IDE

The sources can be opened at least in _Visual Studio 2012_ or higher version, and the sources can be compiled with the compiler of _Visual Studio 2010_ (MSVC v.100) or higher. To avoid any troubles, compile sources with the your current Visual Studio compiler. To run project in Visual Studio:

* Download AREG SDK sources to the local folder;
* Open your preferred Visual Studio IDE (should be VS2012 or higher version);
* Open _areg-sdk.sln_ file located in _areg-sdk root_ folder, this loads all projects;
* Select appropriate _Solution Cofigurations_ and _Solution Platforms_ in the toolbar of IDE, and compile solution.

For example, to compile sources with _Visual Studio 2019_ compiler, select _dbg_v142_ (for _Debug_ build) or _rls_v142_ (for _Release_ build) in _Soution Configurations_, and select either _Win32_ (for 32-bit build) or _x64_ (for 64-bit build) and compile solution. When test IPC examples, make sure **mcrouter** process (or service) is started. All projects are output in **product** subfolder located in _areg-sdk root_.

### Compilation with any other IDE or from command line

Currently AREG SDK does not contain project files of any other IDE. 
At the moment, AREG SDK does not contain make files to run **make**, so that, the developers need manually to generate. This commes soon.


## Preprocessor directives

1. To compile sources with _POSIX API_, specify **POSIX** (or **_POSIX**) preprocessor directive. For more details see [POSIX.md](./POSIX.md)
1. To compile sources with _Win32 API_, specify **WINDOWS** (or **_WINDOWS**) preprocessor directive. For more details see [WIN32.md](./WIN32.md)

**Other preprocessor directives:**
- **DEBUG** to compile debug version
- **NDEBUG** to compile release version
- **ENABLE_TRACES** to enable tracing / logging
- **EXP_AREG_LIB** or **EXPORT_STATIC_SYMBOLS** to compile framework sources as static library
- **EXP_AREG_DLL** or **EXPORT_SHARED_SYMBOLS** to compile framework sources as shared library
- **IMP_AREG_LIB** or **IMPORT_STATIC_SYMBOLS** to link with framework static library
- **IMP_AREG_DLL** or **IMPORT_SHARED_SYMBOLS** to link with framework shared library
- **OUTPUT_DEBUG_LEVEL=N**, to output debugging message on console, where _N_ is one of following debug outputs level: 
  - _0 no outputs_
  - _1 output only errors_
  - _2 output errors and warnings_
  - _3 output errors, warnings and information_
  - _4 or higher output all_

_OUTPUT_DEBUG_LEVEL_ is enabled only if _DEBUG_ is defined. Otherwise, it is ignored. Use this preprocessor directive if really need to output message on console. 

Normally
- framework is compiled with _POSIX_ / _WINDOWS_, _EXP_AREG_DLL_ / _EXP_AREG_LIB_ and _DEBUG_ / _NDEBUG_ options;
- applications are compile with _POSIX_ / _WINDOWS_, _IMP_AREG_DLL_ / _IMP_AREG_LIB_, and _DEBUG_ / _NDEBUG_, and _ENABLE_TRACES_ option to enable logging.

Under _Windows_, change preprocessor directives in _config_user.props_ file to apply changes to all projects. Under _Linux_, all changes should be manually done in each project.


## Log and IPC enabling

When sources are compiled, the application can be started. The AREG framework has default settings to enable logging and inter-process communication. However, use appropriate initialization files to set the right settings.

If after compilation the binary output folder does not contains _‘config’_ subfolder with _.init_ files:
For logs: create _’config’_ subfolder and copy [log.init](./dev/areg/resources/log.init) file located in [./dev/areg/resources](./dev/areg/resources/).
For IPC: create _’config’_ subfolder and copy [router.init](./dev/areg/resources/router.init) file located in [./dev/areg/resources](./dev/areg/resources/).

For further instructions of using or changing _.init_, see the instructions in the files. The syntax of initialization files are simple, and the parameters are described.


## System requirement

AREG SDK supports _POSIX_ API for Linux machines and _Win32_ API for Windows machines, runs on both 32- and 64-bit operating systems. Linux based applications using statically linked AREG framework require at least 300 kB of free RAM and 2 MB of free disk space. The minimum resource requirement may change depending on the operating system (Windows OS requires more resources). The recommended resources are 1MB of free RAM and 15MB of free flash / disk space.


## Examples

AREG SDK contains various examples to demonstrate features of framework and cross-platform compilation. The examples are numerated based on complication. The examples will be periodically updated. We recommend periodically to check the examples. Currently, there are following examples available:

1.  [01_hello](./examples/01_hello/)         -- multithreading console application to output "Hello World" message.
2.  [02_buffer](./examples/02_buffer/)       -- multithreading console application to demonstrate work with shared buffer (stream).
3.  [03_file](./examples/03_file/)           -- console application to demonstrate work with test/binary files and masks in file paths.
4.  [04_trace](./examples/04_trace/)         -- multithreading console application to start and use logging.
5.  [05_timer](./examples/05_timer/)         -- multithreading console application to start Timer Service and use various timers.
6.  [06_threads](./examples/06_threads/)     -- multithreading console application to start and stop various threads.
7.  [07_synch](./examples/07_synch/)         -- multithreading console application to use various combined synchronization objects.
8.  [08_service](./examples/08_service/)     -- multithreading console application to start empty servicing component.
9.  [09_duplicate](./examples/09_duplicate/) -- multithreading console application to duplicate instances of the same servicing component.
10. [10_svchello](./examples/10_svchello/)   -- multithreading console application to demonstrate _Local_ servicing component.
11. [11_svcmix](./examples/11_svcmix/)       -- multithreading console application to demonstrate multi-instances of same _Local_ service.
12. [12_ipchello](./examples/12_ipchello/)   -- multiprocessing (**IPC**) application to demonstrate _Remote_ servicing component and client.
13. [13_ipcmix](./examples/13_ipcmix/)       -- multiprocessing (**IPC**) application to demonstrate meshing and mixture of _Local_ and _Remote_ services.
14. [messager](./examples/messager/)         -- Windows GUI application to demonstrate dynamic modeling, fault tolerant environment and mist network.

For more details of every project, read _ReadMe.txt_ file of each example project.


## Licensing
 
AREG SDK is _dual-licensed under commercial and free open source licenses_. The commercial license gives the commercial support, full rights to create and distribute software on own terms without any open source license obligations. For more details of licensing see [LICENSE.txt](./LICENSE.txt) file.
 
For license, support or any additional information, please visit [Aregtech](https://www.aregtech.com/) for contacts, or contact us by e-mail specified at the top of this file.
