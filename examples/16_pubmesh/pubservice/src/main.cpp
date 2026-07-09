//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : This project is marked as 'main' and contains instances of 
//               the Public service components, where one service 
//               (ServiceShtodwn) handles the system shutdown logic, so that
//               the service clients get notification when the 'main' 
//               application exits and can as well stop working.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/Component.hpp"
#include "areg/logging/areg_log.h"

#include "common/src/MeshDefs.hpp"
#include "common/src/LocalHelloWorldService.hpp"
#include "common/src/PublicHelloWorldClient.hpp"
#include "pubservice/src/PublicServiceComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "16_generated")
    #pragma comment(lib, "16_common")
#endif // _MSC_VER

//!<\brief  The local service component.
class LocalServiceComponent : public areg::Component
{
    static constexpr uint32_t TIMEOUT_CONTROLLER_SERVICE_CLIENT{ 500 };

public:
    LocalServiceComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
        : areg::Component         ( entry, owner )
        , mLocalService     ( static_cast<areg::Component &>(self()) )
        , mControllerClient ( entry.mDependencyServices[0], static_cast<areg::Component &>(self()), TIMEOUT_CONTROLLER_SERVICE_CLIENT )
    {
    }

private:
    LocalHelloWorldService  mLocalService;
    PublicHelloWorldClient  mControllerClient;

    LocalServiceComponent & self()
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
    BEGIN_REGISTER_THREAD( "ControllerServiceThread" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( mesh::PublicControllerService, PublicServiceComponent )
            // register RemoteRegistry, SystemShutdown service implementation and the dependency.
            REGISTER_IMPLEMENT_SERVICE( PublicHelloWorld::ServiceName, PublicHelloWorld::InterfaceVersion )
            REGISTER_IMPLEMENT_SERVICE( SystemShutdown::ServiceName, SystemShutdown::InterfaceVersion )
            REGISTER_DEPENDENCY(mesh::LocalService)
        // end of component description
        END_REGISTER_COMPONENT( mesh::PublicControllerService )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( mesh::LocalService, LocalServiceComponent )
            // register LocalHelloWorld service implementation.
            REGISTER_IMPLEMENT_SERVICE( LocalHelloWorld::ServiceName, LocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(mesh::PublicControllerService)
        // end of component description
        END_REGISTER_COMPONENT( mesh::LocalService )

    // end of thread description
    END_REGISTER_THREAD( "TestMainServiceThread" )

// end of model mesh::ModelName
END_MODEL( _modelName )

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(examples_16_pubmesh_pubservice_main, main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    std::cout << "A Demo of meshed services. The process with controller service, public and local services and clients ..." << std::endl;

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr, false );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::setup( );

    do 
    {
        LOG_SCOPE( examples_16_pubmesh_pubservice_main, main );
        LOG_DBG("The application has been initialized, loading model [ %s ]", _modelName );

        std::cout << "Loading services, wait for services ..." << std::endl;

        // load model to initialize components
        areg::Application::load_model( _modelName );

        LOG_DBG("Servicing model is loaded");
        
        // wait until Application quit signal is set.
        areg::Application::wait_quit(areg::WAIT_INFINITE);

        std::cout
            << (areg::Application::find_model( _modelName ).alive_duration( ) / areg::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        // release and cleanup resources of application.
        areg::Application::release();

    } while (false);
    
    std::cout << "Exit controller services application, check the logs for details!" << std::endl;
    return 0;
}
