# Hello Service!

```
This file is part of AREG SDK
Copyright (c) 2021-2023, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document provides a practical example of creating applications with distributed services using the AREG SDK.

> [!NOTE]
> You can find the source code for this example in the [00_helloservice](../examples/00_helloservice/) directory.

## Table of Content

- [Introduction](#introduction)
- [Demonstrated Features](#demonstrated-features)
- [Key Notes for Service Implementation](#key-notes-for-service-implementation)
- [Showcase](#showcase)
- [Directory Structure](#directory-structure)
- [Service Interface](#service-interface)
- [Code Generation](#code-generation)
- [Model](#model)
    - [Declaring Static Models](#declaring-static-models)
- [Project Examples](#project-examples)
    - [`onethread`](#onethread)
    - [`twothreads`](#twothreads)
    - [`multiprocess`](#multiprocess)
- [Testing `multiprocess`](#testing-multiprocess)

## Introduction

This tutorial offers a step-by-step guide to building service-enabled applications. To get started, it is recommended to create a new directory within the AREG SDK's [examples](../examples) directory and name it `helloservice`. This directory will serve as the root for your projects and the working space for three sample projects included in this tutorial.

Each project demonstrates a different approach to running the service provider (server) and service consumer (client), both of which are located in the `common/src` subdirectory:

- **Project #1** ([onethread](../examples/00_helloservice/onethread/)): Service provider and consumer operate in the same thread within a single process.
- **Project #2** ([twothreads](../examples/00_helloservice/twothreads/)): Service provider and consumer run in separate threads within a single process.
- **Project #3** ([multiprocess](../examples/00_helloservice/multiprocess/)): Service provider and consumer run in separate processes.

By following this tutorial and completing the sample projects, you will gain hands-on experience with creating multithreading and multiprocessing applications, and learn how to implement distributed services in an IoT edge network (mist network).

---

## Demonstrated Features

The following features are illustrated in these examples:

1. Each project defines a model with a service provider (_server_) and a service consumer (_client_) that can run in the same thread, separate threads, or different processes.
2. Applications dynamically load and unload the model to manage the service lifecycle.
3. The server registers the service, and the client automatically discovers it.
4. The client sends a request to the server once the service becomes available.
5. The server processes the request and sends a response back to the client.
6. Upon receiving the response:
   - In **Project #1** and **Project #2**, where both run in a single process, the application exits.
   - In **Project #3**, the client exits while the server continues running.
7. **Project #3** allows multiple clients to connect concurrently, and service discovery ensures the order of client startup is irrelevant.

### Key Notes for Service Implementation

- A **Component** manages service provider and consumer objects.
- A single **Component** can provide and consume multiple services.
- The same service provider can be instantiated multiple times but must have unique **Role Names** when running in separate threads or processes.
- Service consumers identify providers by their **Role Names**, which must be unique:
  - Within a process for _Local_ services.
  - Across a network for _Public_ services.
- Service connection status is tracked via callbacks:
  - **Consumers**: `void serviceConnected(NEService::eNetConnection status, ProxyBase& proxy)`
  - **Providers**: `bool clientConnected(const ProxyAddress& client, NEService::eServiceConnection connectionStatus)`

---

## Showcase

This tutorial demonstrates how to design flexible service providers and consumers that can be split across threads or processes to optimize computing power. All projects share the same service provider and consumer implementations, with variations only in their **models**, declared in the respective `main.cpp` files.

### Directory Structure
- **Generated Code**: Located in the `generated/src` subdirectory, created by the **code generator**.
- **Common Code**: The service provider (_server_) and consumer (_client_) implementations are in the `common/src` subdirectory, shared across all projects.
- **Service Interface**: Defined in the `res` subdirectory and used to generate code.

Before starting, create the following subdirectories under `helloservice`: `common/src` and `service`

---

## Service Interface

To create a multithreading and multiprocessing project, follow these steps to define a `HelloService` interface:

1. In the `helloservice/service` directory, create a file named `HelloService.siml`.
2. Add the following XML content:

```xml
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<ServiceInterface FormatVersion="1.0.0">
    <Overview ID="1" Name="HelloService" Version="1.0.0" isRemote="true">
        <Description>The hello world application</Description>
    </Overview>
    <MethodList>
        <Method ID="2" MethodType="request" Name="HelloService" Response="HelloService">
            <Description>Request to output a greeting.</Description>
            <ParamList>
                <Parameter DataType="String" ID="3" Name="client">
                    <Description>Name of the client requesting the greeting.</Description>
                </Parameter>
            </ParamList>
        </Method>
        <Method ID="4" MethodType="response" Name="HelloService">
            <Description>Response indicating success status.</Description>
            <ParamList>
                <Parameter DataType="bool" ID="5" Name="success">
                    <Description>Indicates whether the greeting was successfully output.</Description>
                </Parameter>
            </ParamList>
        </Method>
    </MethodList>
