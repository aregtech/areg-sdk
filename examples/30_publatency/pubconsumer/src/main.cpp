//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : The application contains a Public service client software
//               component. It receives connected notification and starts 
//               calling remote method of the Public service. To make periodic
//               calls, it uses a timer.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/console/Console.hpp"

#include "pubconsumer/src/LatencyConsumer.hpp"
#include "common/latency_common.hpp"

#ifdef _MSC_VER

    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
    #pragma comment(lib, "30_generated")
    #pragma comment(lib, "winmm")

#endif // _MSC_VER

#if defined(_WIN32)
    #include <windows.h>
    #include <timeapi.h>
#endif // _WIN32

constexpr char const _modelName[]   = { "Latency" };  //!< The name of model
const areg::String   _serviceClient = areg::generate_name("ServiceConsumer"); //!< Generated name of service client component
constexpr uint32_t     CLIENT_EVENT_QUEUE_SIZE { 1024u };

//////////////////////////////////////////////////////////////////////////
//
// The following design instantiates large data servicing client.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD_EX2( "TestServiceConsumerThread", areg::WATCHDOG_IGNORE, areg::DEFAULT_STACK_SIZE, CLIENT_EVENT_QUEUE_SIZE, areg::Bool::Undefined, areg::WAIT_INFINITE )
        // register the latency consumer component
        BEGIN_REGISTER_COMPONENT( _serviceClient.as_string(), LatencyConsumer )
            REGISTER_DEPENDENCY( Latency::LATENCY_SERVICE_ROLE )
        END_REGISTER_COMPONENT( _serviceClient.as_string() )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceConsumerThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(examples_30_consumerlatency_main, main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing remote latency ultra-small consumer...\n");

#if defined(_WIN32)
    timeBeginPeriod(1u);    // raise Windows timer resolution to 1 ms for the benchmark duration
#endif // _WIN32

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr, false );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::setup( );

    do
    {
        LOG_SCOPE( examples_30_consumerlatency_main, main );
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
    printf("Completed testing remote latency consumer, check the logs...\n");

#if defined(_WIN32)
    timeEndPeriod(1u);
#endif // _WIN32

	return 0;
}
