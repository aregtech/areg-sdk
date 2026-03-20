# Hello Service Tutorial

```
This file is part of Areg SDK
Copyright (c) 2021-2026, Aregtech
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

This tutorial demonstrates how to build multithreaded and multiprocess applications with Areg SDK using the same service and client components.

> [!NOTE]
> Complete source code is available in [examples/03_helloservice](../examples/03_helloservice/).

---

## Table of Contents

  - [Introduction](#introduction)
  - [What You Will Learn](#what-you-will-learn)
  - [Key Concepts](#key-concepts)
  - [Project Structure](#project-structure)
  - [Service Interface Definition](#service-interface-definition)
  - [Code Generation](#code-generation)
  - [Understanding Models](#understanding-models)
  - [Example Projects](#example-projects)
    - [onethread](#onethread)
    - [twothreads](#twothreads)
    - [multiprocess](#multiprocess)
  - [Running the Multiprocess Example](#running-the-multiprocess-example)

---

## Introduction

This example contains three projects under `examples/03_helloservice`. All three reuse the same **ServiceComponent** and **ClientComponent** implementations but demonstrate different execution models:

| Project                                                   | Description                                  |
| --------------------------------------------------------- | -------------------------------------------- |
| [onethread](../examples/03_helloservice/onethread/)       | Service and client run in one thread         |
| [twothreads](../examples/03_helloservice/twothreads/)     | Service and client run in separate threads   |
| [multiprocess](../examples/03_helloservice/multiprocess/) | Service and client run in separate processes |

---

## What You Will Learn

1. Define a model with service provider and consumer components
2. Load and unload models dynamically to control service lifecycle
3. Use automatic service discovery between components
4. Send requests from clients and receive responses from services
5. Build applications that scale from single-threaded to distributed

---

## Key Concepts

### Components and Services

- A **Component** can provide and/or consume multiple services
- Multiple providers of the same service can coexist, but each must have a unique **Role Name**

### Role Name Rules

| Service Type | Role Name Scope           |
| ------------ | ------------------------- |
| Local        | Unique within the process |
| Public       | Unique across the network |

### Connection Callbacks

Components receive notifications when connections change:

**Client Component:**
```cpp
void service_connected(areg::NetConnection status, ProxyBase& proxy)
```

**Service Component:**
```cpp
bool clientConnected(const ProxyAddress& client, areg::ServiceConnectionState connectionStatus)
```

---

## Project Structure

```
03_helloservice/
├── services/           Service interface definition (.siml file)
├── generated/src/      Generated code from service interface
├── common/src/         Shared service and client implementations
├── onethread/          Single-thread example
├── twothreads/         Multi-thread example
└── multiprocess/       Multi-process example
    ├── serviceproc/    Service process
    └── clientproc/     Client process
```

---

## Service Interface Definition

Create a service interface file that defines the communication contract. The file `services/HelloService.siml` defines a simple greeting service:

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="HelloService" Version="1.0.0" Category="Public">
        <Description>Hello world service example</Description>
    </Overview>
    <MethodList>
        <Method ID="2" Name="HelloService" MethodType="Request" Response="HelloService">
            <Description>Request to output a greeting.</Description>
            <ParamList>
                <Parameter ID="3" Name="client" DataType="String">
                    <Description>Name of the client requesting the greeting.</Description>
                </Parameter>
            </ParamList>
        </Method>
        <Method ID="4" Name="HelloService" MethodType="Response">
            <Description>Response indicating whether the greeting was successful.</Description>
            <ParamList>
                <Parameter ID="5" Name="success" DataType="bool">
                    <Description>True if the greeting was output successfully.</Description>
                </Parameter>
            </ParamList>
        </Method>
    </MethodList>
</ServiceInterface>
```

**Key attributes:**
- `Category="Public"` makes the service accessible across processes (use `Category="Local"` for thread-only services)
- `MethodType="Request"` with `Response="HelloService"` links the request to its response
- `MethodType="Response"` defines what the service returns

---

## Code Generation

