#pragma once

/************************************************************************
 * \file        monitor/src/TempMonitorServiceProvider.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm: the monitor component.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/base/String.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "examples/33_tempalarm/services/TempMonitorServiceProviderBase.hpp"

class TempMonitorServiceProvider final : public    areg::Component
                                       , protected TempMonitorServiceProviderBase
                                       , private   areg::TimerConsumer
{
public:
    TempMonitorServiceProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

protected:
    void startup_component(areg::ComponentThread & comThread) final;
    void shutdown_component(areg::ComponentThread & comThread) final;
    void process_timer(areg::Timer & timer) final;
    void request_set_thresholds( int16_t high_limit, int16_t hysteresis ) final;
    void request_get_statistics() final;
    //! Starts ReadingTimer again from now: it fires every 200 ms until stopped.
    inline void start_reading_timer()
    {
        mReadingTimer.stop_timer();
        mReadingTimer.start_timer(200, static_cast<areg::DispatcherThread &>(master_thread()), areg::TimerBase::CONTINUOUSLY);
    }

    //! Stops ReadingTimer; safe whether or not it runs.
    inline void stop_reading_timer()
    {
        mReadingTimer.stop_timer();
    }

private:
    static constexpr int16_t cSequence[30]{ 200,210,220,230,240,250,260,270,280,290,
                                             300,310,320,310,300,295,290,292,296,300,
                                             290,270,250,230,210,190,170,150,130,110 };
    static constexpr size_t cSequenceLen{ 30 };
    size_t mIndex{ 0 };
    int16_t mHighLimit{ 0 };
    int16_t mHysteresis{ 0 };
    bool mAccepted{ false };
    bool mAlarmRaised{ false };
    int16_t mLowest{ 200 };
    int16_t mHighest{ 200 };
    uint32_t mAlarmsRaised{ 0 };
    inline TempMonitorServiceProvider & self()
    {   return (*this); }

    areg::Timer mReadingTimer;    //!< Advances the scripted reading sequence, one step per firing.

    TempMonitorServiceProvider() = delete;
    AREG_NOCOPY_NOMOVE(TempMonitorServiceProvider);
};

