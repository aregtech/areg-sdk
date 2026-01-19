# Hello Service!

```

This file is part of the AREG SDK
Copyright (c) 2021–2026, Aregtech
Contact: info\[at]areg.tech
Website: [https://www.areg.tech](https://www.areg.tech)

```

This tutorial demonstrates how to build multithreaded and multiprocess applications with the AREG SDK by reusing the same service and client components.

> [!NOTE]
> Full source code is available in [03_helloservice](../examples/03_helloservice/).

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

This example contains three projects under `examples/03_helloservice`.  
All reuse the same **ServiceComponent** and **ClientComponent** (in `common/src`), but illustrate different execution models:

- **[onethread](../examples/03_helloservice/onethread/):** Both components run in one thread.  
- **[twothreads](../examples/03_helloservice/twothreads/):** Components run in separate threads within one process.  
- **[multiprocess](../examples/03_helloservice/multiprocess/):** Components run in separate processes.

By following this tutorial, you’ll learn how to implement multithreaded and multiprocess applications and see how AREG SDK enables distributed services in edge (mist) networks.

---

## Demonstrated Features

1. A model defines two components: one service provider, one service consumer.  
2. Models can be dynamically loaded/unloaded, controlling the service lifecycle.  
3. Components discover each other automatically.  
4. Once discovered, the client sends a request.  
5. The service processes the request and returns a response.  
6. Response handling:  
   - In **onethread** and **twothreads**, the app exits after response.  
   - In **multiprocess**, the client exits but the service remains active.  
7. **multiprocess** supports multiple clients, independent of startup order.

---

## Key Notes for Service Implementation

- A **Component** can provide and/or consume multiple services.  
- Multiple providers of the same service can coexist, but each must have a unique **Role Name**.  
- Role Names must be unique:  
  - Within a process for _Local_ services.  
  - Across the network for _Public_ services.  
- Service connection status is reported via callbacks:  
  - **Client Component:**  
    ```cpp
    void serviceConnected(NEService::eNetConnection status, ProxyBase& proxy)
    ```  
  - **Service Component:**  
    ```cpp
    bool clientConnected(const ProxyAddress& client, NEService::eServiceConnection connectionStatus)
    ```

---

## Showcase

The examples illustrate how the same components can run in different execution models.  
The business logic is identical — only the **model definitions** in `main.cpp` change.

---

## Directory Structure

- **`generated/src`** → Generated code from the service interface.  
- **`common/src`** → Shared implementations (service + client).  
- **`service`** → Service interface definition (`.siml` file).  

Before starting, create `common/src` and `service` directories under `helloservice`.

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
```

This defines **HelloService** as a *Public* interface (`isRemote="true"`) with a request/response pair.

---

## Code Generation

> [!NOTE]
> Requires [Java 17+](https://java.com/).

### With CMake

In [`CMakeLists.txt`](../examples/03_helloservice/CMakeLists.txt), call:

```cmake
# Add service interface files
addServiceInterface(03_generated examples/03_helloservice/services/HelloService.siml)
```

* `03_generated`: name of the static library containing generated files.
* Path: relative to the `.siml` file.

### With MSVC

Either run manually in the `helloservice` directory or include in a `.bat` pre-build script:

```bash
java -jar <areg-sdk-root>/tools/codegen.jar \
  --root=<areg-sdk-root>/product/generate \
  --doc=<helloservice-project-root>/service/HelloService.siml \
  --target=helloservice/services
```

Generated classes include `HelloServiceStub` (service) and `HelloServiceClientBase` (client).

---

## Model

A **model** defines threads, components, and their relationships. Models may be:

* **Static** — declared at compile time.
* **Dynamic** — created at runtime.

### Concepts

* **Threads & Components:** each thread hosts one or more components. Each component must provide or consume a service.
* **Lifecycle:** unloading a model stops its threads and services.
* **Role Names:** uniquely identify components (per-process for local, global for public).

### Declaring Static Models

1. Define a unique model name.
2. Use macros:

   * `BEGIN_MODEL` / `END_MODEL`
   * `BEGIN_REGISTER_THREAD` / `END_REGISTER_THREAD`
   * `BEGIN_REGISTER_COMPONENT` / `END_REGISTER_COMPONENT`
3. Register services:

   * `REGISTER_IMPLEMENT_SERVICE` (provided by component).
   * `REGISTER_DEPENDENCY` (consumed by component).

> [!NOTE]
> Service providers are identified by **Interface Name**.
> Clients refer by **Role Name**.

All examples reuse [`ServiceComponent`](../examples/03_helloservice/common/src/ServiceComponent.hpp) and [`ClientComponent`](../examples/03_helloservice/common/src/ClientComponent.hpp).

> [!IMPORTANT]
> Link all projects with the **areg library** (shared or static). If you build using `CMake` the call of `macro_declare_executable()` automatically links with the `areg` library.

---

## Project Examples

### [`onethread`](../examples/03_helloservice/onethread/)

Both components run in one thread.

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

With this setup, you can create scalable, cross-platform, distributed applications using the AREG SDK.
