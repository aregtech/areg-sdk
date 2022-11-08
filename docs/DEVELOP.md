# Developer guide
```
This file is part of AREG SDK
Copyright (c) 2021-2022, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document is a developer guide and describes how to develop a service enabled application.

## Table of content
- [Developer guide](#developer-guide)
  - [Table of content](#table-of-content)
  - [File structure](#file-structure)
  - [Service interface prototype](#service-interface-prototype)
    - [Public and Local services](#public-and-local-services)
    - [Data types](#data-types)
      - [Structures](#structures)
      - [Enumerations](#enumerations)
      - [Imported types](#imported-types)
      - [Defined containers](#defined-containers)
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

## File structure

Brief description of AREG SDK file structure:
1. The framework of AREG SDK is in the [framework/areg](../framework/areg) directory.
2. All high level subdirectories in [framework/areg](../framework/areg) contain classes that can be used by developers.
3. All subdirectories named `private` contain code implementations and class declarations that are for internal use and not designed to be used outside of AREG framework.
4. The multicast router of AREG SDK is in the [framework/mcrouter](../framework/mcrouter) directory.
5. The examples to demonstrate AREG SDK features are in the [examples](../examples) directory.
6. The tools to use are declared in the [tools](../tools) directory.
7. The configuration files to customize compilation are in [conf](../conf) directory.
8. The documentation is in the [docs](../docs) directory.

Some more information about file structure is in [README.md](./readme.md).

## Service interface prototype

AREG SDK is an interface-centric real-time communication engine where connected Things provide services, so that they act as ultra-small servers and automatically form a network of distributed services. The service providers (servers) and service clients are software components, whose physical location, discovery and connection are transparent, automated and fully handled by the AREG framework. The services provide data and process requests. The service clients receive responses as a reply on requests, and can dynamically subscribe and unsubscribe on service data. The data, request and responses are defined as service interface (service API), which can be defined in XML format _service interface_ prototype document, and has following sections:
1. The service interface overview.
2. The service specific data types.
3. The service specific data is called _service attributes_.
4. The service specific methods, which consists of _requests_, _responses_ and _broadcasts_.
5. The service specific constants.
6. The includes required to develop the service.

The service interfaces defined in prototype XML document is used to generate servicing base objects, which developers can extend to implement service specific functionalities. An example of the _service interface_ prototype with sections is described in the [Sample.siml](./Sample.siml) file.

> 💡 The Service Interface design GUI tool is in development state and will be available in next releases as a free-ware.

### Public and Local services

The following example defines a _Public_ (used for IPC) interface defined for _Sample_ service.
```xml
<Overview ID="1" Name="Sample" Version="1.0.0" isRemote="true">
    <Description>This is an example of defining service interface.</Description>
</Overview>
```
The `<Overview>` section contains service interface name, interface version and the flag, indicating whether the interface is _Public_ or _Local_. If the `isRemote` attribute is `true` (`isRemote="true"`) it is a _Public_ service interface, visible and accessible in the network, and can be used by any node in the network. Otherwise, it is _Local_ and not visible in the network. The _Local_ services are used to protect sensitive data and enable multithreading asynchronous communication in the application. The _Public_ services are used for multiprocessing communication and they enable connected Things to act as ultra-small servers.

> 💡 If an API of the _Local_ service interface is called outside of the process, the system ignores the call and responds with error code to the caller. Only _Public_ interfaces are accessed for _IPC_ and can be called from any client software component in the network.

### Data types

Every service interface can have specific data types. When a new data type is defined, it can be used to declare any variables, service data (attributes) and parameters of methods. New data types are listed in the section `<DataTypeList>`. All data types should be streamable. It is possible to import external data types in the service interface and there should be `operator >>`or `operator <<` implemented for the imported types to serialize data in the [IEIOStream](../framework/areg/base/IEIOStream.hpp).

#### Structures

In the `<DataTypeList>` section of the service interface XML document can be declared a structure. The `DataType` tag indicates type `Structure` and the name of the structure followed by the list of structure fields. Each field refers to another predefined or new defined `DataType` and has a name. The field of structure may have default value, which is specified in the `<Value>` entry (`<Value IsDefault="true">0</Value>`). Each type of the field must have _assigning operator_, _comparing operator_, and _default and copy constructor_ if the type is an object. The field must be possible explicitly to convert to `unsigned int` and must be possible to serialize in the [IEIOStream](../framework/areg/base/IEIOStream.hpp). There is no need manually to implement operators if the type of the fields are primitives, predefined objects or types defined in the documents. Only imported types may need to have manually implemented operators.

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
In this example, the structure `SomeStruct` has 3 fields with default values to set.

#### Enumerations

In the `<DataTypeList>` section of the service interface XML document can be declared an enumeration. The `DataType` tag indicates type `Enumerate` and the name of the enumeration followed by the list of fields. Each field may have `Value`. The enumerations are automatically streamable.

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
In this example, the enumeration `SomeEnum` has 4 fields, where the first 2 fields have values.

 #### Imported types

In the `<DataTypeList>` section of the service interface XML document can import external types. The `DataType` tag indicates type `Imported` and the name of the new type followed by _Namespace_ (if there is a need) and the relative path to the header file where the imported type is declared. The imported types must be possible to serialize in the [IEIOStream](../framework/areg/base/IEIOStream.hpp), they should have assigning and comparing operators, default and copy constructors.

*An example of imported type:*
 ```xml
