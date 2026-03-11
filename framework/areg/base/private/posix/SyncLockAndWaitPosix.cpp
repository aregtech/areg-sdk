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

#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"
#include <cstdint>

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/WaitablePosix.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include <algorithm>
#include <errno.h>
namespace areg::os {

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
                Lock lock(mapWaitIds.lockable());
                if ( (areg::RETURNED_OK  != waitResult) && (lockAndWait.mFiredEntry == static_cast<int32_t>(areg::os::SyncSignal::Invalid)) )
                {
                    lockAndWait.mFiredEntry = (waitResult == ETIMEDOUT) || (waitResult == EBUSY) ? static_cast<int32_t>(areg::os::SyncSignal::Timeout) : static_cast<int32_t>(areg::os::SyncSignal::Interrupted);
                    break;
                }
            }

            mapWaitIds.unregister_resource_object(reinterpret_cast<ptr_type>(lockAndWait.mContext));

            lockAndWait._unlock( );
        }

        result =lockAndWait.mFiredEntry;
    }

    return result;
}

int32_t SyncLockAndWaitPosix::event_signaled( WaitablePosix & syncWaitable ) noexcept
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_map_sync_resources() };
    Lock rcLock(mapResource.lockable());

    ListLockAndWait * waitList = mapResource.resource( &syncWaitable );
    if ( waitList == nullptr)
        return 0;

    int32_t result { 0 };
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

                ++ result;
                lockAndWait->mFiredEntry = fired;
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

    AREG_OUTPUT_DBG("Waitable [ %s ] ID [ %p ] released [ %d ] threads.", syncWaitable.name().as_string(), &syncWaitable, result);
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
        lockAndWait->mFiredEntry = index != areg::INVALID_INDEX 
                                    ? (index + static_cast<int32_t>(areg::os::SyncSignal::FirstError))
                                    : static_cast<int32_t>(areg::os::SyncSignal::Interrupted);
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
        lockAndWait->mFiredEntry = index + static_cast<int32_t>(areg::os::SyncSignal::FirstError);
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
    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_map_sync_resources() };
    Lock rcLock(mapResource.lockable());

    do 
    {
        SyncLockAndWaitPosix::MapWaitIDResource & mapWaitIds { SyncLockAndWaitPosix::_map_wait_ids() };
        Lock wait(mapWaitIds.lockable());

        SyncLockAndWaitPosix * lockAndWait = mapWaitIds.find_resource_object(static_cast<ptr_type>(threadId));
        if (lockAndWait != nullptr)
        {
            lockAndWait->mFiredEntry = static_cast<int32_t>(areg::os::SyncSignal::AsyncSignal);
            return lockAndWait->_notify_event();
        }

    } while (false);

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
    , mPosixMutex       ( )
    , mMutexValid       ( false )
    , mPosixMutexAttr   ( )
    , mMutexAttrValid   ( false )
    , mCondVariable     ( )
    , mCondVarValid     ( false )
    , mCondAttribute    ( )
    , mCondAttrValid    ( false )
    , mFiredEntry       ( static_cast<int32_t>(areg::os::SyncSignal::Invalid) )
    , mWaitingList      ( count )
{
    ASSERT( listWaitables  != nullptr);

    if ( msTimeout != areg::WAIT_INFINITE )
    {
        areg::os::timeout_from_now( mDeadline, msTimeout );
    }

    if ( _init_sync_objects() )
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
                    // AREG_OUTPUT_DBG("Waitable [ %p ] is [ %s ]", syncWaitable, syncWaitable->IsSignaled() ? "signaled" : "not signaled");

                    if (syncWaitable->check_signaled(mContext) && syncWaitable->notify_request_ownership(mContext))
                    {
                        AREG_OUTPUT_DBG("Waitable [ %s ] with ID [ %p ] of type [ %s ] is signaled, going unlock thread [ %p ]"
                                    , syncWaitable->name().as_string()
                                    , syncWaitable
                                    , areg::os::as_string(syncWaitable->sync_type())
                                    , reinterpret_cast<id_type>(mContext));

                        mFiredEntry = static_cast<int32_t>(i);
                        syncWaitable->notify_released_threads(1);
                        mWaitingList.resize(i);
                        break;
                    }

                    mWaitingList[i] = syncWaitable;
                }
                else
                {
                    mFiredEntry = static_cast<int32_t>(i + static_cast<int32_t>(areg::os::SyncSignal::FirstError));
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
                    mFiredEntry = (i + static_cast<int32_t>(areg::os::SyncSignal::FirstError));
                    mWaitingList.resize(i);
                    break;
                }
            }

            if (eventFired && _request_ownership(static_cast<int32_t>(areg::os::SyncSignal::All)))
            {
                AREG_OUTPUT_DBG("Releasing thread [ %p ], all events are fired.", reinterpret_cast<id_type>(mContext));

                mFiredEntry = static_cast<int32_t>(areg::os::SyncSignal::All);
                for (uint32_t i = 0; i < mWaitingList.size(); ++ i)
                {
                    mWaitingList[i]->notify_released_threads(1);
                }
            }
        }

        mapResources.unlock();
    }
    else
    {
        _release_sync_objects();
    }
}

