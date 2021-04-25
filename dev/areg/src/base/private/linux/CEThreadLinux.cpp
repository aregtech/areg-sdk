/************************************************************************
 * \file        areg/src/base/private/linux/CEThread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread class
 *              Linux specific code
 *
 ************************************************************************/
#include "areg/src/base/CEThread.hpp"

#ifdef  _LINUX

#include "areg/src/base/IEThreadConsumer.hpp"
#include <signal.h>


/************************************************************************/
// Default thread procedure
/************************************************************************/
threadfunc_return_t CEThread::_defaultThreadFunction(void* param)
{
    CEThread*           threadObj = reinterpret_cast<CEThread *>(param);
    threadfunc_return_t result= reinterpret_cast<threadfunc_return_t>( ICEThreadConsumer::EXIT_NO_PARAM );

    if (threadObj != NULL)
    {
        // Linux does not support suspending threads
        // to simulate the CREATE_SUSPENDED behavior of windows, 
        // we wait for a signal that will be set instead of ResumeThread
        sigset_t mask;
        int      sig;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigwait(&mask, &sig);
        ASSERT(sig == SIGUSR1);
        // Linux specific settings
        int      oldVal, err;
        
        ASSERT(threadObj->GetThreadId() == pthread_self());

        err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldVal);
        if (err != 0)
            OUTPUT_ERR("Cannot set cancel state, error code [ %x: %s ].", err, strerror(err));
        err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldVal);
        if (err != 0)
            OUTPUT_ERR("Cannot set cancel type, error code [ %x: %s ].", err, strerror(err));

        // instantiate thread local storage before starting running
        // it should be created in the thread context
        CEThread::_getThreadLocalStorage(threadObj);

        result = reinterpret_cast<threadfunc_return_t>( threadObj->_threadEntry() );

        // delete thread local storage.
        CEThread::_getThreadLocalStorage(static_cast<CEThread *>(NULL));

        threadObj->mWaitForExit.SetEvent();
        CEThread::SwitchThread();
    }

    return result;
}

/**
 * \brief   This function call is a recommendation from MSDN documentation.
 *          It is using undocumented way to set name of thread in native code.
 **/
void CEThread::_setThreadName(ITEM_ID threadId, const char* threadName)
{
    int err = pthread_setname_np(static_cast<pthread_t>(threadId), threadName);
    if (err != 0)
        OUTPUT_ERR("Setting thread name of [ %d ] to [ %s ] failed with error code [ %x: %s ].", threadId, threadName, err, strerror(err));
}

