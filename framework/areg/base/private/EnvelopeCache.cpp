/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/EnvelopeCache.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, a bounded cache of freed message envelope blocks.
 ************************************************************************/
#include "areg/base/private/EnvelopeCache.hpp"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <atomic>
#include <new>
#include <type_traits>
#include <utility>

#if defined(__SANITIZE_ADDRESS__)
    #define AREG_ENVELOPE_ASAN  1
#elif defined(__has_feature)
    #if __has_feature(address_sanitizer)
        #define AREG_ENVELOPE_ASAN  1
    #endif
#endif

#if defined(AREG_ENVELOPE_ASAN)
    #include <sanitizer/asan_interface.h>
#endif

#if defined(_MSC_VER)
    #define AREG_ENVELOPE_COLD  __declspec(noinline)
#else   // _MSC_VER
    #define AREG_ENVELOPE_COLD  __attribute__((noinline))
#endif  // _MSC_VER

namespace
{
    constexpr uint32_t  HEADER_SIZE     { static_cast<uint32_t>(sizeof(areg::EventHeader)) };

    //!< The payload bytes of each size class: 64-byte steps up to 512, then half-power steps.
    constexpr uint32_t  CLASS_PAYLOAD[] { 64u, 128u, 192u, 256u, 320u, 384u, 448u, 512u, 768u, 1024u, 1536u, 2048u, 3072u, 4096u };
    constexpr uint32_t  CLASS_COUNT     { static_cast<uint32_t>(sizeof(CLASS_PAYLOAD) / sizeof(CLASS_PAYLOAD[0])) };
    constexpr uint32_t  NO_CLASS        { CLASS_COUNT };

    //!< The most blocks a magazine holds.
    constexpr uint32_t  MAGAZINE_SIZE   { 16u };
    //!< The most payload bytes a magazine holds, which limits the blocks of the large classes.
    constexpr uint32_t  MAGAZINE_BYTES  { 8u * 1024u };
    //!< The most bytes of blocks the depot keeps, all classes together.
    constexpr uint32_t  DEPOT_BYTES     { 256u * 1024u };

    constexpr uint32_t _class_limit(uint32_t payload) noexcept
    {
        const uint32_t byBytes{ MAGAZINE_BYTES / payload };
        const uint32_t limit{ byBytes != 0u ? byBytes : 1u };
        return (limit < MAGAZINE_SIZE ? limit : MAGAZINE_SIZE);
    }

    //!< The most blocks a magazine of each class holds.
    constexpr uint32_t  CLASS_LIMIT[CLASS_COUNT]
    {
          _class_limit(CLASS_PAYLOAD[ 0]), _class_limit(CLASS_PAYLOAD[ 1]), _class_limit(CLASS_PAYLOAD[ 2])
        , _class_limit(CLASS_PAYLOAD[ 3]), _class_limit(CLASS_PAYLOAD[ 4]), _class_limit(CLASS_PAYLOAD[ 5])
        , _class_limit(CLASS_PAYLOAD[ 6]), _class_limit(CLASS_PAYLOAD[ 7]), _class_limit(CLASS_PAYLOAD[ 8])
        , _class_limit(CLASS_PAYLOAD[ 9]), _class_limit(CLASS_PAYLOAD[10]), _class_limit(CLASS_PAYLOAD[11])
        , _class_limit(CLASS_PAYLOAD[12]), _class_limit(CLASS_PAYLOAD[13])
    };

    //!< Returns the smallest class that holds 'payload' bytes, or NO_CLASS.
    inline uint32_t _class_of(uint32_t payload) noexcept
    {
        if (payload <= 512u)
            return (payload != 0u ? (payload - 1u) >> 6 : 0u);
        else if (payload <= 1024u)
            return (payload <= 768u ? 8u : 9u);
        else if (payload <= 2048u)
            return (payload <= 1536u ? 10u : 11u);
        else if (payload <= 4096u)
            return (payload <= 3072u ? 12u : 13u);
        else
            return NO_CLASS;
    }

    //!< A fixed array of cached blocks of one class.
    struct Magazine
    {
        Magazine *  next    { nullptr };
        uint32_t    count   { 0u };
        uint8_t *   blocks[MAGAZINE_SIZE] { };
    };

    //!< The process-wide store of full and empty magazines of one class.
    struct alignas(64) Depot
    {
        areg::PlainSpinLock lock    { };
        Magazine *  full            { nullptr };
        Magazine *  empty           { nullptr };
    };

    //!< The magazines of one thread: the one in use and the one before it, per class.
    struct ThreadMagazines
    {
        Magazine *  loaded[CLASS_COUNT]     { };
        Magazine *  previous[CLASS_COUNT]   { };
        bool        armed                   { false };
        bool        closed                  { false };
    };

    //!< Returns the thread's magazines when the thread exits.
    struct ThreadExit
    {
        ~ThreadExit();
        bool        armed   { false };
    };

    static_assert(std::is_trivially_destructible_v<Depot>, "The depot must stay usable during static destruction");
    static_assert(std::is_trivially_destructible_v<ThreadMagazines>, "The thread state must have no destructor");

