```
This file is part of AREG SDK
Copyright (c) 2017-2021, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
============================================

# Developer guide

This document is a developer guide and describes how to develop a service enabled application.

## Service interface prototype

AREG SDK is an interface-centric communication engine. Before creating a service, there should be service interface designed, which defines API of service, and has following sections:
1. The overview and type of service interface (_Public_ or _Local_).
2. The data types specific for the service or exported from any header file.
3. Attributes, which are the data of services and developers can describe to be notified on value change.
4. Methods, which are _requests_, _responses_ and _broadcasts_.
5. Constant values, needed by interface.
6. Any additional file to include.

The service interfaces can be defined in a prototype XML document to be used to generate all required base objects to extend and develop service or service clients. An example of the service interface prototype is [Sample.siml](./Sample.siml).

> 💡 There is a UI tool to help to define interface, which is in development state and will be available in next release.

### Public and Local services

The following example defines _Sample_ service as _Public_ (used in IPC).
```xml
<Overview ID="1" Name="Sample" Version="1.0.0" isRemote="true">
    <Description>This is an example of defining service interface.</Description>
</Overview>
```
Remove **isRemote** or set to false `isRemote="true"` to switch service to _Local_.

> 💡 The _Local_ services are not used in IPC, the system will ignore to forward messages and they will not be visible outside of process.


### Data types

Every service interface can have service specific data types. When a new data type is defined, it can be used to declare any variables and parameters of methods. New data types are listed in the section `<DataTypeList> ... </DataTypeList>`. All data types are automatically streamable objects.

#### Structures

The following example demonstrates how to declare a new structure with the fields. The default values are used to set when the object is created. The fields of structure must have `assigning operator`, `comparing operator`, `copy constructor` (if an object), must be possible explicitly to convert to `unsigned int` and must be possible to `stream`.

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
In this example, the structure has 3 fields with default values to set when create the object.

#### Enumerations

The following example demonstrates how to declare enumeration. All enumerations have the `int` type.
```xml
<DataType ID="6" Name="SomeEnum" Type="Enumerate" Values="default">
    <FieldList>
        <EnumEntry ID="7" Name="Invalid">
            <Value>-1</Value>
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
 The enumeration fields may have explicit values. In this example, the `Invalid` field has value `-1`.

 #### Imported types

 The following example demonstrates how to import an existing type. The specified `NEMemory::uAlign` is already existing in `areg/base/NEMemory.hpp`. It is imported to be used in the service.
 ```xml
<DataType ID="11" Name="uAlign" Type="Imported">
    <Description>Example of how to import existing types from other header files. This example exports NEMemory::uAlign</Description>
    <Namespace>NEMemory</Namespace>
    <Location>areg/base/NEMemory.hpp</Location>
</DataType>
```

#### Defined containers

The following example demonstrates how to define a container. The types of containers must be possible to copy, assign and compare. In case of hash-map, the key must be possible to convert to `unsigned int`.

**New array**:
```xml
<DataType ID="12" Name="SomeArray" Type="DefinedType">
    <Description>Defines new type of array</Description>
    <Container>Array</Container>
    <BaseTypeValue>uint32</BaseTypeValue>
</DataType>
```

**New linked list**:
```xml
<DataType ID="13" Name="SomeList" Type="DefinedType">
    <Description>New type of linked list.</Description>
    <Container>LinkedList</Container>
    <BaseTypeValue>String</BaseTypeValue>
</DataType>
```

**New hash-map**:
```xml
<DataType ID="14" Name="SomeMap" Type="DefinedType">
    <Description>An example of defining hash-map, where the key is exported value and the value is new defined type.</Description>
    <Container>HashMap</Container>
    <BaseTypeValue>SomeStruct</BaseTypeValue>
    <BaseTypeKey>String</BaseTypeKey>
</DataType>
```

### Attributes

_Attributes_ in services are data that clients can subscribe to get update notifications. The attributes are listed in the section `<AttributeList> ... <\AttributeList>`. In this example the system notifies connected clients when the value of `SomeAttr1` is changed, i.e. the system checks whether the new value is equal or not, then updates and sends notification to all subscribed clients:
```xml
<Attribute DataType="SomeEnum" ID="15" Name="SomeAttr1" Notify="OnChange">
    <Description>An attribute to notify subscribers only when value is changed.</Description>
</Attribute>
```

