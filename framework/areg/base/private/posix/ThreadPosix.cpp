/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/ThreadWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread class
 *              windows specific code
 *
 ************************************************************************/
#include "areg/base/Thread.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/IEThreadConsumer.hpp"

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

namespace 
{

    //!< POSIX thread structure
    typedef struct S_PosixThread
    {
        pthread_t       pthreadId;      //!< The POSIX thread ID
        pthread_attr_t  pthreadAttr;    //!< The POSIX thread attribute
    } sPosixThread;

} // namespace

/************************************************************************/
// System specific thread routines
/************************************************************************/
void * Thread::_posixThreadRoutine( void * data )
{
    int oldState{ 0 };
    int oldType{ 0 };
    ::pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldType);

    Thread::_defaultThreadFunction(data);

    ::pthread_setcancelstate(oldState, nullptr);
    ::pthread_setcanceltype(oldType, nullptr);

    return nullptr;
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

void Thread::_osSetThreadName( id_type threadId, const char* threadName)
{
    pthread_setname_np(threadId, threadName != nullptr ? threadName : "");
}

#else// !((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void Thread::_osSetThreadName( id_type /*threadId*/, const char* /*threadName*/)
{
}

#endif // !((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void Thread::_osCloseHandle(  THREADHANDLE handle )
{
    if (handle != nullptr)
    {
        sPosixThread *pthread = reinterpret_cast<sPosixThread *>(handle);
        pthread_attr_destroy(&pthread->pthreadAttr);
        delete pthread;
    }
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void Thread::_osSleep(unsigned int timeout)
{
    // since _POSIX_C_SOURCE >= 199309L use nanosleep
    // otherwise, can use:
    //      unsigned int sec    = ms / 1000;
    //      unsigned int micro  = (ms % 1000) * 1000;
    //      sleep(sec);
    //      usleep(micro);
    
    struct timespec ts;
    ts.tv_sec   = timeout / 1'000;
    ts.tv_nsec  = (timeout % 1'000) * 1'000'000 + 1;
    nanosleep(&ts, nullptr);
}

id_type Thread::_osGetCurrentThreadId( void )
{
    return  reinterpret_cast<id_type>( pthread_self() );
}

Thread::eCompletionStatus Thread::_osDestroyThread(unsigned int waitForStopMs)
{
    // Initially, the thread is not valid and not running, nothing to destroy
    Thread::eCompletionStatus result = Thread::eCompletionStatus::ThreadInvalid;
    pthread_t threadId  = reinterpret_cast<pthread_t>(Thread::INVALID_THREAD_ID);

    do
    {
        Lock lock(mSynchObject);
        if (mThreadHandle == Thread::INVALID_THREAD_HANDLE)
        {
            return Thread::eCompletionStatus::ThreadInvalid;
        }

        threadId = reinterpret_cast<pthread_t>(mThreadId);
        _unregisterThread();

    } while(false);

    if ((waitForStopMs != NECommon::DO_NOT_WAIT) && (mWaitForExit.lock(waitForStopMs) == false))
    {
        // here we assume that it was requested to wait for thread exit, but it is still running
        // force to terminate thread and close handles due to waiting timeout expire
        OUTPUT_DBG("The thread [ %s ] should be terminated", mThreadAddress.getThreadName().getString());
        result = Thread::eCompletionStatus::ThreadTerminated;
        pthread_cancel(threadId);
    }
    else
    {
        // The thread completed job normally
        OUTPUT_DBG("The thread [ %s ] completed job", mThreadAddress.getThreadName().getString());
        result = Thread::eCompletionStatus::ThreadCompleted;
        ASSERT (waitForStopMs != NECommon::WAIT_INFINITE || isRunning() == false);
    }

    do
    {
        Lock lock(mSynchObject);
        _cleanResources();
    } while(false);

    return result;
}

bool Thread::_osCreateSystemThread( void )
{
    bool result = false;

    if ((_isValidNoLock() == false) && (mThreadAddress.getThreadName().isEmpty() == false))
    {
        sPosixThread * handle = new sPosixThread;
        if ( handle != nullptr)
        {
            mWaitForRun.resetEvent();
            mWaitForExit.resetEvent( );

            if ((RETURNED_OK == ::pthread_attr_init(&handle->pthreadAttr)) &&
                (RETURNED_OK == ::pthread_attr_setdetachstate(&handle->pthreadAttr, PTHREAD_CREATE_DETACHED)) &&
                (RETURNED_OK == ::pthread_create(&handle->pthreadId, &handle->pthreadAttr, &Thread::_posixThreadRoutine, static_cast<void *>(this))) )
            {
                result          = true;
                mThreadHandle   = static_cast<THREADHANDLE>(handle);
                mThreadId       = reinterpret_cast<id_type>(handle->pthreadId);
                mThreadPriority = Thread::eThreadPriority::PriorityNormal;

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

Thread::eThreadPriority Thread::_osSetPriority( eThreadPriority newPriority )
{
    /**
     * if priority of a thread is changed, a real-time scheduling policy must be used, 
     * possible policies are SCHED_FIFO and SCHED_RR. We use SCHED_RR (round robin) here.
     **/
    static constexpr int schedPolicy{ SCHED_RR };
    static const int minPriority{ sched_get_priority_min( schedPolicy ) };
    static const int maxPriority{ sched_get_priority_max( schedPolicy ) };
    static const int deltaPrio  { (maxPriority - minPriority) / 4 };

    Lock  lock(mSynchObject);
    Thread::eThreadPriority oldPrio = mThreadPriority;
    if (_isValidNoLock() && (newPriority != oldPrio))
    {
        int schedPrio       { MIN_INT_32 };
        pthread_t threadId  = reinterpret_cast<pthread_t>(mThreadId);
        switch (newPriority)
        {
        case Thread::eThreadPriority::PriorityLowest:
            schedPrio  = minPriority;
            break;

        case Thread::eThreadPriority::PriorityLow:
            schedPrio  = minPriority + 1 * deltaPrio;
            break;

        case Thread::eThreadPriority::PriorityNormal:
            schedPrio  = minPriority + 2 * deltaPrio;
            break;

        case Thread::eThreadPriority::PriorityHigh:
            schedPrio  = minPriority + 3 * deltaPrio;
            break;

        case Thread::eThreadPriority::PriorityHighest:
            schedPrio  = maxPriority;
            break;

        default:
            break;  // do nothing, invalid priority value
        }

        struct sched_param schedParam;
        schedParam.sched_priority   = schedPrio;

        if ((MIN_INT_32 != schedPrio) && (RETURNED_OK == ::pthread_setschedparam(threadId, schedPolicy, &schedParam)))
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

#endif  // defined(_POSIX) || defined(POSIX)
