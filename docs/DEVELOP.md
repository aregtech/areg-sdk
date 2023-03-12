# Developer guide
```
This file is part of AREG SDK
Copyright (c) 2021-2022, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document is a developer guide and describes how to develop a service enabled application.

## Table of content
- [File structure](#file-structure)
- [Service interface prototype](#service-interface-prototype)
- [Public and Local services](#public-and-local-services)
- [Data types](#data-types)
    - [Structures](#structures)
    - [Enumerations](#enumerations)
    - [Imported types](#imported-types)
    - [Containers](#containers)
- [Attributes](#attributes)
- [Requests, responses and broadcasts](#requests-responses-and-broadcasts)
    - [Requests](#requests)
    - [Responses](#responses)
    - [Broadcasts](#broadcasts)
- [Constants](#constants)
- [Includes](#includes)
- [Code generator](#code-generator)
- [Generated codes](#generated-codes)
- [Modeling and service initialization](#modeling-and-service-initialization)
- [Hello Service!](#hello-service)
    - [Service Interface](#service-interface)
    - [Generate codes](#generate-codes)
    - [Develop the service provider](#develop-the-service-provider)
    - [Develop the service client](#develop-the-service-client)
    - [Create and load model](#create-and-load-model)
        - [Model with single thread](#model-with-single-thread)
        - [Model with multiple threads](#model-with-multiple-threads)
        - [Model of separate processes](#model-of-separate-processes)
    - [Make the build](#make-the-build)
- [Testing](#testing)
- [Contact us!](#contact-us)

---

## File structure

Brief description of AREG SDK file structure:
1. The framework of AREG SDK is in the [framework/areg](../framework/areg/) directory.
2. High level subdirectories in [framework/areg](../framework/areg/) have developer-usable classes.
3. All `private` subdirectories in framework contain code for internal use only.
4. The [framework/mcrouter](../framework/mcrouter/) directory is multicast router implementation.
5. The [framework/extensions](../framework/extensions/) directory has extended objects that may need extra libraries.
6. The [examples](../examples/) directory contains demonstration of AREG engine features.
7. The [tests](../tests/] directory contains _Unit Tests_ and later will contain benchmark tests.
8. The [tools](../tools/) directory contains programs like design (in progress) and code generator.
9. The [conf](../conf) directory contains build configuration with `cmake`, `make` and `msbuild`.
10. The [docs](../docs) directory contains documentation.
11. The detailed documentations are in [Wiki](https://github.com/aregtech/areg-sdk/wiki) (in progress).

---

## Service interface prototype

The AREG (_Automated Real-time Event Grid_) is an interface-centric real-time communication engine that enables connected Things to act as ultra-small servers and form a distributed network of services. The AREG engine handles the physical location, discovery, and connection of service providers and clients, making communication automatic and transparent. Services process requests and provide data, while clients receive responses and may dynamically subscribe and unsubscribe to data. The service interface (service API) is defined in an XML prototype document, which includes following sections:
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

Requests are actions executed on the service provider (server) side when called by service consumers (clients). Requests may have no or one linked response sent by service provider as a reply to request. If a request has a response, the processing request is blocked until the service provider replies with the linked response. It is possible to manually unblock the request, but then the response must also be manually prepared for a reply. An example of manually unblocked request can be seen in [this example](../examples/21_pubunblock/). Multiple requests can be linked with the same response. If a request has no response, it is always unblocked and can be called one after another. If a request is busy and blocked, and a client triggers the same request, it is immediately replied with a request failure indicating the reason of failure (in this example _reuqest is busy_). All parameters of the request must be serializable.

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

Responses are replies to requests sent by service providers (servers) to service consumers (clients). Each response is linked to at least one request, and if a response has no request, it is ignored and never triggered. When the server calls a response, it is automatically triggered on the client side as a callback. As well, clients can manually subscribe and unsubscribe from the response without calling a request, particularly if they are interested in the result of an action and not concerned about which component triggered the action. 

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

---

## Modeling and service initialization

Once codes are generated, the developer can incorporate the business logic for the service provider (_server_) and service consumer (_client_). These objects must be a part of the _Component_, running on a dedicated thread. The _Component_ object can either extend the server and/or client base class or aggregate separately implemented service provider and consumer objects. Essentially, the _Component_ should hold the service provider and/or service client to dispatch events effectively.

Once the servicing components are implemented, developers can create a hierarchical _model_ consisting of threads, components, provided and consumed services. This model is static and used to instantiate objects and start services. However, developers can modify the model dynamically before it is loaded.

> 💡 An example of a _dynamic model_ is in the [14_pubtraffic](../examples/14_pubtraffic/) project. 

*An example of defining and loading static model:*
```cpp
// Describe mode, set model name
BEGIN_MODEL( "ServiceModel" )

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread", NECommon::WATCHDOG_IGNORE )
        // define component, set role name.
        BEGIN_REGISTER_COMPONENT( NECommon::ServiceHelloName, ServiceComponent )
            // register HelloService service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( NECommon::ServiceHelloName )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

