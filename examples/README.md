
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
- **MFC based projects:** The project [17_winchat](./17_winchat/) is build only under Windows system with Microsoft Compilers (*MSVC* and *ClangCL*) and it requires *Microsoft Foundation Classes (MFC)*.

---

## AREG SDK Examples Overview

The AREG SDK includes a comprehensive set of examples that demonstrate **multithreading**, **IPC**, **service discovery**, **fault-tolerance**, and more.

### Example Projects:

#### **[00_helloservice](./00_helloservice/)**

The **[00_helloservice](./00_helloservice/)** project introduces core concepts of creating **service** (*micro-server*) and **client** objects, showcasing how to switch between **multithreading** and **multiprocessing** by modifying the *Application Model*.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Single-thread, multi-thread, and multi-process service/client interactions.
- **Sub-projects**:
  - `00_onethread`: Service provider and client in the same thread.
  - `00_twothreads`: Service provider and client in separate threads.
  - `00_pubservice` and `00_pubclient`: Service and client in different processes, allowing for scalable client-server interactions.

---

#### **[01_hello](./01_hello/)**

The **[01_hello](./01_hello/)** is a simple **multithreading** project that demonstrates the basics of creating and managing threads.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Demonstrates thread creation and basic thread synchronization.
- **Sub-project**:
  - `01_hello`: Starts a thread that prints "Hello World!" and manages its lifecycle.

---

#### **[02_buffer](./02_buffer/)**

The **[02_buffer](./02_buffer/)** project demonstrates **shared memory** and **data serialization** between threads in a **multithreading** environment.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Shared buffer for passing serialized data between threads.
- **Sub-project**:
  - `02_buffer`: Serializes data, passes it via shared memory buffer, and deserializes it in a separate thread.

---

#### **[03_file](./03_file/)**

The **[03_file](./03_file/)** project shows how to handle file operations such as reading, writing, copying, and deleting files in both text and binary formats.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: File handling in **multithreading** environments.
- **Sub-project**:
  - `03_file`: Performs file operations using relative/absolute paths and filename masks.

---

#### **[04_logging](./04_logging/)**

The **[04_logging](./04_logging/)** project demonstrates **method call tracing and message logging** mechanisms to monitor and debug application activities.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Logs application actions for debugging and analysis.
- **Sub-project**:
  - `04_logging`: Integrates AREG's logging service to log messages during runtime.

---

#### **[05_timer](./05_timer/)**

The **[05_timer](./05_timer/)** project is a demonstration of timers for handling **real-time** events in **multithreaded** applications.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Implements one-time, periodic and continues timers.
- **Sub-project**:
  - `05_timer`: Uses timers to trigger events in a **multithreaded** environment.

---

#### **[06_threads](./06_threads/)**

The **[06_threads](./06_threads/)** project explains how to create and manage custom **event dispatching** threads.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Manages simple and dispatcher threads to process custom events.
- **Sub-project**:
  - `06_threads`: Demonstrates thread management for **real-time** event handling.

---

#### **[07_synch](./07_synch/)**

The **[07_synch](./07_synch/)** project illustrates synchronization between multiple threads using **mutexes** and **events**.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Synchronization primitives for managing thread-safe operations.
- **Sub-project**:
  - `07_synch`: Implements synchronized event handling across threads.

---

#### **[08_service](./08_service/)**

The **[08_service](./08_service/)** project introduces the creation of a **Service Provider** component and shows how to define an *Application Model* to launch services.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Service provider implementation in a single-threaded context.
- **Sub-project**:
  - `08_service`: Defines and deploys a service component within an application model.

---

#### **[09_svcmulti](./09_svcmulti/)**

The **[09_svcmulti](./09_svcmulti/)** project demonstrates the reuse of service components across multiple threads defined in *Application Model*.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Service reuse in different contexts via **multithreading** and **multiprocessing**.
- **Sub-project**:
  - `09_svcmulti`: Implements a service that can be reused across different roles.

---

#### **[10_locsvc](./10_locsvc/)**

The **[10_locsvc](./10_locsvc/)** project introduces **Local Service**, and the communication between a **Service Provider** and **Service Consumer** within the same process.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Local Service, and interaction between *Service Provider* and *Service Consumer* in a **multithreaded** environment.
- **Sub-project**:
  - `10_locsvc`: Defines both service (*micro-server*) and client within the same process.

