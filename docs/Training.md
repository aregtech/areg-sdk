# Hello Service
```
This file is part of AREG SDK
Copyright (c) 2021-2023, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```

This document is an practical example of creating applications with distributed services.

> 💡 The source codes of this example you can find in [00_helloservice](../examples/00_helloservice/) directory.

This tutorial provides a step-by-step guide for creating service-enabled applications using the AREG SDK. To make it easy, we recommend creating a new directory inside the AREG SDK's [examples](../examples] directory and naming it `helloservice`. This will serve as the root directory for your training projects and will be the working directory for the three sample projects included in this tutorial.

All three sample projects use the same service provider (server) and service consumer (client), located in the `common/src` subdirectory. The projects are designed to illustrate different ways of running the service provider and client:

- **Project #1** ([onethread](../examples/00_helloservice/onethread/)): the service provider and service consumer run in the same thread of the same process.
- **Project #2** ([twothreads](../examples/00_helloservice/twothreads/)): the service provider and service consumer run in different threads of the same process.
- **Project #3** ([multiprocess](../examples/00_helloservice/multiprocess/)): the service provider and service consumer run in different processes.

By following this tutorial and completing these sample projects, you'll gain a practical understanding of how to create service-enabled, multithreading, and multiprocessing applications using the AREG SDK. As well as understand the meaning of distributed services at the edge of IoT network (mist-network).

---

## Demonstrated features

Features demonstrated in these examples:

* Each project creates a model, which defines a service provider (_server_) and service consumer (_client_) that can run in the same thread, different threads of the same process, or different processes.
* Applications load and unload the model to start and stop the service.
* The server registers and the client discovers the service automatically.
* Once the service is available, the client sends a request to the server.
* The provider processes the request and sends a response.
* The client receives the response and initiates a quit process.
* In _project #1 and #2_, where the server and client run in the same process, the application quits after receiving the response. In _project #3_, the server continues to run, and the client application quits after receiving the response.
* In _project #3_, multiple clients can start and the starting order plays no role, because service discovery is automated.

When creating a service provider and service consumer, it is important to keep in mind the following notes:
1. The **Component** serves as the holder of the servicing objects (service provider and/or service consumer).
1. The **Component** can provide multiple services, multiple consumers or be a mixture of service providers and clients.
1. The same service provider can be instantiated multiple times, but instances should run in different threads.
1. The services differ by service _interface name_ and/or a _role name_, and the consumers refer to the service by _role name_ in the _model_.
1. Instances of the service provider and its consumer can be in the same thread.
1. The role names of the _Local_ services are unique within the same process, and the role names of the _Public_ services are unique within the same network.
1. Service consumers know the availability of the service via the notification callback `void serviceConnected(bool isConnected, ProxyBase& proxy)`, where `isConnected` parameter indicates the connection status.
1. Service providers know the consumer connection via the notification callback `void clientConnected(const ProxyAddress& client, bool isConnected)`, where `isConnected` parameter indicates the connection status.

---

## Showcase

The purpose of this topic is to demonstrate how to implement service providers and consumers, which can be split and merged across different processes and threads to distribute computing power. It is important to note that in the examples here, all projects use the same service provider and consumer objects, with variations in the **models** declared and used in the **main.cpp** files.

* The generated codes will be located in the `generated/src` subdirectory of `helloservice`, which will be created by the **code generator**.
* The declaration and implementation of the common service consumer (**server**) and service provider (**client**) will be located in the `common/src` subdirectory of `helloservice`, which should be created manually.
* The definition of the **Service Interface** document is located in `res` subdirectory of `helloservice`, which should be create manually.

To start, in the `helloservice` please create `common/src` and `res` subdirectories

### Service Interface

