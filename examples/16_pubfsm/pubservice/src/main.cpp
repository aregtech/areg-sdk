//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project creates an instance of Public services that 
//               contain a finite state-machine ( FSM ). A local service 
//               interacts with the user to change the states of FSM, to run 
//               or stop.
//
//               When the application starts, use one of commands:
//                  - "on" to imitate the power ON.
//                  - "sart" to start the run of FSM (as traffic light 
//                    controller).
//                  - "stop" to stop the run of FSM (as traffic light 
//                    controller).
//                  - "off" to imitate power OFF.
//                  - "quit" or "q", to exit applications. This also closes 
//                    all clients.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "generated/src/NECommon.hpp"
#include "pubservice/src/TrafficLightService.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "16_generated.lib")
#endif // WINDOWS


constexpr char const _modelName[]  { "TheModel" };   // The name of model
constexpr char const _threadName[] { "TestSimpleTrafficThread" };	// The name of component thread

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( _threadName, NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::ServiceLightController, TrafficLightService )
            // register PowerManager and TrafficController service implementation, and the dependencies (client runs in the same thread).
            REGISTER_IMPLEMENT_SERVICE( NEPowerManager::ServiceName, NEPowerManager::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( NETrafficController::ServiceName, NETrafficController::InterfaceVersion )
            // The power controller local service and the client run in the same thread.
            REGISTER_DEPENDENCY(NECommon::ServiceLightController)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::SimpleLightControllerName )
    // end of thread description
    END_REGISTER_THREAD( _threadName )

// end of model
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The main method does not start the logging, enables service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    // load model to initialize components
    Application::loadModel(_modelName);

    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
        
    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

	return 0;
}
