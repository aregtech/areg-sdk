//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project contains multiple instances of Public service
//               clients to invoke remote method calls.The system guarantees
//               that the each call is delivered to the target and the
//               messages are not mixed or lost. The application shuts down
//               when the 'main' service completes the job and exits.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include "common/src/LocalHelloWorldClient.hpp"
#include "common/src/LocalHelloWorldService.hpp"
#include "common/src/PublicHelloWorldClient.hpp"
#include "common/src/MeshDefs.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "16_generated")
    #pragma comment(lib, "16_common")
#endif // _MSC_VER

//!<\brief   A local service component. It also contains service client object.
class ServiceComponent : public areg::Component
{
//////////////////////////////////////////////////////////////////////////
// Statics and constants
//////////////////////////////////////////////////////////////////////////
    static constexpr uint32_t   MAIN_TIMEOUT{ 1'000 };  //!< Timeout to send request to main controller remote service
    static constexpr uint32_t   SECOND_TIMEOUT{ 2'000 };  //!< Timeout to send request to second remote service
    static constexpr uint32_t   THIRD_TIMEOUT{ 3'000 };  //!< Timeout to send request to third remote service
    static constexpr uint32_t   LOCAL_TIMEOUT{ 4'000 };  //!< Timeout to send request to a local service.

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    ServiceComponent( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
        : areg::Component         ( entry.mRoleName, owner )
        , mLocalService     ( static_cast<areg::Component &>(self()) )
        , mControllerClient ( entry.mDependencyServices[0], static_cast<areg::Component &>(self()), ServiceComponent::MAIN_TIMEOUT   )
        , mSecondClient     ( entry.mDependencyServices[1], static_cast<areg::Component &>(self()), ServiceComponent::SECOND_TIMEOUT )
        , mThirdClient      ( entry.mDependencyServices[2], static_cast<areg::Component &>(self()), ServiceComponent::THIRD_TIMEOUT  )
        , mLocalClient      ( entry.mDependencyServices[3], static_cast<areg::Component &>(self()), ServiceComponent::LOCAL_TIMEOUT  )
    {
    }
private:
    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    LocalHelloWorldService  mLocalService;
    PublicHelloWorldClient  mControllerClient;
    PublicHelloWorldClient  mSecondClient;
    PublicHelloWorldClient  mThirdClient;
    LocalHelloWorldClient   mLocalClient;

    //////////////////////////////////////////////////////////////////////////
    // Hidden calls.
    //////////////////////////////////////////////////////////////////////////
    inline ServiceComponent & self()
    {
        return (*this);
    }

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    ServiceComponent() = delete;
    AREG_NOCOPY_NOMOVE( ServiceComponent );
};

constexpr char const _modelName[]{ "ClientsModel" };

// Describe mode, set model name
BEGIN_MODEL( _modelName )

    // define component thread
    BEGIN_REGISTER_THREAD( "TestClientProcessThread" )

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( mesh::LocalService, ServiceComponent )
            // register LocalHelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( LocalHelloWorld::ServiceName, LocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(mesh::PublicControllerService)
            REGISTER_DEPENDENCY(mesh::PublicSecondService)
            REGISTER_DEPENDENCY(mesh::PublicThirdService)
            REGISTER_DEPENDENCY(mesh::LocalService)
        // end of component description
        END_REGISTER_COMPONENT( areg::LocalService )

    // end of thread description
    END_REGISTER_THREAD( "TestClientProcessThread" )

// end of model areg::ModelName
END_MODEL( _modelName )

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(example_16_pubmesh_pubclients_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    std::cout << "A Demo of meshed services. The process of clients ..." << std::endl;

    // force to start logging with default settings
    LOGGING_CONFIGURE_AND_START( nullptr, false );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::setup( );

    do
    {
        LOG_SCOPE( example_16_pubmesh_pubclients_main_main );
        LOG_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        std::cout << "Wait for services ..." << std::endl;

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

    std::cout << "Exit clients application, check the logs for details!" << std::endl;
    return 0;
}
