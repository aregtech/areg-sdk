//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2022 Aregtech UG.All rights reserved.
// Description : This project contains multiple instances of Public and Local
//               services that form a mesh of services and make cross-
//               communication. The application shuts down when the 'main' 
//               service completes the job and exits.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "common/src/NECommon.hpp"
#include "common/src/LocalHelloWorldClient.hpp"
#include "common/src/LocalHelloWorldService.hpp"
#include "common/src/PublicHelloWorldClient.hpp"
#include "common/src/PublicHelloWorldService.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "13_generated.lib")
    #pragma comment(lib, "13_common.lib")
#endif // WINDOWS

//!<\brief  Local service component
class LocalServiceComponent : public Component
{
    static constexpr unsigned int   PUBLIC_CLIENT_TIMEOUT   { 1'000 };  //!< The timeout to send request to public service
    static constexpr unsigned int   LOCAL_CLIENT_TIMEOUT    {   500 };  //!< The timeout to send request to local service

public:
    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static inline Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    {
        return DEBUG_NEW LocalServiceComponent( entry, owner );
    }

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
    {
        delete (&compObject);
    }

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    //!<\brief  Initializes the local component
    LocalServiceComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
        : Component                 ( entry, owner )
        , mLocalService             ( static_cast<Component &>(self()) )
        , mControllerServiceClient  ( entry.mDependencyServices[0], static_cast<Component &>(self( )), PUBLIC_CLIENT_TIMEOUT )
        , mLocServiceClient         ( entry.mDependencyServices[1], static_cast<Component &>(self( )), LOCAL_CLIENT_TIMEOUT )
    {
    }

    virtual ~LocalServiceComponent( void ) = default;

private:
    LocalHelloWorldService  mLocalService;              //!< The instance of local service.
    PublicHelloWorldClient  mControllerServiceClient;   //!< The instance of Public service client.
    LocalHelloWorldClient   mLocServiceClient;          //!< The instance of local service client.

private:
    LocalServiceComponent & self( void )
    {
        return (*this);
    }
};


//!<\brief  A public service component
class PublicServiceComponent : public Component
{
    static constexpr unsigned int   TIMEOUT_CONTROLLER_SERVICE_CLIENT   {   700 };
    static constexpr unsigned int   TIMEOUT_PUBLIC_SERVICE_CLIENT       { 1'000 };
    static constexpr unsigned int   TIMEOUT_LOCAL_SERVICE_CLIENT_1      {   800 };
    static constexpr unsigned int   TIMEOUT_LOCAL_SERVICE_CLIENT_2      { 1'200 };

public:
    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static inline Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    {
        return DEBUG_NEW PublicServiceComponent( entry, owner );
    }

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
    {
        delete (&compObject);
    }

private:

    PublicServiceComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
        : Component                 ( entry, owner )
        , mPublicService            ( static_cast<Component &>(self()) )
        , mControllerServiceClient  ( entry.mDependencyServices[0], static_cast<Component &>(self()), TIMEOUT_CONTROLLER_SERVICE_CLIENT )
        , mPublicServiceClient      ( entry.mDependencyServices[1], static_cast<Component &>(self()), TIMEOUT_PUBLIC_SERVICE_CLIENT )
        , mLocalServiceClient1      ( entry.mDependencyServices[2], static_cast<Component &>(self()), TIMEOUT_LOCAL_SERVICE_CLIENT_1 )
        , mLocalServiceClient2      ( entry.mDependencyServices[3], static_cast<Component &>(self()), TIMEOUT_LOCAL_SERVICE_CLIENT_2 )
    {
    }

    virtual ~PublicServiceComponent( void ) = default;

private:
    PublicHelloWorldService mPublicService;
    PublicHelloWorldClient  mControllerServiceClient;
    PublicHelloWorldClient  mPublicServiceClient;
    LocalHelloWorldClient   mLocalServiceClient1;
    LocalHelloWorldClient   mLocalServiceClient2;

    PublicServiceComponent & self( void )
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
    BEGIN_REGISTER_THREAD( "MeshFirstThread", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::PublicSecondService, PublicServiceComponent )
            // register RemoteRegistry service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NEPublicHelloWorld::ServiceName, NEPublicHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::PublicControllerService)
            REGISTER_DEPENDENCY(NECommon::PublicThirdService)
            REGISTER_DEPENDENCY(NECommon::LocalService)
            REGISTER_DEPENDENCY( _localService )
        // end of component description
        END_REGISTER_COMPONENT( NECommon::PublicSecondService )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::LocalService, LocalServiceComponent )
            // register RemoteRegistry service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NELocalHelloWorld::ServiceName, NELocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::PublicThirdService)
            REGISTER_DEPENDENCY( _localService )
        // end of component description
        END_REGISTER_COMPONENT( NECommon::LocalService )
    // end of thread description
    END_REGISTER_THREAD( "MeshFirstThread" )

    BEGIN_REGISTER_THREAD( "MeshSecondThread", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::PublicThirdService, PublicServiceComponent )
            // register RemoteRegistry service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NEPublicHelloWorld::ServiceName, NEPublicHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::PublicSecondService)
            REGISTER_DEPENDENCY(NECommon::PublicThirdService)
            REGISTER_DEPENDENCY(NECommon::LocalService)
            REGISTER_DEPENDENCY( _localService )
        // end of component description
        END_REGISTER_COMPONENT( NECommon::PublicThirdService )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( _localService, LocalServiceComponent )
            // register LocalHelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NELocalHelloWorld::ServiceName, NELocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::PublicControllerService)
            REGISTER_DEPENDENCY( _localService )
        // end of component description
        END_REGISTER_COMPONENT( _localService )
    // end of thread description
    END_REGISTER_THREAD( "MeshSecondThread" )

// end of model NECommon::ModelName
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_13_pubmesh_pubsvcmesh_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    std::cout << "A Demo of meshed services. The process public and local services and clients ..." << std::endl;

    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication();

    do 
    {
        TRACE_SCOPE( example_13_pubmesh_pubsvcmesh_main_main );
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        std::cout << "Loading services, wait for services ..." << std::endl;

        // load model to initialize components
        Application::loadModel( _modelName );

        TRACE_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        // stop and unload components
        Application::unloadModel( _modelName );

        std::cout
            << (Application::findModel( _modelName ).getAliveDuration( ) / NECommon::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);
    
    std::cout << "Exit meshed services application, check the logs for details!" << std::endl;
    return 0;
}
