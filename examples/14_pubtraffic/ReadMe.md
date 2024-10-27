# 14_pubtraffic Project Overview

The **14_pubtraffic** project demonstrates how to dynamically create and manage models within a **multiprocessing application**, showcasing the ability to handle custom events and dynamic model creation during runtime. The project utilizes **Inter-Process Communication (IPC)** and **Object Remote Procedure Call (Object RPC)** to enable seamless communication between services in a distributed environment.

> [!IMPORTANT]
> To test this example, ensure an `mcrouter` process is running on a network-accessible machine to enable message routing. Verify that the `areg.init` configuration file includes the correct IP address and port number for the `mcrouter`.

## Key Concepts

- **Dynamic Model Creation**: Learn how to dynamically create and manage models during runtime, enabling flexible system behavior and responsiveness to user commands.
- **Service Interface:** Utilize the Service Interface document and code generator to automate **Object Remote Procedure Call (Object RPC)** message creation and dispatching, enhancing efficiency and simplifying communication between host and remote target.
- **Custom Event Handling**: Demonstrates the handling of custom events within a multithreaded system, triggered by user commands such as `start`, `stop`, and `quit`.

## Sub-Projects

1. **14_generated**:
   - Contains codes, generated from the [SimpleTrafficLight.siml](./res/SimpleTrafficLight.siml) Service Interface document. The generated code automates **Object RPC** message creation and communication, enabling efficient interaction between service providers and consumers.

2. **[14_pubclient](./pubclient/)**:
   - A service consumer application that dynamically creates models during runtime. Multiple instances of this client can be launched, with each instance subscribing to different data sets via commands like `sn` (for nominal *South-North* direction data) or `ew` (for nominal *East-West* direction data).

3. **[14_pubservice](./pubservice/)**:
   - An application that provides **Public Services**, handling custom events triggered by user commands. This service listens for commands such as `start` to begin service operations, `stop` to halt them, and `quit` to gracefully terminate the all service provider and consumer applications.

## Communication

Communication between the service provider and consumers is facilitated by **mcrouter** router, which is capable of operating across any networked machine. The AREG Framework automates **service discovery** and ensures **fault tolerance**, enabling reliable **IPC** and ensuring that the order of process startup does not affect the system’s functionality. Services are automatically discovered, and messages are forwarded seamlessly to their intended recipients, maintaining robust inter-process communication.

## Key Features

- **Dynamic Model Management**: The ability to dynamically create models and manage their lifecycle during runtime in a **multithreaded** and **multiprocessing** environment provides flexibility and scalability.
- **Object RPC and IPC**: The project demonstrates the use of **Object RPC** and **IPC** to handle communication between services, automating message dispatch and response handling across multiple processes.
- **Custom Event Handling**: The service provider reacts to user commands by triggering custom events, offering fine-grained control over service behavior and interaction.

## Use Cases

- **Dynamic Runtime Model Creation**: This project is ideal for applications that require models or objects to be created dynamically during runtime, offering flexibility in service configuration and operation.
- **Real-time Inter-Process Communication (IPC)**: Suitable for systems where real-time communication between distributed services is essential, leveraging **Object RPC** and **IPC** to ensure reliable message exchange.
- **Custom Event-Driven Systems**: Applications that rely on custom event handling can benefit from the project’s approach to managing user-triggered events in a multithreaded environment.

## Conclusion

The **14_pubtraffic** project highlights the AREG SDK’s capabilities in managing **dynamic models** and **custom events** within a **multithreaded** and **distributed system**. Through the use of **Object RPC** and **IPC**, the project demonstrates how to achieve real-time communication and event handling, making it a valuable example for developers looking to build scalable, event-driven applications.
