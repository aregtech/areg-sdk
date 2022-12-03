#ifndef AREG_BASE_THREAD_HPP
#define AREG_BASE_THREAD_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/Thread.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread class
 *              Base class for all kind of threads. Use this class or
 *              any derived class if need to access class object
 *              by name, ID or handle.
 *              Every thread as well contains unique thread address.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/RuntimeObject.hpp"

#include "areg/base/NECommon.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/TEResourceMap.hpp"
#include "areg/base/String.hpp"

#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
class ThreadLocalStorage;
class IEThreadConsumer;
class IEInStream;
class String;

//////////////////////////////////////////////////////////////////////////
// Thread class declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A base class of all thread objects running int the system.
 *          Use this or derived classes to keep tracking of instantiated and running thread
 *          that can be accessed by ID, unique names or handles. The class creates (starts) 
 *          and destroys (stops) thread. To implement the cyclic runs, the object requires
 *          instance of thread consumer (IEThreadConsumer). It as well provides the possibility
 *          to save thread specific storage to store objects that are not accessible outside
 *          of the thread context.
 * 
 *          The derived objects are Dispatcher, Worker and Component threads that are able
 *          to receive and process thread specific events.
 *
 * \see     IEThreadConsumer, ThreadLocalStorage, DispatcherThread, WorkerThread, ComponentThread
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
     * \brief   Thread::eCompletionStatus
     *          Thread Completion status used as return value on destroy thread
     **/
    typedef enum class E_CompletionStatus : int
    {
          ThreadTerminated  = -1    //!< The thread was terminate because waiting timeout expired
        , ThreadCompleted   = 0     //!< The thread was valid and normally completed
        , ThreadInvalid     = 1     //!< The thread handle is invalid and is not running, nothing to do
    } eCompletionStatus;

    /**
     * \brief   Thread::eThreadPriority
     *          Defines the thread priorities.
     *          By default, the thread is created by Normal priority.
     *          Increase or decrease thread priority if need.
     **/
    typedef enum class E_ThreadPriority : int
    {
          PriorityUndefined = MIN_INT_32    //!< Undefined priority. If thread is created and not valid
        , PriorityLowest    = -2            //!< Lowest priority level
        , PriorityLow       = -1            //!< Below normal priority level
        , PriorityNormal    =  0            //!< Normal priority level. All threads by default are created with normal priority level
        , PriorityHigh      =  1            //!< Above normal priority level
        , PriorityHighest   =  2            //!< Highest priority level
    } eThreadPriority;

    /**
     * \brief   Converts Thread::eThreadPriority values to string and return string values.
     **/
    inline static const char * getString( Thread::eThreadPriority threadPriority );

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
    static constexpr ptr_type           CURRENT_THREAD          { static_cast<ptr_type>(~0) };

