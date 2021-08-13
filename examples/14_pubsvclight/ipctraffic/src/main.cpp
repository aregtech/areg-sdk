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
#include "areg/base/NEUtilities.hpp"

#include "generated/src/NESimpleLightCommon.hpp"
#include "ipctraffic/src/SimpleTrafficLightClient.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "14_generated.lib")
#endif // WINDOWS

/**
 * \brief   This file demonstrates dynamic initialization of model and the start of service client.
 **/

static const char * const _modelName = "TheModel";
//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The main method does not start the logging, enables service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    NESimpleLightCommon::eTrafficDirection trafficDirection = NESimpleLightCommon::DirectionUnknown;

    String roleName(NESimpleLightCommon::SimpleLightClientNamePrefix);
    char name[128];
    char buffer[32];

    // At first, determine which traffic direction should be set.
    // This is used to react on the right attribute.
    printf("...........................................\n\n");
    printf("Please select which traffic direction to output:\n");
    printf("\t1. Type \'sn\' for South-North direction or input 1\n");
    printf("\t2. Type \'ew\' for East-West direction or input 2\n");
    printf("...........................................\n");
    printf("Type the choice: ");

#ifdef _WINDOWS
    scanf_s("%31s", buffer, 32);
#else   // _POSIX
    scanf("%31s", buffer);
#endif 	// _WINDOWS

    // Check whether the right option is selected.
    if ( (NEString::compareFastIgnoreCase(buffer, "sn") == 0) || (NEString::compareFast(buffer, "1") == 0) )
    {
        trafficDirection = NESimpleLightCommon::DirectionSouthNorth;
        roleName += "SouthNorth";
        roleName = NEUtilities::generateName(roleName, name, 128);
        printf("\nSelected Choice: South-North traffic.\n");
    }
    else if ( (NEString::compareFastIgnoreCase(buffer, "ew") == 0) || (NEString::compareFast(buffer, "2") == 0) )
    {
        trafficDirection = NESimpleLightCommon::DirectionEastWest;
        roleName += "EastWest";
        roleName = NEUtilities::generateName(roleName, name, 128);
        printf("\nSelected Choice: East-West traffic.\n");
    }
    else
    {
        printf("\nSelected Choice: Unexpected...\n");
        printf("Quit the job ...\n");

        // wrong option, quit application.
        return 0;
    }

    printf("\n...........................................\n");

    // Initialize application, disable logging, enables servicing and the timer.
    Application::initApplication(false, true, true, true, NULL, NULL);

    // Create model manually during run-time.
    NERegistry::Model model(_modelName);

    // Add component thread entry.
    NERegistry::ComponentThreadEntry & threadEntry = model.addThread("SimpleTrafficLighThread");
    // Add component in the thread and set the service dependency.
    NERegistry::ComponentEntry & component = threadEntry.addComponent(roleName, SimpleTrafficLightClient::CreateComponent, SimpleTrafficLightClient::DeleteComponent);
    component.addDependencyService(NESimpleLightCommon::SimpleLightControllerName);
    
    // Set component data, i.e. specify the traffic direction.
    NEMemory::uAlign data;
    data.alignInt.mElement = static_cast<int>(trafficDirection);
    component.setComponentData( data );
    
    // Add created model to the model list.
    ComponentLoader::addModelUnique(model);

    // By passing NULL, load all models to initialize components
    Application::loadModel(NULL);
        
    // wait until Application quit signal is set.
    Application::waitAppQuit(IESynchObject::WAIT_INFINITE);

    // By passing NULL, stop and unload all models.
    Application::unloadModel(NULL);

    // release and cleanup resources of application.
    Application::releaseApplication();
    
	return 0;
}