<DataType ID="11" Name="uAlign" Type="Imported">
    <Description>This example exports NEMemory::uAlign in the service interface.</Description>
    <Namespace>NEMemory</Namespace>
    <Location>areg/base/NEMemory.hpp</Location>
</DataType>
```
The example imports type `NEMemory::uAlign`, which is declared in `areg/base/NEMemory.hpp` header file and it is streamable.

#### Defined containers

In the `<DataTypeList>` section of the service interface XML document can be declared new types of standard containers. The `DataType` tag indicates type `DefinedType` and the name of the new type. The `Container` tag specifies the type, the `BaseTypeValue` specifies the type of values and in case of _maps_ the `BaseTypeKey` specifies the type of keys. All types must be possible to `stream` in the [IEIOStream](../framework/areg/base/IEIOStream.hpp).

*An example of defining new type of array:*
```xml
<DataType ID="12" Name="SomeArray" Type="DefinedType">
    <Description>Defines new type of array</Description>
    <Container>Array</Container>
    <BaseTypeValue>uint32</BaseTypeValue>
</DataType>
```
In this example, the values of the array have type `unsigned int`, which is streamable.

*An example of defining new type of linked list:*
```xml
<DataType ID="13" Name="SomeList" Type="DefinedType">
    <Description>New type of linked list.</Description>
    <Container>LinkedList</Container>
    <BaseTypeValue>String</BaseTypeValue>
</DataType>
```
In this example, the values of the linked list have type `String`, which is streamable.

*An example of defining new type of hash map:*
```xml
<DataType ID="14" Name="SomeMap" Type="DefinedType">
    <Description>This example defines hash-map where key is exported and value is new data type.</Description>
    <Container>HashMap</Container>
    <BaseTypeValue>SomeStruct</BaseTypeValue>
    <BaseTypeKey>String</BaseTypeKey>
</DataType>
```
In this example, the values of the hash-map have type `SomeStruct` and the key is type `String`. Both types are streamable. The type `SomeStruct` declared in this Service Interface XML document is automatically declared as streamable, has assigning and compare operators, and has default and copy constructor.

### Attributes

_Attributes_ in services are data, which is similar to Publisher-Subscriber (PubSub / data centric solution). The clients may dynamically subscribe and unsubscribe on data updates and get notifications. The attributes are listed in the section `<AttributeList>`. The type of attribute must be possible to serialize in the [IEIOStream](../framework/areg/base/IEIOStream.hpp).

> 💡 The data of an `Attribute` is always available for the clients. As soon as the client subscribes to the attribute data, it receives at least one notification with the current cached value and a flag of validation state. If data was not cached or invalidated, the first notification may indicate _invalid data_ state, then shortly receive a second notification directly from the server indicating the actual value and data state.<br>
> 💡 It is important to check the data state to make correct reaction. The attribute update callback has semantic `void onAttributeNameUpdate( const DataType & AttributeName, NEService::eDataStateType state )`, where _AttributeName_ is the name and _DataType_ is the type of the attribute. The attribute data is valid only if the `state` parameter is `NEService::eDataStateType::DataIsOK`. In all other cases it should be either ignored or the error should be handled.<br>

*An example of declaring attribute to notify only on value change:*
```xml
<Attribute DataType="SomeEnum" ID="15" Name="SomeAttr1" Notify="OnChange">
    <Description>An attribute to notify subscribers only when value is changed.</Description>
