# Service Interface

```
This file is part of AREG SDK
Copyright (c) 2021-2022, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

The AREG (_Automated Real-time Event Grid_) is an interface-centric real-time communication engine that enables connected Things to act as ultra-small servers and form a distributed network of services. The AREG engine handles the physical location, discovery, and connection of service providers and clients, making communication automatic and transparent. Services process requests and provide data, while clients receive responses and may dynamically subscribe and unsubscribe to data. To simplify the definition of the _Service Interface_ and avoid tedious job of creating Stub, Client and Event objects, there can be used _Service Interface Prototype_ XML structured document file and the code generator.

This document is a developer guide and describes how the _Service Interface_ prototype document is structured.

## Service interface prototype

The _Service Interface_ (service API) is defined in an XML prototype document, which includes following sections:
1. The service interface overview.
2. The service specific data types.
3. The service specific data called _service attributes_.
4. The service specific methods called _requests_, _responses_ and _broadcasts_.
5. The service specific constants.
6. Additional includes required by service interface.

The interface prototype XML document is used by _code generator_ to create base objects to extend and implement service business logic. The [Sample.siml](./Sample.siml) file provides an example of the service interface prototype.

> 💡 The Service Interface design GUI tool is in development state.

### Public and Local services

The following example defines a _Public_ (used for IPC) interface:
```xml
<Overview ID="1" Name="Sample" Version="1.0.0" isRemote="true">
    <Description>This is an example of service interface definition.</Description>
</Overview>
```
The `<Overview>` section contains service interface name, version and the flag `isRemote` that indicates interface accessibility (_Public_ or _Local_). The value `isRemote="true"` means the interface is public and is accessible by any component within the network. The value `isRemote="false" means the service is for multithreading communication and is not accessible outside of process. Any attempt to call an API of _Local_ service is ignored by the system.

### Data types

Service interfaces may include specific data types to declare variables, service data (attributes), and method parameters. The `<DataTypeList>` section lists service specific data types, which must be streamable. External types can be imported, and the `operator >>` or `operator <<` should be able to serialize / deserialize data in the [IEIOStream](../framework/areg/base/IEIOStream.hpp).

#### Structures

The structures are declared in `<DataTypeList>` section using the `<DataType>` tag and attribute `Type="Structure"`. Each field refers to a predefined type (including defined in interface) and must be possible to set default value, assign, compare, copy, and serialize / deserialize in the [IEIOStream](../framework/areg/base/IEIOStream.hpp) object.

*An example of declaring structure with fields and default values:*
```xml
<DataType ID="2" Name="SomeStruct" Type="Structure">
    <Description>Some structure with data. It will become new type.</Description>
    <FieldList>
        <Field DataType="int16" ID="3" Name="anyFiled1">
            <Value IsDefault="true">0</Value>
            <Description>Some filed 1</Description>
        </Field>
        <Field DataType="int32" ID="4" Name="anyField2">
            <Value IsDefault="true">0</Value>
            <Description>Just another field</Description>
        </Field>
        <Field DataType="String" ID="5" Name="anyField3">
            <Value IsDefault="true"/>
            <Description>More field.</Description>
        </Field>
    </FieldList>
</DataType>
```

#### Enumerations

The enumerations are declared in `<DataTypeList>` section using the `<DataType>` tag and attribute `Type="Enumerate"`. Each field may have a value. The enumerations are automatically streamable.

*An example of declaring enumeration with fields and values:*
```xml
<DataType ID="6" Name="SomeEnum" Type="Enumerate" Values="default">
    <Description>A new enumeration of the service interface.</Description>
    <FieldList>
        <EnumEntry ID="7" Name="Invalid">
            <Value>-1</Value>
            <Description>The invalid value of enum.</Description>
        </EnumEntry>
        <EnumEntry ID="8" Name="Nothing">
            <Value>0</Value>
        </EnumEntry>
        <EnumEntry ID="9" Name="Something">
            <Value/>
        </EnumEntry>
        <EnumEntry ID="10" Name="Anything">
            <Value/>
        </EnumEntry>
    </FieldList>
</DataType>
```

 #### Imported types

The imported types are declared in `<DataTypeList>` section using `<DataType>` tag and attribute `Type="Imported"`. The imported types may have _Namespace_ and relative path to the header file where the type is declared. The imported types must be possible to set default value, assign, compare, copy, and serialize / deserialize in the [IEIOStream](../framework/areg/base/IEIOStream.hpp) object.

