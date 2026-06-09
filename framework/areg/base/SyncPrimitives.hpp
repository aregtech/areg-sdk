#ifndef AREG_BASE_SYNCPRIMITIVES_HPP
#define AREG_BASE_SYNCPRIMITIVES_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SyncPrimitives.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              Lockable      - blocking synchronization object interface.
 *              Mutex               - Mutex synchronization object.
 *              SyncEvent           - Event synchronization object.
 *              Semaphore           - Semaphore synchronization object.
 *              CriticalSection     - Critical Section synchronization object.
 *              SpinLock            - Spin-Lock synchronization object.
 *              ResourceLock        - An OS specific resource lock object.
 *              NolockSyncObject    - No Locking synchronization object (makes no locking).
 *              SyncTimer           - Timer synchronization object.
 *              Lock                - Single synchronization auto locking object.
 *              MultiLock           - Multiple synchronization auto locking object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncObject.hpp"

#include <atomic>
#include <chrono>

namespace areg {

/**
 * \brief   This file contains synchronization objects used to synchronize data access
 *          in multi-threading environment. All Synchronization objects are instances of 
 *          SyncObject interface. The instance of Lockable can be 
 *          used in auto-locking objects to synchronize data access.
 *
 *          A special NolockSyncObject is defined to support synchronization functionalities, 
 *          but the object does not block any thread and must not be used in multi-locking operations. 
 *          The purpose of this class to support unified SyncObject interface and use in containers
 *          that do not require synchronization operations.
 *
 *          Lock and MultiLock classes are supporting auto-locking
 *          functionalities. The locking is called during initialization
 *          of object and automatically released when object is destroyed.
 **/

/************************************************************************
 * List of declared classes and hierarchy
 ************************************************************************/
/* class SyncObject; */
    class Lockable;
        class Mutex;
        class Semaphore;
        class CriticalSection;
        class SpinLock;
        class NolockSyncObject;
    class SyncEvent;
    class SyncTimer;

class Lock;
class MultiLock;
class Wait;

//////////////////////////////////////////////////////////////////////////
// Lockable class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base interface for objects that provide synchronization (lock, unlock, try_lock).
 **/
class AREG_API Lockable : public SyncObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor. Initialize with the specified synchronization object type.
     *          Accessible only to derived classes.
     *
     * \param   syncObjectType      The type of synchronization object.
     **/
    Lockable( SyncObject::SyncKind syncObjectType );

public:
    virtual ~Lockable() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
// Pure virtual functions to overwrite
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Attempts to acquire the synchronization object without blocking.
     *
     * \return  Returns true if ownership was acquired or is already held by the current thread;
     *          false if another thread owns it.
     **/
    virtual bool try_lock();

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    Lockable() = delete;
    AREG_NOCOPY_NOMOVE( Lockable );
};

//////////////////////////////////////////////////////////////////////////
// Mutex class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronization object for mutual exclusion across single or multiple processes. Allows
 *          one thread at a time to own it and supports timeout-based locking.
 **/
class AREG_API Mutex final  : public Lockable
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
     * \brief   Creates and optionally initializes the mutex with ownership.
     *
     * \param   initLock    If true, the current thread acquires ownership on initialization.
     **/
    explicit Mutex( bool initLock = true );

    virtual ~Mutex();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Acquires mutex ownership. Blocks for up to timeout milliseconds if owned by another thread.
     *
     * \param   timeout     Timeout in milliseconds; WAIT_INFINITE to wait indefinitely.
     * \return  Returns true if ownership was acquired.
     **/
    inline bool lock( uint32_t timeout = areg::WAIT_INFINITE ) final;

    /**
     * \brief   Releases mutex ownership. Caller must own the mutex.
     *
     * \return  Returns true if successful.
     **/
    inline bool unlock() final;

    /**
     * \brief   Attempts to acquire mutex ownership without blocking.
     *
     * \return  Returns true if acquired or already owned by current thread; false if owned by another.
     **/
    inline bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the mutex is currently locked by any thread.
     **/
    [[nodiscard]]
    inline bool is_locked() const noexcept;

    /**
     * \brief   Returns the thread ID of the thread currently owning the mutex.
     **/
    [[nodiscard]]
    inline id_type owner_thread_id() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden or OS specific implementations
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS-specific implementation to lock and take ownership.
     *
     * \param   timeout     The timeout in milliseconds to wait for the mutex.
     **/
    bool _os_lock_mutex( uint32_t timeout );

    /**
     * \brief   OS-specific implementation to unlock and release ownership.
     **/
    bool _os_unlock_mutex();

    /**
     * \brief   OS-specific creation. If initLock is true, current thread acquires ownership.
     **/
    void _os_create_mutex( bool initLock );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    std::atomic<id_type>    mOwnerThreadId; //!< The ID of thread currently owning mutex

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( Mutex );
};

