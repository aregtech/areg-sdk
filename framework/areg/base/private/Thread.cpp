/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Thread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread class
 *              platform independent code
 *
 ************************************************************************/
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/base/ThreadLocalStorage.hpp"

namespace
{
/**
 * \brief   Internal thread naming prefix. Used to generate unique thread name.
 **/
constexpr std::string_view   DEFAULT_THREAD_PREFIX   { "_AREG_thread_" };

/**
 * \brief   The name of entry in Thread Local Storage to save pointer of Thread Consumer.
 **/
constexpr std::string_view   STORAGE_THREAD_CONSUMER { "ThreadConsumer" };

}

//////////////////////////////////////////////////////////////////////////
// Thread class constants definition
//////////////////////////////////////////////////////////////////////////

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
Thread::MapThreadHandleResource& Thread::_getMapThreadhHandle()
{
    static Thread::MapThreadHandleResource _mapThreadhHandle;
    return _mapThreadhHandle;
}

Thread::MapThreadNameResource& Thread::_getMapThreadName()
{
    static Thread::MapThreadNameResource _mapThreadName;
    return _mapThreadName;
}

Thread::MapThreadIDResource& Thread::_getMapThreadId()
{
    static Thread::MapThreadIDResource _mapThreadId;
    return _mapThreadId;
}

/************************************************************************/
// Default thread procedure
/************************************************************************/
unsigned long Thread::_defaultThreadFunction(void* data)
{
    Thread* threadObj = reinterpret_cast<Thread *>(data);
    IEThreadConsumer::eExitCodes result= IEThreadConsumer::eExitCodes::ExitNoParam;
    if (threadObj != nullptr)
    {
        do 
        {
            // Check if initialization is completed and ready to run.
            Lock lock(threadObj->mSyncObject);
        } while (false);

        // instantiate thread local storage before starting running
        // it should be created in the thread context
        Thread::_getThreadLocalStorage(threadObj);

        result = static_cast<IEThreadConsumer::eExitCodes>( threadObj->_threadEntry() );

        // delete thread local storage.
        Thread::_getThreadLocalStorage(nullptr);
        threadObj->mWaitForExit.setEvent();
    }

    return static_cast<unsigned long>(result);
}

/************************************************************************/
// Local static utility methods
/************************************************************************/
ThreadLocalStorage* Thread::_getThreadLocalStorage( Thread* ownThread )
{
    static __THREAD_LOCAL ThreadLocalStorage* _localStorage {nullptr};
    if ( ownThread == reinterpret_cast<Thread *>(Thread::CURRENT_THREAD) )
    {
        // do nothing, the static local storage item is already instantiated
        ASSERT( _localStorage != nullptr );
    }
    else if ( ownThread != nullptr )
    {
        // called only once, when thread starts.
        // at that moment the thread local storage object 
        // is not initialized and instantiated yet,
        // and it should be instantiated
        ASSERT(_localStorage == nullptr );
        _localStorage = DEBUG_NEW ThreadLocalStorage( *ownThread );
    }
    else
    {
        // called when thread exists
        // the local storage elements should be removed and
        // the object should be deleted.
        ASSERT(ownThread == nullptr );
        ASSERT(_localStorage != nullptr );
        _localStorage->clear();
        delete _localStorage;
        _localStorage = nullptr;
    }

    return _localStorage;
}


//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

Thread::Thread(IEThreadConsumer &threadConsumer, const String & threadName, uint32_t stackSizeKb /*= NECommon::STACK_SIZE_DEFAULT*/)
    : RuntimeObject   ( )

    , mThreadConsumer   (threadConsumer)
    , mThreadHandle     (Thread::INVALID_THREAD_HANDLE)
    , mThreadId         (Thread::INVALID_THREAD_ID)
    , mThreadAddress    (threadName.isEmpty() == false ? threadName : NEUtilities::generateName(DEFAULT_THREAD_PREFIX.data()))
    , mThreadPriority   (Thread::eThreadPriority::PriorityUndefined)
    , mIsRunning        ( false )
    , mStackSizeKB      ( stackSizeKb )

    , mSyncObject       ( )
    , mWaitForRun       (false, false)
    , mWaitForExit      (false, false)
{
    mWaitForExit.setEvent();
}

