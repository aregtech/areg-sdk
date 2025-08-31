# 12_svcmulti Project Overview

The **12_svcmulti** project builds upon the concepts from the [11_service](./../11_service) project, demonstrating how to create **multiple instances** of the same **local service** with distinct names. This example shows how to configure and manage multiple **service providers** running in parallel, each identified by a unique *role* (name) within the application.

## Key Concepts

- **Multiple Service Instances**: Learn how to create and manage multiple instances of the same service, each with a unique name (or *role*) defined in the *Application Model*.
- **Service Roles**: Each service instance operates under a distinct *name*, allowing the system to differentiate between otherwise identical services.
- **Multithreaded Services**: The project demonstrates how to run multiple service providers in separate threads, ensuring efficient parallel execution.

## Features

- **Multiple Service Providers**: The project creates two identical instances of a **local service**, each running under a different *role*, showcasing how to scale services within the same process.
- **Name-Based Service Identification**: Each service instance is assigned a *unique name* called *role* in the *Application Model*, allowing for clear identification and management of services.
- **Multithreaded Execution**: Each service provider operates in its own thread, demonstrating how to manage multiple service providers in a **multithreaded environment**.

## Project Structure

- **Service Instantiation**: The project shows how to define and instantiate multiple instances of the same service, each with a unique role.
- **Application Model**: The services are defined in the application's model, which controls their roles and behavior during runtime.
- **Multithreading**: The services are executed in separate threads, providing a clear example of how to handle multiple service providers concurrently.

## Use Cases

- **Scaling Local Services**: This project is ideal for applications that require multiple instances of the same service, each handling distinct tasks or operating under different conditions.
- **Name-Based Service Management**: Learn how to assign unique *roles* to services, allowing for more granular control over multiple service instances within the same application.
- **Multithreaded Service Providers**: The project offers practical guidance on running multiple service providers in parallel, which is useful in high-performance or resource-intensive applications.

## Conclusion

The **12_svcmulti** project provides a practical guide for creating and managing **multiple local service providers** in a **multithreaded environment** using the AREG Framework. By assigning distinct *roles* to each service instance, developers can scale services and manage them efficiently within a single application process.
