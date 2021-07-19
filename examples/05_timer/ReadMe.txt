========================================================================
    CONSOLE APPLICATION : 05_timer Project Overview
========================================================================

This project demonstrates how to setup and start timers. There are following timers set:
- one time timer (it is triggered only once)
- periodic timer (it is triggered certain ammount of times)
- continues timer (it is triggered and runs continuesly until stopped).

Before timers are started, the Timer Manager (time service) should be started.
The timers are automatically dispatched in a dispatching thread.
The application starts Timer Manager, creates separate 2 dispatcher threads, where each of them have 3 timers (one time, periodic, continues).
Since the objects of Dispatcher Thread and same, the timers and the threads differ by names.
The application runs for 30 seconds, stops timers and exit threads.
All timer outputs are written in log file.