//////////////////////////////////////////////////////////////////////////
// class SyncEvent declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronization event object with manual-reset or auto-reset modes. Manual-reset events
 *          remain signaled until explicitly reset. Auto-reset events automatically reset after a
 *          single waiting thread is released. Used to signal between threads that a particular
 *          event has occurred.
 **/
class AREG_API SyncEvent final  : public SyncObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates a manual-reset or auto-reset synchronization event with an initial state.
     *
     * \param   initLock        If true, the event is initially non-signaled (locked). If false, the
     *                          event is initially signaled. Default is true (non-signaled).
     * \param   autoReset       If true, creates an auto-reset event; otherwise, creates a
     *                          manual-reset event. Default is true (auto-reset).
     **/
    explicit SyncEvent ( bool initLock = true, bool autoReset = true );

    virtual ~SyncEvent();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits for the event to be signaled. If already signaled, returns immediately.
     *          If auto-reset, the event is automatically set to non-signaled after this call.
     *
     * \param   timeout     The timeout in milliseconds to wait for the event to be signaled.
     *                      Use areg::WAIT_INFINITE for indefinite wait. Use areg::DO_NOT_WAIT
     *                      for non-blocking check.
     * \return  Returns true if the event was signaled before or during the timeout; false if
     *          timeout expired.
     **/
    inline bool lock( uint32_t timeout = areg::WAIT_INFINITE ) final;

    /**
     * \brief   Sets the event to signaled state.
     *
     * \return  Returns true if successfully set the event to signaled.
     **/
    inline bool unlock() final;

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Sets the event to signaled state. Equivalent to calling unlock().
     * \return  Returns true if operation succeeded.
     **/
    inline bool set_signaled() noexcept;

    /**
     * \brief   Resets the event to non-signaled state. Only manual-reset events can be manually reset.
     * \return  Returns true if operation succeeded.
     **/
    inline bool reset() noexcept;

    /**
     * \brief   Pulses the event: briefly signals it and immediately resets to non-signaled,
     *          releasing waiting threads.
     **/
    inline void pulse_event() noexcept;

    /**
     * \brief   Returns true if the event is auto-reset; false if manual-reset.
     **/
    [[nodiscard]]
    inline bool is_auto_reset() const noexcept;

    /**
     * \brief   Return value of wait_any() when the operation timed out or was interrupted.
     **/
    static constexpr int32_t WAIT_ANY_TIMEOUT { -1 };

    /**
     * \brief   Waits for any one of the specified events to be signaled. Returns immediately if any
     *          event is already signaled. Maximum areg::MAXIMUM_WAITING_OBJECTS events.
     *
     * \param   events      Array of SyncEvent pointers to wait on.
     * \param   count       Number of events. Must be > 0 and <= areg::MAXIMUM_WAITING_OBJECTS.
     * \param   timeout     Timeout in milliseconds. areg::WAIT_INFINITE waits forever.
     *                      areg::DO_NOT_WAIT checks state without blocking.
     * \return  Returns the zero-based index of the first signaled event, or WAIT_ANY_TIMEOUT if the
     *          operation timed out or was interrupted.
     **/
    static int32_t wait_any( SyncEvent** events, int32_t count, uint32_t timeout = areg::WAIT_INFINITE ) noexcept;

    /**
     * \brief   Waits for either of two events to be signaled. Typed convenience overload for the
     *          common dispatcher pattern (queue event + exit event) — avoids stack array allocation
     *          at the call site. Returns 0 if ev0 fired, 1 if ev1 fired, or WAIT_ANY_TIMEOUT.
     *
     * \param   ev0         First event (index 0).
     * \param   ev1         Second event (index 1).
     * \param   timeout     Timeout in milliseconds. areg::WAIT_INFINITE waits forever.
     * \return  0, 1, or WAIT_ANY_TIMEOUT.
     **/
    static inline int32_t wait_any( SyncEvent& ev0, SyncEvent& ev1, uint32_t timeout = areg::WAIT_INFINITE ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden or OS specific implementations
//////////////////////////////////////////////////////////////////////////
private:
    
    /**
     * \brief   OS-specific implementation to create an event. The event is non-signaled if initLock
     *          is true; signaled otherwise.
     *
     * \param   initLock    If true, the event is created in non-signaled state. If false, the event
     *                      is created in signaled state.
     **/
    void _os_create_event( bool initLock );

    /**
     * \brief   OS-specific implementation to signal an event and release waiting threads.
     *
     * \param   eventHandle     The handle of the event object to signal.
     * \return  Returns true if operation succeeded.
     **/
    bool _os_unlock_event( void * eventHandle ) noexcept;

    /**
     * \brief   OS-specific implementation to wait for an event to be signaled.
     *
     * \param   timeout     The timeout in milliseconds to wait for the event.
     * \return  Returns true if the event was signaled during the timeout.
     **/
    bool _os_lock_event( uint32_t timeout );

    /**
     * \brief   OS-specific implementation to set the event to signaled state.
     *
     * \return  Returns true if operation succeeded.
     **/
    bool _os_set_event() noexcept;

    /**
     * \brief   OS-specific implementation to reset the event to non-signaled state.
     *
     * \return  Returns true if operation succeeded.
     **/
    bool _os_reset_event() noexcept;

    /**
     * \brief   OS-specific implementation to pulse the event: briefly signal it and immediately
     *          reset to non-signaled.
     **/
    void _os_pulse_event() noexcept;

    /**
     * \brief   OS-specific implementation to wait for any one event to be signaled.
     *
     * \param   events      Array of SyncEvent pointers.
     * \param   count       Number of events in the array.
     * \param   timeout     Timeout in milliseconds.
     * \return  Returns the index of the signaled event, or WAIT_ANY_TIMEOUT on failure/timeout.
     **/
    static int32_t _os_wait_any( SyncEvent** events, int32_t count, uint32_t timeout ) noexcept;

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
    AREG_NOCOPY_NOMOVE( SyncEvent );
};

