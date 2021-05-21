//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"
#include "areg/trace/GETrace.h"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS

class HelloThread;
class RunThread;
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
    static const char * THREAD_NAME /* = "HelloThread" */;

public:
    HelloThread( void );

public:
    SynchEvent  mQuit;  //!< Event, signaled when thread completes job.

protected:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/
    /**
     * \brief   This callback function is called from Thread object, when it is
     *          running and fully operable.
     **/
    virtual void onThreadRuns( void );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline HelloThread & self( void )   {return (*this);}
};

//////////////////////////////////////////////////////////////////////////
// RunThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A simple thread. That is created by another thread.
 */
class RunThread     : public    Thread
                    , protected IEThreadConsumer
{
public:
    RunThread( void );

public:
    SynchEvent  mQuit;  //!< Event, signales when thread complets job.
    /**
     * \brief   The thread name;
     */
    static const char * THREAD_NAME /* = "RunThread" */;


protected:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/
    /**
     * \brief   This callback function is called from Thread object, when it is
     *          running and fully operable.
     **/
    virtual void onThreadRuns( void );

    /**
     * \brief   Logs "foo". Used to demonstrate recursive lock of mutex in the same thread context.
     **/
    void foo(Mutex &mutex) const;

    /**
     * \brief   Logs "bar". Used to demonstrate recursive lock of mutex in the same thread context.
     **/
    void bar(Mutex &mutex) const;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline RunThread & self( void )   {return (*this);}
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
    static const char * THREAD_NAME /* = "GoodbyeThread" */;

public:
    GoodbyeThread( void );

public:
    SynchEvent  mQuit;  //!< Event, which is signaled when thread completes job.

protected:
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/
    /**
     * \brief   This callback function is called from Thread object, when it is
     *          running and fully operable.
     **/
    virtual void onThreadRuns( void );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline GoodbyeThread & self( void ) { return (*this); }
};

//////////////////////////////////////////////////////////////////////////
// Static member initialization
//////////////////////////////////////////////////////////////////////////
const char * HelloThread::THREAD_NAME   = "HelloThread";
const char * RunThread::THREAD_NAME     = "RunThread";
const char * GoodbyeThread::THREAD_NAME = "GoodbyeThread";

//////////////////////////////////////////////////////////////////////////
// RunThread implementation
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(main_HelloThread_HelloThread);
DEF_TRACE_SCOPE(main_HelloThread_onThreadRuns);

HelloThread::HelloThread( void )
    : Thread            ( self(), HelloThread::THREAD_NAME )
    , IEThreadConsumer  ( )
    , mQuit             (true, true)
{
    TRACE_SCOPE(main_HelloThread_HelloThread);
    TRACE_DBG("Initialized thread [ %s ]", HelloThread::THREAD_NAME);
}

void HelloThread::onThreadRuns( void )
{
    TRACE_SCOPE(main_HelloThread_onThreadRuns);

    TRACE_INFO("The thread [ %s ] runs, going to output message", getName().getString());
    TRACE_INFO("!!!Hello World!!! from thread [ %s ]", THREAD_NAME);

    // reset events
    mQuit.resetEvent();
    // lock, to wait auto-reset event
    gEventRun.lock(IESynchObject::WAIT_INFINITE);
    TRACE_INFO("Auto-reset event \'gEventRun\' is signaled, locking again");
    gEventRun.lock(IESynchObject::WAIT_INFINITE);
    TRACE_INFO("Auto-reset event \'gEventRun\' is signaled, can continue loop");


    // initialize RunThread object to start.
    RunThread runThread;
    TRACE_DBG("Creating thread [ %s ]", runThread.getName().getString());
    runThread.createThread(Thread::DO_NOT_WAIT);

    // Setup multiple locking.
    unsigned int waitTimeout        = IESynchObject::WAIT_1_MS * 150;
    // This multi-lock uses 3 synchronization events and one mutex
    IESynchObject * synchObjects[]  = {&gEventExit, &runThread.mQuit, &gMutexWait, &gEventRun};
    int count = MACRO_ARRAYLEN(synchObjects);
    MultiLock multiLock(synchObjects, count, false);

    int waitResult = -1;
    do 
    {
        // Wait until either all objects are signaled or until timeout expires.
        // If all events are signaled, exit thread.
        // If timeout is expired, make some job and wait again.
        waitResult = multiLock.lock(waitTimeout, true, false);
        if (waitResult == count)
        {
            TRACE_DBG("All waiting objects of thread [ %s ] are signaled, exit the job.", THREAD_NAME);
        }
        else if ( waitResult == MultiLock::LOCK_INDEX_TIMEOUT )
        {
            TRACE_DBG("Thread [ %s ] waiting timeout expired, continuing the job", THREAD_NAME);
            Lock lock(gMutexDummy);
            TRACE_DBG("Thread [ %s ] goes to sleep for [ %d ] ms", THREAD_NAME, waitTimeout);
            // gMutexWait.unlock();
            Thread::sleep(waitTimeout);
            TRACE_DBG("Thread [ %s ] continues to wait", THREAD_NAME);
        }
        else
        {
            TRACE_ERR("Unexpected waiting result, breaking the loop");
        }

    } while (waitResult == MultiLock::LOCK_INDEX_TIMEOUT);

    TRACE_INFO("Thread [ %s ] completed to wait, stops running thread and quits", THREAD_NAME);
    runThread.destroyThread(Thread::DO_NOT_WAIT);       // call to destroy thread, but do not wait.
    TRACE_INFO("Wait Thread [ %s ] to complete run.", runThread.getName().getString());
    runThread.mQuit.lock(IESynchObject::WAIT_INFINITE); // wait until thread completes job.
    TRACE_INFO("Thread [ %s ] complete loop, wait to finish thread", runThread.getName().getString());
    runThread.completionWait(Thread::WAIT_INFINITE);    // wait until thread completes job.
    TRACE_WARN("Thread [ %s ] is finished!", runThread.getName().getString());

    gEventRun.setEvent();   // Since RunThread is already completed, signal event
    mQuit.setEvent();       // Signal, to indicate
}

