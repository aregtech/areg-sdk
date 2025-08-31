# 01_minimalrpc Project Overview

The **01_minimalrpc** project demonstrates the use of RPC in the multithreading environment. It declares a model with 2 threads and 2 components -- one is a service provider and another is a service consumer, which is calling `reguestHelloService()` method when service is available and triggers a quit application signal, when receives `responseHelloSercice()`.

## Key Concepts

- **Model Declaration**: Learn how to declare a model with multiple threads and components using the AREG Framework.
- **Service Provider and Consumer**: Understand the interaction between service providers and consumers in a multithreading environment.
- **RPC method calls**: A method called on service consumer side is invoked on service provider side.

## Features

- **Modeling**: A simple modeling with 2 threads and 2 servicing components -- Service Provider and Service Consumer.
- **Service Provider**: Implements a service that responds to requests from the consumer.
- **Service Consumer**: Calls the service method and handles the response, demonstrating inter-thread communication.
- **Thread and Component Lifecycle**: The threads and components are automatically created when model is loaded and destroyed when model is unloaded.

## Project Structure

- **Source Code**: The objects and code are in one file for better traceability.
- **Service Interface document**: The service interface XML document with 2 methods: `HelloService` and connected `HelloService`


## Use Cases

- **Multithreading Basics**: Ideal for developers to learn the basics of multithreading development without manual threading.
- **Thread and Component Lifecycle**: Learn when the threads.

## Conclusion

The **01_minimalrpc** project serves as a beginner-friendly introduction to **multithreading** in the AREG framework.
