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
#include "areg/base/areg_global.h"
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
namespace areg::os {

 /************************************************************************
  * dependencies.
  ************************************************************************/
class WaitablePosix;
class SyncLockAndWaitPosix;

//////////////////////////////////////////////////////////////////////////
// SyncLockAndWaitPosix class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX-based synchronization object for locking and waiting on single or multiple
 *          waitable objects. Supports waiting criteria based on flags, with a maximum of
 *          areg::MAXIMUM_WAITING_OBJECTS. Use static methods for waiting; internal methods are
 *          hidden.
 **/
class SyncLockAndWaitPosix
{
    /**
     * \brief   The list of LockAndWait objects.
     **/
    using ListLockAndWait       = LinkedList<SyncLockAndWaitPosix *>;
    /**
     * \brief   The hash map container of waitable object and LockAndWait lists.
     **/
    using MapLockAndWait        = OrderedMap<WaitablePosix *, ListLockAndWait>;

//////////////////////////////////////////////////////////////////////////
// ImplResourceListMap class declaration
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Helper implementation of resource list map providing customizable operations for
     *          WaitablePosix and SyncLockAndWaitPosix types.
     **/
    class ImplResourceListMap : public ResourceListMapImpl<WaitablePosix *, SyncLockAndWaitPosix, ListLockAndWait>
    {
    public:
        /**
         * \brief   Called when removing all resources. Implement to perform cleanup on the list.
         *
         * \param   Key     The key associated with the resource list (unused).
         * \param   List    The list of resource objects.
         **/
        inline void impl_clean_list( WaitablePosix * & /* Key */, ListLockAndWait & List )
        {
        	List.clear();
        }

        /**
         * \brief   Called when adding a resource to the list.
         *
         * \param   List        The list of resource objects.
         * \param   Resource    The resource object to add.
         **/
        inline void impl_add_resource( ListLockAndWait & List, SyncLockAndWaitPosix * Resource )
        {
        	List.push_last(Resource);
        }

        /**
         * \brief   Called when removing a resource from the list.
         *
         * \param   List        The list of resource objects.
         * \param   Resource    The resource object to remove.
         **/
        inline bool impl_remove_resource( ListLockAndWait & List, SyncLockAndWaitPosix * Resource )
        {
        	return List.remove_entry( Resource );
        }
    };

    /**
     * \brief   The resource list map of waitable object where the keys are waitable objects
     *          and the resource objects are WaitAndLock objects in the list. The WaitAndLock
     *          objects in the entire map are not unique, but should be unique in the list.
     **/
    using SyncResourceMapIX = ConcurrentResourceListMap<WaitablePosix *, SyncLockAndWaitPosix *, ListLockAndWait, MapLockAndWait, ImplResourceListMap>;

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
    using ImplWaitIDResource= ResourceMapImpl<ptr_type, SyncLockAndWaitPosix *>;
    /**
     * \brief   Resource map of waitable where the keys are pthread_t (thread ID) and the values are
     *          LockAndWait objects. It is used in the timer.
     **/
    using MapWaitIDResource = ConcurrentResourceMap<ptr_type, SyncLockAndWaitPosix *, MapWaitID, ImplWaitIDResource>;

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
     * \brief   Locks the synchronization object or waits until it is released. Returns immediately
     *          if already signaled.
     *
     * \param   syncWait        The waitable object to check the signaled state.
     * \param   msTimeout       Timeout in milliseconds. areg::WAIT_INFINITE to wait
     *                          indefinitely.
     * \return  Returns one of: SyncSignal::First (signaled), SyncSignal::Timeout (expired),
     *          SyncSignal::Interrupted (interrupted), or SyncSignal::FirstError (error).
     **/
    static int32_t wait_single( WaitablePosix & syncWait, uint32_t msTimeout = areg::WAIT_INFINITE );

    /**
     * \brief   Locks and waits for a list of synchronization objects until one or all are signaled.
     *          Maximum areg::MAXIMUM_WAITING_OBJECTS entries.
     *
     * \param   listWaitables       The list of waitables to check the signaled state. No more than
     *                              areg::MAXIMUM_WAITING_OBJECTS entries.
     * \param   count               The number of waitables in the list. No more than
     *                              areg::MAXIMUM_WAITING_OBJECTS.
     * \param   waitAll             If true, waits for all waitables to be signaled. If false, any
     *                              signaled waitable unlocks.
     * \param   msTimeout           Timeout in milliseconds. areg::WAIT_INFINITE to wait
     *                              indefinitely.
     * \return  Returns: SyncSignal::First + N (if waitAll=false and object N is signaled),
     *          SyncSignal::All (if waitAll=true and all signaled), SyncSignal::Timeout (expired),
     *          SyncSignal::Interrupted (interrupted), or SyncSignal::FirstError + N (error on
     *          object N).
     **/
    static int32_t wait_multiple( WaitablePosix ** listWaitables, int32_t count, bool waitAll = false, uint32_t msTimeout = areg::WAIT_INFINITE);

    /**
     * \brief   Called by waitable object to indicate it is in signaled state.
     *
     * \param   syncWaitable    The waitable object that is in signaled state.
     * \return  Returns the number of threads notified.
     **/
    static int32_t event_signaled( WaitablePosix & syncWaitable );

    /**
     * \brief   Called by waitable object to indicate failure (e.g., invalidation). Unlocks all
     *          waiting threads with error.
     *
     * \param   syncWaitable    The waitable object that failed.
     **/
    static void event_failed( WaitablePosix & syncWaitable );

    /**
     * \brief   Removes waitable object from the waiting list.
     *
     * \param   syncWaitable    The waitable object to remove from the list.
     **/
    static void event_remove( WaitablePosix & syncWaitable );

