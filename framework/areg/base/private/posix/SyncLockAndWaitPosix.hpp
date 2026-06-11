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
#include <atomic>
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
#include "areg/base/private/posix/WaitAnyRegistry.hpp"

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
 *          areg::MAXIMUM_WAITING_OBJECTS. Use static methods for waiting; internal methods are hidden.
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
        inline void impl_clean_list( WaitablePosix * & /* Key */, ListLockAndWait & List ) noexcept
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
        inline bool impl_remove_resource( ListLockAndWait & List, SyncLockAndWaitPosix * Resource ) noexcept
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
// Resource maps keyed by thread ID (for notify_async_signal)
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Map for WaitAll path
     **/
    using MapWaitID          = IdHashMap<SyncLockAndWaitPosix *>;
    using ImplWaitIDResource = ResourceMapImpl<ptr_type, SyncLockAndWaitPosix *>;
    using MapWaitIDResource  = ConcurrentResourceMap<ptr_type, SyncLockAndWaitPosix *, MapWaitID, ImplWaitIDResource>;

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    /**
     * \brief   Lock-free registry for the WaitAny path
     **/
    using WaitAnyRegistryMap = WaitAnyRegistry<256>;
#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

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
     * \param   listWaitables       The list of waitables to check the signaled state. No more than areg::MAXIMUM_WAITING_OBJECTS entries.
     * \param   count               The number of waitables in the list. No more than areg::MAXIMUM_WAITING_OBJECTS.
     * \param   waitAll             If true, waits for all waitables to be signaled. If false, any signaled waitable unlocks.
     * \param   msTimeout           Timeout in milliseconds. areg::WAIT_INFINITE to wait indefinitely.
     * \return  Returns: SyncSignal::First + N (if waitAll=false and object N is signaled),
     *          SyncSignal::All (if waitAll=true and all signaled), SyncSignal::Timeout (expired),
     *          SyncSignal::Interrupted (interrupted), or SyncSignal::FirstError + N (error on object N).
     **/
    static int32_t wait_multiple( WaitablePosix ** listWaitables, int32_t count, bool waitAll = false, uint32_t msTimeout = areg::WAIT_INFINITE);

    /**
     * \brief   Called by waitable object to indicate it is in signaled state.
     *
     * \param   syncWaitable    The waitable object that is in signaled state.
     * \return  Returns the number of threads notified.
     **/
    static int32_t event_signaled( WaitablePosix & syncWaitable ) noexcept;

    /**
     * \brief   Called by waitable object to indicate failure (e.g., invalidation). Unlocks all
     *          waiting threads with error.
     *
     * \param   syncWaitable    The waitable object that failed.
     **/
    static void event_failed( WaitablePosix & syncWaitable ) noexcept;

    /**
     * \brief   Removes waitable object from the waiting list.
     *
     * \param   syncWaitable    The waitable object to remove from the list.
     **/
    static void event_remove( WaitablePosix & syncWaitable ) noexcept;

    /**
     * \brief   Checks whether the waitable is registered in the waiting list.
     *
     * \param   syncWaitable    Waitable to check the registration.
     * \return  Returns true if waitable is registered.
     **/
    [[nodiscard]]
    static inline bool is_waitable_registered( WaitablePosix & syncWaitable ) noexcept;

    /**
     * \brief   Breaks waiting in a locked thread to process asynchronous execution. Thread can lock
     *          again after finishing.
     *
     * \param   threadId    The ID of the thread to interrupt.
     * \return  Returns true if operation succeeded; false if thread is not locked.
     **/
    static bool notify_async_signal( id_type threadId ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Initializes the object, sets flags, and checks signaled state of waitables.
     *
     * \param   listWaitables       The list of waitables. Maximum areg::MAXIMUM_WAITING_OBJECTS entries
     * \param   count               The number of waitables in the list. Maximum areg::MAXIMUM_WAITING_OBJECTS
     * \param   matchCondition      Specifies wait criteria: exact match (all signaled) or any one signaled
     * \param   msTimeout           Timeout in milliseconds to wait.
     **/
    SyncLockAndWaitPosix( WaitablePosix ** listWaitables, int32_t count, areg::os::WaitCondition matchCondition, uint32_t msTimeout );

    ~SyncLockAndWaitPosix();

//////////////////////////////////////////////////////////////////////////
// Hidden operations and attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns static map of thread-ID.
     **/
    static SyncLockAndWaitPosix::MapWaitIDResource& _map_wait_ids() noexcept;

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    /**
     * \brief   Returns static lock-free WaitAny registry (thread-ID -> firedWord pointer).
     **/
    static SyncLockAndWaitPosix::WaitAnyRegistryMap& _map_wait_any_ids() noexcept;

    /**
     * \brief   WaitAny implementation: stack-allocates WaiterNodes, registers them in each
     *          waitable's intrusive list, sleeps on a per-call fire word via the platform
     *          primitive (futex / __ulock / WaitOnAddress), and returns the fired result code.
     *
     * \param   listWaitables   Array of waitables to watch.
     * \param   count           Number of entries in listWaitables.
     * \param   msTimeout       Timeout in milliseconds; areg::WAIT_INFINITE to wait forever.
     * \return  Returns the value stored in firedWord on wakeup (fired index, timeout code, etc.).
     **/
    static int32_t _wait_any_new( WaitablePosix** listWaitables, int32_t count, uint32_t msTimeout ) noexcept;
#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

    /**
     * \brief   Returns static instance of synchronization resource map (WaitAll).
     **/
    static SyncResourceMapIX& _map_sync_resources() noexcept;

    /**
     * \brief   Returns true if no event in the list has fired.
     **/
    inline bool _no_event_fired() const noexcept;

    /**
     * \brief   Returns true if object is valid (TLS sync objects initialized and waiting list not empty)
     **/
    [[nodiscard]]
    inline bool _is_valid() const noexcept;

    /**
     * \brief   Locks the object and waits for event fired criteria. May block the calling thread.
     *
     * \return  Returns true if locking succeeded.
     **/
    inline bool _lock() noexcept;

    /**
     * \brief   Unlocks the object.
     **/
    inline void _unlock() noexcept;

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
    inline int32_t _waitable_index( const WaitablePosix & syncWaitable ) const noexcept;

    /**
     * \brief   Notifies that an event has fired.
     *
     * \return  Returns true if notification succeeded.
     **/
    inline bool _notify_event() noexcept;

    /**
     * \brief   Returns true if waiting list is empty.
     **/
    [[nodiscard]]
    inline bool _is_empty() const noexcept;

    /**
     * \brief   Checks whether the waitable event has fired.
     *
     * \param   syncObject      The waitable object to check.
     * \return  Returns one of event fired state values.
     **/
    int32_t _check_event_fired( WaitablePosix & syncObject ) noexcept;

    /**
     * \brief   Notifies threads to take ownership of a fired event.
     *
     * \param   firedEvent      The index of fired event in the list.
     * \return  Returns true if threads are notified or took ownership.
     **/
    bool _request_ownership( int32_t firedEvent ) noexcept;

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
     * \brief   Pointer to the per-thread mutex (thread-local storage, owned by the calling
     *          thread, initialized once for the thread's lifetime).  Non-owning pointer;
     *          the mutex is never destroyed by this object.
     **/
    mutable pthread_mutex_t*        mMutexPtr;
    /**
     * \brief   Pointer to the per-thread condition variable (thread-local storage).
     *          Non-owning pointer; the cond var is never destroyed by this object.
     **/
    pthread_cond_t*                 mCondPtr;
    /**
     * \brief   True when both mMutexPtr and mCondPtr are valid (TLS was successfully
     *          initialized for the calling thread).
     **/
    bool                            mSyncValid;
    /**
     * \brief   Indicates the fired event object or error code. Written by the signaler
     *          thread and read by the waiter thread; must be atomic to prevent compiler
     *          register-caching across pthread_cond_wait on ARM64 / Apple Silicon.
     **/
    std::atomic<int32_t>            mFiredEntry;
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

inline bool SyncLockAndWaitPosix::is_waitable_registered(WaitablePosix& syncWaitable) noexcept
{
    SyncResourceMapIX& mapResources{ SyncLockAndWaitPosix::_map_sync_resources() };
    return mapResources.exist(&syncWaitable);
}

} // namespace areg::os

#endif  // defined(_POSIX) || defined(POSIX)

#endif  // AREG_BASE_PRIVATE_POSIX_SYNCLOCKANDWAITPOSIX_HPP
