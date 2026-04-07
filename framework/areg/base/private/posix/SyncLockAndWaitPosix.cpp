/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SyncLockAndWaitPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Lock and wait object for POSIX synchronization objects.
 *
 ************************************************************************/


#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"
#include "areg/base/private/posix/WaitablePosix.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include <algorithm>
#include <cstdint>
#include <errno.h>

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// Per-thread POSIX sync objects
//
//  Each thread that calls wait_single() / wait_multiple() needs a private
//  pthread_mutex_t + pthread_cond_t.  In the original design these were
//  allocated, initialised (6 syscalls) and destroyed (4 syscalls) on every
//  single wait call.  Moving them to thread_local reduces that to ONE
//  allocation per thread lifetime.
//
//  Thread-local WaitState is default-constructed on first use and destroyed
//  automatically when the thread exits.  SyncLockAndWaitPosix stores raw
//  (non-owning) pointers into the calling thread's TLS; the signalling
//  thread uses those pointers to wake the correct waiter.
//
//  Lifetime guarantee: SyncLockAndWaitPosix lives on the calling thread's
//  stack.  The destructor calls remove_resource_object() (global lock) so
//  no new event_signaled() call will ever find this object after the
//  destructor completes.  All _notify_event() calls are made under the
//  global lock, so they cannot race with the destructor.
//////////////////////////////////////////////////////////////////////////

namespace {

/**
 * \brief   Per-thread synchronisation state: one mutex + one condition variable,
 *          initialised once and reused for all wait calls on this thread.
 **/
struct WaitState
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    bool            valid{ false };

    WaitState() noexcept
    {
        pthread_mutexattr_t attr;
        if (areg::RETURNED_OK == ::pthread_mutexattr_init(&attr))
        {
            ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
            if (areg::RETURNED_OK == ::pthread_mutex_init(&mutex, &attr))
            {
                pthread_condattr_t condAttr;
                if (areg::RETURNED_OK == ::pthread_condattr_init(&condAttr))
                {
                    ::pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_PRIVATE);
                    valid = (areg::RETURNED_OK == ::pthread_cond_init(&cond, &condAttr));
                    ::pthread_condattr_destroy(&condAttr);
                }

                if (valid == false)
                    ::pthread_mutex_destroy(&mutex);
            }

            ::pthread_mutexattr_destroy(&attr);
        }
    }

    ~WaitState() noexcept
    {
        if (valid)
        {
            ::pthread_cond_destroy(&cond);
            ::pthread_mutex_destroy(&mutex);
            valid = false;
        }
    }
};

thread_local WaitState tls_wait_state;

} // anonymous namespace

//////////////////////////////////////////////////////////////////////////
// SyncLockAndWaitPosix class implementation
//////////////////////////////////////////////////////////////////////////

SyncLockAndWaitPosix::MapWaitIDResource & SyncLockAndWaitPosix::_map_wait_ids() noexcept
{
    static SyncLockAndWaitPosix::MapWaitIDResource _mapWaitIdResource;
    return _mapWaitIdResource;
}

SyncLockAndWaitPosix::SyncResourceMapIX & SyncLockAndWaitPosix::_map_sync_resources() noexcept
{
    static SyncLockAndWaitPosix::SyncResourceMapIX _theSyncResourceMapIX;
    return _theSyncResourceMapIX;
}

int32_t SyncLockAndWaitPosix::wait_single( WaitablePosix & syncWait, uint32_t msTimeout /* = areg::WAIT_INFINITE */ )
{
    WaitablePosix * list[] = { &syncWait };
    return wait_multiple(list, 1, true, msTimeout);
}

