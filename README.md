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

When we were designing AREG SDK, the guiding principle was to create a framework to develop embedded and high-end applications that intelligently interact at the edge of the network. To keep application design homogeneous we defined multi-threading, multi-processing and internet communication service types. These services are neither processes, nor tasks managed by the operating system, they are software components with predefined interface, which methods are invoked remotely.
<br><a href="docs/img/areg-services.png"><img src="docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:50%;height:50%"/></a><br>
> 💡 Currently AREG engine handles multi-threading (_Local_ services) and multi-processing (_Public_ services) communication. The _Internet_ service in our plan. 

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

Configure [_router.init_](./framework/areg/resources/router.init) file located in _./config_ subfolder of binaries. In majority of cases only IP-address and the port number of the multi-cast router should be changed:
```
connection.address.tcpip    = 127.0.0.1	# the address of mcrouter host
connection.port.tcpip       = 8181      # the connection port of mcrouter
```
Ignore multi-cast router if develop only multi-threadin application.

### Logging service

Configure [_log.init_](./framework/areg/resources/log.init) file located in the _./config_ subfolder of binaries. In most of the cases only loging scopes and priorities are modified. The other parameters are described in _log.init_ file.
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

The development guidance and examples of codes you'll find in [develop.md](./docs/develop.md).

## Use cases and benefits

AREG SDK can be used in very large scope of multi-threading or multi-processing application development running on Linux or Windows machines.

### Event-driven

Applications powered by AREG SDK are fully event-driven. There is no need for manual polling to extract and dispatch messages from TCP pool as it is required in many other solutions. The SDK force developers to keep modularity.

### Fault tolerant

AREG engine provides fault tolerant system, where crash of one application does not cause the crash or stuck of others, there is not application start or shutdown priority, the automatic service discovery notifies clients about service availability.

### Distributed

AREG SDK is a lightweight form of distributed computing where the services can run on any computing node in the network, and the application architects can very easy decide how to distributed the computing power.

This examples shows the initialization of multiple services and clients in one process. 

```cpp
// Describe mode, set model name
BEGIN_MODEL(NECommon::ModelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "Thread1" )
        // define component, set name.
        BEGIN_REGISTER_COMPONENT( NECommon::MainService, MainServiceComponent )
            // register RemoteRegistry, SystemShutdown service implementation and the dependency.
            REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( NESystemShutdown::ServiceName, NESystemShutdown::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::LocalService)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::MainService )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::LocalService, LocalServiceComponent )
            // register LocalHelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NELocalHelloWorld::ServiceName, NELocalHelloWorld::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( NECommon::LocalService )
        
    // end of thread description
    END_REGISTER_THREAD( "Thread1" )

// end of model NECommon::ModelName
END_MODEL(NECommon::ModelName)
```

The Public services _NERemoteRegistry::ServiceName_ and _NESystemShutdown::ServiceName_ can be easily split in separate threads or they can run in different processes. Once the model is defined, components and services are initilized and run by simple call:
```cpp
Application::loadModel(NECommon::ModelName);
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
