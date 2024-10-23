
# 12_pubsvc Project Overview

The **12_pubsvc** project demonstrates the use of a *Public Service* in a multi-processing environment using Object RPC (ORPC) for inter-process communication (IPC). It involves two applications: a *Service Provider* and a *Service Consumer*, which send remote requests and receive responses.

### Sub-Projects:

1. **12_generated:** This is generated from the [HelloWorld.siml](./res/HelloWorld.siml) Service Interface document during CMake configuration, or as a pre-build action in Visual Studio's `dummy` project.
   
2. **[12_pubservice](./pubservice):** A console application that functions as a network-discoverable *Public Service* provider. Multiple *Service Consumers* can connect and send requests for the provider to handle.
   
3. **[12_pubclient](./pubclient):** A console application that acts as a *Public Service* consumer. It automatically discovers the provider service and periodically sends remote request calls using a timer.

All communication happens through `mcrouter`, a multicast router that can run on any networked machine. The AREG SDK automates service discovery and ensures fault tolerance, so the process startup order doesn't affect functionality.
