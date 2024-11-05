//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
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

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"
#include "areg/base/NEUtilities.hpp"
#include "aregextend/console/Console.hpp"

#include "common/NECommon.hpp"
#include "pubclient/src/TrafficLightClient.hpp"

#ifdef WIN32
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "aregextend.lib")
    #pragma comment(lib, "14_generated.lib")
#endif // WIN32

//! A demo of dynamic model and client with data update subscription.
int main()
{
    constexpr char const _modelName[]{ "TheModel" };

    NECommon::eTrafficDirection trafficDirection = NECommon::eTrafficDirection::DirectionUnknown;
    std::string_view directions[]
    {
          {"sn"}
        , {"ew"}
        , {"quit"}
    };

    String roleName( NECommon::SimpleLightClientNamePrefix);
    char name[128];

    Console & console = Console::getInstance( );
    console.enableConsoleInput( true );
    console.outputTxt( { 0, 0 }, "A demo of dynamic model and client with data update subscription..." );

    // At first, determine which traffic direction should be set.
    // This is used to react on the right attribute.
    console.outputTxt( { 0, 1 }, "................................................" );
    console.outputTxt( { 0, 2 }, "Please select which traffic direction to output:" );
    console.outputTxt( { 0, 3 }, "\t1. Type \'sn\' for South-North direction      " );
    console.outputTxt( { 0, 4 }, "\t2. Type \'ew\' for East-West direction        " );
    console.outputTxt( { 0, 5 }, "\t3. Type \'quit\' to quit.                     " );
    console.outputTxt( { 0, 6 }, "................................................" );
    console.outputTxt( { 0, 7 }, "Type the choice: " );
    console.waitForInput( [&]( const String cmd ) -> bool
        {
            bool result{ false };
            if ( cmd.compare( directions[0], false ) == NEMath::eCompare::Equal )
            {
                trafficDirection = NECommon::eTrafficDirection::DirectionSouthNorth;
                roleName += "SouthNorth";
                roleName = NEUtilities::generateName( roleName, name, 128 );
                result = true;
                console.outputTxt( { 0, 8 }, "Selected direction is South - North" );
            }
            else if ( cmd.compare( directions[1], false ) == NEMath::eCompare::Equal )
            {
                trafficDirection = NECommon::eTrafficDirection::DirectionEastWest;
                roleName += "EastWest";
                roleName = NEUtilities::generateName( roleName, name, 128 );
                result = true;
                console.outputTxt( { 0, 8 }, "Selected direction is East - West" );
            }
            else if ( cmd.compare( directions[2], false ) == NEMath::eCompare::Equal )
            {
                result = true; // do not set the direction, just stop input
            }

            return result;
        } );

    if ( trafficDirection == NECommon::eTrafficDirection::DirectionUnknown )
        return 0; // quit

    console.moveToLine( 10 );

    // Initialize application, use default settings: enable logging, servicing, routing, timer and watchdog.
    Application::initApplication( );

    // Create model manually during run-time.
    NERegistry::Model model(_modelName);

    // Add component thread entry.
    NERegistry::ComponentThreadEntry & threadEntry = model.addThread("SimpleTrafficLighThread");
    // Add component in the thread and set the service dependency.
    NERegistry::ComponentEntry & component = threadEntry.addComponent(roleName, TrafficLightClient::CreateComponent, TrafficLightClient::DeleteComponent);
    component.addDependencyService( NECommon::SimpleLightControllerName);
    
    // Set component data, i.e. specify the traffic direction.
    NEMemory::uAlign data;
    data.alignInt.mElement = static_cast<int>(trafficDirection);
    component.setComponentData( data );
    
    // Add created model to the model list.
    ComponentLoader::addModelUnique(model);

    // By passing nullptr, load all models to initialize components
    Application::loadModel( nullptr );
        
    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);

    // By passing nullptr, stop and unload all models.
    Application::unloadModel( nullptr );

    // release and cleanup resources of application.
    Application::releaseApplication();
    
	return 0;
}
