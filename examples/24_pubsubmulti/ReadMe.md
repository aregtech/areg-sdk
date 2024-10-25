# 24_pubsubmulti Project Overview

The **24_pubsubmulti** project demonstrates the use of a *Public Service* in a multi-process environment with one process acting as a data publisher and another process containing two subscribers running within the same thread. This example showcases how AREG's Pub/Sub (Publish/Subscribe) feature *efficiently minimizes* event generation by delivering only necessary updates, reducing unnecessary data traffic.

## Key Concepts

- **Efficient Event Delivery**: The project illustrates how AREG’s Pub/Sub system minimizes event notifications by sending updates only when necessary, ensuring that subscribers receive only relevant data.
- **Multiple Subscribers in One Thread**: It demonstrates a scenario where multiple subscribers, residing in the same thread, subscribe to the same data, but at different times.
- **Dynamic Subscription**: Each subscriber can dynamically subscribe to the published data, with notifications sent only when the data changes or based on predefined settings.

## Notification Modes

The system supports two types of notifications for subscribers:

1. **On Change**: Notifications are triggered only when the data value changes.
2. **Always**: Notifications are sent every time the value is set, regardless of whether the value has changed.

This dual-mode notification system offers flexibility depending on the use case, allowing developers to optimize for either change-driven updates or continuous notifications.

## Project Structure

1. **24_generated**:
   - This  library code is generated from the [PubSub.siml](./res/PubSub.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio. The generated code automates **Object Remote Procedure Call (Object RPC)** message creation and dispatching, streamlining communication between the publisher and subscribers. The Object RPC is used for **Inter-Process Communication (IPC)** to handle data exchange efficiently across processes.

2. **[24_publisher](./publisher/)**:
   - An application that provides a network-discoverable *Public Service*, which acts as the *Data Publisher*. It generates and publishes data that is subscribed to by other applications. The publisher continuously provides updates based on the notification mode selected by the subscribers.

3. **[24_subscribermulti](./subscribermulti/)**:
   - An application that includes two *Data Subscribers* running within the same thread. These subscribers are dynamically activated at different times but both subscribe to the same data source provided by the *Publisher*. This sub-project showcases how multiple subscribers can coexist in a single thread and efficiently receive updates, reducing unnecessary data traffic.

## Communication

- **mcrouter**: All communication between the publisher and subscribers is handled via `mcrouter`, which is a router that facilitates message routing in distributed environments. This ensures seamless data exchange across processes in the network.
- **Object RPC for IPC**: The project relies on **Object RPC** for **Inter-Process Communication (IPC)**, which automates and simplifies the dispatching of messages between the service provider (publisher) and subscribers. This helps optimize communication between local and remote processes.

## Key Features

- **Optimized Data Delivery**: AREG Frameworks’s Pub/Sub system ensures that updates are sent efficiently, reducing unnecessary message traffic. This results in more optimized performance, especially in systems with multiple subscribers.
- **Multiple Subscribers in One Thread**: The ability to have multiple subscribers within the same thread receiving data updates demonstrates how AREG Framework can handle complex subscription scenarios without performance overhead.
- **Automatic Service Discovery**: The AREG Framework automates the discovery of the publisher by the subscribers, making process startup order irrelevant. Subscribers can dynamically connect to the publisher without needing to know the details of its location beforehand.

## Use Cases

- **Real-Time Data Monitoring**: The **24_pubsubmulti** project is ideal for applications where real-time data updates need to be delivered to multiple subscribers efficiently. This is particularly useful in scenarios where minimizing network traffic is important.
- **Multiple Subscriber Scenarios**: The ability to run multiple subscribers in a single process or a thread demonstrates how complex subscription patterns can be managed without introducing additional resource consumption.
- **Change-Driven Notifications**: The project’s support for *On Change* notifications makes it suitable for applications where updates are required only when data values change, improving performance in bandwidth-constrained environments.

## Conclusion

The **24_pubsubmulti** project showcases an efficient Pub/Sub system for multi-process environments, with a focus on minimizing event generation and ensuring efficient data delivery. By allowing multiple subscribers to coexist within a single thread and dynamically receive updates, the project highlights how AREG Framework’s Pub/Sub features can be used to build scalable and high-performance systems. The use of **Object RPC** for **Inter-Process Communication (IPC)**, combined with the fault-tolerant communication of `mcrouter`, ensures that the system remains robust and flexible, making it suitable for a wide range of real-time data distribution use cases.
