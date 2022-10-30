//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2022 Aregtech UG.All rights reserved.
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
#include "areg/trace/GETrace.h"

#include "generated/src/NECommon.hpp"
#include "pubservice/src/TrafficLightService.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "14_generated.lib")
#endif // WINDOWS

#ifdef _WINDOWS
    #define MACRO_SCANF(fmt, data, len)     scanf_s(fmt, data, len)
#else   // _POSIX
    #define MACRO_SCANF(fmt, data, len)     scanf(fmt, data)
#endif  // _WINDOWS


constexpr char const _modelName[]  { "TheModel" };   // The name of model
constexpr char const _threadName[] { "TestSimpleTrafficThread" };	// The name of component thread

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( _threadName, NECommon::INVALID_TIMEOUT)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::SimpleLightControllerName, TrafficLightService )
            // register SimpleTrafficLight and SimpleTrafficSwitch service implementation.
            REGISTER_IMPLEMENT_SERVICE( NESimpleTrafficLight::ServiceName, NESimpleTrafficLight::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( NESimpleTrafficSwitch::ServiceName, NESimpleTrafficSwitch::InterfaceVersion )
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
    // Initialize application, enable servicing, routing, timer and watchdog.
    Application::initApplication(false, true, true, true, true, nullptr, nullptr );


    // load model to initialize components
    Application::loadModel(_modelName);

    // The components are initialized. Find the service component thread.
    // It is used to send custom event.
    Thread * thread = Thread::findThreadByName(_threadName);
    ASSERT(thread != nullptr );
    ASSERT(thread->isInstanceOfRuntimeClass("DispatcherThread"));

    bool doLoop         = true;

    do 
    {
        // Wait and check the command. There are 3 options:
        //      - 'start'   to switch ON traffic light and start changing states.
        //      - 'stop'    to stop changing states and set traffic light OFF.
        //      - 'quit' or 'q' to quit application(s). This will also send signal to stop clients.
        char command[32]    = {0};

        printf("\n===============================\n");
        printf("- Type \"start\" to start traffic light.\n");
        printf("- Type \"stop\" to stop the traffic light.\n");
        printf("- Type \"quit\" (or \'q\') to stop application.\n");
        printf("Type command: ");

        if ( MACRO_SCANF("%31s", command, 32) != 1)
        {
            // should never happen, but returned code from scanf must be checked
            printf("\nERROR: Invalid Choice, Quit example application ...\n");

            // wrong option, quit application.
            break;
        }


        if ( NEString::compareFastIgnoreCase<char, char>("start", command) == NEMath::eCompare::Equal)
        {
            // It is 'start' --> start the traffic light.
            DispatcherThread * dispatcher = static_cast<DispatcherThread *>(thread);
            TrafficSwitchEvent::sendEvent( TrafficSwitchData(true), *dispatcher);
        }
        else if ( NEString::compareFastIgnoreCase<char, char>("stop", command) == NEMath::eCompare::Equal)
        {
            // It is 'stop' --> stop the traffic light.
            DispatcherThread * dispatcher = static_cast<DispatcherThread *>(thread);
            TrafficSwitchEvent::sendEvent( TrafficSwitchData(false), *dispatcher);
        }
        else if ( (NEString::compareFastIgnoreCase<char, char>("quit", command) == NEMath::eCompare::Equal) || 
                  (NEString::compareFastIgnoreCase<char, char>("q", command) == NEMath::eCompare::Equal) )
        {
            // It is 'quit' --> quit application(s).
            DispatcherThread * dispatcher = static_cast<DispatcherThread *>(thread);
            TrafficSwitchEvent::sendEvent( TrafficSwitchData(false), *dispatcher);
            doLoop = false; // quit the loop
        }
        else
        {
            printf("Unrecognized command!\n");
        }

        Thread::switchThread();

    } while (doLoop);
        
    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

	return 0;
}
