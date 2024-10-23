
# AREG SDK Examples

```
This document is part of the AREG SDK and describes its examples.
Copyright © 2017-2023, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

## Introduction

The AREG SDK provides a robust framework for building applications that require **multithreading**, **multiprocessing** (**inter-process communication (IPC)**), and **real-time** event handling. This repository features a range of example projects designed to help developers quickly grasp the core functionalities of the AREG Framework, such as creating **service components**, managing **client-server interactions**, implementing **state-machines**, and developing **fault-tolerant** systems.

This guide offers a detailed overview of each example, highlighting the key features that showcase **AREG's capabilities** in building **high-performance**, **real-time**, and distributed systems.

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

### 3. Build with Microsoft Visual Studio

```console
MSBuild ./areg-sdk.sln
```

> [!NOTE]  
> Ensure that the `AREG_EXAMPLES` option is **enabled** in CMake to include the examples in the build. By default, the build of examples is enabled. Example of forcing configure the build with enabled examples: `cmake -B ./build -AREG_EXAMPLES:BOOL=ON`

For more details, refer to the [Build Documentation](./../docs/wiki/BUILD.md).

---

## AREG SDK Examples Overview

The AREG SDK includes a comprehensive set of examples that demonstrate **multithreading**, **IPC**, **service discovery**, **fault-tolerance**, and more.

### Important Notes:

> [!IMPORTANT]

- **IPC Examples**: Projects that use **inter-process communication (IPC)** require the **mcrouter** service to facilitate communication between different processes. Run **mcrouter** from the build directory for testing.
- **Fault-Tolerance**: The AREG SDK is designed with fault-tolerance in mind. The system works reliably as long as the **Service Provider**, **Service Consumer**, and **mcrouter** are running, regardless of their startup order.
- All **xx_generate** projects are generated from **Service Interface** document files (`.siml`). For CMake build these projects and source files are generated during configuration. For Microsoft Visual Studio, these projects are predefined and the sources are generated as a pre-build event of `dummy` project, which exists in this `example` directory.
- The project [17_winchat](./17_winchat/) is build only under Windows system with Microsoft Compilers (*MSVC* and *ClangCL*) and it requires *Microsoft Foundation Classes (MFC)*.

### Example Projects:

---

#### **[00_helloservice](./00_helloservice/)**

The **[00_helloservice](./00_helloservice/)** project introduces core concepts of creating **service** (*micro-server*) and **client** objects, showcasing how to switch between **multithreading** and **multiprocessing** by modifying the *Application Model*.

- **Key Features**: Single-thread, multi-thread, and multi-process service/client interactions.
- **Sub-projects**:  
  - `00_onethread`: Service provider and client in the same thread.  
  - `00_twothreads`: Service provider and client in separate threads.  
  - `00_pubservice` and `00_pubclient`: Service and client in different processes, allowing for scalable client-server interactions.

---

#### **[01_hello](./01_hello/)**

The **[01_hello](./01_hello/)** is a simple **multithreading** project that demonstrates the basics of creating and managing threads.

- **Key Features**: Demonstrates thread creation and basic thread synchronization.
- **Sub-project**:  
  - `01_hello`: Starts a thread that prints "Hello World!" and manages its lifecycle.

---

#### **[02_buffer](./02_buffer/)**

The **[02_buffer](./02_buffer/)** project demonstrates **shared memory** and **data serialization** between threads in a **multithreading** environment.

- **Key Features**: Shared buffer for passing serialized data between threads.
- **Sub-project**:  
  - `02_buffer`: Serializes data, passes it via shared memory buffer, and deserializes it in a separate thread.

---

#### **[03_file](./03_file/)**

The **[03_file](./03_file/)** project shows how to handle file operations such as reading, writing, copying, and deleting files in both text and binary formats.

- **Key Features**: File handling in **multithreading** environments.
- **Sub-project**:  
  - `03_file`: Performs file operations using relative/absolute paths and filename masks.

---

#### **[04_trace](./04_trace/)**

The **[04_trace](./04_trace/)** project demonstrates **tracing and logging** mechanisms to monitor and debug application activities.

- **Key Features**: Logs application actions for debugging and analysis.
- **Sub-project**:  
  - `04_trace`: Integrates AREG's logging service to trace messages during runtime.

---

#### **[05_timer](./05_timer/)**

The **[05_timer](./05_timer/)** project is a demonstration of timers for handling **real-time** events in **multithreaded** applications.

- **Key Features**: Implements periodic and one-time timers.
- **Sub-project**:  
  - `05_timer`: Uses timers to trigger events in a **multithreaded** environment.

---

#### **[06_threads](./06_threads/)**

The **[06_threads](./06_threads/)** project explains how to create and manage custom **event dispatching** threads.

- **Key Features**: Manages simple and dispatcher threads to process custom events.
- **Sub-project**:  
  - `06_threads`: Demonstrates thread management for **real-time** event handling.

---

#### **[07_synch](./07_synch/)**

The **[07_synch](./07_synch/)** project illustrates synchronization between multiple threads using **mutexes** and **events**.

- **Key Features**: Synchronization primitives for managing thread-safe operations.
- **Sub-project**:  
  - `07_synch`: Implements synchronized event handling across threads.

---

#### **[08_service](./08_service/)**

The **[08_service](./08_service/)** project introduces the creation of a **Service Provider** component and shows how to define an *Application Model* to launch services.

- **Key Features**: Service provider implementation in a single-threaded context.
- **Sub-project**:  
  - `08_service`: Defines and deploys a service component within an application model.

---

#### **[09_svcmulti](./09_svcmulti/)**

The **[09_svcmulti](./09_svcmulti/)** project demonstrates the reuse of service components across multiple threads or processes.

- **Key Features**: Service reuse in different contexts via **multithreading** and **multiprocessing**.
- **Sub-project**:  
  - `09_svcmulti`: Implements a service that can be reused across different roles.

---

#### **[10_locsvc](./10_locsvc/)**

The **[10_locsvc](./10_locsvc/)** project introduces **local service** communication between a **Service Provider** and **Service Consumer** within the same process.

- **Key Features**: Local service interaction in a **multithreaded** environment.
- **Sub-project**:  
  - `10_locsvc`: Defines both service (*micro-server*) and client within the same process.

---

#### **[11_locmesh](./11_locmesh/)**

The **[11_locmesh](./11_locmesh/)** project is a demonstration of **service mesh** architecture where services communicate across multiple threads within a single process.

- **Key Features**: **Local service mesh** for **multithreading** communication.
- **Sub-project**:  
  - `11_locmesh`: Creates a mesh of interconnected services for real-time communication.

---

#### **[12_pubsvc](./12_pubsvc/)**

The **[12_pubsvc](./12_pubsvc/)** project is an **inter-process communication (IPC)** project that illustrates a **Public Service** provider and its client interacting across processes.

- **Key Features**: **Public services** accessible to clients in other processes.
- **Sub-projects**:  
  - `12_pubservice`: Provides a network-discoverable public service, which methods are triggered via **Object Remote Procedure Call (Object RPC or ORPC)**.
  - `12_pubclient`: Connects to and consumes the public service.

---

#### **[13_pubmesh](./13_pubmesh/)**

The **[13_pubmesh](./13_pubmesh/)** project demonstrates a **Public Service Mesh** across processes, integrating **local** and **public services**.

- **Key Features**: **Public service mesh** across multiple processes.
- **Sub-projects**:  
  - `13_common`: Contains shared resources used by other sub-projects.
  - `13_pubservice`: Provides and consumes both **public** and **local** services.
  - `13_pubclients`: Hosts multiple client components.
  - `13_pubsvcmesh`: Provides a mixture of **public** and **local** services.

---

#### **[14_pubtraffic](./14_pubtraffic/)**

The **[14_pubtraffic](./14_pubtraffic/)** project shows how to dynamically create and modify the *Application Model* and handle custom events in an IPC system.

- **Key Features**: Dynamic model creation and event processing.
- **Sub-projects**:  
  - `14_pubclient`: Dynamically creates models at runtime.  
  - `14_pubservice`: Handles custom events with custom data.

---

#### **[15_pubworker](./15_pubworker/)**

The **[15_pubworker](./15_pubworker/)** project is an **IPC** project that demonstrates the use of **worker threads** and how to process custom events.

- **Key Features**: **Worker thread** processing in IPC scenarios.
- **Sub-projects**:  
  - `15_pubservice`: Provides a **Public Service** with a **Worker Thread** and custom event handling.
  - `15_pubclient`: Provides a **Public Service Consumer** with a **Worker Thread** and custom event handling.

---

#### **[16_pubfsm](./16_pubfsm/)**

The **[16_pubfsm](./16_pubfsm/)** project demonstrates the implementation of a **Finite State Machine (FSM)** to control service behavior.

- **Key Features**: **FSM** for state-driven service behavior.
- **Sub-projects**:  
  - `16_pubservice`: Creates a **Public Service** with an integrated **FSM**.
  - `16_pubclient`: Provides a dynamic *Application Modeling* to start multiple instances of the process, loads and start *Service Consumer*.

---

#### **[17_winchat](./17_winchat/)**

The **[17_winchat](./17_winchat/)** project is a Windows-based chat application using **IPC** in a **fault-tolerant** system.

- **Key Features**: Dynamic connection to **mcrouter** and real-time communication.
- **Sub-projects**:  
  - `17_register`: Instantiates a public service.  
  - `17_chatter`: Provides and consumes public services.

---

### **[18_locwatchdog](./18_locwatchdog/)**

The **[18_locwatchdog](./18_locwatchdog/)** project showcases a local **watchdog** service designed for **multithreaded** applications. It actively monitors threads and automatically restarts them if they fail, ensuring that service components remain operational and fault-tolerant.

- **Key Features**: Implements a local watchdog for robust fault tolerance within a single process.
- **Sub-project**:
  - `18_locservice`: Monitors and restarts services within the same process, improving reliability and resilience.

---

### **[19_pubwatchdog](./19_pubwatchdog/)**

The **[19_pubwatchdog](./19_pubwatchdog/)** project extends the **watchdog** concept to support **inter-process communication (IPC)**, ensuring fault-tolerance across multiple processes. It monitors public services, restarts them in case of failure, and notifies all connected service consumers.

- **Key Features**: A public service watchdog that monitors threads, restarts services upon failure, and ensures that consumers are notified of disconnections.
- **Sub-projects**:
  - `19_pubclient`: Simulates a service consumer that detects and reacts to provider failures by handling service disconnections.
  - `19_pubservice`: Houses a watchdog responsible for monitoring threads with public service providers and ensuring fault recovery.

---

### **[20_pubdatarate](./20_pubdatarate/)**

The **[20_pubdatarate](./20_pubdatarate/)** project is an **inter-process communication (IPC)** example that focuses on measuring the data rate between a public service and multiple clients. This project helps developers understand performance benchmarks in data transfer scenarios.

- **Key Features**: Measures network data rates between a public service and its clients for performance evaluation.
- **Sub-projects**:
  - `20_pubservice`: Acts as a data generator, sending large datasets to connected clients.
  - `20_pubclient`: Receives data from the service and measures the transfer rate, providing insights into system performance.

---

### **[21_pubunblock](./21_pubunblock/)**

The **[21_pubunblock](./21_pubunblock/)** project demonstrates how to improve service throughput in **inter-process communication (IPC)** by manually unblocking service requests marked as busy. This approach helps optimize request handling and resource utilization.

- **Key Features**: Manual unblocking of service requests, allowing faster request processing and optimized service management.
- **Sub-projects**:
  - `21_pubservice`: Manages service requests and provides functionality to manually unblock and prepare responses efficiently.
  - `21_pubclient`: Frequently sends requests to the service and ensures proper handling of rejected or blocked requests, enhancing throughput.

---

### **[22_pubsub](./22_pubsub/)**

The **[22_pubsub](./22_pubsub/)** project demonstrates the **Publish/Subscribe (Pub/Sub)** pattern within an **inter-process communication (IPC)** setup. It allows a publisher to send data updates while subscribers receive real-time notifications whenever the data changes.

- **Key Features**: Implements the **Pub/Sub** model, providing real-time notifications when subscribed data is updated.
- **Sub-projects**:
  - `22_publisher`: Publishes updates, making data available to all subscribed clients.
  - `22_subscriber`: Listens for changes and receives updates from the publisher when subscribed data is modified.

---

### **[23_pubsubmix](./23_pubsubmix/)**

The **[23_pubsubmix](./23_pubsubmix/)** project extends the **Publish/Subscribe** model by introducing mixed configurations where publishers and subscribers are distributed across different threads and processes. This example showcases robust fault-tolerance during network interruptions in **multithreaded** and **multiprocess** environments.

- **Key Features**: Fault-tolerant **Pub/Sub** model across both threads and processes, providing resilience during network disruptions.
- **Sub-projects**:
  - `23_common`: Contains shared resources used by other sub-projects.
  - `23_pubsubctrl`: Acts as both a **Data Publisher** and **Data Subscriber**, dynamically adjusting its role based on the communication flow.
  - `23_pubsubdyn`: Demonstrates flexibility by functioning as both a **Publisher** and **Subscriber**, supporting dynamic role switching across processes.

---

### **[24_pubsubmulti](./24_pubsubmulti/)**

The **[24_pubsubmulti](./24_pubsubmulti/)** project optimizes the **Publish/Subscribe (Pub/Sub)** model for multiple subscribers by reducing event overhead. It ensures that only necessary notifications are sent, improving system efficiency and performance.

- **Key Features**: Efficient event notification system that minimizes overhead in **Pub/Sub** models involving multiple subscribers.
- **Sub-projects**:
  - `24_publisher`: Provides a network-discoverable **Public Service** that publishes data to clients.
  - `24_subscribermulti`: Subscribes to the public service and verifies that received data is properly separated from local subscriptions, ensuring consistency.

---

### Conclusion

These examples comprehensively demonstrate the versatility of the AREG SDK in managing **multithreading**, **inter-process communication (IPC)**, **service discovery**, and **fault-tolerance**. They provide practical insights into building scalable, efficient, and resilient systems using advanced patterns like **Publish/Subscribe**, **watchdogs**, and **real-time** data management.
