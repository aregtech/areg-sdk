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


#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/private/posix/SyncLockAndWaitPosix.hpp"
#include "areg/base/private/posix/WaitablePosix.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include <algorithm>
#include <cstdint>
#include <errno.h>

// --------------------------------------------------------------------------
// Platform-specific wait primitive for the WaitAny path
// --------------------------------------------------------------------------
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

#  if defined(__APPLE__)

#    include <cstdint>
extern "C" {
    int __ulock_wait(uint32_t operation, void* addr, uint64_t value, uint32_t timeout_us);
    int __ulock_wake(uint32_t operation, void* addr, uint64_t wake_value);
}

#  elif defined(__CYGWIN__)

#    ifndef NOMINMAX
#      define NOMINMAX
#    endif
#    include <windows.h>
#    include <climits>

#  else  // Linux

#    include <linux/futex.h>
#    include <sys/syscall.h>
#    include <time.h>
#    include <unistd.h>
#    include <chrono>

#  endif  // defined(__APPLE__) / defined(__CYGWIN__) / Linux

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// Per-thread POSIX sync objects (used by the WaitAll path only)
//
// The WaitAny path (Linux/macOS/Cygwin) sleeps directly on the stack-
// allocated firedWord via futex / __ulock / WaitOnAddress and never
// touches these TLS objects.
//
// The WaitAll path (count>1 && waitAll=true) and all targets without
// futex-class primitives continue to use the TLS mutex+cond.
//////////////////////////////////////////////////////////////////////////

namespace {

struct WaitState
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    bool            valid{ false };

    WaitState() noexcept
    {
        pthread_mutexattr_t attr;
        if (areg::RETURNED_OK == ::pthread_mutexattr_init(&attr))
        {
            ::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
            if (areg::RETURNED_OK == ::pthread_mutex_init(&mutex, &attr))
            {
                pthread_condattr_t condAttr;
                if (areg::RETURNED_OK == ::pthread_condattr_init(&condAttr))
                {
                    ::pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_PRIVATE);
                    valid = (areg::RETURNED_OK == ::pthread_cond_init(&cond, &condAttr));
                    ::pthread_condattr_destroy(&condAttr);
                }

                if (valid == false)
                    ::pthread_mutex_destroy(&mutex);
            }

            ::pthread_mutexattr_destroy(&attr);
        }
    }

    ~WaitState() noexcept
    {
        if (valid)
        {
            ::pthread_cond_destroy(&cond);
            ::pthread_mutex_destroy(&mutex);
            valid = false;
        }
    }
};

thread_local WaitState tls_wait_state;

} // anonymous namespace

//////////////////////////////////////////////////////////////////////////
// WaitAny platform helpers
//////////////////////////////////////////////////////////////////////////

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

// FIRE_INVALID: waiter sleeps while firedWord holds this value.
// Equals (uint32_t)(int32_t)SyncSignal::Invalid = 0xFFFFFFFF.
static constexpr uint32_t FIRE_INVALID {
    static_cast<uint32_t>(areg::os::SyncSignal::Invalid)
};

