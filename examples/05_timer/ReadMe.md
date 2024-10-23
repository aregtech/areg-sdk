# 05_timer Project Overview

The **[05_timer](./05_timer)** project demonstrates the use of timers within multithreaded applications. It shows how to initialize, start, and stop multiple timers, each managed in separate threads. To use timers, the *Timer Service* (or *Timer Manager*) must be active. Timers are automatically handled in the thread of their respective owners.

The project includes the following types of timers:

- **One-time timer**: Executes a task once and then stops.
- **Periodic timer**: Executes a task repeatedly for a defined number of times.
- **Continuous timer**: Executes a task continuously until manually stopped.

This project highlights how to implement and manage timers for various scenarios.
