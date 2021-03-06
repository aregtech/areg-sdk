#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/SynchObjects.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              IEResourceLock      - blocking synchronization object interface.
 *              Mutex               - Mutex synchronization object.
 *              SynchEvent          - Event synchronization object.
 *              Semaphore           - Semaphore synchronization object.
 *              CriticalSection     - Critical Section synchronization object.
 *              SpinLock            - Spin-Lock synchronization object.
 *              ResourceLock        - An OS specific resource lock object.
 *              NolockSynchObject   - No Locking synchronization object (makes no locking).
 *              SynchTimer          - Timer synchronization object.
 *              Lock                - Single synchronization auto locking object.
 *              MultiLock           - Multiple synchronization auto locking object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IESynchObject.hpp"

#include <atomic>
#include <mutex>

/**
 * \brief   This file contains synchronization objects used to synchronize data access
 *          in multi-threading environment. All Synchronization objects are instances of 
 *          IESynchObject interface. The instance of IEResourceLock can be 
 *          used in auto-locking objects to synchronize data access.
 *
 *          A special NolockSynchObject is defined to support synchronization functionalities, 
 *          but the object does not block any thread and must not be used in multi-locking operations. 
 *          The purpose of this class to support unified IESynchObject interface and use in containers
 *          that do not require synchronization operations.
 *
 *          Lock and MultiLock classes are supporting auto-locking
 *          functionalities. The locking is called during initialization
 *          of object and automatically released when object is destroyed.
 **/

/************************************************************************
 * List of declared classes and hierarchy
 ************************************************************************/
/* class IESynchObject; */
    class IEResourceLock;
        class Mutex;
        class Semaphore;
        class CriticalSection;
        class SpinLock;
        class ResourceLock;
        class NolockSynchObject;
    class SynchEvent;
    class SynchTimer;

class Lock;
class MultiLock;

//////////////////////////////////////////////////////////////////////////
// IEResourceLock class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API IEResourceLock   : public IESynchObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor. Should not be accessed directly.
     *          Only via derived classes
     **/
    IEResourceLock( IESynchObject::eSyncObject synchObjectType );