namespace {

// Compute an absolute CLOCK_REALTIME timespec from a millisecond offset.
// Returns true on success; false if clock_gettime failed.
inline bool _deadline_from_now(timespec& ts, uint32_t msTimeout) noexcept
{
    return (areg::os::POSIX_SUCCESS == ::clock_gettime(CLOCK_REALTIME, &ts))
        && (areg::os::conv_timeout(ts, msTimeout), true);
}

// Sleep until firedWord != FIRE_INVALID, or until the deadline expires.
// Returns FIRE_INVALID on timeout; otherwise firedWord's post-wake value.
#if defined(__linux__)

inline uint32_t _wait_any_sleep(std::atomic<uint32_t>& firedWord, uint32_t msTimeout) noexcept
{
    using Ns = std::chrono::nanoseconds;
    using Ms = std::chrono::milliseconds;
    using Clock = std::chrono::steady_clock;

    const bool     infinite { msTimeout == areg::WAIT_INFINITE };
    const auto     deadline { infinite ? Clock::time_point::max() : Clock::now() + Ms(msTimeout) };

    for (;;)
    {
        uint32_t cur{ firedWord.load(std::memory_order_acquire) };
        if (cur != FIRE_INVALID)
            return cur;

        struct timespec ts {};
        const struct timespec* pts { nullptr };
        if (!infinite)
        {
            const auto now  { Clock::now() };
            if (now >= deadline)
                return FIRE_INVALID;

            const auto rem { std::chrono::duration_cast<Ns>(deadline - now) };
            ts.tv_sec  = static_cast<time_t>(rem.count() / 1'000'000'000LL);
            ts.tv_nsec = static_cast<long>  (rem.count() % 1'000'000'000LL);
            pts = &ts;
        }

        const int rc { static_cast<int>(
            ::syscall(SYS_futex, &firedWord, FUTEX_WAIT_PRIVATE,
                      FIRE_INVALID, pts, nullptr, 0)) };
        (void)rc;
        // On return: re-check firedWord in the loop.
        // ETIMEDOUT -> firedWord still FIRE_INVALID -> next iteration returns FIRE_INVALID.
        // EAGAIN    -> *addr changed before sleep -> next iteration reads new value.
        // EINTR     -> signal interrupted; retry.
    }
}

#elif defined(__APPLE__)

inline uint32_t _wait_any_sleep(std::atomic<uint32_t>& firedWord, uint32_t msTimeout) noexcept
{
    using Us    = std::chrono::microseconds;
    using Ms    = std::chrono::milliseconds;
    using Clock = std::chrono::steady_clock;

    const bool  infinite { msTimeout == areg::WAIT_INFINITE };
    const auto  deadline { infinite ? Clock::time_point::max() : Clock::now() + Ms(msTimeout) };

    for (;;)
    {
        uint32_t cur{ firedWord.load(std::memory_order_acquire) };
        if (cur != FIRE_INVALID)
            return cur;

        uint32_t timeout_us { 0u };  // 0 = infinite for __ulock_wait
        if (!infinite)
        {
            const auto now { Clock::now() };
            if (now >= deadline)
                return FIRE_INVALID;

            const auto rem { std::chrono::duration_cast<Us>(deadline - now) };
            timeout_us = static_cast<uint32_t>(
                std::min<int64_t>(rem.count(), static_cast<int64_t>(UINT32_MAX)));
            if (timeout_us == 0u)
                timeout_us = 1u;  // 0 means infinite; avoid accidental infinite
        }

        const int rc { ::__ulock_wait(areg::os::APPLE_ULOCK_COMPARE_AND_WAIT, &firedWord,
                                      static_cast<uint64_t>(FIRE_INVALID), timeout_us) };
        if (errno == ETIMEDOUT)
            return FIRE_INVALID;
        (void)rc;
    }
}

#else  // Cygwin

inline uint32_t _wait_any_sleep(std::atomic<uint32_t>& firedWord, uint32_t msTimeout) noexcept
{
    const bool     infinite { msTimeout == areg::WAIT_INFINITE };
    const ULONGLONG deadline { infinite ? ULLONG_MAX : GetTickCount64() + static_cast<ULONGLONG>(msTimeout) };

    for (;;)
    {
        uint32_t cur{ firedWord.load(std::memory_order_acquire) };
        if (cur != FIRE_INVALID)
            return cur;

        DWORD remaining { INFINITE };
        if (!infinite)
        {
            const ULONGLONG now { GetTickCount64() };
            if (now >= deadline)
                return FIRE_INVALID;

            remaining = static_cast<DWORD>(
                std::min<ULONGLONG>(deadline - now, static_cast<ULONGLONG>(INFINITE - 1u)));
        }

        uint32_t expected { FIRE_INVALID };
        if (!::WaitOnAddress(&firedWord, &expected, sizeof(expected), remaining))
        {
            if (::GetLastError() == ERROR_TIMEOUT)
                return FIRE_INVALID;
        }
    }
}

#endif  // Linux / macOS / Cygwin

} // anonymous namespace

// WaitAny path: register nodes, early-exit check, sleep, deregister.
int32_t SyncLockAndWaitPosix::_wait_any_new(WaitablePosix** listWaitables, int32_t count, uint32_t msTimeout) noexcept
{
    const pthread_t self{ ::pthread_self() };

    // Stack-allocated fire word and waiter nodes.
    alignas(64) std::atomic<uint32_t> firedWord{ FIRE_INVALID };
    WaiterNode nodes[areg::MAXIMUM_WAITING_OBJECTS];

    // Registration:
    // Register BEFORE checking signaled state.
    for (int32_t i{ 0 }; i < count; ++i)
    {
        nodes[i].mFiredWord  = &firedWord;
        nodes[i].mFiredValue = static_cast<uint32_t>(i);
        nodes[i].mContext    = self;
        nodes[i].mNext       = nullptr;
        listWaitables[i]->register_waiter(&nodes[i]);
    }

    // Early-exit check (already signaled):
    bool ownershipTaken{ false };
    for (int32_t i{ 0 }; i < count; ++i)
    {
        WaitablePosix* w{ listWaitables[i] };
        if (w->check_signaled(self) && w->notify_request_ownership(self))
        {
            uint32_t expected{ FIRE_INVALID };
            if (firedWord.compare_exchange_strong(
                    expected, static_cast<uint32_t>(i),
                    std::memory_order_acq_rel, std::memory_order_relaxed))
            {
                ownershipTaken = true;
                w->notify_released_threads(1);
                break;
            }
            // Another concurrent signal already fired — use that result
        }
    }

    // Sleep if not already fired:
    if (firedWord.load(std::memory_order_acquire) == FIRE_INVALID)
    {
        // Register in the WaitAny map so notify_async_signal() can reach us.
        MapWaitAnyIDResource& mapAny{ SyncLockAndWaitPosix::_map_wait_any_ids() };
        mapAny.register_resource_object(to_num<ptr_type>(self), &firedWord);
        const uint32_t woken{ _wait_any_sleep(firedWord, msTimeout) };
        mapAny.unregister_resource_object(to_num<ptr_type>(self));

        if (woken == FIRE_INVALID)
        {
            // Claim timeout code atomically; if a concurrent signal already fired, the CAS fails — use that result.
            uint32_t expected{ FIRE_INVALID };
            firedWord.compare_exchange_strong(
                expected,
                static_cast<uint32_t>(areg::os::SyncSignal::Timeout),
                std::memory_order_acq_rel,
                std::memory_order_relaxed);
        }
    }

    // Deregistration:
    for (int32_t i{ 0 }; i < count; ++i)
        listWaitables[i]->unregister_waiter(&nodes[i]);

    // Ownership acquisition for the winning waitable:
    // If a waitable fired our node in notify_any_waiters(), ownership was NOT
    // taken there. Take it now.
    // For events, notify_request_ownership() always succeeds.
    // For Mutex, it takes the lock if still free; on failure return the index so the can retry.
    // Auto-reset and notify_released_threads() are handled by event_signaled()
    const uint32_t result{ firedWord.load(std::memory_order_acquire) };
    if (!ownershipTaken && result < static_cast<uint32_t>(areg::MAXIMUM_WAITING_OBJECTS))
    {
        listWaitables[static_cast<int32_t>(result)]->notify_request_ownership(self);
    }

    return static_cast<int32_t>(result);
}

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

//////////////////////////////////////////////////////////////////////////
// SyncLockAndWaitPosix class implementation
//////////////////////////////////////////////////////////////////////////

SyncLockAndWaitPosix::MapWaitIDResource & SyncLockAndWaitPosix::_map_wait_ids() noexcept
{
    static SyncLockAndWaitPosix::MapWaitIDResource _mapWaitIdResource;
    return _mapWaitIdResource;
}

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

SyncLockAndWaitPosix::MapWaitAnyIDResource & SyncLockAndWaitPosix::_map_wait_any_ids() noexcept
{
    static SyncLockAndWaitPosix::MapWaitAnyIDResource _mapWaitAnyIds;
    return _mapWaitAnyIds;
}

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

SyncLockAndWaitPosix::SyncResourceMapIX & SyncLockAndWaitPosix::_map_sync_resources() noexcept
{
    static SyncLockAndWaitPosix::SyncResourceMapIX _theSyncResourceMapIX;
    return _theSyncResourceMapIX;
}

int32_t SyncLockAndWaitPosix::wait_single( WaitablePosix & syncWait, uint32_t msTimeout /* = areg::WAIT_INFINITE */ )
{
    WaitablePosix * list[] = { &syncWait };
    return wait_multiple(list, 1, true, msTimeout);
}

int32_t SyncLockAndWaitPosix::wait_multiple( WaitablePosix ** listWaitables, int32_t count, bool waitAll /* = false */, uint32_t msTimeout /* = areg::WAIT_INFINITE */ )
{
    int32_t result = static_cast<int32_t>(areg::os::SyncSignal::Invalid);
    if ( (listWaitables != nullptr) && (count > 0) )
    {
        count = std::min(areg::MAXIMUM_WAITING_OBJECTS, count);

        // WaitAny path: count=1 OR waitAll=false:
        // Uses stack-allocated WaiterNodes + per-call firedWord + futex/ulock.
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
        if ((count == 1) || !waitAll)
        {
            AREG_OUTPUT_DBG("WaitAny (new path): waiting for [ %d ] event(s).", count);
            return _wait_any_new(listWaitables, count, msTimeout);
        }
#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
        AREG_OUTPUT_DBG("WaitAll (old path): waiting for all [ %d ] events.", count);
        SyncLockAndWaitPosix lockAndWait( listWaitables
                                        , count
                                        , areg::os::WaitCondition::Exact
                                        , msTimeout);

        if ( !lockAndWait._is_empty() && lockAndWait._lock( ) )
        {
            SyncLockAndWaitPosix::MapWaitIDResource & mapWaitIds { SyncLockAndWaitPosix::_map_wait_ids() };
            mapWaitIds.register_resource_object(reinterpret_cast<ptr_type>(lockAndWait.mContext), &lockAndWait);

            int32_t waitResult { ENOLCK };
            while ( lockAndWait._no_event_fired( ) )
            {
                waitResult = lockAndWait._wait_condition( );
                if ( (areg::RETURNED_OK != waitResult) && (lockAndWait.mFiredEntry.load(std::memory_order_acquire) == static_cast<int32_t>(areg::os::SyncSignal::Invalid)) )
                {
                    lockAndWait.mFiredEntry.store( (waitResult == ETIMEDOUT) || (waitResult == EBUSY)
                                                 ? static_cast<int32_t>(areg::os::SyncSignal::Timeout)
                                                 : static_cast<int32_t>(areg::os::SyncSignal::Interrupted)
                                                 , std::memory_order_relaxed );
                    break;
                }
            }

            mapWaitIds.unregister_resource_object(reinterpret_cast<ptr_type>(lockAndWait.mContext));

            lockAndWait._unlock( );
        }

        result = lockAndWait.mFiredEntry.load(std::memory_order_relaxed);
    }

    return result;
}

int32_t SyncLockAndWaitPosix::event_signaled( WaitablePosix & syncWaitable ) noexcept
{
    int32_t result{ 0 };

    // WaitAny path: wake threads registered in the intrusive list
#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    result += syncWaitable.notify_any_waiters();
#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

    // WaitAll path:
    // Only WaitAll waiters (count>1 && waitAll=true) are registered here.
    if (syncWaitable.has_wait_all_waiters())
    {
        SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_map_sync_resources() };
        Lock rcLock(mapResource.lockable());

        ListLockAndWait * waitList = mapResource.resource( &syncWaitable );
        if (waitList != nullptr)
        {
            ASSERT( !waitList->is_empty( ) );
            AREG_OUTPUT_DBG("Waitable [ %s ] ID [ %p ] is signaled (WaitAll path), [ %d ] lock(s) associated."
                            , syncWaitable.name().as_string()
                            , &syncWaitable
                            , waitList->size());

            const ListLockAndWait::LISTPOS end { waitList->invalid_position() };
            for ( ListLockAndWait::LISTPOS pos = waitList->first_position( ); pos != end; )
            {
                SyncLockAndWaitPosix* lockAndWait = waitList->value_at(pos);
                ASSERT(lockAndWait != nullptr);

                if (!syncWaitable.check_signaled(lockAndWait->mContext))
                    break;

                pos = waitList->next_position( pos );
                int32_t fired = lockAndWait->_check_event_fired(syncWaitable);
                if ( fired >= static_cast<int32_t>(areg::os::SyncSignal::First) && fired <= static_cast<int32_t>(areg::os::SyncSignal::All) )
                {
                    if (lockAndWait->_request_ownership(fired))
                    {
                        AREG_OUTPUT_DBG("Waitable [ %s ] [ %p ] fired (WaitAll), waking thread [ %p ] reason [ %d ]"
                                    , syncWaitable.name().as_string()
                                    , &syncWaitable
                                    , lockAndWait->mContext
                                    , fired);

                        ++result;
                        lockAndWait->mFiredEntry.store(fired, std::memory_order_release);
                        lockAndWait->_notify_event();
                    }
                }
            }
        }
    }

    AREG_OUTPUT_DBG("Waitable [ %s ] ID [ %p ] releasing [ %d ] thread(s).",
                    syncWaitable.name().as_string(), &syncWaitable, result);
    syncWaitable.notify_released_threads(result);

    return result;
}

void SyncLockAndWaitPosix::event_remove( WaitablePosix & syncWaitable ) noexcept
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_map_sync_resources() };
    Lock rcLock(mapResource.lockable());

    ListLockAndWait * waitList = mapResource.resource( &syncWaitable );
    if ( waitList == nullptr )
        return;

    AREG_OUTPUT_ERR("Event [ %p / %s ] cleaning resource, notifying error to [ %d ] locked threads."
                    , &syncWaitable
                    , areg::os::as_string(syncWaitable.sync_type())
                    , waitList->size());
    ASSERT( waitList->is_empty( ) == false );

    const ListLockAndWait::LISTPOS end { waitList->invalid_position() };
    for ( ListLockAndWait::LISTPOS pos = waitList->first_position( ); pos != end ; pos = waitList->next_position(pos))
    {
        SyncLockAndWaitPosix * lockAndWait = waitList->value_at(pos);
        ASSERT(lockAndWait != nullptr);
        if (syncWaitable.check_signaled(lockAndWait->mContext) == false)
            break;

        int32_t index = lockAndWait->_waitable_index( syncWaitable );
        lockAndWait->mFiredEntry.store( index != areg::INVALID_INDEX
                                        ? (index + static_cast<int32_t>(areg::os::SyncSignal::FirstError))
                                        : static_cast<int32_t>(areg::os::SyncSignal::Interrupted)
                                        , std::memory_order_release );
        lockAndWait->_notify_event();
    }

    mapResource.unregister_resource(&syncWaitable);
}