> [!NOTE]
> Requires [Java 17+](https://java.com/).

### Using CMake

In your `CMakeLists.txt`, use the `addServiceInterface` macro:

```cmake
addServiceInterface(03_generated examples/03_helloservice/services/HelloService.siml)
```

This creates a static library named `03_generated` containing the generated code.

### Using Command Line

```bash
java -jar <areg-sdk>/tools/codegen.jar \
    --root=<project-root> \
    --doc=services/HelloService.siml \
    --target=generated/src
```

### Generated Classes

| Class                    | Purpose                                        |
| ------------------------ | ---------------------------------------------- |
| `HelloServiceProviderBase`       | Base class for service provider implementation |
| `HelloServiceConsumerBase` | Base class for service consumer implementation |
| `HelloService`         | Namespace with service constants and types     |

---

## Understanding Models

A **model** defines how threads, components, and services are organized. Models can be:

- **Static**: Declared at compile time using macros
- **Dynamic**: Created programmatically at runtime

### Model Lifecycle

1. `Application::load_model()` starts threads and initializes components
2. Services become available and clients connect automatically
3. `Application::unloadModel()` stops threads and releases resources

### Static Model Macros

| Macro                                                 | Purpose                                  |
| ----------------------------------------------------- | ---------------------------------------- |
| `BEGIN_MODEL` / `END_MODEL`                           | Define a model                           |
| `BEGIN_REGISTER_THREAD` / `END_REGISTER_THREAD`       | Define a thread                          |
| `BEGIN_REGISTER_COMPONENT` / `END_REGISTER_COMPONENT` | Define a component                       |
| `REGISTER_IMPLEMENT_SERVICE`                          | Declare a service the component provides |
| `REGISTER_DEPENDENCY`                                 | Declare a service the component consumes |

---

## Example Projects

### onethread

Both service and client run in the same thread. This is the simplest configuration.

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

constexpr char const _model[]{ "ServiceModel" };

BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD("Thread1", areg::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceComponent", ServiceComponent)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceComponent")
        BEGIN_REGISTER_COMPONENT("ServiceClient", ClientComponent)
            REGISTER_DEPENDENCY("ServiceComponent")
        END_REGISTER_COMPONENT("ServiceClient")
    END_REGISTER_THREAD("Thread1")
END_MODEL(_model)

int main()
{
    Application::setup();
    Application::load_model(_model);
    Application::wait_quit();
    Application::unloadModel(_model);
    Application::release();
    return 0;
}
```

---

### twothreads

Service and client run in separate threads within the same process.

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

constexpr char const _model[]{ "ServiceModel" };

BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD("Thread1", areg::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceComponent", ServiceComponent)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceComponent")
    END_REGISTER_THREAD("Thread1")

    BEGIN_REGISTER_THREAD("Thread2", areg::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceClient", ClientComponent)
            REGISTER_DEPENDENCY("ServiceComponent")
        END_REGISTER_COMPONENT("ServiceClient")
    END_REGISTER_THREAD("Thread2")
END_MODEL(_model)

int main()
{
    Application::setup();
    Application::load_model(_model);
    Application::wait_quit();
    Application::unloadModel(_model);
    Application::release();
    return 0;
}
```

---

### multiprocess

Service and client run in separate processes, communicating through `mtrouter`.

**Service Process (`serviceproc`):**

```cpp
BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD("Thread1", areg::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceComponent", ServiceComponent)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceComponent")
    END_REGISTER_THREAD("Thread1")
END_MODEL(_model)
```

**Client Process (`clientproc`):**

```cpp
BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD("Thread1", areg::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT("ServiceClient", ClientComponent)
            REGISTER_DEPENDENCY("ServiceComponent")
        END_REGISTER_COMPONENT("ServiceClient")
    END_REGISTER_THREAD("Thread1")
END_MODEL(_model)
```

The same `ServiceComponent` and `ClientComponent` classes work without modification.

---

## Running the Multiprocess Example

1. **Start mtrouter**

   The message router must be running for inter-process communication:
   ```bash
   ./mtrouter
   ```

2. **Start the service process**

   Only one service instance should run:
   ```bash
   ./serviceproc
   ```

3. **Start client processes**

   Multiple clients can connect simultaneously:
   ```bash
   ./clientproc
   ./clientproc  # Additional clients get unique names automatically
   ```

4. **Observe the communication**

   Clients discover the service, send greeting requests, and receive responses. The service processes requests from all connected clients.

> [!TIP]
> Configure the router address in [areg.init](../framework/areg/resources/areg.init) if running on different machines.

---

With this foundation, you can build scalable distributed applications where the same component code works across threads, processes, and network devices.
