#ifndef AREG_BASE_SYNCHOBJECTS_HPP
#define AREG_BASE_SYNCHOBJECTS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
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
#include <chrono>

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
class Wait;

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
     * \param	initLock    If true, current thread will get ownership
     *                      of the mutex on initialization.
     **/
    explicit Mutex( bool initLock = true );

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
     *          as long, as mutex ownership is not released by owning thread.
     * 
     * \param	timeout	The timeout in milliseconds to wait.
     * \return	Returns true if current thread successfully got mutex ownership
     **/
    inline virtual bool lock( unsigned int timeout = NECommon::WAIT_INFINITE ) override;

    /**
     * \brief	Unlocks / Release mutex.
     * \return	Returns true if succeeded.
     **/
    inline virtual bool unlock( void ) override;

    /**
     * \brief   Attempts to get Mutex object ownership without blocking thread.
     *          If the call is successful, the calling thread takes ownership of Mutex.
     * \return  If the ownership of Mutex is successfully taken or 
     *          the current thread already owns the Mutex, the return value is true.
     *          If another thread already owns the Mutex, the return value is false.
     **/
    inline virtual bool tryLock( void ) override;

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
// Hidden or OS specific implementations
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Locks the mutex, takes the ownership.
     * \param   timeout     The timeout in milliseconds to wait for mutex to lock.
     **/
    bool _osLockMutex( unsigned int timeout );

    /**
     * \brief   Unlocks the mutex, release the ownership.
     **/
    bool _osUnlockMutex( void );

    /**
     * \brief   OS specific method to create mutex.
     *          The thread immediately gets the ownership of the mutex if
     *          the parameter 'initLock' is true.
     **/
    void _osCreateMutex( bool initLock );

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
     * \param	initLock    If true, the initial state of Event is non-signaled.
     *                      When Event state is non-signaled, any thread trying
     *                      lock Event, will be blocked until Event object
     *                      is unlocked / signaled by any other thread.
     *                      By default, the initial state is locked / non-signaled
     * \param	autoReset	If true, Event object is auto-reset and whenever
     *                      a single waiting thread is released, it will change
     *                      state to non-signaled.
     *                      By default, creates auto-reset Synchronization Event.
     **/
    explicit SynchEvent ( bool initLock = true, bool autoReset = true );

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
     * \param	timeout	    The timeout in milliseconds to wait until Event is not
     *                      signaled / unlocked. If WAIT_INFINITE, 
     *                      specify that the wait will not time out. 
     * \return	Returns true if Event was unlocked / signaled and thread was unblock
     *          with no time out or waiting error.
     **/
    inline virtual bool lock( unsigned int timeout = NECommon::WAIT_INFINITE ) override;

    /**
     * \brief	Unlock Event, i.e. set to signaled state
     * \return	Return true if successfully set event state to signaled.
     **/
    inline virtual bool unlock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Manually sets event state to signaled. Same as calling Unlock()
     **/
    inline bool setEvent( void );

    /**
     * \brief   Manually resets state of event, i.e. set is non-signaled.
     **/
    inline bool resetEvent( void );

    /**
     * \brief   Pulse event once. If it was not set, it sets once and immediately
     *          reset to non-signaled state.
     **/
    inline void pulseEvent( void );

    /**
     * \brief   Returns true if event is auto-reset
     **/
    inline bool isAutoReset( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden or OS specific implementations
//////////////////////////////////////////////////////////////////////////
private:
    
    /**
     * \brief   The OS specific call to create an event.
     *          The event is in non-signaled state if the 'initLock' parameter
     *          is true. Otherwise, it is created in signaled state.
     **/
    void _osCreateEvent( bool initLock );

    /**
     * \brief   The OS specific call to set event and release thread(s).
     * \param   evenHandle      The handle of event object to set.
     **/
    bool _osUnlockEvent( void * eventHandle );

    /**
     * \brief   The OS specific call to lock threads and wait for event.
     * \param   timeout     The timeout in milliseconds to wait for event.
     * \return  Returns true if during timeout the event was fired.
     **/
    bool _osLockEvent( unsigned int timeout );

    /**
     * \brief   The OS specific call to set the even in a signaled state.
     **/
    bool _osSetEvent( void );

    /**
     * \brief   The OS specific call to reset the event, i.e. to set in non-signaled state.
     **/
    bool _osResetEvent( void );

    /**
     * \brief   The OS specific call to pulse one time the event to be signaled and release a thread,
     *          then immediately switch to a non-signaled state.
     **/
    void _osPulseEvent( void );

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
    inline virtual bool lock( unsigned int timeout = NECommon::WAIT_INFINITE ) override;

    /**
     * \brief   Unlocks Semaphore, i.e. signals it, and increase lock count number.
     * \return  Returns true if successfully signaled semaphore
     **/
    inline virtual bool unlock( void ) override;

    /**
     * \brief   Always return false. No implementation for Semaphore.
     **/
    inline virtual bool tryLock( void ) override;

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
// Hidden or OS specific methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The OS specific implementation to create a semaphore.
     **/
    void _osCreateSemaphore( void );

    /**
     * \brief   The OS specific implementation to release a semaphore.
     **/
    void _osReleaseSemaphore( void );

    /**
     * \brief   Takes the semaphore ownership and locks it. If the semaphore is
     *          already locked, wait for specified timeout and returns false.
     **/
    bool _osLock( unsigned int timeout );

    /**
     * \brief   Unlocks previousely locked semaphore, so that the other waiting threads
     *          can take the ownership and continue execution.
     **/
    bool _osUnlock( void );

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
// Hidden or OS specific calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The OS specific call to create critical section.
     **/
    void _osCreateCriticalSection( void );

    /**
     * \brief   The OS specific call to release critical secion.
     **/
    void _osReleaseCriticalSection( void );

    /**
     * \brief   The OS specific implementation to enter and lock critical section.
     **/
    bool _osLock( void );

    /**
     * \brief   The OS specific implementation to enter and lock critical section.
     **/
    bool _osUnlock( void );

    /**
     * \brief   The OS specific implementation to try to lock the critical section.
     **/
    bool _osTryLock( void );

    //////////////////////////////////////////////////////////////////////////
// Forbidden method calls
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
    virtual ~SpinLock( void ) = default;

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
    inline virtual bool unlock( void ) override;

    /**
     * \brief   Attempts to take the spin-lock ownership without blocking thread.
     *          If the call is successful, the calling thread
     *          takes ownership of the spin-lock.
     * \return  If current thread successfully has taken the ownership or the thread
     *          already has the ownership of spin-lock, the return value is true.
     *          If another thread already owns the critical section,
     *          the return value is false.
     **/
    inline virtual bool tryLock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    //! An atomic object to use for locking
    std::atomic_bool    mLock;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

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
    inline virtual bool lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ ) override;

    /**
     * \brief   Releases ownership of the resource lock object.
     * \return  Always returns true.
     **/
    inline virtual bool unlock( void ) override;

    /**
     * \brief   Attempts to take the resource lock ownership without blocking thread.
     *          If the call is succeeds, the calling thread takes the ownership 
     *          of the resource lock object.
     * \return  If current thread successfully has taken the ownership or the resource lock
     *          the method returns true. Otherwise, it returns false. After each there should
     *          be unlock called.
     **/
    inline virtual bool tryLock( void ) override;

