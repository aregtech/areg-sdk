/**
 * \file    main.cpp
 * \brief   Publishing an attribute and sending a broadcast, with one subscriber.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include "src/services/MonitorProviderBase.hpp"
#include "src/services/MonitorConsumerBase.hpp"

constexpr uint32_t ALARM_LIMIT{ 40u };

class MonitorProvider final : public    areg::Component
                            , protected MonitorProviderBase
{
public:
    MonitorProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , MonitorProviderBase(static_cast<areg::Component &>(self()))
    {
        // An attribute is invalid until it is set once. Publish a value at startup.
        set_temperature(20u);
    }

protected:
    void request_measure() final
    {
        constexpr uint32_t measured{ 45u };

        set_temperature(measured);              // subscribers are notified
        if (measured > ALARM_LIMIT)
        {
            broadcast_alarm_raised(measured);   // every subscriber hears this
        }

        response_measure(true);
    }

private:
    inline MonitorProvider & self()
    {   return (*this); }
};

class MonitorConsumer final : public    areg::Component
                            , protected MonitorConsumerBase
{
public:
    MonitorConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , MonitorConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    { }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (MonitorConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                // Subscriptions are made here, and again after every reconnection.
                notify_on_temperature_update(true);
                notify_on_broadcast_alarm_raised(true);
                request_measure();
            }
        }

        return result;
    }

    void on_temperature_update(uint32_t Temperature, areg::DataState state) final
    {
        if (state == areg::DataState::DataIsOK)
        {
            std::cout << "consumer: temperature is " << Temperature << std::endl;
        }
        else
        {
            std::cout << "consumer: temperature not available" << std::endl;
        }
    }

    void broadcast_alarm_raised(uint32_t level) final
    {
        std::cout << "consumer: alarm at " << level << std::endl;
    }

    void response_measure(bool taken) final
    {
        std::cout << "consumer: measurement " << (taken ? "taken" : "refused") << std::endl;
        areg::Application::signal_quit();
    }
};

constexpr char const _modelName[]{ "MonitorModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("ProviderThread")
        BEGIN_REGISTER_COMPONENT("MonitorProvider", MonitorProvider)
            REGISTER_IMPLEMENT_SERVICE(Monitor::ServiceName, Monitor::InterfaceVersion)
        END_REGISTER_COMPONENT("MonitorProvider")
    END_REGISTER_THREAD("ProviderThread")

    BEGIN_REGISTER_THREAD("ConsumerThread")
        BEGIN_REGISTER_COMPONENT("MonitorConsumer", MonitorConsumer)
            REGISTER_DEPENDENCY("MonitorProvider")
        END_REGISTER_COMPONENT("MonitorConsumer")
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
