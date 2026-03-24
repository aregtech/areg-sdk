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

#include "areg/base/areg_global.h"
#include "areg/base/DateTime.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/base/Thread.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"

#include "areg/logging/areg_log.h"

#include <chrono>

#ifdef _MSC_VER
#pragma comment(lib, "areg")
#endif // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Global synchronization objects
//////////////////////////////////////////////////////////////////////////

static areg::SyncEvent    gEventExit(true, false);    //!< Manual-reset event, initially non-signaled
static areg::SyncEvent    gEventRun(true, true);      //!< Auto-reset event, initially signaled
static areg::Mutex        gMutexWait(false);          //!< Initially unlocked
static areg::Mutex        gMutexDummy(false);         //!< Initially unlocked

//////////////////////////////////////////////////////////////////////////
// HelloThread: simple demo worker
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(sync_main_HelloThread, HelloThread);
DEF_LOG_SCOPE(sync_main_HelloThread, on_run);

class HelloThread : public areg::Thread, protected areg::ThreadConsumer
{
public:
    HelloThread()
        : areg::Thread(static_cast<areg::ThreadConsumer &>(*this), "HelloThread"), areg::ThreadConsumer(), mQuit(true, true)
    {
        LOG_SCOPE( sync_main_HelloThread, HelloThread );
        LOG_DBG("Initialized thread [ %s ]", name().as_string());
    }

    areg::SyncEvent mQuit; //!< Signaled when the thread completes

protected:
    void on_run() override
    {
        LOG_SCOPE( sync_main_HelloThread, on_run );
        LOG_INFO("!!! Hello Thread !!!, The thread [ %s ] started", name().as_string());

        mQuit.reset();

        // Wait for auto-reset event before continuing
        gEventRun.lock(areg::WAIT_INFINITE);
        LOG_INFO("Auto-reset event 'gEventRun' is signaled");

        // Multi-lock with multiple objects
        areg::SyncObject* objects[] = { &gEventExit, &gMutexWait, &gEventRun };
        areg::MultiLock multiLock(objects, std::size(objects), false);

        constexpr uint32_t waitTimeout{ areg::WAIT_1_MILLISECOND * 150 };

        do
        {
            int32_t waitResult = multiLock.lock(waitTimeout, true, false);

            if (waitResult == areg::MultiLock::LOCK_INDEX_ALL)
            {
                LOG_INFO("All objects are signaled, exiting thread [ %s ]", name().as_string());
                std::cout << "All synchronization objects are signaled, exiting thread." << std::endl;
                break;
            }
            else if (waitResult == areg::MultiLock::LOCK_INDEX_TIMEOUT)
            {
                areg::Lock lock(gMutexDummy);
                LOG_DBG("Timeout expired, thread [ %s ] simulating work", name().as_string());
                std::cout << "Wait multi-lock timeout expired, continue the job." << std::endl;
                areg::Thread::sleep(waitTimeout);
            }
            else
            {
                LOG_ERR("Unexpected wait result %d, breaking loop", waitResult);
                std::cerr << "Unexpected waiting result, breaking the loop" << std::endl;
            }
        } while (true);

        mQuit.set_signaled();
    }
};

//////////////////////////////////////////////////////////////////////////
// GoodbyeThread: simple demo worker
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(sync_main_GoodbyeThread, GoodbyeThread);
DEF_LOG_SCOPE(sync_main_GoodbyeThread, on_run);

class GoodbyeThread : public areg::Thread, protected areg::ThreadConsumer
{
public:
    GoodbyeThread()
        : areg::Thread(static_cast<areg::ThreadConsumer &>(*this), "GoodbyeThread"), areg::ThreadConsumer(), mQuit(false, true)
    {
        LOG_SCOPE( sync_main_GoodbyeThread, GoodbyeThread );
        LOG_DBG("Initialized thread [ %s ]", name().as_string());
    }

    areg::SyncEvent mQuit; //!< Signaled when the thread completes

protected:
    void on_run() override
    {
        LOG_SCOPE( sync_main_GoodbyeThread, on_run );
        LOG_INFO("!!! Goodbye World !!! Thread [ %s ] started", name().as_string());

        mQuit.reset();

        // Multi-lock with exit event + dummy mutex
        areg::SyncObject* objects[] = { &gEventExit, &gMutexDummy };
        areg::MultiLock multiLock(objects, std::size(objects), false);

        int32_t waitResult = multiLock.lock(areg::WAIT_INFINITE, false, false);
        LOG_DBG("GoodbyeThread finished lock with result [%d]", waitResult);
        std::cout << "Multi-lock is signaled the elem " << waitResult << " is unlocked" << std::endl;

        if (waitResult >= 0)
            multiLock.unlock(waitResult);

        areg::Thread::sleep(areg::WAIT_500_MILLISECONDS);
        mQuit.set_signaled();
    }
};

//////////////////////////////////////////////////////////////////////////
// Main: demo entry point
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(sync_main, main);

int main()
{
    std::cout << "Demo: synchronization objects and multi-lock ..." << std::endl;

    LOGGING_CONFIGURE_AND_START(nullptr, false);

    {
        LOG_SCOPE( sync_main, main );

        gMutexWait.lock();
        gEventRun.reset();
        gEventExit.reset();

        HelloThread helloThread;
        LOG_DBG("Starting Hello Thread");
        helloThread.start(areg::DO_NOT_WAIT);

        areg::Thread::sleep(areg::WAIT_500_MILLISECONDS);
        gEventRun.set_signaled();   // let HelloThread proceed

        areg::Thread::sleep(areg::WAIT_500_MILLISECONDS);
        gMutexWait.unlock();
        areg::Thread::sleep(areg::WAIT_1_SECOND);

        GoodbyeThread goodbyeThread;
        LOG_DBG("Starting Goodbye Thread");
        goodbyeThread.start(areg::WAIT_INFINITE);

        areg::Thread::sleep(areg::WAIT_1_SECOND);

        areg::SyncObject* objects[] = { &helloThread.mQuit, &goodbyeThread.mQuit, &gMutexDummy };
        gEventExit.set_signaled();
        gEventRun.set_signaled();

        areg::MultiLock multiLock(objects, std::size(objects), true);
        std::cout << "All sync objects unlocked. Completing all threads." << std::endl;

        helloThread.shutdown(areg::WAIT_INFINITE);
        goodbyeThread.shutdown(areg::WAIT_INFINITE);

        constexpr uint32_t eventTimeout{ 1000 };
        LOG_INFO("Testing event synchronization with timeout [%u] ms", eventTimeout);

        areg::DateTime start{ areg::DateTime::now() };
        areg::SyncEvent localEvent(false, false);
        localEvent.lock(eventTimeout);
        areg::DateTime end{ areg::DateTime::now() };
        uint64_t duration = end.time() - start.time();
        LOG_INFO("The event was locked for [%lld] ns", duration);
        std::cout << "The event was locked for " << duration << " ns" << std::endl;
    }

    LOGGING_STOP();

    std::cout << "Exit application, check logs for details." << std::endl;
    return 0;
}
