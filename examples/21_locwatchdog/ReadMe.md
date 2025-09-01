# 21_locwatchdog

🧵 **Type:** Multithreading / Single Process

## Overview
The **21_locwatchdog** project demonstrates the use of a **Watchdog** within a **Local Service** environment, showing how AREG SDK can monitor and maintain thread activity. The watchdog enforces timeouts for threads, automatically restarting any that become unresponsive, and logs thread restarts for debugging and system monitoring.

## Concepts Shown

- **Watchdog for Thread Monitoring**: Automatically detects unresponsive threads and restarts them to maintain system stability.
- **Local Service with Multithreading**: Implements multiple threads within the same process, including separate **Service Provider** and **Service Consumer** threads.
- **Service Interface Automation**: Uses AREG SDK’s Service Interface and code generator to automate **Object RPC** message creation.
- **Fault Tolerance**: Ensures continuous operation by monitoring thread health and handling unresponsive threads.
- **Logging and Observability**: Provides detailed logs of thread activity, watchdog actions, and service interactions.

## How It Works

The project contains a **Local Service** that runs entirely within a single process, with separate threads for the **Service Provider** and **Service Consumer**. A **Watchdog** monitors all threads, and if any thread exceeds its allowed response time, the watchdog terminates and restarts it. The **Service Consumer** logs these events, offering visibility into thread health and recovery.

Generated code from [HelloWatchdog.siml](./services/HelloWatchdog.siml) automates **Object RPC** communication between service components, ensuring smooth interaction within the multithreaded environment. The project demonstrates robust internal communication without requiring external network connections.

## Use Cases

- **Fault-Tolerant Systems**: Maintain system stability by automatically recovering unresponsive threads.
- **Multithreaded Local Services**: Explore interactions between multiple threads running in the same process.
- **Watchdog Monitoring**: Learn how to implement and monitor background tasks in a multithreaded environment.

## Takeaway

This project illustrates how a **Watchdog** can maintain thread health and ensure fault-tolerant, multithreaded services using AREG SDK.
