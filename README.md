# AREG IoT SDK to simplify multitasking programming

**_Bring your product to live service enabled_**


## Introduction

**AREG SDK** is a lightweight _real-time asynchronous and automated messaging engine_ to enable [_mist computing_](https://csrc.nist.gov/publications/detail/sp/500-325/final) at the edge of the network, where devices connect to IoT fog network and provide device specific services, as if they act as a thin servers.

## More than embedded

When we were designing AREG SDK, the guiding principle was to create a framework to develop embedded applications that intelligently interact at the edge of the IoT network. To keep application design homogeneous, we defined Local services for inter-thread and the Public services for inter-process communications. As a base of IPC we use Object Remote Procedure Call (_ORPC_) protocol and multicast routing (MCR) network communication model, and combine features of client-server (request-reply) and subscriber-provider (publisher-subscriber) models. In the MCR network the engine of AREG Framework forms logical connections of software nodes, where every node can be a mixture of logical servers and clients, which provide or use service(s). This approach creates a fault tolerant system and scalable mesh of services running directly on connected Things.


## Get rid of tedious complexity

Complex multiprocessing projects may suffer from mixed client-server complex networks, may have problems due to dependencies of start and shutdown priority of applications, may produce bugs when manually messaging and dispatching. The automation and intelligence of AREG SDK engine easily handles and simplifies any such complexity, and helps developers to focus on application business logic as they would program a single process application with one thread where methods of objects are event-driven.

## Benefits

The major features of AREG SDK to benefit:
* A combination of request-response and broadcast communication, and subscription service.
* Transparency of service location, which also eases the creation of a simulation and a test automation.
* Automated service discovery, automated messaging and dispatching, featured logging and code generator.


## Autonomous device with services

Traditionally, IoT devices are connected clients of cloud or fog servers to stream data from sensors for further processing. Since data is generated and collected at devices, it makes sense to provide network accessible (Public) services directly on Things. Such a concept changes the role of connected devices and is a good foothold to provide robust solutions such as:
* _Increased data privacy_, which is an important factor for sensitive data. 
* _Decreased data streaming_, which is a fundamental condition to optimize network communication. 
* _Autonomous, intelligent and self-aware_ device development by providing network services directly in the environment of data origin. 


## Software build

AREG SDK can be used in Linux (Embedded Linux) and Windows machines running on x86, amd64, arm 32- and 64-bit processors. The SDK and example codes can be compiled by using _Makefile_, _Eclipse for C/C++ Developer_ and _Visual Studio_. AREG SDK consists of [AREG framework library](./framework/areg/) and [Multicast Router](./framework/msrouter).

#### make

_make_ and _Makefile_ in AREG SDK are used to build software with _POSIX_ API. Call _make_ in command line with or without argument:
```
$ make [all] [framework] [examples]
```
#### Eclipse
In _Eclipse for C/C++ Developer_ IDE import AREG SDK framework and examples to build software with _POSIX_ API.

#### Microsoft Visual Studio
In Microsoft Visual Studio IDE (MS VC 2012 or higher), open _areg-sdk.sln_ file. If Visual Studio offers to convert project files, ignore. **In Visual Studio toolbar, select appropriate _Solution Configuration_ and _PlatformToolset_** and compile projects.

For details on how to import projects, build and integrate framework, run examples, configure logging and multicast routing, please refer to [HOWTO.md](./docs/HOWTO.md) document.

## Examples

The AREG SDK contains various [examples](./examples/) to demonstrate features of the framework. The examples are listed in the [examples/README.md](./examples/README.md) document.


## Licensing
 
AREG SDK is _dual-licensed under free open source and commercial licenses_. The free open source license allows R&D, startups, freelancers and individuals to use SDK without costs. The commercial license gives the commercial support, full rights to create and distribute software without open source license obligations. For more details of licensing see [LICENSE.txt](./LICENSE.txt) file.
 
For license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) web site or contact _info[at]aregtec.com_.