    Depot   _depots[CLASS_COUNT];

    //!< The bytes of blocks in the full magazines of all depots.
    std::atomic<uint32_t>   _depotBytes { 0u };

    thread_local ThreadMagazines _threadMagazines;

    inline void _poison(uint8_t * block, uint32_t idx) noexcept
    {
#if defined(AREG_ENVELOPE_ASAN)
        ASAN_POISON_MEMORY_REGION(block, HEADER_SIZE + CLASS_PAYLOAD[idx]);
#else   // AREG_ENVELOPE_ASAN
        (void)block; (void)idx;
#endif  // AREG_ENVELOPE_ASAN
    }

    inline void _unpoison(uint8_t * block, uint32_t idx) noexcept
    {
#if defined(AREG_ENVELOPE_ASAN)
        ASAN_UNPOISON_MEMORY_REGION(block, HEADER_SIZE + CLASS_PAYLOAD[idx]);
#else   // AREG_ENVELOPE_ASAN
        (void)block; (void)idx;
#endif  // AREG_ENVELOPE_ASAN
    }

    //!< Frees every block of the magazine and leaves it empty.
    void _release_blocks(Magazine & magazine, uint32_t idx) noexcept
    {
        for (uint32_t i = 0u; i < magazine.count; ++i)
        {
            _unpoison(magazine.blocks[i], idx);
            delete [] magazine.blocks[i];
        }

        magazine.count = 0u;
    }

    //!< Frees the blocks of the magazines in the list and the magazines.
    void _release_list(Magazine * list, uint32_t idx) noexcept
    {
        while (list != nullptr)
        {
            Magazine * next{ list->next };
            _release_blocks(*list, idx);
            delete list;
            list = next;
        }
    }

    inline uint32_t _magazine_bytes(const Magazine & magazine, uint32_t idx) noexcept
    {
        return magazine.count * (HEADER_SIZE + CLASS_PAYLOAD[idx]);
    }

    //!< Reserves depot room for the magazine's blocks. Returns false if the depot is full.
    inline bool _depot_reserve(const Magazine & magazine, uint32_t idx) noexcept
    {
        const uint32_t bytes{ _magazine_bytes(magazine, idx) };
        if (_depotBytes.fetch_add(bytes, std::memory_order_relaxed) + bytes <= DEPOT_BYTES)
            return true;

        _depotBytes.fetch_sub(bytes, std::memory_order_relaxed);
        return false;
    }

    //!< Registers the thread exit handler once per thread.
    inline void _arm(ThreadMagazines & mags) noexcept
    {
        if (mags.armed == false)
        {
            static thread_local ThreadExit _threadExit;
            _threadExit.armed = true;
            mags.armed = true;
        }
    }

    //!< Empties both magazines of the class, giving non-empty ones to the depot when 'toDepot' is set.
    void _flush_class(ThreadMagazines & mags, uint32_t idx, bool toDepot) noexcept
    {
        Magazine * own[2]{ mags.loaded[idx], mags.previous[idx] };
        mags.loaded[idx]   = nullptr;
        mags.previous[idx] = nullptr;

        for (Magazine * magazine : own)
        {
            if (magazine == nullptr)
                continue;

            if (toDepot && (magazine->count != 0u) && _depot_reserve(*magazine, idx))
            {
                Depot & depot{ _depots[idx] };
                depot.lock.lock();
                magazine->next = depot.full;
                depot.full     = magazine;
                depot.lock.unlock();
                magazine       = nullptr;
            }

            if (magazine != nullptr)
            {
                _release_blocks(*magazine, idx);
                delete magazine;
            }
        }
    }

    ThreadExit::~ThreadExit()
    {
        ThreadMagazines & mags{ _threadMagazines };
        mags.closed = true;
        for (uint32_t idx = 0u; idx < CLASS_COUNT; ++idx)
        {
            _flush_class(mags, idx, true);
        }
    }

    AREG_ENVELOPE_COLD uint8_t * _take_slow(ThreadMagazines & mags, uint32_t idx) noexcept
    {
        if (mags.closed)
            return nullptr;

        Magazine *& loaded  { mags.loaded[idx] };
        Magazine *& previous{ mags.previous[idx] };
        if ((previous != nullptr) && (previous->count != 0u))
        {
            std::swap(loaded, previous);
            return loaded->blocks[--loaded->count];
        }

        Depot & depot{ _depots[idx] };
        depot.lock.lock();
        Magazine * full{ depot.full };
        if (full != nullptr)
        {
            depot.full = full->next;
            if (previous != nullptr)
            {
                previous->next = depot.empty;
                depot.empty    = previous;
                previous       = nullptr;
            }
        }

        depot.lock.unlock();

        if (full == nullptr)
            return nullptr;

        _depotBytes.fetch_sub(_magazine_bytes(*full, idx), std::memory_order_relaxed);
        _arm(mags);
        full->next = nullptr;
        previous   = loaded;
        loaded     = full;
        return loaded->blocks[--loaded->count];
    }

