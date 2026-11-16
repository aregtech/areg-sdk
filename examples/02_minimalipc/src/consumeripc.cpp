/**
 * \file    src/consumeripc.cpp
 * \brief   Minimal IPC example with request-response.
 *          It calls 'hello_service()' of remote object running in other process.
 *          Quits application when service is disconnected or received 'hello_service()' response.
 *          This example requires `mtrouter`.
 **/

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/02_minimalipc/services/HelloServiceConsumerBase.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "02_generated")
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// ServiceConsumer declaration
//////////////////////////////////////////////////////////////////////////
class ServiceConsumer final : public    areg::Component
                            , protected HelloServiceConsumerBase
{
public:
    ServiceConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
		: areg::Component( entry, owner )
		, HelloServiceConsumerBase( entry.mDependencyServices[0].mRoleName, owner )
	{   }

protected:
    //!< Service discovery notification. Called when the "ServiceProvder" is available and unavailable.
    //!< The `status` parameter contains availability flag. Return `true` if the service connection notification is relevant.
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase& proxy) final
    {
        if (HelloServiceConsumerBase::service_connected(status, proxy) && areg::is_service_connected(status))
            request_hello_service();  // Call of method of remote "ServiceProvider" object.
        else if (areg::is_service_connected(status) == false)
            areg::Application::signal_quit(); // quit application if service connection is lost.

        // Return `true` if the service connection notification is relevant. "Relevance" can be checked via proxy.
        return (static_cast<const areg::ProxyBase *>(service_proxy()) == static_cast<const areg::ProxyBase *>(&proxy));
    }

    //!< The response from Service Provider
    void response_hello_service() final
    {
        std::cout << "\'Good bye Service!\'" << std::endl;
        areg::Application::signal_quit();   // quit application is if received response
    }
};

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////
int main()
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog, using default settings.
    areg::Application::setup();
    // The service consumer in "Thread1", which depends on the remote "ServiceProvider".
    areg::Model model("ConsumerModel");
    model.add_thread("Thread1")
         .add_component<ServiceConsumer>("ServiceConsumer")
         .add_dependency_service("ServiceProvider");
    areg::ComponentLoader::add_model_unique(model);
    // load model to initialize components
    areg::Application::load_model("ConsumerModel");
    // wait until Application quit signal is set.
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    // release and cleanup resources of application.
    areg::Application::release();
    return 0;
}