//////////////////////////////////////////////////////////////////////////
// Declare Thread as runtime object
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(Thread)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Initialize Thread object, sets the instance of thread consumer
     *          and optional thread name.
     * \param	threadConsumer	The valid instance of thread consumer object.
     * \param	threadName	    The thread name to set and track in the system.
     *                          It should be unique name to be able to track.
     *                          If nullptr or the name is duplicated, the system will not
     *                          be able to track the thread by name.
     **/
    Thread( IEThreadConsumer & threadConsumer, const String & threadName );

    /**
     * \brief	Free thread resources and ensures that thread handle is closed.
     **/
    virtual ~Thread( void );

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Operations
/************************************************************************/

    /**
     * \brief	Creates and Starts thread, if it was not created before.
     *          If thread was already created, no action will be performed.
     * \param	waitForStartMs	Waiting time out in milliseconds until thread
     *                          is created and running.
     *                          -   Set DO_NOT_WAIT to escape waiting. The function
     *                              returns immediately when thread is created and
     *                              gives no guarantee that it already runs.
     *                          -   Set WAIT_INFINITE to ensure that thread is running.
     *                          -   Set any other value in milliseconds to specify waiting time
     *                              until thread starts running or timeout expires.
     * \return	Returns true if new thread is successfully created and started.
     **/
    virtual bool createThread( unsigned int waitForStartMs = NECommon::DO_NOT_WAIT );

    /**
     * \brief	Destroys thread and frees resources. Once thread is destroyed,
     *          it can be re-created again. The calling thread (current thread)
     *          may be blocked until target thread is destroyed.
     * \param	waitForStopMs	Waiting time out in milliseconds until target thread is finis run.
     *                          -   Set DO_NOT_WAIT to trigger exit thread and immediately return
     *                              without waiting for thread to complete job.
     *                          -   Set WAIT_INFINITE to wait until thread completes job and exit.
     *                          -   Set any other value in milliseconds to specify waiting time
     *                              until thread completes the job or timeout expires.
     * \return	Returns the thread completion status. The following statuses are defined:
     *              Thread::ThreadTerminated  -- The waiting timeout expired and thread was terminated;
     *              Thread::ThreadCompleted   -- The thread was valid and completed normally;
     *              Thread::ThreadInvalid     -- The thread was not valid and was not running, nothing was done.
     **/
    virtual Thread::eCompletionStatus destroyThread( unsigned int waitForStopMs = NECommon::DO_NOT_WAIT );

    /**
     * \brief   It calls destroyThread() with infinite timeout. In each thread class the shutdown procedure may differ.
     *          For more details see description in each class derived from Thread.
     **/
    virtual void shutdownThread( void );

    /**
     * \brief   Wait for thread completion. It will neither sent exit message, nor terminate thread.
     *          The function waits as long, until the thread is completed or timeout is expired.
     *          It will return true if thread has been completed and exits normally, or the waiting 
     *          timeout is NECommon::DO_NOT_WAIT.
     * \param   waitForCompleteMs   The timeout in milliseconds to wait for completion.
     * \return  Returns true if either thread completed or the waiting timeout is NECommon::DO_NOT_WAIT.
     **/
    virtual bool completionWait( unsigned int waitForCompleteMs = NECommon::WAIT_INFINITE );

    /**
     * \brief   It calls destroyThread() with waiting timeout 10 ms. If waiting time is expired, 
     *          it immediately terminates the thread and returns completion status 'terminated'.
     *          Use this function only if thread does not react anymore and immediate termination
     *          is required. By calling this method, the system does not guarantee the graceful
     *          way of cleaning resources and stacks.
     * 
     * \return	Returns the thread completion status. The following statuses are defined:
     *              Thread::ThreadTerminated  -- The waiting timeout expired and thread was terminated;
     *              Thread::ThreadCompleted   -- The thread was valid and completed normally;
     *              Thread::ThreadInvalid     -- The thread was not valid and was not running, nothing was done.
     **/
    virtual Thread::eCompletionStatus terminateThread( void );

/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief	Returns true if target thread is running.
     **/
    inline bool isRunning( void ) const;

    /**
     * \brief   Returns true if thread is valid. The valid thread has valid handle, valid thread ID.
     *          The valid thread no necessarily is running.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns thread ID
     **/
    inline id_type getId( void ) const;

    /**
     * \brief   Returns thread name
     **/
    inline const String & getName( void ) const;

    /**
     * \brief	Returns the address object of thread.
     **/
    inline const ThreadAddress & getAddress( void ) const;

    /**
     * \brief   Sets the thread priority level and returns the old priority level.
     *          If thread is not created or destroyed, the function will ignore and return Timer::UndefinedPriority value.
     *          If thread is created and the priority level is Timer::UndefinedPriority, function will do nothing and
     *          return the current priority level.
     * \param   newPriority     New priority level for thread. There are following valid priorities for valid thread available:
     *                          - Thread::PriorityLowest  -- The lowest priority level
     *                          - Thread::PriorityLow     -- Priority below Normal and above Lowest
     *                          - Thread::PriorityNormal  -- Normal priority level, all threads by default are created with Normal priority level
     *                          - Thread::PriorityHigh    -- Priority above Normal and below Highest
     *                          - Thread::PriorityNighest -- The Highest thread priority
     * \return  Returns old thread priority.
     *          If newPriority is Timer::UndefinedPriority, makes not changes and returns current priority level.
     *          If thread is not valid, returns Timer::UndefinedPriority.
     **/
    inline Thread::eThreadPriority setPriority( Thread::eThreadPriority newPriority );

    /**
     * \brief   Returns thread current priority level. By default, thread is created with Normal priority level
     *          If thread is not created, returns Thread::UndefinedPriority.
     *          If thread is created, returns following values:
     *              - Thread::PriorityLowest  -- The lowest priority level
     *              - Thread::PriorityLow     -- Priority below Normal and above Lowest
     *              - Thread::PriorityNormal  -- Normal priority level, all threads by default are created with Normal priority level
     *              - Thread::PriorityHigh    -- Priority above Normal and below Highest
     *              - Thread::PriorityNighest -- The Highest thread priority
     **/
    inline Thread::eThreadPriority getPriority( void ) const;

