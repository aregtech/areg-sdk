# Examples

```
This file is part of AREG SDK and describes the examples
Copyright (c) 2017-2022, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

## Quick Build

The AREG SDK provides a collection of examples that showcase the engine and framework features. 

Please note that examples labeled as **IPC** (Inter-Process Communication) require the utilization of [_mcrouter_](https://github.com/aregtech/areg-sdk/tree/master/framework/mcrouter) and involve _Public_ services. To successfully test IPC applications, ensure that `mcrouter` is running. Examples not marked as **IPC** only involve _Local_ services and do not require `mcrouter` usage.

To compile the examples, you must first build the _areg_ library and _mcrouter_. The examples can be built using `cmake`, `make`, or `msbuild`. To enable or disable the building of examples, utilize the `AREG_BUILD_EXAMPLES` option in `make` or `cmake`. By default, it is enabled. To disable it, set it to `OFF` or `0`.

Example of quick build using `cmake`:
```bash
cmake -B ./build
cmake --build ./build -j 8
```

Example of quick build using `make`:
```bash
make all -j 8
```

Example of quick build using `msbuild`:
```bash
msbuild .
```

For more detailed information about the build process and the supported options, refer to the [Software-build](https://github.com/aregtech/areg-sdk/wiki/03.-Software-build) Wiki page of the AREG SDK.

---

# List of Examples

Here is a list of examples available in the AREG SDK:

0. [00_helloservice](https://github.com/aregtech/areg-sdk/tree/master/examples/00_helloservice) - A multithreading and multiprocessing application used as an example in the [Hello Service!](https://github.com/aregtech/areg-sdk/blob/master/docs/HelloService.md), which is a part of the [developer guide](https://github.com/aregtech/areg-sdk/blob/master/docs/DEVELOP.md).
1. [01_hello](https://github.com/aregtech/areg-sdk/tree/master/examples/01_hello) - A multithreading console application that outputs the "Hello World" message.
2. [02_buffer](https://github.com/aregtech/areg-sdk/tree/master/examples/02_buffer) - A multithreading console application demonstrating streaming in a shared buffer.
3. [03_file](https://github.com/aregtech/areg-sdk/tree/master/examples/03_file) - A console application demonstrating working with text and binary files, including the use of file name masks.
4. [04_trace](https://github.com/aregtech/areg-sdk/tree/master/examples/04_trace) - A multithreading console application for starting and using logging.
5. [05_timer](https://github.com/aregtech/areg-sdk/tree/master/examples/05_timer) - A multithreading console application for starting the Timer Service and using various timers.
6. [06_threads](https://github.com/aregtech/areg-sdk/tree/master/examples/06_threads) - A multithreading console application for starting and stopping various threads.
7. [07_synch](https://github.com/aregtech/areg-sdk/tree/master/examples/07_synch) - A multithreading console application demonstrating the use of various combined synchronization objects.
8. [08_service](https://github.com/aregtech/areg-sdk/tree/master/examples/08_service) - A multithreading console application for starting an empty servicing component.
9. [09_svcmulti](https://github.com/aregtech/areg-sdk/tree/master/examples/09_svcmulti) - A multithreading console application for creating multiple instances of the same servicing component.
10. [10_locsvc](https://github.com/aregtech/areg-sdk/tree/master/examples/10_locsvc) - A multithreading console application demonstrating a _Local_ servicing component.
11. [11_locmesh](https://github.com/aregtech/areg-sdk/tree/master/examples/11_locmesh) - A multithreading console application demonstrating multiple instances of the same _Local_ service.
12. [12_pubsvc](https://github.com/aregtech/areg-sdk/tree/master/examples/12_pubsvc) - A multiprocessing (**IPC**) application demonstrating a _Public_ service and the service client.
13. [13_pubmesh](https://github.com/aregtech/areg-sdk/tree/master/examples/13_pubmesh) - A multiprocessing (**IPC**) application demonstrating a mesh of _Public_ and _Local_ services, along with the use of service clients.
14. [14_pubtraffic](https://github.com/aregtech/areg-sdk/tree/master/examples/14_pubtraffic) - A multiprocessing (**IPC**) application demonstrating dynamic model creation and custom event processing.
15. [15_pubworker](https://github.com/aregtech/areg-sdk/tree/master/examples/15_pubworker) - A multiprocessing (**IPC**) application demonstrating the definition of a _Worker Thread_ in the model and custom event processing.
16. [16_pubfsm](https://github.com/aregtech/areg-sdk/tree/master/examples/16_pubfsm) - A multiprocessing (**IPC**) application demonstrating the work of a Finite State-Machine (FSM) using AREG framework features.
17. [17_winchat](https://github.com/aregtech/areg-sdk/tree/master/examples/17_winchat) - A Windows GUI application (**IPC**) demonstrating dynamic modeling, fault tolerance, and a distributed system.
18. [18_locwatchdog](https://github.com/aregtech/areg-sdk/tree/master/examples/18_locwatchdog) - A multithreading application demonstrating the watchdog feature with a local service.
19. [19_pubwatchdog](https://github.com/aregtech/areg-sdk/tree/master/examples/19_pubwatchdog) - A multiprocessing (**IPC**) application demonstrating the watchdog feature with a local service.
20. [20_pubdatarate](https://github.com/aregtech/areg-sdk/tree/master/examples/20_pubdatarate) - A multiprocessing (**IPC**) application demonstrating network communication data rate.
21. [21_pubunblock](https://github.com/aregtech/areg-sdk/tree/master/examples/21_pubunblock) - A multiprocessing (**IPC**) application demonstrating manual unblocking of requests to process more pending requests.

Please note that the list may be updated periodically. We recommend checking the list regularly.

For more details about each project, refer to the _ReadMe.txt_ file of each example project.