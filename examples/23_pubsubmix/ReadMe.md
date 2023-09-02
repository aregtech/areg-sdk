# 22_pubsub Project Overview

The [23_pubsubmix](https://github.com/aregtech/areg-sdk/tree/master/examples/23_pubsubmix) project showcases the utilization of a *Public Service* in a multi-processing environment, which acts list PubSub (Publish / Subscriber) service. In this example there is a mixture of Publishers and Subscribers utilizing same Service Interface, running in the same process, but different threads, or different processes. There are 2 types notifications to receive when subscribe on data, called 'Attributes', and it depends on the design:
- **On change**: receive update notifications only if the value is changed. In this example it is a string, which sends notification only when value is changed.
- **Always**   : receive update notification each time value is set even if the value is not changed.

The agenda is to demonstrate that the Subscribers, which run in the same process still receive the data update notification, while the network connection is broken. During the test, start one instance of `23_pubsubctrl` and multiple instances of `23_pubsubdyn`. During the run, you can stop and restart the router `mcrouter`, you can invalidate data, can start and stop Publisher, etc., and each time:
1. The local Subscribers remain alive and active, get the actual values, because run in the same process with the Publisher;
2. The remote Subscribers get disconnected notifications and wait for the next update. As soon as get connection, they receive the actual values and continue run.

The demonstration employing Object RPC (ORPC) for inter-process communication (IPC). The demonstration involves the creation of two applications: one serving as the *Service Provider* to act as a data Publisher and the other as the *Service Consumer* to act as a data Subscriber. The Subscriber can dynamically subscribe and unsubscribe on data update. All data update notifications receive automatically and the system guarantees that as soon as Subscriber assigns on data it receives the latest data even if it is not updated yet. This guaranties that if for any reason the Subscriber does not miss the update notification.

The project comprises three sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/23_pubsubmix/generated) (23_generated) - This sub-project contains a static library consisting of generated code derived from the [PubSubMix.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/23_pubsubmix/res/PubSubMix.siml) *Service Interface* document. The code generation is facilitated using the `codegen.jar` tool.

2. [common](https://github.com/aregtech/areg-sdk/tree/master/examples/22_pubsub/common) (23_common) - This sub-project is a static library consisting of common objects that are shared between other processes.

3. [pubsubctrl](https://github.com/aregtech/areg-sdk/tree/master/examples/23_pubsubmix/pubsubctrl) (23_pubsubctrl) - This sub-project represents a console application functioning as a network-discoverable *Public Service* provider, which acts as a Data Publisher service, as well as contains Data Subscribers. Multiple remote *Service Consumers* as a Data Subscriber can connect to the network to subscribe on data provided by Publisher. It as well sends shutdown signal to all other connected processes, so that they quit application. Only one instance of this process should run, since the service role names are fixed.

3. [pubsubdyn](https://github.com/aregtech/areg-sdk/tree/master/examples/23_pubsubmix/23_pubsubdyn) (23_pubsubdyn) - This sub-project involves a console application housing a *Public Service* consumer object and acts as a mixture of Data Publisher and Data Subscribers. The Publishers publish data for the subscribers inside the process. It as well has Subscribers that receive data from `23_pubsubctrl`, so that it creates a mesh of Publishers and Subscribers. At least one Subscriber should run to discover the service provided by the `23_pubsubctrl` to receive updates as well as the state of the service to quit the application when `23_pubsubctrl` quits. All data have names and values (named data), which output on the console to see the updates from various Publishers.

All communication takes place through `mcrouter`, a multicast router that can operate on any machine within the network. The use of the AREG SDK automates service discovery and ensures fault-tolerant system behavior. Consequently, the order in which the processes are started does not impact the functionality.
