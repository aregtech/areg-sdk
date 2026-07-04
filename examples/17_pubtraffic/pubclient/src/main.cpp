//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : This project contains an instance service client, which model
//               is defined dynamically during run-time. This approach 
//               guaranties to start and run several instance of the same 
//               applications, where each instance of service client 
//               (or provider) have generated and unique names.
//
//               When the application starts, use one of commands:
//                  - Type "sn" to subscribe to one set of data updates.
//                  - Type "ew" to subscribe to another set of data updates.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"
#include "areg/base/UtilityDefs.hpp"
#include "aregextend/console/Console.hpp"

#include "common/TrafficDefs.hpp"
#include "pubclient/src/TrafficLightClient.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
    #pragma comment(lib, "17_generated")
#endif // _MSC_VER

//! A demo of dynamic model and client with data update subscription.
int main()
{
    constexpr char const _modelName[]{ "TheModel" };

    traffic::TrafficDirection trafficDirection = traffic::TrafficDirection::Undefined;
    std::string_view directions[]
    {
          {"sn"}
        , {"ew"}
        , {"quit"}
    };

    areg::String roleName( traffic::SimpleLightClientNamePrefix);
    char name[128];

    areg::ext::Console & console = areg::ext::Console::instance( );
    console.enable_console_input( true );
    console.output_txt( { 0, 0 }, "A demo of dynamic model and client with data update subscription..." );

    // At first, determine which traffic direction should be set.
    // This is used to react on the right attribute.
    console.output_txt( { 0, 1 }, "................................................" );
    console.output_txt( { 0, 2 }, "Please select which traffic direction to output:" );
    console.output_txt( { 0, 3 }, "\t1. Type \'sn\' for South-North direction      " );
    console.output_txt( { 0, 4 }, "\t2. Type \'ew\' for East-West direction        " );
    console.output_txt( { 0, 5 }, "\t3. Type \'quit\' to quit.                     " );
    console.output_txt( { 0, 6 }, "................................................" );
    console.output_txt( { 0, 7 }, "Type the choice: " );
    console.wait_for_input( [&]( const areg::String cmd ) -> bool
        {
            bool result{ false };
            if ( cmd.compare( directions[0], false ) == areg::Ordering::Equal )
            {
                trafficDirection = traffic::TrafficDirection::SouthNorth;
                roleName += "SouthNorth";
                roleName = areg::generate_name( roleName, name, 128 );
                result = true;
                console.output_txt( { 0, 8 }, "Selected direction is South - North" );
            }
            else if ( cmd.compare( directions[1], false ) == areg::Ordering::Equal )
            {
                trafficDirection = traffic::TrafficDirection::EastWest;
                roleName += "EastWest";
                roleName = areg::generate_name( roleName, name, 128 );
                result = true;
                console.output_txt( { 0, 8 }, "Selected direction is East - West" );
            }
            else if ( cmd.compare( directions[2], false ) == areg::Ordering::Equal )
            {
                result = true; // do not set the direction, just stop input
            }

            return result;
        } );

    if ( trafficDirection == traffic::TrafficDirection::Undefined )
        return 0; // quit

    console.move_to_line( 10 );

    // Initialize application, use default settings: enable logging, servicing, routing, timer and watchdog.
    areg::Application::setup( );

    // Create model manually during run-time.
    areg::Model model(_modelName);

    // Add component thread entry.
    areg::ComponentThreadEntry & threadEntry = model.add_thread("SimpleTrafficLighThread");
    // Add component in the thread and set the service dependency.
    areg::ComponentEntry& component = threadEntry.add_component<TrafficLightClient>(roleName);
    component.add_dependency_service( traffic::SimpleLightControllerName);
    
    // Set component data, i.e. specify the traffic direction.
    std::any data = std::make_any< traffic::TrafficDirection>(trafficDirection);
    component.set_data( data );
    
    // Add created model to the model list.
    areg::ComponentLoader::add_model_unique(model);

    // By passing nullptr, load all models to initialize components
    areg::Application::load_model( nullptr );
        
    // wait until Application quit signal is set.
    areg::Application::wait_quit(areg::WAIT_INFINITE);

    // By passing nullptr, stop and unload all models.
    areg::Application::unload_model( nullptr );

    // release and cleanup resources of application.
    areg::Application::release();
    
	return 0;
}
