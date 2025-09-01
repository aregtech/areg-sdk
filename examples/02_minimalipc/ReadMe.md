# 02_minimalipc

🚀 **Type:** IPC / Multi-Process

## Overview
The **02_minimalipc** example demonstrates how to use **RPC between separate processes** in AREG.  
It sets up a **Service Provider** and a **Service Consumer** in different executables, showing automatic service discovery and safe cross-process communication via `serviceConnected()`.

This addresses common pain points in multi-process development: ensuring the consumer only calls a service when it is ready, handling provider disconnections, and avoiding low-level IPC code.

> [!NOTE]
> To run this example, a **Multi-Target Router** must be started to enable cross-process communication.

## Concepts Shown
- **Service Interface Generation**: Automatic RPC code from `.siml` files.  
- **Auto-Discovery via `serviceConnected()`**: Consumer is notified when the provider connects or disconnects. Requests can only be triggered when connected; subscriptions and responses are also managed accordingly.  
- **Cross-Process RPC**: Consumer requests are executed in the provider process.  
- **Request/Response Lifecycle**: Calls to `requestHelloService()` trigger `responseHelloService()` in the provider.  
- **Independent Component Lifecycle**: Provider and consumer run in separate executables with automatic creation and cleanup.

## How It Works
The project contains three subprojects:  

1. **02_generated** – Auto-generated RPC code from [`HelloService.siml`](./services/HelloService.siml). Provides glue for cross-process messaging.  

2. **02_provideripc** – Executable running the **Service Provider**. Listens for requests and responds with `responseHelloService()`.  

3. **02_consumeripc** – Executable running the **Service Consumer**.  
   - Waits for `serviceConnected()` notifications from the provider.  
   - Once connected, it can safely call `requestHelloService()` and subscribe to updates.  
   - Handles disconnecti
