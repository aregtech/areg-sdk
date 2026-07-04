//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
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

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/console/Console.hpp"

#include "common/TrafficDefs.hpp"
#include "pubservice/src/TrafficLightService.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
    #pragma comment(lib, "17_generated")
#endif // _MSC_VER

constexpr char const _modelName[]  { "TheModel" };   // The name of model
constexpr char const _threadName[] { "TestSimpleTrafficThread" };	// The name of component thread

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( _threadName )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( traffic::SimpleLightControllerName, TrafficLightService )
            // register SimpleTrafficLight and SimpleTrafficSwitch service implementation.
            REGISTER_IMPLEMENT_SERVICE( SimpleTrafficLight::ServiceName, SimpleTrafficLight::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( traffic::SimpleLightControllerName )
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
    areg::Application::setup(true, true, true, true, true, nullptr);


    // load model to initialize components
    areg::Application::load_model(_modelName);

    // The components are initialized. Find the service component thread.
    // It is used to send custom event.
    areg::Thread * thread = areg::Thread::find_by_number(areg::crc32_calculate(_threadName));
    ASSERT(thread != nullptr );
    ASSERT(thread->is_runtime("DispatcherThread"));

    bool doLoop = true;
    areg::ext::Console & console = areg::ext::Console::instance( );
    console.enable_console_input( true );

    std::string_view commands[]
    {
          { "quit"  }
        , { "stop"  }
        , { "start" }
    };

    console.output_txt( { 0, 0 }, "A demo of a service to send data update notification (PubSub feature)...." );

    do 
    {
        // Wait and check the command. There are 3 options:
        //      - 'start'   to switch ON traffic light and start changing states.
        //      - 'stop'    to stop changing states and set traffic light OFF.
        //      - 'quit' or 'q' to quit application(s). This will also send signal to stop clients.
        console.output_txt( { 0, 2 }, "============================================" );
        console.output_txt( { 0, 3 }, "- Type \"start\" to start the traffic light." );
        console.output_txt( { 0, 4 }, "- Type \"stop\"  to stop the traffic light." );
        console.output_txt( { 0, 5 }, "- Type \"quit\"  to quit the traffic light." );
        console.output_txt( { 0, 6 }, "Type command: " );
        console.refresh_screen( );
        console.wait_for_input( [&]( const areg::String & cmd ) -> bool
            {
                if ( cmd.compare( commands[0], false ) == areg::Ordering::Equal )
                {
                    // It is 'quit' --> quit application(s).
                    areg::DispatcherThread * dispatcher = static_cast<areg::DispatcherThread *>(thread);
                    TrafficSwitchEvent::send_event( TrafficSwitchData( false ), *dispatcher );
                    doLoop = false; // quit the loop
                }
                else if ( cmd.compare( commands[1], false ) == areg::Ordering::Equal )
                {
                    // It is 'stop' --> stop the traffic light.
                    areg::DispatcherThread * dispatcher = static_cast<areg::DispatcherThread *>(thread);
                    TrafficSwitchEvent::send_event( TrafficSwitchData( false ), *dispatcher );
                }
                else if ( cmd.compare( commands[2], false ) == areg::Ordering::Equal )
                {
                    // It is 'start' --> start the traffic light.
                    areg::DispatcherThread * dispatcher = static_cast<areg::DispatcherThread *>(thread);
                    TrafficSwitchEvent::send_event( TrafficSwitchData( true ), *dispatcher );
                }

                return true;
            });

        areg::Thread::switch_thread();

    } while (doLoop);
        
    // stop and unload components
    areg::Application::unload_model(_modelName);

    // release and cleanup resources of application.
    areg::Application::release();

	return 0;
}
