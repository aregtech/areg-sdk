//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : The application provides network discoverable Public service,
//               which predefined methods are called from remote clients.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "pubservice/src/ServiceComponent.hpp"

#ifdef WIN32
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "21_generated.lib")
#endif // WINDOWS

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
    BEGIN_REGISTER_THREAD( "TestServiceThread", NECommon::WATCHDOG_IGNORE )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _serviceName, ServiceComponent )
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEHelloUnblock::ServiceName, NEHelloUnblock::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( _serviceName )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

// end of model description
END_MODEL( _modelName )

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE( example_21_pubunblock_pubservice_main_main );
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main( )
{
    printf( "Testing unblock request service ...\n" );

    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    do
    {
        TRACE_SCOPE( example_21_pubunblock_pubservice_main_main );
        TRACE_DBG( "The application has been initialized, loading model [ %s ]", _modelName );

        // load model to initialize components
        Application::loadModel( _modelName );

        TRACE_DBG( "Servicing model is loaded" );

        // wait until Application quit signal is set.
        Application::waitAppQuit( NECommon::WAIT_INFINITE );

        // stop and unload components
        Application::unloadModel( _modelName );

        // release and cleanup resources of application.
        Application::releaseApplication( );

    } while ( false );

    printf( "Completed test unblock service request. Check the logs...\n" );

    return 0;
}
