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

//////////////////////////////////////////////////////////////////////////
// SyncLockAndWaitPosix class implementation
//////////////////////////////////////////////////////////////////////////

SyncLockAndWaitPosix::MapWaitIDResource & SyncLockAndWaitPosix::_mapWaitResourceIds()
{
    static SyncLockAndWaitPosix::MapWaitIDResource _mapWaitIdResource;
    return _mapWaitIdResource;
}

SyncLockAndWaitPosix::SyncResourceMapIX & SyncLockAndWaitPosix::_mapSyncResources()
{
    static SyncLockAndWaitPosix::SyncResourceMapIX _theSyncResourceMapIX;
    return _theSyncResourceMapIX;
}

int SyncLockAndWaitPosix::waitForSingleObject( WaitablePosix & syncWait, unsigned int msTimeout /* = NECommon::WAIT_INFINITE */ )
{
    WaitablePosix * list[] = { &syncWait };
    return waitForMultipleObjects(list, 1, true, msTimeout);
}

int SyncLockAndWaitPosix::waitForMultipleObjects( WaitablePosix ** listWaitables, int count, bool waitAll /* = false */, unsigned int msTimeout /* = NECommon::WAIT_INFINITE */ )
{
    int result = static_cast<int>(NESyncTypesIX::SyncSignal::Invalid);
    if ( (listWaitables != nullptr) && (count > 0) )
    {
        AREG_OUTPUT_DBG("Going to wait [ %s%d ] event(s).", (waitAll && (count > 1) ? "all " : ""), count);
        SyncLockAndWaitPosix lockAndWait(    listWaitables
                                        , count
                                        , waitAll ? NESyncTypesIX::WaitCondition::Exact : NESyncTypesIX::WaitCondition::Any
                                        , msTimeout);

        if ( (lockAndWait._isEmpty() == false) && lockAndWait._lock( ) )
        {
            SyncLockAndWaitPosix::MapWaitIDResource & mapReousrces { SyncLockAndWaitPosix::_mapWaitResourceIds() };
            mapReousrces.registerResourceObject(reinterpret_cast<ptr_type>(lockAndWait.mContext), &lockAndWait);

            int waitResult = ENOLCK;
            bool makeLoop = true;
            while ( makeLoop && lockAndWait._noEventFired( ) )
            {
                waitResult = lockAndWait._waitCondition( );
                mapReousrces.lock();
                if ( (NECommon::RETURNED_OK  != waitResult) && (lockAndWait.mFiredEntry == static_cast<int32_t>(NESyncTypesIX::SyncSignal::Invalid)) )
                {
                    lockAndWait.mFiredEntry = (waitResult == ETIMEDOUT) || (waitResult == EBUSY) ? static_cast<int32_t>(NESyncTypesIX::SyncSignal::Timeout) : static_cast<int32_t>(NESyncTypesIX::SyncSignal::Interrupted);
                    makeLoop = false;
                }
                mapReousrces.unlock();
            }

            mapReousrces.unregisterResourceObject(reinterpret_cast<ptr_type>(lockAndWait.mContext));

            lockAndWait._unlock( );
        }

        result =lockAndWait.mFiredEntry;
    }

    return result;
}

