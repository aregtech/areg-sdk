# Service Interface Guide

```
This file is part of Areg SDK
Copyright (c) 2021-2026, Aregtech
Contact: info[at]areg.tech
Website: https://www.areg.tech
```

This guide explains how to define service interfaces and generate code for Areg SDK applications.

---

## Table of Contents

- [Overview](#overview)
- [Service Interface Structure](#service-interface-structure)
  - [Overview Section](#overview-section)
  - [Data Types](#data-types)
  - [Attributes](#attributes)
  - [Methods](#methods)
  - [Constants](#constants)
  - [Includes](#includes)
- [Code Generator](#code-generator)
- [Generated Code](#generated-code)
- [Benefits](#benefits)

---

## Overview

A **Service Interface** defines the communication contract between service providers and consumers in Areg SDK. It specifies:

- Service identity (name, version, scope)
- Custom data types (structures, enumerations, containers)
- Attributes (observable data)
- Methods (requests, responses, broadcasts)
- Constants

Service interfaces are defined in XML files (`.siml` extension). The code generator reads these files and creates C++ base classes that handle serialization, message routing, and communication.

See [Sample.siml](./Sample.siml) for a complete example.

---

## Service Interface Structure

### Overview Section

The `<Overview>` element defines service identity:

```xml
<Overview ID="1" Name="Sample" Version="1.0.0" Category="Public">
    <Description>Example service interface.</Description>
</Overview>
```

| Attribute | Description |
|-----------|-------------|
| `Name` | Service name (used in generated namespace) |
| `Version` | Service version for compatibility checking |
| `Category` | `Public` for IPC, `Local` for single-process only |

**Category values:**
- `Public` (or `isRemote="true"`) — Accessible across processes and network
- `Local` (or `isRemote="false"`) — Only accessible within the same process

---

### Data Types

The `<DataTypeList>` section defines custom types. All types must be serializable.

#### Structures

```xml
<DataType ID="2" Name="SomeStruct" Type="Structure">
    <FieldList>
        <Field DataType="int16" ID="3" Name="anyField1">
            <Value IsDefault="true">0</Value>
            <Description>Field 1</Description>
        </Field>
        <Field DataType="String" ID="5" Name="anyField2">
            <Value IsDefault="true"/>
            <Description>Field 2</Description>
        </Field>
    </FieldList>
</DataType>
```

#### Enumerations

```xml
<DataType ID="6" Name="SomeEnum" Type="Enumerate" Values="default">
    <FieldList>
        <EnumEntry ID="7" Name="Invalid">
            <Value>-1</Value>
            <Description>Invalid enum value</Description>
        </EnumEntry>
        <EnumEntry ID="8" Name="Nothing">
            <Value>0</Value>
        </EnumEntry>
        <EnumEntry ID="9" Name="Something">
            <Value/>
        </EnumEntry>
    </FieldList>
</DataType>
```

#### Imported Types

Reference types from external headers:

```xml
<DataType ID="11" Name="uAlign" Type="Imported">
    <Namespace>NEMemory</Namespace>
    <Location>areg/base/NEMemory.hpp</Location>
</DataType>
```

#### Containers

Define arrays, lists, or maps:

**Array:**
```xml
<DataType ID="12" Name="SomeArray" Type="DefinedType">
    <Container>Array</Container>
    <BaseTypeValue>uint32</BaseTypeValue>
</DataType>
```

**Hash Map:**
```xml
<DataType ID="14" Name="SomeMap" Type="DefinedType">
    <Container>HashMap</Container>
    <BaseTypeValue>SomeStruct</BaseTypeValue>
    <BaseTypeKey>String</BaseTypeKey>
</DataType>
```

| Container Type | Description |
|----------------|-------------|
| `Array` | Dynamic array |
| `LinkedList` | Doubly-linked list |
| `HashMap` | Key-value map (requires `BaseTypeKey`) |

---

### Attributes

Attributes are observable data that clients can subscribe to. When an attribute changes, subscribed clients receive notifications.

```xml
<Attribute DataType="SomeEnum" ID="15" Name="SomeAttr1" Notify="OnChange">
    <Description>Notifies subscribers when the value changes.</Description>
</Attribute>
```

| Notify Value | Behavior |
|--------------|----------|
| `OnChange` | Notify only when value changes |
| `Always` | Notify on every update |

---

### Methods

Methods define the service API. There are three types:

#### Requests

Client-initiated calls to the service:

```xml
<Method ID="17" MethodType="Request" Name="SomeRequest" Response="SomeResponse">
    <Description>Request that expects a response.</Description>
</Method>
```

- `Response` attribute links to the response method
- Omit `Response` for one-way requests

#### Responses

Service replies to requests:

```xml
<Method ID="19" MethodType="Response" Name="SomeResponse">
    <ParamList>
        <Parameter DataType="bool" ID="26" Name="succeeded"/>
    </ParamList>
</Method>
```

Responses are sent only to the requesting client.

#### Broadcasts

Service-initiated notifications to all subscribed clients:

```xml
<Method ID="29" MethodType="Broadcast" Name="SomeBroadcast">
    <ParamList>
        <Parameter DataType="SomeEnum" ID="30" Name="value1"/>
        <Parameter DataType="SomeStruct" ID="31" Name="value2"/>
    </ParamList>
</Method>
```

Clients must explicitly subscribe to receive broadcasts.

---

### Constants

Shared read-only values:

```xml
<Constant DataType="uint16" ID="35" Name="SomeTimeout">
    <Value>100</Value>
</Constant>
```

Constants are accessible from both providers and consumers.

---

### Includes

Additional header files required by the service:

```xml
<IncludeList>
    <Location ID="36" Name="areg/base/NEMath.hpp"/>
</IncludeList>
```

---

## Code Generator

The code generator creates C++ classes from service interface files.

### Requirements

- Java 17 or later

### Usage

```bash
java -jar <areg-sdk>/tools/codegen.jar \
    --root=<project-root> \
    --doc=<path-to-siml> \
    --target=<output-directory>
```

| Parameter | Description |
|-----------|-------------|
| `--root` | Project root directory |
| `--doc` | Path to `.siml` file (relative to root) |
| `--target` | Output directory for generated files |

### CMake Integration

Use the `addServiceInterface` macro:

```cmake
addServiceInterface(MyService_generated services/MyService.siml)
```

This creates a static library containing the generated code.

For details, see the [Code Generator Guide](./wiki/03a-code-generator.md).

---

## Generated Code

The generator creates:

| File | Purpose |
|------|---------|
| `NE<ServiceName>.hpp` | Namespace with types, constants, and method IDs |
| `<ServiceName>Stub.hpp` | Base class for service provider |
| `<ServiceName>ClientBase.hpp` | Base class for service consumer |
| `<ServiceName>Events.hpp` | Event classes for request/response handling |
| `<ServiceName>Proxy.hpp` | Proxy class for client-side communication |

### Implementing a Service Provider

Extend the generated `Stub` class and implement request handlers:

```cpp
class MyServiceImpl : public MyServiceStub
{
public:
    void requestSomeRequest(/* parameters */) override
    {
        // Implement business logic
        responseSomeResponse(true);
    }
};
```

### Implementing a Service Consumer

Extend the generated `ClientBase` class and handle responses:

```cpp
class MyClient : public MyServiceClientBase
{
public:
    void responseSomeResponse(bool succeeded) override
    {
        // Handle response
    }
};
```

---

## Benefits

| Benefit | Description |
|---------|-------------|
| **Consistency** | Uniform API definitions across services |
| **Automation** | Generated code handles serialization and routing |
| **Type Safety** | Compile-time checking of method signatures |
| **Flexibility** | Same interface works for Local and Public services |
| **Maintainability** | Single source of truth for service contracts |
