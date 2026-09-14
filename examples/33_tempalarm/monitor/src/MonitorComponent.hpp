#pragma once

/************************************************************************
 * \file        monitor/src/MonitorComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "examples/33_tempalarm/services/TempAlarmProviderBase.hpp"

/**
 * \brief   Walks a fixed reading sequence and raises or clears the alarm
 *          against the thresholds an operator sets.
 **/
class MonitorComponent final : public    areg::Component
                             , protected TempAlarmProviderBase
                             , private   areg::TimerConsumer
{
public:
    MonitorComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner);
    virtual ~MonitorComponent(void) = default;

protected:
/************************************************************************
 * TempAlarmProviderBase overrides
 ************************************************************************/
    void request_set_thresholds(int16_t highLimit, int16_t hysteresis) override;
    void request_start_sequence(void) override;
    void request_get_statistics(void) override;

/************************************************************************
 * areg::TimerConsumer overrides
 ************************************************************************/
    void process_timer(areg::Timer & timer) override;

private:
    //!< Arms the scripted run on the first valid thresholds.
    void start_scripted_run(void);
    //!< Advances to the next reading and re-evaluates the alarm.
    void advance_reading(void);
    //!< Compares the current reading against the thresholds.
    void evaluate_alarm(void);

private:
    areg::Timer mTimer;             //!< Fires every 200 ms to advance the reading.
    uint32_t    mIndex;             //!< Position in the reading sequence.
    int16_t     mReading;           //!< Current reading, in tenths of a degree.
    int16_t     mLowest;            //!< Lowest reading seen since start.
    int16_t     mHighest;           //!< Highest reading seen since start.
    uint32_t    mAlarmsRaised;      //!< How many times the alarm has been raised.
    bool        mAlarmActive;       //!< True while the alarm is raised.
    bool        mThresholdsSet;     //!< True once a valid threshold pair was accepted.
    bool        mRunStarted;        //!< True once the scripted sequence is armed and running.
    int16_t     mHighLimit;         //!< Accepted high limit.
    int16_t     mHysteresis;        //!< Accepted hysteresis.

    inline MonitorComponent & self(void)
    {   return (*this); }
};