In this example, the system notifies clients each time when the value is set, i.e. each time when the value is set, the system does not make any checks, it just sets the value and sends notification to all subscribed clients:
```xml
<Attribute DataType="SomeStruct" ID="16" Name="SomeAttr2" Notify="Always">
    <Description>Another attribute to notify subscribers any time when value is set (maybe not changed).</Description>
</Attribute>
```

### Requests, responses and broadcasts

Every service prototype contains `Request`, `Response` and `Broadcast` methods. The _Requests_ are called by clients to be executed on service. The _Responses_ are replies of requests. _Broadcasts_ are special methods that are not connected with requests or response, the service triggers it to deliver multiple parameters to subscribed clients.

#### Requests
The requests are called by service clients to run on the service side. The requests may have parameters. The requests can be connected with a certain response. If so, the processing request is blocked until service does not reply with response. It is as well possible manually to unblock the request, but then the response must be manually prepared to reply. Multiple requests can be connected with the same response. The request may have no response at all. 

This example demonstrates the definition of request `SomeRequest1` with no parameter connected with a response `SomeResponse1`:
```xml
<Method ID="17" MethodType="request" Name="SomeRequest1" Response="SomeResponse1">
    <Description>Request and response with no parameters.</Description>
</Method>
```

This example demonstrates the definition of request `SomeRequest2` with multiple parameters connected with response `SomeResponse`:
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
```

This example demonstrates the definition of 2 requests `SomeRequest2` and `SomeRequest3` connected with the same response `SomeResponse`:
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
```

This example demonstrates the definition of request `StandAlone` with no response:
```xml
<Method ID="27" MethodType="request" Name="StandAlone">
    <Description>A request with no response.</Description>
</Method>
```

#### Responses

The responses are sent to the service client as a reply of the executed request. When a client calls a request to execute on the service, the system automatically delivers the response when the service replies. It is as well possible dynamically to subscribe on a response without calling request, so that when a service replies on the request, all subscribed clients as well receive response to react on certain request completion. This example demonstrates the definition of response with and without parameters:
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

> 💡 The system sends only one response per request when the request is in running state. The multiple responses on one request will have no effect, because the first response will mark the request as idle, so that the next replies will have no effect.

#### Broadcasts

Broadcasts are special methods to send one or more data at the same time. The clients may dynamically subscribe on _Broadcast_ and each time the service calls broadcast, all subscribed it is delivered to all subscribed clients. The broadcasts are not connected with requests or responses. This example demonstrates the definition of a broadcast with parameters.
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

The constants are listed in `<ConstantList> ... </ConstantList>` of the prototype document, the constants are used to share read-only value between clients and the service. This example demonstrates the definition of service specific constant:
```xml
<Constant DataType="uint16" ID="35" Name="SomeConst">
    <Value>100</Value>
    <Description>define a constant if need.</Description>
</Constant>
```

### Includes

The includes are listed in `<IncludeList> ... </IncludeList> of the prototype document. The includes are just relative paths of header files automatically to include when build clients or the service. The includes are used if no type is imported to the service prototype document, but they should be included in the build. For example, a header file might contain algorithms that should be included in the build of service and/or clients. In this case the header file can be included in the _Includes_ so that they are automatically included in build of service and client. This example demonstrate the definition of include list and entries:
```xml
<IncludeList>
    <Location ID="36" Name="areg/base/NEMath.hpp">
        <Description>can make additional includes</Description>
    </Location>
