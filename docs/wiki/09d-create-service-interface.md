# Creating Service Interface Documents with Lusan

This document explains how to create and maintain **Service Interface documents** using **Lusan**, the graphical design and diagnostics tool of the Areg SDK ecosystem. Service interfaces define the contract between **Service Providers** and **Service Consumers** and are the foundation for code generation, communication, and observability in AREG-based systems.

---

## What Is a Service Interface in Areg SDK?

A **Service Interface** describes how distributed components communicate. It defines:

- Service Interface name, version and type (remote or local)
- Service Interface data types
- Service Interface attributes (data) and update notification type
- Service Interface remote methods (requests, responses and broadcasts) and their parameters
- Service Interface constants and additional includes.

Service interfaces are **implementation-independent**. They are used as input for the Areg code generator, which produces consistent and ready-to-use C++ code for both service providers and service consumers.

---

## Why Use Lusan for Service Interface Design?

While service interfaces can be authored manually, **Lusan provides a visual, structured, and validated workflow** that significantly reduces errors and accelerates development.

Key benefits:
- XML Visual modeling instead of manual editing
- Early validation of interface structure
- Guaranteed compatibility with the code generator
- Easier review and maintenance of interfaces over time

---

## Prerequisites

Before creating a service interface with Lusan, ensure that:

- Lusan is built, installed and runs  
  👉 https://github.com/aregtech/areg-sdk-tools
- Areg SDK is available in your development environment
- Java is installed (required later for code generation)
- You have a basic understanding of services, methods, and attributes
- Setup the project directories according to the [guidelines](./09c-use-lusan.md)

---

## Creating a New Service Interface

### Step 1: Launch Lusan

Start the Lusan application from the build output or installation directory, select or setup workspace with project specific directories, which are displayed in the `Workspace` navigation tab.

<div align="center"><a href="../img/lusan-service-if-general.png"><img src="../img/lusan-service-if-general.png" alt="Lusan File System Navigation Bar" style="width:80%;height:80%"/></a></div>

---

### Step 2: Create or Open a Service Interface Document

From the `File` menu or toolbar select `New Service Interface` or open an existing one by browsing file system from the `Workspace` navigation tab.    
Lusan manages service interface files in a structured, project-oriented way.

<div align="center"><a href="../img/lusan-service-if-new-doc.png"><img src="../img/lusan-service-if-new-doc.png" alt="Lusan new service interface" style="width:80%;height:80%"/></a></div>

By default, when create new service interface, it has name `NewServiceInterface`, `Private` type (for multithreading) and version `0.0.1`.

---

### Step 3: Define Service Metadata

In `Overview` type of the Service Interface document specify the core information:
- Service Interface Type 
  The type `Private` is used only in multithreading environment and is not accessible remotely, while `Public` type is used for both -- for multithreading and inter-process communication (IPC), and can be accessed by remote consumers. The type `Internet` is not used yet and cannot be selected.
- Version  
  Set the version number for the service interface control.
- Optional description of the Service Interface, which is useful for documentation.

> [!IMPORTANT]
> The Service Interface Name is automatically set when saving the document. The Service Interface name can be any valid C++ identifier style name -- can contain and start letters, numbers and underscore characters, but cannot contain spaces or special characters. It is recommended to use meaningful names that reflect the purpose of the service. The Service Interface name is not the name of the remotely accessible service name.

This metadata uniquely identifies the service and ensures compatibility between providers and consumers.

<div align="center"><a href="../img/lusan-service-if-overview.png"><img src="../img/lusan-service-if-overview.png" alt="Lusan service interface overview" style="width:80%;height:80%"/></a></div>

---

### Step 4: Define Data Types

Define custom **data types** used in the service interface in the section (tab) `Data Types`. It supports:
- Structures with fields of primitive or complex types;
- Enumerations with named values.
- Additional imported types from other C++ headers.
- Standard simple containers

<div align="center"><a href="../img/lusan-service-if-data-types.png"><img src="../img/lusan-service-if-data-types.png" alt="Lusan service interface data types" style="width:80%;height:80%"/></a></div>

All defined data types are Service Interface specific and are generated as part of the service interface namespace. The service interface, besides or primitives and simple integrated object types, can use any user defined data type.

---

### Step 5: Add Attributes

The Attributes are data of the Service Interface that is shared by Service Providers with Service Consumers. Define attributes in the `Data Attributes` section (tab), and specify the data update notification type: `notify always` and `notify only on value change`. The notifications are automatically generated, depending on the selected notification type:

<div align="center"><a href="../img/lusan-service-if-data-types.png"><img src="../img/lusan-service-if-data-types.png" alt="Lusan service interface data attributes" style="width:80%;height:80%"/></a></div>

The `Data Attributes` is similar to the PubSub technology. It is used to allow Service Consumers to subscribe on a Service Interface data:
- if Attribute's notification type is `OnChange`, the notification is sent only when the value of the attribute is changed;
- if Attribute's notification type is `Always`, the notification is sent every time the attribute is updated by the Service Provider.

