# 17_winchat Project Overview

The **17_winchat** project is a Windows-exclusive GUI application that demonstrates a fault-tolerant, distributed communication system using the AREG Framework. It highlights dynamic model creation, runtime subscription services, and inter-process communication (IPC) in a distributed environment. This project is designed to compile with **MFC (Microsoft Foundation Classes)** and requires Microsoft Visual C++ for development.

## Key Concepts

- **Windows GUI Application**: This project leverages MFC for building a graphical user interface on Windows, showcasing distributed system communication in real-time.
- **Fault-Tolerant Distributed System**: The project demonstrates a fault-tolerant architecture where service discovery and connection handling are automated.
- **Dynamic Model Creation**: Models are created dynamically at runtime, enabling flexible service interaction.
- **Service Interface**: Utilize the AREG SDK's Service Interface and code generator to automate **Object Remote Procedure Call (Object RPC)** message creation, simplifying IPC and reducing manual effort.

## Project Structure

1. **17_generated**:
   - Code generated from [CentralMessager.siml](./res/CentralMessager.siml), [ConnectionManager.siml](./res/ConnectionManager.siml), [DirectConnection.siml](./res/DirectConnection.siml), and [DirectMessager.siml](./res/DirectMessager.siml). These service interfaces define the interactions between various components, creating all necessary code for seamless communication.

2. **[17_register](./register/)**:
   - This application hosts a *Public Service* that other applications can utilize. Upon startup, users input the IP address of the `mcrouter` for network communication.

3. **[17_chatter](./chatter/)**:
   - This application provides both *Public Services* and *Service Consumers*. It dynamically creates service models during runtime, and like the register application, users must input the `mcrouter` IP address to facilitate communication.

## Communication

- **mcrouter**: All communication in the project is handled by `mcrouter`, which is responsible for routing messages between distributed services. The AREG Framework automates service discovery and ensures that communication between services remains reliable, even if the order of startup is inconsistent.
- **Object RPC (ORPC)**: The project uses **Object Remote Procedure Call (ORPC)** to manage remote communication between services, automating the message dispatching process for seamless IPC.

## Key Features

- **Windows GUI with MFC**: The project provides a user-friendly interface for managing and interacting with distributed services in a fault-tolerant system.
- **Service Discovery**: Automatic service discovery ensures that applications can find each other on the network without manual intervention, enhancing flexibility and reliability.
- **Runtime Dynamic Models**: Services and communication models are created dynamically during runtime, allowing for adaptable and scalable communication between distributed services.
- **Manual mcrouter IP Entry**: Users manually input the `mcrouter` IP address at startup, ensuring that communication is correctly routed in the network.

## Use Cases

- **Distributed Communication Systems**: Ideal for learning how to build and manage a fault-tolerant communication system in a distributed environment using Windows GUI applications.
- **Dynamic Model Creation**: The project is useful for understanding how to dynamically create and manage service models at runtime, a key feature for scalable applications.
- **Message Routing**: Explore how to handle communication in a networked environment using a router (`mcrouter`), which allows multiple services to connect and interact in real time.

## Conclusion

The **17_winchat** project provides a practical example of building a **Windows-exclusive**, MFC-based distributed communication system using the AREG SDK. With dynamic model creation, automatic service discovery, and robust IPC through **Object RPC**, this project serves as a valuable resource for developers aiming to implement fault-tolerant systems with real-time communication.
