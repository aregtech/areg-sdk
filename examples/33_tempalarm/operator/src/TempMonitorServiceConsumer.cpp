/************************************************************************
 * \file        operator/src/TempMonitorServiceConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm: the simulated operator component.
 ************************************************************************/
#include "operator/src/TempMonitorServiceConsumer.hpp"

#include <iostream>

#include "areg/appbase/Application.hpp"

TempMonitorServiceConsumer::TempMonitorServiceConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component(entry, owner)
    , TempMonitorServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    , areg::TimerConsumer()
    , mDeadline(static_cast<areg::TimerConsumer &>(self()), "Deadline")
    , mPace(static_cast<areg::TimerConsumer &>(self()), "Pace")
{
}

void TempMonitorServiceConsumer::startup_component(areg::ComponentThread & thread)
{
    areg::Component::startup_component(thread);
    arm_deadline(cConnectSeconds);
}

bool TempMonitorServiceConsumer::service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    bool result{ false };
    if (TempMonitorServiceConsumerBase::service_connected(status, proxy))
    {
        result = true;
        if (areg::is_service_connected(status))
        {
            mDeadline.stop_timer();
            mConnected = true;
            // Subscriptions are made here, and again after every reconnection.
            notify_on_reading_update(true);
            notify_on_alarm_active_update(true);
            notify_on_broadcast_alarm_raised(true);
            notify_on_broadcast_alarm_cleared(true);

            // The stall watchdog ticks from here; the steps begin once.
            mPace.stop_timer();
            mPace.start_timer(1000, static_cast<areg::DispatcherThread &>(master_thread()),
                              areg::TimerBase::CONTINUOUSLY);
            if (mStep == Step::Start)
            {
                begin(Step::RefuseZeroHysteresis);
            }
        }
        else if ((status == areg::ServiceConnectionState::Disconnected) ||
                 (status == areg::ServiceConnectionState::ConnectionLost))
        {
            // The provider went away. The framework reconnects and
            // calls this again; the reconnect deadline is the exit.
            if (is_quitting() == false)
            {
                std::cout << "operator: lost the monitor, waiting to reconnect" << std::endl;
                arm_deadline(cReconnectSeconds);
            }
        }
        else if ((status == areg::ServiceConnectionState::Rejected) ||
                 (status == areg::ServiceConnectionState::Shutdown))
        {
            // Terminal states. The framework does not reconnect
            // from these.
            std::cout << "operator: service refused or shut down" << std::endl;
            mDeadline.stop_timer();
            std::cerr << "service is " << areg::as_string(status)
                      << ", giving up" << std::endl;
            mPace.stop_timer();
            quit_with(1);
        }
    }

    return result;
}

void TempMonitorServiceConsumer::process_timer(areg::Timer & timer)
{
    if (&timer == &mDeadline)
    {
        fail(mConnected ? "the provider did not come back within the reconnect deadline"
                        : "no provider connected within the connect deadline");
        return;
    }

    if ((&timer == &mPace) && (cStallTicks != 0) && (++mIdleTicks >= cStallTicks))
    {
        stalled();
        return;
    }
}

void TempMonitorServiceConsumer::response_set_thresholds( bool accepted, const areg::String & reason )
{
    mHeld = false;
    mJumped = false;
    switch (mStep)
    {
    case Step::RefuseZeroHysteresis:
        {
            StepEnd ending(*this);
            if (accepted)
            {
                fail("expected refusal for hysteresis 0");
                return;
            }
            std::cout << "operator: refused (hysteresis 0): " << reason.as_string() << std::endl;
        }
        break;
    case Step::RefuseWideHysteresis:
        {
            StepEnd ending(*this);
            if (accepted)
            {
                fail("expected refusal for hysteresis wider than the limit");
                return;
            }
            std::cout << "operator: refused (hysteresis 400): " << reason.as_string() << std::endl;
        }
        break;
    case Step::AcceptThresholds:
        {
            StepEnd ending(*this);
            if (!accepted)
            {
                fail("expected thresholds to be accepted");
                return;
            }
            std::cout << "operator: thresholds accepted" << std::endl;
        }
        break;
    default:
        dropped("response set_thresholds");
        break;
    }
}

void TempMonitorServiceConsumer::response_get_statistics( int16_t lowest, int16_t highest, uint32_t alarms_raised )
{
    mHeld = false;
    mJumped = false;
    switch (mStep)
    {
    case Step::RequestStats:
        {
            StepEnd ending(*this);
            if ((lowest != 110) || (highest != 320) || (alarms_raised != 1))
            {
                fail("statistics mismatch");
                return;
            }
            std::cout << "operator: statistics lowest=" << lowest << " highest=" << highest << " alarms_raised=" << alarms_raised << std::endl;
            if ((mRaisedCount != 1) || (mClearedCount != 1))
            {
                fail("expected exactly one raise and one clear");
                return;
            }
            std::cout << "operator: exactly one raise and one clear observed" << std::endl;
        }
        break;
    default:
        dropped("response get_statistics");
        break;
    }
}

void TempMonitorServiceConsumer::request_set_thresholds_failed(areg::ResultType reason)
{
    std::cerr << "request set_thresholds failed, reason " << static_cast<int>(reason) << std::endl;
    mPace.stop_timer();
    quit_with(1);
}