</Attribute>
```
In this example the attribute `SomeAttr1` has type `SomeEnum` and it is designed to notify only if the value is changed.

*An example of declaring attribute to notify when value set:*
```xml
<Attribute DataType="SomeStruct" ID="16" Name="SomeAttr2" Notify="Always">
    <Description>Another attribute to notify subscribers any time when value is set (maybe not changed).</Description>
</Attribute>
```
In this example the attribute `SomeAttr2` has type `SomeSruct` and it is designed to notify each time when value is set.

The difference of _OnChange_ and _Always_ notifications is that in one case the notification is sent when value is changed, i.e. **it is compares stored and new values, and notifies if values are not equal**, and on second case the notification is sent whenever value is set, i.e. **it does not compare stored and new values, and notifies immediately when value is set**.

### Requests, responses and broadcasts
_Requests_ and _Responses_ in services are methods, which is similar to Request-Reply (action centric solution). The clients send requests to process on server component and as a reply receive response. In addition, there are _Broadcasts_, which act as methods and have the logic of PubSub, where clients dynamically subscribe to the event to receive a _Broadcast_. The _Broadcasts_ act as events, are triggered as callbacks that can deliver multiple data as parameters. _Requests_, _Responses_ and _Broadcasts_ are defined in the `<MethodList>` section of service interface XML document.

#### Requests
The _Requests_ are called by clients to execute on the service provider (server) side. The requests may have parameters. The requests may have a linked response. If a request has a response, then the processing request is blocked until service replies with the linked response. It is possible manually to unblock the request, but then the response must be manually prepared to reply. Multiple requests can be linked with the same response. The request may have no response and in this case it is always unblocked and can be called one after another.

> 💡 When developers implement service provider (server) they extend the generated `Stub` object and implement request calls, which are declared in a base `Stub` class as _pure virtual functions_.<br>
> 💡 If request fails (for example, it is in busy state), the client receives failure notification. For proper error handling, the clients should override and implement failure callbacks of all used requests. The error code is passed as a parameter in request failure callback. If request failure happens and the developer does not override the failure callback, automatically an error message is logged.

*An example to demonstrate a request with no parameter linked with a response:*
```xml
<Method ID="17" MethodType="request" Name="SomeRequest1" Response="SomeResponse1">
    <Description>Request and response with no parameters.</Description>
</Method>
<Method ID="19" MethodType="response" Name="SomeResponse1">
    <Description/>
</Method>
```
In this example, the request `SomeRequest1` has no parameter and it is linked with the response `SomeResponse1`. The request is blocking and it is unblocked when response `SomeResponse1` is called, so that the next call of `SomeRequest1` can be processed. In the generated C++ code, the request has semantic `void requestSomeRequest1()`.

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
In this example, the request `SomeRequest2` has 3 parameters, where the last parameter `param3` has default value, and the request is linked with response `SomeResponse` with 1 parameter. The request is blocking and it is unblocked when response `SomeResponse` is called, so the next call of `SomeRequest1` can be processed. In the generated C++ code, the request has semantic `void requestSomeRequest2(int param1, const NESample::SomeStruct & param2, NESample::SomeEnum param3 = NESample::SomeEnum::Nothing)`.

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

In this example, 2 different requests `SomeRequest2` and `SomeRequest3` are linked with response `SomeResponse`. Here one request does not block the other while it is processing (i.e. in a _busy_ state). For example, when `SomeRequest2` is called and it is in _busy_ state, the `SomeRequest3` can be also called, and when servicing component replies with the response `SomeResponse`, it automatically unblocks both requests, because both requests are linked with the same response. In other words, the call of response unblocks all linked requests in a _busy_ state. In the generated C++ code, the request `SomeRequest2` has semantic `void requestSomeRequest2(int param1, const NESample::SomeStruct & param2, NESample::SomeEnum param3 = NESample::SomeEnum::Nothing)`, and the request `SomeRequest3` has semantic `void requestSomeRequest3(const NEMemory::uAlign & param)`.

*An example to demonstrate a request with no response:*
```xml
<Method ID="27" MethodType="request" Name="StandAlone">
    <Description>A request with no response.</Description>