void CEThread::_closeHandle( THREADHANDLE handle )
{
    // handle is just a pointer to the thread, so do nothing here
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void CEThread::Sleep(unsigned int ms)
{
    // sleep takes seconds as an integer, this is not sufficient
    // usleep akes microseconds, but might fail if a wait time > 1s is specified (depending on system)
    // so we use nanosleep:
    struct timespec wait, remaining;
    int             waitRes = EINTR;

    wait.tv_sec   = ms / 1000;
    // 1 ms = 1000 µs = 1000000 ns
    wait.tv_nsec  = (ms - 1000 * wait.tv_sec) * 1000000;
    while (waitRes == EINTR) {
        waitRes = nanosleep(&wait, &remaining);
        switch (waitRes) {
            /* possible values:
                 0         wait done, abort
                 EFAULT    data copy error, abort
                 EINTR     interruption by signal, continue waiting the remaining time
                 EINVAL    data error (tv_nsec >= 10^9 or tv_sec < 0)
                           should not be possible, since ms is unsigned, and due to calculation
                           abort anyway
            */
        case 0:
            // we're done
            break;
        case EINTR:
            // TODO: would memcpy(&wait, &remaining, sizeof(struct timespec)) be faster?
            wait.tv_sec  = remaining.tv_sec;
            wait.tv_nsec = remaining.tv_nsec;
            break;
        default:
            OUTPUT_ERR("Waiting for [ %d ]ms in thread [ %p ] failed with error code [ %x: %s ].", ms, CEThread::GetCurrentThreadId(), waitRes, strerror(waitRes));
        }
    } 
}

ITEM_ID CEThread::GetCurrentThreadId( void )
{
    return static_cast<ITEM_ID>(pthread_self());
}


CEThread::eCompletionStatus CEThread::DestroyThread(unsigned int waitForStopMs /* = CEThread::DO_NOT_WAIT */)
{
    mSynchObject.Lock(ICZSynchObject::WAIT_INFINITE);

    CEThread::eCompletionStatus result = CEThread::ThreadInvalid;

    pthread_t threadId = IsValid() ? GetThreadId() : CEThread::INVALID_THREAD_ID;
    if (threadId != CEThread::INVALID_THREAD_ID)
    {
        UnregisterThread();
        mSynchObject.Unlock();  // unlock, to let thread complete exit task.

        if (waitForStopMs != CEThread::DO_NOT_WAIT && mWaitForExit.Lock(waitForStopMs) == false)
        {
#ifdef  _DEBUG
            //////////////////////////////////////////////////////////////////////////
            // AAvetyan:    In case if thread does not exit properly, 
            //              uncomment these lines to raise assertion
            //              and see the point where the thread is suspended
            //////////////////////////////////////////////////////////////////////////
            // avd: unfortunately, there is no way to suspend a thread in linux without cooperation from that thread
            //      so this debug trick won't work here
            //::SuspendThread(handle);
            //ASSERT(false);  // <== rais assertion
            //::ResumeThread(handle);
            //////////////////////////////////////////////////////////////////////////
            //
            //////////////////////////////////////////////////////////////////////////
#endif  // _DEBUG

            // here we assume that it was requested to wait for thread exit, but it is still running
            // force to terminate thread and close handles due to waiting timeout expire
            // pthread_cancel should cancel the thread almost immediately, but this is not guaranteed
            // se we better wait for cancellation
            result = CEThread::ThreadTerminated;
            int err = pthread_cancel(threadId);
            if (err!= 0) 
            {
                OUTPUT_ERR("Canceling thread [ %d ] failed with error code [ %x: %s ].", threadId, err, strerror(err));
            }
            err = pthread_join(threadId, NULL); // don't care about return value
            if (err != 0)
            {
                OUTPUT_ERR("Waiting for canceled thread [ %d ] failed with error code [ %x: %s ].", threadId, err, strerror(err));
            }
        }
        else
        {
            // The thread completed job normally
            result = CEThread::ThreadCompleted;
            ASSERT (IsRunning() == false);
        }
        mSynchObject.Lock(ICZSynchObject::WAIT_INFINITE);
        CleanResources();
    }
    else
    {
        // The thread is not valid and not running, nothing to destroy
        result = CEThread::ThreadInvalid;
    }
    mSynchObject.Unlock(); // nothing to do, the thread is already destroyed

    return result;
}

bool CEThread::CreateSystemThread( void )
{
    bool result = false;
    if (isValidNoLock() == false && mThreadAddress.GetThreadName().empty() == false)
    {
        mWaitForRun.ResetEvent();
        pthread_t        threadId  = 0;
        int              err;

        err = pthread_create(&threadId, NULL, CEThread::_defaultThreadFunction, static_cast<void *>(this));
        if (err == 0)
        {
            result          = true;
            mThreadHandle   = static_cast<THREADHANDLE>(this);
            mThreadId       = threadId;

            mWaitForExit.ResetEvent( );
            if (RegisterThread() == false)
            {
                result = false;
                UnregisterThread();
                CleanResources();
                mWaitForExit.SetEvent( );
            }
            else
            {
                mThreadPriority = CEThread::PriorityNormal;
            }
        }
    }
    return result;
}

CEThread::eThreadPriority CEThread::SetThreadPriority( const eThreadPriority & newPriority )
{
    static const int InvalidPriority    = static_cast<int>(0x80000000); /*-2147483648*/
    /**
     * if priority of a thread is changed, a realtime scheduling policy must be used, 
     * possible policies are SCHED_FIFO and SCHED_RR. We use SCHED_RR (round robin) here.
     **/
    static const int schedPolicy        = SCHED_RR;
    static const int minPriority        = sched_get_priority_min(schedPolicy), 
                     maxPriority        = sched_get_priority_max(schedPolicy);

    CZLock  lock(mSynchObject);
    CEThread::eThreadPriority oldPrio = mThreadPriority;
    if (isValidNoLock())
    {
        int prio = InvalidPriority;
        switch (newPriority)
        {
        case CEThread::PriorityLowest:
            prio = minPriority;
            break;

        case CEThread::PriorityLow:
            prio = (minPriority + maxPriority) / 4;
            break;

        case CEThread::PriorityNormal:
            prio = (minPriority + maxPriority) / 2;
            break;

        case CEThread::PriorityHigh:
            prio = (minPriority + maxPriority) * 3 / 4;
            break;

        case CEThread::PriorityHighest:
            prio = maxPriority;
            break;

        default:
            break;  // do nothing, invalid priority value
        }
        if (prio != InvalidPriority) {
            struct sched_param sp;
            sp.sched_priority = prio;
            int err = pthread_setschedparam(mThreadId, schedPolicy, &sp);
            if (err == 0)
                mThreadPriority = newPriority;
            else
                OUTPUT_ERR("Cannot set thread priority for thread [ %d ], failed with error code [ %x: %s ].", mThreadId, err, strerror(err));
        }
    }
    else
    {
        oldPrio = CEThread::PriorityUndefined;
    }
    return oldPrio;
}

bool CEThread::ResumeThread( void )
{
    int err = pthread_kill(mThreadId, SIGUSR1);
    if (err != 0)
        OUTPUT_ERR("Sending resume signal to thread [ %d ] failed with error code [ %x: %s ].", mThreadId, err, strerror(err));
    return err == 0;
}

#endif  // _LINUX
