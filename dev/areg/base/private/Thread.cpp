/************************************************************************
 * \file        areg/base/private/Thread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Thread class
 *              platform independent code
 *
 ************************************************************************/
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/base/ThreadLocalStorage.hpp"

//////////////////////////////////////////////////////////////////////////
// Thread class constants definition
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Thread class public constants, types and enum
/************************************************************************/
/**
 * \brief   Invalid thread ID.
 **/
const ITEM_ID           Thread::INVALID_THREAD_ID           = static_cast<unsigned int>(0);    /*0x00000000*/

/************************************************************************/
// Thread class private constants, types and enum
/************************************************************************/
/**
 * \brief   Thread invalid handle
 **/
THREADHANDLE const  Thread::INVALID_THREAD_HANDLE       = static_cast<THREADHANDLE>(NULL);
/**
 * \brief   Internal thread naming prefix. Used to generate unique thread name.
 **/
const char* const   Thread::DEFAULT_THREAD_PREFIX       = static_cast<const char *>("_AREG_Thread_");
/**
 * \brief   Identified Current Thread. Used for Local Storage
 **/
Thread* const       Thread::CURRENT_THREAD              = reinterpret_cast<Thread *>(~0); /*0xFFFFFFFF*/
/**
 * \brief   The name of entry in Thread Local Storage to save pointer of Thread Consumer.
 **/
const char* const   Thread::STORAGE_THREAD_CONSUMER     = "ThreadConsumer";
/**
 * \brief   MS Exception value, used to set thread name.
 **/
const unsigned int  Thread::SET_NAME_MS_VC_EXCEPTION    = static_cast<unsigned int>(0x406D1388);

/************************************************************************/
// Thread class
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// Thread class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Implement runtime
/************************************************************************/
IMPLEMENT_RUNTIME(Thread, RuntimeObject)

/************************************************************************/
// Define internal static mapping objects
/************************************************************************/
Thread::MapThreadHandleResource       Thread::_mapThreadhHandle;
Thread::MapThreadNameResource         Thread::_mapThreadName;
Thread::MapThreadIDResource           Thread::_mapThreadId;

/************************************************************************/
// Default thread procedure
/************************************************************************/
unsigned long Thread::_defaultThreadFunction(void* data)
{
    Thread* threadObj = reinterpret_cast<Thread *>(data);
    IEThreadConsumer::eExitCodes result= IEThreadConsumer::EXIT_NO_PARAM;
    if (threadObj != NULL)
    {
        do 
        {
            Lock lock(threadObj->mSynchObject);
        } while (false);

        OUTPUT_DBG("Thread [ %s ] starts job...", static_cast<const char *>(threadObj->getName()));

        // instantiate thread local storage before starting running
        // it should be created in the thread context
        Thread::_getThreadLocalStorage(threadObj);

        result = static_cast<IEThreadConsumer::eExitCodes>( threadObj->_threadEntry() );

        // delete thread local storage.
        Thread::_getThreadLocalStorage(static_cast<Thread *>(NULL));

        OUTPUT_DBG("Thread [ %s ] completed job with code [ %s ]", static_cast<const char *>(threadObj->getName()), IEThreadConsumer::getString(result));

        threadObj->mWaitForExit.setEvent();
        // Thread::switchThread();
    }
#ifdef  _DEBUG
    else
    {
        OUTPUT_WARN("Unknown thread completed job with code [ %s ]", IEThreadConsumer::getString(result));
    }
#endif // _DEBUG

    return static_cast<unsigned long>(result);
}

