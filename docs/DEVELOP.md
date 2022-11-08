# Developer guide
```
This file is part of AREG SDK
Copyright (c) 2017-2022, Aregtech
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

Here is the brief description of AREG SDK file structure
1. The framework of AREG SDK is declared in `framework/areg` directory.
2. All high level subdirectories of `framework/areg` contain classes that can be used by developers.
3. All `private` subdirectories inside subdirectories contain implementation sources and classes for internal use and are not designed to be used by developers.
4. The multicast router of AREG SDK is declared in `framework/mcrouter` directory.
5. The examples to demonstrate AREG SDK features are in `examples` directory.
6. The tools to use are declared in `tools` directory.
7. The configurations files to customize compilation are in `conf` directory.
8. The documents are in `docs` directory.

Some more file structure information is in [README.md](./readme.md).

## Service interface prototype

AREG SDK is an interface-centric communication engine. Before creating a service, there should be service interface designed, which defines API of service, and has following sections:
1. The overview and type of service interface (_Public_ or _Local_).
2. The data types specific for the service or can be exported common types, which are possible to serialize.
3. Attributes, which are the data of services to subscribe on data set or data change notifications.
4. Methods, which are _requests_, _responses_ and _broadcasts_.
5. Constants required in service interface.
6. Any additional file to include for the service.

The service interfaces can be defined in a prototype XML document to generate basic servicing and client objects to extend. Developers may extend the generated files and implement required interfaces. An example of the service interface prototype is described in [Sample.siml](./Sample.siml) XML format Service Interface document file, which contains all sections.

> 💡 The Service Interface design UI tool is in development state and will be available in next releases as a free-ware.

### Public and Local services

The following example defines a _Public_ (used in IPC) interface defined for _Sample_ service.
```xml
<Overview ID="1" Name="Sample" Version="1.0.0" isRemote="true">
    <Description>This is an example of defining service interface.</Description>
</Overview>
```
If there is no `isRemote` attribute or it is set `false` (`isRemote="false"`) then the service interface is _Local_ and it is used for multithreading communication. The _Local_ services are not visible and are not accessible outside of the process. If `isRemote` attribute is set `true` like in the example (`isRemote="true"`), the service that provides the interface is _Public_ and the API can be accessed from any note of the network. Protect sensitive data with _Local_ services.

> 💡 If the API of the _Local_ service interface is called outside of the process, the system ignores the call and returns error message to the calling client component. Only _Public_ interfaces are accessed for _IPC_ and can be called from any client software component in the network.

### Data types

Every service interface can have specific data types. When a new data type is defined, it can be used to declare any variables, service data (attributes) and parameters of methods. New data types are listed in the section `<DataTypeList> ... </DataTypeList>`. All data types are streamable objects. It is possible to import a common data type in the service interface. The import is done by including header file and by specifying the name of the type, structure or class. Each imported type must be streamable, i.e. there should be `operator >>`or `operator <<` declared and implemented for the type, to serialize data in [IEIOStream](../framework/areg/base/IEIOStream.hpp) object. You'll get compilation error if the imported type does not have operators.

#### Structures

In the _DataTypeList_ section of Service Interface XML document, the developers can declare a new structure with the fields. The _DataType_ tag indicates type _Structure_ and the name of the structure followed by _Description_ and the list of structure fields. Each field has _data type_ and the name. If a structure has default value it should be specified in the _Value_ entry (`<Value IsDefault="true">0</Value>`). Each type of the field of structure must have `assigning operator`, `comparing operator`, `copy constructor` (if an object), must be possible explicitly to convert to `unsigned int` and must be possible to `stream` in [IEIOStream](../framework/areg/base/IEIOStream.hpp) object.

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
In this example, the structure has 3 fields with default values to set.

#### Enumerations

In the _DataTypeList_ section of Service Interface XML document, the developers can declare a new enumeration with the fields. The _DataType_ tag indicates type _Enumerate_ and the name of the enumeration followed by _Description_ and the list of fields. Each field may contain _Value_. The enumerations automatically are streamable.

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

In the _DataTypeList_ section of Service Interface XML document the developers can import defined types. The _DataType_ tag indicates type _Imported_ and the name of the imported type followed by _Description_, _Namespace_ if present and the location of relative path of the imported file. Any imported type must be possible to `stream` in [IEIOStream](../framework/areg/base/IEIOStream.hpp) object.

*An example of imported type:*
 ```xml
