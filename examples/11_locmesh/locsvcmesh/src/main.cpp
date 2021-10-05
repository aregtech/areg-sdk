//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
// Description : The application that instantiates multiple instances of the
//               same service and clients in different threads.The definition
//               of servicing components, service provider or service client
//               are done in modeling that are loaded on application start 
//               and unload when exit.
//
//               One of the components is marked as 'main' and the application
//               runs as long, until the main component does not trigger 'quit'
//               event to stop application and release the resources.Similar to
//               previous project, it has request, response, broadcast and 
//               subscription services features dynamically subscribe and 
//               unsubscribe to data update messages during run-time.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "locsvcmesh/src/MainComponent.hpp"
#include "locsvcmesh/src/SecondaryComponent.hpp"
#include "locsvcmesh/src/ClientComponent.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "11_generated.lib")
#endif // WINDOWS

constexpr char const _modelName[]       { "TestModel" };    //!< The name of model
constexpr char const _mainServiceName[] { "MainService" };  //!< Main component that controls shutdown.

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
            // register HelloWorld service implementation.
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
            // register HelloWorld service implementation and the dependencies.
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
            // register HelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY( _mainServiceName )
            REGISTER_DEPENDENCY( "Test_SecondaryComponent")
        // end of component description
        END_REGISTER_COMPONENT( "Test_ThirdComponent" )

        BEGIN_REGISTER_COMPONENT("Test_ClientComponent", ClientComponent)
            // register HelloWorld service dependencies.
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
DEF_TRACE_SCOPE(example_11_locsvcmesh_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    printf("Testing multiple local servicing components of the same service interface running with different names in different threads...\n");
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, false, true, nullptr, nullptr );

    do 
    {
        TRACE_SCOPE(example_11_locsvcmesh_main_main);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        NEMemory::uAlign isMain = {true};
        ComponentLoader::getInstance().setComponentData(_mainServiceName, isMain);

        // load model to initialize components
        Application::loadModel(_modelName);

        TRACE_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(_modelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);
    
    printf("Completed testing multiple local servicing components, check the logs...\n");

	return 0;
}