/************************************************************************/
// Local static utility methods
/************************************************************************/
ThreadLocalStorage* Thread::_getThreadLocalStorage( Thread* ownThread /*= Thread::CURRENT_THREAD*/ )
{
    static __THREAD_LOCAL ThreadLocalStorage* _localStorage = NULL;
    if ( ownThread == Thread::CURRENT_THREAD )
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
        _localStorage = DEBUG_NEW ThreadLocalStorage( *ownThread );
    }
    else
    {
        // called when thread exists
        // the local storage elements should be removed and
        // the object should be deleted.
        ASSERT(ownThread == NULL);
        ASSERT(_localStorage != NULL);
        _localStorage->removeAll();
        delete _localStorage;
        _localStorage = NULL;
    }

    return _localStorage;
}


//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Thread::Thread(IEThreadConsumer &threadConsumer, const char* threadName /* = NULL */)
    : RuntimeObject   ( )

    , mThreadConsumer   (threadConsumer)
    , mThreadHandle     (Thread::INVALID_THREAD_HANDLE)
    , mThreadId         (Thread::INVALID_THREAD_ID)
    , mThreadAddress    (NEString::isEmpty<char>(threadName) == false ? threadName : NEUtilities::generateName(Thread::DEFAULT_THREAD_PREFIX).getString())
    , mThreadPriority   (Thread::PriorityUndefined)
    , mIsRunning        (static_cast<unsigned int>(0))

    , mSynchObject      ( )
    , mWaitForRun       (false, false)
    , mWaitForExit      (false, false)
{
    mWaitForExit.setEvent();
}

