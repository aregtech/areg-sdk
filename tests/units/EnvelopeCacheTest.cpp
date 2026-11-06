/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/EnvelopeCacheTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for the envelope block cache: size classes,
 *              reuse, the cap, the hand-off between threads and thread exit.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/private/EnvelopeCache.hpp"

#include <atomic>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

namespace
{
    //!< The cache keeps 16 blocks of 384 bytes per magazine and 256 KiB of blocks in the depot.
    constexpr uint32_t  PAYLOAD     { 384u };
    constexpr uint32_t  LIMIT       { 16u };
    constexpr uint32_t  DEPOT_HELD  { (256u * 1024u) / (PAYLOAD + static_cast<uint32_t>(sizeof(areg::EventHeader))) };

    areg::MessageEnvelope _make(uint32_t reserve)
    {
        areg::EventHeader header{};
        return areg::MessageEnvelope(header, reserve);
    }

    const void * _block(const areg::MessageEnvelope & env)
    {
        return env.raw_envelope();
    }

    uint32_t _length(const areg::MessageEnvelope & env)
    {
        return env.raw_envelope()->envHeader.bufHeader.biLength;
    }

    //!< Releases a hand-made block of the given header values through RawBufferPtr.
    void _release_made(uint32_t offset, uint32_t length)
    {
        uint8_t * block{ new uint8_t[offset + length] };
        areg::RawBuffer * raw{ reinterpret_cast<areg::RawBuffer *>(block) };
        raw->bufHeader.biOffset = offset;
        raw->bufHeader.biLength = length;
        areg::RawBufferPtr owner(raw);
        owner.reset();
    }
}

/**
 * \brief   A payload up to 4 KiB gets the payload of its size class; a larger one its aligned size.
 **/
TEST(EnvelopeCacheTest, payload_rounded_to_class)
{
    EXPECT_EQ(_length(_make(1u)), 64u);
    EXPECT_EQ(_length(_make(300u)), 320u);
    EXPECT_EQ(_length(_make(512u)), 512u);
    EXPECT_EQ(_length(_make(513u)), 768u);
    EXPECT_EQ(_length(_make(1500u)), 1536u);
    EXPECT_EQ(_length(_make(4096u)), 4096u);
    EXPECT_EQ(_length(_make(5000u)), 5056u);
}

/**
 * \brief   A freed block is handed out again on the same thread; trim empties the cache.
 **/
TEST(EnvelopeCacheTest, block_reused_on_same_thread)
{
    areg::envelope_cache_trim();
    ASSERT_EQ(areg::envelope_cache_held(PAYLOAD), 0u);

    const void * first{ nullptr };
    {
        areg::MessageEnvelope env{ _make(PAYLOAD) };
        first = _block(env);
    }

    EXPECT_EQ(areg::envelope_cache_held(PAYLOAD), 1u);

    areg::MessageEnvelope again{ _make(PAYLOAD) };
    EXPECT_EQ(_block(again), first);
    EXPECT_EQ(areg::envelope_cache_held(PAYLOAD), 0u);

    again.invalidate();
    areg::envelope_cache_trim();
    EXPECT_EQ(areg::envelope_cache_held(PAYLOAD), 0u);
}

/**
 * \brief   Only an envelope block of exactly a class size is cached.
 **/
TEST(EnvelopeCacheTest, only_exact_envelope_blocks_cached)
{
    constexpr uint32_t ENVELOPE_OFFSET{ static_cast<uint32_t>(sizeof(areg::EventHeader)) };
    areg::envelope_cache_trim();

    _release_made(static_cast<uint32_t>(sizeof(areg::BufferHeader)), 320u);
    EXPECT_EQ(areg::envelope_cache_held(320u), 0u);

    _release_made(ENVELOPE_OFFSET, 300u);
    EXPECT_EQ(areg::envelope_cache_held(320u), 0u);

    _release_made(ENVELOPE_OFFSET, 5056u);
    EXPECT_EQ(areg::envelope_cache_held(4096u), 0u);

    _release_made(ENVELOPE_OFFSET, 320u);
    EXPECT_EQ(areg::envelope_cache_held(320u), 1u);

    areg::envelope_cache_trim();
}

/**
 * \brief   Freeing many blocks on one thread caches no more than two magazines and the depot cap.
 **/
TEST(EnvelopeCacheTest, cache_is_bounded)
{
    areg::envelope_cache_trim();
    {
        std::vector<areg::MessageEnvelope> list;
        list.reserve(10000u);
        for (uint32_t i = 0u; i < 10000u; ++i)
        {
            list.push_back(_make(PAYLOAD));
        }
    }

    const uint32_t held{ areg::envelope_cache_held(PAYLOAD) };
    EXPECT_LE(held, 2u * LIMIT + DEPOT_HELD);
    EXPECT_GE(held, DEPOT_HELD);
    areg::envelope_cache_trim();
}

/**
 * \brief   Blocks freed on a thread that exits reach the depot, up to its cap, and another
 *          thread allocates them.
 **/
