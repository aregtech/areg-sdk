/************************************************************************
 * \file        areg/base/private/CEThread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread class
 *              platform independent code
 *
 ************************************************************************/
#include "areg/base/CEThread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/base/CEThreadLocalStorage.hpp"

//////////////////////////////////////////////////////////////////////////
// CEThread class constants definition
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// CEThread class public constants, types and enum
/************************************************************************/
/**
 * \brief   Invalid thread ID.
 **/
const ITEM_ID           CEThread::INVALID_THREAD_ID         = static_cast<unsigned int>(0);    /*0x00000000*/

/************************************************************************/
// CEThread class private constants, types and enum
/************************************************************************/
/**
 * \brief   Thread invalid handle
 **/
THREADHANDLE const      CEThread::INVALID_THREAD_HANDLE     = static_cast<THREADHANDLE>(NULL);
/**
 * \brief   Internal thread naming prefix. Used to generate unique thread name.
 **/
const char* const       CEThread::DEFAULT_THREAD_PREFIX     = static_cast<const char *>("_AREG_Thread_");
/**
 * \brief   Identified Current Thread. Used for Local Storage
 **/
CEThread* const         CEThread::CURRENT_THREAD            = reinterpret_cast<CEThread *>(~0); /*0xFFFFFFFF*/
/**
 * \brief   The name of entry in Thread Local Storage to save pointer of Thread Consumer.
 **/
const char* const       CEThread::STORAGE_THREAD_CONSUMER   = "ThreadConsumer";
/**
 * \brief   MS Exception value, used to set thread name.
 **/
const unsigned int      CEThread::SET_NAME_MS_VC_EXCEPTION  = static_cast<unsigned int>(0x406D1388);

/************************************************************************/
// CEThread class
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// CEThread class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Implement runtime
/************************************************************************/
IMPLEMENT_RUNTIME(CEThread, CERuntimeObject)

/************************************************************************/
// Define internal static mapping objects
/************************************************************************/
CEThread::MapThreadHandleResource       CEThread::_mapThreadhHandle;
CEThread::MapThreadNameResource         CEThread::_mapThreadName;
CEThread::MapThreadIDResource           CEThread::_mapThreadId;

/************************************************************************/
// Default thread procedure
/************************************************************************/
unsigned long CEThread::_defaultThreadFunction(void* data)
{
    CEThread* threadObj = reinterpret_cast<CEThread *>(data);
    IEThreadConsumer::eExitCodes result= IEThreadConsumer::EXIT_NO_PARAM;
    if (threadObj != NULL)
    {
        do 
        {
            CELock lock(threadObj->mSynchObject);
        } while (false);

        OUTPUT_DBG("Thread [ %s ] starts job...", static_cast<const char *>(threadObj->GetThreadName()));

        // instantiate thread local storage before starting running
        // it should be created in the thread context
        CEThread::_getThreadLocalStorage(threadObj);

        result = static_cast<IEThreadConsumer::eExitCodes>( threadObj->_threadEntry() );

        // delete thread local storage.
        CEThread::_getThreadLocalStorage(static_cast<CEThread *>(NULL));

        OUTPUT_DBG("Thread [ %s ] completed job with code [ %s ]", static_cast<const char *>(threadObj->GetThreadName()), IEThreadConsumer::GetString(result));

        threadObj->mWaitForExit.SetEvent();
        CEThread::SwitchThread();
    }
#ifdef  _DEBUG
    else
    {
        OUTPUT_WARN("Unknown thread completed job with code [ %s ]", IEThreadConsumer::GetString(result));
    }
#endif // _DEBUG

    return static_cast<unsigned long>(result);
}