//////////////////////////////////////////////////////////////////////////
// class Semaphore declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Synchronization semaphore with a configurable count. Maintains a count between zero and
 *          a maximum. Used to control access to a limited resource by multiple threads.
 **/
class AREG_API Semaphore final  : public Lockable
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
     * \brief   Creates a semaphore with the specified maximum count and initial count.
     *
     * \param   maxCount        The maximum number of threads allowed to acquire the semaphore simultaneously.
     * \param   initCount       The initial count. Must be between 0 and maxCount. A count of 0
     *                          means the semaphore is initially locked. Default is 0.
     **/
    explicit Semaphore( int32_t maxCount, int32_t initCount = 0 );

    virtual ~Semaphore();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Acquires the semaphore by decrementing its count. Blocks if count is zero until it
     *          is incremented or timeout expires.
     *
     * \param   timeout     The timeout in milliseconds. Use areg::WAIT_INFINITE to wait indefinitely.
     * \return  Returns true if successfully acquired; false if timeout expired.
     **/
    inline bool lock( uint32_t timeout = areg::WAIT_INFINITE ) final;

    /**
     * \brief   Releases the semaphore by incrementing its count.
     *
     * \return  Returns true if successfully incremented.
     **/
    inline bool unlock() final;

    /**
     * \brief   Not implemented for semaphore. Always returns false.
     *
     * \return  Returns false.
     **/
    inline bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
//// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the maximum count of the semaphore.
     **/
    [[nodiscard]]
    inline uint32_t max_count() const noexcept;

    /**
     * \brief   Returns the current count of the semaphore.
     **/
    [[nodiscard]]
    inline uint32_t current_count() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden or OS specific methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS-specific implementation to create the semaphore.
     **/
    void _os_create_semaphore();

    /**
     * \brief   OS-specific implementation to release semaphore resources.
     **/
    void _os_release_semaphore();

    /**
     * \brief   OS-specific implementation to acquire the semaphore. Blocks if already locked, or
     *          returns false on timeout.
     *
     * \param   timeout     The timeout in milliseconds to wait for semaphore availability.
     * \return  Returns true if successfully acquired.
     **/
    bool _os_lock( uint32_t timeout );

    /**
     * \brief   OS-specific implementation to release the semaphore. Allows waiting threads to acquire it.
     *
     * \return  Returns true if successfully released.
     **/
    bool _os_unlock();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   
     **/
    const uint32_t      mMaxCount;  //!< Maximum lock count number of semaphore

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    std::atomic_uint    mCurrCount; //!< Current lock count number of semaphore
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    Semaphore() = delete;
    AREG_NOCOPY_NOMOVE( Semaphore );
};

//////////////////////////////////////////////////////////////////////////
// class CriticalSection declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Single-process synchronization object providing mutual exclusion similar to mutex but
 *          slightly more efficient. Can only be used by threads within the same process.
 **/
class AREG_API CriticalSection  final   : public Lockable
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    CriticalSection();

    virtual ~CriticalSection();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Waits for and acquires ownership of the critical section. Blocks indefinitely if
     *          owned by another thread.
     *
     * \return  Always returns true for critical sections.
     **/
    inline bool lock();

    /**
     * \brief   Waits for and acquires ownership of the critical section. The timeout parameter is
     *          ignored; always blocks indefinitely.
     *
     * \return  Always returns true.
     **/
    bool lock( uint32_t /*timeout = areg::WAIT_INFINITE*/) final;

    /**
     * \brief   Releases ownership of the critical section.
     *
     * \return  Always returns true.
     **/
    bool unlock() final;

    /**
     * \brief   Attempts to acquire critical section ownership without blocking.
     *
     * \return  Returns true if acquired or already owned by current thread; false if owned by
     *          another thread.
     **/
    bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
