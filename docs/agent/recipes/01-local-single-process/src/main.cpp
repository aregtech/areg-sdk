/**
 * \file    main.cpp
 * \brief   Provider and consumer of one service, in two threads of one process.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include "src/services/HelloServiceProviderBase.hpp"
#include "src/services/HelloServiceConsumerBase.hpp"

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
        std::cout << "provider: hello, " << client << std::endl;
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

    void response_hello_service(bool success) final
    {
        std::cout << "consumer: " << (success ? "greeted" : "failed") << std::endl;
        areg::Application::signal_quit();
    }

    void request_hello_service_failed(areg::ResultType /*reason*/) final
    {
        std::cerr << "consumer: request failed" << std::endl;
        areg::Application::signal_quit();
    }
};

constexpr char const _modelName[]{ "HelloModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("ProviderThread")
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceProvider")
    END_REGISTER_THREAD("ProviderThread")

    BEGIN_REGISTER_THREAD("ConsumerThread")
        BEGIN_REGISTER_COMPONENT("ServiceConsumer", ServiceConsumer)
            REGISTER_DEPENDENCY("ServiceProvider")
        END_REGISTER_COMPONENT("ServiceConsumer")
    END_REGISTER_THREAD("ConsumerThread")

END_MODEL(_modelName)

int main()
{
    areg::Application::setup();
    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return 0;
}