*An example of imported type:*
 ```xml
<DataType ID="11" Name="uAlign" Type="Imported">
    <Description>This example exports NEMemory::uAlign in the service interface.</Description>
    <Namespace>NEMemory</Namespace>
    <Location>areg/base/NEMemory.hpp</Location>
</DataType>
```

#### Containers

The containers (defined types) are declared in `<DataTypeList>` section using `<DataType>` tag and attribute `Type="DefinedType"`. The `<Container>` tag specifies the type of container, the `<BaseTypeValue>` specifies the type of values and in case of _maps_ the `<BaseTypeKey>` specifies the type of keys. All types must be possible to `stream` in the [IEIOStream](../framework/areg/base/IEIOStream.hpp).

*An example of defining new type of array of uint32 values:*
```xml
<DataType ID="12" Name="SomeArray" Type="DefinedType">
    <Description>Defines new type of array</Description>
    <Container>Array</Container>
    <BaseTypeValue>uint32</BaseTypeValue>
</DataType>
```

*An example of defining new type of linked list of strings:*
```xml
<DataType ID="13" Name="SomeList" Type="DefinedType">
    <Description>New type of linked list.</Description>
    <Container>LinkedList</Container>
    <BaseTypeValue>String</BaseTypeValue>
</DataType>
```

*An example of defining new type of hash map, which keys are strings and values have type `SomeStruct`:*
```xml
<DataType ID="14" Name="SomeMap" Type="DefinedType">
    <Description>This example defines hash-map where key is exported and value is new data type.</Description>
    <Container>HashMap</Container>
    <BaseTypeValue>SomeStruct</BaseTypeValue>
    <BaseTypeKey>String</BaseTypeKey>
</DataType>
```

---

### Attributes

_Attributes_ in services, similar to a PubSub solution, are data to exchange with service consumers(clients). Clients can dynamically subscribe or unsubscribe to receive notifications on data updates. The attributes are listed under the `<AttributeList>` section and must be serializable in the [IEIOStream](../framework/areg/base/IEIOStream.hpp) object.

Clients always can access to the data of an attribute. When a client subscribes to an attribute, it receives a notification with the cached value and a validation flag. If the data has not been cached or it is invalid, the first notification may indicate _invalid_ state, followed shortly by a second notification from the server with the actual value and data state. It is important to check the data state to make correct reactions.

*An example of declaring attribute to notify only on value change:*
```xml
<Attribute DataType="SomeEnum" ID="15" Name="SomeAttr1" Notify="OnChange">
    <Description>An attribute to notify subscribers only when value is changed.</Description>
</Attribute>
```

*An example of declaring attribute to notify when value set:*
```xml
<Attribute DataType="SomeStruct" ID="16" Name="SomeAttr2" Notify="Always">
    <Description>Another attribute to notify subscribers any time when value is set (maybe not changed).</Description>
</Attribute>
```

If the notification type of an _Attribute_ is `OnChange`, the value is forwarded to subscribed clients only if new value is not equal or the validation state has been changed. In case of `Always`, the value is forwarded each time the data is set.

### Requests, responses and broadcasts

_Requests_ and _Responses_ in the services are methods that act like _Request-Reply_ (actions-centric) solution, where clients send requests as actions to process on a server and the server replies with a response. There are as well _Broadcast_ methods, which act like following the logic of PubSub (data-centric), where clients can dynamically subscribe and unsubscribe to a _Broadcast_ event. All of these methods are defined in the `<MethodList>` section of the service interface XML document.

> 💡 In the service interface XML document the request, response and the broadcast may have same name.

> 💡 It is recommended that all parameter names in responses and broadcasts have unique names. If a parameter name is not unique, it must have the same type in all methods. For instance, if a broadcast and a response have a parameter named `success` and has type `bool`, that parameter with the same name can exist in other responses and broadcast, but as well must have type `bool` and nothing else.

#### Requests

The requests are actions executed on the service provider (server) side when called by service consumers (clients). Requests may have no or one linked response sent by service provider as a reply to request. If a request has a response, the processing request is blocked until the service provider replies with the linked response. It is possible to manually unblock the request, but then the response must also be manually prepared for a reply. An example of manually unblocked request can be seen in [this example](../examples/21_pubunblock/). Multiple requests can be linked with the same response. If a request has no response, it is always unblocked and can be called one after another. If a request is busy and blocked, and a client triggers the same request, it is immediately replied with a request failure indicating the reason of failure (in this example _reuqest is busy_). All parameters of the request must be serializable.

