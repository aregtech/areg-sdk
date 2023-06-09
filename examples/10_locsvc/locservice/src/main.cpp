//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : The application that instantiates a simple service with 
//               request, response, broadcast and subscription servicing 
//               features, where clients can dynamically subscribe and 
//               unsubscribe to data update messages during run-time.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"
#include "locservice/src/ServicingComponent.hpp"
#include "locservice/src/ServiceClient.hpp"

#ifdef _WIN32
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "10_generated.lib")
#endif // _WIN32

constexpr char const _modelName[]   { "TestModel" };            //!< The name of model
constexpr char const _roleName[]    { "TestServiceComponent" }; //!< Service role name

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate twice the same implementation
// of component, but running in different thread and having different
// role name. Note, no need to make changes in the code.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT(_roleName, ServicingComponent )
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT(_roleName)
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

    //////////////////////////////////////////////////////////////////////////
    //
    // Use of service component in the other thread.
    //
    //////////////////////////////////////////////////////////////////////////

    // define component thread
    BEGIN_REGISTER_THREAD( "TestClientThread", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "TestServiceClient", ServiceClient )
            REGISTER_DEPENDENCY( "TestServiceComponent" )
        // end of component description
        END_REGISTER_COMPONENT( "TestServiceClient" )
    // end of thread description
    END_REGISTER_THREAD( "TestClientThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(examples_10_locsvc_main);
//! \brief   A Demo to demonstrate simple request, response and broadcast.
int main()
{
    std::cout << "A Demo to demonstrate simple request, response and broadcast ..." << std::endl;

    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    Application::initApplication( true, true, false, true, true, nullptr, nullptr );

    do 
    {
        TRACE_SCOPE( examples_10_locsvc_main );
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        // load model to initialize components
        Application::loadModel(_modelName);

        TRACE_DBG("Servicing model is loaded");

        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(_modelName);

        std::cout
            << (Application::findModel( _modelName ).getAliveDuration( ) / NECommon::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    std::cout << "Exit application, check the logs for details!" << std::endl;
    return 0;
}