// end of model description
END_MODEL( "ServiceModel" )

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The main method enables logging, service manager and timer.
 *          It loads and unloads the services, releases application resources.
 **/
int main()
{
    // Initialize application using default settings.
    Application::initApplication( );

    // load model to initialize components
    Application::loadModel("ServiceModel");

    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);

    // stop and unload components
    Application::unloadModel("ServiceModel");

    // release and cleanup resources of application.
    Application::releaseApplication();
	return 0;
}

```

This example creates a _model_ named `"ServiceModel"`, which is loaded in `int main()` function when call `Application::loadModel("ServiceModel")`, and unload when call `Application::unloadModel("ServiceModel")` to stop services and release objects.

The example [10_locsvc](../examples/10_locsvc/) and higher contain creation of models of creating _Local_ and/or _Public_ services. Browse examples for more practical overview.

---

## Hello Service!

> 💡 The source codes of this example are in [00_helloservice](../examples/00_helloservice). 

This topic is a step-by-step training example to create service enabled applications. For simplicity, create new directory inside `areg-sdk/examples` directory and name it `helloservice`, so that the `~/areg-sdk/examples/helloservice` is the root of training projects and it is working directory for 3 sample projects that use same service provider (server) and service client located in `common/src` subdirectory, where:
- In the _project #1_ (`onethread`) the service provider and client run in the same thread of the same process.
- In the _project #2_ (`twothreads`) the service provider and client run in different threads of the same process.
- In the _project #3_ (`multiprocess`) the service provider and client run in different processes.

While in the _project #1 and #2_ the service acts like _Local_ (because client and server are in the same process), in case of _project #3_ it is a _Public_ service and the client / server can run either on the same or different machines. To run _project #3_ we need to start `mcrouter` on any machine within the network. Make sure the [router.init](../framework/mcrouter/resources/router.init) file is properly configured (connection with `mcrouter`) for each executable and it is located in the `config` subdirectory of the executable.

Features demonstrated in this examples:
- Each project creates a model, which is a definition of service provider and service client that can be:
  - in the same thread of the same process;
  - in the different threads of the same process;
  - in different processes.
- Each application loads the model to instantiate service and unloads the model to stop service.
- The service provider automatically registers and the service client automatically discovers the service.
- As soon as service is available for the client, it sends a request to the service provider.
- The service provider processes the request and replies with a response.
- The service client receives a response and initiates _quit_.
  - since in _project #1 and 2_ service provider and client run in same process, application quits after response;
  - in _project #3_ service provider application continues to run and service client application quits after response.
- In case of _project #3_:
  - multiple clients can start in parallel or one after another;
  - one or more clients can start before and/or after service provider starts;
  - no matter applications' starting order, service discovery is automated.

The agenda of this topic is to demonstrate service and client implementation, as well the possibility to split and merge services in processes and threads to distribute computing power. Pay attention that all projects instantiate the same service provider and client objects, the differences are in _models_ declared and used in `main.cpp` files.

> 💡 Important development notes when create service provider and service client:<br>
> 1. The `Component` is the holder of the servicing objects (service provider and/or service client).<br>
> 2. The `Component` can have implementation of multiple service interfaces (multiple service providers).<br>
> 3. The same service provider can be instantiated multiple times, but instances should run in different threads.<br>
> 4. The services differ by service interface name and _role_ names.<br>
> 5. In the _model_ the clients refer to the service by _role_ names.<br>
> 6. One class can extend and mix interfaces of multiple different providers and clients.<br>
> 7. The instances of the same service interface provider and client can be in the same thread.<br>
> 8. The `Component` can contain a mixture of service providers and clients.<br>
> 9. The _role_ names of the _Local_ services must be unique within the same process.<br>
> 10. The _role_ names of the _Public_ services must be unique within the same network.<br>
> 11. The service clients know availability of service via callback `void serviceConnected( bool isConnected, ProxyBase & proxy )`, where `isConnected` indicates service availability.<br>
> 12. The service providers know the client connection via callback `void clientConnected( const ProxyAddress & client, bool isConnected )`, where `isConnected` indicates client connection status.

### Service Interface

In `helloservice` create a `res` folder. Create `HelloService.siml` file service interface file and copy this XML in the file, which defines _Public_ service interface with a request and response.
```xml
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<ServiceInterface FormatVersion="1.0.0">
    <Overview ID="1" Name="HelloService" Version="1.0.0" isRemote="true">
        <Description>The hello world application</Description>
    </Overview>
    <MethodList>
        <Method ID="2" MethodType="request" Name="HelloService" Response="HelloService">
            <Description>The request to output a greeting.</Description>
            <ParamList>
                <Parameter DataType="String" ID="3" Name="client">
                    <Description>The name of client to output the greeting.</Description>
                </Parameter>
            </ParamList>
        </Method>
        <Method ID="4" MethodType="response" Name="HelloService">
            <Description>The response indicating success status to output a greeting</Description>
            <ParamList>
                <Parameter DataType="bool" ID="5" Name="success">
                    <Description>Flag, indicates the success of output.</Description>
                </Parameter>
            </ParamList>
        </Method>
    </MethodList>
