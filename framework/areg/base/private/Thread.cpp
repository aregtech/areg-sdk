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
#include "areg/base/ThreadConsumer.hpp"
#include "areg/base/ThreadLocalStorage.hpp"
namespace areg {

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
AREG_IMPLEMENT_RUNTIME(Thread, RuntimeObject)

/************************************************************************/
// Define internal static mapping objects
/************************************************************************/
Thread::MapThreadHandleResource& Thread::_map_threadh_handle() noexcept
{
    static Thread::MapThreadHandleResource _mapThreadhHandle;
    return _mapThreadhHandle;
}

Thread::MapThreadNameResource& Thread::_map_thread_name() noexcept
{
    static Thread::MapThreadNameResource _mapThreadName;
    return _mapThreadName;
}

Thread::MapThreadIDResource& Thread::_map_thread_id() noexcept
{
    static Thread::MapThreadIDResource _mapThreadId;
    return _mapThreadId;
}

/************************************************************************/
// Default thread procedure
/************************************************************************/
unsigned long Thread::_default_thread_function(void* data)
{
    Thread* threadObj = reinterpret_cast<Thread *>(data);
    ThreadConsumer::ExitCode result= ThreadConsumer::ExitCode::NoParam;
    if (threadObj != nullptr)
    {
        do 
        {
            // Check if initialization is completed and ready to run.
            Lock lock(threadObj->mSyncObject);
        } while (false);

        // instantiate thread local storage before starting running
        // it should be created in the thread context
        Thread::_thread_local_storage(threadObj);

        result = static_cast<ThreadConsumer::ExitCode>( threadObj->_thread_entry() );

        // delete thread local storage.
        Thread::_thread_local_storage(nullptr);
        threadObj->mWaitForExit.set_signaled();
    }

    return static_cast<unsigned long>(result);
}

/************************************************************************/
// Local static utility methods
/************************************************************************/
ThreadLocalStorage* Thread::_thread_local_storage( Thread* ownThread )
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

Thread::Thread(ThreadConsumer &threadConsumer, const String & threadName, uint32_t stackSizeKb /*= areg::STACK_SIZE_DEFAULT*/)
    : RuntimeObject   ( )

    , mThreadConsumer   (threadConsumer)
    , mThreadHandle     (Thread::INVALID_THREAD_HANDLE)
    , mThreadId         (Thread::INVALID_THREAD_ID)
    , mThreadAddress    (threadName.is_empty() == false ? threadName : areg::generate_name(DEFAULT_THREAD_PREFIX.data()))
    , mThreadPriority   (Thread::ThreadPriority::Undefined)
    , mIsRunning        ( false )
    , mStackSizeKB      ( stackSizeKb )

