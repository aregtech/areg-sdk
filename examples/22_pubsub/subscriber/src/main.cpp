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

#include "subscriber/src/Subscriber.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg")
    #pragma comment(lib, "22_generated.lib")
    #pragma comment(lib, "areg-extend.lib")
#endif // WINDOWS

constexpr char const _modelName[]  { "PubSub_model" };  //!< The name of model
constexpr char const _serviceName[]{ "Publisher" };     //!< The name of the service
const String         _subscriber (NEUtilities::generateName("Subscriber")); //!< Generated name of service client component

//////////////////////////////////////////////////////////////////////////
//
// The following design instantiates large data servicing client.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _subscriber, Subscriber )
            // register service dependency
            REGISTER_DEPENDENCY( _serviceName )
        // end of component description
        END_REGISTER_COMPONENT( _subscriber )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_22_pubsub_subscriber_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing PubSub service consumer...\n");
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    do
    {
        TRACE_SCOPE(example_22_pubsub_subscriber_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        // load model to initialize components
        Application::loadModel(_modelName);

        TRACE_DBG("Servicing model is loaded");

        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    printf("Completed testing PubSub service feature, check the logs...\n");

	return 0;
}