TEST(EnvelopeCacheTest, blocks_pass_between_threads)
{
    areg::envelope_cache_trim();

    std::vector<areg::MessageEnvelope> list;
    std::set<const void *> freed;
    std::thread producer([&list]() {
        for (uint32_t i = 0u; i < 2000u; ++i)
        {
            list.push_back(_make(PAYLOAD));
        }
    });

    producer.join();
    for (const areg::MessageEnvelope & env : list)
    {
        freed.insert(_block(env));
    }

    std::thread consumer([&list]() { list.clear(); });
    consumer.join();
    EXPECT_EQ(areg::envelope_cache_held(PAYLOAD), DEPOT_HELD);

    uint32_t reused{ 0u };
    std::thread second([&list, &freed, &reused]() {
        for (uint32_t i = 0u; i < DEPOT_HELD; ++i)
        {
            list.push_back(_make(PAYLOAD));
            reused += (freed.count(_block(list.back())) != 0u ? 1u : 0u);
        }
    });

    second.join();
    EXPECT_EQ(reused, DEPOT_HELD);
    EXPECT_EQ(areg::envelope_cache_held(PAYLOAD), 0u);

    list.clear();
    areg::envelope_cache_trim();
}

namespace
{
    uint32_t    _heldBefore { 0u };
    uint32_t    _heldAfter  { 0u };

    //!< Frees its envelope in the thread exit, after the cache closed the thread.
    struct LateOwner
    {
        ~LateOwner()
        {
            _heldBefore = areg::envelope_cache_held(PAYLOAD);
            env.invalidate();
            _heldAfter  = areg::envelope_cache_held(PAYLOAD);
        }

        areg::MessageEnvelope env;
    };
}

/**
 * \brief   A block freed after the thread's cache closed goes to the heap.
 **/
TEST(EnvelopeCacheTest, free_after_thread_exit_goes_to_heap)
{
    areg::envelope_cache_trim();
    std::thread worker([]() {
        static thread_local LateOwner owner;
        owner.env = _make(PAYLOAD);
        areg::MessageEnvelope early{ _make(PAYLOAD) };
        early.invalidate();
    });

    worker.join();
    EXPECT_EQ(_heldAfter, _heldBefore);
    EXPECT_EQ(areg::envelope_cache_held(PAYLOAD), 1u);
    areg::envelope_cache_trim();
}

/**
 * \brief   Envelopes cross from two producers to two consumers; every payload arrives intact.
 **/
TEST(EnvelopeCacheTest, concurrent_handoff_keeps_payload)
{
    constexpr uint32_t PER_PRODUCER { 50000u };
    constexpr uint32_t BATCH        { 64u };
    constexpr uint32_t PRODUCERS    { 2u };

    std::mutex lock;
    std::condition_variable ready;
    std::deque<std::vector<areg::MessageEnvelope>> queue;
    uint32_t done{ 0u };
    uint32_t received{ 0u };
    uint32_t damaged{ 0u };

    auto produce = [&](uint32_t tag) {
        std::vector<areg::MessageEnvelope> batch;
        for (uint32_t i = 0u; i < PER_PRODUCER; ++i)
        {
            areg::MessageEnvelope env{ _make(PAYLOAD) };
            const uint32_t value{ (tag << 24) | i };
            uint8_t * data{ env.buffer() };
            for (uint32_t pos = 0u; pos < PAYLOAD; pos += sizeof(value))
            {
                std::memcpy(data + pos, &value, sizeof(value));
            }

            env.set_size_used(PAYLOAD);
            batch.push_back(std::move(env));
            if (batch.size() == BATCH)
            {
                std::lock_guard<std::mutex> guard(lock);
                queue.push_back(std::move(batch));
                batch.clear();
                ready.notify_one();
            }
        }

        std::lock_guard<std::mutex> guard(lock);
        if (batch.empty() == false)
        {
            queue.push_back(std::move(batch));
        }

        ++done;
        ready.notify_all();
    };

    auto consume = [&]() {
        while (true)
        {
            std::vector<areg::MessageEnvelope> batch;
            {
                std::unique_lock<std::mutex> guard(lock);
                ready.wait(guard, [&]() { return (queue.empty() == false) || (done == PRODUCERS); });
                if (queue.empty())
                    return;

                batch = std::move(queue.front());
                queue.pop_front();
            }

            uint32_t bad{ 0u };
            for (const areg::MessageEnvelope & env : batch)
            {
                const uint8_t * data{ env.buffer() };
                uint32_t first{ 0u };
                std::memcpy(&first, data, sizeof(first));
                for (uint32_t pos = 0u; pos < PAYLOAD; pos += sizeof(first))
                {
                    uint32_t value{ 0u };
                    std::memcpy(&value, data + pos, sizeof(value));
                    bad += (value != first ? 1u : 0u);
                }
            }

            std::lock_guard<std::mutex> guard(lock);
            received += static_cast<uint32_t>(batch.size());
            damaged  += bad;
        }
    };

    std::thread p1(produce, 1u), p2(produce, 2u), c1(consume), c2(consume);
    p1.join();
    p2.join();
    c1.join();
    c2.join();

    EXPECT_EQ(received, PRODUCERS * PER_PRODUCER);
    EXPECT_EQ(damaged, 0u);
    EXPECT_LE(areg::envelope_cache_held(PAYLOAD), DEPOT_HELD);
    areg::envelope_cache_trim();
}