</Method>
```
In this example, the request `StandAlone` has no response. It means when a request is processed, the service provider can continue receiving further calls of the request `StandAlone`. In the generated C++ code, the request `StandAlone` has semantic `void requestStandAlone()`.

#### Responses
The _Responses_ are replies to request calls sent from service providers (servers) to the clients. Each response is linked at least with one request. If a response has no request, it is ignored and never triggered. When a response is called on the service provider side, it is automatically triggered on the client as a callback. Any client can manually subscribe and unsubscribe on the response without calling a request. This normally happens if a client is interested only in the reply to a certain request. For example, when the client is interested in the result of a certain action and not interested which component triggered the action.

> 💡 The responses are virtual callbacks triggered by system when appropriate reply received from service provider (server). The developers need to extend the generated `ClientBase` object and implement those responses, which request is called or is manually subscribed. For example, if a client does not call the request `SomeRequest1` and is not manually subscribed on the response `SomeResponse1`, the response `SomeResponse1` is never triggered for that client, so that there is no need to override the default implementation in `ClientBase` class. Otherwise, the client should implement the response.

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
In this example, the responses are linked with the requests (see chapter [Requests](#requests)), where the response `SomeResponse1` is connected with one request and the response `SomeResponse` is linked with 2 requests. The semantic of the response `SomeResponse1` is `void responseSomeResponse1()` and the semantic of response `SomeResponse` is `void responseSomeResponse(bool succeeded)`.

> 💡 When request is processing, it is in _busy_ state and it is blocked. The call of response automatically unblocks all linked requests. It is very important to send the response after processing request(s). Otherwise, while the request is _busy_ and any client calls the same request, the system generates a _request busy_ response message and delivers it to the client. So that, if the response of a request is not called, the request is processed only once (for first call), where all other calls are rejected by the system.<br>
> 💡 If a request does not have linked response, it is never blocked and it is never in _busy_ state.<br>
> 💡 Clients can dynamically subscribe to a certain response without calling linked request. In this case, each time the service provider replies with a response message, it is called not only on the request caller side, but also on the subscribed client side.

#### Broadcasts
The _Broadcasts_ are special service methods to fire an event and pass several data at the same time. Broadcasts have no linked request and act like events fired by service provider, which may deliver multiple data. During runtime the clients need manually to subscribe for the broadcast to receive.

> 💡 Unlike _Attribute_, the _Broadcast_ data is available only during call. It means that if a client subscribes to the broadcast, it receives notification only when the service provider calls broadcast. In case of _Attribute_, the client receives first cached data, then the actual data, even if it was not changed.

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
In this example, the broadcast `SomeBroadcast` has 4 parameters to deliver together subscribed clients. The semantic of this broadcast is `void broadcastSomeBroadcast(NESample::SomeEnum value1, const NEMemory::uAlign & value2, const NESample::SomeStruct & value3, NESample::SomeArray value4)`.

> 💡 In the service interface XML document the request, response and the broadcast may have same name.<br>
> 💡 It is recommended that at all **parameter** names at least in _responses_ and _broadcasts_ have unique names. If any parameter name is not unique, then it must have the same type in all methods. For example, if a broadcast and a response have a parameter named `success`, that parameter should be of the same type, and if `success` in the response has type `bool`, it should not be declared in the broadcast as `int`.

### Constants
The Service Interface XML document may have specific constants listed in `<ConstantList>` section. The constants are used to share read-only value between clients and the service provider.

*An example to demonstrate declaration of a constant:*
```xml
<Constant DataType="uint16" ID="35" Name="SomeConst">
    <Value>100</Value>
    <Description>Define a constant if need.</Description>
</Constant>
```
In this example, it is declared a constant named `SomeConst` having type `unsigned short` and value `100`. This value is shared between the service provider and clients.

### Includes
The Service Interface XML document may have a special include in the code. For example, a service may include a special header file with the implementation of algorithms. The includes are listed in the `<IncludeList>` section.

*An example to demonstrate declaration of includes:*
```xml
<IncludeList>
    <Location ID="36" Name="areg/base/NEMath.hpp">
        <Description>can make additional includes</Description>
    </Location>
