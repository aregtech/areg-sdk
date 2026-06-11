//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates how to create custom event 
//               dispatching and simple threads.As events to dispatch, the 
//               demo uses timers and timer events.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/component/Timer.hpp"
#include "areg/logging/areg_log.h"

#ifdef  _MSC_VER
    #pragma comment(lib, "areg")
#endif

//////////////////////////////////////////////////////////////////////////
// HelloThread
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(threads_main_HelloThread, HelloThread);
DEF_LOG_SCOPE(threads_main_HelloThread, on_run);

namespace {
    areg::Mutex gSync(false);
}

class HelloThread final : public areg::Thread
                        , protected areg::ThreadConsumer
{
public:
    HelloThread()
        : areg::Thread(*this, "HelloThread")
    {
        LOG_SCOPE( threads_main_HelloThread, HelloThread);
        LOG_DBG("Initialized thread [HelloThread]");
    }

protected:
/************************************************************************/
// ThreadConsumer interface overrides
/************************************************************************/

    void on_run() final
    {
        LOG_SCOPE( threads_main_HelloThread, on_run);
        LOG_INFO("!!!Hello World!!! !!!Hello Tracing!!!");
        LOG_INFO("The thread [%s] runs, sleeping %u ms", name().as_string(), areg::WAIT_500_MILLISECONDS);
        do
        {
            areg::Lock lock(gSync);
            std::cout << "The thread [" << name().as_string() << "] runs, sleeping " << areg::WAIT_500_MILLISECONDS << " ms" << std::endl;

        } while (false);

        areg::Thread::sleep(areg::WAIT_500_MILLISECONDS);
    }
};


//////////////////////////////////////////////////////////////////////////
// HelloDispatcher
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(threads_main_HelloDispatcher, HelloDispatcher);
DEF_LOG_SCOPE(threads_main_HelloDispatcher, ready_for_events);
DEF_LOG_SCOPE(threads_main_HelloDispatcher, dispatch_event);

class HelloDispatcher final : public areg::DispatcherThread
                            , private areg::TimerConsumer
{
public:
    HelloDispatcher() 
        : areg::DispatcherThread("HelloDispatcher", areg::DEFAULT_BLOCK_SIZE, areg::IGNORE_VALUE )
        , areg::TimerConsumer()
        , mTimer(*this, "aTimer")
    {
        LOG_SCOPE( threads_main_HelloDispatcher, HelloDispatcher);
        LOG_DBG("Instantiated hello dispatcher");
    }

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/
    void ready_for_events(bool isReady) final
    {
        LOG_SCOPE( threads_main_HelloDispatcher, ready_for_events );
        areg::DispatcherThread::ready_for_events(isReady);
        if (isReady)
        {
            areg::Lock lock(gSync);
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
    bool dispatch_event(areg::Event & eventElem) final
    {
        LOG_SCOPE( threads_main_HelloDispatcher, dispatch_event );
        LOG_DBG("Received event [%u], custom dispatching here", eventElem.event_id());

        areg::Lock lock(gSync);
        std::cout << "Received event [" << eventElem.event_id() << "], custom dispatching here" << std::endl;
        return true; // prevent process_timer()
    }

    [[nodiscard]]
    bool post_event( areg::Event & eventElem ) final
    {
        return areg::EventDispatcher::post_event( eventElem );
    }

/************************************************************************/
// TimerConsumer interface overrides.
/************************************************************************/
    void process_timer(areg::Timer &) final
    {
        ASSERT(false);  // this never happens, because we interrupt in dispatch_event()
    }

private:
    areg::Timer mTimer;
};

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(threads_main, main);

int main()
{
    std::cout << "Demo: create and destroy simple + dispatcher threads..." << std::endl;
    LOGGING_CONFIGURE_AND_START(nullptr, false);
    do
    {
        LOG_SCOPE( threads_main, main );

        areg::Application::start_timer_manager();

        HelloThread helloThread;
        helloThread.start(areg::WAIT_INFINITE);

        HelloDispatcher helloDispatcher;
        helloDispatcher.start(areg::WAIT_INFINITE);

        areg::Thread::sleep(areg::WAIT_1_SECOND);

        LOG_INFO("Stopping dispatcher [%s]", helloDispatcher.name().as_string());
        helloDispatcher.shutdown(areg::WAIT_INFINITE);

        LOG_INFO("Stopping thread [%s]", helloThread.name().as_string());
        helloThread.shutdown(areg::WAIT_INFINITE);
    } while (false);

    LOGGING_STOP();
    std::cout << "Exit application, check logs for details!" << std::endl;
    return 0;
}
