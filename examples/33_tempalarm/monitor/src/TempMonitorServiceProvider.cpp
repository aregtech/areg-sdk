/************************************************************************
 * \file        monitor/src/TempMonitorServiceProvider.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm: the monitor component.
 ************************************************************************/
#include "monitor/src/TempMonitorServiceProvider.hpp"

#include <iostream>

#include "areg/appbase/Application.hpp"

TempMonitorServiceProvider::TempMonitorServiceProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component(entry, owner)
    , TempMonitorServiceProviderBase(static_cast<areg::Component &>(*this))
    , areg::TimerConsumer()
    , mReadingTimer(static_cast<areg::TimerConsumer &>(self()), "ReadingTimer")
{
    set_reading(cSequence[0]);
    set_alarm_active(false);
}

void TempMonitorServiceProvider::startup_component(areg::ComponentThread & comThread)
{
    areg::Component::startup_component(comThread);
}

void TempMonitorServiceProvider::shutdown_component(areg::ComponentThread & comThread)
{
    mReadingTimer.stop_timer();
    areg::Component::shutdown_component(comThread);
}

void TempMonitorServiceProvider::process_timer(areg::Timer & timer)
{
    if (&timer == &mReadingTimer)
    {
        ++mIndex;
        if (mIndex >= cSequenceLen)
        {
            stop_reading_timer();
            return;
        }

        int16_t value = cSequence[mIndex];
        set_reading(value);

        if (value < mLowest) { mLowest = value; }
        if (value > mHighest) { mHighest = value; }

        if (mAccepted)
        {
            if (!mAlarmRaised && (value >= mHighLimit))
            {
                mAlarmRaised = true;
                ++mAlarmsRaised;
                set_alarm_active(true);
                broadcast_alarm_raised(value);
                std::cout << "monitor: alarm raised at " << value << std::endl;
            }
            else if (mAlarmRaised && (value < static_cast<int16_t>(mHighLimit - mHysteresis)))
            {
                mAlarmRaised = false;
                set_alarm_active(false);
                broadcast_alarm_cleared(value);
                std::cout << "monitor: alarm cleared at " << value << std::endl;
            }
        }
    }
}

void TempMonitorServiceProvider::request_set_thresholds( int16_t high_limit, int16_t hysteresis )
{
    bool accepted = true;
    areg::String reason;

    if ((high_limit < 0) || (high_limit > 1000))
    {
        accepted = false;
        reason = "high limit must be between 0 and 1000";
    }
    else if (hysteresis <= 0)
    {
        accepted = false;
        reason = "hysteresis must be greater than 0";
    }
    else if (hysteresis >= high_limit)
    {
        accepted = false;
        reason = "hysteresis must be smaller than the high limit";
    }

    if (accepted)
    {
        mHighLimit = high_limit;
        mHysteresis = hysteresis;
        mAccepted = true;
        mAlarmsRaised = 0;
        std::cout << "monitor: thresholds accepted, high=" << high_limit << " hysteresis=" << hysteresis << std::endl;
        start_reading_timer();
    }
    else
    {
        std::cout << "monitor: thresholds refused: " << reason.as_string() << std::endl;
    }

    response_set_thresholds(accepted, reason);
}

void TempMonitorServiceProvider::request_get_statistics()
{
    std::cout << "monitor: statistics requested" << std::endl;
    response_get_statistics(mLowest, mHighest, mAlarmsRaised);
}