void SyncLockAndWaitPosix::event_failed( WaitablePosix & syncWaitable ) noexcept
{
    SyncResourceMapIX & mapResource { SyncLockAndWaitPosix::_map_sync_resources() };
    Lock rcLock(mapResource.lockable());

    ListLockAndWait * waitList = mapResource.resource( &syncWaitable );
    if ( waitList == nullptr )
        return;

    AREG_OUTPUT_WARN("Event [ %p ] failed, notifying error to [ %d ] locked threads.", &syncWaitable, waitList->size());
    ASSERT( waitList->is_empty( ) == false );

    const ListLockAndWait::LISTPOS end { waitList->invalid_position() };
    for ( ListLockAndWait::LISTPOS pos = waitList->first_position( ); pos != end; pos = waitList->next_position(pos))
    {
        SyncLockAndWaitPosix * lockAndWait = waitList->value_at(pos);
        ASSERT(lockAndWait != nullptr);
        if (syncWaitable.check_signaled(lockAndWait->mContext) == false)
            break;

        int32_t index = lockAndWait->_waitable_index( syncWaitable );
        ASSERT(index != areg::INVALID_INDEX);
        lockAndWait->mFiredEntry.store(index + static_cast<int32_t>(areg::os::SyncSignal::FirstError), std::memory_order_release);
        lockAndWait->_notify_event();
    }
}

