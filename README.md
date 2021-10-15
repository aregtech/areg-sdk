<h1 align="center" style="display: block; font-size: 2.5em; font-weight: bold; margin-block-start: 1em; margin-block-end: 1em;">
<a name="logo" href="https://www.aregtech.com"><img align="center" src="/docs/img/areg-sdk-1280x360px-logo.png" alt="AREG SDK Home" style="width:100%;height:100%"/></a>
  <br><br><b>AREG SDK</b>
</h1>

> 💡 By ![Staring us](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=Staring%20us) on GitHub and spreading AREG SDK link on social media, you help and inspire us to provide featured technology that helps to cut costs and create service enabled product!

<!-- markdownlint-disable -->
## Project status
<table class="no-border">
  <tr>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/c-cpp.yml/badge.svg" alt="C++ compiltation"/></td>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/msbuild.yml/badge.svg" alt="MS Build"/></td>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/codeql-analysis.yml/badge.svg" atl="CodeQL"/></td>
    <td><img src="https://github.com/aregtech/areg-sdk/actions/workflows/linter.yml/badge.svg" alt="Super-Linter"/></td>
  </tr>
  <tr>
    <td><img src="https://img.shields.io/github/v/release/aregtech/areg-sdk?label=%20%F0%9F%93%A3%20Release&style=flat&logoColor=b0c0c0&labelColor=363D44" alt="Latest release"/></td>
    <td><img src="https://img.shields.io/badge/Solution-C++17-blue.svg?style=flat&logo=c%2B%2B&logoColor=b0c0c0&labelColor=363D44" alt="C++ solution"/></td>
    <td><img src="https://img.shields.io/badge/OS-linux%20%7C%20windows-blue??style=flat&logo=Linux&logoColor=b0c0c0&labelColor=363D44" alt="Operating systems"/></td>
    <td><img src="https://img.shields.io/badge/CPU-x86%20%7C%20x86__64%20%7C%20arm%20%7C%20aarch6-blue?style=flat&logo=amd&logoColor=b0c0c0&labelColor=363D44" alt="CPU Architect"/></td>
  </tr>
</table>

## Introduction

