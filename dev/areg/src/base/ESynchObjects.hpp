#ifndef AREG_BASE_ESYNCHOBJECTS_HPP
#define AREG_BASE_ESYNCHOBJECTS_HPP
/************************************************************************
 * \file        areg/src/base/ESynchObjects.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              IEBlockingSynchObject  - blocking synchronization object interface
 *              CEMutex                - Mutex synchronization object
 *              CESynchEvent           - Event synchronization object
 *              CESemaphore            - Semaphore synchronization object
 *              CESynchTimer           - Timer synchronization object
 *              CECriticalSection      - Critical Section synchronization object
 *              IEResourceLock         - Base resource locking class
 *              CEResourceLock         - Operating system dependent resource locking object.
 *              CENolockSynchObject    - No Locking synchronization object (makes no locking)
 *              CELock                 - Single synchronization auto locking object.
 *              CEMultiLock            - Multiple synchronization auto locking object.
 *              CEInterlockedValue     - Interlock object for atomic operations.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/IESynchObject.hpp"

/************************************************************************
 * \brief   Declared classes hierarchy
 ************************************************************************/
/* class IESynchObject; */
    class IEBlockingSynchObject;
        class CEMutex;
        class CESemaphore;
        class CECriticalSection;
        class IEResourceLock;
            class CEResourceLock;
            class CENolockSynchObject;
    class CESynchEvent;
    class CESynchTimer;

class CELock;
class CEMultiLock;
class CEInterlockedValue;

/**
 * \details The synchronization classes are used to synchronize
 *          data access from different threads as well as
 *          to send a trigger to notify a thread about
 *          particular event.
 *
 *          All Synchronization objects are instance of IESynchObject
 *          interface. The instance of IEBlockingSynchObject can be 
 *          used in auto-locking objects to synchronize data access.
 *
 *          A special CENolockSynchObject is defined to support
 *          synchronization functionalities, but the class instance
 *          does not block any thread and must not be used in 
 *          multi-locking operations. The purpose of this class
 *          to support IESynchObject interface to pass to container
 *          classes supporting data access synchronization.
 *
 *          CELock and CEMultiLock classes are supporting auto-locking
 *          functionalities. The locking is called during initialization
 *          of object and automatically released when object is destroyed.
 *
 *          CEInterlockedValue object is used for atomic operations.
 **/

/**
 * \brief   Base class of blocking synchronization objects.
 *          Used to synchronize object data access. 
 *          It is base class of Mutex and Critical section.
 *          For Semaphore, it will always return false. 
 *          The difficulties here is controlling which thread locked 
 *          Semaphore before. It differs from IESynchObject by 
 *          having additional TryLock() function, which attempts 
 *          to get ownership without blocking thread
 **/
//////////////////////////////////////////////////////////////////////////
// IEBlockingSynchObject class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API IEBlockingSynchObject    : public IESynchObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor. Should not be accessed directly.
     *          Only via derived classes
     **/
    IEBlockingSynchObject( IESynchObject::eSyncObject synchObjectType );

public:
    /**
     * \brief   
     **/
    virtual ~IEBlockingSynchObject( void );

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
    virtual bool TryLock( void );

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    IEBlockingSynchObject( void );
    IEBlockingSynchObject( const IEBlockingSynchObject & /*src*/ );
    const IEBlockingSynchObject & operator = ( const IEBlockingSynchObject & /*src*/ );
};

/**
 * \brief   Mutex synchronization object wrapper class.
 *          A mutex object is a synchronization object whose state is set 
 *          to signaled / locked when it is not owned by any thread, 
 *          and non-signaled / unlocked when it is owned. 
 *          Only one thread at a time can own a mutex object.
 *          It is useful in coordinating mutually exclusive access 
 *          to a shared resource. 
 *          For example, to prevent two threads from writing to 
 *          shared memory at the same time, each thread waits for 
 *          ownership of a mutex object, i.e. locks mutex, before 
 *          executing the code that accesses the memory. After 
 *          writing to the shared memory, the thread 
 *          releases / unlocks the mutex object.
 **/
