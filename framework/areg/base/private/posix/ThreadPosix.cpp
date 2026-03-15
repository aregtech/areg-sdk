/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/ThreadWin.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread class
 *              windows specific code
 *
 ************************************************************************/
#include "areg/base/Thread.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/ThreadConsumer.hpp"
#include "areg/base/UtilityDefs.hpp"

#ifndef _POSIX_PRIORITY_SCHEDULING
    #define _POSIX_PRIORITY_SCHEDULING
#endif  // _POSIX_PRIORITY_SCHEDULING

#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>

#ifdef min
    #undef min
#endif // min
#ifdef max
    #undef max
#endif // max
#include <limits>

#if __has_include(<sys/unistd.h>)
    #include <sys/signal.h>
    #include <sys/unistd.h>
#else   // !__has_include(<sys/unistd.h>)
    #include <unistd.h>
    #include <signal.h>
#endif  // __has_include(<sys/unistd.h>)

namespace {

    //!< POSIX thread structure
    struct PosixThread
    {
        pthread_t       pthreadId;      //!< The POSIX thread ID
        pthread_attr_t  pthreadAttr;    //!< The POSIX thread attribute
    };

} // namespace

namespace areg {

/************************************************************************/
// System specific thread routines
/************************************************************************/
void* Thread::_posix_thread_routine(void* data)
{
    int32_t oldState{ 0 };
    int32_t oldType{ 0 };
    ::pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldType);

    Thread::_default_thread_function(data);

    ::pthread_setcancelstate(oldState, nullptr);
    ::pthread_setcanceltype(oldType, nullptr);

    ::pthread_exit(nullptr);
    return nullptr;
}

unsigned long Thread::_windows_thread_routine(void* /* data */)
{
    ASSERT(false);
    return 0;
}

/**
 * \brief   This function call is a recommendation from MSDN documentation.
 *          It is using undocumented way to set name of thread in native code.
 **/
#if ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void Thread::_os_set_name(id_type threadId, const char* threadName)
{
    pthread_setname_np(threadId, threadName != nullptr ? threadName : "");
}

#else// !((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void Thread::_os_set_name(id_type /*threadId*/, const char* /*threadName*/)
{
}

#endif // !((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 12)))

