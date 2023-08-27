//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project contains multiple instances of Public service
//               clients to invoke remote method calls.The system guarantees
//               that the each call is delivered to the target and the
//               messages are not mixed or lost. The application shuts down
//               when the 'main' service completes the job and exits.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "common/src/LocalHelloWorldClient.hpp"
#include "common/src/LocalHelloWorldService.hpp"
#include "common/src/PublicHelloWorldClient.hpp"
#include "common/src/NECommon.hpp"

#ifdef WIN32
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "13_generated.lib")
    #pragma comment(lib, "13_common.lib")
#endif // WIN32

//!<\brief   A local service component. It also contains service client object.
class ServiceComponent : public Component
{
    //////////////////////////////////////////////////////////////////////////
    // Statics and constants
    //////////////////////////////////////////////////////////////////////////
    static constexpr unsigned int   MAIN_TIMEOUT{ 1'000 };  //!< Timeout to send request to main controller remote service
    static constexpr unsigned int   SECOND_TIMEOUT{ 2'000 };  //!< Timeout to send request to second remote service
    static constexpr unsigned int   THIRD_TIMEOUT{ 3'000 };  //!< Timeout to send request to third remote service
    static constexpr unsigned int   LOCAL_TIMEOUT{ 4'000 };  //!< Timeout to send request to a local service.

public:

    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    {
        return DEBUG_NEW ServiceComponent( entry, owner );
    }

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry )
    {
        delete (&compObject);
    }

    //////////////////////////////////////////////////////////////////////////
    // Constructor / destructor
    //////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    ServiceComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
        : Component         ( entry.mRoleName, owner )
        , mLocalService     ( static_cast<Component &>(self()) )
        , mControllerClient ( entry.mDependencyServices[0], static_cast<Component &>(self()), ServiceComponent::MAIN_TIMEOUT   )
        , mSecondClient     ( entry.mDependencyServices[1], static_cast<Component &>(self()), ServiceComponent::SECOND_TIMEOUT )
        , mThirdClient      ( entry.mDependencyServices[2], static_cast<Component &>(self()), ServiceComponent::THIRD_TIMEOUT  )
        , mLocalClient      ( entry.mDependencyServices[3], static_cast<Component &>(self()), ServiceComponent::LOCAL_TIMEOUT  )
{
}

    /**
     * \brief   Destructor.
     **/
    virtual ~ServiceComponent( void ) = default;

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
    inline ServiceComponent & self( void )
    {
        return (*this);
    }

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    ServiceComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServiceComponent );
};

constexpr char const _modelName[]{ "ClientsModel" };

// Describe mode, set model name
BEGIN_MODEL( _modelName )

    // define component thread
    BEGIN_REGISTER_THREAD( "TestClientProcessThread", NECommon::WATCHDOG_IGNORE)

        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::LocalService, ServiceComponent )
            // register LocalHelloWorld service implementation and the dependencies.
            REGISTER_IMPLEMENT_SERVICE( NELocalHelloWorld::ServiceName, NELocalHelloWorld::InterfaceVersion )
            REGISTER_DEPENDENCY(NECommon::PublicControllerService)
            REGISTER_DEPENDENCY(NECommon::PublicSecondService)
            REGISTER_DEPENDENCY(NECommon::PublicThirdService)
            REGISTER_DEPENDENCY(NECommon::LocalService)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::LocalService )

    // end of thread description
    END_REGISTER_THREAD( "TestClientProcessThread" )

// end of model NECommon::ModelName
END_MODEL( _modelName )

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(example_13_pubmesh_pubclients_main_main);
/**
 * \brief   The main method enables logging, service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    std::cout << "A Demo of meshed services. The process of clients ..." << std::endl;

    // force to start logging with default settings
    TRACER_CONFIGURE_AND_START( nullptr );
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    do
    {
        TRACE_SCOPE( example_13_pubmesh_pubclients_main_main );
        TRACE_DBG("The application has been initialized, loading model [ %s ]", _modelName);

        std::cout << "Wait for services ..." << std::endl;

        // load model to initialize components
        Application::loadModel( _modelName );

        TRACE_DBG("Servicing model is loaded");

        // wait until Application quit signal is set.
        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        std::cout
            << (Application::findModel( _modelName ).getAliveDuration( ) / NECommon::DURATION_1_MILLI)
            << " ms passed. Model is unloaded, releasing resources to exit application ..."
            << std::endl;

        // release and cleanup resources of application.
        Application::releaseApplication();

    } while (false);

    std::cout << "Exit clients application, check the logs for details!" << std::endl;
    return 0;
}
