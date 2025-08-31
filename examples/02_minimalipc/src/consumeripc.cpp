/**
 * \file    src/consumeripc.cpp
 * \brief   Minimal IPC example with request-response.
 *          It calls 'requestHelloService()' of remote object running in other process.
 *          Quits application when service is disconnected or received 'responseHelloService()' response.
 *          This example requires `mcrouter`.
 **/

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/02_minimalipc/services/HelloServiceClientBase.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "02_generated")
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// ServiceConsumer declaration
//////////////////////////////////////////////////////////////////////////
class ServiceConsumer   : public    Component
                        , protected HelloServiceClientBase
{
public:
    /**
     * \brief   Called to instantiate the service component when loading the model.
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
	{
	    return DEBUG_NEW ServiceConsumer(entry, owner);
	}

    /**
     * \brief   Called when unloading model to delete service component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & /*entry*/ )
	{
		delete (&compObject);
	}

protected:
    ServiceConsumer(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
		: Component             ( entry, owner )
		, HelloServiceClientBase( entry.mDependencyServices[0].mRoleName, owner )
	{   }

    //!< Service discovery notification. Called when the "ServiceProvder" is available and unavailable.
    //!< The `status` parameter contains availability flag. Return `true` if the service connection notification is relevant.
    virtual bool serviceConnected(NEService::eServiceConnection status, ProxyBase& proxy) override
    {
        if (HelloServiceClientBase::serviceConnected(status, proxy) && NEService::isServiceConnected(status))
            requestHelloService();  // Call of method of remote "ServiceProvider" object.
        else if (NEService::isServiceConnected(status) == false)
            Application::signalAppQuit(); // quit application if service connection is lost.

        // Return `true` if the service connection notification is relevant. "Relevance" can be checked via proxy.
        return (static_cast<const ProxyBase *>(getProxy()) == static_cast<const ProxyBase *>(&proxy));
    }

    //!< The response from Service Provider
    virtual void responseHelloService(void) override
    {
        std::cout << "\'Bey Service!\'" << std::endl;
        Application::signalAppQuit();   // quit application is if received response
    }
};

//////////////////////////////////////////////////////////////////////////
// Define the model to load and instantiate threads and objects
//////////////////////////////////////////////////////////////////////////
//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };
// Describe model, register the service and the client in 2 different threads "Thread1" and "Thread2"
BEGIN_MODEL(_modelName)
    // Thread 1 without watchdog, contains a service consumer
    BEGIN_REGISTER_THREAD( "Thread1", NECommon::WATCHDOG_IGNORE )
        BEGIN_REGISTER_COMPONENT( "ServiceClient", ServiceConsumer )
            REGISTER_DEPENDENCY( "ServiceProvider" ) /* dependency reference to the remote service*/
        END_REGISTER_COMPONENT( "ServiceClient" )
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
