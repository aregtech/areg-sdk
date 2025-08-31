# 03_helloservice

🧵🚀 **Type:** Multithreading + IPC

## Overview
The **03_helloservice** example demonstrates building applications using **Inter-Process Communication (IPC)** and **multithreading** in the AREG Framework.  
It shows how to implement service (*micro-server*) and client objects and deploy them in single-threaded, multithreaded, or multiprocess setups by changing the *Application Model*.  

> [!NOTE]
> To run multiprocess examples, a **Multi-Target Router** must be started to enable IPC communication between processes.

## Concepts Shown
- **Service and Client Objects**: Implement service providers and consumers using AREG.  
- **Service Interface & Code Generation**: Automate **Object RPC** message creation and dispatching for reliable communication.  
- **Multithreading**: Run services and clients in the same or separate threads.  
- **Inter-Process Communication (IPC)**: Communicate between processes using Object RPC via a Multi-Target Router.  
- **Scalable Deployment**: Switch between single-threaded, multithreaded, or multiprocess deployment by modifying the Application Model.

## How It Works
- **03_generated**  
  Contains files generated from the [HelloService.siml](./services/HelloService.siml) Service Interface. These files automate RPC message creation and simplify communication between threads and processes.  

- **[common](./common/)**  
  Holds shared implementations of both the service provider and consumer. These reusable components can be instantiated in any subproject by adjusting the Application Model.  

- **[03_onethread](./onethread/)**  
  Demonstrates running the service provider and consumer in a **single thread** within one process. Both components share the same thread.  

- **[03_twothreads](./twothreads/)**  
  Demonstrates running the service provider and consumer in **separate threads** within one process. Components remain in the common directory, but the Application Model assigns different threads.  

- **[03_pubservice](./multiprocess/serviceproc/)** and **[03_pubclient](./multiprocess/clientproc/)**  
  Demonstrate a **Public Service** setup across separate processes. The consumer in `03_pubclient` discovers and communicates with the provider in `03_pubservice`. These examples require the **Multi-Target Router** to run properly.

## Use Cases
- Learn to implement services that communicate across **threads and processes**.  
- Build **service-consumer architectures** that scale from single-threaded apps to multiprocess systems.  
- Explore **event-driven applications** with asynchronous communication patterns.  
- Start building **cross-platform IPC applications** with reusable components.

## Takeaway
A foundational example illustrating **multithreaded and multiprocess service deployment** in AREG, highlighting **Object RPC**, reusable components, and IPC via a **Multi-Target Router**.