<DataType ID="11" Name="uAlign" Type="Imported">
    <Description>This example exports NEMemory::uAlign in the service interface.</Description>
    <Namespace>NEMemory</Namespace>
    <Location>areg/base/NEMemory.hpp</Location>
</DataType>
```
The example imports type `NEMemory::uAlign`, which is declared in `areg/base/NEMemory.hpp` header file and is streamable.

#### Defined containers

In the _DataTypeList_ section of Service Interface XML document the developers can declare new container types. The _DataType_ tag indicates type _DefinedType_ and the name of the type followed by _Description_. The _Container_ tag specifies the type of container and the _BaseTypeValue_ define the type of values stored in the container, which must be possible to `stream` in [IEIOStream](../framework/areg/base/IEIOStream.hpp) object. If the container is _HashMap_ in addition it has _BaseTypeKey_, which indicates the type of keys in the map and which must be possible to stream.

*An example of defining new type of array:*
```xml
<DataType ID="12" Name="SomeArray" Type="DefinedType">
    <Description>Defines new type of array</Description>
    <Container>Array</Container>
    <BaseTypeValue>uint32</BaseTypeValue>
</DataType>
```
In this example, the values of array have type `unsigned int`, which is streamable.

*An example of defining new type of linked list:*
```xml
<DataType ID="13" Name="SomeList" Type="DefinedType">
    <Description>New type of linked list.</Description>
    <Container>LinkedList</Container>
    <BaseTypeValue>String</BaseTypeValue>
</DataType>
```
In this example, the values of linked list have type `String`, which is streamable.

*An example of defining new type of hash map:*
```xml
<DataType ID="14" Name="SomeMap" Type="DefinedType">
    <Description>This example defines hash-map where key is exported and value is new data type.</Description>
    <Container>HashMap</Container>
    <BaseTypeValue>SomeStruct</BaseTypeValue>
    <BaseTypeKey>String</BaseTypeKey>
</DataType>
```
In this example, the values of hash-map have type `SomeStruct` and the key are type `String`. Both types are streamable. The type `SomeStruct` declared in the this Service Interface XML document is automatically declared as streamable, has assigning and compare operators, and has default constructor.

### Attributes

_Attributes_ in services are data that clients can subscribe to get update notifications either each time when data is set of only when data is updated. The attributes are listed in the section `<AttributeList> ... </AttributeList>`. The type of attribute must be possible to stream in [IEIOStream](../framework/areg/base/IEIOStream.hpp) object.

> 💡 The data of an `Attribute` is always available for the client. As soon as the client subscribes on the attribute data, it will receive at least one notification of the current value and a flag, indicating the validation state of the data. If on first notification the data state is _invalid_ the client may receive second notification with the valid value sent by servicing component.<br>
> 💡 It is important to check the state flag to make correct reaction. The attribute notification callback has semantic `void onAttributeNameUpdate( const DataType & AttributeName, NEService::eDataStateType state )`, where the _AttributeName_ is the name of and the _DataType_ is the type of the attribute. The attribute data is valid only if `state` parameter is equal to `NEService::eDataStateType::DataIsOK`. In all other cases the data value is invalid and can be ignored or the error can be handled.<br>
> 💡 All attributes in the same service interface must have unique names and the type of all attributes must be streamable.

*An example of declaring attribute to notify only on value change:*
```xml
<Attribute DataType="SomeEnum" ID="15" Name="SomeAttr1" Notify="OnChange">
    <Description>An attribute to notify subscribers only when value is changed.</Description>