public:
    /**
     * \brief   
     **/
    virtual ~IEResourceLock( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
// Pure virtual functions to overwrite
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Attempts to get synchronization object ownership without blocking thread.
     *          If the call is successful, the calling thread takes ownership of synchronization object.
     * \return  If the ownership of synchronization object is successfully taken or 
     *          the current thread already owns the synchronization object, the return value is true.
     *          If another thread already owns the synchronization object, the return value is false.
     **/
    virtual bool tryLock( void );

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    IEResourceLock( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( IEResourceLock );
};

//////////////////////////////////////////////////////////////////////////
// Mutex class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A mutex is a synchronization object which state is set to signaled / locked 
 *          when it is not owned by any thread, and non-signaled / unlocked when it is owned. 
 *          Only one thread at a time can own a mutex object. It is used in mutually exclusive access 
 *          to a shared resource. For example, to prevent two threads from writing to shared memory 
 *          at the same time. Then, each thread waits for ownership of a mutex object, i.e. locks mutex, 
 *          before executing the code to accesses the memory. After writing in the shared memory, 
 *          the thread releases / unlocks the mutex object to let other thread to access same
 *          memory for writing or reading.
 **/
class AREG_API Mutex   : public IEResourceLock
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
friend class MultiLock;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Initialize Synchronization object.
     * \param	lock	If true, current thread will get ownership
     *                  of mutex on initialization
     **/
    explicit Mutex( bool lock = true );

    /**
     * \brief	Destructor
     **/
    virtual ~Mutex( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Locks synchronization object, i.e. current thread
     *          gets mutex ownership. If mutex is owned by other
     *          thread, the current thread will wait timeout milliseconds
     *          and will resume execution.
     *          If timeout is WAIT_INFINITE, current thread will wait
     *          as long, as mutex ownership is not released by owning thread
     * \param	timeout	Timeout in milliseconds to wait.
     * \return	Returns true if current thread successfully got mutex ownership
     **/
    virtual bool lock( unsigned int timeout = NECommon::WAIT_INFINITE ) override;

    /**
     * \brief	Unlocks / Release mutex.
     * \return	Returns true if succeeded.
     **/
    virtual bool unlock( void ) override;

    /**
     * \brief   Attempts to get Mutex object ownership without blocking thread.
     *          If the call is successful, the calling thread takes ownership of Mutex.
     * \return  If the ownership of Mutex is successfully taken or 
     *          the current thread already owns the Mutex, the return value is true.
     *          If another thread already owns the Mutex, the return value is false.
     **/
    virtual bool tryLock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Return true if Mutex is already locked by any thread.
     **/
    inline bool isLocked( void ) const;

    /**
     * \brief   Returns the ID of Thread, which is currently owning mutex
     **/
    inline id_type getOwnerThreadId( void ) const;

//////////////////////////////////////////////////////////////////////////
// Internal operations
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Locks the mutex, takes the ownership.
     * \param   timeout     The timeout in milliseconds to wait for mutext to lock.
     **/
    bool _lockMutex( unsigned int timeout );

    /**
     * \brief   Unlocks the mutex, release the ownership.
     **/
    bool _unlockMutex( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   The ID of thread currently owning mutex
     **/
    std::atomic<id_type>    mOwnerThreadId;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( Mutex );
};

//////////////////////////////////////////////////////////////////////////
// class SynchEvent declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   An event object is a synchronization object which state can be explicitly set 
 *          to signaled or non-signaled.
 *          Followings are the two types of event object:
 *          1.  Manual-reset event: An event object which state remains signaled until it 
 *              is explicitly reset to non-signaled  by unlock() function call. While it is 
 *              signaled, any number of waiting threads, or threads that subsequently specify 
 *              the same event object in one of the wait functions, can be released. 
 *          2.  Auto-reset event: An event object which state remains signaled until a single 
 *              waiting thread is released. Then, the system automatically sets the to non-signaled.
 *              If no threads are waiting, the event object's state remains signaled. If more than one 
 *              thread are waiting, a one of waiting threads is selected. Do not assume a first-in, 
 *              first-out (FIFO) order.
 *
 *          The event object is useful in sending a signal to a thread indicating that a particular 
 *          event has occurred. A single thread can specify different event objects in 
 *          several simultaneous overlapped operations, then use lock() function to wait for the 
 *          state of event object to be signaled. The creating thread specifies the initial state 
 *          of the object in Constructor and whether it is a manual-reset or auto-reset event object.
 **/
class AREG_API SynchEvent  : public IESynchObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Creates either Manual Reset or Auto-reset Event Synchronization object as well as
     *          the initial state of Event -- whether it is signaled or not
     * \param	lock	    If true, the initial state of Event is non-signaled.
     *                      When Event state is non-signaled, any thread trying
     *                      lock Event, will be blocked until Event object
     *                      is not unlocked / signaled by any other thread.
     *                      By default, the initial state is locked / non-signaled
     * \param	autoReset	If true, Event object is auto-reset and whenever
     *                      a single waiting thread is released, it will change
     *                      state to non-signaled.
     *                      By default, creates auto-reset Synchronization Event.
     **/
    explicit SynchEvent (bool lock = true, bool autoReset = true );

    /**
     * \brief   Destructor. Sets Event to signal state first.
     **/
    virtual ~SynchEvent( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	If Event is in signal state, calling thread is not blocked.
     *          Otherwise, thread is block as long, until Event is not signaled
     *          or until timeout is expired. If Timeout is WAIT_INFINITE, thread
     *          will be unlocked only when event signaled / unlocked.
     *          If Event is auto-reset, this call automatically set to non-signaled state 
     * \param	timeout	    Timeout in milliseconds to wait until Event is not
     *                      signaled / unlocked. If WAIT_INFINITE, 
     *                      specify that the wait will not time out. 
     * \return	Returns true if Event was unlocked / signaled and thread was unblock
     *          with no time out or waiting error.
     **/
    virtual bool lock( unsigned int timeout = NECommon::WAIT_INFINITE ) override;

    /**
     * \brief	Unlock Event, i.e. set to signaled state
     * \return	Return true if successfully set event state to signaled.
     **/
    virtual bool unlock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Manually sets event state to signaled. Same as calling Unlock()
     **/
    bool setEvent( void );

    /**
     * \brief   Manually resets state of event, i.e. set is non-signaled.
     **/
    bool resetEvent( void );

    /**
     * \brief   Pulse event once. It it was not set, it will set once and immediately
     *          reset to non-signaled state.
     **/
    void pulseEvent( void );

    /**
     * \brief   Returns true if event is auto-reset
     **/
    inline bool isAutoReset( void ) const;

private:
    
    bool _unlockEvent( void * eventHandle );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Flag, indicating whether event is auto-reset or not
     **/
    const bool  mAutoReset;

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( SynchEvent );
};

//////////////////////////////////////////////////////////////////////////
// class Semaphore declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A semaphore object is a synchronization object that 
 *          maintains a count between zero and a specified maximum value. 
 *          The count is decremented each time a thread completes 
 *          a wait / lock for the semaphore object and incremented 
 *          each time a thread releases / unlocks the semaphore.
 *          When the count reaches zero, no more threads can successfully
 *          wait / lock for the semaphore object state to become signaled.
 *          The state of a semaphore is set to signaled when its count
 *          is greater than zero, and non-signaled when its count is zero.
 *          The semaphore object is useful in controlling a shared resource
 *          that can support a limited number of users. It acts as a 
 *          gate that limits the number of threads sharing the resource 
 *          to a specified maximum number.
 *          Mutex is a Semaphore with limited number of access set to one.
 **/
class AREG_API Semaphore: public IEResourceLock
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
friend class MultiLock;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates Semaphore synchronization object sets the maximum number thread access.
     * \param	maxCount	The maximum number of thread access.
     *                      Semaphore remains in signaled / unlocked
     *                      state, until this number is more than zero.
     *                      When this number is zero, Semaphore is set
     *                      to non-signaled / locked state.
     * \param	initCount	The initial count for the semaphore object.
     *                      This value must be greater than or equal to zero 
     *                      and less than or equal to maxCount. 
     *                      The state of a semaphore is signaled when its count 
     *                      is greater than zero and non-signaled when it is zero.
     *                      The count is decreased by one whenever 
     *                      lock() function releases a thread that was waiting
     *                      for the semaphore. The count is increased by a specified
     *                      amount by calling the unlock() function.
     *                      If this parameter is zero, Semaphore is initially in
     *                      non-signaled / locked state
     **/
    explicit Semaphore( int maxCount, int initCount = 0 );

    /**
     * \brief   Destructor. Unlocks Semaphore and destroy object.
     **/
    virtual ~Semaphore( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	When this function is called, it the calling thread is blocked unless
     *          semaphore count is not greater than zero. And thread is released either
     *          when semaphore is signaled or timeout expired. 
     *          WAIT_INFINITE means wait until semaphore is signaled.
     * \param	timeout     The timeout to wait if semaphore is in non-signaled state.
     *                      Otherwise the lock count decreased and thread is released.
     * \return	Returns true if thread was released because of signaled state of semaphore.
     **/
    virtual bool lock( unsigned int timeout = NECommon::WAIT_INFINITE ) override;

    /**
     * \brief   Unlocks Semaphore, i.e. signals it, and increase lock count number.
     * \return  Returns true if successfully signaled semaphore
     **/
    virtual bool unlock( void ) override;

    /**
     * \brief   Always return false. No implementation for Semaphore.
     **/
    virtual bool tryLock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the maximum lock count of Semaphore object
     **/
    inline long getMaxCount( void ) const;

    /**
     * \brief   Returns the current lock count of Semaphore object.
     **/
    inline long getCurrentCount( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Maximum lock count number of semaphore
     **/
    const long          mMaxCount;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Current lock count number of semaphore
     **/
    std::atomic_long    mCurrCount;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    Semaphore( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( Semaphore );
};

//////////////////////////////////////////////////////////////////////////
// class CriticalSection declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A critical section object provides synchronization 
 *          similar to provided by a mutex object, except 
 *          that a critical section can be used only by the threads
 *          of a single process. Event, mutex, and semaphore objects
 *          can also be used in a single-process application, but 
 *          critical section objects provide a slightly faster, more 
 *          efficient mechanism for mutual-exclusion synchronization. 
 *          Like a mutex object, a critical section object can be owned 
 *          by only one thread at a time, which makes it useful for 
 *          protecting a shared resource from simultaneous access.
 *          Unlike a mutex object, there is no way to tell whether a 
 *          critical section has been abandoned.
 **/
class AREG_API CriticalSection  : public IEResourceLock
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor. Creates and Initialize critical section object
     **/
    CriticalSection( void );

    /**
     * \brief   Destructor. Destroys critical section object
     **/
    virtual ~CriticalSection( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Waits for ownership of critical section object. If the critical
     *          section object is currently owned by another thread, call of
     *          this function cause wait indefinitely for ownership.
     *          In contrast, when a mutex object is used for mutual exclusion,
     *          the lock() function accept a specified time-out interval.
     * \return  CriticalSection always return true.
     **/
    inline bool lock( void );

    /**
     * \brief   Waits for ownership of critical section object. If the critical
     *          section object is currently owned by another thread, call of
     *          this function cause wait indefinitely for ownership.
     *          In contrast, when a mutex object is used for mutual exclusion,
     *          the lock() function accept a specified time-out interval.
     *
     * \param   timeout     NOT USED in case of critical section object.
     * \return  In case of critical section, always return true
     **/
    virtual bool lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/) override;

    /**
     * \brief   Releases ownership of the specified critical section object.
     * \return	In case of critical section, always return true
     **/
    virtual bool unlock( void ) override;

    /**
     * \brief   Attempts to enter a critical section without blocking thread.
     *          If the call is successful, the calling thread 
     *          takes ownership of the critical section.
     * \return  If the critical section is successfully entered or 
     *          the current thread already owns the critical section,
     *          the return value is true.
     *          If another thread already owns the critical section,
     *          the return value is false.
     **/
    virtual bool tryLock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( CriticalSection );
};

//////////////////////////////////////////////////////////////////////////
// class SpinLock declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   In spin lock the thread spins in the loop until the lock is available.
 *          It is the fastest synchronization, but because of spinning, it is not
 *          recommended to lock SpinLock for longer time. Normally, it is locked for
 *          a very short operation. The lock must be unlocked as soon as possible to
 *          let the other threads to take the ownership. SpinLock is similar to mutex,
 *          except that it can be used only by the threads of a single process.
 *          Like a mutex object, a spin-lock object can be owned
 *          by only one thread at a time, which makes it useful for
 *          protecting a shared resource from simultaneous access.
 *          Unlike a mutex object, there is no way to tell whether a
 *          critical section has been abandoned.
 **/
class AREG_API SpinLock: public IEResourceLock
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor. Creates and Initialize spin-lock object.
     **/
    SpinLock( void );

    /**
     * \brief   Destroys spin-lock object
     **/
    virtual ~SpinLock( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits for ownership of spin-lock object. If the spin-lock
     *          object is currently owned by another thread, call of
     *          this function cause wait indefinitely for ownership.
     *          In contrast, when a mutex object is used for mutual exclusion,
     *          the lock() function accept a specified time-out interval.
     * \return  Spin-lock always return true
     **/
    inline bool lock( void );

    /**
     * \brief   Waits for ownership of spin-lock object. If the spin-lock
     *          object is currently owned by another thread, call of
     *          this function cause wait indefinitely for ownership.
     *          In contrast, when a mutex object is used for mutual exclusion,
     *          the lock() function accept a specified time-out interval.
     *
     * \param   timeout     NOT USED in case of critical section object.
     * \return  Spin-lock always return true
     **/
    virtual bool lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ ) override;

    /**
     * \brief   Releases ownership of the spin-lock object.
     * \return  Spin-lock always return true
     **/
    virtual bool unlock( void ) override;

    /**
     * \brief   Attempts to take the spin-lock ownershipt without blocking thread.
     *          If the call is successful, the calling thread
     *          takes ownership of the spin-lock.
     * \return  If current thread successfully has taken the ownership or the thread
     *          already has the ownership of spin-lock, the return value is true.
     *          If another thread already owns the critical section,
     *          the return value is false.
     **/
    virtual bool tryLock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( SpinLock );
};

//////////////////////////////////////////////////////////////////////////
// ResuorceLock class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   ResourceLock is an OS specific synchronization object to
 *          to synchronize resource access. It uses recursive locking
 *          mechanism, so that, when accessing resources in the same
 *          thread context, it is not locked / blocked. This helps to
 *          avoid deadlocks.
 **/
class AREG_API ResourceLock : public    IEResourceLock
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes resource locking synchronization object.
     *          If needed, sets the initial state of resource locked.
     * \param   initLock    If true, sets the initial state of the
     *                      synchronization object to locked.
     **/
    explicit ResourceLock( bool initLock = false );

    /**
     * \brief   Destructor.
     **/
    virtual ~ResourceLock( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Waits for ownership of resource lock object. If resource lock
     *          has no ownership, it takes the ownership and immediately returns.
     *          Otherwise, it waits infinite until the thread takes the ownership.
     *          The recursive call to lock within same thread context will not
     *          block the thread.
     * \param   timeout     NOT USED. I will always wait infinite until the thread
     *                      takes the ownership.
     * \return  Always returns true.
     **/
    virtual bool lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ ) override;

    /**
     * \brief   Releases ownership of the resource lock object.
     * \return  Always returns true.
     **/
    virtual bool unlock( void ) override;

    /**
     * \brief   Attempts to take the resource lock ownershipt without blocking thread.
     *          If the call is succeeds, the calling thread takes the ownership 
     *          of the resource lock object.
     * \return  If current thread successfully has taken the ownership or the resource lock
     *          the method returns true. Otherwise, it returns false. After each there should
     *          be unlock called.
     **/
    virtual bool tryLock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( ResourceLock );
};

//////////////////////////////////////////////////////////////////////////
// NolockSynchObject class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   No Lock Synchronization object is a dummy class
 *          doing no synchronization action, but having implementation
 *          of overrides. Some classes might need having synchronization
 *          of data access and some other might not need.
 *          For this reason, the reference to IESynchObject might be
 *          passed as a main synchronization object and by calling
 *          lock() / unlock() either data access would be really
 *          synchronized or synchronization is imitated / ignored.
 *
 * \note    Do not use this class for Multi-locking (see MultiLock)
 *          The locking might be imitated only by using Lock object
 *          or calling lock() / unlock() directly.
 **/
class AREG_API NolockSynchObject   : public IEResourceLock
{

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Constructor.
     **/
    NolockSynchObject( void );

    /**
     * \brief   Destructor
     **/
    virtual ~NolockSynchObject( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   No real locking when call this method.
     * \return  Spin-lock always return true
     * \return	Always returns true.
     **/
    inline bool lock( void );

    /**
     * \brief   No real locking when call this method.
     * \param	NOT USED.
     * \return	Always returns true.
     **/
    virtual bool lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ ) override;

    /**
     * \brief   No real unlocking.
     * \return	Always returns true.
     **/
    virtual bool unlock( void ) override;

    /**
     * \brief   Always return true. No real locking.
     **/
    virtual bool tryLock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( NolockSynchObject );
};

//////////////////////////////////////////////////////////////////////////
// class SynchTimer declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A waitable timer object is a synchronization object
 *          whose state is set to signaled when the specified
 *          due time arrives. There are two types of waitable
 *          timers that can be created:
 *          manual-reset and synchronization.
 *          A timer of either type can also be a periodic timer.
 *          1.  manual-reset timer: A timer whose state remains
 *              signaled until unlock() is called to establish a new due time.
 *          2.  synchronization timer: A timer whose state remains signaled
 *              until a thread completes a wait operation on the timer object.
 *          3.  periodic timer: A timer that is reactivated each time
 *              the specified period expires, until the timer is reset
 *              or canceled. A periodic timer is either a periodic manual-reset
 *              timer or a periodic synchronization timer.
 **/
class AREG_API SynchTimer: public IESynchObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates Waitable Timer, either manual-reset synchronization or periodic timer.
     * \param	timeMilliseconds	Time in milliseconds of Waitable Timer
     * \param	periodic	        If true, it is periodic timer
     * \param	autoReset	        If true, it is synchronization timer,
     *                              otherwise it is manual-reset timer.
     * \param	initSignaled        If true, the timer is activated and
     *                              set	to signaled state.
     **/
    SynchTimer( unsigned int timeMilliseconds, bool periodic = false, bool autoReset = true, bool initSignaled = true );

    /**
     * \brief   Destructor. Signals and Destroys waitable timer.
     **/
    virtual ~SynchTimer( void );

    //////////////////////////////////////////////////////////////////////////
    // Override operations, IESynchObject interface
    //////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	If waitable timer is in non-signaled / locked state,
     *          the thread will be blocked waiting for timer event, or until
     *          waiting timeout expires.
     *          WAIT_INFINITE means wait until waitable timer is signaled.
     * \param	timeout     The timeout to wait if timer is in non-signaled state.
     * \return	Returns true if thread was released because of signaled state of timer.
     **/
    virtual bool lock( unsigned int timeout = NECommon::WAIT_INFINITE ) override;

    /**
     * \brief   Activates the specified waitable timer.
     *          It will not set timer to signaled state immediately.
     *          When the due time arrives, the timer is signaled and
     *          the thread that set the timer calls the optional completion routine.
     *          To set due time, timer is using parameters passed in constructor.
     * \return  Return true if timer was successfully activate.
     **/
    virtual bool unlock( void ) override;

    //////////////////////////////////////////////////////////////////////////
    // Operations / Attributes
    //////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Activates timer manually. Same as unlock() function call.
     * \return  Returns true if waitable timer successfully activated
     **/
    bool setTimer( void );

    /**
     * \brief   Sets the specified waitable timer to the inactive state.
     *          It does not change the signaled state of the timer.
     *          It stops the timer before it can be set to the signaled state.
     *          Therefore, threads performing a wait operation on the timer
     *          remain waiting until they time out or the timer is reactivated
     *          and its state is set to signaled. If the timer is already in
     *          the signaled state, it remains in that state.
     **/
    bool cancelTimer( void );

    /**
     * \brief   Returns due time in milliseconds of waitable timer
     **/
    inline unsigned int dueTime( void ) const;

    /**
     * \brief   If true, the waitable timer is periodic
     **/
    bool isPeriodic( void ) const;

    /**
     * \brief   If true, it is auto-reset waitable timer
     *          Otherwise, it is manual reset.
     **/
    bool isAutoreset( void ) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Due time in milliseconds
     **/
    const unsigned int  mTimeMilliseconds;
    /**
     * \brief   Flag containing information whether timer is periodic or not
     **/
    const bool          mIsPeriodic;
    /**
     * \brief   Flag containing information whether timer is auto-reset or not.
     **/
    const bool          mIsAutoReset;

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Forbidden methods
    //////////////////////////////////////////////////////////////////////////
private:
    SynchTimer( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SynchTimer );
};

