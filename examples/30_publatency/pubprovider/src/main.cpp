//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : The application provides network discoverable Public service,
//               which predefined methods are called from remote clients.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/String.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include "aregextend/console/Console.hpp"

#include "pubprovider/src/LatencyProvider.hpp"
#include "examples/30_publatency/services/Latency.hpp"
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

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate servicing component.
//
//////////////////////////////////////////////////////////////////////////

constexpr char const _modelName[]  { "ServiceModel" };   //!< The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceProviderThread" )
        // register the latency provider component
        BEGIN_REGISTER_COMPONENT( Latency::LATENCY_SERVICE_ROLE, LatencyProvider )
            REGISTER_IMPLEMENT_SERVICE( Latency::ServiceName, Latency::InterfaceVersion )
        END_REGISTER_COMPONENT(Latency::LATENCY_SERVICE_ROLE )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceProviderThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(examples_30_pubprovider_main, main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main(int argc, char* argv[])
{
    printf("Testing latency service provider component, run as a ultra-small service provider...\n");

#if defined(_WIN32)
    timeBeginPeriod(1u);    // raise Windows timer resolution to 1 ms for the benchmark duration
#endif // _WIN32

    // Parse command-line arguments so the benchmark can start unattended.
    // Example: 23_pubservice -t=0 -l=1 -c=10 -s
    if (argc > 1)
    {
        areg::String cmd;
        for (int i = 1; i < argc; ++i)
        {
            if (i > 1)
            {
                cmd += " ";
            }

            cmd += argv[i];
        }

        cmd.make_lower();
    }

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr, false );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::setup( );

    do
    {
        LOG_SCOPE(examples_30_pubprovider_main, main );
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
    printf("Completed testing latency service provider component. Check the logs...\n");

#if defined(_WIN32)
    timeEndPeriod(1u);
#endif // _WIN32

	return 0;
}
