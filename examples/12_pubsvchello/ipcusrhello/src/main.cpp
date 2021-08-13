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
#include "areg/base/NEUtilities.hpp"
#include "areg/trace/GETrace.h"


#include "generated/src/NEProjectCommon.hpp"
#include "ServiceClient.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg")
    #pragma comment(lib, "12_generated.lib")
#endif // WINDOWS

/**
 * \brief   This file demonstrates simple initialization of servicing local component
 *          used for local multi-threading communication. And how to duplicate same
 *          implemented service in the system. The duplication should not happen in the
 *          same thread, rather in different threads with different role names.
 *          The servicing component does not contain any servicing method, it uses
 *          timer to run and exit application after certain time.
 **/

static const char * _modelName      = "HelloModel";  //!< The name of model
static const String _serviceClient  = NEUtilities::generateName("ServiceClient"); //!< Generated name of service client component
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
        BEGIN_REGISTER_COMPONENT( _serviceClient.getString(), ServiceClient )
            // register dummy 'empty service'. In this example we demonstrate simple initialization
            REGISTER_DEPENDENCY(NEProjectCommon::PublicServiceName)
        // end of component description
        END_REGISTER_COMPONENT( _serviceClient.getString() )
    // end of thread description
    END_REGISTER_THREAD( "TestServiceThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_12_ipcusrhello_ipcusrhello_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing remote servicing ultra-small client...\n");
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START(NULL);
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, NULL, NULL);

    do 
    {
        TRACE_SCOPE(example_12_ipcusrhello_ipcusrhello_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

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
    
    printf("Completed testing remote servicing client, check the logs...\n");

	return 0;
}