</ServiceInterface>
```

### Generate codes

> 💡 You must have Java installed on your machine to be able to run code generator.

Open command line terminal in `helloservice` folder and run following command:
```bash
$ java -jar ../../tools/codegen.jar --root=./ --doc=res/HelloService.siml --target=generated/src
```
This runs `codegen.jar` and generates base files located in the `generated/src` subdirectory of `helloservice`.

> 💡 Generated codes:<br>
> 1. Generated service provider base classes end with `Stub`. In our case it is `HelloServiceStub`.<br>
> 2. Generate service client base classes end with `ClientBase`. In our case it is `HelloServiceClassBase`.<br>
> 3. The `Stub` objects are to extend and implement _requests_.<br>
> 4. The `ClientBase` objects are to extend and implement _responses_, _broadcasts_ and _data update_ notifications.<br>
> 5. For better error handling, the derived from `ClientBase` object should implement _request failed_ methods.

### Develop the service provider

Develop the service provider in the directory `helloservice/common/src` to share in all projects. 

In the `helloservice/common/src` directory create file `ServiceComponent.hpp` to develop `ServiceComponent` object as a service provider component. 
- The `ServiceComponent` extends the `Component` and `HelloServiceStub` classes.
- The `ServiceComponent` contains 2 static methods `Component * CreateComponent( ... )` to instantiate and `void DeleteComponent( ... )` to free the component object.
- The `ServiceComponent` contains the `void requestHelloService( const String & client )` override method.

> 💡 The declaration of the service provider component `ServiceComponent` you can find in the file [common/src/ServiceComponent.hpp](../examples/00_helloservice/common/src/ServiceComponent.hpp).

_Declaration of `ServiceComponent` class:_
```cpp
/**
 * \file    common/src/ServiceComponent.hpp
 * \brief   Declaration of the service component.
 **/
#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/HelloServiceStub.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceComponent declaration
//////////////////////////////////////////////////////////////////////////
class ServiceComponent  : public    Component
                        , protected HelloServiceStub
{
//////////////////////////////////////////////////////////////////////////
// static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called to instantiate the service component when loading the model.
     * \param   entry   Indicates the component description entry from Registry.
     * \param   owner   The component owning thread.
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called when unloading model to delete service component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

protected:
    ServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    virtual ~ServiceComponent(void) = default;

//////////////////////////////////////////////////////////////////////////
// HelloService Interface Requests
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The request to output a greeting.
     * \param   client  The name of the client to output the greeting.
     **/
    virtual void requestHelloService( const String & client ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    //!< The wrapper of this pointer.
    inline ServiceComponent & self( void )
    {   return (*this); }

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceComponent );
};
```

> 💡 The implementation of the service provider component `ServiceComponent` you can find in the file [common/src/ServiceComponent.cpp](../examples/00_helloservice/common/src/ServiceComponent.cpp).

_Implementation of ServiceComponent class:_
```cpp
/**
 * \file    common/src/ServiceComponent.cpp
 * \brief   Implementation of the service component.
 **/
