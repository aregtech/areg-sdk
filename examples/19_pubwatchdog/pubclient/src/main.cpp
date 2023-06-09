//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : The application contains a Public service client software
//               component. It receives connected notification and starts 
//               calling remote method of the Public service. To make periodic
//               calls, it uses a timer.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/trace/GETrace.h"


#include "generated/src/NECommon.hpp"
#include "pubclient/src/ServiceClient.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg")
    #pragma comment(lib, "19_generated.lib")
#endif // WINDOWS

constexpr char const _modelName[]= { "HelloModel" };  //!< The name of model
const String     _serviceClient  = NEUtilities::generateName("ServiceClient"); //!< Generated name of service client component

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
        BEGIN_REGISTER_COMPONENT( _serviceClient, ServiceClient )
            // register service dependency
            REGISTER_DEPENDENCY(NECommon::ServiceRoleName )
        // end of component description
        END_REGISTER_COMPONENT( _serviceClient )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_19_pubclient_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing remote servicing ultra-small client...\n");
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    do
    {
        TRACE_SCOPE(example_19_pubclient_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        // load model to initialize components
        Application::loadModel(_modelName);

        TRACE_DBG("Servicing model is loaded");

        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(_modelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    printf("Completed testing remote servicing client, check the logs...\n");

	return 0;
}