    /**
     * \brief   Checks whether the waitable is registered in the waiting list.
     *
     * \param   syncWaitable    Waitable to check the registration.
     * \return  Returns true if waitable is registered.
     **/
    [[nodiscard]]
    static bool is_waitable_registered( WaitablePosix & syncWaitable );

    /**
     * \brief   Breaks waiting in a locked thread to process asynchronous execution. Thread can lock
     *          again after finishing.
     *
     * \param   threadId    The ID of the thread to interrupt.
     * \return  Returns true if operation succeeded; false if thread is not locked.
     **/
    static bool notify_async_signal( id_type threadId );

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes the object, sets flags, and checks signaled state of waitables.
     *
     * \param   listWaitables       The list of waitables. Maximum areg::MAXIMUM_WAITING_OBJECTS
     *                              entries.
     * \param   count               The number of waitables in the list. Maximum
     *                              areg::MAXIMUM_WAITING_OBJECTS.
     * \param   matchCondition      Specifies wait criteria: exact match (all signaled) or any one
     *                              signaled.
     * \param   msTimeout           Timeout in milliseconds to wait.
     **/
    SyncLockAndWaitPosix( WaitablePosix ** listWaitables, int32_t count, areg::os::WaitCondition matchCondition, uint32_t msTimeout );

    ~SyncLockAndWaitPosix();

//////////////////////////////////////////////////////////////////////////
// Hidden operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns static map of waitables keyed by id_type.
     **/
    static SyncLockAndWaitPosix::MapWaitIDResource& _map_wait_ids();

    /**
     * \brief   Returns static instance of synchronization resource map.
     **/
    static SyncResourceMapIX& _map_sync_resources();

    /**
     * \brief   Returns true if no event in the list has fired.
     **/
    inline bool _no_event_fired() const;

    /**
     * \brief   Initializes internal POSIX objects. Returns true if initialization succeeded.
     **/
    inline bool _init_sync_objects();

    /**
     * \brief   Releases internal POSIX objects to free resources.
     **/
    inline void _release_sync_objects();

    /**
     * \brief   Returns true if object is valid (POSIX objects initialized and waiting list not
     *          empty).
     **/
    [[nodiscard]]
    inline bool _is_valid() const;

    /**
     * \brief   Locks the object and waits for event fired criteria. May block the calling thread.
     *
     * \return  Returns true if locking succeeded.
     **/
    inline bool _lock();

    /**
     * \brief   Unlocks the object.
     **/
    inline void _unlock();

    /**
     * \brief   Waits for condition variable with infinite or timeout-based wait.
     *
     * \return  Returns POSIX error code; 0 indicates success.
     **/
    inline int32_t _wait_condition();

    /**
     * \brief   Returns the index of registered waitable in the list.
     *
     * \param   syncWaitable    The instance of waitable object to lookup in the list.
     **/
    inline int32_t _waitable_index( const WaitablePosix & syncWaitable ) const;

    /**
     * \brief   Notifies that an event has fired.
     *
     * \return  Returns true if notification succeeded.
     **/
    inline bool _notify_event();

    /**
     * \brief   Returns true if waiting list is empty.
     **/
    [[nodiscard]]
    inline bool _is_empty() const;

    /**
     * \brief   Checks whether the waitable event has fired.
     *
     * \param   syncObject      The waitable object to check.
     * \return  Returns one of event fired state values.
     **/
    int32_t _check_event_fired( WaitablePosix & syncObject );

    /**
     * \brief   Notifies threads to take ownership of a fired event.
     *
     * \param   firedEvent      The index of fired event in the list.
     * \return  Returns true if threads are notified or took ownership.
     **/
    bool _request_ownership( int32_t firedEvent );

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Describes the waiting type. Either should wait for all events or for any.
     **/
    const WaitMode                  mDescribe;
    /**
     * \brief   Describes the lock and wait condition.
     **/
    const areg::os::WaitCondition   mMatchCondition;
    /**
     * \brief   Timeout in milliseconds to wait when blocks the thread.
     **/
    const uint32_t                  mWaitTimeout;
    /**
     * \brief   Absolute deadline computed once at construction for timed waits. Avoids
     *          resetting the deadline on spurious condition variable wakeups.
     **/
    timespec                        mDeadline;
    /**
     * \brief   The ID of thread that instantiated LockAndWait object.
     **/
    const pthread_t                 mContext;
    /**
     * \brief   Internal POSIX mutex object to synchronize data access.
     **/
    mutable pthread_mutex_t         mPosixMutex;
    /**
     * \brief   The POSIX mutex validity flag.
     **/
    mutable bool                    mMutexValid;
    /**
     * \brief   Internal POSIX mutex attribute to initialize mutex.
     **/
    mutable pthread_mutexattr_t     mPosixMutexAttr;
    /**
     * \brief   The POSIX mutex attribute validity flag.
     **/
    mutable bool                    mMutexAttrValid;
    /**
     * \brief   Internal POSIX conditional variable.
     **/
    pthread_cond_t                  mCondVariable;
    /**
     * \brief   The POSIX conditional variable validity flag.
     **/
    bool                            mCondVarValid;
    /**
     * \brief   Internal POSIX conditional variable attribute
     **/
    pthread_condattr_t              mCondAttribute;
    /**
     * \brief   The POSIX conditional variable attribute validity flag.
     **/
    bool                            mCondAttrValid;
    /**
     * \brief   Indicates the fired event object or error code.
     **/
    int32_t                         mFiredEntry;
    /**
     * \brief   The list of waitables.
     **/
    WaitingList                     mWaitingList;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    SyncLockAndWaitPosix() = delete;
    AREG_NOCOPY_NOMOVE( SyncLockAndWaitPosix );
};

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SYNCLOCKANDWAITPOSIX_HPP
