#ifndef AREG_BASE_THREAD_HPP
#define AREG_BASE_THREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/Thread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Thread class
 *              Base class for all kind of threads. Use this class or
 *              any derived class if need to access class object
 *              by name, ID or handle.
 *              Every thread as well contains unique thread address.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/RuntimeObject.hpp"

#include "areg/base/CommonDefs.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/ResourceMap.hpp"
#include "areg/base/String.hpp"

#include <string_view>
#include <limits>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class ThreadLocalStorage;
    class ThreadConsumer;
    class InStream;
    class String;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Thread class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A base class of all thread objects running in the system. Use this or derived classes to
 *          keep tracking of instantiated and running threads that can be accessed by ID, unique
 *          names or handles. The class creates (starts) and destroys (stops) threads. To implement
 *          cyclic runs, provide a ThreadConsumer instance. Provides thread-specific storage
 *          accessible only within the thread context. Derived classes include Dispatcher, Worker,
 *          and Component threads that process thread-specific events.
 **/
class AREG_API Thread  : public RuntimeObject
{
/************************************************************************/
// Internal class declaration
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
// class Thread internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Public constants, types and enum
/************************************************************************/

    /**
     * \brief   Thread::ThreadCompletion
     *          Thread Completion status used as return value on destroy thread
     **/
    enum class ThreadCompletion : int32_t
    {
          Terminated = -1   //!< The thread was terminate because waiting timeout expired
        , Completed  = 0    //!< The thread was valid and normally completed
        , Invalid    = 1    //!< The thread handle is invalid and is not running, nothing to do
    };

    /**
     * \brief   Thread::ThreadPriority
     *          Defines the thread priorities.
     *          By default, the thread is created by Normal priority.
     *          Increase or decrease thread priority if need.
     **/
    enum class ThreadPriority : int32_t
    {
          Undefined = std::numeric_limits<int32_t>::min()    //!< Undefined priority. If thread is created and not valid
        , Lowest    = -2    //!< Lowest priority level
        , Low       = -1    //!< Below normal priority level
        , Normal    =  0    //!< Normal priority level. All threads by default are created with normal priority level
        , High      =  1    //!< Above normal priority level
        , Highest   =  2    //!< Highest priority level
    };

    [[nodiscard]]
    inline static constexpr const char * as_string( Thread::ThreadPriority threadPriority ) noexcept;

    /**
     * \brief   Thread::INVALID_THREAD_ID
     *          Invalid thread ID.
     **/
    static constexpr id_type            INVALID_THREAD_ID       { 0 };

private:
/************************************************************************/
// Private constants
/************************************************************************/
    /**
     * \brief   Thread::INVALID_THREAD_HANDLE
     *          Thread invalid handle
     **/
    static constexpr THREADHANDLE       INVALID_THREAD_HANDLE   { nullptr };

    /**
     * \brief   Thread::CURRENT_THREAD
     *          Identified Current Thread. Used for Local Storage
     **/
    static constexpr ptr_type           CURRENT_THREAD          { static_cast<ptr_type>(std::numeric_limits<ptr_type>::max()) };

//////////////////////////////////////////////////////////////////////////
// Declare Thread as runtime object
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME(Thread)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes Thread object with a thread consumer and optional thread name and stack
     *          size.
     *
     * \param   threadConsumer      The valid instance of thread consumer object.
     * \param   threadName          The thread name to set and track in the system. Should be unique
     *                              to enable thread tracking by name. If nullptr or duplicated, the
     *                              system cannot track the thread by name.
     * \param   stackSizeKb         The stack size of the thread in kilobytes (1 KB = 1024 Bytes).
     *                              Pass `areg::DEFAULT_STACK_SIZE` (0) to use the system
     *                              default stack size.
     **/
    Thread( ThreadConsumer & threadConsumer, const String & threadName, uint32_t stackSizeKb = areg::DEFAULT_STACK_SIZE);

    virtual ~Thread();

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief   Creates and starts the thread if it has not already been created. If thread was
     *          already created, no action is performed.
     *
     * \param   waitForStartMs      Waiting timeout in milliseconds until thread is created and
     *                              running. Set DO_NOT_WAIT for immediate return without guarantee
     *                              of thread running. Set WAIT_INFINITE to ensure thread is
     *                              running. Set other values in milliseconds for specific timeout.
     * \return  Returns true if new thread is successfully created and started.
     **/
    virtual bool start( uint32_t waitForStartMs = areg::DO_NOT_WAIT );

