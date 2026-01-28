#ifndef AREG_BASE_PRIVATE_POSIX_SYNCLOCKANDWAITIX_HPP
#define AREG_BASE_PRIVATE_POSIX_SYNCLOCKANDWAITIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SyncLockAndWaitIX.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Lock and wait object for POSIX synchronization objects.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/NECommon.hpp"
#include "areg/base/private/posix/NESyncTypesIX.hpp"
#include "areg/base/IESyncObject.hpp"
#include "areg/base/TEHashMap.hpp"
#include "areg/base/TELinkedList.hpp"
#include "areg/base/TEFixedArray.hpp"
#include "areg/base/TEResourceMap.hpp"
#include "areg/base/TEResourceListMap.hpp"

#include <pthread.h>

 /************************************************************************
  * dependencies.
  ************************************************************************/
class IEWaitableBaseIX;
class SyncLockAndWaitIX;

//////////////////////////////////////////////////////////////////////////
// SyncLockAndWaitIX class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   LockAndWait object that makes locking of single and multiple objects and waits until
 *          one or all objects are signaled. The waiting criteria depends on the flag.
 *          There is a limitation of waiting objects at once, and the maximum numbers are
 *          equal to NECommon::MAXIMUM_WAITING_OBJECTS.
 *          Use static methods for waiting functionalities. The internal methods are hidden.
 **/
class SyncLockAndWaitIX
{
    /**
     * \brief   The list of LockAndWait objects.
     **/
    using ListLockAndWait       = TELinkedList<SyncLockAndWaitIX *>;
    /**
     * \brief   The hash map container of waitable object and LockAndWait lists.
     **/
    using MapLockAndWait        = TEMap<IEWaitableBaseIX *, ListLockAndWait>;

//////////////////////////////////////////////////////////////////////////
// ImplResourceListMap class declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The helper class of resource list map that contains helper functions implementation.
     **/
    class ImplResourceListMap : public TEResourceListMapImpl<IEWaitableBaseIX *, SyncLockAndWaitIX, ListLockAndWait>
    {
    public:
        /**
         * \brief   Called when all resources are removed.
         *          This function is called from RemoveAllResources() for every single
         *          resource being unregistered.
         * \param   Key     The Key value of resource.
         * \param   List    The list of resource objects.
         **/
        inline void implCleanResourceList( IEWaitableBaseIX * & /* Key */, ListLockAndWait & List )
        {
        	List.clear();
        }

        /**
         * \brief   Called when need to add resource object to the list.
         * \param   List        The list of resource objects.
         * \param   Resource    The resource object to add to the list.
         **/
        inline void implAddResource( ListLockAndWait & List, SyncLockAndWaitIX * Resource )
        {
        	List.pushLast(Resource);
        }

        /**
         * \brief   Called when need to remove resource object from the list.
         * \param   List        The list of resource objects.
         * \param   Resource    The resource object to remove from the list.
         **/
        inline bool implRemoveResource( ListLockAndWait & List, SyncLockAndWaitIX * Resource )
        {
        	return List.removeEntry( Resource );
        }
    };

    /**
     * \brief   The resource list map of waitable object where the keys are waitable objects
     *          and the resource objects are WaitAndLock objects in the list. The WaitAndLock
     *          objects in the entire map are not unique, but should be unique in the list.
     **/
    using SyncResourceMapIX = TELockResourceListMap<IEWaitableBaseIX *, SyncLockAndWaitIX *, ListLockAndWait, MapLockAndWait, ImplResourceListMap>;

//////////////////////////////////////////////////////////////////////////
// The resource map for timer.
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The resource map of waitable, where keys are id_type and the values are WaitAndLock objects
     **/
    using MapWaitID         = TEIdHashMap<SyncLockAndWaitIX *>;
    /**
     * \brief   Helper object for resource map basic method implementations
     **/
    using ImplWaitIDResource= TEResourceMapImpl<ptr_type, SyncLockAndWaitIX *>;
    /**
     * \brief   Resource map of waitable where the keys are pthread_t (thread ID) and the values are
     *          LockAndWait objects. It is used in the timer.
     **/
    using MapWaitIDResource = TELockResourceMap<ptr_type, SyncLockAndWaitIX *, MapWaitID, ImplWaitIDResource>;

//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class TimerManager;
    friend class WaitableTimerIX;

//////////////////////////////////////////////////////////////////////////
// Constants and statics
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   SyncLockAndWaitIX::eWaitType
     *          Describes the waiting type.
     **/
    typedef enum class E_WaitType
    {
          WaitSingleObject      //!< Waits a single object
        , WaitMultipleObjects   //!< Waits for multiple object

    } eWaitType;