---

#### **[11_locmesh](./11_locmesh/)**

The **[11_locmesh](./11_locmesh/)** project is a demonstration of **mesh of local services** where multiple service providers and consumers communicate across multiple threads within a single process.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: **Mesh of Local Services** and **multithreading** communication.
- **Sub-project**:
  - `11_locmesh`: Creates a mesh of interconnected services for real-time communication.

---

#### **[12_pubsvc](./12_pubsvc/)**

The **[12_pubsvc](./12_pubsvc/)** project is an **inter-process communication (IPC)** project that illustrates a **Public Service** provider and its client (consumer) interacting across processes.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: **Public services** accessible to clients in other processes.
- **Sub-projects**:
  - `12_pubservice`: Provides a network-discoverable public service, which methods are triggered via **Object Remote Procedure Call (Object RPC or ORPC)**.
  - `12_pubclient`: Connects to and consumes the public service.

---

#### **[13_pubmesh](./13_pubmesh/)**

The **[13_pubmesh](./13_pubmesh/)** project demonstrates a **Mesh of Public Services** across processes and integrating **Local Services** in the processes.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: **Mesh of Public Services** across multiple processes.
- **Sub-projects**:
  - `13_common`: Contains shared resources used by other sub-projects.
  - `13_pubservice`: Provides and consumes both **Public** and **Local** Services. It also includes a Shutdown Service to signal a shutdown event for all processes.
  - `13_pubclients`: Hosts multiple client (Consumer) components.
  - `13_pubsvcmesh`: Provides a mixture of **Public** and **Local** services.

---

#### **[14_pubtraffic](./14_pubtraffic/)**

The **[14_pubtraffic](./14_pubtraffic/)** project shows how to dynamically create and modify the *Application Model* and handle custom events in an IPC system based on AREG Framework.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Dynamic model creation and event processing.
- **Sub-projects**:
  - `14_pubclient`: Dynamically creates models at runtime.
  - `14_pubservice`: Handles timer and custom events with custom data.

---

#### **[15_pubworker](./15_pubworker/)**

The **[15_pubworker](./15_pubworker/)** project is an **IPC** project that demonstrates the use of **Worker Threads** and how to process custom events using AREG Framework.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: **Worker Thread** processing in multithreaded communication and IPC scenarios.
- **Sub-projects**:
  - `15_pubservice`: A **Public Service Provider** component with a **Worker Thread** and custom event handling used in multithreaded communication.
  - `15_pubclient`: A **Public Service Consumer** component with a **Worker Thread** and custom event handling used in multithreaded communication.

---

#### **[16_pubfsm](./16_pubfsm/)**

The **[16_pubfsm](./16_pubfsm/)** project demonstrates the implementation of a **Finite State Machine (FSM)** to control service behavior.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: **FSM** for state-driven service behavior.
- **Sub-projects**:
  - `16_pubservice`: Creates a **Public Service** with an integrated **FSM**.
  - `16_pubclient`: Provides a dynamic *Application Modeling* to start multiple instances of the process, loads and start *Service Consumer*.

---

#### **[17_winchat](./17_winchat/)**

The **[17_winchat](./17_winchat/)** project is a Windows-based chat application using **IPC** in a **fault-tolerant** system.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Dynamic connection to **mcrouter** and real-time communication.
- **Sub-projects**:
  - `17_register`: Instantiates a **Public Service** used by all processes.
  - `17_chatter`: Provides and consumes dynamic Public Services.

---

#### **[18_locwatchdog](./18_locwatchdog/)**

The **[18_locwatchdog](./18_locwatchdog/)** project showcases a **Watchdog** object in Local Service case for **multithreaded** applications. It actively monitors threads and automatically restarts them if they fail, ensuring that service components remain operational and fault-tolerant.

- **Multitasking**: Multithreading project, no run of `mcrouter` is required.
- **Key Features**: Within a single process implements a Local Service in a thread with the Watchdog for robust fault tolerance.
- **Sub-project**:
  - `18_locservice`: Within multithreading process, a Watchdog monitors and restarts service, improving reliability and resilience.

---

#### **[19_pubwatchdog](./19_pubwatchdog/)**

