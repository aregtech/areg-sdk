#ifndef AREG_BASE_THREAD_HPP
#define AREG_BASE_THREAD_HPP
/************************************************************************
 * \file        areg/base/Thread.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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

#include "areg/base/ThreadAddress.hpp"
#include "areg/base/EContainers.hpp"
#include "areg/base/TEResourceMap.hpp"
#include "areg/base/String.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class ThreadLocalStorage;
class IEThreadConsumer;
class IEInStream;

/**
 * \brief       This is base class for all kinds of threads.
 *              Use this class of any of derived class if there is need
 *              to access thread object by unique name, ID or handle.
 *              The class creates and destroys thread and support
 *              other basic functionalities. The thread class is
 *              an instance of Runtime Object and the Runtime type
 *              can be checked at any time.
 *              To instantiate the class the reference to
 *              IEThreadConsumer should be passed to trigger
 *              Run() and Exit() functions. All threads should have
 *              unique name. If the name of thread is not passed,
 *              it will be automatically generated.
 *              Every thread contains Thread Address and the instances
 *              of threads can be accessed by referring to Thread Address.
 *              The thread address is unique within system. It contains
 *              process ID and thread name information.
 * 
 * \details     Every thread has Address and the thread address is unique
 *              within system. It contains Process ID and the unique within
 *              process thread name. Creating and Destroying thread 
 *              supports waiting functionality. The calling thread
 *              will wait as long, until either new created thread 
 *              is running or timeout expired.
 *              The thread consumer object reference should be passed in
 *              the constructor of thread object to trigger appropriate.
 *              For more details about Thread Consumer see IEThreadConsumer
 *              interface description.
 *              Thread Objects are runtime object instances. If system
 *              supports different types of threads (dispatcher thread,
 *              worker thread, etc.) their types can be checked by runtime
 *              information.
 *              When thread is started, it initializes local storage.
 *              In thread local storage can be saved any instantiated element
 *              by its name. The local storage is deleted when thread exists.
 *              Any element, which is instantiated and saved in local storage
 *              by pointer, should be deleted before thread exists.
 *              The local storage is valid and accessible only within
 *              thread context. Which means that it is impossible to
 *              access local storage from another thread.
 *              For more information of using thread local storage
 *              see ThreadLocalStorage interface.
 *
 * \see         IEThreadConsumer, ThreadLocalStorage
 **/
//////////////////////////////////////////////////////////////////////////
// Thread class declaration
//////////////////////////////////////////////////////////////////////////
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
    typedef enum E_CompletionStatus
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
    typedef enum E_ThreadPriority
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
     * \brief   Thread::DO_NOT_WAIT
     *          Constant, indicating do not wait for thread start
     **/
    static const unsigned int   DO_NOT_WAIT                 = static_cast<unsigned int>(0);                 /*0x00000000*/
    /**
     * \brief   Thread::WAIT_SWITCH
     *          Constant, used to switch the thread. Minimum waiting time.
     **/
    static const unsigned int   WAIT_SWITCH                 = static_cast<unsigned int>(1);
    /**
     * \brief   Thread::WAIT_1_MILLISECOND
     *          Constant, wait for 1 millisecond of created thread startup
     **/
    static const unsigned int   WAIT_1_MILLISECOND          = static_cast<unsigned int>(1);
    /**
     * \brief   Thread::WAIT_5_MILLISECONDS
     *          Constant, wait for 5 milliseconds of created thread startup
     **/
    static const unsigned int   WAIT_5_MILLISECONDS         = static_cast<unsigned int>(5);
    /**
     * \brief   Thread::WAIT_10_MILLISECONDS
     *          Constant, wait for 10 milliseconds of created thread startup
     **/
    static const unsigned int   WAIT_10_MILLISECONDS       = static_cast<unsigned int>(10);
    /**
     * \brief   Thread::WAIT_50_MILLISECONDS
     *          Constant, wait for 10 milliseconds of created thread startup
     **/
    static const unsigned int   WAIT_50_MILLISECONDS        = static_cast<unsigned int>(50);
    /**
     * \brief   Thread::WAIT_100_MILLISECONDS
     *          Constant, wait for 100 milliseconds of created thread startup
     **/
    static const unsigned int   WAIT_100_MILLISECONDS       = static_cast<unsigned int>(100);
    /**
     * \brief   Thread::WAIT_500_MILLISECONDS
     *          Constant, wait for 500 milliseconds of created thread startup
     **/
    static const unsigned int   WAIT_500_MILLISECONDS       = static_cast<unsigned int>(500);
    /**
     * \brief   Thread::WAIT_1_SECOND
     *          Constant, wait for 1 second of created thread startup
     **/
    static const unsigned int   WAIT_1_SECOND               = static_cast<unsigned int>(1 * 1000);
    /**
     * \brief   Thread::WAIT_5_SECONDS
     *          Constant, wait for 5 seconds of created thread startup
     **/
    static const unsigned int   WAIT_5_SECONDS              = static_cast<unsigned int>(5 * 1000);
    /**
     * \brief   Thread::WAIT_10_SECONDS
     *          Constant, wait for 10 seconds of created thread startup
     **/
    static const unsigned int   WAIT_10_SECONDS             = static_cast<unsigned int>(10 * 1000);
    /**
     * \brief   Thread::WAIT_INFINITE
     *          Constant, wait until created thread did not start
     **/
    static const unsigned int   WAIT_INFINITE               /*= INFINITE*/; // 0xFFFFFFFF
    /**
     * \brief   Thread::INVALID_THREAD_ID
     *          Invalid thread ID.
     **/
    static const ITEM_ID        INVALID_THREAD_ID           /*= 0*/;