// Hidden or OS specific calls
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS-specific creation of critical section.
     **/
    void _os_create_critical_section();

    /**
     * \brief   OS-specific cleanup and release of critical section.
     **/
    void _os_release_critical_section();

    /**
     * \brief   OS-specific implementation to enter and lock the critical section.
     **/
    bool _os_lock();

    /**
     * \brief   OS-specific implementation to exit and unlock the critical section.
     **/
    bool _os_unlock();

    /**
     * \brief   OS-specific implementation to attempt locking without blocking.
     **/
    bool _os_try_lock();

    //////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( CriticalSection );
};

//////////////////////////////////////////////////////////////////////////
// class SpinLock declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Recursive spin-lock for fast synchronization.
 *
 *          Uses an atomic owner-thread ID and a recursion counter to support
 *          same-thread re-entry without deadlocking. No OS primitives are
 *          required -- implemented entirely with C++17 standard atomics and
 *          portable CPU-pause hints.
 *
 *          Use only for short critical sections. Spinning wastes CPU cycles
 *          while waiting; prefer CriticalSection or Mutex for longer holds.
 *
 *          Compatible with all platforms: Windows (x86/x86-64), Linux, macOS,
 *          Cygwin, MinGW, ARM (32-bit and 64-bit). Works with MSVC, GCC, Clang.
 **/
class AREG_API SpinLock final   : public Lockable
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    SpinLock();

    virtual ~SpinLock() = default;

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Acquires spin-lock ownership. If called again from the same
     *          thread, increments the recursion counter and returns immediately.
     *          Spins until the lock is available when called from another thread.
     *
     * \return  Always returns true.
     **/
    inline bool lock();

    /**
     * \brief   Acquires spin-lock ownership (timeout parameter is ignored).
     *          Identical to lock().
     *
     * \return  Always returns true.
     **/
    bool lock( uint32_t /*timeout = areg::WAIT_INFINITE*/ ) final;

    /**
     * \brief   Releases one recursion level of the spin-lock.
     *          The underlying lock is released only when the recursion counter
     *          reaches zero.
     *
     * \return  Returns true if the calling thread owns the lock; false otherwise.
     **/
    bool unlock() final;

    /**
     * \brief   Attempts to acquire the spin-lock without spinning.
     *          If called from the owning thread, increments the recursion counter
     *          and returns true immediately.
     *          If the lock is held by another thread, returns false immediately.
     *
     * \return  Returns true if acquired or already owned; false if busy.
     **/
    bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:


#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    std::atomic<id_type>    mOwner;     //!< Thread ID of the current owner; 0 = unlocked.
    std::atomic<uint32_t>   mCount;     //!< Recursion depth; 0 when unlocked.

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( SpinLock );
};

//////////////////////////////////////////////////////////////////////////
// ResuorceLock class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   OS-specific recursive synchronization lock for resource access. Supports recursive
 *          locking within the same thread context to avoid deadlocks.
 **/
using ResourceLock = SpinLock;

//////////////////////////////////////////////////////////////////////////
// NolockSyncObject class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Dummy synchronization object that performs no locking. Used as a no-op when
 *          synchronization is not required but a SyncObject interface is expected.
 **/
class AREG_API NolockSyncObject  final  : public Lockable
{

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    NolockSyncObject();

    virtual ~NolockSyncObject() = default;

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   No-op lock operation.
     * \return  Always returns true.
     **/
    inline bool lock();

    /**
     * \brief   No-op lock operation with timeout (timeout is ignored).
     * \return  Always returns true.
     **/
    inline bool lock( uint32_t /*timeout = areg::WAIT_INFINITE*/ ) final;

    /**
     * \brief   No-op unlock operation.
     * \return  Always returns true.
     **/
    inline bool unlock() final;

    /**
     * \brief   No-op try-lock operation.
     * \return  Always returns true.
     **/
    inline bool try_lock() final;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( NolockSyncObject );
};

//////////////////////////////////////////////////////////////////////////
// class SyncTimer declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Waitable timer synchronization object. Can be manual-reset or auto-reset, and optionally
 *          periodic. The timer's state is set to signaled when the specified due time expires.
 **/
class AREG_API SyncTimer final  : public SyncObject
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a waitable timer with the specified timeout, type, and mode.
     *
     * \param   msTimeout       The timer timeout in milliseconds.
     * \param   is_periodic     If true, the timer fires repeatedly at the specified interval;
     *                          otherwise, it fires once.
     * \param   isAutoReset     If true, the timer is auto-reset (synchronization timer);
     *                          otherwise, it is manual-reset.
     * \param   isSteady        If true, uses a steady high-resolution timer; otherwise, uses a
     *                          system clock.
     **/
    SyncTimer( uint32_t msTimeout, bool is_periodic = false, bool isAutoReset = true, bool isSteady = true );

    virtual ~SyncTimer();

