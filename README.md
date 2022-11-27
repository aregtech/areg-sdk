<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:100%;height:100%"/></a>
  <br /><br /><strong>AREG SDK</strong>
</h1>

![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=%20%F0%9F%93%A3%20Latest%20release&style=flat&logoColor=b0c0c0&labelColor=363D44)

The latest development sources are in [20220701-candidate](https://github.com/aregtech/areg-sdk/tree/20220701-candidate) branch. Please check the [build status](https://github.com/aregtech/areg-sdk/actions) before cloning it.

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
   - [Simulation and tests](#simulation-and-tests)
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
* _Autonomous, intelligent and self-aware devices_ with services directly in the environment of data origin.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## More than embedded[![](./docs/img/pin.svg)](#more-than-embedded)

When we were designing AREG SDK, the guiding principle was to provide a homogeneous solution for Multithreading, Multiprocessing and Internet communication wrapped in services appropriately having _Local_, _Public_ and _Internet_ categories. These services are neither processes nor tasks managed by the operating system, they are software components with a predefined interface, in which methods are invoked remotely.
<br /><a href="docs/img/areg-services.png"><img src="docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:70%;height:70%"/></a><br />
> 💡 In current version, the AREG communication engine supports _Local_ (multithreading) and _Public_ (multiprocessing) service categories. The _Internet_ (web) service category is in development pipeline.

The AREG engine forms a fault-tolerant system and does not require process startup priority. It automatically discovers services, automates communications, enables distributed service programming, and helps developers to focus on application business logic as if they would program a single process application with one thread where methods of objects are event-driven. The engine guarantees that:
* The crash of one application does not cause the crash of the system.
* The service clients automatically receive service availability status notifications.
* The requests, response and notifications are automatically invoked and run in thread-safe mode.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Composition[![](./docs/img/pin.svg)](#composition)

Currently AREG SDK consists of:
1. [Multicast router (_mcrouter_)](./framework/mcrouter/) for IPC that runs either as a service managed by the OS or as a console application.
2. [AREG framework (or engine)](./framework/areg/) is a (shared or static) library to link with every application.
3. [Code generator tool](./tools/) to create client and server base objects from a service prototype document.

The network communication model of AREG framework is based on _multicast router_ and service architecture is _interface-centric_, which in combination of both enable to comprise all features of _action-centric_ architecture (Client-Server / Request-Reply model) and _data-centric_ architecture (Publisher-Subscriber / PubSub model).

> 💡 The [examples](./examples) of _AREG SDK_ contain multiple projects to demonstrate the features and fault tolerant behavior of AREG communication engine.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

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

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Software build[![](./docs/img/pin.svg)](#software-build)

> 💡 Chek the [Wiki page](https://github.com/aregtech/areg-sdk/wiki) of _AREG SDK_ for mode details. We change the content and add more details.

The source codes of AREG framework and examples support following platform, CPU and compilers:
<table>
  <tr>
    <td nowrap><strong>OS</strong></td>
    <td><i>Linux</i> (list of <a href="./docs/POSIX.md#posix-api" alt="POSX API">POSIX API</a>), <i>Windows 8</i> and higher.</td>
  </tr>
  <tr>
    <td nowrap><strong>CPU</strong></td>
    <td><i>x86</i>, <i>x86_64</i>, <i>arm</i> and <i>aarch64</i>.</td>
  </tr>
  <tr>
    <td nowrap><strong>Compilers</strong></td>
    <td><i>GCC</i>, <i>g++</i>, <i>clang</i>, <i>MSVC</i> and <i>cygwin CC</i>.</td>
  </tr>
</table>

The tools to use to compile sources:
| Solution | Platforms | API | Quick actions to compile |
| --- | --- | --- | --- |
| `CMakeLists.txt` | **Linux, Windows** | _POSIX_, _Win32_ | Make one of these actions:<br /> &nbsp;&nbsp; - Call `cmake` in _command line_.<br /> &nbsp;&nbsp; - Configure and build in _Visual Studio Code_;<br /> &nbsp;&nbsp; - Configure and build in _Microsoft Visual Studio_. |
| `Makefile` | **Linux**| _POSIX_ | Call `make` in _command line_. |
| `areg-sdk.sln` | **Windows** | _Win32_ | Open and build in _Microsoft Visual Studio_. |
| `.project` | **Linux, Windows** | _POSIX_ | Import and build projects in _Eclipse_. |

> 💡 The other POSIX-compliant OS and compilers are not tested yet.<br />
> 💡 Make user specific changes (like switch compiler or output folder) only in appropriate `user` files:<br />
> - For `cmake`, make changes in [conf/cmake/user.cmake](./conf/cmake/user.cmake) file.<br />
> - For `make`, make changes in [conf/make/user.mk](./conf/make/user.mk) file.
> - For `MSBuild`, make changes in [conf/msvc/user.props](./conf/msvc/user.props) file.

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
2. Open `areg-sdk` folder in [MSVC](https://visualstudio.microsoft.com/) (2019 or higher), select `CMakeLists.txt` in `areg-sdk` root, configure and build.
3. Open `areg-sdk` folder in [VS Code](https://code.visualstudio.com/), select `CMakeLists.txt` in `areg-sdk` root, configure and build.
4. Open [Eclipse](https://www.eclipse.org/ide/), import all projects from `areg-sdk` root, select projects and build.

> 💡 Compilation with _Eclipse_ under **Windows** might require to change the Toolchain.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Software integration[![](./docs/img/pin.svg)](#software-integration)

#### Multicast router

Configure [_router.init_](./framework/areg/resources/router.init) file to set the IP-address and the port of _multicast router_ (`mcrouter`):
```
connection.address.tcpip    = 127.0.0.1	# the address of mcrouter host
connection.port.tcpip       = 8181      # the connection port of mcrouter
```
The multicast router is a process that can run as a system service or application to form the network, and it can run on any device like PC, gateway or even a small router. In case of multithreading application, there is no need to configure and start `mcrouter`.

#### Logging service

Configure [_log.init_](./framework/areg/resources/log.init) to set scopes, priorities and file name for logging:
```
log.file        = %home%/logs/%appname%_%time%.log # create logs in 'log' subfolder of user home 
scope.mcrouter.*= NOTSET ;                         # disable logs for mcrouter.

scope.my_app.*                   = DEBUG | SCOPE ; # enable all logs of my_app
scope.my_app.ignore_this_scope   = NOTSET ;        # disable logs of certain scopes in my_app
scope.my_app.ignore_this_group_* = NOTSET ;        # disable logs of certain scope group in my_app
```
> 💡 By default, the `router.init` and `log.init` files are located in the `config` subfolder of binaries.<br />
> 💡 To enable all logs of all applications, use `scope.*  = DEBUG | SCOPE ;` .<br />
> 💡 Currently logging is possible only in file.

#### Development

The development guidance and step-by-step example to create a simple service-enabled application are described in [DEVELOP](./docs/DEVELOP.md). See [_Hello Service!_](./docs/DEVELOP.md#hello-service) as an example to create service. More details are in the [Wiki page](https://github.com/aregtech/areg-sdk/wiki) of _AREG SDK_.


<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Use cases and benefits[![](./docs/img/pin.svg)](#use-cases-and-benefits)

AREG SDK can be used in a very large scope of multithreading and multiprocessing application development.

#### Distributed solution

AREG SDK is a solution of distributed services that can run on any node of the network and because of automations and transparent communication interact as if the components are located in one process. To distribute services in the processes and define relationships, developers create _model_ to load during runtime.

The following is a demonstration of a static _model_ to start and stop services:
```cpp
// Defines static model with 2 services
BEGIN_MODEL("MyModel")

  BEGIN_REGISTER_THREAD( "Thread1" )
    BEGIN_REGISTER_COMPONENT( "SystemShutdown", SystemShutdownService )
      REGISTER_IMPLEMENT_SERVICE( NESystemShutdown::ServiceName, NESystemShutdown::InterfaceVersion )
    END_REGISTER_COMPONENT( "SystemShutdown" )
  END_REGISTER_THREAD( "Thread1" )

  BEGIN_REGISTER_THREAD( "Thread2" )
    BEGIN_REGISTER_COMPONENT( "RemoteRegistry", RemoteRegistryService )
      REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
      REGISTER_DEPENDENCY("SystemShutdown")
    END_REGISTER_COMPONENT( "RemoteRegistry" )
  END_REGISTER_THREAD( "Thread2" )

END_MODEL("MyModel")

int main()
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );
    // load model to start service components
    Application::loadModel("MyModel");
    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    // stop and unload service components
    Application::unloadModel("MyModel");
    // release and cleanup resources of application.
    Application::releaseApplication();

    return 0;
}
```
This example uses MACRO to create a model `"MyModel"` with two services:
1. Service with the _role_ `"SystemShutdown"` is registered in the thread `"Thread1"` and provides an interface with name `NESystemShutdown::ServiceName`.
2.  Service with the _role_ `"RemoteRegistry"` is registered in the thread `"Thread2"`, provides an interface with name `NERemoteRegistry::ServiceName` and has dependency (i.e. _is a client_) of service with _role_ `"SystemShutdown"`.

The services are started when load model by calling function `Application::loadModel("MyModel")`, and stopped when call function `Application::unloadModel("MyModel")`. When define a model, these two services can be registered in the same thread or distributed in 2 processes. In all cases, the physical location of service components remain transparent, so that the architectures have more flexibility to distribute computing power. An example of developing a service and a client in one and multiple processes is in [**Hello Service!**](./docs/DEVELOP.md#hello-service) project described in the development guide.

#### Driverless devices

Normally, the devices are supplied with the drivers to install in the system and with the header files to integrate in the application(s). The drivers often run in Kernel mode and the crash of the driver crashes the entire system. Driver development requires a special technique, which is different for each operating system, and it is hard to debug. 
<br /><a href="/docs/img/driver-solution.png"><img src="/docs/img/driver-solution.png" alt="Kkernel-mode driver solution" style="width:70%;height:70%"/></a><br />
Our proposal is to deliver driverless service-enabled devices, where device-specific services are described in the interface prototype documents. 
<br /><a href="/docs/img/driverless-solution.png"><img src="/docs/img/driverless-solution.png" alt="AREG SDK driverless solution" style="width:70%;height:70%"/></a><br />
In contrast to drivers, the service development does not differ from user mode application development, it is faster to develop, easily serves multiple applications (_service clients_), contains fewer risks and the code generator helps to generate client object from service interface document.

#### Real-time solutions

When a remote method of the service interface is called, the engine of AREG SDK automatically generates and delivers messages to the target and automatically invokes the exact methods of the exact target objects. This makes communication real-time with ultra-low networking latency. Such solutions are highly required to develop time-sensitive applications for automotive, flock of drones, medtech, real-time manufacturing, real-time monitoring and other projects.
<br /><a href="/docs/img/areg-sdk-features.png"><img src="/docs/img/areg-sdk-features.png" alt="AREG SDK and multicast features" style="width:70%;height:70%"/></a><br />

#### Digital twin

Often, the digital twin applications use client-server architecture, where the middleware server collects the data of external devices and the UI application virtualizes them. In such solutions, devices interact either through server or UI client applications. The event-driven and the service-oriented architecture, and the real-time communication of AREG framework is a good solution to develop digital twin applications to virtualize, monitor and control external devices, and immediately react to environment or device state change in real-time mode. External devices may also communicate without additional layers, which is an important factor for emergency, security and safety cases.

#### Simulation and tests

When hardware provisioning to all employees is impossible, testing and checking unexpected phenomena of rapidly changing software in a simulated environment is the most rational solution. If unit tests are for testing a small portion of code and the tests may contain bugs, the simulation is used by developers and testers to check functionality and stability of the system. Simulations are portable and accessible to everyone, help to optimize solutions and avoid unnecessary risks. Projects using simulations are better prepared for remote work and easier to outsource.
<br /><a href="/docs/img/software-layers.png"><img src="/docs/img/software-layers.png" alt="Software application 4 layers" style="width:70%;height:70%"/></a><br />
The software components in applications normally are split into Data, Controller, Business and the optional Presentation layers. Distributed and service-oriented solution of AREG framework can ease system testing in a simulated environment, where the Simulation application provides an implementation of Data layer services, so that the rest of the application can be tested without any change.

The same technique of simulating data can be used to create API-driven test automations.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Examples[![](./docs/img/pin.svg)](#examples)

There are various [examples](./examples/) to demonstrate features of the AREG SDK. The examples are listed in the [examples/README.md](./examples/README.md) document.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Licensing[![](./docs/img/pin.svg)](#licensing)
 
AREG SDK is under free open source [_Apache License Version 2.0_](./LICENSE.txt). However, AREG SDK can be commercially licensed, which includes the commercial support, full rights to create and distribute software without open source license obligations. For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) website or contact _info[at]aregtech[dot]com_.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Call for action[![](./docs/img/pin.svg)](#call-for-action)

We look for help and welcome to join the project:
* See the list of [open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) where we look for help.
* If you need new features, please open [new issue](https://github.com/aregtech/areg-sdk/issues) or start [new discussion](https://github.com/aregtech/areg-sdk/discussions).
* Please submit your pull requests in the [candidate branch](https://github.com/aregtech/areg-sdk/tree/20220701-candidate), which is our branch with latest sources.
* When create a pull request, please understand that reviewing and testing takes time, and we as well pay attention on coding style.
* If you look for invoiced commercial support or trainings, or if your project has possibility commercially support AREG SDK, please contact info[at]aregtech[dot]com. 

Did we help your project? Have you learned something new? Have we inspired you for new great ideas? Then we ask not to be indifferent and [![star AREG SDK](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=star%20AREG%20SDK)](https://github.com/aregtech/areg-sdk/). This small thank inspires contributors and help us to expand our community. Please also share the project with your network at [![Twitter](https://img.shields.io/twitter/url?label=Twitter&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk)](https://twitter.com/intent/tweet?text=Wow:&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk) and other social media platforms.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

**Follow us**<br />
[![Follow us on twitter](https://img.shields.io/twitter/follow/aregtech.svg?style=social)](https://twitter.com/intent/follow?screen_name=aregtech) 
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat&logo=linkedin&logoColor=b0c0c0&labelColor=363D44)](https://www.linkedin.com/company/aregtech) [![Join the chat at https://gitter.im/areg-sdk/community](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

---

![Viewers](https://gpvc.arturio.dev/aregtech)

<!-- markdownlint-enable -->
