# 16_pubmesh Project Overview

The **16_pubmesh** project demonstrates the AREG Framework's capability to manage a **meshed network** of **distributed services**, integrating both **Public** and **Local Services**. The project showcases how the framework efficiently connects **Service Providers** and **Service Consumers** to handle **inter-process communication (IPC)** using *Object Remote Procedure Call (Object RPC)*, enabling scalable and reliable communication across the network.

> [!IMPORTANT]
> To test this example, ensure an `mcrouter` process is running on a network-accessible machine to enable message routing. Verify that the `areg.init` configuration file includes the correct IP address and port number for the `mcrouter`.

## Key Concepts

- **Distributed Service Mesh**: Learn how to create a meshed network of services where **Public** and **Local Services** interact seamlessly, enabling efficient message forwarding and processing across multiple nodes.
- **Service Interface:** Utilize the Service Interface document and code generator to automate **Object Remote Procedure Call (Object RPC)** message creation and dispatching, enhancing efficiency and simplifying communication between host and remote target.
- **IPC (Inter-Process Communication)**: Explore scenarios where service providers and consumers communicate across multiple processes.
- **Service Discovery**: Both **Public** and **Local Services** are automatically discovered and connected within a process or across the network, ensuring real-time interaction and fault tolerance.

## Sub-Projects

1. **16_generated**:
   - This project contains code generated from the [LocalHelloWorld.siml](./services/LocalHelloWorld.siml), [PublicHelloWorld.siml](./services/PublicHelloWorld.siml), and [SystemShutdown.siml](./services/SystemShutdown.siml) Service Interface documents during CMake configuration or as a pre-build action in Visual Studio. The generated code automates **Object RPC** communication across the service mesh.

2. **[16_common](./common/)**:
   - A static library that provides common objects and components shared by all sub-projects in this example, enabling code reuse and consistency across the mesh.

3. **[16_pubservice](./pubservice/)**:
   - An application hosting both *Public* and *Local Service* instances. It also manages the `"PublicControllerService"` - a controller responsible for executing the `SystemShutdown` service to broadcast a system shutdown message across the network for graceful termination.

4. **[16_pubsvcmesh](./pubsvcmesh/)**:
   - An application that has mixed multiple *Service Providers* and *Service Consumers*, demonstrating how distributed services interact and exchange messages across the system.

5. **[16_pubclients](./pubclients/)**:
   - An application containing *Public* consumers and a *Local Service*, showcasing how consumers dynamically discover and interact with multiple providers in the mesh.

## Communication

- The communication in **16_pubmesh** is powered by `mcrouter`, which is a router running on any machine within the network. The AREG Framework handles **service discovery** and **fault tolerance**, ensuring that the startup order of processes does not affect the communication. The system is designed to ensure messages are reliably forwarded to their intended recipients within the mesh, even in complex multi-node environments.
- **Object RPC for IPC**: The **Object RPC** protocol handles inter-process communication between the provider and consumers, enabling easy and efficient remote interaction. This is particularly important for scaling distributed systems, where different processes need to exchange data or requests seamlessly.

## Key Features

- **Meshed Network of Services**: Seamlessly integrates **Public Services** within a distributed mesh, demonstrating real-time communication across multiple nodes. Co-exist with **Local Services**.
- **Object RPC Automation**: Utilizes the **code generator** to automatically create and manage **Object RPC** messages, reducing manual effort and improving development efficiency.
- **Service Discovery and Fault Tolerance**: The AREG SDK ensures that services are automatically discovered and communication remains stable even if nodes or services start out of order.
- **Graceful System Shutdown**: The system may easily have centralized shutdown service, ensuring that all connected applications terminate safely and synchronously when required.

## Use Cases

- **Distributed Service Networks**: Ideal for applications that require a **meshed architecture** where multiple **Public Services** may co-exist with **Local Services**, and interact across different machines, providing high availability and load distribution.
- **Dynamic Service Interaction**: The project demonstrates how consumers and providers can dynamically discover and communicate with each other, making it perfect for systems that require **scalable** and **reliable** communication across a distributed network.
- **Router**: Useful for scenarios where services need to message to multiple recipients, ensuring efficient message routing across the system.

## Conclusion

The **16_pubmesh** project highlights the power of the AREG SDK in managing a **distributed service mesh** that integrates both **Public** and **Local Services**. Through the use of **Object RPC**, automated service discovery, and fault tolerance, this project provides a comprehensive guide for developers looking to build scalable, reliable, and efficient service-based applications in a multi-process environment.