SyncLockAndWaitPosix::~SyncLockAndWaitPosix()
{
	SyncLockAndWaitPosix::_map_sync_resources().remove_resource_object(this, true);
}

inline bool SyncLockAndWaitPosix::_no_event_fired() const noexcept
{
    return (mFiredEntry == static_cast<int32_t>(areg::os::SyncSignal::Invalid));
}

inline bool SyncLockAndWaitPosix::_is_empty() const noexcept
{
    return mWaitingList.is_empty();
}

inline bool SyncLockAndWaitPosix::_notify_event() noexcept
{
    bool result = false;

    if (_is_valid() && _lock())
    {
        result = areg::RETURNED_OK == pthread_cond_signal(&mCondVariable);
        _unlock();
    }

    return result;
}

inline bool SyncLockAndWaitPosix::_init_sync_objects() noexcept
{
    // Init POSIX mutex
    if (areg::RETURNED_OK == ::pthread_mutexattr_init( &mPosixMutexAttr ))
    {
        mMutexAttrValid = true;
        if (areg::RETURNED_OK == ::pthread_mutexattr_settype( &mPosixMutexAttr, PTHREAD_MUTEX_NORMAL ))
        {
            mMutexValid = (areg::RETURNED_OK == ::pthread_mutex_init( &mPosixMutex, &mPosixMutexAttr ));
        }
    }

    // Init POSIX condition variable
    if (areg::RETURNED_OK == ::pthread_condattr_init( &mCondAttribute ))
    {
        mCondAttrValid = true;
        if (areg::RETURNED_OK == ::pthread_condattr_setpshared( &mCondAttribute, PTHREAD_PROCESS_PRIVATE ))
        {
            mCondVarValid = (areg::RETURNED_OK == ::pthread_cond_init( &mCondVariable, &mCondAttribute ));
        }
    }

    return (mMutexValid && mCondVarValid);
}

inline void SyncLockAndWaitPosix::_release_sync_objects() noexcept
{
    if (mMutexValid)
    {
        ::pthread_mutex_destroy(&mPosixMutex);
        mMutexValid = false;
    }

    if (mMutexAttrValid)
    {
        ::pthread_mutexattr_destroy(&mPosixMutexAttr);
        mMutexAttrValid = false;
    }

    if (mCondVarValid)
    {
        ::pthread_cond_destroy(&mCondVariable);
        mCondVarValid = false;
    }

    if (mCondAttrValid)
    {
        ::pthread_condattr_destroy(&mCondAttribute);
        mCondAttrValid = false;
    }
}

inline bool SyncLockAndWaitPosix::_is_valid() const noexcept
{
    return ((mMutexValid && mCondVarValid) && (mWaitingList.is_empty() == false));
}

inline bool SyncLockAndWaitPosix::_lock() noexcept
{
    return (mMutexValid && (areg::RETURNED_OK == pthread_mutex_lock(&mPosixMutex)));
}

inline void SyncLockAndWaitPosix::_unlock() noexcept
{
    if (mMutexValid)
    {
        ::pthread_mutex_unlock( &mPosixMutex );
    }
}

inline int32_t SyncLockAndWaitPosix::_wait_condition()
{
    if ( mWaitTimeout == areg::WAIT_INFINITE )
    {
        return ::pthread_cond_wait( &mCondVariable, &mPosixMutex );
    }
    else
    {
        return ::pthread_cond_timedwait( &mCondVariable, &mPosixMutex, &mDeadline );
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

    if (syncObject.check_signaled(mContext) && (mWaitingList.size() != 0) && (mFiredEntry == static_cast<int32_t>(areg::os::SyncSignal::Invalid)))
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
