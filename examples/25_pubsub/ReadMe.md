# 25_pubsub

🚀 **Type:** IPC / Multi-Process

## Overview

The **25_pubsub** project demonstrates a distributed system using the *Publish/Subscribe* (Pub/Sub) model. A *Public Service* (**Publisher**) broadcasts data updates to multiple subscribing processes (**Subscribers**). The publisher supports two notification modes: **On Change** (notify only when data changes) and **Always** (notify on every update). When no subscribers are present, updates occur internally without notifications, reducing unnecessary network traffic.

> [!NOTE]
> This example requires **Multi-Target Router (`mtrouter`)** for message routing. Ensure `mtrouter` is running on a network-accessible machine and that the `areg.init` file contains the correct IP address and port.

## Concepts Shown

- **Publish/Subscribe Model**: Dynamic publisher-subscriber communication, allowing subscribers to join or leave at any time.  
- **Service Interface Automation**: Generated code automates **Object RPC** messaging for efficient **IPC**.  
- **Object RPC for IPC**: Ensures high-performance, reliable communication between publisher and subscribers.  
- **Real-Time Data Updates**: Subscribers automatically receive the latest data upon connecting, even if no recent updates occurred.

## How It Works

The **Publisher** broadcasts updates to all connected subscribers. Depending on the selected mode, notifications are sent only on data changes or every update. New subscribers immediately receive the latest data to remain in sync with the system state.

**Subscribers** register with the publisher and receive notifications as data updates. The system leverages `mtrouter` for cross-machine message routing, while the Areg Framework automates service discovery using `serviceConnected()` and ensures fault-tolerant, seamless inter-process communication.

## Sub-Projects

1. **25_generated**:  
   - Generated code from [PubSub.siml](./services/PubSub.siml) automates **Object RPC** messaging, simplifying IPC between publisher and subscribers.

2. **[25_publisher](./publisher/)**:  
   - Acts as the Public Service Provider (**Publisher**), sending updates to subscribers according to the configured notification mode.

3. **[25_subscriber](./subscriber/)**:  
   - Client application acting as a **Subscriber**. Multiple instances can run simultaneously, each receiving updates from the publisher and immediately syncing on connection.

## Use Cases

- **Real-Time Data Distribution**: Suitable for systems requiring instant updates, such as sensor monitoring, stock tickers, or event notifications.  
- **Multi-Process Systems**: Supports dynamic joining and leaving of processes without interrupting the data flow.  
- **Flexible Notification Modes**: Choose between notifications only on data changes or on every update for different application needs.

## Takeaway

Demonstrates a scalable Pub/Sub implementation using Areg SDK, enabling reliable, real-time data distribution between multiple processes with fault-tolerant IPC.
