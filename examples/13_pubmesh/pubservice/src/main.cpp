//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project is marked as 'main' and contains instances of 
//               the Public service components, where one service 
//               (ServiceShtodwn) handles the system shutdown logic, so that
//               the service clients get notification when the 'main' 
//               application exits and can as well stop working.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/Component.hpp"
#include "areg/logging/GELog.h"

#include "common/src/NECommon.hpp"
#include "common/src/LocalHelloWorldService.hpp"
#include "common/src/PublicHelloWorldClient.hpp"
#include "pubservice/src/PublicServiceComponent.hpp"

#ifdef MS_VISUAL_CPP
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "13_generated.lib")
    #pragma comment(lib, "13_common")
#endif // MS_VISUAL_CPP

//!<\brief  The local service component.
class LocalServiceComponent : public Component
{
    static constexpr unsigned int TIMEOUT_CONTROLLER_SERVICE_CLIENT{ 500 };

public:

    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    {
        return DEBUG_NEW LocalServiceComponent( entry, owner );
    }

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     *          entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /* entry */ )
    {
        delete (&compObject);
    }

protected:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    LocalServiceComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
        : Component         ( entry, owner )
        , mLocalService     ( static_cast<Component &>(self()) )
        , mControllerClient ( entry.mDependencyServices[0], static_cast<Component &>(self()), TIMEOUT_CONTROLLER_SERVICE_CLIENT )
    {
    }

    /**
     * \brief   Destructor.
     **/
    virtual ~LocalServiceComponent( void ) = default;

private:
    LocalHelloWorldService  mLocalService;
    PublicHelloWorldClient  mControllerClient;

    LocalServiceComponent & self( void )
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

//!< The name of model, common in every process.
constexpr char  _modelName[]{ "TestModel" };

// Describe mode, set model name
BEGIN_MODEL( _modelName )

    // define component thread
    BEGIN_REGISTER_THREAD( "ControllerServiceThread", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::PublicControllerService, PublicServiceComponent )
            // register RemoteRegistry, SystemShutdown service implementation and the dependency.
            REGISTER_IMPLEMENT_SERVICE( NEPublicHelloWorld::ServiceName, NEPublicHelloWorld::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( NESystemShutdown::ServiceName, NESystemShutdown::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::LocalService)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::PublicControllerService )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::LocalService, LocalServiceComponent )
            // register LocalHelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( NELocalHelloWorld::ServiceName, NELocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::PublicControllerService)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::LocalService )

    // end of thread description
    END_REGISTER_THREAD( "TestMainServiceThread" )

// end of model NECommon::ModelName
END_MODEL( _modelName )

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(example_13_pubmesh_pubservice_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    std::cout << "A Demo of meshed services. The process with controller service, public and local services and clients ..." << std::endl;

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    do 
    {
        LOG_SCOPE( example_13_pubmesh_pubservice_main_main );
        LOG_DBG("The application has been initialized, loading model [ %s ]", _modelName );

        std::cout << "Loading services, wait for services ..." << std::endl;

        // load model to initialize components
        Application::loadModel( _modelName );

        LOG_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        std::cout
            << (Application::findModel( _modelName ).getAliveDuration( ) / NECommon::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);
    
    std::cout << "Exit controller services application, check the logs for details!" << std::endl;
    return 0;
}
