# 13_locsvc

🧵 **Type:** Multithreading / Single Process

## Overview
The **13_locsvc** example demonstrates how to create and manage a **Local Service** within a single, **multithreaded process** using the Areg Framework.  
It shows a service provider and consumer communicating asynchronously, without exposing the service externally.  
The consumer uses `serviceConnected()` to detect when the provider is ready, ensuring requests and subscriptions occur only when safe.

This addresses common challenges in multithreaded service design: coordinating service availability, handling request/response flows, and managing event subscriptions without boilerplate code.

## Concepts Shown
- **Local Service**: Provider and consumer operate entirely within one process for secure internal communication.  
- **Multithreading**: Components can run in separate threads and communicate asynchronously.  
- **Auto-Discovery with `serviceConnected()`**: Consumer subscribes to events and sends requests only when the provider is connected.  
- **Request/Response Handling**: Structured request/response flows with automatic event broadcasting.  
- **Runtime Subscription**: Consumers can dynamically subscribe or unsubscribe from provider events.

## How It Works
The project contains two subprojects:

1. **13_generated** – Contains code generated from [`HelloWorld.siml`](./services/HelloWorld.siml) via `codegen.jar`. Provides the necessary RPC glue for the local service.  

2. **[13_locservice](./locservice/)** – Implements both the service provider and consumer.  
   - The **provider** handles incoming requests and broadcasts events.  
   - The **consumer** waits for `serviceConnected()` notifications before sending requests or subscribing to updates.  
   - Communication is fully asynchronous and event-driven, demonstrating real-time inter-thread interaction within a single process.

At runtime, threads and components are created automatically. Consumers only send requests or subscribe to events **after receiving a connected status** from `serviceConnected()`. The provider can broadcast updates to all subscribed consumers safely, and disconnections are handled by cleaning up subscriptions.

## Use Cases
- Build **secure, internal services** confined to a single process.  
- Learn **asynchronous request/response and event broadcasting** between threads.  
- Implement **dynamic subscription management**, allowing consumers to adapt at runtime.  
- Use as a foundation for **multithreaded service architectures** without external dependencies.

## Takeaway
A clear, practical example of implementing a **Local Service in a multithreaded process**, demonstrating **safe request/response handling, asynchronous communication, and dynamic event subscriptions** using `serviceConnected()`.
