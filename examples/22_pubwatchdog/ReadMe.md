# 22_pubwatchdog

🚀 **Type:** IPC / Multi-Process

## Overview

The **22_pubwatchdog** project demonstrates the use of a **Watchdog** in a distributed **Public Service** environment using the AREG Framework. The Watchdog monitors service threads, automatically restarting any that fail to respond, and notifies connected **Service Providers** and **Service Consumers**, ensuring fault-tolerant and reliable system operation.

> [!NOTE]
> This example requires **Multi-Target Router (`mcrouter`)** to route messages between processes. Ensure `mcrouter` is running on a network-accessible machine and that the `areg.init` file contains the correct IP address and port.

## Concepts Shown

- **Watchdog-Controlled Threads**: Automatically monitors and restarts unresponsive service threads.  
- **Service Interface Automation**: Uses the AREG SDK's code generator to automate **Object RPC** creation and dispatching.  
- **Fault-Tolerant Public Services**: Demonstrates real-time thread recovery and uninterrupted service operation.  
- **Consumer Notifications**: Sends **serviceConnected()** or disconnect notifications to all connected consumers.  

## How It Works

The project consists of a public service provider and consumer applications communicating via `mcrouter`. The provider runs service threads monitored by the Watchdog. If a thread becomes unresponsive, the Watchdog terminates it, sends a **service disconnected** notification to all consumers, restarts the thread, and sends a **service connected** notification. Consumers automatically resume communication with the restarted service.

The AREG Framework automates service discovery using `serviceConnected()` and manages inter-process communication (IPC) reliably, regardless of the startup order of the processes.

## Sub-Projects

1. **22_generated**:  
   - Contains code generated from [HelloWatchdog.siml](./services/HelloWatchdog.siml) during CMake configuration or pre-build actions. This code automates **Object RPC** messaging and provides the infrastructure for the Watchdog service.

2. **[22_pubclient](./pubclient/)**:  
   - Public Service Consumer that sends requests to the provider, simulating delayed responses to trigger the Watchdog timeout mechanism.

3. **[22_pubservice](./pubservice/)**:  
   - Public Service Provider where threads, controlled by the Watchdog, handle incoming requests. Unresponsive threads are terminated, restarted, and consumers are notified automatically.

## Use Cases

- **High-Availability Public Services**: Maintain continuous service operation in distributed environments.  
- **Real-Time Thread Monitoring**: Learn how to monitor and recover threads automatically in production-grade systems.  
- **Distributed System Reliability**: Demonstrates best practices for fault-tolerant IPC and service discovery.

## Takeaway

Showcases how a Watchdog can maintain fault-tolerant, distributed public services with automatic thread recovery and seamless consumer notifications.