    AREG_ENVELOPE_COLD bool _put_slow(ThreadMagazines & mags, uint32_t idx, uint8_t * block) noexcept
    {
        if (mags.closed)
            return false;

        Magazine *& loaded  { mags.loaded[idx] };
        Magazine *& previous{ mags.previous[idx] };
        if ((previous != nullptr) && (previous->count < CLASS_LIMIT[idx]))
        {
            std::swap(loaded, previous);
            loaded->blocks[loaded->count++] = block;
            return true;
        }

        Depot & depot{ _depots[idx] };
        Magazine * empty{ nullptr };
        const bool toDepot{ (previous != nullptr) && _depot_reserve(*previous, idx) };
        depot.lock.lock();
        if (toDepot)
        {
            previous->next = depot.full;
            depot.full     = previous;
            previous       = nullptr;
        }

        if ((previous == nullptr) && (depot.empty != nullptr))
        {
            empty       = depot.empty;
            depot.empty = empty->next;
        }

        depot.lock.unlock();

        if (previous != nullptr)
        {
            // The depot is at its cap: the full magazine is emptied and reused.
            _release_blocks(*previous, idx);
            empty    = previous;
            previous = nullptr;
        }
        else if (empty == nullptr)
        {
            empty = new (std::nothrow) Magazine;
            if (empty == nullptr)
                return false;
        }

        _arm(mags);
        empty->next = nullptr;
        previous    = loaded;
        loaded      = empty;
        loaded->blocks[loaded->count++] = block;
        return true;
    }

    inline uint8_t * _take(uint32_t idx) noexcept
    {
        ThreadMagazines & mags{ _threadMagazines };
        Magazine * loaded{ mags.loaded[idx] };
        uint8_t * block{ ((loaded != nullptr) && (loaded->count != 0u)) ? loaded->blocks[--loaded->count] : _take_slow(mags, idx) };
        if (block != nullptr)
        {
            _unpoison(block, idx);
        }

        return block;
    }

    inline bool _put(uint32_t idx, uint8_t * block) noexcept
    {
        ThreadMagazines & mags{ _threadMagazines };
        Magazine * loaded{ mags.loaded[idx] };
        _poison(block, idx);
        if ((loaded != nullptr) && (loaded->count < CLASS_LIMIT[idx]))
        {
            loaded->blocks[loaded->count++] = block;
            return true;
        }
        else if (_put_slow(mags, idx, block))
        {
            return true;
        }

        _unpoison(block, idx);
        return false;
    }
}

uint8_t * areg::envelope_block_alloc(uint32_t & payload) noexcept
{
    const uint32_t idx{ _class_of(payload) };
    if (idx == NO_CLASS)
        return new (std::nothrow) uint8_t[payload + HEADER_SIZE];

    payload = CLASS_PAYLOAD[idx];
    uint8_t * block{ _take(idx) };
    return (block != nullptr ? block : new (std::nothrow) uint8_t[payload + HEADER_SIZE]);
}

void areg::release_raw_buffer(areg::RawBuffer * buffer) noexcept
{
    const areg::BufferHeader & header{ buffer->bufHeader };
    if (header.biOffset == HEADER_SIZE)
    {
        const uint32_t idx{ _class_of(header.biLength) };
        if ((idx != NO_CLASS) && (CLASS_PAYLOAD[idx] == header.biLength) && _put(idx, reinterpret_cast<uint8_t *>(buffer)))
            return;
    }

    delete [] reinterpret_cast<uint8_t *>(buffer);
}

void areg::envelope_cache_trim() noexcept
{
    ThreadMagazines & mags{ _threadMagazines };
    for (uint32_t idx = 0u; idx < CLASS_COUNT; ++idx)
    {
        _flush_class(mags, idx, false);

        Depot & depot{ _depots[idx] };
        depot.lock.lock();
        Magazine * full { depot.full };
        Magazine * empty{ depot.empty };
        depot.full      = nullptr;
        depot.empty     = nullptr;
        depot.lock.unlock();

        for (const Magazine * magazine = full; magazine != nullptr; magazine = magazine->next)
        {
            _depotBytes.fetch_sub(_magazine_bytes(*magazine, idx), std::memory_order_relaxed);
        }

        _release_list(full, idx);
        _release_list(empty, idx);
    }
}

uint32_t areg::envelope_cache_held(uint32_t payload) noexcept
{
    const uint32_t idx{ _class_of(payload) };
    if (idx == NO_CLASS)
        return 0u;

    const ThreadMagazines & mags{ _threadMagazines };
    uint32_t result{ 0u };
    result += (mags.loaded[idx]   != nullptr ? mags.loaded[idx]->count   : 0u);
    result += (mags.previous[idx] != nullptr ? mags.previous[idx]->count : 0u);

    Depot & depot{ _depots[idx] };
    depot.lock.lock();
    for (const Magazine * magazine = depot.full; magazine != nullptr; magazine = magazine->next)
    {
        result += magazine->count;
    }

    depot.lock.unlock();
    return result;
}