    , mSyncObject       ( )
    , mWaitForRun       (false, false)
    , mWaitForExit      (false, false)
{
    mWaitForExit.set_signaled();
}

Thread::~Thread()
{
    _clean_resources(false);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
ThreadLocalStorage & Thread::current_thread_storage() noexcept
{
    ThreadLocalStorage* localStorage = Thread::_thread_local_storage(reinterpret_cast<Thread *>(Thread::CURRENT_THREAD));
    return (*localStorage);
}

bool Thread::start(uint32_t waitForStartMs /* = areg::DO_NOT_WAIT */)
{
    bool result = false;

    do 
    {
        Lock  lock(mSyncObject);
        result = _os_create();
    } while (false);

    if ( result )
    {
        if (waitForStartMs != areg::DO_NOT_WAIT)
        {
            mWaitForRun.lock(waitForStartMs);
        }
        else
        {
            Thread::switch_thread();   // do not wait, but switch thread
        }
    }

    return result;
}

void Thread::trigger_exit()
{
}

Thread::ThreadCompletion Thread::shutdown( uint32_t waitForStopMs /* = areg::DO_NOT_WAIT */ )
{
    Thread::ThreadCompletion result{ _os_destroy_thread( waitForStopMs ) };

    if ( mSyncObject.try_lock( ) )
    {
        _clean_resources( true );
        mSyncObject.unlock( );
    }

    return result;
}

Thread::ThreadCompletion Thread::terminate()
{
    return shutdown( areg::WAIT_10_MILLISECONDS );
}

bool Thread::wait_completion( uint32_t waitForCompleteMs /*= areg::WAIT_INFINITE*/ )
{
    mSyncObject.lock(areg::WAIT_INFINITE);

    bool result = false;
    THREADHANDLE  handle = mThreadHandle;
    if (handle != Thread::INVALID_THREAD_HANDLE)
    {
        mSyncObject.unlock();  // unlock, to let thread complete exit task.

        result = (waitForCompleteMs == areg::DO_NOT_WAIT) || mWaitForExit.lock(waitForCompleteMs) ;
    }
    else
    {
        mSyncObject.unlock();  // unlock, to let thread complete exit task.
        result = true;
    }
    return result;
}

bool Thread::on_pre_run()
{
    return mWaitForRun.set_signaled();
}

void Thread::on_post_exit()
{
}

const String & Thread::thread_name( id_type threadId ) noexcept
{
    Thread* threadObj = Thread::find_by_id( threadId);
    return (threadObj != nullptr ? threadObj->name() : String::empty_string());
}

const ThreadAddress & Thread::thread_address( id_type threadId ) noexcept
{
    Thread* threadObj = Thread::find_by_id( threadId);
    return (threadObj != nullptr ? threadObj->address() : ThreadAddress::invalid_thread_address());
}

size_t Thread::current_stack_size() noexcept
{
    Thread* threadObj = Thread::current_thread();
    return (threadObj != nullptr ? _os_stack_size(threadObj->mThreadHandle) : 0);
}

int32_t Thread::_thread_entry()
{
    ThreadConsumer::ExitCode result = ThreadConsumer::ExitCode::Terminated;

    if (Thread::_find_by_handle(mThreadHandle) != nullptr )
    {
        Thread::current_thread_storage().set_item(STORAGE_THREAD_CONSUMER.data(), reinterpret_cast<void *>(&mThreadConsumer));

        _set_running(true);

        if (on_pre_run())
        {
            mThreadConsumer.on_run();
        }

        _set_running(false);

        result = static_cast<ThreadConsumer::ExitCode>(mThreadConsumer.on_exit());
        on_post_exit();

        Thread::current_thread_storage().remove_item(STORAGE_THREAD_CONSUMER.data());
    }

    _clean_resources( true );
    return static_cast<int32_t>(result);
}

void Thread::_clean_resources(bool unregister)
{
    THREADHANDLE handle{ Thread::INVALID_THREAD_HANDLE };

    do
    {
        Lock lock(mSyncObject);

        if (unregister)
        {
            _unregister_thread();
        }

        handle          = mThreadHandle;
        mThreadHandle   = Thread::INVALID_THREAD_HANDLE;
        mThreadId       = Thread::INVALID_THREAD_ID;
        mIsRunning      = false;
        mThreadPriority = Thread::ThreadPriority::Undefined;
    } while (false);

    Thread::_os_close_handle(handle);
}

bool Thread::_register_thread()
{
    Thread::_map_threadh_handle().register_resource_object(mThreadHandle, this);
    Thread::_map_thread_name().register_resource_object(mThreadAddress.name(), this);
    Thread::_map_thread_id().register_resource_object(mThreadId, this);

    _os_set_name(mThreadId, mThreadAddress.name());
    return mThreadConsumer.on_thread_registered(this);
}

void Thread::_unregister_thread()
{
    if (_is_valid_no_lock())
    {
        mThreadConsumer.on_thread_unregistering();
        
        Thread::_map_threadh_handle().unregister_resource_object(mThreadHandle);
        Thread::_map_thread_name().unregister_resource_object(mThreadAddress.name());
        Thread::_map_thread_id().unregister_resource_object(mThreadId);

        if (Thread::_map_threadh_handle().is_empty())
        {
            Thread::_map_threadh_handle().remove_all_resources();
        }

        if (Thread::_map_thread_name().is_empty())
        {
            Thread::_map_thread_name().remove_all_resources();
        }

        if (Thread::_map_thread_id().is_empty())
        {
            Thread::_map_thread_id().remove_all_resources();
        }
    }
    else
    {
        ASSERT(mThreadHandle == Thread::INVALID_THREAD_HANDLE);
    }
}

ThreadConsumer& Thread::current_thread_consumer() noexcept
{
    ASSERT(current_thread() != nullptr );
    ThreadLocalStorage& localStorage = Thread::current_thread_storage();
    ThreadConsumer* consumer = reinterpret_cast<ThreadConsumer *>(localStorage.item(STORAGE_THREAD_CONSUMER).valPtr.mElement);
    ASSERT(consumer != nullptr );
    return (*consumer);
}

Thread * Thread::first_thread( id_type & threadId ) noexcept
{
    return _map_thread_id().resource_first_key( threadId );
}

Thread * Thread::next_thread( id_type & threadId ) noexcept
{
    return _map_thread_id().resource_next_key( threadId );
}

#ifdef  _DEBUG
/************************************************************************/
// Thread debugging function
/************************************************************************/
void Thread::dump_threads()
{
    Thread::MapThreadNameResource& mapNames{ Thread::_map_thread_name() };
    mapNames.lock();

    if (mapNames.is_empty() == false)
    {
        String threadName("");
        Thread* threadObj{ nullptr };
        mapNames.resource_first_key(threadName);
        do
        {
            threadObj = mapNames.resource_next_key(threadName);
        } while (threadObj != nullptr );
    }

    mapNames.unlock();
}

#endif // _DEBUG

} // namespace areg