int32_t SyncLockAndWaitPosix::wait_multiple( WaitablePosix ** listWaitables, int32_t count, bool waitAll /* = false */, uint32_t msTimeout /* = areg::WAIT_INFINITE */ )
{
    int32_t result = static_cast<int32_t>(areg::os::SyncSignal::Invalid);
    if ( (listWaitables != nullptr) && (count > 0) )
    {
        AREG_OUTPUT_DBG("Going to wait [ %s%d ] event(s).", (waitAll && (count > 1) ? "all " : ""), count);
        SyncLockAndWaitPosix lockAndWait( listWaitables
                                        , count
                                        , waitAll ? areg::os::WaitCondition::Exact : areg::os::WaitCondition::Any
                                        , msTimeout);

        if ( !lockAndWait._is_empty() && lockAndWait._lock( ) )
        {
            SyncLockAndWaitPosix::MapWaitIDResource & mapWaitIds { SyncLockAndWaitPosix::_map_wait_ids() };
            mapWaitIds.register_resource_object(reinterpret_cast<ptr_type>(lockAndWait.mContext), &lockAndWait);

            int32_t waitResult { ENOLCK };
            while ( lockAndWait._no_event_fired( ) )
            {
                waitResult = lockAndWait._wait_condition( );
                if ( (areg::RETURNED_OK != waitResult) && (lockAndWait.mFiredEntry.load(std::memory_order_acquire) == static_cast<int32_t>(areg::os::SyncSignal::Invalid)) )
                {
                    lockAndWait.mFiredEntry.store( (waitResult == ETIMEDOUT) || (waitResult == EBUSY)
                                                 ? static_cast<int32_t>(areg::os::SyncSignal::Timeout)
                                                 : static_cast<int32_t>(areg::os::SyncSignal::Interrupted)
                                                 , std::memory_order_relaxed );
                    break;
                }
            }

            mapWaitIds.unregister_resource_object(reinterpret_cast<ptr_type>(lockAndWait.mContext));

            lockAndWait._unlock( );
        }

        result = lockAndWait.mFiredEntry.load(std::memory_order_relaxed);
    }

    return result;
}

int32_t SyncLockAndWaitPosix::event_signaled( WaitablePosix & syncWaitable ) noexcept
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_map_sync_resources() };
    Lock rcLock(mapResource.lockable());

    ListLockAndWait * waitList = mapResource.resource( &syncWaitable );
    if ( waitList == nullptr )
        return 0;

    int32_t result{ 0 };

    ASSERT( !waitList->is_empty( ) );
    AREG_OUTPUT_DBG("Waitable [ %s ] ID [ %p ] is signaled, there are [ %d ] locks associated with it."
                    , syncWaitable.name().as_string()
                    , &syncWaitable
                    , waitList->size());

    const ListLockAndWait::LISTPOS end { waitList->invalid_position() };
    for ( ListLockAndWait::LISTPOS pos = waitList->first_position( ); pos != end; )
    {
        SyncLockAndWaitPosix* lockAndWait = waitList->value_at(pos);
        ASSERT(lockAndWait != nullptr);

        if (!syncWaitable.check_signaled(lockAndWait->mContext))
            break;

        pos = waitList->next_position( pos );
        int32_t fired = lockAndWait->_check_event_fired(syncWaitable);
        if ( fired >= static_cast<int32_t>(areg::os::SyncSignal::First) && fired <= static_cast<int32_t>(areg::os::SyncSignal::All) )
        {
            if (lockAndWait->_request_ownership(fired))
            {
                AREG_OUTPUT_DBG(   "The waitable [ %s ] [ %p ] is fired, unlocking thread [ %p ] with fired event reason [ %d ]"
                            , syncWaitable.name().as_string()
                            , &syncWaitable
                            , lockAndWait->mContext
                            , static_cast<int32_t>(fired));

                ++result;
                lockAndWait->mFiredEntry.store(fired, std::memory_order_release);

                // Notify the waiter while the global lock is still held.
                // This guarantees the SyncLockAndWaitPosix object is alive:
                // the waiter's destructor calls remove_resource_object()
                // under the same global lock, so the object cannot be
                // destroyed while we hold the lock here.
                lockAndWait->_notify_event();
            }
#ifdef  DEBUG
            else
            {
                AREG_OUTPUT_WARN("The waitable [ %p ] is marked as signaled, but it rejected lock [ %p ], ignoring notifying", &syncWaitable, lockAndWait);
            }
#endif // DEBUG
        }
#ifdef DEBUG
        else if (fired > static_cast<int32_t>(areg::os::SyncSignal::All))
        {
            AREG_OUTPUT_ERR("Lock and Wait object detected unexpected fired event [ %d ]", fired);
        }
#endif // DEBUG
    }

    AREG_OUTPUT_DBG("Waitable [ %s ] ID [ %p ] releasing [ %d ] threads.", syncWaitable.name().as_string(), &syncWaitable, result);
    syncWaitable.notify_released_threads(result);

    return result;
}

