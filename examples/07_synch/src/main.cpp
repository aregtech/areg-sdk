//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2022 Aregtech UG.All rights reserved.
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
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"
#include "areg/trace/GETrace.h"

#include <string_view>

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS

class HelloThread;
class GoodbyeThread;

//////////////////////////////////////////////////////////////////////////
// Synchronization objects
//////////////////////////////////////////////////////////////////////////

SynchEvent  gEventExit(true, false);    //!< Non-signaled, manual reset event
SynchEvent  gEventRun(true, true);      //!< Non-signaled, auto-reset event
Mutex       gMutexWait(false);          //!< Not locked mutex
Mutex       gMutexDummy(false);         //!< Not locked mutex

//////////////////////////////////////////////////////////////////////////
// HelloThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A simple thread. Demonstration of how to extend thread base class
 *          and implement thread consumer object.
 */
class HelloThread   : public    Thread
                    , protected IEThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The thread name;
     */
    static constexpr std::string_view THREAD_NAME { "HelloThread" };

public:
    HelloThread( void );

    virtual ~HelloThread( void ) = default;

    SynchEvent  mQuit;  //!< Event, signaled when thread completes job.

protected:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/
    /**
     * \brief   This callback function is called from Thread object, when it is
     *          running and fully operable.
     **/
    virtual void onThreadRuns( void ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline HelloThread & self( void )   {return (*this);}
};

//////////////////////////////////////////////////////////////////////////
// GoodbyeThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A simple thread. That is created by another thread.
 */
class GoodbyeThread : public    Thread
                    , protected IEThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The thread name;
     */
    static constexpr std::string_view THREAD_NAME { "GoodbyeThread" };

public:
    GoodbyeThread( void );
    virtual ~GoodbyeThread( void ) = default;

    SynchEvent  mQuit;  //!< Event, which is signaled when thread completes job.

protected:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/
    /**
     * \brief   This callback function is called from Thread object, when it is
     *          running and fully operable.
     **/
    virtual void onThreadRuns( void ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline GoodbyeThread & self( void ) { return (*this); }
};

//////////////////////////////////////////////////////////////////////////
// Static member initialization
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RunThread implementation
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(main_HelloThread_HelloThread);
DEF_TRACE_SCOPE(main_HelloThread_onThreadRuns);

HelloThread::HelloThread( void )
    : Thread            ( self(), HelloThread::THREAD_NAME.data() )
    , IEThreadConsumer  ( )
    , mQuit             (true, true)
{
    TRACE_SCOPE(main_HelloThread_HelloThread);
    TRACE_DBG("Initialized thread [ %s ]", HelloThread::THREAD_NAME.data());
}

void HelloThread::onThreadRuns( void )
{
    TRACE_SCOPE(main_HelloThread_onThreadRuns);

    TRACE_INFO("The thread [ %s ] runs, going to output message", getName().getString());
    TRACE_INFO("!!!Hello World!!! from thread [ %s ]", THREAD_NAME.data());

    // reset events
    mQuit.resetEvent();
    gMutexDummy.lock( );

    // lock, to wait auto-reset event
    gEventRun.lock(NECommon::WAIT_INFINITE);
    TRACE_INFO("Auto-reset event \'gEventRun\' is signaled, locking again");

    // Setup multiple locking.
    constexpr unsigned int waitTimeout        = NECommon::WAIT_1_MILLISECOND * 150;

    // This multi-lock uses 3 synchronization events and one mutex
    IESynchObject * synchObjects[]  = {&gEventExit, &gMutexWait, &gEventRun};
    int count = MACRO_ARRAYLEN(synchObjects);
    MultiLock multiLock(synchObjects, count, false);

    do 
    {
        // Wait until either all objects are signaled or until timeout expires.
        // If all events are signaled, exit thread.
        // If timeout is expired, make some job and wait again.
        int waitResult = multiLock.lock(waitTimeout, true, false);
        
        if (waitResult == MultiLock::LOCK_INDEX_ALL)
        {
            TRACE_DBG("All waiting objects of thread [ %s ] are signaled, exit the job.", THREAD_NAME.data());
            break;  // exit loop
        }
        else if ( waitResult == MultiLock::LOCK_INDEX_TIMEOUT )
        {
            TRACE_DBG("Thread [ %s ] waiting timeout expired, continuing the job", THREAD_NAME.data());
            Lock lock(gMutexDummy);
            Thread::sleep(waitTimeout);
            TRACE_DBG("Thread [ %s ] continues to wait", THREAD_NAME.data());
        }
        else
        {
            TRACE_ERR("Unexpected waiting result, breaking the loop");
        }

    } while (true);

    Thread::sleep( waitTimeout );

    gMutexDummy.unlock();   // Unlock a mutex
    mQuit.setEvent();       // Signal, to indicate
}

