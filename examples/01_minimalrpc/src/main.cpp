/**
 * \file    src/main.cpp
 * \brief   Minimal RPC example to call 'requestHelloService()' of remote object running in the same process, but in other thread.
 **/

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/01_minimalrpc/services/HelloServiceStub.hpp"
#include "examples/01_minimalrpc/services/HelloServiceClientBase.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "01_generated")
#endif // _MSC_VER

//!< Service Provider: ServiceProvider declaration
class ServiceProvider   : public    Component
                        , protected HelloServiceStub
{
public:
    ServiceProvider(const NERegistry::ComponentEntry& entry, ComponentThread& owner)
        : Component(entry, owner)
        , HelloServiceStub(static_cast<Component&>(self()))
    {   }

    //!< The request method of the HelloService Interface
    virtual void requestHelloService(void) override
    {
        std::cout << "\'Hello Service!\'" << std::endl;
        Application::signalAppQuit();   // quit application is if received response
    }

private:
    inline ServiceProvider& self(void)
    {   return (*this); }
};

//!< ServiceConsumer declaration
class ServiceConsumer   : public    Component
                        , protected HelloServiceClientBase
{
public:
    ServiceConsumer(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
		: Component             ( entry, owner )
		, HelloServiceClientBase( entry.mDependencyServices[0].mRoleName, owner )
	{   }

    //!< Service discovery notification. Called when the "ServiceProvder" is available and unavailable.
    //!< The `status` parameter contains availability flag. Return `true` if the service connection notification is relevant.
    virtual bool serviceConnected(NEService::eServiceConnection status, ProxyBase& proxy) override
    {
        if (HelloServiceClientBase::serviceConnected(status, proxy) && NEService::isServiceConnected(status))
            requestHelloService();  // Call of method of remote "ServiceProvider" object.
        // Return `true` if the service connection notification is relevant.
        return true;
    }
};

//////////////////////////////////////////////////////////////////////////
// Define the model to load and instantiate threads and objects
//////////////////////////////////////////////////////////////////////////
// Describe model, register the service provider and service consumer in 
// 2 different threads "Thread1" and "Thread2"
BEGIN_MODEL("ServiceModel")
    // Thread 1 without watchdog, contains a service provider
    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( "ServiceProvider", ServiceProvider )
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceProvider" )
    END_REGISTER_THREAD( "Thread1" )

    // Thread 2 without watchdog, contains a service consumer
    BEGIN_REGISTER_THREAD( "Thread2" )
        BEGIN_REGISTER_COMPONENT( "ServiceConsumer", ServiceConsumer )
            REGISTER_DEPENDENCY( "ServiceProvider" ) /* dependency reference to the remote service*/
        END_REGISTER_COMPONENT( "ServiceConsumer" )
    END_REGISTER_THREAD( "Thread2" )

// end of model description
END_MODEL("ServiceModel")

//!< main function
int main(void)
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog, using default settings.
    Application::initApplication();
    // load model to initialize components
    Application::loadModel("ServiceModel");
    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    // release and cleanup resources of application.
    Application::releaseApplication();
    return 0;
}
