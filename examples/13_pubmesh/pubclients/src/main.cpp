//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
// Description : This project contains multiple instances of Public service
//               clients to invoke remote method calls.The system guarantees
//               that the each call is delivered to the target and the
//               messages are not mixed or lost. The application shuts down
//               when the 'main' service completes the job and exits.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "generated/src/NECommon.hpp"
#include "pubclients/src/LocalServiceComponent.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "13_generated.lib")
#endif // WINDOWS


// Describe mode, set model name
BEGIN_MODEL(NECommon::ModelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestClientProcessThread" )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::LocalService, LocalServiceComponent )
            // register LocalHelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NELocalHelloWorld::ServiceName, NELocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::MainService)
            REGISTER_DEPENDENCY(NECommon::PublicSecondaryService)
            REGISTER_DEPENDENCY(NECommon::PublicThirdService)
            REGISTER_DEPENDENCY(NECommon::LocalService)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::LocalService )

    // end of thread description
    END_REGISTER_THREAD( "TestClientProcessThread" )

// end of model NECommon::ModelName
END_MODEL(NECommon::ModelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_13_pubclients_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, nullptr, nullptr );

    do
    {
        TRACE_SCOPE(example_13_pubclients_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", NECommon::ModelName);

        // load model to initialize components
        Application::loadModel(NECommon::ModelName);

        TRACE_DBG("Servicing model is loaded");

        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(NECommon::ModelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    return 0;
}