//////////////////////////////////////////////////////////////////////////
// static operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief	Search by thread name and return pointer the thread object.
     *          If name could not find, returns nullptr
     * \param	threadName	The unique name of thread to search
     * \return	If not nullptr, the thread object was found.
     **/
    inline static Thread * findThreadByName( const String & threadName) ;

    /**
     * \brief	Search by thread ID and return pointer the thread object.
     *          If ID could not find, returns nullptr
     * \param	threadID    The unique ID of thread to search
     * \return	If not nullptr, the thread object was found.
     **/
    inline static Thread * findThreadById( id_type threadId );

    /**
     * \brief	Search by thread context and return pointer the thread object.
     *          If Context could not find, returns nullptr
     * \param	threadContext   The unique Context of thread.
     *                          It contains Process and Thread IDs information
     * \return	If not nullptr, the thread object was found.
     **/
    inline static Thread * findThreadByAddress( const ThreadAddress & threadAddres );

    /**
     * \brief   Lookup Thread by thread ID and returns Thread Address object,
     *          which contains information of Process ID and Thread ID.
     * \param	threadID	The ID of thread to get address
     * \return	If found, returns valid thread address object.
     *          Otherwise returns invalid thread address.
     **/
    inline static const ThreadAddress & findThreadAddressById( id_type threadId );

    /**
     * \brief   Lookup Thread by thread name and returns Thread Address object,
     *          which contains information of Process ID and Thread ID.
     * \param	threadName	The name of thread to get address
     * \return	If found, returns valid thread address object.
     *          Otherwise returns invalid thread address.
     **/
    inline static const ThreadAddress & findThreadAddressByName( const String & threadName );

    /**
     * \brief   Suspends current thread and puts in a sleep mode for specified timeout in milliseconds.
     *          It continues execution when time is expired.
     * \param   msTimeout   Timeout in milliseconds to put thread in sleep mode.
     **/
    inline static void sleep( unsigned int msTimeout );

    /**
     * \brief   Switches thread processing time.
     *          It does not put thread in sleep, but let other thread take processing time.
     **/
    inline static void switchThread( void );

    /**
     * \brief   Return the ID of current thread.
     **/
    inline static id_type getCurrentThreadId( void );

    /**
     * \brief   Returns the thread object of current thread.
     *          The current thread must be registered in the resource map.
     **/
    inline static Thread * getCurrentThread( void );

    /**
     * \brief   Returns the name of current thread.
     *          If Thread is not registered, returns empty string.
     **/
    inline static const String & getCurrentThreadName( void );

    /**
     * \brief   Returns the address of current thread.
     *          If Thread is not registered, returns invalid address.
     **/
    inline static const ThreadAddress & getCurrentThreadAddress( void );

    /**
     * \brief   Returns the current Thread Consumer, saved in local storage.
     **/
    static IEThreadConsumer & getCurrentThreadConsumer( void );

    /**
     * \brief   Returns the Local Storage Object of the Thread.
     **/
    static ThreadLocalStorage & getCurrentThreadStorage( void );

    /**
     * \brief   Returns the name of thread by specified ID. 
     *          If Thread is not registered, returns empty string.
     **/
    static const String & getThreadName( id_type threadId );

    /**
     * \brief   Returns the address of thread by specified ID. 
     *          If Thread is not registered, returns invalid address.
     **/
    static const ThreadAddress & getThreadAddress( id_type threadId );