void TempMonitorServiceConsumer::request_get_statistics_failed(areg::ResultType reason)
{
    std::cerr << "request get_statistics failed, reason " << static_cast<int>(reason) << std::endl;
    mPace.stop_timer();
    quit_with(1);
}

void TempMonitorServiceConsumer::broadcast_alarm_raised( int16_t reading )
{
    ++mRaisedCount;
    std::cout << "operator: alarm raised at " << reading << std::endl;
}

void TempMonitorServiceConsumer::broadcast_alarm_cleared( int16_t reading )
{
    ++mClearedCount;
    std::cout << "operator: alarm cleared at " << reading << std::endl;
}

void TempMonitorServiceConsumer::on_reading_update(int16_t Reading, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        std::cout << "operator: reading " << Reading << std::endl;

        mHeld = false;
        mJumped = false;
        switch (mStep)
        {
        case Step::WatchReadings:
            {
                StepEnd ending(*this);
                if (Reading != 110)
                {
                    stay();
                }
            }
            break;
        default:
            dropped("update Reading");
            break;
        }
    }
}

void TempMonitorServiceConsumer::on_alarm_active_update(bool /* AlarmActive */, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        // no action needed; alarm_active() is readable directly by a late subscriber
    }
}

const char * TempMonitorServiceConsumer::step_slot()
{
    switch (mStep)
    {
    case Step::RefuseZeroHysteresis:  return "step_RefuseZeroHysteresis";
    case Step::RefuseWideHysteresis:  return "step_RefuseWideHysteresis";
    case Step::AcceptThresholds:  return "step_AcceptThresholds";
    case Step::WatchReadings:  return "step_WatchReadings";
    case Step::RequestStats:  return "step_RequestStats";
    default:  return "no step";
    }
}

const char * TempMonitorServiceConsumer::step_detail()
{
    switch (mStep)
    {
    case Step::RefuseZeroHysteresis:  return "sent request_set_thresholds(300, 0) and awaits response set_thresholds";
    case Step::RefuseWideHysteresis:  return "sent request_set_thresholds(300, 400) and awaits response set_thresholds";
    case Step::AcceptThresholds:  return "sent request_set_thresholds(300, 30) and awaits response set_thresholds";
    case Step::WatchReadings:  return "awaits update Reading";
    case Step::RequestStats:  return "sent request_get_statistics() and awaits response get_statistics";
    default:  return "waits for nothing";
    }
}

void TempMonitorServiceConsumer::stalled()
{
    fail("the scenario stopped making progress");
    std::cerr << "  " << step_slot() << " " << step_detail()
              << (mRan ? ". Its check ran and kept the step."
                       : ". Nothing arrived.") << std::endl;
    for (uint32_t kept = 0; kept < mDroppedKept; ++ kept)
    {
        std::cerr << "  dropped: " << mDroppedWhat[kept]
                  << " arrived on " << mDroppedStep[kept]
                  << ", which has no check for it."
                  << std::endl;
    }
    if (mDroppedCount > mDroppedKept)
    {
        std::cerr << "  dropped: and "
                  << (mDroppedCount - mDroppedKept)
                  << " more." << std::endl;
    }
}

void TempMonitorServiceConsumer::dropped(const char * what)
{
    if (mDroppedKept < cDroppedMost)
    {
        mDroppedWhat[mDroppedKept] = what;
        mDroppedStep[mDroppedKept] = step_slot();
        ++ mDroppedKept;
    }
    ++ mDroppedCount;
}

void TempMonitorServiceConsumer::fail(const char * why)
{
    std::cerr << "FAIL [" << step_slot() << "]: " << why
              << std::endl;
    mDeadline.stop_timer();
    mPace.stop_timer();
    quit_with(1);
}

void TempMonitorServiceConsumer::arm_deadline(uint32_t seconds)
{
    mDeadline.stop_timer();
    if (seconds != 0)
    {
        mDeadline.start_timer(seconds * 1000,
                              static_cast<areg::DispatcherThread &>(master_thread()),
                              areg::TimerBase::ONE_TIME);
    }
}

void TempMonitorServiceConsumer::begin(Step step)
{
    mStep = step;
    mNext = step;
    mJumped = false;
    mHeld = false;
    mRan = false;
    progressed();
    switch (step)
    {
    case Step::RefuseZeroHysteresis:
        std::cout << "step RefuseZeroHysteresis" << std::endl;
        request_set_thresholds(300, 0);
        break;
    case Step::RefuseWideHysteresis:
        std::cout << "step RefuseWideHysteresis" << std::endl;
        request_set_thresholds(300, 400);
        break;
    case Step::AcceptThresholds:
        std::cout << "step AcceptThresholds" << std::endl;
        request_set_thresholds(300, 30);
        break;
    case Step::WatchReadings:
        std::cout << "step WatchReadings" << std::endl;
        break;
    case Step::RequestStats:
        std::cout << "step RequestStats" << std::endl;
        request_get_statistics();
        break;
    case Step::Done:
        mDeadline.stop_timer();
        mPace.stop_timer();
        quit_with(0);
        break;
    default:
        break;
    }
}

void TempMonitorServiceConsumer::complete()
{
    mEnding = false;
    if (is_quitting() || mHeld)
    {
        mHeld = false;
        return;
    }

    begin(mJumped ? mNext : static_cast<Step>(static_cast<uint32_t>(mStep) + 1));
}