Thread::~Thread( void )
{
    _cleanResources(false);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
ThreadLocalStorage & Thread::getCurrentThreadStorage( void )
{
    ThreadLocalStorage* localStorage = Thread::_getThreadLocalStorage(reinterpret_cast<Thread *>(Thread::CURRENT_THREAD));
    return (*localStorage);
}

bool Thread::createThread(unsigned int waitForStartMs /* = NECommon::DO_NOT_WAIT */)
{
    bool result = false;

    do 
    {
        Lock  lock(mSyncObject);
        result = _osCreateSystemThread();
    } while (false);

    if ( result )
    {
        if (waitForStartMs != NECommon::DO_NOT_WAIT)
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

void Thread::triggerExit( void )
{
}

Thread::eCompletionStatus Thread::shutdownThread( unsigned int waitForStopMs /* = NECommon::DO_NOT_WAIT */ )
{
    Thread::eCompletionStatus result{ _osDestroyThread( waitForStopMs ) };

    if ( mSyncObject.tryLock( ) )
    {
        _cleanResources( true );
        mSyncObject.unlock( );
    }

    return result;
}

Thread::eCompletionStatus Thread::terminateThread( void )
{
    return shutdownThread( NECommon::WAIT_10_MILLISECONDS );
}

bool Thread::completionWait( unsigned int waitForCompleteMs /*= NECommon::WAIT_INFINITE*/ )
{
    mSyncObject.lock(NECommon::WAIT_INFINITE);

    bool result = false;
    THREADHANDLE  handle = mThreadHandle;
    if (handle != Thread::INVALID_THREAD_HANDLE)
    {
        mSyncObject.unlock();  // unlock, to let thread complete exit task.

        result = (waitForCompleteMs == NECommon::DO_NOT_WAIT) || mWaitForExit.lock(waitForCompleteMs) ;
    }
    else
    {
        mSyncObject.unlock();  // unlock, to let thread complete exit task.
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
}

const String & Thread::getThreadName( id_type threadId )
{
    Thread* threadObj = Thread::findThreadById( threadId);
    return (threadObj != nullptr ? threadObj->getName() : String::getEmptyString());
}

const ThreadAddress & Thread::getThreadAddress( id_type threadId )
{
    Thread* threadObj = Thread::findThreadById( threadId);
    return (threadObj != nullptr ? threadObj->getAddress() : ThreadAddress::getInvalidThreadAddress());
}

const size_t Thread::getCurrentStackSize(void)
{
    Thread* threadObj = Thread::getCurrentThread();
    return (threadObj != nullptr ? _osGetCurrentStackSize(threadObj->mThreadHandle) : 0);
}

int Thread::_threadEntry( void )
{
    IEThreadConsumer::eExitCodes result = IEThreadConsumer::eExitCodes::ExitTerminated;

    if (Thread::_findThreadByHandle(mThreadHandle) != nullptr )
    {
        Thread::getCurrentThreadStorage().setStorageItem(STORAGE_THREAD_CONSUMER.data(), reinterpret_cast<void *>(&mThreadConsumer));

        _setRunning(true);

        if (onPreRunThread())
        {
            mThreadConsumer.onThreadRuns();
        }

        _setRunning(false);

        result = static_cast<IEThreadConsumer::eExitCodes>(mThreadConsumer.onThreadExit());
        onPostExitThread();

        Thread::getCurrentThreadStorage().removeStoragteItem(STORAGE_THREAD_CONSUMER.data());
    }

    _cleanResources( true );

    return static_cast<int>(result);
}

void Thread::_cleanResources(bool unregister)
{
    Lock lock(mSyncObject);

    if (unregister)
    {
        _unregisterThread();
    }

    THREADHANDLE handle{ mThreadHandle };
    mThreadHandle   = Thread::INVALID_THREAD_HANDLE;
    mThreadId       = Thread::INVALID_THREAD_ID;
    mIsRunning      = false;
    mThreadPriority = Thread::eThreadPriority::PriorityUndefined;

    Thread::_osCloseHandle(handle);
}

bool Thread::_registerThread( void )
{
    Thread::_getMapThreadhHandle().registerResourceObject(mThreadHandle, this);
    Thread::_getMapThreadName().registerResourceObject(mThreadAddress.getThreadName(), this);
    Thread::_getMapThreadId().registerResourceObject(mThreadId, this);

    _osSetThreadName(mThreadId, mThreadAddress.getThreadName());
    return mThreadConsumer.onThreadRegistered(this);
}

void Thread::_unregisterThread( void )
{
    if (_isValidNoLock())
    {
        mThreadConsumer.onThreadUnregistering();
        
        Thread::_getMapThreadhHandle().unregisterResourceObject(mThreadHandle);
        Thread::_getMapThreadName().unregisterResourceObject(mThreadAddress.getThreadName());
        Thread::_getMapThreadId().unregisterResourceObject(mThreadId);

        if (Thread::_getMapThreadhHandle().isEmpty())
        {
            Thread::_getMapThreadhHandle().removeAllResources();
        }

        if (Thread::_getMapThreadName().isEmpty())
        {
            Thread::_getMapThreadName().removeAllResources();
        }

        if (Thread::_getMapThreadId().isEmpty())
        {
            Thread::_getMapThreadId().removeAllResources();
        }
    }
    else
    {
        ASSERT(mThreadHandle == Thread::INVALID_THREAD_HANDLE);
    }
}

IEThreadConsumer& Thread::getCurrentThreadConsumer( void )
{
    ASSERT(getCurrentThread() != nullptr );
    ThreadLocalStorage& localStorage = Thread::getCurrentThreadStorage();
    IEThreadConsumer* consumer = reinterpret_cast<IEThreadConsumer *>(localStorage.getStorageItem(STORAGE_THREAD_CONSUMER).alignPtr.mElement);
    ASSERT(consumer != nullptr );
    return (*consumer);
}

Thread * Thread::getFirstThread( id_type & OUT threadId )
{
    return _getMapThreadId().resourceFirstKey( threadId );
}

Thread * Thread::getNextThread( id_type & IN OUT threadId )
{
    return _getMapThreadId().resourceNextKey( threadId );
}

#ifdef  _DEBUG
/************************************************************************/
// Thread debugging function
/************************************************************************/
void Thread::dumpThreads( void )
{
    Thread::MapThreadNameResource& mapNames{ Thread::_getMapThreadName() };
    mapNames.lock();

    if (mapNames.isEmpty() == false)
    {
        String threadName("");
        Thread* threadObj{ nullptr };
        mapNames.resourceFirstKey(threadName);
        do
        {
            threadObj = mapNames.resourceNextKey(threadName);
        } while (threadObj != nullptr );
    }

    mapNames.unlock();
}

#endif // _DEBUG
