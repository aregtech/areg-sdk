/**
 * \file    foreign_provider.cpp
 * \brief   Service provider of the foreign calls test, in its own model. Answers its requests
 *          on its own thread, or leaves them pending for a foreign thread to answer.
 */

#include "foreign_calls.hpp"

#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/33_tempalarm/services/TempMonitorServiceProviderBase.hpp"

//!< Service provider. Publishes itself for the foreign threads of the test.
class ForeignProvider final : public    areg::Component
                            , public    TempMonitorServiceProviderBase
                            , public    foreign::ProviderCalls
{
public:
    ForeignProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component                   ( entry, owner )
        , TempMonitorServiceProviderBase    ( static_cast<areg::Component &>(*this) )
    {
    }

    void startup_service_interface(areg::Component & holder) override
    {
        TempMonitorServiceProviderBase::startup_service_interface(holder);
        set_reading(0);
        set_alarm_active(false);
        foreign::gProvider.store(static_cast<foreign::ProviderCalls *>(this));
    }

    void shutdown_service_interface(areg::Component & holder) noexcept override
    {
        foreign::gProvider.store(nullptr);
        TempMonitorServiceProviderBase::shutdown_service_interface(holder);
    }

    void request_set_thresholds(int16_t /*high_limit*/, int16_t /*hysteresis*/) override
    {
        foreign::gPendingThresholds.fetch_add(1);
    }

    void request_get_statistics() override
    {
        response_get_statistics(0, 0, 0);
    }

    void call_set_reading(int16_t value) override
    {
        set_reading(value);
    }

    void call_broadcast_alarm_raised(int16_t value) override
    {
        broadcast_alarm_raised(value);
    }

    void call_response_set_thresholds() override
    {
        response_set_thresholds(true, areg::String("foreign"));
    }
};

BEGIN_MODEL("ForeignProviderModel")
    BEGIN_REGISTER_THREAD( "ForeignProviderThread" )
        BEGIN_REGISTER_COMPONENT( "TempMonitor", ForeignProvider )
            REGISTER_IMPLEMENT_SERVICE( TempMonitorService::ServiceName, TempMonitorService::InterfaceVersion )
        END_REGISTER_COMPONENT( "TempMonitor" )
    END_REGISTER_THREAD( "ForeignProviderThread" )
END_MODEL("ForeignProviderModel")
