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
<a href="/docs/img/mist-network.png"><img src="/docs/img/mist-network.png" alt="IoT-to-Cloud (Nebula) network, mist network, mist computing" style="width:50%;height:50%"/></a>
<br>
<br>
Since data is generated and collected at the edge of the network (mist network), it makes sense to change the role of connected Things and provide network accessible (_Public_) services directly on devices. This is a good foothold for robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* Develop _autonomous, intelligent and self-aware devices_ by providing network services directly in the environment of data origin.

## More than embedded

When we were designing AREG SDK, the guiding principle was to create a framework embedded and high-end applications that intelligently interact at the edge of the network. To keep application design homogeneous we defined multi-threading (_local_), multi-processing (_Public_) and internet (_Internet_) communication service types. These services are neither processes, nor tasks managed by the operating system, they are software components with predefined interface, which methods are invoked remotely.
<br><a href="docs/img/areg-services.png"><img src="docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:50%;height:50%"/></a><br>
> 💡 Currently AREG engine handles multi-threading (_Local_ services) and multi-processing (_Public_ services) communication. 

The existing automations simplify distributed programming, and help developers to focus on application business logic as they would program a single process application with one thread where methods of objects are event-driven. The AREG engine
* Forms fault tolerant and scalable system of distributed services.
* Automatically discovers services, delivers and dispatches messhages.
* Guaranties the requests called by client objects are automatically executed on server components.
* Guaranties the responses sent by server objects are automatically invoked on exact client side, they are not mixed or missed.
* Guaranties that on data, response and broadcast subscription, appropriate methods are invoked without additional interaction.

## Composition

AREG SDK consists:
1. [Multi-cast router (_mcrouter_)](./framework/mcrouter/) is used for IPC. Runs either as a service managed by OS or as a console application.
2. [AREG framework (or engine)](./framework/areg/) is a shared or static library used in every application.
3. [Code generator tool](./tools/) to create client and server bases objects from service interface XML-document.

The framework as well contains integrated logging service that can be configured. More tools and features are already planned to bring to the market.

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
For detailed instructions to load and/or compile projects see [HOWTO](./docs/HOWTO.md) document.

## Integration

### Mulit-cast router

Configure [_router.init_](./framework/areg/resources/router.init) file located in _./config_ subfolder of binary location. In most of the cases only IP-address and the port number of the multi-cast router should be changed:
```
connection.address.tcpip    = 127.0.0.1	# the address of mcrouter host
connection.port.tcpip       = 8181      # the connection port of mcrouter
```
Ignore multi-cast router if develop only multi-threadin application.

### Logging service

Configure [_log.init_](./framework/areg/resources/log.init) file located in the _./config_ subfolder of binary location. The configuration file contains well document description of parameters. In most of the cases only scopes and priorities are modified. Some common parameters:
```
log.enable = true                         # <== Enable / disable logging globaly
log.file   = ./logs/%appname%_%time%.log  # <== Logging file mask. Used app name and timestamp.

# #######################################
# Application(s) Scopes
# #######################################

scope.mcrouter.*    = NOTSET ;  # disable logs for mcrouter.

scope.my_app.*                   = DEBUG | SCOPE ; # enable all logs of my_app
scope.my_app.ignore_this_scope   = NOTSET ;        # disable logs of certain scopes in my_app
scope.my_app.ignore_this_group_* = NOTSET ;        # disable logs of certain scope group in my_app
```
> 💡 To enable all logs of all applications, use **scope.*** **= DEBUG | SCOPE** .<br>
> 💡 In current version the logging is possible only in the file.

## Development

For details of how to create Service Interface document, how to run code generator and how to write application, please see [develop.md](./docs/develop.md).

## Use cases and benefits

AREG SDK can be used practically in every application that are developed as multi-threading or multi-processing running on Linux or Windows machines.

### Event-driven

The majority of applications are event-driven, when the components are reacting and states are changed by certain events. AREG engine is 100% event-driven, there is no need manually to poll messages from TCP stack and dispatch them by data type as it is required in many other solutions.

### Fault tolerant

There is no need of application start and shutdown priority to run services and establish connections as it is required in many other solutions. The engine provides 100% fault tolerant system and the automatic service discovery makes sure that clients are automatically notified when waiting service is available in the system.

### Distributed

AREG SDK is a lightweight form of distributed computing where the predefined services can run on any node on the accessible network. The system architect may easily define services running on one or many processes. 

In this example, 2 services and one client run in one process and one thread:
```cpp
// Describe mode, set model name
BEGIN_MODEL(NECommon::ModelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestMainServiceThread" )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::MainService, MainServiceComponent )
            
            // register RemoteRegistry, SystemShutdown service implementation and the dependency.
            REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( NESystemShutdown::ServiceName, NESystemShutdown::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::LocalService)

        // end of component description
        END_REGISTER_COMPONENT( NECommon::MainService )

// end of model NECommon::ModelName
END_MODEL(NECommon::ModelName)
```
In this example, 2 services run in 2 threads and each of them as well has a client:
```cpp
// Describe mode, set model name
BEGIN_MODEL(NECommon::ModelName)

    // define first  thread
    BEGIN_REGISTER_THREAD( "Thread1" )

        // define first component
        BEGIN_REGISTER_COMPONENT( "component1", OneServiceComponent )

            // register RemoteRegistry service implementation and the dependency.
            REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::LocalService)

        // end of component description
        END_REGISTER_COMPONENT( "component1" )

    // define second thread
    BEGIN_REGISTER_THREAD( "Thread2" )

        // define second component
        BEGIN_REGISTER_COMPONENT( "component2", TwoServiceComponent )

            // register SystemShutdown service implementation and the dependency.
            REGISTER_IMPLEMENT_SERVICE( NESystemShutdown::ServiceName, NESystemShutdown::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::LocalService)

        // end of component description
        END_REGISTER_COMPONENT( "component2" )

// end of model NECommon::ModelName
END_MODEL(NECommon::ModelName)
```

The major features of AREG SDK to benefit:
* The combination of request-response, broadcast and subscription services.
* Transparency of service location, which also eases simulation development and test automation.
* Automated service discovery, automated messaging and dispatching, featured logging and code generator.

## Examples

We provide various [examples](./examples/) to demonstrate features of the framework. The examples are listed in the [examples/README.md](./examples/README.md) document.

## Licensing
 
AREG SDK is dual-licensed under free open source (Apache version 2 license) and commercial licenses that gives the commercial support, full rights to create and distribute software without open source license obligations. For licensing details see [LICENSE](./LICENSE.txt) document.
 
For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) web site or contact _info[at]aregtech.com_.

## Inspire us!

Did you like the project? Please share the project with your connections on [![Twitter](https://img.shields.io/twitter/url?label=Twitter&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk)](https://twitter.com/intent/tweet?text=Wow:&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk) and [![Star us](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=Star%20us)](https://github.com/aregtech/areg-sdk/) on GitHub to inspire us! You can as well become a distributor or suggest features.

[![Follow us](https://img.shields.io/twitter/follow/aregtech.svg?style=social)](https://twitter.com/intent/follow?screen_name=aregtech) [![Follow us](https://img.shields.io/badge/LinkedIn-Follow%20us-blue)](https://www.linkedin.com/company/aregtech/)

<!-- markdownlint-enable -->