*An example to demonstrate a request with no parameter linked with a response:*
```xml
<Method ID="17" MethodType="request" Name="SomeRequest1" Response="SomeResponse1">
    <Description>Request and response with no parameters.</Description>
</Method>
<Method ID="19" MethodType="response" Name="SomeResponse1">
    <Description/>
</Method>
```

*An example to demonstrate a request with multiple parameters linked with a response:*
```xml
<Method ID="20" MethodType="request" Name="SomeRequest2" Response="SomeResponse">
    <Description>A request with parameters that is connected to SomeResponse interface.</Description>
    <ParamList>
        <Parameter DataType="int32" ID="21" Name="param1"/>
        <Parameter DataType="SomeStruct" ID="23" Name="param2"/>
        <Parameter DataType="SomeEnum" ID="24" Name="param3">
            <Value IsDefault="true">SomeEnum::Nothing</Value>
            <Description>parameter with default value</Description>
        </Parameter>
    </ParamList>
</Method>
<Method ID="22" MethodType="response" Name="SomeResponse">
    <Description>Response, where 2 requests can connect.</Description>
    <ParamList>
        <Parameter DataType="bool" ID="26" Name="succeeded"/>
    </ParamList>
</Method>
```

*An example to demonstrate 2 different requests linked with the same response:*
```xml
<Method ID="20" MethodType="request" Name="SomeRequest2" Response="SomeResponse">
    <Description>A request with parameters that is connected to SomeResponse interface.</Description>
    <ParamList>
        <Parameter DataType="int32" ID="21" Name="param1"/>
        <Parameter DataType="SomeStruct" ID="23" Name="param2"/>
        <Parameter DataType="SomeEnum" ID="24" Name="param3">
            <Value IsDefault="true">SomeEnum::Nothing</Value>
            <Description>parameter with default value</Description>
        </Parameter>
    </ParamList>
</Method>
<Method ID="25" MethodType="request" Name="SomeRequest3" Response="SomeResponse">
    <Description>Another request with parameter that is connected with SomeResponse method</Description>
    <ParamList>
        <Parameter DataType="NEMemory::uAlign" ID="28" Name="param"/>
    </ParamList>
</Method>
<Method ID="22" MethodType="response" Name="SomeResponse">
    <Description>Response, where 2 requests can connect.</Description>
    <ParamList>
        <Parameter DataType="bool" ID="26" Name="succeeded"/>
    </ParamList>
</Method>
```

*An example to demonstrate a request with no response:*
```xml
<Method ID="27" MethodType="request" Name="StandAlone">
    <Description>A request with no response.</Description>
</Method>
```

#### Responses

The responses are replies to requests sent by service providers (servers) to service consumers (clients). Each response is linked to at least one request, and if a response has no request, it is ignored and never triggered. When the server calls a response, it is automatically triggered on the client side as a callback. As well, clients can manually subscribe and unsubscribe from the response without calling a request, particularly if they are interested in the result of an action and not concerned about which component triggered the action. 

*An example to demonstrate declaration of the responses.*
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

