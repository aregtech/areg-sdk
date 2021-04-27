/************************************************************************
 * \file        areg/base/private/posix/CESynchLockAndWaitIX.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Lock and wait object for POSIX synchronization objects.
 *
 ************************************************************************/

#include "areg/base/private/posix/CESynchLockAndWaitIX.hpp"

#ifdef _POSIX

#include "areg/base/private/posix/IEWaitableBaseIX.hpp"
#include "areg/base/ESynchObjects.hpp"
#include "areg/base/CEThread.hpp"
#include <errno.h>

//////////////////////////////////////////////////////////////////////////
// CESynchWaitableMapIX class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CESynchResourceMapIX class implementation
//////////////////////////////////////////////////////////////////////////

CESynchResourceMapIX::CESynchResourceMapIX( void )
    : TELockResourceListMap<IEWaitableBaseIX *, CESynchLockAndWaitIX, MapLockAndWait, ListLockAndWait, CEResourceListMapImpl>( )
{
    ; // do nothing
}

CESynchResourceMapIX::~CESynchResourceMapIX( void )
{
    ; // do nothing
}

CESynchResourceMapIX & CESynchResourceMapIX::GetInstance( void )
{
    static CESynchResourceMapIX _mapSynchResources;
    return _mapSynchResources;
}

//////////////////////////////////////////////////////////////////////////
// CESynchLockAndWaitIX class implementation
//////////////////////////////////////////////////////////////////////////

MapWaitIDResource   CESynchLockAndWaitIX::_mapWaitIdResource;


int CESynchLockAndWaitIX::WaitForSingleObject( IEWaitableBaseIX & synchWait, unsigned int msTimeout /* = IESynchObject::WAIT_INFINITE */ )
{
    IEWaitableBaseIX * list[] = { &synchWait };
    return WaitForMultipleObjects(list, 1, true, msTimeout);
}

int CESynchLockAndWaitIX::WaitForMultipleObjects( IEWaitableBaseIX ** listWaitables, int count, bool waitAll /* = false */, unsigned int msTimeout /* = IEBlockingSynchObject::WAIT_INFINITE */ )
{
    int result = static_cast<int>(NESynchTypesIX::SynchObjectInvalid);
    if ( (listWaitables != NULL) && (count > 0) )
    {
        OUTPUT_DBG("Going to wait [ %s%d ] event(s).", (waitAll && (count > 1) ? "all " : ""), count);
        CESynchLockAndWaitIX lockAndWait(   listWaitables
                                          , count
                                          , waitAll ? NESynchTypesIX::MatchConditionExact : NESynchTypesIX::MatchConditionAny
                                          , msTimeout);

        if ( (lockAndWait.IsEmpty() == false) && lockAndWait.Lock( ) )
        {
            _mapWaitIdResource.RegisterResourceObject(reinterpret_cast<ITEM_ID>(lockAndWait.mContext), &lockAndWait);

            int waitResult = ENOLCK;
            while ( lockAndWait.NoEventFired( ) )
            {
                waitResult = lockAndWait.WaitCondition( );
                if ( RETURNED_OK  != waitResult)
                {
                    lockAndWait.mFiredEntry = (waitResult == ETIMEDOUT) || (waitResult == EBUSY) ? NESynchTypesIX::SynchObjectTimeout : NESynchTypesIX::SynchWaitInterrupted;
                    break;
                }
            }

            _mapWaitIdResource.UnregisterResourceObject(reinterpret_cast<ITEM_ID>(lockAndWait.mContext));

            lockAndWait.Unlock( );
        }

        result = static_cast<int>(lockAndWait.mFiredEntry);
    }

    return result;
}

