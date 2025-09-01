# 27_pubsubmulti

🧵🚀 **Type:** Multithreading + IPC

## Overview

The **27_pubsubmulti** project demonstrates a multi-process Pub/Sub (Publish/Subscribe) system where one process acts as a publisher and another process hosts multiple subscribers running in the same thread. The example highlights efficient event delivery by sending updates only when necessary, reducing unnecessary network traffic while ensuring subscribers receive relevant data.

> [!NOTE]
> This example requires **Multi-Target Router (`mcrouter`)** for message routing. Ensure `mcrouter` is running on a network-accessible machine and that the `areg.init` file contains the correct IP address and port.

## Concepts Shown

- **Efficient Event Delivery**: Sends notifications only when needed, reducing unnecessary data traffic.  
- **Multiple Subscribers in One Thread**: Demonstrates two subscribers sharing a single thread subscribing to the same data source at different times.  
- **Dynamic Subscription**: Subscribers can dynamically subscribe to the published data, receiving notifications according to the selected mode.  
- **Dual Notification Modes**: Supports **On Change** and **Always** modes for flexible subscriber updates.  
- **Object RPC for IPC**: Automates message creation and dispatching for efficient inter-process communication.

## How It Works

A publisher process continuously generates data and broadcasts it to subscribers using AREG’s Pub/Sub system. Multiple subscribers in a single thread can dynamically connect and receive updates from the publisher. Notifications are delivered based on the subscriber’s selected mode: either only when data changes or every time data is set.

Communication is managed by **mcrouter**, providing reliable inter-process message delivery. The AREG Framework handles automatic service discovery via `serviceConnected()`, allowing subscribers to connect to the publisher regardless of process startup order.

## Sub-Projects

1. **27_generated**:  
   - Generated code from [PubSub.siml](./services/PubSub.siml) automates **Object RPC** messaging, streamlining communication between publisher and subscribers for efficient **IPC**.

2. **[27_publisher](./publisher/)**:  
   - Hosts the *Public Service* acting as the **Publisher**, broadcasting data updates to connected subscribers according to their notification mode.

3. **[27_subscribermulti](./subscribermulti/)**:  
   - Contains two *Data Subscribers* running in the same thread. Subscribers dynamically activate at different times, subscribing to the same data source, demonstrating efficient multi-subscriber handling within a single thread.

## Use Cases

- **Real-Time Data Monitoring**: Efficiently deliver updates to multiple subscribers with minimal network traffic.  
- **Multi-Subscriber Environments**: Manage multiple subscribers in one thread without performance overhead.  
- **Change-Driven Notifications**: Suitable for scenarios where updates are needed only when data values change, optimizing bandwidth and performance.

## Takeaway

Illustrates a scalable, multi-process Pub/Sub system with efficient event delivery, supporting multiple subscribers in a single thread and leveraging **Object RPC** and `mcrouter` for fault-tolerant IPC.
