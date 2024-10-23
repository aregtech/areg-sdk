## 18_locwatchdog Project Overview

The **18_locwatchdog** project demonstrates the use of a *Watchdog* in a *Local Service*. The watchdog guards threads by setting a timeout, restarting any thread that fails to respond within that time. Notifications of thread termination and restart are sent to all *Service Providers* and *Service Consumers*.

### Sub-Projects:

1. **18_generated:** Generated from [HelloWatchdog.siml](./res/HelloWatchdog.siml) during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.

2. **[18_locservice](./locservice/):** A multithreaded application with a *Local Service* not accessible externally. It contains two threads: one for the *Service Provider* and one for the *Service Consumer*. Logs track the watchdog’s behavior.
