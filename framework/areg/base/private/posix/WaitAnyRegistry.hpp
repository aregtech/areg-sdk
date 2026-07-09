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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
 * \brief   Lock-free open-addressing hash map that maps a thread ID
 *          to the pointer to the per-call firedWord atomic
 *          Used exclusively by the WaitAny path in SyncLockAndWaitPosix:
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
     **/
    struct Slot
    {
        //!< Thread-ID key. 0 = EMPTY, ~0 = TOMBSTONE, otherwise a valid thread ID.
        std::atomic<ptr_type>               mKey   { 0u };
        //!< Pointer to the thread's stack-local firedWord. null until published.
        std::atomic<std::atomic<uint32_t>*> mValue { nullptr };
    };

    static_assert(std::atomic<ptr_type>::is_always_lock_free, "WaitAnyRegistry: ptr_type atomic must be lock-free.");
    static_assert(std::atomic<std::atomic<uint32_t>*>::is_always_lock_free, "WaitAnyRegistry: pointer atomic must be lock-free.");

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The hash table; default-initialized to EMPTY slots.
    Slot mTable[Capacity];

//////////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Maps a thread-ID key to an initial probe index.
     *          Uses a multiplicative (Fibonacci) hash to scatter aligned-pointer keys
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
     * \param   key     Thread ID
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
                    // we own this slot. Publish the value.
                    slot.mValue.store(value, std::memory_order_release);
                    return;
                }
                // CAS failed: another thread claimed this slot; probe next
            }
            // occupied by a different key: probe next
        }

        ASSERT(false);
    }

    /**
     * \brief   Removes the entry for the given key and marks its slot as TOMBSTONE so
     *          that probe chains from other threads remain intact.
     *
     * \param   key     The thread ID to uregister
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
                slot.mValue.store(nullptr, std::memory_order_release);
                slot.mKey.store(TOMBSTONE_KEY, std::memory_order_release);
                return;
            }

            if (k == EMPTY_KEY)
                return;  // key not in table -- should not happen
        }
    }

    /**
     * \brief   Looks up the firedWord pointer for the given thread-ID key.
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
                for (int32_t tries { 0 }; tries < 512; ++tries)
                {
                    std::atomic<uint32_t>* val { slot.mValue.load(std::memory_order_acquire) };
                    if (val != nullptr)
                        return val;  // value is now visible and consistent

                    if (slot.mKey.load(std::memory_order_acquire) != key)
                        return nullptr;
                }

                return nullptr;  // spin limit reached
            }
        }

        return nullptr;
    }
};

} // namespace areg::os

#endif  // defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

#endif  // AREG_BASE_PRIVATE_POSIX_WAITANYREGISTRY_HPP