</Attribute>
```
In this example the attribute of `SomeEnum` type has name `SomeAttr1` and it is designed to notify connected clients only if the value is changed. The type `SomeEnum` must be possible to compare.

*An example of declaring attribute to notify only on value change:*
```xml
<Attribute DataType="SomeStruct" ID="16" Name="SomeAttr2" Notify="Always">
    <Description>Another attribute to notify subscribers any time when value is set (maybe not changed).</Description>
</Attribute>
```
In this example the attribute of `SomeSruct` type has name `SomeAttr2` and it is designed to notify connected clients each time when value is set.

The difference of _OnChange_ and _Always_ notifications is that _OnChange_ the system sends notifications only if stored value and new value differ, i.e. **it is comparing stored and new values**. In case of _Always_ notifications, the system notifies clients each time when value is set, indifferent whether it has been changed or not,i.e. **it is NOT comparing stored and new values**.

### Requests, responses and broadcasts
Service interface may have `Request`, `Response` and `Broadcast` methods. The _Requests_ are called by clients to be executed on the service. The _Responses_ are replies to the requests. _Broadcasts_ are methods acting like events to deliver multiple parameters.

#### Requests
The requests are called by clients to be executed on the service. The requests may have parameters. The requests may have linked response. If request has a response, then the processing request is blocked until service replies with the response. It is possible manually to unblock the request, but then the response must be manually prepared to reply. Multiple requests can be linked with the same response. The request may have no response at all and in this case the request can be called one after another.

> 💡 When developers implement service provider (server) then must extend the generated `Stub` object and implement all request calls, which are declared in a base `Stub` class as _pure virtual_.<br>
> 💡 For an error handling of a request call, the client side should implement request failed methods, which are generated in the `ClientBase` objects. The passed error code as a parameter, indicates the failure reason. If the developer does not implement the failure method and the failure is triggered, there will be an error message in the logs.

*An example to demonstrate the definition of request with no parameter and linked response:*
```xml
<Method ID="17" MethodType="request" Name="SomeRequest1" Response="SomeResponse1">
    <Description>Request and response with no parameters.</Description>
</Method>
<Method ID="19" MethodType="response" Name="SomeResponse1">
    <Description/>
</Method>
<Method ID="19" MethodType="response" Name="SomeResponse1">
    <Description/>
</Method>
```
In this example the request `SomeRequest1` has no parameter and it is linked with the response `SomeResponse1`. By default, the request is blocking and it is unblocked when service provider calls response `SomeResponse1`, so that it can process the next call of `SomeRequest1`. In the generated C++ code, the request has semantic `void requestSomeRequest1()` and the response `void responseSomeResponse1()`.

*An example to demonstrate the request with multiple parameters linked with the response:*
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
In this example the request `SomeRequest2` has 3 parameters, where the last parameter `param3` has default value, and the request is linked the response `SomeResponse` with 1 parameter. By default, the request is blocking and it is unblocked when service provider calls response `SomeResponse`, so that it can process the next call of `SomeRequest1`. In the generated C++ code, the request has semantic `void requestSomeRequest2(int param1, const NESample::SomeStruct & param2, NESample::SomeEnum param3 = NESample::SomeEnum::Nothing)` and the response `void responseSomeResponse(bool succeeded)`.

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

In this example the 2 different requests `SomeRequest2` and `SomeRequest3` are linked the response `SomeResponse`. By default, while requests are processing they are blocked and in busy state. If 2 requests are linked with the same response and while one request is busy, it is not blocking the call of other requests linked with the same response. As soon as the response is called, it unblocks all linked requests. For example, if developer calls request `SomeRequest2`, while it is processing (i.e. it is in busy state) the developer can  call request `SomeRequest3` and it will be processed. When developer calls response `SomeResponse3`, it unblocks both requests `SomeRequest2` and `SomeRequest3`. In other words, the response `SomeResponse` unblocks all linked busy requests. In the generated C++ code, the request `SomeRequest2` has semantic `void requestSomeRequest2(int param1, const NESample::SomeStruct & param2, NESample::SomeEnum param3 = NESample::SomeEnum::Nothing)`, the request `SomeRequest3` has semantic `void requestSomeRequest3(const NEMemory::uAlign & param)` and the response is `void responseSomeResponse(bool succeeded)`.

*An example to demonstrate request with no response:*
```xml
<Method ID="27" MethodType="request" Name="StandAlone">
    <Description>A request with no response.</Description>
