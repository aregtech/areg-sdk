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

#include "generated/src/NECommon.hpp"
#include "pubservice/src/ServicingComponent.hpp"

#ifdef _WIN32
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "12_generated.lib")
#endif // _WIN32

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate twice the same implementation
// of component, but running in different thread and having different
// role name. Note, no need to make changes in the code.
//
//////////////////////////////////////////////////////////////////////////

constexpr char const _modelName[]  { "ServiceModel" };   //!< The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread", NECommon::WATCHDOG_IGNORE)
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
//! \brief   A Demo public service to process requests, and send response and broadcast.
int main()
{
    std::cout << "A Demo public service to process requests, and send response and broadcast ..." << std::endl;

    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application use default settings: enable logging, servicing, routing, timer and watchdog.
    Application::initApplication( );

    do 
    {
        TRACE_SCOPE(example_12_pubservice_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        // load model to initialize components
        Application::loadModel( _modelName );
        TRACE_DBG( "Servicing model is loaded" );
        // wait until Application quit signal is set.
        Application::waitAppQuit( NECommon::WAIT_INFINITE );
        // stop and unload components
        Application::unloadModel( _modelName );

        std::cout
            << (Application::findModel( _modelName ).getAliveDuration( ) / NECommon::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        // release and cleanup resources of application.
        Application::releaseApplication( );

    } while (false);

    std::cout << "Exit application, check the logs for details!" << std::endl;
    return 0;
}