private:

    /**
     * \brief   Creates the resource lock object.
     *          May set initially into the monitor more.
     **/
    void _osCreateResourceLock( bool initLock );

    /**
     * \brief   Releases the not used resource lock.
     **/
    void _osReleaseResourceLock( void );

    /**
     * \brief   Call to lock the resource. It suspends the calling thread if the resources is already locked.
     *          If it is locked, the thread wait for 'timeout' in milliseconds and returns false.
     *          Returns true if succeeded to lock the resource and take the ownership.
     **/
    bool _osLock( unsigned int timeout );

    /**
     * \brief   Call to unlock previosely locked resources. It releases resource lock and lets the other threads
     *          to lock and take the ownership.
     **/
    bool _osUnlock( void );

    /**
     * \brief   Tries to lock the resource lock. If resource lock is free, it locks and immediately returns true.
     *          Otherwise, it is not locked and immediately returns false.
     **/
    bool _osTryLock( void );

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
    virtual ~NolockSynchObject( void ) = default;

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
    inline virtual bool lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ ) override;

    /**
     * \brief   No real unlocking.
     * \return	Always returns true.
     **/
    inline virtual bool unlock( void ) override;

    /**
     * \brief   Always return true. No real locking.
     **/
    inline virtual bool tryLock( void ) override;

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
     * \param	msTimeout	Time in milliseconds of Waitable Timer
     * \param	isPeriodic  If true, it is periodic timer
     * \param	isAutoReset	If true, it is synchronization timer, otherwise it is manual-reset timer.
     * \param   isSteady    If true, it uses steady high resolution timer;
     **/
    SynchTimer( unsigned int msTimeout, bool isPeriodic = false, bool isAutoReset = true, bool isSteady = true );

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
    inline virtual bool lock( unsigned int timeout = NECommon::WAIT_INFINITE ) override;

    /**
     * \brief   Activates the specified waitable timer.
     *          It will not set timer to signaled state immediately.
     *          When the due time arrives, the timer is signaled and
     *          the thread that set the timer calls the optional completion routine.
     *          To set due time, timer is using parameters passed in constructor.
     * \return  Return true if timer was successfully activate.
     **/
    inline virtual bool unlock( void ) override;

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Activates timer manually. Same as unlock() function call.
     * \return  Returns true if waitable timer successfully activated
     **/
    inline bool setTimer( void );

    /**
     * \brief   Sets the specified waitable timer to the inactive state.
     *          It does not change the signaled state of the timer.
     *          It stops the timer before it can be set to the signaled state.
     *          Therefore, threads performing a wait operation on the timer
     *          remain waiting until they time out or the timer is reactivated
     *          and its state is set to signaled. If the timer is already in
     *          the signaled state, it remains in that state.
     **/
    inline bool cancelTimer( void );

    /**
     * \brief   Returns due time in milliseconds of waitable timer
     **/
    inline unsigned int dueTime( void ) const;

    /**
     * \brief   If true, the waitable timer is periodic
     **/
    inline bool isPeriodic( void ) const;

    /**
     * \brief   If true, it is auto-reset waitable timer
     *          Otherwise, it is manual reset.
     **/
    inline bool isAutoreset( void ) const;

