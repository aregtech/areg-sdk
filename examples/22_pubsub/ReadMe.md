# 22_pubsub Project Overview

The [22_pubsub](https://github.com/aregtech/areg-sdk/tree/master/examples/22_pubsub) project showcases the utilization of a *Public Service* in a multi-processing environment, which acts list PubSub (Publish / Subscriber) service. There are 2 types notifications to receive when subscribe on data, called 'Attributes', and it depends on the design:
- **On change**: receive update notifications only if the value is changed. In this example it is a string, which sends notification only when value is changed.
- **Always**   : receive update notification each time value is set even if the value is not changed.

The demonstration employing Object RPC (ORPC) for inter-process communication (IPC). The demonstration involves the creation of two applications: one serving as the *Service Provider* to act as a data Publisher and the other as the *Service Consumer* to act as a data Subscriber. The Subscriber can dynamically subscribe and unsubscribe on data update. All data update notifications receive automatically and the system guarantees that as soon as Subscriber assigns on data it receives the latest data even if it is not updated yet. This guaranties that if for any reason the Subscriber does not miss the update notification.

The project comprises three sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/22_pubsub/generated) (22_generated) - This sub-project contains a static library consisting of generated code derived from the [PubSub.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/22_pubsub/res/PubSub.siml) *Service Interface* document. The code generation is facilitated using the `codegen.jar` tool.

2. [pubsubservice](https://github.com/aregtech/areg-sdk/tree/master/examples/22_pubsub/pubsubservice) (22_pubsubservice) - This sub-project represents a console application functioning as a network-discoverable *Public Service* provider, which acts as a Data Publisher service. Multiple remote *Service Consumers* as a Data Subscriber can connect to the network to subscribe on data provided by Publisher.

3. [pubsubclient](https://github.com/aregtech/areg-sdk/tree/master/examples/22_pubsub/pubsubclient) (22_pubsubclient) - This sub-project involves a console application housing a *Public Service* consumer object and acts as a Data Subscriber. It automatically discovers the service provided by the `22_pubsubservice`, dynamically subscribes on Data that is interested and receives update notification 'On Change' and 'Always', depending on 'Attribute' (Data) Property, which is defined on design level.

All communication takes place through `mcrouter`, a multicast router that can operate on any machine within the network. The use of the AREG SDK automates service discovery and ensures fault-tolerant system behavior. Consequently, the order in which the processes are started does not impact the functionality.
