/**
 * \file    src/provideripc.cpp
 * \brief   Minimal IPC example with request-response.
 *          It gets 'requestHelloService()' call from remote consumer running in other process.
 *          Then sends 'responseHelloService()' and quits application.
 *          This example requires `mcrouter`.
 **/

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/02_minimalipc/services/HelloServiceStub.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "02_generated")
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Service Provider: ServiceProvider declaration
//////////////////////////////////////////////////////////////////////////
class ServiceProvider   : public    Component
                        , protected HelloServiceStub
{
public:
    /**
     * \brief   Called to instantiate the service component when loading the model.
     **/
    static Component* CreateComponent(const NERegistry::ComponentEntry& entry, ComponentThread& owner)
    {
        return DEBUG_NEW ServiceProvider(entry, owner);
    }

    /**
     * \brief   Called when unloading model to delete service component.
     **/
    static void DeleteComponent(Component& compObject, const NERegistry::ComponentEntry& /*entry*/)
    {
        delete (&compObject);
    }

protected:
    ServiceProvider(const NERegistry::ComponentEntry& entry, ComponentThread& owner)
        : Component(entry, owner)
        , HelloServiceStub(static_cast<Component&>(self()))
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // HelloService Interface Requests
    //////////////////////////////////////////////////////////////////////////
    virtual void requestHelloService(void) override
    {
        std::cout << "\'Hello Service!\'" << std::endl;
        responseHelloService();
        Application::signalAppQuit();
    }

private:
    //!< The wrapper of this pointer.
    inline ServiceProvider& self(void)
    {   return (*this); }
};

//////////////////////////////////////////////////////////////////////////
// Define the model to load and instantiate threads and objects
//////////////////////////////////////////////////////////////////////////
//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };

// Describe model, register the service and the client in 2 different threads "Thread1" and "Thread2"
BEGIN_MODEL(_modelName)
    // Thread 1, provides a service
    BEGIN_REGISTER_THREAD( "Thread1", NECommon::WATCHDOG_IGNORE )
        BEGIN_REGISTER_COMPONENT( "ServiceProvider", ServiceProvider )
            REGISTER_IMPLEMENT_SERVICE( NEHelloService::ServiceName, NEHelloService::InterfaceVersion )
        END_REGISTER_COMPONENT( "ServiceProvider" )
    END_REGISTER_THREAD( "Thread1" )
// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////
int main(void)
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog, using default settings.
    Application::initApplication();
    // load model to initialize components
    Application::loadModel(_modelName);
    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
    // stop and unload components
    Application::unloadModel(_modelName);
    // release and cleanup resources of application.
    Application::releaseApplication();
    return 0;
}