bool SyncLockAndWaitPosix::is_waitable_registered( WaitablePosix & syncWaitable ) noexcept
{
    SyncResourceMapIX & mapResources { SyncLockAndWaitPosix::_map_sync_resources() };
    return mapResources.exist(&syncWaitable);
}

bool SyncLockAndWaitPosix::notify_async_signal( id_type threadId ) noexcept
{
    const ptr_type key{ static_cast<ptr_type>(threadId) };

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)
    // Check WaitAny map first
    {
        MapWaitAnyIDResource & mapAny{ SyncLockAndWaitPosix::_map_wait_any_ids() };
        Lock waitAny(mapAny.lockable());

        std::atomic<uint32_t>* firedWord{
            mapAny.find_resource_object(key)
        };
        if (firedWord != nullptr)
        {
            uint32_t expected{ FIRE_INVALID };
            if (firedWord->compare_exchange_strong(
                    expected,
                    static_cast<uint32_t>(areg::os::SyncSignal::AsyncSignal),
                    std::memory_order_acq_rel,
                    std::memory_order_relaxed))
            {
                // Wake the sleeping thread.
#  if defined(__linux__)
                ::syscall(SYS_futex, firedWord, FUTEX_WAKE_PRIVATE, 1, nullptr, nullptr, 0);
#  elif defined(__APPLE__)
                ::__ulock_wake(areg::os::APPLE_ULOCK_COMPARE_AND_WAIT, firedWord, 0u);
#  else  // Cygwin
                ::WakeByAddressSingle(firedWord);
#  endif
            }
            return true;  // thread was registered in WaitAny map
        }
    }
