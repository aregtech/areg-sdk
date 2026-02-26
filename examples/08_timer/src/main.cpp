//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates use of timers. The demo initializes,
//               starts and stops multiple timers processed in different 
//               threads. The timers require the start of Timer Manager (timer 
//               service). The timers are automatically dispatched in the timer
//               owner thread.
//
//               Following timers are used:
//                  - one time timer  ( it runs only once )
//                  - periodic timer  ( it runs certain amount of times )
//                  - continues timer ( it runs continuously until stopped ).
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/component/Timer.hpp"
#include "areg/logging/GELog.h"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
#endif

DEF_LOG_SCOPE(timer_main_TimerDispatcher_TimerDispatcher);
DEF_LOG_SCOPE(timer_main_TimerDispatcher_startTimers);
DEF_LOG_SCOPE(timer_main_TimerDispatcher_stopTimers);
DEF_LOG_SCOPE(timer_main_TimerDispatcher_processTimer);
DEF_LOG_SCOPE(timer_main_startTimerThread);
DEF_LOG_SCOPE(timer_main_stopTimerThread);
DEF_LOG_SCOPE(timer_main_main);

//! \brief  An example of a dispatcher thread, which starts and processes timers.
//!         Indifferent in which thread context the timers are started,
//!         they all are processed in the context of binding thread.
//!         The timer should have unique names.
class TimerDispatcher   : public areg::DispatcherThread
                        , private areg::TimerConsumer
{
    static constexpr uint32_t TIMEOUT_ONE_TIME{ areg::TIMEOUT_1_MS * 500 }; //!< The timeout in milliseconds of one time timer
    static constexpr uint32_t TIMEOUT_PERIODIC_TIME{ areg::TIMEOUT_1_MS * 80 }; //!< The timeout in milliseconds of periodic timer
    static constexpr uint32_t TIMEOUT_CONTINUOUS_TIME{ areg::TIMEOUT_1_MS * 50 }; //!< The timeout in milliseconds of continues timer

public:
    explicit TimerDispatcher(const areg::String & name)
        : areg::DispatcherThread( name, areg::DEFAULT_BLOCK_SIZE, areg::IGNORE_VALUE )
        , areg::TimerConsumer()
        , mOneTime(*this, name + "_one_time")
        , mPeriodic(*this, name + "_periodic")
        , mContinuous(*this, name + "_continuous")
    {
        LOG_SCOPE(timer_main_TimerDispatcher_TimerDispatcher);
        LOG_DBG("Instantiated timer dispatcher thread [ %s ]", getName().getString());
    }

    void startTimers()
    {
        LOG_SCOPE(timer_main_TimerDispatcher_startTimers);
        auto start = [&](areg::Timer & t, uint32_t timeout, int32_t count)
        {
            if (t.startTimer(timeout, static_cast<areg::DispatcherThread&>(*this), count))
            {
                LOG_DBG("Timer [ %s ] started, timeout [%u]", t.getName().getString(), t.getTimeout());

            }
            else
            {
                LOG_ERR("Failed to start timer [ %s ]", t.getName().getString());
            }
        };

        start(mOneTime, TIMEOUT_ONE_TIME, 1);
        start(mPeriodic, TIMEOUT_PERIODIC_TIME, (areg::TIMEOUT_1_SEC*5 / 2)/TIMEOUT_PERIODIC_TIME);
        start(mContinuous, TIMEOUT_CONTINUOUS_TIME, areg::Timer::CONTINUOUSLY);
    }

    void stopTimers()
    {
        LOG_SCOPE(timer_main_TimerDispatcher_stopTimers);
        LOG_INFO("Stopping timers for thread [ %s ]", getName().getString());
        mOneTime.stopTimer();
        mPeriodic.stopTimer();
        mContinuous.stopTimer();
    }

protected:
    void processTimer(areg::Timer & timer) override
    {
        LOG_SCOPE(timer_main_TimerDispatcher_processTimer);
        LOG_DBG("Timer [ %s ] expired, timeout [%u], events [%d], thread [%s]"
                , timer.getName().getString()
                , timer.getTimeout()
                , timer.getEventCount()
                , getName().getString());

        printf("%s : Timer [ %s ] expired.\n", areg::DateTime::getNow().formatTime().getString(), timer.getName().getString());
    }

    //! Override the default implementation to escape assertion
    virtual bool postEvent(areg::Event& eventElem) override
    {
        ASSERT(AREG_RUNTIME_CAST(&eventElem, TimerEvent) != nullptr);    // Make sure that only timer events are passed.
        return areg::EventDispatcher::postEvent(eventElem);
    }
private:
    areg::Timer mOneTime;
    areg::Timer mPeriodic;
    areg::Timer mContinuous;
};

//////////////////////////////////////////////////////////////////////////
// Demo helpers
//////////////////////////////////////////////////////////////////////////
namespace
{
    constexpr uint32_t TIMEOUT_APPLICATION = areg::TIMEOUT_1_SEC * 5;

    void startTimerThread(TimerDispatcher & thread)
    {
        LOG_SCOPE(timer_main_startTimerThread);
        thread.createThread(areg::WAIT_INFINITE);
        LOG_DBG("%s to create thread [ %s ]", thread.isValid() ? "SUCCEEDED" : "FAILED", thread.getName().getString());
        thread.startTimers();
    }

    void stopTimerThread(TimerDispatcher & thread)
    {
        LOG_SCOPE(timer_main_stopTimerThread);
        thread.stopTimers();
        thread.triggerExit();
        thread.shutdownThread(areg::WAIT_INFINITE);
        LOG_WARN("Thread [ %s ] completed job.", thread.getName().getString());
    }
}

//////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////
int main()
{
    std::cout << "Demo: timers in binding thread context ..." << std::endl;

    areg::Application::startLogging(true);
    do
    {
        LOG_SCOPE(timer_main_main);

        if (!areg::Application::startTimerManager())
        {
            LOG_ERR("Failed to start timer manager");
        }
        else
        {
            LOG_INFO("Timer manager started successfully");
        }

        TimerDispatcher t1("TimerThread_1");
        TimerDispatcher t2("TimerThread_2");

        startTimerThread(t1);
        startTimerThread(t2);

        LOG_INFO("Main thread sleeping to let timers run...");
        areg::Thread::sleep(TIMEOUT_APPLICATION);

        stopTimerThread(t1);
        stopTimerThread(t2);

        areg::Application::stopTimerManager();
    } while (false);
    
    areg::Application::stopLogging();

    std::cout << "Exit application, check logs for details!" << std::endl;
    return 0;
}
