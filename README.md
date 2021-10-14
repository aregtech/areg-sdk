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
<img src="/docs/img/mist-network-1280x640.png" alt="IoT-to-Cloud (Nebula) network, mist network, mist computing" style="width:75%;height:75%"/>
<br>
<br>
Since data is generated and collected at the edge of the network (mist network), it makes sense to change the role of connected Things and provide network accessible (_Public_) services directly on devices. This is a good foothold for robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* Develop _autonomous, intelligent and self-aware devices_ by providing network services directly in the environment of data origin.

## More than embedded

When we were designing AREG SDK, the guiding principle was to create a framework to develop embedded applications that intelligently interact at the edge of the network. To keep application design homogeneous and escape complexity of multiple communication layers, we defined _services_ for multi-threading, distributed and multi-processing, and for internet communication.
<br><br><img src="docs/img/services.png" alt="AREG SDK distributed services" style="width:60%;height:60%"/><br><br>
The designed services are neither processes, nor tasks managed by the operating system, they are software components with Service Interface(s), which methods are invoked remotely (_Object Remote Procedure Call_ / _ORPC_).
> 💡 The current implementation of AREG engine intelligently handles and distributed message for multi-threading (_Local_ services) and multi-processing (_Public_ services) communication. 

## Escape complexity of multiprocessing

AREG SDK simplifies the multiprocessing programming and helps developers to focus on application business logic as they would program a single process application with one thread where methods of objects are event-driven. The engine creates fault tolerant and scalable system of distributed services, it guarantees that the system automatically delivers the exact and the newest data on subscription, and the system triggers the exact request or response method of the exact servicing component (_server_ or _client_), and the messages are neither mixed, nor lost. 
<br><br><img src="/docs/img/service-oriented.png" alt="Service Oriented design" style="width:80%;height:80%"/><br>

## Benefits
The major features of AREG SDK to benefit:
* The combination of request-response, broadcast and subscription services.
* Transparency of service location, which also eases simulation development and test automation.
* Automated service discovery, automated messaging and dispatching, featured logging and code generator.

## Software build

AREG SDK consists of [framework library](./framework/areg/) and [multi-cast router](./framework/mcrouter/) sources to compile.
* **Supported OS**: POSIX-compliant OS (list of [POSIX API](./docs/POSIX.md#the-list-of-posix-api-used-in-areg-sdk-including-multicast-message-router)), Windows 8 and higher.
* **Supported CPU**: x86, amd64, arm and aarch64.
* **Supported compilers**: Version C++17 GCC, g++, clang and MSVC.
> 💡 The other POSIX-compliant OS and compilers are not tested yet.

Compile AREG SDK sources and examples using following tools:
* **On Linux or Windows**: import projects in _Eclipse_ to compile with POSIX.
* **On Windows**: open areg-sdk.sln file in _MS Visual Studio_ to compile with Win32.
* **On Linux**: in the terminal call “_make_” as shown below to compile with POSIX.
```shell
$ make [all] [framework] [examples]
```
For detailed instructions to load and/or compile projects see [HOWTO](./docs/HOWTO.md) document.

## Integration

AREG SDK consts of _mcrounter_ executable and _areg_ library.
* After compilation, _mcrouter_ can be started either as a system service or console application.
* The _areg_ library can be linked with other binaries as a shared or static library.
* Before starting _mcrouter_ or areg powered application, modify _router.init_.
```
connection.type             = tcpip     # right now it supports only 'tcpip' (TCP/IP)
connection.enable.tcpip     = true      # if 'true' the service mcrouter connection is enabled
connection.name.tcpip       = TCPIP     # unique connection name
connection.address.tcpip    = 127.0.0.1	# the address of service mcrouter host
connection.port.tcpip       = 8181      # service mcrouter connection port
```
Akk binaries should have same settings to be able to connect with same multi-cast router.

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
