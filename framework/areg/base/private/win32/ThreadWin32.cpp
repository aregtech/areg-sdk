/************************************************************************
 * \file        areg/base/private/win32/ThreadWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
// Thread class public constants, types and enum
/************************************************************************/
/**
 * \brief   Constant, wait until created thread did not start
 **/
const unsigned int      Thread::WAIT_INFINITE             = static_cast<unsigned int>(INFINITE);

/************************************************************************/
// System specific thread routines
/************************************************************************/
void * Thread::_posixThreadRoutine( void * /*data*/ )
{
    ASSERT(false);
    return NULL;
}

unsigned long Thread::_windowsThreadRoutine( void * data )
{
    return Thread::_defaultThreadFunction(data);
}

/**
 * \brief   This function call is a recommendation from MSDN documentation.
 *          It is using undocumented way to set name of thread in native code.
 **/
void Thread::_setThreadName(ITEM_ID threadId, const char* threadName)
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
        RaiseException( Thread::SET_NAME_MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
    }
    __except(EXCEPTION_CONTINUE_EXECUTION)
    {
        ; // do nothing
    }
}

void Thread::_closeHandle(  THREADHANDLE handle )
{
    if (handle != NULL)
        ::CloseHandle(static_cast<HANDLE>(handle));
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void Thread::sleep(unsigned int ms)
{
    ::Sleep(ms);
}

ITEM_ID Thread::getCurrentThreadId( void )
{
    return static_cast<ITEM_ID>(::GetCurrentThreadId());
}

Thread::eCompletionStatus Thread::destroyThread(unsigned int waitForStopMs /* = Thread::DO_NOT_WAIT */)
{
    mSynchObject.lock(IESynchObject::WAIT_INFINITE);

    Thread::eCompletionStatus result = Thread::ThreadInvalid;

    THREADHANDLE handle = mThreadHandle;
    if (handle != Thread::INVALID_THREAD_HANDLE)
    {
        _unregisterThread();
        mSynchObject.unlock();  // unlock, to let thread complete exit task.

        if ((waitForStopMs != Thread::DO_NOT_WAIT) && (mWaitForExit.lock(waitForStopMs) == false))
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
            result = Thread::ThreadTerminated;
            ::TerminateThread(static_cast<HANDLE>(handle), static_cast<DWORD>(IEThreadConsumer::EXIT_TERMINATED));
        }
        else
        {
            // The thread completed job normally
            result = Thread::ThreadCompleted;
            ASSERT (waitForStopMs != Thread::WAIT_INFINITE || isRunning() == false);
        }

        mSynchObject.lock(IESynchObject::WAIT_INFINITE);
        _cleanResources();
    }
    else
    {
        // The thread is not valid and not running, nothing to destroy
        result = Thread::ThreadInvalid;
    }
    mSynchObject.unlock(); // nothing to do, the thread is already destroyed

    return result;
}

bool Thread::_createSystemThread( void )
{
    bool result = false;
    if ((_isValidNoLock() == false) && (mThreadAddress.getThreadName().isEmpty() == false))
    {
        mWaitForRun.resetEvent();
        mWaitForExit.resetEvent( );

        unsigned long threadId  = 0;
        HANDLE handle = ::CreateThread(NULL, 0, 
                                       (LPTHREAD_START_ROUTINE)&Thread::_windowsThreadRoutine, 
                                       static_cast<void *>(this), 0 /*CREATE_SUSPENDED*/, &threadId);
        if (handle != static_cast<HANDLE>(NULL))
        {
            result          = true;
            mThreadHandle   = static_cast<THREADHANDLE>(handle);
            mThreadId       = threadId;
            mThreadPriority = Thread::PriorityNormal;

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

Thread::eThreadPriority Thread::setPriority( eThreadPriority newPriority )
{
    Lock  lock(mSynchObject);
    Thread::eThreadPriority oldPrio = mThreadPriority;
    if (_isValidNoLock() && (newPriority != mThreadPriority))
    {
        int Prio = MIN_INT_32;
        switch (newPriority)
        {
        case Thread::PriorityLowest:
            Prio = THREAD_PRIORITY_LOWEST;
            break;

        case Thread::PriorityLow:
            Prio = THREAD_PRIORITY_BELOW_NORMAL;
            break;

        case Thread::PriorityNormal:
            Prio = THREAD_PRIORITY_NORMAL;
            break;

        case Thread::PriorityHigh:
            Prio = THREAD_PRIORITY_ABOVE_NORMAL;
            break;

        case Thread::PriorityHighest:
            Prio = THREAD_PRIORITY_HIGHEST;
            break;

        case Thread::PriorityUndefined:     // fall through
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
