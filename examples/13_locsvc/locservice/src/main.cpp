//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : The application that instantiates a simple service with 
//               request, response, broadcast and subscription servicing 
//               features, where clients can dynamically subscribe and 
//               unsubscribe to data update messages during run-time.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"
#include "locservice/src/ServicingComponent.hpp"
#include "locservice/src/ServiceClient.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "13_generated")
#endif // _MSC_VER

constexpr char const _modelName[]   { "TestModel" };            //!< The name of model
constexpr char const _roleName[]    { "TestServiceComponent" }; //!< Service role name

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate twice the same implementation
// of component, but running in different thread and having different
// role name. Note, no need to make changes in the code.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT(_roleName, ServicingComponent )
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( HelloWorld::ServiceName, HelloWorld::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT(_roleName)
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

    //////////////////////////////////////////////////////////////////////////
    //
    // Use of service component in the other thread.
    //
    //////////////////////////////////////////////////////////////////////////

    // define component thread
    BEGIN_REGISTER_THREAD( "TestClientThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "TestServiceClient", ServiceClient )
            REGISTER_DEPENDENCY( "TestServiceComponent" )
        // end of component description
        END_REGISTER_COMPONENT( "TestServiceClient" )
    // end of thread description
    END_REGISTER_THREAD( "TestClientThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(examples_13_locsvc, main);
//! \brief   A Demo to demonstrate simple request, response and broadcast.
int main()
{
    std::cout << "A Demo to demonstrate simple request, response and broadcast ..." << std::endl;

    // force to start logging with default settings
    areg::Application::setup( true, true, false, true, false, nullptr );
    LOGGING_CONFIGURE_AND_START(nullptr, false);

    do 
    {
        LOG_SCOPE( examples_13_locsvc, main );
        LOG_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        // load model to initialize components
        areg::Application::load_model(_modelName);

        LOG_DBG("Servicing model is loaded");

        // wait until Application quit signal is set.
        areg::Application::wait_quit(areg::WAIT_INFINITE);

        // stop and unload components
        areg::Application::unload_model(_modelName);

        std::cout
            << (areg::Application::find_model( _modelName ).alive_duration( ) / areg::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        // release and cleanup resources of application.
        areg::Application::release();

    } while (false);

    std::cout << "Exit application, check the logs for details!" << std::endl;
    return 0;
}
