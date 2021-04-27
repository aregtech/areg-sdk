/************************************************************************
 * \file        areg/base/private/win32/CEThreadWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread class
 *              windows specific code
 *
 ************************************************************************/
#include "areg/base/CEThread.hpp"
#include "areg/base/IEThreadConsumer.hpp"

#ifdef  _WINDOWS

#include <windows.h>
#include <processthreadsapi.h>

/************************************************************************/
// CEThread class public constants, types and enum
/************************************************************************/
/**
 * \brief   Constant, wait until created thread did not start
 **/
const unsigned int      CEThread::WAIT_INFINITE             = static_cast<unsigned int>(INFINITE);

/************************************************************************/
// System specific thread routines
/************************************************************************/
void * CEThread::_posixThreadRoutine( void * data )
{
    ASSERT(false);
    return NULL;
}

unsigned long CEThread::_windowsThreadRoutine( void * data )
{
    return CEThread::_defaultThreadFunction(data);
}

/**
 * \brief   This function call is a recommendation from MSDN documentation.
 *          It is using undocumented way to set name of thread in native code.
 **/
void CEThread::_setThreadName(ITEM_ID threadId, const char* threadName)
{
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

    __try
    {
        RaiseException( CEThread::SET_NAME_MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
    }
    __except(EXCEPTION_CONTINUE_EXECUTION)
    {
        ; // do nothing
    }
}

void CEThread::_closeHandle(  THREADHANDLE handle )
{
    if (handle != NULL)
        ::CloseHandle(static_cast<HANDLE>(handle));
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void CEThread::Sleep(unsigned int ms)
{
    ::Sleep(ms);
}

ITEM_ID CEThread::GetCurrentThreadId( void )
{
    return static_cast<ITEM_ID>(::GetCurrentThreadId());
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
#ifdef  _DEBUG
            //////////////////////////////////////////////////////////////////////////
            // AAvetyan:    In case if thread does not exit properly, 
            //              uncomment these lines to raise assertion
            //              and see the point where the thread is suspended
            //////////////////////////////////////////////////////////////////////////
            ::SuspendThread(handle);
            ASSERT(false);  // <== rais assertion
            ::ResumeThread(handle);
            //////////////////////////////////////////////////////////////////////////
            //
            //////////////////////////////////////////////////////////////////////////
#endif  // _DEBUG

            // here we assume that it was requested to wait for thread exit, but it is still running
            // force to terminate thread and close handles due to waiting timeout expire
            result = CEThread::ThreadTerminated;
            ::TerminateThread(static_cast<HANDLE>(handle), static_cast<DWORD>(IEThreadConsumer::EXIT_TERMINATED));
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
        mWaitForRun.ResetEvent();
        mWaitForExit.ResetEvent( );

        unsigned long threadId  = 0;
        HANDLE handle = ::CreateThread(NULL, 0, 
                                       (LPTHREAD_START_ROUTINE)&CEThread::_windowsThreadRoutine, 
                                       static_cast<void *>(this), 0 /*CREATE_SUSPENDED*/, &threadId);
        if (handle != static_cast<HANDLE>(NULL))
        {
            result          = true;
            mThreadHandle   = static_cast<THREADHANDLE>(handle);
            mThreadId       = threadId;
            mThreadPriority = CEThread::PriorityNormal;

            if (RegisterThread() == false)
            {
                result = false;
                UnregisterThread();
                CleanResources();
            }
        }
    }

    return result;
}

CEThread::eThreadPriority CEThread::SetThreadPriority( eThreadPriority newPriority )
{
    CELock  lock(mSynchObject);
    CEThread::eThreadPriority oldPrio = mThreadPriority;
    if (isValidNoLock() && (newPriority != mThreadPriority))
    {
        int Prio = MIN_INT_32;
        switch (newPriority)
        {
        case CEThread::PriorityLowest:
            Prio = THREAD_PRIORITY_LOWEST;
            break;

        case CEThread::PriorityLow:
            Prio = THREAD_PRIORITY_BELOW_NORMAL;
            break;

        case CEThread::PriorityNormal:
            Prio = THREAD_PRIORITY_NORMAL;
            break;

        case CEThread::PriorityHigh:
            Prio = THREAD_PRIORITY_ABOVE_NORMAL;
            break;

        case CEThread::PriorityHighest:
            Prio = THREAD_PRIORITY_HIGHEST;
            break;

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