int SyncLockAndWaitPosix::eventSignaled( WaitablePosix & syncWaitable )
{
    int result = 0;

    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_mapSyncResources() };
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &syncWaitable );
    if ( waitList != nullptr)
    {
        ASSERT( waitList->isEmpty( ) == false );
        AREG_OUTPUT_DBG("Waitable [ %s ] ID [ %p ] is signaled, there are [ %d ] locks associated with it."
                    , syncWaitable.getName().getString()
                    , &syncWaitable
                    , waitList->getSize());

        ListLockAndWait::LISTPOS end = waitList->invalidPosition();
        for ( ListLockAndWait::LISTPOS pos = waitList->firstPosition( ); pos != end; )
        {
            SyncLockAndWaitPosix * lockAndWait = waitList->valueAtPosition(pos);
            ASSERT(lockAndWait != nullptr);

            if (syncWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            pos = waitList->nextPosition( pos );
            int32_t fired = lockAndWait->_checkEventFired(syncWaitable);
            if ( fired >= static_cast<int32_t>(NESyncTypesIX::SyncSignal::First) && fired <= static_cast<int32_t>(NESyncTypesIX::SyncSignal::All) )
            {
                if (lockAndWait->_requestOwnership(fired))
                {
                    AREG_OUTPUT_DBG(   "The waitable [ %s ] [ %p ] is fired, unlocking thread [ %p ] with fired event reason [ %d ]"
                                , syncWaitable.getName().getString()
                                , &syncWaitable
                                , lockAndWait->mContext
                                , static_cast<int>(fired));

                    ++ result;
                    lockAndWait->mFiredEntry = fired;
                    lockAndWait->_notifyEvent();
                }
#ifdef  DEBUG
                else
                {
                    AREG_OUTPUT_WARN("The waitable [ %p ] is marked as signaled, but it rejected lock [ %p ], ignoring notifying", &syncWaitable, lockAndWait);
                }
#endif // DEBUG
            }
#ifdef DEBUG
            else if (fired > static_cast<int32_t>(NESyncTypesIX::SyncSignal::All))
            {
                AREG_OUTPUT_ERR("Lock and Wait object detected unexpected fired event [ %d ]", fired);
            }
#endif // DEBUG

        }

        AREG_OUTPUT_DBG("Waitable [ %s ] ID [ %p ] released [ %d ] threads.", syncWaitable.getName().getString(), &syncWaitable, result);
        syncWaitable.notifyReleasedThreads(result);
    }

    mapResource.unlock( );
    return result;
}

void SyncLockAndWaitPosix::eventRemove( WaitablePosix & syncWaitable )
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_mapSyncResources() };
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &syncWaitable );
    if ( waitList != nullptr )
    {
        AREG_OUTPUT_ERR("The event [ %p / %s] is cleaning resource, there is still wait list, going to notify error [ %d ] locked threads and clean resources."
                    , &syncWaitable
                    , NESyncTypesIX::getString(syncWaitable.getSyncType())
                    , waitList->getSize());
        ASSERT( waitList->isEmpty( ) == false );

        ListLockAndWait::LISTPOS end = waitList->invalidPosition();
        for ( ListLockAndWait::LISTPOS pos = waitList->firstPosition( ); pos != end ; pos = waitList->nextPosition(pos))
        {
            SyncLockAndWaitPosix * lockAndWait = waitList->valueAtPosition(pos);
            ASSERT(lockAndWait != nullptr);
            if (syncWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            int index = lockAndWait->_getWaitableIndex( syncWaitable );
            lockAndWait->mFiredEntry = index != NECommon::INVALID_INDEX 
                                        ? (index + static_cast<int32_t>(NESyncTypesIX::SyncSignal::FirstError))
                                        : static_cast<int32_t>(NESyncTypesIX::SyncSignal::Interrupted);
            lockAndWait->_notifyEvent();
        }

        mapResource.unregisterResource(&syncWaitable);
    }

    mapResource.unlock( );
}

void SyncLockAndWaitPosix::eventFailed( WaitablePosix & syncWaitable )
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_mapSyncResources() };
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &syncWaitable );
    if ( waitList != nullptr )
    {
        AREG_OUTPUT_WARN("The event [ %p ] failed, going to notify error [ %d ] locked threads.", &syncWaitable, waitList->getSize());
        ASSERT( waitList->isEmpty( ) == false );

        ListLockAndWait::LISTPOS end = waitList->invalidPosition();
        for ( ListLockAndWait::LISTPOS pos = waitList->firstPosition( ); pos != end; pos = waitList->nextPosition(pos))
        {
            SyncLockAndWaitPosix * lockAndWait = waitList->valueAtPosition(pos);
            ASSERT(lockAndWait != nullptr);
            if (syncWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            int index = lockAndWait->_getWaitableIndex( syncWaitable );
            ASSERT(index != NECommon::INVALID_INDEX);
            lockAndWait->mFiredEntry = index + static_cast<int32_t>(NESyncTypesIX::SyncSignal::FirstError);
            lockAndWait->_notifyEvent();
        }
    }

    mapResource.unlock( );
}

bool SyncLockAndWaitPosix::isWaitableRegistered( WaitablePosix & syncWaitable )
{
    bool result = false;

    SyncResourceMapIX & mapResources { SyncLockAndWaitPosix::_mapSyncResources() };
    mapResources.lock();

    result = mapResources.existResource(&syncWaitable);

    mapResources.unlock();

    return result;
}

