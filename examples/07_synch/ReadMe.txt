========================================================================
    CONSOLE APPLICATION : 07_synch Project Overview
========================================================================

This project demonstrates how to use synchronization objects and how to wait for one or more synchronization events.
The waiting of an event can be combination of different types of synchronization objects. 
For example, the thread can wait to one or all events like "released mutex" and "event".
The synchronization event is not a default synchronization object in POSIX, but exist in Windows.
The project runs indentical in Windows and POSIX (Linux).

The following synchronization objects are used: Mutex and Event.
The Mutex is recursive, so that if can be multiple times locked within one thread.
The Events are auto- or manual-reset.
Threads wait for more than one synchronization objects and they can be the same type of events or mixture.
A thread can wait for one of events or all events in the synchronization wating list.
