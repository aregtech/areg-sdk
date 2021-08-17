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

#include "generated/src/NECommon.hpp"
#include "ipcfsmclient/src/TrafficLightClient.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "16_generated.lib")
#endif // WINDOWS

/**
 * \brief   This file demonstrates dynamic initialization of model and the start of service client.
 **/

static char clientRole[128];    //!< The role name of service client.
static const char * _modelName  = "TheModel";   //!< The name of model
static const char * _threadName = "TestTrafficLightThread"; //!< The name of component wonning thread.
static const char * _compName   = NEUtilities::generateName(NECommon::SerivceLightClient, clientRole, 128); //!< Generated name of the service owner, unique for client application.

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( _threadName )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _compName, TrafficLightClient )
            // register TrafficController dependency.
            REGISTER_DEPENDENCY(NECommon::ServiceLightController)
        // end of component description
        END_REGISTER_COMPONENT( _compName )
    // end of thread description
    END_REGISTER_THREAD( _threadName )

// end of model
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(ipcfsmclient_src_main_main);

/**
 * \brief   The main method does not start the logging, enables service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    bool isEastWest = true;
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
        isEastWest = false;
        printf("\nSelected Choice: South-North traffic.\n");
    }
    else if ( (NEString::compareFastIgnoreCase(buffer, "ew") == 0) || (NEString::compareFast(buffer, "2") == 0) )
    {
        isEastWest = true;
        printf("\nSelected Choice: East-West traffic.\n");
    }
    else
    {
        printf("\nSelected Choice: Unexpected...\n");
        printf("Set default direction: %s\n", isEastWest ? "East-West" : "South-North");
    }

    printf("\n...........................................\n");

    NEMemory::uAlign data;
    data.alignBool.mElement = isEastWest;
    ComponentLoader::setComponentData(_compName, data);

    // Initialize application, disable logging, enables servicing and the timer.
    Application::initApplication(true, true, true, true, NULL, NULL);

    do 
    {
        TRACE_SCOPE(ipcfsmclient_src_main_main);
        TRACE_DBG("Starting traffic light for direction [ %s ]", isEastWest ? "East-West" : "South-North");

        // By passing NULL, load all models to initialize components
        Application::loadModel(NULL);

        // wait until Application quit signal is set.
        Application::waitAppQuit(IESynchObject::WAIT_INFINITE);

        // By passing NULL, stop and unload all models.
        Application::unloadModel(NULL);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    return 0;
}