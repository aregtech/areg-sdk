
# 10_locsvc Project Overview

The **10_locsvc** project demonstrates how to create and manage a **Local Service** using the AREG Framework, Service Interface document and the code generator of AREG SDK. This example covers the creation of both the service provider and the service consumer, showcasing how local services operate within a single process, without external visibility.

The **Local Service** interface is defined in the [HelloWorld.siml](./res/HelloWorld.siml) file, and the corresponding source code is automatically generated during the build process using `codegen.jar`.

## Key Concepts

- **Local Service**: The project focuses on implementing a service provider and consumer that are only accessible within the local process, ensuring better control over internal communication.
- **Service Interface:** Utilize the Service Interface document and code generator to automate **Object Remote Procedure Call (Object RPC)** message creation and dispatching, enhancing efficiency and simplifying communication between host and remote target.
- **Request and Response Handling**: Learn how to manage requests, responses, and broadcasting of events in the context of a local service.

## Project Structure

1. **10_generated**: 
   - This directory contains code generated from the [HelloWorld.siml](./res/HelloWorld.siml) interface file during the CMake configuration or as part of a pre-build action in Visual Studio. It includes all necessary files to define the local service's functionality.

2. **[10_locservice](./locservice/)**: 
   - This sub-project implements both the **service provider** and the **service consumer**. It demonstrates:
     - **Request Handling**: The service provider processes requests sent by the remote service consumer and responds accordingly.
     - **Response Processing**: The service consumer receives and processes the provider's responses.
     - **Broadcasting**: The service provider broadcasts events to all subscribed consumers.

This version is more streamlined while maintaining the original meaning.
## Features

- **Local-Only Access**: The service operates strictly within the process, ensuring that no external access is allowed, making it ideal for internal process communication.
- **Request/Response Model**: The project demonstrates a clear request/response communication model between the service provider and consumer.
- **Event Broadcasting**: The service provider can broadcast updates or events to all interested consumers.
- **Runtime Subscription**: Consumers can dynamically subscribe or unsubscribe from specific broadcasts, allowing real-time control over the services they consume.

## Use Cases

- **Internal Process Communication**: This project is ideal for scenarios where services need to communicate internally within a single application without external access.
- **Dynamic Service Consumption**: The ability to subscribe and unsubscribe to events (broadcasts) at runtime allows for adaptable service usage in applications that require dynamic control.
- **Event-Driven Architectures**: The broadcasting functionality is perfect for event-driven systems, where multiple consumers need to receive real-time updates from a local service.

## Conclusion

The **10_locsvc** project provides a comprehensive example of how to implement a **Local Service** in a **multithreaded environment** using the AREG SDK. By defining a local service provider and consumer, this project demonstrates effective internal communication, request/response management, and real-time event broadcasting within a single process.