//////////////////////////////////////////////////////////////////////////
// CEMutex class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEMutex   : public IEBlockingSynchObject
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
friend class CEMultiLock;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Initialize Synchronization object.
     * \param	lock	If true, current thread will get ownership
     *                  of mutex on initialization
     **/
    CEMutex( bool lock = true );

    /**
     * \brief	Destructor
     **/
    virtual ~CEMutex( void );

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
    virtual bool Lock( unsigned int timeout = IESynchObject::WAIT_INFINITE );

    /**
     * \brief	Unlocks / Release mutex.
     * \return	Returns true if succeeded.
     **/
    virtual bool Unlock( void );

    /**
     * \brief   Attempts to get Mutex object ownership without blocking thread.
     *          If the call is successful, the calling thread takes ownership of Mutex.
     * \return  If the ownership of Mutex is successfully taken or 
     *          the current thread already owns the Mutex, the return value is true.
     *          If another thread already owns the Mutex, the return value is false.
     **/
    virtual bool TryLock( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Return true if Mutex is already locked by any thread.
     **/
    inline bool IsLocked( void ) const;

    /**
     * \brief   Returns the ID of Thread, which is currently owning mutex
     **/
    inline ITEM_ID GetOwningThreadId( void ) const;

//////////////////////////////////////////////////////////////////////////
// Internal operations
//////////////////////////////////////////////////////////////////////////
private:
    void _setOwnership( void );

    void _releaseOwnership( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The ID of thread currently owning mutex
     **/
    ITEM_ID     mOwnerThreadId;

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    CEMutex( const CEMutex & /*src*/ );
    const CEMutex & operator = ( const CEMutex & /*src*/ );
};

/**
 * \brief   Event synchronization object wrapper class.
 *          An event object is a synchronization object whose 
 *          state can be explicitly set to signaled / non-signaled 
 *          by calling function.
 *          Followings are the two types of event object:
 *          1.  Manual-reset event: An event object whose state 
 *              remains signaled until it is explicitly reset to 
 *              non-signaled  by Unlock() function call. While it is 
 *              signaled, any number of waiting threads, or threads 
 *              that subsequently specify the same event object in 
 *              one of the wait functions, can be released. 
 *          2.  Auto-reset event: An event object whose state remains 
 *              signaled until a single waiting thread is released, 
 *              at which time the system automatically sets the state 
 *              to non-signaled. If no threads are waiting, the event 
 *              object's state remains signaled. If more than one 
 *              thread is waiting, a waiting thread is selected. 
 *              Do not assume a first-in, first-out (FIFO) order.
 *
 *          The event object is useful in sending a signal to a 
 *          thread indicating that a particular event has occurred.
 *          A single thread can specify different event objects in 
 *          several simultaneous overlapped operations, then use 
 *          Lock() function to wait for the state of event object 
 *          to be signaled. The creating thread specifies the initial 
 *          state of the object in Constructor and whether it is 
 *          a manual-reset or auto-reset event object.
 **/
//////////////////////////////////////////////////////////////////////////
// class CESynchEvent declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CESynchEvent  : public IESynchObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Default Constructor. Creates either Manual Reset or
     *          Auto-reset Event Synchronization object as well as
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
    CESynchEvent (bool lock = true, bool autoReset = true );

    /**
     * \brief   Destructor. Sets Event to signal state first.
     **/
    virtual ~CESynchEvent( void );

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
    virtual bool Lock( unsigned int timeout = IESynchObject::WAIT_INFINITE );

    /**
     * \brief	Unlock Event, i.e. set to signaled state
     * \return	Return true if successfully set event state to signaled.
     **/
    virtual bool Unlock( void );

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Manually sets event state to signaled. Same as calling Unlock()
     **/
    bool SetEvent( void );

    /**
     * \brief   Manually resets state of event, i.e. set is non-signaled.
     **/
    bool ResetEvent( void );

    /**
     * \brief   Pulse event once. It it was not set, it will set once and immediately
     *          reset to non-signaled state.
     **/
    void PulseEvent( void );

    /**
     * \brief   Returns true if event is auto-reset
     **/
    inline bool IsAutoReset( void ) const;

private:
    // int unlockedWaitForEvent(unsigned int timeout);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Flag, indicating whether event is auto-reset or not
     **/
    bool    mAutoReset;

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    CESynchEvent( const CESynchEvent & /*src*/ );
    const CESynchEvent & operator = ( const CESynchEvent & /*src*/ );
};

