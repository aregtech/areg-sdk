# Examples
```
This file is part of AREG SDK and describes the examples
Copyright (c) 2017-2022, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
The AREG SDK contains various examples to demonstrate features of the engine and framework. 

> 💡 **To run _Public_ services of examples based on IPC, make sure the process _mcrouter_ has started**. Otherewise, the service is no visible in the network. The [_mcrouter_](./../framework/mcrouter/) is a part of the AREG SDK.

To compile examples, call one of these from command line:

```shell
# Compile areg-sdk sources and examples. The option 'all' is optional
$ make all
```
If `areg` library is already compiled, call
```shell
# Compile only examples
$ make examples
```

Below is the list of examples:

0.  [00_helloservice](./00_helloservice) -- multithreading and multiprocessing applications used as an example in [developer guide](./../docs/DEVELOP.md).
1.  [01_hello](./01_hello/) -- multithreading console application to output "Hello World" message.
2.  [02_buffer](./02_buffer/) -- multithreading console application to demonstrate streaming in shared buffer.
3.  [03_file](./03_file/) -- console application to demonstrate work with text and binary files, and use of file name masks.
4.  [04_trace](./04_trace/) -- multithreading console application to start and use logging.
5.  [05_timer](./05_timer/) -- multithreading console application to start Timer Service and use various timers.
6.  [06_threads](./06_threads/) -- multithreading console application to start and stop various threads.
7.  [07_synch](./07_synch/) -- multithreading console application to use various combined synchronization objects.
8.  [08_service](./08_service/) -- multithreading console application to start empty servicing component.
9.  [09_svcmulti](./09_svcmulti/) -- multithreading console application to create multiple instances of the same servicing component.
10. [10_locsvc](./10_locsvc/) -- multithreading console application to demonstrate _Local_ servicing component.
11. [11_locmesh](./11_locmesh/) -- multithreading console application to demonstrate multi-instances of same _Local_ service.
12. [12_pubsvc](./12_pubsvc/) -- multiprocessing (**IPC**) application to demonstrate _Public_ service and the service client.
13. [13_pubmesh](./13_pubmesh/) -- multiprocessing (**IPC**) application to demonstrate mesh of _Public_ and _Local_ services and the use of service clients.
14. [14_pubtraffic](./14_pubtraffic/) -- multiprocessing (**IPC**) application to demonstrate dynamic model creation and the custom event processing.
15. [15_pubworker](./15_pubworker/) -- multiprocessing (**IPC**) application to demonstrate _Worker Thread_ definition in the model and custom event processing.
16. [16_pubfsm](./16_pubfsm/) -- multiprocessing (**IPC**) application to demonstrate the work of Finite State-Machine (FSM) using AREG framework features.
17. [17_winchat](./17_winchat/) -- Windows GUI application (**IPC**) to demonstrate dynamic modeling, fault tolerant and distributed system.
18. [18_locwatchdog](./18_locwatchdog) -- multithreading application to demonstrate watchdog feature with local service.
19. [19_pubwatchdog](./19_pubwatchdog) -- multiprocessing (**IPC**) application to demonstrate watchdog feature with local service.
20. [20_pubdatarate](./20_pubdatarate) -- multiprocessing (**IPC**) application to demonstrate the network communication data rate.

The list will be updated. We recommend periodically checking the list.

For more details of every project, read _ReadMe.txt_ file of each example project.