void SyncLockAndWaitPosix::event_remove( WaitablePosix & syncWaitable ) noexcept
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_map_sync_resources() };
    Lock rcLock(mapResource.lockable());

    ListLockAndWait * waitList = mapResource.resource( &syncWaitable );
    if ( waitList == nullptr )
        return;

    AREG_OUTPUT_ERR("The event [ %p / %s] is cleaning resource, there is still wait list, going to notify error [ %d ] locked threads and clean resources."
                    , &syncWaitable
                    , areg::os::as_string(syncWaitable.sync_type())
                    , waitList->size());
    ASSERT( waitList->is_empty( ) == false );

    const ListLockAndWait::LISTPOS end { waitList->invalid_position() };
    for ( ListLockAndWait::LISTPOS pos = waitList->first_position( ); pos != end ; pos = waitList->next_position(pos))
    {
        SyncLockAndWaitPosix * lockAndWait = waitList->value_at(pos);
        ASSERT(lockAndWait != nullptr);
        if (syncWaitable.check_signaled(lockAndWait->mContext) == false)
            break;

        int32_t index = lockAndWait->_waitable_index( syncWaitable );
        lockAndWait->mFiredEntry.store( index != areg::INVALID_INDEX
                                        ? (index + static_cast<int32_t>(areg::os::SyncSignal::FirstError))
                                        : static_cast<int32_t>(areg::os::SyncSignal::Interrupted)
                                        , std::memory_order_release );
        lockAndWait->_notify_event();
    }

    mapResource.unregister_resource(&syncWaitable);
}

void SyncLockAndWaitPosix::event_failed( WaitablePosix & syncWaitable ) noexcept
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_map_sync_resources() };
    Lock rcLock(mapResource.lockable());

    ListLockAndWait * waitList = mapResource.resource( &syncWaitable );
    if ( waitList == nullptr )
        return;

    AREG_OUTPUT_WARN("The event [ %p ] failed, going to notify error [ %d ] locked threads.", &syncWaitable, waitList->size());
    ASSERT( waitList->is_empty( ) == false );

    const ListLockAndWait::LISTPOS end { waitList->invalid_position() };
    for ( ListLockAndWait::LISTPOS pos = waitList->first_position( ); pos != end; pos = waitList->next_position(pos))
    {
        SyncLockAndWaitPosix * lockAndWait = waitList->value_at(pos);
        ASSERT(lockAndWait != nullptr);
        if (syncWaitable.check_signaled(lockAndWait->mContext) == false)
            break;

        int32_t index = lockAndWait->_waitable_index( syncWaitable );
        ASSERT(index != areg::INVALID_INDEX);
        lockAndWait->mFiredEntry.store(index + static_cast<int32_t>(areg::os::SyncSignal::FirstError), std::memory_order_release);
        lockAndWait->_notify_event();
    }
}

bool SyncLockAndWaitPosix::is_waitable_registered( WaitablePosix & syncWaitable ) noexcept
{
    SyncResourceMapIX & mapResources { SyncLockAndWaitPosix::_map_sync_resources() };
    return mapResources.exist(&syncWaitable);
}

