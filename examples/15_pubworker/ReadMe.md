# 15_pubworker Project Overview

The **15_pubworker** project demonstrates the use of **Worker Threads** to perform auxiliary work, and use custom event for the communication between **Worker** and **Component Threads** within a same process. By leveraging **Object Remote Procedure Call (Object RPC)** and **Inter-Process Communication (IPC)**, this project showcases how worker threads can be used to offload tasks and manage service attributes efficiently.

> [!IMPORTANT]
> To test this example, ensure an `mcrouter` process is running on a network-accessible machine to enable message routing. Verify that the `areg.init` configuration file includes the correct IP address and port number for the `mcrouter`.

## Key Concepts

- **Worker Threads**: Learn how to utilize worker threads to handle background tasks, such as processing user input or simulating device output, ensuring that the component thread remains responsive.
- **Service Interface:** Utilize the Service Interface document and code generator to automate **Object Remote Procedure Call (Object RPC)** message creation and dispatching, enhancing efficiency and simplifying communication between host and remote target.
- **Custom Event Handling**: Demonstrates how custom events can be processed within both **Worker** and **Component Threads**, providing flexibility in task management.

## Project Structure

1. **15_generated**:
   - This library contains code generated from the [PatientInformation.siml](./services/PatientInformation.siml) Service Interface document during CMake configuration or as part of a pre-build action in Visual Studio. The generated code simplifies the implementation of **Object RPC**, automating message creation and dispatch for inter-process communication.

2. **[15_pubclient](./pubclient/)**:
   - This sub-project includes a **Service Consumer** and a **Worker Thread**. The Worker Thread simulates an output device, performing tasks in the background while the service consumer interacts with the public service provider.

3. **[15_pubservice](./pubservice/)**:
   - This sub-project hosts a **Public Service Provider** along with a **Worker Thread** that processes user inputs from the console and updates the service’s attributes. Both the service and worker thread are part of the static model, which is initialized at startup and unloaded at exit.

## Communication

Communication between the service provider, consumer, and worker threads is managed via `mcrouter`, which is a router that can operate across any networked machine. The AREG Framework automates **service discovery** and provides **fault tolerance**, ensuring reliable **IPC** and **RPC** communication regardless of process startup order. The system dynamically discovers services, and messages are routed to the appropriate threads and processes.

## Key Features

- **Worker and Component Thread Integration**: The project demonstrates how **worker threads** and **component threads** can work together, enabling background processing and efficient task offloading.
- **Object RPC and IPC**: Leverages **Object RPC** and **IPC** to automate message handling between service providers and consumers, allowing smooth interaction between distributed services and threads.
- **Custom Event Processing**: Highlights the use of custom events within worker threads, making it easy to communicate and deliver data.

## Use Cases

- **Background Task Offloading**: Ideal for systems where certain tasks (e.g., device simulation or data processing) can be offloaded to worker threads, allowing the main thread to remain responsive.
- **Device Application**: In some devices, the system may need to wait for an unknown period, such as waiting for user input, while still remaining operational. During this wait time, the device must continue to communicate with other applications. In this scenario, a worker thread can handle the waiting for user input, while another thread manages communication with other processes, ensuring smooth operation.
- **Distributed Service Management**: Suitable for applications that require seamless communication between distributed services and multiple threads using **Object RPC** and **IPC**.
- **Event-Driven Systems**: The project serves as a great example of event-driven architecture, where user inputs trigger custom events that are processed in the background by worker threads.

## Conclusion

The **15_pubworker** project demonstrates the power of **Worker Threads** in managing background tasks and processing custom events in a **multithreaded, service distributed system**. With automated **Object RPC** and **IPC**, the project provides an efficient way to handle communication between service providers, consumers, and threads, making it an excellent resource for building scalable, responsive applications using the AREG SDK.
