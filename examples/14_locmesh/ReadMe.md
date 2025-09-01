# 14_locmesh

🧵 **Type:** Multithreading / Single Process

## Overview
The **14_locmesh** project builds on [13_locsvc](./../13_locsvc), showing how to run **multiple instances** of the same **local service** in a **multithreaded environment**. Each instance operates in its own thread with a unique name, enabling efficient parallel processing and dynamic service interaction.

## Concepts Shown
- **Multiple Service Instances**: Create and manage multiple service instances, each with a unique name for precise identification.
- **Service Interface & Object RPC**: Use the Service Interface document and code generator to automate **Object RPC** messages for efficient inter-service communication.
- **Multithreading**: Each service instance runs in a separate thread, demonstrating parallel operation in a multithreaded system.
- **Service Provider and Consumer Interaction**: Services interact asynchronously, handling requests, responses, and events in real-time.
- **Application Lifecycle Management**: Observe how multiple service instances can be started, monitored, and terminated gracefully.

## How It Works
The project generates code from [HelloWorld.siml](./services/HelloWorld.siml) to define the service interface. Multiple service providers and consumers are instantiated in separate threads, each with a unique name. One instance acts as the *main* service (`MainService`). The application continues running until a quit message is sent, illustrating controlled lifecycle management in a multithreaded setup.

## Use Cases
- Parallel execution of **multiple service instances** within the same process.
- Dynamic **real-time communication** between service providers and consumers.
- Efficient **multithreaded workload distribution**, where each service handles tasks independently.
- Scalable **internal service management** in multithreaded applications.

## Takeaway
The **14_locmesh** example demonstrates managing **multiple local service instances** in a **multithreaded environment**, enabling developers to build scalable, high-performance applications with parallel service execution.