To create a multithreading and multiprocessing project with distributed services, follow these simple steps for creating a `HelloService` interface file:
1. In the created `helloservice/res' directory, create a new file called `HelloService.siml`.
2. Copy and paste the following XML code into the `HelloService.siml` file and save it:
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

In this example, in the section _*MethodList_ we defined methods **HelloService** request and its response **HelloService**, which are linked.

### Generate codes

> 💡 To ensure that the code generator runs successfully on your machine, it is essential to have [Java](https://java.com/) installed.

To generate base files for the `HelloService` service, follow these steps:
1. Open the command line terminal in the `helloservice` directory.
2. Run the following command:
```bash
java -jar ../../tools/codegen.jar --root=./ --doc=res/HelloService.siml --target=generated/src
```

This will run the `codegen.jar` and generate service provider and service consumer base files for the `HelloService` located in the `generated/src` subdirectory inside `helloservice`, and you will get the following generated codes:
* The generated service provider base classes end with `Stub`. In our case, it is `HelloServiceStub`.
* The generated service consumer base classes end with `ClientBase`. In our case, it is `HelloServiceClassBase`.
* The `Stub objects are extended to override and implement _requests_ of service provider.
* The `ClientBase` objects are extended to override and implement used _responses_, _broadcasts_ and _data update notifications_ of service consumers. For better error handling, developers should as well override and implement the _request failed_ methods of used requests.

### Develop the service provider

To develop the service provider (_server_) for the `HelloService` and share it with all example project considered in this manual, follow these steps:

1. Open previously created `helloservice/common/src` directory.
2. Create a new file called `ServiceComponent.hpp`.

> 💡 The declaration of the service provider object `ServiceComponent` can be found in the file [ServiceComponent.hpp](../examples/00_helloservice/common/src/ServiceComponent.hpp).
```cpp
 // common/src/ServiceComponent.hpp
#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/HelloServiceStub.hpp"

class ServiceComponent  : public    Component
                        , protected HelloServiceStub
{
protected:
    ServiceComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    virtual ~ServiceComponent(void) = default;

public:
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

protected:
    // override the pure virtual method in HelloServiceStub
    virtual void requestHelloService( const String & client ) override;

private:
    ServiceComponent & self( void )
    {   return (*this); }

private:
    // disable default, copy constructor and assignment operator
    ServiceComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceComponent );
};
```
3. Create a new file called `ServiceComponent.cpp`.
4. Copy and paste the following code into the `ServiceComponent.cpp` file, and save it:
> 💡 The implementation of the service provider object `ServiceComponent` can be found in the file [ServiceComponent.cpp](../examples/00_helloservice/common/src/ServiceComponent.cpp).

```cpp
 // common/src/ServiceComponent.cpp
#include "common/src/ServiceComponent.hpp"

#include <iostream>

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
    std::cout << "\'Hello Service!\' from " << client.getString() << std::endl; // output message
    responseHelloService(true); // reply to unblock the request
}
```

In this example:

* The **ServiceComponent** class is an instance of both **Component** and **HelloServiceStub** objects.
* The constructor and destructor are both protected, ensuring that they cannot be used outside of class.
* The service provider has static **CreateComponent** and **DeleteComponent** methods for creating and deleting instances of the service provider.
* The service provider overrides and implements the pure virtual method **requestHelloService** inherited from **HelloServiceStub**.
* The **requestHelloService** method replies with **responseHelloService** to the service consumer and unblocks the request for the next call.

### Develop the service consumer

To develop the service consumer (_client_) for the `HelloService` and share it with all example project considered in this manual, follow these steps:

1. Open previously created `helloservice/common/src` directory.
2. Create a new file called `ClientComponent.hpp`.
> 💡 The declaration of the service provider object `ClientComponent` can be found in the file [ClientComponent.hpp](../examples/00_helloservice/common/src/ClientComponent.hpp).

