# AREG IoT SDK to simplify multitasking programming

**_Bring your product to live service enabled_**


## Introduction

**AREG IoT SDK** (or **AREG SDK**) is an Object Remote Procedure Call (_ORPC_) based, lightweight _real-time asynchronous communication framework written in C++_ that simplifies multitasking and service enabled applications development for embedded and high-end machines. AREG SDK enables _mist computing_ at the edge of the network, where devices connect to IoT fog network not only as traditional clients, but as thin servers and provide device specific services.


## More than embedded

When we were designing AREG SDK, the guiding principle was to create a framework to develop embedded applications that intelligently interact at the edge of the IoT network. To keep application design homogeneous, there are defined Local services for inter-thread and the Public services for inter-process communications. As a base of IPC we use multicast routing (MCR) network communication model to have combined features of client-server and subscriber-provider (publisher-subscriber) models. In MCR network client-server nodes have logical connections and every node can be a mixture of logical servers and clients. This approach helps to create a fault tolerant, scalable and distributed system of meshed services running directly on connected Things.


## Benefits

AREG framework features to benefit projects:
* A combination of request-response and broadcast communication, and subscription service.
* Transparency of service location, which also eases the creation of a simulation and a test automation.
* Automated service discovery, automated messaging, dispatching, code generator and featured logging.


## Challenges to solve

Traditionally, IoT devices are connected clients of cloud or fog servers to stream data from sensors for further processing. Since data is generated and collected at devices, it makes sense to provide network accessible (public) services directly on Things. Such a concept changes the role of connected devices and is a good foothold to provide robust solutions such as:
* _Increased data privacy_, which is an important factor for sensitive data. 
* _Decreased data streaming_, which is a fundamental condition to optimize network communication. 
* _Autonomous, intelligent and self-aware_ devices development by providing network services directly in the environment of data origin. 


## Building software

AREG SDK provides _Eclipse for C/C++ Developer_ project files for **POSIX**, and _Visual Studio_ project files for **Windows** versions of the framework and examples. _The default settings of provided project files are enough to compile all_.

For details on how to import projects, build and integrate framework, run examples, configure logging and multicast routing, please refer to [HOWTO.md](./docs/HOWTO.md) document.

## Examples

The AREG SDK contains various [examples](./examples/) to demonstrate features of the framework. The examples are listed in the [examples/README.md](./examples/README.md) document.


## Licensing
 
AREG SDK is _dual-licensed under commercial and free open source licenses_. The commercial license gives the commercial support, full rights to create and distribute software without open source license obligations. For more details of licensing see [LICENSE.txt](./LICENSE.txt) file.
 
For license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) web site or contact _info[at]aregtec.com_.