bool SyncLockAndWaitPosix::notifyAsyncSignal( id_type threadId )
{
    bool result{false};

    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_mapSyncResources() };
    mapResource.lock( );
    do 
    {
        SyncLockAndWaitPosix::MapWaitIDResource & mapReousrces { SyncLockAndWaitPosix::_mapWaitResourceIds() };
        mapReousrces.lock();
        SyncLockAndWaitPosix * lockAndWait = mapReousrces.findResourceObject(static_cast<ptr_type>(threadId));
        if (lockAndWait != nullptr)
        {
            lockAndWait->mFiredEntry = static_cast<int32_t>(NESyncTypesIX::SyncSignal::AsyncSignal);
            result = lockAndWait->_notifyEvent();
        }

        mapReousrces.unlock();
    } while (false);

    mapResource.unlock();

    return result;
}

SyncLockAndWaitPosix::SyncLockAndWaitPosix(   WaitablePosix ** listWaitables
                                            , int count
                                            , NESyncTypesIX::WaitCondition matchCondition
                                            , unsigned int msTimeout )
    : mDescribe         ( count > 1 ? SyncLockAndWaitPosix::WaitMode::Multiple : SyncLockAndWaitPosix::WaitMode::Single )
    , mMatchCondition   ( matchCondition )
    , mWaitTimeout      ( msTimeout )
    , mContext          ( pthread_self() )
    , mPosixMutex       ( )
    , mMutexValid       ( false )
    , mPosixMutexAttr   ( )
    , mMutexAttrValid   ( false )
    , mCondVariable     ( )
    , mCondVarValid     ( false )
    , mCondAttribute    ( )
    , mCondAttrValid    ( false )
    , mFiredEntry       ( static_cast<int32_t>(NESyncTypesIX::SyncSignal::Invalid) )
    , mWaitingList      ( count )
{
    ASSERT( listWaitables  != nullptr);

    if ( _initPosixSyncObjects() )
    {
        SyncResourceMapIX & mapResources { SyncLockAndWaitPosix::_mapSyncResources() };
        mapResources.lock();
        count = std::min(NECommon::MAXIMUM_WAITING_OBJECTS, count);

        if ( (mMatchCondition == NESyncTypesIX::WaitCondition::Any ) || (mDescribe == SyncLockAndWaitPosix::WaitMode::Single) )
        {
            for ( uint32_t i = 0; i < static_cast<uint32_t>(count); ++ i, ++ listWaitables )
            {
                WaitablePosix * syncWaitable = *listWaitables;
                if (syncWaitable != nullptr)
                {
                    ASSERT( (static_cast<unsigned int>(syncWaitable->getSyncType()) & static_cast<unsigned int>(NESyncTypesIX::SyncKind::SoWaitable)) != 0);

                    mapResources.registerResourceObject(syncWaitable, this);
                    // AREG_OUTPUT_DBG("Waitable [ %p ] is [ %s ]", syncWaitable, syncWaitable->IsSignaled() ? "signaled" : "not signaled");

                    if (syncWaitable->checkSignaled(mContext) && syncWaitable->notifyRequestOwnership(mContext))
                    {
                        AREG_OUTPUT_DBG("Waitable [ %s ] with ID [ %p ] of type [ %s ] is signaled, going unlock thread [ %p ]"
                                    , syncWaitable->getName().getString()
                                    , syncWaitable
                                    , NESyncTypesIX::getString(syncWaitable->getSyncType())
                                    , reinterpret_cast<id_type>(mContext));

                        mFiredEntry = static_cast<int32_t>(i);
                        syncWaitable->notifyReleasedThreads(1);
                        mWaitingList.resize(i);
                        break;
                    }

                    mWaitingList[i] = syncWaitable;
                }
                else
                {
                    mFiredEntry = static_cast<int32_t>(i + static_cast<int32_t>(NESyncTypesIX::SyncSignal::FirstError));
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
                    mapResources.registerResourceObject(syncWaitable, this);
                    if ( (eventFired) && (syncWaitable->checkSignaled(mContext) == false) )
                    {
                        eventFired = false;
                    }

                    mWaitingList[i] = syncWaitable;
                }
                else
                {
                    mFiredEntry = (i + static_cast<int32_t>(NESyncTypesIX::SyncSignal::FirstError));
                    mWaitingList.resize(i);
                    break;
                }
            }

            if (eventFired && _requestOwnership(static_cast<int32_t>(NESyncTypesIX::SyncSignal::All)))
            {
                AREG_OUTPUT_DBG("Releasing thread [ %p ], all events are fired.", reinterpret_cast<id_type>(mContext));

                mFiredEntry = static_cast<int32_t>(NESyncTypesIX::SyncSignal::All);
                for (uint32_t i = 0; i < mWaitingList.getSize(); ++ i)
                {
                    mWaitingList[i]->notifyReleasedThreads(1);
                }
            }
        }

        mapResources.unlock();
    }
    else
    {
        _releasePosixSyncObjects();
    }
}

