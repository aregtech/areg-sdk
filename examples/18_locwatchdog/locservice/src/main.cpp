//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
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

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "18_generated.lib")
#endif // WINDOWS

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
    BEGIN_REGISTER_THREAD( "TestServiceThread", NEHelloWatchdog::TimeoutWatchdog)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT(_roleName, ServicingComponent )
            // register HelloWatchdog service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEHelloWatchdog::ServiceName, NEHelloWatchdog::InterfaceVersion )
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
    BEGIN_REGISTER_THREAD( "TestClientThread", NECommon::INVALID_TIMEOUT)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "TestServiceClient", ServiceClient )
            REGISTER_DEPENDENCY(_roleName)
        // end of component description
        END_REGISTER_COMPONENT( "TestServiceClient" )
    // end of thread description
    END_REGISTER_THREAD( "TestClientThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing Watchdog on local services\n");
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, timer and watchdog.
    Application::initApplication(true, true, false, true, true );

    do
    {
        TRACE_SCOPE(main_main);
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

    printf("Completed testing watchdog with timeout...\n");

	return 0;
}
