//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
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
#include "areg/logging/GELog.h"

#include "locsvcmesh/src/ClientComponent.hpp"
#include "locsvcmesh/src/ServicingComponents.hpp"

#ifdef MS_VISUAL_CPP
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "11_generated.lib")
#endif // MS_VISUAL_CPP

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
    BEGIN_REGISTER_THREAD( "ServiceThread", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _mainServiceName, ControllerComponent )
            // register HelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
        // end of component description
        END_REGISTER_COMPONENT( _mainServiceName )
    // end of thread description
    END_REGISTER_THREAD( "ServiceThread" )

    //////////////////////////////////////////////////////////////////////////
    //
    // Use of service component in the other thread.
    //
    //////////////////////////////////////////////////////////////////////////

    BEGIN_REGISTER_THREAD( "SecondaryThread", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "SecondaryComponent", SecondaryComponent )
            // register HelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(_mainServiceName)
            REGISTER_DEPENDENCY("SecondaryComponent")
        // end of component description
        END_REGISTER_COMPONENT( "SecondaryComponent" )

    END_REGISTER_THREAD("SecondaryThread")

    // define component thread
    BEGIN_REGISTER_THREAD( "MixedThread", NECommon::WATCHDOG_IGNORE)

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( "ThirdComponent", SecondaryComponent )
            // register HelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NEHelloWorld::ServiceName, NEHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY( _mainServiceName )
            REGISTER_DEPENDENCY( "SecondaryComponent")
        // end of component description
        END_REGISTER_COMPONENT( "ThirdComponent" )

        BEGIN_REGISTER_COMPONENT("ClientComponent", ClientComponent)
            // register HelloWorld service dependencies.
            REGISTER_DEPENDENCY( _mainServiceName )
            REGISTER_DEPENDENCY( "SecondaryComponent")
            REGISTER_DEPENDENCY( "ThirdComponent" )
        END_REGISTER_COMPONENT("ClientComponent")

    // end of thread description
    END_REGISTER_THREAD( "MixedThread" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(example_11_locsvcmesh_main);
//! \brief   A Demo of mesh of local services and clients.
int main()
{
    std::cout << "A Demo of mesh of local services and clients ..." << std::endl;

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr );
    Application::initApplication( true, true, false, true, true, nullptr );

    do
    {
        LOG_SCOPE( example_11_locsvcmesh_main );
        LOG_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        NEMemory::uAlign data;
        data.alignBool.mElement = true;
        ComponentLoader::getInstance().setComponentData(_mainServiceName, data );

        // load model to initialize components
        Application::loadModel(_modelName);
        LOG_DBG("Servicing model is loaded");
        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);
        // stop and unload components
        Application::unloadModel(_modelName);

        std::cout
            << (Application::findModel( _modelName ).getAliveDuration( ) / NECommon::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    std::cout << "Exit application, check the logs for details!" << std::endl;
    return 0;
}
