/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SynchLockAndWaitIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Lock and wait object for POSIX synchronization objects.
 *
 ************************************************************************/

#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/IEWaitableBaseIX.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/Thread.hpp"
#include <errno.h>

//////////////////////////////////////////////////////////////////////////
// SynchWaitableMapIX class implementation
//////////////////////////////////////////////////////////////////////////

SynchLockAndWaitIX::SynchResourceMapIX  SynchLockAndWaitIX::_theSynchResourceMapIX;

//////////////////////////////////////////////////////////////////////////
// SynchLockAndWaitIX class implementation
//////////////////////////////////////////////////////////////////////////

SynchLockAndWaitIX::MapWaitIDResource   SynchLockAndWaitIX::_mapWaitIdResource;


int SynchLockAndWaitIX::waitForSingleObject( IEWaitableBaseIX & synchWait, unsigned int msTimeout /* = NECommon::WAIT_INFINITE */ )
{
    IEWaitableBaseIX * list[] = { &synchWait };
    return waitForMultipleObjects(list, 1, true, msTimeout);
}

int SynchLockAndWaitIX::waitForMultipleObjects( IEWaitableBaseIX ** listWaitables, int count, bool waitAll /* = false */, unsigned int msTimeout /* = NECommon::WAIT_INFINITE */ )
{
    int result = static_cast<int>(NESynchTypesIX::SynchObjectInvalid);
    if ( (listWaitables != nullptr) && (count > 0) )
    {
        OUTPUT_DBG("Going to wait [ %s%d ] event(s).", (waitAll && (count > 1) ? "all " : ""), count);
        SynchLockAndWaitIX lockAndWait(   listWaitables
                                        , count
                                        , waitAll ? NESynchTypesIX::eMatchCondition::MatchConditionExact : NESynchTypesIX::eMatchCondition::MatchConditionAny
                                        , msTimeout);

        if ( (lockAndWait._isEmpty() == false) && lockAndWait._lock( ) )
        {
            _mapWaitIdResource.registerResourceObject(reinterpret_cast<id_type>(lockAndWait.mContext), &lockAndWait);

            int waitResult = ENOLCK;
            bool makeLoop = true;
            while ( makeLoop && lockAndWait._noEventFired( ) )
            {
                waitResult = lockAndWait._waitCondition( );
                _mapWaitIdResource.lock();
                if ( (RETURNED_OK  != waitResult) && (lockAndWait.mFiredEntry == NESynchTypesIX::SynchObjectInvalid) )
                {
                    lockAndWait.mFiredEntry = (waitResult == ETIMEDOUT) || (waitResult == EBUSY) ? NESynchTypesIX::SynchObjectTimeout : NESynchTypesIX::SynchWaitInterrupted;
                    makeLoop = false;
                }
                _mapWaitIdResource.unlock();
            }

            _mapWaitIdResource.unregisterResourceObject(reinterpret_cast<id_type>(lockAndWait.mContext));

            lockAndWait._unlock( );
        }

        result = static_cast<int>(lockAndWait.mFiredEntry);
    }

    return result;
}

int SynchLockAndWaitIX::eventSignaled( IEWaitableBaseIX & synchWaitable )
{
    int result = 0;

    SynchResourceMapIX & mapResource = SynchLockAndWaitIX::_theSynchResourceMapIX;
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &synchWaitable );
    if ( waitList != nullptr)
    {
        ASSERT( waitList->isEmpty( ) == false );
        OUTPUT_DBG("Waitable [ %s ] ID [ %p ] is signaled, there are [ %d ] locks accosiated with it."
                    , synchWaitable.getName().getString()
                    , &synchWaitable
                    , waitList->getSize());

        ListLockAndWait::LISTPOS end = waitList->invalidPosition();
        for ( ListLockAndWait::LISTPOS pos = waitList->firstPosition( ); pos != end; )
        {
            SynchLockAndWaitIX * lockAndWait = waitList->valueAtPosition(pos);
            ASSERT(lockAndWait != nullptr);

            if (synchWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            pos = waitList->nextPosition( pos );
            NESynchTypesIX::eSynchObjectFired fired = lockAndWait->_checkEventFired(synchWaitable);
            if ( fired >= NESynchTypesIX::SynchObject0 && fired <= NESynchTypesIX::SynchObjectAll )
            {
                if (lockAndWait->_requestOwnership(fired))
                {
                    OUTPUT_DBG(   "The waitable [ %s ] [ %p ] is fired, unlocking thread [ %p ] with fired event reason [ %d ]"
                                , synchWaitable.getName().getString()
                                , &synchWaitable
                                , lockAndWait->mContext
                                , static_cast<int>(fired));

                    ++ result;
                    lockAndWait->mFiredEntry = fired;
                    lockAndWait->_notifyEvent();
                }
#ifdef  DEBUG
                else
                {
                    OUTPUT_WARN("The waitable [ %p ] is marked as signaled, but it rejected lock [ %p ], ignoring notifying", &synchWaitable, lockAndWait);
                }
#endif // DEBUG
            }
#ifdef DEBUG
            else if (fired > NESynchTypesIX::SynchObjectAll)
            {
                OUTPUT_ERR("Lock and Wait object detected unexpected fired event [ %d ]", static_cast<int>(fired));
            }
#endif // DEBUG

        }

        OUTPUT_DBG("Waitable [ %s ] ID [ %p ] released [ %d ] threads.", synchWaitable.getName().getString(), &synchWaitable, result);
        synchWaitable.notifyReleasedThreads(result);
    }

    mapResource.unlock( );
    return result;
}