private:
/************************************************************************/
// Private constants
/************************************************************************/
    /**
     * \brief   Thread::INVALID_THREAD_HANDLE
     *          Thread invalid handle
     **/
    static THREADHANDLE const       INVALID_THREAD_HANDLE   /*= static_cast<void *>(NULL)*/;

    /**
     * \brief   Thread::DEFAULT_THREAD_PREFIX
     *          Internal thread naming prefix. Used to generate unique thread name.
     **/
    static const char * const       DEFAULT_THREAD_PREFIX   /*= "_AREG_GThread_"*/;

    /**
     * \brief   Thread::CURRENT_THREAD
     *          Identified Current Thread. Used for Local Storage
     **/
    static Thread * const         CURRENT_THREAD          /*= reinterpret_cast<Thread *>(-1)*/; /*0xFFFFFFFF*/

    /**
     * \brief   Thread::STORAGE_THREAD_CONSUMER
     *          The name of entry in Thread Local Storage to save pointer of Thread Consumer.
     **/
    static const char * const       STORAGE_THREAD_CONSUMER /*= "ThreadConsumer"*/;

    /**
     * \brief   Thread::SET_NAME_MS_VC_EXCEPTION
     *          MS Exception value, used to set thread name.
     **/
    static const unsigned int       SET_NAME_MS_VC_EXCEPTION/*= 0x406D1388*/;

//////////////////////////////////////////////////////////////////////////
// Declare Thread as runtime object
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(Thread)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Initialize Thread object, indicate thread consumer and
     *          pass thread name, set after creating.
     * \param	threadConsumer	Reference to thread consumer interface
     * \param	threadName	    Thread name to set immediately after create
     *                          Thread name can be useful during debugging.
     *                          If no thread name is passed, the unique
     *                          name will be generated by system.
     **/
    Thread( IEThreadConsumer & threadConsumer, const char * threadName = NULL);

    /**
     * \brief	Destructor. 
     *          Ensures that thread handle is closed before destroying object
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
     *          After creating thread, current calling thread may be blocked
     *          until new created thread did not resume. The waiting timeout
     *          is specified in waitForStartMs.
     * \param	waitForStartMs	Waiting time out in milliseconds until new
     *                          created thread starts running.
     *                          If DO_NOT_WAIT, current thread will not be
     *                          blocked and will continue execution.
     *                          if WAIT_INFINITE, current thread will be blocked
     *                          as long, until new created thread did not resume
     * \return	Returns true if new thread is successfully created and started.
     **/
    virtual bool createThread( unsigned int waitForStartMs = Thread::DO_NOT_WAIT );

    /**
     * \brief	Destroys thread and free resources. Once thread is destroyed,
     *          it can be re-created again. The calling thread (current thread)
     *          may be blocked until target thread is not destroyed.
     * \param	waitForStopMs	Waiting time out in milliseconds until
     *                          target thread is not finished running.
     *                          If DO_NOT_WAIT, calling thread (current thread)
     *                          will not be blocked and will not wait until target
     *                          thread finished running. It will immediately
     *                          close thread handle and free resources.
     *                          If WAIT_INFINITE, calling (current) thread will wait
     *                          until target thread completes running.
     *                          Any other value may cause thread termination if 
     *                          waiting time is expired and the target thread still runs.
     * \return	Returns the thread completion status. The following statuses are defined:
     *              Thread::ThreadTerminated  -- The waiting timeout expired and thread was terminated;
     *              Thread::ThreadCompleted   -- The thread was valid and completed normally;
     *              Thread::ThreadInvalid     -- The thread was not valid and was not running, nothing was done.
     **/
    virtual Thread::eCompletionStatus destroyThread( unsigned int waitForStopMs = Thread::DO_NOT_WAIT );

    /**
     * \brief   In this object, it calls destroyThread with infinite timeout.
     *          In each thread class the shutdown procedure may differ. For more details see description in each class
     *          derived from Thread.
     **/
    virtual void shutdownThread( void );

    /**
     * \brief   Wait for thread completion. It will neither sent exit message, nor terminate thread.
     *          The function waits as long, until the thread is not completed.
     *          It will return true if thread has been completed or waiting timeout is Thread::DO_NOT_WAIT.
     *          If thread exists normally, it will return true.
     * \param   waitForCompleteMs   The timeout to wait for completion.
     * \return  Returns true if either thread completed or the waiting timeout is Thread::DO_NOT_WAIT.
     **/
    virtual bool completionWait( unsigned int waitForCompleteMs = Thread::WAIT_INFINITE );

