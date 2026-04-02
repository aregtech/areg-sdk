# 23_pubdatarate

🚀 **Type:** IPC / Multi-Process

## Overview

Distributed systems demand frameworks that stay out of the way when data moves fast.
**23_pubdatarate** stress-tests exactly that: a **Public Service Provider** streams
continuous bitmap payloads to one or more **Service Consumers** across process
boundaries, while the framework routes, delivers, and tracks every message -- all
without a dedicated networking library underneath.

The result on a single machine with a **mobile-class CPU**:

| Platform            | Measured Throughput |
|---------------------|---------------------|
| Windows (localhost) | **~1.5 GB/s**       |
| WSL / Bridged NIC   | **~2.5 GB/s**       |

These numbers come from a full-stack, service-oriented pipeline -- location-transparent
service discovery, type-safe IPC, automatic reconnection -- none of which is stripped
away for the benchmark. What you measure here is what your production system gets.

Use this example to:

- **Profile** data-intensive pipelines before committing to an architecture.
- **Validate** that the framework overhead is negligible for your use case.
- **Experiment** with subscriber counts and payload sizes to find your system's ceiling.

> [!NOTE]
> This example requires **Multi-Target Router (`mtrouter`)** for inter-process message
> routing. Ensure `mtrouter` is running on a network-accessible machine and that the
> `areg.init` file has the correct IP address and port configured.

## Concepts Shown

- **Real-Time Data Rate Monitoring**: Measure and display network throughput for communications between provider and consumers.  
- **Service Interface Automation**: Uses the Areg SDK's code generator to automate **Object RPC** message creation and dispatching.  
- **Efficient Large Data Transfer**: Demonstrates sending and receiving bitmap images over IPC.  
- **Fault-Tolerant Communication**: Ensures reliable delivery of messages even if network or process issues occur.  

## How It Works

The service provider generates bitmap images based on user input and transmits them to connected consumers via `mtrouter`. Both the provider and consumers display data transfer rates in real-time, enabling developers to assess and optimize throughput. The Areg Framework automates **service discovery** using `service_connected()` and manages message routing seamlessly across processes.

The **Object RPC** mechanism handles large payloads efficiently, simplifying inter-process communication for high-data scenarios.

## Sub-Projects

1. **23_generated**:  
   - Generated code from [LargeData.siml](./services/LargeData.siml) automates **Object RPC** creation and dispatching, providing the IPC infrastructure for large data communication.

2. **[23_pubservice](./pubservice/)**:  
   - Public Service Provider that creates gray bitmap images in response to user input and transmits them to connected consumers while displaying real-time data rates.

3. **[23_pubclient](./pubclient/)**:  
   - Service Consumer that receives bitmap images, processes them, and displays network throughput, providing insights into the performance of data-intensive communication.

## Use Cases

- **Network Performance Analysis**: Monitor and optimize network throughput in distributed systems.  
- **Large Data Transmission**: Demonstrates reliable transfer of sizable data payloads like images.  
- **Distributed Fault-Tolerant Systems**: Build resilient systems with automatic service discovery and robust inter-process communication.

## Takeaway

Provides a practical example of monitoring and optimizing high-throughput IPC using **Object RPC** in distributed Areg SDK applications.
