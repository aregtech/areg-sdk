//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : The application contains a Public service client software
//               component. It receives connected notification and starts 
//               calling remote method of the Public service. To make periodic
//               calls, it uses a timer.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/trace/GETrace.h"

#include "common/src/NECommon.hpp"
#include "common/src/PubSubMixed.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg")
    #pragma comment(lib, "23_generated.lib")
    #pragma comment(lib, "23_common.lib")
    #pragma comment(lib, "areg-extend.lib")
#endif // WINDOWS

namespace
{
    constexpr char const _modelName[]   { "PubSub" };                  //!< The name of model
    constexpr std::string_view  _title  { "PubSub mix features, secondary application..."};
    const String SecondRole(NEUtilities::generateName(NECommon::PublisherSecond));
    const String ThirddRole(NEUtilities::generateName(NECommon::PublisherThird));
}

//////////////////////////////////////////////////////////////////////////
//
// The following design instantiates large data servicing client.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "DynPubSubThread1", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( SecondRole, PubSubMixed)
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEPubSubMix::ServiceName, NEPubSubMix::InterfaceVersion )
            // register service dependency
            REGISTER_DEPENDENCY( NECommon::ContollerPublisher )
            REGISTER_DEPENDENCY(ThirddRole)
        // end of component description
        END_REGISTER_COMPONENT(SecondRole)
    // end of thread description
    END_REGISTER_THREAD( "DynPubSubThread1" )

    // define component thread
    BEGIN_REGISTER_THREAD( "DynPubSubThread2", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT(ThirddRole, PubSubMixed)
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEPubSubMix::ServiceName, NEPubSubMix::InterfaceVersion )
            // register service dependency
            REGISTER_DEPENDENCY( NECommon::ContollerPublisher )
            REGISTER_DEPENDENCY(SecondRole)
        // end of component description
        END_REGISTER_COMPONENT(ThirddRole)
    // end of thread description
    END_REGISTER_THREAD( "DynPubSubThread2" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_23_pubsubmix_pubsubdyn_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing PubSub service consumer...\n");
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    do
    {
        TRACE_SCOPE(example_23_pubsubmix_pubsubdyn_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        // Output the title
        Console & console = Console::getInstance();
        console.clearScreen();
        console.outputTxt(NECommon::CoordTitle, _title);
        console.outputTxt(NECommon::CoordSubtitle, NECommon::Separator);

        // Set this value to have correct outputs on console, it plays no other role.
        NEMemory::uAlign first{}, second{};
        first.alignInt.mElement = 0;
        ComponentLoader::setComponentData(SecondRole, first);
        second.alignInt.mElement = 1;
        ComponentLoader::setComponentData(ThirddRole, second);

        // load model to initialize components
        Application::loadModel(_modelName);

        TRACE_DBG("Servicing model is loaded");

        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    printf("Completed testing PubSub service feature, check the logs...\n");

	return 0;
}
