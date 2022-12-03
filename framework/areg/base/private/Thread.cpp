/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Thread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread class
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
Thread::MapThreadHandleResource       Thread::_mapThreadhHandle;
Thread::MapThreadNameResource         Thread::_mapThreadName;
Thread::MapThreadIDResource           Thread::_mapThreadId;

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
            Lock lock(threadObj->mSynchObject);
        } while (false);

        OUTPUT_DBG("Thread [ %s ] starts job...", static_cast<const char *>(threadObj->getName().getString( )));

        // instantiate thread local storage before starting running
        // it should be created in the thread context
        Thread::_getThreadLocalStorage(threadObj);

        result = static_cast<IEThreadConsumer::eExitCodes>( threadObj->_threadEntry() );

        // delete thread local storage.
        Thread::_getThreadLocalStorage(nullptr);

        OUTPUT_DBG("Thread [ %s ] completed job with code [ %s ]"
                        , threadObj->getName().getString()
                        , IEThreadConsumer::getString( result ) );

        threadObj->mWaitForExit.setEvent();
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
ThreadLocalStorage* Thread::_getThreadLocalStorage( Thread* ownThread )
{
    static __THREAD_LOCAL ThreadLocalStorage* _localStorage = nullptr;
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

Thread::Thread(IEThreadConsumer &threadConsumer, const String & threadName )
    : RuntimeObject   ( )

    , mThreadConsumer   (threadConsumer)
    , mThreadHandle     (Thread::INVALID_THREAD_HANDLE)
    , mThreadId         (Thread::INVALID_THREAD_ID)
    , mThreadAddress    (threadName.isEmpty() == false ? threadName : NEUtilities::generateName(DEFAULT_THREAD_PREFIX.data()))
    , mThreadPriority   (Thread::eThreadPriority::PriorityUndefined)
    , mIsRunning        ( false )

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
    ThreadLocalStorage* localStorage = Thread::_getThreadLocalStorage(reinterpret_cast<Thread *>(Thread::CURRENT_THREAD));
    return (*localStorage);
}

bool Thread::createThread(unsigned int waitForStartMs /* = NECommon::DO_NOT_WAIT */)
{
    bool result = false;

    do 
    {
        Lock  lock(mSynchObject);
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


Thread::eCompletionStatus Thread::destroyThread( unsigned int waitForStopMs /* = NECommon::DO_NOT_WAIT */ )
{
    return _osDestroyThread( waitForStopMs );
}

void Thread::shutdownThread( void )
{
    destroyThread(NECommon::WAIT_INFINITE);
}

Thread::eCompletionStatus Thread::terminateThread( void )
{
    return destroyThread( NECommon::WAIT_10_MILLISECONDS );
}

bool Thread::completionWait( unsigned int waitForCompleteMs /*= NECommon::WAIT_INFINITE*/ )
{
    mSynchObject.lock(NECommon::WAIT_INFINITE);

    bool result = false;
    THREADHANDLE  handle = mThreadHandle;
    if (handle != Thread::INVALID_THREAD_HANDLE)
    {
        mSynchObject.unlock();  // unlock, to let thread complete exit task.

        result = (waitForCompleteMs == NECommon::DO_NOT_WAIT) || mWaitForExit.lock(waitForCompleteMs) ;
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
}

const String & Thread::getThreadName( id_type threadId )
{
    static String emptyString;
    Thread* threadObj = Thread::findThreadById( threadId);
    return (threadObj != nullptr ? threadObj->getName() : emptyString);
}

const ThreadAddress & Thread::getThreadAddress( id_type threadId )
{
    Thread* threadObj = Thread::findThreadById( threadId);
    return (threadObj != nullptr ? threadObj->getAddress() : ThreadAddress::getInvalidThreadAddress());
}

int Thread::_threadEntry( void )
{
    IEThreadConsumer::eExitCodes result = IEThreadConsumer::eExitCodes::ExitTerminated;

    if (Thread::_findThreadByHandle(mThreadHandle) != nullptr )
    {
        Thread::getCurrentThreadStorage().setStorageItem(STORAGE_THREAD_CONSUMER.data(), (void *)&mThreadConsumer);

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
    else
    {
        ; // do nothing
    }

    return static_cast<int>(result);
}

void Thread::_cleanResources( void )
{
    THREADHANDLE handle = mThreadHandle;

    mThreadHandle   = INVALID_THREAD_HANDLE;
    mThreadId       = INVALID_THREAD_ID;
    mIsRunning      = false;
    mThreadPriority = Thread::eThreadPriority::PriorityUndefined;

    Thread::_osCloseHandle(handle);
}

bool Thread::_registerThread( void )
{
    Thread::_mapThreadhHandle.registerResourceObject(mThreadHandle, this);
    Thread::_mapThreadName.registerResourceObject(mThreadAddress.getThreadName(), this);
    Thread::_mapThreadId.registerResourceObject(mThreadId, this);

    _osSetThreadName(mThreadId, mThreadAddress.getThreadName());
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
    ASSERT(getCurrentThread() != nullptr );
    ThreadLocalStorage& localStorage = Thread::getCurrentThreadStorage();
    IEThreadConsumer* consumer = reinterpret_cast<IEThreadConsumer *>(localStorage.getStorageItem(STORAGE_THREAD_CONSUMER).alignPtr.mElement);
    ASSERT(consumer != nullptr );
    return (*consumer);
}

Thread * Thread::getFirstThread( id_type & OUT threadId )
{
    return _mapThreadId.resourceFirstKey( threadId );
}

Thread * Thread::getNextThread( id_type & IN OUT threadId )
{
    return _mapThreadId.resourceNextKey( threadId );
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
        Thread* threadObj{ nullptr };
        Thread::_mapThreadName.resourceFirstKey(threadName);
        do
        {
            OUTPUT_WARN("The thread with name [ %s ] is still registered in resource!", static_cast<const char *>(threadName.getString()));
            threadObj = Thread::_mapThreadName.resourceNextKey(threadName);

        } while (threadObj != nullptr );
    }

    Thread::_mapThreadName.unlock();

    OUTPUT_DBG("<<< .......................");
    OUTPUT_DBG("<<< End dump Thread objects");
}

#endif // _DEBUG
