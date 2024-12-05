# Service Interface

```
This file is part of AREG SDK
Copyright (c) 2021-2023, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

## Table of Contents

- [General Information](#general-information)
- [Service Interface Prototype](#service-interface-prototype)
  - [Overview: Public and Local Services](#overview-public-and-local-services)
  - [Data Types](#data-types)
    - [Structures](#structures)
    - [Enumerations](#enumerations)
    - [Imported Types](#imported-types)
    - [Containers](#containers)
  - [Attributes](#attributes)
  - [Requests, Responses, and Broadcasts](#requests-responses-and-broadcasts)
    - [Requests](#requests)
    - [Responses](#responses)
    - [Broadcasts](#broadcasts)
  - [Constants](#constants)
  - [Includes](#includes)
- [Code Generator](#code-generator)
  - [Generated Codes](#generated-codes)
  - [Benefits of Using the Service Interface and Code Generator](#benefits-of-using-the-service-interface-and-code-generator)

---

## General Information

The **Service Interface** in the AREG SDK enables the seamless definition and automation of service communication between providers and consumers in distributed systems. The AREG SDK provides an XML-based **Service Interface Prototype** that specifies the structure of service APIs, including data types, attributes, methods (requests, responses, broadcasts), and constants.

To streamline development, the SDK includes a **Code Generator** that uses this XML prototype to create the necessary code artifacts for service providers and consumers. These artifacts ensure efficiency, consistency, and alignment with AREG's real-time communication framework.

This guide explains the structure and components of the Service Interface Prototype and how to use the code generator effectively to simplify development.

---

## Service Interface Prototype

The Service Interface Prototype is an XML document that specifies a service API's structure. It consists of several sections:

1. **Overview**: A high-level overview of the service.
2. **Data Types**: Custom data types used by the service.
3. **Attributes**: Data or state specific to the service.
4. **Requests, Responses, and Broadcasts**: Methods for sending requests, receiving responses, and broadcasting events.
5. **Constants**: Read-only values specific to the service.
6. **Additional Includes**: Additional files required for the service interface.

Developers can use this XML document as input to the code generator, creating base objects that can then be extended to incorporate specific business logic. A sample XML document is available in [Sample.siml](./Sample.siml).

### Overview: Public and Local Services

Defines the high-level service properties such as its name, version, and scope (Public for IPC or Local for multithreading).

```xml
<Overview ID="1" Name="Sample" Version="1.0.0" isRemote="true">
    <Description>This is an example of a service interface definition.</Description>
</Overview>
```

- `<Overview>` specifies the interface's name, version, and `isRemote` status.
- `isRemote="true"` marks the service as public and accessible within the network, while `isRemote="false"` restricts it to local (multithreading) communication only. Calls to a local service API from external processes are ignored.

---

### Data Types

Specifies custom data types, including structures, enumerations, imported types, and containers like arrays or hash maps. The `<DataTypeList>` section organizes service-specific data types. These types should be streamable.

#### Structures

Structures are defined with the `<DataType>` tag and `Type="Structure"`, listing fields and supporting serialization/de-serialization.

**Example:**
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

Enumerations use `<DataType Type="Enumerate">` and list possible values.

**Example:**
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

Imported types are predefined types from external headers, marked with `Type="Imported"`.

**Example:**
```xml
<DataType ID="11" Name="uAlign" Type="Imported">
    <Namespace>NEMemory</Namespace>
    <Location>areg/base/NEMemory.hpp</Location>
</DataType>
```

#### Containers

Containers are composite types, defined with `Type="DefinedType"`. Types include arrays, linked lists, and hash maps.

**Example 1: Array of uint32 values**
```xml
<DataType ID="12" Name="SomeArray" Type="DefinedType">
    <Container>Array</Container>
    <BaseTypeValue>uint32</BaseTypeValue>
</DataType>
```

**Example 2:** Hash map with string keys and values of type `SomeStruct`
```xml
<DataType ID="14" Name="SomeMap" Type="DefinedType">
    <Container>HashMap</Container>
    <BaseTypeValue>SomeStruct</BaseTypeValue>
    <BaseTypeKey>String</BaseTypeKey>
</DataType>
```

### Attributes

Attributes are exchangeable data elements between the service provider and clients. Clients can subscribe to receive notifications when data changes. Attributes are listed in the `<AttributeList>` section.

**Example**: Attribute that notifies on every change:
```xml
<Attribute DataType="SomeEnum" ID="15" Name="SomeAttr1" Notify="OnChange">
    <Description>Attribute that notifies subscribers only when the value changes.</Description>
</Attribute>
```

### Requests, Responses, and Broadcasts

Service methods are categorized as requests, responses, or broadcasts.

#### Requests

Requests are client-initiated actions. They can have a linked response, causing the server to reply after execution.

**Example**: Request with no parameters:
```xml
<Method ID="17" MethodType="request" Name="SomeRequest1" Response="SomeResponse1"/>
```

#### Responses

Responses are server replies to requests, automatically triggering a callback on the client side.

**Example**: Response with parameters:
```xml
<Method ID="19" MethodType="response" Name="SomeResponse">
    <ParamList>
        <Parameter DataType="bool" ID="26" Name="succeeded"/>
    </ParamList>
</Method>
```

#### Broadcasts

Broadcasts are server-initiated events with no linked requests. Clients must subscribe manually to receive them.

**Example**: Broadcast with parameters:
```xml
<Method ID="29" MethodType="broadcast" Name="SomeBroadcast">
    <ParamList>
        <Parameter DataType="SomeEnum" ID="30" Name="value1"/>
        <Parameter DataType="SomeStruct" ID="31" Name="value2"/>
    </ParamList>
</Method>
```

### Constants

Constants define read-only values, shared between providers and consumers.

**Example**:
```xml
<Constant DataType="uint16" ID="35" Name="SomeTimeout">
    <Value>100</Value>
</Constant>
```

### Includes

The `<IncludeList>` section specifies additional files required by the service.

**Example**:
```xml
<IncludeList>
    <Location ID="36" Name="areg/base/NEMath.hpp"/>
</IncludeList>
```

---

## Code Generator

The AREG SDK includes a code generator tool in the [_tools_](../tools/) directory to automate code creation based on the Service Interface XML document. Generated files should not be modified directly; instead, extend the classes and override only necessary functions.

To generate source code, execute `codegen.jar` with the following parameters:
- `--root`: Project root path.
- `--doc`: Relative path to the Service Interface XML.
- `--target`: Output folder for generated files.

**Example**:
```bash
java -jar <areg-sdk-root>/tools/codegen.jar --root=<project-root> --target=product/generated --doc=services/MyService.siml
```

For more details see [AREG SDK Code Generator (`codegen.jar`)](./wiki/03a-code-generator.md) document.

### Generated Codes

The code generator simplifies development by creating classes for **Service Providers** (servers) and **Service Consumers** (clients). It also generates a namespace prefixed with `NE`. Extend classes with `Stub` for servers and `ClientBase` for clients, as required.

### Benefits of Using the Service Interface and Code Generator

- **Efficiency**: Automates repetitive tasks, reducing manual coding.
- **Consistency**: Ensures uniformity across service definitions and implementations.
- **Flexibility**: Allows developers to focus on business logic by extending generated classes.
- **Scalability**: Supports public (IPC) and local (multithreading) services, enabling seamless communication in diverse setups.

---
