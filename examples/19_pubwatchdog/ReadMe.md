## 19_pubwatchdog Project Overview

[19_pubwatchdog](https://github.com/aregtech/areg-sdk/tree/master/examples/19_pubwatchdog) demonstrates the utilization of a *Watchdog* in a Public Service. The Watchdog acts as a guard to control threads, allowing developers to set a timeout in milliseconds. Threads that fail to respond within the specified runtime will be terminated and restarted by the system. When a thread is killed, all Service Providers and Service Consumers receive a *connection lost* notification. Upon restarting, they receive a *service available* or *client connection* notification, enabling them to resume communication. The watchdog can be disabled for a specific thread by setting a timeout of *0ms*.

In this example application, the watchdog is triggered by putting the thread to sleep with a timeout greater than the watchdog's timeout. Once the watchdog timeout is reached, the system kills and restarts the thread.

The project consists of three sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/19_pubwatchdog/generated) (19_generated): This static library contains the generated codes and common objects. It is generated from the [HelloWatchdog.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/18_locwatchdog/res/HelloWatchdog.siml) Service Interface document using the `codegen.jar` tool.

2. [pubservice](https://github.com/aregtech/areg-sdk/tree/master/examples/19_pubwatchdog/pubservice) (19_pubservice): This application provides a Public Service. The thread responsible for providing the service is declared with a watchdog timeout greater than 0ms. When a Service Consumer sends a request, the thread goes to sleep. Once the watchdog timeout expires, the system kills and restarts the thread. All other Service Providers and Consumers automatically receive *disconnect* and *connect* notifications.

3. [pubclient](https://github.com/aregtech/areg-sdk/tree/master/examples/19_pubwatchdog/pubclient) (19_pubclient): This application instantiates the Service Consumer object. By connecting with the Service Provider, it sends a request to put the thread to sleep, triggering the Watchdog restart action.

Communication within the system is facilitated by `mcrouter`, a multicast router that operates on any machine within the network. The AREG SDK automates service discovery and ensures a fault-tolerant system, making the order of process startup irrelevant.
