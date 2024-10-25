# 23_pubsubmix Project Overview

The **23_pubsubmix** project demonstrates a hybrid *Publish/Subscribe* (Pub/Sub) service where both publishers and subscribers can operate within the same process or across different processes in a multi-process environment. This setup allows local subscribers to receive updates reliably, even during network disruptions.

Using **Object Remote Procedure Call (Object RPC)** for efficient **Inter-Process Communication (IPC)**, this project ensures that subscribers dynamically receive the latest data upon subscription, whether the communication is local or remote. This example highlights the flexibility of combining both local and remote Pub/Sub communication within the same application.

## Key Concepts

- **Hybrid Pub/Sub Model**: This model enables publishers and subscribers to reside within the same process or across multiple processes, ensuring robust communication regardless of network conditions.
- **Service Interface Automation**: Code generated from a **Service Interface** document automates **Object RPC** message creation and dispatch, simplifying Pub/Sub system implementation and improving efficiency for **IPC**.
- **Object RPC for IPC**: This project leverages **Object RPC** for inter-process communication, facilitating efficient data exchange between processes or threads.

## Project Structure

1. **23_generated**:
   - Contains code generated from the [PubSubMix.siml](./res/PubSubMix.siml) Service Interface document, either during CMake configuration or as a pre-build step in Visual Studio. This code automates **Object RPC** messaging, simplifying communication across processes.

2. **[23_common](./common/)**:
   - A static library containing shared objects and functions used by other project processes. This common library promotes modular design and reuse of core components.

3. **[23_pubsubctrl](./pubsubctrl/)**:
   - An application acting as both a *Data Publisher* and a *Data Subscriber (Service Consumer)*. It publishes data to subscribers and subscribes to updates from other publishers. Additionally, this process manages communication and can send shutdown signals to all connected processes when needed.

4. **[23_pubsubdyn](./pubsubdyn/)**:
   - An application functioning as both a *Data Publisher* and *Data Subscriber*. It connects to the `23_pubsubctrl` service to publish and receive updates, allowing it to switch between publishing and subscribing dynamically based on system requirements.

## Communication

- **mcrouter**: The `mcrouter` manages message delivery between publishers and subscribers, ensuring reliable, fault-tolerant communication across the network.
- **Object RPC for IPC**: Using **Object RPC** streamlines IPC, providing an adaptable, high-performance way to manage messaging between services and simplifying remote procedure call implementation for distributed Pub/Sub systems.

## Key Features

- **Hybrid Communication**: The Pub/Sub system supports both local and remote subscribers, allowing any process to function as both a **Publisher** and **Subscriber** simultaneously.
- **Network Resilience**: All **Publishers** and **Subscribers** within the same process receive updates regardless of network connectivity, ensuring uninterrupted service.
- **Immediate Data Updates**: Subscribers automatically receive the latest data upon connecting, ensuring they’re always up-to-date even if no recent updates have been published.
- **Dynamic Subscription and Publishing**: Both *23_pubsubctrl* and *23_pubsubdyn* can seamlessly switch between publisher and subscriber roles, making the system highly adaptable in dynamic, multi-process environments.
- **Fault-Tolerant Communication**: With `mcrouter` and the AREG SDK, the system remains fault-tolerant, supporting automatic service discovery and recovery, ensuring reliable Pub/Sub functionality even during failures.

## Use Cases

- **Multi-Process Communication**: Ideal for systems where multiple processes need to communicate and share data dynamically, particularly where local and remote subscribers are combined.
- **Resilient Pub/Sub Systems**: Demonstrates building resilient Pub/Sub systems capable of continuing operation under challenging network conditions, making it well-suited for reliability-critical environments.
- **Dynamic Data Distribution**: Allows for the flexible creation and connection of publishers and subscribers, making it suitable for real-time, multi-process data distribution.

## Conclusion

The **23_pubsubmix** project exemplifies the versatility of a hybrid Pub/Sub system, where publishers and subscribers can coexist in the same process or be distributed across multiple processes. By leveraging **Object RPC** for **Inter-Process Communication (IPC)** and maintaining fault-tolerant communication with `mcrouter`, this project showcases a resilient Pub/Sub architecture capable of reliable real-time data distribution, making it ideal for dynamic, multi-process environments where subscribers always stay updated.