/**
 * \brief   Semaphore Synchronization object wrapper class
 *          A semaphore object is a synchronization object that 
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
//////////////////////////////////////////////////////////////////////////
// class CESemaphore declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CESemaphore: public IEBlockingSynchObject
{
//////////////////////////////////////////////////////////////////////////
// Friend objects
//////////////////////////////////////////////////////////////////////////
friend class CEMultiLock;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor, creates Semaphore synchronization object
     *          sets the maximum number thread access.
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
     *                      Lock() function releases a thread that was waiting
     *                      for the semaphore. The count is increased by a specified
     *                      amount by calling the Unlock() function.
     *                      If this parameter is zero, Semaphore is initially in
     *                      non-signaled / locked state
     **/
    CESemaphore( int maxCount, int initCount = 0 );

    /**
     * \brief   Destructor. Unlocks Semaphore and destroy object.
     **/
    virtual ~CESemaphore( void );

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
    virtual bool Lock( unsigned int timeout = IESynchObject::WAIT_INFINITE );

    /**
     * \brief   Unlocks Semaphore, i.e. signals it, and increase lock count number.
     * \return  Returns true if successfully signaled semaphore
     **/
    virtual bool Unlock( void );

    /**
     * \brief   Always return false. No implementation for Semaphore.
     **/
    virtual bool TryLock( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the maximum lock count of Semaphore object
     **/
    inline int GetMaxCount( void ) const;

    /**
     * \brief   Returns the current lock count of Semaphore object.
     **/
    inline int GetCurrentCount( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Maximum lock count number of semaphore
     **/
    const long      mMaxCount;
    /**
     * \brief   Current lock count number of semaphore
     **/
    long            mCurrCount;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    CESemaphore( void );
    CESemaphore( const CESemaphore & /*src*/ );
    const CESemaphore & operator = ( const CESemaphore & /*src*/ );
};


/**
 * \brief   Waitable Timer Synchronization object wrapper class
 *          A waitable timer object is a synchronization object 
 *          whose state is set to signaled when the specified 
 *          due time arrives. There are two types of waitable 
 *          timers that can be created: 
 *          manual-reset and synchronization.
 *          A timer of either type can also be a periodic timer.
 *          1.  manual-reset timer: A timer whose state remains 
 *              signaled until Unlock() is called to establish a new due time. 
 *          2.  synchronization timer: A timer whose state remains signaled 
 *              until a thread completes a wait operation on the timer object. 
 *          3.  periodic timer: A timer that is reactivated each time 
 *              the specified period expires, until the timer is reset 
 *              or canceled. A periodic timer is either a periodic manual-reset
 *              timer or a periodic synchronization timer. 
 **/
//////////////////////////////////////////////////////////////////////////
// class CESynchTimer declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CESynchTimer    : public IESynchObject
{
//////////////////////////////////////////////////////////////////////////
// Internal defined types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   CESynchTimer::NANOSECONDS_KOEF_100
     *          Coefficient to calculate due time in 100 nanosecond intervals.
     *          Used to convert milliseconds to due time of waitable timer
     **/
    static const unsigned int NANOSECONDS_KOEF_100    = 10000;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor. Creates Waitable Timer, either manual-reset,
     *          synchronization or periodic timer.
     * \param	timeMilliseconds	Time in milliseconds of Waitable Timer
     * \param	periodic	        If true, it is periodic timer
     * \param	autoReset	        If true, it is synchronization timer,
     *                              otherwise it is manual-reset timer.
     * \param	initSignaled        If true, the timer is activated and 
     *                              set	to signaled state.
     **/
    CESynchTimer( unsigned int timeMilliseconds, bool periodic = false, bool autoReset = true, bool initSignaled = true );

    /**
     * \brief   Destructor. Signals and Destroys waitable timer
     **/
    virtual ~CESynchTimer( void );

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
    virtual bool Lock( unsigned int timeout = IESynchObject::WAIT_INFINITE );

    /**
     * \brief   Activates the specified waitable timer. 
     *          It will not set timer to signaled state immediately.
     *          When the due time arrives, the timer is signaled and 
     *          the thread that set the timer calls the optional completion routine.
     *          To set due time, timer is using parameters passed in constructor.
     * \return  Return true if timer was successfully activate.
     **/
    virtual bool Unlock( void );

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Activates timer manually. Same as Unlock() function call.
     * \return  Returns true if waitable timer successfully activated
     **/
    bool SetTimer( void );

    /**
     * \brief   Sets the specified waitable timer to the inactive state.
     *          It does not change the signaled state of the timer.
     *          It stops the timer before it can be set to the signaled state.
     *          Therefore, threads performing a wait operation on the timer 
     *          remain waiting until they time out or the timer is reactivated 
     *          and its state is set to signaled. If the timer is already in 
     *          the signaled state, it remains in that state.
     **/
    bool CancelTimer( void );

    /**
     * \brief   Returns due time in milliseconds of waitable timer
     **/
    inline unsigned int GetDueTime( void ) const;

    /**
     * \brief   If true, the waitable timer is periodic
     **/
    bool IsPeriodic( void ) const;

    /**
     * \brief   If true, it is auto-reset waitable timer
     *          Otherwise, it is manual reset.
     **/
    bool IsAutoreset( void ) const;

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
    CESynchTimer( void );
    CESynchTimer( const CESynchTimer & /*src*/ );
    const CESynchTimer & operator = ( const CESynchTimer & /*src*/ );
};

/**
 * \brief   Critical Section Synchronization object wrapper class
 *          A critical section object provides synchronization 
 *          similar to  that provided by a mutex object, except 
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
//////////////////////////////////////////////////////////////////////////
// class CECriticalSection declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CECriticalSection  : public IEBlockingSynchObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor. Creates and Initialize critical section object
     **/
    CECriticalSection( void );

    /**
     * \brief   Destructor. Destroys critical section object
     **/
    virtual ~CECriticalSection( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits for ownership of critical section object. If the critical
     *          section object is currently owned by another thread, call of
     *          this function cause wait indefinitely for ownership.
     *          In contrast, when a mutex object is used for mutual exclusion,
     *          the Lock() function accept a specified time-out interval.
     * \return  CriticalSection always return true
     **/
    inline bool Lock( void );

    /**
     * \brief   Waits for ownership of critical section object. If the critical
     *          section object is currently owned by another thread, call of
     *          this function cause wait indefinitely for ownership.
     *          In contrast, when a mutex object is used for mutual exclusion,
     *          the Lock() function accept a specified time-out interval.
     *
     * \param   timeout     NOT USED in case of critical section object.
     * \return  In case of critical section, always return true
     **/
    virtual bool Lock( unsigned int /*timeout = IESynchObject::WAIT_INFINITE*/);

    /**
     * \brief   Releases ownership of the specified critical section object.
     * \return	In case of critical section, always return true
     **/
    virtual bool Unlock( void );

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
    virtual bool TryLock( void );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    CECriticalSection( const CECriticalSection & /*src*/ );
    const CECriticalSection & operator = ( const CECriticalSection & /*src*/ );
};

/**
 * \brief   This is a base class of synchronization object that locks the resource
 *          access. It contains an operating system dependent synchronization object
 *          such as CriticalSection, Mutex or Spin Lock that locks resource access.
 *          The CENolockSynchObject also derives this interface.
 **/
//////////////////////////////////////////////////////////////////////////
// IEResourceLock class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API IEResourceLock   : public IEBlockingSynchObject
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes resource lock object by passing type of synchronization object.
     * \param   synchObjectType     The type of synchronization object. It should be either
     *                              of type SO_RES_LOCK or SO_NOLOCK.
     **/
    IEResourceLock( IESynchObject::eSyncObject synchObjectType );

    /**
     * \brief   Destructor.
     **/
    virtual ~IEResourceLock( );

private:
    IEResourceLock( void );
    IEResourceLock( const IEResourceLock & /*src*/ );
    const IEResourceLock & operator = ( const IEResourceLock & /*src*/ );
};