</ServiceInterface>
```

This interface declares the **HelloService** as _Public_ (`isRemote="true"`) with a request and its corresponding response.

---

## Code Generation

> [!NOTE]
> Ensure [Java](https://java.com/) is installed to run the code generator. The minimum required version is 17

Run the following command in the `helloservice` directory to generate service-related files:

```bash
java -jar <areg-sdk-root>/tools/codegen.jar --root=<areg-sdk-root>/product/generate --doc=<helloservice-project-root>/service/HelloService.siml --target=helloservice/services
```

The generated code includes:
- **Service Provider Base Classes** (e.g., `HelloServiceStub`)
- **Service Consumer Base Classes** (e.g., `HelloServiceClientBase`)

Follow the remaining sections to implement the **Service Provider** and **Service Consumer**, with code snippets and explanations provided. Each example builds on these core implementations.

--- 

### Model

The AREG Engine utilizes a service-oriented architecture to manage the relationships between threads, service providers, and service consumers. These relationships are encapsulated within a **model**. There are two types of models: **static models**, predefined during application development, and **dynamic models**, created and managed at runtime. An application can include multiple models, which can be loaded dynamically to automatically instantiate threads, components, and services.

Key concepts of the model:

- **Components and Threads**:
  Each thread must host at least one component, and each component must include at least one service provider or service consumer. Components can extend or contain multiple service provider/consumer objects, with the model specifying the services provided and consumed.
- **Model Lifecycle**:
  When a model is no longer required, it can be unloaded, stopping all threads and shutting down all services registered within that model.

#### Declaring Static Models

To declare static models in your application, follow these guidelines:

1. **Unique Model Names**:
   Each application must include at least one model with a unique name.
2. **Model Declaration**:
   Use the **BEGIN_MODEL** and **END_MODEL** macros to define the model, specifying its name.
3. **Threads**:
   Each model must define at least one thread with a unique name. Use the **BEGIN_REGISTER_THREAD** and **END_REGISTER_THREAD** macros to declare threads, and optionally specify a _watchdog timeout_.
4. **Components**:
   Each thread must register at least one component with a unique name (_Role Name_).
   - If a component provides or consumes only **Local** services, its name must be unique within the application.
   - If a component provides or consumes at least one **Public** service, its name must be unique across the network.
   - Use the **BEGIN_REGISTER_COMPONENT** and **END_REGISTER_COMPONENT** macros to register components, specifying the component object associated with its _Role Name_.
5. **Service Providers**:
   - Use the **REGISTER_IMPLEMENT_SERVICE** macro to declare services provided by a component.
   - A component can provide multiple services, but each must have a unique interface within the component and thread.
6. **Service Consumers**:
   - Use the **REGISTER_DEPENDENCY** macro to define a component's dependency on a service provided by another component (identified by its _Role Name_).
   - A component can consume multiple services, and a service can be consumed by multiple components.

> [!NOTE]
> Service providers are identified by their _Service Interface Name_, while service consumers refer to the provider using the _Role Name_ of the component. This ensures that consumers can distinguish between multiple implementations of the same service interface.

Example source code for implementing models can be found in the [`00_helloservice`](../examples/00_helloservice/) directory. All examples utilize common [`ServiceComponent`](../examples/00_helloservice/common/src/ServiceComponent.hpp) and [`ClientComponent`](../examples/00_helloservice/common/src/ClientComponent.hpp) objects.

> [!IMPORTANT]
> All example projects must be linked with the **areg library** (_shared_ or _static_).

---

### Project Examples

#### [`onethread`](../examples/00_helloservice/onethread/)

This example demonstrates a model where both the service provider and consumer operate within the same thread of a single process. The service starts automatically when the model is loaded.
**Steps to create and run**:

1. Create a `onethread` project.
2. Add a `main.cpp` file.
3. Copy the following code into `main.cpp` to define the model, compile, and run:

```cpp
#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

