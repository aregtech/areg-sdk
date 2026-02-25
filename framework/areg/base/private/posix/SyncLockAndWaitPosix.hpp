#ifndef AREG_BASE_PRIVATE_POSIX_SYNCLOCKANDWAITPOSIX_HPP
#define AREG_BASE_PRIVATE_POSIX_SYNCLOCKANDWAITPOSIX_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/SyncLockAndWaitPosix.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Lock and wait object for POSIX synchronization objects.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include <cstdint>

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/CommonDefs.hpp"
#include "areg/base/private/posix/SyncDefsPosix.hpp"
#include "areg/base/SyncObject.hpp"
#include "areg/base/HashMap.hpp"
#include "areg/base/LinkedList.hpp"
#include "areg/base/FixedArray.hpp"
#include "areg/base/ResourceMap.hpp"
#include "areg/base/ResourceListMap.hpp"

#include <pthread.h>

 /************************************************************************
  * dependencies.
  ************************************************************************/
class WaitablePosix;
class SyncLockAndWaitPosix;

//////////////////////////////////////////////////////////////////////////
// SyncLockAndWaitPosix class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   LockAndWait object that makes locking of single and multiple objects and waits until
 *          one or all objects are signaled. The waiting criteria depends on the flag.
 *          There is a limitation of waiting objects at once, and the maximum numbers are
 *          equal to areg::MAXIMUM_WAITING_OBJECTS.
 *          Use static methods for waiting functionalities. The internal methods are hidden.
 **/
class SyncLockAndWaitPosix
{
    /**
     * \brief   The list of LockAndWait objects.
     **/
    using ListLockAndWait       = areg::LinkedList<SyncLockAndWaitPosix *>;
    /**
     * \brief   The hash map container of waitable object and LockAndWait lists.
     **/
    using MapLockAndWait        = areg::OrderedMap<WaitablePosix *, ListLockAndWait>;

//////////////////////////////////////////////////////////////////////////
// ImplResourceListMap class declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The helper class of resource list map that contains helper functions implementation.
     **/
    class ImplResourceListMap : public areg::ResourceListMapImpl<WaitablePosix *, SyncLockAndWaitPosix, ListLockAndWait>
    {
    public:
        /**
         * \brief   Called when all resources are removed.
         *          This function is called from RemoveAllResources() for every single
         *          resource being unregistered.
         * \param   Key     The Key value of resource.
         * \param   List    The list of resource objects.
         **/
        inline void implCleanResourceList( WaitablePosix * & /* Key */, ListLockAndWait & List )
        {
        	List.clear();
        }

        /**
         * \brief   Called when need to add resource object to the list.
         * \param   List        The list of resource objects.
         * \param   Resource    The resource object to add to the list.
         **/
        inline void implAddResource( ListLockAndWait & List, SyncLockAndWaitPosix * Resource )
        {
        	List.pushLast(Resource);
        }

        /**
         * \brief   Called when need to remove resource object from the list.
         * \param   List        The list of resource objects.
         * \param   Resource    The resource object to remove from the list.
         **/
        inline bool implRemoveResource( ListLockAndWait & List, SyncLockAndWaitPosix * Resource )
        {
        	return List.removeEntry( Resource );
        }
    };

    /**
     * \brief   The resource list map of waitable object where the keys are waitable objects
     *          and the resource objects are WaitAndLock objects in the list. The WaitAndLock
     *          objects in the entire map are not unique, but should be unique in the list.
     **/
    using SyncResourceMapIX = areg::ConcurrentResourceListMap<WaitablePosix *, SyncLockAndWaitPosix *, ListLockAndWait, MapLockAndWait, ImplResourceListMap>;

//////////////////////////////////////////////////////////////////////////
// The resource map for timer.
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The resource map of waitable, where keys are id_type and the values are WaitAndLock objects
     **/
    using MapWaitID         = IdHashMap<SyncLockAndWaitPosix *>;
    /**
     * \brief   Helper object for resource map basic method implementations
     **/
    using ImplWaitIDResource= areg::ResourceMapImpl<ptr_type, SyncLockAndWaitPosix *>;
    /**
     * \brief   Resource map of waitable where the keys are pthread_t (thread ID) and the values are
     *          LockAndWait objects. It is used in the timer.
     **/
    using MapWaitIDResource = areg::ConcurrentResourceMap<ptr_type, SyncLockAndWaitPosix *, MapWaitID, ImplWaitIDResource>;

//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class TimerManager;
    friend class WaitableTimerPosix;

//////////////////////////////////////////////////////////////////////////
// Constants and statics
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   SyncLockAndWaitPosix::WaitMode
     *          Describes the waiting type.
     **/
    enum class WaitMode
    {
          Single    //!< Waits a single object
        , Multiple  //!< Waits for multiple object

    };