/************************************************************************/
// Thread debugging function
/************************************************************************/
#ifdef _DEBUG
    /**
     * \brief   Dumps all created threads information in Output window console.
     *          Valid only for debug version.
     **/
    static void dumpThreads( void );
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////////
// Protected override operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// Thread protected overrides
/************************************************************************/
    /**
     * \brief	This callback is called before thread consumer run function call,
     *          i.e. immediately on thread entry function .
     *          Overwrite function if additional actions should be performed before run.
     *          If function returns false, the new created thread will
     *          immediately exit with error code without calling run method
     *          of thread consumer.
     * \return	If returns false, thread will stop running and exit.
     **/
    virtual bool onPreRunThread( void );

    /**
     * \brief   This callback is called after thread consumer completes running
     *          and before thread exits from starting point.
     **/
    virtual void onPostExitThread( void );

    /**
     * \brief   Returns the first thread element from resource map.
     * \param   threadId    If method succeeds, on output is valid thread ID.
     * \return  If succeed, returns pointer to valid thread object.
     **/
    static Thread * getFirstThread( id_type & OUT threadId );

    /**
     * \brief   Returns the next thread element resource map.
     * \param   threadId    ON input this should contain valid thread ID.
     *                      If method succeeds, on output is valid thread ID.
     * \return  If succeed, returns pointer to valid thread object.
     **/
    static Thread * getNextThread( id_type & IN OUT threadId );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Reference to Thread Consumer interface
     **/
    IEThreadConsumer &      mThreadConsumer;
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
    Thread::eThreadPriority mThreadPriority;
    /**
     * \brief   Flag indicating whether thread is running or not.
     **/
    bool                    mIsRunning;
    /**
     * \brief   Object to synchronize data access
     **/
    mutable ResourceLock    mSynchObject;
    /**
     * \brief   Synchronization Event object, signaled when new created thread starts running
     **/
    SynchEvent              mWaitForRun;
    /**
     * \brief   Synchronization Event object, signaled when thread completes running and going to exist
     **/
    SynchEvent              mWaitForExit;

//////////////////////////////////////////////////////////////////////////
// Private / Hidden types, variables and methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Thread starting point.
     * \param   data    Pointer to thread class.
     * \return  Thread exit code. If succeed, returns 0.
     **/
    static unsigned long _defaultThreadFunction( void * data );

    /**
     * \brief   Cleans data of Thread object, i.e. reset running flag, invalidates thread info.
     **/
    void _cleanResources( void );

    /**
     * \brief   Registers Thread. Returns true if succeed
     **/
    bool _registerThread( void );

    /**
     * \brief   Unregisters thread. Returns true if Thread was valid.
     **/
    void _unregisterThread( void );

    /**
     * \brief   Thread entry point. Consumer function call performed here.
     * \return  Returns thread routine exit code.
     **/
    int  _threadEntry( void );

    /**
     * \brief   Set running / not running flag
     **/
    inline void _setRunning(bool isRunning);

    /**
     * \brief   Checks whether the thread is valid or not without locking synchronization objects.
     * \return  Returns true if thread data is valid.
     **/
    inline bool _isValidNoLock( void ) const;

    /**
     * \brief   Creates new, returns current or deletes existing Local Thread Storage
     *          object, depending on ownThread parameter. If ownThread is nullptr, deletes
     *          Thread Local Storage. If ownThread is a valid pointer and not equal to
     *          Thread::CURRENT_THREAD, creates new storage, otherwise returns existing.
     *          This function is initialized in Thread Procedure (_defaultThreadFunction)
     *          and deleted on exit from thread procedure.
     * \param   ownThread   The Thread Object owning local storage.
     *                      a) if Thread::CURRENT_THREAD,  returns existing local storage;
     *                      b) if nullptr, deletes existing local storage (created before);
     *                      c) if pointer of valid Thread Object, creates local storage 
     *                          (called in Thread Procedure).
     *                      Use only with Thread::CURRENT_THREAD
     * \return  The pointer of current Local Storage object.
     **/
    static ThreadLocalStorage * _getThreadLocalStorage( Thread* ownThread );

    /**
     * \brief	Search by thread handle and return pointer the thread object.
     *          If handle could not find, returns nullptr
     * \param	threadHandle    The unique handle of thread to search
     * \return	If not nullptr, the thread object was found.
     **/
    inline static Thread * _findThreadByHandle( THREADHANDLE threadHandle );

    /**
     * \brief   Searches in resources make thread by ID.
     *          Returns thread handle, if found. Otherwise, returns nullptr.
     * \param   threadId    The ID of thread to search.
     * \return  If not nullptr, the handle of valid thread returned.
     *          Otherwise, the thread is invalid, meaning not exists, not created or already closed.
     **/
    inline static THREADHANDLE _findThreadHandleById( id_type threadId);

