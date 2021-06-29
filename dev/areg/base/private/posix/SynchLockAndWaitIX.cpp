/************************************************************************
 * \file        areg/base/private/posix/SynchLockAndWaitIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Lock and wait object for POSIX synchronization objects.
 *
 ************************************************************************/

#include "areg/base/private/posix/SynchLockAndWaitIX.hpp"

#ifdef _POSIX

#include "areg/base/private/posix/IEWaitableBaseIX.hpp"
#include "areg/base/ESynchObjects.hpp"
#include "areg/base/Thread.hpp"
#include <errno.h>

//////////////////////////////////////////////////////////////////////////
// SynchWaitableMapIX class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SynchResourceMapIX class implementation
//////////////////////////////////////////////////////////////////////////

SynchResourceMapIX::SynchResourceMapIX( void )
    : TELockResourceListMap<IEWaitableBaseIX *, SynchLockAndWaitIX, MapLockAndWait, ListLockAndWait, ResourceListMapImpl>( )
{
    ; // do nothing
}

SynchResourceMapIX::~SynchResourceMapIX( void )
{
    ; // do nothing
}

SynchResourceMapIX & SynchResourceMapIX::getInstance( void )
{
    static SynchResourceMapIX _mapSynchResources;
    return _mapSynchResources;
}

//////////////////////////////////////////////////////////////////////////
// SynchLockAndWaitIX class implementation
//////////////////////////////////////////////////////////////////////////

MapWaitIDResource   SynchLockAndWaitIX::_mapWaitIdResource;


int SynchLockAndWaitIX::waitForSingleObject( IEWaitableBaseIX & synchWait, unsigned int msTimeout /* = IESynchObject::WAIT_INFINITE */ )
{
    IEWaitableBaseIX * list[] = { &synchWait };
    return waitForMultipleObjects(list, 1, true, msTimeout);
}

int SynchLockAndWaitIX::waitForMultipleObjects( IEWaitableBaseIX ** listWaitables, int count, bool waitAll /* = false */, unsigned int msTimeout /* = IEBlockingSynchObject::WAIT_INFINITE */ )
{
    int result = static_cast<int>(NESynchTypesIX::SynchObjectInvalid);
    if ( (listWaitables != NULL) && (count > 0) )
    {
        OUTPUT_DBG("Going to wait [ %s%d ] event(s).", (waitAll && (count > 1) ? "all " : ""), count);
        SynchLockAndWaitIX lockAndWait(   listWaitables
                                          , count
                                          , waitAll ? NESynchTypesIX::MatchConditionExact : NESynchTypesIX::MatchConditionAny
                                          , msTimeout);

        if ( (lockAndWait._isEmpty() == false) && lockAndWait._lock( ) )
        {
            _mapWaitIdResource.registerResourceObject(reinterpret_cast<ITEM_ID>(lockAndWait.mContext), &lockAndWait);

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

            _mapWaitIdResource.unregisterResourceObject(reinterpret_cast<ITEM_ID>(lockAndWait.mContext));

            lockAndWait._unlock( );
        }

        result = static_cast<int>(lockAndWait.mFiredEntry);
    }

    return result;
}

