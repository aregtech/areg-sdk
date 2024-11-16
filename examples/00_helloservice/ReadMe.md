
# 00_helloservice Project Overview

The **00_helloservice** project is a foundational example from the AREG SDK, designed to demonstrate how to build an application using **Inter-Process Communication (IPC)** and **multithreading** only by changing the *Application Model*. This project showcases the creation and interaction of service (*micro-server*) and client objects, illustrating various deployment scenarios (various *models*) within the AREG framework.

## Key Concepts

- **Service and Client Objects**: Learn how to implement service providers and consumers using the AREG Framework.
- **Service Interface:** Utilize the Service Interface document and code generator to automate **Object Remote Procedure Call (Object RPC)** message creation and dispatching, enhancing efficiency and simplifying communication between host and remote target.
- **Multithreading**: Understand how services and clients can coexist or run independently across threads.
- **IPC (Inter-Process Communication)**: Explore scenarios where service provider and consumer reusable objects communicate across multiple processes using **Object RPC**.
- **Scalable Architecture**: The example demonstrates different deployment strategies, from single-threaded setups to more complex multiprocess applications only by changing *Application Model*.

## Project Structure

1. **00_generated**: 
   - This library contains files automatically created from the [HelloService.siml](./services/HelloService.siml) Service Interface. The files are generated during CMake setup or as a pre-build step in Visual Studio. The generated code simplifies and automates the process of creating and managing **RPC** based communication between multiple threads and processes.

2. **[common](./common/)**:
   - This directory holds shared implementations of both the **Service Provider** and **Service Consumer** components. These shared components are reused across different sub-projects, allowing developers to build multithreaded or multiprocessing applications by modifying the *Application Model* definition.

3. **[00_onethread](./onethread/)**:
   - This example shows how the service provider and client can run in a single thread within the same process. Both components use the same thread, and their code is found in the *common* directory. The decision to organize them this way is made in the *Application Model* object.

4. **[00_twothreads](./twothreads/)**:
   - In this scenario, the service provider and client run in two separate threads within the same process. The shared implementations are again located in the *common* directory. The decision to organize them this way is made in the *Application Model* object.

5. **[00_pubservice](./multiprocess/serviceproc/)** and **[00_pubclient](./multiprocess/clientproc/)**:
   - These projects set up a *Public Service*, allowing the *Service Consumer* in the `00_pubclient` project to send requests to the *Service Provider* in the `00_pubservice` project. The service provider and consumer components are implemented in the *common* directory. This example shows how to define a multiprocess application using the *Application Model* object.

## Key Features

- **Single-threaded and Multithreaded Examples**: Learn how to structure applications where the service provider and client either share the same thread or operate in separate threads within the same process.
- **Multiprocessing with IPC**: Discover how to set up a service that communicates with multiple clients across different processes.
- **Reusable Components**: The *common* directory demonstrates how to centralize service and client implementations for use across various deployment strategies.
- **Object Remote Procedure Call (Object RPC)**: This project demonstrates how **Object RPC** facilitates communication between different threads and processes, enabling seamless request and response exchanges between components running in the same or multiple processes.
- **Code generation**: A highly effective way to use the Service Interface document and code generator to reduce errors, avoid tedious tasks, and focus more on the core business logic of the application.

## Use Cases

- **Multithreading in IPC Applications**: Ideal for developers looking to implement services that communicate efficiently across threads and processes.
- **Service-Oriented Architectures**: Useful for building applications that follow a service-consumer model, allowing for scalable and modular design.
- **Event-driven Architectures**: Ideal for building applications with asynchronous communication, enabling the design of scalable and autonomous systems.
- **Cross-Platform Development**: The AREG SDK is designed for cross-platform applications, and this example provides a foundation for creating services that work on different operating systems.