/************************************************************************/
// Local static utility methods
/************************************************************************/
CEThreadLocalStorage* CEThread::_getThreadLocalStorage( CEThread* ownThread /*= CEThread::CURRENT_THREAD*/ )
{
    static __THREAD_LOCAL CEThreadLocalStorage* _localStorage = NULL;
    if ( ownThread == CEThread::CURRENT_THREAD )
    {
        // do nothing, the static local storage item is already instantiated
        ASSERT( _localStorage != NULL );
    }
    else if ( ownThread != NULL )
    {
        // called only once, when thread starts.
        // at that moment the thread local storage object 
        // is not initialized and instantiated yet,
        // and it should be instantiated
        ASSERT(_localStorage == NULL);
        _localStorage = DEBUG_NEW CEThreadLocalStorage( *ownThread );
    }
    else
    {
        // called when thread exists
        // the local storage elements should be removed and
        // the object should be deleted.
        ASSERT(ownThread == NULL);
        ASSERT(_localStorage != NULL);
        _localStorage->RemoveAllItems();
        delete _localStorage;
        _localStorage = NULL;
    }

    return _localStorage;
}


//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEThread::CEThread(IEThreadConsumer &threadConsumer, const char* threadName /* = NULL */)
    : CERuntimeObject   ( )

    , mThreadHandle     (CEThread::INVALID_THREAD_HANDLE)
    , mThreadId         (CEThread::INVALID_THREAD_ID)
    , mThreadAddress    (NEString::isEmpty<char>(threadName) == false ? threadName : NEUtilities::GenerateName(CEThread::DEFAULT_THREAD_PREFIX).GetBuffer())
    , mIsRunning        (static_cast<unsigned int>(0))
    , mThreadConsumer   (threadConsumer)

    , mSynchObject      ( )
    , mWaitForRun       (false, false)
    , mWaitForExit      (false, false)
    , mThreadPriority   (CEThread::PriorityUndefined)
{
    mWaitForExit.SetEvent();
}

