========================================================================
    CONSOLE APPLICATION : 05_timer Project Overview
========================================================================

This project demonstrates use of timers. It explains how to initialize, start and stop timers.

There are following timers are used:
- one time timer  (it runs only once)
- periodic timer  (it runs certain amount of times)
- continues timer (it runs continuously until stopped).

The timer requires the start of Timer Manager (timer service), which normally starts when initializing an application.
The timers are automatically dispatched in the timer owner thread.
