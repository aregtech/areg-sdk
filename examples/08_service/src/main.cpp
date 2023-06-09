//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project demonstrates the use of simple local servicing 
//               objects to create multithreading applications.It instantiates
//               a dummy ( empty ) local service interface, which is not 
//               visible outside of the process.
//
//               The service is created as an instance of Component, dummy 
//               Stub and Timer consumer. The service is registered in a model
//               that is loaded at start and unloaded when the application 
//               completes the job.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"
#include "src/ServicingComponent.hpp"

#ifdef  _WIN32
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg.lib")
#endif // _WIN32

constexpr char const _modelName[] { "TestModel" };  //!< The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "Test_ServiceThread", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "TestServicingComponent", ServicingComponent )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( NEService::EmptyServiceName, NEService::EmptyServiceVersion )
        // end of component description
        END_REGISTER_COMPONENT( "TestServicingComponent" )
    // end of thread description
    END_REGISTER_THREAD( "Test_ServiceThread" )

// end of model description
END_MODEL(_modelName)

DEF_TRACE_SCOPE(examples_08_service_main);
//! A Demo of loading and starting an empty service with no functionalities
int main()
{
    std::cout << "A Demo of loading and starting an empty service with no functionalities ..." << std::endl;

    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    Application::initApplication(true, true, false, true, true, nullptr, nullptr );

    do 
    {
        unsigned int timeout{ NECommon::WAIT_5_SECONDS };

        TRACE_SCOPE(examples_08_service_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);
        ASSERT( Application::findModel( _modelName ).isValid( ) );

        Application::loadModel(_modelName);
        std::cout << "Service model is loaded. Waiting maximum for " << timeout << " ms to unload model." << std::endl;
        Application::waitAppQuit( timeout );    // wait for quit signal to complete application.
        Application::unloadModel(_modelName);   // stop and unload components
        
        unsigned int duration = static_cast<unsigned int>(Application::findModel( _modelName ).getAliveDuration( ) / NECommon::DURATION_1_MILLI);
        timeout = MACRO_MIN( timeout, duration );
        std::cout << timeout << " ms passed. Model is unloaded, releasing resources to exit application..." << std::endl;
        Application::releaseApplication();      // release and cleanup resources of application.

    } while (false);

    std::cout << "Exit application, check the logs for details!" << std::endl;
    return 0;
}
