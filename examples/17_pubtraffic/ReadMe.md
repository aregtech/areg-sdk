# 17_pubtraffic

🧵🚀 **Type:** Multithreading + IPC

## Overview
The **17_pubtraffic** project demonstrates how to dynamically create and manage **runtime models** within a **multithreaded, multi-process application**. It shows how services communicate using **Object Remote Procedure Call (Object RPC)** over **Inter-Process Communication (IPC)**, with custom events triggered by user commands.

> [!NOTE]
> A **Multi-Target Router** (`mtrouter`) must be running on a network-accessible machine. Ensure the `areg.init` file includes the correct IP and port for the router.

## Concepts Shown
- **Dynamic Model Creation**: Create and manage service models at runtime for flexible system behavior.
- **Custom Event Handling**: Handle user-triggered events such as `start`, `stop`, and `quit`.
- **Object RPC for IPC**: Automates inter-process message dispatching between provider and consumer services.
- **Service Discovery**: Consumers automatically connect to providers using `serviceConnected()`, regardless of startup order.
- **Multithreaded & Multiprocess Execution**: Services run in separate threads and processes for scalability and concurrency.

## How It Works
Code is generated from [SimpleTrafficLight.siml](./services/SimpleTrafficLight.siml) to handle Object RPC communication. The project includes a service provider (`17_pubservice`) that listens for custom commands and multiple clients (`17_pubclient`) that dynamically create models and subscribe to specific datasets. Messages between processes are routed via `mtrouter`, ensuring reliable and fault-tolerant IPC.

## Sub-Projects
1. **17_generated**: Auto-generated Object RPC code from service interface documents.
2. **[17_pubclient](./pubclient/)**: Dynamically creates models at runtime and subscribes to specific datasets. Multiple client instances can run concurrently.
3. **[17_pubservice](./pubservice/)**: Provides Public Services, handles custom events, and manages the lifecycle of all clients and services based on user commands.

## Use Cases
- **Dynamic Runtime Model Management**: Ideal for systems that require creating objects or services dynamically during execution.
- **Real-Time IPC**: Demonstrates reliable inter-process communication in distributed systems using Object RPC.
- **Custom Event-Driven Applications**: Supports event-driven systems with precise control over service behavior via user commands.

## Takeaway
The **17_pubtraffic** example demonstrates **dynamic model creation**, **custom event handling**, and **real-time IPC** using **Object RPC** in a **multithreaded, distributed environment**, providing a blueprint for scalable and responsive service-driven applications.
