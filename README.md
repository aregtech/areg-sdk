# AREG IoT SDK to simplify multitasking programming

**_Bring your product to live service enabled_**

## Introduction

**AREG SDK** is a lightweight _real-time asynchronous and automated messaging engine_ to enable [_mist computing_](https://csrc.nist.gov/publications/detail/sp/500-325/final) at the edge of the network, where devices connect to IoT fog network and provide device specific services, as if they act as a thin servers.

## Service enabled autonomous device

Traditionally, IoT devices are connected clients of cloud or fog servers to stream data from sensors for further processing. Since data is generated and collected at devices, it makes sense to provide network accessible (Public) services directly on Things. Such a concept changes the role of connected devices and is a good foothold to provide robust solutions such as:
* _Increased data privacy_, which is an important factor for sensitive data. 
* _Decreased data streaming_, which is a fundamental condition to optimize network communication. 
* _Autonomous, intelligent and self-aware_ device development by providing network services directly in the environment of data origin. 

## More than embedded

When we were designing AREG SDK, the guiding principle was to create a framework to develop embedded applications that intelligently interact at the edge of the network. To keep application design homogeneous, we defined _Local_ services for multithreading and the _Public_ services for inter-process communications. The Object Remote Procedure Call (_ORPC_) protocol and multicast routing (_MCR_) network communication help to combine client-server (request-reply) and subscriber-provider (publisher-subscriber) model features. The engine of AREG Framework forms logical connections of software components, where every node can have a mixed role of logical server(s) and client(s) providing or using service(s). This approach creates a fault tolerant and scalable system with a mesh of services running directly on connected Things.

## Break complexity

Multiprocessing projects may suffer from mixed client-server complex networks, have problems starting or shutting down applications by priorities, produce bugs when manually messaging and dispatching, and have issues when synchronizing data access. The automation and intelligence of AREG SDK engine easily handles and simplifies such complexity, and helps developers to focus on application business logic as they would program a single process application with one thread where methods of objects are event-driven.

## Benefits

The major features of AREG SDK to benefit:
* A combination of request-response, broadcast and subscription services.
* Transparency of service location, which also eases simulation development and test automation.
* Automated service discovery, automated messaging and dispatching, featured logging and code generator.

## Software build

AREG SDK consists of [framework library](./framework/areg/) and [multicast router](./framework/mcrouter/) that can be compiled with various toolchains for various platforms. In addition, compile and try [examples](./examples/).

* **Supported OS:** _POSIX API_ supporting OS (see the list of used [POSIX API](./docs/POSIX.md)), _Windows 7_ and higher.
* **Supported HW architecture:** _x86_, _amd64_, _arm_ and _aarch64_.
* **Supported compilers:** _GCC_, _g++_, _clang_ and _MSVC_

Compile AREG SDK sources and examples using following tools:
- Import Eclipse projects in _Eclipse for C++ Developers_ IDE and compile under Linux or Windows with POSIX API.
- Open _areg-sdk.sln_ file in _Microsoft Visual Studio_ IDE and compile under Windows with Win32 API.
- Call _make_ from command line as shown below and compile under Linux with POSIX API.
```
$ make [all] [framework] [examples]
```
For detailed instructions to load and/or compile projects please see [HOWTO.md](./docs/HOWTO.md) file.

## Examples

We provide various [examples](./examples/) to demonstrate features of the framework. The examples are listed in the [examples/README.md](./examples/README.md) document.

## Licensing
 
AREG SDK is _dual-licensed under free open source and commercial licenses_. The free open source Apache version 2 license allows R&D, startups, small business, non-profit organizations, freelancers and individuals to use sources without costs. The commercial license gives the commercial support, full rights to create and distribute software without open source license obligations. For more details of licensing see [LICENSE.txt](./LICENSE.txt) file.
 
For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) web site or contact _info[at]aregtec.com_.
