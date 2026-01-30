/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SyncLockAndWaitIX.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Lock and wait object for POSIX synchronization objects.
 *
 ************************************************************************/

#include "areg/base/private/posix/SyncLockAndWaitIX.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/IEWaitableBaseIX.hpp"
#include "areg/base/SyncObjects.hpp"
#include "areg/base/Thread.hpp"
#include <errno.h>

//////////////////////////////////////////////////////////////////////////
// SyncLockAndWaitIX class implementation
//////////////////////////////////////////////////////////////////////////

SyncLockAndWaitIX::MapWaitIDResource & SyncLockAndWaitIX::_mapWaitResourceIds( void )
{
    static SyncLockAndWaitIX::MapWaitIDResource _mapWaitIdResource;
    return _mapWaitIdResource;
}

SyncLockAndWaitIX::SyncResourceMapIX & SyncLockAndWaitIX::_mapSyncResources( void )
{
    static SyncLockAndWaitIX::SyncResourceMapIX _theSyncResourceMapIX;
    return _theSyncResourceMapIX;
}

int SyncLockAndWaitIX::waitForSingleObject( IEWaitableBaseIX & syncWait, unsigned int msTimeout /* = NECommon::WAIT_INFINITE */ )
{
    IEWaitableBaseIX * list[] = { &syncWait };
    return waitForMultipleObjects(list, 1, true, msTimeout);
}

int SyncLockAndWaitIX::waitForMultipleObjects( IEWaitableBaseIX ** listWaitables, int count, bool waitAll /* = false */, unsigned int msTimeout /* = NECommon::WAIT_INFINITE */ )
{
    int result = static_cast<int>(NESyncTypesIX::SyncObjectInvalid);
    if ( (listWaitables != nullptr) && (count > 0) )
    {
        OUTPUT_DBG("Going to wait [ %s%d ] event(s).", (waitAll && (count > 1) ? "all " : ""), count);
        SyncLockAndWaitIX lockAndWait(    listWaitables
                                        , count
                                        , waitAll ? NESyncTypesIX::eMatchCondition::MatchConditionExact : NESyncTypesIX::eMatchCondition::MatchConditionAny
                                        , msTimeout);

        if ( (lockAndWait._isEmpty() == false) && lockAndWait._lock( ) )
        {
            SyncLockAndWaitIX::MapWaitIDResource & mapReousrces { SyncLockAndWaitIX::_mapWaitResourceIds() };
            mapReousrces.registerResourceObject(reinterpret_cast<ptr_type>(lockAndWait.mContext), &lockAndWait);

            int waitResult = ENOLCK;
            bool makeLoop = true;
            while ( makeLoop && lockAndWait._noEventFired( ) )
            {
                waitResult = lockAndWait._waitCondition( );
                mapReousrces.lock();
                if ( (RETURNED_OK  != waitResult) && (lockAndWait.mFiredEntry == NESyncTypesIX::SyncObjectInvalid) )
                {
                    lockAndWait.mFiredEntry = (waitResult == ETIMEDOUT) || (waitResult == EBUSY) ? NESyncTypesIX::SyncObjectTimeout : NESyncTypesIX::SyncWaitInterrupted;
                    makeLoop = false;
                }
                mapReousrces.unlock();
            }

            mapReousrces.unregisterResourceObject(reinterpret_cast<ptr_type>(lockAndWait.mContext));

            lockAndWait._unlock( );
        }

        result = static_cast<int>(lockAndWait.mFiredEntry);
    }

    return result;
}

