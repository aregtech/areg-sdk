//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project contains an instance service client, which model
//               is defined dynamically during run-time.This approach 
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

#include "generated/src/NECommon.hpp"
#include "pubclient/src/TrafficLightClient.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "16_generated.lib")
#endif // WINDOWS

#ifdef _WINDOWS
    #define MACRO_SCANF(fmt, data, len)     scanf_s(fmt, data, len)
#else   // _POSIX
    #define MACRO_SCANF(fmt, data, len)     scanf(fmt, data)
#endif  // _WINDOWS


constexpr char const _modelName[]  { "TheModel" };   //!< The name of model
constexpr char const _threadName[] { "TestTrafficLightThread" }; //!< The name of component wonning thread.
static char clientRole[128];    //!< The role name of service client.
static const char* const _compName  = NEUtilities::generateName(NECommon::SerivceLightClient, clientRole, 128); //!< Generated name of the service owner, unique for client application.

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( _threadName, NECommon::WATCHDOG_IGNORE)
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
DEF_TRACE_SCOPE(pubclient_src_main_main);

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

    if ( MACRO_SCANF( "%31s", buffer, 32 ) != 1 )
    {
        printf( "\nERROR: Unexpected choice, quit application ...\n" );
        return 0;
    }

    // Check whether the right option is selected.
    if ( (NEString::compareFastIgnoreCase(buffer, "sn") == NEMath::eCompare::Equal) || 
         (NEString::compareFast(buffer, "1") == NEMath::eCompare::Equal) )
    {
        isEastWest = false;
        printf("\nSelected Choice: South-North traffic.\n");
    }
    else if ((NEString::compareFastIgnoreCase(buffer, "ew") == NEMath::eCompare::Equal) || 
             (NEString::compareFast(buffer, "2") == NEMath::eCompare::Equal) )
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

    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    do 
    {
        TRACE_SCOPE(pubclient_src_main_main);
        TRACE_DBG("Starting traffic light for direction [ %s ]", isEastWest ? "East-West" : "South-North");

        // By passing nullptr, load all models to initialize components
        Application::loadModel( nullptr );

        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // By passing nullptr, stop and unload all models.
        Application::unloadModel( nullptr );

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    return 0;
}
