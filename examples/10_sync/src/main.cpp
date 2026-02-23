//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates the use of synchronization objects,
//               including waiting for multiple synchronization objects such 
//               as mutex and synchronization events, that differ by types 
//               such as automatic or manual reset, or initially locked or 
//               unlocked, etc, and how they may be combined in one waiting 
//               scope. For example, a thread can wait for one, many or a
//               mixture of synchronization objects such as Mutex or 
//               synchronization events.
//
//               The synchronization events are not part of POSIX standard, 
//               but exists in Windows. By business logic they have similarity
//               with POSIX signals of conditional variables. Synchronization
//               events are powerful and useful objects.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/DateTime.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/base/Thread.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"

#include "areg/logging/GELog.h"

#include <chrono>

#ifdef _MSC_VER
#pragma comment(lib, "areg")
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Global synchronization objects
//////////////////////////////////////////////////////////////////////////

static SyncEvent    gEventExit(true, false);    //!< Manual-reset event, initially non-signaled
static SyncEvent    gEventRun(true, true);      //!< Auto-reset event, initially signaled
static Mutex        gMutexWait(false);          //!< Initially unlocked
static Mutex        gMutexDummy(false);         //!< Initially unlocked

//////////////////////////////////////////////////////////////////////////
// HelloThread: simple demo worker
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(sync_main_HelloThread_HelloThread);
DEF_LOG_SCOPE(sync_main_HelloThread_onThreadRuns);

class HelloThread : public Thread, protected ThreadConsumer
{
public:
    HelloThread()
        : Thread(static_cast<ThreadConsumer &>(*this), "HelloThread"), ThreadConsumer(), mQuit(true, true)
    {
        LOG_SCOPE(sync_main_HelloThread_HelloThread);
        LOG_DBG("Initialized thread [ %s ]", getName().getString());
    }

    SyncEvent mQuit; //!< Signaled when the thread completes

protected:
    void onThreadRuns() override
    {
        LOG_SCOPE(sync_main_HelloThread_onThreadRuns);
        LOG_INFO("!!! Hello Thread !!!, The thread [ %s ] started", getName().getString());

        mQuit.resetEvent();

        // Wait for auto-reset event before continuing
        gEventRun.lock(areg::WAIT_INFINITE);
        LOG_INFO("Auto-reset event 'gEventRun' is signaled");

        // Multi-lock with multiple objects
        SyncObject* objects[] = { &gEventExit, &gMutexWait, &gEventRun };
        MultiLock multiLock(objects, std::size(objects), false);

        constexpr uint32_t waitTimeout{ areg::WAIT_1_MILLISECOND * 150 };

        do
        {
            int32_t waitResult = multiLock.lock(waitTimeout, true, false);

            if (waitResult == MultiLock::LOCK_INDEX_ALL)
            {
                LOG_INFO("All objects are signaled, exiting thread [ %s ]", getName().getString());
                std::cout << "All synchronization objects are signaled, exiting thread." << std::endl;
                break;
            }
            else if (waitResult == MultiLock::LOCK_INDEX_TIMEOUT)
            {
                Lock lock(gMutexDummy);
                LOG_DBG("Timeout expired, thread [ %s ] simulating work", getName().getString());
                std::cout << "Wait multi-lock timeout expired, continue the job." << std::endl;
                Thread::sleep(waitTimeout);
            }
            else
            {
                LOG_ERR("Unexpected wait result %d, breaking loop", waitResult);
                std::cerr << "Unexpected waiting result, breaking the loop" << std::endl;
            }
        } while (true);

        mQuit.setEvent();
    }
};

//////////////////////////////////////////////////////////////////////////
// GoodbyeThread: simple demo worker
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(sync_main_GoodbyeThread_GoodbyeThread);
DEF_LOG_SCOPE(sync_main_GoodbyeThread_onThreadRuns);

class GoodbyeThread : public Thread, protected ThreadConsumer
{
public:
    GoodbyeThread()
        : Thread(static_cast<ThreadConsumer &>(*this), "GoodbyeThread"), ThreadConsumer(), mQuit(false, true)
    {
        LOG_SCOPE(sync_main_GoodbyeThread_GoodbyeThread);
        LOG_DBG("Initialized thread [ %s ]", getName().getString());
    }

    SyncEvent mQuit; //!< Signaled when the thread completes

protected:
    void onThreadRuns() override
    {
        LOG_SCOPE(sync_main_GoodbyeThread_onThreadRuns);
        LOG_INFO("!!! Goodbye World !!! Thread [ %s ] started", getName().getString());

        mQuit.resetEvent();

        // Multi-lock with exit event + dummy mutex
        SyncObject* objects[] = { &gEventExit, &gMutexDummy };
        MultiLock multiLock(objects, std::size(objects), false);

        int32_t waitResult = multiLock.lock(areg::WAIT_INFINITE, false, false);
        LOG_DBG("GoodbyeThread finished lock with result [%d]", waitResult);
        std::cout << "Multi-lock is signaled the elem " << waitResult << " is unlocked" << std::endl;

        if (waitResult >= 0)
            multiLock.unlock(waitResult);

        Thread::sleep(areg::WAIT_500_MILLISECONDS);
        mQuit.setEvent();
    }
};

//////////////////////////////////////////////////////////////////////////
// Main: demo entry point
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(sync_main_main);

int main()
{
    std::cout << "Demo: synchronization objects and multi-lock ..." << std::endl;

    LOGGING_CONFIGURE_AND_START(nullptr);

    {
        LOG_SCOPE(sync_main_main);

        gMutexWait.lock();
        gEventRun.resetEvent();
        gEventExit.resetEvent();

        HelloThread helloThread;
        LOG_DBG("Starting Hello Thread");
        helloThread.createThread(areg::DO_NOT_WAIT);

        Thread::sleep(areg::WAIT_500_MILLISECONDS);
        gEventRun.setEvent();   // let HelloThread proceed

        Thread::sleep(areg::WAIT_500_MILLISECONDS);
        gMutexWait.unlock();
        Thread::sleep(areg::WAIT_1_SECOND);

        GoodbyeThread goodbyeThread;
        LOG_DBG("Starting Goodbye Thread");
        goodbyeThread.createThread(areg::WAIT_INFINITE);

        Thread::sleep(areg::WAIT_1_SECOND);

        SyncObject* objects[] = { &helloThread.mQuit, &goodbyeThread.mQuit, &gMutexDummy };
        gEventExit.setEvent();
        gEventRun.setEvent();

        MultiLock multiLock(objects, std::size(objects), true);
        std::cout << "All sync objects unlocked. Completing all threads." << std::endl;

        helloThread.shutdownThread(areg::WAIT_INFINITE);
        goodbyeThread.shutdownThread(areg::WAIT_INFINITE);

        constexpr uint32_t eventTimeout{ 1000 };
        LOG_INFO("Testing event synchronization with timeout [%u] ms", eventTimeout);

        DateTime start{ DateTime::getNow() };
        SyncEvent localEvent(false, false);
        localEvent.lock(eventTimeout);
        DateTime end{ DateTime::getNow() };
        uint64_t duration = end.getTime() - start.getTime();
        LOG_INFO("The event was locked for [%lld] ns", duration);
        std::cout << "The event was locked for " << duration << " ns" << std::endl;
    }

    LOGGING_STOP();

    std::cout << "Exit application, check logs for details." << std::endl;
    return 0;
}