int SyncLockAndWaitIX::eventSignaled( IEWaitableBaseIX & syncWaitable )
{
    int result = 0;

    SyncResourceMapIX & mapResource { SyncLockAndWaitIX::_mapSyncResources() };
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &syncWaitable );
    if ( waitList != nullptr)
    {
        ASSERT( waitList->isEmpty( ) == false );
        OUTPUT_DBG("Waitable [ %s ] ID [ %p ] is signaled, there are [ %d ] locks associated with it."
                    , syncWaitable.getName().getString()
                    , &syncWaitable
                    , waitList->getSize());

        ListLockAndWait::LISTPOS end = waitList->invalidPosition();
        for ( ListLockAndWait::LISTPOS pos = waitList->firstPosition( ); pos != end; )
        {
            SyncLockAndWaitIX * lockAndWait = waitList->valueAtPosition(pos);
            ASSERT(lockAndWait != nullptr);

            if (syncWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            pos = waitList->nextPosition( pos );
            NESyncTypesIX::eSyncObjectFired fired = lockAndWait->_checkEventFired(syncWaitable);
            if ( fired >= NESyncTypesIX::SyncObject0 && fired <= NESyncTypesIX::SyncObjectAll )
            {
                if (lockAndWait->_requestOwnership(fired))
                {
                    OUTPUT_DBG(   "The waitable [ %s ] [ %p ] is fired, unlocking thread [ %p ] with fired event reason [ %d ]"
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
                    OUTPUT_WARN("The waitable [ %p ] is marked as signaled, but it rejected lock [ %p ], ignoring notifying", &syncWaitable, lockAndWait);
                }
#endif // DEBUG
            }
#ifdef DEBUG
            else if (fired > NESyncTypesIX::SyncObjectAll)
            {
                OUTPUT_ERR("Lock and Wait object detected unexpected fired event [ %d ]", static_cast<int>(fired));
            }
#endif // DEBUG

        }

        OUTPUT_DBG("Waitable [ %s ] ID [ %p ] released [ %d ] threads.", syncWaitable.getName().getString(), &syncWaitable, result);
        syncWaitable.notifyReleasedThreads(result);
    }

    mapResource.unlock( );
    return result;
}

void SyncLockAndWaitIX::eventRemove( IEWaitableBaseIX & syncWaitable )
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitIX::_mapSyncResources() };
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &syncWaitable );
    if ( waitList != nullptr )
    {
        OUTPUT_ERR("The event [ %p / %s] is cleaning resource, there is still wait list, going to notify error [ %d ] locked threads and clean resources."
                    , &syncWaitable
                    , NESyncTypesIX::getString(syncWaitable.getSyncType())
                    , waitList->getSize());
        ASSERT( waitList->isEmpty( ) == false );

        ListLockAndWait::LISTPOS end = waitList->invalidPosition();
        for ( ListLockAndWait::LISTPOS pos = waitList->firstPosition( ); pos != end ; pos = waitList->nextPosition(pos))
        {
            SyncLockAndWaitIX * lockAndWait = waitList->valueAtPosition(pos);
            ASSERT(lockAndWait != nullptr);
            if (syncWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            int index = lockAndWait->_getWaitableIndex( syncWaitable );
            lockAndWait->mFiredEntry = index != NECommon::INVALID_INDEX ? static_cast<NESyncTypesIX::eSyncObjectFired>(index + NESyncTypesIX::SyncObject0Error) : NESyncTypesIX::eSyncObjectFired::SyncWaitInterrupted;
            lockAndWait->_notifyEvent();
        }

        mapResource.unregisterResource(&syncWaitable);
    }

    mapResource.unlock( );
}

void SyncLockAndWaitIX::eventFailed( IEWaitableBaseIX & syncWaitable )
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitIX::_mapSyncResources() };
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &syncWaitable );
    if ( waitList != nullptr )
    {
        OUTPUT_WARN("The event [ %p ] failed, going to notify error [ %d ] locked threads.", &syncWaitable, waitList->getSize());
        ASSERT( waitList->isEmpty( ) == false );

        ListLockAndWait::LISTPOS end = waitList->invalidPosition();
        for ( ListLockAndWait::LISTPOS pos = waitList->firstPosition( ); pos != end; pos = waitList->nextPosition(pos))
        {
            SyncLockAndWaitIX * lockAndWait = waitList->valueAtPosition(pos);
            ASSERT(lockAndWait != nullptr);
            if (syncWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            int index = lockAndWait->_getWaitableIndex( syncWaitable );
            ASSERT(index != NECommon::INVALID_INDEX);
            lockAndWait->mFiredEntry = static_cast<NESyncTypesIX::eSyncObjectFired>(index + NESyncTypesIX::SyncObject0Error);
            lockAndWait->_notifyEvent();
        }
    }

    mapResource.unlock( );
}

bool SyncLockAndWaitIX::isWaitableRegistered( IEWaitableBaseIX & syncWaitable )
{
    bool result = false;

    SyncResourceMapIX & mapResources { SyncLockAndWaitIX::_mapSyncResources() };
    mapResources.lock();

    result = mapResources.existResource(&syncWaitable);

    mapResources.unlock();

    return result;
}

bool SyncLockAndWaitIX::notifyAsyncSignal( id_type threadId )
{
    bool result{false};

    SyncResourceMapIX & mapResource { SyncLockAndWaitIX::_mapSyncResources() };
    mapResource.lock( );
    do 
    {
        SyncLockAndWaitIX::MapWaitIDResource & mapReousrces { SyncLockAndWaitIX::_mapWaitResourceIds() };
        mapReousrces.lock();
        SyncLockAndWaitIX * lockAndWait = mapReousrces.findResourceObject(static_cast<ptr_type>(threadId));
        if (lockAndWait != nullptr)
        {
            lockAndWait->mFiredEntry = NESyncTypesIX::SyncAsyncSignal;
            result = lockAndWait->_notifyEvent();
        }

        mapReousrces.unlock();
    } while (false);

    mapResource.unlock();

    return result;
}

