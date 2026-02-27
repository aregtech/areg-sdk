//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project contains multiple instances of Public and Local
//               services that form a mesh of services and make cross-
//               communication. The application shuts down when the 'main' 
//               service completes the job and exits.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/GELog.h"

#include "common/src/MeshDefs.hpp"
#include "common/src/LocalHelloWorldClient.hpp"
#include "common/src/LocalHelloWorldService.hpp"
#include "common/src/PublicHelloWorldClient.hpp"
#include "common/src/PublicHelloWorldService.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "16_generated")
    #pragma comment(lib, "16_common")
#endif // _MSC_VER

//!<\brief  Local service component
class LocalServiceComponent : public areg::Component
{
    static constexpr uint32_t   PUBLIC_CLIENT_TIMEOUT   { 1'000 };  //!< The timeout to send request to public service
    static constexpr uint32_t   LOCAL_CLIENT_TIMEOUT    {   500 };  //!< The timeout to send request to local service

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    //!< Initializes the local component
    LocalServiceComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
        : areg::Component                 ( entry, owner )
        , mLocalService             ( static_cast<areg::Component &>(self()) )
        , mControllerServiceClient  ( entry.mDependencyServices[0], static_cast<areg::Component &>(self( )), PUBLIC_CLIENT_TIMEOUT )
        , mLocServiceClient         ( entry.mDependencyServices[1], static_cast<areg::Component &>(self( )), LOCAL_CLIENT_TIMEOUT )
    {
    }

private:
    LocalHelloWorldService  mLocalService;              //!< The instance of local service.
    PublicHelloWorldClient  mControllerServiceClient;   //!< The instance of Public service client.
    LocalHelloWorldClient   mLocServiceClient;          //!< The instance of local service client.

private:
    LocalServiceComponent & self()
    {
        return (*this);
    }
};


//!<\brief  A public service component
class PublicServiceComponent : public areg::Component
{
    static constexpr uint32_t   TIMEOUT_CONTROLLER_SERVICE_CLIENT   {   700 };
    static constexpr uint32_t   TIMEOUT_PUBLIC_SERVICE_CLIENT       { 1'000 };
    static constexpr uint32_t   TIMEOUT_LOCAL_SERVICE_CLIENT_1      {   800 };
    static constexpr uint32_t   TIMEOUT_LOCAL_SERVICE_CLIENT_2      { 1'200 };

public:

    PublicServiceComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
        : areg::Component                 ( entry, owner )
        , mPublicService            ( static_cast<areg::Component &>(self()) )
        , mControllerServiceClient  ( entry.mDependencyServices[0], static_cast<areg::Component &>(self()), TIMEOUT_CONTROLLER_SERVICE_CLIENT )
        , mPublicServiceClient      ( entry.mDependencyServices[1], static_cast<areg::Component &>(self()), TIMEOUT_PUBLIC_SERVICE_CLIENT )
        , mLocalServiceClient1      ( entry.mDependencyServices[2], static_cast<areg::Component &>(self()), TIMEOUT_LOCAL_SERVICE_CLIENT_1 )
        , mLocalServiceClient2      ( entry.mDependencyServices[3], static_cast<areg::Component &>(self()), TIMEOUT_LOCAL_SERVICE_CLIENT_2 )
    {
    }

private:
    PublicHelloWorldService mPublicService;
    PublicHelloWorldClient  mControllerServiceClient;
    PublicHelloWorldClient  mPublicServiceClient;
    LocalHelloWorldClient   mLocalServiceClient1;
    LocalHelloWorldClient   mLocalServiceClient2;

    PublicServiceComponent & self()
    {
        return (*this);
    }

};


//////////////////////////////////////////////////////////////////////////
//
// The following design will instantiate twice the same implementation
// of component, but running in different thread and having different
// role name. Note, no need to make changes in the code.
//
//////////////////////////////////////////////////////////////////////////

constexpr char const _modelName[]       { "ModelName" };
constexpr char const _localService[]    { "AnotherLocalService" };

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "MeshFirstThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( mesh::PublicSecondService, PublicServiceComponent )
            // register RemoteRegistry service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( PublicHelloWorld::ServiceName, PublicHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(mesh::PublicControllerService)
            REGISTER_DEPENDENCY(mesh::PublicThirdService)
            REGISTER_DEPENDENCY(mesh::LocalService)
            REGISTER_DEPENDENCY( _localService )
        // end of component description
        END_REGISTER_COMPONENT( mesh::PublicSecondService )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( mesh::LocalService, LocalServiceComponent )
            // register RemoteRegistry service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( LocalHelloWorld::ServiceName, LocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(mesh::PublicThirdService)
            REGISTER_DEPENDENCY( _localService )
        // end of component description
        END_REGISTER_COMPONENT( mesh::LocalService )
    // end of thread description
    END_REGISTER_THREAD( "MeshFirstThread" )

    BEGIN_REGISTER_THREAD( "MeshSecondThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( mesh::PublicThirdService, PublicServiceComponent )
            // register RemoteRegistry service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( PublicHelloWorld::ServiceName, PublicHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(mesh::PublicSecondService)
            REGISTER_DEPENDENCY(mesh::PublicThirdService)
            REGISTER_DEPENDENCY(mesh::LocalService)
            REGISTER_DEPENDENCY( _localService )
        // end of component description
        END_REGISTER_COMPONENT( mesh::PublicThirdService )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _localService, LocalServiceComponent )
            // register LocalHelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( LocalHelloWorld::ServiceName, LocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(mesh::PublicControllerService)
            REGISTER_DEPENDENCY( _localService )
        // end of component description
        END_REGISTER_COMPONENT( _localService )
    // end of thread description
    END_REGISTER_THREAD( "MeshSecondThread" )

// end of model mesh::ModelName
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(example_16_pubmesh_pubsvcmesh_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    std::cout << "A Demo of meshed services. The process public and local services and clients ..." << std::endl;

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::initApplication();

    do 
    {
        LOG_SCOPE( example_16_pubmesh_pubsvcmesh_main_main );
        LOG_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        std::cout << "Loading services, wait for services ..." << std::endl;

        // load model to initialize components
        areg::Application::loadModel( _modelName );

        LOG_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        areg::Application::waitAppQuit(areg::WAIT_INFINITE);

        std::cout
            << (areg::Application::findModel( _modelName ).getAliveDuration( ) / areg::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        // release and cleanup resources of application.
        areg::Application::releaseApplication();

    } while (false);
    
    std::cout << "Exit meshed services application, check the logs for details!" << std::endl;
    return 0;
}
