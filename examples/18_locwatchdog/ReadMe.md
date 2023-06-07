## 18_locwatchdog Project Overview

[18_locwatchdog](https://github.com/aregtech/areg-sdk/tree/master/examples/18_locwatchdog) demonstrates the utilization of a watchdog in a Local Service. The watchdog acts as a guard to control threads, allowing developers to set a timeout in milliseconds. Threads that fail to respond within the specified runtime will be terminated and restarted by the system. When a thread is killed, all Service Providers and Service Consumers receive a *connection lost* notification. Upon restarting, they receive a *service available* or *client connection* notification, enabling them to resume communication. The watchdog can be disabled for a specific thread by setting a timeout of *0ms*.

In this example application, the watchdog is triggered by putting the thread to sleep with a timeout greater than the watchdog's timeout. Once the watchdog timeout is reached, the system kills and restarts the thread.

The project consists of two sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/18_locwatchdog/generated) (18_generated): This static library contains the generated codes and common objects. It is generated from the [HelloWatchdog.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/18_locwatchdog/res/HelloWatchdog.siml) Service Interface document using the `codegen.jar` tool.

2. [locservice](https://github.com/aregtech/areg-sdk/tree/master/examples/18_locwatchdog/locservice) (18_locservice): This sub-project is a multithreaded application with a Local Service that is not accessible outside of the process. It contains two threads, one for the Service Provider and one for the Service Consumer. Developers can analyze the logs to track the execution and behavior of the watchdog in the application.
