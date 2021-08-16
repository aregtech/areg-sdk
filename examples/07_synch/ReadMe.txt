========================================================================
    CONSOLE APPLICATION : 07_synch Project Overview
========================================================================

This project demonstrates the use of synchronization objects, including waiting for multiple synchronization objects 
and events. The waiting of multiple synchronization objects can differ by types. For example, a thread can wait for 
one, many or a mixture of synchronization objects such as Mutex or SynchEvents (synchronization events). The 
synchronization events are not part of POSIX standard, but exists in Windows. By business logic they are similar to 
POSIX signals of conditional variables. It is a powerful and useful object to use. 

The example creates several Mutex and SynchEvent objects, it waits for one or a mixture of synchronization objects. 
AREG SDK uses recursive mutex, auto- and manual-reset SynchEvents.
