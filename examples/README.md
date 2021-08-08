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

1.  [01_hello](./01_hello/)         -- multithreading console application to output "Hello World" message.
2.  [02_buffer](./02_buffer/)       -- multithreading console application to demonstrate work with shared buffer (stream).
3.  [03_file](./03_file/)           -- console application to demonstrate work with test/binary files and masks in file paths.
4.  [04_trace](./04_trace/)         -- multithreading console application to start and use logging.
5.  [05_timer](./05_timer/)         -- multithreading console application to start Timer Service and use various timers.
6.  [06_threads](./06_threads/)     -- multithreading console application to start and stop various threads.
7.  [07_synch](./07_synch/)         -- multithreading console application to use various combined synchronization objects.
8.  [08_service](./08_service/)     -- multithreading console application to start empty servicing component.
9.  [09_duplicate](./09_duplicate/) -- multithreading console application to duplicate instances of the same servicing component.
10. [10_svchello](./10_svchello/)   -- multithreading console application to demonstrate _Local_ servicing component.
11. [11_svcmix](./11_svcmix/)       -- multithreading console application to demonstrate multi-instances of same _Local_ service.
12. [12_ipchello](./12_ipchello/)   -- multiprocessing (**IPC**) application to demonstrate _Remote_ servicing component and client.
13. [13_ipcmix](./13_ipcmix/)       -- multiprocessing (**IPC**) application to demonstrate meshing and mixture of _Local_ and _Remote_ services.
15. [14_ipclight](./14_ipclight/)   -- multiprocessing (**PIC**) application to demonstrate create and load model dynamically, send and process custom event.
14. [messager](./messager/)         -- Windows GUI application (**IPC**) to demonstrate dynamic modeling, fault tolerant environment and mist network.

The list will be updated. We recommend periodically checking the list.

For more details of every project, read _ReadMe.txt_ file of each example project.