</Method>
```
In this example there is a request `StandAlone`, which has no response. It means that as soon as service provider processed the request, it is immediately unblocked, so that the service provider can continue processing further calls of the request `StandAlone`. In the generated C++ code, the request `StandAlone` has semantic `void requestStandAlone()`.

#### Responses
The responses are used by service provider as a result of request calls. Each response must be linked with at least one request. If a response has no request, it is ignored and is never triggered. After calling the response on service provider side, it is triggered on client side. If a client is not interested calling the request, but is interested in the response as a result of call, it can manually and dynamically subscribe on the response.

> 💡 The responses are called by the service provider (server), which are delivered at the client side. Developers need to extend the generated `ClientBase` object and implement responses that are expected. There is no need to implement all response if the client is using certain request calls. It is enough to implement only expected responses, because others are never triggered, unless developer does not manually subscribe on certain response(s).

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
In this example, the responses are linked with the requests (see chapter [Requests](#requests)), where the response `SomeResponse1` is connected with one request and the response `SomeResponse` is linked with multiple requests. The semantic of the response `SomeResponse1` is `void responseSomeResponse1()` and the semantic of response `SomeResponse` is `void responseSomeResponse(bool succeeded)`.

> 💡 Any processing request is automatically in _busy_ state. The call of response is automatically unblocks all linked requests. So that, it is very important to send the response to unlock the request(s). Otherwise, while the request is busy and a client calls that busy request, the system generates _request is busy_ failure message and delivers to the client, so that, the service receives the request only once (first calls) and all other calls are rejected by the system.<br>
> 💡 If a request does not have linked request, it is never blocked and it is never in _busy_ state.<br>
> 💡 It is possible dynamically to subscribe on a certain `Response` without triggering a `Request`. In this case, subscribed client receives the `Response` each time when the `Request` is processed and the response is replied.

#### Broadcasts
The broadcasts are special service methods to fire an event and pass several data at the same time. Broadcasts are not linked neither with requests, nor with responses. not related neither with requests, nor with response. The broadcasts are fired on service provider side and delivered to every client subscribed for the broadcast. The clients may dynamically subscribe or un-subscribe on a broadcast. If no client is subscribed on the broadcast, the system ignores the event.

> 💡 Unlike `Attributes`, the broadcast data is available only once. It means that if a client subscribes on the broadcast, it will not immediately receive notification, but only when the servicing component triggers broadcast after the client is subscribed on the broadcast.

*An example to demonstrate declaration of a broadcast with parameters:*
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
In this example, the broadcast `SomeBroadcast` has 4 parameters as values to deliver together to all subscribed clients. The semantic of this broadcast is `void broadcastSomeBroadcast(NESample::SomeEnum value1, const NEMemory::uAlign & value2, const NESample::SomeStruct & value3, NESample::SomeArray value4);`

> 💡 In the service interface XML document the request, response and the broadcast may have same name.<br>
> 💡 It is recommended that at least all names of the responses and broadcasts have unique names. If any name is not unique, then must have same type. It means, that there must not be a parameter named `success` having type `bool` in a response or broadcast and having type `int` in the other response or broadcast.

### Constants
The Service Interface XML document may have specific constants listed in `<ConstantList> ... </ConstantList>` section. The constants are used to share read-only value between clients and the service provider.

*An example to demonstrate declaration of a constant:*
```xml
<Constant DataType="uint16" ID="35" Name="SomeConst">
    <Value>100</Value>
    <Description>Define a constant if need.</Description>
