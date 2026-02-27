//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates how to create custom event 
//               dispatching and simple threads.As events to dispatch, the 
//               demo uses timers and timer events.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/component/Timer.hpp"
#include "areg/logging/GELog.h"

#ifdef  _MSC_VER
    #pragma comment(lib, "areg")
#endif

//////////////////////////////////////////////////////////////////////////
// HelloThread
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(threads_main_HelloThread_Ctor);
DEF_LOG_SCOPE(threads_main_HelloThread_onThreadRuns);

Mutex gSync(false);

class HelloThread   : public Thread
                    , protected ThreadConsumer
{
public:
    HelloThread()
        : Thread(*this, "HelloThread")
    {
        LOG_SCOPE(threads_main_HelloThread_Ctor);
        LOG_DBG("Initialized thread [HelloThread]");
    }

protected:
/************************************************************************/
// ThreadConsumer interface overrides
/************************************************************************/

    void on_thread_runs() override
    {
        LOG_SCOPE(threads_main_HelloThread_onThreadRuns);
        LOG_INFO("!!!Hello World!!! !!!Hello Tracing!!!");
        LOG_INFO("The thread [%s] runs, sleeping %u ms", name().as_string(), NECommon::WAIT_500_MILLISECONDS);
        do
        {
            Lock lock(gSync);
            std::cout << "The thread [" << name().as_string() << "] runs, sleeping " << NECommon::WAIT_500_MILLISECONDS << " ms" << std::endl;

        } while (false);

        Thread::sleep(NECommon::WAIT_500_MILLISECONDS);
    }
};


//////////////////////////////////////////////////////////////////////////
// HelloDispatcher
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(threads_main_HelloDispatcher_Ctor);
DEF_LOG_SCOPE(threads_main_HelloDispatcher_readyForEvents);
DEF_LOG_SCOPE(threads_main_HelloDispatcher_dispatchEvent);

class HelloDispatcher   : public DispatcherThread
                        , private TimerConsumer
{
public:
    HelloDispatcher() 
        : DispatcherThread("HelloDispatcher", NECommon::DEFAULT_BLOCK_SIZE, NECommon::IGNORE_VALUE )
        , TimerConsumer()
        , mTimer(*this, "aTimer")
    {
        LOG_SCOPE(threads_main_HelloDispatcher_Ctor);
        LOG_DBG("Instantiated hello dispatcher");
    }

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/
    void readyForEvents(bool isReady) override
    {
        LOG_SCOPE(threads_main_HelloDispatcher_readyForEvents);
        DispatcherThread::readyForEvents(isReady);
        if (isReady)
        {
            Lock lock(gSync);
            LOG_DBG("Dispatcher thread is ready for event dispatching");
            std::cout << "Dispatcher thread is ready for event dispatching" << std::endl;
            mTimer.start_timer(100);
        }
        else
        {
            mTimer.stop_timer();
        }
    }

/************************************************************************/
// EventRouter interface overrides
/************************************************************************/
    bool dispatchEvent(Event & eventElem) override
    {
        LOG_SCOPE(threads_main_HelloDispatcher_dispatchEvent);
        LOG_DBG("Received event [%s], custom dispatching here", eventElem.getRuntimeClassName().as_string());

        Lock lock(gSync);
        std::cout << "Received event [" << eventElem.getRuntimeClassName().as_string() << "], custom dispatching here" << std::endl;
        return true; // prevent process_timer()
    }

    virtual bool post_event( Event & eventElem ) override
    {
        return EventDispatcher::post_event( eventElem );
    }

/************************************************************************/
// TimerConsumer interface overrides.
/************************************************************************/
    void process_timer(Timer &) override
    {
        ASSERT(false);  // this never happens, because we interrupt in dispatchEvent()
    }

private:
    Timer mTimer;
};

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(threads_main_main);

int main()
{
    std::cout << "Demo: create and destroy simple + dispatcher threads..." << std::endl;
    LOGGING_CONFIGURE_AND_START(nullptr);
    do
    {
        LOG_SCOPE(threads_main_main);

        Application::start_timer_manager();

        HelloThread helloThread;
        helloThread.create_thread(NECommon::WAIT_INFINITE);

        HelloDispatcher helloDispatcher;
        helloDispatcher.create_thread(NECommon::WAIT_INFINITE);

        Thread::sleep(NECommon::WAIT_1_SECOND);

        LOG_INFO("Stopping dispatcher [%s]", helloDispatcher.name().as_string());
        helloDispatcher.shutdown_thread(NECommon::WAIT_INFINITE);

        LOG_INFO("Stopping thread [%s]", helloThread.name().as_string());
        helloThread.shutdown_thread(NECommon::WAIT_INFINITE);
    } while (false);

    LOGGING_STOP();
    std::cout << "Exit application, check logs for details!" << std::endl;
    return 0;
}
