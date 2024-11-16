//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : The application provides network discoverable Public service,
//               which predefined methods are called from remote clients.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/GELog.h"
#include "aregextend/console/Console.hpp"

#include "common/src/NECommon.hpp"
#include "common/src/PubSubMixed.hpp"
#include "pubsubctrl/src/PubSubController.hpp"

#ifdef WIN32
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "23_generated.lib")
    #pragma comment(lib, "23_common.lib")
    #pragma comment(lib, "aregextend.lib")
#endif // WINDOWS

namespace
{
    //!< The title to output on the console.
    constexpr std::string_view  _title      { "PubSub features, mixed connections, controller ..."};

    //!< The name of the model
    constexpr char const _modelName[]       { "ServiceModel" };

    //!< Generates the name of second Publisher that is using same interface.
    String SecondRole(NEUtilities::generateName(NECommon::PublisherSecond));
}

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate servicing component.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL( _modelName )

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread1", NECommon::WATCHDOG_IGNORE )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT(NECommon::ContollerPublisher, PubSubController )
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEPubSubMix::ServiceName, NEPubSubMix::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::ContollerPublisher)
        // end of component description
        END_REGISTER_COMPONENT(NECommon::ContollerPublisher)
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread1" )

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread2", NECommon::WATCHDOG_IGNORE )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT(SecondRole, PubSubMixed)
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEPubSubMix::ServiceName, NEPubSubMix::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::ContollerPublisher)
            REGISTER_DEPENDENCY(SecondRole)
        // end of component description
        END_REGISTER_COMPONENT(SecondRole)
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread2" )

// end of model description
END_MODEL( _modelName )

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(example_23_pubsubmix_pubsubctrl_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main( )
{
    printf( "Testing PubSub featured service ...\n" );

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    do
    {
        LOG_SCOPE(example_23_pubsubmix_pubsubctrl_main);
        LOG_DBG( "The application has been initialized, loading model [ %s ]", _modelName );

        // Output the title of the application.
        Console & console = Console::getInstance();
        console.clearScreen();
        console.outputTxt(NECommon::CoordTitle, _title);
        console.outputTxt(NECommon::CoordSubtitle, NECommon::Separator);

        // Set this value to have correct outputs on console, it plays no other role.
        NEMemory::uAlign data{};
        data.alignInt.mElement = 1;
        ComponentLoader::setComponentData(SecondRole, data);

        // load model to initialize components
        Application::loadModel( _modelName );

        LOG_DBG( "Servicing model is loaded" );

        // wait until Application quit signal is set.
        Application::waitAppQuit( NECommon::WAIT_INFINITE );

        // release and cleanup resources of application.
        Application::releaseApplication( );

    } while ( false );

    printf( "Completed PubSub featured service. Check the logs...\n" );

    return 0;
}
