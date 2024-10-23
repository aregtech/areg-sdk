
## 13_pubmesh Project Overview

The **13_pubmesh** project showcases the AREG Framework’s ability to handle a meshed network of distributed services, combining *Public* and *Local* services. The framework seamlessly connects *Service Consumers* with *Service Providers* and forwards messages efficiently for processing.

### Sub-Projects:

1. **13_generated:** Generated from [LocalHelloWorld.siml](./res/LocalHelloWorld.siml), [PublicHelloWorld.siml](./res/PublicHelloWorld.siml), and [SystemShutdown.siml](./res/SystemShutdown.siml) Service Interface documents during CMake configuration or as a pre-build action in Visual Studio’s `dummy` project.
   
2. **[13_common](./common/):** A static library providing common objects shared across other sub-projects.

3. **[13_pubservice](./pubservice/):** A console application that hosts both *Public* and *Local* service instances. It also includes a *Controller Service* (named `"PublicControllerService"`) that runs the `SystemShutdown` service, responsible for sending a shutdown message to notify all applications in the system to terminate gracefully.

4. **[13_pubsvcmesh](./pubsvcmesh/):** An application that creates a mesh network with mixed instances of *Service Providers* and *Service Consumers*.

5. **[13_pubclients](./pubclients/):** An application housing *Public* and *Local* service consumers.

Communication is facilitated through `mcrouter`, a multicast router that operates on any machine within the network. The AREG SDK automates service discovery, ensuring fault-tolerant system behavior regardless of the process startup order. Messages are reliably delivered to the intended targets.