void Thread::_os_close_handle(THREADHANDLE handle)
{
    if (handle != nullptr)
    {
        PosixThread* pthread = reinterpret_cast<PosixThread*>(handle);
        pthread_attr_destroy(&pthread->pthreadAttr);
        delete pthread;
    }
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void Thread::_os_sleep(uint32_t timeout)
{
    // since _POSIX_C_SOURCE >= 199309L use nanosleep
    // otherwise, can use:
    //      uint32_t sec    = ms / 1000;
    //      uint32_t micro  = (ms % 1000) * 1000;
    //      sleep(sec);
    //      usleep(micro);

    struct timespec ts;
    ts.tv_sec = timeout / 1'000;
    ts.tv_nsec = (timeout % 1'000) * 1'000'000 + 1;
    nanosleep(&ts, nullptr);
}

id_type Thread::_os_thread_id()  noexcept
{
    return to_num<id_type, pthread_t>(pthread_self());
}

Thread::ThreadCompletion Thread::_os_destroy_thread(uint32_t waitForStopMs)
{
    // Initially, the thread is not valid and not running, nothing to destroy
    Thread::ThreadCompletion result = Thread::ThreadCompletion::Invalid;
    pthread_t threadId = to_ptr<pthread_t, id_type>(Thread::INVALID_THREAD_ID);

    do
    {
        Lock lock(mSyncObject);
        if (mThreadHandle == Thread::INVALID_THREAD_HANDLE)
        {
            return Thread::ThreadCompletion::Invalid;
        }

        threadId = to_ptr<pthread_t, id_type>(mThreadId);
        _unregister_thread();

    } while (false);

    if ((waitForStopMs != DO_NOT_WAIT) && (mWaitForExit.lock(waitForStopMs) == false))
    {
        // here we assume that it was requested to wait for thread exit, but it is still running
        // force to terminate thread and close handles due to waiting timeout expire
        AREG_OUTPUT_DBG("The thread [ %s ] should be terminated", mThreadAddress.name().as_string());
        result = Thread::ThreadCompletion::Terminated;
        pthread_cancel(threadId);
        mWaitForRun.reset();
        mWaitForExit.set_signaled();
    }
    else
    {
        // The thread completed job normally
        AREG_OUTPUT_DBG("The thread [ %s ] completed job", mThreadAddress.name().as_string());
        result = Thread::ThreadCompletion::Completed;
        ASSERT(waitForStopMs != WAIT_INFINITE || is_running() == false);
    }

    return result;
}

bool Thread::_os_create() noexcept
{
    bool result{ false };

    if ((_is_valid_no_lock() == false) && (mThreadAddress.name().is_empty() == false))
    {
        PosixThread* handle = new  PosixThread;
        if (handle != nullptr)
        {
            mWaitForRun.reset();
            mWaitForExit.reset();
            if (RETURNED_OK == ::pthread_attr_init(&handle->pthreadAttr))
            {
                if (mStackSizeKB != STACK_SIZE_DEFAULT)
                {
                    size_t stackSizeBytes = static_cast<size_t>(mStackSizeKB) * 1024u;
                    ::pthread_attr_setstacksize(&handle->pthreadAttr, stackSizeBytes);
                }

                if ((RETURNED_OK == ::pthread_attr_setdetachstate(&handle->pthreadAttr, PTHREAD_CREATE_DETACHED)) &&
                    (RETURNED_OK == ::pthread_create(&handle->pthreadId, &handle->pthreadAttr, &Thread::_posix_thread_routine, static_cast<void*>(this))))
                {
                    result = true;
                    mThreadHandle = static_cast<THREADHANDLE>(handle);
                    mThreadId = to_num<id_type, pthread_t>(handle->pthreadId);
                    mThreadPriority = Thread::ThreadPriority::Normal;

                    if (_register_thread() == false)
                    {
                        result = false;
                        _clean_resources(true);
                    }
                }

                if (result == false)
                {
                    _os_close_handle(handle);
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

Thread::ThreadPriority Thread::_os_set_priority(ThreadPriority newPriority) noexcept
{
    /**
     * if priority of a thread is changed, a real-time scheduling policy must be used,
     * possible policies are SCHED_FIFO and SCHED_RR. We use SCHED_RR (round robin) here.
     **/
    static constexpr int32_t schedPolicy{ SCHED_RR };
    static const int32_t minPriority{ sched_get_priority_min(schedPolicy) };
    static const int32_t maxPriority{ sched_get_priority_max(schedPolicy) };
    static const int32_t deltaPrio{ (maxPriority - minPriority) / 4 };

    Lock  lock(mSyncObject);
    Thread::ThreadPriority oldPrio = mThreadPriority;
    if (_is_valid_no_lock() && (newPriority != oldPrio))
    {
        int32_t schedPrio{ std::numeric_limits<int32_t>::min() };
        pthread_t threadId{ to_ptr<pthread_t, id_type>(mThreadId) };
        switch (newPriority)
        {
        case Thread::ThreadPriority::Lowest:
            schedPrio = minPriority;
            break;

        case Thread::ThreadPriority::Low:
            schedPrio = minPriority + 1 * deltaPrio;
            break;

        case Thread::ThreadPriority::Normal:
            schedPrio = minPriority + 2 * deltaPrio;
            break;

        case Thread::ThreadPriority::High:
            schedPrio = minPriority + 3 * deltaPrio;
            break;

        case Thread::ThreadPriority::Highest:
            schedPrio = maxPriority;
            break;

        default:
            break;  // do nothing, invalid priority value
        }

        struct sched_param schedParam;
        schedParam.sched_priority = schedPrio;

        if ((std::numeric_limits<int32_t>::min() != schedPrio) && (areg::RETURNED_OK == ::pthread_setschedparam(threadId, schedPolicy, &schedParam)))
        {
            mThreadPriority = newPriority;
        }
#ifdef DEBUG
        else
        {
            AREG_OUTPUT_ERR("Cannot set thread priority [ %s ] (POSIX priority %d ) for thread [ %p ] , failed with error code [ %x ]."
                , Thread::as_string(newPriority)
                , schedParam.sched_priority
                , static_cast<id_type>(mThreadId)
                , errno);
        }
#endif // DEBUG
    }

    return oldPrio;
}

size_t Thread::_os_stack_size(THREADHANDLE handle) noexcept
{
    size_t size{ 0u };
    PosixThread* thread = reinterpret_cast<PosixThread*>(handle);
    return ((thread != nullptr) && (RETURNED_OK == pthread_attr_getstacksize(&thread->pthreadAttr, &size)) ? size : 0);
}

} // namespace areg

#endif  // defined(_POSIX) || defined(POSIX)
