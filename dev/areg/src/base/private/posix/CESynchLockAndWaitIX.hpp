#ifndef AREG_BASE_PRIVATE_POSIX_CESYNCHLOCKANDWAITIX_HPP
#define AREG_BASE_PRIVATE_POSIX_CESYNCHLOCKANDWAITIX_HPP
/************************************************************************
 * \file        areg/src/base/private/posix/CESynchLockAndWaitIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Lock and wait object for POSIX synchronization objects.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/src/base/GEGlobal.h"

#ifdef _POSIX

#include "areg/src/base/private/posix/NESynchTypesIX.hpp"
#include "areg/src/base/IESynchObject.hpp"
#include "areg/src/base/TEHashMap.hpp"
#include "areg/src/base/TELinkedList.hpp"
#include "areg/src/base/TEFixedArray.hpp"
#include "areg/src/base/TEResourceMap.hpp"
#include "areg/src/base/TEResourceListMap.hpp"

#include <pthread.h>

class IEWaitableBaseIX;
class CESynchLockAndWaitIX;

//////////////////////////////////////////////////////////////////////////
// CESynchWaitableMapIX class declaration
//////////////////////////////////////////////////////////////////////////

typedef TEListImpl<const CESynchLockAndWaitIX *>                                                                        ListLockAndWaitImpl;
typedef TELinkedList<CESynchLockAndWaitIX *, const CESynchLockAndWaitIX *, ListLockAndWaitImpl>                         ListLockAndWait;
typedef TEHashMapImpl<const IEWaitableBaseIX *, ListLockAndWait &>                                                      MapLockAndWaitImpl;
typedef TEHashMap<IEWaitableBaseIX *, ListLockAndWait, const IEWaitableBaseIX *, ListLockAndWait &, MapLockAndWaitImpl> MapLockAndWait;

class CEResourceListMapImpl : public TEResourceListMapImpl<IEWaitableBaseIX *, CESynchLockAndWaitIX, ListLockAndWait>
{
public:
    /**
     * \brief	Called when all resources are removed.
     *          This function is called from RemoveAllResources() for every single
     *          resource being unregistered.
     * \param	Key	    The Key value of resource.
     * \param	List    The list of resource objects.
     **/
    inline void ImplCleanResourceList( IEWaitableBaseIX * & /* Key */, ListLockAndWait & List )
    {   List.RemoveAll();                   }

    /**
     * \brief	Called when need to add resource object to the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to add to the list.
     **/
    inline void ImplAddResourceToList( ListLockAndWait & List, CESynchLockAndWaitIX * Resource )
    {   List.AddTail(Resource);             }

    /**
     * \brief	Called when need to remove resource object from the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to remove from the list.
     **/
    inline bool ImplRemoveResourceFromList( ListLockAndWait & List, CESynchLockAndWaitIX * Resource )
    {   return List.RemoveElem( Resource ); }
};

//////////////////////////////////////////////////////////////////////////
// CESynchResourceMapIX class declaration
//////////////////////////////////////////////////////////////////////////
class CESynchResourceMapIX : public TELockResourceListMap<  IEWaitableBaseIX *
                                                          , CESynchLockAndWaitIX
                                                          , MapLockAndWait
                                                          , ListLockAndWait
                                                          , CEResourceListMapImpl>
{
public:
    static CESynchResourceMapIX & GetInstance( void );

private:
    CESynchResourceMapIX( void );

    ~CESynchResourceMapIX( void );
};

class CESynchLockAndWaitIX;

/**
 * \brief   Resource map of CESynchLockAndWaitIX associated with thread ID as a key.
 **/
typedef TEIdHashMapImpl< CESynchLockAndWaitIX * >                                       ImplMapWaitID;
typedef TEIdHashMap<CESynchLockAndWaitIX *, CESynchLockAndWaitIX *, ImplMapWaitID>      MapWaitID;
typedef TEResourceMapImpl<ITEM_ID, CESynchLockAndWaitIX>                                ImplWaitIDResource;
typedef TELockResourceMap<ITEM_ID, CESynchLockAndWaitIX, MapWaitID, ImplWaitIDResource> MapWaitIDResource;

class CESynchLockAndWaitIX
{
    friend class CETimerManager;
    friend class CEWaitableTimerIX;

private:
    typedef enum E_WaitType
    {
          WaitSingleObject
        , WaitMultipleObjects
    } eWaitType;

    typedef enum E_EventFired
    {
          EventFiredNone
        , EventFiredOne
        , EventFiredAll

    } eEventFired;

    typedef TEFixedArray<IEWaitableBaseIX *, IEWaitableBaseIX *>  WaitingList;

public:
    static const int            _MAX_WAIT_COUNT         /* = 32 */;

public:
    static int WaitForSingleObject( IEWaitableBaseIX & synchWait, unsigned int msTimeout = IESynchObject::WAIT_INFINITE );

    static int WaitForMultipleObjects( IEWaitableBaseIX ** listWaitables, int count, bool waitAll = false, unsigned int msTimeout = IEBlockingSynchObject::WAIT_INFINITE);

    static int EventSignaled( IEWaitableBaseIX & synchWaitable );

    static void EventFailed( IEWaitableBaseIX & synchWaitable );

    static void EventRemove( IEWaitableBaseIX & synchWaitable );

    static bool IsResourceRegistered( IEWaitableBaseIX & synchWaitable );

private:
    CESynchLockAndWaitIX( IEWaitableBaseIX ** listWaitables, int count, NESynchTypesIX::eMatchCondition matchCondition, unsigned int msTimeout );
    virtual ~CESynchLockAndWaitIX( void );

private:

    inline bool NoEventFired( void ) const;

    inline bool InitPosixSynchObjects( void );

    inline void ReleasePosixSynchObjects( void );

    inline bool IsValid( void ) const;

    inline bool Lock( void );

    inline void Unlock( void );

    inline int WaitCondition( void );

    inline int GetWaitableIndex( const IEWaitableBaseIX & synchWaitable ) const;

    inline bool NotifyEvent( void );

    inline bool IsEmpty( void ) const;

    bool SetEventFiedEntry( IEWaitableBaseIX & synchObject );

    NESynchTypesIX::eSynchObjectFired CheckEventFired( IEWaitableBaseIX & synchObject );

    bool RequestOwnership( const NESynchTypesIX::eSynchObjectFired firedEvent );

    bool NotifyAsynchSignal( void );

private:
    const eWaitType                         mDescribe;
    const NESynchTypesIX::eMatchCondition   mMatchCondition;
    const unsigned int                      mWaitTimeout;
    const pthread_t                         mContext;
    mutable pthread_mutex_t                 mPosixMutex;
    mutable pthread_mutexattr_t             mPosixMutexAttr;
    pthread_cond_t                          mCondVariable;
    pthread_condattr_t                      mCondAttribute;
    NESynchTypesIX::eSynchObjectFired       mFiredEntry;
    WaitingList                             mWaitingList;

    static MapWaitIDResource                _mapWaitIdResource;
};

inline bool CESynchLockAndWaitIX::NoEventFired( void ) const
{   return (mFiredEntry == NESynchTypesIX::SynchObjectInvalid); }

inline bool CESynchLockAndWaitIX::IsEmpty( void ) const
{   return mWaitingList.IsEmpty();                              }

#endif  // _POSIX
#endif  // AREG_BASE_PRIVATE_POSIX_CESYNCHLOCKANDWAITIX_HPP