bool SyncLockAndWaitPosix::notify_async_signal( id_type threadId ) noexcept
{
    SyncLockAndWaitPosix::MapWaitIDResource & mapWaitIds { SyncLockAndWaitPosix::_map_wait_ids() };
    Lock wait(mapWaitIds.lockable());

    SyncLockAndWaitPosix * lockAndWait = mapWaitIds.find_resource_object(static_cast<ptr_type>(threadId));
    if (lockAndWait != nullptr)
    {
        lockAndWait->mFiredEntry.store(static_cast<int32_t>(areg::os::SyncSignal::AsyncSignal), std::memory_order_release);
        return lockAndWait->_notify_event();
    }

    return false;
}

SyncLockAndWaitPosix::SyncLockAndWaitPosix(   WaitablePosix ** listWaitables
                                            , int32_t count
                                            , areg::os::WaitCondition matchCondition
                                            , uint32_t msTimeout )
    : mDescribe         ( count > 1 ? SyncLockAndWaitPosix::WaitMode::Multiple : SyncLockAndWaitPosix::WaitMode::Single )
    , mMatchCondition   ( matchCondition )
    , mWaitTimeout      ( msTimeout )
    , mDeadline         ( { 0, 0 } )
    , mContext          ( pthread_self() )
    , mMutexPtr         ( tls_wait_state.valid ? &tls_wait_state.mutex : nullptr )
    , mCondPtr          ( tls_wait_state.valid ? &tls_wait_state.cond  : nullptr )
    , mSyncValid        ( tls_wait_state.valid )
    , mFiredEntry       ( static_cast<int32_t>(areg::os::SyncSignal::Invalid) )
    , mWaitingList      ( count )
{
    ASSERT( listWaitables  != nullptr);

    if ( msTimeout != areg::WAIT_INFINITE )
    {
        areg::os::timeout_from_now( mDeadline, msTimeout );
    }

    if ( mSyncValid )
    {
        SyncResourceMapIX & mapResources { SyncLockAndWaitPosix::_map_sync_resources() };
        mapResources.lock();
        count = std::min(areg::MAXIMUM_WAITING_OBJECTS, count);

        if ( (mMatchCondition == areg::os::WaitCondition::Any ) || (mDescribe == SyncLockAndWaitPosix::WaitMode::Single) )
        {
            for ( uint32_t i = 0; i < static_cast<uint32_t>(count); ++ i, ++ listWaitables )
            {
                WaitablePosix * syncWaitable = *listWaitables;
                if (syncWaitable != nullptr)
                {
                    ASSERT( (static_cast<uint32_t>(syncWaitable->sync_type()) & static_cast<uint32_t>(areg::os::SyncKind::SoWaitable)) != 0);

                    mapResources.register_resource_object(syncWaitable, this);

                    if (syncWaitable->check_signaled(mContext) && syncWaitable->notify_request_ownership(mContext))
                    {
                        AREG_OUTPUT_DBG("Waitable [ %s ] with ID [ %p ] of type [ %s ] is signaled, going unlock thread [ %p ]"
                                    , syncWaitable->name().as_string()
                                    , syncWaitable
                                    , areg::os::as_string(syncWaitable->sync_type())
                                    , reinterpret_cast<id_type>(mContext));

                        mFiredEntry.store(static_cast<int32_t>(i), std::memory_order_relaxed);
                        syncWaitable->notify_released_threads(1);
                        mWaitingList.resize(i);
                        break;
                    }

                    mWaitingList[i] = syncWaitable;
                }
                else
                {
                    mFiredEntry.store(static_cast<int32_t>(i + static_cast<int32_t>(areg::os::SyncSignal::FirstError)), std::memory_order_relaxed);
                    mWaitingList.resize(i);
                    break;
                }
            }
        }
        else
        {
            bool eventFired = true;

            for ( uint32_t i = 0; i < static_cast<uint32_t>(count); ++ i, ++ listWaitables )
            {
                WaitablePosix * syncWaitable = *listWaitables;
                if ( syncWaitable != nullptr )
                {
                    mapResources.register_resource_object(syncWaitable, this);
                    if ( (eventFired) && (syncWaitable->check_signaled(mContext) == false) )
                    {
                        eventFired = false;
                    }

                    mWaitingList[i] = syncWaitable;
                }
                else
                {
                    mFiredEntry.store(static_cast<int32_t>(i + static_cast<int32_t>(areg::os::SyncSignal::FirstError)), std::memory_order_relaxed);
                    mWaitingList.resize(i);
                    break;
                }
            }

            if (eventFired && _request_ownership(static_cast<int32_t>(areg::os::SyncSignal::All)))
            {
                AREG_OUTPUT_DBG("Releasing thread [ %p ], all events are fired.", reinterpret_cast<id_type>(mContext));

                mFiredEntry.store(static_cast<int32_t>(areg::os::SyncSignal::All), std::memory_order_relaxed);
                for (uint32_t i = 0; i < mWaitingList.size(); ++ i)
                {
                    mWaitingList[i]->notify_released_threads(1);
                }
            }
        }

        mapResources.unlock();
    }
}