    /**
     * \brief   Triggers exit event for the thread.
     **/
    virtual void trigger_exit();

    /**
     * \brief   Shuts down the thread and frees resources. If waiting timeout expires, the function
     *          terminates the thread. A shutdown thread can be re-created. The calling thread may
     *          be blocked until the target thread completes.
     *
     * \param   waitForStopMs       Waiting timeout in milliseconds until target thread finishes.
     *                              Set DO_NOT_WAIT to trigger exit and return immediately. Set
     *                              WAIT_INFINITE to trigger exit and wait until thread completes.
     *                              Set other values in milliseconds for specific timeout.
     * \return  Thread completion status: Terminated if waiting timeout expired and thread was
     *          terminated; Completed if thread completed normally; Invalid if thread was not valid
     *          and not running.
     **/
    virtual Thread::ThreadCompletion shutdown( uint32_t waitForStopMs = areg::DO_NOT_WAIT );

    /**
     * \brief   Waits for thread completion without sending exit message or terminating the thread.
     *          Returns true if thread completes normally or if waiting timeout is DO_NOT_WAIT.
     *
     * \param   waitForCompleteMs       The timeout in milliseconds to wait for completion.
     * \return  Returns true if either thread completed or the waiting timeout is
     *          areg::DO_NOT_WAIT.
     **/
    virtual bool wait_completion( uint32_t waitForCompleteMs = areg::WAIT_INFINITE );

    /**
     * \brief   Terminates the thread with a 10 ms timeout. Use only if thread is unresponsive and
     *          immediate termination is required. Does not guarantee graceful resource cleanup.
     *
     * \return  Thread completion status: Terminated if timeout expired and thread was terminated;
     *          Completed if thread completed normally; Invalid if thread was not valid and not
     *          running.
     **/
    virtual Thread::ThreadCompletion terminate();

/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief   Returns true if target thread is running.
     **/
    [[nodiscard]]
    inline bool is_running() const noexcept;

    /**
     * \brief   Returns true if thread is valid. A valid thread has a valid handle and thread ID but
     *          is not necessarily running.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns the thread ID.
     **/
    [[nodiscard]]
    inline id_type id() const noexcept;

    /**
     * \brief   Returns the thread name.
     **/
    [[nodiscard]]
    inline const String & name() const noexcept;

    /**
     * \brief   Returns the address object of the thread.
     **/
    [[nodiscard]]
    inline const ThreadAddress & address() const noexcept;

    /**
     * \brief   Sets the thread priority level and returns the old priority. Returns
     *          UndefinedPriority if thread is not created or destroyed. Returns current priority if
     *          newPriority is UndefinedPriority.
     *
     * \param   newPriority     New priority level: Lowest, Low, Normal (default), High, or
     *                          PriorityHighest.
     * \return  Returns old thread priority. Returns UndefinedPriority if thread is invalid.
     **/
    inline Thread::ThreadPriority set_priority( Thread::ThreadPriority newPriority ) noexcept;

    /**
     * \brief   Returns the current thread priority level. Returns UndefinedPriority if thread is
     *          not created.
     **/
    [[nodiscard]]
    inline Thread::ThreadPriority priority() const noexcept;

    /**
     * \brief   Returns the predefined stack size of the thread in kilobytes. Zero means system
     *          default stack size is used.
     **/
    [[nodiscard]]
    inline uint32_t stack_size() const noexcept;

//////////////////////////////////////////////////////////////////////////
// static operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Searches for thread by name and returns its pointer. Returns nullptr if not found.
     *
     * \param   threadName      The unique name of thread to search.
     * \return  Pointer to the thread object if found; nullptr otherwise.
     **/
    [[nodiscard]]
    inline static Thread * find_by_name( const String & threadName) noexcept;

    /**
     * \brief   Searches for thread by ID and returns its pointer. Returns nullptr if not found.
     *
     * \param   threadId    The unique ID of thread to search.
     * \return  Pointer to the thread object if found; nullptr otherwise.
     **/
    [[nodiscard]]
    inline static Thread * find_by_id( id_type threadId ) noexcept;