</Constant>
```
In this example, it is declared a constant named `SomeConst` having type `unsigned short` and having value `100`. This values is shared between service provider and clients.

### Includes
The Service Interface XML document may have specific files, which should be included in the code. The includes are listed in `<IncludeList> ... </IncludeList> section. For example, a service may include special header file with the implementation of algorithms.

*An example to demonstrate declaration of includes:*
```xml
<IncludeList>
    <Location ID="36" Name="areg/base/NEMath.hpp">
        <Description>can make additional includes</Description>
    </Location>
</IncludeList>
```
In this example, after generating code, the header file `areg/base/NEMath.hpp` will be included for service providers and service client codes.

## Code generator
To avoid tedious jobs and minimize mistakes when coding, AREG SDK provides a code-generator located in [tools](../tools/) folder to generate base objects from Service Interface XML document. The generated files must not be modified. Instead, they should be extended to implement all necessary functions. Normally, the service providers implement `Request` methods and the service clients implement `Response`, `Broadcast` and `Attribute` notification methods by need. Edit and run [generate.sh](../tools/generate.sh) or [generate.bat](../tools/generate.bat) file to generate source code or run `codegen.jar` from command line.

> 💡 Do not forget to set correct `AREG_SDK_ROOT` folder in `generate.sh` and `generate.bat` file to run code generator.

Before calling code generator, make sure that there is Java installed on the machine and the `codegen.jar` is included in the `CLASSPATH`. 
- The `--root` option of the code generator is the root of developer project.
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

*An example of [generate.bat](./../examples/12_pubsvc/res/generate.bat) file:*
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

The code generator creates sources to extend and implement service providers (_server_) and service clients, as well it creates namespace with shared common data required by generated classes and internal classes required by the SDK. To develop service provider (server), the developers can extend classes ending with `Stub`. To develop service clients, developers can extend classes ending with `ClientBase`. Shared data and types are defined in the generated namespace starting with `NE`. The files generated in `private` subdirectory should not be used, because all objects and files in `private` subdirectories are for internal use. The generated files should not be modified.

*An example of folder structure of generated codes:*
<br><a href="./img/generated-sources.png"><img src="./img/generated-sources.png" alt="File structure after generating codes"/></a><br>

> 💡 Since the codes are generated for both service providers and the service client, and these objects may be located in different projects, it is recommended compile generate codes as a separate _static library_ to link with service provider and client projects.

## Modeling and service initialization

After generating codes, the developer can implement the business logic of the service provider and the service client. The objects must be a part of the `Component`, which runs in a _component thread_. This extended `Component` object can either extend service provider / client base class or can aggregate separately implemented provider and client objects. In other word, the `Component` should be the holder of the service provider or service client to be able to dispatch events.

After implementing the servicing components, developer can define a _model_, which is a hierarchical design of threads, components, provided and dependent services. Such fixed modeling is _static model_. The _model_ is used to load to instantiate objects and start services. During runtime the developer can dynamically modify model before it is loaded or can load multiple models.

An example of _dynamic model_ is in [14_pubtraffic](../examples/14_pubtraffic/) project. 

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

The example [10_locsvc](../examples/10_locsvc/) and higher contain implementations of _Local_ and _Public_ services and the clients. Browse [examples](../examples/) to learn more. All these examples have declaration of models.

> 💡 The difference of _Local_ and _Public_ services:
> - The _Local_ services are used for multithreading communication and cannot be accessed outside of process.
> - The _Public_ service are used for multithreading and multiprocessing (**IPC**) communication and can be accessed by any process in the network.

