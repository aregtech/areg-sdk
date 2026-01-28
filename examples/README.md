# Areg SDK Examples

```
This document is part of the Areg SDK and describes its example projects.
Copyright (c) 2017-2026, Aregtech
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

---

## Introduction

The **Areg SDK** is a framework for building **high-performance, real-time, and distributed applications**. It excels in:

* 🧵 **Multithreading** – Efficient handling of concurrent tasks within a single process.
* 🚀 **Multiprocessing / IPC** – Seamless inter-process communication across distributed systems.
* ⚙️ **Service Discovery & Fault-Tolerance** – Automatic service registration, recovery, and resilient operation.
* ⏱ **Event-Driven Architecture** – Real-time event handling with **timers**, **state-machines**, and **worker threads**.

This repository contains **example projects** illustrating Areg SDK capabilities. Each example provides practical insights into building scalable, robust, and efficient applications.

---

## Quick Start – Build & Run

> [!IMPORTANT]
> **Requirements:** Ensure you have **CMake 3.20+** or **MSVS 2019+**. Ensure you have **Java17+** on your machine for code generator.

1. **Clone the Repository**

```bash
git clone https://github.com/aregtech/areg-sdk.git
cd areg-sdk
```

2. **Build with CMake**

```bash
cmake -B ./build
cmake --build ./build -j 20
```

> [!NOTE]
> By default, `AREG_EXAMPLES` is `ON` to include examples. For advanced build options, see [Building Areg SDK with CMake](./../docs/wiki/01b-cmake-build.md).

3. **Build with Visual Studio**

```console
MSBuild ./areg-sdk.sln
```

### Developer Notes

* **IPC Projects**: Multiprocessing examples require **mtrouter** for inter-process communication.
* **Fault-Tolerance**: Services work reliably regardless of startup order due to automatic discovery and recovery.
* **Generated Projects**: Projects with names `NN_generated` are created from **Service Interface** (`.siml`) documents during build.
* **Windows-only Projects**: [20\_winchat](./20_winchat/) requires **MFC** and MSVC/ClangCL compilers.

---

## Example Projects

| Project                                | Scope              | Highlights                         | Key Feature / Solution                                                                                              |
| -------------------------------------- | ------------------ | ---------------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| [01\_minimalrpc](./01_minimalrpc/)     | 🧵 Multithreading  | Object RPC, Minimal Setup          | Minimal thread-to-thread RPC with auto-discovery; provider handles request and signals app quit.                    |
| [02\_minimalipc](./02_minimalipc/)     | 🚀 Multiprocessing | IPC, Minimal Setup                 | Demonstrates minimal inter-process communication setup; illustrates message exchange across processes via mtrouter. |
| [03\_helloservice](./03_helloservice/) | 🚀 Multiprocessing | Service/Client, Threading Model    | Introduces service creation and client-server interactions; flexible threading and process models for IPC.          |
| [04\_hellothread](./04_hellothread/)   | 🧵 Multithreading  | Thread Lifecycle                   | Demonstrates thread creation, management, and synchronization; prints "Hello World!" in a separate thread.          |
| [05\_buffer](./05_buffer/)             | 🧵 Multithreading  | Shared Memory, Serialization       | Shows how to serialize data into a shared buffer, pass it between threads, and deserialize it in another thread.    |
| [06\_file](./06_file/)                 | 🧵 Multithreading  | File Handling                      | Demonstrates reading, writing, copying, and deleting files in text and binary formats across threads.               |
| [07\_logging](./07_logging/)           | 🧵 Multithreading  | Logging, Tracing                   | Shows application method call tracing and message logging for debugging and runtime analysis.                       |
| [08\_timer](./08_timer/)               | 🧵 Multithreading  | Timers, Real-Time Events           | Demonstrates one-shot, periodic, and continuous timers for real-time event handling in threads.                     |
| [09\_threads](./09_threads/)           | 🧵 Multithreading  | Event Dispatching, Threads         | Illustrates creating custom event dispatching threads for real-time multithreaded applications.                     |
| [10\_sync](./10_sync/)                 | 🧵 Multithreading  | Mutex, Events                      | Demonstrates synchronization primitives (mutexes, events) for safe multithreaded operations.                        |
| [11\_service](./11_service/)           | 🧵 Multithreading  | Service Provider                   | Shows single-threaded service provider creation and deployment within an application model.                         |
| [12\_svcmulti](./12_svcmulti/)         | 🧵 Multithreading  | Service Reuse                      | Demonstrates service reuse across multiple threads and processes within an application model.                       |
| [13\_locsvc](./13_locsvc/)             | 🧵 Multithreading  | Local Service                      | Introduces a local service; demonstrates Service Provider and Consumer communication in a single process.           |
| [14\_locmesh](./14_locmesh/)           | 🧵 Multithreading  | Service Mesh                       | Shows a mesh of local services communicating across multiple threads for real-time interactions.                    |
| [15\_pubsvc](./15_pubsvc/)             | 🚀 Multiprocessing | Public Service, IPC                | Demonstrates a public service provider and client communication across processes using Object RPC.                  |
| [16\_pubmesh](./16_pubmesh/)           | 🚀 Multiprocessing | Mesh of Public Services            | Illustrates a networked mesh of public and local services with IPC and process-level integration.                   |
| [17\_pubtraffic](./17_pubtraffic/)     | 🚀 Multiprocessing | Dynamic Model, Events              | Demonstrates dynamic application model creation and event handling in a multi-process IPC system.                   |
| [18\_pubworker](./18_pubworker/)       | 🚀 Multiprocessing | Worker Threads, Custom Events      | Shows worker thread usage for handling custom events in multithreaded IPC scenarios.                                |
| [19\_pubfsm](./19_pubfsm/)             | 🚀 Multiprocessing | FSM, State-Driven                  | Implements a finite state machine to control public service behavior with IPC support.                              |
| [20\_winchat](./20_winchat/)           | 🚀 Multiprocessing | Windows Chat, Real-Time            | Demonstrates a Windows-based chat application using IPC with fault-tolerant public services.                        |
| [21\_locwatchdog](./21_locwatchdog/)   | 🧵 Multithreading  | Watchdog, Fault-Tolerance          | Implements a local service watchdog to monitor threads and automatically restart failed services.                   |
| [22\_pubwatchdog](./22_pubwatchdog/)   | 🚀 Multiprocessing | Watchdog, IPC                      | Extends watchdog functionality to monitor public services across processes, ensuring fault-tolerant operations.     |
| [23\_pubdatarate](./23_pubdatarate/)   | 🚀 Multiprocessing | Data Rate, IPC                     | Measures data transfer rates between public services and clients; useful for performance benchmarking.              |
| [24\_pubunblock](./24_pubunblock/)     | 🚀 Multiprocessing | Manual Unblocking, Throughput      | Demonstrates manually unblocking service requests to optimize handling in asynchronous communication.               |
| [25\_pubsub](./25_pubsub/)             | 🚀 Multiprocessing | Pub/Sub, IPC                       | Implements the Publish/Subscribe pattern with notifications on change or on every update for subscribed clients.    |
| [26\_pubsubmix](./26_pubsubmix/)       | 🚀 Multiprocessing | Hybrid Pub/Sub, Fault-Tolerance    | Shows mixed Pub/Sub configuration across threads and processes; maintains reliability during network interruptions. |
| [27\_pubsubmulti](./27_pubsubmulti/)   | 🚀 Multiprocessing | Multi-Subscriber, Efficient Events | Optimizes Pub/Sub with multiple subscribers per thread, reducing event overhead and improving efficiency.           |
| [28\_stlsync](./28_stlsync/)           | 🧵 Multithreading  | Synchronization Event and STL      | Event-based synchronization with STL threads for safe multithreading.                                               |
| [29_syncevent](./29_syncevent/)        | 🧵 Multithreading  | Auto-Reset Events, Signal Persistence | Demonstrates reliable Areg `SyncEvent` signaling: events stay signaled until locked, no spurious wakeups, works the same on Windows and Linux. |


---

## Conclusion

These examples demonstrate **Areg SDK's core strengths**:

* Scalable **multithreaded** and **multi-process** applications
* **Fault-tolerant** services with automatic recovery
* **Real-time, event-driven** systems
* Complex **Publish/Subscribe** and **service mesh** architectures

Use them as **practical references** for understanding patterns, best practices, and high-performance solutions provided by Areg SDK.