SyncLockAndWaitPosix::~SyncLockAndWaitPosix()
{
	SyncLockAndWaitPosix::_mapSyncResources().removeResourceObject(this, true);
}

inline bool SyncLockAndWaitPosix::_noEventFired() const
{
    return (mFiredEntry == static_cast<int32_t>(NESyncTypesIX::SyncSignal::Invalid));
}

inline bool SyncLockAndWaitPosix::_isEmpty() const
{
    return mWaitingList.isEmpty();
}

inline bool SyncLockAndWaitPosix::_notifyEvent()
{
    bool result = false;

    if (_isValid() && _lock())
    {
        result = NECommon::RETURNED_OK == pthread_cond_signal(&mCondVariable);
        _unlock();
    }

    return result;
}

inline bool SyncLockAndWaitPosix::_initPosixSyncObjects()
{
    // Init POSIX mutex
    if (NECommon::RETURNED_OK == ::pthread_mutexattr_init( &mPosixMutexAttr ))
    {
        mMutexAttrValid = true;
        if (NECommon::RETURNED_OK == ::pthread_mutexattr_settype( &mPosixMutexAttr, PTHREAD_MUTEX_NORMAL ))
        {
            mMutexValid = (NECommon::RETURNED_OK == ::pthread_mutex_init( &mPosixMutex, &mPosixMutexAttr ));
        }
    }

    // Init POSIX condition variable
    if (NECommon::RETURNED_OK == ::pthread_condattr_init( &mCondAttribute ))
    {
        mCondAttrValid = true;
        if (NECommon::RETURNED_OK == ::pthread_condattr_setpshared( &mCondAttribute, PTHREAD_PROCESS_PRIVATE ))
        {
            mCondVarValid = (NECommon::RETURNED_OK == ::pthread_cond_init( &mCondVariable, &mCondAttribute ));
        }
    }

    return (mMutexValid && mCondVarValid);
}

inline void SyncLockAndWaitPosix::_releasePosixSyncObjects()
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

inline bool SyncLockAndWaitPosix::_isValid() const
{
    return ((mMutexValid && mCondVarValid) && (mWaitingList.isEmpty() == false));
}

inline bool SyncLockAndWaitPosix::_lock()
{
    return (mMutexValid && (NECommon::RETURNED_OK == pthread_mutex_lock(&mPosixMutex)));
}

inline void SyncLockAndWaitPosix::_unlock()
{
    if (mMutexValid)
    {
        ::pthread_mutex_unlock( &mPosixMutex );
    }
}

inline int SyncLockAndWaitPosix::_waitCondition()
{
    if ( mWaitTimeout == NECommon::WAIT_INFINITE)
    {
        return ::pthread_cond_wait(&mCondVariable, &mPosixMutex);
    }
    else
    {
        timespec waitTimeout;
        NESyncTypesIX::timeoutFromNow(waitTimeout, mWaitTimeout);
        return ::pthread_cond_timedwait( &mCondVariable, &mPosixMutex, &waitTimeout );
    }
}

inline int SyncLockAndWaitPosix::_getWaitableIndex( const WaitablePosix & syncWaitable ) const
{
    int result = NECommon::INVALID_INDEX;
    for ( uint32_t i = 0; i < mWaitingList.getSize(); ++ i )
    {
        if (mWaitingList[i] == &syncWaitable)
        {
            result = i;
            break;
        }
    }

    return result;
}