void SynchLockAndWaitIX::eventRemove( IEWaitableBaseIX & synchWaitable )
{
    SynchResourceMapIX & mapResource = SynchLockAndWaitIX::_theSynchResourceMapIX;
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &synchWaitable );
    if ( waitList != nullptr )
    {
        OUTPUT_ERR("The event [ %p / %s] is cleaning resource, there is still wait list, going to notify error [ %d ] locked threads and clean resources."
                    , &synchWaitable
                    , NESynchTypesIX::getString(synchWaitable.getSynchType())
                    , waitList->getSize());
        ASSERT( waitList->isEmpty( ) == false );

        ListLockAndWait::LISTPOS end = waitList->invalidPosition();
        for ( ListLockAndWait::LISTPOS pos = waitList->firstPosition( ); pos != end ; pos = waitList->nextPosition(pos))
        {
            SynchLockAndWaitIX * lockAndWait = waitList->valueAtPosition(pos);
            ASSERT(lockAndWait != nullptr);
            if (synchWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            int index = lockAndWait->_getWaitableIndex( synchWaitable );
            lockAndWait->mFiredEntry = index != NECommon::INVALID_INDEX ? static_cast<NESynchTypesIX::eSynchObjectFired>(index + NESynchTypesIX::SynchObject0Error) : NESynchTypesIX::eSynchObjectFired::SynchWaitInterrupted;
            lockAndWait->_notifyEvent();
        }

        mapResource.unregisterResource(&synchWaitable);
    }

    mapResource.unlock( );
}

void SynchLockAndWaitIX::eventFailed( IEWaitableBaseIX & synchWaitable )
{
    SynchResourceMapIX & mapResource = SynchLockAndWaitIX::_theSynchResourceMapIX;
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &synchWaitable );
    if ( waitList != nullptr )
    {
        OUTPUT_WARN("The event [ %p ] failed, going to notify error [ %d ] locked threads.", &synchWaitable, waitList->getSize());
        ASSERT( waitList->isEmpty( ) == false );

        ListLockAndWait::LISTPOS end = waitList->invalidPosition();
        for ( ListLockAndWait::LISTPOS pos = waitList->firstPosition( ); pos != end; pos = waitList->nextPosition(pos))
        {
            SynchLockAndWaitIX * lockAndWait = waitList->valueAtPosition(pos);
            ASSERT(lockAndWait != nullptr);
            if (synchWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            int index = lockAndWait->_getWaitableIndex( synchWaitable );
            ASSERT(index != NECommon::INVALID_INDEX);
            lockAndWait->mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(index + NESynchTypesIX::SynchObject0Error);
            lockAndWait->_notifyEvent();
        }
    }

    mapResource.unlock( );
}

bool SynchLockAndWaitIX::isWaitableRegistered( IEWaitableBaseIX & synchWaitable )
{
    bool result = false;

    SynchResourceMapIX & mapResources = SynchLockAndWaitIX::_theSynchResourceMapIX;
    mapResources.lock();

    result = mapResources.existResource(&synchWaitable);

    mapResources.unlock();

    return result;
}

