//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project creates an instance of Public service. It also
//               defines and processes custom events.
//
//               When the application starts, use one of commands:
//                  - "start" to start the jobs in service.
//                  - "stop" to stop the jobs in service.
//                  - "quit" or "q", to exit applications.
//                    This also closes all clients.
//
//               Type "start" to start changing light states.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/GELog.h"
#include "aregextend/console/Console.hpp"

#include "common/NECommon.hpp"
#include "pubservice/src/TrafficLightService.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "aregextend.lib")
    #pragma comment(lib, "14_generated.lib")
#endif // WINDOWS

constexpr char const _modelName[]  { "TheModel" };   // The name of model
constexpr char const _threadName[] { "TestSimpleTrafficThread" };	// The name of component thread

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( _threadName, NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::SimpleLightControllerName, TrafficLightService )
            // register SimpleTrafficLight and SimpleTrafficSwitch service implementation.
            REGISTER_IMPLEMENT_SERVICE( NESimpleTrafficLight::ServiceName, NESimpleTrafficLight::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( NECommon::SimpleLightControllerName )
    // end of thread description
    END_REGISTER_THREAD( _threadName )

// end of model
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
//! A demo of a service to send data update notification (PubSub feature).
int main()
{
    // Initialize application, enable servicing, routing, timer and watchdog.
    Application::initApplication(true, true, true, true, true, nullptr);


    // load model to initialize components
    Application::loadModel(_modelName);

    // The components are initialized. Find the service component thread.
    // It is used to send custom event.
    Thread * thread = Thread::findThreadByName(_threadName);
    ASSERT(thread != nullptr );
    ASSERT(thread->isInstanceOfRuntimeClass("DispatcherThread"));

    bool doLoop = true;
    Console & console = Console::getInstance( );
    console.enableConsoleInput( true );

    std::string_view commands[]
    {
          { "quit"  }
        , { "stop"  }
        , { "start" }
    };

    console.outputTxt( { 0, 0 }, "A demo of a service to send data update notification (PubSub feature)...." );

    do 
    {
        // Wait and check the command. There are 3 options:
        //      - 'start'   to switch ON traffic light and start changing states.
        //      - 'stop'    to stop changing states and set traffic light OFF.
        //      - 'quit' or 'q' to quit application(s). This will also send signal to stop clients.
        console.outputTxt( { 0, 2 }, "============================================" );
        console.outputTxt( { 0, 3 }, "- Type \"start\" to start the traffic light." );
        console.outputTxt( { 0, 4 }, "- Type \"stop\"  to stop the traffic light." );
        console.outputTxt( { 0, 5 }, "- Type \"quit\"  to quit the traffic light." );
        console.outputTxt( { 0, 6 }, "Type command: " );
        console.refreshScreen( );
        console.waitForInput( [&]( const String & cmd ) -> bool
            {
                if ( cmd.compare( commands[0], false ) == NEMath::eCompare::Equal )
                {
                    // It is 'quit' --> quit application(s).
                    DispatcherThread * dispatcher = static_cast<DispatcherThread *>(thread);
                    TrafficSwitchEvent::sendEvent( TrafficSwitchData( false ), *dispatcher );
                    doLoop = false; // quit the loop
                }
                else if ( cmd.compare( commands[1], false ) == NEMath::eCompare::Equal )
                {
                    // It is 'stop' --> stop the traffic light.
                    DispatcherThread * dispatcher = static_cast<DispatcherThread *>(thread);
                    TrafficSwitchEvent::sendEvent( TrafficSwitchData( false ), *dispatcher );
                }
                else if ( cmd.compare( commands[2], false ) == NEMath::eCompare::Equal )
                {
                    // It is 'start' --> start the traffic light.
                    DispatcherThread * dispatcher = static_cast<DispatcherThread *>(thread);
                    TrafficSwitchEvent::sendEvent( TrafficSwitchData( true ), *dispatcher );
                }

                return true;
            });

        Thread::switchThread();

    } while (doLoop);
        
    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

	return 0;
}