</IncludeList>
```

## Code generator

When the service interface prototype document is designed, developers can use the code generator tool to generate base objects that helps to avoid tedious jobs and escape mechanical mistakes when type codes. The code generator is located in the [tools](./../tools/) folder of areg-sdk. Edit and run `generate.sh` or `generate.bat` file to generate source code.

Before calling code generator, make sure that there is Java installed on the machine and the `codegen.jar` is included in the `CLASSPATH` environment variable.

The followings are the parameters of code generator:
```bash
$ java com.aregtech.CMFMain --root=<project_root> --doc=<relative_path_to_siml> --target=<relative_path_to_target_location>
```
where:
- `<project_root>` if the path of your project, for example `~/aregtech/areg-sdk/examples`;
- `<relative_path_to_siml>` the service interface prototype file path relative to the project root, for example `12_pubsvc/res/SystemShutdown.siml`
- `<relative_path_to_target_location>` the generated code output folder path relative to the project root, for example `12_pubsvc/generated`

Example of running code generator, copied from [generate.bat](./../examples/12_pubsvc/res/generate.bat) file:
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

@echo Generating class of HelloWorld Service Interface in folder %CODE_GEN% .....
java com.aregtech.CMFMain --root=%PROJECT_ROOT% --doc=res\HelloWorld.siml --target=%CODE_GEN%
```

## Generated codes

The code generator generates sources to implement service and clients, as well generates helper classes to send, receive and dispatch objects. After calling the code generator, ths system will have such file structure
<br><a href="./img/generated-sources.png"><img src="./img/generated-sources.png" alt="File structure after generating codes"/></a><br><br>
Since the codes are generated for both service and the clients, and both of them may be compiled in different projects, we recommend to compile generated codes as a static library to link with the executables or shared libraries.

## Modeling and service initialization

After generating codes, the implemented services must be a part of the `Component` object. To initialize services we recommend creating a _model_. The models are created static or dynamic. For an example of dynamic model, see project [14_pubtraffic](./../examples/14_pubtraffic/). This is an example of defining model and start of _Public_ service:
```cpp
constexpr char const _modelName[]  { "ServiceModel" };   //!< The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::ServiceHelloName, ServicingComponent )
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( NECommon::ServiceHelloName )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_12_pubservice_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing simple remote servicing component. Run as a ultra-small Server...\n");

    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, nullptr, nullptr );

    do 
    {
        TRACE_SCOPE(example_12_pubservice_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        // load model to initialize components
        Application::loadModel(_modelName);

        TRACE_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(_modelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);
    
    printf("Completed testing simple remote servicing component. Check the logs...\n");

	return 0;
}

```

The example [10_locsvc](./../examples/10_locsvc/) and higher contain implementations of _Local_ and _Public_ services and the clients. Browse examples to learn more.

## Develop a Service

Before developing a service, it is important to know that 
1. Service owner is a `Component` object. The `Component` can provide more than one service interfaces.
2. All service base classes generated from prototype XML document end with `Stub` and contain request pure virtual methods. For example, the base service class name from Sample interface is `SampleStub`, which contains list of request methods declared as pure virtual.

The easiest way to create new service, to extend it from `Component` and `xxxStub` objects. However, it is not a MUST option, but when instantiate a `Component`, the instance of service stub should be instantiated as well. When extend `Component` and `xxxStub`
1. Implement `Component * CreateComponent( ... )` and `void DeleteComponent( ... )` **static** methods
2. Implement all pure virtual methods derived from `xxxStub`.

The following is an example of implementing simple service:
```cpp
#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/HelloWorldStub.hpp"

//////////////////////////////////////////////////////////////////////////
// ServicingComponent declaration
//////////////////////////////////////////////////////////////////////////

class ServicingComponent    : public    Component
                            , protected HelloWorldStub
{
//////////////////////////////////////////////////////////////////////////
// static methods
//////////////////////////////////////////////////////////////////////////
public:

    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

protected:
    ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    virtual ~ServicingComponent(void) = default;

//////////////////////////////////////////////////////////////////////////
// HelloWorldStub overrides
//////////////////////////////////////////////////////////////////////////
protected:

    virtual void requestHelloWorld( const String & client ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline ServicingComponent & self( void )
    { return (*this); };

    ServicingComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServicingComponent );
};

//////////////////////////////////////////////////////////////////////////
// ServicingComponent implementation
//////////////////////////////////////////////////////////////////////////

Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServicingComponent(entry, owner);
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( owner, entry.mRoleName)
    , HelloWorldStub    ( static_cast<Component &>(self()) )
{
}

void ServicingComponent::requestHelloWorld(const String & client)
{
    printf("Hello %s!!!\n", client.getString());

    responseHelloWorld(true); // indicate success
}
```