#include "common/src/ServiceComponent.hpp"

Component * ServiceComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServiceComponent(entry, owner);
}

void ServiceComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServiceComponent::ServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( entry, owner )
    , HelloServiceStub  ( static_cast<Component &>(self()) )
{
}

void ServiceComponent::requestHelloService(const String & client)
{
    // output message
    printf("\'Hello Service!\' from %s\n", client.getString());

    // reply to unblock the request
    responseHelloService(true);
}
```
In this example:
- The class `ServiceComponent` is an instance of `Component` and `HelloServiceStub`.
- The service provider is created in the **static** method `Component * ServiceComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)`.
- The service provider is deleted in the **static** method `void ServiceComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)`.
- The service provider implements virtual method `void requestHelloService(const String & client)` inherited from `HelloServiceStub`.
- In the request `requestHelloService` replies `responseHelloService` to the client and unblocks the request.

### Develop the service client

Develop the service client in the directory `helloservice/common/src` to share in all projects. 

In the `helloservice/common/src` subfolder create file `ClientComponent.hpp` to develop the `ClientComponent` object as a service client component.
- The `ClientComponent` extends `Component` and `HelloServiceClientBase` classes.
- The `ClientComponent` contains 2 static methods `Component * CreateComponent( ... )` to create and `void DeleteComponent( ... )` to free the component object.
- The method `void serviceConnected( bool isConnected, ProxyBase & proxy )` is overwritten to know the service available status to start communication.
- The `ClientComponent` contains overrides to handle response and request failure.

> 💡 The declaration of the service client component `ClientComponent` is in the file [common/src/ClientComponent.hpp](../examples/00_helloservice/common/src/ClientComponent.hpp)

_Declaration of `ClientComponent` class:_
```cpp
/**
 * \file    common/src/ClientComponent.hpp
 * \brief   Declaration of the service client component.
 **/
#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/HelloServiceClientBase.hpp"

//////////////////////////////////////////////////////////////////////////
// ClientComponent declaration
//////////////////////////////////////////////////////////////////////////
class ClientComponent   : public    Component
                        , protected HelloServiceClientBase
{
//////////////////////////////////////////////////////////////////////////
// static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called to instantiate the service component when loading the model.
     * \param   entry   Indicates the component description entry from Registry.
     * \param   owner   The component owning thread.
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called when unloading model to delete service component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

protected:
    ClientComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    virtual ~ClientComponent(void) = default;

/************************************************************************
 * Response HelloService
 ************************************************************************/
    /**
     * \brief   The response indicating success status to output a greeting
     * \param   success Flag, indicates the success of output.
     **/
    virtual void responseHelloService( bool success ) override;

    /**
     * \brief   Overwrite to handle error of HelloService request call.
     * \param   FailureReason   The failure reason value of request call.
     **/
    virtual void requestHelloServiceFailed( NEService::eResultType FailureReason ) override;

/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered when gets service connected / disconnected event.
     *          Client should be initialized and the listeners should be setup
     *          here. No request can be called, while service is not connected.
     * \param   isConnected     Indicates service connection status.
     * \param   proxy           The Service Interface Proxy object.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    //!< The wrapper of this pointer.
    inline ClientComponent & self( void )
    {   return (*this); }

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ClientComponent );
};
```

> 💡 The implementation of the service client component `ClientComponent` is in the file  [common/src/ClientComponent.cpp](../examples/00_helloservice/common/src/ClientComponent.cpp).

_Implementation of ClientComponent class:_

```cpp
/**
 * \file    common/src/ClientComponent.hpp
 * \brief   Implementation of the service client component.
 **/
#include "common/src/ClientComponent.hpp"

#include "areg/base/Thread.hpp"
#include "areg/base/NECommon.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

Component * ClientComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ClientComponent(entry, owner);
}

void ClientComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ClientComponent::ClientComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( entry, owner )
    , HelloServiceClientBase( entry.mDependencyServices[0].mRoleName.getString(), owner )
{
}