// Model name
constexpr char const _model[]{ "ServiceModel" };

BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD("Thread1", NECommon::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceComponent", ServiceComponent)
            REGISTER_IMPLEMENT_SERVICE(NEHelloService::ServiceName, NEHelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceComponent")
        BEGIN_REGISTER_COMPONENT("ServiceClient", ClientComponent)
            REGISTER_DEPENDENCY("ServiceComponent")
        END_REGISTER_COMPONENT("ServiceClient")
    END_REGISTER_THREAD("Thread1")
END_MODEL(_model)

int main()
{
    Application::initApplication();
    Application::loadModel(_model);
    Application::waitAppQuit();
    Application::unloadModel(_model);
    Application::releaseApplication();
    return 0;
}
```

---

#### [`twothreads`](../examples/00_helloservice/twothreads/)

This example separates the service provider and consumer into different threads within the same process.
**Steps to create and run**:

1. Create a `twothreads` project.
2. Add a `main.cpp` file.
3. Copy the following code into `main.cpp`, compile, and run:

```cpp
#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

// Model name
constexpr char const _model[]{ "ServiceModel" };

BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD("Thread1", NECommon::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceComponent", ServiceComponent)
            REGISTER_IMPLEMENT_SERVICE(NEHelloService::ServiceName, NEHelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceComponent")
    END_REGISTER_THREAD("Thread1")
    BEGIN_REGISTER_THREAD("Thread2", NECommon::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceClient", ClientComponent)
            REGISTER_DEPENDENCY("ServiceComponent")
        END_REGISTER_COMPONENT("ServiceClient")
    END_REGISTER_THREAD("Thread2")
END_MODEL(_model)

int main()
{
    Application::initApplication();
    Application::loadModel(_model);
    Application::waitAppQuit();
    Application::unloadModel(_model);
    Application::releaseApplication();
    return 0;
}
```

---

#### [`multiprocess`](../examples/00_helloservice/multiprocess/)

This example distributes the service provider and consumer across separate processes. Two projects are required:

1. **`serviceproc` (Service Provider)**
   Registers the service provider. See [source code](../examples/00_helloservice/multiprocess/serviceproc/src/main.cpp).

2. **`clientproc` (Service Consumer)**
   Registers the service consumer. See [source code](../examples/00_helloservice/multiprocess/clientproc/src/main.cpp).

---

### Testing `multiprocess`

1. Start **mcrouter** locally or configure its IP in the [_areg.init_](../framework/areg/resources/areg.init) file.
2. Run `serviceproc` and `clientproc`.
   - Start **serviceproc** (only one instance allowed).
   - Launch multiple **clientproc** instances if needed (unique names are auto-generated).
3. Services will communicate across platforms or networks seamlessly.

By organizing applications using this model, you can build scalable, efficient, and cross-platform systems with ease.
