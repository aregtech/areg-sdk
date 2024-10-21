
# AREG SDK Examples

```
This document is part of the AREG SDK and describes its examples.
Copyright © 2017-2023, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

## Quick Build Guide

To get started with AREG SDK, follow these steps:

### 1. Clone the AREG SDK Repository
Clone the AREG SDK repository using the following command:

```bash
git clone https://github.com/aregtech/areg-sdk.git
```

### 2. Build the AREG SDK with CMake
Use CMake to build the SDK:

```bash
cmake -B ./build
cmake --build ./build -j 20
```

### 3. Build with Microsoft Visual Studio
For Microsoft Visual Studio users, open the `areg-sdk.sln` file and build the projects. Alternatively, use the command line:

```console
MSBuild ./areg-sdk.sln
```

> [!NOTE]
> To compile the example projects, ensure that the `AREG_EXAMPLES` option is **not** set to `OFF`. If `AREG_EXAMPLES=OFF`, examples will be excluded from the build.

For more detailed build instructions and options, visit the [Build Documentation](./../docs/wiki/BUILD.md) in the AREG SDK Wiki.

---

## AREG SDK Examples Overview

The AREG SDK includes a variety of examples demonstrating features like multithreading, IPC, synchronization, and service components.

> [!IMPORTANT]
> For examples marked with **IPC**, running the **mcrouter** is required. You can start **mcrouter** from the build directory to enable **inter-process communication** (*IPC*) during testing.


> [!NOTE]
> The order of starting the applications does not affect system behavior. The AREG SDK supports fault tolerance, meaning the system runs smoothly as long as the Service Provider, Service Consumer, and the Multicast Router (**mcrouter**) are up, regardless of their startup sequence.

### Available Examples:

1. **[00_helloservice](./examples/00_helloservice):**  
   A multithreaded and multiprocess application used in the [Hello Service Guide](https://github.com/aregtech/areg-sdk/blob/master/docs/HelloService.md).

2. **[01_hello](./examples/01_hello):**  
   A simple multithreaded console application that outputs "Hello World."

3. **[02_buffer](./examples/02_buffer):**  
   Demonstrates streaming in a shared buffer using multithreading.

4. **[03_file](./examples/03_file):**  
   Shows how to handle text and binary files, including file name masks.

5. **[04_trace](./examples/04_trace):**  
   Demonstrates logging and tracing in a multithreaded environment.

6. **[05_timer](./examples/05_timer):**  
   Demonstrates the Timer Service and various timer functions using multithreading.

7. **[06_threads](./examples/06_threads):**  
   Shows how to start and stop multiple threads.

8. **[07_synch](./examples/07_synch):**  
   Demonstrates synchronization with various objects in a multithreaded application.

9. **[08_service](./examples/08_service):**  
   Demonstrates starting an empty service component using multithreading.

10. **[09_svcmulti](./examples/09_svcmulti):**  
    Demonstrates creating multiple instances of the same service component in a multithreaded environment.

11. **[10_locsvc](./examples/10_locsvc):**  
    Demonstrates a local service component running in a multithreaded application.

12. **[11_locmesh](./examples/11_locmesh):**  
    Demonstrates multiple instances of a local service using multithreading.

13. **[12_pubsvc](./examples/12_pubsvc):**  
    **IPC** example of a Public Service, where the provider and consumer run in separate processes. Multiple consumers can be started, but only one provider should be running.

14. **[13_pubmesh](./examples/13_pubmesh):**  
    **IPC** example demonstrating a mesh of Public and Local services running across multiple processes and threads.

15. **[14_pubtraffic](./examples/14_pubtraffic):**  
    **IPC** example of a Public Service with data attributes and timers. One provider instance should run, with multiple consumers.

16. **[15_pubworker](./examples/15_pubworker):**  
    **IPC** example of a Public Service where the provider also contains a Worker Thread.

17. **[16_pubfsm](./examples/16_pubfsm):**  
    **IPC** example of a Service Provider with two Public Service Interfaces and an integrated Finite State-Machine (**FSM**). One provider instance and multiple consumer instances can be run.

18. **[17_winchat](./examples/17_winchat):**  
    **IPC** GUI application demonstrating dynamic modeling, fault tolerance, and distributed systems. One instance of the **register** application should be started, and multiple **chatter** instances can be run. The system remains operational even if **register** closes.

19. **[18_locwatchdog](./examples/18_locwatchdog):**  
    A local service example with a Watchdog, where the provider thread is restarted upon Watchdog timeout expiration.

20. **[19_pubwatchdog](./examples/19_pubwatchdog):**  
    **IPC** example of a Public Service where the provider runs in a thread with a Watchdog. The thread restarts when the Watchdog timeout expires.

21. **[20_pubdatarate](./examples/20_pubdatarate):**  
    **IPC** example transferring large amounts of dummy data, demonstrating network data rate.

22. **[21_pubunblock](./examples/21_pubunblock):**  
    **IPC** example showing how to manually unblock requests and trigger multiple consumers, while handling provider responses when unblocked request processing is completed.

23. **[22_pubsub](./examples/22_pubsub):**  
    **IPC** example demonstrating the PubSub feature, with one process as the Publisher and multiple Subscribers run in separate threads.

24. **[23_pubsubmix](./examples/23_pubsubmix):**  
    **IPC** example showing a mix of Publishers and Subscribers in processes and threads.

25. **[24_pubsubmulti](./examples/24_pubsubmulti):**  
    **IPC** example demonstrating multiple Publishers and multiple Subscribers across threads and processes.

### More Information:
For detailed information on each example, refer to the `ReadMe.md` file inside each project folder. The list of examples is updated regularly, so check back for new additions. 
