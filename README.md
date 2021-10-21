<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:100%;height:100%"/></a>
  <br><br><b>AREG SDK</b>
</h1>

> 💡 By ![star us](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=staring%20us) on GitHub and spreading the AREG SDK link on social media, you help and inspire us for further development of technology, which helps to cut costs of projects and create intelligent service enabled products!

![Latest release](https://img.shields.io/github/v/release/aregtech/areg-sdk?label=%20%F0%9F%93%A3%20Latest%20release&style=flat&logoColor=b0c0c0&labelColor=363D44)

<!-- markdownlint-disable -->
## 📌 Project status
<table class="no-border">
  <tr>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/c-cpp.yml/badge.svg" alt="C++ compiltation"/></td>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml/badge.svg" alt="MS Build"/></td>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml/badge.svg" atl="CodeQL"/></td>
  </tr>
  <tr>
    <td><img src="https://img.shields.io/badge/Solution-C++17-blue.svg?style=flat&logo=c%2B%2B&logoColor=b0c0c0&labelColor=363D44" alt="C++ solution"/></td>
    <td><img src="https://img.shields.io/badge/OS-linux%20%7C%20windows-blue??style=flat&logo=Linux&logoColor=b0c0c0&labelColor=363D44" alt="Operating systems"/></td>
    <td><img src="https://img.shields.io/badge/CPU-x86%20%7C%20x86__64%20%7C%20arm%20%7C%20aarch6-blue?style=flat&logo=amd&logoColor=b0c0c0&labelColor=363D44" alt="CPU Architect"/></td>
  </tr>
</table>

## 📌 Introduction

**AREG SDK** is an Interface centric lightweight real-time asynchronous communication engine to provide [distributed](https://en.wikipedia.org/wiki/Distributed_computing) and enable [mist](https://csrc.nist.gov/publications/detail/sp/500-325/final) computing, where Things interact and provide services, as if they act like thin distributed servers.

## 📌 Table of content
1. [Motivation](#-motivation).
2. [More than embedded](#-more-than-embedded).
3. [Composition](#-composition).
4. [Software build](#-software-build).
5. [Integration and development](#-integration-and-development).
   - [Multicast router](#mulitcast-router).
   - [Logging service](#logging-service).
   - [Development](#development).
6. [Use cases and benefits](#-use-cases-and-benefits).
   - [Distributes solutions](#distributed-solution).
   - [Driverless devices](#driverless-devices).
   - [Real-time solutions](#real-time-solutions).
   - [Digital twin](#digital-twin).
   - [Simulation and test automations](#simulation-and-test-automations).
7. [Examples](#-examples).
8. [Licensing](#-licensing).
9. [Call for action](#-call-for-action)!

## 📌 Motivation

Traditionally, devices are connected clients to stream data to the cloud or fog servers for further processing.
<br><br><a href="/docs/img/mist-network.png"><img src="/docs/img/mist-network.png" alt="IoT-to-Cloud (Nebula) network" style="width:70%;height:70%"/></a><br><br>
Since data is generated and collected at the edge of the network (mist network), it makes sense to change the role of connected Things and provide network accessible (_Public_) services directly on devices. This extends _Cloud_ to the extreme edge and it is a good foothold for robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* Develop _autonomous, intelligent and self-aware devices_ by providing network services directly in the environment of data origin.

## 📌 More than embedded

When we were designing AREG SDK, the guiding principle was to create a framework to develop embedded and high-end applications that intelligently interact at the edge of the network. To keep application design homogeneous we defined multithreading, multiprocessing and internet communication service categories. These services are neither processes, nor tasks managed by the operating system, they are software components with predefined interface, which methods are invoked remotely.
<br><a href="docs/img/areg-services.png"><img src="docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:70%;height:70%"/></a><br>
> 💡 In current version AREG engine handles multithreading (_Local_) and multiprocessing (_Public_) communication. 

The AREG engine forms a fault tolerant system, discovers services and automates communication, simplifies distributed programming, and helps developers to focus on application business logic as if they would program a single process application with one thread where methods of objects are event-driven. The engine guaranties that:
* The crash of one application does not cause the crash of the system.
* The service clients are automatically notified about service availability and connection.
* The requests called by service clients are automatically invoked to be executed on service components.
* The service responses are automatically invoked on the exact client side, and they are not mixed or missed.
* The subscriptions on data change, on response and broadcast are automatically invoked on client side when service triggers a call.

## 📌 Composition

AREG SDK consists of:
1. [Multicast router (_mcrouter_)](./framework/mcrouter/) to use in IPC. It runs either as a service managed by the OS or as a console application.
2. [AREG framework (or engine)](./framework/areg/) is a library (shared or static) linked in every application.
3. [Code generator tool](./tools/) to create client and server base objects from a service prototype document.

The framework contains a dynamic and configurable logging service. More tools and features are planned in next releases.

## 📌 Software build

Get latest sources using `git` or download ZIP files:
```shell
$ git clone https://github.com/aregtech/areg-sdk.git ./
$ cd areg-sdk
```

AREG SDK sources are developed for:
* **Supported OS**: POSIX-compliant OS (list of [POSIX API](./docs/POSIX.md#the-list-of-posix-api-used-in-areg-sdk-including-multicast-message-router)), Windows 8 and higher.
* **Supported CPU**: x86, x86_64, arm and aarch64.
* **Supported compilers**: Version C++17 GCC, g++, clang and MSVC.

> 💡 The other POSIX-compliant OS and compilers are not tested yet.

Compile AREG SDK sources and examples using following tools:
* **On Linux or Windows**: import projects in _Eclipse_ to compile with POSIX API (select preferred Toolchain before compile).
* **On Windows**: open areg-sdk.sln file in _MS Visual Studio_ (VS2019 and higher requiered) to compile with Win32 API.
* **On Linux**: in the terminal call “_make_” as shown below to compile with POSIX API.
```shell
$ make [all] [framework] [examples]
```
For details on how to load and/or compile projects for various compilers and hardware see [HOWTO](./docs/HOWTO.md) document.

## 📌 Integration and development

#### Mulitcast router

Configure [_router.init_](./framework/areg/resources/router.init) file to set IP-address and the port number of the multicast router:
```
connection.address.tcpip    = 127.0.0.1	# the address of mcrouter host
connection.port.tcpip       = 8181      # the connection port of mcrouter
```
In case of multithreading application development the configuration of router.init can be ignored.

#### Logging service

Configure [_log.init_](./framework/areg/resources/log.init) to specify scopes, log priorities and log file name:
```
log.file        = %home%/logs/%appname%_%time%.log # create logs in 'log' subfolder of user home 
scope.mcrouter.*= NOTSET ;                         # disable logs for mcrouter.

scope.my_app.*                   = DEBUG | SCOPE ; # enable all logs of my_app
scope.my_app.ignore_this_scope   = NOTSET ;        # disable logs of certain scopes in my_app
scope.my_app.ignore_this_group_* = NOTSET ;        # disable logs of certain scope group in my_app
```
> 💡 By default, the _router.init_ and _log.init_ files are located in _config_ subfolder of binaries.<br>
> 💡 To enable all logs of all applications, use ```scope.*  = DEBUG | SCOPE ;``` .<br>
> 💡 In current version the logging is possible only in the file.

### Development

The development guidance and examples of codes you'll find in [DEVELOP](./docs/DEVELOP.md) document.

## 📌 Use cases and benefits

AREG SDK can be used in a very large scopes of multithreading or multiprocessing application development running on Linux or Windows machines.

#### Distributed solution

AREG SDK provides a lightweight form of distributed computing where the services can be distributed and run on any node of the network, and the application architects can easily distribute the computing power between threads and processes. The automated service discovery makes service location transparent, so that the applications interact as if the components are located in one process. Developers define a dynamic or static _model_, which is a description of service relationship and is used to load and start services. The following is a demonstration of a static _model_, which is loaded in `int main()` function to start services, and is unloaded to stop services when exits `main()`.
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
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, nullptr, nullptr );

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
In the example, the `"RemoveRegistry"` and the `"SystemShudown"` are the names of components called _roles_, and the `NERemoteRegistry::ServiceName` and the `NESystemShutdown::ServiceName` are the _interface names_. In combination, they define the _service name_, which is used to access in the network. The MACRO create static _model_ `NECommon::ModelName`, which defines services that are started when call method `Application::loadModel(NECommon::ModelName)`. Services are stopped when call `Application::unloadModel(NECommon::ModelName)`.

In this example services can be merged in one thread and in case of the _Public_ services they can be split and distributed in 2 processes, where every process contains its own model to start service. Independent on service location, neither software developers, nor service client objects feel difference except possible slight network latency when run IPC. It is as well possible to instantiate 2 instances of the same service implementation, but they must have unique _role names_ within one system. Means, the _Public_ services must have unique names within a network and the _Local_ services must have unique names within a process. As an example of developing service and client that can be split in multiple processes or split in one thread, see explanation of [Hello Service!](./docs/DEVELOP.md#hello-service) project developer guide.

#### Driverless devices

Normally, the devices are supplied with the drivers to install in the system and with the header files to integrate in the application(s). The drivers often run in Kernel mode, where the crash of the driver crashes the entire system. Driver development requires special technique, which is different for each operating system and it is hard to debug. 
<br><a href="/docs/img/driver-solution.png"><img src="/docs/img/driver-solution.png" alt="Kkernel-mode driver solution" style="width:70%;height:70%"/></a><br>
Our proposal is to deliver driverless service enabled devices, where device specific services are described in the interface prototype documents. 
<br><a href="/docs/img/driverless-solution.png"><img src="/docs/img/driverless-solution.png" alt="AREG SDK driverless solution" style="width:70%;height:70%"/></a><br>
In contrast to drivers, the service development does not differ from any user mode application development, it is faster to develop, easily serves multiple applications (service clients), contains less risks and requires less development resources. The service client base objects are generated from supplied prototype documents, used by developers to communicate with service and develop application specific business logic.

#### Real-time solutions

When a remote method of the service interface is called, the engine of AREG SDK immediately generates and delivers messages to the target component, which invokes appropriate methods of addressed service. This makes communication real-time with ultra-low networking latency. Such solutions are highly required to develop time-sensitive applications for automotive, flock of drones, medtech, real-time manufacturing, real-time monitoring and other projects.
<br><a href="/docs/img/areg-sdk-features.png"><img src="/docs/img/areg-sdk-features.png" alt="AREG SDK and multicast features" style="width:70%;height:70%"/></a><br>

#### Digital twin

Often, the digital twin applications use client-server architecture, where the middleware server collects the data of external devices and the UI application virtualizes them. In such solutions devices interact either through server or UI client application. The event-driven architecture and the real-time communication of AREG SDK is a perfect solution to develop digital twin applications that virtualize, monitor and control external devices, and immediately react to environment or device state change in real-time mode. External devices may communicate without additional layer, which is an important factor for emergency, security and safety cases.

#### Simulation and test automations

When hardware provisioning to all employees is impossible, testing and checking unexpected phenomena of rapidly changing software in a simulated environment can be the most rational solution. If unit tests are used by developers to test a small portion of code and they may contain bugs, the simulation is used by developers and testers to check the system functionality and stability. Simulations are portable and accessible to everyone, help to optimize solutions and avoid unnecessary risks. Projects using simulations are better prepared for remote work and easier to outsource.
<br><a href="/docs/img/software-layers.png"><img src="/docs/img/software-layers.png" alt="Software application 4 layers" style="width:70%;height:70%"/></a><br>
The software components in applications normally are split in Data, Controller, Business and the optional Presentation layers. Distributed and service oriented solution of the AREG engine eases to test the system in simulated environment, where the Simulation application provides implementation of Data layer services, so that the rest of application can be tested without any change.

The same technique of simulating data can be used to create API driven test automations.

## 📌 Examples

We provide various [examples](./examples/) to demonstrate features of the AREG framework. The examples are listed in the [examples/README.md](./examples/README.md) document.

## 📌 Licensing
 
AREG SDK is dual-licensed under free open source license (Apache version 2 license) and commercial license, which gives the commercial support, full rights to create and distribute software without open source license obligations. For licensing details see [LICENSE](./LICENSE.txt) document.
 
For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) website or contact _info[at]aregtech.com_.

## 📌 Call for action!

Do you like the project, have more ideas or need features? You are welcomed to become code distributor or request features. Please share the project with your connections on [![Twitter](https://img.shields.io/twitter/url?label=Twitter&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk)](https://twitter.com/intent/tweet?text=Wow:&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk) and other social media platforms. [![Star the project](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=Star%20the%20project)](https://github.com/aregtech/areg-sdk/) on GitHub to inspire us, because we'll feel useful and motivated to develop more!

**Follow us**

[![Follow us](https://img.shields.io/twitter/follow/aregtech.svg?style=social)](https://twitter.com/intent/follow?screen_name=aregtech) [![Follow us](https://img.shields.io/badge/LinkedIn-Aregtech-blue??style=flat&logo=linkedin&logoColor=b0c0c0&labelColor=363D44)](https://www.linkedin.com/company/aregtech/)

![Viewers](https://gpvc.arturio.dev/aregtech)

<!-- markdownlint-enable -->
