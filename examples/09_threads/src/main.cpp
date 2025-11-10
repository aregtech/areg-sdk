//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project demonstrates how to create custom event 
//               dispatching and simple threads.As events to dispatch, the 
//               demo uses timers and timer events.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/IETimerConsumer.hpp"
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
                    , protected IEThreadConsumer
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
// IEThreadConsumer interface overrides
/************************************************************************/

    void onThreadRuns() override
    {
        LOG_SCOPE(threads_main_HelloThread_onThreadRuns);
        LOG_INFO("!!!Hello World!!! !!!Hello Tracing!!!");
        LOG_INFO("The thread [%s] runs, sleeping %u ms", getName().getString(), NECommon::WAIT_500_MILLISECONDS);
        do
        {
            Lock lock(gSync);
            std::cout << "The thread [" << getName().getString() << "] runs, sleeping " << NECommon::WAIT_500_MILLISECONDS << " ms" << std::endl;

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
                        , private IETimerConsumer
{
public:
    HelloDispatcher() 
        : DispatcherThread("HelloDispatcher", NECommon::DEFAULT_BLOCK_SIZE, NECommon::IGNORE_VALUE )
        , IETimerConsumer()
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
            mTimer.startTimer(100);
        }
        else
        {
            mTimer.stopTimer();
        }
    }

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/
    bool dispatchEvent(Event & eventElem) override
    {
        LOG_SCOPE(threads_main_HelloDispatcher_dispatchEvent);
        LOG_DBG("Received event [%s], custom dispatching here", eventElem.getRuntimeClassName().getString());

        Lock lock(gSync);
        std::cout << "Received event [" << eventElem.getRuntimeClassName().getString() << "], custom dispatching here" << std::endl;
        return true; // prevent processTimer()
    }

    virtual bool postEvent( Event & eventElem ) override
    {
        return EventDispatcher::postEvent( eventElem );
    }

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/
    void processTimer(Timer &) override
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

        Application::startTimerManager();

        HelloThread helloThread;
        helloThread.createThread(NECommon::WAIT_INFINITE);

        HelloDispatcher helloDispatcher;
        helloDispatcher.createThread(NECommon::WAIT_INFINITE);

        Thread::sleep(NECommon::WAIT_1_SECOND);

        LOG_INFO("Stopping dispatcher [%s]", helloDispatcher.getName().getString());
        helloDispatcher.shutdownThread(NECommon::WAIT_INFINITE);

        LOG_INFO("Stopping thread [%s]", helloThread.getName().getString());
        helloThread.shutdownThread(NECommon::WAIT_INFINITE);
    } while (false);

    LOGGING_STOP();
    std::cout << "Exit application, check logs for details!" << std::endl;
    return 0;
}
