/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/SyncPrimitives.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Synchronization objects. Common part
 *
 ************************************************************************/
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/private/WaitWord.hpp"

#include <algorithm>

namespace
{
    //!< Bits of SpinLock::mCount holding the recursion depth of the owner.
    constexpr uint32_t  LOCK_DEPTH      { 0x0000'FFFFu };

    //!< One thread asleep on SpinLock::mCount; the high bits count the sleepers.
    constexpr uint32_t  LOCK_SLEEPER    { 0x0001'0000u };

    //!< Pause re-checks before a waiter goes to sleep.
    constexpr uint32_t  LOCK_SPIN_PAUSES{ 64u };

    /**
     * \brief   Takes a SpinLock word that another thread holds: re-checks it LOCK_SPIN_PAUSES
     *          times, then counts itself as a sleeper and sleeps on the word until it is free.
     **/
    void _spin_lock_contended(std::atomic<uint32_t> & word) noexcept
    {
        for (uint32_t spin = 0u; spin < LOCK_SPIN_PAUSES; ++spin)
        {
            areg::Thread::cpu_pause();
            uint32_t value{ word.load(std::memory_order_relaxed) };
            if (((value & LOCK_DEPTH) == 0u) &&
                word.compare_exchange_weak(value, value + 1u, std::memory_order_acquire, std::memory_order_relaxed))
            {
                return;
            }
        }

        uint32_t value{ word.fetch_add(LOCK_SLEEPER, std::memory_order_relaxed) + LOCK_SLEEPER };
        for (;;)
        {
            if ((value & LOCK_DEPTH) == 0u)
            {
                // Leaves the sleepers and takes the lock in one step.
                if (word.compare_exchange_weak(value, value - LOCK_SLEEPER + 1u, std::memory_order_acquire, std::memory_order_relaxed))
                    return;

                continue;
            }

            areg::os::_os_wait_word(word, value);
            value = word.load(std::memory_order_relaxed);
        }
    }
}

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Lockable class implementation
//////////////////////////////////////////////////////////////////////////

Lockable::Lockable( SyncObject::SyncKind syncObjectType )
    : SyncObject   (syncObjectType)
{
    ASSERT( syncObjectType == SyncObject::SyncKind::SoMutex      ||
            syncObjectType == SyncObject::SyncKind::SoSemaphore  ||
            syncObjectType == SyncObject::SyncKind::SoCritical   ||
            syncObjectType == SyncObject::SyncKind::SoSpinlock   ||
            syncObjectType == SyncObject::SyncKind::SoNolock     );
}

bool Lockable::try_lock()
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// Mutex class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Mutex class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Mutex::Mutex( bool initLock /* = true */ )
    : Lockable( SyncObject::SyncKind::SoMutex )
    , mOwnerThreadId( 0 )
{
    _os_create_mutex( initLock );
}

Mutex::~Mutex()
{
    _os_unlock_mutex( );
}

//////////////////////////////////////////////////////////////////////////
// SyncEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SyncEvent class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SyncEvent::SyncEvent( bool initLock /* = true */, bool autoReset /* = true */ )
    : SyncObject( SyncObject::SyncKind::SoEvent )
    , mAutoReset( autoReset )
{
    _os_create_event( initLock );
}

SyncEvent::SyncEvent( areg::NullTag ) noexcept
    : SyncObject( SyncObject::SyncKind::SoEvent )
    , mAutoReset( false )
{
    // mSyncObject stays nullptr (set by SyncObject base). No OS handle allocated.
}

SyncEvent::~SyncEvent()
{
    if (mSyncObject != nullptr)
        _os_unlock_event( mSyncObject );
}

int32_t SyncEvent::wait_any( SyncEvent** events, int32_t count, uint32_t timeout /* = areg::WAIT_INFINITE */ ) noexcept
{
    if ( (events == nullptr) || (count <= 0) )
    {
        return SyncEvent::WAIT_ANY_TIMEOUT;
    }

    return _os_wait_any( events, std::min(count, areg::MAXIMUM_WAITING_OBJECTS), timeout );
}

//////////////////////////////////////////////////////////////////////////
// Semaphore class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Semaphore class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Semaphore::Semaphore( int32_t maxCount, int32_t initCount /* = 0 */ )
    : Lockable( SyncObject::SyncKind::SoSemaphore )

    , mMaxCount ( static_cast<uint32_t>(std::max<int>(maxCount, 1)) )
    , mCurrCount( areg::is_in_range<int32_t>(initCount, 0, static_cast<int32_t>(mMaxCount)) ? static_cast<uint32_t>(initCount) : 0 )
{
    _os_create_semaphore( );
}

Semaphore::~Semaphore()
{
    ASSERT( mSyncObject != nullptr );
    _os_release_semaphore( );
}

bool Semaphore::lock( uint32_t timeout /* = areg::WAIT_INFINITE */ )
{
    ASSERT( mSyncObject != nullptr );
    bool result = false;
    if ( _os_lock( timeout ) )
    {
        mCurrCount.fetch_sub( 1 );
        result = true;
    }

    return result;
}

bool Semaphore::unlock()
{
    ASSERT( mSyncObject != nullptr );
    bool result = false;
    if ( _os_unlock() )
    {
        mCurrCount.fetch_add( 1 );
        result = true;
    }

    return result;
}

bool Semaphore::try_lock()
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// CriticalSection implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CriticalSection class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CriticalSection::CriticalSection()
    : Lockable( SyncObject::SyncKind::SoCritical )
{
    _os_create_critical_section( );
}

CriticalSection::~CriticalSection()
{
    ASSERT( mSyncObject != nullptr );
    _os_release_critical_section( );
}

//////////////////////////////////////////////////////////////////////////
// SpinLock class implementation
//////////////////////////////////////////////////////////////////////////

SpinLock::SpinLock()
    : Lockable(SyncObject::SyncKind::SoSpinlock)
    , mOwner(0)
    , mCount(0u)
{
}

bool SpinLock::lock(uint32_t /*timeout = areg::WAIT_INFINITE*/)
{
    const id_type self = Thread::current_thread_id();
    if (mOwner.load(std::memory_order_relaxed) == self)
    {
        mCount.fetch_add(1u, std::memory_order_relaxed);
        return true;
    }

    uint32_t expected{ 0u };
    if (mCount.compare_exchange_strong(expected, 1u, std::memory_order_acquire, std::memory_order_relaxed) == false)
    {
        _spin_lock_contended(mCount);
    }

    mOwner.store(self, std::memory_order_relaxed);
    return true;
}

bool SpinLock::try_lock()
{
    const id_type self = Thread::current_thread_id();
    if (mOwner.load(std::memory_order_relaxed) == self)
    {
        mCount.fetch_add(1u, std::memory_order_relaxed);
        return true;
    }

    uint32_t value{ mCount.load(std::memory_order_relaxed) };
    if (((value & LOCK_DEPTH) == 0u) &&
        mCount.compare_exchange_strong(value, value + 1u, std::memory_order_acquire, std::memory_order_relaxed))
    {
        mOwner.store(self, std::memory_order_relaxed);
        return true;
    }

    return false;
}

bool SpinLock::unlock()
{
    const id_type self = Thread::current_thread_id();
    if (mOwner.load(std::memory_order_relaxed) != self)
    {
        return false;
    }

    // The owner is cleared before the release and restored if the lock is still held.
    mOwner.store(0, std::memory_order_relaxed);
    const uint32_t value{ mCount.fetch_sub(1u, std::memory_order_release) };
    if ((value & LOCK_DEPTH) > 1u)
    {
        mOwner.store(self, std::memory_order_relaxed);
    }
    else if (value >= LOCK_SLEEPER)
    {
        areg::os::_os_wake_word(mCount);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
// NolockSyncObject class implementation
//////////////////////////////////////////////////////////////////////////

NolockSyncObject::NolockSyncObject()
    : Lockable( SyncObject::SyncKind::SoNolock )
{
}

//////////////////////////////////////////////////////////////////////////
// SyncTimer implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SyncTimer class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SyncTimer::SyncTimer( uint32_t msTimeout, bool is_periodic /* = false */, bool is_auto_reset /* = true */, bool isSteady /* = true */ )
    : SyncObject  ( SyncObject::SyncKind::SoTimer )

    , mTimeout      ( msTimeout )
    , mIsPeriodic   ( is_periodic )
    , mIsAutoReset  ( is_auto_reset )
{
    _os_create_timer( isSteady );
}

SyncTimer::~SyncTimer()
{
    ASSERT( mSyncObject != nullptr );
    _os_release_time( );
    mSyncObject = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// MultiLock class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MultiLock class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
MultiLock::MultiLock(SyncObject* pObjects[], int32_t count, bool autoLock /* = true */)
    : mSyncObjArray (pObjects)
    , mSizeCount    (std::min(count, areg::MAXIMUM_WAITING_OBJECTS))
    , mAutoLock     (autoLock)
{
#ifdef DEBUG
    for (int32_t i = 0; i < mSizeCount; ++i)
    {
        ASSERT(pObjects[i] != nullptr);
        switch (pObjects[i]->type())
        {
        case SyncObject::SyncKind::SoMutex:
        case SyncObject::SyncKind::SoEvent:
        case SyncObject::SyncKind::SoSemaphore:
        case SyncObject::SyncKind::SoTimer:
            break;

        case SyncObject::SyncKind::SoCritical:
        case SyncObject::SyncKind::SoNolock:
        case SyncObject::SyncKind::SoSpinlock:
        case SyncObject::SyncKind::SoUnknown:
        default:
            ASSERT(false);  // multi-lock is not for these objects
            break;
        }
    }
#endif // DEBUG

    areg::mem_zero(static_cast<void *>(mLockedStates), areg::MAXIMUM_WAITING_OBJECTS * sizeof(LockState)  );
    if ((mSizeCount != 0) && autoLock)
    {
        lock(areg::WAIT_INFINITE, true);
    }
}

MultiLock::~MultiLock()
{
    if (mAutoLock)
    {
        unlock();
    }
}

bool MultiLock::unlock()
{
    for (int i = 0; i < mSizeCount; ++ i)
    {
        if (mLockedStates[i] == MultiLock::LockState::Locked)
        {
            mSyncObjArray[i]->unlock( );
        }
    }

    return true;
}

bool MultiLock::unlock( int32_t index )
{
    bool result = false;
    if ( (index >= 0) && (index < mSizeCount) )
    {
        if (mLockedStates[index] == MultiLock::LockState::Locked)
        {
            result = mSyncObjArray[index]->unlock( );
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Wait class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Wait class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
Wait::Wait()
    : mTimer(nullptr)
{
    _os_init_timer();
}

Wait::~Wait()
{
    _os_release_timer();
}

} // namespace areg
