# 21_pubunblock Project Overview

The **21_pubunblock** project demonstrates how to handle and manually unblock service requests in a distributed system based on AREG Framework. When a request is in progress and marked as *busy* due to the absence of a response, additional client requests for the same operation receive a *failure* response, indicating that the initial request is still being processed. Manual unblocking allows the service provider to continue handling new requests without waiting for the previous ones to complete. Once the service provider is ready, it sends responses to the corresponding consumers.

## Key Concepts

- **Manual Unblocking**: In a scenario where a service provider has pending *busy* requests, manual unblocking ensures that the provider can continue processing new requests. This feature is critical for systems with **asynchronous communication** requiring non-blocking behavior or requests and quick response handling.
- **Service Interface Automation**: Code is generated from the **Service Interface** document to automate **Object Remote Procedure Call (Object RPC)** message creation and dispatching. This facilitates efficient **Inter-Process Communication (IPC)** between the service provider and consumers, allowing seamless message exchanges.
- **Manual Response**: When a request is manually unblocked, the developer must prepare the response before sending it to the correct target. The AREG Framework provides a simple and efficient mechanism, requiring minimal effort from the developer to ensure the response is sent to the appropriate *Service Consumer*. Even if multiple consumers are waiting for responses, the framework guarantees that each response is delivered to the correct target. 

## Project Structure

1. **21_generated**:
   - This is a library of codes generated from the [HelloUnblock.siml](./res/HelloUnblock.siml) Service Interface document during the CMake configuration or as a pre-build action in Visual Studio. The generated code utilizes the Service Interface document and the code generator to automate **Object RPC**, improving communication efficiency between the service provider and remote consumers via **IPC**.

2. **[21_pubservice](./pubservice/)**:
   - This sub-project provides a *Public Service* that processes requests from clients with a delay, which is a normal behavior in asynchronous communication systems such as AREG Framework. In this case, the system accumulates pending requests and automatically marks requests as *busy* until the request are processing. To enable continues request processing, in the codes the requests are manually unblocked, so that the next upcoming request can be immediately started to be processing. When the request is finished to be processed, the response is prepared and sent back to the target consumer.

3. **[21_pubclient](./pubclient/)**:
   - A *Service Client* application that continuously sends frequent requests to the service provider. Since requests are immediately unblocked on *Service Provider* side, the *Service Consumer* never receives a request failure with *busy* flag. And as soon as the response is prepared and sent back, the Consumer receives and reacts on response.

## Communication

- **mcrouter**: All communication between the service provider and clients is facilitated by `mcrouter`, which is a router designed to deliver messages fot appropriate targets. The AREG SDK provides built-in fault tolerance and service discovery features, ensuring the system's reliability even when processes start in different orders.
- **Object RPC**: The **Object Remote Procedure Call (Object RPC)** mechanism simplifies **Inter-Process Communication (IPC)** by automating message handling between the service provider and consumers. This is key for distributed applications needing efficient and reliable communication between multiple processes.

## Key Features

- **Manual Request Unblocking**: The core feature of this project is the ability to manually unblock requests, allowing the service provider to continue processing new requests while waiting to complete ongoing ones. This prevents the system with asynchronous communication from stalling.
- **Non-Blocking Client Responses**: Normally, when a client request is processed and the developer replies with the response, the system automatically prepares the response and sends to the target Consumer. In case of manual unblocking or request, the develop needs manually preparing response then reply with the response. This response preparation mechanism ensures that the right target Consumer receives the response.
- **Fault-Tolerant Communication**: Leveraging `mcrouter` and the AREG SDK’s fault-tolerant capabilities, the system can manage communication failures gracefully, ensuring that service providers and consumers can reconnect and resume operations without data loss.

## Use Cases

- **High Request Frequency Systems**: This project is suitable for systems where clients frequently send requests to a service provider, ensuring that no client request is blocked or delayed unnecessarily.
- **Non-Blocking Applications**: The manual unblocking mechanism is ideal for applications that need to maintain responsiveness even when some requests are taking longer to process, improving overall system performance.
- **Asynchronous communication**: This solution of unblocking the request, give more flexible and better error handling possibility in asynchronous communication systems.
- **Multi-Client Distributed Systems**: The project demonstrates an efficient way to handle multiple requests from various clients in a distributed system, ensuring seamless communication and request processing.

## Conclusion

The **21_pubunblock** project showcases a practical method for handling request accumulation and manual unblocking in distributed systems.