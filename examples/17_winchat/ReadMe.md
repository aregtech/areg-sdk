# 17_winchat Project Overview

The **17_winchat** project is a Windows-exclusive GUI application that demonstrates a fault-tolerant, distributed communication system using the AREG Framework. It highlights dynamic model creation, runtime subscription services, and inter-process communication (IPC) in a distributed environment. This project is designed to compile with **MFC (Microsoft Foundation Classes)** and requires Microsoft Visual C++ for development.

> [!IMPORTANT]
> To test this example, ensure an `mcrouter` process is running on a network-accessible machine to enable message routing. Verify that the `areg.init` configuration file includes the correct IP address and port number for the `mcrouter`.

## Key Concepts

- **Windows GUI Application**: This project leverages MFC for building a graphical user interface on Windows, showcasing distributed system communication in real-time.
- **Fault-Tolerant Distributed System**: The project demonstrates a fault-tolerant architecture where service discovery and connection handling are automated.
- **Dynamic Model Creation**: Models are created dynamically at runtime, enabling flexible service interaction.
- **Service Interface**: Utilize the AREG SDK's Service Interface and code generator to automate **Object Remote Procedure Call (Object RPC)** message creation, simplifying IPC and reducing manual effort.

## Sub-Projects

1. **17_generated**:
   - Contains codes generated from [CentralMessager.siml](./res/CentralMessager.siml), [ConnectionManager.siml](./res/ConnectionManager.siml), [DirectConnection.siml](./res/DirectConnection.siml), and [DirectMessager.siml](./res/DirectMessager.siml). These service interfaces define the interactions between various components, creating all necessary code for seamless communication. The code is generated automatically during the CMake build process or as a pre-build action in Visual Studio. The generated code handles **Remote Procedure Calls (RPC)**, simplifying the communication between processes.

2. **[17_register](./register/)**:
   - This application hosts a *Public Service* that other applications can utilize. Upon startup, users input the IP address of the `mcrouter` for network communication.

3. **[17_chatter](./chatter/)**:
   - This application provides both *Public Services* and *Service Consumers*. It dynamically creates service models during runtime, and like the register application, users must input the `mcrouter` IP address to facilitate communication.

## Communication

Communication between the service provider, consumer, and worker threads is managed via `mcrouter`, which is a router that can operate across any networked machine. The AREG Framework automates **service discovery** and provides **fault tolerance**, ensuring reliable **IPC** and **RPC** communication regardless of process startup order. The system dynamically discovers services, and messages are routed to the appropriate threads and processes.

## Key Features

- **Windows GUI with MFC**: The project provides a user-friendly interface for managing and interacting with distributed services in a fault-tolerant system.
- **Service Discovery**: Automatic service discovery ensures that applications can find each other on the network without manual intervention, enhancing flexibility and reliability.
- **Runtime Dynamic Models**: Services and communication models are created dynamically during runtime, allowing for adaptable and scalable communication between distributed services.

## Use Cases

- **Distributed Communication Systems**: Ideal for learning how to build and manage a fault-tolerant communication system in an environment of distributed services.
- **Dynamic Model Creation**: The project is useful for understanding how to dynamically create and manage service models at runtime, a key feature for scalable applications.
- **Message Routing**: Explore how to handle communication in a networked environment using a router (`mcrouter`), which allows multiple services to connect and interact in real time.

## Conclusion

The **17_winchat** project provides a practical example of building a **Windows-exclusive**, MFC-based distributed system using the AREG SDK. With dynamic model creation, automatic service discovery, and robust IPC through **Object RPC**, this project serves as a valuable resource for developers aiming to implement fault-tolerant systems with real-time communication.