SyncLockAndWaitPosix::~SyncLockAndWaitPosix()
{
    // Remove from the global map under the global lock. After this returns,
    // no new event_signaled() call will find this object in the map.
    SyncLockAndWaitPosix::_map_sync_resources().remove_resource_object(this, true);
}

inline bool SyncLockAndWaitPosix::_no_event_fired() const noexcept
{
    return (mFiredEntry.load(std::memory_order_acquire) == static_cast<int32_t>(areg::os::SyncSignal::Invalid));
}

inline bool SyncLockAndWaitPosix::_is_empty() const noexcept
{
    return mWaitingList.is_empty();
}

inline bool SyncLockAndWaitPosix::_notify_event() noexcept
{
    // Acquiring mMutexPtr before pthread_cond_signal prevents the missed-
    // signal race: the waiter holds mMutexPtr from before checking
    // _no_event_fired() through entry into pthread_cond_wait.  Therefore
    // the signal is always delivered AFTER the waiter is inside cond_wait.
    if (!mSyncValid)
        return false;

    ::pthread_mutex_lock(mMutexPtr);
    const bool result = (areg::RETURNED_OK == ::pthread_cond_signal(mCondPtr));
    ::pthread_mutex_unlock(mMutexPtr);
    return result;
}

inline bool SyncLockAndWaitPosix::_is_valid() const noexcept
{
    return (mSyncValid && (mWaitingList.is_empty() == false));
}

inline bool SyncLockAndWaitPosix::_lock() noexcept
{
    return (mSyncValid && (areg::RETURNED_OK == ::pthread_mutex_lock(mMutexPtr)));
}

inline void SyncLockAndWaitPosix::_unlock() noexcept
{
    if (mSyncValid)
    {
        ::pthread_mutex_unlock(mMutexPtr);
    }
}

inline int32_t SyncLockAndWaitPosix::_wait_condition()
{
    if ( mWaitTimeout == areg::WAIT_INFINITE )
    {
        return ::pthread_cond_wait( mCondPtr, mMutexPtr );
    }
    else
    {
        return ::pthread_cond_timedwait( mCondPtr, mMutexPtr, &mDeadline );
    }
}

inline int32_t SyncLockAndWaitPosix::_waitable_index( const WaitablePosix & syncWaitable ) const noexcept
{
    for ( uint32_t i = 0; i < mWaitingList.size(); ++ i )
    {
        if (mWaitingList[i] == &syncWaitable)
        {
            return i;
        }
    }

    return areg::INVALID_INDEX;
}

