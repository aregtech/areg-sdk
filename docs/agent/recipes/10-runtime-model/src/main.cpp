/**
 * \file    main.cpp
 * \brief   The model built at run time instead of by macro: the number of
 *          consumers is decided when the application starts.
 **/
#include <atomic>
#include <iostream>
#include <mutex>
#include <sstream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Model.hpp"

#include "src/services/HelloServiceProviderBase.hpp"
#include "src/services/HelloServiceConsumerBase.hpp"

namespace
{
    //! How many consumers to create. A real application reads this from its
    //! command line or its configuration; the count is what the macros cannot take.
    constexpr uint32_t  ConsumerCount   { 3 };

    std::atomic_uint    theAnswered     { 0 };

    std::mutex          theOutputLock;

    //! Writes one whole line. Four threads share std::cout here, and a chain of
    //! << from two of them at once interleaves and tears the line in half.
    void say( const std::string & line )
    {
        const std::lock_guard<std::mutex> lock( theOutputLock );
        std::cout << line << std::endl;
    }
}

class ServiceProvider final : public    areg::Component
                            , protected HelloServiceProviderBase
{
public:
    ServiceProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , HelloServiceProviderBase(static_cast<areg::Component &>(self()))
    { }

protected:
    void request_hello_service(const areg::String & client) final
    {
        std::ostringstream line;
        line << "provider: hello, " << client;
        say( line.str() );
        response_hello_service(true);
    }

private:
    inline ServiceProvider & self()
    {   return (*this); }
};

class ServiceConsumer final : public    areg::Component
                            , protected HelloServiceConsumerBase
{
public:
    ServiceConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , HelloServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    { }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (HelloServiceConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                request_hello_service(role_name());
            }
        }

        return result;
    }

    void response_hello_service(bool /*success*/) final
    {
        const uint32_t answered{ theAnswered.fetch_add(1) + 1 };
        std::ostringstream line;
        line << "consumer: greeted " << answered;
        say( line.str() );
        if (answered == ConsumerCount)
        {
            areg::Application::signal_quit();
        }
    }
};

int main()
{
    areg::Application::setup();

    // Nothing below is a macro. These are the same classes BEGIN_MODEL fills in.
    areg::Model model("RuntimeModel");

    areg::ComponentThreadEntry & providerThread = model.add_thread("ProviderThread");
    areg::ComponentEntry & provider = providerThread.add_component<ServiceProvider>("ServiceProvider");
    provider.add_supported_service(HelloService::ServiceName, HelloService::InterfaceVersion);

    for (uint32_t i = 1; i <= ConsumerCount; ++i)
    {
        const areg::String name{ areg::String("Consumer") + areg::String::make_string(i) };
        areg::ComponentThreadEntry & thread = model.add_thread(name + "Thread");
        areg::ComponentEntry & consumer = thread.add_component<ServiceConsumer>(name);
        consumer.add_dependency_service("ServiceProvider");
    }

    areg::ComponentLoader::add_model_unique(model);
    areg::Application::load_model(nullptr);      // nullptr: every model added so far
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(nullptr);
    areg::Application::release();
    return 0;
}