//////////////////////////////////////////////////////////////////////////
// OS specific hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS spceific call to create synchronization timer. 
     **/
    void _osCreateTimer( bool isSteady );

    /**
     * \brief   OS specific call to release the synchronization timer.
     **/
    void _osReleaseTime( void );

    /**
     * \brief   Call to lock the thread for specified timeout. If timer is fired, it releases
     *          the thread without waiting timeout and returns true.
     **/
    bool _osLock( unsigned int timeout );

    /**
     * \brief   Sets the timer with the timeout specified in the constructor.
     **/
    bool _osSetTimer( void );

    /**
     * \brief   Cancels the timer if did not expired, so that the locked threads are released.
     *          The same method is called to unlock the timer.
     **/
    bool _osCancelTimer( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Timeout in milliseconds
     **/
    const unsigned int  mTimeout;
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
 *          Any time thread is calling foo() or bar() methods of MyClass, on function entry
 *          Single Synchronization Object Locking instance is created,
 *          which gets a reference to the Mutex object.
 *          By default, the auto-locking is enable, and by this current thread will try
 *          to get Mutex ownership and will call lock() function during initialization.
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
    enum class eLockedState : int
    {
          StateUnlocked = 0     //!< Unlocked state
        , StateLocked   = 1     //!< Locked state
    };

public:
    /**
     * \brief   MultiLock::LOCK_INDEX_INVALID
     *          Invalid index of synchronization list
     **/
    static constexpr int    LOCK_INDEX_INVALID      { -1 };

    /**
     * \brief   MultiLock::LOCK_INDEX_COMPLETION
     *          The completion routine index.
     *          Returned if waiting function returns WAIT_IO_COMPLETION
     **/
    static constexpr int    LOCK_INDEX_COMPLETION   { -2 };
    /**
     * \brief   MultiLock::LOCK_INDEX_TIMEOUT
     *          The index, indicating waiting timeout.
     **/
    static constexpr int    LOCK_INDEX_TIMEOUT      { -3 };
    /**
     * \brief   MultiLock::LOCK_INDEX_ALL
     *          All synchronization objects are locked.
     *          Same as MAX_SIZE_OF_ARRAY (64)
     **/
    static constexpr int    LOCK_INDEX_ALL          { NECommon::MAXIMUM_WAITING_OBJECTS };

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
     * \param	timeout	    The timeout to wait for signaled objects
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
    inline int lock( unsigned int timeout = NECommon::WAIT_INFINITE, bool waitForAll = false, bool isAlertable = false );

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
     * \brief   The OS specific call to lock multipe synchronization objects.
     *          It returns the index of locking object, that the thread has
     *          taken the ownership.
     * \pram    timeout     Timeout in milliseconds to wait. If expired,
     *                      returns timeout expired value withoud locking.
     * \param   waitForAll  It true, waits for all objects.
     * \param   isAlertable If true and thread is locked it returns the
     *                      complition routine index.
     **/
    int _osLock( unsigned int timeout, bool waitForAll, bool isAlertable );

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
// Wait class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Wait puts the thread in a waiting state for seconds, milliseconds
 *          or microseconds. Do not use this object if the thread should sleep
 *          for longer time like minutes or even ten seconds. Use this object
 *          for a very short time where the high resolution timer matters.
 *          The minimum timeout to suspend the calling thread is 1 microsecond.
 *          The object ignores passed timeouts in nanoseconds and rounds
 *          it to microseconds. The calling thread continues execution when 
 *          the timeout expires.
 *          The Wait object is not designed to use for synchronization.
 **/
class AREG_API Wait
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    //!< The high resolution clock, close to real-time
    using SteadyTime    = std::chrono::steady_clock::time_point;
    //!< The system clock, with higher precision
    using SystemTime    = std::chrono::system_clock::time_point;
    //!< Duration in nanoseconds.
    using Duration      = std::chrono::nanoseconds;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    //!< One nanosecond duration
    static constexpr Duration   ONE_NS      { NECommon::DURATION_1_NS };
    //!< One microsecond duration
    static constexpr Duration   ONE_MUS     { NECommon::DURATION_1_MICRO };
    //!< One millisecond duration
    static constexpr Duration   ONE_MS      { NECommon::DURATION_1_MILLI };
    //!< One second duration
    static constexpr Duration   ONE_SEC     { NECommon::DURATION_1_SEC };
    //!< The minimum waiting timeout
    static constexpr Duration   MIN_WAIT    { ONE_MS  * 5 };

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

    //!< The waiting results
    enum class eWaitResult
    {
          WaitInvalid   = 0 //!< No waiting, due to invalid timeout
        , WaitIgnored   = 1 //!< The waiting timeout is value, but ignored waiting, because timeout in nanoseconds
        , WaitInMilli   = 2 //!< Wait thread in milliseconds or longer
        , WaitInMicro   = 3 //!< Wait thread in microseconds up to MIN_WAIT.
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    Wait(void);
    ~Wait(void);

//////////////////////////////////////////////////////////////////////////
// Static operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts the system clock to high resolution steady clock.
     * \param   time    The system time to convert.
     * \return  Returns converted high resolution steady clock.
     **/
    static inline Wait::SystemTime convertToSystemClock(const Wait::SteadyTime& time);
    
    /**
     * \brief   Converts the high resolution steady clock to system clock .
     * \param   time    The high resolution steady time to convert.
     * \return  Returns converted system clock.
     **/
    static inline Wait::SteadyTime convertToSteadyClock(const Wait::SystemTime& time);

    /**
     * \brief   Calculates and returns time difference in nanoseconds when the 
     *          method is called until the specified steady high-resolution time
     *          in the future. Returns negative value is specified future time 
     *          is already passed.
     * 
     * \param   future  The steady high-resolution time in future.
     * \return  The time difference in nanoseconds.
     **/
    static inline Wait::Duration fromNow(const Wait::SteadyTime& future);

    /**
     * \brief   Calculates and returns time difference in nanoseconds of the steady
     *          high-resolution time in the passed until the time when the method 
     *          is called. Returns negative value if specified passed time is in the
     *          future (i.e. did not reach).
     *
     * \param   passed  The steady high-resolution time in passed.
     * \return  The time difference in nanoseconds.
     **/
    static inline Wait::Duration untilNow(const Wait::SteadyTime& past);

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Suspends the thread from further execution for specified timeout
     *          in milliseconds.
     * 
     * \param   ms      The timeout in milliseconds.
     * \return  Returns waiting results. Any value, which is not equal to eWaitResult::WaitInvalid
     *          is considered successful operation.
     **/
    inline Wait::eWaitResult wait(uint32_t ms) const;

    /**
     * \brief   Suspends the thread from further execution for specified timeout
     *          in nanoseconds. The nanosecond part of the timeout is ignored and
     *          rounded to microseconds, so that the thread is suspended from the
     *          execution in microseconds.
     *
     *          The accuracy of timeout depends on the system and supported feature
     *          of the hardware.
     *
     * \param   timeout     The timeout in microseconds with nanoseconds duration.
     * \return  Returns waiting results. Any value, which is not equal to eWaitResult::WaitInvalid
     *          is considered successful operation.
     **/
    inline Wait::eWaitResult waitFor(Wait::Duration timeout) const;

    /**
     * \brief   Suspends the thread from execution until reached the steady high 
     *          resolution timer.
     *
     * \param   future  The time in the future. The thread is not suspended
     *                  if difference between time in the future is less than
     *                  the time when the method is called or the difference
     *                  is in nanoseconds. The minimum timeout should be
     *                  1 microsecond.
     * \return  Returns waiting results. Any value, which is not equal to eWaitResult::WaitInvalid
     *          is considered successful operation.
     **/
    inline Wait::eWaitResult waitUntil(const Wait::SteadyTime& future) const;

//////////////////////////////////////////////////////////////////////////
// Hidden OS dependent calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes the timer object depending on the OS.
     **/
    void _osInitTimer(void);
    /**
     * \brief   Releases the timer object depending on the OS.
     **/
    void _osReleaseTimer(void);
    /**
     * \brief   OS dependent implementation of thread waiting.
     *          The accuracy depends on the OS and hardware provided features.
     * \param   time    The time in the future to suspend thread and wait.
     * \return  Returns waiting results. Any value, which is not equal to eWaitResult::WaitInvalid
     *          is considered successful operation.
     **/
    eWaitResult _osWaitFor(const Wait::Duration& timeout) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< OS dependent timer.
    TIMERHANDLE mTimer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(Wait);
};

//////////////////////////////////////////////////////////////////////////
// Inline functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mutex class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool Mutex::lock( unsigned int timeout /* = NECommon::WAIT_INFINITE */ )
{
    ASSERT( mSynchObject != nullptr );
    return _osLockMutex( timeout );
}

inline bool Mutex::tryLock( void )
{
    return lock( NECommon::DO_NOT_WAIT );
}

inline bool Mutex::unlock( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osUnlockMutex( );
}

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

inline bool SynchEvent::unlock( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osUnlockEvent( mSynchObject );
}

inline bool SynchEvent::lock( unsigned int timeout /* = NECommon::WAIT_INFINITE */ )
{
    ASSERT( mSynchObject != nullptr );
    return _osLockEvent( timeout );
}

inline bool SynchEvent::setEvent( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osSetEvent( );
}

inline bool SynchEvent::resetEvent( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osResetEvent( );
}

inline void SynchEvent::pulseEvent( void )
{
    ASSERT( mSynchObject != nullptr );
    _osPulseEvent( );
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
// CriticalSection class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool CriticalSection::lock( unsigned int  /*timeout = NECommon::WAIT_INFINITE */ )
{
    ASSERT( mSynchObject != nullptr );
    return _osLock( );
}

inline bool CriticalSection::unlock( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osUnlock( );
}

inline bool CriticalSection::tryLock( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osTryLock( );
}

inline bool CriticalSection::lock( void )
{
    return lock( NECommon::WAIT_INFINITE );
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool SpinLock::unlock( void )
{
    mLock.store( false, std::memory_order_release );
    return true;
}

inline bool SpinLock::tryLock( void )
{
    return ((mLock.load( std::memory_order_relaxed ) == false) && (mLock.exchange( true, std::memory_order_acquire ) == false));
}

inline bool SpinLock::lock( void )
{
    return lock( NECommon::WAIT_INFINITE );
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool ResourceLock::lock( unsigned int timeout /*= NECommon::WAIT_INFINITE */ )
{
    ASSERT( mSynchObject != nullptr );
    return _osLock( timeout );
}

inline bool ResourceLock::unlock( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osUnlock( );
}

inline bool ResourceLock::tryLock( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osTryLock( );
}

//////////////////////////////////////////////////////////////////////////
// ResourceLock class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool NolockSynchObject::lock( void )
{
    return true;
}

inline bool NolockSynchObject::lock( unsigned int /*timeout = NECommon::WAIT_INFINITE*/ )
{
    return true;
}

inline bool NolockSynchObject::unlock( void )
{
    return true;
}

inline bool NolockSynchObject::tryLock( void )
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
// SynchTimer class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool SynchTimer::lock( unsigned int timeout /* = NECommon::WAIT_INFINITE */ )
{
    ASSERT( mSynchObject != nullptr );
    return _osLock( timeout );
}

inline bool SynchTimer::unlock( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osCancelTimer( );
}

inline bool SynchTimer::setTimer( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osSetTimer( );
}

inline bool SynchTimer::cancelTimer( void )
{
    ASSERT( mSynchObject != nullptr );
    return _osCancelTimer( );
}

inline unsigned int SynchTimer::dueTime( void ) const
{
    return mTimeout;
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

//////////////////////////////////////////////////////////////////////////
// MultiLock class inline functions
//////////////////////////////////////////////////////////////////////////
inline int MultiLock::lock( unsigned int timeout /* = NECommon::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/ )
{
    ASSERT( mSizeCount != 0 );
    return _osLock( timeout, waitForAll, isAlertable );
}

//////////////////////////////////////////////////////////////////////////
// Wait class inline functions
//////////////////////////////////////////////////////////////////////////

inline Wait::SystemTime Wait::convertToSystemClock(const Wait::SteadyTime& time)
{
    SystemTime result = std::chrono::system_clock::now();
    SteadyTime steady = std::chrono::steady_clock::now();
    return std::chrono::time_point_cast<SystemTime::duration>(result + (time - steady));
}

inline Wait::SteadyTime Wait::convertToSteadyClock(const Wait::SystemTime& time)
{
    SteadyTime result = std::chrono::steady_clock::now();
    SystemTime system = std::chrono::system_clock::now();
    return std::chrono::time_point_cast<SteadyTime::duration>(result + (time - system));
}

inline Wait::Duration Wait::fromNow(const Wait::SteadyTime& future)
{
    return (future - std::chrono::steady_clock::now());
}

inline Wait::Duration Wait::untilNow(const Wait::SteadyTime& past)
{
    return (std::chrono::steady_clock::now() - past);
}

inline Wait::eWaitResult Wait::wait(uint32_t ms) const
{
    return _osWaitFor(Wait::Duration{ ms * Wait::ONE_MS });
}

inline Wait::eWaitResult Wait::waitFor(Wait::Duration timeout) const
{
    return _osWaitFor(timeout);
}

inline Wait::eWaitResult Wait::waitUntil(const Wait::SteadyTime& future) const
{
    return _osWaitFor(future - std::chrono::steady_clock::now());
}

#endif  // AREG_BASE_SYNCHOBJECTS_HPP
