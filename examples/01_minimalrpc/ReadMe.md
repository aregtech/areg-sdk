# 01\_minimalrpc

🧵 **Type:** Multithreading / Single Process

## Overview

The **01\_minimalrpc** example demonstrates the most minimal use of **RPC between threads** in a single process using the AREG Framework.
It defines a simple model with **two threads** and **two components**: a **Service Provider** and a **Service Consumer**.
The consumer waits for `serviceConnected()` to confirm the provider is available before calling `requestHelloService()`. Once invoked, the application terminates — keeping the logic as simple as possible.

This addresses the common pain points of multithreaded communication: knowing when a service is ready, avoiding race conditions, and managing threads safely without boilerplate.

## Concepts Shown

* **Model Declaration**: Defining a minimal model with multiple threads and components in AREG.
* **Service Provider/Consumer Interaction**: Asynchronous communication across threads.
* **One-Way RPC Call**: Consumer executes `requestHelloService()` on the provider without requiring a response.
* **Auto-Discovery**: `serviceConnected()` notifies the consumer when the provider is ready to accept requests.
* **Thread and Component Lifecycle**: Automatic creation and cleanup of threads and components with the model.

## How It Works

The project includes:

* **Service Interface Document**: Defines the `HelloService` RPC method.
* **Source Code**: Implements provider and consumer in separate threads.

At runtime, the framework creates the threads and components. The **consumer receives `serviceConnected()` notifications** when the provider connects or disconnects. Once connected, it calls `requestHelloService()`, and the application shuts down, demonstrating the simplest possible RPC scenario.

## Use Cases

* Learn the **absolute minimal setup** for thread-to-thread RPC.
* Understand **service connection handling** and lifecycle management.
* A stepping stone before exploring **multi-process IPC examples**.

## Takeaway

A **minimal, beginner-friendly example** of one-way **thread-to-thread RPC with auto-discovery** in AREG, showing safe asynchronous communication and managed component lifecycles.
