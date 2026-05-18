//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : The application contains a Public service client software
//               component. It receives connected notification and starts 
//               calling remote method of the Public service. To make periodic
//               calls, it uses a timer.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/logging/areg_log.h"


#include "common/WatchdogDefs.hpp"
#include "pubclient/src/ServiceClient.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "22_generated")
#endif // _MSC_VER

constexpr char const _modelName[]= { "HelloModel" };  //!< The name of model
const areg::String     _serviceClient  = areg::generate_name("ServiceClient"); //!< Generated name of service client component

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
    BEGIN_REGISTER_THREAD( "TestServiceThread")
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _serviceClient, ServiceClient )
            // register service dependency
            REGISTER_DEPENDENCY(watchdog::ServiceRoleName )
        // end of component description
        END_REGISTER_COMPONENT( _serviceClient )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(examples_22_pubclient_main, main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing remote servicing ultra-small client...\n");
    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr, false );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::setup( );

    do
    {
        LOG_SCOPE( examples_22_pubclient_main, main );
        LOG_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        // load model to initialize components
        areg::Application::load_model(_modelName);

        LOG_DBG("Servicing model is loaded");

        // wait until Application quit signal is set.
        areg::Application::wait_quit(areg::WAIT_INFINITE);

        // stop and unload components
        areg::Application::unload_model(_modelName);

        // release and cleanup resources of application.
        areg::Application::release();

    } while (false);

    printf("Completed testing remote servicing client, check the logs...\n");

	return 0;
}
