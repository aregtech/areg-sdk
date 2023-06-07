# 15_pubworker Project Overview

The [15_pubworker](https://github.com/aregtech/areg-sdk/tree/master/examples/15_pubworker) project showcases the usage of *Worker Threads* and the processing of custom events in both the Worker and Component threads.

The project consists of three sub-projects:

1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/15_generated/generated) (15_generated): This static library contains generated codes and common objects. It is generated from the [PatientInformation.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/15_pubworker/res/PatientInformation.siml) *Service Interface* document using the `codegen.jar` tool.

2. [pubservice](https://github.com/aregtech/areg-sdk/tree/master/examples/15_pubworker/pubservice) (15_pubservice): This sub-project includes an instance of a *Public Service Provider* and a *Worker Thread*. The Worker Thread receives user inputs from the console and updates the attributes of the Service. Both the Service and the Worker Thread are defined in the static model to load on startup and unload on exit.

3. [pubclient](https://github.com/aregtech/areg-sdk/tree/master/examples/15_pubworker/pubclient) (15_pubclient): This sub-project contains a *Service Consumer* and a *Worker Thread* defined in the model. The Worker Thread simulates the output device.

Communication within the system is facilitated by `mcrouter`, a multicast router that operates on any machine within the network. The AREG SDK automates service discovery and ensures a fault-tolerant system, making the order of process startup irrelevant.
