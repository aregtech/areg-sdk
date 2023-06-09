# Service Interface

```
This file is part of AREG SDK
Copyright (c) 2021-2022, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

## Table of Contents

1. [General Information](#general-information)
2. [Service interface prototype](#service-interface-prototype)
    - [Public and Local Services](#public-and-local-services)
    - [Data Types](#data-types)
        - [Structures](#structures)
        - [Enumerations](#enumerations)
        - [Imported Types](#imported-types)
        - [Containers](#containers)
    - [Attributes](#attributes)
    - [Requests, Responses and Broadcasts](#requests-responses-and-broadcasts)
        - [Requests](#requests)
        - [Responses](#responses)
        - [Broadcasts](#broadcasts)
    - [Constants](#constants)
    - [Includes](#includes)
3. [Code Generator](#code-generator)
4. [Generated Codes](#generated-codes)

---

## General Information

The AREG (*Automated Real-time Event Grid*) is a powerful interface-centric real-time communication engine that facilitates the seamless interaction between connected Things. By acting as ultra-small servers and forming a distributed network of services, AREG simplifies the physical location, discovery, and connection processes for service providers and clients, making communication automatic and transparent. Service providers process requests and provide data, while clients receive responses and have the ability to dynamically subscribe and unsubscribe to data.

To simplify the definition of the Service Interface and eliminate the need for manual creation of Stub, Client, and Event objects, the AREG SDK provides a Service Interface Prototype XML structured document file and a code generator. This allows developers to quickly and easily define their Service Interface and generate the necessary code artifacts.

This document serves as a comprehensive developer guide, outlining the structure and format of the Service Interface prototype document. It provides detailed instructions on how to create a Service Interface using the XML document and how to utilize the code generator to automate the code generation process.

By following the guidelines presented in this document, developers can efficiently define their Service Interfaces and leverage the full capabilities of the AREG Framework, enabling the seamless communication and interaction between services and clients within their applications.

---

## Service Interface Prototype

The Service Interface Prototype is an XML document that defines the structure and specifications of a service API. It consists of the following sections:

1. Service Interface Overview: Provides an overview of the service and its functionality.
2. Service-Specific Data Types: Defines any custom data types used by the service.
3. Service Attributes: Describes the service-specific data or state.
4. Requests, Responses, and Broadcasts: Specifies the service-specific methods for sending requests, receiving responses, and broadcasting events.
5. Constants: Defines any constants specific to the service.
6. Additional Includes: Lists any additional includes required by the service interface.

The Service Interface Prototype XML document is used by the code generator to create base objects that can be extended and implemented to incorporate the service's business logic. An example of a service interface prototype can be found in the [Sample.siml](./Sample.siml) file.

Please note that the Service Interface design GUI tool is currently in development.

> 💡 Stay tuned for updates on the development of the Service Interface design [GUI tool](https://github.com/aregtech/areg-sdk-tools).

### Public and Local Services

Here is an example that defines a _Public_ interface for IPC (Inter-Process Communication):

```xml
<Overview ID="1" Name="Sample" Version="1.0.0" isRemote="true">
    <Description>This is an example of a service interface definition.</Description>
</Overview>
```

The `<Overview>` section contains the service interface's name, version, and the `isRemote` flag, which indicates the accessibility of the interface. When `isRemote="true"`, it means the interface is public and accessible by any component within the network. On the other hand, when `isRemote="false"`, the service is designed for multithreading communication and is not accessible outside of the process. Any attempt to call an API of a Local service is ignored by the system.

### Data Types

Service interfaces can include specific data types to declare variables, service data (attributes), and method parameters. The `<DataTypeList>` section lists the service-specific data types, which should be streamable. Here are the different types that can be defined:

#### Structures

Structures are declared using the `<DataType>` tag with the attribute `Type="Structure"`. Each field refers to a predefined type and must support default values, assignment, comparison, copying, and serialization/deserialization in the [IEIOStream](../framework/areg/base/IEIOStream.hpp) object. 

**Example:**
```xml
<DataType ID="2" Name="SomeStruct" Type="Structure">
    <!-- Structure description -->
    <FieldList>
        <!-- Field 1 -->
        <Field DataType="int16" ID="3" Name="anyField1">
            <Value IsDefault="true">0</Value>
            <Description>Some field 1</Description>
        </Field>
        <!-- Field 2 -->
        <Field DataType="int32" ID="4" Name="anyField2">
            <Value IsDefault="true">0</Value>
            <Description>Just another field</Description>
        </Field>
        <!-- Field 3 -->
        <Field DataType="String" ID="5" Name="anyField3">
            <Value IsDefault="true"/>
            <Description>More field</Description>
        </Field>
    </FieldList>
