/**
 * \file    src/provideripc.cpp
 * \brief   Minimal IPC example with request-response.
 *          It gets 'hello_service()' call from remote consumer running in other process.
 *          Then sends 'hello_service()' and quits application.
 *          This example requires `mtrouter`.
 **/

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/02_minimalipc/services/HelloServiceProviderBase.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "02_generated")
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Service Provider: ServiceProvider declaration
//////////////////////////////////////////////////////////////////////////
class ServiceProvider final : public    areg::Component
                            , protected HelloServiceProviderBase
{
public:
    ServiceProvider(const areg::ComponentEntry& entry, areg::ComponentThread& owner)
        : areg::Component(entry, owner)
        , HelloServiceProviderBase(static_cast<areg::Component&>(*this))
    {   }

//////////////////////////////////////////////////////////////////////////
// HelloService Interface Requests
//////////////////////////////////////////////////////////////////////////
protected:
    void request_hello_service() final
    {
        std::cout << "\'Hello Service!\'" << std::endl;
        response_hello_service();
        areg::Application::signal_quit();
    }

private:
    //!< The wrapper of this pointer.
    inline ServiceProvider& self()
    {   return (*this); }
};

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////
int main()
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog, using default settings.
    areg::Application::setup();
    // The service provider in "Thread1".
    areg::Model model("ProviderModel");
    model.add_thread("Thread1")
         .add_component<ServiceProvider>("ServiceProvider")
         .add_supported_service(HelloService::ServiceName, HelloService::InterfaceVersion);
    areg::ComponentLoader::add_model_unique(model);
    // load model to initialize components
    areg::Application::load_model("ProviderModel");
    // wait until Application quit signal is set.
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    // release and cleanup resources of application.
    areg::Application::release();
    return 0;
}