SyncLockAndWaitIX::SyncLockAndWaitIX(   IEWaitableBaseIX ** listWaitables
                                            , int count
                                            , NESyncTypesIX::eMatchCondition matchCondition
                                            , unsigned int msTimeout )
    : mDescribe         ( count > 1 ? SyncLockAndWaitIX::eWaitType::WaitMultipleObjects : SyncLockAndWaitIX::eWaitType::WaitSingleObject )
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
    , mFiredEntry       ( NESyncTypesIX::SyncObjectInvalid )
    , mWaitingList      ( count )
{
    ASSERT( listWaitables  != nullptr);

    if ( _initPosixSyncObjects() )
    {
        SyncResourceMapIX & mapResources { SyncLockAndWaitIX::_mapSyncResources() };
        mapResources.lock();
        count = MACRO_MIN(NECommon::MAXIMUM_WAITING_OBJECTS, count);

        if ( (mMatchCondition == NESyncTypesIX::eMatchCondition::MatchConditionAny ) || (mDescribe == SyncLockAndWaitIX::eWaitType::WaitSingleObject) )
        {
            for ( uint32_t i = 0; i < static_cast<uint32_t>(count); ++ i, ++ listWaitables )
            {
                IEWaitableBaseIX * syncWaitable = *listWaitables;
                if (syncWaitable != nullptr)
                {
                    ASSERT( (static_cast<unsigned int>(syncWaitable->getSyncType()) & static_cast<unsigned int>(NESyncTypesIX::eSyncObject::SoWaitable)) != 0);

                    mapResources.registerResourceObject(syncWaitable, this);
                    // OUTPUT_DBG("Waitable [ %p ] is [ %s ]", syncWaitable, syncWaitable->IsSignaled() ? "signaled" : "not signaled");

                    if (syncWaitable->checkSignaled(mContext) && syncWaitable->notifyRequestOwnership(mContext))
                    {
                        OUTPUT_DBG("Waitable [ %s ] with ID [ %p ] of type [ %s ] is signaled, going unlock thread [ %p ]"
                                    , syncWaitable->getName().getString()
                                    , syncWaitable
                                    , NESyncTypesIX::getString(syncWaitable->getSyncType())
                                    , reinterpret_cast<id_type>(mContext));

                        mFiredEntry = static_cast<NESyncTypesIX::eSyncObjectFired>(i);
                        syncWaitable->notifyReleasedThreads(1);
                        mWaitingList.resize(i);
                        break;
                    }

                    mWaitingList[i] = syncWaitable;
                }
                else
                {
                    mFiredEntry = static_cast<NESyncTypesIX::eSyncObjectFired>(i + NESyncTypesIX::SyncObject0Error);
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
                IEWaitableBaseIX * syncWaitable = *listWaitables;
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
                    mFiredEntry = static_cast<NESyncTypesIX::eSyncObjectFired>(i + NESyncTypesIX::SyncObject0Error);
                    mWaitingList.resize(i);
                    break;
                }
            }

            if (eventFired && _requestOwnership(NESyncTypesIX::SyncObjectAll))
            {
                OUTPUT_DBG("Releasing thread [ %p ], all events are fired.", reinterpret_cast<id_type>(mContext));

                mFiredEntry = NESyncTypesIX::SyncObjectAll;
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

SyncLockAndWaitIX::~SyncLockAndWaitIX( void )
{
	SyncLockAndWaitIX::_mapSyncResources().removeResourceObject(this, true);
}

inline bool SyncLockAndWaitIX::_noEventFired( void ) const
{
    return (mFiredEntry == NESyncTypesIX::SyncObjectInvalid);
}

inline bool SyncLockAndWaitIX::_isEmpty( void ) const
{
    return mWaitingList.isEmpty();
}

inline bool SyncLockAndWaitIX::_notifyEvent( void )
{
    bool result = false;

    if (_isValid() && _lock())
    {
        result = RETURNED_OK == pthread_cond_signal(&mCondVariable);
        _unlock();
    }

    return result;
}

inline bool SyncLockAndWaitIX::_initPosixSyncObjects( void )
{
    // Init POSIX mutex
    if (RETURNED_OK == ::pthread_mutexattr_init( &mPosixMutexAttr ))
    {
        mMutexAttrValid = true;
        if (RETURNED_OK == ::pthread_mutexattr_settype( &mPosixMutexAttr, PTHREAD_MUTEX_NORMAL ))
        {
            mMutexValid = (RETURNED_OK == ::pthread_mutex_init( &mPosixMutex, &mPosixMutexAttr ));
        }
    }

    // Init POSIX condition variable
    if (RETURNED_OK == ::pthread_condattr_init( &mCondAttribute ))
    {
        mCondAttrValid = true;
        if (RETURNED_OK == ::pthread_condattr_setpshared( &mCondAttribute, PTHREAD_PROCESS_PRIVATE ))
        {
            mCondVarValid = (RETURNED_OK == ::pthread_cond_init( &mCondVariable, &mCondAttribute ));
        }
    }

    return (mMutexValid && mCondVarValid);
}

inline void SyncLockAndWaitIX::_releasePosixSyncObjects( void )
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

inline bool SyncLockAndWaitIX::_isValid( void ) const
{
    return ((mMutexValid && mCondVarValid) && (mWaitingList.isEmpty() == false));
}

inline bool SyncLockAndWaitIX::_lock( void )
{
    return (mMutexValid && (RETURNED_OK == pthread_mutex_lock(&mPosixMutex)));
}

inline void SyncLockAndWaitIX::_unlock( void )
{
    if (mMutexValid)
    {
        ::pthread_mutex_unlock( &mPosixMutex );
    }
}

inline int SyncLockAndWaitIX::_waitCondition( void )
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

inline int SyncLockAndWaitIX::_getWaitableIndex( const IEWaitableBaseIX & syncWaitable ) const
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

NESyncTypesIX::eSyncObjectFired SyncLockAndWaitIX::SyncLockAndWaitIX::_checkEventFired( IEWaitableBaseIX & syncObject )
{
    NESyncTypesIX::eSyncObjectFired result = NESyncTypesIX::SyncObjectInvalid;

    if (syncObject.checkSignaled(mContext) && (mWaitingList.getSize() != 0) && (mFiredEntry == NESyncTypesIX::SyncObjectInvalid))
    {
        if (mDescribe == SyncLockAndWaitIX::eWaitType::WaitSingleObject)
        {
            result = NESyncTypesIX::SyncObject0;
        }
        else if (mMatchCondition == NESyncTypesIX::eMatchCondition::MatchConditionAny)
        {
            result = static_cast<NESyncTypesIX::eSyncObjectFired>(_getWaitableIndex(syncObject));
        }
        else
        {
            uint32_t i = 0;
#ifdef _DEBUG
            for ( ; i < mWaitingList.getSize(); ++ i)
            {
                IEWaitableBaseIX *waitable = mWaitingList[i];
                ASSERT(waitable != nullptr);
                if (waitable->checkSignaled(mContext) == false)
                {
                    OUTPUT_DBG( "Waitable [ %s ] ID [ %p ] type [ %s ] is NOT signaled yet, interrupting checkup."
                        , waitable->getName( ).getString( )
                        , waitable
                        , NESyncTypesIX::getString( waitable->getSyncType( ) ) );
                    break;
                }
#ifdef DEBUG
                else
                {
                    OUTPUT_DBG( "Waitable [ %s ] ID [ %p ] type [ %s ] is signaled."
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
                result = NESyncTypesIX::SyncObjectAll;
        }
    }

    return result;
}

bool SyncLockAndWaitIX::_requestOwnership( const NESyncTypesIX::eSyncObjectFired firedEvent )
{
    bool result = false;

    ASSERT(firedEvent >= NESyncTypesIX::SyncObject0 && firedEvent <= NESyncTypesIX::SyncObjectAll);
    if ( firedEvent != NESyncTypesIX::SyncObjectAll )
    {
        ASSERT(mWaitingList.getSize() > static_cast<uint32_t>(firedEvent));
        IEWaitableBaseIX *waitable = mWaitingList[static_cast<uint32_t>(firedEvent)];
        
#ifdef DEBUG
        if (waitable == nullptr)
        {
            OUTPUT_ERR("Attention! A waitable at index [ %d ] in the list [ %p ] having size [ %d] of thread [ %p ] has address [ %p ]"
                        , static_cast<int>(firedEvent)
                        , this
                        , mWaitingList.getSize()
                        , Thread::getCurrentThread()
                        , waitable);
        }

        ASSERT(waitable != nullptr);
#endif // DEBUG

        OUTPUT_DBG("Thread [ %p ] requests ownership of waitable [ %s ], thread waits no more waitables."
                    , reinterpret_cast<id_type>(mContext)
                    , NESyncTypesIX::getString(waitable->getSyncType()));

        result = waitable->notifyRequestOwnership(mContext);
    }
    else
    {
        ASSERT(mWaitingList.getSize() <= static_cast<uint32_t>(firedEvent));
        OUTPUT_DBG("Thread [ %p ] requests ownership of [ %d ] waitables.", reinterpret_cast<id_type>(mContext), mWaitingList.getSize());

        result = true;
        for (uint32_t i = 0; (i < mWaitingList.getSize()) && result; ++ i)
        {
            IEWaitableBaseIX *waitable = mWaitingList[i];
            ASSERT(waitable != nullptr);
            result = waitable->notifyRequestOwnership(mContext);

            OUTPUT_DBG("Thread [ %p ] requested ownership of waitable [ %s ] listed [ %d of %d ], the ownership is [ %s ], the current state of waitable [ %s ]"
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
