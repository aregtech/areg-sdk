========================================================================
    CONSOLE APPLICATION : 07_synch Project Overview
========================================================================

This project demonstrates the use of synchronization objects, including 
waiting for multiple synchronization objects such as mutex and 
synchronization events, that differ by types such as automatic or manual
reset, or initially locked or unlocked, etc, and how they may be combined
in one waiting scope. For example, a thread can wait for one, many or a 
mixture of synchronization objects such as Mutex or synchronization events.

The synchronization events are not part of POSIX standard, but exists in 
Windows. By business logic they have similarity with POSIX signals of 
conditional variables. Synchronization events are powerful and useful objects. 
