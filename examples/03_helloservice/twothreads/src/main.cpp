/**
 * \file    twothreads/src/main.cpp
 * \brief   Runs service and the client in one thread.
 **/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "03_generated")
#endif // _MSC_VER

//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };

// Describe model, register the service and the client in 2 different threads "Thread1" and "Thread2"
BEGIN_MODEL(_modelName)
    // Thread 1, provides a service
    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( "ServiceComponent", ServiceComponent )
            REGISTER_IMPLEMENT_SERVICE( HelloService::ServiceName, HelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceComponent" )
    END_REGISTER_THREAD( "Thread1" )

    // Thread 2, is a service client.
    BEGIN_REGISTER_THREAD( "Thread2" )
        BEGIN_REGISTER_COMPONENT( "ServiceClient", ClientComponent )
            REGISTER_DEPENDENCY( "ServiceComponent" ) /* reference to the service*/
        END_REGISTER_COMPONENT( "ServiceClient" )
    END_REGISTER_THREAD( "Thread2" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////

int main()
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::init_application( );

    // load model to initialize components
    areg::Application::load_model(_modelName);

    // wait until Application quit signal is set.
    areg::Application::wait_app_quit(areg::WAIT_INFINITE);

    // stop and unload components
    areg::Application::unload_model(_modelName);

    // release and cleanup resources of application.
    areg::Application::release_application();

    return 0;
}
