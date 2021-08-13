//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "svcmix/src/MainComponent.hpp"
#include "svcmix/src/SecondaryComponent.hpp"
#include "svcmix/src/ClientComponent.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "11_generated.lib")
#endif // WINDOWS

/**
 * \brief   This file demonstrates simple initialization of servicing local component
 *          used for local multi-threading communication. And how to duplicate same
 *          implemented service in the system. The duplication should not happen in the
 *          same thread, rather in different threads with different role names.
 *          The servicing component does not contain any servicing method, it uses
 *          timer to run and exit application after certain time.
 **/

static const char * _modelName          = "TestModel";      //!< The name of model
static const char * _mainServiceName    = "MainService";    //!< Main component that controls shutdown.

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
    BEGIN_REGISTER_THREAD( "Test_ServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _mainServiceName, MainComponent )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( _mainServiceName )
    // end of thread description
    END_REGISTER_THREAD( "Test_ServiceThread" )

    //////////////////////////////////////////////////////////////////////////
    //
    // Use of service component in the other thread.
    //
    //////////////////////////////////////////////////////////////////////////
    
    BEGIN_REGISTER_THREAD( "Test_SecondaryThread")
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "Test_SecondaryComponent", SecondaryComponent )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(_mainServiceName)
            REGISTER_DEPENDENCY("Test_SecondaryComponent")
        // end of component description
        END_REGISTER_COMPONENT( "Test_SecondaryComponent" )

    END_REGISTER_THREAD("Test_SecondaryThread")

    // define component thread
    BEGIN_REGISTER_THREAD( "Test_MixedThread" )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "Test_ThirdComponent", SecondaryComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY( _mainServiceName )
            REGISTER_DEPENDENCY( "Test_SecondaryComponent")
        // end of component description
        END_REGISTER_COMPONENT( "Test_ThirdComponent" )

        BEGIN_REGISTER_COMPONENT("Test_ClientComponent", ClientComponent)
            REGISTER_DEPENDENCY( _mainServiceName )
            REGISTER_DEPENDENCY( "Test_SecondaryComponent")
            REGISTER_DEPENDENCY( "Test_ThirdComponent" )
        END_REGISTER_COMPONENT("Test_ClientComponent")

    // end of thread description
    END_REGISTER_THREAD( "Test_MixedThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_11_svcmix_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing multiple local servicing components of the same service interface running with different names in different threads...\n");
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START(NULL);
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, false, true, NULL, NULL);

    do 
    {
        TRACE_SCOPE(example_11_svcmix_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        NEMemory::uAlign isMain = {true};
        ComponentLoader::getInstance().setComponentData(_mainServiceName, isMain);

        // load model to initialize components
        Application::loadModel(_modelName);

        TRACE_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        Application::waitAppQuit(IESynchObject::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(_modelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);
    
    printf("Completed testing multiple local servicing components, check the logs...\n");

	return 0;
}
