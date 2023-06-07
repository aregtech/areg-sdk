# 12_pubsvc Project Overview

The [12_pubsvc](https://github.com/aregtech/areg-sdk/tree/master/examples/12_pubsvc) project showcases the utilization of a *Public Service* in a multi-processing environment, employing Object RPC (ORPC) for inter-process communication (IPC). The demonstration involves the creation of two applications: one serving as the *Service Provider* and the other as the *Service Consumer*, making remote requests and receiving corresponding responses.

The project comprises three sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/12_pubsvc/generated) (12_generated) - This sub-project contains a static library consisting of generated code derived from the [HelloWorld.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/12_pubsvc/res/HelloWorld.siml) *Service Interface* document. The code generation is facilitated using the `codegen.jar` tool.

2. [pubservice](https://github.com/aregtech/areg-sdk/tree/master/examples/12_pubsvc/pubservice) (12_pubservice) - This sub-project represents a console application functioning as a network-discoverable *Public Service* provider. Multiple remote *Service Consumers* can connect to the network and initiate requests to be executed by the `12_pubservice` application.

3. [pubclient](https://github.com/aregtech/areg-sdk/tree/master/examples/12_pubsvc/pubclient) (12_pubclient) - This sub-project involves a console application housing a *Public Service* consumer object. It automatically discovers the service provided by the `12_pubservice` application and triggers remote request calls. A timer is employed to facilitate periodic request calls.

All communication takes place through `mcrouter`, a multicast router that can operate on any machine within the network. The use of the AREG SDK automates service discovery and ensures fault-tolerant system behavior. Consequently, the order in which the processes are started does not impact the functionality.