// Returning blocks in reverse order makes each available for reuse without aliasing live blocks.
TEST(EnvelopeCacheTest, reverse_release_reuses_each_live_block_once)
{
    areg::envelope_cache_trim();
    constexpr uint32_t COUNT{ 2u * LIMIT + 1u };
    std::vector<areg::MessageEnvelope> envelopes;
    std::set<const void*> original;
    for (uint32_t index = 0u; index < COUNT; ++index)
    {
        envelopes.push_back(_make(PAYLOAD));
        EXPECT_TRUE(original.insert(_block(envelopes.back())).second);
    }

    for (uint32_t index = COUNT; index != 0u; --index)
    {
        envelopes[index - 1u].invalidate();
    }

    EXPECT_EQ(areg::envelope_cache_held(PAYLOAD), COUNT);
    std::set<const void*> reused;
    for (uint32_t index = 0u; index < COUNT; ++index)
    {
        envelopes[index] = _make(PAYLOAD);
        EXPECT_EQ(original.count(_block(envelopes[index])), 1u);
        EXPECT_TRUE(reused.insert(_block(envelopes[index])).second);
        EXPECT_EQ(envelopes[index].size_used(), 0u);
        EXPECT_TRUE(envelopes[index].is_unique());
    }

    EXPECT_EQ(reused, original);
    EXPECT_EQ(areg::envelope_cache_held(PAYLOAD), 0u);
    envelopes.clear();
    areg::envelope_cache_trim();
    EXPECT_EQ(areg::envelope_cache_held(PAYLOAD), 0u);
}

// Allocations of different size classes never reuse a block that is still live.
TEST(EnvelopeCacheTest, size_classes_do_not_reuse_live_other_class)
{
    areg::envelope_cache_trim();
    constexpr uint32_t CLASSES[]{ 64u, 128u, 192u, 256u, 320u, 384u, 448u
        , 512u, 768u, 1024u, 1536u, 2048u, 3072u, 4096u };
    std::vector<areg::MessageEnvelope> envelopes;
    std::vector<const void*> addresses;
    for (const uint32_t payload : CLASSES)
    {
        envelopes.push_back(_make(payload));
        addresses.push_back(_block(envelopes.back()));
        EXPECT_EQ(_length(envelopes.back()), payload);
    }

    for (areg::MessageEnvelope& envelope : envelopes)
    {
        envelope.invalidate();
    }

    for (uint32_t index = 0u; index < std::size(CLASSES); ++index)
    {
        EXPECT_EQ(areg::envelope_cache_held(CLASSES[index]), 1u);
        envelopes[index] = _make(CLASSES[index]);
        EXPECT_EQ(_block(envelopes[index]), addresses[index]);
        EXPECT_EQ(_length(envelopes[index]), CLASSES[index]);
        EXPECT_EQ(areg::envelope_cache_held(CLASSES[index]), 0u);
    }

    envelopes.clear();
    areg::envelope_cache_trim();
    for (const uint32_t payload : CLASSES)
    {
        EXPECT_EQ(areg::envelope_cache_held(payload), 0u);
    }
}

// Releasing an oversized block does not add it to the cache.
TEST(EnvelopeCacheTest, uncached_large_blocks_leave_no_cached_entries)
{
    areg::envelope_cache_trim();
    areg::MessageEnvelope envelope{ _make(8192u) };
    EXPECT_GE(_length(envelope), 8192u);
    envelope.invalidate();
    EXPECT_EQ(areg::envelope_cache_held(8192u), 0u);
    EXPECT_EQ(areg::envelope_cache_held(4096u), 0u);
    areg::envelope_cache_trim();
}

// A payload of 0 falls in the smallest class.
TEST(EnvelopeCacheTest, empty_payload_is_in_the_smallest_class)
{
    areg::envelope_cache_trim();
    EXPECT_EQ(areg::envelope_cache_held(0u), areg::envelope_cache_held(1u));
}

namespace
{
    //! Allocates and frees an envelope when the thread ends, after the cache of the thread closed.
    struct LateEnvelope
    {
        ~LateEnvelope()
        {
            areg::MessageEnvelope env(true, 256u);
            valid = env.is_valid();
        }

        void touch() noexcept { }

        static inline std::atomic_bool valid{ false };
    };
}

// An envelope made while the thread ends gets its block from the heap and returns it there.
TEST(EnvelopeCacheTest, envelope_at_thread_exit_bypasses_the_cache)
{
    std::thread worker([]
    {
        static thread_local LateEnvelope late;
        late.touch();
        areg::MessageEnvelope env(true, 256u);
        EXPECT_TRUE(env.is_valid());
    });

    worker.join();
    EXPECT_TRUE(LateEnvelope::valid.load());
}
