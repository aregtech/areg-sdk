/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/ThreadWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread class
 *              windows specific code
 *
 ************************************************************************/

#ifdef  _WIN32

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>
#include <processthreadsapi.h>
#include <limits>

namespace areg {

/************************************************************************/
// System specific thread routines
/************************************************************************/
void * Thread::_posix_thread_routine( void * /*data*/ )
{
    ASSERT(false);
    return nullptr;
}

unsigned long Thread::_windows_thread_routine( void * data )
{
    return Thread::_default_thread_function(data);
}

/**
 * \brief   This function call is a recommendation from MSDN documentation.
 *          It is using undocumented way to set name of thread in native code.
 **/
void Thread::_os_set_name( id_type threadId, const char* threadName)
{
#ifdef _MSC_VER
    /**
     * \brief   MS Exception value, used to set thread name.
     **/
    static constexpr uint32_t   SET_NAME_MS_VC_EXCEPTION{ 0x406D1388u };

#pragma pack(push, 8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD   dwType;     // Must be 0x1000.
        LPCSTR  szName;     // Pointer to name (in user addr space).
        DWORD   dwThreadID; // Thread ID (-1=caller thread).
        DWORD   dwFlags;    // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    THREADNAME_INFO info{};
    info.dwType     = 0x1000;
    info.szName     = threadName;
    info.dwThreadID = static_cast<DWORD>(threadId);
    info.dwFlags    = 0;

#pragma warning(disable: 6312)
    __try
    {
        RaiseException( SET_NAME_MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR *>(&info) );
    }
    __except(EXCEPTION_CONTINUE_EXECUTION)
    {
        return;
    }
#pragma warning(default: 6312)
#endif // _MSC_VER
}

void Thread::_os_close_handle(  THREADHANDLE handle )
{
    if ( handle != nullptr )
    {
        ::CloseHandle(static_cast<HANDLE>(handle));
    }
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void Thread::_os_sleep(uint32_t timeout)
{
    ::Sleep(timeout);
}

id_type Thread::_os_thread_id() noexcept
{
    return static_cast<id_type>(::GetCurrentThreadId());
}

Thread::ThreadCompletion Thread::_os_destroy_thread(uint32_t waitForStopMs)
{
    mSyncObject.lock(areg::WAIT_INFINITE);

    Thread::ThreadCompletion result = Thread::ThreadCompletion::Invalid;

    THREADHANDLE handle = mThreadHandle;
    if (handle != Thread::INVALID_THREAD_HANDLE)
    {
        _unregister_thread();
        mSyncObject.unlock();  // unlock, to let thread complete exit task.

        if ((waitForStopMs != areg::DO_NOT_WAIT) && (mWaitForExit.lock(waitForStopMs) == false))
        {
#ifdef  _DEBUG
            //////////////////////////////////////////////////////////////////////////
            // AAvetyan:    In case if thread does not exit properly, 
            //              uncomment these lines to raise assertion
            //              and see the point where the thread is suspended
            //////////////////////////////////////////////////////////////////////////
            ::SuspendThread(handle);
            // ASSERT(false);  // <== raise assertion
            // ::ResumeThread(handle);
            //////////////////////////////////////////////////////////////////////////
            //
            //////////////////////////////////////////////////////////////////////////
#endif  // _DEBUG

#ifdef _MSC_VER
    #pragma warning(disable: 6258)
#endif // _MSC_VER
            // here we assume that it was requested to wait for thread exit, but it is still running
            // force to terminate thread and close handles due to waiting timeout expire
            result = Thread::ThreadCompletion::Terminated;
            ::TerminateThread(static_cast<HANDLE>(handle), static_cast<DWORD>(ThreadConsumer::ExitCode::Terminated));
            this->mWaitForRun.reset();
            this->mWaitForExit.set_signaled();
#ifdef _MSC_VER
    #pragma warning(default: 6258)
#endif // _MSC_VER
        }
        else
        {
            // The thread completed job normally
            result = Thread::ThreadCompletion::Completed;
            ASSERT (waitForStopMs != areg::WAIT_INFINITE || is_running() == false);
        }

        mSyncObject.lock(areg::WAIT_INFINITE);
    }
    else
    {
        // The thread is not valid and not running, nothing to destroy
        result = Thread::ThreadCompletion::Invalid;
    }

    mSyncObject.unlock(); // nothing to do, the thread is already destroyed

    return result;
}

bool Thread::_os_create() noexcept
{
    if (_is_valid_no_lock() || mThreadAddress.name().is_empty())
        return false;
    
    mWaitForRun.reset();
    mWaitForExit.reset( );

    DWORD threadId  { 0 };
    HANDLE handle = ::CreateThread( nullptr
                                  , mStackSizeKB * areg::ONE_KILOBYTE
                                  , (LPTHREAD_START_ROUTINE)(&Thread::_windows_thread_routine)
                                  , static_cast<void *>(this)
                                  , mStackSizeKB != areg::DEFAULT_STACK_SIZE ? 0u : STACK_SIZE_PARAM_IS_A_RESERVATION
                                  , &threadId);
    if (handle == nullptr)
        return false;
    
    mThreadHandle   = static_cast<THREADHANDLE>(handle);
    mThreadId       = static_cast<id_type>(threadId);
    mThreadPriority = Thread::ThreadPriority::Normal;

    if (!_register_thread())
    {
        _clean_resources(true);
        return false;
    }

    return true;
}

Thread::ThreadPriority Thread::_os_set_priority( ThreadPriority newPriority ) noexcept
{
    Lock  lock(mSyncObject);
    Thread::ThreadPriority oldPrio{ mThreadPriority };

    if (_is_valid_no_lock() && (newPriority != mThreadPriority))
    {
        int32_t Prio = std::numeric_limits<int32_t>::min();
        switch (newPriority)
        {
        case Thread::ThreadPriority::Lowest:
            Prio = THREAD_PRIORITY_LOWEST;
            break;

        case Thread::ThreadPriority::Low:
            Prio = THREAD_PRIORITY_BELOW_NORMAL;
            break;

        case Thread::ThreadPriority::Normal:
            Prio = THREAD_PRIORITY_NORMAL;
            break;

        case Thread::ThreadPriority::High:
            Prio = THREAD_PRIORITY_ABOVE_NORMAL;
            break;

        case Thread::ThreadPriority::Highest:
            Prio = THREAD_PRIORITY_HIGHEST;
            break;

        case Thread::ThreadPriority::Undefined:     // fall through
        default:
            break;  // do nothing, invalid priority value
        }

        if ((std::numeric_limits<int32_t>::min() != Prio) && (::SetThreadPriority(mThreadHandle, Prio) == TRUE))
        {
            mThreadPriority = newPriority;
        }
    }

    return oldPrio;
}

size_t Thread::_os_stack_size(THREADHANDLE handle) noexcept
{
    ULONG size{ 0u };
    return ((handle != nullptr) && SetThreadStackGuarantee(&size) ? static_cast<size_t>(size) : 0);
}

void Thread::_os_yield_to_thread() noexcept
{
    ::SwitchToThread();
}

} // namespace areg
#endif  // _WIN32
