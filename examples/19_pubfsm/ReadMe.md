# 19_pubfsm Project Overview

The **19_pubfsm** project demonstrates how to build and manage a *Finite-State Machine* (**FSM**) using the AREG Framework's powerful features like *Timers* and *Events*. This project showcases how FSM models can be developed to change the data and the states of Services.

> [!IMPORTANT]
> To test this example, ensure an `mcrouter` process is running on a network-accessible machine to enable message routing. Verify that the `areg.init` configuration file includes the correct IP address and port number for the `mcrouter`.

## Key Features

- **Finite-State Machine (FSM)**: Learn how the AREG Framework enables the creation of FSMs for complex event-driven applications, controlling service states and handling transitions.
- **Multi-Instance Service Consumers**: The project demonstrates how dynamic models can allow multiple instances of *Service Consumers* to subscribe to data updates in a flexible and scalable manner.

## Sub-Projects

1. **19_generated**:
   - This sub-project is generated from the [PowerManager.siml](./services/PowerManager.siml) and [TrafficController.siml](./services/TrafficController.siml) service interface documents. The code is generated automatically during the CMake build process or as a pre-build action in Visual Studio. The generated code handles **Remote Procedure Calls (RPC)**, simplifying the communication between processes.

2. **[19_pubclient](./pubclient/)**:
   - This is a *Service Consumer* that can run as multiple consumer instances with unique names and subscribe to different data streams. Subscription commands include:
     - `sn` for the nominal *South-North* traffic-light direction data.
     - `ew` for the nominal *East-West* traffic-light direction data.

   This feature allows users to observe and interact with different data updates from the *Service Provider* in real time.

3. **[19_pubservice](./pubservice/)**:
   - This sub-project defines a *Public Service* that implements a **Finite-State Machine (FSM)** and imitates a traffic-light controller. The FSM model can be controlled using various commands, such as:
     - `on`: Turn the FSM on.
     - `start`: Start the FSM.
     - `stop`: Stop the FSM.
     - `off`: Turn the FSM off.
   
   This service interacts with *Service Consumers* through state transitions and broadcasts data updates when the FSM state changes.

## Communication and Service Discovery

- **mcrouter**: The communication between the *Service Provider* and *Service Consumers* is managed through `mcrouter`, AREG's router for handling RPC message routing and inter-process communication. 
- **Service Discovery and Fault Tolerance**: The AREG Framework ensures automatic service discovery and fault tolerance, meaning the order in which services start is irrelevant. This makes the system robust and reliable even in unpredictable environments.

## Use Cases

- **Finite-State Machines**: This project is ideal for applications that require complex state management with multiple events and triggers in a system with real-time data processing.
- **Scalable Consumer Management**: With dynamic FSM models, multiple instances of *Service Consumers* can subscribe to different data streams, making the architecture highly scalable.
- **Event-Driven Applications**: Perfect for developers looking to create event-driven applications that need to handle asynchronous communication and dynamic state transitions across multiple processes.

## Conclusion

The **19_pubfsm** project highlights the AREG Framework's ability to efficiently implement a *Finite-State Machine*. Whether you're developing device application or any event-driven systems, this project provides a scalable, robust solution using FSM, triggers, timers, and events. By leveraging the AREG SDK's features like service discovery and fault tolerance, developers can focus on building highly reliable, state-based systems with ease.
