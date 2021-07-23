# AREG IoT SDK to simplify multitasking programming

**_Bring your product to live service enabled_**

## Introduction

**AREG IoT SDK** (or **AREG SDK**) is an Object Remote Procedure Call (_ORPC_) based, lightweight _real-time asynchronous communication framework written in C++_ that simplifies multitasking and service enabled applications development for embedded and/or high-end machines. AREG SDK enables _mist computing_ at the edge of the network, where devices connect to fog not only as traditional clients, but as thin servers and provide services to execute device specific or a narrow range of tasks.

Ideally, AREG is used in multi-tasking projects or in the projects where connected Things need to communicate in real-time mode. 


## Inter-process communication

The IPC of AREG framework is based on Multicast Routing (MCR) network communication model that has combined features of client-server and publish-subscribe models. In MCR client-server nodes have logical connections and all messages pass via router. Every application in the network can provide multiple logical nodes where every node can be a mixture of logical servers and clients. This approach helps to form a mesh of services to trigger directly on connected Things.

The benefits of such an approach are connection abstraction, a higher degree of data privacy, network mesh, application scalability, fault tolerance, dynamic connection and message distribution, and optimized streaming.


## Development benefits

In the AREG framework service discovery and messaging are fully automated. In addition, the SDK has a code-generator that does the tedious job for developers and keeps codes clean. For the examples of generated codes, see services source code [examples](#examples). Software developers can benefit by using AREG framework:
* The automations and code generator help to focus on business logic.
* Transparency of service physical location as if developers would write code for a single process application.
* It automatically makes the application design modular.
* Easier to create a simulator by simulating data layer and placing it in another process.
* Featured logging scopes allow to filter special logs and priorities without recompiling the code.


## Challenges and solutions

Traditionally, IoT devices are connected clients of cloud or fog servers to stream data from sensors for further processing. Since data is generated and collected at devices, it makes sense to provide network accessible (public) services directly on things. Such a concept changes the role of connected devices and primarily solves the following challenges:
* It significantly _increases data privacy_, which is a very important factor for sensitive data. 
* It _decreases data streaming_, which is a fundamental condition to optimize network communication. 
* It renders devices _more autonomous and more intelligent_ by providing network services directly in the environment of data origin. 


## More than embedded

AREG is a framework, where embedded applications do not need to adapt additional IoT mist computing enabling layers to increase the visibility in the network. Current 2 types of services (Local and Remote) help to keep the design and architecture of the entire embedded software homogeneous, where Local services are used in multithreading and the Remote services are used in multi-processing communication.

The guiding principles of AREG framework are:
* The network of connected devices provides information, instead of simply data.
* The network delivers only information that has been requested, and when it has been requested.
* Devices create a dynamic system that works together using a subscriber-provider model.
* Device applications are service providers that are accessible in the network to execute device specific tasks.
* Devices adapt to network configuration, dynamically discover data providers and available services.

This concept of system architecture reduces the amount of streaming data in the network that has a direct impact on energy consumption, and the connected Things directly communicate with each other, remain autonomous and become an intelligent extension of the fog network.


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


## Compilation

AREG SDK provides Visual Studio and Eclipse IDE project files. Framework source paths are relative to **"areg-sdk/dev"** folder. _By default, there is no need to make additional changes in project settings to compile all_.

### Linux compilation

The compilation under Linux requires _Eclipse IDE for C/C++ Developers_.

1. Open _Eclipse_ and select _File_ ⇒ _Import_ to import existing projects located in _areg-sdk root_.
1. In the _Projects_ list box select all projects and click _Finish_ button.

### Windows compilation

The compilation under Windows requires _Visual Studio 2012_ or higher.

1. Open _areg-sdk.sln_ in Visual Studio IDE (should be VS2012 or higher version);
1. Select _Solution Cofigurations_ and _Solution Platforms_ in the toolbar, and compile the entire solution.

For example, to compile sources in _Visual Studio 2019_, in _Soution Configurations_ select _dbg_v142_ for _Debug_ or _rls_v142_ for _Release_ builds, and select either _Win32_ for 32-bit or _x64_ for 64-bit builds and compile solution. All projects are output in **product** subfolder located in _areg-sdk root_.


### Other IDE or command line

Currently AREG SDK does not contain project files of any other IDE. The support of _make_ or _cmake_ to run from the command line will be integrated soon.

## Preprocessor directives

1. To compile sources with _POSIX API_, specify **POSIX** preprocessor directive. For more details see [POSIX.md](./POSIX.md)
1. To compile sources with _Win32 API_, specify **WINDOWS** preprocessor directive. For more details see [WIN32.md](./WIN32.md)

**Other preprocessor directives:**
- _DEBUG_ or _NDEBUG_ to compile debug or release versions
- _ENABLE_TRACES_ to enable tracing / logging
- _EXP_AREG_LIB_ or _EXPORT_STATIC_SYMBOLS_ to compile framework as static library
- _EXP_AREG_DLL_ or _EXPORT_SHARED_SYMBOLS_ to compile framework as shared library
- _IMP_AREG_LIB_ or _IMPORT_STATIC_SYMBOLS_ to link with framework static library
- _IMP_AREG_DLL_ or _IMPORT_SHARED_SYMBOLS_ to link with framework shared library

Applications that use AREG framework, should specify _IMP_AREG_DLL_ / _IMP_AREG_LIB_ option.


## Log and IPC enabling

The AREG framework has default settings to enable logging and IPC. However, use _.init_ files to set the right settings. If after compilation the binary output folder does not contains _‘config’_ subfolder with _.init_ files, create create _’config’_ subfolder in output folder and:
* **For logs:** copy [log.init](./dev/areg/resources/log.init) file located in [./dev/areg/resources](./dev/areg/resources/).
* **For IPC :** copy [router.init](./dev/areg/resources/router.init) file located in [./dev/areg/resources](./dev/areg/resources/).

For options to change settings see the description in the _.init_ files.


## Examples

The AREG SDK contains various examples to demonstrate features of the framework. **To run IPC examples, make sure the _mcrouter_ process has started**.

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
 
For license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) or contact by email _info[at]aregtec.com_.
