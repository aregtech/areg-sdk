# 23_pubdatarate

🚀 **Type:** IPC / Multi-Process

## Overview

The **23_pubdatarate** project demonstrates real-time monitoring of network data rates in a distributed system using the Areg SDK. A **Public Service Provider** generates and transmits bitmap image data to connected **Service Consumers**, allowing developers to observe and optimize data throughput in data-intensive applications.

> [!NOTE]
> This example requires **Multi-Target Router (`mtrouter`)** for message routing. Ensure `mtrouter` is running on a network-accessible machine and that the `areg.init` file has the correct IP address and port.

## Concepts Shown

- **Real-Time Data Rate Monitoring**: Measure and display network throughput for communications between provider and consumers.  
- **Service Interface Automation**: Uses the Areg SDK's code generator to automate **Object RPC** message creation and dispatching.  
- **Efficient Large Data Transfer**: Demonstrates sending and receiving bitmap images over IPC.  
- **Fault-Tolerant Communication**: Ensures reliable delivery of messages even if network or process issues occur.  

## How It Works

The service provider generates bitmap images based on user input and transmits them to connected consumers via `mtrouter`. Both the provider and consumers display data transfer rates in real-time, enabling developers to assess and optimize throughput. The Areg Framework automates **service discovery** using `serviceConnected()` and manages message routing seamlessly across processes.

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
