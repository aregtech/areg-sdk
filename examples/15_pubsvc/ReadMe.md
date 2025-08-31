# 15_pubsvc

🚀 **Type:** IPC / Multi-Process

## Overview
The **15_pubsvc** project demonstrates a *Public Service* in a **multi-process environment** using **Object Remote Procedure Call (Object RPC)** for inter-process communication (IPC). It involves a *Service Provider* and a *Service Consumer*, showing how AREG Framework handles distributed services, automatic discovery, and robust IPC.

> [!NOTE]
> To run this example, a **Multi-Target Router** (`mcrouter`) must be running on a network-accessible machine. Ensure the `areg.init` configuration file contains the correct IP address and port for the router.

## Concepts Shown
- **Public Service**: A discoverable service accessible by remote consumers over the network.
- **Object RPC for IPC**: Simplifies communication between processes via automated request/response dispatching.
- **Service Discovery**: Consumers automatically detect the provider using `serviceConnected()`, making startup order irrelevant.
- **Fault Tolerance**: Services can recover from failures without affecting system operation.
- **Efficient Communication**: Object RPC reduces overhead and ensures reliable message delivery between processes.

## How It Works
Code is generated from [HelloWorld.siml](./services/HelloWorld.siml) to define the service interface. The *Service Provider* listens for remote requests, while the *Service Consumer* automatically discovers it and periodically sends requests. All IPC messages are routed through `mcrouter`, enabling communication across networked machines.

## Sub-Projects
1. **15_generated**: Auto-generated Object RPC code from the service interface document for cross-process communication.
2. **[15_pubservice](./pubservice/)**: Implements the network-discoverable *Public Service* provider that handles requests from multiple consumers.
3. **[15_pubclient](./pubclient/)**: Implements a consumer that discovers the provider and sends periodic requests, simulating real-world interactions.

## Use Cases
- Distributed multiprocess applications where multiple clients communicate with a central service provider.
- Public, network-accessible services requiring high availability, automated discovery, and reliable message handling.

## Takeaway
The **15_pubsvc** example demonstrates how to implement a **Public Service** with **Object RPC** in a **multi-process environment**, using `mcrouter` for message routing, automatic service discovery via `serviceConnected()`, and fault-tolerant communication across distributed applications.