//////////////////////////////////////////////////////////////////////////
// Lock class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Class to use auto-locking for a single locking object.
 *          This class can be used for all instances of IESynchObject.
 *          The purpose of using this object is to lock synchronization
 *          object in a certain code scope.
 *
 * \example Mutex use
 *
 *          In this example bellow MyClass contains Mutex as a synchronization object
 *          Any time thread is calling foo() or bar() methods ot MyClass, on funtion entry
 *          Single Synchronization Object Locking instance is created,
 *          which gets a reference to the Mutex object.
 *          By default, the auto-locking is enable, and by this current thread will try
 *          to get Mutext ownership and will call lock() function during initialization.
 *          As soon as thread gets ownership, it will continue code execution in foo() and bar()
 *          function. And as soon as thread leaves these function, i.e. gets out of function scope,
 *          the destructor of locking object will automatically release mutex ownership
 *
 * \code[.cpp]
 *
 *          class MyClass
 *          {
 *          public:
 *              MyClass( void );
 *              void foo( void );
 *              void bar( void );
 *
 *          private:
 *              Mutex mMutex;
 *          };
 *
 *          MyClass::MyClass( void )
 *              : mMutex(true)
 *          {   ; }
 *
 *          void MyClass::foo( void )
 *          {
 *              Lock lock(mMutex);
 *              // perform actions here
 *          }
 *
 *          void MyClass::bar( void )
 *          {
 *              Lock lock(mMutex);
 *              // perform actions here
 *          }
 **/
