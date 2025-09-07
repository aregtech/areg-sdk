# 16_pubmesh

🧵🚀 **Type:** Multithreading + IPC

## Overview
The **16_pubmesh** project demonstrates how to create a **distributed service mesh** combining **Public** and **Local Services** in a **multithreaded, multi-process environment** using **Object Remote Procedure Call (Object RPC)**. The example shows how services interact seamlessly across processes and threads, leveraging automatic discovery and reliable message routing.

> [!NOTE]
> To run this example, a **Multi-Target Router** (`mtrouter`) must be running on a network-accessible machine. Ensure the `areg.init` file includes the correct IP address and port for the router.

## Concepts Shown
- **Distributed Service Mesh**: Multiple services (Public and Local) interact in real-time across threads and processes.
- **Object RPC for IPC**: Simplifies inter-process communication, automating request/response handling.
- **Service Discovery**: Services automatically detect each other using `serviceConnected()`, enabling dynamic network interaction.
- **Multithreaded Services**: Each service instance can run in its own thread, ensuring efficient parallel execution.
- **Fault Tolerance**: Services maintain reliable communication even if nodes start out of order or fail temporarily.

## How It Works
Code is generated from [LocalHelloWorld.siml](./services/LocalHelloWorld.siml), [PublicHelloWorld.siml](./services/PublicHelloWorld.siml), and [SystemShutdown.siml](./services/SystemShutdown.siml) to handle Object RPC communication. The system includes multiple applications hosting service providers and consumers that interact across threads and processes. All IPC messages are routed through `mtrouter`, ensuring reliable message delivery across the mesh.

## Sub-Projects
1. **16_generated**: Auto-generated code for Object RPC communication across services.
2. **[16_common](./common/)**: Shared components for reuse across sub-projects.
3. **[16_pubservice](./pubservice/)**: Hosts Public and Local Services, including a `"PublicControllerService"` to broadcast shutdown commands.
4. **[16_pubsvcmesh](./pubsvcmesh/)**: Demonstrates multiple providers and consumers interacting within the mesh.
5. **[16_pubclients](./pubclients/)**: Contains Public consumers and a Local Service, showing dynamic discovery and interaction.

## Use Cases
- **Distributed Service Networks**: Ideal for meshed architectures combining Public and Local Services across multiple machines.
- **Dynamic Service Interaction**: Services and consumers dynamically discover and communicate in real-time.
- **Message Routing via Router**: Ensures efficient delivery to multiple recipients across threads and processes.

## Takeaway
The **16_pubmesh** example demonstrates how to implement a **multithreaded, distributed service mesh** using **Object RPC**, automatic service discovery via `serviceConnected()`, and message routing through `mtrouter`, providing a scalable and fault-tolerant framework for complex multi-process applications.