//////////////////////////////////////////////////////////////////////////
// OS specific hidden calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   POSIX specific thread routine.
     *          This calls _defaultThreadFunction.
     * \param   data    Pointer to the thread object.
     * \return  Returns nullptr.
     **/
    static void * _posixThreadRoutine( void * data );

    /**
     * \brief   Windows specific thread routine.
     *          This calls _defaultThreadFunction.
     * \param   data    Pointer to the thread object.
     * \return  Returns thread exit code.
     **/
    static unsigned long _windowsThreadRoutine( void * data );

    /**
     * \brief	Sets name to new created name. Might be useful during debugging.
     * \param	threadId	The unique ID of thread to set name
     * \param	threadName	The name to set.
     * \return
     **/
    static void _osSetThreadName( id_type threadId, const char * threadName );

    /**
     * \brief   System depended call. Closes the handle object of thread.
     *          Makes thread  data invalid.
     **/
    static void _osCloseHandle( THREADHANDLE handle );

    /**
     * \brief   Puts the thread in a sleeping mode for specified duration in milliseconds.
     **/
    static void _osSleep( unsigned int timeout );

    /**
     * \brief   OS specific implementation of getting the ID of current thread.
     **/
    static id_type _osGetCurrentThreadId( void );

    /**
     * \brief   OS specific implementation to create and registers thread. Returns true if succeed.
     **/
    bool _osCreateSystemThread( void );

    /**
     * \brief   OS specific implementation of deleting thread. The passed waiting timeout in
     *          milliseconds specifies how long should the caller wait for thread to complete.
     *          If timer expires and thread did not complete, it kills / cancels the thread.
     **/
    Thread::eCompletionStatus _osDestroyThread( unsigned int waitForStopMs );

    /**
     * \brief   OS specific implementation to set thread new priority.
     *          returns the previos priority of the thread.
     **/
    Thread::eThreadPriority _osSetPriority( eThreadPriority newPriority );

private:
/************************************************************************/
// Resource mapping types, used to control resources, used by thread
/************************************************************************/
    /**
     * \brief   Thread resource mapping by thread ID.
     *          The unique thread ID is set when thread is created
     **/
    using   MapThreadID             = TEIdMap< Thread* >;
    using   ImplThreadIDResource    = TEResourceMapImpl<id_type, Thread>;
    using   MapThreadIDResource     = TELockResourceMap<id_type, Thread, MapThreadID,ImplThreadIDResource>;
    /**
     * \brief   Thread resource mapping by thread handle. 
     *          The unique thread handle can be used to access thread object.
     **/
    using   MapThreadPoiters        = TEPointerMap< Thread* >;
    using   ImplThreadHandleResource= TEResourceMapImpl< void *, Thread >;
    using   MapThreadHandleResource = TELockResourceMap< void *, Thread, MapThreadPoiters,ImplThreadHandleResource >;
    /**
     * \brief   Thread resource mapping by thread name. 
     *          The unique thread name can be used to access thread object.
     **/
    using   MapThreadName           = TEStringMap<Thread *>;
    using   ImplThreadNameResource  = TEResourceMapImpl<String, Thread>;
    using   MapThreadNameResource   = TELockResourceMap<String, Thread, MapThreadName, ImplThreadNameResource>;

/************************************************************************/
// Resource controlling and mapping variables
/************************************************************************/
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    static  MapThreadHandleResource     _mapThreadhHandle;  //!< Map of thread object where key is thread handle
    static  MapThreadNameResource       _mapThreadName;     //!< Map of thread object where key is thread name
    static  MapThreadIDResource         _mapThreadId;       //!< Map of thread object where key is thread ID
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    Thread( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( Thread );
};

//////////////////////////////////////////////////////////////////////////
// Thread class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline Thread* Thread::_findThreadByHandle(THREADHANDLE threadHandle)
{
    return Thread::_mapThreadhHandle.findResourceObject(threadHandle);
}

