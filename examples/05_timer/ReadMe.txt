========================================================================
    CONSOLE APPLICATION : 05_timer Project Overview
========================================================================

This project demonstrates use of timers. The demo initializes, starts 
and stops multiple timers processed in different threads. The timers
require the start of Timer Manager (timer service). The timers are 
automatically dispatched in the timer owner thread.


Following timers are used:
- one time timer  (it runs only once)
- periodic timer  (it runs certain amount of times)
- continues timer (it runs continuously until stopped).
