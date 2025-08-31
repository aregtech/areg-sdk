# 01_minimalrpc

🧵 **Type:** Multithreading / Single Process

## Overview
The **01_minimalrpc** example demonstrates how to use **RPC between threads** in a single process using the AREG Framework.  
It defines a simple model with **two threads** and **two components**: a **Service Provider** and a **Service Consumer**.  
The consumer waits for `serviceConnected()` to indicate the provider is available before calling `requestHelloService()`. Upon receiving the response, it triggers application quit.

This addresses the common pain points of multithreaded communication: knowing when a service is ready, avoiding race conditions, and managing threads safely without boilerplate.

## Concepts Shown
- **Model Declaration**: How to define a model with multiple threads and components in AREG.  
- **Service Provider/Consumer Interaction**: Asynchronous communication between threads.  
- **RPC Method Calls**: Consumer requests are executed on the provider with zero-boilerplate messaging.  
- **Auto-Discovery**: `serviceConnected()` notifies the consumer when the provider is ready to accept requests and manage subscriptions.  
- **Thread and Component Lifecycle**: Automatic creation and destruction of threads and components with the model.

## How It Works
The project includes:  
- **Service Interface Document**: Defines `HelloService` RPC methods.  
- **Source Code**: Implements provider and consumer in separate threads.  

At runtime, the framework creates the threads and components. The **consumer receives `serviceConnected()` notifications** when the provider connects or disconnects. Once connected, it calls `requestHelloService()`, the provider responds via `responseHelloService()`, and the consumer triggers application shutdown, demonstrating a complete request/response lifecycle.

## Use Cases
- Learn **multithreaded RPC** without manual thread management.  
- Understand **service connection handling** and lifecycle management.  
- Serve as a foundation before moving to **multi-process IPC examples**.

## Takeaway
A beginner-friendly demonstration of **thread-to-thread RPC with auto-discovery** in AREG, showing safe asynchronous communication and managed component lifecycles.