</DataType>
```

#### Enumerations

Enumerations are declared using the `<DataType>` tag with the attribute `Type="Enumerate"`. Each field can have a value and is automatically streamable.

**Example:**
```xml
<DataType ID="6" Name="SomeEnum" Type="Enumerate" Values="default">
    <!-- Enumeration description -->
    <FieldList>
        <!-- Enum Entry 1 -->
        <EnumEntry ID="7" Name="Invalid">
            <Value>-1</Value>
            <Description>The invalid value of enum</Description>
        </EnumEntry>
        <!-- Enum Entry 2 -->
        <EnumEntry ID="8" Name="Nothing">
            <Value>0</Value>
        </EnumEntry>
        <!-- Enum Entry 3 -->
        <EnumEntry ID="9" Name="Something">
            <Value/>
        </EnumEntry>
        <!-- Enum Entry 4 -->
        <EnumEntry ID="10" Name="Anything">
            <Value/>
        </EnumEntry>
    </FieldList>
</DataType>
```

#### Imported Types

Imported types are declared using the `<DataType>` tag with the attribute `Type="Imported"`. These types may have a namespace and a relative path to the header file where the type is declared. They should support default values, assignment, comparison, copying, and serialization/deserialization in the [IEIOStream](../framework/areg/base/IEIOStream.hpp) object.

**Example:**
```xml
<DataType ID="11" Name="uAlign" Type="Imported">
    <!-- Imported type description -->
    <Namespace>NEMemory</Namespace>
    <Location>areg/base/NEMemory.hpp</Location>
</DataType>
```

#### Containers

Containers, or defined types, are declared using the `<DataType>` tag with the attribute `Type="DefinedType"`. The `<Container>` tag specifies the type of container, the `<BaseTypeValue>` specifies the type of values, and in the case of maps, the `<BaseTypeKey>` specifies the type of keys. All types should support streaming in the [IEIOStream](../framework/areg/base/IEIOStream.hpp).

**Example 1:** Array of uint32 values
```xml
<DataType ID="

12" Name="SomeArray" Type="DefinedType">
    <!-- Defined type description -->
    <Container>Array</Container>
    <BaseTypeValue>uint32</BaseTypeValue>
</DataType>
```

**Example 2:** Linked list of strings
```xml
<DataType ID="13" Name="SomeList" Type="DefinedType">
    <!-- Defined type description -->
    <Container>LinkedList</Container>
    <BaseTypeValue>String</BaseTypeValue>
</DataType>
```

**Example 3:** Hash map with string keys and values of type `SomeStruct`
```xml
<DataType ID="14" Name="SomeMap" Type="DefinedType">
    <!-- Defined type description -->
    <Container>HashMap</Container>
    <BaseTypeValue>SomeStruct</BaseTypeValue>
    <BaseTypeKey>String</BaseTypeKey>
</DataType>
```

> 💡 Note: The examples provided are for illustration purposes and can be adjusted according to the specific requirements of the service interface.

### Attributes

Attributes in services are data elements that can be exchanged with service consumers (clients). Clients have the ability to dynamically subscribe or unsubscribe to receive notifications on data updates. The attributes are listed in the `<AttributeList>` section and should be serializable in the [IEIOStream](../framework/areg/base/IEIOStream.hpp) object.

When a *Service Consumer* subscribes to an attribute, it receives a notification containing the cached value and a validation flag. If the data has not been cached or is invalid, the first notification may indicate an *"Invalid"* state. Shortly after, the server sends a second notification with the actual value and data state. It is important to check the data state to respond correctly.

Here are examples of declaring attributes with different notification behaviors:

*1. Attribute that notifies subscribers only when the value changes:*

```xml
<Attribute DataType="SomeEnum" ID="15" Name="SomeAttr1" Notify="OnChange">
    <!-- Attribute description -->
    <Description>An attribute that notifies subscribers only when the value changes.</Description>
