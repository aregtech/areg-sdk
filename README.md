```
This file is part of AREG SDK.
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
========================================================================

**_Bring your product to live service enabled_**


# AREG IoT SDK to simplify multitasking programming

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

**AREG IoT SDK** (or **AREG SDK**) is a _framework written in C++_ that simplifies multitasking and service enabled applications development for embedded and/or high-end machines. 

Materially, AREG SDK enables _mist computing_ at the edge of the network, where devices connect to fog not only as traditional clients, but as ultra-small logical servers that provide services to execute either device specific or a narrow range of tasks. _Service enabled_ embedded applications make IoT devices an integral part of IoT fog. The key importance of such a model are capabilities of embedded devices to provide and discover services used by applications in the network.

The project _AREG_ has been started as a simple asynchronous multithreading module to communicate a machine in a digital twin project, and it has grown as a mist computing framework. Our agenda is to make AREG powerful, but lightweight and easy to use embedded and IoT edge device application development framework, which supports various operating systems and communication channels. We focus on technologies to help software developers to focus on application business logic.


### General

With the help of AREG framework developers can implement service enabled multitasking applications running on embedded and/or high-end machines with 32- or 64-bits operating systems such as Linux (_POSIX API_) or Windows (_Win32 API_). The inter-process communication (_IPC_) is based on a multicast routing communication model, which has a combined features of client-server and publish-subscribe network comunication models. It forms a mesh of automatically discoverable services distributed in any physical or virtual node. It can be used to develop a single embedded application (process) or complex multi-process application running on one or several machines. The architects may easily balance computing resources by distributing service components between multiple processes and this remains transparent for software developers as they would write software for a single process or even in a single thread. Ideally, it is used in multi-processing projects or in the projects where connected Things within a network need to communicate in reatl-time mode.

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
- _Code generator_ to generate servicing base objects to extend and implement the service logic.

The other tools and features, such as _remote logging service_, _design tools_, support of other _embedded RTOS_ (freeRTOS, for example) are in the development phase. Tools and features like featured log viewer, testing tool, multiple communication channels and protocols, further extension supported OS, etc., are in development pipeline.

## More than embedded

With the rapid growth of the Internet of Things (_IoT_) technology market, many corporates are faced with pulling together several technological solutions. Solutions that are not well chosen or thought through can create additional engineering, application development, and quality assurance difficulties in order to achieve goals. Technology change during any development phase is more difficult, more expensive, and sometimes impossible. Thus, there is a critical need to choosing the right technology, architecture, and application frameworks in line with business needs.

Billions of smart devices with embedded operating systems are connected in both small and large networks. This requires robust solutions in the areas of network communications, device management, interoperability, data streaming, and communication protocols. AREG SDK focuses not only on embedded development, but on mist computing paradigm and communication protocols to form the network of ultra-small servers and increase visibility of provided services. AREG is a framework, where embedded applications do not need to adapt additional IoT enabling layers to increase the visibility in the network. This keeps design and architecture of the entire embedded software homogeneous.


## Intelligent edge device

In traditional cloud-based application development, data is sent and stored in a centralized cloud data center where data are analyzed and decisions are made. Due to bandwidth constraints and privacy conserns, moving big data from IoT devices to the cloud might be ineffective or not a feasible solution. In addition, the cloud is not able to provide ultra-low latency or location-aware services to develop time-sensitive and location-aware applications. To solve the issues of network communication latency and high bandwidth, data privacy and dispersion, there is a need to move computing power closer to the edge of the network, where data originate. The industry proposes edge-centric computing paradigms to address these issues. Where previously devices mainly collected and sent data for further computation, today smart devices are able to perform complex computations on-side. This extends cloud computing power by bringing and distributing services closer to the edge of the network.

The mist computing paradigm is the distributing computing mechanism to the extreme edge where IoT devices are located (i.e. IoT devices themselves) in order to minimize the communication latency between IoT devices in milliseconds. It is the first computing location in the IoT-fog-cloud continuum. The proposal is to grant the IoT devices the capability of self-awareness in terms of self-organizing, self-managing and several self-* mechanisms so that IoT devices become fully autonomous and are able to operate continuously, even in the event of no available internet or an unstable connection. In mist computing, IoT devices may act not only as “thin clients”, but also as “thin servers” by enabling services accessed within the network where the data can be processed directly on the device itself.

The guiding principles of mist computing are:
* The network of connected devices provides information, instead of simply data.
* The network delivers only information that has been requested, and when it has been requested.
* Devices create a dynamic system that works together using a subscriber-provider model.
* Device applications are service providers that are accessible in the network to execute device specific or a narrow range of tasks.
* Devices adapt to network configuration and information and dynamically discover data providers and available services before executing an application.

This concept of system architecture sufficiently reduces the amount of streaming data in the network and the devices use up to five time sless energy for computing rather than for wireless communication. In mist computing, connected things can directly communicate with each other, bypassing any other network node, remaining autonomous and becoming an intelligent extension of the fog network.


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

To compile with Visual Studio, use _areg-sdk.sln_ file. To compile with Eclipse / GCC, include projects in Eclipse workspace. In framework source codes, all paths are relative to **"areg-sdk/dev"** folder. To compile AREG framework and the examples, download sources to the local folder and follow IDE specific instructions bellow.

### Compilation with Eclipse IDE and GCC compiler under Linux

The compilation with Eclipse requires _Eclipse IDE for C/C++ Developers_ and _GCC_ compiler version 98 or higher.

* Open _Eclipse IDE for C/C++ Developers_ application;
* Create a new workspace and specify the workfspace folder the _areg-sdk root_;
* In Eclipse IDE select _File_ ==> _Import_ menu;
* In _Select_ dialog choose _General_ ==> _Existing Projects into Workspace_ and click _Next_ button;
* In _Import Projects_ dialog choose _Select root directory_, click _Browse_ and select _areg-sdk root_ folder;
* After scanning, select at least **areg** or **mcrouter** in the _Projects_ list box, or select all projects;
* Click _Finish_ button.

By default, there is no need to make additional changes and the projects can be compiled. The _Debug_ and _Release_ builds output in appropriate Debug or Release subfolders of each project. If change settings, make sure _POSIX_ preprocessor directive is set. To run IPC examples, make sure the **mcrouter** process has started.

### Compilation with Visual Studio IDE

The sources can be opened at least in _Visual Studio 2012_ or higher version, and the sources can be compiled with the compiler of _Visual Studio 2010_ (MSVC v.100) or higher. To avoid any trouble, compile sources with the your current Visual Studio compiler. To run project in Visual Studio:

* Open _areg-sdk.sln_ with your preferred Visual Studio IDE (should be VS2012 or higher version);
* Select desired _Solution Cofigurations_ and _Solution Platforms_ in the toolbar of IDE, and compile solution.

For example, to compile sources with _Visual Studio 2019_ compiler, select _dbg_v142_ (for _Debug_ build) or _rls_v142_ (for _Release_ build) in _Soution Configurations_, and select either _Win32_ (for 32-bit build) or _x64_ (for 64-bit build) and compile solution. When test IPC examples, make sure **mcrouter** process (or service) is started. All projects are output in **product** subfolder located in _areg-sdk root_.

### Compilation with any other IDE or from command line

Currently AREG SDK does not contain project files of any other IDE. The support of _make_ or _cmake_ to run from command line will be integrate soon.

## Preprocessor directives

1. To compile sources with _POSIX API_, specify **POSIX** (or **_POSIX**) preprocessor directive. For more details see [POSIX.md](./POSIX.md)
1. To compile sources with _Win32 API_, specify **WINDOWS** (or **_WINDOWS**) preprocessor directive. For more details see [WIN32.md](./WIN32.md)

**Other preprocessor directives:**
- _DEBUG_ to compile debug version
- _NDEBUG_ to compile release version
- _ENABLE_TRACES_ to enable tracing / logging
- _EXP_AREG_LIB_ or _EXPORT_STATIC_SYMBOLS_ to compile framework sources as static library
- _EXP_AREG_DLL_ or _EXPORT_SHARED_SYMBOLS_ to compile framework sources as shared library
- _IMP_AREG_LIB_ or _IMPORT_STATIC_SYMBOLS_ to link with framework static library
- _IMP_AREG_DLL or _IMPORT_SHARED_SYMBOLS_ to link with framework shared library

Normally,
- **framework is compiled** with _POSIX_ / _WINDOWS_, _EXP_AREG_DLL_ / _EXP_AREG_LIB_ and _DEBUG_ / _NDEBUG_ options;
- **applications are compile** with _POSIX_ / _WINDOWS_, _IMP_AREG_DLL_ / _IMP_AREG_LIB_, and _DEBUG_ / _NDEBUG_, and _ENABLE_TRACES_ option to enable logging.

Under _Windows_, change preprocessor directives in _config_user.props_ file to apply changes to all projects. Under _Linux_, all changes should be manually done in each project.


## Log and IPC enabling

The AREG framework has default settings to enable logging and inter-process communication. However, use initialization files to set the right settings.

If after compilation the binary output folder does not contains _‘config’_ subfolder with _.init_ files:
**For logs:** create _’config’_ subfolder and copy [log.init](./dev/areg/resources/log.init) file located in [./dev/areg/resources](./dev/areg/resources/).
**For IPC :** create _’config’_ subfolder and copy [router.init](./dev/areg/resources/router.init) file located in [./dev/areg/resources](./dev/areg/resources/).

To change the default settings in _.init_, see the description in the files.


## System requirement

AREG supports Linux or Windows 32- and 64-bit operating systems. Linux applications use at least 300 kB of RAM and 2 MB of disk space. The minimum resource requirement may change depending on the operating system (Windows OS requires more resources). The recommended resources are 1MB of free RAM and 15MB of free flash / disk space.


## Examples

AREG SDK contains various examples to demonstrate features of framework. The examples are numerated and increase by comlication. The examples will be periodically updated. Currently, there are following examples available:

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
