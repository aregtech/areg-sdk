# 19_pubfsm

🧵🚀 **Type:** Multithreading + IPC

## Overview
The **19_pubfsm** project demonstrates how to implement a **Finite-State Machine (FSM)** using the AREG Framework, combining **Timers** and **Events** to control the states and data of services. It highlights how FSMs can manage dynamic models and coordinate multiple service consumers in a **multiprocess** environment.

> [!NOTE]
> Ensure a **Multi-Target Router** (`mcrouter`) is running on a network-accessible machine. Verify that the `areg.init` configuration file includes the correct IP address and port for the router.

## Concepts Shown
- **Finite-State Machine (FSM)**: Control complex service states and transitions using timers and events.
- **Multi-Instance Service Consumers**: Dynamically create multiple consumer instances that subscribe to different data streams.
- **Timers and Events**: Use time-based and event-driven triggers to change FSM states and broadcast data updates.
- **Object RPC for IPC**: Enables seamless communication between service providers and consumers across processes.
- **Service Discovery**: Services are automatically discovered using `serviceConnected()`, ensuring robust communication regardless of startup order.

## How It Works
Generated code from [PowerManager.siml](./services/PowerManager.siml) and [TrafficController.siml](./services/TrafficController.siml) automates RPC communication between processes. The example includes a public service provider (`19_pubservice`) implementing the FSM, and multiple service consumers (`19_pubclient`) subscribing to data updates. Commands such as `on`, `start`, `stop`, and `off` control FSM state transitions while `sn` and `ew` commands allow consumers to monitor different traffic-light directions.

## Sub-Projects
1. **19_generated**: Contains auto-generated code for RPC handling from service interface documents.
2. **[19_pubclient](./pubclient/)**: Multiple consumer instances subscribing to different data streams in real-time.
3. **[19_pubservice](./pubservice/)**: Public service implementing the FSM, broadcasting state changes and updates to consumers.

## Use Cases
- **Finite-State Machines**: Ideal for applications requiring complex state management with timed or event-triggered transitions.
- **Scalable Consumer Handling**: Supports multiple service consumers subscribing to different data streams, enabling scalable architectures.
- **Event-Driven Systems**: Useful for systems requiring asynchronous event handling, real-time communication, and dynamic state transitions.

## Takeaway
The **19_pubfsm** example shows how the AREG SDK enables **robust FSM implementations** in a **multithreaded, distributed environment**, combining timers, events, Object RPC, and service discovery to create scalable, fault-tolerant, event-driven applications.
