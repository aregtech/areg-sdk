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


#include "common/LargeDataDefs.hpp"
#include "pubclient/src/ServiceClient.hpp"

#ifdef _MSC_VER

    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
    #pragma comment(lib, "23_generated")

#endif // _MSC_VER

constexpr char const _modelName[]   = { "DataRate" };  //!< The name of model
const areg::String   _serviceClient = areg::generate_name("ServiceConsumer"); //!< Generated name of service client component
constexpr uint32_t     CLIENT_EVENT_QUEUE_SIZE { areg::QUEUE_SIZE_MAXIMUM };

//////////////////////////////////////////////////////////////////////////
//
// The following design instantiates large data servicing client.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD_EX2( "TestServiceConsumerThread", areg::WATCHDOG_IGNORE, areg::DEFAULT_STACK_SIZE, CLIENT_EVENT_QUEUE_SIZE )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _serviceClient, ServiceClient )
            // register service dependency
            REGISTER_DEPENDENCY(LargeData::ServiceRoleName )
        // end of component description
        END_REGISTER_COMPONENT( _serviceClient )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceConsumerThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(examples_23_clientdatarate_main, main);
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
        LOG_SCOPE( examples_23_clientdatarate_main, main );
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

    // Release the console before writing directly to stdout so the final message
    // appears at the right position (just below the last output row), not mid-screen.
    areg::ext::Console::instance().uninitialize();
    printf("Completed testing remote servicing client, check the logs...\n");

	return 0;
}
