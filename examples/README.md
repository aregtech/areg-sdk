
# AREG SDK Examples

```
This document is part of the AREG SDK and describes its examples.
Copyright © 2017-2023, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

## Introduction

This repository contains a variety of example projects designed to demonstrate the capabilities of the AREG SDK. These examples showcase core concepts such as service creation, inter-process communication (IPC), multithreading, service discovery, fault-tolerance, and more. Below is a detailed overview of each example project and its components.


## Quick Build Guide

Follow these steps to get started with AREG SDK:

### 1. Clone the AREG SDK Repository
To clone the AREG SDK repository, run the following command:

```bash
git clone https://github.com/aregtech/areg-sdk.git
```

### 2. Build the AREG SDK with CMake
To build the SDK using CMake, follow these steps:

```bash
cmake -B ./build
cmake --build ./build -j 20
```

### 3. Build with Microsoft Visual Studio
If you're using Microsoft Visual Studio, open the `areg-sdk.sln` solution file in Visual Studio and build the projects. Alternatively, you can use the command line:

```console
MSBuild ./areg-sdk.sln
```

> [!NOTE]
> To include the example projects in the build, make sure the `AREG_EXAMPLES` option is **not** set to `OFF`. If `AREG_EXAMPLES=OFF`, the examples will be excluded from the build.

For more detailed build instructions and available options, refer to the [Build Documentation](./../docs/wiki/BUILD.md).

---

## AREG SDK Examples Overview

The AREG SDK includes a variety of examples that showcase its features, such as multithreading, multiprocessing, synchronization, and service components.

> [!IMPORTANT]
> Examples marked with **IPC** (*Inter-Process Communication*) require running the **mcrouter** to enable inter-process communication during testing. You can start **mcrouter** from the build directory as a console application.

> [!NOTE]
> The startup order of the applications does not affect system behavior. Since the **AREG Framework** supports fault tolerance, the system operates smoothly as long as the Service Provider, Service Consumer, and the Multicast Router (**mcrouter**) are running, regardless of their startup sequence.

### Available Examples:

> [!CAUTION]
> All **xx_generate** projects are generated from Service Interface document files (extension `.siml`) during CMake configuration. In case of Microsoft Visual Studio, the files of **xx_generate** are created via the script, which is triggered as a pre-build event of a `dummy` project with no functionality.

### **[00_helloservice](./00_helloservice/)**

The **[00_helloservice](./00_helloservice/)** project demonstrates the basics of creating service and client objects in multithreading or IPC scenarios by only changing the *Application Model*.

- **Key Features**: Demonstrates single-thread, multi-thread, and multi-process service/client interactions via *model* definition.
- **Sub-projects**:
  - `00_onethread`: Service provider and client in the same thread.
  - `00_twothreads`: Service provider and client in separate threads within the same process.
  - `00_pubservice` and `00_pubclient`: Public Service and Client objects in separate processes, enabling multiple clients to interact with a single service provider.

---

### **[01_hello](./01_hello/)**

The **[01_hello](./01_hello)** project is a basic example that demonstrates how to create a thread.

- **Key Features**: Demonstrates basic example of a thread.
- **Sub-project**:
  - `01_hello`: Starts a thread, which prints "Hello World!", and waits that it completes and exits.

---

### **[02_buffer](./02_buffer/)**

The **[02_buffer](./02_buffer/)** project demonstrates how to use a *Shared Buffer* in a multithreading environment.

- **Key Features**: Showcases data serialization in the memory shared between threads.
- **Sub-project**:
  - `02_buffer`: Serializes a data in the thread, starts a thread and passes the *Shared Buffer* to the thread, which then deserializes data and outputs on console.

---

### **[03_file](./03_file/)**

The **[03_file](./03_file/)** project demonstrates how to work with file objects in both text and binary formats.

- **Key Features**: Demonstrates create, write, read, copy and delete files, illustrates use of file name mask.
- **Sub-project**:
  - `03_file`: Handles text and binary files, operations with relative and absolute file path, and use of file name mask.

---

### **[04_trace](./04_trace/)**

The **[04_trace](./04_trace/)** project demonstrates the usage of tracing in the AREG SDK to log activities.

- **Key Features**: Demonstrates logging and tracing functionality to debug and monitor applications.
- **Sub-project**:
  - `04_trace`: Initializes and starts logging service in the application, logs messages.

---

### **[05_timer](./05_timer/)**

The **[05_timer](./05_timer/)** project showcases the use of timers in multithreading environment.

- **Key Features**: Demonstrates periodic and continues timers, and the timer event processed in the thread.
- **Sub-project**:
  - `05_timer`: Implements one-time, periodic and continues timers, where the events are processed in the timer owning thread.

---

### **[06_threads](./06_threads/)**

The **[06_threads](./06_threads/)** project demonstrates the creation and management of custom *event dispatching* and simple threads.

- **Key Features**: Showcases a simple and dispatcher thread to process timer events.
- **Sub-project**:
  - `06_threads`: Creates and handles two types of threads: `Thread` and `DispatcherThread` objects where `DispatcherThread` processes timer events.

---

### **[07_synch](./07_synch/)**

The **[07_synch](./07_synch/)** project shows how synchronization mechanisms are used between threads in a multithreading environment.

- **Key Features**: Showcases `Mutex` and `SynchEvent` synchronization objects and the technique of waiting events.
- **Sub-project**:
  - `07_synch`: Utilizes synchronization object, simultaneously waits for one or all events of different synchronization types.

---

### **[08_service](./08_service/)**

The **[08_service](./08_service/)** project demonstrates creation of a component, which is providing a service (*Service Provider* component), and the creation of a simple *Application Model* to load and start service runtime.

- **Key Features**: Creates a *Service Provider* component and defines a *model* to load runtime.
- **Sub-project**:
  - `08_service`: Implements a *Service Provider* component with empty service (no function), defines a *model*, which describes the thread and the component, a loads model to startup local service.

---

### **[09_svcmulti](./09_svcmulti/)**

The **[09_svcmulti](./09_svcmulti/)** demonstrates reuse of the same *Service Provider* component.

- **Key Features**: Demonstrates the creation of a model, the technique of service reuse in a thread-component definition.
- **Sub-project**:
  - `09_svcmulti`: An empty servicing component (no function) that is reused in the model by having other *role name*.

---

### **[10_locsvc](./10_locsvc/)**

The **[10_locsvc](./10_locsvc/)** project demonstrates how local services are defined using AREG's *application modeling* and how the service client object (*Service Consumer*) communicates with the micro-server (Service Provider) to trigger a remote method.

- **Key Features**: Local multithreading communication between a service (*Service Provider*) and a client within a single process.
- **Sub-project**:
  - `10_locsvc`: Implements service and client in the same process, which are defined in the *Application Model*.

---

### **[11_locmesh](./11_locmesh/)**

The **[11_locmesh](./11_locmesh/)** project demonstrates a mesh of services that communicate in multithreading environment, simulating a distributed system in a single process.

- **Key Features**: Local service mesh communication.
- **Sub-project**:
  - `11_locmesh`: Creates a mesh of interconnected local services, which are defined in the *Application Model*.

---

### **[12_pubsvc](./12_pubsvc/)**

The **[12_pubsvc](./12_pubsvc/)** is an *inter-process communication* (**IPC**) project that demonstrates simple implementation of an application with a component that provides a *Public* service (micro-server or *Service Provider*) and another application that contains a service consumer component (service client or *Service Consumer*).

- **Key Features**: Public services that are accessible by external clients.
- **Sub-project**:
  - `12_pubservice`: A console application that functions as a network-discoverable *Public Service* provider (*micro-server* / *Service Provider*).
  - `12_pubsvc`: A console application that acts as a *Public Service* consumer (client / *Service Consumer*).

---

### **[13_pubmesh](./13_pubmesh/)**

The **[13_pubmesh](./13_pubmesh/)** is an *inter-process communication* (**IPC**) project that demonstrates a mesh network of *Public* services running across multiple processes and threads containing *Local* services.

- **Key Features**: Public service mesh network mixed with local services.
- **Sub-project**:
  - `13_common`: a library providing common objects shared across other sub-projects.
  - `13_pubservice`: Hosts both *Public* and *Local* service instances, consumes *Local* services.
  - `13_pubsvcmesh`: Creates a mesh network with mixed instances of service providers and consumers.
  - `13_pubclients`: Housing *Public* service consumers, provides and consumes *Local* services for multithreading communication.

---

### **[14_pubtraffic](./14_pubtraffic/)**

The **[14_pubtraffic](./14_pubtraffic)** is an *inter-process communication* (**IPC**) project that demonstrates dynamic *Application Model* creation, load and unload functionality and custom event processing.

- **Key Features**: Demonstrates how the dynamic model is possible to create, creates custom event with custom data to process in multithreading environment.
- **Sub-project**:
  - `14_pubclient`: A service consumer that dynamically creates models during runtime, multiple instances can be started.
  - `14_pubservice`: Provides both *Public* and *Local* services, and handles custom events with custom data.
  - 

---

### **[15_pubworker](./15_pubworker)**

The **[15_pubworker](./15_pubworker)** is an *inter-process communication* (**IPC**) project that demonstrates the use of *Worker Threads* and the processing of custom events in both Worker and Component threads.

- **Key Features**: Showcase a Worker and a Component threads that communicate using custom event with data.
- **Sub-projects**:
  - `15_pubclient`: Service Consumer simulating input/output processing via *Worker Thread*.
  - `15_pubservice`: Public Service provider simulating input/output processing via *Worker Thread.

---

### **[16_pubfsm](./16_pubfsm)**

The **[16_pubfsm](./16_pubfsm/)** is an *inter-process communication* (**IPC**) project that demonstrates a *finite state machine* (**FSM**) within a public service to control its behavior based on events.

- **Key Features**: Demonstrates *finite state machine* for controlling service behavior.
- **Sub-project**:
  - `16_pubclient`: Showcase dynamic models during runtime, allowing to start multiple instances with unique names.
  - `16_pubservice`: Creates a *Public Service* with a *Finite-State Machine* (**FSM**).

---

### **[17_winchat](./17_winchat/)**

The **[17_winchat](./17_winchat/)** is an *inter-process communication* (**IPC**) project that demonstrates a Windows-based applications running in a fault tolerant environment.

- **Key Features**: Dynamic connection to the `mcrouter`, dynamic model creation and subscription services during runtime.
- **Sub-project**:
  - `17_register`: Instantiates a *Public Service* used by other applications.
  - `17_chatter`: Provides *Public Services* and *Service Consumers*, dynamically loads *models* during runtime.

---

### **[18_locwatchdog](./18_locwatchdog/)**

The **[18_locwatchdog](./18_locwatchdog/)** project demonstrates a watchdog service that monitors threads and restarts them if they fail, so that the servicing components restart as well.

- **Key Features**: Local watchdog service for fault-tolerance.
- **Sub-project**:
  - `18_locservice`: Monitors and restarts services within the same process.

---

### **[19_pubwatchdog](./19_pubwatchdog)**

The [19_pubwatchdog](./19_pubwatchdog/) is an *inter-process communication* (**IPC**) project that extends the watchdog concept to public services, monitoring services across processes for fault-tolerance.

- **Key Features**: A Watchdog that monitors a thread with a Public service, restarts services in case of failure and notifies all service consumers.
- **Sub-project**:
  - `19_pubclient`: The service consumer object receives *service disconnected* after simulated service provider crash.
  - `19_pubservice`: Contains a Watchdog that is responsible to monitor threads with service providers.

---

### **[20_pubdatarate](./20_pubdatarate)**

The **[20_pubdatarate](./20_pubdatarate/)** is an *inter-process communication* (**IPC**) project that measures the data rate between a public service and multiple clients.

- **Key Features**: Performance measurement of network data rates.
- **Sub-projects**:
  - `20_pubservice`: Generates and sends data.
  - `20_pubclient`: Receives and measures data rate.

---

### **[21_pubunblock](./21_pubunblock/)**

The [21_pubunblock](./21_pubunblock/) is an *inter-process communication* (**IPC**) project that demonstrates how to manually unblock service requests that are marked as busy, optimizing service throughput.

- **Key Features**: Manual unblocking of service requests, manual preparation of the request.
- **Sub-projects**:
  - `21_pubservice`: Manages request unblocking, prepares the response to reply back.
  - `21_pubclient`: Sends frequent requests to the service before they processing is finished and checks if any request was rejected to be processed.

---

### **[22_pubsub](./22_pubsub)**

The **[22_pubsub](./22_pubsub/)** is an *inter-process communication* (**IPC**) project that demonstrates the *Publish/Subscribe* pattern, where a publisher sends updates, and subscribers receive them.

- **Key Features**: Demonstrates publish/subscribe with notifications on data change.
- **Sub-projects**:
  - `22_publisher`: Publishes data to subscribe on updates.
  - `22_subscriber`: Receives updates when subscribed data are modified.

---

### **[23_pubsubmix](./23_pubsubmix/)**

The **[23_pubsubmix](./23_pubsubmix)** is an *inter-process communication* (**IPC**) project that extends the Publish/Subscribe model, mixing publishers and subscribers in separate threads and processes.

- **Key Features**: Mixed publisher/subscriber configuration with fault-tolerance during network interruptions.
- **Sub-project**:
  - `23_common`: library containing shared objects used by other processes.
  - `23_pubsubctrl`: Acts as both a *Data Publisher* and *Data Subscriber*.
  - `23_pubsubdyn`: Acts as both a *Data Publisher* and *Subscriber*, the 

---

### **[24_pubsubmulti](./24_pubsubmulti/)**

The **24_pubsubmulti** project optimizes the PubSub model by ensuring multiple subscribers only receive necessary notifications, reducing event overhead.

- **Key Features**: Efficient event notification handling for multiple subscribers.
- **Sub-project**:
  - `24_publisher`: Provides a network-discoverable *Public Service* that publishes data.
  - `24_subscribermulti`: Subscribes on data provided by Public Service and checks that the data is not mixed with the local PubSub data.

---

### Conclusion

These examples provide a comprehensive exploration of the AREG SDK’s capabilities, covering multithreading, IPC, service discovery, and fault-tolerance in various deployment scenarios. Each project highlights different aspects of the framework, making it easier for developers to build robust, efficient applications.

