```
This file is part of AREG SDK and describes the examples
Copyright (c) Aregtech, 2017-2021
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
============================================
# Examples

AREG SDK contains various of examples
The AREG SDK contains various examples to demonstrate features of the framework.  **To run IPC examples, make sure the process _mcrouter_ has started**. The [_mcrouter_](./../framework/mcrouter/) is a part of the AREG framework.

Below is the list of examples:

1.  [01_hello](./01_hello/) -- multithreading console application to output "Hello World" message.
2.  [02_buffer](./02_buffer/) -- multithreading console application to demonstrate work with shared buffer (stream).
3.  [03_file](./03_file/) -- console application to demonstrate work with test/binary files and masks in file paths.
4.  [04_trace](./04_trace/) -- multithreading console application to start and use logging.
5.  [05_timer](./05_timer/) -- multithreading console application to start Timer Service and use various timers.
6.  [06_threads](./06_threads/) -- multithreading console application to start and stop various threads.
7.  [07_synch](./07_synch/) -- multithreading console application to use various combined synchronization objects.
8.  [08_service](./08_service/) -- multithreading console application to start empty servicing component.
9.  [09_svcmulti](./09_svcmulti/) -- multithreading console application to create multiple instances of the same servicing component.
10. [10_locsvchello](./10_locsvchello/) -- multithreading console application to demonstrate _Local_ servicing component.
11. [11_locsvcmix](./11_locsvcmix/) -- multithreading console application to demonstrate multi-instances of same _Local_ service.
12. [12_pubsvchello](./12_pubsvchello/) -- multiprocessing (**IPC**) application to demonstrate _Public_ service and the service client.
13. [13_pubsvcmix](./13_pubsvcmix/) -- multiprocessing (**IPC**) application to demonstrate mesh of _Public_ and _Local_ services and the use of service clients.
14. [14_pubsvclight](./14_pubsvclight/) -- multiprocessing (**PIC**) application to demonstrate dynamic model creation and the custom event processing.
15. [15_pubsvcworker](./15_pubsvcworker/) -- multiprocessing (**PIC**) application to demonstrate _Worker Thread_ definition in the model and custom event processing.
16. [16_pubfsm](./16_pubfsm/) -- multiprocessing (**IPC**) application to demonstrate work of Finite State-Machine created by features of AREG framework.
16. [17_17_winchat](./17_winchat/) -- Windows GUI application (**IPC**) to demonstrate dynamic modeling, fault tolerant and distributed system, and mist network.

The list will be updated. We recommend periodically checking the list.

For more details of every project, read _ReadMe.txt_ file of each example project.