The Service Consumer during runtime can subscribe and unsubscribe on any number of attributes of the Service Interface, and the Service Provider is responsible to send the notifications when the attribute is updated.

The Attributes enable reactive and event-driven communication patterns.

---

### Step 6: Add Service Methods (RPC)

Define **service methods** that represent callable operations:
- Method name
- Method type (request, response, broadcast)
- Input parameters and types. Optionally can be set the default value
- In case of request methods, define connection with the response.
- Optionally, describe each method and parameter for documentation.

<div align="center"><a href="../img/lusan-service-if-methods.png"><img src="../img/lusan-service-if-methods.png" alt="Lusan service interface methods" style="width:80%;height:80%"/></a></div>

Request are used by Service Consumer to trigger operations on the Service Provider:
  - A request can have zero or more input parameters.
  - A request can be connected to one response method or not at all (fire-and-forget).
  - A request with a response is by default blocking, meaning the Service Consumer waits for the response before proceeding next call of the same request.
  - If a request is not connected to a response, it is fire-and-forget, non-blocking and can be called by consumer multiple times in a row.
  - The Service Provider can manually unblock the request if needed, and  the Service Consumer can send next call of the same request without waiting for response. In this case:
    - the Service Provider can handle multiple calls of the same request in parallel;
    - the Service Provider is responsible to send the responses in the correct order matching the requests;
    - to prepare and send the response, the Service Provider must use a special Session ID created by the system.
  - If a request is blocking (connected to a response), and the Service Consumer calls the same request again until the response is received, the system replies with the `Request is busy` failure.
  - The Service Providers are responsible to implement the request handling logic.
  - The Service Consumers are responsible to implement the logic of request failure.

Responses are used by Service Provider to send back results of a request to the Service Consumer:
  - A response can have zero or more output parameters.
  - A response is always connected to a request.
  - A Service Consumer during runtime can manually subscribe or unsubscribe on the response to get notified when the response is replied.
  - When Service Consumer sends a request, it cannot unsubscribe on the response until the response is received.
  - The Service Consumer is responsible to implement the logic of response handling.
 
 Broadcasts are used by Service Provider to notify Service Consumers about an event or occurrence:
  - A broadcast can have zero or more output parameters.
  - A broadcast is often used to delivery multiple data at once to the multiple Service Consumers.
  - A broadcast is not connected to any request or response.
  - A Service Consumer during runtime can manually subscribe or unsubscribe on the broadcast to get notified the event.
  - The Service Consumer is responsible to implement the logic of broadcast handling.

Lusan enforces valid method definitions and prevents common design errors.

The Response-Request model enables reactive and event-driven communication patterns.

---

### Step 7: Define Constants

Define **constants** used in the Service Interface in the `Constants` section (tab). Constants are specific for the Service Interface values shared with Service Provider and Service Consumer. Constants do not have notifications and the Service Consumer cannot subscribe on them.

<div align="center"><a href="../img/lusan-service-if-constants.png"><img src="../img/lusan-service-if-constants.png" alt="Lusan service interface constants" style="width:80%;height:80%"/></a></div>

---

### Step 8: Define Additional Includes

Define additional include files int the `Includes` section (tab). Specify any additional C++ header files required by the service interface. These headers are included in the generated code to ensure all types and dependencies are resolved.

<div align="center"><a href="../img/lusan-service-if-includes.png"><img src="../img/lusan-service-if-includes.png" alt="Lusan service interface methods" style="width:80%;height:80%"/></a></div>

---

## Using the Service Interface in a Project

After saving the service interface document add it to your Areg-based project: 
- The easiest way to add is calling the `addServiceInterface()` or `addServiceInterfaceEx()` Areg specific CMake function;
- Point the name of a static library name and the relative path to the `.siml` document;
- The call automatically triggers the **code generator (`codegen.jar`)** to generate C++ files;
- The generated files are included to build a static library;
- The static library is used when building a binary with the Service Provider or Service Consumer;
- One static library can have one or more Service Interface documents.

This guarantees that runtime communication exactly matches the designed interface.  
The developers can also manually call the `codegen.jar` with parameter manually to trigger code generator.  
You can find more details in the description of [`addServiceInterface()`](./02b-cmake-functions.md#addserviceinterface) or in the description of [code generator](./03a-code-generator.md)

---

## Best Practices

- Design interfaces early, before implementation
- Keep interfaces stable and versioned
- Use attributes and broadcasts to avoid excessive polling
- Use requests and responses to trigger remote methods asynchronously
- Review interfaces visually in Lusan during code reviews

---

## Summary

Lusan provides a **clear, visual, and validated workflow** for creating service interfaces in Areg SDK. By designing interfaces graphically and generating code automatically, teams reduce manual errors, improve consistency, and accelerate development of distributed systems.

Service interfaces created with Lusan become the **single source of truth** for communication across the entire application.