int SynchLockAndWaitIX::eventSignaled( IEWaitableBaseIX & synchWaitable )
{
    int result = 0;

    SynchResourceMapIX & mapResource = SynchResourceMapIX::getInstance( );
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &synchWaitable );
    if ( waitList != NULL)
    {
        ASSERT( waitList->isEmpty( ) == false );
        OUTPUT_DBG("Waitable [ %s ] ID [ %p ] is signaled, there are [ %d ] locks accosiated with it."
                    , synchWaitable.getName()
                    , &synchWaitable
                    , waitList->getSize());

        for ( LISTPOS pos = waitList->firstPosition( ); pos != NULL; )
        {
            SynchLockAndWaitIX * lockAndWait = waitList->getAt(pos);
            ASSERT(lockAndWait != NULL);

            if (synchWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            pos = waitList->nextPosition( pos );
            NESynchTypesIX::eSynchObjectFired fired = lockAndWait->_checkEventFired(synchWaitable);
            if ( fired >= NESynchTypesIX::SynchObject0 && fired <= NESynchTypesIX::SynchObjectAll )
            {
                if (lockAndWait->_requestOwnership(fired))
                {
                    OUTPUT_DBG(   "The waitable [ %s ] [ %p ] is fired, unlocking thread [ %p ] with fired event reason [ %d ]"
                                , synchWaitable.getName()
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

        OUTPUT_DBG("Waitable [ %s ] ID [ %p ] released [ %d ] threads.", synchWaitable.getName(), &synchWaitable, result);
        synchWaitable.notifyReleasedThreads(result);
    }

    mapResource.unlock( );
    return result;
}

void SynchLockAndWaitIX::eventRemove( IEWaitableBaseIX & synchWaitable )
{
    SynchResourceMapIX & mapResource = SynchResourceMapIX::getInstance( );
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &synchWaitable );
    if ( waitList != NULL )
    {
        OUTPUT_ERR("The event [ %p / %s] is cleaning resource, there is still wait list, going to notify error [ %d ] locked threads and clean resources."
                    , &synchWaitable
                    , NESynchTypesIX::getString(synchWaitable.getSynchType())
                    , waitList->getSize());
        ASSERT( waitList->isEmpty( ) == false );

        for ( LISTPOS pos = waitList->firstPosition( ); pos != NULL ; pos = waitList->nextPosition(pos))
        {
            SynchLockAndWaitIX * lockAndWait = waitList->getAt(pos);
            ASSERT(lockAndWait != NULL);
            if (synchWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            int index = lockAndWait->_getWaitableIndex( synchWaitable );
            ASSERT(index != NECommon::InvalidIndex);
            lockAndWait->mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(index + NESynchTypesIX::SynchObject0Error);
            lockAndWait->_notifyEvent();
        }

        mapResource.unregisterResource(&synchWaitable);
    }

    mapResource.unlock( );
}

void SynchLockAndWaitIX::eventFailed( IEWaitableBaseIX & synchWaitable )
{
    SynchResourceMapIX & mapResource = SynchResourceMapIX::getInstance( );
    mapResource.lock( );

    ListLockAndWait * waitList = mapResource.getResource( &synchWaitable );
    if ( waitList != NULL )
    {
        OUTPUT_WARN("The event [ %p ] failed, going to notify error [ %d ] locked threads.", &synchWaitable, waitList->getSize());
        ASSERT( waitList->isEmpty( ) == false );

        for ( LISTPOS pos = waitList->firstPosition( ); pos != NULL; pos = waitList->nextPosition(pos))
        {
            SynchLockAndWaitIX * lockAndWait = waitList->getAt(pos);
            ASSERT(lockAndWait != NULL);
            if (synchWaitable.checkSignaled(lockAndWait->mContext) == false)
                break;

            int index = lockAndWait->_getWaitableIndex( synchWaitable );
            ASSERT(index != NECommon::InvalidIndex);
            lockAndWait->mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(index + NESynchTypesIX::SynchObject0Error);
            lockAndWait->_notifyEvent();
        }
    }

    mapResource.unlock( );
}

bool SynchLockAndWaitIX::isWaitableRegistered( IEWaitableBaseIX & synchWaitable )
{
    bool result = false;

    SynchResourceMapIX & mapResources = SynchResourceMapIX::getInstance();
    mapResources.lock();

    result = mapResources.existResource(&synchWaitable);

    mapResources.unlock();

    return result;
}

bool SynchLockAndWaitIX::notifyAsynchSignal( ITEM_ID threadId )
{
    bool result = false;

    SynchResourceMapIX & mapResource = SynchResourceMapIX::getInstance( );
    mapResource.lock( );
    do 
    {
        SynchLockAndWaitIX::_mapWaitIdResource.lock();
        SynchLockAndWaitIX * lockAndWait = SynchLockAndWaitIX::_mapWaitIdResource.findResourceObject(threadId);
        if (lockAndWait != NULL)
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
    : mDescribe         ( count > 1 ? WaitMultipleObjects : WaitSingleObject )
    , mMatchCondition   ( matchCondition )
    , mWaitTimeout      ( msTimeout )
    , mContext          ( pthread_self() )
    , mPosixMutex       ( NULL )
    , mPosixMutexAttr   ( NULL )
    , mCondVariable     ( NULL )
    , mCondAttribute    ( NULL )
    , mFiredEntry       ( NESynchTypesIX::SynchObjectInvalid )
    , mWaitingList      ( count )
{
    ASSERT( listWaitables  != NULL);

    if ( _initPosixSynchObjects() )
    {
        SynchResourceMapIX & mapResources = SynchResourceMapIX::getInstance();
        mapResources.lock();
        count = MACRO_MIN(NECommon::MAXIMUM_WAITING_OBJECTS, count);

        if ( (mMatchCondition == NESynchTypesIX::MatchConditionAny ) || (mDescribe == WaitSingleObject) )
        {
            for ( int i = 0; i < count; ++ i, ++ listWaitables )
            {
                IEWaitableBaseIX * synchWaitable = *listWaitables;
                if (synchWaitable != NULL)
                {
                    ASSERT( (static_cast<unsigned int>(synchWaitable->getSynchType()) & static_cast<unsigned int>(NESynchTypesIX::SoWaitable)) != 0);

                    mapResources.registerResourceObject(synchWaitable, this);
                    // OUTPUT_DBG("Waitable [ %p ] is [ %s ]", synchWaitable, synchWaitable->IsSignaled() ? "signaled" : "not signaled");

                    if (synchWaitable->checkSignaled(mContext) && synchWaitable->notifyRequestOwnership(mContext))
                    {
                        OUTPUT_DBG("Waitable [ %s ] with ID [ %p ] of type [ %s ] is signaled, going unlock thread [ %p ]"
                                    , synchWaitable->getName()
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
                if ( synchWaitable != NULL )
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
                for (int i = 0; i < mWaitingList.getSize(); ++ i)
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
    SynchResourceMapIX::getInstance().removeResourceObject(this, true);
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
    bool mutexReady = false;
    if (RETURNED_OK == pthread_mutexattr_init( &mPosixMutexAttr ))
    {
        if (RETURNED_OK == pthread_mutexattr_settype( &mPosixMutexAttr, PTHREAD_MUTEX_NORMAL ))
        {
            mutexReady = (RETURNED_OK == pthread_mutex_init( &mPosixMutex, &mPosixMutexAttr ));
        }
    }

    // Init POSIX condition variable
    bool condVarReady= false;
    if (RETURNED_OK == pthread_condattr_init( &mCondAttribute ))
    {
        if (RETURNED_OK == pthread_condattr_setpshared( &mCondAttribute, PTHREAD_PROCESS_PRIVATE ))
        {
            condVarReady = (RETURNED_OK == pthread_cond_init( &mCondVariable, &mCondAttribute ));
        }
    }

    return (mutexReady && condVarReady);
}

inline void SynchLockAndWaitIX::_releasePosixSynchObjects( void )
{
    if (mPosixMutex != NULL)
        pthread_mutex_destroy(&mPosixMutex);

    if (mPosixMutexAttr != NULL)
        pthread_mutexattr_destroy(&mPosixMutexAttr);

    if (mCondVariable != NULL)
        pthread_cond_destroy(&mCondVariable);

    if (mCondAttribute != NULL)
        pthread_condattr_destroy(&mCondAttribute);

    mPosixMutex     = NULL;
    mPosixMutexAttr = NULL;
    mCondVariable   = NULL;
    mCondAttribute  = NULL;
}

inline bool SynchLockAndWaitIX::_isValid( void ) const
{
    return ((mPosixMutex != NULL) && (mCondVariable != NULL) && (mWaitingList.isEmpty() == false));
}

inline bool SynchLockAndWaitIX::_lock( void )
{
    return (mPosixMutex != NULL ? RETURNED_OK == pthread_mutex_lock(&mPosixMutex) : false);
}

inline void SynchLockAndWaitIX::_unlock( void )
{
    if (mPosixMutex != NULL)
        pthread_mutex_unlock( &mPosixMutex );
}

inline int SynchLockAndWaitIX::_waitCondition( void )
{
    if ( mWaitTimeout == IESynchObject::WAIT_INFINITE)
    {
        return pthread_cond_wait(&mCondVariable, &mPosixMutex);
    }
    else
    {
        timespec waitTimeout;
        NESynchTypesIX::timeoutFromNow(waitTimeout, mWaitTimeout);
        return pthread_cond_timedwait( &mCondVariable, &mPosixMutex, &waitTimeout );
    }
}

inline int SynchLockAndWaitIX::_getWaitableIndex( const IEWaitableBaseIX & synchWaitable ) const
{
    int result = NECommon::InvalidIndex;
    for ( int i = 0; i < mWaitingList.getSize(); ++ i )
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
        if (mDescribe == SynchLockAndWaitIX::WaitSingleObject)
        {
            result = NESynchTypesIX::SynchObject0;
        }
        else if (mMatchCondition == NESynchTypesIX::MatchConditionAny)
        {
            result = static_cast<NESynchTypesIX::eSynchObjectFired>(_getWaitableIndex(synchObject));
        }
        else
        {
            int i = 0;
#ifdef _DEBUG
            for ( ; i < mWaitingList.getSize(); ++ i)
            {
                IEWaitableBaseIX *waitable = mWaitingList[i];
                ASSERT(waitable != NULL);
                if (waitable->checkSignaled(mContext))
                {
                    OUTPUT_DBG("Waitable [ %s ] ID [ %p ] type [ %s ] is signaled."
                                , waitable->getName()
                                , waitable
                                , NESynchTypesIX::getString(waitable->getSynchType()));
                }
                else
                {
                    OUTPUT_DBG("Waitable [ %s ] ID [ %p ] type [ %s ] is NOT signaled yet, interrupting checkup."
                                , waitable->getName()
                                , waitable
                                , NESynchTypesIX::getString(waitable->getSynchType()));
                    break;
                }
            }
#else   // !_DEBUG
            for (  ; (i < mWaitingList.getSize()) && mWaitingList.getAt(i)->IsSignaled(); ++ i)
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
        ASSERT(mWaitingList.getSize() > static_cast<int>(firedEvent));
        IEWaitableBaseIX *waitable = mWaitingList[static_cast<int>(firedEvent)];
        
#ifdef DEBUG
        if (waitable == NULL)
        {
            OUTPUT_ERR("Atention! A waitable at index [ %d ] in the list [ %p ] having size [ %d] of thread [ %p ] has address [ %p ]"
                        , static_cast<int>(firedEvent)
                        , this
                        , mWaitingList.getSize()
                        , Thread::getCurrentThread()
                        , waitable);
        }

        ASSERT(waitable != NULL);
#endif // DEBUG

        OUTPUT_DBG("Thread [ %p ] requests ownership of waitable [ %s ], thread waits no more waitables."
                    , reinterpret_cast<id_type>(mContext)
                    , NESynchTypesIX::getString(waitable->getSynchType()));

        result = waitable->notifyRequestOwnership(mContext);
    }
    else
    {
        ASSERT(mWaitingList.getSize() <= static_cast<int>(firedEvent));
        OUTPUT_DBG("Thread [ %p ] requests ownership of [ %d ] waitables.", reinterpret_cast<id_type>(mContext), mWaitingList.getSize());

        result = true;
        for (int i = 0; (i < mWaitingList.getSize()) && result; ++ i)
        {
            IEWaitableBaseIX *waitable = mWaitingList[i];
            ASSERT(waitable != NULL);
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

bool SynchLockAndWaitIX::_notifyAsynchSignal( void )
{
    SynchResourceMapIX & mapResource = SynchResourceMapIX::getInstance( );
    mapResource.lock( );

    mFiredEntry = NESynchTypesIX::SynchAsynchSignal;
    bool result = _notifyEvent();

    mapResource.unlock( );

    return result;
}

#endif // !_POSIX
