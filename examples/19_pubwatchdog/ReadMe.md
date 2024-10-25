# 19_pubwatchdog Project Overview

The **19_pubwatchdog** project demonstrates the implementation of a **Watchdog** in a **Public Service** environment using the AREG Framework. It showcases how the watchdog monitors threads by setting timeouts, automatically restarting threads that fail to respond, and sending notifications to connected **Service Providers** and/or **Service Consumers**. This example highlights the AREG Framework's capabilities in maintaining a fault-tolerant, distributed system through effective thread management.

## Key Concepts

- **Watchdog for Public Services**: The Watchdog monitors threads in the application, ensuring they are responsive. If a thread fails to respond, the watchdog terminates and restarts the thread, while notifying all connected service providers and consumers.
- **Service Interface Automation**: Using the AREG SDK's Service Interface and code generator, **Object Remote Procedure Call (Object RPC)** message creation and dispatching are automated. This enhances efficiency in communication between service components and simplifies the development process.
- **Fault Tolerance**: The project demonstrates automatic thread recovery in real-time, ensuring the public service remains operational despite thread failures.

## Project Structure

1. **19_generated**:
   - Contains code generated from the [HelloWatchdog.siml](./res/HelloWatchdog.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio. This generated code defines the necessary infrastructure for the Watchdog service and automates **Object RPC** messaging.

2. **[19_pubclient](./pubclient/)**:
   - A *Service Consumer* application that connects to the *Public Service Provider*. It sends requests to simulate delayed responses, triggering the watchdog's timeout mechanism.

3. **[19_pubservice](./pubservice/)**:
   - A *Public Service Provider* application where a thread, controlled by the watchdog, handles service requests. When the service thread becomes unresponsive (e.g., due to a simulated delay), the watchdog:
     - Terminates the unresponsive thread.
     - Sends a **service disconnected** notification to all connected service consumers.
     - Restarts the thread and resumes the service.
     - Notifies all service consumers with a **service connected** message.
   - Once reconnected, service consumers can continue sending requests to the newly restarted service.

## Communication

- **mcrouter**: All communication between the service provider and service consumers is handled through `mcrouter`, which is a router that supports distributed systems. The AREG SDK ensures automatic service discovery, efficient message routing, and fault tolerance, making the startup order of processes irrelevant.
- **Object RPC (ORPC)**: The AREG SDK automates **Object Remote Procedure Call (ORPC)** message creation and dispatching, enabling seamless communication between the **Service Provider** and **Service Consumers**.

## Key Features

- **Watchdog-Controlled Threads**: The project demonstrates the use of a watchdog to monitor service threads, automatically restarting unresponsive threads and notifying all connected consumers.
- **Automatic Service Recovery**: The watchdog ensures that services remain available by automatically restarting threads when failures occur, minimizing downtime.
- **Seamless Consumer Notifications**: Connected service consumers are automatically notified when a service disconnects or reconnects, allowing them to continue communication with minimal disruption.
- **Multithreaded Public Services**: The project showcases the interaction between multiple service components and threads, all managed within a fault-tolerant public service architecture.

## Use Cases

- **Public Service Fault Tolerance**: This project is ideal for scenarios where public services need to maintain high availability. The watchdog ensures continuous operation by monitoring and recovering unresponsive service threads.
- **Real-Time Thread Monitoring**: Learn how to implement real-time monitoring and automatic recovery of service threads using a watchdog in an application.
- **Distributed Systems**: The project demonstrates best practices for building distributed systems with public services that require fault tolerance and automatic service discovery.

## Conclusion

The **19_pubwatchdog** project provides a comprehensive example of using a watchdog to maintain the stability of an application of distributed services. By automatically restarting unresponsive threads and sending notifications to connected consumers, the project showcases how the AREG SDK can be leveraged to build resilient, fault-tolerant applications. Through the use of **Object RPC** and **mcrouter**, communication between services is efficient and robust, ensuring seamless interaction in real-time applications.
