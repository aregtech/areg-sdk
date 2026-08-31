/**
 * \file    main.cpp
 * \brief   Two services and three components. The middle one is both a consumer
 *          of the first service and the provider of the second.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include "src/services/MeterServiceProviderBase.hpp"
#include "src/services/MeterServiceConsumerBase.hpp"
#include "src/services/ReportServiceProviderBase.hpp"
#include "src/services/ReportServiceConsumerBase.hpp"

//! Owns the meter value and publishes it as an attribute.
class MeterProvider final   : public    areg::Component
                            , protected MeterServiceProviderBase
{
public:
    MeterProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , MeterServiceProviderBase(static_cast<areg::Component &>(self()))
    { }

protected:
    void startup_component(areg::ComponentThread & comThread) final
    {
        areg::Component::startup_component(comThread);
        set_reading(42);
    }

private:
    inline MeterProvider & self()
    {   return (*this); }
};

//! Consumer of one service and provider of another. A component may be both.
class Collector final   : public    areg::Component
                        , protected MeterServiceConsumerBase
                        , protected ReportServiceProviderBase
{
public:
    Collector(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , MeterServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
        , ReportServiceProviderBase(static_cast<areg::Component &>(self()))
    { }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (MeterServiceConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                notify_on_reading_update(true);
            }
        }

        return result;
    }

    //! The first notification can carry an invalid value even when the provider
    //! has already set one. The state has to be checked every time.
    void on_reading_update(uint32_t Reading, areg::DataState state) final
    {
        if (state == areg::DataState::DataIsOK)
        {
            std::cout << "collector: reading " << Reading << std::endl;
            broadcast_report(Reading);
        }
    }

private:
    inline Collector & self()
    {   return (*this); }
};

class Display final : public    areg::Component
                    , protected ReportServiceConsumerBase
{
public:
    Display(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , ReportServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    { }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (ReportServiceConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                notify_on_broadcast_report(true);
            }
        }

        return result;
    }

    void broadcast_report(uint32_t value) final
    {
        std::cout << "display: report " << value << std::endl;
        areg::Application::signal_quit();
    }
};

constexpr char const _modelName[]{ "MeterModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("MeterThread")
        BEGIN_REGISTER_COMPONENT("MeterProvider", MeterProvider)
            REGISTER_IMPLEMENT_SERVICE(MeterService::ServiceName, MeterService::InterfaceVersion)
        END_REGISTER_COMPONENT("MeterProvider")
    END_REGISTER_THREAD("MeterThread")

    BEGIN_REGISTER_THREAD("CollectorThread")
        BEGIN_REGISTER_COMPONENT("Collector", Collector)
            REGISTER_IMPLEMENT_SERVICE(ReportService::ServiceName, ReportService::InterfaceVersion)
            REGISTER_DEPENDENCY("MeterProvider")
        END_REGISTER_COMPONENT("Collector")
    END_REGISTER_THREAD("CollectorThread")

    BEGIN_REGISTER_THREAD("DisplayThread")
        BEGIN_REGISTER_COMPONENT("Display", Display)
            REGISTER_DEPENDENCY("Collector")
        END_REGISTER_COMPONENT("Display")
    END_REGISTER_THREAD("DisplayThread")

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
