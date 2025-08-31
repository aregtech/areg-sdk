
# AREG SDK Examples

```
This document is part of the AREG SDK and describes its examples.
Copyright (c) 2017-2023, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

---

## Introduction

The AREG SDK provides a robust framework for building applications that require **multithreading**, **multiprocessing** (**inter-process communication or IPC**), and **real-time** event handling. This repository features a range of example projects designed to help developers quickly grasp the core functionalities of the AREG Framework, such as creating **service components**, managing **client-server interactions**, implementing **state-machines**, and developing **fault-tolerant** systems.

This guide offers a detailed overview of each example, highlighting the key features that showcase **AREG's capabilities** in building **high-performance**, **real-time**, and distributed systems.

---

## Quick Build Guide

Follow these steps to get started with the AREG SDK:

### 1. Clone the AREG SDK Repository

```bash
git clone https://github.com/aregtech/areg-sdk.git
```

### 2. Build the AREG SDK with CMake

```bash
cmake -B ./build
cmake --build ./build -j 20
```

> [!IMPORTANT]
> To build the examples, ensure the CMake option `AREG_EXAMPLES` is set to `ON`. This option is **enabled** by default.
> If you want to disable the examples during the build process, set `AREG_EXAMPLES` to `OFF` using the following command:
> ```bash
> cmake -B ./build -DAREG_EXAMPLES:BOOL=OFF
> ```

For more details, refer to the [Building AREG SDK with CMake](./../docs/wiki/01b-cmake-build.md), also see the list of default CMake variable settings in the [user.cmake](./../conf/cmake/user.cmake) file.

### 3. Build with Microsoft Visual Studio

```console
MSBuild ./areg-sdk.sln
```

---

## Important Notes

- **IPC Examples**: Projects that use **inter-process communication (IPC)** require the **mcrouter** service to facilitate communication between different processes. The classification of such projects is described in the **Multitasking** section of example description. Run **mcrouter** from the build directory for testing.
- **Fault-Tolerance**: The AREG SDK is designed with fault-tolerance in mind. The system works reliably as long as the **Service Provider**, **Service Consumer**, and **mcrouter** are running, regardless of their startup order.
- **Generated projects:** All **xx_generate** projects are generated from **Service Interface** document files (`.siml`). For CMake build these projects and source files are generated during configuration. For Microsoft Visual Studio, these projects are predefined and the sources are generated as a pre-build event of `dummy` project, which exists in this `example` directory.
- **MFC based projects:** The project [20_winchat](./20_winchat/) is build only under Windows system with Microsoft Compilers (*MSVC* and *ClangCL*) and it requires *Microsoft Foundation Classes (MFC)*.

---

## AREG SDK Examples Overview

The AREG SDK includes a comprehensive set of examples that demonstrate **multithreading**, **IPC**, **service discovery**, **fault-tolerance**, and more.

### Example Projects:

#### **[03_helloservice](./03_helloservice/)**

The **[03_helloservice](./03_helloservice/)** project introduces core concepts of creating **service** (*micro-server*) and **client** objects, showcasing how to switch between **multithreading** and **multiprocessing** by modifying the *Application Model*.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Single-thread, multi-thread, and multi-process service/client interactions.
- **Sub-projects**:
  - `03_onethread`: Service provider and client in the same thread.
  - `03_twothreads`: Service provider and client in separate threads.
  - `03_pubservice` and `03_pubclient`: Service and client in different processes, allowing for scalable client-server interactions.

---

#### **[04_hellothread](./04_hellothread/)**

The **[04_hellothread](./04_hellothread/)** is a simple **multithreading** project that demonstrates the basics of creating and managing threads.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Demonstrates thread creation and basic thread synchronization.
- **Sub-project**:
  - `04_hellothread`: Starts a thread that prints "Hello World!" and manages its lifecycle.

---

#### **[05_buffer](./05_buffer/)**

The **[05_buffer](./05_buffer/)** project demonstrates **shared memory** and **data serialization** between threads in a **multithreading** environment.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Shared buffer for passing serialized data between threads.
- **Sub-project**:
  - `05_buffer`: Serializes data, passes it via shared memory buffer, and deserializes it in a separate thread.

---

#### **[06_file](./06_file/)**

The **[06_file](./06_file/)** project shows how to handle file operations such as reading, writing, copying, and deleting files in both text and binary formats.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: File handling in **multithreading** environments.
- **Sub-project**:
  - `06_file`: Performs file operations using relative/absolute paths and filename masks.

---

#### **[07_logging](./07_logging/)**

The **[07_logging](./07_logging/)** project demonstrates **method call tracing and message logging** mechanisms to monitor and debug application activities.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Logs application actions for debugging and analysis.
- **Sub-project**:
  - `07_logging`: Integrates AREG's logging service to log messages during runtime.

---

#### **[08_timer](./08_timer/)**

The **[08_timer](./08_timer/)** project is a demonstration of timers for handling **real-time** events in **multithreaded** applications.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Implements one-time, periodic and continues timers.
- **Sub-project**:
  - `08_timer`: Uses timers to trigger events in a **multithreaded** environment.

---

#### **[09_threads](./09_threads/)**

The **[09_threads](./09_threads/)** project explains how to create and manage custom **event dispatching** threads.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Manages simple and dispatcher threads to process custom events.
- **Sub-project**:
  - `09_threads`: Demonstrates thread management for **real-time** event handling.

---

#### **[10_synch](./10_synch/)**

The **[10_synch](./10_synch/)** project illustrates synchronization between multiple threads using **mutexes** and **events**.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Synchronization primitives for managing thread-safe operations.
- **Sub-project**:
  - `10_synch`: Implements synchronized event handling across threads.

---

#### **[11_service](./11_service/)**

The **[11_service](./11_service/)** project introduces the creation of a **Service Provider** component and shows how to define an *Application Model* to launch services.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Service provider implementation in a single-threaded context.
- **Sub-project**:
  - `11_service`: Defines and deploys a service component within an application model.

---

#### **[12_svcmulti](./12_svcmulti/)**

The **[12_svcmulti](./12_svcmulti/)** project demonstrates the reuse of service components across multiple threads defined in *Application Model*.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Service reuse in different contexts via **multithreading** and **multiprocessing**.
- **Sub-project**:
  - `12_svcmulti`: Implements a service that can be reused across different roles.

---

#### **[13_locsvc](./13_locsvc/)**

The **[13_locsvc](./13_locsvc/)** project introduces **Local Service**, and the communication between a **Service Provider** and **Service Consumer** within the same process.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Local Service, and interaction between *Service Provider* and *Service Consumer* in a **multithreaded** environment.
- **Sub-project**:
  - `13_locsvc`: Defines both service (*micro-server*) and client within the same process.

---

#### **[14_locmesh](./14_locmesh/)**

The **[14_locmesh](./14_locmesh/)** project is a demonstration of **mesh of local services** where multiple service providers and consumers communicate across multiple threads within a single process.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: **Mesh of Local Services** and **multithreading** communication.
- **Sub-project**:
  - `14_locmesh`: Creates a mesh of interconnected services for real-time communication.

---

#### **[15_pubsvc](./15_pubsvc/)**

The **[15_pubsvc](./15_pubsvc/)** project is an **inter-process communication (IPC)** project that illustrates a **Public Service** provider and its client (consumer) interacting across processes.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: **Public services** accessible to clients in other processes.
- **Sub-projects**:
  - `15_pubservice`: Provides a network-discoverable public service, which methods are triggered via **Object Remote Procedure Call (Object RPC or ORPC)**.
  - `15_pubclient`: Connects to and consumes the public service.

---

#### **[16_pubmesh](./16_pubmesh/)**

The **[16_pubmesh](./16_pubmesh/)** project demonstrates a **Mesh of Public Services** across processes and integrating **Local Services** in the processes.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: **Mesh of Public Services** across multiple processes.
- **Sub-projects**:
  - `16_common`: Contains shared resources used by other sub-projects.
  - `16_pubservice`: Provides and consumes both **Public** and **Local** Services. It also includes a Shutdown Service to signal a shutdown event for all processes.
  - `16_pubclients`: Hosts multiple client (Consumer) components.
  - `16_pubsvcmesh`: Provides a mixture of **Public** and **Local** services.

---

#### **[17_pubtraffic](./17_pubtraffic/)**

The **[17_pubtraffic](./17_pubtraffic/)** project shows how to dynamically create and modify the *Application Model* and handle custom events in an IPC system based on AREG Framework.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Dynamic model creation and event processing.
- **Sub-projects**:
  - `17_pubclient`: Dynamically creates models at runtime.
  - `17_pubservice`: Handles timer and custom events with custom data.

---

#### **[18_pubworker](./18_pubworker/)**

The **[18_pubworker](./18_pubworker/)** project is an **IPC** project that demonstrates the use of **Worker Threads** and how to process custom events using AREG Framework.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: **Worker Thread** processing in multithreaded communication and IPC scenarios.
- **Sub-projects**:
  - `18_pubservice`: A **Public Service Provider** component with a **Worker Thread** and custom event handling used in multithreaded communication.
  - `18_pubclient`: A **Public Service Consumer** component with a **Worker Thread** and custom event handling used in multithreaded communication.

---

#### **[19_pubfsm](./19_pubfsm/)**

The **[19_pubfsm](./19_pubfsm/)** project demonstrates the implementation of a **Finite State Machine (FSM)** to control service behavior.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: **FSM** for state-driven service behavior.
- **Sub-projects**:
  - `19_pubservice`: Creates a **Public Service** with an integrated **FSM**.
  - `19_pubclient`: Provides a dynamic *Application Modeling* to start multiple instances of the process, loads and start *Service Consumer*.

---

#### **[20_winchat](./20_winchat/)**

The **[20_winchat](./20_winchat/)** project is a Windows-based chat application using **IPC** in a **fault-tolerant** system.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Dynamic connection to **mcrouter** and real-time communication.
- **Sub-projects**:
  - `20_register`: Instantiates a **Public Service** used by all processes.
  - `20_chatter`: Provides and consumes dynamic Public Services.

---

#### **[21_locwatchdog](./21_locwatchdog/)**

The **[21_locwatchdog](./21_locwatchdog/)** project showcases a **Watchdog** object in Local Service case for **multithreaded** applications. It actively monitors threads and automatically restarts them if they fail, ensuring that service components remain operational and fault-tolerant.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Within a single process implements a Local Service in a thread with the Watchdog for robust fault tolerance.
- **Sub-project**:
  - `21_locservice`: Within multithreading process, a Watchdog monitors and restarts service, improving reliability and resilience.

---

#### **[22_pubwatchdog](./22_pubwatchdog/)**

The **[22_pubwatchdog](./22_pubwatchdog/)** project extends the **Watchdog** concept to support **inter-process communication (IPC)**, ensuring fault-tolerance across multiple processes. It monitors public services, restarts them in case of failure, and notifies all connected service consumers.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: A Watchdog that monitors threads with Public Service, restarts services upon failure, and ensures that consumers are notified of disconnections.
- **Sub-projects**:
  - `22_pubclient`: Simulates a service consumer that reacts to Provider failures by handling service disconnections.
  - `22_pubservice`: Houses a Watchdog responsible for monitoring threads with Public Service Providers and ensuring fault recovery.

---

#### **[23_pubdatarate](./23_pubdatarate/)**

The **[23_pubdatarate](./23_pubdatarate/)** project is an **inter-process communication (IPC)** example that focuses on measuring the data rate between a public service and multiple clients. This project helps developers understand performance benchmarks in data transfer scenarios.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Measures network data rates between a public service and its clients for performance evaluation.
- **Sub-projects**:
  - `23_pubservice`: Acts as a data generator, sending large datasets to connected clients.
  - `23_pubclient`: Receives data from the service and measures the transfer rate, providing insights into system performance.

---

#### **[24_pubunblock](./24_pubunblock/)**

The **[24_pubunblock](./24_pubunblock/)** project demonstrates how to improve service throughput in **inter-process communication (IPC)** by manually unblocking service requests marked as busy. This approach helps optimize request handling and resource utilization in asynchronous communication.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Manual unblocking of service requests, allowing faster request processing and optimized service management in applications with asynchronous communication.
- **Sub-projects**:
  - `24_pubservice`: Manages service requests in asynchronous communication and provides functionality to manually unblock and prepare responses efficiently.
  - `24_pubclient`: Frequently sends requests to the service and ensures proper handling of requests and responses, enhancing throughput.

---

#### **[25_pubsub](./25_pubsub/)**

The **[25_pubsub](./25_pubsub/)** project demonstrates the **Publish/Subscribe (Pub/Sub)** pattern within an **inter-process communication (IPC)** setup. It allows a publisher to send data updates while subscribers receive real-time notifications whenever the data changes.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Implements the **Pub/Sub** model, providing real-time notifications when subscribed data is updated.
- **Sub-projects**:
  - `25_publisher`: Publishes updates, making data available to all subscribed clients.
  - `25_subscriber`: Listens for changes and receives updates from the publisher when subscribed data is modified.

---

#### **[26_pubsubmix](./26_pubsubmix/)**

The **[26_pubsubmix](./26_pubsubmix/)** project extends the **Publish/Subscribe** model by introducing mixed configurations where publishers and subscribers are distributed across different threads and processes. This example showcases robust fault-tolerance during network interruptions in **multithreaded** and **multiprocess** environments.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Fault-tolerant **Pub/Sub** model across both threads and processes, providing resilience during network disruptions.
- **Sub-projects**:
  - `26_common`: Contains shared resources used by other sub-projects.
  - `26_pubsubctrl`: Acts as both a **Data Publisher** and **Data Subscriber**, dynamically adjusting its role based on the communication flow.
  - `26_pubsubdyn`: Demonstrates flexibility by functioning as both a **Publisher** and **Subscriber**, supporting dynamic role switching across processes.

---

#### **[27_pubsubmulti](./27_pubsubmulti/)**

The **[27_pubsubmulti](./27_pubsubmulti/)** project optimizes the **Publish/Subscribe (Pub/Sub)** model for multiple remote subscribers in one thread by reducing event overhead. It ensures that only necessary notifications are sent, improving system efficiency and performance.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Efficient event notification system that minimizes overhead in **Pub/Sub** models involving multiple subscribers within one thread.
- **Sub-projects**:
  - `27_publisher`: Provides a network-discoverable **Public Service** that publishes data to clients.
  - `27_subscribermulti`: Contains two Subscribes to the public service running in one thread. It verifies that received data is properly separated for subscriptions, ensuring consistency.

---

## Conclusion

These examples comprehensively demonstrate the versatility of the AREG SDK in managing **multithreading**, **inter-process communication (IPC)**, **service discovery**, and **fault-tolerance**. They provide practical insights into building scalable, efficient, and resilient systems using advanced patterns like **Publish/Subscribe**, **watchdogs**, and **real-time** data management.
