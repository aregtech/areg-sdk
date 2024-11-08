# 20_pubdatarate Project Overview

The **20_pubdatarate** project demonstrates the network communication data rates within system using AREG SDK. This project highlights a service provider generating and sending a bitmap image data to connected service consumers, allowing developers to monitor and optimize data transfer rates in real-time. Both the **Service Provider** and **Service Consumer** applications display network data rates, providing insights into the performance of data-intensive applications.

> [!IMPORTANT]
> To test this example, ensure an `mcrouter` process is running on a network-accessible machine to enable message routing. Verify that the `areg.init` configuration file includes the correct IP address and port number for the `mcrouter`.

## Key Concepts

- **Data Rate Monitoring**: The project showcases how to network data rates can be measured during communication between the service provider and service consumers, helping optimize performance in real-time.
- **Service Interface Automation**: Code generated from the **Service Interface** document automates **Object Remote Procedure Call (Object RPC)** message creation, which streamlines communication between the host and remote consumers. This approach simplifies the handling of **Inter-Process Communication (IPC)**.

## Sub-Projects

1. **20_generated**:
   - Contains codes generated from the [LargeData.siml](./services/LargeData.siml) Service Interface document during CMake configuration or as a pre-build action in Visual Studio. The generated code automates **Object RPC** for message creation and dispatching, which is crucial for efficient **Inter-Process Communication (IPC)** between service components.

2. **[20_pubservice](./pubservice/)**:
   - This sub-project is a *Public Service* application that generates and sends bitmap image data to connected consumers. It dynamically responds to user input to create gray bitmap images, which are transmitted to service consumers for processing. The data rate for these transmissions is displayed in the console for real-time performance monitoring.

3. **[20_pubclient](./pubclient/)**:
   - A *Service Consumer* application that receives bitmap data from the service provider. It processes and displays the received image while tracking the network data rate for further analysis, offering developers insights into the application's data throughput.

## Communication

- **mcrouter**: All communication between the service provider and consumers is facilitated by `mcrouter`, which is a router capable of operating across networked machines. The AREG SDK ensures seamless service discovery, message routing, and fault tolerance, making it ideal for distributed systems where the order of process startup is irrelevant.
- **Object RPC (ORPC)**: The project leverages **Object Remote Procedure Call (Object RPC)** for message exchange, enabling efficient inter-process communication. This approach simplifies communication between the **Service Provider** and **Service Consumers** by automating the transfer of large data like bitmap images.

## Key Features

- **Real-Time Data Rate Monitoring**: Both the service provider and consumer applications display real-time data transfer rates, helping developers optimize network performance for applications requiring high data throughput.
- **Bitmap Image Transfer**: The service provider generates and transmits bitmap images based on user input, demonstrating the practical handling of large data in a networked environment.
- **Fault Tolerance**: The AREG SDK ensures robust fault tolerance through automatic service discovery, guaranteeing seamless communication between processes even in the face of network or process failures.
- **Efficient IPC**: The use of **Object RPC** simplifies inter-process communication, making the system more efficient when dealing with data-intensive tasks like bitmap image transmission.

## Use Cases

- **Network Performance Tuning**: This project is ideal for developers looking to optimize network communication based on AREG SDK.
- **Large Data Transmission**: The system demonstrates how to efficiently transmit large data sets, such as bitmap images, over the network while maintaining service reliability.
- **Fault-Tolerant Distributed Systems**: The project offers a practical example of how to build distributed systems that are resilient to faults and capable of dynamic service discovery and recovery.

## Conclusion

The **20_pubdatarate** project serves as a valuable example for developers seeking to optimize data transmission in multiprocessing applications based on AREG SDK. By generating and sending bitmap image data, the project demonstrates efficient **Inter-Process Communication (IPC)** using **Object RPC**. With real-time data rate monitoring, this project provides a solid foundation for applications that require high data throughput and robust network performance. Through its seamless integration with `mcrouter` and the AREG SDK, the project offers a scalable solution for handling data-intensive tasks in distributed environments.