    /**
     * \brief   SyncLockAndWaitIX::eEventFired
     *          Describes the fired state of each event.
     **/
    typedef enum class E_EventFired
    {
          EventFiredNone        //!< No event has been fired.
        , EventFiredOne         //!< Fired one event.
        , EventFiredAll         //!< All waiting events are fired.

    } eEventFired;
    /**
     * \brief   The fixed array of waitable. The maximum size of array is NECommon::MAXIMUM_WAITING_OBJECTS
     **/
    using WaitingList   = TEFixedArray<IEWaitableBaseIX *>;

//////////////////////////////////////////////////////////////////////////
// Public static methods.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Call to lock the synchronization object of wait until it is released by other thread.
     *          If waitable is signaled, the function immediately returns. If waitable is not signaled,
     *          the function block thread until either waitable is signaled or timeout expires. In case
     *          of some waitable such as Mutex, this call takes the ownership. in case of Synchronization Events
     *          this may reset signaled state or leave in signaled state, depending on Synchronization Event types.
     *          For more details see the description of each waitable object.
     * \param   syncWait    The waitable object to check the signaled state.
     * \param   msTimeout   The timeout in milliseconds to wait until waitable is signaled.
     *                      If NECommon::WAIT_INFINITE, it will wait until event is signaled or failed.
     *                      Any other value indicates timeout to wait.
     * \return  It returns one of following values:
     *              - NESyncTypesIX::SyncObject0 if waitable was signaled;
     *              - NESyncTypesIX::SyncObjectTimeout if waiting timeout is expired;
     *              - NESyncTypesIX::SyncWaitInterrupted if waiting was interrupted by such event like timer;
     *              - NESyncTypesIX::SyncObject0Error if error happened. For example, the waitable is invalidated.
     **/
    static int waitForSingleObject( IEWaitableBaseIX & syncWait, unsigned int msTimeout = NECommon::WAIT_INFINITE );

    /**
     * \brief   Call to lock and wait the list of synchronization objects until one or all objects are signaled.
     *          The waiting criteria depends on waitable signal state, waitAll parameter and timeout.
     *          There can be maximum NECommon::MAXIMUM_WAITING_OBJECTS in the list to wait. This limitation is synchronized
     *          with Microsoft limitation. In practice, normally there are very few events in the list.
     *          In case of some waitable such as Mutex, this call takes the ownership. in case of Synchronization Events
     *          this may reset signaled state or leave in signaled state, depending on Synchronization Event types.
     *          For more details see the description of each waitable object.
     * \param   listWaitables   The list of waitables to check the signaled state. There should be no more than 
     *                          NECommon::MAXIMUM_WAITING_OBJECTS entries in the list.
     * \param   count           The number of waitables in the list. There should be no more than 
     *                          NECommon::MAXIMUM_WAITING_OBJECTS entries.
     * \param   waitAll         If true, the call is locks the thread until all waitables in the list are signaled.
     *                          If false, any waitable in signaled state unlocks the thread.
     * \param   msTimeout       The timeout in milliseconds to wait until waitable is signaled.
     *                          If NECommon::WAIT_INFINITE, it will wait until event is signaled or failed.
     *                          Any other value indicates timeout to wait.
     * \return  It returns one of following values:
     *              - NESyncTypesIX::SyncObject0 + N if 'waitAll' flag is false and waitable was signaled, where 'N' is the index of waitable in the list.
     *              - NESyncTypesIX::SyncObjectAll if 'waitAll' flag is true and all waitables are signaled.
     *              - NESyncTypesIX::SyncObjectTimeout if waiting timeout is expired;
     *              - NESyncTypesIX::SyncWaitInterrupted if waiting was interrupted by such event like timer;
     *              - NESyncTypesIX::SyncObject0Error + N if error happened, where 'N' is the index of failed waitable object. For example, the waitable is invalidated.
     **/
    static int waitForMultipleObjects( IEWaitableBaseIX ** listWaitables, int count, bool waitAll = false, unsigned int msTimeout = NECommon::WAIT_INFINITE);

