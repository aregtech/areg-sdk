# 20_winchat

🧵🚀 **Type:** Multithreading + IPC

## Overview
The **20_winchat** project is a **Windows-exclusive GUI application** demonstrating a fault-tolerant, distributed communication system using the AREG SDK. It features **dynamic model creation**, **runtime service subscription**, and **inter-process communication (IPC)**. Built with **MFC (Microsoft Foundation Classes)**, it requires Microsoft Visual C++ for development.

> [!NOTE]
> Ensure a **Multi-Target Router** (`mtrouter`) is running on a network-accessible machine. Verify that the `areg.init` configuration file includes the correct IP address and port for the router.

## Concepts Shown
- **Windows GUI Application**: MFC-based GUI to visualize and interact with distributed services.
- **Fault-Tolerant Distributed System**: Automated service discovery and connection management ensure reliability.
- **Dynamic Model Creation**: Models are created at runtime, enabling flexible interaction with services.
- **Object RPC for IPC**: Automates inter-process communication between service providers and consumers.
- **Service Discovery**: Services are automatically discovered via `serviceConnected()` to simplify setup and enhance robustness.

## How It Works
Generated code from [CentralMessager.siml](./services/CentralMessager.siml), [ConnectionManager.siml](./services/ConnectionManager.siml), [DirectConnection.siml](./services/DirectConnection.siml), and [DirectMessager.siml](./services/DirectMessager.siml) handles RPC communication between processes. The **register** application (`20_register`) hosts a public service, while the **chatter** application (`20_chatter`) dynamically creates consumers and providers at runtime. Users provide the `mtrouter` IP to enable message routing between components.

## Sub-Projects
1. **20_generated**: Auto-generated code for RPC and service interaction from service interface documents.
2. **[20_register](./register/)**: Hosts a public service and connects to `mtrouter` for network communication.
3. **[20_chatter](./chatter/)**: Hosts public services and consumers; dynamically creates models at runtime and interacts with `mtrouter`.

## Use Cases
- **Distributed Communication Systems**: Demonstrates building fault-tolerant systems with multiple services.
- **Dynamic Runtime Models**: Useful for applications requiring runtime service creation and subscription management.
- **Message Routing**: Shows how to route messages across networked services using `mtrouter` in a Windows environment.

## Takeaway
The **20_winchat** example illustrates how to build a **Windows-exclusive, MFC-based distributed system** with **dynamic models**, **automatic service discovery**, and **robust IPC** using the AREG SDK, making it an essential reference for developers designing scalable, real-time communication systems.