//////////////////////////////////////////////////////////////////////////
// Override operations, SyncObject interface
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Waits for the timer to fire or timeout to expire.
     *
     * \param   timeout     The timeout in milliseconds to wait for the timer to fire. Use
     *                      areg::WAIT_INFINITE to wait indefinitely.
     * \return  Returns true if the timer fired before timeout.
     **/
    inline bool lock( uint32_t timeout = areg::WAIT_INFINITE ) final;

    /**
     * \brief   Activates the timer. The timer will fire at the due time specified in the constructor.
     *
     * \return  Returns true if the timer was successfully activated.
     **/
    inline bool unlock() final;

//////////////////////////////////////////////////////////////////////////
// Operations / Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Activates the timer. Equivalent to calling unlock().
     *
     * \return  Returns true if the timer was successfully activated.
     **/
    inline bool set_timer();

    /**
     * \brief   Stops the timer without changing its signaled state. Threads waiting on the timer
     *          will remain waiting until timeout or reactivation.
     *
     * \return  Returns true if successfully stopped.
     **/
    inline bool cancel_timer();

    /**
     * \brief   Returns the due time in milliseconds.
     **/
    inline uint32_t due_time() const;

    /**
     * \brief   Returns true if the timer is periodic; false if it fires only once.
     **/
    [[nodiscard]]
    inline bool is_periodic() const noexcept;

    /**
     * \brief   Returns true if the timer is auto-reset; false if manual-reset.
     **/
    [[nodiscard]]
    inline bool is_autoreset() const noexcept;

//////////////////////////////////////////////////////////////////////////
// OS specific hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   OS-specific implementation to create the synchronization timer.
     *
     * \param   isSteady    If true, uses a steady high-resolution timer; otherwise, uses a system clock.
     **/
    void _os_create_timer( bool isSteady );

    /**
     * \brief   OS-specific implementation to release the timer resources.
     **/
    void _os_release_time();

    /**
     * \brief   OS-specific implementation to wait for the timer to fire. Returns immediately if the
     *          timer has already fired.
     *
     * \param   timeout     The timeout in milliseconds to wait for the timer to fire.
     * \return  Returns true if the timer fired before timeout.
     **/
    bool _os_lock( uint32_t timeout );

    /**
     * \brief   OS-specific implementation to activate the timer with the timeout from the constructor.
     *
     * \return  Returns true if successfully activated.
     **/
    bool _os_set_timer();

    /**
     * \brief   OS-specific implementation to cancel the timer.
     *
     * \return  Returns true if successfully cancelled.
     **/
    bool _os_cancel_timer();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Timeout in milliseconds
     **/
    const uint32_t  mTimeout;
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
    SyncTimer() = delete;
    AREG_NOCOPY_NOMOVE( SyncTimer );
};

//////////////////////////////////////////////////////////////////////////
// Lock class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   RAII wrapper for automatic scoped locking of synchronization objects.
  *
  * \example Mutex use
  *
  *          In this example MyClass contains Mutex as a synchronization object.
  *          Any time a thread calls foo() or bar(), a Lock instance is created on
  *          entry and automatically releases the Mutex when the function returns.
  *
  * \code[.cpp]
  *
  *          class MyClass
  *          {
  *          public:
  *              MyClass();
  *              void foo();
  *              void bar();
  *
  *          private:
  *              Mutex mMutex;
  *          };
  *
  *          MyClass::MyClass()
  *              : mMutex(true)
  *          {   ; }
  *
  *          void MyClass::foo()
  *          {
  *              Lock lock(mMutex);
  *              // perform actions here
  *          }
  *
  *          void MyClass::bar()
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
     * \brief   Initializes the lock wrapper. If autoLock is true, immediately acquires the
     *          synchronization object and releases it on destruction.
     *
     * \param   syncObj     Reference to the synchronization object.
     * \param   autoLock    If true, automatically locks on construction and unlocks on destruction.
     *                      Manual lock/unlock must be called otherwise.
     **/
    explicit Lock( SyncObject &syncObj, bool autoLock = true );

    ~Lock();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Manually acquires the synchronization object with optional timeout.
     *
     * \param   timeout     The timeout in milliseconds to wait for ownership.
     * \return  Returns true if the synchronization object was successfully locked.
     **/
    inline bool lock( uint32_t timeout = areg::WAIT_INFINITE );

    /**
     * \brief   Manually releases the synchronization object.
     *
     * \return  Returns true if the synchronization object was successfully unlocked.
     **/
    inline bool unlock();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Reference to Synchronization object passed in constructor
     **/
    SyncObject &  mSyncObject;
    /**
     * \brief   Auto-locking flag. Indicates whether synchronization
     *          object is locked / unlocked automatically or manually
     *          If true, the object automatically is locked / unlocked
     *          in constructor and in destructor
     **/
    const bool      mAutoLock;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    Lock() = delete;
    AREG_NOCOPY_NOMOVE( Lock );
};

//////////////////////////////////////////////////////////////////////////
// MultiLock class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Auto-locking synchronization for multiple objects. Waits for either one or all objects
 *          to be signaled. Cannot include CriticalSection objects.
 **/
