/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/ThreadWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread class
 *              windows specific code
 *
 ************************************************************************/
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"

#ifdef  _WINDOWS

#include <windows.h>
#include <processthreadsapi.h>

/************************************************************************/
// System specific thread routines
/************************************************************************/
void * Thread::_posixThreadRoutine( void * /*data*/ )
{
    ASSERT(false);
    return nullptr;
}

unsigned long Thread::_windowsThreadRoutine( void * data )
{
    return Thread::_defaultThreadFunction(data);
}

/**
 * \brief   This function call is a recommendation from MSDN documentation.
 *          It is using undocumented way to set name of thread in native code.
 **/
void Thread::_osSetThreadName( id_type threadId, const char* threadName)
{
    /**
     * \brief   MS Exception value, used to set thread name.
     **/
    static constexpr unsigned int   SET_NAME_MS_VC_EXCEPTION    = 0x406D1388u;

#pragma pack(push, 8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD   dwType;     // Must be 0x1000.
        LPCSTR  szName;     // Pointer to name (in user addr space).
        DWORD   dwThreadID; // Thread ID (-1=caller thread).
        DWORD   dwFlags;    // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    THREADNAME_INFO info;
    info.dwType     = 0x1000;
    info.szName     = threadName;
    info.dwThreadID = static_cast<DWORD>(threadId);
    info.dwFlags    = 0;

#pragma warning(disable: 6312)
    __try
    {
        RaiseException( SET_NAME_MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
    }
    __except(EXCEPTION_CONTINUE_EXECUTION)
    {
        return;
    }
#pragma warning(default: 6312)
}

void Thread::_osCloseHandle(  THREADHANDLE handle )
{
    if ( handle != nullptr )
    {
        ::CloseHandle(static_cast<HANDLE>(handle));
    }
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void Thread::_osSleep(unsigned int timeout)
{
    ::Sleep(timeout);
}

id_type Thread::_osGetCurrentThreadId( void )
{
    return static_cast<id_type>(::GetCurrentThreadId());
}

Thread::eCompletionStatus Thread::_osDestroyThread(unsigned int waitForStopMs)
{
    mSynchObject.lock(NECommon::WAIT_INFINITE);

    Thread::eCompletionStatus result = Thread::eCompletionStatus::ThreadInvalid;

    THREADHANDLE handle = mThreadHandle;
    if (handle != Thread::INVALID_THREAD_HANDLE)
    {
        _unregisterThread();
        mSynchObject.unlock();  // unlock, to let thread complete exit task.

        if ((waitForStopMs != NECommon::DO_NOT_WAIT) && (mWaitForExit.lock(waitForStopMs) == false))
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

#pragma warning(disable: 6258)
            // here we assume that it was requested to wait for thread exit, but it is still running
            // force to terminate thread and close handles due to waiting timeout expire
            result = Thread::eCompletionStatus::ThreadTerminated;
            ::TerminateThread(static_cast<HANDLE>(handle), static_cast<DWORD>(IEThreadConsumer::eExitCodes::ExitTerminated));
            this->mWaitForRun.resetEvent();
            this->mWaitForExit.setEvent();
#pragma warning(default: 6258)
        }
        else
        {
            // The thread completed job normally
            result = Thread::eCompletionStatus::ThreadCompleted;
            ASSERT (waitForStopMs != NECommon::WAIT_INFINITE || isRunning() == false);
        }

        mSynchObject.lock(NECommon::WAIT_INFINITE);
        _cleanResources();
    }
    else
    {
        // The thread is not valid and not running, nothing to destroy
        result = Thread::eCompletionStatus::ThreadInvalid;
    }

    mSynchObject.unlock(); // nothing to do, the thread is already destroyed

    return result;
}

bool Thread::_osCreateSystemThread( void )
{
    bool result = false;
    if ((_isValidNoLock() == false) && (mThreadAddress.getThreadName().isEmpty() == false))
    {
        mWaitForRun.resetEvent();
        mWaitForExit.resetEvent( );

        unsigned long threadId  = 0;
        HANDLE handle = ::CreateThread( nullptr, 0,
                                       (LPTHREAD_START_ROUTINE)&Thread::_windowsThreadRoutine, 
                                       static_cast<void *>(this), 0 /*CREATE_SUSPENDED*/, &threadId);
        if (handle != nullptr)
        {
            result          = true;
            mThreadHandle   = static_cast<THREADHANDLE>(handle);
            mThreadId       = threadId;
            mThreadPriority = Thread::eThreadPriority::PriorityNormal;

            if (_registerThread() == false)
            {
                result = false;
                _unregisterThread();
                _cleanResources();
            }
        }
    }

    return result;
}

Thread::eThreadPriority Thread::_osSetPriority( eThreadPriority newPriority )
{
    Lock  lock(mSynchObject);
    Thread::eThreadPriority oldPrio{ mThreadPriority };

    if (_isValidNoLock() && (newPriority != mThreadPriority))
    {
        int Prio = MIN_INT_32;
        switch (newPriority)
        {
        case Thread::eThreadPriority::PriorityLowest:
            Prio = THREAD_PRIORITY_LOWEST;
            break;

        case Thread::eThreadPriority::PriorityLow:
            Prio = THREAD_PRIORITY_BELOW_NORMAL;
            break;

        case Thread::eThreadPriority::PriorityNormal:
            Prio = THREAD_PRIORITY_NORMAL;
            break;

        case Thread::eThreadPriority::PriorityHigh:
            Prio = THREAD_PRIORITY_ABOVE_NORMAL;
            break;

        case Thread::eThreadPriority::PriorityHighest:
            Prio = THREAD_PRIORITY_HIGHEST;
            break;

        case Thread::eThreadPriority::PriorityUndefined:     // fall through
        default:
            break;  // do nothing, invalid priority value
        }
        
        if ((MIN_INT_32 != Prio)  && (::SetThreadPriority(mThreadHandle, Prio) == TRUE))
        {
            mThreadPriority = newPriority;
        }
    }

    return oldPrio;
}

#endif  // _WINDOWS