```cpp
// common/src/ClientComponent.hpp
#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/component/Component.hpp"
#include "generated/src/HelloServiceClientBase.hpp"

class ClientComponent   : public    Component
                        , protected HelloServiceClientBase
{
protected:
    ClientComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    virtual ~ClientComponent(void) = default;

public:
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

protected:

    // Override to get connection notification.
    virtual bool serviceConnected( bool isConnected, ProxyBase & proxy ) override;

    // Overrides of response of HelloServiceClientBase.
    virtual void responseHelloService( bool success ) override;

    // Overrides of request failure of HelloServiceClientBase.
    virtual void requestHelloServiceFailed( NEService::eResultType FailureReason ) override;

private:
    ClientComponent & self( void )
    {   return (*this); }

private:
    // disable default, copy constructor and assignment operator
    ClientComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ClientComponent );
};
```
3. Create a new file called `ClientComponent.cpp`.
5. Copy and paste the following code into the `ClientComponent.cpp` file, and save it:
> 💡 The implementation of the service provider object `ClientComponent` can be found in the file [ClientComponent.cpp](../examples/00_helloservice/common/src/ClientComponent.cpp).
```cpp
// common/src/ClientComponent.cpp
#include "common/src/ClientComponent.hpp"

#include "areg/base/Thread.hpp"
#include "areg/base/NECommon.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"

#include <iostream>

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
    std::cout << (success ? "Succeeded" : "Failed") << "to output message.\n" << std::endl;

    // Sleep for no reason, just to have a small delay to see the outputs on console.
    Thread::sleep(NECommon::WAIT_1_SECOND); 

    Application::signalAppQuit(); // set signal to quit application
}

void ClientComponent::requestHelloServiceFailed(NEService::eResultType FailureReason)
{
    // make error handling here.
    std::cerr << "Cannot execute request, fail reason " << NEService::getString(FailureReason) << std::endl;
}
```

In this example:

* The **ClientComponent** class is an instance of both **Component** and **HelloServiceClientBase** objects.
* The constructor and destructor are both protected, ensuring that they cannot be used outside of class.
* The service consumer has static **CreateComponent** and **DeleteComponent** methods for creating and deleting instances of the components.
* The service consumer object overrides **serviceConnected** method to get service available notification triggered by the system.
* The **requestHelloService** is called within **serviceConnected** method immediately when received _service available_ system notification.
* The service consumer overrides the **responseHelloService** virtual methods inherited from **HelloServiceClientBase** to react on the reply from service, where it outputs the result and signals to quit the application.
* The service consumer overrides the **requestHelloServiceFailed** virtual methods inherited from **HelloServiceClientBase** to react on request failure and make better error handling.

### Model

The AREG engine employs a service-oriented architecture to organize the relationships between threads, service providers and service consumers, and these are defined in a **model**. There are two types of models: _static_, which are predefined, and dynamic, which are created at runtime. An application can have multiple models, and developers can load specific models at runtime to automatically create threads, components, and services.

Each thread must have at least one component, and each component must have at least one service provider or service consumer object. Components can extend or contain service provider/consumer objects, and the model should list the provided and consumed services. When a model is no longer needed, it can be unloaded, and the system will stop all threads and shut down all services registered in that model.

To declare _static models_ in an application, follow these guidelines:

- Each application must have at least one model with a unique name.
- To declare a model, use the **BEGIN_MODEL** and **END_MODEL** macro and specify the name of the model.
- Each model must have at least one thread with a unique name.
- To declare a thread, use the **BEGIN_REGISTER_THREAD** and **END_REGISTER_THREAD** macro and specify the name of the thread. You can also specify a _watchdog timeout_ in the **BEGIN_REGISTER_THREAD** macro.
- Each thread must have at least one component with a unique name (known as _Role Name_). 
    - If the component provides or consumes only _Local_ services, the component name should be unique within the application.
    - If the component provides or consumes at least one _Public_ service, the component should have a unique name within the entire network.
- To declare a component, use the **BEGIN_REGISTER_COMPONENT** and **END_REGISTER_COMPONENT** macro, specify the component _Role Name_, and in the **BEGIN_REGISTER_COMPONENT** macro, specify the name of the component object that is bound to that _Role_.
- Each component must have at least one provided or consumed service interface.
- To declare a _Service Provider_, use the **REGISTER_IMPLEMENT_SERVICE** macro and specify the name of the implemented _Service Interface_.
    - A component can provide multiple Service _Interfaces_, and all should be unique within the same component.
    - The same _Service Interface_ can be used multiple times, but should be unique within one thread.
