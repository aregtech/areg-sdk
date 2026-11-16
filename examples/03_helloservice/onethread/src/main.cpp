/**
 * \file    onethread/src/main.cpp
 * \brief   Runs service and the client in one thread.
 **/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ServiceComponent.hpp"
#include "common/src/ClientComponent.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "03_generated")
#endif // _MSC_VER

//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////

int main()
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::setup( );

    // The service and the client in one thread "Thread1".
    areg::Model model(_modelName);
    areg::ComponentThreadEntry & thread = model.add_thread("Thread1");
    thread.add_component<ServiceComponent>("ServiceComponent")
          .add_supported_service(HelloService::ServiceName, HelloService::InterfaceVersion);
    thread.add_component<ClientComponent>("ServiceClient")
          .add_dependency_service("ServiceComponent");
    areg::ComponentLoader::add_model_unique(model);

    // load model to initialize components
    areg::Application::load_model(_modelName);

    // wait until Application quit signal is set.
    areg::Application::wait_quit(areg::WAIT_INFINITE);

    // stop and unload components
    areg::Application::unload_model(_modelName);

    // release and cleanup resources of application.
    areg::Application::release();

    return 0;
}