    /**
     * \brief   SyncLockAndWaitPosix::SignalState
     *          Describes the fired state of each event.
     **/
    enum class SignalState
    {
          None  //!< No event has been fired.
        , One   //!< Fired one event.
        , All   //!< All waiting events are fired.

    };
    /**
     * \brief   The fixed array of waitable. The maximum size of array is areg::MAXIMUM_WAITING_OBJECTS
     **/
    using WaitingList   = FixedArray<WaitablePosix *>;

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
     *                      If areg::WAIT_INFINITE, it will wait until event is signaled or failed.
     *                      Any other value indicates timeout to wait.
     * \return  It returns one of following values:
     *              - areg::os::SyncSignal::First if waitable was signaled;
     *              - areg::os::SyncSignal::Timeout if waiting timeout is expired;
     *              - areg::os::SyncSignal::Interrupted if waiting was interrupted by such event like timer;
     *              - areg::os::SyncSignal::FirstError if error happened. For example, the waitable is invalidated.
     **/
    static int32_t waitForSingleObject( WaitablePosix & syncWait, uint32_t msTimeout = areg::WAIT_INFINITE );

    /**
     * \brief   Call to lock and wait the list of synchronization objects until one or all objects are signaled.
     *          The waiting criteria depends on waitable signal state, waitAll parameter and timeout.
     *          There can be maximum areg::MAXIMUM_WAITING_OBJECTS in the list to wait. This limitation is synchronized
     *          with Microsoft limitation. In practice, normally there are very few events in the list.
     *          In case of some waitable such as Mutex, this call takes the ownership. in case of Synchronization Events
     *          this may reset signaled state or leave in signaled state, depending on Synchronization Event types.
     *          For more details see the description of each waitable object.
     * \param   listWaitables   The list of waitables to check the signaled state. There should be no more than 
     *                          areg::MAXIMUM_WAITING_OBJECTS entries in the list.
     * \param   count           The number of waitables in the list. There should be no more than 
     *                          areg::MAXIMUM_WAITING_OBJECTS entries.
     * \param   waitAll         If true, the call is locks the thread until all waitables in the list are signaled.
     *                          If false, any waitable in signaled state unlocks the thread.
     * \param   msTimeout       The timeout in milliseconds to wait until waitable is signaled.
     *                          If areg::WAIT_INFINITE, it will wait until event is signaled or failed.
     *                          Any other value indicates timeout to wait.
     * \return  It returns one of following values:
     *              - areg::os::SyncSignal::First + N if 'waitAll' flag is false and waitable was signaled, where 'N' is the index of waitable in the list.
     *              - areg::os::SyncSignal::All if 'waitAll' flag is true and all waitables are signaled.
     *              - areg::os::SyncSignal::Timeout if waiting timeout is expired;
     *              - areg::os::SyncSignal::Interrupted if waiting was interrupted by such event like timer;
     *              - areg::os::SyncSignal::FirstError + N if error happened, where 'N' is the index of failed waitable object. For example, the waitable is invalidated.
     **/
    static int32_t waitForMultipleObjects( WaitablePosix ** listWaitables, int32_t count, bool waitAll = false, uint32_t msTimeout = areg::WAIT_INFINITE);

    /**
     * \brief   Called by waitable object to indicate that it is in signaled state.
     * \param   syncWaitable    The waitable object that is in signaled state.
     * \return  Returns the number of threads that are notified.
     *          In case of Mutex this should be one. In case of Synchronization Event there can be multiple threads.
     *          For more details see description of each waitable object
     **/
    static int32_t eventSignaled( WaitablePosix & syncWaitable );

    /**
     * \brief   Called by waitable object to indicate wait failure. For example, when waitable object is invalidated.
     *          This call unlocks all threads that wait for event and the waiting return indicates error.
     * \param   syncWaitable    The waitable object that should indicate error.
     **/
    static void eventFailed( WaitablePosix & syncWaitable );