    /**
     * \brief   Called by waitable object to indicate that it is in signaled state.
     * \param   syncWaitable    The waitable object that is in signaled state.
     * \return  Returns the number of threads that are notified.
     *          In case of Mutex this should be one. In case of Synchronization Event there can be multiple threads.
     *          For more details see description of each waitable object
     **/
    static int eventSignaled( IEWaitableBaseIX & syncWaitable );

    /**
     * \brief   Called by waitable object to indicate wait failure. For example, when waitable object is invalidated.
     *          This call unlocks all threads that wait for event and the waiting return indicates error.
     * \param   syncWaitable    The waitable object that should indicate error.
     **/
    static void eventFailed( IEWaitableBaseIX & syncWaitable );

    /**
     * \brief   Call to remove waitable object from the waiting list.
     * \param   syncWaitable    The waitable object to remove from the list.
     **/
    static void eventRemove( IEWaitableBaseIX & syncWaitable );

    /**
     * \brief   Checks whether the waitable is registered or not.
     * \param   syncWaitable    Waitable to check the registration.
     * \return  Returns true if waitable is registered.
     **/
    static bool isWaitableRegistered( IEWaitableBaseIX & syncWaitable );

    /**
     * \brief   Notifies the asynchronous execution within a locked thread. This call breaks waiting process of thread
     *          that can be locked again when finished processing asynchronous execution.
     * \param   threadId    The ID of the thread that is going to break waiting.
     * \return  Returns true if operation succeeded. The operation can fail if thread is not locked.
     **/
    static bool notifyAsyncSignal( id_type threadId );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes WaitAndLock object, sets flags and checks signaled sate of waitables.
     * \param   listWaitables   The list of waitables. The maximum number of entries should be NECommon::MAXIMUM_WAITING_OBJECTS
     * \param   count           The number of waitables in the list. The maximum number of entries should be NECommon::MAXIMUM_WAITING_OBJECTS.
     * \param   matchCondition  The signaled state matching criteria. Either it should have exact match, i.e. wait all events to be signaled,
     *                          or it should wait for any event to be in signaled state.
     * \param   msTimeout       Initializes the timeout in milliseconds to wait.
     **/
    SyncLockAndWaitIX( IEWaitableBaseIX ** listWaitables, int count, NESyncTypesIX::eMatchCondition matchCondition, unsigned int msTimeout );

    /**
     * \brief   Destructor.
     **/
    ~SyncLockAndWaitIX( void );

//////////////////////////////////////////////////////////////////////////
// Hidden operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns static list of waitables, where keys are id_type and values are waitables.
     **/
    static SyncLockAndWaitIX::MapWaitIDResource& _mapWaitResourceIds(void);

    /**
     * \brief   Returns the static instance of synchronization resource map.
     */
    static SyncResourceMapIX& _mapSyncResources(void);

    /**
     * \brief   Returns true if no event in the list is fired.
     **/
    inline bool _noEventFired( void ) const;

    /**
     * \brief   Initializes internal POSIX objects. Returns true if initialization succeeded.
     **/
    inline bool _initPosixSyncObjects( void );

