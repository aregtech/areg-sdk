//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : The application provides network discoverable Public service,
//               which predefined methods are called from remote clients.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/console/Console.hpp"

#include "common/src/PubSubDefs.hpp"
#include "common/src/PubSubMixed.hpp"
#include "pubsubctrl/src/PubSubController.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "26_generated")
    #pragma comment(lib, "26_common")
    #pragma comment(lib, "aregextend")
#endif // _MSC_VER

namespace
{
    //!< The title to output on the console.
    constexpr std::string_view  _title      { "PubSub features, mixed connections, controller ..."};

    //!< The name of the model
    constexpr char const _modelName[]       { "ServiceModel" };

    //!< Generates the name of second Publisher that is using same interface.
    areg::String SecondRole(areg::generate_name(pubsub::PublisherSecond));
}

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate servicing component.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL( _modelName )

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread1" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT(pubsub::ContollerPublisher, PubSubController )
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( PubSubMix::ServiceName, PubSubMix::InterfaceVersion )
            REGISTER_DEPENDENCY(pubsub::ContollerPublisher)
        // end of component description
        END_REGISTER_COMPONENT(pubsub::ContollerPublisher)
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread1" )

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread2" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT(SecondRole, PubSubMixed)
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( PubSubMix::ServiceName, PubSubMix::InterfaceVersion )
            REGISTER_DEPENDENCY(pubsub::ContollerPublisher)
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
DEF_LOG_SCOPE(example_26_pubsubmix_pubsubctrl_main);
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
    areg::Application::setup( );

    do
    {
        LOG_SCOPE(example_26_pubsubmix_pubsubctrl_main);
        LOG_DBG( "The application has been initialized, loading model [ %s ]", _modelName );

        // Output the title of the application.
        aregext::Console & console = aregext::Console::instance();
        console.clear_screen();
        console.output_txt(pubsub::CoordTitle, _title);
        console.output_txt(pubsub::CoordSubtitle, pubsub::Separator);

        // Set this value to have correct outputs on console, it plays no other role.
        areg::ComponentLoader::set_component_data(SecondRole, std::make_any<int32_t>(1));

        // load model to initialize components
        areg::Application::load_model( _modelName );

        LOG_DBG( "Servicing model is loaded" );

        // wait until Application quit signal is set.
        areg::Application::wait_quit( areg::WAIT_INFINITE );

        // release and cleanup resources of application.
        areg::Application::release( );

    } while ( false );

    printf( "Completed PubSub featured service. Check the logs...\n" );

    return 0;
}