    /**
     * \brief   Call to remove waitable object from the waiting list.
     * \param   syncWaitable    The waitable object to remove from the list.
     **/
    static void eventRemove( WaitablePosix & syncWaitable );

    /**
     * \brief   Checks whether the waitable is registered or not.
     * \param   syncWaitable    Waitable to check the registration.
     * \return  Returns true if waitable is registered.
     **/
    static bool isWaitableRegistered( WaitablePosix & syncWaitable );

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
     * \param   listWaitables   The list of waitables. The maximum number of entries should be areg::MAXIMUM_WAITING_OBJECTS
     * \param   count           The number of waitables in the list. The maximum number of entries should be areg::MAXIMUM_WAITING_OBJECTS.
     * \param   matchCondition  The signaled state matching criteria. Either it should have exact match, i.e. wait all events to be signaled,
     *                          or it should wait for any event to be in signaled state.
     * \param   msTimeout       Initializes the timeout in milliseconds to wait.
     **/
    SyncLockAndWaitPosix( WaitablePosix ** listWaitables, int32_t count, areg::os::WaitCondition matchCondition, uint32_t msTimeout );

    /**
     * \brief   Destructor.
     **/
    ~SyncLockAndWaitPosix();

//////////////////////////////////////////////////////////////////////////
// Hidden operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns static list of waitables, where keys are id_type and values are waitables.
     **/
    static SyncLockAndWaitPosix::MapWaitIDResource& _mapWaitResourceIds();

    /**
     * \brief   Returns the static instance of synchronization resource map.
     */
    static SyncResourceMapIX& _mapSyncResources();

    /**
     * \brief   Returns true if no event in the list is fired.
     **/
    inline bool _noEventFired() const;

    /**
     * \brief   Initializes internal POSIX objects. Returns true if initialization succeeded.
     **/
    inline bool _initPosixSyncObjects();

    /**
     * \brief   Releases internal POSIX objects to free resources.
     **/
    inline void _releasePosixSyncObjects();

    /**
     * \brief   Returns true if object is valid. The LockAndWait object is valid if internal POSIX objects are
     *          initialized and the waiting list not empty.
     **/
    inline bool _isValid() const;

    /**
     * \brief   Locks the WaitAndLock object, waits for event fired criteria.
     *          This may block the calling thread.
     * \return  Returns true if locking succeeded.
     **/
    inline bool _lock();

    /**
     * \brief   Unlocks WaitAndLock object.
     **/
    inline void _unlock();

    /**
     * \brief   Called to wait for condition variable. Either it waits with infinite wait flag or with timeout.
     * \return  Returns POSIX error code. If 0, the waiting method succeeded.
     **/
    inline int32_t _waitCondition();

    /**
     * \brief   Returns the index of registered waitable in the list.
     * \param   syncWaitable   The instance of waitable object to lookup in the list.
     **/
    inline int32_t _getWaitableIndex( const WaitablePosix & syncWaitable ) const;

    /**
     * \brief   Called to notify the event has been fired.
     * \return  Returns true if succeeded to notify.
     **/
    inline bool _notifyEvent();

    /**
     * \brief   Checks whether the waiting list is empty.
     **/
    inline bool _isEmpty() const;

    /**
     * \brief   Checks whether the waitable event is fired or not.
     * \param   syncObject The waitable object to check.
     * \return  Returns one of event fired state.
     **/
    int32_t _checkEventFired( WaitablePosix & syncObject );

    /**
     * \brief   Called to notify threads to take fired event ownership.
     * \param   firedEvent  The index of fired event in the list that notifies the threads to take ownership.
     * \return  Returns true if threads are notified or took ownership.
     **/
    bool _requestOwnership( int32_t firedEvent );

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Describes the waiting type. Either should wait for all events or for any.
     **/
    const WaitMode                     mDescribe;
    /**
     * \brief   Describes the lock and wait condition.
     **/
    const areg::os::WaitCondition   mMatchCondition;
    /**
     * \brief   Timeout in milliseconds to wait when blocks the thread.
     **/
    const uint32_t                  mWaitTimeout;
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
    int32_t                             mFiredEntry;
    /**
     * \brief   The list of waitables.
     **/
    WaitingList                         mWaitingList;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    SyncLockAndWaitPosix() = delete;
    AREG_NOCOPY_NOMOVE( SyncLockAndWaitPosix );
};

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SYNCLOCKANDWAITPOSIX_HPP