</IncludeList>
```
In this example, after generating code, the header file `areg/base/NEMath.hpp` is included for service providers and service client codes.

## Code generator
To avoid tedious jobs and minimize mistakes when coding, AREG SDK provides a code-generator located in the [tools](../tools/) folder to generate base objects using definitions in the Service Interface XML document. The generated files must not be modified. Instead, they should be extended to implement all necessary functions. Normally, the service providers implement _Request_ methods and the service clients implement _Response_, _Broadcast_ and _Attribute update_ notification methods by need. Edit and run [generate.sh](../tools/generate.sh) or [generate.bat](../tools/generate.bat) file to generate source code or run `codegen.jar` from command line.

> 💡 Before running scripts in `generate.sh` and `generate.bat` files, set correct `AREG_SDK_ROOT` folder path.

Before calling code generator, make sure that there is Java installed on the machine and the `codegen.jar` is included in the `CLASSPATH`. 
- The `--root` option of the code generator is the root of the development project.
- The `--doc` option of the code generator is the relative to --root path of the service interface XML document to generate code.
- The `--target` option of the code generator is the path relative to `--root` to output generated files.

*Examples of running code generator from command line:*
```bash
$ java -jar codegen.jar --root=~/projects/my_project --target=src/generated/ --doc=interface/MyService.siml
```
You may as well explicitly specify the full path of `codegen.jar`
```bash
$ java -jar ~/projects/areg-sdk/codegen.jar --root=~/projects/my_project --target=src/generated/ --doc=interface/MyService.siml
```

*An example of [generate.bat](../examples/12_pubsvc/res/generate.bat) file:*
```bat
:: set the AREG_SDK_ROOT directory here
set AREG_SDK_ROOT=E:\Projects\aregtech\areg-sdk
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

## Generated codes

The code generator creates sources to extend and implement service providers (_server_) and service clients, as well it creates namespace with shared common data required by generated classes and internal classes required by the SDK. To develop a service provider (server), the developers can extend classes ending with `Stub`. To develop service clients, developers can extend classes ending with `ClientBase`. Shared data and types are defined in the generated namespace starting with `NE`. The files generated in `private` subdirectory should not be used, because all objects and files in `private` subdirectories are for internal use. The generated files should not be modified.

*An example of folder structure of generated codes:*
<br><a href="./img/generated-sources.png"><img src="./img/generated-sources.png" alt="File structure after generating codes"/></a><br>

> 💡 Since the codes are generated for both service providers and the service client, and these objects may be located in different projects, it is recommended compile generate codes as a separate _static library_ to link with service provider and client projects.

## Modeling and service initialization

After generating codes, the developer can implement the business logic of the service provider and the service client. The objects must be a part of the `Component`, which runs in a _component thread_. This extended `Component` object can either extend service provider / client base class or can aggregate separately implemented provider and client objects. In other words, the `Component` should be the holder of the service provider or service client to be able to dispatch events.

After implementing the servicing components, developers can define a _model_, which is a hierarchical design of threads, components, provided and dependent services. Such fixed modeling is a _static model_. The _model_ is used to load to instantiate objects and start services. During runtime the developer can dynamically modify a _model_ before it is loaded or can load multiple models.

An example of a _dynamic model_ is in the [14_pubtraffic](../examples/14_pubtraffic/) project. 

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

In this example, create a _model_ with name `"ServiceModel"`, load it in `main()` function when call `Application::loadModel("ServiceModel")`, and unload when call `Application::unloadModel("ServiceModel")` to stop services and release objects.

The example [10_locsvc](../examples/10_locsvc/) and higher contain implementations of _Local_ and _Public_ services and the clients. Browse [examples](../examples/) to learn more. All these examples have declarations of models.

> 💡 The difference of _Local_ and _Public_ services:
> - The _Local_ services are used for multithreading communication and cannot be accessed outside of the process.
> - The _Public_ services are used for multithreading and multiprocessing (**IPC**) communication and can be accessed by any process in the network.

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
    : Component         ( owner, entry.mRoleName)
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
    : Component             ( owner, entry.mRoleName )
    , HelloServiceClientBase  ( entry.mDependencyServices[0].mRoleName.getString(), owner )
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
