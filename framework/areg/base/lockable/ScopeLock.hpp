#ifndef AREG_BASE_SCOPELOCK_HPP
#define AREG_BASE_SCOPELOCK_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/lockable/ScopeLock.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects
 *              Declared following synchronization objects:
 *              Lock                - Single synchronization auto locking object.
 *              MultiLock           - Multiple synchronization auto locking object.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncObject.hpp"

namespace areg {
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
     * \brief   Size of synchronization object. 
     *          Cannot be more than MAX_SIZE_OF_ARRAY (64)
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

} // namespace areq

#endif // AREG_BASE_SCOPELOCK_HPP