In this example
* The class `ServicingComponent` is an instance of `Component` and `HelloWorldStub`.
* The service is created in **static** `Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)` method.
* The service is deleted in **static** `void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)` method.
* The service implements virtual method `virtual void requestHelloWorld(const String & client)` inherited from `HelloWorldStub`
* In the request `requestHelloWorld` called respone `responseHelloWorld`, so it is immediately unblocked. Otherwise, it would remain _blocked_ until response is sent.

## Develop a Service client.

Before developing service client, import to know that:
1. Service client owner is a `Component` object. There can be more than clients in `Component` and there can be several instances of the same client in the `Component`.
2. All service client base classes generated from prototype XML document end with `ClientBase` and contains list of all responses, broadcasts, attribute update, and request failure methods (callbacks). For example, the base service class name from Sample interface is `SampleClientBase`, which contains response, broadcasts, attribute update and request failure methods.
3. Whenever service client is connected with the service (i.e. service is available) or disconnected, the system invokes method `bool serviceConnected( bool isConnected, ProxyBase & proxy )`, where the parameter `isConnected` indicates the connection status and parameter `proxy` indicates which object have got connection. In case if an instance of `Component` contains multiple clients, the `proxy` parameter indicates which clinet have got or lost the connection.

The easiest way to create new service client, to extend it from `Component` and `xxxClientBase` objects. However, it is not a MUST option.
1. Implement `Component * CreateComponent( ... )` and `void DeleteComponent( ... )` **static** methods.
2. Implement only those response, broadcast, attribute update, and request failure methods, which are used by client. All other methods can be ignored, since they will be never triggered. However, you can implement all methods. The _implementation_ means to write the business logic when one of this methods are triggered (i.e. event is fired).

The following is an example of implementing simple service client:
```cpp
#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/HelloWorldClientBase.hpp"
#include "areg/appbase/Application.hpp"

class ServiceClient : public    Component
                    , protected HelloWorldClientBase
{
//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

public:
    
    ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    virtual ~ServiceClient(void) = default;

//////////////////////////////////////////////////////////////////////////
// HelloWorldClientBase overrides
//////////////////////////////////////////////////////////////////////////
protected:

    virtual void responseHelloWorld( bool success ) override;

/************************************************************************/
// Error handling.
/************************************************************************/

    virtual void requestHelloWorldFailed( NEService::eResultType FailureReason ) override;

/************************************************************************/
// IEProxyListener Overrides
/************************************************************************/

    //!< Triggered by system, when the service is either available and connected or disconnected.
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden / FOrbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ServiceClient & self( void )
    {   return (*this); }

    ServiceClient( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceClient );
};

//////////////////////////////////////////////////////////////////////////
// ServiceClient implementation
//////////////////////////////////////////////////////////////////////////

Component * ServiceClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServiceClient(entry, owner);
}

void ServiceClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServiceClient::ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( owner, entry.mRoleName )
    , HelloWorldClientBase  ( entry.mDependencyServices[0].mRoleName.getString(), owner )
{
}

bool ServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    bool result = HelloWorldClientBase::serviceConnected(isConnected, proxy);
    if (isConnected)
    {
        // Up from this part the client
        //  - can send requests
        //  - can subscribe on data or event.

        // call request
        requestHelloWorld( getRoleName() );
    }
    else
    {
        // Make cleanups, release subscription here.
        // Since we've lost connection, exit client
        Application::signalAppQuit();
    }

    return result;
}

void ServiceClient::responseHelloWorld( bool success )
{
    printf("%s to output message.\n", success ? "succeeded" : "failed");

    // The client completed the job, set signal to exit application
    Application::signalAppQuit();
}

void ServiceClient::requestHelloWorldFailed(NEService::eResultType FailureReason)
{
    // make error handling here.
    printf("Failed to execute request, retry again.");
    if (isConnected())
    {
        // the service is still connected, can re-send reqest.
        requestHelloWorld( getRoleName() );
    }
}

```
In this example
* The class `ServiceClient` is an instance of `Component` and `HelloWorldClientBase`.
* The service client is created in **static** `Component * ServiceClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)` method.
* The service client is deleted in **static** `void ServiceClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)` method.
* The service client overrides virtual method `virtual bool serviceConnected(bool isConnected, ProxyBase & proxy)` used to react on service connect / diconnect event.
* The service client overrides virtual method `virtual void responseHelloWorld( bool success )` inherited from `HelloWorldClientBase`
* The service client overrides virtual method `virtual requestHelloWorldFailed(NEService::eResultType FailureReason)` inherited from `HelloWorldClientBase`
* In the `serviceConnected` when clinets establishes the connection, it calls method `requestHelloWorl` to run on remote service.