bool SynchLockAndWaitIX::notifyAsynchSignal( id_type threadId )
{
    bool result = false;

    SynchResourceMapIX & mapResource = SynchLockAndWaitIX::_theSynchResourceMapIX;
    mapResource.lock( );
    do 
    {
        SynchLockAndWaitIX::_mapWaitIdResource.lock();
        SynchLockAndWaitIX * lockAndWait = SynchLockAndWaitIX::_mapWaitIdResource.findResourceObject(threadId);
        if (lockAndWait != nullptr)
        {
            lockAndWait->mFiredEntry = NESynchTypesIX::SynchAsynchSignal;
            result = lockAndWait->_notifyEvent();
        }

        SynchLockAndWaitIX::_mapWaitIdResource.unlock();

    } while (false);

    mapResource.unlock();

    return result;
}

SynchLockAndWaitIX::SynchLockAndWaitIX(   IEWaitableBaseIX ** listWaitables
                                            , int count
                                            , NESynchTypesIX::eMatchCondition matchCondition
                                            , unsigned int msTimeout )
    : mDescribe         ( count > 1 ? SynchLockAndWaitIX::eWaitType::WaitMultipleObjects : SynchLockAndWaitIX::eWaitType::WaitSingleObject )
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
    , mFiredEntry       ( NESynchTypesIX::SynchObjectInvalid )
    , mWaitingList      ( count )
{
    ASSERT( listWaitables  != nullptr);

    if ( _initPosixSynchObjects() )
    {
        SynchResourceMapIX & mapResources = SynchLockAndWaitIX::_theSynchResourceMapIX;
        mapResources.lock();
        count = MACRO_MIN(NECommon::MAXIMUM_WAITING_OBJECTS, count);

        if ( (mMatchCondition == NESynchTypesIX::eMatchCondition::MatchConditionAny ) || (mDescribe == SynchLockAndWaitIX::eWaitType::WaitSingleObject) )
        {
            for ( int i = 0; i < count; ++ i, ++ listWaitables )
            {
                IEWaitableBaseIX * synchWaitable = *listWaitables;
                if (synchWaitable != nullptr)
                {
                    ASSERT( (static_cast<unsigned int>(synchWaitable->getSynchType()) & static_cast<unsigned int>(NESynchTypesIX::eSynchObject::SoWaitable)) != 0);

                    mapResources.registerResourceObject(synchWaitable, this);
                    // OUTPUT_DBG("Waitable [ %p ] is [ %s ]", synchWaitable, synchWaitable->IsSignaled() ? "signaled" : "not signaled");

                    if (synchWaitable->checkSignaled(mContext) && synchWaitable->notifyRequestOwnership(mContext))
                    {
                        OUTPUT_DBG("Waitable [ %s ] with ID [ %p ] of type [ %s ] is signaled, going unlock thread [ %p ]"
                                    , synchWaitable->getName().getString()
                                    , synchWaitable
                                    , NESynchTypesIX::getString(synchWaitable->getSynchType())
                                    , reinterpret_cast<id_type>(mContext));

                        mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(i);
                        synchWaitable->notifyReleasedThreads(1);
                        mWaitingList.resize(i);
                        break;
                    }

                    mWaitingList[i] = synchWaitable;
                }
                else
                {
                    mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(i + NESynchTypesIX::SynchObject0Error);
                    mWaitingList.resize(i);
                    break;
                }
            }
        }
        else
        {
            bool eventFired = true;

            for ( int i = 0; i < count; ++ i, ++ listWaitables )
            {
                IEWaitableBaseIX * synchWaitable = *listWaitables;
                if ( synchWaitable != nullptr )
                {
                    mapResources.registerResourceObject(synchWaitable, this);
                    if ( (eventFired) && (synchWaitable->checkSignaled(mContext) == false) )
                    {
                        eventFired = false;
                    }

                    mWaitingList[i] = synchWaitable;
                }
                else
                {
                    mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(i + NESynchTypesIX::SynchObject0Error);
                    mWaitingList.resize(i);
                    break;
                }
            }

            if (eventFired && _requestOwnership(NESynchTypesIX::SynchObjectAll))
            {
                OUTPUT_DBG("Releasing thread [ %p ], all events are fired.", reinterpret_cast<id_type>(mContext));

                mFiredEntry = NESynchTypesIX::SynchObjectAll;
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
        _releasePosixSynchObjects();
    }
}

SynchLockAndWaitIX::~SynchLockAndWaitIX( void )
{
	SynchLockAndWaitIX::_theSynchResourceMapIX.removeResourceObject(this, true);
}

inline bool SynchLockAndWaitIX::_noEventFired( void ) const
{
    return (mFiredEntry == NESynchTypesIX::SynchObjectInvalid);
}

inline bool SynchLockAndWaitIX::_isEmpty( void ) const
{
    return mWaitingList.isEmpty();
}

inline bool SynchLockAndWaitIX::_notifyEvent( void )
{
    bool result = false;

    if (_isValid() && _lock())
    {
        result = RETURNED_OK == pthread_cond_signal(&mCondVariable);
        _unlock();
    }

    return result;
}

inline bool SynchLockAndWaitIX::_initPosixSynchObjects( void )
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

inline void SynchLockAndWaitIX::_releasePosixSynchObjects( void )
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

inline bool SynchLockAndWaitIX::_isValid( void ) const
{
    return ((mMutexValid && mCondVarValid) && (mWaitingList.isEmpty() == false));
}

inline bool SynchLockAndWaitIX::_lock( void )
{
    return (mMutexValid && (RETURNED_OK == pthread_mutex_lock(&mPosixMutex)));
}

inline void SynchLockAndWaitIX::_unlock( void )
{
    if (mMutexValid)
    {
        ::pthread_mutex_unlock( &mPosixMutex );
    }
}

inline int SynchLockAndWaitIX::_waitCondition( void )
{
    if ( mWaitTimeout == NECommon::WAIT_INFINITE)
    {
        return ::pthread_cond_wait(&mCondVariable, &mPosixMutex);
    }
    else
    {
        timespec waitTimeout;
        NESynchTypesIX::timeoutFromNow(waitTimeout, mWaitTimeout);
        return ::pthread_cond_timedwait( &mCondVariable, &mPosixMutex, &waitTimeout );
    }
}

inline int SynchLockAndWaitIX::_getWaitableIndex( const IEWaitableBaseIX & synchWaitable ) const
{
    int result = NECommon::INVALID_INDEX;
    for ( uint32_t i = 0; i < mWaitingList.getSize(); ++ i )
    {
        if (mWaitingList[i] == &synchWaitable)
        {
            result = i;
            break;
        }
    }

    return result;
}

NESynchTypesIX::eSynchObjectFired SynchLockAndWaitIX::SynchLockAndWaitIX::_checkEventFired( IEWaitableBaseIX & synchObject )
{
    NESynchTypesIX::eSynchObjectFired result = NESynchTypesIX::SynchObjectInvalid;

    if (synchObject.checkSignaled(mContext) && (mWaitingList.getSize() != 0) && (mFiredEntry == NESynchTypesIX::SynchObjectInvalid))
    {
        if (mDescribe == SynchLockAndWaitIX::eWaitType::WaitSingleObject)
        {
            result = NESynchTypesIX::SynchObject0;
        }
        else if (mMatchCondition == NESynchTypesIX::eMatchCondition::MatchConditionAny)
        {
            result = static_cast<NESynchTypesIX::eSynchObjectFired>(_getWaitableIndex(synchObject));
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
                        , NESynchTypesIX::getString( waitable->getSynchType( ) ) );
                    break;
                }
#ifdef DEBUG
                else
                {
                    OUTPUT_DBG( "Waitable [ %s ] ID [ %p ] type [ %s ] is signaled."
                        , waitable->getName( ).getString( )
                        , waitable
                        , NESynchTypesIX::getString( waitable->getSynchType( ) ) );
                }
#endif // DEBUG
            }
#else   // !_DEBUG
            for (  ; (i < mWaitingList.getSize()) && mWaitingList.valueAtPosition(i)->checkSignaled(mContext); ++ i)
                ;
#endif  // !_DEBUG

            if (i == mWaitingList.getSize())
                result = NESynchTypesIX::SynchObjectAll;
        }
    }

    return result;
}

bool SynchLockAndWaitIX::_requestOwnership( const NESynchTypesIX::eSynchObjectFired firedEvent )
{
    bool result = false;

    ASSERT(firedEvent >= NESynchTypesIX::SynchObject0 && firedEvent <= NESynchTypesIX::SynchObjectAll);
    if ( firedEvent != NESynchTypesIX::SynchObjectAll )
    {
        ASSERT(mWaitingList.getSize() > static_cast<uint32_t>(firedEvent));
        IEWaitableBaseIX *waitable = mWaitingList[static_cast<int>(firedEvent)];
        
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
                    , NESynchTypesIX::getString(waitable->getSynchType()));

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
                        , NESynchTypesIX::getString(waitable->getSynchType())
                        , i + 1
                        , mWaitingList.getSize()
                        , result ? "taken" : "rejected"
                        , waitable->checkSignaled(mContext) ? "signaled" : "not signaled");
        }
    }

    return result;
}

#endif // defined(_POSIX) || defined(POSIX)
