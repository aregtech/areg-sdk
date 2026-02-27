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
DEF_LOG_SCOPE(timer_main_TimerDispatcher_start_timers);
DEF_LOG_SCOPE(timer_main_TimerDispatcher_stop_timers);
DEF_LOG_SCOPE(timer_main_TimerDispatcher_process_timer);
DEF_LOG_SCOPE(timer_main_start_timer_thread);
DEF_LOG_SCOPE(timer_main_stop_timer_thread);
DEF_LOG_SCOPE(timer_main_main);

//! \brief  An example of a dispatcher thread, which starts and processes timers.
//!         Indifferent in which thread context the timers are started,
//!         they all are processed in the context of binding thread.
//!         The timer should have unique names.
class TimerDispatcher   : public DispatcherThread
                        , private TimerConsumer
{
    static constexpr uint32_t TIMEOUT_ONE_TIME{ NECommon::TIMEOUT_1_MS * 500 }; //!< The timeout in milliseconds of one time timer
    static constexpr uint32_t TIMEOUT_PERIODIC_TIME{ NECommon::TIMEOUT_1_MS * 80 }; //!< The timeout in milliseconds of periodic timer
    static constexpr uint32_t TIMEOUT_CONTINUOUS_TIME{ NECommon::TIMEOUT_1_MS * 50 }; //!< The timeout in milliseconds of continues timer

public:
    explicit TimerDispatcher(const String & threadName)
        : DispatcherThread(threadName, NECommon::DEFAULT_BLOCK_SIZE, NECommon::IGNORE_VALUE )
        , TimerConsumer()
        , mOneTime(*this, threadName + "_one_time")
        , mPeriodic(*this, threadName + "_periodic")
        , mContinuous(*this, threadName + "_continuous")
    {
        LOG_SCOPE(timer_main_TimerDispatcher_TimerDispatcher);
        LOG_DBG("Instantiated timer dispatcher thread [ %s ]", name().as_string());
    }

    void start_timers()
    {
        LOG_SCOPE(timer_main_TimerDispatcher_start_timers);
        auto start = [&](Timer & t, uint32_t timeout, int32_t count)
        {
            if (t.start_timer(timeout, static_cast<DispatcherThread&>(*this), count))
            {
                LOG_DBG("Timer [ %s ] started, timeout [%u]", t.name().as_string(), t.timeout());

            }
            else
            {
                LOG_ERR("Failed to start timer [ %s ]", t.name().as_string());
            }
        };

        start(mOneTime, TIMEOUT_ONE_TIME, 1);
        start(mPeriodic, TIMEOUT_PERIODIC_TIME, (NECommon::TIMEOUT_1_SEC*5 / 2)/TIMEOUT_PERIODIC_TIME);
        start(mContinuous, TIMEOUT_CONTINUOUS_TIME, Timer::CONTINUOUSLY);
    }

    void stop_timers()
    {
        LOG_SCOPE(timer_main_TimerDispatcher_stop_timers);
        LOG_INFO("Stopping timers for thread [ %s ]", name().as_string());
        mOneTime.stop_timer();
        mPeriodic.stop_timer();
        mContinuous.stop_timer();
    }

protected:
    void process_timer(Timer & timer) override
    {
        LOG_SCOPE(timer_main_TimerDispatcher_process_timer);
        LOG_DBG("Timer [ %s ] expired, timeout [%u], events [%d], thread [%s]"
                , timer.name().as_string()
                , timer.timeout()
                , timer.event_count()
                , name().as_string());

        printf("%s : Timer [ %s ] expired.\n", DateTime::now().format_time().as_string(), timer.name().as_string());
    }

    //! Override the default implementation to escape assertion
    virtual bool post_event(Event& eventElem) override
    {
        ASSERT(AREG_RUNTIME_CAST(&eventElem, TimerEvent) != nullptr);    // Make sure that only timer events are passed.
        return EventDispatcher::post_event(eventElem);
    }
private:
    Timer mOneTime;
    Timer mPeriodic;
    Timer mContinuous;
};

//////////////////////////////////////////////////////////////////////////
// Demo helpers
//////////////////////////////////////////////////////////////////////////
namespace
{
    constexpr uint32_t TIMEOUT_APPLICATION = NECommon::TIMEOUT_1_SEC * 5;

    void start_timer_thread(TimerDispatcher & thread)
    {
        LOG_SCOPE(timer_main_start_timer_thread);
        thread.create_thread(NECommon::WAIT_INFINITE);
        LOG_DBG("%s to create thread [ %s ]", thread.is_valid() ? "SUCCEEDED" : "FAILED", thread.name().as_string());
        thread.start_timers();
    }

    void stop_timer_thread(TimerDispatcher & thread)
    {
        LOG_SCOPE(timer_main_stop_timer_thread);
        thread.stop_timers();
        thread.trigger_exit();
        thread.shutdown_thread(NECommon::WAIT_INFINITE);
        LOG_WARN("Thread [ %s ] completed job.", thread.name().as_string());
    }
}

//////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////
int main()
{
    std::cout << "Demo: timers in binding thread context ..." << std::endl;

    Application::start_logging(true);
    do
    {
        LOG_SCOPE(timer_main_main);

        if (!Application::start_timer_manager())
        {
            LOG_ERR("Failed to start timer manager");
        }
        else
        {
            LOG_INFO("Timer manager started successfully");
        }

        TimerDispatcher t1("TimerThread_1");
        TimerDispatcher t2("TimerThread_2");

        start_timer_thread(t1);
        start_timer_thread(t2);

        LOG_INFO("Main thread sleeping to let timers run...");
        Thread::sleep(TIMEOUT_APPLICATION);

        stop_timer_thread(t1);
        stop_timer_thread(t2);

        Application::stop_timer_manager();
    } while (false);
    
    Application::stop_logging();

    std::cout << "Exit application, check logs for details!" << std::endl;
    return 0;
}
