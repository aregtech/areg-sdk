/**
 * \file    twothreads/src/main.cpp
 * \brief   Runs service and the client in one thread.
 **/
#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef MS_VISUAL_CPP
    #pragma comment(lib, "areg")
    #pragma comment(lib, "00_generated.lib")
#endif // MS_VISUAL_CPP

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
