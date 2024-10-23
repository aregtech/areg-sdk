## 19_pubwatchdog Project Overview

The **[19_pubwatchdog](./pubwatchdog)** project demonstrates using a *Watchdog* in a *Public Service*. The watchdog controls threads by setting a timeout. Threads that fail to respond are terminated and restarted, with notifications sent to all *Service Providers* and *Service Consumers*.

### Sub-Projects:

1. **19_generated:** Generated from [HelloWatchdog.siml](./res/HelloWatchdog.siml) during CMake configuration or as a pre-build action in Visual Studio's `dummy` project.

2. **[19_pubclient](./pubclient/):** A *Service Consumer* application that connects with the *Service Provider*, sending requests to simulate a delay to trigger the watchdog.

3. **[19_pubservice](./pubservice/):** A *Public Service* application where the thread responsible for the service is controlled by a watchdog. The thread goes to sleep upon receiving a request, which simulates a crash, and the Watchdog  it after the timeout:
  - destroys the thread;
  - sends *service disconnected* notification to all connected service consumers;
  - restarts the thread with services;
  - *service consumers*  automatically receive *service connected* notification;
  - service consumers can continue communication and send requests.

Communication is facilitated by `mcrouter`, and the AREG SDK ensures fault tolerance and service discovery automation, making the process startup order irrelevant.