## Hello Service!

> 💡 The source codes of this example are in [00_helloservice](../examples/00_helloservice). 

This topic is a step-by-step training example to create service enabled applications. For simplicity, create new directory inside `areg-sdk/examples` directory and name it `helloservice`, so that the `~/areg-sdk/examples/helloservice` is the root of training projects and it is working directory for 3 sample projects that use same service provider (server) and service client located in `common/src` subdirectory, where:
- In the _project #1_ (`onethread`) the service provider and client run in the same thread of the same process.
- In the _project #2_ (`twothreads`) the service provider and client run in different threads of the same process.
- In the _project #3_ (`multiprocess`) the service provider and client run in different processes.

While in the _project #1 and #2_ the service acts like _Local_ (because client and server are in the same process), in case of _project #3_ it is a _Public_ service and the client / server can run either on the same machine or different machine. To run _project #3_ we need to start `mcrouter` on any machine within the network. Make sure the [router.init](../framework/mcrouter/resources/router.init) file is properly configured (connection with `mcrouter`) for each executable and it is located in `config` subdirectory of executable.

Features demonstrated in this examples:
- Each project creates a model, which is a definition of service provider and service client that can be:
  - in the same thread of the same process;
  - in the different threads of the same process;
  - in different processes.
- Each application loads model to instantiate service and unloads model to stop service.
- The service provider automatically registers and service client automatically discovers service.
- As soon as service is available for the client, it sends a request to service provider.
- The service provider processes the request and replies with response.
- The service client receives response and initiates _quit_.
  - since in _project #1 and 2_ service provider and client run in same process, application quits after response;
  - in _project #3_ service provider application continues to run and service client application quits after response.
- In case of _project #3_:
  - multiple clients can start in parallel or one after another;
  - one or more clients can start before and/or after service provider starts;
  - no matter of applications starting order, service discovery is automated.

The agenda of this topic is to demonstrate service and client implementation, as well the possibility to split and merge services in processes and threads to distribute computing power. Pay attention that all projects instantiate the same service provider and client objects, the differences are in _models_ declared and used in `main.cpp` files.

> 💡 Important development notes when create service provider and service client:<br>
> 1. The `Component` is the holder of the servicing objects (service provider and/or service client).<br>
> 2. The `Component` can have implementation of multiple service interfaces (multiple service providers).<br>
> 3. The same service provider can be instantiated multiple times, but instances should run in different threads.<br>
> 4. The services differ by service interface name and _role_ names.<br>
> 5. In the _model_ the clients refer to the service by _role_ names.<br>
> 6. One class can extend and mix interfaces of multiple different providers and clients.<br>
> 7. The instances of the same service interface provider and client can be in the same thread.<br>
> 8. The `Component` can contain mixture of service providers and clients.<br>
> 9. The _role_ names of a _Local_ services must be unique within same process.<br>
> 10. The _role_ names of a _Public_ services must be unique within same network.<br>
> 11. The service clients know availability of service via callback `void serviceConnected( bool isConnected, ProxyBase & proxy )`, where `isConnected` indicates service availability.<br>
> 12. The service providers know new client connection via callback `void clientConnected( const ProxyAddress & client, bool isConnected )`, where `isConnected` indicates client connection status.

### Service Interface

In `helloservice` create a `res` folder. Create a file `HelloService.siml` service interface file and copy this XML in the file, which defines _Public_ service interface with a request and response.
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
- The `ServiceComponent` derives `Component` and `HelloServiceStub` classes.
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
     * \param   client  The name of client to output the greeting.
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

