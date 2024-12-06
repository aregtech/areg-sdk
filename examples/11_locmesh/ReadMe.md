
# 11_locmesh Project Overview

The **11_locmesh** project builds on the [10_locsvc](./../10_locsvc) example, demonstrating how to run **multiple instances** of the same **local service** within a **multithreaded environment**. Each service instance operates in its own thread and is assigned a unique name, allowing for efficient parallel processing and service interaction.

## Key Concepts

- **Multiple Service Instances**: Learn how to create and manage multiple instances of the same service, each identified by a unique name within the application.
- **Service Interface:** Utilize the Service Interface document and code generator to automate **Object Remote Procedure Call (Object RPC)** message creation and dispatching, enhancing efficiency and simplifying communication between host and remote target.
- **Multithreading**: Each service instance runs in a separate thread, demonstrating how services operate concurrently in a multithreaded system.
- **Service Provider and Consumer Interaction**: The project highlights how multiple service providers and consumers asynchronously interact in real-time, showcasing dynamic service-to-service communication.

## Sub-Projects

1. **11_generated**:
   - This library contains code generated from the [HelloWorld.siml](./services/HelloWorld.siml) service interface document during CMake configuration or as part of a pre-build action in Visual Studio. It includes the source files necessary to define and instantiate multiple service instances.

2. **[11_locmesh](./locsvcmesh)**:
   - This sub-project demonstrates the instantiation of **multiple service providers and consumers**, each running in separate threads. Key features include:
     - **Multithreaded Service Instances**: Multiple service instances are created and assigned unique names, allowing the system to manage them independently in parallel.
     - **Main Service**: One of the service providers is designated as the *main* service (`MainService`), acting as a primary service instance.
     - **Graceful Shutdown**: The application runs until a *quit* message is sent, demonstrating how to manage the lifecycle of multiple service instances within the application.

## Features

- **Multithreaded Local Services**: Each service provider and consumer operates in its own thread, allowing for high-performance parallel execution.
- **Unique Service Names**: Each service instance is given a unique name, ensuring that the system can distinguish between and manage multiple instances of the same service.
- **Service Interaction**: The project demonstrates how service providers and consumers interact, exchanging data and responding to events in real-time.
- **Application Lifecycle Management**: The application runs until a termination message is received, showcasing how to manage the lifecycle of a multithreaded application.

## Use Cases

- **Parallel Service Processing**: This project is designed for complex, multithreaded applications where multiple service providers and consumers operate within the same environment. It supports scenarios where each service instance runs in a separate thread, allowing services to interact efficiently across threads.
- **Concurrent Service Instances**: The project also covers cases where multiple instances of the same service run simultaneously, with each instance handling different tasks in parallel. This setup enables workload distribution and enhances performance by processing tasks concurrently within a single process.
- **Dynamic Service Interaction**: Learn how to manage real-time communication between multiple service providers and consumers, which is useful for complex systems requiring high throughput and responsiveness.
- **Multithreaded Application Management**: The example demonstrates best practices for running, managing, and terminating multithreaded applications with multiple service instances.

## Conclusion

The **11_locmesh** project offers a practical guide for creating complex multithreaded application, managing **multiple local service instances** in a **multithreaded environment** using the AREG SDK. By assigning unique names to each service and running them in parallel, developers can build scalable, high-performance applications that handle multiple tasks concurrently.
