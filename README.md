# AREG IoT SDK to simplify multitasking programming

**_Bring your product to live service enabled_**

## Introduction

**AREG IoT SDK** (or **AREG SDK**) is an Object Remote Procedure Call (_ORPC_) based, lightweight _real-time asynchronous communication framework written in C++_ that simplifies multitasking and service enabled applications development for embedded and/or high-end machines. AREG SDK enables _mist computing_ at the edge of the network, where devices connect to fog not only as traditional clients, but as thin servers and provide services to execute device specific or a narrow range of tasks.

AREG is used in multithreading and multiprocessing projects where connected Things need to communicate in real-time mode. 


## Inter-process communication

The IPC of AREG framework is based on Multicast Routing (MCR) network communication model that has combined features of client-server and publish-subscribe models. In MCR client-server nodes have logical connections and all messages pass via router. Every application in the network can provide multiple logical nodes where every node can be a mixture of logical servers and clients. This approach helps to create a fault tolerant, scalable and distributed system of meshed services running directly on connected Things.


## Development benefits

Software developers can benefit by using AREG framework:
* The full automations of messaging and dispatching, and code generator help to focus on business logic.
* The transparency of service location and modular design ease to simulate data layer to test the rest of the codes.
* Featured logging scopes allow to filter special logs and priorities without recompiling the code.

For the examples of generated codes, logs and modular design, see [_examples_](#examples) chapter.

## Challenges and solutions

Traditionally, IoT devices are connected clients of cloud or fog servers to stream data from sensors for further processing. Since data is generated and collected at devices, it makes sense to provide network accessible (public) services directly on Things. Such a concept changes the role of connected devices and primarily solves the following challenges:
* It significantly _increases data privacy_, which is a very important factor for sensitive data. 
* It _decreases data streaming_, which is a fundamental condition to optimize network communication. 
* It renders devices _more autonomous and more intelligent_ by providing network services directly in the environment of data origin. 


## More than embedded

Embedded applications developed with the help of _AREG framework_ do not need to adapt additional IoT mist enabling layers to increase the visibility in the network. Current 2 types of services (local and public) help to keep the design and architecture of the entire software homogeneous, where Local services are used in multithreading and the Public services are used in multi-processing communication.

The guiding principles of AREG framework are:
* The network of connected devices provides information, instead of simply data.
* The network delivers only information that has been requested, and when it has been requested.
* Devices create a dynamic system that works together using a subscriber-provider model.
* Device applications are service providers that are accessible in the network to execute device specific tasks.
* Devices adapt to network configuration, dynamically discover data providers and available services.

This concept of system architecture reduces the amount of streaming data in the network that has a direct impact on energy consumption, and the connected Things directly communicate with each other, remaining autonomous and becoming an intelligent extension of the fog network.


## Compilation

AREG SDK provides _Eclipse for C/C++ Developer_ project files to compile **POSIX** version of the framework and examples, and _Visual Studio_ project files to compile **Windows** version of the framework and examples. Import Eclipse projects or open Visual Studio solution file to compile. _The projects default settings should be enough to compile all_.

- _POSIX_ compilation requires **POSIX** preprocessor directive. More details about _POSIX_ and API are in [POSIX.md](./docs/POSIX.md). 
- _Windows_ compilation requires **WINDOWS** preprocessor directive. More details about _Winodws_ are in [WIN32.md](./docs/WIN32.md).
- The detailed instructions about importing projects and integration in other projects are in [HOWTO.md](./docs/HOWTO.md).

## Log and IPC enabling

The AREG framework has default settings to enable logging and IPC. However, use _.init_ files to set the right settings. If after compilation the binary output folder does not contains _‘config’_ subfolder with _.init_ files, create _’config’_ subfolder in output folder and:
* **For logs:** copy [log.init](./framework/areg/resources/log.init) file located in [framework/areg/resources](./framework/areg/resources/).
* **For IPC :** copy [router.init](./framework/areg/resources/router.init) file located in [framework/areg/resources](./framework/areg/resources/).

For changing settings options see the description in the _.init_ files.


## Examples

The AREG SDK contains various [examples](./examples/) to demonstrate features of the framework. The examples are listed and described in the [examples/README.md](./examples/README.md) file. Please read the details.

## Documents

Read AREG SDK document structure in [docs/README.md](./docs/README.md).

## Licensing
 
AREG SDK is _dual-licensed under commercial and free open source licenses_. The commercial license gives the commercial support, full rights to create and distribute software on own terms without any open source license obligations. For more details of licensing see [LICENSE.txt](./LICENSE.txt) file.
 
For license, support or additional information, please visit [Aregtech](https://www.aregtech.com/) web site or contact by email _info[at]aregtec.com_.