class AREG_API MultiLock
{
//////////////////////////////////////////////////////////////////////////
// Internally defined types and constants
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   MultiLock::LockState
     *          The current locking state of every single synchronization object
     **/
    enum class LockState : uint8_t
    {
          Unlocked  = 0 //!< Unlocked state
        , Locked    = 1 //!< Locked state
    };

public:
    /**
     * \brief   MultiLock::LOCK_INDEX_INVALID
     *          Invalid index of synchronization list
     **/
    static constexpr int32_t    LOCK_INDEX_INVALID      { -1 };

    /**
     * \brief   MultiLock::LOCK_INDEX_COMPLETION
     *          The completion routine index.
     *          Returned if waiting function returns WAIT_IO_COMPLETION
     **/
    static constexpr int32_t    LOCK_INDEX_COMPLETION   { -2 };
    /**
     * \brief   MultiLock::LOCK_INDEX_TIMEOUT
     *          The index, indicating waiting timeout.
     **/
    static constexpr int32_t    LOCK_INDEX_TIMEOUT      { -3 };
    /**
     * \brief   MultiLock::LOCK_INDEX_ALL
     *          All synchronization objects are locked.
     *          Same as MAX_SIZE_OF_ARRAY (64)
     **/
    static constexpr int32_t    LOCK_INDEX_ALL          { areg::MAXIMUM_WAITING_OBJECTS };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes multi-lock with list of synchronization objects. Optionally locks all
     *          objects immediately.
     *
     * \param   pObjects    List of synchronization objects (no CriticalSection).
     * \param   count       Number of objects in the list.
     * \param   autoLock    If true, immediately locks all objects.
     * \note    CriticalSection objects are not allowed; assertion will be raised if included.
     **/
    MultiLock( SyncObject* pObjects[], int32_t count, bool autoLock = true );

    ~MultiLock();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Locks synchronization objects, waiting either for one or all to be signaled.
     *
     * \param   timeout         Timeout in milliseconds to wait.
     * \param   waitForAll      If true, waits for all objects; if false, waits for any one.
     * \param   isAlertable     If true, returns early when I/O completion or APC is queued.
     * \return  Returns the index of the locked object, LOCK_INDEX_ALL for all objects, or
     *          LOCK_INDEX_INVALID on error.
     **/
    inline int32_t lock( uint32_t timeout = areg::WAIT_INFINITE, bool waitForAll = false, bool isAlertable = false );

    /**
     * \brief   Unlocks all previously locked synchronization objects.
     *
     * \return  Returns true if all objects were unlocked.
     **/
    bool unlock();

    /**
     * \brief   Unlocks the synchronization object at the given index.
     *
     * \param   index       The index of the object to unlock.
     * \return  Returns true if the object was unlocked.
     **/
    bool unlock(int32_t index);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   OS-specific implementation to lock multiple objects. Returns the index of the locked
     *          object that the thread now owns.
     *
     * \param   timeout         Timeout in milliseconds. Returns timeout code if expired.
     * \param   waitForAll      If true, waits for all objects.
     * \param   isAlertable     If true, returns on I/O completion routine or APC.
     **/
    int32_t _os_lock( uint32_t timeout, bool waitForAll, bool isAlertable );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Locking state of every object within array.
     **/
    LockState           mLockedStates[areg::MAXIMUM_WAITING_OBJECTS];
    /**
     * \brief   List of synchronization objects passed on initialization
     **/
    SyncObject* const*  mSyncObjArray;
    /**
     * \brief   Size of synchronization object. Cannot be more than MAX_SIZE_OF_ARRAY (64)
     **/
    const int32_t       mSizeCount;
    /**
     * \brief   Flag, indicating whether auto-locking is enabled or disabled.
     **/
    const bool          mAutoLock;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor is deleted.
     **/
    MultiLock() = delete;
    AREG_NOCOPY_NOMOVE( MultiLock );
};

