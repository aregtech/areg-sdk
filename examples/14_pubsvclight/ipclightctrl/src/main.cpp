//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
// Description : A simple traffic light controller, Ansi-style
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "generated/src/NESimpleLightCommon.hpp"
#include "ipclightctrl/src/SimpleTrafficLightCtrl.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "14_generated.lib")
#endif // WINDOWS

/**
 * \brief   This file demonstrates simple initialization of local and remote servicing component
 *          used in multiprocessing application development. This executable contains main
 *          controlling component to shutdown system.
 **/

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate twice the same implementation
// of component, but running in different thread and having different
// role name. Note, no need to make changes in the code.
//
//////////////////////////////////////////////////////////////////////////

static const char * const _modelName    = "TheModel";   // The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestSimpleTrafficThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NESimpleLightCommon::SimpleLightControllerName, SimpleTrafficLightCtrl )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( NESimpleTrafficLight::ServiceName, NESimpleTrafficLight::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( NESimpleTrafficSwitch::ServiceName, NESimpleTrafficSwitch::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( NESimpleLightCommon::SimpleLightControllerName )
    // end of thread description
    END_REGISTER_THREAD( "TestSimpleTrafficThread" )

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
    // Initialize application, disable logging, enables servicing and the timer.
    Application::initApplication(false, true, true, true, NULL, NULL);


    // load model to initialize components
    Application::loadModel(_modelName);

    // The components are initialized. Find the service component thread.
    // It is used to send custom event.
    Thread * thread = Thread::findThreadByName("SimpleTrafficThread");
    ASSERT(thread != NULL);
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

#ifdef _WINDOWS
        scanf_s("%31s", command, 32);
#else   // _POSIX
        scanf("%31s", command);
#endif	// _WINDOWS


        if ( NEString::compareFastIgnoreCase<char, char>("start", command) == 0 )
        {
            // It is 'start' --> start the traffic light.
            DispatcherThread * dispatcher = static_cast<DispatcherThread *>(thread);
            TrafficSwitchEvent::sendEvent( TrafficSwitchData(true), *dispatcher);
        }
        else if ( NEString::compareFastIgnoreCase<char, char>("stop", command) == 0 )
        {
            // It is 'stop' --> stop the traffic light.
            DispatcherThread * dispatcher = static_cast<DispatcherThread *>(thread);
            TrafficSwitchEvent::sendEvent( TrafficSwitchData(false), *dispatcher);
        }
        else if ( (NEString::compareFastIgnoreCase<char, char>("quit", command) == 0 ) || (NEString::compareFastIgnoreCase<char, char>("q", command) == 0 ) )
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