CEThread::~CEThread( void )
{
    CleanResources();
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
CEThreadLocalStorage & CEThread::GetThreadLocalStorage( void )
{
    CEThreadLocalStorage* localStorage = CEThread::_getThreadLocalStorage(CEThread::CURRENT_THREAD);
    return (*localStorage);
}

void CEThread::SwitchThread( void )
{
    CEThread::Sleep(CEThread::WAIT_SWITCH);
}

bool CEThread::CreateThread(unsigned int waitForStartMs /* = CEThread::DO_NOT_WAIT */)
{
    bool result = false;

    do 
    {
        CELock  lock(mSynchObject);
        result = CreateSystemThread();
    } while (false);

    if ( result )
    {
        if (waitForStartMs != CEThread::DO_NOT_WAIT)
        {
            mWaitForRun.Lock(waitForStartMs);
        }
        else
        {
            CEThread::SwitchThread();   // do not wait, but switch thread
        }
    }

    return result;
}


void CEThread::ShutdownThread( void )
{
    ASSERT(false);
}

bool CEThread::CompletionWait( unsigned int waitForCompleteMs /*= CEThread::WAIT_INFINITE*/ )
{
    mSynchObject.Lock(IESynchObject::WAIT_INFINITE);

    bool result = false;
    THREADHANDLE  handle = mThreadHandle;
    if (handle != CEThread::INVALID_THREAD_HANDLE)
    {
        mSynchObject.Unlock();  // unlock, to let thread complete exit task.

        result = (waitForCompleteMs == CEThread::DO_NOT_WAIT) || mWaitForExit.Lock(waitForCompleteMs) ;
    }
    else
    {
        mSynchObject.Unlock();  // unlock, to let thread complete exit task.
        result = true;
    }
    return result;
}

bool CEThread::PreRunThread( void )
{
    return mWaitForRun.SetEvent();
}

void CEThread::PostExitThread( void )
{
    ;   // mWaitForRun.ResetEvent();
}

const char * CEThread::GetThreadName( ITEM_ID threadId )
{
    CEThread* threadObj = CEThread::FindThreadById( threadId);
    return (threadObj != NULL ? threadObj->GetThreadName().GetBuffer() : CEString::EmptyString);
}

const CEThreadAddress & CEThread::GetThreadAddress( ITEM_ID threadId )
{
    CEThread* threadObj = CEThread::FindThreadById( threadId);
    return (threadObj != NULL ? threadObj->GetThreadAddress() : CEThreadAddress::INVALID_THREAD_ADDRESS);
}

int CEThread::_threadEntry( void )
{
    int result = IEThreadConsumer::EXIT_TERMINATED;

    if (CEThread::_findThreadByHandle(mThreadHandle) != NULL)
    {
        CEThread::GetThreadLocalStorage().SetStorageItem(CEThread::STORAGE_THREAD_CONSUMER, (void *)&mThreadConsumer);

        result = IEThreadConsumer::EXIT_ERROR;

        SetRunning(true);

        if (PreRunThread())
        {
            mThreadConsumer.ThreadRuns();
        }

        SetRunning(false);

        result = mThreadConsumer.ThreadExit();
        PostExitThread();

        CEThread::GetThreadLocalStorage().RemoveStoragteItem(CEThread::STORAGE_THREAD_CONSUMER);
    }
    else
    {
        ; // do nothing
    }

    return result;
}

void CEThread::CleanResources( void )
{
    THREADHANDLE handle = mThreadHandle;

    mThreadHandle   = INVALID_THREAD_HANDLE;
    mThreadId       = INVALID_THREAD_ID;
    mIsRunning      = static_cast<unsigned int>(0);
    mThreadPriority = CEThread::PriorityUndefined;

    CEThread::_closeHandle(handle);
}

bool CEThread::RegisterThread( void )
{
    CEThread::_mapThreadhHandle.RegisterResourceObject(mThreadHandle, this);
    CEThread::_mapThreadName.RegisterResourceObject(mThreadAddress.GetThreadName(), this);
    CEThread::_mapThreadId.RegisterResourceObject(mThreadId, this);

    _setThreadName(mThreadId, mThreadAddress.GetThreadName());
    return mThreadConsumer.ThreadRegistered(this);
}

void CEThread::UnregisterThread( void )
{
    if (isValidNoLock())
    {
        mThreadConsumer.ThreadUnregistering();
        
        CEThread::_mapThreadhHandle.UnregisterResourceObject(mThreadHandle);
        CEThread::_mapThreadName.UnregisterResourceObject(mThreadAddress.GetThreadName());
        CEThread::_mapThreadId.UnregisterResourceObject(mThreadId);

        if (CEThread::_mapThreadhHandle.IsEmpty())
            CEThread::_mapThreadhHandle.RemoveAllResources();

        if (CEThread::_mapThreadName.IsEmpty())
            CEThread::_mapThreadName.RemoveAllResources();

        if (CEThread::_mapThreadId.IsEmpty())
            CEThread::_mapThreadId.RemoveAllResources();
    }
    else
    {
        ASSERT(mThreadHandle == CEThread::INVALID_THREAD_HANDLE);
    }
}

IEThreadConsumer& CEThread::GetCurrentThreadConsumer( void )
{
    ASSERT(GetCurrentThread() != NULL);
    CEThreadLocalStorage& localStorage = CEThread::GetThreadLocalStorage();
    IEThreadConsumer* consumer = reinterpret_cast<IEThreadConsumer *>(localStorage.GetStorageItem(CEThread::STORAGE_THREAD_CONSUMER).alignPtr.mElement);
    ASSERT(consumer != NULL);
    return (*consumer);
}

CEThread::eThreadPriority CEThread::GetThreadPriority( void ) const
{
    CELock  lock(mSynchObject);
    return (IsValid() ? mThreadPriority : CEThread::PriorityUndefined);
}

#ifdef  _DEBUG
/************************************************************************/
// CEThread debugging function
/************************************************************************/
void CEThread::DumpThreads( void )
{
    OUTPUT_DBG(">>> Starting dump Thread objects");
    OUTPUT_DBG(">>> ............................");

    CEThread::_mapThreadName.ResourceLock();

    if (CEThread::_mapThreadName.IsEmpty() == false)
    {
        CEString threadName("");
        CEThread* threadObj = CEThread::_mapThreadName.GetResourceFirstKey(threadName);
        do
        {
            OUTPUT_WARN("The thread with name [ %s ] is still registered in resource!", static_cast<const char *>(threadName.GetBuffer()));
            threadObj = CEThread::_mapThreadName.GetResourceNextKey(threadName);

        } while (threadObj != NULL);
    }

    CEThread::_mapThreadName.ResourceUnlock();

    OUTPUT_DBG("<<< .......................");
    OUTPUT_DBG("<<< End dump Thread objects");
}

#endif // _DEBUG