**AREG SDK** is an Interface centric lightweight real-time asynchronous communication engine to provide IPC and enable [mist computing](https://csrc.nist.gov/publications/detail/sp/500-325/final), where Things interact and provide services, as if they act like thin distributed servers.

## Motivation

Traditionally, devices are connected clients to stream data to the cloud or fog servers for further processing.
<br>
<a href="/docs/img/mist-network.png"><img src="/docs/img/mist-network.png" alt="IoT-to-Cloud (Nebula) network, mist network, mist computing" style="width:70%;height:70%"/></a>
<br>
<br>
Since data is generated and collected at the edge of the network (mist network), it makes sense to change the role of connected Things and provide network accessible (_Public_) services directly on devices. This extends _Cloud_ to the extreme edge and is a good foothold for robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* Develop _autonomous, intelligent and self-aware devices_ by providing network services directly in the environment of data origin.

## More than embedded

When we were designing AREG SDK, the guiding principle was to create a framework to develop embedded and high-end applications that intelligently interact at the edge of the network. To keep application design homogeneous we defined multi-threading, multi-processing and internet communication service types. These services are neither processes, nor tasks managed by the operating system, they are software components with predefined interface, which methods are invoked remotely.
<br><a href="docs/img/areg-services.png"><img src="docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:70%;height:70%"/></a><br>
> 💡 Currently AREG engine handles only multithreading (_Local_ services) and multiprocessing (_Public_ services) communication. 

The implemented engine forms a fault tolerant system, automates communication, simplifies distributed programming, and helps developers to focus on application business logic as they would program a single process application with one thread where methods of objects are event-driven. The AREG engine guaranties that:
* The crash of one application does not cause a crash of another.
* The service users (clients) are automatically notified about service availability.
* The requests called by client objects are automatically executed on service.
* The sent responses are automatically invoked on the exact client side, they are not mixed or missed.
* The subscriptions on data, response and broadcast are invoked on client side without additional interaction.

## Composition

AREG SDK consists of:
1. [Multicast router (_mcrouter_)](./framework/mcrouter/) to use in IPC. It runs either as a service managed by the OS or as a console application.
2. [AREG framework (or engine)](./framework/areg/) is a shared or static library linked in every application.
3. [Code generator tool](./tools/) to create client and server base objects from service interface XML-document.

The framework contains an integrated logging service that can be configured. More tools and features are planed for next releases.

## Software build

AREG SDK sources are developed to compile:
* **Supported OS**: POSIX-compliant OS (list of [POSIX API](./docs/POSIX.md#the-list-of-posix-api-used-in-areg-sdk-including-multicast-message-router)), Windows 8 and higher.
* **Supported CPU**: x86, x86_64 / amd64, arm and aarch64.
* **Supported compilers**: Version C++17 GCC, g++, clang and MSVC.

> 💡 The other POSIX-compliant OS and compilers are not tested yet.

Compile AREG SDK sources and examples using following tools:
* **On Linux or Windows**: import projects in _Eclipse_ to compile with POSIX (you would probably need to change Toolchain).
* **On Windows**: open areg-sdk.sln file in _MS Visual Studio_ to compile with Win32.
* **On Linux**: in the terminal call “_make_” as shown below to compile with POSIX.
```shell
$ make [all] [framework] [examples]
```
For details to load and/or compile projects see [HOWTO](./docs/HOWTO.md) document.

## Integration and development

#### Mulitcast router

Configure [_router.init_](./framework/areg/resources/router.init) file to set IP-address and the port number of the multicast router:
```
connection.address.tcpip    = 127.0.0.1	# the address of mcrouter host
connection.port.tcpip       = 8181      # the connection port of mcrouter
```
In case of multi-threading applications the configuration of router.init can be ignored.

#### Logging service

Configure [_log.init_](./framework/areg/resources/log.init) to specify scopes, log priorities and log file name:
```
log.file        = %home%/logs/%appname%_%time%.log # create logs in 'log' subsolfder of user home 
scope.mcrouter.*= NOTSET ;                         # disable logs for mcrouter.

scope.my_app.*                   = DEBUG | SCOPE ; # enable all logs of my_app
scope.my_app.ignore_this_scope   = NOTSET ;        # disable logs of certain scopes in my_app
scope.my_app.ignore_this_group_* = NOTSET ;        # disable logs of certain scope group in my_app
```
> 💡 By default, the _router.init_ and _log.init_ files are located in _config_ subfolder of binaries.<br>
> 💡 To enable all logs of all applications, use ```scope.*  = DEBUG | SCOPE ;``` .<br>
> 💡 In current version the logging is possible only in the file.

### Development

The development guidance and examples of codes you'll find in [develop.md](./docs/develop.md).

## Use cases and benefits

AREG SDK can be used in a very large scopes of multithreading or multiprocessing application development running on Linux or Windows machines.

#### Distributed solution

AREG SDK is a lightweight form of distributed computing where the services can run on any node in the network, and the application architects can easily decide how to distribute the computing power. The automated service discovery makes service location transparent, so that the applications interact as if the components are located in the same process and even in the same thread.

This example shows the definition of 2 services in separate threads that can be very easily merged in one thread and in case of _Public_ services easily split in 2 processes. Independent of these service location, neither software developers, nor client objects feel difference except possible slight network latency in case of IPC.
```cpp
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
```
In this example, the ```"RemoveRegistry"``` and the ```"SystemShudown"``` are the names of services (servicing components) called _role names_, and the ```NERemoteRegistry::ServiceName``` and the ```NESystemShutdown::ServiceName``` are the interface names. These MACRO create _model_ that describes the relation of the objects, which are instantiated during model loading:
```cpp
Application::loadModel(NECommon::ModelName); // start services.
```
It is as well possible to instantiate 2 instances of the same service implementation, but they must have unique _role names_ within one system. This means, the _Public_ services must have unique names within a single network and the _Local_ services must have unique names within a process.

#### Driverless

Normally, the devices are supplied with the drivers to install in the system and the software components are supplied with the header files to integrate in the other application. There is no need to deliver drivers or header files, because with AREG SDK devices provide device specific services that are described in XML files that can be used to generate all necessary files to implement service clients.

#### Real-time solutions

As soon as a remote method is called, the engine of AREG SDK immediately generates and delivers messages to the target component, which invokes appropriate methods of service interface. This makes communication latency ultra-low and comparable with real-time. The AREG solution can be used in such industries like automotive, flock of drones, medtech, real-time manufacturing, real-time monitoring, etc.

#### Digital twin

In many solutions, the digital twins use client-server architecture, where the server is a middleware component that collects all the states of the external devices connected as clients, and the GUI application is just another client that visualizes the collected data. In such solutions devices interact through server middleware or the UI client application. The event-driven architecture and the real-time communication of AREG SDK is a perfect solution to use in digital twin development where the application monitors and controls external devices, and immediately reacts on state change, and where the devices can communicate directly with each other without any intermediate layer. In case of security and emergency, this is the most optimal and the most secured solution.

#### Simulations and automated tests

Testing rapidly changing software in a risk-free simulated environment to check unexpected phenomena is a good solution when hardware provisioning to all employees is impossible (too expensive, too big, only very few prototypes exist). Simulations are portable and accessible to everyone, help to optimize solutions, avoid unnecessary risks and protect lives. Projects using simulations are better prepared for remote work.

The software components in applications normally are split in Data, Controller, Business and the optional Presentation layers. Distributed, fault tolerant and service oriented solution of the AREG SDK engine eases testing application in simulated environment, where the Simulation application provides implementation of Data layer services, so that the rest of application can be tested without any change. 

## Examples

We provide various [examples](./examples/) to demonstrate features of the framework. The examples are listed in the [examples/README.md](./examples/README.md) document.

## Licensing
 
AREG SDK is dual-licensed under free open source (Apache version 2 license) and commercial licenses that gives the commercial support, full rights to create and distribute software without open source license obligations. For licensing details see [LICENSE](./LICENSE.txt) document.
 
For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) web site or contact _info[at]aregtech.com_.

## Inspire us!

Did you like the project? Please share the project with your connections on [![Twitter](https://img.shields.io/twitter/url?label=Twitter&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk)](https://twitter.com/intent/tweet?text=Wow:&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk) and [![Star us](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=Star%20us)](https://github.com/aregtech/areg-sdk/) on GitHub to inspire us! You can as well become a distributor or suggest features.

[![Follow us](https://img.shields.io/twitter/follow/aregtech.svg?style=social)](https://twitter.com/intent/follow?screen_name=aregtech) [![Follow us](https://img.shields.io/badge/LinkedIn-Follow%20us-blue)](https://www.linkedin.com/company/aregtech/)

<!-- markdownlint-enable -->