</Attribute>
```

*2. Attribute that notifies subscribers each time the value is set (even if not changed):*

```xml
<Attribute DataType="SomeStruct" ID="16" Name="SomeAttr2" Notify="Always">
    <!-- Attribute description -->
    <Description>Another attribute that notifies subscribers every time the value is set (even if not changed).</Description>
</Attribute>
```

> 💡 Note: The examples provided demonstrate the different notification types for attributes. The specific attribute configuration should align with the requirements of the service interface.

### Requests, Responses, and Broadcasts

In Services, there are three types of methods: requests, responses, and broadcasts. These methods are defined in the `<MethodList>` section of the service interface XML document.

#### Requests

Requests are actions initiated by clients and executed on the *Service Provider* (server) side. They follow a request-reply pattern, where clients send requests and the server replies with a response. Requests can either have no response or be linked to a specific response. When a request has a linked response, the processing of the request is blocked until the server replies. Multiple requests can be linked to the same response. All parameters of a request must be serializable.

**Example:** A request with no parameter linked to a response:
```xml
<Method ID="17" MethodType="request" Name="SomeRequest1" Response="SomeResponse1">
    <Description>Request and response with no parameters.</Description>
</Method>
<Method ID="19" MethodType="response" Name="SomeResponse1">
    <Description/>
</Method>
```

#### Responses

Responses are the *Service Providers's* (server's) replies to requests. Each response is linked to at least one request. When the server calls a response, it triggers a remote callback on the client side. Clients can also manually subscribe and unsubscribe from responses without making a request. It allows clients to receive the result of an action without concerning themselves with which component triggered the action.

**Example:** Declaration of responses:
```xml
<Method ID="19" MethodType="response" Name="SomeResponse1">
    <Description/>
</Method>
<Method ID="22" MethodType="response" Name="SomeResponse">
    <Description>Response, where 2 requests can connect.</Description>
    <ParamList>
        <Parameter DataType="bool" ID="26" Name="succeeded"/>
    </ParamList>
</Method>
```

#### Broadcasts

Broadcasts are special service methods used to fire events and pass multiple data simultaneously. They do not have linked methods and act as events triggered by the *Service Provider* (server). Clients need to manually subscribe to broadcasts during runtime to receive the data. Broadcast data is only available during the call and is not stored beyond that point. Unlike attributes, which provide cached and actual data, broadcasts only notify clients when the server calls the broadcast.

**Example:** Declaration of a broadcast with parameters:
```xml
<Method ID="29" MethodType="broadcast" Name="SomeBroadcast">
    <Description>Broadcast with parameters. Can pass multiple parameters at once.</Description>
    <ParamList>
        <Parameter DataType="SomeEnum" ID="30" Name="value1">
            <Description>Parameter name must be different from 'param1' in response.</Description>
        </Parameter>
        <Parameter DataType="NEMemory::uAlign" ID="31" Name="value2"/>
        <Parameter DataType="SomeStruct" ID="32" Name="value3"/>
        <Parameter DataType="SomeArray" ID="34" Name="value4"/>
    </ParamList>
</Method>
```

Note: It is important to ensure unique parameter names in responses and broadcasts. If a parameter name is not unique, it must have the same type in all methods to avoid conflicts.

### Constants

Constants in the Service Interface XML document facilitate the sharing of read-only values between service providers and clients. These values can be used for various purposes, such as defining timeouts or other configuration parameters. Constants are listed in the `<ConstantList>` section of the document.

Example: Declaration of a constant:
```xml
<Constant DataType="uint16" ID="35" Name="SomeTimeout">
    <Value>100</Value>
    <Description>Defines a constant for specifying a timeout period.</Description>