//////////////////////////////////////////////////////////////////////////
// Wait class
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   High-resolution wait object for suspending threads for short durations. Minimum timeout
 *          is 1 microsecond. Used for precise timing where millisecond-resolution sleep is
 *          insufficient. Not suitable for long waits or thread synchronization.
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

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    //!< One nanosecond duration
    static constexpr Duration   ONE_NS      { areg::DURATION_1_NS };
    //!< One microsecond duration
    static constexpr Duration   ONE_MUS     { areg::DURATION_1_MICRO };
    //!< One millisecond duration
    static constexpr Duration   ONE_MS      { areg::DURATION_1_MILLI };
    //!< One second duration
    static constexpr Duration   ONE_SEC     { areg::DURATION_1_SEC };
    //!< The minimum waiting timeout
    static constexpr Duration   MIN_WAIT    { ONE_MS  * 5 };

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    //!< The waiting results
    enum class WaitResolution
    {
          Invalid       = 0 //!< No waiting, due to invalid timeout
        , Ignored       = 1 //!< The waiting timeout is set, but ignored, because timeout in nanoseconds
        , Millisecond   = 2 //!< Wait thread in milliseconds or longer
        , Microsecond   = 3 //!< Wait thread in microseconds up to MIN_WAIT.
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    Wait();
    ~Wait();

//////////////////////////////////////////////////////////////////////////
// Static operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converts from high-resolution steady clock to system clock.
     *
     * \param   time    The steady time value to convert.
     * \return  Returns the converted system clock time.
     **/
    [[nodiscard]]
    static inline Wait::SystemTime convert_to_system_clock(const Wait::SteadyTime& time);
    
    /**
     * \brief   Converts from system clock to high-resolution steady clock.
     *
     * \param   time    The system time value to convert.
     * \return  Returns the converted steady clock time.
     **/
    [[nodiscard]]
    static inline Wait::SteadyTime convert_to_steady_clock(const Wait::SystemTime& time);

    /**
     * \brief   Calculates the time remaining from now until a future steady clock time.
     *
     * \param   future      A steady high-resolution time point in the future.
     * \return  Returns the duration in nanoseconds until the future time. Negative if the time is
     *          already in the past.
     **/
    [[nodiscard]]
    static inline Wait::Duration from_now(const Wait::SteadyTime& future);

    /**
     * \brief   Calculates the elapsed time from a past steady clock time until now.
     *
     * \param   passed      A steady high-resolution time point in the past.
     * \return  Returns the duration in nanoseconds elapsed since the past time. Negative if the
     *          time is in the future.
     **/
    [[nodiscard]]
    static inline Wait::Duration until_now(const Wait::SteadyTime& passed);

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Suspends the thread for the specified number of milliseconds.
     *
     * \param   ms      The duration in milliseconds to suspend the thread.
     * \return  Returns the wait resolution. Any value other than WaitResolution::Invalid indicates
     *          successful operation.
     **/
    inline Wait::WaitResolution wait(uint32_t ms) const;

    /**
     * \brief   Suspends the thread for the specified duration in nanoseconds (rounded to
     *          microseconds).
     *
     * \param   timeout     The duration in nanoseconds (rounded to microseconds) to suspend the
     *                      thread. Minimum is 1 microsecond.
     * \return  Returns the wait resolution. Any value other than WaitResolution::Invalid indicates
     *          successful operation.
     **/
    inline Wait::WaitResolution wait_for(Wait::Duration timeout) const;

    /**
     * \brief   Suspends the thread until the specified steady high-resolution time is reached.
     *
     * \param   future      A future steady high-resolution time point. The thread is not suspended
     *                      if the difference is less than 1 microsecond.
     * \return  Returns the wait resolution. Any value other than WaitResolution::Invalid indicates
     *          successful operation.
     **/
    inline Wait::WaitResolution wait_until(const Wait::SteadyTime& future) const;

//////////////////////////////////////////////////////////////////////////
// Hidden OS dependent calls
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   OS-specific implementation to initialize the timer.
     **/
    void _os_init_timer();
    /**
     * \brief   OS-specific implementation to release the timer resources.
     **/
    void _os_release_timer();
    /**
     * \brief   OS-specific implementation to suspend the thread for a specified duration.
     *
     * \param   timeout     The duration in nanoseconds to suspend the thread.
     * \return  Returns the wait resolution. Any value other than WaitResolution::Invalid indicates
     *          successful operation.
     **/
    WaitResolution _os_wait_for(const Wait::Duration& timeout) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    TIMERHANDLE mTimer; //!< OS dependent timer.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(Wait);
};

//////////////////////////////////////////////////////////////////////////
// Inline functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mutex class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool Mutex::lock( uint32_t timeout /* = areg::WAIT_INFINITE */ )
{
    ASSERT( mSyncObject != nullptr );
    return _os_lock_mutex( timeout );
}

inline bool Mutex::try_lock()
{
    return lock( areg::DO_NOT_WAIT );
}

inline bool Mutex::unlock()
{
    ASSERT( mSyncObject != nullptr );
    return _os_unlock_mutex( );
}

inline bool Mutex::is_locked() const noexcept
{
    return (mOwnerThreadId.load() != 0);
}

inline id_type Mutex::owner_thread_id() const noexcept
{
    return mOwnerThreadId.load();
}

//////////////////////////////////////////////////////////////////////////
// SyncEvent class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool SyncEvent::is_auto_reset() const noexcept
{
    return mAutoReset;
}

inline int32_t SyncEvent::wait_any( SyncEvent& ev0, SyncEvent& ev1, uint32_t timeout ) noexcept
{
    SyncEvent* events[2]{ &ev0, &ev1 };
    return wait_any(events, 2, timeout);
}

inline bool SyncEvent::unlock()
{
    return (mSyncObject != nullptr ? _os_unlock_event(mSyncObject) : false);
}

inline bool SyncEvent::lock( uint32_t timeout /* = areg::WAIT_INFINITE */ )
{
    return (mSyncObject != nullptr ? _os_lock_event(timeout) : false);
}