- To declare a _Service Consumer_, use the **REGISTER_DEPENDENCY** macro and specify the _Role Name_ of the dependent component.
    - A component can have multiple consumers of the same Service Interface, and the dependency only needs to be declared once.
    - A component can provide and consume the same Service Interface.

> 💡 Note that provided services are identified by their _Service Interface Name_. On the other hand, when accessing used services, the consumer uses the _Role Name_ of the component. This is necessary because a single _Service Interface_ can have multiple implementations, and the consumer needs to reference the specific component by its unique name, which is the _Role Name_ of the service provider component.

The source codes of the examples can be found in the  [00_helloservice](../examples/00_helloservice/) directory. All example projects use the same [ServiceComponent](../examples/00_helloservice/common/src/ServiceComponent.hpp) and [ClientComponent](../examples/00_helloservice/common/src/ClientComponent.hpp) objects.

> 💡 **IMPORTANT:** All example projects must be linked with **areg library** (_shared_ or _static_).

### Project [`onethread`](../examples/00_helloservice/onethread/)

> 💡 The sources of the example are available in the [this file](../examples/00_helloservice/onethread/src/main.cpp).

In this example, we define a _model_ where both service provider and consumer are running in the same thread of the same process. The service is automatically started when the model is loaded. Create `onethread` project, create `main.cpp` file, copy and paste following code, compile and run:
```cpp
// onethread/src/main.cpp

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

// Define the model and describe components
BEGIN_MODEL("ServiceModel")

    BEGIN_REGISTER_THREAD( "Thread1", NECommon::WATCHDOG_IGNORE )
        // register service provider component
        BEGIN_REGISTER_COMPONENT("ServiceComponent", ServiceComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceComponent" )
        
        // register service consumer component
        BEGIN_REGISTER_COMPONENT( "ServiceClient", ClientComponent )
            REGISTER_DEPENDENCY( "ServiceComponent" )
        END_REGISTER_COMPONENT( "ServiceClient" )
    END_REGISTER_THREAD( "Thread1" )

END_MODEL("ServiceModel")

int main( void )
{
    constexpr char const model[]{ "ServiceModel" };
    
    Application::initApplication( );  // initialize internals
    Application::loadModel(model);    // load model, initialize components
    Application::waitAppQuit();       // wait to receive quit signal
    Application::unloadModel(model);  // stop and unload components
    Application::releaseApplication();// release resources

    return 0;
}
```

### Project [`twothreads`](../examples/00_helloservice/twothreads/)

> 💡 The sources of the example are available in the [this file](../examples/00_helloservice/twothreads/src/main.cpp).

In this example, we define a _model_ where both service provider and consumer are running in the separate threads of the same process. The service is automatically started when the model is loaded.  Create `onethread` project, create `main.cpp` file, copy and paste following code, compile and run:
```cpp
// twothreads/src/main.cpp

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

// Define the model and describe components
BEGIN_MODEL("ServiceModel")
    BEGIN_REGISTER_THREAD( "Thread1", NECommon::WATCHDOG_IGNORE )
        // register service provider component
        BEGIN_REGISTER_COMPONENT( "ServiceComponent", ServiceComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceComponent" )
    END_REGISTER_THREAD( "Thread1" )

    BEGIN_REGISTER_THREAD( "Thread2", NECommon::WATCHDOG_IGNORE )
        // register service consumer component
        BEGIN_REGISTER_COMPONENT( "ServiceClient", ClientComponent )
            REGISTER_DEPENDENCY( "ServiceComponent" )
        END_REGISTER_COMPONENT( "ServiceClient" )
    END_REGISTER_THREAD( "Thread2" )

END_MODEL("ServiceModel")

int main( void )
{
    constexpr char const model[]{ "ServiceModel" };
    
    Application::initApplication( );  // initialize internals
    Application::loadModel(model);    // load model, initialize components
    Application::waitAppQuit();       // wait to receive quit signal
    Application::unloadModel(model);  // stop and unload components
    Application::releaseApplication();// release resources

    return 0;
}
```

### Project [`multiprocess`](../examples/00_helloservice/multiprocess/)

