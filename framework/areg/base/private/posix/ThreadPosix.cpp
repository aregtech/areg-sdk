/************************************************************************
 * \file        areg/base/private/posix/ThreadWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread class
 *              windows specific code
 *
 ************************************************************************/
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"

#ifdef  _POSIX

#ifndef _POSIX_PRIORITY_SCHEDULING
    #define _POSIX_PRIORITY_SCHEDULING
#endif  // _POSIX_PRIORITY_SCHEDULING

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <time.h> 
#include <errno.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/types.h>

//!< POSIX thread structure
typedef struct S_PosixThread
{
    pthread_t       pthreadId;      //!< The POSIX thread ID
    pthread_attr_t  pthreadAttr;    //!< The POSIX thread attribute
} sPosixThread;

  /************************************************************************/
// Thread class public constants, types and enum
/************************************************************************/
/**
 * \brief   Constant, wait until created thread did not start
 **/
const unsigned int      Thread::WAIT_INFINITE             = static_cast<unsigned int>(~0);

/************************************************************************/
// System specific thread routines
/************************************************************************/
void * Thread::_posixThreadRoutine( void * data )
{
    int oldState = 0;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState);

    Thread::_defaultThreadFunction(data);

    pthread_setcancelstate(oldState, NULL);
    return NULL;
}

unsigned long Thread::_windowsThreadRoutine( void * /* data */ )
{
    ASSERT(false);
    return 0;
}

/**
 * \brief   This function call is a recommendation from MSDN documentation.
 *          It is using undocumented way to set name of thread in native code.
 **/
#if ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void Thread::_setThreadName(ITEM_ID threadId, const char* threadName)
{
    pthread_setname_np(threadId, threadName != NULL ? threadName : "");
}

#else// !((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void Thread::_setThreadName(ITEM_ID /*threadId*/, const char* /*threadName*/)
{
}