    /**
     * \brief   Releases internal POSIX objects to free resources.
     **/
    inline void _releasePosixSyncObjects( void );

    /**
     * \brief   Returns true if object is valid. The LockAndWait object is valid if internal POSIX objects are
     *          initialized and the waiting list not empty.
     **/
    inline bool _isValid( void ) const;

    /**
     * \brief   Locks the WaitAndLock object, waits for event fired criteria.
     *          This may block the calling thread.
     * \return  Returns true if locking succeeded.
     **/
    inline bool _lock( void );

    /**
     * \brief   Unlocks WaitAndLock object.
     **/
    inline void _unlock( void );

    /**
     * \brief   Called to wait for condition variable. Either it waits with infinite wait flag or with timeout.
     * \return  Returns POSIX error code. If 0, the waiting method succeeded.
     **/
    inline int _waitCondition( void );

    /**
     * \brief   Returns the index of registered waitable in the list.
     * \param   syncWaitable   The instance of waitable object to lookup in the list.
     **/
    inline int _getWaitableIndex( const IEWaitableBaseIX & syncWaitable ) const;

    /**
     * \brief   Called to notify the event has been fired.
     * \return  Returns true if succeeded to notify.
     **/
    inline bool _notifyEvent( void );

    /**
     * \brief   Checks whether the waiting list is empty.
     **/
    inline bool _isEmpty( void ) const;

    /**
     * \brief   Checks whether the waitable event is fired or not.
     * \param   syncObject The waitable object to check.
     * \return  Returns one of event fired state.
     **/
    NESyncTypesIX::eSyncObjectFired _checkEventFired( IEWaitableBaseIX & syncObject );

    /**
     * \brief   Called to notify threads to take fired event ownership.
     * \param   firedEvent  The index of fired event in the list that notifies the threads to take ownership.
     * \return  Returns true if threads are notified or took ownership.
     **/
    bool _requestOwnership( const NESyncTypesIX::eSyncObjectFired firedEvent );

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Describes the waiting type. Either should wait for all events or for any.
     **/
    const eWaitType                     mDescribe;
    /**
     * \brief   Describes the lock and wait condition.
     **/
    const NESyncTypesIX::eMatchCondition   mMatchCondition;
    /**
     * \brief   Timeout in milliseconds to wait when blocks the thread.
     **/
    const unsigned int                  mWaitTimeout;
    /**
     * \brief   The ID of thread that instantiated LockAndWait object.
     **/
    const pthread_t                     mContext;
    /**
     * \brief   Internal POSIX mutex object to synchronize data access.
     **/
    mutable pthread_mutex_t             mPosixMutex;
    /**
     * \brief   The POSIX mutex validity flag.
     **/
    mutable bool                        mMutexValid;
    /**
     * \brief   Internal POSIX mutex attribute to initialize mutex.
     **/
    mutable pthread_mutexattr_t         mPosixMutexAttr;
    /**
     * \brief   The POSIX mutex attribute validity flag.
     **/
    mutable bool                        mMutexAttrValid;
    /**
     * \brief   Internal POSIX conditional variable.
     **/
    pthread_cond_t                      mCondVariable;
    /**
     * \brief   The POSIX conditional variable validity flag.
     **/
    bool                                mCondVarValid;
    /**
     * \brief   Internal POSIX conditional variable attribute
     **/
    pthread_condattr_t                  mCondAttribute;
    /**
     * \brief   The POSIX conditional variable attribute validity flag.
     **/
    bool                                mCondAttrValid;
    /**
     * \brief   Indicates the fired event object or error code.
     **/
    NESyncTypesIX::eSyncObjectFired     mFiredEntry;
    /**
     * \brief   The list of waitables.
     **/
    WaitingList                         mWaitingList;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    SyncLockAndWaitIX( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( SyncLockAndWaitIX );
};

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SYNCLOCKANDWAITIX_HPP
