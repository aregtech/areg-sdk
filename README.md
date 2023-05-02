<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:100%;height:100%"/></a>
  <br /><br /><strong>AREG SDK</strong>
</h1>

[![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=Latest%20release&style=social)](https://github.com/aregtech/areg-sdk/releases/tag/v1.0.0)
[![Stars](https://img.shields.io/github/stars/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/stargazers)
[![Fork](https://img.shields.io/github/forks/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/network/members)
[![Watchers](https://img.shields.io/github/watchers/aregtech/areg-sdk?style=social)](https://github.com/aregtech/areg-sdk/watchers)


---

<!-- markdownlint-disable -->
## Project Status[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#project-status)
<table class="no-border">
  <tr>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml" alt="CMake"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/cmake.yml/badge.svg" alt="CMake build"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/c-cpp.yml" alt="C/C++"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/c-cpp.yml/badge.svg" alt="C/C++ Make"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml" alt="MS Build"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml/badge.svg" alt="MS Build"/></a></td>
    <td><a href="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml" alt="CodeQL"><img src="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml/badge.svg" alt="CodeQL"/></a></td>
  </tr>
  <tr>
    <td><img src="https://img.shields.io/badge/Solution-C++17-blue.svg?style=flat&logo=c%2B%2B&logoColor=b0c0c0&labelColor=363D44" alt="C++ solution"/></td>
    <td><img src="https://img.shields.io/badge/OS-linux%20%7C%20windows-blue??style=flat&logo=Linux&logoColor=b0c0c0&labelColor=363D44" alt="Operating systems"/></td>
    <td colspan="2"><img src="https://img.shields.io/badge/CPU-x86%20%7C%20x86__64%20%7C%20arm%20%7C%20aarch64-blue?style=flat&logo=amd&logoColor=b0c0c0&labelColor=363D44" alt="CPU Architect"/></td>
  </tr>
</table>

---

## Introduction[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#introduction)

**AREG** (_Automated Real-time Event Grid_) is a powerful and lightweight cross-platform communication engine designed to facilitate seamless data transmission in IoT [fog- and mist-network](https://csrc.nist.gov/publications/detail/sp/500-325/final). AREG creates a grid of services using interface-centric approach, allowing multiple connected devices and software nodes to operate like thin distributed servers and clients. It automates real-time data transmission, utilizing both Client-Server and Publish-Subscribe models to ensure efficient and reliable communication between connected software nodes.

---

## Table of contents[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#table-of-contents)
- [Motivation](#motivation)
- [Composition](#composition)
- [Software build](#software-build)
    - [Clone sources](#clone-sources)
    - [Build with `cmake`](#build-with-cmake)
    - [Build with `make`](#build-with-make)
    - [Build with `msbuild`](#build-with-msbuild)
    - [Build with IDE](#build-with-ide)
- [Integration](#integration)
    - [Integrate for development](#integrate-for-development)
    - [Configure multicast router](#configure-multicast-router)
    - [Configure logging](#configure-logging)
- [Roadmap](#roadmap)
- [Interface-centricity](#interface-centricity)
- [More than embedded](#more-than-embedded)
- [Use cases and benefits](#use-cases-and-benefits)
    - [Distributed solution](#distributed-solution)
    - [Driverless devices](#driverless-devices)
    - [Real-time solutions](#real-time-solutions)
    - [Digital twin](#digital-twin)
    - [Simulation and tests](#simulation-and-tests)
- [Examples](#examples)
- [License](#license)
- [Call to action](#call-to-action)
- [Thank you!](#thank-you)

---

## Motivation[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#motivation)

Traditionally, devices act as connected clients to stream data to the cloud or fog servers for further processing.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/mist-network.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/mist-network.png" alt="IoT-to-Cloud (Nebula) network" style="width:70%;height:70%"/></a></div>

As data is generated and collected at the edge of the network (**mist network**), it is logical to redefine the role of connected Things and enable network-accessible services (_Public Services_) on them, thereby extending the _Cloud_ to the extreme edge. This approach serves as a strong foundation for implementing robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* _Autonomous, intelligent and self-aware devices_ with services directly in the environment of data origin.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Composition[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#composition)

The major modules of AREG SDK are:
1. [Multicast router (_mcrouter_)](https://github.com/aregtech/areg-sdk/tree/master/framework/mcrouter/) is a console application or OS-managed service that routes message.
2. [AREG engine](https://github.com/aregtech/areg-sdk/tree/master/framework/areg/) is a library to link with every application.
3. [Code generator](https://github.com/aregtech/areg-sdk/tree/master/tools/) is a tool that creates service provider / consumer objects from interface documents.

> 💡 The multiple [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/) demonstrate the features and fault tolerant behavior of AREG communication engine.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Software build[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#software-build)

> 💡 Check the [Wiki page](https://github.com/aregtech/areg-sdk/wiki) of _AREG SDK_. We change the content and add more details.

The source codes of AREG framework and examples support following platform, CPU and compilers:
<table>
  <tr>
    <td nowrap><strong>Platforms</strong></td><td><i>Linux</i> (list of <a href="https://github.com/aregtech/areg-sdk/blob/master/docs/POSIX.md#posix-api" alt="POSX API">POSIX API</a>), <i>Cygwin</i>, <i>Windows</i>.</td>
  </tr>
  <tr>
    <td nowrap><strong>CPU</strong></td><td><i>x86</i>, <i>x86_64</i>, <i>arm</i> and <i>aarch64</i>.</td>
  </tr>
  <tr>
    <td nowrap><strong>Compilers</strong></td><td><i>GCC</i>, <i>Clang</i>, <i>MSVC</i> and <i>Cygwin gcc</i>.</td>
  </tr>
</table>

The tools to compile sources:
| Tool | Solution | Platforms | API | Quick actions to compile |
| --- | --- | --- | --- | --- |
| `cmake` | `CMakeLists.txt` | **Linux, Cygwin, Windows** | _POSIX_, _Win32_ | &nbsp;&nbsp; - Build with `cmake`.<br /> &nbsp;&nbsp; - Build in _VSCode_;<br /> &nbsp;&nbsp; - Build in _MSVS_. |
| `make` | `Makefile` | **Linux, Cygwin**| _POSIX_ | &nbsp;&nbsp; - Build with `make`. |
| `msbuild` | `areg-sdk.sln` | **Windows** | _Win32_ | &nbsp;&nbsp; - Build with `msbuild`.<br />&nbsp;&nbsp; - Open and build in _MSVS_. |

To customize the build, refer to the [Wiki pages](https://github.com/aregtech/areg-sdk/wiki). By default, the binaries are in `<areg-sdk>/product/build/<family>-<compiler>/<platform>-<bitness>-<cpu>-<build type>/bin` folder (for example, `areg-sdk/product/build/gnu-gcc/linux-64-x86_64-release/bin`).

> 💡 The other POSIX-compliant OS and compilers are not tested yet.<br />

### Clone sources

To get AREG SDK source codes and dependent modules, open _Terminal_ in your `projects` folder and take these steps:
```bash
# This clones the source codes of AREG SDK and dependent module
git clone --recurse-submodules https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```

If you already have AREG SDK sources and only need to clone the submodules, take these steps:
```bash
cd areg-sdk
git submodule update --init --recursive
```

If you have AREG SDK all sources and only need to update to the latest submodule sources, take these steps:
```bash
cd areg-sdk
git submodule update --remote --recursive
```

> 💡 Note, after running this Git command your submodule sources may differ from the sources that AREG SDK uses.

To build applications after cloning sources, employ `cmake`, `make`, or `msbuild` tools. Ensure that you have a C++17 GNU, Clang, Cygwin, or MSVC compiler installed on your machine.

### Build with `cmake`

Firstly, [clone the sources](#clone-sources) properly. To build with [cmake](https://cmake.org/), using default (`g++`) compiler and options, open _Terminal_ in `areg-sdk` folder and take the steps:
```bash
# Step 1: Initialize cache and build configuration in folder './build' folder.
#         Default options: g++ compiler, release build, enabled examples and unit tests
cmake -B ./build

# Step 2: Compile sources.
cmake --build ./build -j 8
 ```

### Build with `make`

Firstly, [clone the sources](#clone-sources) properly. To build with [make](https://www.gnu.org/software/make/), using default (`g++`) compiler and options, open _Terminal_ in `areg-sdk` folder and take the steps:
```bash
# Compile sources with default options: g++ compiler, release build, enabled examples and unit tests
make -j 8
```

### Build with `msbuild`

Firstly, [clone the sources](#clone-sources) properly. To build with [MSBuild](https://visualstudio.microsoft.com/downloads/), using MSVC default options, open _Terminal_ in `areg-sdk` folder and take the steps:
```bash
# Compile sources with default options: msvc compiler, debug build, enabled examples and unit tests
MSBuild .
```

### Build with IDE

Firstly, [clone the sources](#clone-sources) properly. Here we consider builds with [Microsoft Visual Studio](https://visualstudio.microsoft.com/) and [Visual Studio Code](https://code.visualstudio.com/).

1. Open [`areg-sdk.sln`](https://github.com/aregtech/areg-sdk/blob/master/areg-sdk.sln) solution file in _Microsoft Visual Studio_ and build the solution with MSVC.
2. Open `areg-sdk` folder in **Visual Studio Code** => select [**CMakeLists.txt**](https://github.com/aregtech/areg-sdk/blob/master/CMakeLists.txt) in **Explorer** => mouse right click => click on **Configure All Projects** in menu => wait for completion => mouse right click on **CMakeLists.txt** => click on **Build All Projects** to build the sources with default (`g++`) compiler and default options.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Integration[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#integration)

> 💡 Check the [Wiki page](https://github.com/aregtech/areg-sdk/wiki) of _AREG SDK_. We change the content and add more details.

### Integrate for development

Firstly, [clone the sources](#clone-sources) properly. Use following parameters to change compilation default settings:

| Parameter | Description, possible values |
| --- | --- |
| **AREG_OUTPUT_BIN** | Change the default output folder of compiled shared libraries and executables. |
| **AREG_OUTPUT_LIB** | Change the default output folder of compiled static libraries. |
| **AREG_COMPILER_FAMILY** | Set compilers by specifying _gnu_, _llvm_, _cygwin_ or _msvc_. |
| **AREG_COMPILER** | Set compiler by specifying _g++_, _gcc_, _clang++_, _clang_ or _cl_. |
| **AREG_BINARY** | Set AREG library type by specifying _shared_ or _static_. |
| **AREG_BUILD_TYPE** | Set the build type by specifying _Debug_ or _Release_. |
| **AREG_ENABLE_EXT** | Set the flag to indicate the build of a special library with extended features. |

More details of parameters for each tool are described in the appropriate `user` configuration files:
- For builds with `cmake`, in the [conf/cmake/user.cmake](https://github.com/aregtech/areg-sdk/blob/master/conf/cmake/user.cmake) file.
- For builds with `make`, in the [conf/make/user.mk](https://github.com/aregtech/areg-sdk/blob/master/conf/make/user.mk) file.
- For builds with `MSBuild`, in the [conf/msvc/user.props](https://github.com/aregtech/areg-sdk/blob/master/conf/msvc/user.props) file.

An example to configure and compile codes with `cmake`:
```bash
# Step 1: configure, make appropriate settings
cmake -B ./build -DAREG_COMPILER=clang++ -DAREG_BINARY=static -DAREG_BUILD_TYPE=Debug -DAREG_ENABLE_EXT:BOOL=ON AREG_OUTPUT_BIN="~/product/areg-sdk/"

# Step 2: build applications
cmake --build -j 8
```

In this scenario, the source codes are set up to build using `clang++`. The AREG engine is created as a static library with enabled extensions (may require additional dependencies). All applications are built in Debug mode, and the resulting binaries are stored in the `~/product/areg-sdk/` directory.

The additional development guidance and step-by-step example to create a simple service-enabled application are described in [_Hello Service!_](https://github.com/aregtech/areg-sdk/blob/master/docs/HelloService.md) document, which demonstrate the various scenarios of creating service provider and service consumer within the same thread, within the same process and within the same network. The other useful documents are in [DEVELOP.md](https://github.com/aregtech/areg-sdk/blob/master/docs/DEVELOP.md).

### Configure multicast router

Configure [_router.init_](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/router.init) file to set the IP-address and the port of _multicast router_ (`mcrouter`):
```
connection.address.tcpip    = 127.0.0.1	# the address of mcrouter host
connection.port.tcpip       = 8181      # the connection port of mcrouter
```
The multicast router is required only for multiprocessing applications and can be ignored in case of multithreading. It forms a network and requires GPOS.

### Configure logging

Configure [_log.init_](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/log.init) to set scopes, priorities and file name for logging:
```
log.file        = %home%/logs/%appname%_%time%.log # create logs in 'log' subfolder of user home 
scope.mcrouter.*= NOTSET ;                         # disable logs for mcrouter.

scope.my_app.*                   = DEBUG | SCOPE ; # enable all logs of my_app
scope.my_app.ignore_this_scope   = NOTSET ;        # disable logs of certain scopes in my_app
scope.my_app.ignore_this_group_* = NOTSET ;        # disable logs of certain scope group in my_app
```
> 💡 By default, the `router.init` and `log.init` files are located in the `config` subfolder of binaries.<br />
> 💡 To enable all logs of all applications, use `scope.*  = DEBUG | SCOPE ;` .<br />
> 💡 Currently logging is possible only in the file.


<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Roadmap[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#roadmap)

The aim of the AREG SDK is a lightweight, self-sufficient development and testing system to help developers to create complex **Desktop**, **Embedded**, and **IoT edge** applications in a shorter amount of time (_30-50% faster_). 
- **Planned features of the framework:**
  * Multi-channel and multi-protocol communication.
  * Logging service to collect logs in the network.
  * _Internet_ (web) category services.
- **Planned tools:**
  * Service interface designer.
  * Interactive log viewer.
  * Service testing and Data simulation tool.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Interface-centricity[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#interface-centricity)

**ORPC**, or _Object Remote Procedure Call_, is a remote procedure call concept that targets an interface on an object. It enables the construction of a _service mesh_ (or _service grid_), where applications offer reusable services, and programmable client objects request method execution of programmable server objects without needing to know their location in the network.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/interface-centric.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/interface-centric.png" alt="Service oriented and interface-centric" style="width:50%;height:50%"/></a></div>

ORPC concept is interface-centric, similar to object-oriented programming, and supports transparent handling of multiple instances of the same object. There are no protocol restrictions, although the bi-directional communication is required to send messages to all connected nodes. The programmable server objects are called _service providers_, and the programmable clients are called _service consumers_. This approach enables AREG to combine the features of _action-centric_ (Client-Server / Request-Reply) and _data-centric_ (Publish-Subscribe / PubSub) models.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## More than embedded[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#more-than-embedded)

AREG is designed to provide a homogeneous solution for multithreading, multiprocessing, and internet communications through categorized services (_Local_, _Public_, and _Internet_). These services are software components with predefined interfaces, which methods are invoked remotely.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-services.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:70%;height:70%"/></a></div>

> 💡 Currently AREG engine supports _Local_ (multithreading) and _Public_ (multiprocessing) service categories.

AREG forms a fault-tolerant system that automatically discovers and automates communications between services, allowing developers to focus on application logic development. The system ensures:
* The crash of one application does not affect the entire system.
* The clients automatically receive service availability notifications.
* The requests, responses, and notifications are invoked in their own thread context.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Use cases and benefits[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#use-cases-and-benefits)

AREG SDK can be used in a very large scope of multithreading and multiprocessing application development.

### Distributed solution

<details open><summary> Click to show / hide <code>distributed solution</code>.</summary><br/>

The AREG SDK is a distributed services solution that enables components to interact seamlessly across nodes on the network, appearing as if they are located within a single process. To define relationships and distribute services across processes, developers create runtime loadable models.

The following is a demonstration of 2 _models_ defined in 2 different processes, where one _model_ defines **service provider** and the second is at the same time is a **service consumer** (client) and a **service provider**. For simplicity, let's name them `service.cpp` and `mixed.cpp`.

A: _model of service provider component in **service.cpp**_:
```cpp
// service.cpp, define service provider
BEGIN_MODEL("MyModel")

  BEGIN_REGISTER_THREAD( "Thread1" )
    BEGIN_REGISTER_COMPONENT( "SystemShutdown", SystemShutdownService )
      REGISTER_IMPLEMENT_SERVICE( NESystemShutdown::ServiceName, NESystemShutdown::InterfaceVersion )
    END_REGISTER_COMPONENT( "SystemShutdown" )
  END_REGISTER_THREAD( "Thread1" )

END_MODEL("MyModel")
```

B: _model of service consumer and service provider (mixed) component in **mixed.cpp**_:
```cpp
// mixed.cpp, define service provider and service consumer.
BEGIN_MODEL("MyModel")

  BEGIN_REGISTER_THREAD( "Thread1" )
    BEGIN_REGISTER_COMPONENT( "RemoteRegistry", RemoteRegistryService )
      REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
      REGISTER_DEPENDENCY("SystemShutdown")
    END_REGISTER_COMPONENT( "RemoteRegistry" )
  END_REGISTER_THREAD( "Thread1" )

END_MODEL("MyModel")
```
After declaration of services, each project can load them in the `main()` method and load them as it is shown in the next example.

C: _common code_ for **service.cpp** and **mixed.cpp** files:
```cpp
int main()
{
    Application::initApplication( );    // initialize defaults
    Application::loadModel("MyModel");  // load model, start services
    Application::waitAppQuit( );        // wait for quit signal
    Application::unloadModel("MyModel");// unload model, stop services
    Application::releaseApplication();  // release resources

    return 0;
}
```

In these codes:
1. **service.cpp** contains model to register service `"SystemShutdown"` that implements interface `NESystemShutdown::ServiceName`.
2.  **mixed.cpp** contains model to register service `"RemoteRegistry"` that implements interface `NERemoteRegistry::ServiceName` and consumes (requires) service `"SystemShutdown"`.
3. `int main()` is identical in both files. It initializes resources, loads model and waits for the completion.

The developers should create `SystemShutdownService` and `RemoteRegistryService` objects that implement the business logic of _request_ methods of provided and _response_ methods of used service interfaces. When these processes start, the services are automatically discovered via `mcrouter`. With this technique, the projects easily develop multiprocessing applications where provided services can be distributed and accessed remotely within the network formed by `mcrouter`.

An example of developing a service provider and consumer in one and multiple processes is in [**Hello Service!**](https://github.com/aregtech/areg-sdk/blob/master/docs/HelloService.md) guide. As well there are multiple [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/) of multiprocessing and multithreading applications.
</details>

### Driverless devices

<details open><summary> Click to show / hide <code>driverless devices</code>.</summary><br/>

Normally, the devices are supplied with the drivers to install in the system. 

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/driver-solution.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/driver-solution.png" alt="kernel-mode driver solution" style="width:70%;height:70%"/></a></div>

Proposed are driverless service-enabled devices that do not require installation of drivers in the system.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/driverless-solution.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/driverless-solution.png" alt="AREG SDK driverless solution" style="width:70%;height:70%"/></a></div>

The services, described in the interface prototype documents, are faster and easier to develop than drivers, as they do not require special techniques and can be developed like user mode applications. Service development reduces risks and the code generator simplifies client object generation from a service interface document.
</details>

### Real-time solutions

<details open><summary> Click to show / hide <code>real-time solutions</code>.</summary><br/>

AREG engine automatically generates and delivers messages to the target and invokes the exact methods of the target objects in real-time with ultra-low networking latency. This makes it ideal for developing time-sensitive applications for industries such as automotive, drone fleets, medtech, real-time manufacturing, and monitoring.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/areg-sdk-features.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/areg-sdk-features.png" alt="AREG SDK and multicast features" style="width:70%;height:70%"/></a></div>
</details>

### Digital twin

<details open><summary> Click to show / hide <code>digital twin</code>.</summary><br/>

AREG framework's event-driven and service-oriented architecture, coupled with real-time communication, offers a robust solution for digital twin applications. This framework allows for visualization, monitoring, and control of external devices, while also enabling immediate reaction to changes in the environment or device state. AREG's approach eliminates the need for additional communication layers, making it an ideal solution for emergency, security, and safety applications.

</details>

### Simulation and tests

<details open><summary> Click to show / hide <code>simulation and tests</code>.</summary></br>

Simulating a software environment is a practical solution for testing and checking rapidly changing software, especially when hardware provisioning is not feasible. Simulations are portable, accessible, and help to optimize solutions while avoiding unnecessary risks. Projects using simulations are better prepared for remote work and easier to outsource.

<div align="center"><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/software-layers.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/software-layers.png" alt="Software application 4 layers" style="width:70%;height:70%"/></a></div>

The distributed and service-oriented solution of AREG engine eases application testing in a simulated environment. For example, the Simulation application may provide identical Data layer services for seamless testing of the rest of the application.

The same technique of simulating data can be used to create API-driven test automations.
</details>

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Examples[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#examples)

There are various [examples](https://github.com/aregtech/areg-sdk/tree/master/examples/) to demonstrate features of the AREG SDK. The examples are listed in the [examples/README.md](https://github.com/aregtech/areg-sdk/blob/master/examples/README.md) document.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## License[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#license)
 
AREG SDK is under free open source [_Apache License Version 2.0_](https://github.com/aregtech/areg-sdk/blob/master/LICENSE.txt). However, AREG SDK can be commercially licensed, which includes the commercial support, full rights to create and distribute software without open source license obligations. For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) website or contact _info[at]aregtech[dot]com_.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Call to action[![](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/pin.svg)](#call-to-action)

Join our project and provide assistance by:
* Checking out the list of [open issues](https://github.com/aregtech/areg-sdk/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) where we need help.
* If you need new features, please open a [new issue](https://github.com/aregtech/areg-sdk/issues) or start a [discussion](https://github.com/aregtech/areg-sdk/discussions).
* hen creating a pull request, kindly consider the time it takes for reviewing and testing, and maintain proper coding style.
* If you require invoiced commercial support or training, or if you wish to support AREG SDK commercially, kindly contact us at info[at]aregtech[dot]com. 

⭐ [![star AREG SDK](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=star%20AREG%20SDK)](https://github.com/aregtech/areg-sdk/) ⭐ AREG SDK repository if liked. This gesture of appreciation encourages contributors and helps us grow our community. Contact us for the feedback or new ideas.

<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

## Thank you!

Special thanks to all contributors and supporters that starred this repository.

**Our contributors:**

<a href="https://github.com/aregtech/areg-sdk/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=aregtech/areg-sdk" width=50% height=50%/>
</a>


**Our stargazers**:

[![Stargazers repo roster for @aregtech/areg-sdk](https://reporoster.com/stars/aregtech/areg-sdk)](https://github.com/aregtech/areg-sdk/stargazers)


<div align="right">[ <a href="#table-of-contents">↑ to top ↑</a> ]</div>

---

**Follow us** at<br />
[![Follow us on twitter](https://img.shields.io/twitter/follow/aregtech.svg?style=social)](https://twitter.com/intent/follow?screen_name=aregtech) 
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Aregtech-blue?style=flat&logo=linkedin&logoColor=b0c0c0&labelColor=363D44)](https://www.linkedin.com/company/aregtech)
[![Join the chat at https://gitter.im/areg-sdk/community](https://badges.gitter.im/areg-sdk/community.svg)](https://gitter.im/areg-sdk/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

---

**Share** the project link with your network on social media.

<a href="https://www.reddit.com/submit?url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk&title=Awesome%20communcation%20engine!" target="_blank"><img src="https://img.shields.io/twitter/url?label=Reddit&logo=Reddit&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Reddit"/></a>&nbsp;
<a href="https://www.linkedin.com/shareArticle?mini=true&url=https%3A//github.com/aregtech/areg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=LinkedIn&logo=LinkedIn&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on LinkedIn"/></a>&nbsp;
<a href="https://twitter.com/intent/tweet?text=%23AREG%20%23realtime%20communication%20engine%20for%20%23embedded%20and%20%23IoT%0A%0Ahttps%3A//github.com/aregtech/areg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Twitter&logo=Twitter&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Shared on Twitter"/></a>&nbsp;
<a href="https://www.facebook.com/sharer/sharer.php?u=https%3A//github.com/aregtech/areg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Facebook&logo=Facebook&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Facebook"/></a>&nbsp;
<a href="https://t.me/share/url?text=Awesome%20communication%20engine!&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Telegram&logo=Telegram&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Telegram"/></a>&nbsp;
<a href="https://wa.me/?text=Awesome%20communication%20engine!%5Cn%20https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" target="_blank"><img src="https://img.shields.io/twitter/url?label=Whatsapp&logo=Whatsapp&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk" alt="Share on Whatsapp"/></a>&nbsp;
<a href="mailto:?subject=Awesome%20communication%20engine&body=Checkout%20this%20awesome%20communication%20engine%3A%0Ahttps%3A//github.com/aregtech/areg-sdk%0A" target="_blank"><img src="https://img.shields.io/twitter/url?label=GMail&logo=GMail&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk"/></a>

---

![Viewers](https://gpvc.arturio.dev/aregtech)

<!-- markdownlint-enable -->