//////////////////////////////////////////////////////////////////////////
// GoodbyeThread implementation
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(main_GoodbyeThread_GoodbyeThread);
DEF_TRACE_SCOPE(main_GoodbyeThread_onThreadRuns);

GoodbyeThread::GoodbyeThread( void )
    : Thread            ( self(), GoodbyeThread::THREAD_NAME.data() )
    , IEThreadConsumer  ( )
    , mQuit             (false, true)
{
    TRACE_SCOPE(main_GoodbyeThread_GoodbyeThread);
    TRACE_DBG("Initialized thread [ %s ]", GoodbyeThread::THREAD_NAME.data());
}

void GoodbyeThread::onThreadRuns( void )
{
    TRACE_SCOPE(main_GoodbyeThread_onThreadRuns);

    TRACE_INFO("The thread [ %s ] runs, going to output message", getName().getString());
    TRACE_INFO("!!!Hello World!!! from thread [ %s ]", THREAD_NAME.data());

    mQuit.resetEvent();

    // Initialize multi-lock object
    // This multi-lock uses 1 synchronization event and 1 mutex.
    IESynchObject * synchObjects[]  = {&gEventExit, &gMutexDummy};
    MultiLock multiLock(synchObjects, MACRO_ARRAYLEN(synchObjects), false);

    int waitResult = multiLock.lock( NECommon::WAIT_INFINITE, false, false );
    TRACE_DBG( "Lock finished with result [ %d ]", waitResult );
    multiLock.unlock( waitResult );
    Thread::sleep( NECommon::WAIT_500_MILLISECONDS );

    mQuit.setEvent();           // set event to inform the thread completed job.
}

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(main_main);
/**
 * \brief   Demo to create and destroy thread.
 */
int main()
{
    printf("Initializing to test synchronization objects...\n");
    // Force to start logging. See outputs log files in appropriate "logs" sub-folder.
    // To change the configuration and use dynamic logging, use macro TRACER_START_LOGGING
    // and specify the logging configuration file, where you can change logging format,
    // filter logging priority and scopes.
    TRACER_CONFIGURE_AND_START( nullptr );

    do
    {
        // set this part of code in a block (for example, 'do-while' block).
        // otherwise, the logs will not be visible, since in the time when
        // scope is initialized, the logging is not active yet.
        TRACE_SCOPE(main_main);

        // initialize synchronization object states
        gMutexWait.lock();
        gEventRun.resetEvent();
        gEventExit.resetEvent();

        // Hello thread.
        TRACE_DBG("Starting Hello Thread");
        HelloThread helloThread;
        helloThread.createThread( NECommon::DO_NOT_WAIT);

        TRACE_INFO( "Sleep thread for [ %d ] ms, to signal \'gEventRun\' auto-reset event.", NECommon::WAIT_500_MILLISECONDS );
        Thread::sleep( NECommon::WAIT_500_MILLISECONDS);
        gEventRun.setEvent();                           // signal auto-reset event to let hello thread to run

        Thread::sleep( NECommon::WAIT_500_MILLISECONDS);   // sleep to check whether auto-reset is working
        gMutexWait.unlock();                            // release mutex to let other thread to take ownership
        Thread::sleep( NECommon::WAIT_1_SECOND);           // sleep for no reason

        // Goodbye thread
        TRACE_DBG("Starting Goodbye Thread");
        GoodbyeThread goodbyeThread;
        goodbyeThread.createThread(NECommon::WAIT_INFINITE);
        TRACE_DBG("Created thread [ %s ], which is in [ %s ] state", goodbyeThread.getName().getString(), goodbyeThread.isRunning() ? "RUNNING" : "NOT RUNNING");

        Thread::sleep( NECommon::WAIT_1_SECOND);           // sleep for no reason

        // Initialize multi-lock object
        // In this multi-lock are used 3 synchronization event object.
        // Wait until all events are signaled.
        IESynchObject * synchObjects[] = {&helloThread.mQuit, &goodbyeThread.mQuit, &gMutexDummy};
        gEventExit.setEvent();
        gEventRun.setEvent();

        MultiLock multiLock(synchObjects, MACRO_ARRAYLEN(synchObjects), true);

        // stop and destroy thread, clean resources. Wait until thread ends.
        TRACE_INFO("The threads completed jobs, wait threads to shutdown to exit application");
        helloThread.destroyThread(NECommon::WAIT_INFINITE);
        goodbyeThread.destroyThread(NECommon::WAIT_INFINITE);

    } while (false);

    // Stop logging.
    TRACER_STOP_LOGGING();

    printf("Completed testing synchronization objects, check the logs...\n");

    return 0;
}
