# AREG SDK to simplify multiprocessing programming

**_Bring your product to live service enabled_**

## Introduction

**AREG SDK** is an Interface centric lightweight real-time asynchronous communication engine to provide IPC and enable [mist computing](https://csrc.nist.gov/publications/detail/sp/500-325/final), where Things interact and provide services, as if they act like thin servers.

## Service enabled devices

Traditionally, devices are connected clients to stream data to the cloud or fog servers for further processing. Since data is generated and collected at the edge of the network, it makes sense to change the role of connected Things and provide network accessible (_Public_) services directly on devices. This is a good foothold for robust solutions such as:
* _Increase data privacy_, which is an important factor for sensitive data.
* _Decrease data streaming_, which is a fundamental condition to optimize network communication.
* Develop _autonomous, intelligent and self-aware devices_ by providing network services directly in the environment of data origin.

## More than embedded

When we were designing AREG SDK, the guiding principle was to create a framework to develop embedded applications that intelligently interact at the edge of the network. To keep application design homogeneous, we defined _Local_ services for multithreading and the _Public_ services for inter-process communications. These services are neither processes, nor tasks managed by the operating system, they are software components with Service Interface(s), which methods are invoked remotely. This technique of remote object communication is well known as Object Remote Procedure Call (_ORPC_).

## Escape complexity of multiprocessing

AREG SDK simplifies the multiprocessing programming and helps developers to focus on application business logic as they would program a single process application with one thread where methods of objects are event-driven. The engine guarantees that the system automatically delivers the exact and the newest data on subscription, and the system triggers the exact request or response method of the exact servicing component (_service provider_ or _service user_), and the messages are neither mixed, nor lost. The fault tolerant and scalable system of meshed services helps to escape application start and shutdown priority dependencies.

## Benefits

The major features of AREG SDK to benefit:
* The combination of request-response, broadcast and subscription services.
* Transparency of service location, which also eases simulation development and test automation.
* Automated service discovery, automated messaging and dispatching, featured logging and code generator.

## Software build

AREG SDK consists of [framework library](./framework/areg/) and [multicast router](./framework/mcrouter/) sources to compile.
* **Supported OS**: POSIX-compliant OS (list of [POSIX API](./docs/POSIX.md#the-list-of-posix-api-used-in-areg-sdk-including-multicast-message-router)), Windows 8 and higher.
* **Supported CPU**: x86, amd64, arm and aarch64.
* **Supported compilers**: Version C++17 GCC, g++, clang and MSVC.

Compile AREG SDK sources and examples using following tools:
* **On Linux or Windows**: import projects in _Eclipse_ to compile with POSIX.
* **On Windows**: open areg-sdk.sln file in _MS Visual Studio_ to compile with Win32.
* **On Linux**: in the terminal call “_make_” as shown below to compile with POSIX.
```
$ make [all] [framework] [examples]
```
For detailed instructions to load and/or compile projects see [HOWTO](./docs/HOWTO.md) document.

## Examples

We provide various [examples](./examples/) to demonstrate features of the framework. The examples are listed in the [examples/README.md](./examples/README.md) document.

## Licensing
 
AREG SDK is dual-licensed under free open source (Apache version 2 license) and commercial licenses that gives the commercial support, full rights to create and distribute software without open source license obligations. For licensing details see [LICENSE](./LICENSE.txt) document.
 
For commercial license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) web site or contact _info[at]aregtech.com_.