## Load model

When services and clients are created, the developers can decide how to distribute service and client objects.
* The service and the client can run in the same thread of the same process.
* The service and the client can run in separate threads of the sam process.
* The service and the clinet can run in separate processes (_Public_ service case).

We'll consider all of them. The _model_ can be created either statically (fixed) or dynamically (can vary). Here we conside static models, since it is more readable and easier.

### Model with single thread

This example demonstrates how to instantiate service and clinet whith one thread (_Local_ servicing).
> 💡 In the example, there are 2 components declared in one thread.

```cpp
// main.cpp

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "ServiceClient.hpp"
#include "ServicingComponent.hpp"

constexpr char const _modelName[]  { "ServiceModel" };   //!< The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( "ServiceClient", ServiceClient )
            REGISTER_DEPENDENCY( "TheService" ) /* reference to the service*/
        END_REGISTER_COMPONENT( "ServiceClient" )

        BEGIN_REGISTER_COMPONENT( "TheService", ServicingComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
        END_REGISTER_COMPONENT( "TheService" )
    END_REGISTER_THREAD( "Thread1" )

// end of model description
END_MODEL(_modelName)

// main method
int main( void )
{
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, nullptr, nullptr );

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

### Model with multiple threads

This example demonstrates how to instantiate service and clinet running on separate threads, but in the same process (_Local_ servicing).

```cpp
// main.cpp

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "ServiceClient.hpp"
#include "ServicingComponent.hpp"

constexpr char const _modelName[]  { "ServiceModel" };   //!< The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( "ServiceClient", ServiceClient )
            REGISTER_DEPENDENCY( "TheService" )
        END_REGISTER_COMPONENT( "ServiceClient" )
    END_REGISTER_THREAD( "Thread1" )

    BEGIN_REGISTER_THREAD( "Thread2" )
        BEGIN_REGISTER_COMPONENT( "TheService", ServicingComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
        END_REGISTER_COMPONENT( "TheService" )
    END_REGISTER_THREAD( "Thread2" )

// end of model description
END_MODEL(_modelName)

// main method
int main( void )
{
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, nullptr, nullptr );

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

### Model with separate processes

This example demonstrates how to instantiate service and clinet running in separate processes (_Public_ servicing). It is required to have 2 projects with 2 `main()` functions.

> 💡 Note, the model and thread names in these 2 processes have same name, but the services differ.

In the Service provider process:
```cpp
// main.cpp, Service provider project

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "ServicingComponent.hpp"

constexpr char const _modelName[]  { "ServiceModel" };   //!< The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( "TheService", ServicingComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
        END_REGISTER_COMPONENT( "TheService" )
    END_REGISTER_THREAD( "Thread1" )

// end of model description
END_MODEL(_modelName)

// main method
int main( void )
{
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, nullptr, nullptr );

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

This is a source code of secont `component`.

```cpp
// main.cpp, Service provider project

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "ServiceClient.hpp"

constexpr char const _modelName[]  { "ServiceModel" };   //!< The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( "ServiceClient", ServiceClient )
            REGISTER_DEPENDENCY( "TheService" )
        END_REGISTER_COMPONENT( "ServiceClient" )
    END_REGISTER_THREAD( "Thread1" )

// end of model description
END_MODEL(_modelName)

// main method
int main( void )
{
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, nullptr, nullptr );

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
In both cases, the services are up and running when load model.
