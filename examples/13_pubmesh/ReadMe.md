# 13_pubmesh Project Overview

The [13_pubmesh](https://github.com/aregtech/areg-sdk/tree/master/examples/13_pubmesh) project showcases the powerful capabilities of AREG Framework by demonstrating a meshed network of distributed services composed of both *Public* and *Local* services. With AREG, the complex network is seamlessly handled, connecting Service Consumers with Service Providers and efficiently forwarding messages for processing.

The project comprises the following sub-projects:
1. [generated](https://github.com/aregtech/areg-sdk/tree/master/examples/13_pubmesh/generated) (13_generated): A static library containing generated code derived from the [HelloWorld.siml](https://github.com/aregtech/areg-sdk/blob/master/examples/13_pubmesh/res/HelloWorld.siml) *Service Interface* document, generated using the `codegen.jar` tool.
2. [common](https://github.com/aregtech/areg-sdk/tree/master/examples/13_pubmesh/common) (13_common): A static library providing common objects used across other projects.
3. [pubservice](https://github.com/aregtech/areg-sdk/tree/master/examples/13_pubmesh/pubservice) (13_pubservice): A console application featuring instances of *Public* and *Local* services.
4. [pubsvcmesh](https://github.com/aregtech/areg-sdk/tree/master/examples/13_pubmesh/pubsvcmesh) (13_pubsvcmesh): An application with mixed instances of *Service Providers* and *Service Consumers*, forming a mesh network.
5. [pubclients](https://github.com/aregtech/areg-sdk/tree/master/examples/13_pubmesh/pubclients) (13_pubclients): An application containing *Public* and *Local* service consumers.

Communication within the system is facilitated through `mcrouter`, a multicast router that operates on any machine within the network. The AREG SDK automates service discovery and ensures fault-tolerant system behavior, making the order of process startup irrelevant.

The system guarantees the reliable delivery of calls to their intended targets without mixing or losing messages.

To learn more about the project and explore the sub-projects in detail, visit the respective links provided. The AREG Framework empowers developers to create robust and interconnected systems, enabling efficient communication in distributed environments.
