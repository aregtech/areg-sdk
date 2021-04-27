/************************************************************************
 * \file        areg/base/private/posix/CEThreadWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread class
 *              windows specific code
 *
 ************************************************************************/
#include "areg/base/CEThread.hpp"
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

typedef struct S_PosixThread
{
    pthread_t       pthreadId;
    pthread_attr_t  pthreadAttr;
} sPosixThread;

  /************************************************************************/
// CEThread class public constants, types and enum
/************************************************************************/
/**
 * \brief   Constant, wait until created thread did not start
 **/
const unsigned int      CEThread::WAIT_INFINITE             = static_cast<unsigned int>(~0);

/************************************************************************/
// System specific thread routines
/************************************************************************/
void * CEThread::_posixThreadRoutine( void * data )
{
    int oldState = 0;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState);

    CEThread::_defaultThreadFunction(data);

    pthread_setcancelstate(oldState, NULL);
    return NULL;
}

unsigned long CEThread::_windowsThreadRoutine( void * /* data */ )
{
    ASSERT(false);
    return 0;
}

/**
 * \brief   This function call is a recommendation from MSDN documentation.
 *          It is using undocumented way to set name of thread in native code.
 **/
#if ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void CEThread::_setThreadName(ITEM_ID threadId, const char* threadName)
{
    pthread_setname_np(threadId, threadName != NULL ? threadName : "");
}

#else// !((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void CEThread::_setThreadName(ITEM_ID /*threadId*/, const char* /*threadName*/)
{
}

#endif // !((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void CEThread::_closeHandle(  THREADHANDLE handle )
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

void CEThread::Sleep(unsigned int ms)
{
    // since _POSIX_C_SOURCE >= 199309L use nanosleep
    // otherwise, can use:
    //      unsigned int sec    = ms / 1000;
    //      unsigned int micro  = (ms % 1000) * 1000;
    //      sleep(sec);
    //      usleep(micro);
    
    struct timespec ts;
    ts.tv_sec   = ms / 1000;
    ts.tv_nsec  = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

ITEM_ID CEThread::GetCurrentThreadId( void )
{
    return  reinterpret_cast<ITEM_ID>( pthread_self() );
}

CEThread::eCompletionStatus CEThread::DestroyThread(unsigned int waitForStopMs /* = CEThread::DO_NOT_WAIT */)
{
    mSynchObject.Lock(IESynchObject::WAIT_INFINITE);

    CEThread::eCompletionStatus result = CEThread::ThreadInvalid;

    THREADHANDLE handle = mThreadHandle;
    if (handle != CEThread::INVALID_THREAD_HANDLE)
    {
        UnregisterThread();
        mSynchObject.Unlock();  // unlock, to let thread complete exit task.

        if (waitForStopMs != CEThread::DO_NOT_WAIT && mWaitForExit.Lock(waitForStopMs) == false)
        {
            // here we assume that it was requested to wait for thread exit, but it is still running
            // force to terminate thread and close handles due to waiting timeout expire
            result = CEThread::ThreadTerminated;
            pthread_cancel( reinterpret_cast<pthread_t>(mThreadId) );
        }
        else
        {
            // The thread completed job normally
            result = CEThread::ThreadCompleted;
            ASSERT (waitForStopMs != CEThread::WAIT_INFINITE || IsRunning() == false);
        }

        mSynchObject.Lock(IESynchObject::WAIT_INFINITE);
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

    if ((isValidNoLock() == false) && (mThreadAddress.GetThreadName().IsEmpty() == false))
    {
        sPosixThread * handle = new sPosixThread;
        if ( handle != NULL)
        {
            mWaitForRun.ResetEvent();
            mWaitForExit.ResetEvent( );

            if ((RETURNED_OK == pthread_attr_init(&handle->pthreadAttr)) &&
                (RETURNED_OK == pthread_attr_setdetachstate(&handle->pthreadAttr, PTHREAD_CREATE_DETACHED)) &&
                (RETURNED_OK == pthread_create(&handle->pthreadId, &handle->pthreadAttr, &CEThread::_posixThreadRoutine, static_cast<void *>(this))) )
            {
                result          = true;
                mThreadHandle   = static_cast<THREADHANDLE>(handle);
                mThreadId       = reinterpret_cast<ITEM_ID>(handle->pthreadId);
                mThreadPriority = CEThread::PriorityNormal;

                if (RegisterThread() == false)
                {
                    result = false;
                    UnregisterThread();
                    CleanResources();
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

CEThread::eThreadPriority CEThread::SetThreadPriority( eThreadPriority newPriority )
{
    /**
     * if priority of a thread is changed, a real-time scheduling policy must be used, 
     * possible policies are SCHED_FIFO and SCHED_RR. We use SCHED_RR (round robin) here.
     **/
    static const int schedPolicy    = SCHED_RR;
    static const int minPriority    = sched_get_priority_min(schedPolicy);
    static const int maxPriority    = sched_get_priority_max(schedPolicy);
    static const int deltaPrio      = (maxPriority - minPriority) / 4;

    CELock  lock(mSynchObject);
    CEThread::eThreadPriority oldPrio = mThreadPriority;
    if (isValidNoLock() && (newPriority != oldPrio))
    {
        int schedPrio       = MIN_INT_32;
        pthread_t threadId  = reinterpret_cast<pthread_t>(mThreadId);
        switch (newPriority)
        {
        case CEThread::PriorityLowest:
            schedPrio  = minPriority;
            break;

        case CEThread::PriorityLow:
            schedPrio  = minPriority + 1 * deltaPrio;
            break;

        case CEThread::PriorityNormal:
            schedPrio  = minPriority + 2 * deltaPrio;
            break;

        case CEThread::PriorityHigh:
            schedPrio  = minPriority + 3 * deltaPrio;
            break;

        case CEThread::PriorityHighest:
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
                , CEThread::GetString(newPriority)
                , schedParam.sched_priority
                , mThreadId
                , errno);
        }
#endif // DEBUG
    }

    return oldPrio;
}

#endif  // _POSIX