//////////////////////////////////////////////////////////////////////////
// RunThread implementation
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(main_RunThread_RunThread);
DEF_TRACE_SCOPE(main_RunThread_onThreadRuns);
DEF_TRACE_SCOPE(main_RunThread_foo);
DEF_TRACE_SCOPE(main_RunThread_bar);

RunThread::RunThread( void )
    : Thread            ( self(), RunThread::THREAD_NAME )
    , IEThreadConsumer  ( )
    , mQuit             (true, false)
{
    TRACE_SCOPE(main_RunThread_RunThread);
    TRACE_DBG("Initialized thread [ %s ]", RunThread::THREAD_NAME);
}

void RunThread::onThreadRuns( void )
{
    TRACE_SCOPE(main_RunThread_onThreadRuns);
    TRACE_INFO("The thread [ %s ] runs, going to output message", getName().getString());
    TRACE_INFO("!!!Hello World!!! from thread [ %s ]", THREAD_NAME);

    // Set invent, inform Run Thread started.
    gEventRun.setEvent();
    // Initialize multi-lock.
    // This multi-lock has 1 synchronization event and 2 mutexes.
    IESynchObject * synchObjects[] = {&gEventExit, &gMutexWait, &gMutexDummy};
    MultiLock multiLock(synchObjects, MACRO_ARRAYLEN(synchObjects), false);

    unsigned int waitTimeout = 234;
    int result = -1;
    do 
    {
        // Run multi-lock, wait until any of synchronization objects is released.
        // If exit event is signaled, complete job.
        // Otherwise, continue loop.
        result = multiLock.lock(IESynchObject::WAIT_INFINITE, false, false);
        if (result == 0)
        {
            TRACE_DBG("Received signal to exit threads.");
        }
        else if (result == 1)
        {
            TRACE_DBG("Mutex \'gMutexWait\' was released, take ownership and sleep thread for [ %d ] ms", waitTimeout);
            foo(gMutexWait);
            multiLock.unlock(result);
            Thread::sleep(waitTimeout);
        }
        else if (result == 2)
        {
            TRACE_DBG("Mutex \'gMutexDummy\' was released, take ownership and sleep thread for [ %d ] ms", waitTimeout);
            foo(gMutexDummy);
            multiLock.unlock(result);
            Thread::sleep(waitTimeout);
        }
        else
        {
            TRACE_ERR("Unexpected synchronization waiting result [ %d ]", result);
        }
    } while (result != 0);

    // Set invent, inform Run Thread completed.
    gEventRun.setEvent();
    mQuit.setEvent();
}

void RunThread::foo(Mutex &mutex) const
{
    // a method to log 'foo'
    TRACE_SCOPE(main_RunThread_foo);
    Lock lock(mutex);   // lock passed mutex

    TRACE_DBG("foo");
    bar(mutex);         // call bar to lock mutex recursively
}

void RunThread::bar(Mutex &mutex) const
{
    // a method to log 'bar'
    TRACE_SCOPE(main_RunThread_bar);
    Lock lock(mutex);   // lock passed mutex

    TRACE_DBG("bar");
}

//////////////////////////////////////////////////////////////////////////
// GoodbyeThread implementation
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(main_GoodbyeThread_GoodbyeThread);
DEF_TRACE_SCOPE(main_GoodbyeThread_onThreadRuns);

