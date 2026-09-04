/************************************************************************
 * \file        monitor/src/MonitorComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm.
 ************************************************************************/
#include "monitor/src/MonitorComponent.hpp"
#include "areg/component/ComponentThread.hpp"
#include <algorithm>

namespace
{
    //!< The reading sequence, in tenths of a degree Celsius. Repeats the last value once done.
    constexpr int16_t   sSequence[] =
    {
        200, 210, 220, 230, 240, 250, 260, 270, 280, 290,
        300, 310, 320, 310, 300, 295, 290, 292, 296, 300,
        290, 270, 250, 230, 210, 190, 170, 150, 130, 110
    };

    constexpr uint32_t  sCount{ static_cast<uint32_t>(sizeof(sSequence) / sizeof(sSequence[0])) };

    constexpr uint32_t  sPeriodMs{ 200 };
}

MonitorComponent::MonitorComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component           ( entry, owner )
    , TempAlarmProviderBase     ( static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer       ( )
    , mTimer                    ( static_cast<areg::TimerConsumer &>(self()), "MonitorTimer" )
    , mIndex                    ( 0 )
    , mReading                  ( sSequence[0] )
    , mLowest                   ( sSequence[0] )
    , mHighest                  ( sSequence[0] )
    , mAlarmsRaised             ( 0 )
    , mAlarmActive              ( false )
    , mThresholdsSet            ( false )
    , mHighLimit                ( 0 )
    , mHysteresis               ( 0 )
{
    set_reading(mReading);
    set_alarm_active(mAlarmActive);
    mTimer.start_timer(sPeriodMs, static_cast<areg::DispatcherThread &>(owner), areg::TimerBase::CONTINUOUSLY);
}

void MonitorComponent::request_set_thresholds(int16_t highLimit, int16_t hysteresis)
{
    bool success{ false };
    areg::String reason;

    if ((highLimit < 0) || (highLimit > 1000))
    {
        reason = "high limit must be between 0 and 1000";
    }
    else if (hysteresis <= 0)
    {
        reason = "hysteresis must be greater than 0";
    }
    else if (hysteresis >= highLimit)
    {
        reason = "hysteresis must be smaller than the high limit";
    }
    else
    {
        mHighLimit      = highLimit;
        mHysteresis     = hysteresis;
        mThresholdsSet  = true;
        success         = true;
    }

    response_set_thresholds(success, reason);
    if (success)
    {
        evaluate_alarm();
    }
}

void MonitorComponent::request_get_statistics(void)
{
    response_get_statistics(mLowest, mHighest, mAlarmsRaised);
}

void MonitorComponent::process_timer(areg::Timer & timer)
{
    if (&timer == &mTimer)
    {
        advance_reading();
    }
}

void MonitorComponent::advance_reading(void)
{
    if (mIndex < (sCount - 1))
    {
        ++mIndex;
    }

    mReading = sSequence[mIndex];
    mLowest  = std::min(mLowest, mReading);
    mHighest = std::max(mHighest, mReading);

    set_reading(mReading);
    evaluate_alarm();
}

void MonitorComponent::evaluate_alarm(void)
{
    if (!mThresholdsSet)
    {
        return;
    }

    if (!mAlarmActive && (mReading >= mHighLimit))
    {
        mAlarmActive = true;
        ++mAlarmsRaised;
        set_alarm_active(true);
        broadcast_alarm_raised(mReading);
    }
    else if (mAlarmActive && (mReading < static_cast<int16_t>(mHighLimit - mHysteresis)))
    {
        mAlarmActive = false;
        set_alarm_active(false);
        broadcast_alarm_cleared(mReading);
    }
}
