//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : The application provides network discoverable Public service,
//               which predefined methods are called from remote clients.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include "pubservice/src/ServiceComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "24_generated")
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate servicing component.
//
//////////////////////////////////////////////////////////////////////////

constexpr char const _modelName[]{ "ServiceModel" };   //!< The name of model
constexpr char const _serviceName[]{ "UnblockRequestService" }; //!< The name of the service

// Describe mode, set model name
BEGIN_MODEL( _modelName )

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _serviceName, ServiceComponent )
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( HelloUnblock::ServiceName, HelloUnblock::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( _serviceName )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

// end of model description
END_MODEL( _modelName )

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE( example_24_pubunblock_pubservice_main_main );
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main( )
{
    printf( "Testing unblock request service ...\n" );

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::init_application( );

    do
    {
        LOG_SCOPE( example_24_pubunblock_pubservice_main_main );
        LOG_DBG( "The application has been initialized, loading model [ %s ]", _modelName );

        // load model to initialize components
        areg::Application::load_model( _modelName );

        LOG_DBG( "Servicing model is loaded" );

        // wait until Application quit signal is set.
        areg::Application::wait_app_quit( areg::WAIT_INFINITE );

        // stop and unload components
        areg::Application::unload_model( _modelName );

        // release and cleanup resources of application.
        areg::Application::release_application( );

    } while ( false );

    printf( "Completed test unblock service request. Check the logs...\n" );

    return 0;
}