    /**
     * \brief   Searches for thread by address and returns its pointer. Returns nullptr if not
     *          found.
     *
     * \param   threadAddres    The unique address of the thread containing the name, process ID,
     *                          and thread ID.
     * \return  Pointer to the thread object if found; nullptr otherwise.
     **/
    [[nodiscard]]
    inline static Thread * find_by_address( const ThreadAddress & threadAddres ) noexcept;

    /**
     * \brief   Searches for thread by ID and returns its address containing process and thread IDs.
     *
     * \param   threadId    The ID of thread to get address.
     * \return  Valid thread address if found; invalid address otherwise.
     **/
    [[nodiscard]]
    inline static const ThreadAddress & find_address( id_type threadId ) noexcept;

    /**
     * \brief   Searches for thread by name and returns its address containing process and thread
     *          IDs.
     *
     * \param   threadName      The name of thread to get address.
     * \return  Valid thread address if found; invalid address otherwise.
     **/
    [[nodiscard]]
    inline static const ThreadAddress & find_address( const String & threadName ) noexcept;

    /**
     * \brief   Suspends the current thread in sleep mode for the specified duration in
     *          milliseconds.
     *
     * \param   msTimeout       Timeout in milliseconds.
     **/
    inline static void sleep( uint32_t msTimeout );

    /**
     * \brief   Yields thread processing time to allow other threads to run.
     **/
    inline static void switch_thread() noexcept;

    /**
     * \brief   Returns the ID of the current thread.
     **/
    [[nodiscard]]
    inline static id_type current_thread_id() noexcept;

    /**
     * \brief   Returns the thread object of the current thread. The current thread must be
     *          registered.
     **/
    [[nodiscard]]
    inline static Thread * current_thread() noexcept;

    /**
     * \brief   Returns the name of the current thread. Returns empty string if not registered.
     **/
    [[nodiscard]]
    inline static const String & current_thread_name() noexcept;

    /**
     * \brief   Returns the address of the current thread. Returns invalid address if not registered.
     **/
    [[nodiscard]]
    inline static const ThreadAddress & current_thread_address() noexcept;

    /**
     * \brief   Returns the thread consumer of the current thread from thread-local storage.
     **/
    [[nodiscard]]
    static ThreadConsumer & current_thread_consumer() noexcept;

    /**
     * \brief   Returns the thread-local storage object of the current thread.
     **/
    [[nodiscard]]
    static ThreadLocalStorage & current_thread_storage() noexcept;

    /**
     * \brief   Returns the name of thread by specified ID. Returns empty string if not registered.
     *
     * \param   threadId    The ID of the thread.
     **/
    [[nodiscard]]
    static const String & thread_name( id_type threadId ) noexcept;

    /**
     * \brief   Returns the address of thread by specified ID. Returns invalid address if not
     *          registered.
     *
     * \param   threadId    The ID of the thread.
     **/
    [[nodiscard]]
    static const ThreadAddress & thread_address( id_type threadId ) noexcept;

    /**
     * \brief   Returns the stack size of the current thread in bytes.
     **/
    [[nodiscard]]
    static size_t current_stack_size() noexcept;

/************************************************************************/
// Thread debugging function
/************************************************************************/
#ifdef _DEBUG
    /**
     * \brief   Dumps all created threads information to the output window. Valid only in debug
     *          builds.
     **/
    static void dump_threads();
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////////
// Protected override operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// Thread protected overrides
/************************************************************************/
    /**
     * \brief   Callback invoked before thread consumer runs, immediately on thread entry. Override
     *          to perform additional setup. Returns false to cause the thread to exit immediately
     *          without running the consumer.
     *
     * \return  Returns false to stop the thread; true to continue.
     **/
    virtual bool on_pre_run();

    /**
     * \brief   Callback invoked after the thread consumer completes and before the thread exits.
     **/
    virtual void on_post_exit();

    /**
     * \brief   Returns the first thread from the resource map.
     *
     * \param[out] threadId    On success, contains the ID of the first thread.
     * \return  Pointer to the first thread object if successful; nullptr otherwise.
     **/
    static Thread * first_thread( id_type & threadId ) noexcept;

