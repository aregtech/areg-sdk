# 26_pubsubmix

🧵🚀 **Type:** Multithreading + IPC

## Overview

The **26_pubsubmix** project demonstrates a hybrid *Publish/Subscribe* (Pub/Sub) system where publishers and subscribers can operate locally within the same process or remotely across multiple processes. Subscribers reliably receive data updates in real time, even during network disruptions, ensuring seamless communication between local and distributed components.

> [!NOTE]
> This example requires **Multi-Target Router (`mcrouter`)** for message routing. Ensure `mcrouter` is running on a network-accessible machine and that the `areg.init` file contains the correct IP address and port.

## Concepts Shown

- **Hybrid Pub/Sub Model**: Supports publishers and subscribers in the same process or across processes, ensuring robust communication.  
- **Service Interface Automation**: Generated code automates **Object RPC** messaging, simplifying Pub/Sub implementation and improving **IPC** efficiency.  
- **Object RPC for IPC**: Provides high-performance inter-process communication between publishers and subscribers.  
- **Dynamic Subscription and Publishing**: Processes can switch roles between publisher and subscriber based on system requirements.  
- **Network Resilience**: Local subscribers receive updates regardless of network connectivity, maintaining uninterrupted service.

## How It Works

The **26_pubsubmix** system consists of multiple processes that can act as both publishers and subscribers. Publishers broadcast updates to all connected subscribers, while subscribers automatically receive the latest data upon subscription. Local subscribers in the same process are updated instantly, and remote subscribers rely on `mcrouter` for reliable, fault-tolerant message delivery.

Processes like **26_pubsubctrl** and **26_pubsubdyn** manage both publishing and subscribing dynamically, allowing seamless switching of roles while maintaining synchronized state across the system. The AREG Framework handles service discovery via `serviceConnected()` and ensures fault-tolerant IPC.

## Sub-Projects

1. **26_generated**:  
   - Generated code from [PubSubMix.siml](./services/PubSubMix.siml) automates **Object RPC** messaging for efficient communication across processes.

2. **[26_common](./common/)**:  
   - Static library containing shared objects and functions for reuse across project processes.

3. **[26_pubsubctrl](./pubsubctrl/)**:  
   - Acts as both *Publisher* and *Subscriber*, managing data updates and sending shutdown signals to connected processes if needed.

4. **[26_pubsubdyn](./pubsubdyn/)**:  
   - Dynamically connects to **26_pubsubctrl**, publishing and receiving updates while switching roles between publisher and subscriber as required.

## Use Cases

- **Multi-Process Communication**: Ideal for systems combining local and remote subscribers to dynamically share data.  
- **Resilient Pub/Sub Systems**: Maintains updates during network disruptions, supporting reliability-critical environments.  
- **Dynamic Data Distribution**: Enables flexible creation and connection of publishers and subscribers for real-time, multi-process data delivery.

## Takeaway

Demonstrates a resilient hybrid Pub/Sub system with local and remote subscribers, leveraging **Object RPC** and fault-tolerant IPC via `mcrouter` for reliable real-time data distribution.
