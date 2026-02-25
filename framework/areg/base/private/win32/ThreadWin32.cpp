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
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"

#ifdef  _WIN32

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>
#include <processthreadsapi.h>
#include <limits>

/************************************************************************/
// System specific thread routines
/************************************************************************/
void * areg::Thread::_posixThreadRoutine( void * /*data*/ )
{
    ASSERT(false);
    return nullptr;
}

unsigned long areg::Thread::_windowsThreadRoutine( void * data )
{
    return areg::Thread::_defaultThreadFunction(data);
}

/**
 * \brief   This function call is a recommendation from MSDN documentation.
 *          It is using undocumented way to set name of thread in native code.
 **/
void areg::Thread::_osSetThreadName( id_type threadId, const char* threadName)
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

void areg::Thread::_osCloseHandle(  THREADHANDLE handle )
{
    if ( handle != nullptr )
    {
        ::CloseHandle(static_cast<HANDLE>(handle));
    }
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void areg::Thread::_osSleep(uint32_t timeout)
{
    ::Sleep(timeout);
}

id_type areg::Thread::_osGetCurrentThreadId()
{
    return static_cast<id_type>(::GetCurrentThreadId());
}

areg::Thread::ThreadCompletion areg::Thread::_osDestroyThread(uint32_t waitForStopMs)
{
    mSyncObject.lock(areg::WAIT_INFINITE);

    areg::Thread::ThreadCompletion result = areg::Thread::ThreadCompletion::Invalid;

    THREADHANDLE handle = mThreadHandle;
    if (handle != areg::Thread::INVALID_THREAD_HANDLE)
    {
        _unregisterThread();
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
            result = areg::Thread::ThreadCompletion::Terminated;
            ::TerminateThread(static_cast<HANDLE>(handle), static_cast<DWORD>(areg::ThreadConsumer::ExitCode::Terminated));
            this->mWaitForRun.resetEvent();
            this->mWaitForExit.setEvent();
#ifdef _MSC_VER
    #pragma warning(default: 6258)
#endif // _MSC_VER
        }
        else
        {
            // The thread completed job normally
            result = areg::Thread::ThreadCompletion::Completed;
            ASSERT (waitForStopMs != areg::WAIT_INFINITE || isRunning() == false);
        }

        mSyncObject.lock(areg::WAIT_INFINITE);
    }
    else
    {
        // The thread is not valid and not running, nothing to destroy
        result = areg::Thread::ThreadCompletion::Invalid;
    }

    mSyncObject.unlock(); // nothing to do, the thread is already destroyed

    return result;
}

bool areg::Thread::_osCreateSystemThread()
{
    bool result = false;
    if ((_isValidNoLock() == false) && (mThreadAddress.getThreadName().isEmpty() == false))
    {
        mWaitForRun.resetEvent();
        mWaitForExit.resetEvent( );

        unsigned long threadId  { 0 };
        unsigned long dwFlags   { mStackSizeKB != areg::STACK_SIZE_DEFAULT ? 0u : STACK_SIZE_PARAM_IS_A_RESERVATION };
        unsigned long dwStack   { mStackSizeKB * areg::ONE_KILOBYTE };
        HANDLE handle = ::CreateThread( nullptr
                                      , dwStack
                                      , (LPTHREAD_START_ROUTINE)(&areg::Thread::_windowsThreadRoutine)
                                      , static_cast<void *>(this)
                                      , dwFlags
                                      , &threadId);
        if (handle != nullptr)
        {
            result          = true;
            mThreadHandle   = static_cast<THREADHANDLE>(handle);
            mThreadId       = threadId;
            mThreadPriority = areg::Thread::ThreadPriority::Normal;

            if (_registerThread() == false)
            {
                result = false;
                _cleanResources(true);
            }
        }
    }

    return result;
}

areg::Thread::ThreadPriority areg::Thread::_osSetPriority( ThreadPriority newPriority )
{
    Lock  lock(mSyncObject);
    areg::Thread::ThreadPriority oldPrio{ mThreadPriority };

    if (_isValidNoLock() && (newPriority != mThreadPriority))
    {
        int32_t Prio = std::numeric_limits<int32_t>::min();
        switch (newPriority)
        {
        case areg::Thread::ThreadPriority::Lowest:
            Prio = THREAD_PRIORITY_LOWEST;
            break;

        case areg::Thread::ThreadPriority::Low:
            Prio = THREAD_PRIORITY_BELOW_NORMAL;
            break;

        case areg::Thread::ThreadPriority::Normal:
            Prio = THREAD_PRIORITY_NORMAL;
            break;

        case areg::Thread::ThreadPriority::High:
            Prio = THREAD_PRIORITY_ABOVE_NORMAL;
            break;

        case areg::Thread::ThreadPriority::Highest:
            Prio = THREAD_PRIORITY_HIGHEST;
            break;

        case areg::Thread::ThreadPriority::Undefined:     // fall through
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

size_t areg::Thread::_osGetCurrentStackSize(THREADHANDLE handle)
{
    ULONG size{ 0u };
    return ((handle != NULL) && SetThreadStackGuarantee(&size) ? static_cast<size_t>(size) : 0);
}

#endif  // _WIN32
