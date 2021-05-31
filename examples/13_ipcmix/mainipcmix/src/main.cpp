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

#include "shareipcmix/src/NERemoteHelloWorld.hpp"
#include "shareipcmix/src/NELocalHelloWorld.hpp"
#include "shareipcmix/src/IPCMixCommon.hpp"
#include "MainServiceComponent.hpp"
#include "LocalServiceComponent.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "13_shareipcmix.lib")
#endif // WINDOWS

/**
 * \brief   This file demonstrates simple initialization of local and remote servicing component
 *          used in multiprocessing application development. This executable contains main
 *          controlling component to shutdown system.
 And how to duplicate same
 *          implemented service in the system. The duplication should not happen in the
 *          same thread, rather in different threads with different role names.
 *          The servicing component does not contain any servicing method, it uses
 *          timer to run and exit application after certain time.
 **/

//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate twice the same implementation
// of component, but running in different thread and having different
// role name. Note, no need to make changes in the code.
//
//////////////////////////////////////////////////////////////////////////

// Describe mode, set model name
BEGIN_MODEL(IPCMixCommon::ModelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "Test_MainServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( IPCMixCommon::MainService, MainServiceComponent )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( NERemoteHelloWorld::ServiceName, NERemoteHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(IPCMixCommon::LocalService)
        // end of component description
        END_REGISTER_COMPONENT( IPCMixCommon::MainService )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( IPCMixCommon::LocalService, LocalServiceComponent )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_IMPLEMENT_SERVICE( NELocalHelloWorld::ServiceName, NELocalHelloWorld::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( IPCMixCommon::LocalService )
    // end of thread description
    END_REGISTER_THREAD( "Test_MainServiceThread" )

// end of model IPCMixCommon::ModelName
END_MODEL(IPCMixCommon::ModelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_12_ipchello_svcipchello_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    // force to start logging with default settings
    TRACER_FORCE_LOGGING();
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, NULL, NULL);

    do 
    {
        TRACE_SCOPE(example_12_ipchello_svcipchello_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", IPCMixCommon::ModelName);

        NEMemory::uAlign isMain = {true};
        ComponentLoader::setComponentData(IPCMixCommon::MainService, isMain);

        // load model to initialize components
        Application::loadModel(IPCMixCommon::ModelName);

        TRACE_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        Application::waitAppQuit(IESynchObject::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(IPCMixCommon::ModelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);
    
	return 0;
}
