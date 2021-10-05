//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
// Description : This project contains multiple instances of Public and Local
//               services that form a mesh of services and make cross-
//               communication. The application shuts down when the 'main' 
//               service completes the job and exits.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "generated/src/NECommon.hpp"

#include "pubsvcmesh/src/RemoteServiceComponent.hpp"
#include "pubsvcmesh/src/LocalServiceComponent.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "13_generated.lib")
#endif // WINDOWS


//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate twice the same implementation
// of component, but running in different thread and having different
// role name. Note, no need to make changes in the code.
//
//////////////////////////////////////////////////////////////////////////

constexpr char const AnotherLocalService[] { "AnotherLocalService" };

// Describe mode, set model name
BEGIN_MODEL(NECommon::ModelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestSecondServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::PublicSecondaryService, RemoteServiceComponent )
            // register RemoteRegistry service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::MainService)
            REGISTER_DEPENDENCY(NECommon::PublicThirdService)
            REGISTER_DEPENDENCY(NECommon::LocalService)
            REGISTER_DEPENDENCY(AnotherLocalService)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::PublicSecondaryService )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::LocalService, LocalServiceComponent )
            // register RemoteRegistry service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::PublicThirdService)
            REGISTER_DEPENDENCY(AnotherLocalService)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::LocalService )

    // end of thread description
    END_REGISTER_THREAD( "TestSecondServiceThread" )

    BEGIN_REGISTER_THREAD( "TestThirdServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::PublicThirdService, RemoteServiceComponent )
            // register RemoteRegistry service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NERemoteRegistry::ServiceName, NERemoteRegistry::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::PublicSecondaryService)
            REGISTER_DEPENDENCY(NECommon::PublicThirdService)
            REGISTER_DEPENDENCY(NECommon::LocalService)
            REGISTER_DEPENDENCY(AnotherLocalService)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::PublicThirdService )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( AnotherLocalService, LocalServiceComponent )
            // register LocalHelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NELocalHelloWorld::ServiceName, NELocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::MainService)
            REGISTER_DEPENDENCY(AnotherLocalService)
        // end of component description
        END_REGISTER_COMPONENT(AnotherLocalService )

    // end of thread description
    END_REGISTER_THREAD( "TestThirdServiceThread" )

// end of model NECommon::ModelName
END_MODEL(NECommon::ModelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_13_pubsvcmesh_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing and the timer.
    Application::initApplication(true, true, true, true, nullptr, nullptr );

    do 
    {
        TRACE_SCOPE(example_13_pubsvcmesh_main_main);
        String modelName(NECommon::ModelName);
        TRACE_DBG("The application has been initialized, loading model [ %s ]", modelName.getString());

        // load model to initialize components
        Application::loadModel(NECommon::ModelName);

        TRACE_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel(NECommon::ModelName);

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);
    
	return 0;
}
