
# AREG SDK Examples

```
This document is part of the AREG SDK and describes its examples.
Copyright © 2017-2023, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

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

For more detailed build instructions and available options, refer to the [Build Documentation](./../docs/wiki/BUILD.md) on the AREG SDK Wiki.

---

## AREG SDK Examples Overview

The AREG SDK includes a variety of examples that showcase its features, such as multithreading, multiprocessing, synchronization, and service components.

> [!IMPORTANT]
> Examples marked with **IPC** (*Inter-Process Communication*) require running the **mcrouter** to enable inter-process communication during testing. You can start **mcrouter** from the build directory as a console application.

> [!NOTE]
> The startup order of the applications does not affect system behavior. Since the **AREG Framework** supports fault tolerance, the system operates smoothly as long as the Service Provider, Service Consumer, and the Multicast Router (**mcrouter**) are running, regardless of their startup sequence.

### Available Examples:

1. **[00_helloservice](./examples/00_helloservice):**  
   A multithreaded and multiprocess application used in the [Hello Service Guide](https://github.com/aregtech/areg-sdk/blob/master/docs/HelloService.md).

2. **[01_hello](./examples/01_hello):**  
   A basic multithreaded console application that outputs "Hello World."

3. **[02_buffer](./examples/02_buffer):**  
   Demonstrates multithreaded streaming in a shared buffer.

4. **[03_file](./examples/03_file):**  
   Shows how to work with text and binary files, including file name masks.

5. **[04_trace](./examples/04_trace):**  
   Demonstrates logging and tracing in a multithreaded environment.

6. **[05_timer](./examples/05_timer):**  
   Demonstrates the Timer Service and various timer functions in a multithreaded environment.

7. **[06_threads](./examples/06_threads):**  
   Shows how to start and stop multiple threads.

8. **[07_synch](./examples/07_synch):**  
   Demonstrates multithreaded synchronization using various objects.

9. **[08_service](./examples/08_service):**  
   Demonstrates starting an empty service component in a multithreaded environment.

10. **[09_svcmulti](./examples/09_svcmulti):**  
    Demonstrates creating multiple instances of the same service component using multithreading.

11. **[10_locsvc](./examples/10_locsvc):**  
    Demonstrates a local service component in a multithreaded application.

12. **[11_locmesh](./examples/11_locmesh):**  
    Demonstrates multiple instances of a local service using multithreading.

13. **[12_pubsvc](./examples/12_pubsvc):**  
    **IPC** example of a Public Service where the provider and consumer run in separate processes. Multiple consumers can be started, but only one provider (*micro-server*) should run.

14. **[13_pubmesh](./examples/13_pubmesh):**  
    **IPC** example demonstrating a mesh of Public and Local services running across multiple processes and threads.

15. **[14_pubtraffic](./examples/14_pubtraffic):**  
    **IPC** example of a Public Service with data attributes and timers. Multiple consumers can be started, but only one provider (*micro-server*) should run.

16. **[15_pubworker](./examples/15_pubworker):**  
    **IPC** example where the Service Provider contains both a Public Service and a Worker Thread.

17. **[16_pubfsm](./examples/16_pubfsm):**  
    **IPC** example of a Service Provider with two Public Service Interfaces and an integrated Finite State-Machine (**FSM**). Multiple consumers can be started, but only one provider (*micro-server*) should run.

18. **[17_winchat](./examples/17_winchat):**  
    **IPC** GUI application that showcases dynamic modeling, fault tolerance, and distributed systems. A single instance of the **register** application must be started, multiple instances of the **chatter** application. Each **chatter** instance dynamically creates and loads its model when a "chat" is initiated. Even if the **register** application is closed, the system remains fully operational, and communication between the **chatter** instances continues without interruption.

19. **[18_locwatchdog](./examples/18_locwatchdog):**  
    A local service example with a Watchdog, where the provider thread restarts when the Watchdog timeout expires.

20. **[19_pubwatchdog](./examples/19_pubwatchdog):**  
    **IPC** example of a Public Service with a Watchdog in a separate process. The provider thread restarts when the Watchdog timeout expires.

21. **[20_pubdatarate](./examples/20_pubdatarate):**  
    **IPC** example transferring large amounts of dummy data, demonstrating the data transfer rate over the network.

22. **[21_pubunblock](./examples/21_pubunblock):**  
    **IPC** example showing how to manually unblock pending requests, allowing them to be triggered multiple times while request processing is still ongoing. Once the request is processed, the response is manually prepared and sent. The **AREG Framework** ensures that responses are delivered to the correct Consumer without mixing.

23. **[22_pubsub](./examples/22_pubsub):**  
    **IPC** example demonstrating the PubSub feature, where one process acts as a Publisher and others as Subscribers.

24. **[23_pubsubmix](./examples/23_pubsubmix):**  
    **IPC** example demonstrating the interaction of Publishers and Subscribers across processes. It highlights how the PubSub system handles data updates and communication resilience, even in cases where inter-process communication is disrupted—such as when the **mcrouter** is stopped and restarted. The example shows how the system recovers and continues to function once communication is reestablished.

25. **[24_pubsubmulti](./examples/24_pubsubmulti):**  
    **IPC** example demonstrating the interaction of Publishers and Subscribers across multiple threads and multiple processes.

### More Information:
For detailed information on each example, refer to the `ReadMe.md` file inside each project folder. The list of examples is updated regularly, so check back for new additions.