/************************************************************************
 * Attributes
 ************************************************************************/

    /**
     * \brief	Returns true if target thread is running.
     **/
    inline bool isRunning( void ) const;

    /**
     * \brief   Returns true if thread is valid. 
     *          The valid thread has valid handle, valid thread ID,
     *          but no need that valid thread is running.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns thread ID
     **/
    inline ITEM_ID getId( void ) const;

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
    Thread::eThreadPriority setPriority( Thread::eThreadPriority newPriority );

    /**
     * \brief   Returns thread current priority level. By default, thread is created with Normal priority level
     *          If thread is not created, returns Timer::UndefinedPriority.
     *          If thread is created, returns following values:
     *              - Thread::PriorityLowest  -- The lowest priority level
     *              - Thread::PriorityLow     -- Priority below Normal and above Lowest
     *              - Thread::PriorityNormal  -- Normal priority level, all threads by default are created with Normal priority level
     *              - Thread::PriorityHigh    -- Priority above Normal and below Highest
     *              - Thread::PriorityNighest -- The Highest thread priority
     **/
    Thread::eThreadPriority getPriority( void ) const;

//////////////////////////////////////////////////////////////////////////
// static operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief	Search by thread name and return pointer the thread object.
     *          If name could not find, returns NULL
     * \param	threadName	The unique name of thread to search
     * \return	If not NULL, the thread object was found.
     **/
    inline static Thread * findThreadByName( const char * threadName) ;

    /**
     * \brief	Search by thread ID and return pointer the thread object.
     *          If ID could not find, returns NULL
     * \param	threadID    The unique ID of thread to search
     * \return	If not NULL, the thread object was found.
     **/
    inline static Thread * findThreadById( ITEM_ID threadId );

    /**
     * \brief	Search by thread context and return pointer the thread object.
     *          If Context could not find, returns NULL
     * \param	threadContext   The unique Context of thread.
     *                          It contains Process and Thread IDs information
     * \return	If not NULL, the thread object was found.
     **/
    inline static Thread * findThreadByAddress( const ThreadAddress & threadAddres );

    /**
     * \brief   Lookup Thread by thread ID and returns Thread Address object,
     *          which contains information of Process ID and Thread ID.
     * \param	threadID	The ID of thread to get address
     * \return	If found, returns valid thread address object.
     *          Otherwise returns invalid thread address.
     **/
    inline static const ThreadAddress & findThreadAddressById( ITEM_ID threadId );

    /**
     * \brief   Lookup Thread by thread name and returns Thread Address object,
     *          which contains information of Process ID and Thread ID.
     * \param	threadName	The name of thread to get address
     * \return	If found, returns valid thread address object.
     *          Otherwise returns invalid thread address.
     **/
    inline static const ThreadAddress & findThreadAddressByName( const char * threadName );

    /**
     * \brief   Suspends current thread and puts in a sleep mode for specified timeout in milliseconds.
     *          It continues execution when time is expired.
     * \param   msTimeout   Timeout in milliseconds to put thread in sleep mode.
     **/
    static void sleep( unsigned int msTimeout );

    /**
     * \brief   Switches thread processing time.
     *          It does not put thread in sleep, but let other thread take processing time.
     **/
    static void switchThread( void );

    /**
     * \brief   Return the ID of current thread.
     **/
    static ITEM_ID getCurrentThreadId( void );

    /**
     * \brief   Returns the thread object of current thread.
     *          The current thread must be registered in the resource map.
     **/
    static inline Thread * getCurrentThread( void );

    /**
     * \brief   Returns the name of current thread.
     *          If Thread is not registered, returns empty string.
     **/
    static inline const char * getCurrentThreadName( void );

    /**
     * \brief   Returns the address of current thread.
     *          If Thread is not registered, returns invalid address.
     **/
    static inline const ThreadAddress & getCurrentThreadAddress( void );

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
    static const char * getThreadName( ITEM_ID threadId );

    /**
     * \brief   Returns the address of thread by specified ID. 
     *          If Thread is not registered, returns invalid address.
     **/
    static const ThreadAddress & getThreadAddress( ITEM_ID threadId );

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
    static Thread * getFirstThread( ITEM_ID & OUT threadId );

    /**
     * \brief   Returns the next thread element resource map.
     * \param   threadId    ON input this should contain valid thread ID.
     *                      If method succeeds, on output is valid thread ID.
     * \return  If succeed, returns pointer to valid thread object.
     **/
    static Thread * getNextThread( ITEM_ID & IN OUT threadId );

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
    ITEM_ID                 mThreadId;
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
    mutable CriticalSection mSynchObject;
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
     * \brief   Cleans data of Thread object, i.e. reset running flag, invalidates thread info.
     **/
    void _cleanResources( void );

    /**
     * \brief   Creates and Registers Thread. Returns true if succeed
     **/
    bool _createSystemThread( void );

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
     * \brief   POSIX specific thread routine.
     *          This calls _defaultThreadFunction.
     * \param   data    Pointer to the thread object.
     * \return  Returns NULL.
     **/
    static void * _posixThreadRoutine( void * data);

    /**
     * \brief   Windows specific thread routine.
     *          This calls _defaultThreadFunction.
     * \param   data    Pointer to the thread object.
     * \return  Returns thread exit code.
     **/
    static unsigned long _windowsThreadRoutine( void * data );

    /**
     * \brief   Thread starting point.
     * \param   data    Pointer to thread class.
     * \return  Thread exit code. If succeed, returns 0.
     **/
    static unsigned long _defaultThreadFunction( void* data );

    /**
     * \brief	Sets name to new created name. Might be useful during debugging.
     * \param	threadId	The unique ID of thread to set name
     * \param	threadName	The name to set.
     * \return	
     **/
    static void _setThreadName(ITEM_ID threadId, const char* threadName);

    /**
     * \brief   Creates new, returns current or deletes existing Local Thread Storage
     *          object, depending on ownThread parameter. If ownThread is NULL, deletes
     *          Thread Local Storage. If ownThread is a valid pointer and not equal to
     *          Thread::CURRENT_THREAD, creates new storage, otherwise returns existing.
     *          This function is initialized in Thread Procedure (_defaultThreadFunction)
     *          and deleted on exit from thread procedure.
     * \param   ownThread   The Thread Object owning local storage.
     *                      a) if Thread::CURRENT_THREAD,  returns existing local storage;
     *                      b) if NULL, deletes existing local storage (created before);
     *                      c) if pointer of valid Thread Object, creates local storage 
     *                          (called in Thread Procedure).
     *                      Use only with Thread::CURRENT_THREAD
     * \return  The pointer of current Local Storage object.
     **/
    static ThreadLocalStorage * _getThreadLocalStorage(Thread* ownThread = Thread::CURRENT_THREAD);

    /**
     * \brief	Search by thread handle and return pointer the thread object.
     *          If handle could not find, returns NULL
     * \param	threadHandle    The unique handle of thread to search
     * \return	If not NULL, the thread object was found.
     **/
    inline static Thread * _findThreadByHandle( THREADHANDLE threadHandle );

    /**
     * \brief   Searches in resources make thread by ID.
     *          Returns thread handle, if found. Otherwise, returns NULL.
     * \param   threadId    The ID of thread to search.
     * \return  If not NULL, the handle of valid thread returned.
     *          Otherwise, the thread is invalid, meaning not exists, not created or already closed.
     **/
    inline static THREADHANDLE _findThreadHandleById(ITEM_ID threadId);

    /**
     * \brief   System depended call. Closes the handle object of thread.
     *          Makes thread  data invalid.
     **/
    static void _closeHandle( THREADHANDLE handle );