class AREG_API Lock
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor. Initialize single locking instance.
     *          If auto-locking flag is set, it will immediately
     *          lock instance of synchronization object and 
     *          and unlock in destructor
     * \param	syncObj	    Reference of Synchronization object instance
     * \param	autoLock	If set true, synchronization object will be
     *                      locked immediately in the constructor and 
     *                      unlocked in destructor.
     *                      Otherwise, lock and unlock should be called
     *                      manually. Note, that if this flag is false,
     *                      synchronization will not be automatically
     *                      unlocked in destructor
     **/
    explicit Lock( IESynchObject &syncObj, bool autoLock = true );

    /**
     * \brief   Destructor. If auto-locking was enabled, it will call
     *          unlock() method of synchronization object to release
     *          ownership of object.
     **/
    ~Lock( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Called to lock synchronization object manually
     * \param	timeout	The timeout to wait during locking.
     *                  Has same meaning as timeout parameter
     *                  in all classes of synchronization object.
     * \return	Returns true if synchronization object successfully locked
     **/
    inline bool lock( unsigned int timeout = NECommon::WAIT_INFINITE );

    /**
     * \brief   Called to unlock synchronization object manually
     * \return  Returns true if synchronization object successfully unlocked.
     **/
    inline bool unlock( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Reference to Synchronization object passed in constructor
     **/
    IESynchObject &   mSynchObject;
    /**
     * \brief   Auto-locking flag. Indicates whether synchronization
     *          object is locked / unlocked automatically or manually
     *          If true, the object automatically is locked / unlocked
     *          in constructor and in destructor
     **/
    const bool          mAutoLock;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    Lock( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( Lock );
};

//////////////////////////////////////////////////////////////////////////
// MultiLock class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Class to use auto-locking for multiple locking object.
 *          This class is getting list of synchronization objects
 *          and on lock waits either for all objects to be signaled
 *          or  only one of them. The principle of auto-locking is same
 *          at it is for locking single synchronization object.
 *          The parameter list can contain any type of synchronization
 *          object, except Critical Section.
 *
 * \see     Lock
 **/
class AREG_API MultiLock
{
//////////////////////////////////////////////////////////////////////////
// Internally defined types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   MultiLock::eLockedState
     *          The current locking state of every single synchronization object
     **/
    typedef enum class E_LockedState : int
    {
          StateUnlocked = 0     //!< Unlocked state
        , StateLocked   = 1     //!< Locked state
    } eLockedState;

public:
    /**
     * \brief   MultiLock::LOCK_INDEX_INVALID
     *          Invalid index of synchronization list
     **/
    static constexpr int    LOCK_INDEX_INVALID          = -1;

    /**
     * \brief   MultiLock::LOCK_INDEX_COMPLETION
     *          The completion routine index.
     *          Returned if waiting function returns WAIT_IO_COMPLETION
     **/
    static constexpr int    LOCK_INDEX_COMPLETION      = -2;
    /**
     * \brief   MultiLock::LOCK_INDEX_TIMEOUT
     *          The index, indicating waiting timeout.
     **/
    static constexpr int    LOCK_INDEX_TIMEOUT         = -3;
    /**
     * \brief   MultiLock::LOCK_INDEX_ALL
     *          All synchronization objects are locked.
     *          Same as MAX_SIZE_OF_ARRAY (64)
     **/
    static constexpr int    LOCK_INDEX_ALL             = NECommon::MAXIMUM_WAITING_OBJECTS;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Initializes Multi-lock object, receives list of synchronization 
     *          objects and flag whether it should initially lock or not. 
     *          If requested to lock, it will wait for all objects to be signaled.
     *
     * \note    There must be no Critical Section synchronization object in the list. 
     *          Otherwise, assertion is raised.
     *
     * \param	pObjects	List of Synchronization objects
     * \param	count	    Number of Synchronization objects in the list
     * \param	autoLock	If true, it will automatically lock all
     *                      synchronization objects and wait for all objects
     *                      to be signaled.
     **/
    MultiLock( IESynchObject* pObjects[], int count, bool autoLock = true );

    /**
     * \brief   Destructor. If auto-lock is enabled, unlocks all synchronization
     *          objects and free resources.
     **/
    ~MultiLock( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Locks all synchronization objects wait until they are signaled.
     *          The call of function can block calling thread either until
     *          at least one object is signaled or all of them, depending on
     *          passed flag.
     * \param	timeout	    Timeout to wait for signaled objects
     * \param	waitForAll	If true, it will wait until all objects are signaled
     * \param   isAlertable If this parameter is true and the thread is in the 
     *                      waiting state, the function returns when the system 
     *                      queues an I/O completion routine or APC, and 
     *                      the thread runs the routine or function. 
     *                      Otherwise, the function does not return until
     *                      either one or all objects are not signaled,
     *                      depending on waitForAll parameter.
     *
     * \return	Returns the index of locked synchronization object in array.
     *          If return value is LOCK_INDEX_INVALID (invalid index), there was 
     *          an error locking and operation failed.
     *          If return value is LOCK_INDEX_ALL (all objects), all objects are
     *          locked / signaled.
     *          Otherwise, it returns valid index of locked object in array.
     **/
    int lock( unsigned int timeout = NECommon::WAIT_INFINITE, bool waitForAll = false, bool isAlertable = false );

    /**
     * \brief   Unlocks every synchronization object, which was locked before
     **/
    bool unlock( void );

    /**
     * \brief   Unlocks certain entry in the synchronization objects list.
     * \param   index   The entry of previously locked object to unlock
     * \brief   Returns true if operation succeeded.
     **/
    bool unlock(int index);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Locking state of every object within array.
     **/
    eLockedState                mLockedStates[NECommon::MAXIMUM_WAITING_OBJECTS];
    /**
     * \brief   List of synchronization objects passed on initialization
     **/
    IESynchObject * const *     mSyncObjArray;
    /**
     * \brief   Size of synchronization object. 
     *          Cannot be more than MAX_SIZE_OF_ARRAY (64)
     **/
    const int                   mSizeCount;
    /**
     * \brief   Flag, indicating whether auto-locking is enabled or disabled.
     **/
    const bool                  mAutoLock;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    MultiLock( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( MultiLock );
};

//////////////////////////////////////////////////////////////////////////
// Inline functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mutex class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool Mutex::isLocked( void ) const
{
    return (mOwnerThreadId.load() != 0);
}

inline id_type Mutex::getOwnerThreadId( void ) const
{
    return mOwnerThreadId.load();
}

//////////////////////////////////////////////////////////////////////////
// SynchEvent class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool SynchEvent::isAutoReset( void ) const
{
    return mAutoReset;
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class inline functions
//////////////////////////////////////////////////////////////////////////
inline long Semaphore::getMaxCount( void ) const
{
    return mMaxCount;
}

inline long Semaphore::getCurrentCount( void ) const
{
    return mCurrCount.load();
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer class inline functions
//////////////////////////////////////////////////////////////////////////
inline unsigned int SynchTimer::dueTime( void ) const
{
    return mTimeMilliseconds;
}

inline bool SynchTimer::isPeriodic( void ) const
{
    return mIsPeriodic;
}

inline bool SynchTimer::isAutoreset( void ) const
{
    return mIsAutoReset;
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool CriticalSection::lock( void )
{
    return lock( NECommon::WAIT_INFINITE );
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool SpinLock::lock( void )
{
    return lock(NECommon::WAIT_INFINITE);
}

//////////////////////////////////////////////////////////////////////////
// ResourceLock class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool NolockSynchObject::lock( void )
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
// Lock class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool Lock::lock(unsigned int timeout /* = NECommon::WAIT_INFINITE */)
{
    return mSynchObject.lock(timeout);
}

inline bool Lock::unlock( void )
{
    return mSynchObject.unlock();
}
