# 18_pubworker

🧵🚀 **Type:** Multithreading + IPC

## Overview
The **18_pubworker** project demonstrates how to use **Worker Threads** for background processing and custom event communication between **Worker** and **Component Threads** within the same process. It also leverages **Object Remote Procedure Call (Object RPC)** and **Inter-Process Communication (IPC)** to manage service attributes and offload tasks efficiently.

> [!NOTE]
> Ensure a **Multi-Target Router** (`mtrouter`) is running on a network-accessible machine. The `areg.init` configuration file must include the correct IP and port for the router.

## Concepts Shown
- **Worker Threads**: Offload background tasks while keeping component threads responsive.
- **Custom Event Handling**: Process events in both worker and component threads for flexible task management.
- **Object RPC for IPC**: Automates communication between service providers and consumers across processes.
- **Service Discovery**: Services are automatically discovered using `serviceConnected()`, regardless of process startup order.
- **Multithreaded and Multiprocess Execution**: Demonstrates coordinated operation of multiple threads and processes.

## How It Works
Generated code from [PatientInformation.siml](./services/PatientInformation.siml) handles Object RPC communication. The example includes a service provider (`18_pubservice`) and a consumer (`18_pubclient`), each with worker threads that handle background tasks like user input simulation or attribute updates. Messages and events are routed through `mtrouter` to ensure robust inter-thread and inter-process communication.

## Sub-Projects
1. **18_generated**: Library with auto-generated Object RPC code for service communication.
2. **[18_pubclient](./pubclient/)**: Service consumer with a worker thread simulating background tasks while interacting with the provider.
3. **[18_pubservice](./pubservice/)**: Public service provider with a worker thread for user input and attribute updates, all managed within the static model initialized at startup.

## Use Cases
- **Background Task Offloading**: Offload tasks such as device simulation or data processing to worker threads to keep main threads responsive.
- **Device Applications**: Handle long waits for user input or device events without blocking other service operations.
- **Distributed Service Management**: Enable seamless communication between distributed services and threads via Object RPC and IPC.
- **Event-Driven Architectures**: Demonstrates handling of user-triggered custom events in a multithreaded environment.

## Takeaway
The **18_pubworker** example shows how **Worker Threads**, combined with **Object RPC** and **IPC**, enable efficient background processing and event handling in **multithreaded, distributed service applications**, providing a template for scalable and responsive systems using the Areg SDK.
