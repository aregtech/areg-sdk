/**
 * \file    src/main.cpp
 * \brief   Minimal RPC example to call 'hello_service()' of remote object running in the same process, but in other thread.
 **/

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/01_minimalrpc/services/HelloServiceProviderBase.hpp"
#include "examples/01_minimalrpc/services/HelloServiceConsumerBase.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "01_generated")
#endif // _MSC_VER

//!< Service Provider: ServiceProvider declaration
class ServiceProvider final : public    areg::Component
                            , protected HelloServiceProviderBase
{
public:
    ServiceProvider(const areg::ComponentEntry& entry, areg::ComponentThread& owner)
        : areg::Component(entry, owner)
        , HelloServiceProviderBase(static_cast<areg::Component&>(self()))
    {   }

    //!< The request method of the HelloService Interface
    void request_hello_service() final
    {
        std::cout << "\'Hello Service!\'" << std::endl;
        areg::Application::signal_quit();   // quit application is if received response
    }

private:
    inline ServiceProvider& self()
    {   return (*this); }
};

//!< ServiceConsumer declaration
class ServiceConsumer final : public    areg::Component
                            , protected HelloServiceConsumerBase
{
public:
    ServiceConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
		: areg::Component             ( entry, owner )
		, HelloServiceConsumerBase( entry.mDependencyServices[0].mRoleName, owner )
	{   }

    //!< Service discovery notification. Called when the "ServiceProvder" is available and unavailable.
    //!< The `status` parameter contains availability flag. Return `true` if the service connection notification is relevant.
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase& proxy) final
    {
        if (HelloServiceConsumerBase::service_connected(status, proxy) && areg::is_service_connected(status))
            request_hello_service();  // Call of method of remote "ServiceProvider" object.
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
            REGISTER_IMPLEMENT_SERVICE( HelloService::ServiceName, HelloService::InterfaceVersion )
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
int main()
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog, using default settings.
    areg::Application::setup();
    // load model to initialize components
    areg::Application::load_model("ServiceModel");
    // wait until Application quit signal is set.
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    // release and cleanup resources of application.
    areg::Application::release();
    return 0;
}