int CESynchLockAndWaitIX::EventSignaled( IEWaitableBaseIX & synchWaitable )
{
    int result = 0;

    CESynchResourceMapIX & mapResource = CESynchResourceMapIX::GetInstance( );
    mapResource.ResourceLock( );

    ListLockAndWait * waitList = mapResource.GetResource( &synchWaitable );
    if ( waitList != NULL )
    {
        ASSERT( waitList->IsEmpty( ) == false );
        OUTPUT_DBG("Waitable [ %s ] ID [ %p ] is signaled, there are [ %d ] locks accosiated with it."
                    , synchWaitable.GetName()
                    , &synchWaitable
                    , waitList->GetSize());

        for ( LISTPOS pos = waitList->GetHeadPosition( ); pos != NULL && synchWaitable.IsSignaled(); )
        {
            CESynchLockAndWaitIX * lockAndWait = waitList->GetAt(pos);
            pos = waitList->GetNextPosition( pos );
            ASSERT(lockAndWait != NULL);
            NESynchTypesIX::eSynchObjectFired fired = lockAndWait->CheckEventFired(synchWaitable);
            if ( fired >= NESynchTypesIX::SynchObject0 && fired <= NESynchTypesIX::SynchObjectAll )
            {
                if (lockAndWait->RequestOwnership(fired))
                {
                    OUTPUT_DBG(   "The waitable [ %s ] [ %p ] is fired, unlocking thread [ %p ] with fired event reason [ %d ]"
                                , synchWaitable.GetName()
                                , &synchWaitable
                                , lockAndWait->mContext
                                , static_cast<int>(fired));

                    ++ result;
                    lockAndWait->mFiredEntry = fired;
                    lockAndWait->NotifyEvent();
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

        OUTPUT_DBG("Waitable [ %s ] ID [ %p ] released [ %d ] threads.", synchWaitable.GetName(), &synchWaitable, result);
        synchWaitable.ThreadsReleased(result);
    }
#ifdef DEBUG
    else
    {
        // OUTPUT_INFO("The waitable [ %s ] ID [ %p ] does not have any waiting thread.", synchWaitable.GetName(), &synchWaitable);
    }
#endif // DEBUG


    mapResource.ResourceUnlock( );
    return result;
}

void CESynchLockAndWaitIX::EventRemove( IEWaitableBaseIX & synchWaitable )
{
    CESynchResourceMapIX & mapResource = CESynchResourceMapIX::GetInstance( );
    mapResource.ResourceLock( );

    ListLockAndWait * waitList = mapResource.GetResource( &synchWaitable );
    if ( waitList != NULL )
    {
        OUTPUT_ERR("The event [ %p / %s] is cleaning resource, there is still wait list, going to notify error [ %d ] locked threads and clean resources."
                    , &synchWaitable
                    , NESynchTypesIX::GetString(synchWaitable.GetSynchType())
                    , waitList->GetSize());
        ASSERT( waitList->IsEmpty( ) == false );

        for ( LISTPOS pos = waitList->GetHeadPosition( ); pos != NULL && synchWaitable.IsSignaled(); pos = waitList->GetNextPosition(pos))
        {
            CESynchLockAndWaitIX * lockAndWait = waitList->GetAt(pos);
            ASSERT(lockAndWait != NULL);

            int index = lockAndWait->GetWaitableIndex( synchWaitable );
            ASSERT(index != -1);
            lockAndWait->mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(index + NESynchTypesIX::SynchObject0Error);
            lockAndWait->NotifyEvent();
        }

        mapResource.UnregisterResource(&synchWaitable);
    }

    mapResource.ResourceUnlock( );
}

void CESynchLockAndWaitIX::EventFailed( IEWaitableBaseIX & synchWaitable )
{
    CESynchResourceMapIX & mapResource = CESynchResourceMapIX::GetInstance( );
    mapResource.ResourceLock( );

    ListLockAndWait * waitList = mapResource.GetResource( &synchWaitable );
    if ( waitList != NULL )
    {
        OUTPUT_WARN("The event [ %p ] failed, going to notify error [ %d ] locked threads.", &synchWaitable, waitList->GetSize());
        ASSERT( waitList->IsEmpty( ) == false );

        for ( LISTPOS pos = waitList->GetHeadPosition( ); pos != NULL && synchWaitable.IsSignaled(); pos = waitList->GetNextPosition(pos))
        {
            CESynchLockAndWaitIX * lockAndWait = waitList->GetAt(pos);
            ASSERT(lockAndWait != NULL);

            int index = lockAndWait->GetWaitableIndex( synchWaitable );
            ASSERT(index != -1);
            lockAndWait->mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(index + NESynchTypesIX::SynchObject0Error);
            lockAndWait->NotifyEvent();
        }
    }

    mapResource.ResourceUnlock( );
}

bool CESynchLockAndWaitIX::IsResourceRegistered( IEWaitableBaseIX & synchWaitable )
{
    bool result = false;

    CESynchResourceMapIX & mapResources = CESynchResourceMapIX::GetInstance();
    mapResources.ResourceLock();

    result = mapResources.ResourceKeyExist(&synchWaitable);

    mapResources.ResourceUnlock();

    return result;
}

CESynchLockAndWaitIX::CESynchLockAndWaitIX(   IEWaitableBaseIX ** listWaitables
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

    if ( InitPosixSynchObjects() )
    {
        CESynchResourceMapIX & mapResources = CESynchResourceMapIX::GetInstance();
        mapResources.ResourceLock();
        count = MACRO_MIN(CEMultiLock::MAXIMUM_WAITING_OBJECTS, count);

        if ( (mMatchCondition == NESynchTypesIX::MatchConditionAny ) || (mDescribe == WaitSingleObject) )
        {
            for ( int i = 0; i < count; ++ i, ++ listWaitables )
            {
                IEWaitableBaseIX * synchWaitable = *listWaitables;
                if (synchWaitable != NULL)
                {
                    ASSERT( (static_cast<unsigned int>(synchWaitable->GetSynchType()) & static_cast<unsigned int>(NESynchTypesIX::SoWaitable)) != 0);

                    mapResources.RegisterResourceObject(synchWaitable, this);
                    // OUTPUT_DBG("Waitable [ %p ] is [ %s ]", synchWaitable, synchWaitable->IsSignaled() ? "signaled" : "not signaled");

                    if (synchWaitable->IsSignaled() && synchWaitable->RequestsOwnership(mContext))
                    {
                        OUTPUT_DBG("Waitable [ %s ] with ID [ %p ] of type [ %s ] is signaled, going unlock thread [ %p ]"
                                    , synchWaitable->GetName()
                                    , synchWaitable
                                    , NESynchTypesIX::GetString(synchWaitable->GetSynchType())
                                    , mContext);

                        mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(i);
                        synchWaitable->ThreadsReleased(1);
                        mWaitingList.Resize(i);
                        break;
                    }

                    mWaitingList[i] = synchWaitable;
                }
                else
                {
                    mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(i + NESynchTypesIX::SynchObject0Error);
                    mWaitingList.Resize(i);
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
                    mapResources.RegisterResourceObject(synchWaitable, this);
                    if ( (eventFired) && (synchWaitable->IsSignaled() == false) )
                    {
                        eventFired = false;
                    }

                    mWaitingList[i] = synchWaitable;
                }
                else
                {
                    mFiredEntry = static_cast<NESynchTypesIX::eSynchObjectFired>(i + NESynchTypesIX::SynchObject0Error);
                    mWaitingList.Resize(i);
                    break;
                }
            }

            if (eventFired && RequestOwnership(NESynchTypesIX::SynchObjectAll))
            {
                OUTPUT_DBG("Releasing thread [ %p ], all events are fired.", mContext);

                mFiredEntry = NESynchTypesIX::SynchObjectAll;
                for (int i = 0; i < mWaitingList.GetSize(); ++ i)
                {
                    mWaitingList[i]->ThreadsReleased(1);
                }
            }
        }

        mapResources.ResourceUnlock();
    }
    else
    {
        ReleasePosixSynchObjects();
    }
}

CESynchLockAndWaitIX::~CESynchLockAndWaitIX( void )
{
    CESynchResourceMapIX::GetInstance().RemoveResourceObject(this, true);
}

inline bool CESynchLockAndWaitIX::NotifyEvent( void )
{
    bool result = false;

    if (IsValid() && Lock())
    {
        result = RETURNED_OK == pthread_cond_signal(&mCondVariable);
        Unlock();
    }

    return result;
}

inline bool CESynchLockAndWaitIX::InitPosixSynchObjects( void )
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

inline void CESynchLockAndWaitIX::ReleasePosixSynchObjects( void )
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

inline bool CESynchLockAndWaitIX::IsValid( void ) const
{
    return ((mPosixMutex != NULL) && (mCondVariable != NULL) && (mWaitingList.IsEmpty() == false));
}

inline bool CESynchLockAndWaitIX::Lock( void )
{
    return (mPosixMutex != NULL ? RETURNED_OK == pthread_mutex_lock(&mPosixMutex) : false);
}

inline void CESynchLockAndWaitIX::Unlock( void )
{
    if (mPosixMutex != NULL)
        pthread_mutex_unlock( &mPosixMutex );
}

inline int CESynchLockAndWaitIX::WaitCondition( void )
{
    if ( mWaitTimeout == IESynchObject::WAIT_INFINITE)
    {
        return pthread_cond_wait(&mCondVariable, &mPosixMutex);
    }
    else
    {
        timespec waitTimeout;
        NESynchTypesIX::TimeoutFromNow(waitTimeout, mWaitTimeout);
        return pthread_cond_timedwait( &mCondVariable, &mPosixMutex, &waitTimeout );
    }
}

inline int CESynchLockAndWaitIX::GetWaitableIndex( const IEWaitableBaseIX & synchWaitable ) const
{
    int result = -1;
    for ( int i = 0; i < mWaitingList.GetSize(); ++ i )
    {
        if (mWaitingList[i] == &synchWaitable)
        {
            result = i;
            break;
        }
    }

    return result;
}

NESynchTypesIX::eSynchObjectFired CESynchLockAndWaitIX::CESynchLockAndWaitIX::CheckEventFired( IEWaitableBaseIX & synchObject )
{
    NESynchTypesIX::eSynchObjectFired result = NESynchTypesIX::SynchObjectInvalid;

    if (synchObject.IsSignaled() && mWaitingList.GetSize() != 0)
    {
        if (mDescribe == CESynchLockAndWaitIX::WaitSingleObject)
        {
            result = NESynchTypesIX::SynchObject0;
        }
        else if (mMatchCondition == NESynchTypesIX::MatchConditionAny)
        {
            result = static_cast<NESynchTypesIX::eSynchObjectFired>(GetWaitableIndex(synchObject));
        }
        else
        {
            int i = 0;
#ifdef _DEBUG
            for ( ; i < mWaitingList.GetSize(); ++ i)
            {
                IEWaitableBaseIX *waitable = mWaitingList[i];
                ASSERT(waitable != NULL);
                if (waitable->IsSignaled())
                {
                    OUTPUT_DBG("Waitable [ %s ] ID [ %p ] type [ %s ] is signaled."
                                , waitable->GetName()
                                , waitable
                                , NESynchTypesIX::GetString(waitable->GetSynchType()));
                }
                else
                {
                    OUTPUT_DBG("Waitable [ %s ] ID [ %p ] type [ %s ] is NOT signaled yet, interrupting checkup."
                                , waitable->GetName()
                                , waitable
                                , NESynchTypesIX::GetString(waitable->GetSynchType()));
                    break;
                }
            }
#else   // !_DEBUG
            for (  ; (i < mWaitingList.GetSize()) && mWaitingList.GetAt(i)->IsSignaled(); ++ i)
                ;
#endif  // !_DEBUG

            if (i == mWaitingList.GetSize())
                result = NESynchTypesIX::SynchObjectAll;
        }
    }

    return result;
}

bool CESynchLockAndWaitIX::RequestOwnership( const NESynchTypesIX::eSynchObjectFired firedEvent )
{
    bool result = false;

    ASSERT(firedEvent >= NESynchTypesIX::SynchObject0 && firedEvent <= NESynchTypesIX::SynchObjectAll);
    if ( firedEvent != NESynchTypesIX::SynchObjectAll )
    {
        ASSERT(mWaitingList.GetSize() > static_cast<int>(firedEvent));
        IEWaitableBaseIX *waitable = mWaitingList[static_cast<int>(firedEvent)];
        
#ifdef DEBUG
        if (waitable == NULL)
        {
            OUTPUT_ERR("Atention! A waitable at index [ %d ] in the list [ %p ] having size [ %d] of thread [ %p ] has address [ %p ]"
                        , static_cast<int>(firedEvent)
                        , this
                        , mWaitingList.GetSize()
                        , CEThread::GetCurrentThread()
                        , waitable);
        }

        ASSERT(waitable != NULL);
#endif // DEBUG

        OUTPUT_DBG("Thread [ %p ] requests ownership of waitable [ %s ], thread waits no more waitables."
                    , mContext
                    , NESynchTypesIX::GetString(waitable->GetSynchType()));

        result = waitable->RequestsOwnership(mContext);
    }
    else
    {
        ASSERT(mWaitingList.GetSize() <= static_cast<int>(firedEvent));
        OUTPUT_DBG("Thread [ %p ] requests ownership of [ %d ] waitables.", mContext, mWaitingList.GetSize());

        result = true;
        for (int i = 0; (i < mWaitingList.GetSize()) && result; ++ i)
        {
            IEWaitableBaseIX *waitable = mWaitingList[i];
            ASSERT(waitable != NULL);
            result = waitable->RequestsOwnership(mContext);

            OUTPUT_DBG("Thread [ %p ] requested ownership of waitable [ %s ] listed [ %d of %d ], the ownership is [ %s ], the current state of waitable [ %s ]"
                        , mContext
                        , NESynchTypesIX::GetString(waitable->GetSynchType())
                        , i + 1
                        , mWaitingList.GetSize()
                        , result ? "taken" : "rejected"
                        , waitable->IsSignaled() ? "signaled" : "not signaled");
        }
    }

    return result;
}

bool CESynchLockAndWaitIX::NotifyAsynchSignal( void )
{
    CESynchResourceMapIX & mapResource = CESynchResourceMapIX::GetInstance( );
    mapResource.ResourceLock( );

    mFiredEntry = NESynchTypesIX::SynchAsynchSignal;
    bool result = NotifyEvent();

    mapResource.ResourceUnlock( );

    return result;
}

#endif // !_POSIX
