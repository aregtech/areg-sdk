# 15_pubsvc Project Overview

The **15_pubsvc** project demonstrates the implementation of a *Public Service* in a multi-process environment using **Object Remote Procedure Call (Object RPC)** for inter-process communication (IPC). This example involves two main applications: a *Service Provider* and a *Service Consumer*. The service provider handles remote requests sent by the consumer, allowing developers to understand how AREG Framework manages distributed systems and public services.

> [!IMPORTANT]
> To test this example, ensure an `mcrouter` process is running on a network-accessible machine to enable message routing. Verify that the `areg.init` configuration file includes the correct IP address and port number for the `mcrouter`.

## Key Concepts

- **Public Service**: A service that can be discovered and accessed by remote consumers over the network.
- **Object RPC for IPC**: The communication between the provider and consumers is handled via **Object Remote Procedure Call (Object RPC)**, which simplifies **Inter-Process Communication (IPC)**.
- **Service Discovery**: The AREG Framework automates the discovery of the *Service Provider* by the *Service Consumer*, making the startup order of processes irrelevant.

## Sub-Projects

1. **15_generated**:
   - Contains code is generated from the [HelloWorld.siml](./services/HelloWorld.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio. The generated code automates **Object RPC** message creation and dispatching, improving efficiency in communication between the *Service Provider* and *Service Consumer*. Object RPC is used for **IPC**, ensuring seamless remote interaction between processes.

2. **[15_pubservice](./pubservice/)**:
   - An application acting as a network-discoverable *Public Service* provider. It listens for remote requests from consumers and processes them. Multiple *Service Consumers* can connect to the provider simultaneously, sending requests to be handled.

3. **[15_pubclient](./pubclient/)**:
   - An application that acts as a *Public Service* consumer. The client application automatically discovers the *Public Service* provider using AREG Frameworks's built-in service discovery. It periodically sends remote request calls using a timer, simulating real-world use cases where a service might need regular interactions with the provider.

## Communication

- **mcrouter**: All communication between the *Service Provider* and *Service Consumer* is managed through `mcrouter`, which is a router that enables efficient message routing in distributed systems. It can run on any networked machine, ensuring flexible and robust message delivery.
- **Object RPC for IPC**: The **Object RPC** protocol handles inter-process communication between the provider and consumers, enabling easy and efficient remote interaction. This is particularly important for scaling distributed systems, where different processes need to exchange data or requests seamlessly.

## Key Features

- **Automatic Service Discovery**: AREG Framework automates the discovery of the *Public Service*, so *Service Consumers* can easily connect to the provider without needing explicit configuration. This ensures smooth communication even when the processes are started in any order.
- **Fault Tolerance**: The AREG SDK ensures fault tolerance in service communication, meaning if a service fails, it can recover or restart without affecting the overall system's operation.
- **Efficient Communication**: By using **Object RPC** for communication, the system reduces overhead and simplifies the development process. It ensures that requests and responses between the provider and consumers are dispatched efficiently.

## Use Cases

- **Multiprocess Applications**: The **15_pubsvc** project is ideal for scenarios where multiple clients need to communicate with a single service provider in a network of distributed services.
- **Public Services**: It is well-suited for applications where a network-accessible service needs to process requests from multiple consumers and maintain high availability and reliability.

## Conclusion

The **15_pubsvc** project showcases how AREG SDK simplifies the development of distributed systems with public services at the edge of the network. By leveraging **Object RPC** for **Inter-Process Communication (IPC)**, it enables efficient communication between the service provider and consumers in a multi-process environment. With automated service discovery and fault tolerance, this project highlights the flexibility and scalability of AREG SDK for building robust, real-time systems. Communication via `mcrouter` ensures seamless data exchange, making the project suitable for a variety of real-world use cases where distributed services are required.
