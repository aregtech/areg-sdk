# 12_svcmulti

🧵 **Type:** Multithreading / Single Process

## Overview
The **12_svcmulti** project extends the concepts from [11_service](./../11_service), demonstrating how to create **multiple instances** of the same **local service**, each with a unique *role* (name). This example highlights how to configure and manage several **service providers** running in parallel within a **multithreaded application**.

## Concepts Shown
- **Multiple Service Instances**: Create and manage multiple instances of the same service, each with a distinct name (*role*) defined in the *Application Model*.
- **Service Roles**: Differentiate otherwise identical services using unique names to allow precise control and identification.
- **Multithreaded Execution**: Run multiple service providers in separate threads to ensure parallel and efficient operation.
- **Service Lifecycle**: Observe how each service instance is loaded, executed, and unloaded safely within the multithreaded environment.
- **Internal Communication**: Services remain confined to the local process, providing isolated and predictable execution.

## How It Works
Each service instance is defined in the *Application Model* with a unique *role*. The project creates two or more instances of the same service, each running in its own thread. The framework handles initialization, execution, and cleanup for each service instance, demonstrating how **multithreaded applications** can scale internal services efficiently. All interactions are confined within the process.

## Use Cases
- Scale **local services** by running multiple instances in parallel.
- Manage **services with unique roles** for fine-grained control.
- Implement **multithreaded service providers** for high-performance, resource-intensive tasks within a single process.

## Takeaway
The **12_svcmulti** example shows how to manage **multiple local service providers** in a **multithreaded environment**, assigning distinct *roles* to scale services safely and efficiently.
