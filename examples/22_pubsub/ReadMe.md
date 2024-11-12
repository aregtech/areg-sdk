# 22_pubsub Project Overview

The **22_pubsub** project demonstrates a system with distributed services using the *Publish/Subscribe* (Pub/Sub) model, where a *Public Service* (**Publisher**) shares data updates with multiple subscribing processes. The **Public Service** provides two notification modes:

- **On Change**: Subscribers are notified only when data values change.
- **Always**: Subscribers receive notifications every time data is updated, regardless of changes.

When no **Subscriber** is listening for updates, the **Publisher** updates data internally without sending notifications, optimizing network traffic compared to many traditional Pub/Sub implementations.

This project uses **Object Remote Procedure Call (Object RPC)** for efficient **Inter-Process Communication (IPC)**, enabling seamless interaction between publishers and subscribers.

> [!IMPORTANT]
> To test this example, ensure an `mcrouter` process is running on a network-accessible machine to enable message routing. Verify that the `areg.init` configuration file includes the correct IP address and port number for the `mcrouter`.

## Key Concepts

- **Publish/Subscribe Model**: This project employs the Pub/Sub model, where a publisher broadcasts updates, and multiple subscribers can join or leave dynamically to receive these updates.
- **Service Interface Automation**: Code is generated from a **Service Interface** document, automating **Object RPC** message creation and dispatching. This streamlines process communication, reducing development time.
- **Object RPC for IPC**: Leveraging **Object RPC** for inter-process communication allows the publisher and subscribers to exchange messages efficiently, supporting high-performance data distribution.

## Sub-Projects

1. **22_generated**:
   - Contains code generated from the [PubSub.siml](./services/PubSub.siml) Service Interface document during CMake configuration or as a pre-build step in Visual Studio. This code automates **Object RPC** messaging, making **IPC** between the publisher and subscribers straightforward and efficient.

2. **[22_publisher](./publisher/)**:
   - Acts as the *Public Service Provider* or **Publisher**, broadcasting data updates to subscribed consumers. When any **Subscriber** is registered, it sends data notifications either on change or every time data is set, depending on the selected notification mode.

3. **[22_subscriber](./subscriber/)**:
   - A client application that acts as *Service Consumer* or **Subscriber**. It subscribes to updates from the *Publisher*. Multiple instances of this application can run, each subscribing to the same data. Subscribers receive the latest data immediately upon connecting, even if no recent updates have been broadcast.

## Communication

Communication between the service provider and consumers is facilitated by **mcrouter** router, which is capable of operating across any networked machine. The AREG Framework automates **service discovery** and ensures **fault tolerance**, enabling reliable **IPC** and ensuring that the order of process startup does not affect the system's functionality. Services are automatically discovered, and messages are forwarded seamlessly to their intended recipients, maintaining robust inter-process communication.

## Key Features

- **Dynamic Subscription/Unsubscription**: Subscribers can join or leave the system at any time without affecting the ongoing publishing. The AREG Framework ensures that new subscribers receive the latest information as soon as they connect.
- **Two Notification Modes**: The system supports two notification types:
  - **On Change**: Subscribers are notified only when data changes.
  - **Always**: Subscribers receive a notification every time data is updated, even if it hasn't changed.
- **Immediate Data Updates**: Subscribers automatically receive the most recent data upon connection, ensuring they are always current, even if no new updates have recently been published.
- **Fault-Tolerant Communication**: The AREG Framework and `mcrouter` ensure robust communication, automatically handling failures and maintaining service discovery. This fault tolerance keeps data updates reliable, even with network or process issues.

## Use Cases

- **Real-Time Data Distribution**: This project is ideal for applications requiring real-time data updates, such as stock price monitoring, sensor data collection, or event notification systems.
- **Multi-Process Systems**: Perfect for systems with multiple processes that need to dynamically subscribe to or unsubscribe from data streams without disrupting service.
- **Flexible Notification Systems**: Subscribers can choose between receiving updates only when there are changes or on every data event, allowing the system to meet a variety of real-time data monitoring needs.

## Conclusion

The **22_pubsub** project showcases a scalable implementation of the Pub/Sub model, supporting effective communication between distributed processes with **Object RPC** and **IPC**. By leveraging the AREG SDK's service discovery and fault tolerance, the project provides a reliable and flexible solution for real-time data distribution in multi-process environments. This project is suited for scenarios requiring efficient, continuous data streams, demonstrating how publishers and subscribers interact smoothly in a distributed system.