int32_t SyncLockAndWaitPosix::SyncLockAndWaitPosix::_check_event_fired( WaitablePosix & syncObject ) noexcept
{
    int32_t result = static_cast<int32_t>(areg::os::SyncSignal::Invalid);

    if (syncObject.check_signaled(mContext) && (mWaitingList.size() != 0) && (mFiredEntry.load(std::memory_order_relaxed) == static_cast<int32_t>(areg::os::SyncSignal::Invalid)))
    {
        if (mDescribe == SyncLockAndWaitPosix::WaitMode::Single)
        {
            result = static_cast<int32_t>(areg::os::SyncSignal::First);
        }
        else if (mMatchCondition == areg::os::WaitCondition::Any)
        {
            result = static_cast<int32_t>(_waitable_index(syncObject));
        }
        else
        {
            uint32_t i = 0;
#ifdef _DEBUG
            for ( ; i < mWaitingList.size(); ++ i)
            {
                WaitablePosix *waitable = mWaitingList[i];
                ASSERT(waitable != nullptr);
                if (waitable->check_signaled(mContext) == false)
                {
                    AREG_OUTPUT_DBG( "Waitable [ %s ] ID [ %p ] type [ %s ] is NOT signaled yet, interrupting checkup."
                        , waitable->name( ).as_string( )
                        , waitable
                        , areg::os::as_string( waitable->sync_type( ) ) );
                    break;
                }
#ifdef DEBUG
                else
                {
                    AREG_OUTPUT_DBG( "Waitable [ %s ] ID [ %p ] type [ %s ] is signaled."
                        , waitable->name( ).as_string( )
                        , waitable
                        , areg::os::as_string( waitable->sync_type( ) ) );
                }
#endif // DEBUG
            }
#else   // !_DEBUG
            for (  ; (i < mWaitingList.size()) && mWaitingList.value_at(i)->check_signaled(mContext); ++ i)
                ;
#endif  // !_DEBUG

            if (i == mWaitingList.size())
            {
                result = static_cast<int32_t>(areg::os::SyncSignal::All);
            }
        }
    }

    return result;
}

bool SyncLockAndWaitPosix::_request_ownership( int32_t firedEvent ) noexcept
{
    bool result = false;

    ASSERT(firedEvent >= static_cast<int32_t>(areg::os::SyncSignal::First) && firedEvent <= static_cast<int32_t>(areg::os::SyncSignal::All));
    if ( firedEvent != static_cast<int32_t>(areg::os::SyncSignal::All) )
    {
        ASSERT(mWaitingList.size() > static_cast<uint32_t>(firedEvent));
        WaitablePosix *waitable = mWaitingList[firedEvent];

#ifdef DEBUG
        if (waitable == nullptr)
        {
            AREG_OUTPUT_ERR("Attention! A waitable at index [ %d ] in the list [ %p ] having size [ %d] of thread [ %p ] has address [ %p ]"
                        , firedEvent
                        , this
                        , mWaitingList.size()
                        , Thread::current_thread()
                        , waitable);
        }

        ASSERT(waitable != nullptr);
#endif // DEBUG

        AREG_OUTPUT_DBG("Thread [ %p ] requests ownership of waitable [ %s ], thread waits no more waitables."
                    , reinterpret_cast<id_type>(mContext)
                    , areg::os::as_string(waitable->sync_type()));

        result = waitable->notify_request_ownership(mContext);
    }
    else
    {
        ASSERT(mWaitingList.size() <= static_cast<uint32_t>(firedEvent));
        AREG_OUTPUT_DBG("Thread [ %p ] requests ownership of [ %d ] waitables.", reinterpret_cast<id_type>(mContext), mWaitingList.size());

        result = true;
        for (uint32_t i = 0; (i < mWaitingList.size()) && result; ++ i)
        {
            WaitablePosix *waitable = mWaitingList[i];
            ASSERT(waitable != nullptr);
            result = waitable->notify_request_ownership(mContext);

            AREG_OUTPUT_DBG("Thread [ %p ] requested ownership of waitable [ %s ] listed [ %d of %d ], the ownership is [ %s ], the current state of waitable [ %s ]"
                        , reinterpret_cast<id_type>(mContext)
                        , areg::os::as_string(waitable->sync_type())
                        , i + 1
                        , mWaitingList.size()
                        , result ? "taken" : "rejected"
                        , waitable->check_signaled(mContext) ? "signaled" : "not signaled");
        }
    }

    return result;
}

} // namespace areg::os
#endif // defined(_POSIX) || defined(POSIX)