#endif // !((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void Thread::_closeHandle(  THREADHANDLE handle )
{
    if (handle != NULL)
    {
        sPosixThread *pthread = reinterpret_cast<sPosixThread *>(handle);
        pthread_attr_destroy(&pthread->pthreadAttr);
        delete pthread;
    }
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void Thread::sleep(unsigned int ms)
{
    // since _POSIX_C_SOURCE >= 199309L use nanosleep
    // otherwise, can use:
    //      unsigned int sec    = ms / 1000;
    //      unsigned int micro  = (ms % 1000) * 1000;
    //      sleep(sec);
    //      usleep(micro);
    
    struct timespec ts;
    ts.tv_sec   = ms / 1000;
    ts.tv_nsec  = (ms % 1000) * 1000000 + 1;
    nanosleep(&ts, NULL);
}

ITEM_ID Thread::getCurrentThreadId( void )
{
    return  static_cast<ITEM_ID>( pthread_self() );
}

Thread::eCompletionStatus Thread::destroyThread(unsigned int waitForStopMs /* = Thread::DO_NOT_WAIT */)
{
    // Initially, the thread is not valid and not running, nothing to destroy
    Thread::eCompletionStatus result = Thread::ThreadInvalid;
    pthread_t threadId  = static_cast<pthread_t>(Thread::INVALID_THREAD_ID);

    do
    {
        Lock lock(mSynchObject);
        if (mThreadHandle == Thread::INVALID_THREAD_HANDLE)
        {
            return Thread::ThreadInvalid;
        }

        threadId = static_cast<pthread_t>(mThreadId);
        _unregisterThread();

    } while(false);

    if ((waitForStopMs != Thread::DO_NOT_WAIT) && (mWaitForExit.lock(waitForStopMs) == false))
    {
        // here we assume that it was requested to wait for thread exit, but it is still running
        // force to terminate thread and close handles due to waiting timeout expire
        OUTPUT_DBG("The thread [ %s ] should be terminated", mThreadAddress.getThreadName().getString());
        result = Thread::ThreadTerminated;
        pthread_cancel(threadId);
    }
    else
    {
        // The thread completed job normally
        OUTPUT_DBG("The thread [ %s ] completed job", mThreadAddress.getThreadName().getString());
        result = Thread::ThreadCompleted;
        ASSERT (waitForStopMs != Thread::WAIT_INFINITE || isRunning() == false);
    }

    do
    {
        Lock lock(mSynchObject);
        _cleanResources();
    } while(false);

    return result;
}

bool Thread::_createSystemThread( void )
{
    bool result = false;

    if ((_isValidNoLock() == false) && (mThreadAddress.getThreadName().isEmpty() == false))
    {
        sPosixThread * handle = new sPosixThread;
        if ( handle != NULL)
        {
            mWaitForRun.resetEvent();
            mWaitForExit.resetEvent( );

            if ((RETURNED_OK == pthread_attr_init(&handle->pthreadAttr)) &&
                (RETURNED_OK == pthread_attr_setdetachstate(&handle->pthreadAttr, PTHREAD_CREATE_DETACHED)) &&
                (RETURNED_OK == pthread_create(&handle->pthreadId, &handle->pthreadAttr, &Thread::_posixThreadRoutine, static_cast<void *>(this))) )
            {
                result          = true;
                mThreadHandle   = static_cast<THREADHANDLE>(handle);
                mThreadId       = static_cast<ITEM_ID>(handle->pthreadId);
                mThreadPriority = Thread::PriorityNormal;

                if (_registerThread() == false)
                {
                    result = false;
                    _unregisterThread();
                    _cleanResources();
                }
            }
            else
            {
                delete handle;
            }
        }
    }

    return result;
}

Thread::eThreadPriority Thread::setPriority( eThreadPriority newPriority )
{
    /**
     * if priority of a thread is changed, a real-time scheduling policy must be used, 
     * possible policies are SCHED_FIFO and SCHED_RR. We use SCHED_RR (round robin) here.
     **/
    static const int schedPolicy    = SCHED_RR;
    static const int minPriority    = sched_get_priority_min(schedPolicy);
    static const int maxPriority    = sched_get_priority_max(schedPolicy);
    static const int deltaPrio      = (maxPriority - minPriority) / 4;

    Lock  lock(mSynchObject);
    Thread::eThreadPriority oldPrio = mThreadPriority;
    if (_isValidNoLock() && (newPriority != oldPrio))
    {
        int schedPrio       = MIN_INT_32;
        pthread_t threadId  = static_cast<pthread_t>(mThreadId);
        switch (newPriority)
        {
        case Thread::PriorityLowest:
            schedPrio  = minPriority;
            break;

        case Thread::PriorityLow:
            schedPrio  = minPriority + 1 * deltaPrio;
            break;

        case Thread::PriorityNormal:
            schedPrio  = minPriority + 2 * deltaPrio;
            break;

        case Thread::PriorityHigh:
            schedPrio  = minPriority + 3 * deltaPrio;
            break;

        case Thread::PriorityHighest:
            schedPrio  = maxPriority;
            break;

        default:
            break;  // do nothing, invalid priority value
        }

        struct sched_param schedParam;
        schedParam.sched_priority   = schedPrio;

        if ((MIN_INT_32 != schedPrio) && (RETURNED_OK == pthread_setschedparam(threadId, schedPolicy, &schedParam)))
        {
            mThreadPriority = newPriority;
        }
#ifdef DEBUG
        else
        {
            OUTPUT_ERR("Cannot set thread priority [ %s ] (POSIX priority %d ) for thread [ %p ] , failed with error code [ %x ]."
                , Thread::getString(newPriority)
                , schedParam.sched_priority
                , static_cast<id_type>(mThreadId)
                , errno);
        }
#endif // DEBUG
    }

    return oldPrio;
}

#endif  // _POSIX
