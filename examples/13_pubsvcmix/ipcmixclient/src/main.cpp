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

#include "generated/src/NECommon.hpp"
#include "ipcmixclient/src/LocalServiceComponent.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "13_generated.lib")
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
BEGIN_MODEL(NECommon::ModelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestClientProcessThread" )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::LocalService, LocalServiceComponent )
            // register LocalHelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NELocalHelloWorld::ServiceName, NELocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::MainService)
            REGISTER_DEPENDENCY(NECommon::PublicSecondaryService)
            REGISTER_DEPENDENCY(NECommon::PublicThirdService)
            REGISTER_DEPENDENCY(NECommon::LocalService)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::LocalService )

    // end of thread description
    END_REGISTER_THREAD( "TestClientProcessThread" )

// end of model NECommon::ModelName
END_MODEL(NECommon::ModelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_13_pubsvcmix_ipcmixclient_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START(NULL);
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, NULL, NULL);

    do 
    {
        TRACE_SCOPE(example_13_pubsvcmix_ipcmixclient_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", NECommon::ModelName);

        // load model to initialize components
        Application::loadModel(NECommon::ModelName);

        TRACE_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        Application::waitAppQuit(IESynchObject::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(NECommon::ModelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);
    
	return 0;
}