GoodbyeThread::GoodbyeThread( void )
    : Thread            ( self(), GoodbyeThread::THREAD_NAME )
    , IEThreadConsumer  ( )
    , mQuit             (false, true)
{
    TRACE_SCOPE(main_GoodbyeThread_GoodbyeThread);
    TRACE_DBG("Initialized thread [ %s ]", GoodbyeThread::THREAD_NAME);
}

void GoodbyeThread::onThreadRuns( void )
{
    TRACE_SCOPE(main_GoodbyeThread_onThreadRuns);

    TRACE_INFO("The thread [ %s ] runs, going to output message", getName().getString());
    TRACE_INFO("!!!Hello World!!! from thread [ %s ]", THREAD_NAME);

    mQuit.resetEvent();

    // Initialize multi-lock object
    // This multi-lock uses 1 synchronization event and 1 mutex.
    IESynchObject * synchObjects[]  = {&gEventExit, &gMutexDummy};
    MultiLock multiLock(synchObjects, MACRO_ARRAYLEN(synchObjects), false);
    unsigned int waitTimeout = 257;
    int result = -1;

    // Run the loop until do not request to exit thread.
    while ((result = multiLock.lock(IESynchObject::WAIT_INFINITE, false, false)) != 0)
    {
        TRACE_DBG("Dummy mutex ownership is taken (synch [ %d ]), unlock and sleep for [ %d ] ms", result, waitTimeout);
        multiLock.unlock(result);
        Thread::sleep(waitTimeout);
    }

    TRACE_DBG("Received signal to exit thread [ %s ]", THREAD_NAME);

    Thread::sleep(waitTimeout); // sleep for no reason.
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
    // Force to start logging. See outputs log files in appropriate "logs" sub-folder.
    // To change the configuration and use dynamic logging, use macro TRACER_START_LOGGING
    // and specify the logging configuration file, where you can change logging format,
    // filter logging priority and scopes.
    TRACER_FORCE_LOGGING();

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
        helloThread.createThread(Thread::DO_NOT_WAIT);
        TRACE_DBG("Created thread [ %s ], which is in [ %s ] state", helloThread.getName().getString(), helloThread.isRunning() ? "RUNNING" : "NOT RUNNING");

        Thread::sleep(Thread::WAIT_500_MILLISECONDS);
        gEventRun.setEvent();                           // signal auto-reset event to let hello thread to run
        Thread::switchThread();
        TRACE_INFO("Sleep thread for [ %d ] ms, to signal \'gEventRun\' auto-reset event.", Thread::WAIT_500_MILLISECONDS);
        Thread::sleep(Thread::WAIT_500_MILLISECONDS);   // sleep to check whether auto-reset is working
        gEventRun.setEvent();                           // signal auto-reset event to continue hello thread to run
        Thread::switchThread();
        gMutexWait.unlock();                            // release mutex to let other thread to take ownership
        Thread::sleep(Thread::WAIT_1_SECOND);           // sleep for no reason

        // Goodbye thread
        TRACE_DBG("Starting Goodbye Thread");
        GoodbyeThread goodbyeThread;
        goodbyeThread.createThread(Thread::WAIT_INFINITE);
        TRACE_DBG("Created thread [ %s ], which is in [ %s ] state", goodbyeThread.getName().getString(), goodbyeThread.isRunning() ? "RUNNING" : "NOT RUNNING");

        Thread::sleep(Thread::WAIT_1_SECOND);           // sleep for no reason

        // Find Run Thread by name, which is created by other threads.
        Thread * runThread = Thread::findThreadByName(RunThread::THREAD_NAME);
        if (runThread != NULL)
        {
            // make output to make sure the thread is registered.
            TRACE_DBG("Found [ %s ] thread [ %s ], which is in [ %s ] state."
                            , runThread->isValid() ? "VALID" : "INVALID"
                            , runThread->getName().getString()
                            , runThread->isRunning() ? "RUNNING" : "NOT RUNNING");
        }

        Thread::sleep(Thread::WAIT_1_SECOND * 3);   // sleep for no reason.

        // Initialize multi-lock object
        // In this multi-lock are used 3 synchronization event object.
        // Wait until all events are signaled.
        IESynchObject * synchObjects[] = {&helloThread.mQuit, &goodbyeThread.mQuit, &gEventRun};
        gEventExit.setEvent();
        MultiLock multiLock(synchObjects, MACRO_ARRAYLEN(synchObjects), true);

        // stop and destroy thread, clean resources. Wait until thread ends.
        TRACE_INFO("The threads completed jobs, wait threads to shutdown to exit application");
        helloThread.destroyThread(Thread::WAIT_INFINITE);
        goodbyeThread.destroyThread(Thread::WAIT_INFINITE);

    } while (false);

    // Stop logging.
    TRACER_STOP_LOGGING();

    return 0;
}