The [_Request_](#requests) chapter contains examples of requests linked to responses. It's crucial that the server calls the response after executing the request, or else the request will not be released and the system will not trigger the next call of the request. It is important to note that a response must not be called within the request method, but it can be called at any time while the request is pending. Since the AREG system is asynchronous, the time between when the server request is triggered, processed, and replied to with a response is uncertain.

#### Broadcasts

_Broadcasts_ are a type of special service method used to fire an event and pass multiple data simultaneously. Broadcasts have no linked methods and act as events fired by the service provider (server). The service consumers (clients) need to manually subscribe to the broadcast during runtime to receive the data. 

Broadcast data is only available during the call and is not stored (cached) beyond that point. This means that if a client subscribes to a broadcast, it will only receive a notification when the server calls the broadcast. On the other hand, in the case of an attribute, the client receives the cached data initially, followed by the actual data, even if it has not changed.

*An example to demonstrate declaration of the broadcast with parameters:*
```xml
<Method ID="29" MethodType="broadcast" Name="SomeBroadcast">
    <Description>Broadcast with parameters. Can pass multiple parameters at once.</Description>
    <ParamList>
        <Parameter DataType="SomeEnum" ID="30" Name="value1">
            <Description>We already have 'param1' in response, this parameter name must differ.</Description>
        </Parameter>
        <Parameter DataType="NEMemory::uAlign" ID="31" Name="value2"/>
        <Parameter DataType="SomeStruct" ID="32" Name="value3"/>
        <Parameter DataType="SomeArray" ID="34" Name="value4"/>
    </ParamList>
</Method>
```

### Constants
The Service Interface XML document may have constants listed in `<ConstantList>` section. The constants are used to share read-only value between service provider and clients. For example, a timeout value.

*An example to demonstrate declaration of a constant:*
```xml
<Constant DataType="uint16" ID="35" Name="SomeTimeout">
    <Value>100</Value>
    <Description>Define a constant if need.</Description>
</Constant>
```

### Includes
The Service Interface XML document may have additional includes. For example, a separate _DataType_ document, to share between multiple services or a header file to share between service provider and service consumers. The includes are listed in the `<IncludeList>` section.

*An example to demonstrate declaration of includes:*
```xml
<IncludeList>
    <Location ID="36" Name="areg/base/NEMath.hpp">
        <Description>can make additional includes</Description>
    </Location>
</IncludeList>
```

---

## Code generator

The AREG SDK offers a code-generator located in the [_tools_](../tools/) folder to simplify coding and reduce errors. This tool generates base objects using definitions from the Service Interface XML document. It is important to note that the generated files should not be modified, but rather extended to implement required override functions. Typically, service providers will implement _Request_ methods and make decisions when to call responses, broadcasts or set attribute data, while service clients will implement used _Response_, _Broadcast_, and _Attribute_ update notification methods as needed, and make decisions when to send request and subscribe to events. To generate source code, edit and run the "generate.sh" or "generate.bat" file, or run "codegen.jar" from the command line.

> 💡 To avoid unnecessary code and improve performance, service consumers should only override the responses, broadcasts, and update notifications that they actually use. Any unneeded overrides will never be called. For instance, if a service has two attributes named `One` and `Two`, and a client is only subscribed to updates for `One`, there is no need to override the update of `Two` since it will never be received.

> 💡 Before running `generate.sh` or `generate.bat` scripts, set correct `AREG_SDK_ROOT` folder path.

The code-generator requires [Java](https://www.java.com/) installed on machine. The code-generator requires following parameters:
- `--root` option is the path to development project and all paths are relative to the _root_.
- `--doc` option is the relative path to the service interface XML document to generate code.
- `--target` option is the relative path to output generated files.

*Examples of running code generator from command line:*
```bash
$ java -jar ~/projects/areg-sdk/codegen.jar --root=~/projects/my_project --target=src/generated/ --doc=interface/MyService.siml
```

*An example of [generate.bat](../examples/12_pubsvc/res/generate.bat) file in AREG SDK:*
```bat
:: set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=C:\Projects\aregtech\areg-sdk
:: .bat file directory
set BATCH_ROOT=%~dp0
:: In case of examples, one level up.
set PROJECT_ROOT=%BATCH_ROOT%\..
:: Specify the relative path of output folder
set CODE_GEN=generated\src
:: Include codegen.jar in CLASSPATH
set CLASSPATH=%CLASSPATH%;%AREG_SDK_ROOT%\tools;%AREG_SDK_ROOT%\tools\codegen.jar

@echo Generating class of HelloService Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\HelloService.siml --target=%CODE_GEN%
```
---

## Generated codes

The code generator simplifies development by creating sources for both service providers (servers) and service consumers (clients). It also generates a namespace with prefix `NE` for shared common data. To create a server, developers extend classes ending with `Stub`, while clients extend classes ending with `ClientBase`. It is important to note that the files generated in the `private` subdirectory should not be modified and used, since they are for internal use only.

*An example of folder structure of generated codes:*
<br><a href="https://github.com/aregtech/areg-sdk/blob/master/docs/img/generated-sources.png"><img src="https://raw.githubusercontent.com/aregtech/areg-sdk/master/docs/img/generated-sources.png" alt="File structure after generating codes"/></a><br>

> 💡 In case of multiprocessing applications, it is recommended to include generated codes in a separate _static library_ to link with service provider and service consumer applications.