inline bool SyncEvent::set_signaled() noexcept
{
    return (mSyncObject != nullptr ? _os_set_event( ) : false);
}

inline bool SyncEvent::reset() noexcept
{
    return (mSyncObject != nullptr ? _os_reset_event( ) : false);
}

inline void SyncEvent::pulse_event() noexcept
{
    if (mSyncObject != nullptr)
    {
        _os_pulse_event();
    }
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class inline functions
//////////////////////////////////////////////////////////////////////////
inline uint32_t Semaphore::max_count() const noexcept
{
    return mMaxCount;
}

inline uint32_t Semaphore::current_count() const noexcept
{
    return mCurrCount.load();
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool CriticalSection::lock( uint32_t  /*timeout = areg::WAIT_INFINITE */ )
{
    ASSERT( mSyncObject != nullptr );
    return _os_lock( );
}

inline bool CriticalSection::unlock()
{
    ASSERT( mSyncObject != nullptr );
    return _os_unlock( );
}

inline bool CriticalSection::try_lock()
{
    ASSERT( mSyncObject != nullptr );
    return _os_try_lock( );
}

inline bool CriticalSection::lock()
{
    return lock( areg::WAIT_INFINITE );
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool SpinLock::lock()
{
    return lock(areg::WAIT_INFINITE);
}



//////////////////////////////////////////////////////////////////////////
// NolockSyncObject class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool NolockSyncObject::lock()
{
    return true;
}

inline bool NolockSyncObject::lock( uint32_t /*timeout = areg::WAIT_INFINITE*/ )
{
    return true;
}

inline bool NolockSyncObject::unlock()
{
    return true;
}

inline bool NolockSyncObject::try_lock()
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer class inline functions
//////////////////////////////////////////////////////////////////////////

inline bool SyncTimer::lock( uint32_t timeout /* = areg::WAIT_INFINITE */ )
{
    ASSERT( mSyncObject != nullptr );
    return _os_lock( timeout );
}

inline bool SyncTimer::unlock()
{
    ASSERT( mSyncObject != nullptr );
    return _os_cancel_timer( );
}

inline bool SyncTimer::set_timer()
{
    ASSERT( mSyncObject != nullptr );
    return _os_set_timer( );
}

inline bool SyncTimer::cancel_timer()
{
    ASSERT( mSyncObject != nullptr );
    return _os_cancel_timer( );
}

inline uint32_t SyncTimer::due_time() const
{
    return mTimeout;
}

inline bool SyncTimer::is_periodic() const noexcept
{
    return mIsPeriodic;
}

inline bool SyncTimer::is_autoreset() const noexcept
{
    return mIsAutoReset;
}

//////////////////////////////////////////////////////////////////////////
// Lock class inline functions
//////////////////////////////////////////////////////////////////////////
inline bool Lock::lock(uint32_t timeout /* = areg::WAIT_INFINITE */)
{
    return mSyncObject.lock(timeout);
}

inline bool Lock::unlock()
{
    return mSyncObject.unlock();
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class inline functions
//////////////////////////////////////////////////////////////////////////
inline int32_t MultiLock::lock( uint32_t timeout /* = areg::WAIT_INFINITE */, bool waitForAll /* = false */, bool isAlertable /*= false*/ )
{
    ASSERT( mSizeCount != 0 );
    return _os_lock( timeout, waitForAll, isAlertable );
}

//////////////////////////////////////////////////////////////////////////
// Wait class inline functions
//////////////////////////////////////////////////////////////////////////

inline Wait::SystemTime Wait::convert_to_system_clock(const Wait::SteadyTime& time)
{
    SystemTime result = std::chrono::system_clock::now();
    SteadyTime steady = std::chrono::steady_clock::now();
    return std::chrono::time_point_cast<SystemTime::duration>(result + (time - steady));
}

inline Wait::SteadyTime Wait::convert_to_steady_clock(const Wait::SystemTime& time)
{
    SteadyTime result = std::chrono::steady_clock::now();
    SystemTime system = std::chrono::system_clock::now();
    return std::chrono::time_point_cast<SteadyTime::duration>(result + (time - system));
}

inline Wait::Duration Wait::from_now(const Wait::SteadyTime& future)
{
    return (future - std::chrono::steady_clock::now());
}

inline Wait::Duration Wait::until_now(const Wait::SteadyTime& passed)
{
    return (std::chrono::steady_clock::now() - passed);
}

inline Wait::WaitResolution Wait::wait(uint32_t ms) const
{
    return _os_wait_for(Wait::Duration{ ms * areg::DURATION_1_MILLI });
}

inline Wait::WaitResolution Wait::wait_for(Wait::Duration timeout) const
{
    return _os_wait_for(timeout);
}

inline Wait::WaitResolution Wait::wait_until(const Wait::SteadyTime& future) const
{
    return _os_wait_for(future - std::chrono::steady_clock::now());
}

} // namespace areg
#endif  // AREG_BASE_SYNCPRIMITIVES_HPP