The **[19_pubwatchdog](./19_pubwatchdog/)** project extends the **Watchdog** concept to support **inter-process communication (IPC)**, ensuring fault-tolerance across multiple processes. It monitors public services, restarts them in case of failure, and notifies all connected service consumers.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: A Watchdog that monitors threads with Public Service, restarts services upon failure, and ensures that consumers are notified of disconnections.
- **Sub-projects**:
  - `19_pubclient`: Simulates a service consumer that reacts to Provider failures by handling service disconnections.
  - `19_pubservice`: Houses a Watchdog responsible for monitoring threads with Public Service Providers and ensuring fault recovery.

---

#### **[20_pubdatarate](./20_pubdatarate/)**

The **[20_pubdatarate](./20_pubdatarate/)** project is an **inter-process communication (IPC)** example that focuses on measuring the data rate between a public service and multiple clients. This project helps developers understand performance benchmarks in data transfer scenarios.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Measures network data rates between a public service and its clients for performance evaluation.
- **Sub-projects**:
  - `20_pubservice`: Acts as a data generator, sending large datasets to connected clients.
  - `20_pubclient`: Receives data from the service and measures the transfer rate, providing insights into system performance.

---

#### **[21_pubunblock](./21_pubunblock/)**

The **[21_pubunblock](./21_pubunblock/)** project demonstrates how to improve service throughput in **inter-process communication (IPC)** by manually unblocking service requests marked as busy. This approach helps optimize request handling and resource utilization in asynchronous communication.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Manual unblocking of service requests, allowing faster request processing and optimized service management in applications with asynchronous communication.
- **Sub-projects**:
  - `21_pubservice`: Manages service requests in asynchronous communication and provides functionality to manually unblock and prepare responses efficiently.
  - `21_pubclient`: Frequently sends requests to the service and ensures proper handling of requests and responses, enhancing throughput.

---

#### **[22_pubsub](./22_pubsub/)**

The **[22_pubsub](./22_pubsub/)** project demonstrates the **Publish/Subscribe (Pub/Sub)** pattern within an **inter-process communication (IPC)** setup. It allows a publisher to send data updates while subscribers receive real-time notifications whenever the data changes.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Implements the **Pub/Sub** model, providing real-time notifications when subscribed data is updated.
- **Sub-projects**:
  - `22_publisher`: Publishes updates, making data available to all subscribed clients.
  - `22_subscriber`: Listens for changes and receives updates from the publisher when subscribed data is modified.

---

#### **[23_pubsubmix](./23_pubsubmix/)**

The **[23_pubsubmix](./23_pubsubmix/)** project extends the **Publish/Subscribe** model by introducing mixed configurations where publishers and subscribers are distributed across different threads and processes. This example showcases robust fault-tolerance during network interruptions in **multithreaded** and **multiprocess** environments.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Fault-tolerant **Pub/Sub** model across both threads and processes, providing resilience during network disruptions.
- **Sub-projects**:
  - `23_common`: Contains shared resources used by other sub-projects.
  - `23_pubsubctrl`: Acts as both a **Data Publisher** and **Data Subscriber**, dynamically adjusting its role based on the communication flow.
  - `23_pubsubdyn`: Demonstrates flexibility by functioning as both a **Publisher** and **Subscriber**, supporting dynamic role switching across processes.

---

#### **[24_pubsubmulti](./24_pubsubmulti/)**

The **[24_pubsubmulti](./24_pubsubmulti/)** project optimizes the **Publish/Subscribe (Pub/Sub)** model for multiple remote subscribers in one thread by reducing event overhead. It ensures that only necessary notifications are sent, improving system efficiency and performance.

- **Multitasking**: Multiprocessing project with **IPC**, run of `mcrouter` is required.
- **Key Features**: Efficient event notification system that minimizes overhead in **Pub/Sub** models involving multiple subscribers within one thread.
- **Sub-projects**:
  - `24_publisher`: Provides a network-discoverable **Public Service** that publishes data to clients.
  - `24_subscribermulti`: Contains two Subscribes to the public service running in one thread. It verifies that received data is properly separated for subscriptions, ensuring consistency.

---

## Conclusion

These examples comprehensively demonstrate the versatility of the AREG SDK in managing **multithreading**, **inter-process communication (IPC)**, **service discovery**, and **fault-tolerance**. They provide practical insights into building scalable, efficient, and resilient systems using advanced patterns like **Publish/Subscribe**, **watchdogs**, and **real-time** data management.
