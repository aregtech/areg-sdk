//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project demonstrates how to instantiate multiple 
//               instances of the same service differing by names. It is an
//               extension of 08_service, uses empty services and instantiates
//               2 identical services with different names.
//
//               The same local service can be instantiate more than once if 
//               instances differ by name and run in different threads.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/GELog.h"
#include "src/ServicingComponent.hpp"

#ifdef  _MSC_VER
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg")
#endif // _MSC_VER

constexpr char const _modelName[] { "TestModel" };  //!< The name of model

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate twice the same implementation
// of component, but running in different thread and having different
// role names. Note, no need to make changes in the code.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread1", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "TestService1", ServicingComponent )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( NEService::EmptyServiceName, NEService::EmptyServiceVersion )
        // end of component description
        END_REGISTER_COMPONENT( "TestService1" )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread1" )

    //////////////////////////////////////////////////////////////////////////
    //
    // Duplication of same service implementation in the registers.
    //
    // The duplication requires other component thread and different role
    // name of the component. The same implementation could do by using
    // 2 models, where each mode contains thread and component.
    //
    //////////////////////////////////////////////////////////////////////////

    // define component thread
    BEGIN_REGISTER_THREAD( "TestServiceThread2", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "TestService2", ServicingComponent )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( NEService::EmptyServiceName, NEService::EmptyServiceVersion )
        // end of component description
        END_REGISTER_COMPONENT( "TestService2" )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread2" )

// end of model description
END_MODEL(_modelName)

DEF_LOG_SCOPE( examples_09_svcmulti_main);
//! A Demo of loading and starting an multiple instances of the same service with no functionalities
int main()
{
    std::cout << "A Demo of loading and starting an multiple instances of the same service with no functionalities ..." << std::endl;

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr );
    Application::initApplication(true, true, false, true, true, nullptr);

    do 
    {
        LOG_SCOPE(examples_09_svcmulti_main);
        LOG_DBG("The application has been initialized, loading model [ %s ]", _modelName);
        ASSERT( Application::findModel( _modelName ).isValid( ) );

        Application::loadModel(_modelName);
        std::cout << "Service model is loaded. Waiting to quit application signal." << std::endl;
        Application::waitAppQuit( NECommon::WAIT_INFINITE ); // wait for quit signal to complete application.
        Application::unloadModel(_modelName);                // stop and unload components
        
        std::cout
            << (Application::findModel( _modelName ).getAliveDuration( ) / NECommon::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        Application::releaseApplication();      // release and cleanup resources of application.

    } while (false);

    std::cout << "Exit application, check the logs for details!" << std::endl;
    return 0;
}
