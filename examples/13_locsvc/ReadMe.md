# 13_locsvc Project Overview

The **13_locsvc** project demonstrates creating and managing a **Local Service** within the AREG Framework, using the Service Interface document and AREG SDK's code generator. This example shows how to build a service provider and consumer that communicate asynchronously within a single, multithreaded process, without external visibility.

The **Local Service** interface is defined in the [HelloWorld.siml](./services/HelloWorld.siml) file, and the corresponding source code is automatically generated during the build process via `codegen.jar`.

## Key Concepts

- **Local Service**: The project showcases a service provider and consumer that are confined to the local process, providing a controlled environment for internal communication.
- **Multithreaded Environment**: Using Local Services, the project creates a complex, multithreaded setup where service providers and consumers can operate across different threads and communicate asynchronously.
- **Service Interface**: With the Service Interface document and code generator, **Object Remote Procedure Call (Object RPC)** messages are automated for efficient, simplified communication between the host and local targets.
- **Request and Response Handling**: Manage requests, responses, and event broadcasts within the scope of a local service.

## Sub-Projects

1. **13_generated**:
   - Contains code generated from the [HelloWorld.siml](./services/HelloWorld.siml) interface file during CMake configuration or pre-build action in Visual Studio. This includes files essential for defining the local service functionality.

2. **[13_locservice](./locservice/)**:
   - Implements both the **service provider** and **service consumer**. Communication between these objects relies on **RPC messages**, and is event-driven and asynchronous. Key components include:
     - **Request Handling**: The service provider processes incoming requests from the consumer and sends appropriate responses.
     - **Response Processing**: The consumer manages responses from the provider.
     - **Event Broadcasting**: The service provider can broadcast events to subscribed consumers.

## Features

- **Local-Only Access**: The service is confined within the process, making it ideal for secure, internal communication.
- **Request/Response Model**: Demonstrates a structured request/response interaction between provider and consumer.
- **Event Broadcasting**: Allows the service provider to broadcast updates or events to all subscribed consumers.
- **Runtime Subscription**: Consumers can dynamically subscribe or unsubscribe from broadcasts in real time, enabling flexible service usage.

## Use Cases

- **Dynamic Service Consumption**: Consumers can subscribe or unsubscribe from events at runtime, allowing flexible service usage in scenarios that require real-time adaptability.
- **Event-Driven Architectures**: Ideal for event-driven systems, enabling multiple components to communicate asynchronously within a single process.

## Conclusion

The **13_locsvc** project provides a clear example of implementing a **Local Service** in a **multithreaded environment** using the AREG SDK. By defining a local service provider and consumer, it demonstrates effective request/response handling, asynchronous communication, and real-time event broadcasting within a single process.
