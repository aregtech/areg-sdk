//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates the use of simple local servicing 
//               objects to create multithreading applications.It instantiates
//               a dummy ( empty ) local service interface, which is not 
//               visible outside of the process.
//
//               The service is created as an instance of Component, dummy 
//               Provider and Timer consumer. The service is registered in a model
//               that is loaded at start and unloaded when the application 
//               completes the job.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"
#include "src/ServicingComponent.hpp"

#include <algorithm>

#ifdef  _MSC_VER
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg")
#endif // _MSC_VER

constexpr char const _modelName[] { "TestModel" };  //!< The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "Test_ServiceThread")
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "TestServicingComponent", ServicingComponent )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( areg::EmptyServiceName, areg::EmptyServiceVersion )
        // end of component description
        END_REGISTER_COMPONENT( "TestServicingComponent" )
    // end of thread description
    END_REGISTER_THREAD( "Test_ServiceThread" )

// end of model description
END_MODEL(_modelName)

DEF_LOG_SCOPE(examples_11_service, main);
//! A Demo of loading and starting an empty service with no functionalities
int main()
{
    std::cout << "A Demo of loading and starting an empty service with no functionalities ..." << std::endl;

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr, false );
    areg::Application::setup(true, true, false, true, true, nullptr );

    do 
    {
        uint32_t timeout{ areg::WAIT_10_SECONDS };

        LOG_SCOPE( examples_11_service, main );
        LOG_DBG("The application has been initialized, loading model [ %s ]", _modelName);
        ASSERT( areg::Application::find_model( _modelName ).is_valid( ) );

        areg::Application::load_model(_modelName);
        std::cout << "Service model is loaded. Waiting maximum for " << timeout << " ms to unload model." << std::endl;
        areg::Application::wait_quit( timeout );    // wait for quit signal to complete application.
        areg::Application::unload_model(_modelName);   // stop and unload components
        
        uint32_t duration = static_cast<uint32_t>(areg::Application::find_model( _modelName ).alive_duration( ) / areg::DURATION_1_MILLI);
        timeout = std::min( timeout, duration );
        std::cout << timeout << " ms passed. Model is unloaded, releasing resources to exit application..." << std::endl;
        areg::Application::release();      // release and cleanup resources of application.

    } while (false);

    std::cout << "Exit application, check the logs for details!" << std::endl;
    return 0;
}
