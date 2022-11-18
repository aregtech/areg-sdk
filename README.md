<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:100%;height:100%"/></a>
  <br /><br /><strong>AREG SDK</strong>
</h1>

![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=%20%F0%9F%93%A3%20Latest%20release&style=flat&logoColor=b0c0c0&labelColor=363D44)

The latest development sources can be cloned from [20220701-candidate](https://github.com/aregtech/areg-sdk/tree/20220701-candidate) branch. Please check the branch [build status](https://github.com/aregtech/areg-sdk/actions) before using it.

---

<!-- markdownlint-disable -->
## Project Status[![](./docs/img/pin.svg)](#project-status)
<table class="no-border">
  <tr>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/c-cpp.yml/badge.svg" alt="C++ compiltation"/></td>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml/badge.svg" alt="MS Build"/></td>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml/badge.svg" atl="CodeQL"/></td>
  </tr>
  <tr>
    <td><img src="https://img.shields.io/badge/Solution-C++17-blue.svg?style=flat&logo=c%2B%2B&logoColor=b0c0c0&labelColor=363D44" alt="C++ solution"/></td>
    <td><img src="https://img.shields.io/badge/OS-linux%20%7C%20windows-blue??style=flat&logo=Linux&logoColor=b0c0c0&labelColor=363D44" alt="Operating systems"/></td>
    <td><img src="https://img.shields.io/badge/CPU-x86%20%7C%20x86__64%20%7C%20arm%20%7C%20aarch64-blue?style=flat&logo=amd&logoColor=b0c0c0&labelColor=363D44" alt="CPU Architect"/></td>
  </tr>
</table>

---

## Introduction[![](./docs/img/pin.svg)](#introduction)

**AREG SDK** is an interface-centric real-time asynchronous communication engine to enable distributed- and [mist-](https://csrc.nist.gov/publications/detail/sp/500-325/final)computing, where connected Things interact and provide services as if they act like thin distributed servers. The communication engine of AREG SDK is based on Object Remote Procedure Call (**ORPC**) protocol.

---

## Table of contents[![](./docs/img/pin.svg)](#table-of-contents)
1. [Motivation](#motivation)
2. [More than embedded](#more-than-embedded)
3. [Composition](#composition)
4. [Roadmap](#roadmap)
5. [Software build](#software-build)
   - [Build with `cmake`](#build-with-cmake)
   - [Build with `make`](#build-with-make)
   - [Build with IDE](#build-with-ide)
6. [Software integration](#software-integration)
   - [Multicast router](#multicast-router)
   - [Logging service](#logging-service)
   - [Development](#development)
7. [Use cases and benefits](#use-cases-and-benefits)
   - [Distributes solutions](#distributed-solution)
   - [Driverless devices](#driverless-devices)
   - [Real-time solutions](#real-time-solutions)
   - [Digital twin](#digital-twin)
   - [Simulation and test automations](#simulation-and-test-automations)
8. [Examples](#examples)
9. [Licensing](#licensing)
10. [Call for action](#call-for-action)

---

## Motivation[![](./docs/img/pin.svg)](#motivation)

Traditionally, devices act as connected clients to stream data to the cloud or fog servers for further processing.
<br /><br /><a href="/docs/img/mist-network.png"><img src="/docs/img/mist-network.png" alt="IoT-to-Cloud (Nebula) network" style="width:70%;height:70%"/></a><br /><br />
Since data is generated and collected at the edge of the network (**mist network**), it makes sense to change the role of connected Things and provide network-accessible (_Public_) services directly on devices. This extends _Cloud_ to the extreme edge and it is a good foothold for robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* Develop _autonomous, intelligent and self-aware devices_ by providing services directly in the environment of data origin.

[ [↑ to top ↑](#table-of-contents) ]

---

## More than embedded[![](./docs/img/pin.svg)](#more-than-embedded)

When we were designing AREG SDK, the guiding principle was to provide a homogeneous solution for Multithreading, Multiprocessing and Internet communication wrapped in services appropriately having _Local_, _Public_ and _Internet_ categories. These services are neither processes nor tasks managed by the operating system, they are software components with a predefined interface, in which methods are invoked remotely.
<br /><a href="docs/img/areg-services.png"><img src="docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:70%;height:70%"/></a><br />
> 💡 In current version, the AREG communication engine supports _Local_ (multithreading) and _Public_ (multiprocessing) service categories. The _Internet_ (web) service category is in development pipeline.

The AREG engine forms a fault-tolerant system and does not require process startup priority. It automatically discovers services, automates communications, enables distributed service programming, and helps developers to focus on application business logic as if they would program a single process application with one thread where methods of objects are event-driven. The engine guarantees that:
* The crash of one application does not cause the crash of the system.
* The service clients automatically receive service availability status notifications.
* The requests, response and notifications are automatically invoked and run in thread-safe mode.

[ [↑ to top ↑](#table-of-contents) ]

---

## Composition[![](./docs/img/pin.svg)](#composition)

Currently AREG SDK consists of:
1. [Multicast router (_mcrouter_)](./framework/mcrouter/) to use for IPC. It runs either as a service managed by the OS or as a console application.
2. [AREG framework (or engine)](./framework/areg/) is a (shared or static) library to link with every application.
3. [Code generator tool](./tools/) to create client and server base objects from a service prototype document.

The network communication model of AREG framework is based on _multicast router_ and service architecture is _interface-centric_, which in combination of both enable to comprise all features of _action-centric_ architecture (Client-Server / Request-Reply model) and _data-centric_ architecture (Publisher-Subscriber / PubSub model).

> 💡 The [examples](./examples) of _AREG SDK_ contain multiple projects to check the features and fault tolerant behavior of AREG communication engine.

[ [↑ to top ↑](#table-of-contents) ]

---

## Roadmap[![](./docs/img/pin.svg)](#roadmap)
 The aim of AREG SDK is a lightweight self-sufficient system, which consist of an intelligent framework and multiple tools that help developers to create complex **Desktop**, **Embedded** and/or **IoT edge** applications in shorter time (_reduce development and testing time 50-30%_). 
- **Planned framework features:**
  * Multi-channel and multi-protocol communication.
  * Logging service (separate process) to collect logs in the network.
  * _Internet_ (web) category service.
- **Planned tools:**
  * Service interface designer.
  * Interactive log viewer.
  * Service testing and Data simulation tool.

[ [↑ to top ↑](#table-of-contents) ]

---

## Software build[![](./docs/img/pin.svg)](#software-build)

The source codes of AREG framework and examples support following platform, CPU and compilers:
<table>
  <tr>
    <td nowrap><strong>OS</strong></td>
    <td>Linux (list of <a href="./docs/POSIX.md#posix-api" alt="POSX API">POSIX API</a>), Windows 8 and higher.</td>
  </tr>
  <tr>
    <td nowrap><strong>CPU</strong></td>
    <td>x86, x86_64, arm and aarch64.</td>
  </tr>
  <tr>
    <td nowrap><strong>Compilers (C++17 or heigher)</strong></td>
    <td>GCC, g++, clang and MSVC.</td>
  </tr>
</table>

The tools to use to compile sources:
| Solution | Platforms | API | Quick actions to compile |
| --- | --- | --- | --- |
| `CMakeLists.txt` | **Linux, Windows** | _POSIX_, _Win32_ | Make one of these actions:<br /> &nbsp;&nbsp; - Call `cmake` in _command line_.<br /> &nbsp;&nbsp; - Configure and build in _Visual Studio Code_;<br /> &nbsp;&nbsp; - Configure and build in _Microsoft Visual Studio_ (VS2019 or higher). |
| `Makefile` | **Linux**| _POSIX_ | Call `make` in _command line_. |
| `areg-sdk.sln` | **Windows** | _Win32_ | Open and build in _Microsoft Visual Studio_ (VS2019 or higher). |
| `.project` | **Linux, Windows** | _POSIX_ | Import and build projects in _Eclipse_. |

> 💡 The other POSIX-compliant OS and compilers are not tested yet.<br />
> 💡 Make user specific changes (like switch compiler or output folder) only in appropriate `user` files:<br />
> - For `cmake`, make changes in [conf/cmake/user.cmake](./conf/cmake/user.cmake) file.<br />
> - For `make`, make changes in [conf/make/user.mk](./conf/make/user.mk) file.

After compilation, normally binaries are located in `<areg-sdk>/product/build/<compiler-platform-path>/bin` folder. Details on how to change compiler, load and compile sources for various targets are described in [HOWTO](./docs/HOWTO.md) document. The next are quick overviews.

#### Build with `cmake`

To build with [cmake](https://cmake.org/), open _Terminal_ in your `projects` folder and take the steps:
```bash
# Step 1: Get sources from GitHub
$ git clone https://github.com/aregtech/areg-sdk.git
$ cd areg-sdk

# Step 2: Create a subdirectory for CMake cache files.
$ mkdir build && cd build

# Step 3: Initialize cache and build configuration, enable examples.
$ cmake .. -DBUILD_EXAMPLES=ON

# Step 4: Compile sources by calling: cmake --build . [-j [concurrent jobs]]
$ cmake --build . -j 8
 ```

#### Build with `make`

To build with [make](https://www.gnu.org/software/make/), open _Terminal_ in your `projects` folder and take the steps:
```bash
# Step 1: Get sources from GitHub
$ git clone https://github.com/aregtech/areg-sdk.git
$ cd areg-sdk

# Step 2: Compile sources by calling: make [all] [framework] [examples]
$ make
```

#### Build with IDE

Open _Terminal_ or preferred IDE to clone source codes like this:
```bash
$ git clone https://github.com/aregtech/areg-sdk.git
$ cd areg-sdk
```
Depending on IDE or preferences make one of following:

1. Open `areg-sdk.sln` file in [MSVS](https://visualstudio.microsoft.com/) (2019 or higher) and compile solution.
2. Open `areg-sdk` folder in [MSVC](https://visualstudio.microsoft.com/) (2019 or higher), select `CMakeLists.txt` located in the `areg-sdk` root, configure and build.
3. Open `areg-sdk` folder in [VS Code](https://code.visualstudio.com/), select `CMakeLists.txt` located in the `areg-sdk` root, configure and build.
4. Open [Eclipse](https://www.eclipse.org/ide/), import all projects from `areg-sdk` root, select projects and build.

> 💡 Compilation with _Eclipse_ under **Windows** might require to change the Toolchain.

[ [↑ to top ↑](#table-of-contents) ]

---

## Software integration[![](./docs/img/pin.svg)](#software-integration)

#### Multicast router

Configure [_router.init_](./framework/areg/resources/router.init) file to set the IP-address and the port of _multicast router_:
```
connection.address.tcpip    = 127.0.0.1	# the address of mcrouter host
connection.port.tcpip       = 8181      # the connection port of mcrouter
```
The multicast router forms the network and can run on any device. For example, in case of M2M communication, it can run on a gateway, in case of IPC, it can run on the same machine. In case of multithreading application development, there is no need to configure `router.init` and run _mcrouter_.

#### Logging service

Configure [_log.init_](./framework/areg/resources/log.init) to set scopes, log priorities and log file name:
```
log.file        = %home%/logs/%appname%_%time%.log # create logs in 'log' subfolder of user home 
scope.mcrouter.*= NOTSET ;                         # disable logs for mcrouter.

scope.my_app.*                   = DEBUG | SCOPE ; # enable all logs of my_app
scope.my_app.ignore_this_scope   = NOTSET ;        # disable logs of certain scopes in my_app
scope.my_app.ignore_this_group_* = NOTSET ;        # disable logs of certain scope group in my_app
```
> 💡 By default, the `router.init` and `log.init` files are located in the `config` subfolder of binaries.<br />
> 💡 To enable all logs of all applications, use `scope.*  = DEBUG | SCOPE ;` .<br />
> 💡 In the current version the logging is possible only in file.

### Development

The development guidance and step-by-step example to create a simple service-enabled application are described in [DEVELOP](./docs/DEVELOP.md).

[ [↑ to top ↑](#table-of-contents) ]

---

## Use cases and benefits[![](./docs/img/pin.svg)](#use-cases-and-benefits)

AREG SDK can be used in a very large scope of multithreading and multiprocessing application development running on Linux or Windows machines.

#### Distributed solution

AREG SDK is a distributed computing solution, where the services can be distributed and run on any node of the network. The automatic service discovery makes service location transparent, so that the applications interact as if the components are located in one process. Developers define a _model_, which is a description of service relationship, and load it to start services during runtime. The services can easily be distributed between multiple processes.

The following is a demonstration of a static _model_ description, which is loaded to start services and unloaded to stop them.
```cpp
// main.cpp source file.

// Defines static model with 2 services
BEGIN_MODEL(NECommon::ModelName)

    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( "RemoteRegistry", RemoteRegistryService )
            REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
        END_REGISTER_COMPONENT( "RemoteRegistry" )
    END_REGISTER_THREAD( "Thread1" )

    BEGIN_REGISTER_THREAD( "Thread2" )
        BEGIN_REGISTER_COMPONENT( "SystemShutdown", SystemShutdownService )
            REGISTER_IMPLEMENT_SERVICE( NESystemShutdown::ServiceName, NESystemShutdown::InterfaceVersion )
        END_REGISTER_COMPONENT( "SystemShutdown" )
    END_REGISTER_THREAD( "Thread2" )

END_MODEL(NECommon::ModelName)

// the main function
int main()
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    // load model to start service components
    Application::loadModel(NECommon::ModelName);

    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);

    // stop and unload service components
    Application::unloadModel(NECommon::ModelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

    return 0;
}
```
In the example, the `"RemoveRegistry"` and the `"SystemShudown"` are the names of components called _roles_, and the `NERemoteRegistry::ServiceName` and the `NESystemShutdown::ServiceName` are the _interface names_. In combination, they define the _service name_ used to access in the network. These MACRO create static _model_ `NECommon::ModelName`, which is loaded when call `Application::loadModel(NECommon::ModelName)`, and the services are stopped when call `Application::unloadModel(NECommon::ModelName)`.

In this example, services can be merged in one thread or distributed in 2 processes by defining a _model_ in each process. Independent on service location, neither software developers, nor service client objects feel a difference except for possible slight network latency when running IPC. The services must have unique names within the scope of visibility. Means, in case of _Public_ services, the names are unique within a network, and in case of _Local_ services, the names are unique within a process scope. An example of developing a service and a client in one and multiple processes is in [**Hello Service!**](./docs/DEVELOP.md#hello-service) project described in the development guide.

#### Driverless devices

Normally, the devices are supplied with the drivers to install in the system and with the header files to integrate in the application(s). The drivers often run in Kernel mode and the crash of the driver crashes the entire system. Driver development requires a special technique, which is different for each operating system, and it is hard to debug. 
<br /><a href="/docs/img/driver-solution.png"><img src="/docs/img/driver-solution.png" alt="Kkernel-mode driver solution" style="width:70%;height:70%"/></a><br />
Our proposal is to deliver driverless service-enabled devices, where device-specific services are described in the interface prototype documents. 
<br /><a href="/docs/img/driverless-solution.png"><img src="/docs/img/driverless-solution.png" alt="AREG SDK driverless solution" style="width:70%;height:70%"/></a><br />
In contrast to drivers, the service development does not differ from user mode application development, it is faster to develop, easily serves multiple applications (service clients), contains fewer risks and requires less development resources. The client object generated from the supplied service interface prototype document is easily integrated into the application to communicate and trigger device-specific service(s).

#### Real-time solutions

When a remote method of the service interface is called, the engine of AREG SDK immediately generates and delivers messages to the target component, which invokes appropriate methods of addressed service. This makes communication real-time with ultra-low networking latency. Such solutions are highly required to develop time-sensitive applications for automotive, flock of drones, medtech, real-time manufacturing, real-time monitoring and other projects.
<br /><a href="/docs/img/areg-sdk-features.png"><img src="/docs/img/areg-sdk-features.png" alt="AREG SDK and multicast features" style="width:70%;height:70%"/></a><br />

#### Digital twin

Often, the digital twin applications use client-server architecture, where the middleware server collects the data of external devices and the UI application virtualizes them. In such solutions, devices interact either through server or UI client applications. The event-driven and the service-oriented architecture, and the real-time communication of AREG SDK is a perfect solution to develop digital twin applications that virtualize, monitor and control external devices, and immediately react to environment or device state change in real-time mode. External devices may also communicate without additional layers, which is an important factor for emergency, security and safety cases.

#### Simulation and test automations

When hardware provisioning to all employees is impossible, testing and checking unexpected phenomena of rapidly changing software in a simulated environment can be the most rational solution. If unit tests are used by developers to test a small portion of code and they may contain bugs, the simulation is used by developers and testers to check the system's functionality and stability. Simulations are portable and accessible to everyone, help to optimize solutions and avoid unnecessary risks. Projects using simulations are better prepared for remote work and easier to outsource.
<br /><a href="/docs/img/software-layers.png"><img src="/docs/img/software-layers.png" alt="Software application 4 layers" style="width:70%;height:70%"/></a><br />
The software components in applications normally are split into Data, Controller, Business and the optional Presentation layers. Distributed and service-oriented solution of the AREG engine eases system testing in a simulated environment, where the Simulation application provides an implementation of Data layer services, so that the rest of the application can be tested without any change.

The same technique of simulating data can be used to create API-driven test automations.

[ [↑ to top ↑](#table-of-contents) ]

---

## Examples[![](./docs/img/pin.svg)](#examples)

There are various [examples](./examples/) to demonstrate features of the AREG SDK. The examples are listed in the [examples/README.md](./examples/README.md) document.

[ [↑ to top ↑](#table-of-contents) ]

---

## Licensing[![](./docs/img/pin.svg)](#licensing)
 
AREG SDK is dual-licensed under free open source license (Apache version 2 license) and commercial license, which gives the commercial support, full rights to create and distribute software without open source license obligations. For licensing details see [LICENSE](./LICENSE.txt) document.
 
For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) website or contact _info[at]aregtech.com_.

[ [↑ to top ↑](#table-of-contents) ]

---

## Call for action[![](./docs/img/pin.svg)](#call-for-action)

We look for help and welcome to join the project:
* See the list of [open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) where we look for help.
* If you need new features, please open [new issue](https://github.com/aregtech/areg-sdk/issues) or start [new discussion](https://github.com/aregtech/areg-sdk/discussions).
* Please submit your pull requests in the [candidate branch](https://github.com/aregtech/areg-sdk/tree/20220701-candidate), which is our branch with latest sources.
* When create a pull request, please understand that reviewing and testing takes time, and we as well pay attention on coding style.
* If you look for invoiced commercial support or trainings, or if your project has possibility commercially support AREG SDK, please contact info[at]aregtec[dot]com. 

Did we help your project or did you just like the idea? Have you learned something new or have we inspired you for new great ideas? Then we ask you not to be indifferent and [![star AREG SDK](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=star%20AREG%20SDK)](https://github.com/aregtech/areg-sdk/). This small thank will inspire our contributors and help us expand our community. Please also share the project with your connections on [![Twitter](https://img.shields.io/twitter/url?label=Twitter&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk)](https://twitter.com/intent/tweet?text=Wow:&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk) and other social media platforms.

[ [↑ to top ↑](#table-of-contents) ]

---

**Follow us**<br />
[![Follow us on twitter](https://img.shields.io/twitter/follow/aregtech.svg?style=social)](https://twitter.com/intent/follow?screen_name=aregtech) 
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat&logo=linkedin&logoColor=b0c0c0&labelColor=363D44)](https://www.linkedin.com/company/aregtech) [![Join the chat at https://gitter.im/areg-sdk/community](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

---

![Viewers](https://gpvc.arturio.dev/aregtech)

<!-- markdownlint-enable -->
