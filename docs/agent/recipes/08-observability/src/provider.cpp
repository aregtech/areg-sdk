/**
 * \file    provider.cpp
 * \brief   The process that provides the service.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/logging/areg_log.h"

#include "src/services/HelloServiceProviderBase.hpp"

DEF_LOG_SCOPE(obs_Provider, request_hello_service);

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
        LOG_SCOPE(obs_Provider, request_hello_service);
        LOG_INFO("greeting %s", client.as_string());
        std::cout << "provider: hello, " << client << std::endl;
        response_hello_service(true);
    }

private:
    inline ServiceProvider & self()
    {   return (*this); }
};

constexpr char const _modelName[]{ "ProviderModel" };

BEGIN_MODEL(_modelName)
    BEGIN_REGISTER_THREAD("ProviderThread")
        BEGIN_REGISTER_COMPONENT("ServiceProvider", ServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("ServiceProvider")
    END_REGISTER_THREAD("ProviderThread")
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
