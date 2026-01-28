# 24_pubunblock

🚀 **Type:** IPC / Multi-Process

## Overview

The **24_pubunblock** project demonstrates handling and manually unblocking service requests in a distributed system using the Areg Framework. When a request is marked as *busy* due to being in progress, subsequent client requests for the same operation receive a failure response. Manual unblocking allows the service provider to continue processing new requests without waiting for previous ones to complete, while responses are sent to the appropriate consumers once ready.

> [!NOTE]
> This example requires **Multi-Target Router (`mtrouter`)** for message routing. Ensure `mtrouter` is running on a network-accessible machine and that the `areg.init` file contains the correct IP address and port.

## Concepts Shown

- **Manual Request Unblocking**: Enables the service provider to continue processing new requests while previous requests are still being handled.  
- **Service Interface Automation**: Uses generated code from the Service Interface document to automate **Object RPC** creation and dispatch, simplifying **IPC**.  
- **Manual Response Handling**: Developers prepare and send responses manually to ensure each **Service Consumer** receives the correct data.  
- **Fault-Tolerant Communication**: Combines Areg SDK with `mtrouter` to ensure robust message delivery and service continuity.

## How It Works

The **Public Service Provider** processes incoming client requests and may mark them as *busy* if a previous request is still active. To avoid stalling, the provider can manually unblock pending requests, allowing new requests to be processed immediately. Once a request is completed, the response is prepared and sent to the correct consumer.

**Service Consumers** continuously send frequent requests. With manual unblocking in place, clients rarely encounter *busy* failures, and responses are received as soon as they are ready. The system leverages `mtrouter` and the Areg Framework to ensure automatic service discovery via `serviceConnected()` and reliable inter-process communication.

## Sub-Projects

1. **24_generated**:  
   - Generated code from [HelloUnblock.siml](./services/HelloUnblock.siml) automates **Object RPC** creation and dispatching, providing efficient IPC between provider and consumers.

2. **[24_pubservice](./pubservice/)**:  
   - Public Service that processes requests with potential delays. Uses manual unblocking to continue handling incoming requests and sends responses to the appropriate consumers.

3. **[24_pubclient](./pubclient/)**:  
   - Service Consumer that continuously sends requests. Receives responses promptly thanks to the manual unblocking mechanism on the provider side.

## Use Cases

- **High-Frequency Request Systems**: Maintain responsiveness when many clients send simultaneous requests.  
- **Non-Blocking Applications**: Prevents system stalling when some requests take longer to process.  
- **Asynchronous Communication**: Enhances flexibility and error handling in distributed systems.  
- **Multi-Client Distributed Systems**: Efficiently handles multiple consumers interacting with a single service provider.

## Takeaway

Illustrates practical manual unblocking of service requests to maintain responsiveness and reliability in distributed Areg SDK applications.