Thread::~Thread( void )
{
    _cleanResources();
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
ThreadLocalStorage & Thread::getCurrentThreadStorage( void )
{
    ThreadLocalStorage* localStorage = Thread::_getThreadLocalStorage(Thread::CURRENT_THREAD);
    return (*localStorage);
}

void Thread::switchThread( void )
{
    Thread::sleep(Thread::WAIT_SWITCH);
}

bool Thread::createThread(unsigned int waitForStartMs /* = Thread::DO_NOT_WAIT */)
{
    bool result = false;

    do 
    {
        Lock  lock(mSynchObject);
        result = _createSystemThread();
    } while (false);

    if ( result )
    {
        if (waitForStartMs != Thread::DO_NOT_WAIT)
        {
            mWaitForRun.lock(waitForStartMs);
        }
        else
        {
            Thread::switchThread();   // do not wait, but switch thread
        }
    }

    return result;
}


void Thread::shutdownThread( void )
{
    destroyThread(Thread::WAIT_INFINITE);
}

bool Thread::completionWait( unsigned int waitForCompleteMs /*= Thread::WAIT_INFINITE*/ )
{
    mSynchObject.lock(IESynchObject::WAIT_INFINITE);

    bool result = false;
    THREADHANDLE  handle = mThreadHandle;
    if (handle != Thread::INVALID_THREAD_HANDLE)
    {
        mSynchObject.unlock();  // unlock, to let thread complete exit task.

        result = (waitForCompleteMs == Thread::DO_NOT_WAIT) || mWaitForExit.lock(waitForCompleteMs) ;
    }
    else
    {
        mSynchObject.unlock();  // unlock, to let thread complete exit task.
        result = true;
    }
    return result;
}

bool Thread::onPreRunThread( void )
{
    return mWaitForRun.setEvent();
}

void Thread::onPostExitThread( void )
{
    ;   // mWaitForRun.ResetEvent();
}

const char * Thread::getThreadName( ITEM_ID threadId )
{
    Thread* threadObj = Thread::findThreadById( threadId);
    return (threadObj != NULL ? threadObj->getName().getString() : String::EmptyString);
}

const ThreadAddress & Thread::getThreadAddress( ITEM_ID threadId )
{
    Thread* threadObj = Thread::findThreadById( threadId);
    return (threadObj != NULL ? threadObj->getAddress() : ThreadAddress::INVALID_THREAD_ADDRESS);
}

int Thread::_threadEntry( void )
{
    int result = IEThreadConsumer::EXIT_TERMINATED;

    if (Thread::_findThreadByHandle(mThreadHandle) != NULL)
    {
        Thread::getCurrentThreadStorage().setStorageItem(Thread::STORAGE_THREAD_CONSUMER, (void *)&mThreadConsumer);

        result = IEThreadConsumer::EXIT_ERROR;

        _setRunning(true);

        if (onPreRunThread())
        {
            mThreadConsumer.onThreadRuns();
        }

        _setRunning(false);

        result = mThreadConsumer.onThreadExit();
        onPostExitThread();

        Thread::getCurrentThreadStorage().removeStoragteItem(Thread::STORAGE_THREAD_CONSUMER);
    }
    else
    {
        ; // do nothing
    }

    return result;
}

void Thread::_cleanResources( void )
{
    THREADHANDLE handle = mThreadHandle;

    mThreadHandle   = INVALID_THREAD_HANDLE;
    mThreadId       = INVALID_THREAD_ID;
    mIsRunning      = static_cast<unsigned int>(0);
    mThreadPriority = Thread::PriorityUndefined;

    Thread::_closeHandle(handle);
}

bool Thread::_registerThread( void )
{
    Thread::_mapThreadhHandle.registerResourceObject(mThreadHandle, this);
    Thread::_mapThreadName.registerResourceObject(mThreadAddress.getThreadName(), this);
    Thread::_mapThreadId.registerResourceObject(mThreadId, this);

    _setThreadName(mThreadId, mThreadAddress.getThreadName());
    return mThreadConsumer.onThreadRegistered(this);
}

void Thread::_unregisterThread( void )
{
    if (_isValidNoLock())
    {
        mThreadConsumer.onThreadUnregistering();
        
        Thread::_mapThreadhHandle.unregisterResourceObject(mThreadHandle);
        Thread::_mapThreadName.unregisterResourceObject(mThreadAddress.getThreadName());
        Thread::_mapThreadId.unregisterResourceObject(mThreadId);

        if (Thread::_mapThreadhHandle.isEmpty())
            Thread::_mapThreadhHandle.removeAllResources();

        if (Thread::_mapThreadName.isEmpty())
            Thread::_mapThreadName.removeAllResources();

        if (Thread::_mapThreadId.isEmpty())
            Thread::_mapThreadId.removeAllResources();
    }
    else
    {
        ASSERT(mThreadHandle == Thread::INVALID_THREAD_HANDLE);
    }
}

IEThreadConsumer& Thread::getCurrentThreadConsumer( void )
{
    ASSERT(getCurrentThread() != NULL);
    ThreadLocalStorage& localStorage = Thread::getCurrentThreadStorage();
    IEThreadConsumer* consumer = reinterpret_cast<IEThreadConsumer *>(localStorage.getStorageItem(Thread::STORAGE_THREAD_CONSUMER).alignPtr.mElement);
    ASSERT(consumer != NULL);
    return (*consumer);
}

Thread::eThreadPriority Thread::getPriority( void ) const
{
    Lock  lock(mSynchObject);
    return (isValid() ? mThreadPriority : Thread::PriorityUndefined);
}

#ifdef  _DEBUG
/************************************************************************/
// Thread debugging function
/************************************************************************/
void Thread::dumpThreads( void )
{
    OUTPUT_DBG(">>> Starting dump Thread objects");
    OUTPUT_DBG(">>> ............................");

    Thread::_mapThreadName.lock();

    if (Thread::_mapThreadName.isEmpty() == false)
    {
        String threadName("");
        Thread* threadObj = Thread::_mapThreadName.resourceFirstKey(threadName);
        do
        {
            OUTPUT_WARN("The thread with name [ %s ] is still registered in resource!", static_cast<const char *>(threadName.getString()));
            threadObj = Thread::_mapThreadName.resourceNextKey(threadName);

        } while (threadObj != NULL);
    }

    Thread::_mapThreadName.unlock();

    OUTPUT_DBG("<<< .......................");
    OUTPUT_DBG("<<< End dump Thread objects");
}

#endif // _DEBUG