/**
 * \brief   This is a base class of synchronization object that locks the resource
 *          access. It contains an operating system dependent synchronization object
 *          such as CriticalSection, Mutex or Spin Lock that locks resource access.
 *          The CENolockSynchObject also derives this interface.
 **/
//////////////////////////////////////////////////////////////////////////
// IEResourceLock class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEResourceLock   : public IEResourceLock
{

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the object. Depending on operating system it will
     *          delegate another OS specific synchronization object like
     *          CriticalSection, Spin Lock or Mutex to lock and unlock
     *          resource object.
     * \param   initLock    If true, the resource is initially locked.
     **/
    CEResourceLock( bool initLock = false );

    /**
     * \brief   Destructor.
     **/
    virtual ~CEResourceLock( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Call to lock the resource or wait for specified timeout until
     *          either timeout expires or the resource can be locked. If timeout
     *          is IESynchObject::WAIT_INFINITE, it will wait as long, unti resource
     *          access is released.
     * \param	timeout     The timeout to wait to lock the resource. If timeout is
     *                      IESynchObject::WAIT_INFINITE, it waits as long, until
     *                      resource access is released and it can be locked.
     * \return	Returns true if succeeded.
     **/
    virtual bool Lock( unsigned int timeout = IESynchObject::WAIT_INFINITE );

    /**
     * \brief   Releases ownership of resource.
     * \return	Returns true if operation succeeds.
     **/
    virtual bool Unlock( void );

    /**
     * \brief   Attempts to lock the resource without blocking thread.
     *          If the call is successful, the calling thread 
     *          takes ownership of the resource.
     * \return  Returns true if operation succeeds.
     **/
    virtual bool TryLock( void );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    CEResourceLock( const CEResourceLock & /*src*/ );
    const CEResourceLock & operator = ( const CEResourceLock & /*src*/ );
};

/**
 * \brief   No Lock Synchronization object is a dummy class
 *          doing no synchronization action, but having implementation
 *          of overrides. Some classes might need having synchronization
 *          of data access and some other might not need.
 *          For this reason, the reference to IESynchObject might be
 *          passed as a main synchronization object and by calling
 *          Lock() / Unlock() either data access would be really
 *          synchronized or synchronization is imitated / ignored.
 *
 * \note    Do not use this class for Multi-locking (see CEMultiLock)
 *          The locking might be imitated only by using CELock object
 *          or calling Lock() / Unlock() directly.
 **/
//////////////////////////////////////////////////////////////////////////
// CENolockSynchObject class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CENolockSynchObject   : public IEResourceLock
{

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     * \param   lock        Does not play any role. 
     *                      Only to make compatible with other locking constructors
     * \param   autoReset   Does not play any role. 
     *                      Only to make compatible with other locking constructors
     **/
    CENolockSynchObject( bool lock = true, bool autoReset = true );

    /**
     * \brief   Initialization constructor
     * \param   maxCount    Does not play any role. 
     *                      Only to make compatible with other locking constructors
     * \param   initCount   Does not play any role. 
     *                      Only to make compatible with other locking constructors
     **/
    CENolockSynchObject( int maxCount, int initCount = 0 );

    /**
     * \brief   Destructor
     **/
    virtual ~CENolockSynchObject( void );

//////////////////////////////////////////////////////////////////////////
// Override operations, IESynchObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   No real locking performed by calling this function.
     *          The calling thread will not be blocked and 
     *          will continue running
     * \param	NOT USED. 
     * \return	Always returns true
     **/
    virtual bool Lock( unsigned int /*timeout = IESynchObject::WAIT_INFINITE*/ );

    /**
     * \brief   No real unlocking.
     * \return	Always returns true.
     **/
    virtual bool Unlock( void );

    /**
     * \brief   Always return true. No real locking.
     **/
    virtual bool TryLock( void );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    CENolockSynchObject( const CENolockSynchObject & /*src*/ );
    const CENolockSynchObject & operator = ( const CENolockSynchObject & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CELock class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Class to use auto-locking for a single locking object.
 *          This class can be used for all instances of IESynchObject.
 *          The purpose of using this object is to lock synchronization
 *          object in a certain code scope.
 *
 * \example CEMutex use
 *
 *          In this example bellow MyClass contains Mutex as a synchronization object
 *          Any time thread is calling foo() or bar() methods ot MyClass, on funtion entry
 *          Single Synchronization Object Locking instance is created,
 *          which gets a reference to the Mutex object.
 *          By default, the auto-locking is enable, and by this current thread will try
 *          to get Mutext ownership and will call Lock() function during initialization.
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
 *              CEMutex mMutex;
 *          };
 *
 *          MyClass::MyClass( void )
 *              : mMutex(true)
 *          {   ; }
 *
 *          void MyClass::foo( void )
 *          {
 *              CELock lock(mMutex);
 *              // perform actions here
 *          }
 *
 *          void MyClass::bar( void )
 *          {
 *              CELock lock(mMutex);
 *              // perform actions here
 *          }
 **/
class AREG_API CELock
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
    CELock( IESynchObject &syncObj, bool autoLock = true );

    /**
     * \brief   Destructor. If auto-locking was enabled, it will call
     *          Unlock() method of synchronization object to release
     *          ownership of object.
     **/
    ~CELock( void );

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
    inline bool Lock( unsigned int timeout = IESynchObject::WAIT_INFINITE );

    /**
     * \brief   Called to unlock synchronization object manually
     * \return  Returns true if synchronization object successfully unlocked.
     **/
    inline bool Unlock( void );

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
    CELock( void );
    CELock( const CELock & /*src*/ );
    const CELock & operator = ( const CELock & /*src*/ );
};


/**
 * \brief   Class to use auto-locking for multiple locking object.
 *          This class is getting list of synchronization objects
 *          and on lock waits either for all objects to be signaled
 *          or  only one of them. The principle of auto-locking is same
 *          at it is for locking single synchronization object.
 *          The parameter list can contain any type of synchronization
 *          object, except Critical Section.
 *
 * \see     CELock
 **/
//////////////////////////////////////////////////////////////////////////
// CEMultiLock class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEMultiLock
{
//////////////////////////////////////////////////////////////////////////
// Internally defined types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   CEMultiLock::eLockedState
     *          The current locking state of every single synchronization object
     **/
    typedef enum E_LockedState
    {
          STATE_UNLOCKED    = 0     //!< Unlocked state
        , STATE_LOCKED      = 1     //!< Locked state
    } eLockedState;

public:
    /**
     * \brief   CEMultiLock::MAXIMUM_WAITING_OBJECTS
     *          The maximum number of synchronization objects that is able to lock. 
     **/
    static const int MAXIMUM_WAITING_OBJECTS    = 32;

    /**
     * \brief   CEMultiLock::LOCK_INDEX_INVALID
     *          Invalid index of synchronization list
     **/
    static const int LOCK_INDEX_INVALID         = -1;

    /**
     * \brief   CEMultiLock::LOCK_INDEX_COMPLETION
     *          The completion routine index.
     *          Returned if waiting function returns WAIT_IO_COMPLETION
     **/
    static const int LOCK_INDEX_COMPLETION      = -2;
    /**
     * \brief   CEMultiLock::LOCK_INDEX_TIMEOUT
     *          The index, indicating waiting timeout.
     **/
    static const int LOCK_INDEX_TIMEOUT         = -3;
    /**
     * \brief   CEMultiLock::LOCK_INDEX_ALL
     *          All synchronization objects are locked.
     *          Same as MAX_SIZE_OF_ARRAY (64)
     **/
    static const int LOCK_INDEX_ALL             = MAXIMUM_WAITING_OBJECTS;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Constructor. Initialize Multi-lock object, receives list of
     *          synchronization objects and flag whether it should initially
     *          lock or not. If requested to lock, it will wait for all
     *          objects to be signaled.
     *          NOTE:   There must be no Critical Section synchronization 
     *                  object in the list. Otherwise, assertion is raised.
     * \param	pObjects	List of Synchronization objects
     * \param	count	    Number of Synchronization objects in the list
     * \param	autoLock	If true, it will automatically lock all
     *                      synchronization objects and wait for all objects
     *                      to be signaled.
     **/
    CEMultiLock( IESynchObject* pObjects[], int count, bool autoLock = true );

    /**
     * \brief   Destructor. If auto-lock is enabled, unlocks all synchronization
     *          objects and free resources.
     **/
    ~CEMultiLock( void );

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
    int Lock( unsigned int timeout = IESynchObject::WAIT_INFINITE, bool waitForAll = false, bool isAlertable = false );

    /**
     * \brief   Unlocks every synchronization object, which was locked before
     **/
    bool Unlock( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Locking state of every object within array.
     **/
    eLockedState              mLockedStates[MAXIMUM_WAITING_OBJECTS];
    /**
     * \brief   List of synchronization objects passed on initialization
     **/
    IESynchObject * const *   mSyncObjArray;
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
    CEMultiLock( void );
    CEMultiLock( const CEMultiLock & /*src*/ );
    const CEMultiLock & operator = ( const CEMultiLock & /*src*/ );
};

/**
 * \brief   Interlocked Variable Access Synchronization object 
 *          wrapper class. Use this class to synchronize access 
 *          to variables that are shared by multiple threads. 
 *          This class provides simple operations performed 
 *          on single integer variable. Simple reads and writes 
 *          to properly-aligned 32-bit variables are atomic 
 *          operations, all bits are updated in an atomic fashion.
 *          However, access is not guaranteed to be synchronized. 
 *          If two threads are reading and writing from the same 
 *          variable, you cannot determine if one thread will 
 *          perform its read operation before the other performs 
 *          its write operation.
 **/
//////////////////////////////////////////////////////////////////////////
// class CEInterlockedValue declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEInterlockedValue
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Default constructor. If pointer to variable is passed
     *          the variable access will be synchronized by class.
     *          Otherwise, it will use internal variable for synchronization
     *          with initial value zero. If valid pointer is passed, all
     *          atomic operations will have direct changes 
     *          on external 32-bit variable, which should remain valid.
     * \note    If pointer to variable is passed, it should be valid
     *          while performing atomic operations. Otherwise, use
     *          constructor with initialization.
     * \param	varPtr  The pointer to variable to synchronize access
     **/
    CEInterlockedValue( unsigned int * varPtr = NULL );

    /**
     * \brief	Constructor. Initialize internal variable by setting initial value
     *          and synchronize interlocked access of internal variable
     * \param	initValue	The initial value of internal variable to set
     **/
    CEInterlockedValue( unsigned int initValue );

    /**
     * \brief   Destructor.
     **/
    ~CEInterlockedValue( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Increments (increases by one) the value of 
     *          32-bit variable as an atomic operation.
     * \return  The function returns the resulting incremented value.
     **/
    unsigned int Increment( void );

    /**
     * \brief   Decrements (decreases by one) the value of 
     *          32-bit existing variable as an atomic operation.
     * \return	The function returns the resulting decremented value
     **/
    unsigned int Decrement( void );

    /**
     * \brief   Decrements (decreases by one) the value of 
     *          32-bit existing variable as an atomic operation.
     * \return	The function returns true if resulting decrement
     *          value is greater than zero. Otherwise returns false.
     **/
    bool TestDecrement( void );

    /**
     * \brief	Decrements (decrease by one) the value of
     *          32-bit existing variable as an atomic operation
     *          if the value is greater than zero. Otherwise,
     *          operation is ignored. Returned true, if reached
     *          zero. If 32-bit is already zero or after decrease 
     *          operation value is not equal zero, it returns false.
     **/
    bool TestDecrementZero( void );

    /**
     * \brief	Performs an atomic addition of existing and passed 32-bit values
     * \param	increment   The 32-bit value to add existing value
     * \return	The function returns the initial value of the existing variable.
     **/
    unsigned int Add( unsigned int increment );

    /**
     * \brief	Performs an atomic subtraction of existing and passed 32-bit values
     * \param	increment   The 32-bit value to subtract existing value
     * \return	The function returns the initial value of the existing variable.
     **/
    unsigned int Subtract( unsigned int decrement );

    /**
     * \brief	Gets 32-bit existing variable to the specified value as an atomic operation.
     * \param	out_data    Reference to variable to pass value
     * \return	
     **/
    void GetData( unsigned int & out_data ) const;

    /**
     * \brief	Gets 32-bit existing variable to the specified value as an atomic operation.
     * \return	Returns 32-bit value.
     **/
    unsigned int GetData( void ) const;

    /**
     * \brief	Sets to 32-bit existing variable specified value as an atomic operation.
     * \param	data	Value to set to existing variable.
     * \return	
     **/
    void SetData( unsigned int data );

    /**
     * \brief	Sets bits to 32-bit existing variable specified value as an atomic operation.
     * \param	mask	Bit mask to set.
     * \return	The function returns the initial value of the existing variable.
     **/
    unsigned int SetBits( unsigned int mask );

    /**
     * \brief	Removes bits from 32-bit existing variable specified value as an atomic operation.
     * \param	mask	Bit mask to remove.
     * \return	The function returns the initial value of the existing variable.
     **/
    unsigned int RemoveBits( unsigned int mask );

    /**
     * \brief	Toggles bits on 32-bit existing variable specified value as an atomic operation.
     * \param	mask	Bit mask to toggle.
     * \return	The function returns the initial value of the existing variable.
     **/
    unsigned int ToggleBits( unsigned int mask );

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converts class object to unsigned int
     **/
    inline operator unsigned int( void ) const;
    /**
     * \brief   Assigns new value
     **/
    inline CEInterlockedValue & operator = ( unsigned int newValue );

    /**
     * \brief   Compares equality of existing value with given
     * \return  Returns true if values are equal
     **/
    inline bool operator == ( unsigned int rhs ) const;

    /**
     * \brief   Compares inequality of existing value with given
     * \return  Returns true if values are unequal
     **/
    inline bool operator != ( unsigned int rhs ) const;

    /**
     * \brief   Compares whether existing value is greater than given
     * \return  Returns true if existing value is greater than given
     **/
    inline bool operator  > ( unsigned int rhs ) const;

    /**
     * \brief   Compares whether existing value is smaller than given
     * \return  Returns true if existing value is smaller than given
     **/
    inline bool operator  < ( unsigned int rhs ) const;

    /**
     * \brief   Compares whether existing value is greater or equal than given
     * \return  Returns true if existing value is greater or equal than given
     **/
    inline bool operator >= ( unsigned int rhs ) const;

    /**
     * \brief   Compares whether existing value is smaller or equal than given
     * \return  Returns true if existing value is smaller or equal than given
     **/
    inline bool operator <= ( unsigned int rhs ) const;

    /**
     * \brief   Increase by one
     **/
    inline CEInterlockedValue & operator ++ ( void );
    
    /**
     * \brief   Decrease by one
     **/
    inline CEInterlockedValue & operator -- ( void );
    
    /**
     * \brief   Add value
     **/
    inline CEInterlockedValue & operator += ( unsigned int addValue );
    
    /**
     * \brief   Subtract value
     **/
    inline CEInterlockedValue & operator -= ( unsigned int subValue );

/************************************************************************/
// Friend global operators to compare values
/************************************************************************/
    /**
     * \brief   Global friend method.
     *          Compares equality of integer number and value of thread safe object
     * \param   lhs     Left-side integer
     * \param   rhs     Right-side object with value
     * \return  Returns true if 2 values are equal.
     **/
    friend inline bool operator == ( unsigned int lhs, const CEInterlockedValue & rhs );

    /**
     * \brief   Global friend method.
     *          Compares inequality of integer number and value of thread safe object
     * \param   lhs     Left-side integer
     * \param   rhs     Right-side object with value
     * \return  Returns true if 2 values are not equal.
     **/
    friend inline bool operator != ( unsigned int lhs, const CEInterlockedValue & rhs );

    /**
     * \brief   Global friend method.
     *          Compares whether integer number is greater than value of thread safe object
     * \param   lhs     Left-side integer
     * \param   rhs     Right-side object with value
     * \return  Returns true if integer value is greater than value of object.
     **/
    friend inline bool operator  > ( unsigned int lhs, const CEInterlockedValue & rhs );

    /**
     * \brief   Global friend method.
     *          Compares whether integer number is smaller than value of thread safe object
     * \param   lhs     Left-side integer
     * \param   rhs     Right-side object with value
     * \return  Returns true if integer value is smaller than value of object.
     **/
    friend inline bool operator  < ( unsigned int lhs, const CEInterlockedValue & rhs );

    /**
     * \brief   Global friend method.
     *          Compares whether integer number is greater or equal than value of thread safe object
     * \param   lhs     Left-side integer
     * \param   rhs     Right-side object with value
     * \return  Returns true if integer value is greater or equal than value of object.
     **/
    friend inline bool operator >= ( unsigned int lhs, const CEInterlockedValue & rhs );

    /**
     * \brief   Global friend method.
     *          Compares whether integer number is smaller or equal than value of thread safe object
     * \param   lhs     Left-side integer
     * \param   rhs     Right-side object with value
     * \return  Returns true if integer value is or equal smaller than value of object.
     **/
    friend inline bool operator <= ( unsigned int lhs, const CEInterlockedValue & rhs );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Internal variable to perform operations if no external variable is passed
     **/
    unsigned int   mValue;
    /**
     * \brief   Pointer to 32-bit variable to perform atomic operation
     **/
    mutable int *  mData;

    /**
     * \brief   An addition context variable. Can be OS or compiler specific.
     **/
    mutable void *  mContext;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    CEInterlockedValue( const CEInterlockedValue & /*src*/ );
    const CEInterlockedValue & operator = ( const CEInterlockedValue & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// Inline functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMutex class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool CEMutex::IsLocked( void ) const
{   return (mOwnerThreadId != 0);       }

inline ITEM_ID CEMutex::GetOwningThreadId( void ) const
{   return mOwnerThreadId;          }

//////////////////////////////////////////////////////////////////////////
// CESynchEvent class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool CESynchEvent::IsAutoReset( void ) const
{   return mAutoReset;              }

//////////////////////////////////////////////////////////////////////////
// CESemaphore class inline functions
//////////////////////////////////////////////////////////////////////////
inline int CESemaphore::GetMaxCount( void ) const
{   return mMaxCount;               }

inline int CESemaphore::GetCurrentCount( void ) const
{   return mCurrCount;              }

//////////////////////////////////////////////////////////////////////////
// CESynchTimer class inline functions
//////////////////////////////////////////////////////////////////////////
inline unsigned int CESynchTimer::GetDueTime( void ) const
{   return mTimeMilliseconds;       }

inline bool CESynchTimer::IsPeriodic( void ) const
{   return mIsPeriodic;             }

inline bool CESynchTimer::IsAutoreset( void ) const
{   return mIsAutoReset;            }

//////////////////////////////////////////////////////////////////////////
// CECriticalSection class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool CECriticalSection::Lock( void )
{   return Lock(IESynchObject::WAIT_INFINITE);  }

//////////////////////////////////////////////////////////////////////////
// CELock class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool CELock::Lock(unsigned int timeout /* = IESynchObject::WAIT_INFINITE */)
{   return mSynchObject.Lock(timeout);          }

inline bool CELock::Unlock( void )
{   return mSynchObject.Unlock();               }

//////////////////////////////////////////////////////////////////////////
// CEInterlockedValue class inline functions
//////////////////////////////////////////////////////////////////////////

inline CEInterlockedValue::operator unsigned int ( void ) const
{   return GetData();                                                                       }

inline CEInterlockedValue & CEInterlockedValue::operator = ( unsigned int newValue )
{   SetData(newValue); return (*this);                                                      }

inline bool CEInterlockedValue::operator == ( unsigned int rhs ) const
{   return (GetData() == rhs);                                                              }

inline bool CEInterlockedValue::operator != ( unsigned int rhs ) const
{   return (GetData() != rhs);                                                              }

inline bool CEInterlockedValue::operator > ( unsigned int rhs ) const
{   return (GetData() > rhs);                                                               }

inline bool CEInterlockedValue::operator < ( unsigned int rhs ) const
{   return (GetData() < rhs);                                                               }

inline bool CEInterlockedValue::operator >= ( unsigned int rhs ) const
{   return (GetData() >= rhs);                                                              }

inline bool CEInterlockedValue::operator <= ( unsigned int rhs ) const
{   return (GetData() <= rhs);                                                              }

inline CEInterlockedValue & CEInterlockedValue::operator ++ ( void )
{   Add(static_cast<unsigned int>(1)); return (*this);                                      }

inline CEInterlockedValue & CEInterlockedValue::operator -- ( void )
{   Subtract(static_cast<unsigned int>(1)); return (*this);                                 }

inline CEInterlockedValue & CEInterlockedValue::operator += ( unsigned int addValue )
{   Add(addValue); return (*this);                                                          }

inline CEInterlockedValue & CEInterlockedValue::operator -= ( unsigned int subValue )
{   Subtract(subValue); return (*this);                                                     }

/************************************************************************
 * friend methods
 ************************************************************************/
inline bool operator == ( unsigned int lhs, const CEInterlockedValue & rhs )
{   return (lhs == rhs.GetData());                                                          }

inline bool operator != ( unsigned int lhs, const CEInterlockedValue & rhs )
{   return (lhs != rhs.GetData());                                                          }

inline bool operator > ( unsigned int lhs, const CEInterlockedValue & rhs )
{   return (lhs > rhs.GetData());                                                           }

inline bool operator < ( unsigned int lhs, const CEInterlockedValue & rhs )
{   return (lhs < rhs.GetData());                                                           }

inline bool operator >= ( unsigned int lhs, const CEInterlockedValue & rhs )
{   return (lhs >= rhs.GetData());                                                          }

inline bool operator <= ( unsigned int lhs, const CEInterlockedValue & rhs )
{   return (lhs <= rhs.GetData());                                                          }

#endif  // AREG_BASE_ESYNCHOBJECTS_HPP