bool ClientComponent::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    bool result = false;
    if ( HelloServiceClientBase::serviceConnected(isConnected, proxy) )
    {
        if (isConnected)
        {
            // Up from this part the client can:
            //      a. call requests to run on the server side.
            //      b. subscribe on data update notification
            //      c. subscribe on broadcasts and responses.

            // call request to run on server side.
            requestHelloService( getRoleName() );
        }
        else
        {
            // No connection, make cleanups, release subscription here, signal to quit application.
            Application::signalAppQuit();
        }
    }

    return result;
}

void ClientComponent::responseHelloService( bool success )
{
    printf("%s to output message.\n", success ? "succeeded" : "failed");

    // Sleep for no reason! Do not do this in a real application.
    // It is done to give a chance to see an output message on the console.
    // Otherwise, the next line of code closes the application and you miss the message on console.
    Thread::sleep(NECommon::WAIT_1_SECOND);

    // The client completed the job, set signal to quit application
    Application::signalAppQuit();
}

void ClientComponent::requestHelloServiceFailed(NEService::eResultType FailureReason)
{
    // make error handling here.
    printf("Failed to execute request, retry again.\n");
    if (isConnected())
    {
        // the service is still connected, and can resend the request.
        requestHelloService( getRoleName() );
    }
}
```
In this example:
- The `ServiceClient` is an instance of `Component` and `HelloServiceClientBase`.
- The service client is created in the **static** method `Component * ServiceClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)`.
- The service client is deleted in the **static** method `void ServiceClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)`.
- The service client overrides virtual method `bool serviceConnected(bool isConnected, ProxyBase & proxy)` to react on service connect / disconnect events. It calls the request when service is connected and initiates application quit when service is disconnected.
* The service client overrides virtual method `void responseHelloService( bool success )` to react on request reply. It waits 1 second and initiates application to quit.
* The service client overrides virtual method `requestHelloServiceFailed(NEService::eResultType FailureReason)` to make error handling.

Now there is a service provider and service client available, and we can start creating applications using generated codes and using developed classes.

### Create and load model

The developers can decide how to distribute service and client objects. The following examples are applications where:
- The service and the client run in the same thread of the same process.
- The service and the client run in the separate threads of the same process.
- The service and the client run in separate processes (_Public_ service case).

Each application is created in a separate project. The distribution of service provider and client is defined in the _model_. In the examples, the macro:
- `BEGIN_MODEL` is a declaration of the model with the name. There can be multiple models in the application.
- `BEGIN_REGISTER_THREAD` is a declaration of component thread with name and watchdog timeout (value `NECommon::WATCHDOG_IGNORE` ignores the watchdog). Service providers and service clients run in a _component thread_.
- `BEGIN_REGISTER_COMPONENT` is a declaration of servicing component with _role_ name and the class name.
- `REGISTER_IMPLEMENT_SERVICE` is declaration of provided service interface. Each provided service must be listed separately in `REGISTER_IMPLEMENT_SERVICE` macro.
- `REGISTER_DEPENDENCY` is a declaration of service client, which is dependent on the service provider. The dependency is not the service interface name, but the _role_ name of the servicing component where the service provider belongs. Each dependency must be listed separately in `REGISTER_DEPENDENCY` macro.

#### Model with single thread

> 💡 The sources of this example are available in the file [./onethread/src/main.cpp](../examples/00_helloservice/onethread/src/main.cpp).

*An example to demonstrate design of a model where service provider and service client run in the same thread:*
- Create `onethread/src` subdirectory in the `helloservice`
- Create the `main.cpp` file to design the _model_ and implement `main()` function.

```cpp
/**
 * \file    onethread/src/main.cpp
 * \brief   Runs service and the client in one thread.
 **/
#include "areg/base/GEGlobal.h"
#include "areg/base/NEUtilities.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

#include <string>

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef WINDOWS
    #pragma comment(lib, "areg")
    #pragma comment(lib, "00_generated.lib")
#endif // WINDOWS

//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };

// Describe model, register the service and the client in one thread "Thread1"
BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD( "Thread1", NECommon::WATCHDOG_IGNORE )
        // register service in the thread
        BEGIN_REGISTER_COMPONENT("ServiceComponent", ServiceComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceComponent" )
        
        // register client in the same thread
        BEGIN_REGISTER_COMPONENT( "ServiceClient", ClientComponent )
            REGISTER_DEPENDENCY( "ServiceComponent" ) /* reference to the service*/
        END_REGISTER_COMPONENT( "ServiceClient" )
    END_REGISTER_THREAD( "Thread1" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////

int main( void )
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    // load model to initialize components
    Application::loadModel(_modelName);

    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);

    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

    return 0;
}
```
In this example, it is designed _model_: 
- a model named `"ServiceModel"`;
- a thread named `"Thread1"` and no watchdog;
  - a component `ServiceComponent` with _role_ name `"ServiceComponent"`;
    - a declaration of provided `NEHelloService::ServiceName` service interface;
  - a component `ServiceClient` with _role_ name `"ServiceClient"`;
    - a declaration of dependency on service `"ServiceComponent"`.

In the `main()` function:
1. The application initializes internals `Application::initApplication( )`.
2. The loads model to start service in `Application::loadModel(_modelName)`.
3. The application waits for a _quit_ signal by calling `Application::waitAppQuit(NECommon::WAIT_INFINITE)`.
4. The application unloads model and stops service in `Application::unloadModel(_modelName)`.
5. The application releases internals in `Application::releaseApplication()` and exits with result 0.

#### Model with multiple threads

> 💡 The sources of this example are available in the file [./twothreads/src/main.cpp](../examples/00_helloservice/twothreads/src/main.cpp).

*An example to demonstrate design of a model where service provider and service client run in the separate threads:*
- Create `twothreads/src` subdirectory in the `helloservice`
- Create the `main.cpp` file to design the _model_ and implement `main()` function.

```cpp
/**
 * \file    twothreads/src/main.cpp
 * \brief   Runs service and the client in one thread.
 **/
#include "areg/base/GEGlobal.h"
#include "areg/base/NEUtilities.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

#include <string>

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef WINDOWS
    #pragma comment(lib, "areg")
    #pragma comment(lib, "00_generated.lib")
#endif // WINDOWS

//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };

// Describe model, register the service and the client in 2 different threads "Thread1" and "Thread2"
BEGIN_MODEL(_modelName)
    // Thread 1, provides a service
    BEGIN_REGISTER_THREAD( "Thread1", NECommon::WATCHDOG_IGNORE )
        BEGIN_REGISTER_COMPONENT( "ServiceComponent", ServiceComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceComponent" )
    END_REGISTER_THREAD( "Thread1" )

    // Thread 2, is a service client.
    BEGIN_REGISTER_THREAD( "Thread2", NECommon::WATCHDOG_IGNORE )
        BEGIN_REGISTER_COMPONENT( "ServiceClient", ClientComponent )
            REGISTER_DEPENDENCY( "ServiceComponent" ) /* reference to the service*/
        END_REGISTER_COMPONENT( "ServiceClient" )
    END_REGISTER_THREAD( "Thread2" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////

int main( void )
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    // load model to initialize components
    Application::loadModel(_modelName);

    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);

    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

    return 0;
}
```
In this example, it is designed _model_: 
- a model named `"ServiceModel"`;
- a thread named `"Thread1"` and no watchdog;
  - a component `ServiceComponent` with _role_ name `"ServiceComponent"`;
    - a declaration of provided `NEHelloService::ServiceName` service interface;
- a thread named `"Thread2"` and no watchdog;
  - a component `ServiceClient` with _role_ name `"ServiceClient"`;
    - a declaration of dependency on service `"ServiceComponent"`.

Same as in previous example, in the `main()` function:
1. The application initializes internals `Application::initApplication( )`.
2. The loads model to start service in `Application::loadModel(_modelName)`.
3. The application waits for a _quit_ signal by calling `Application::waitAppQuit(NECommon::WAIT_INFINITE)`.
4. The application unloads model and stops service in `Application::unloadModel(_modelName)`.
5. The application releases internals in `Application::releaseApplication()` and exits with result 0.

#### Model with separate processes

> 💡 To be able to create _Public_ service (i.e. to support **IPC**), the service interface must be declared as _Public_ by defining `isRemote="true"` attribute of `Overview` entry in the XML document as it is defined in [HelloService.siml](../examples/00_helloservice/res/HelloService.siml) file (see `<Overview ID="1" Name="HelloService" Version="1.0.0" isRemote="true">` in line 3).

This example requires 2 processes to demonstrate _Public_ service, where one process provides and the other process uses the service. It requires one project per process. Create empty `main.cpp` source file in `multiprocess\serviceproc\src` and `multiprocess\clientproc\src` subdirectory of the `helloservice`. 

> 💡 To run this example, start `mcrouter` process that will connect server and client nodes.

**Service provider process**

> 💡 The sources of this example are available in the file [./multiprocess/serviceproc/src/main.cpp](../examples/00_helloservice/multiprocess/serviceproc/src/main.cpp). It provides _Public_ service.

*An example to demonstrate design of a model where service provider run in the separate process:*
- Create `multiprocess\serviceproc\src` subdirectory in the `helloservice`
- Create the `main.cpp` file to design the _model_ and implement `main()` function.

```cpp
/**
 * \file    multiprocess/serviceproc/src/main.cpp
 * \brief   Runs service in the process.
 **/