inline THREADHANDLE Thread::_findThreadHandleById( id_type threadId)
{
    Thread * result = Thread::_mapThreadId.findResourceObject(threadId);
    return (result != nullptr ? result->mThreadHandle : nullptr);
}

inline bool Thread::_isValidNoLock( void ) const
{
    return (mThreadHandle != INVALID_THREAD_HANDLE && mThreadId != 0);
}

inline bool Thread::isRunning( void ) const
{
    Lock lock(mSynchObject);
    return mIsRunning;
}

inline bool Thread::isValid( void ) const
{
    Lock lock(mSynchObject);
    return _isValidNoLock();
}

inline id_type Thread::getId( void ) const
{
    Lock lock(mSynchObject);
    return mThreadId;
}

inline const String& Thread::getName( void ) const
{
    Lock lock(mSynchObject);
    return mThreadAddress.getThreadName();
}

inline const ThreadAddress & Thread::getAddress( void ) const
{
    Lock lock(mSynchObject);
    return mThreadAddress;
}

inline Thread* Thread::findThreadByName(const String & threadName)
{
    return (!threadName.isEmpty() ? Thread::_mapThreadName.findResourceObject(threadName) : nullptr);
}

inline Thread* Thread::findThreadById( id_type threadId)
{
    return Thread::_mapThreadId.findResourceObject(threadId);
}

inline Thread* Thread::findThreadByAddress(const ThreadAddress& threadAddress)
{
    return Thread::findThreadByName(threadAddress.getThreadName());
}

inline const ThreadAddress & Thread::findThreadAddressById( id_type threadId)
{
    Thread* threadObj = Thread::findThreadById(threadId);
    return (threadObj != nullptr ? threadObj->getAddress() : ThreadAddress::getInvalidThreadAddress());
}

inline const ThreadAddress& Thread::findThreadAddressByName(const String & threadName)
{
    Thread* threadObj = Thread::findThreadByName(threadName);
    return (threadObj != nullptr ? threadObj->getAddress() : ThreadAddress::getInvalidThreadAddress());
}

inline void Thread::_setRunning( bool isRunning )
{
    Lock lock(mSynchObject);
    mIsRunning  = isRunning;
}

inline Thread * Thread::getCurrentThread( void )
{
    return Thread::findThreadById(Thread::_osGetCurrentThreadId());
}

inline const String & Thread::getCurrentThreadName( void )
{
    return Thread::getThreadName( Thread::_osGetCurrentThreadId() );
}

inline const ThreadAddress & Thread::getCurrentThreadAddress( void )
{
    return Thread::getThreadAddress( Thread::_osGetCurrentThreadId() );
}

inline Thread::eThreadPriority Thread::getPriority( void ) const
{
    Lock  lock( mSynchObject );
    return (isValid( ) ? mThreadPriority : Thread::eThreadPriority::PriorityUndefined);
}

inline void Thread::sleep( unsigned int ms )
{
    _osSleep( ms );
}

inline void Thread::switchThread( void )
{
    Thread::_osSleep( NECommon::WAIT_SWITCH );
}

inline id_type Thread::getCurrentThreadId( void )
{
    return _osGetCurrentThreadId( );
}

inline Thread::eThreadPriority Thread::setPriority( eThreadPriority newPriority )
{
    return _osSetPriority( newPriority );
}

inline const char * Thread::getString( Thread::eThreadPriority threadPriority )
{
    switch ( threadPriority )
    {
    case Thread::eThreadPriority::PriorityUndefined:
        return "Thread::PriorityUndefined";
    case Thread::eThreadPriority::PriorityLowest:
        return "Thread::PriorityLowest";
    case Thread::eThreadPriority::PriorityLow:
        return "Thread::PriorityLow";
    case Thread::eThreadPriority::PriorityNormal:
        return "Thread::PriorityNormal";
    case Thread::eThreadPriority::PriorityHigh:
        return "Thread::PriorityHigh";
    case Thread::eThreadPriority::PriorityHighest:
        return "Thread::PriorityHighest";
    default:
        return "ERR: Invalid Thread::eThreadPriority value!";
    }
}

#endif  // AREG_BASE_THREAD_HPP
