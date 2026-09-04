/************************************************************************
 * \file        operator/src/OperatorComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm: the simulated operator.
 ************************************************************************/
#include "operator/src/OperatorComponent.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/appbase/Application.hpp"
#include <iostream>

namespace
{
    //!< Gives the 30 readings, 200 ms apart, time to finish before statistics are asked for.
    constexpr uint32_t sStatsDelayMs{ 8000 };
}

int OperatorComponent::sExitCode{ 1 };

OperatorComponent::OperatorComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component           ( entry, owner )
    , TempAlarmConsumerBase     ( entry.mDependencyServices[0].mRoleName, owner )
    , areg::TimerConsumer       ( )
    , mStatsTimer               ( static_cast<areg::TimerConsumer &>(self()), "StatsTimer" )
    , mStep                     ( Step::RefuseZero )
    , mAlarmsRaised             ( 0 )
    , mAlarmsCleared            ( 0 )
{
}

int OperatorComponent::get_exit_code(void)
{
    return sExitCode;
}

bool OperatorComponent::service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    bool result{ false };
    if (TempAlarmConsumerBase::service_connected(status, proxy))
    {
        result = true;
        if (areg::is_service_connected(status))
        {
            notify_on_broadcast_alarm_raised(true);
            notify_on_broadcast_alarm_cleared(true);

            std::cout << "Step 1: set high limit 300, hysteresis 0, expect a refusal ..." << std::endl;
            mStep = Step::RefuseZero;
            request_set_thresholds(300, 0);
        }
    }

    return result;
}

void OperatorComponent::request_set_thresholds_failed([[maybe_unused]] areg::ResultType reason)
{
    if (is_connected())
    {
        switch (mStep)
        {
        case Step::RefuseZero:     request_set_thresholds(300, 0);    break;
        case Step::RefuseWide:     request_set_thresholds(300, 400);  break;
        case Step::Accept:         request_set_thresholds(300, 30);   break;
        default:                                                      break;
        }
    }
}

void OperatorComponent::request_get_statistics_failed([[maybe_unused]] areg::ResultType reason)
{
    if (is_connected() && (mStep == Step::Statistics))
    {
        request_get_statistics();
    }
}

void OperatorComponent::response_set_thresholds(bool success, const areg::String & reason)
{
    switch (mStep)
    {
    case Step::RefuseZero:
        if (success)
        {
            fail(1, "a hysteresis of 0 was accepted instead of refused");
            return;
        }

        std::cout << "Step 1: refused as expected, reason: " << reason << std::endl;
        std::cout << "Step 2: set high limit 300, hysteresis 400, expect a refusal ..." << std::endl;
        mStep = Step::RefuseWide;
        request_set_thresholds(300, 400);
        break;

    case Step::RefuseWide:
        if (success)
        {
            fail(2, "a hysteresis wider than the limit was accepted instead of refused");
            return;
        }

        std::cout << "Step 2: refused as expected, reason: " << reason << std::endl;
        std::cout << "Step 3: set high limit 300, hysteresis 30, expect acceptance ..." << std::endl;
        mStep = Step::Accept;
        request_set_thresholds(300, 30);
        break;

    case Step::Accept:
        if (!success)
        {
            fail(3, areg::String("").format("high limit 300, hysteresis 30 was refused: %s", reason.as_string()));
            return;
        }

        std::cout << "Step 3: accepted." << std::endl;
        std::cout << "Step 4: following the readings ..." << std::endl;
        mStep = Step::Readings;
        mStatsTimer.start_timer(sStatsDelayMs, static_cast<areg::DispatcherThread &>(master_thread()), areg::TimerBase::ONE_TIME);
        break;

    default:
        break;      // a stray answer to an earlier retry: ignore it.
    }
}

void OperatorComponent::broadcast_alarm_raised(int16_t reading)
{
    ++mAlarmsRaised;
    std::cout << "  alarm RAISED at reading " << reading << std::endl;
}

void OperatorComponent::broadcast_alarm_cleared(int16_t reading)
{
    ++mAlarmsCleared;
    std::cout << "  alarm CLEARED at reading " << reading << std::endl;
}

void OperatorComponent::process_timer(areg::Timer & timer)
{
    if ((&timer == &mStatsTimer) && (mStep == Step::Readings))
    {
        std::cout << "Step 5: sequence should have finished, asking for statistics ..." << std::endl;
        mStep = Step::Statistics;
        request_get_statistics();
    }
}

void OperatorComponent::response_get_statistics(int16_t lowest, int16_t highest, uint32_t alarmsRaised)
{
    if (mStep != Step::Statistics)
    {
        return;     // a stray answer: ignore it.
    }

    std::cout << "  lowest = " << lowest << ", highest = " << highest << ", alarms raised = " << alarmsRaised << std::endl;

    if (lowest != 110)
    {
        fail(5, areg::String("").format("expected lowest 110, got %d", static_cast<int>(lowest)));
        return;
    }

    if (highest != 320)
    {
        fail(5, areg::String("").format("expected highest 320, got %d", static_cast<int>(highest)));
        return;
    }

    if (alarmsRaised != 1)
    {
        fail(5, areg::String("").format("expected 1 alarm raised, got %u", alarmsRaised));
        return;
    }

    std::cout << "Step 6: checking the raise and clear count received ..." << std::endl;
    if ((mAlarmsRaised != 1) || (mAlarmsCleared != 1))
    {
        fail(6, areg::String("").format("expected 1 raise and 1 clear, got %u raise(s) and %u clear(s)", mAlarmsRaised, mAlarmsCleared));
        return;
    }

    std::cout << "All steps passed." << std::endl;
    finish();
}

void OperatorComponent::fail(int step, const areg::String & why)
{
    std::cout << "FAILED at step " << step << ": " << why << std::endl;
    sExitCode = 1;
    areg::Application::signal_quit();
}

void OperatorComponent::finish(void)
{
    sExitCode = 0;
    areg::Application::signal_quit();
}