#include "areg/base/GEGlobal.h"
#include "areg/base/NEUtilities.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"

#include <string>

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef WINDOWS
    #pragma comment(lib, "areg")
    #pragma comment(lib, "00_generated.lib")
#endif // WINDOWS

//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };

// Describe model, register the provided service in this model
BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD( "Thread1", NECommon::WATCHDOG_IGNORE )
        BEGIN_REGISTER_COMPONENT( "ServiceComponent", ServiceComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceComponent" )
    END_REGISTER_THREAD( "Thread1" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////

int main( void )
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    // load model to initialize components
    Application::loadModel(_modelName);

    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);

    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

    return 0;
}
```

In this example, it is designed _model_: 
- a model named `"ServiceModel"`;
- a thread named `"Thread1"` and no watchdog;
  - a component `ServiceComponent` with _role_ name `"ServiceComponent"`;
    - a declaration of provided `NEHelloService::ServiceName` service interface;

Same as in previous examples, in the `main()` function:
1. The application initializes internals `Application::initApplication( )`.
2. The loads model to start service in `Application::loadModel(_modelName)`.
3. The application waits for a _quit_ signal by calling `Application::waitAppQuit(NECommon::WAIT_INFINITE)`.
4. The application unloads model and stops service in `Application::unloadModel(_modelName)`.
5. The application releases internals in `Application::releaseApplication()` and exits with result 0.

**Service client process**

> 💡 The sources of this example are available in the file [./multiprocess/clientproc/src/main.cpp](../examples/00_helloservice/multiprocess/clientproc/src/main.cpp). It uses _Public_ service.

*An example to demonstrate design of a model where service provider run in the separate process:*
- Create `multiprocess\clientproc\src` subdirectory in the `helloservice`
- Create the `main.cpp` file to design the _model_ and implement `main()` function.

```cpp
/**
 * \file    multiprocess/clientproc/src/main.hpp
 * \brief   Runs service and the client in one thread.
 **/

#include "areg/base/GEGlobal.h"
#include "areg/base/NEUtilities.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ClientComponent.hpp"

#include <string>

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef WINDOWS
    #pragma comment(lib, "areg")
    #pragma comment(lib, "00_generated.lib")
#endif // WINDOWS

//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };
//!< Client component name. Let's generate the name for client service, we'll use it later.
const std::string   _client(NEUtilities::generateName("ServiceClient").getString());

// Describe model, register the service consumer (client)
BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD( "Thread1", NECommon::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT( _client.c_str(), ClientComponent )
            REGISTER_DEPENDENCY( "ServiceComponent" ) /* reference to the service*/
        END_REGISTER_COMPONENT( _client )
    END_REGISTER_THREAD( "Thread1" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////

int main( void )
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    // load model to initialize components
    Application::loadModel(_modelName);

    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);

    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

    return 0;
}
```

In this example, it is designed _model_: 
- a model named `"ServiceModel"`;
- a thread named `"Thread1"` and no watchdog;
  - a component `ServiceClient` with dynamic _role_ name, which starts with `"ServiceClient"`;
  - a declaration of dependency on service `"ServiceComponent"`.

Same as in previous examples, in the `main()` function:
1. The application initializes internals `Application::initApplication( )`.
2. The loads model to start service in `Application::loadModel(_modelName)`.
3. The application waits for a _quit_ signal by calling `Application::waitAppQuit(NECommon::WAIT_INFINITE)`.
4. The application unloads model and stops service in `Application::unloadModel(_modelName)`.
5. The application releases internals in `Application::releaseApplication()` and exits with result 0.

> 💡 In this particular project, the _role_ name of client component has dynamic name, which is changing on each start. The declaration `const std::string   _client( NEUtilities::generateName("ServiceClient").getString() )` generates a name with timestamp, so that each new application starts and generates a new name. This will help to run  multiple instances of client application in parallel or one after another.

### Make the build

The created projects can be built with the help of `MS Visual Studio`, `Eclipse`, `Make` command line tool, `CMake` command line tool, etc. Here it is not considered creating projects for MS Visual Studio and Eclipse. The appropriate project files for the mentioned IDE can be found in each project folder of [00_helloservice](../examples/00_helloservice). Here we consider creating `Makefiles` and compile projects.

*The following folders should have `Makefile` file:*
```
helloservice/Makefile

