/**
 * \file    worker_provider.cpp
 * \brief   Service provider of the worker proxy test, in its own model. Answers every request;
 *          a threshold request also updates the attribute and sends a broadcast.
 */

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/33_tempalarm/services/TempMonitorServiceProviderBase.hpp"

//!< Service provider of the worker proxy test.
class WorkerTestProvider final : public    areg::Component
                               , protected TempMonitorServiceProviderBase
{
public:
    WorkerTestProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component                   ( entry, owner )
        , TempMonitorServiceProviderBase    ( static_cast<areg::Component &>(*this) )
    {
    }

    void startup_service_interface(areg::Component & holder) override
    {
        TempMonitorServiceProviderBase::startup_service_interface(holder);
        set_reading(0);
        set_alarm_active(false);
    }

    void request_set_thresholds(int16_t high_limit, int16_t /*hysteresis*/) override
    {
        response_set_thresholds(true, areg::String("ok"));
        set_reading(high_limit);
        broadcast_alarm_raised(high_limit);
    }

    void request_get_statistics() override
    {
        response_get_statistics(1, 2, 3);
    }
};

BEGIN_MODEL("WorkerProviderModel")
    BEGIN_REGISTER_THREAD( "WorkerProviderThread" )
        BEGIN_REGISTER_COMPONENT( "TempMonitor", WorkerTestProvider )
            REGISTER_IMPLEMENT_SERVICE( TempMonitorService::ServiceName, TempMonitorService::InterfaceVersion )
        END_REGISTER_COMPONENT( "TempMonitor" )
    END_REGISTER_THREAD( "WorkerProviderThread" )
END_MODEL("WorkerProviderModel")
