# 05_timer Project Overview

The [05_timer](https://github.com/aregtech/areg-sdk/tree/master/examples/05_timer) project showcases the usage of timers in applications. It demonstrates the initialization, starting, and stopping of multiple timers, each processed in different threads. To utilize timers, the *Timer Service* (also known as *Timer Manager*) needs to be started. The timers are automatically dispatched in the thread of the timer owner.

The project utilizes the following types of timers:
- One-time timer: Executes a task only once.
- Periodic timer: Executes a task a certain number of times.
- Continuous timer: Executes a task continuously until it is stopped.