helloservice/generated/Makefile
helloservice/generated/src/Makefile

helloservice/multiprocess/clientproc/Makefile
helloservice/multiprocess/clientproc/src/Makefile

helloservice/multiprocess/serviceproc/Makefile
helloservice/multiprocess/serviceproc/src/Makefile

helloservice/onethread/Makefile
helloservice/onethread/src/Makefile

helloservice/twothreads/Makefile
helloservice/twothreads/src/Makefile
```

We use `make` default settings of `areg-sdk` to build the projects. To avoid flooding we'll not go deep in `Makefile` and demonstrate only one case. For more details, see identical content of every `Makefile` in the [00_helloservice](../examples/00_helloservice) and make a copy by removing _"00_"_ prefix:

```shell
helloservice_BASE       := $(AREG_EXAMPLES)/helloservice
helloservice_OUTPUT_OBJ := $(AREG_OUTPUT_OBJ)/helloservice

helloservice_CXXFLAGS   = -I$(helloservice_BASE) $(examples_CXXFLAGS)
helloservice_LDFLAGS    = -L $(AREG_OUTPUT_LIB) -Wl,-Bstatic -l$(generated_PROJECT_NAME) -Wl,-Bdynamic $(examples_LDFLAGS)

# 'generated' have to be first
include $(helloservice_BASE)/generated/Makefile
include $(helloservice_BASE)/onethread/Makefile
include $(helloservice_BASE)/twothreads/Makefile
include $(helloservice_BASE)/multiprocess/serviceproc/Makefile
include $(helloservice_BASE)/multiprocess/clientproc/Makefile

helloservice: $(AREG_OUTPUT_BIN)/$(onethread_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(twothreads_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(serviceproc_TARGET_BIN) $(AREG_OUTPUT_BIN)/$(clientproc_TARGET_BIN)

.PHONY: helloservice
```

When ready, include new `helloservice` projects of in the [examples/Makefile](../examples/Makefile):

**Step #1:** Include `Makefile` of `helloservice`:
```shell
include $(AREG_EXAMPLES)/helloservice/Makefile
```
**Step #2:** Include `helloservice` in build (project added at the end):
```shell
examples: 00_helloservice 01_hello 02_buffer 03_file 04_trace   \
    05_timer 06_threads 07_synch 08_service 09_svcmulti         \
    10_locsvc 11_locmesh 12_pubsvc 13_pubmesh                   \
    14_pubtraffic 15_pubworker 16_pubfsm helloservice
```
Now the projects are ready, open the Terminal in _areg-sdk_ root folder and call:
```shell
$ make examples
```
or if build all projects, call:
```shell
$make
```

The compiled binaries are located in `<areg-sdk>/product/build/<compiler-platform-path>/bin`.

## Testing

> 💡 More testing examples are listed in the [examples](../examples/) folder of `areg-sdk`.

1. To compile all projects, call `make all` in command line.
2. To compile example projects, call `make examples` in the command line.
3. All compiled files are located in `<areg-sdk>/product/build/<compiler-platform-path>/bin` folder.
4. If running an application with a _Public_ service (**IPC** projects), start `mcrouter`.
5. There is no need to start the _mcrouter_ if testing an application with no _Public_ service.
6. All compiled files are in the `bin` folder, switch there to start applications.
7. Some applications may create logs in the `logs` folder of binary location. Open to analyze logs.

## Contact us!

Contact us at info[at]aregtech.com if you need help, training or more information.
If you linked the project and the idea behind that, please ![star the AREG SDK project](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=star%20the%20AREG%20SDK%20project) at GitHub, so that contributors feel appreciated for the job.
