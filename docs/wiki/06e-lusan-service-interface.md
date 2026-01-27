# Creating Service Interface Documents with Lusan

This guide explains how to create and maintain **Service Interface documents** using **Lusan**, the graphical design and diagnostics tool of the Areg SDK ecosystem. Service Interfaces define the communication contract between **Service Providers** and **Service Consumers** and serve as the foundation for code generation, runtime communication, and observability in Areg-based distributed systems.

---

## Table of Contents

- [Introduction](#introduction)
- [Service Interface Concept in Areg SDK](#service-interface-concept-in-areg-sdk)
- [Why Design Service Interfaces with Lusan](#why-design-service-interfaces-with-lusan)
- [Prerequisites](#prerequisites)
- [Creating a Service Interface](#creating-a-service-interface)
- [Using the Service Interface in a Project](#using-the-service-interface-in-a-project)
- [Best Practices](#best-practices)
- [Summary](#summary)

---

## Introduction

A **Service Interface** describes how distributed components communicate in an Areg-based system. It defines data types, methods, notifications, and versioning rules in a technology-independent manner. The Service Interface is consumed by the Areg code generator (`codegen.jar`) to produce consistent, type-safe C++ code for both Service Providers and Service Consumers.

Lusan provides a visual, validated environment to design these interfaces correctly and efficiently, eliminating the need for manual XML editing.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Service Interface Concept in Areg SDK

A Service Interface defines the following elements:

- **Service Metadata**: Interface name, version, and category
- **Data Types**: Custom structures, enumerations, and imported types
- **Data Attributes**: Shared data with state notification behavior
- **Service Methods**: Requests, responses, and broadcasts
- **Constants**: Read-only values shared between providers and consumers
- **Include Dependencies**: External headers required by the interface

### Implementation Independence

Service Interfaces are **implementation-independent**. They describe *what* the service provides, not *how* it is implemented. This separation ensures long-term stability, compatibility, and maintainability of distributed systems.

The same interface definition is used by both Service Providers (implementing the service) and Service Consumers (using the service), guaranteeing consistent communication contracts.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Why Design Service Interfaces with Lusan

Although Service Interface files (`.siml`) can be written manually as XML documents, Lusan provides a safer and more productive approach.

### Key Advantages

**Visual Modeling**
- Graphical editor eliminates manual XML editing
- Clear visual representation of service structure
- Intuitive forms for defining interface components

**Immediate Validation**
- Real-time validation of interface structure
- Prevention of invalid relationships (e.g., orphaned responses)
- Guaranteed compatibility with Areg code generator

**Reduced Errors**
- Enforces correct relationships between interface elements
- Prevents common design mistakes at creation time
- Type-safe definitions reduce runtime communication errors

**Improved Productivity**
- Faster interface design compared to manual XML editing
- Easier review, documentation, and maintenance
- Simplified collaboration across development teams

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Prerequisites

Before creating Service Interface documents, ensure the following:

| Requirement | Description |
|-------------|-------------|
| **Lusan** | Built and available from [areg-sdk-tools](https://github.com/aregtech/areg-sdk-tools) |
| **Areg SDK** | Available in the development environment (for code generation) |
| **Java** | Java 17+ installed for code generation with `codegen.jar` |
| **Workspace** | Configured according to [Setup and Using Lusan](./06d-setup-lusan.md) |

**Recommended Knowledge**:
- Basic understanding of service-oriented architecture
- Familiarity with Service Provider and Service Consumer roles
- Understanding of remote method calls and event notifications

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Creating a Service Interface

### Launching Lusan

Start the Lusan application from the build output or installation directory:

```bash
./build/bin/lusan
```

Select or create a workspace with project-specific directories. These directories are displayed in the **Workspace** navigation tab, providing quick access to project files.

<div align="center"><a href="../img/lusan-si-start-app.png"><img src="../img/lusan-si-start-app.png" alt="Lusan workspace navigation" style="width:50%;height:50%"/></a></div>

### Creating or Opening a Document

**Creating a New Interface**:
1. Navigate to **File → New → Service Interface**
2. Or click the **New Service Interface** button in the toolbar

**Opening an Existing Interface**:
1. Browse the workspace navigation pane
2. Double-click a `.siml` file
3. Or use **File → Open** to browse file system

<div align="center"><a href="../img/lusan-service-if-new-doc.png"><img src="../img/lusan-service-if-new-doc.png" alt="Lusan new service interface" style="width:80%;height:80%"/></a></div>

A newly created interface is initialized with:
- **Name**: `NewServiceInterface`
- **Category**: `Private`
- **Version**: `0.0.1`

### Defining Service Metadata

In the **Overview** section, define the core metadata:

<div align="center"><a href="../img/lusan-service-if-overview.png"><img src="../img/lusan-service-if-overview.png" alt="Lusan service interface overview" style="width:80%;height:80%"/></a></div>

**Service Interface Category**

The category determines the accessibility scope of the service:

- **Private** (`Category="Private"`):
  - Used for multithreading within a single process
  - Not remotely accessible via IPC
  - Faster communication (no serialization overhead)
  - Suitable for internal component communication

- **Public** (`Category="Public"`):
  - Supports both multithreading and inter-process communication (IPC)
  - Accessible across process boundaries and devices
  - Enables distributed system architectures
  - Suitable for services shared between applications

> [!NOTE]
> The `Internet` category is reserved for future use and is not currently supported.

**Version**

Defines compatibility rules between Service Providers and Service Consumers:
- Use semantic versioning (e.g., `1.0.0`, `1.2.3`)
- Increment version when making interface changes
- Consumers can verify compatibility at runtime

**Description**

Optional documentation field for:
- Interface purpose and functionality
- Usage guidelines
- Version history notes
- Long-term maintenance information

**Service Interface Name**

> [!IMPORTANT]
> The Service Interface name is assigned when the document is saved for the first time.
>
> **Naming Rules**:
> - Must be a valid C++ identifier
> - May contain ASCII letters, numbers, and underscore characters
> - Must start with a letter or underscore
> - Spaces and special characters are not allowed
>
> **Note**: The Service Interface name is *not* the runtime service instance name. Multiple service instances can share the same interface.

### Defining Data Types

Custom data types are defined in the **Data Types** section.

<div align="center"><a href="../img/lusan-service-if-data-types.png"><img src="../img/lusan-service-if-data-types.png" alt="Lusan service interface data types" style="width:80%;height:80%"/></a></div>

**Supported Type Categories**:

🔹**Structures**
- Composite types with named fields
- Fields can be primitive types or other defined structures
- Supports nested structures and complex hierarchies

🔹**Enumerations**
- Named integer constants
- Define valid values for parameters and return types
- Supports custom underlying integer types

🔹**Imported Types**
- Types defined in external headers
- Allows reuse of existing C++ types
- Requires corresponding include directive

🔹**Standard Containers**
- Simple containers (arrays, lists, maps)
- Container element types can be primitive or custom

🔹**Type Availability**:
All data types are generated within the Service Interface namespace and are available for use in:
- Data attributes
- Method parameters and return values
- Constants

### Defining Data Attributes

Data Attributes represent shared state published by the Service Provider and consumed by Service Consumers.

<div align="center"><a href="../img/lusan-service-if-attributes.png"><img src="../img/lusan-service-if-attributes.png" alt="Lusan service interface data attributes" style="width:80%;height:80%"/></a></div>

**Attribute Configuration**:

🔹**Name**
- Unique identifier for the attribute
- Must be a valid C++ identifier

🔹**Data Type**
- Any primitive or custom type defined in the interface
- Determines the type of shared state

🔹**Notification Mode**

Controls when Service Consumers receive updates:

- **OnChange**: Sends updates only when the attribute value changes
  - Reduces network traffic
  - Suitable for infrequently changing state

- **Always**: Sends updates on every modification
  - Guarantees delivery of all updates
  - Suitable for time-series data or counters

🔹**Description**
- Optional documentation for the attribute

🔹**Attribute Behavior**:
- Service Consumers subscribe or unsubscribe to attributes at runtime
- Service Provider is responsible for publishing updates
- Supports reactive and event-driven communication patterns
- Multiple consumers can subscribe to the same attribute simultaneously

### Defining Service Methods

Service methods define callable operations and notifications.

<div align="center"><a href="../img/lusan-service-if-methods.png"><img src="../img/lusan-service-if-methods.png" alt="Lusan service interface methods" style="width:80%;height:80%"/></a></div>

**Method Types**:

🔹**Requests**
- Initiated by Service Consumers
- Directed to Service Provider
- May have input parameters
- Can be paired with responses

**Request Execution Modes**:
- **Blocking**: Enforces sequential execution, must be paired with a response
- **Fire-and-forget**: Allows parallel invocation, no response expected

**Advanced**: Manual request unblocking and response ordering is possible using system-generated session identifiers for complex asynchronous workflows.

🔹**Responses**
- Sent by Service Provider
- Always belong to a specific request
- May carry output parameters
- Delivered to the originating Service Consumer
- Additionally, delivered to all Service Consumer subscribed on the response

🔹**Broadcasts**
- Independent notifications from Service Provider
- Not tied to specific requests
- May carry event parameters
- Sent to all subscribed Service Consumers simultaneously

**Method Validation**:
Lusan validates:
- Request-response pairing (blocking requests must have responses)
- Parameter type consistency
- Unique method names within the interface

### Defining Constants

Constants are defined in the **Constants** section and represent shared read-only values.

<div align="center"><a href="../img/lusan-service-if-constants.png"><img src="../img/lusan-service-if-constants.png" alt="Lusan service interface constants" style="width:80%;height:80%"/></a></div>

**Constant Properties**:

🔹**Name**
- Unique identifier for the constant
- Must be a valid C++ identifier

🔹**Data Type**
- Any primitive or enumeration type
- Complex types are supported for constants if they can be represented as constants

🔹**Value**
- Compile-time constant value
- Must match the specified data type

🔹**Behavior**:
- Constants do not generate notifications
- Cannot be subscribed to
- Shared between Service Providers and Service Consumers
- Useful for protocol versions, buffer sizes, timeouts, etc.

### Defining Additional Includes

Additional C++ headers required by the Service Interface are defined in the **Includes** section.

<div align="center"><a href="../img/lusan-service-if-includes.png"><img src="../img/lusan-service-if-includes.png" alt="Lusan service interface includes" style="width:80%;height:80%"/></a></div>

**Include Configuration**:

**Header Path**
- Relative or absolute path to the header file
- Standard library headers: `<vector>`, `<string>`, etc.
- Custom headers: `"MyCustomTypes.hpp"`

**Purpose**:
- Import external type definitions
- Reference types not defined in the Service Interface
- Reuse existing C++ types and classes

**Code Generation**:
These headers are automatically included in the generated code for both Service Provider and Service Consumer implementations.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Using the Service Interface in a Project

After saving the Service Interface document (`.siml` file), integrate it into your project build system.

### CMake Integration

Add the Service Interface to your CMake build configuration:

```cmake
# Include Areg SDK
include(<areg-sdk-path>/areg.cmake)

# Generate service interface code and create static library
addServiceInterface(MyServiceLib ./services/MyService.siml)

# Link the library with your Service Provider or Consumer
macro_declare_executable(
    MyApplication               # Executable name
    MyServiceLib                # Generated service library
    main.cpp                    # Application sources
    MyServiceProvider.cpp
)
```

### Build Process

When you build your project:

1. **Code Generation**: CMake automatically invokes `codegen.jar`
2. **Source Generation**: C++ stub and proxy classes are generated
3. **Compilation**: Generated sources are compiled into a static library
4. **Linking**: The library is linked with your Service Provider and Consumer executables

### Generated Code

The code generator produces:
- **Service Provider Stub**: Base class for implementing the service
- **Service Consumer Proxy**: Client class for consuming the service
- **Event Classes**: Request, response, and broadcast event types
- **Serialization Code**: Data marshaling for IPC communication

**For detailed information**, see:
- [`addServiceInterface()` CMake function](./02e-cmake-functions.md#addserviceinterface)
- [Code Generator documentation](./06b-code-generator.md)

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Best Practices

### Design Process

**Design Before Implementation**
- Define Service Interfaces before writing Service Provider or Consumer code
- Review interfaces with the team early in the development cycle
- Validate interface completeness before code generation

**Interface Stability**
- Keep interfaces stable once deployed
- Use semantic versioning for interface changes
- Maintain backward compatibility when possible
- Document breaking changes clearly

### Communication Patterns

**Prefer Reactive Patterns**
- Use attributes and broadcasts over polling
- Leverage notification mechanisms for state changes
- Reduce network traffic with `OnChange` attributes

**Asynchronous Operations**
- Use asynchronous request patterns when possible
- Avoid blocking calls in performance-critical paths
- Consider fire-and-forget requests for non-critical operations

### Documentation

**Comprehensive Descriptions**
- Document all interface elements (attributes, methods, parameters)
- Explain expected behavior and constraints
- Include version history in service description

**Visual Reviews**
- Review interfaces visually in Lusan during code reviews
- Use Lusan to explain service architecture to new team members
- Maintain interface diagrams for system documentation

### Version Management

**Semantic Versioning**
- Use three-part version numbers: `major.minor.patch`
- Increment major version for breaking changes
- Increment minor version for backward-compatible additions
- Increment patch version for bug fixes

**Compatibility Testing**
- Test compatibility between different interface versions
- Verify runtime version checks work correctly
- Document version compatibility matrix

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

## Summary

Lusan provides a visual, validated, and developer-friendly workflow for designing Service Interfaces in Areg SDK. By defining interfaces graphically and generating code automatically, development teams reduce errors, improve consistency, and accelerate the creation of scalable distributed systems.

### Key Benefits

- **Visual Design**: Graphical editor eliminates manual XML editing
- **Validation**: Real-time validation prevents design errors
- **Code Generation**: Automatic C++ code generation from interface definitions
- **Type Safety**: Strongly-typed communication contracts
- **Productivity**: Faster development and easier maintenance

### Integration with Areg SDK

Service Interfaces created with Lusan:
- Serve as the single source of truth for communication across applications
- Are consumed by `codegen.jar` to generate C++ stub and proxy classes
- Integrate seamlessly with CMake build systems
- Enable both multithreading and IPC communication patterns

Lusan is an essential tool for building reliable, maintainable distributed applications with Areg SDK.

<div align="right"><kbd><a href="#table-of-contents">↑ Back to top ↑</a></kbd></div>

---

Copyright © 2026, Aregtech, www.areg.tech, email: info[at]areg.tech
