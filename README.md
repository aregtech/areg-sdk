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
<img src="/docs/img/mist-network.png" alt="IoT-to-Cloud (Nebula) network, mist network, mist computing" style="width:75%;height:75%" align-"center"/>
<br>
<br>
Since data is generated and collected at the edge of the network (mist network), it makes sense to change the role of connected Things and provide network accessible (_Public_) services directly on devices. This is a good foothold for robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* Develop _autonomous, intelligent and self-aware devices_ by providing network services directly in the environment of data origin.

## More than embedded

When we were designing AREG SDK, the guiding principle was to create a framework to develop applications for embedded and high-end systems that intelligently interact at the edge of the network. To keep application design homogeneous and escape complexity of multiple communication layers, we defined _types of services_ for multi-threading, multi-processing and internet communication. These services are neither processes, nor tasks managed by the operating system, they are software components with predefined interface, which methods are invoked remotely.
<br><img src="docs/img/areg-services.png" alt="AREG SDK distributed services" style="width:75%;height:75%"/><br>
> 💡 The current implementation of AREG engine intelligently handles and distributes messages for multi-threading (_Local_ services) and multi-processing (_Public_ services) communication. 

The automations of AREG SDK engine simplify distributed and multi-processing programming, and help developers to focus on application business logic as they would program a single process application with one thread where methods of objects are event-driven. The engine forms fault tolerant and scalable system of distributed services, it automatically discovers services for the clients, delivers and dispatches messhages, and  guarantees that:
* The requests called by client objects are automatically executed on server components.
* The responses sent by server component are automatically invoked on exact client side and they are not mixed or missed.
* On subscription, the engine automatically delivers data to subscriber without additional interaction.

## Composition

AREG SDK consists:
# [Multi-cast router (_mcrouter_)](./framework/mcrouter/), which is used for inter-process communication (_IPC_), should be only one instance per network, and can run either as a service managed by the operatting system or as a console application.
# [AREG framework (or engine)](./framework/areg/), which is a library (static or shared) and should be used in every application.
# [Code generator tool](./tools/), which generates client and server bases objects from service interface document.
# Integrated logging service that logs scopes and messages, can filter logs by priority and scopes.

With each major release there are more tools and features are already planned to bring to the market.

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

Configure [_router.init_](./framework/areg/resources/router.init) file, make sure that it is located in _./config_ subfolder of your binary location. In most of the cases you would need to modify only these parameter. Set the IP-address and port number of the machine where multi-cast router (_mcrouter_) is running and the port that is listening. Make sure that _mcrouter_ and the application have same configuration.
```
connection.address.tcpip    = 127.0.0.1	# the address of service mcrouter host
connection.port.tcpip       = 8181      # service mcrouter connection port
```
You may ignore multi-cast router and the configuration if you only develop multi-threadin application.

### Logging service

Configure [_log.init_](./framework/areg/resources/log.init) file, make sure that is is located in the _./config_ subfolder of your binary location. The configuration of logging is realitevely well document, for details see the document description. In most of the cases you would need to use the following settings:
```
log.enable = true     # <== Enable logging globaly
log.file   = ./logs/%appname%_%time%.log  # <== the mask of logging file. Used app name and timestamp.

# #######################################
# Application(s) Scopes
# #######################################

scope.mcrouter.*    = NOTSET ;  # disable logs for the entire mcrouter.

scope.my_app.*                   = DEBUG | SCOPE ; # my_app logs everything
scope.my_app.ignore_this_scope   = NOTSET ;        # my_app disables log of certain scopes
scope.my_app.ignore_this_group_* = NOTSET ;        # my_app disables log of certain scope group

scope.another_app.log_only_warn_and_errors = WARN | SCOPE ;  # another_app enables warnings of certain scope
scope.another_app.do_not_log_scopes        = WARN ;          # another_app enables warnings and disables scope logs
```

## Benefits
The major features of AREG SDK to benefit:
* The combination of request-response, broadcast and subscription services.
* Transparency of service location, which also eases simulation development and test automation.
* Automated service discovery, automated messaging and dispatching, featured logging and code generator.

## Development

### Design Service Interface
The service interface document is an XML structured file.
```xml
    <MethodList>
        <Method ID="7" MethodType="request" Name="HelloWorld" Response="HelloWorld">
            <Description>Request to print hello world</Description>
            <ParamList>
                <Parameter DataType="String" ID="8" Name="roleName">
                    <Description>The role name of client component that requested to print hello world</Description>
                </Parameter>
                <Parameter DataType="String" ID="9" Name="addMessage">
                    <Value IsDefault="true">""</Value>
                    <Description>Additional message to output. Can be empty.</Description>
                </Parameter>
            </ParamList>
        </Method>
...
    </MethodList>
```

## Examples

We provide various [examples](./examples/) to demonstrate features of the framework. The examples are listed in the [examples/README.md](./examples/README.md) document.

## Licensing
 
AREG SDK is dual-licensed under free open source (Apache version 2 license) and commercial licenses that gives the commercial support, full rights to create and distribute software without open source license obligations. For licensing details see [LICENSE](./LICENSE.txt) document.
 
For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) web site or contact _info[at]aregtech.com_.

## Inspire us!

Did you like the project? Please share the project with your connections on [![Twitter](https://img.shields.io/twitter/url?label=Twitter&style=social&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk)](https://twitter.com/intent/tweet?text=Wow:&url=https%3A%2F%2Fgithub.com%2Faregtech%2Fareg-sdk) and [![Star us](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=Star%20us)](https://github.com/aregtech/areg-sdk/) on GitHub to inspire us! You can as well become a distributor or suggest features.

[![Follow us](https://img.shields.io/twitter/follow/aregtech.svg?style=social)](https://twitter.com/intent/follow?screen_name=aregtech) [![Follow us](https://img.shields.io/badge/LinkedIn-Follow%20us-blue)](https://www.linkedin.com/company/aregtech/)

<!-- markdownlint-enable -->