private:
/************************************************************************/
// Resource mapping types, used to control resources, used by thread
/************************************************************************/
    /**
     * \brief   Thread resource mapping by thread ID.
     *          The unique thread ID is set when thread is created
     **/
    typedef TEIdHashMapImpl<Thread *>                                                   ImplMapThreadID;
    typedef TEIdHashMap<Thread *, Thread *, ImplMapThreadID>                            MapThreadID;
    typedef TEResourceMapImpl<ITEM_ID, Thread>                                          ImplThreadIDResource;
    typedef TELockResourceMap<ITEM_ID, Thread, MapThreadID,ImplThreadIDResource>        MapThreadIDResource;
    /**
     * \brief   Thread resource mapping by thread handle. 
     *          The unique thread handle can be used to access thread object.
     **/
    typedef TEPointerHashMapImpl<Thread *>                                              ImplMapThreadHandle;
    typedef TEPointerHashMap<Thread *, Thread *, ImplMapThreadHandle>                   MapThreadHandle;
    typedef TEResourceMapImpl<void *, Thread>                                           ImplThreadHandleResource;
    typedef TELockResourceMap<void *, Thread, MapThreadHandle,ImplThreadHandleResource> MapThreadHandleResource;
    /**
     * \brief   Thread resource mapping by thread name. 
     *          The unique thread name can be used to access thread object.
     **/
    typedef TEStringHashMapImpl<Thread *>                                               ImplMapThreadName;
    typedef TEStringHashMap<Thread *, Thread *, ImplMapThreadName>                      MapThreadName;
    typedef TEResourceMapImpl<String, Thread>                                           ImplThreadNameResource;
    typedef TELockResourceMap<String, Thread, MapThreadName, ImplThreadNameResource>    MapThreadNameResource;

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
    Thread( void );
    Thread( const Thread & /*src*/ );
    const Thread & operator = ( const Thread & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// Thread class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline Thread* Thread::_findThreadByHandle(THREADHANDLE threadHandle)
{
    return Thread::_mapThreadhHandle.findResourceObject(threadHandle);
}

inline THREADHANDLE Thread::_findThreadHandleById(ITEM_ID threadId)
{
    Thread * result = Thread::_mapThreadId.findResourceObject(threadId);
    return (result != NULL ? result->mThreadHandle : NULL);
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

inline ITEM_ID Thread::getId( void ) const
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

inline Thread* Thread::findThreadByName(const char* threadName)
{
    return (threadName != NULL ? Thread::_mapThreadName.findResourceObject(threadName) : NULL);
}

inline Thread* Thread::findThreadById(ITEM_ID threadId)
{
    return Thread::_mapThreadId.findResourceObject(threadId);
}

inline Thread* Thread::findThreadByAddress(const ThreadAddress& threadAddress)
{
    return Thread::findThreadByName(threadAddress.getThreadName());
}

inline const ThreadAddress & Thread::findThreadAddressById(ITEM_ID threadId)
{
    Thread* threadObj = Thread::findThreadById(threadId);
    return (threadObj != NULL ? threadObj->getAddress() : ThreadAddress::INVALID_THREAD_ADDRESS);
}

inline const ThreadAddress& Thread::findThreadAddressByName(const char* threadName)
{
    Thread* threadObj = Thread::findThreadByName(threadName);
    return (threadObj != NULL ? threadObj->getAddress() : ThreadAddress::INVALID_THREAD_ADDRESS);
}

inline void Thread::_setRunning( bool isRunning )
{
    Lock lock(mSynchObject);
    mIsRunning  = isRunning;
}

inline Thread * Thread::getCurrentThread( void )
{
    return Thread::findThreadById(Thread::getCurrentThreadId());
}

inline const char * Thread::getCurrentThreadName( void )
{
    return Thread::getThreadName( Thread::getCurrentThreadId() );
}

inline const ThreadAddress & Thread::getCurrentThreadAddress( void )
{
    return Thread::getThreadAddress( Thread::getCurrentThreadId() );
}

inline const char * Thread::getString( Thread::eThreadPriority threadPriority )
{
    switch ( threadPriority )
    {
    case Thread::PriorityUndefined:
        return "Thread::PriorityUndefined";
    case Thread::PriorityLowest:
        return "Thread::PriorityLowest";
    case Thread::PriorityLow:
        return "Thread::PriorityLow";
    case Thread::PriorityNormal:
        return "Thread::PriorityNormal";
    case Thread::PriorityHigh:
        return "Thread::PriorityHigh";
    case Thread::PriorityHighest:
        return "Thread::PriorityHighest";
    default:
        return "ERR: Invalid Thread::eThreadPriority value!";
    }
}

#endif  // AREG_BASE_THREAD_HPP