    /**
     * \brief   Returns the next thread from the resource map.
     *
     * \param[in,out] threadId    On input, contains valid thread ID. On success, contains the ID of
     *                            the next thread.
     * \return  Pointer to the next thread object if successful; nullptr otherwise.
     **/
    static Thread * next_thread( id_type & threadId ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Reference to Thread Consumer interface
     **/
    ThreadConsumer &      mThreadConsumer;
    /**
     * \brief   Thread handle
     **/
    THREADHANDLE            mThreadHandle;
    /**
     * \brief   The ID of thread.
     **/
    id_type                 mThreadId;
    /**
     * \brief   Thread address, containing process ID and thread ID info
     **/
    const ThreadAddress     mThreadAddress;
    /**
     * \brief   The thread current priority level.
     **/
    Thread::ThreadPriority mThreadPriority;
    /**
     * \brief   Flag indicating whether thread is running or not.
     **/
    bool                    mIsRunning;
    /**
     * \brief   The thread stack size in kilobytes.
     **/
    uint32_t                mStackSizeKB;
    /**
     * \brief   Object to synchronize data access
     **/
    mutable ResourceLock    mSyncObject;
    /**
     * \brief   Synchronization Event object, signaled when new created thread starts running
     **/
    SyncEvent               mWaitForRun;
    /**
     * \brief   Synchronization Event object, signaled when thread completes running and going to exist
     **/
    SyncEvent               mWaitForExit;

//////////////////////////////////////////////////////////////////////////
// Private / Hidden types, variables and methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Thread starting point function called by the OS.
     *
     * \param   data    Pointer to the thread object.
     * \return  Thread exit code; 0 if successful.
     **/
    static unsigned long _default_thread_function( void * data );

    /**
     * \brief   Cleans up thread data including reset running flag and thread info invalidation.
     *
     * \param   unregister      If true, removes thread from resource maps and closes handles. If
     *                          false, only closes handles and resets variables.
     **/
    void _clean_resources( bool unregister);

    /**
     * \brief   Registers the thread in resource maps.
     *
     * \return  Returns true if registration succeeds.
     **/
    bool _register_thread();

    /**
     * \brief   Unregisters the thread from resource maps.
     **/
    void _unregister_thread();

    /**
     * \brief   Thread entry point where the thread consumer function is called.
     *
     * \return  Returns thread routine exit code.
     **/
    int32_t  _thread_entry();

    /**
     * \brief   Sets the running state of the thread.
     *
     * \param   is_running      True to mark thread as running; false otherwise.
     **/
    inline void _set_running(bool is_running) noexcept;

    /**
     * \brief   Checks whether the thread is valid without acquiring synchronization locks.
     *
     * \return  Returns true if thread data is valid.
     **/
    [[nodiscard]]
    inline bool _is_valid_no_lock() const noexcept;

    /**
     * \brief   Creates, returns, or deletes thread-local storage based on ownThread parameter. If
     *          ownThread is nullptr, deletes storage. If ownThread is CURRENT_THREAD, returns
     *          existing storage. Otherwise, creates new storage. Called in thread procedure and
     *          deleted on thread exit.
     *
     * \param   ownThread       Thread owning the storage: CURRENT_THREAD returns existing storage;
     *                          nullptr deletes storage; valid Thread pointer creates new storage.
     * \return  Pointer to current thread-local storage object.
     **/
    static ThreadLocalStorage * _thread_local_storage( Thread* ownThread );

    /**
     * \brief   Searches for thread by handle and returns its pointer. Returns nullptr if not found.
     *
     * \param   threadHandle    The unique handle of thread to search.
     * \return  Pointer to the thread object if found; nullptr otherwise.
     **/
    [[nodiscard]]
    inline static Thread * _find_by_handle( THREADHANDLE threadHandle ) noexcept;

    /**
     * \brief   Searches for thread by ID and returns its handle. Returns nullptr if not found or
     *          thread is invalid.
     *
     * \param   threadId    The ID of thread to search.
     * \return  Thread handle if found; nullptr if thread is invalid.
     **/
    [[nodiscard]]
    inline static THREADHANDLE _find_handle( id_type threadId) noexcept;

//////////////////////////////////////////////////////////////////////////
// OS specific hidden calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   POSIX-specific thread routine that calls _default_thread_function.
     *
     * \param   data    Pointer to the thread object.
     * \return  Returns nullptr.
     **/
    static void * _posix_thread_routine( void * data );

    /**
     * \brief   Windows-specific thread routine that calls _default_thread_function.
     *
     * \param   data    Pointer to the thread object.
     * \return  Returns thread exit code.
     **/
    static unsigned long _windows_thread_routine( void * data );

    /**
     * \brief   Sets the OS-level name of the created thread for debugging purposes.
     *
     * \param   threadId        The unique ID of thread.
     * \param   threadName      The name to set.
     **/
    static void _os_set_name( id_type threadId, const char * threadName );

    /**
     * \brief   Closes the OS-level thread handle and invalidates thread data.
     *
     * \param   handle      The thread handle to close.
     **/
    static void _os_close_handle( THREADHANDLE handle );

    /**
     * \brief   OS-specific implementation to put the thread in sleep mode for specified duration in
     *          milliseconds.
     *
     * \param   timeout     Sleep duration in milliseconds.
     **/
    static void _os_sleep( uint32_t timeout );

    /**
     * \brief   OS-specific implementation to get the ID of the current thread.
     **/
    static id_type _os_thread_id() noexcept;

    /**
     * \brief   OS-specific implementation to get the stack size of the current thread in bytes.
     *
     * \param   handle      The thread handle.
     **/
    static size_t _os_stack_size( THREADHANDLE handle ) noexcept;

    /**
     * \brief   OS-specific implementation to create and register the thread.
     * \return  Returns true if creation succeeds.
     **/
    bool _os_create() noexcept;

    /**
     * \brief   OS-specific implementation to delete the thread. Terminates thread if waiting timeout expires.
     * \param   waitForStopMs       Waiting timeout in milliseconds.
     **/
    Thread::ThreadCompletion _os_destroy_thread( uint32_t waitForStopMs );

    /**
     * \brief   OS-specific implementation to set thread priority and return the previous priority.
     * \param   newPriority     The new priority level.
     **/
    Thread::ThreadPriority _os_set_priority( ThreadPriority newPriority ) noexcept;

private:
/************************************************************************/
// Resource mapping types, used to control resources, used by thread
/************************************************************************/
    /**
     * \brief   Thread resource mapping by thread ID.
     *          The unique thread ID is set when thread is created
     **/
    using   MapThreadID             = IdMap<Thread *>;
    using   ImplThreadIDResource    = ResourceMapImpl<id_type, Thread *>;
    using   MapThreadIDResource     = ConcurrentResourceMap<id_type, Thread *, MapThreadID,ImplThreadIDResource>;
    /**
     * \brief   Thread resource mapping by thread handle. 
     *          The unique thread handle can be used to access thread object.
     **/
    using   MapThreadPoiters        = PtrMap<Thread *>;
    using   ImplThreadHandleResource= ResourceMapImpl< void *, Thread *>;
    using   MapThreadHandleResource = ConcurrentResourceMap< void *, Thread *, MapThreadPoiters,ImplThreadHandleResource >;
    /**
     * \brief   Thread resource mapping by thread name. 
     *          The unique thread name can be used to access thread object.
     **/
    using   MapThreadName           = StringMap<Thread *>;
    using   ImplThreadNameResource  = ResourceMapImpl<String, Thread *>;
    using   MapThreadNameResource   = ConcurrentResourceMap<String, Thread *, MapThreadName, ImplThreadNameResource>;

/************************************************************************/
// Resource controlling and mapping variables
/************************************************************************/
    /**
     * \brief   Returns the static map of thread objects indexed by thread handle.
     **/
    [[nodiscard]]
    static  Thread::MapThreadHandleResource & _map_threadh_handle() noexcept;

    /**
     * \brief   Returns the static map of thread objects indexed by thread name.
     **/
    [[nodiscard]]
    static  Thread::MapThreadNameResource & _map_thread_name() noexcept;

    /**
     * \brief   Returns the static map of thread objects indexed by thread ID.
     **/
    [[nodiscard]]
    static  Thread::MapThreadIDResource & _map_thread_id() noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    Thread() = delete;
    AREG_NOCOPY_NOMOVE( Thread );
};

//////////////////////////////////////////////////////////////////////////
// Thread class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline Thread* Thread::_find_by_handle(THREADHANDLE threadHandle) noexcept
{
    return Thread::_map_threadh_handle().find_resource_object(threadHandle);
}

inline THREADHANDLE Thread::_find_handle( id_type threadId) noexcept
{
    Thread * result = Thread::_map_thread_id().find_resource_object(threadId);
    return (result != nullptr ? result->mThreadHandle : nullptr);
}

inline bool Thread::_is_valid_no_lock() const noexcept
{
    return ((mThreadHandle != INVALID_THREAD_HANDLE) && (mThreadId != 0));
}

inline bool Thread::is_running() const noexcept
{
    Lock lock(mSyncObject);
    return mIsRunning;
}

inline bool Thread::is_valid() const noexcept
{
    Lock lock(mSyncObject);
    return _is_valid_no_lock();
}

inline id_type Thread::id() const noexcept
{
    Lock lock(mSyncObject);
    return mThreadId;
}

inline const String& Thread::name() const noexcept
{
    Lock lock(mSyncObject);
    return mThreadAddress.name();
}

inline const ThreadAddress & Thread::address() const noexcept
{
    Lock lock(mSyncObject);
    return mThreadAddress;
}

inline Thread* Thread::find_by_name(const String & threadName) noexcept
{
    return (!threadName.is_empty() ? Thread::_map_thread_name().find_resource_object(threadName) : nullptr);
}

inline Thread* Thread::find_by_id( id_type threadId) noexcept
{
    return Thread::_map_thread_id().find_resource_object(threadId);
}

inline Thread* Thread::find_by_address(const ThreadAddress& threadAddress) noexcept
{
    return Thread::find_by_name(threadAddress.name());
}

inline const ThreadAddress & Thread::find_address( id_type threadId) noexcept
{
    Thread* threadObj = Thread::find_by_id(threadId);
    return (threadObj != nullptr ? threadObj->address() : ThreadAddress::invalid_thread_address());
}

inline const ThreadAddress& Thread::find_address(const String & threadName) noexcept
{
    Thread* threadObj = Thread::find_by_name(threadName);
    return (threadObj != nullptr ? threadObj->address() : ThreadAddress::invalid_thread_address());
}

inline void Thread::_set_running( bool is_running ) noexcept
{
    Lock lock(mSyncObject);
    mIsRunning  = is_running;
}

inline Thread * Thread::current_thread() noexcept
{
    return Thread::find_by_id(Thread::_os_thread_id());
}

inline const String & Thread::current_thread_name() noexcept
{
    return Thread::thread_name( Thread::_os_thread_id() );
}

inline const ThreadAddress & Thread::current_thread_address() noexcept
{
    return Thread::thread_address( Thread::_os_thread_id() );
}

inline Thread::ThreadPriority Thread::priority() const noexcept
{
    Lock  lock( mSyncObject );
    return (is_valid( ) ? mThreadPriority : Thread::ThreadPriority::Undefined);
}

inline uint32_t Thread::stack_size() const noexcept
{
    return mStackSizeKB;
}

inline void Thread::sleep( uint32_t ms )
{
    _os_sleep( ms );
}

inline void Thread::switch_thread() noexcept
{
    Thread::_os_sleep( areg::WAIT_SWITCH );
}

inline id_type Thread::current_thread_id() noexcept
{
    return _os_thread_id( );
}

inline Thread::ThreadPriority Thread::set_priority( ThreadPriority newPriority ) noexcept
{
    return _os_set_priority( newPriority );
}

inline constexpr const char * Thread::as_string( Thread::ThreadPriority threadPriority ) noexcept
{
    switch ( threadPriority )
    {
    case Thread::ThreadPriority::Undefined:
        return "Thread::ThreadPriority::Undefined";
    case Thread::ThreadPriority::Lowest:
        return "Thread::ThreadPriority::Lowest";
    case Thread::ThreadPriority::Low:
        return "Thread::ThreadPriority::Low";
    case Thread::ThreadPriority::Normal:
        return "Thread::ThreadPriority::Normal";
    case Thread::ThreadPriority::High:
        return "Thread::ThreadPriority::High";
    case Thread::ThreadPriority::Highest:
        return "Thread::ThreadPriority::Highest";
    default:
        return "ERR: Invalid Thread::ThreadPriority value!";
    }
}

} // namespace areg
#endif  // AREG_BASE_THREAD_HPP