In this example, we define a _model_ where service provider and service consumer are running in the separate processes. The services provider and consumer automatically started when the model is loaded. For each process (application) we need to create separate project.

#### [`serviceproc`](../examples/00_helloservice/multiprocess/serviceproc/) service provider application

> 💡 The sources of the example are available in the [this file](../examples/00_helloservice/multiprocess/serviceproc/src/main.cpp).

Since this application only provides the service, we define a model and register the service provider component. Create `serviceproc` project, create `main.cpp` file, copy and paste following code, and compile:
```cpp
// multiprocess/serviceproc/src/main.cpp

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"

// Define the model and describe service provide component
BEGIN_MODEL("ServiceModel")

    BEGIN_REGISTER_THREAD( "aThread", NECommon::WATCHDOG_IGNORE )
        BEGIN_REGISTER_COMPONENT( "ServiceComponent", ServiceComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceComponent" )
    END_REGISTER_THREAD( "aThread" )

END_MODEL("ServiceModel")

int main( void )
{
    constexpr char const model[]{ "ServiceModel" };
    
    Application::initApplication( );  // initialize internals
    Application::loadModel(model);    // load model, initialize components
    Application::waitAppQuit();       // wait to receive quit signal
    Application::unloadModel(model);  // stop and unload components
    Application::releaseApplication();// release resources

    return 0;
}
```

#### [`clientproc`](../examples/00_helloservice/multiprocess/clientproc/) service consumer application

> 💡 The sources of the example are available in the [this file](../examples/00_helloservice/multiprocess/clientproc/src/main.cpp).

Since this application only consumes the service, we define a model and register the service consumer component. Create `clientproc` project, create `main.cpp` file, copy and paste following code, and compile:
```cpp
// multiprocess/clientproc/src/main.cpp

#include "areg/base/GEGlobal.h"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/String.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ClientComponent.hpp"

// Generate unique names to run multiple service consumer instances.
const String _client(NEUtilities::generateName("ServiceClient"));

// Define the model and describe service consumer component
BEGIN_MODEL("ServiceModel")

    BEGIN_REGISTER_THREAD( "aThread", NECommon::WATCHDOG_IGNORE )
        BEGIN_REGISTER_COMPONENT( _client, ClientComponent )
            REGISTER_DEPENDENCY( "ServiceComponent" )
        END_REGISTER_COMPONENT( _client )
    END_REGISTER_THREAD( "aThread" )

END_MODEL("ServiceModel")

int main( void )
{
    constexpr char const model[]{ "ServiceModel" };
    
    Application::initApplication( );  // initialize internals
    Application::loadModel(model);    // load model, initialize components
    Application::waitAppQuit();       // wait to receive quit signal
    Application::unloadModel(model);  // stop and unload components
    Application::releaseApplication();// release resources

    return 0;
}
```

#### Test `multiprocess`

After successfully compiling **serviceproc** and **clientproc**, you can now start and establish communication between the processes. To begin, start [**mcrouter**](../framework/mcrouter/) locally, then start **serviceproc** and **clientproc**. Keep in mind that:

- The order in which the processes are started is not important.
- Only one instance of **serviceproc** should be started, as we have created a fixed-name _Public_ service.
- Multiple instances of **clientproc** can be started simultaneously, as we generate a unique name for each service consumer.
- If you configure [_router.init_](../framework/mcrouter/resources/router.init) properly and specify the _IP address_ of **mcrouter**, these processes can also run on remote machines.
- Even if **serviceproc** and **clientproc** are compiled for different platforms, they can still communicate by establishing a remote connection with **mcrouter**.

By following these guidelines, you can easily and effectively develop multithreading and multiprocessing applications while ensuring seamless communication between them. It is possible to create multiple services and distribute them throughout the network, allowing connected Things to serve as lightweight service providers and consumers. Computing power can be easily distributed among processes and machines, and applications can be developed for multiple platforms, even mix the platforms. Creating simulation applications is also simplified by focusing on the Service Interface API. Additionally, you have the flexibility to decide whether to provide services as _Local_ or _Public_.
