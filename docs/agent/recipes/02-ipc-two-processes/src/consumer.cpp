/**
 * \file    consumer.cpp
 * \brief   The process that consumes the service.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include "src/services/HelloServiceConsumerBase.hpp"

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
    }
};

constexpr char const _modelName[]{ "ConsumerModel" };

// A unique role name lets several consumer processes run at the same time.
const areg::String _consumer(areg::generate_name("ServiceConsumer"));

BEGIN_MODEL(_modelName)
    BEGIN_REGISTER_THREAD("ConsumerThread")
        BEGIN_REGISTER_COMPONENT(_consumer, ServiceConsumer)
            REGISTER_DEPENDENCY("ServiceProvider")
        END_REGISTER_COMPONENT(_consumer)
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
