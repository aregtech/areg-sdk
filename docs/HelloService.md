
# Hello Service!

```

This file is part of AREG SDK
Copyright (c) 2021–2023, Aregtech
Contact: info\[at]aregtech.com
Website: [https://www.aregtech.com](https://www.aregtech.com)

````

This tutorial walks through building distributed applications with the AREG SDK.

> [!NOTE]
> Full source code is in [03_helloservice](../examples/03_helloservice/).

---

## Table of Contents

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

---

## Introduction

This example consists of three projects under `examples/03_helloservice`. Each project reuses the same **ServiceComponent** and **ClientComponent** (in `common/src`) but demonstrates different execution models:

- **[onethread](../examples/03_helloservice/onethread/):** Both components run in the same thread.  
- **[twothreads](../examples/03_helloservice/twothreads/):** Components run in separate threads of one process.  
- **[multiprocess](../examples/03_helloservice/multiprocess/):** Components run in separate processes.

By completing the steps, you’ll learn how to create multithreaded and multiprocess applications and understand how AREG SDK enables distributed services in edge (mist) networks.

---

## Demonstrated Features

1. A model defines two components: one providing a service, one consuming it.  
2. Models can be loaded/unloaded dynamically, managing the service lifecycle.  
3. Components discover each other automatically.  
4. Once discovered, the client component sends a request.  
5. The service component processes and responds.  
6. On response:  
   - In **onethread** and **twothreads**, the app exits.  
   - In **multiprocess**, the client exits but the service keeps running.  
7. **multiprocess** supports multiple clients, regardless of start order.

---

## Key Notes for Service Implementation

- A **Component** can provide and/or consume multiple services.  
- Multiple service providers of the same type can exist, but each must have a unique **Role Name**.  
- Role Names must be unique:  
  - Within a process for _Local_ services.  
  - Across the network for _Public_ services.  
- Service connection status is tracked via callbacks:  
  - **Client Component:** `void serviceConnected(NEService::eNetConnection status, ProxyBase& proxy)`  
  - **Service Component:** `bool clientConnected(const ProxyAddress& client, NEService::eServiceConnection connectionStatus)`

---

## Showcase

These examples show how components can be flexibly arranged across threads or processes. The logic is identical; only the **models** differ in `main.cpp`.

---

## Directory Structure

- **`generated/src`** → Code generated from the service interface.  
- **`common/src`** → Shared component implementations (service + client).  
- **`service`** → Service interface definition (`.siml` file).  

Before starting, create `common/src` and `service` under `helloservice`.

---

## Service Interface

Create `helloservice/service/HelloService.siml`:

```xml
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<ServiceInterface FormatVersion="1.0.0">
    <Overview ID="1" Name="HelloService" Version="1.0.0" isRemote="true">
        <Description>Hello world application</Description>
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
                    <Description>Was the greeting successfully output?</Description>
                </Parameter>
            </ParamList>
        </Method>
    </MethodList>
</ServiceInterface>
````

This declares **HelloService** as *Public* (`isRemote="true"`) with a request/response pair.

---

## Code Generation

> \[!NOTE]
> Requires [Java 17+](https://java.com/).

Run in the `helloservice` directory:

```bash
java -jar <areg-sdk-root>/tools/codegen.jar \
  --root=<areg-sdk-root>/product/generate \
  --doc=<helloservice-project-root>/service/HelloService.siml \
  --target=helloservice/services
```

This generates base classes such as `HelloServiceStub` (service) and `HelloServiceClientBase` (client).

---

## Model

A **model** describes threads, components, and their relationships. Models can be static (declared at compile time) or dynamic (created at runtime).

### Concepts

* **Threads & Components:**
  Each thread hosts at least one component. Each component must provide or consume at least one service.
* **Lifecycle:**
  Unloading a model stops all its threads and services.
* **Role Names:**
  Components are identified by Role Names, unique per process (local) or across the network (public).

### Declaring Static Models

1. **Unique model name** — each app needs at least one.
2. **Declare with macros:**

   * `BEGIN_MODEL` / `END_MODEL`
   * `BEGIN_REGISTER_THREAD` / `END_REGISTER_THREAD`
   * `BEGIN_REGISTER_COMPONENT` / `END_REGISTER_COMPONENT`
3. **Register services:**

   * `REGISTER_IMPLEMENT_SERVICE` (provided by component).
   * `REGISTER_DEPENDENCY` (consumed by component).

> \[!NOTE]
> Service providers are identified by **Interface Name**; clients refer by **Role Name**.

See [`03_helloservice`](../examples/03_helloservice/) for examples. All reuse [`ServiceComponent`](../examples/03_helloservice/common/src/ServiceComponent.hpp) and [`ClientComponent`](../examples/03_helloservice/common/src/ClientComponent.hpp).

> \[!IMPORTANT]
> Link all projects with the **areg library** (shared or static).

---

## Project Examples

### [`onethread`](../examples/03_helloservice/onethread/)

Both components run in one thread.

**main.cpp:**

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

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

### [`twothreads`](../examples/03_helloservice/twothreads/)

Components run in separate threads.

**main.cpp:**

```cpp
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

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

### [`multiprocess`](../examples/03_helloservice/multiprocess/)

Components run in different processes:

* **`serviceproc`** → Registers the service. ([source](../examples/03_helloservice/multiprocess/serviceproc/src/main.cpp))
* **`clientproc`** → Registers the client. ([source](../examples/03_helloservice/multiprocess/clientproc/src/main.cpp))

---

## Testing `multiprocess`

1. Start **mtrouter** (or set IP in [areg.init](../framework/areg/resources/areg.init)).
2. Run `serviceproc` (only one instance).
3. Run one or more `clientproc` (unique names auto-assigned).
4. Clients and services discover each other and communicate transparently across processes or devices.

---

With this setup, you can build scalable, cross-platform, and distributed applications using AREG SDK.