</Constant>
```

In the given example, the constant named "SomeTimeout" is declared with a data type of `uint16` and a value of `100`. It can be referenced by both the service provider and clients to ensure consistent behavior throughout the system.


### Includes

The Service Interface XML document allows for the inclusion of additional files or resources that are required by the service provider and service consumers. These includes are listed in the `<IncludeList>` section of the document.

Example: Declaration of includes:
```xml
<IncludeList>
    <Location ID="36" Name="areg/base/NEMath.hpp">
        <Description>Additional include file for mathematical operations.</Description>
    </Location>
</IncludeList>
```

In the provided example, an include file named "NEMath.hpp" from the `areg/base` directory is declared. This inclusion allows the service provider and service consumers to access the functionality provided by the included file, such as mathematical operations. Including relevant files ensures that the necessary dependencies are met and promotes modularity and code reuse within the system.

---

## Code Generator

The AREG Software Development Kit (SDK) includes a code generator tool located in the [_tools_](../tools/) folder. This tool aims to simplify the coding process and reduce errors by generating base objects based on the definitions provided in the Service Interface XML document. It is important to note that the generated files should not be modified directly but extended to implement the required override functions.

Typically, service providers will implement _Request_ methods and make decisions on when to call responses, broadcasts, or update attribute data. On the other hand, service clients will implement the relevant _Response_, _Broadcast_, and _Attribute_ update notification methods as needed, making decisions on when to send requests and subscribe to events.

To generate the source code, you can edit and run the "generate.sh" or "generate.bat" file, or run "codegen.jar" from the command line. Make sure to set the correct `AREG_SDK_ROOT` folder path before running the code generation scripts.

> 💡 To optimize performance and avoid unnecessary code, service consumers should only override the responses, broadcasts, and update notifications that they actually use. Unneeded overrides will never be called. For example, if a service has attributes named `One` and `Two`, and a client is only subscribed to updates for `One`, there is no need to override the update of `Two` since it will never be received.

The code generator requires [Java](https://www.java.com/) to be installed on your machine. It requires the following parameters:
- `--root`: Path to the development project. All paths are relative to the root.
- `--doc`: Relative path to the Service Interface XML document for code generation.
- `--target`: Relative path to the output folder for the generated files.

**Example:** Run the code generator from the command line
```bash
java -jar ~/projects/areg-sdk/codegen.jar --root=~/projects/my_project --target=src/generated/ --doc=interface/MyService.siml
```

Here's an example of a [generate.bat](../examples/12_pubsvc/res/generate.bat) file in the AREG SDK:
```batch
:: Set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=C:\Projects\aregtech\areg-sdk
:: .bat file directory
set BATCH_ROOT=%~dp0
:: In case of examples, one level up.
set PROJECT_ROOT=%BATCH_ROOT%\..
:: Specify the relative path of the output folder
set CODE_GEN=generated\src
:: Include codegen.jar in the CLASSPATH
set CLASSPATH=%CLASSPATH%;%AREG_SDK_ROOT%\tools;%AREG_SDK_ROOT%\tools\codegen.jar

@echo Generating class of HelloService Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\HelloService.siml --target=%CODE_GEN%
```

These examples demonstrate how to run the code generator tool from the command line by specifying the necessary parameters.

---

## Generated Codes

The code generator provided by the AREG Software Development Kit (SDK) simplifies the development process by automatically creating source code for both **Service Providers** (*servers*) and **Service Consumers** (*clients*). Additionally, it generates a namespace with the prefix `NE` for shared common data. When creating a server, developers extend classes that end with `Stub`, while clients extend classes that end with `ClientBase`. It is important to note that the files generated in the `private` subdirectory are for internal use only and should not be modified or used directly.

Here's an example of the folder structure for the generated codes:

[![File structure after generating codes](https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/generated-sources.png)](https://github.com/aregtech/areg-sdk/blob/master/docs/img/generated-sources.png)

> 💡 It is recommended to include the generated codes in a separate **static library** that can be linked with both Service Provider and Service Consumer applications. This approach is particularly effective in multiprocessing applications, where Service Providers and Service Consumers run in separate processes or on separate machines.

---
