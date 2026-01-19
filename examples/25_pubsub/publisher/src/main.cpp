//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : The application provides network discoverable Public service,
//               which predefined methods are called from remote clients.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/GELog.h"

#include "publisher/src/Publisher.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "25_generated")
    #pragma comment(lib, "aregextend")
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate servicing component.
//
//////////////////////////////////////////////////////////////////////////

constexpr char const _modelName[]   { "PubSub_model" }; //!< The name of model
constexpr char const _serviceName[] { "Publisher" };    //!< The name of the service

// Describe mode, set model name
BEGIN_MODEL( _modelName )

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _serviceName, Publisher )
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEPubSub::ServiceName, NEPubSub::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( _serviceName )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

// end of model description
END_MODEL( _modelName )

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(example_25_pubsub_publisher_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main( )
{
    printf( "Testing PubSub featured service ...\n" );

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    do
    {
        LOG_SCOPE( example_25_pubsub_publisher_main_main );
        LOG_DBG( "The application has been initialized, loading model [ %s ]", _modelName );

        // load model to initialize components
        Application::loadModel( _modelName );

        LOG_DBG( "Servicing model is loaded" );

        // wait until Application quit signal is set.
        Application::waitAppQuit( NECommon::WAIT_INFINITE );

        // release and cleanup resources of application.
        Application::releaseApplication( );

    } while ( false );

    printf( "Completed PubSub featured service. Check the logs...\n" );

    return 0;
}
