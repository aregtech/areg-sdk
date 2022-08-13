//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
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
#include "areg/trace/GETrace.h"
#include "src/ServicingComponent.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS

/**
 * \brief   This file demonstrates simple initialization of servicing local component
 *          used for local multi-threading communication. And how to duplicate same
 *          implemented service in the system. The duplication should not happen in the
 *          same thread, rather in different threads with different role names.
 *          The servicing component does not contain any servicing method, it uses
 *          timer to run and exit application after certain time.
 **/

constexpr char const _modelName[] { "TestModel" };  //!< The name of model

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
    BEGIN_REGISTER_THREAD( "TestServiceThread1", NECommon::INVALID_TIMEOUT)
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
    BEGIN_REGISTER_THREAD( "TestServiceThread2", NECommon::INVALID_TIMEOUT)
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

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE( examples_09_svcmulti_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing multiple empty servicing components in multiple threads...\n");

    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, timer and watchdog.
    Application::initApplication(true, true, false, true, true, nullptr, nullptr );

    do 
    {
        TRACE_SCOPE(examples_09_svcmulti_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        // load model to initialize components
        Application::loadModel(_modelName);

        TRACE_DBG("Servicing model is loaded");

        // wait until 'gExit' event is signaled
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(_modelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    printf("Completed testing multiple empty servicing components, check logs...\n");

	return 0;
}