int32_t SyncLockAndWaitPosix::SyncLockAndWaitPosix::_checkEventFired( WaitablePosix & syncObject )
{
    int32_t result = static_cast<int32_t>(NESyncTypesIX::SyncSignal::Invalid);

    if (syncObject.checkSignaled(mContext) && (mWaitingList.getSize() != 0) && (mFiredEntry == static_cast<int32_t>(NESyncTypesIX::SyncSignal::Invalid)))
    {
        if (mDescribe == SyncLockAndWaitPosix::WaitMode::Single)
        {
            result = static_cast<int32_t>(NESyncTypesIX::SyncSignal::First);
        }
        else if (mMatchCondition == NESyncTypesIX::WaitCondition::Any)
        {
            result = static_cast<int32_t>(_getWaitableIndex(syncObject));
        }
        else
        {
            uint32_t i = 0;
#ifdef _DEBUG
            for ( ; i < mWaitingList.getSize(); ++ i)
            {
                WaitablePosix *waitable = mWaitingList[i];
                ASSERT(waitable != nullptr);
                if (waitable->checkSignaled(mContext) == false)
                {
                    AREG_OUTPUT_DBG( "Waitable [ %s ] ID [ %p ] type [ %s ] is NOT signaled yet, interrupting checkup."
                        , waitable->getName( ).getString( )
                        , waitable
                        , NESyncTypesIX::getString( waitable->getSyncType( ) ) );
                    break;
                }
#ifdef DEBUG
                else
                {
                    AREG_OUTPUT_DBG( "Waitable [ %s ] ID [ %p ] type [ %s ] is signaled."
                        , waitable->getName( ).getString( )
                        , waitable
                        , NESyncTypesIX::getString( waitable->getSyncType( ) ) );
                }
#endif // DEBUG
            }
#else   // !_DEBUG
            for (  ; (i < mWaitingList.getSize()) && mWaitingList.valueAtPosition(i)->checkSignaled(mContext); ++ i)
                ;
#endif  // !_DEBUG

            if (i == mWaitingList.getSize())
            {
                result = static_cast<int32_t>(NESyncTypesIX::SyncSignal::All);
            }
        }
    }

    return result;
}

bool SyncLockAndWaitPosix::_requestOwnership( int32_t firedEvent )
{
    bool result = false;

    ASSERT(firedEvent >= static_cast<int32_t>(NESyncTypesIX::SyncSignal::First) && firedEvent <= static_cast<int32_t>(NESyncTypesIX::SyncSignal::All));
    if ( firedEvent != static_cast<int32_t>(NESyncTypesIX::SyncSignal::All) )
    {
        ASSERT(mWaitingList.getSize() > static_cast<uint32_t>(firedEvent));
        WaitablePosix *waitable = mWaitingList[firedEvent];
        
#ifdef DEBUG
        if (waitable == nullptr)
        {
            AREG_OUTPUT_ERR("Attention! A waitable at index [ %d ] in the list [ %p ] having size [ %d] of thread [ %p ] has address [ %p ]"
                        , firedEvent
                        , this
                        , mWaitingList.getSize()
                        , Thread::getCurrentThread()
                        , waitable);
        }

        ASSERT(waitable != nullptr);
#endif // DEBUG

        AREG_OUTPUT_DBG("Thread [ %p ] requests ownership of waitable [ %s ], thread waits no more waitables."
                    , reinterpret_cast<id_type>(mContext)
                    , NESyncTypesIX::getString(waitable->getSyncType()));

        result = waitable->notifyRequestOwnership(mContext);
    }
    else
    {
        ASSERT(mWaitingList.getSize() <= static_cast<uint32_t>(firedEvent));
        AREG_OUTPUT_DBG("Thread [ %p ] requests ownership of [ %d ] waitables.", reinterpret_cast<id_type>(mContext), mWaitingList.getSize());

        result = true;
        for (uint32_t i = 0; (i < mWaitingList.getSize()) && result; ++ i)
        {
            WaitablePosix *waitable = mWaitingList[i];
            ASSERT(waitable != nullptr);
            result = waitable->notifyRequestOwnership(mContext);

            AREG_OUTPUT_DBG("Thread [ %p ] requested ownership of waitable [ %s ] listed [ %d of %d ], the ownership is [ %s ], the current state of waitable [ %s ]"
                        , reinterpret_cast<id_type>(mContext)
                        , NESyncTypesIX::getString(waitable->getSyncType())
                        , i + 1
                        , mWaitingList.getSize()
                        , result ? "taken" : "rejected"
                        , waitable->checkSignaled(mContext) ? "signaled" : "not signaled");
        }
    }

    return result;
}

#endif // defined(_POSIX) || defined(POSIX)