#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

    // Fallback: WaitAll map
    {
        SyncLockAndWaitPosix::MapWaitIDResource & mapWaitIds { SyncLockAndWaitPosix::_map_wait_ids() };
        Lock wait(mapWaitIds.lockable());

        SyncLockAndWaitPosix * lockAndWait = mapWaitIds.find_resource_object(key);
        if (lockAndWait != nullptr)
        {
            lockAndWait->mFiredEntry.store(static_cast<int32_t>(areg::os::SyncSignal::AsyncSignal), std::memory_order_release);
            return lockAndWait->_notify_event();
        }
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
// WaitAll path: SyncLockAndWaitPosix constructor / destructor
//////////////////////////////////////////////////////////////////////////

SyncLockAndWaitPosix::SyncLockAndWaitPosix(   WaitablePosix ** listWaitables
                                            , int32_t count
                                            , areg::os::WaitCondition matchCondition
                                            , uint32_t msTimeout )
    : mDescribe         ( count > 1 ? SyncLockAndWaitPosix::WaitMode::Multiple : SyncLockAndWaitPosix::WaitMode::Single )
    , mMatchCondition   ( matchCondition )
    , mWaitTimeout      ( msTimeout )
    , mDeadline         ( { 0, 0 } )
    , mContext          ( pthread_self() )
    , mMutexPtr         ( tls_wait_state.valid ? &tls_wait_state.mutex : nullptr )
    , mCondPtr          ( tls_wait_state.valid ? &tls_wait_state.cond  : nullptr )
    , mSyncValid        ( tls_wait_state.valid )
    , mFiredEntry       ( static_cast<int32_t>(areg::os::SyncSignal::Invalid) )
    , mWaitingList      ( count )
{
    ASSERT( listWaitables  != nullptr);

    if ( msTimeout != areg::WAIT_INFINITE )
    {
        areg::os::timeout_from_now( mDeadline, msTimeout );
    }

    if ( mSyncValid )
    {
        SyncResourceMapIX & mapResources { SyncLockAndWaitPosix::_map_sync_resources() };
        mapResources.lock();
        count = std::min(areg::MAXIMUM_WAITING_OBJECTS, count);

        bool eventFired{ true };
        for ( uint32_t i = 0; i < static_cast<uint32_t>(count); ++i, ++listWaitables )
        {
            WaitablePosix * syncWaitable = *listWaitables;
            if ( syncWaitable != nullptr )
            {
                mapResources.register_resource_object(syncWaitable, this);
                if ( eventFired && !syncWaitable->check_signaled(mContext) )
                    eventFired = false;

                mWaitingList[i] = syncWaitable;
                syncWaitable->add_wait_all_registration();
            }
            else
            {
                mFiredEntry.store(static_cast<int32_t>(i + static_cast<int32_t>(areg::os::SyncSignal::FirstError)), std::memory_order_relaxed);
                mWaitingList.resize(i);
                break;
            }
        }

        if (eventFired && _request_ownership(static_cast<int32_t>(areg::os::SyncSignal::All)))
        {
            mFiredEntry.store(static_cast<int32_t>(areg::os::SyncSignal::All), std::memory_order_relaxed);
            for (uint32_t i = 0; i < mWaitingList.size(); ++i)
            {
                mWaitingList[i]->notify_released_threads(1);
            }
        }

        mapResources.unlock();
    }
}

SyncLockAndWaitPosix::~SyncLockAndWaitPosix()
{
    // Remove from global map first, then decrement per-waitable counters.
    SyncLockAndWaitPosix::_map_sync_resources().remove_resource_object(this, true);
    for (uint32_t i = 0; i < mWaitingList.size(); ++i)
    {
        if (mWaitingList[i] != nullptr)
        {
            mWaitingList[i]->remove_wait_all_registration();
        }
    }
}

inline bool SyncLockAndWaitPosix::_no_event_fired() const noexcept
{
    return (mFiredEntry.load(std::memory_order_acquire) == static_cast<int32_t>(areg::os::SyncSignal::Invalid));
}

inline bool SyncLockAndWaitPosix::_is_empty() const noexcept
{
    return mWaitingList.is_empty();
}

inline bool SyncLockAndWaitPosix::_notify_event() noexcept
{
    if (!mSyncValid)
        return false;

    ::pthread_mutex_lock(mMutexPtr);
    const bool result = (areg::RETURNED_OK == ::pthread_cond_signal(mCondPtr));
    ::pthread_mutex_unlock(mMutexPtr);
    return result;
}

inline bool SyncLockAndWaitPosix::_is_valid() const noexcept
{
    return (mSyncValid && (mWaitingList.is_empty() == false));
}

inline bool SyncLockAndWaitPosix::_lock() noexcept
{
    return (mSyncValid && (areg::RETURNED_OK == ::pthread_mutex_lock(mMutexPtr)));
}

inline void SyncLockAndWaitPosix::_unlock() noexcept
{
    if (mSyncValid)
    {
        ::pthread_mutex_unlock(mMutexPtr);
    }
}

inline int32_t SyncLockAndWaitPosix::_wait_condition()
{
    if ( mWaitTimeout == areg::WAIT_INFINITE )
    {
        return ::pthread_cond_wait( mCondPtr, mMutexPtr );
    }
    else
    {
        return ::pthread_cond_timedwait( mCondPtr, mMutexPtr, &mDeadline );
    }
}

inline int32_t SyncLockAndWaitPosix::_waitable_index( const WaitablePosix & syncWaitable ) const noexcept
{
    for ( uint32_t i = 0; i < mWaitingList.size(); ++ i )
    {
        if (mWaitingList[i] == &syncWaitable)
        {
            return i;
        }
    }

    return areg::INVALID_INDEX;
}

int32_t SyncLockAndWaitPosix::SyncLockAndWaitPosix::_check_event_fired( WaitablePosix & syncObject ) noexcept
{
    int32_t result = static_cast<int32_t>(areg::os::SyncSignal::Invalid);

    if (syncObject.check_signaled(mContext) && (mWaitingList.size() != 0) && (mFiredEntry.load(std::memory_order_relaxed) == static_cast<int32_t>(areg::os::SyncSignal::Invalid)))
    {
        uint32_t i = 0;
        for ( ; i < mWaitingList.size(); ++i)
        {
            WaitablePosix * waitable = mWaitingList.value_at(i);
            ASSERT(waitable != nullptr);
            if (!waitable->check_signaled(mContext))
                break;
        }

        if (i == mWaitingList.size())
        {
            result = static_cast<int32_t>(areg::os::SyncSignal::All);
        }
    }

    return result;
}

bool SyncLockAndWaitPosix::_request_ownership( int32_t firedEvent ) noexcept
{
    bool result = false;

    ASSERT(firedEvent >= static_cast<int32_t>(areg::os::SyncSignal::First) && firedEvent <= static_cast<int32_t>(areg::os::SyncSignal::All));
    if ( firedEvent != static_cast<int32_t>(areg::os::SyncSignal::All) )
    {
        ASSERT(mWaitingList.size() > static_cast<uint32_t>(firedEvent));
        WaitablePosix *waitable = mWaitingList[firedEvent];
        ASSERT(waitable != nullptr);

        result = waitable->notify_request_ownership(mContext);
    }
    else
    {
        result = true;
        for (uint32_t i = 0; (i < mWaitingList.size()) && result; ++ i)
        {
            WaitablePosix *waitable = mWaitingList[i];
            ASSERT(waitable != nullptr);
            result = waitable->notify_request_ownership(mContext);
        }
    }

    return result;
}

} // namespace areg::os
#endif // defined(_POSIX) || defined(POSIX)
