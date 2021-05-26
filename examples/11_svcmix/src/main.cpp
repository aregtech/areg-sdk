//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "MainComponent.hpp"
#include "SecondaryComponent.hpp"
#include "ClientComponent.hpp"

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

static const char * gModelName      = "Test_ServiceHello";  //!< The name of model
static const char * gMainComponent  = "Test_MainComponent"; //!< Main component that controls shutdown.

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate twice the same implementation
// of component, but running in different thread and having different
// role name. Note, no need to make changes in the code.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL(gModelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "Test_ServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( gMainComponent, MainComponent )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, 1, 0, 0 )
        // end of component description
        END_REGISTER_COMPONENT( gMainComponent )
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
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, 1, 0, 0 )
            REGISTER_DEPENDENCY(gMainComponent)
            REGISTER_DEPENDENCY("Test_SecondaryComponent")
        // end of component description
        END_REGISTER_COMPONENT( "Test_SecondaryComponent" )

    END_REGISTER_THREAD("Test_SecondaryThread")

    // define component thread
    BEGIN_REGISTER_THREAD( "Test_MixedThread" )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "Test_ThirdComponent", SecondaryComponent )
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, 1, 0, 0 )
            REGISTER_DEPENDENCY( gMainComponent )
            REGISTER_DEPENDENCY( "Test_SecondaryComponent")
        // end of component description
        END_REGISTER_COMPONENT( "Test_ThirdComponent" )

        BEGIN_REGISTER_COMPONENT("Test_ClientComponent", ClientComponent)
            REGISTER_DEPENDENCY( gMainComponent )
            REGISTER_DEPENDENCY( "Test_SecondaryComponent")
            REGISTER_DEPENDENCY( "Test_ThirdComponent" )
        END_REGISTER_COMPONENT("Test_ClientComponent")

    // end of thread description
    END_REGISTER_THREAD( "Test_MixedThread" )

// end of model description
END_MODEL(gModelName)

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
    // force to start logging with default settings
    TRACER_FORCE_LOGGING();
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, false, true, NULL, NULL);

    do 
    {
        TRACE_SCOPE(example_11_svcmix_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", gModelName);

        NEMemory::uAlign isMain = {true};
        ComponentLoader::getInstance().setComponentData(gMainComponent, isMain);

        // load model to initialize components
        Application::loadModel(gModelName);

        TRACE_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        Application::waitAppQuit(IESynchObject::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(gModelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);
    
	return 0;
}