In the `helloservice/common/src` subfolder create file `ClientComponent.hpp` to develop `ClientComponent` object as a service client component.
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
            //      a. call requests to run on server side.
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
    // It is done to give chance to see output message on console.
    // Otherwise, the next line of code close the application and yuo miss the message on console.
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
- The service client overrides virtual method `bool serviceConnected(bool isConnected, ProxyBase & proxy)` to react on service connect / disconnect events. It calls request when service is connected and initiates application quit when service is disconnected.
* The service client overrides virtual method `void responseHelloService( bool success )` to react on request reply. It waits 1 second and initiates application quit.
* The service client overrides virtual method `requestHelloServiceFailed(NEService::eResultType FailureReason)` to make error handling.

Now there are service provider and service client available, and we can start creating applications using generated codes and using developed classes.

### Create and load model

The developers can decide how to distribute service and client objects. The following examples are applications where:
- The service and the client run in the same thread of the same process.
- The service and the client run in the separate threads of the same process.
- The service and the client run in separate processes (_Public_ service case).

Each application is created in separate project. The distribution of service provider and client is defined in the _model_. In the examples, the macro:
- `BEGIN_MODEL` is declaration of the model with the name. There can be multiple models in the application.
- `BEGIN_REGISTER_THREAD` is declaration of component thread with name and watchdog timeout (value `NECommon::WATCHDOG_IGNORE` ignores the watchdog). Service providers and service clients run in _component thread_.
- `BEGIN_REGISTER_COMPONENT` is declaration of servicing component with _role_ name and the class name.
- `REGISTER_IMPLEMENT_SERVICE` is declaration of provided service interface. Each provided service must be listed separately in `REGISTER_IMPLEMENT_SERVICE` macro.
- `REGISTER_DEPENDENCY` is a declaration of service client, which is dependent on service provider. The dependency is not the service interface name, but the _role_ name of the servicing component where the service provider belongs. Each dependency must be listed separately in `REGISTER_DEPENDENCY` macro.

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
3. The application wait for _quit_ signal calling `Application::waitAppQuit(NECommon::WAIT_INFINITE)`.
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
3. The application wait for _quit_ signal calling `Application::waitAppQuit(NECommon::WAIT_INFINITE)`.
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
3. The application wait for _quit_ signal calling `Application::waitAppQuit(NECommon::WAIT_INFINITE)`.
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
3. The application wait for _quit_ signal calling `Application::waitAppQuit(NECommon::WAIT_INFINITE)`.
4. The application unloads model and stops service in `Application::unloadModel(_modelName)`.
5. The application releases internals in `Application::releaseApplication()` and exits with result 0.

> 💡 In this particular project, the _role_ name of client component has dynamic name, which is changing on each start. The declaration `const std::string   _client( NEUtilities::generateName("ServiceClient").getString() )` generates a name with timestamp, so that each new application start generate new name. This will help to run  multiple instances of client application in parallel or one after another.

### Make the build

The created projects can be built with the help of `MS Visual Studio`, `Eclipse`, `Make` command line tool, `CMake` command line tool, etc. Here it is not considered creating projects for MS Visual Studio and Eclipse. The appropriate project files for the mentioned IDE can be found in each project folder of [00_helloservice](../examples/00_helloservice). Here considered creating `Makefiles` and compile projects.

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

> 💡 More testing examples are listed in [examples](../examples/) folder of `areg-sdk`.

1. To compile all projects, call `make all` in command line.
2. To compile example projects, call `make examples` in command line
3. All compiled files are located in `<areg-sdk>/product/build/<compiler-platform-path>/bin` folder.
4. If run application with a _Public_ service (**IPC** projects or examples), start `mcrouter`.
5. There is no need to start _mcrouter_ if testing application with no _Public_ service.
6. All compiled files are in the `bin` folder, switch there to start applications.
7. Some application may create logs in the `logs` folder of binary location. Open to analyze logs.

## Contact us!

Contact us at info[at]aregtech.com if need help or more information.
If you linked the project and the idea behind that, please ![star us](https://img.shields.io/github/stars/aregtech/areg-sdk.svg?style=social&label=staring%20us) at GitHub, so that contributors feel being appreciated for the job.
