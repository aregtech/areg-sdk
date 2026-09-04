#pragma once

/************************************************************************
 * \file        operator/src/OperatorComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm: the simulated operator.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "examples/33_tempalarm/services/TempAlarmConsumerBase.hpp"

/**
 * \brief   Runs the fixed scenario against the monitor and sets the process
 *          exit code to 0 only when every expectation held.
 **/
class OperatorComponent final : public    areg::Component
                              , protected TempAlarmConsumerBase
                              , private   areg::TimerConsumer
{
public:
    OperatorComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner);
    virtual ~OperatorComponent(void) = default;

    //!< The exit code of the scenario: 0 when every step passed.
    static int get_exit_code(void);

protected:
/************************************************************************
 * TempAlarmConsumerBase overrides
 ************************************************************************/
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override;
    void response_set_thresholds(bool success, const areg::String & reason) override;
    void request_set_thresholds_failed(areg::ResultType reason) override;
    void response_get_statistics(int16_t lowest, int16_t highest, uint32_t alarmsRaised) override;
    void request_get_statistics_failed(areg::ResultType reason) override;
    void broadcast_alarm_raised(int16_t reading) override;
    void broadcast_alarm_cleared(int16_t reading) override;

/************************************************************************
 * areg::TimerConsumer overrides
 ************************************************************************/
    void process_timer(areg::Timer & timer) override;

private:
    //!< The scenario step currently waiting for its answer.
    enum class Step
    {
        RefuseZero,     //!< step 1: hysteresis 0 must be refused
        RefuseWide,     //!< step 2: hysteresis wider than the limit must be refused
        Accept,         //!< step 3: a valid pair must be accepted
        Readings,       //!< step 4: following the readings, waiting out the sequence
        Statistics,      //!< step 5 and 6: statistics and the raise/clear count
    };

    //!< Marks the scenario failed at the given step and quits.
    void fail(int step, const areg::String & why);
    //!< Marks the scenario passed and quits.
    void finish(void);

private:
    static int  sExitCode;

    areg::Timer mStatsTimer;   //!< Fires once the reading sequence has had time to finish.
    Step        mStep;
    uint32_t    mAlarmsRaised; //!< Broadcasts received in this run.
    uint32_t    mAlarmsCleared;//!< Broadcasts received in this run.

    inline OperatorComponent & self(void)
    {   return (*this); }
};
