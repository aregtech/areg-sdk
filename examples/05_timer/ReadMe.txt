========================================================================
    CONSOLE APPLICATION : 05_timer Project Overview
========================================================================

This project demonstrates use of timers.
It explains how to initialize, start and stop timer.
There are following timers are used:
- one time timer  (it runs only once)
- periodic timer  (it runs certain ammount of times)
- continues timer (it runs continuously until stopped).

The timer requires start of Timer Manager (timer service), which is normally starts when initialize application.
The timers are automatically dispatched in timer owner thread.
