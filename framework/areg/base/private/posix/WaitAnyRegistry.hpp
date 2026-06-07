#ifndef AREG_BASE_PRIVATE_POSIX_WAITANYREGISTRY_HPP
#define AREG_BASE_PRIVATE_POSIX_WAITANYREGISTRY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/WaitAnyRegistry.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, lock-free hash map for the WaitAny thread registry.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

#include "areg/base/areg_types.h"
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace areg::os {

//////////////////////////////////////////////////////////////////////////
// WaitAnyRegistry<Capacity> class template declaration
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Lock-free open-addressing hash map that maps a thread ID (ptr_type key)
 *          to the pointer to the per-call firedWord atomic (std::atomic<uint32_t>*).
 *
 *          Used exclusively by the WaitAny path in SyncLockAndWaitPosix:
 *            - register_resource_object() : called by the SLEEPING thread before
 *              entering OS wait, with its own thread ID and a pointer to its
 *              stack-local firedWord.
 *            - unregister_resource_object(): called by the SAME thread after waking.
 *            - find_resource_object()      : called by ANY thread (notify_async_signal)
 *              to retrieve the firedWord pointer for a given thread ID.
 *
 *          Algorithm
 *          ---------
 *          Each slot holds two independent word-sized atomics: mKey (ptr_type) and
 *          mValue (std::atomic<uint32_t>*). Both are natively lock-free on all
 *          supported platforms (x86-64, ARM64, Cygwin/x64) without requiring
 *          platform-specific intrinsics.
 *
 *          Collision resolution uses linear probing with tombstone markers:
 *            - EMPTY_KEY (0)      : slot is free; probe chain terminates here.
 *            - TOMBSTONE_KEY (~0) : slot was occupied and deleted; probing continues.
 *
 *          Publish ordering
 *          ---------
 *          Registration is a two-step publish: first CAS the key, then release-store
 *          the value. Because the CAS winner is the only writer of the value, there
 *          is no overwrite hazard. A concurrent find() that observes the key but
 *          sees a null value spins briefly (bounded spin, typically 0-1 iterations)
 *          until the value store is visible or the key is tombstoned. In practice
 *          this window is 2-4 CPU instructions, closed before the thread reaches
 *          the OS wait call.
 *
 *          Unregistration reverses the order: clear the value first (release), then
 *          tombstone the key (release). A find() that observes a matching key with
 *          null value during this window will spin until the key is tombstoned, then
 *          exit the spin and return nullptr — correct, because the thread is awake.
 *
 *          Thread-safety
 *          ---------
 *          All state transitions use release-acquire ordering. There is no global
 *          mutex on any code path. All operations are wait-free except the brief
 *          bounded spin in find() that resolves the publish/unpublish window.
 *
 *          Capacity notes
 *          ---------
 *          Capacity must be a power of 2. Choose at least 4× the peak number of
 *          simultaneously registered threads (keeps load factor ≤ 25%). The default
 *          of 256 supports up to ~64 concurrently blocking threads with near-O(1)
 *          average probe length.
 *
 * \tparam  Capacity    Number of hash table slots. Must be a power of 2. Default: 256.
 **/
template<size_t Capacity = 256>
class WaitAnyRegistry
{
    static_assert((Capacity & (Capacity - 1u)) == 0u, "WaitAnyRegistry: Capacity must be a power of 2.");
    static_assert(Capacity >= 16u, "WaitAnyRegistry: Capacity must be at least 16.");

//////////////////////////////////////////////////////////////////////////
// Internal types
//////////////////////////////////////////////////////////////////////////
private:

    //! Sentinel: slot is empty; probe chains end here.
    static constexpr ptr_type EMPTY_KEY     { 0u };
    //! Sentinel: slot was deleted; probe chains skip over it.
    static constexpr ptr_type TOMBSTONE_KEY { static_cast<ptr_type>(-1) };

    /**
     * \brief   One hash-table slot: independent word-sized atomics for key and value.
     *
     *          Key and value are published in two separate stores; find() uses a
     *          bounded spin to bridge the narrow window between them. Both atomics
     *          are pointer-sized and lock-free on all supported platforms.
     **/
    struct Slot
    {
        //! Thread-ID key. 0 = EMPTY, ~0 = TOMBSTONE, otherwise a valid thread ID.
        std::atomic<ptr_type>               mKey   { 0u };
        //! Pointer to the thread's stack-local firedWord. null until published.
        std::atomic<std::atomic<uint32_t>*> mValue { nullptr };
    };

    static_assert(std::atomic<ptr_type>::is_always_lock_free,
                  "WaitAnyRegistry: ptr_type atomic must be lock-free.");
    static_assert(std::atomic<std::atomic<uint32_t>*>::is_always_lock_free,
                  "WaitAnyRegistry: pointer atomic must be lock-free.");

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //! The hash table; default-initialized to EMPTY slots.
    Slot mTable[Capacity];

//////////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Maps a thread-ID key to an initial probe index.
     *
     *          Uses a multiplicative (Fibonacci) hash to scatter aligned-pointer keys
     *          (on macOS pthread_t is a heap pointer with low bits == 0; a plain modulo
     *          would produce poor distribution). The bit-mix disperses low-bit entropy
     *          across the full index range before masking.
     **/
    static size_t _probe_start(ptr_type key) noexcept
    {
        if constexpr (sizeof(ptr_type) == 8u)
        {
            uint64_t h = static_cast<uint64_t>(key);
            h ^= h >> 30u;
            h *= 11400714819323198485ULL;  // 2^64 / phi  (Fibonacci / golden-ratio constant)
            h ^= h >> 27u;
            return static_cast<size_t>(h) & (Capacity - 1u);
        }
        else
        {
            uint32_t h = static_cast<uint32_t>(key);
            h ^= h >> 16u;
            h *= 0x45d9f3bu;             // 32-bit mixing constant
            h ^= h >> 16u;
            return static_cast<size_t>(h) & (Capacity - 1u);
        }
    }

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Registers a thread's firedWord pointer under its thread-ID key.
     *
     *          Called ONLY by the registering thread, before entering OS wait.
     *          Performs a CAS on mKey to claim the first available slot, then stores
     *          the value with release ordering. The two-step publish is safe because
     *          only the CAS winner writes the value — there is no overwrite hazard.
     *
     * \param   key     Thread ID (from to_num<ptr_type>(pthread_self())).
     * \param   value   Pointer to the caller's stack-local firedWord atomic.
     **/
    void register_resource_object(ptr_type key, std::atomic<uint32_t>* value) noexcept
    {
        const size_t start { _probe_start(key) };

        for (size_t probe = 0u; probe < Capacity; ++probe)
        {
            Slot& slot { mTable[(start + probe) & (Capacity - 1u)] };
            ptr_type current { slot.mKey.load(std::memory_order_relaxed) };

            if ((current == EMPTY_KEY) || (current == TOMBSTONE_KEY))
            {
                if (slot.mKey.compare_exchange_strong(current, key,
                        std::memory_order_acq_rel, std::memory_order_relaxed))
                {
                    // CAS succeeded: we own this slot. Publish the value.
                    slot.mValue.store(value, std::memory_order_release);
                    return;
                }
                // CAS failed: another thread claimed this slot; probe next
            }
            // occupied by a different key: probe next
        }

        // Should never reach here if Capacity >> concurrent thread count
        ASSERT(false);
    }

    /**
     * \brief   Removes the entry for the given key and marks its slot as TOMBSTONE so
     *          that probe chains from other threads remain intact.
     *
     *          Called ONLY by the same thread that called register_resource_object().
     *          Clears the value first (release) then tombstones the key (release) so
     *          that a concurrent find() observes a consistent null value once the key disappears.
     *
     * \param   key     The same thread ID passed to register_resource_object().
     **/
    void unregister_resource_object(ptr_type key) noexcept
    {
        const size_t start { _probe_start(key) };

        for (size_t probe = 0u; probe < Capacity; ++probe)
        {
            Slot& slot { mTable[(start + probe) & (Capacity - 1u)] };
            const ptr_type k { slot.mKey.load(std::memory_order_relaxed) };

            if (k == key)
            {
                // Clear value first so find() returns nullptr after the key is tombstoned
                slot.mValue.store(nullptr, std::memory_order_release);
                slot.mKey.store(TOMBSTONE_KEY, std::memory_order_release);
                return;
            }

            if (k == EMPTY_KEY)
                return;  // key not in table — should not happen
        }
    }

    /**
     * \brief   Looks up the firedWord pointer for the given thread-ID key.
     *
     *          Called from notify_async_signal() by any thread. Lock-free.
     *
     *          There is a narrow window in which a valid key is observed but the value
     *          has not yet been stored (the 2-4 instruction gap between CAS(key) and
     *          store(value) in register_resource_object). This function resolves the
     *          window with a bounded spin: it re-reads the key and value until either
     *          the value appears or the key changes (indicating unregistration).
     *
     *          In practice the spin exits after 0-1 iterations because the window
     *          is closed before the registering thread reaches the OS wait call.
     *
     * \param   key     Thread ID to locate.
     * \return  Pointer to the firedWord atomic, or nullptr if not registered.
     **/
    [[nodiscard]]
    std::atomic<uint32_t>* find_resource_object(ptr_type key) const noexcept
    {
        const size_t start { _probe_start(key) };

        for (size_t probe = 0u; probe < Capacity; ++probe)
        {
            const Slot& slot { mTable[(start + probe) & (Capacity - 1u)] };
            const ptr_type k { slot.mKey.load(std::memory_order_acquire) };

            if (k == EMPTY_KEY)
                return nullptr;     // probe chain ends; key was never registered here

            if (k == key)
            {
                // Key found. Resolve the narrow window between CAS(key) and store(value)
                // in register_resource_object, and between store(nullptr) and store(TOMBSTONE)
                // in unregister_resource_object, with a bounded spin.
                for (int32_t tries { 0 }; tries < 512; ++tries)
                {
                    std::atomic<uint32_t>* val { slot.mValue.load(std::memory_order_acquire) };
                    if (val != nullptr)
                        return val;  // value is now visible and consistent

                    // Value is null: either publish or unpublish in progress.
                    // Verify the key is still ours to distinguish from a concurrent unregister.
                    if (slot.mKey.load(std::memory_order_acquire) != key)
                        return nullptr;  // slot was tombstoned (thread already woke)
                }

                return nullptr;  // spin limit reached (extremely rare; treat as not found)
            }

            // TOMBSTONE or different key: continue probing
        }

        return nullptr;
    }
};

} // namespace areg::os

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITANYREGISTRY_HPP
