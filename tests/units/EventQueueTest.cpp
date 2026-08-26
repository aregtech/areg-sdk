/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/EventQueueTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for EventQueue.
 *              Covers single-threaded correctness (push/pop, priority lanes,
 *              capacity, exit state, doorbell polling) and multi-threaded
 *              stress (many producers, single consumer) verifying no event loss
 *              and no lost wake-up.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/component/private/EventQueue.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/EventDefs.hpp"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace
{
    using areg::Event;
    using areg::EventType;
    using areg::EventPriority;
    using areg::EventQueue;

    //!< Event protectedly inherits MessageEnvelope, so set_event_id() is reachable only
    //!< from a derived class. This thin tag carries the test id in the event buffer.
    struct TagEvent : public Event
    {
        TagEvent(uint32_t tag, EventPriority prio)
            : Event(EventType::EventCustomExternal, prio)
        {
            set_event_id(tag);
        }
    };

    //!< Builds a valid normal/priority event tagged with \a tag (read back via event_id()).
    inline Event makeEvent(uint32_t tag, EventPriority prio = EventPriority::NormalPrio)
    {
        return TagEvent(tag, prio);
    }

    //!< An EventQueue holding its lanes, the state a running dispatcher keeps it in.
    struct ReadyQueue : public EventQueue
    {
        explicit ReadyQueue(uint32_t maxQueue, bool dropOnFull = false, uint32_t waitMs = areg::QUEUE_DEFAULT_FULL_WAIT_MS)
            : EventQueue(maxQueue, dropOnFull, waitMs)
        {
            acquire_lanes();
        }
    };
}

//////////////////////////////////////////////////////////////////////////
// Single-threaded correctness
//////////////////////////////////////////////////////////////////////////

TEST(EventQueueTest, empty_on_construction)
{
    ReadyQueue queue(0u);
    EXPECT_FALSE(queue.has_pending());
    EXPECT_FALSE(queue.is_exit_triggered());
    EXPECT_FALSE(queue.pop_event().is_valid());
    EXPECT_FALSE(queue.wait_event(areg::DO_NOT_WAIT));
}

TEST(EventQueueTest, push_pop_single)
{
    ReadyQueue queue(0u);
    Event evt = makeEvent(42u);
    queue.push_event(evt);

    EXPECT_FALSE(evt.is_valid());           // moved-from after push
    EXPECT_TRUE(queue.has_pending());
    EXPECT_TRUE(queue.wait_event(areg::DO_NOT_WAIT));

    Event out = queue.pop_event();
    ASSERT_TRUE(out.is_valid());
    EXPECT_EQ(out.event_id(), 42u);
    EXPECT_FALSE(queue.has_pending());
    EXPECT_FALSE(queue.pop_event().is_valid());
}

TEST(EventQueueTest, fifo_order_normal_lane)
{
    ReadyQueue queue(0u);
    constexpr uint32_t COUNT{ 100u };
    for (uint32_t i = 0u; i < COUNT; ++i)
    {
        Event evt = makeEvent(i);
        queue.push_event(evt);
    }

    for (uint32_t i = 0u; i < COUNT; ++i)
    {
        Event out = queue.pop_event();
        ASSERT_TRUE(out.is_valid());
        EXPECT_EQ(out.event_id(), i);
    }

    EXPECT_FALSE(queue.has_pending());
}

TEST(EventQueueTest, priority_lane_drained_first)
{
    ReadyQueue queue(0u);
    Event normal   = makeEvent(1u, EventPriority::NormalPrio);   queue.push_event(normal);
    Event high     = makeEvent(2u, EventPriority::HighPrio);     queue.push_event(high);
    Event critical = makeEvent(3u, EventPriority::CriticalPrio); queue.push_event(critical);

    EXPECT_EQ(queue.pop_event().event_id(), 3u);    // critical first
    EXPECT_EQ(queue.pop_event().event_id(), 2u);    // then high
    EXPECT_EQ(queue.pop_event().event_id(), 1u);    // then normal

    EXPECT_FALSE(queue.has_pending());
}

TEST(EventQueueTest, exit_preempts_is_sticky_and_resets)
{
    ReadyQueue queue(0u);
    Event normal = makeEvent(7u);
    queue.push_event(normal);

    queue.trigger_exit();
    EXPECT_TRUE(queue.is_exit_triggered());
    EXPECT_TRUE(queue.has_pending());

    // Exit preempts the queued normal event and is sticky.
    EXPECT_TRUE(queue.pop_event().is_exit_prio());
    EXPECT_TRUE(queue.pop_event().is_exit_prio());

    // Clearing exit lets the still-queued normal event resurface.
    queue.reset_exit();
    EXPECT_FALSE(queue.is_exit_triggered());
    Event out = queue.pop_event();
    ASSERT_TRUE(out.is_valid());
    EXPECT_EQ(out.event_id(), 7u);
    EXPECT_FALSE(queue.has_pending());
}

TEST(EventQueueTest, push_event_exit_routes_to_sticky_flag)
{
    // An ExitPrio event must NOT be queued: it sets the sticky exit flag so pop_event()
    // synthesizes the singleton ExitEvent and the queue itself stays empty.
    ReadyQueue queue(0u);
    Event exit = makeEvent(0u, EventPriority::ExitPrio);
    queue.push_event(exit);

    EXPECT_TRUE(queue.is_exit_triggered());
    EXPECT_TRUE(queue.has_pending());
    EXPECT_TRUE(queue.pop_event().is_exit_prio());
}

TEST(EventQueueTest, pop_events_preempts_with_exit)
{
    // pop_events() must honor the sticky exit flag and return a single ExitEvent,
    // exactly like pop_event().
    ReadyQueue queue(0u);
    Event normal = makeEvent(5u);
    queue.push_event(normal);
    queue.trigger_exit();

    Event out[4];
    const uint32_t popped = queue.pop_events(out, 4u);
    EXPECT_EQ(popped, 1u);
    EXPECT_TRUE(out[0].is_exit_prio());
}

TEST(EventQueueTest, push_events_routes_exit_to_flag)
{
    // A batch whose highest-priority slot is an exit must set the sticky flag, not queue it.
    ReadyQueue queue(0u);
    Event batch[3] =
    {
          makeEvent(0u, EventPriority::ExitPrio)
        , makeEvent(1u, EventPriority::HighPrio)
        , makeEvent(2u, EventPriority::NormalPrio)
    };

    const uint32_t overflow = queue.push_events(batch, 3u);
    EXPECT_EQ(overflow, 0u);
    EXPECT_TRUE(queue.is_exit_triggered());
    EXPECT_TRUE(queue.pop_event().is_exit_prio());   // exit preempts the queued high/normal events
}

TEST(EventQueueTest, capacity_overflow_returns_event)
{
    constexpr uint32_t CAPACITY{ 32u };
    // A short block timeout: the test wants the event handed back, not the full wait.
    ReadyQueue queue(CAPACITY, false, 50u);
    for (uint32_t i = 0u; i < CAPACITY; ++i)
    {
        Event evt = makeEvent(i);
        queue.push_event(evt);
    }

    Event overflow = makeEvent(999u);
    Event removed;
    queue.push_event(overflow, &removed);
    ASSERT_TRUE(removed.is_valid());
    EXPECT_EQ(removed.event_id(), 999u);
}

TEST(EventQueueTest, remove_all_events_empties_queue)
{
    ReadyQueue queue(0u);
    for (uint32_t i = 0u; i < 50u; ++i)
    {
        Event evt = makeEvent(i);
        queue.push_event(evt);
    }
    Event high = makeEvent(100u, EventPriority::HighPrio);
    queue.push_event(high);

    queue.remove_all_events();
    EXPECT_FALSE(queue.has_pending());
    EXPECT_FALSE(queue.pop_event().is_valid());
}

//////////////////////////////////////////////////////////////////////////
// Doorbell wake-up
//////////////////////////////////////////////////////////////////////////

TEST(EventQueueTest, wait_event_wakes_on_push)
{
    ReadyQueue queue(0u);
    std::atomic<bool> woke{ false };
    std::thread consumer([&]
    {
        woke.store(queue.wait_event(areg::WAIT_INFINITE), std::memory_order_release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_FALSE(woke.load(std::memory_order_acquire));  // still blocked: queue empty

    Event evt = makeEvent(1u);
    queue.push_event(evt);
    consumer.join();
    EXPECT_TRUE(woke.load(std::memory_order_acquire));
}

TEST(EventQueueTest, wait_event_wakes_on_exit)
{
    ReadyQueue queue(0u);
    std::atomic<bool> sawExit{ false };
    std::thread consumer([&]
    {
        queue.wait_event(areg::WAIT_INFINITE);
        sawExit.store(queue.is_exit_triggered(), std::memory_order_release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    queue.trigger_exit();
    consumer.join();
    EXPECT_TRUE(sawExit.load(std::memory_order_acquire));
}

//////////////////////////////////////////////////////////////////////////
// Multi-threaded stress
//////////////////////////////////////////////////////////////////////////

// Single producer drives one event at a time while the consumer blocks on
// wait_event(WAIT_INFINITE). A missed wake-up would stall the consumer until
// the watchdog exit fires, leaving consumed < ITERS and failing the test.
TEST(EventQueueTest, blocking_consumer_no_lost_wakeup)
{
    ReadyQueue queue(0u);
    constexpr uint32_t ITERS{ 200000u };
    std::atomic<uint32_t> consumed{ 0u };

    std::thread consumer([&]
    {
        for (;;)
        {
            queue.wait_event(areg::WAIT_INFINITE);
            bool exit = false;
            for (;;)
            {
                Event evt = queue.pop_event();
                if (!evt.is_valid())
                    break;
                if (evt.is_exit_prio())
                {
                    exit = true;
                    break;
                }
                consumed.fetch_add(1u, std::memory_order_release);
            }
            if (exit)
                break;
        }
    });

    for (uint32_t i = 0u; i < ITERS; ++i)
    {
        Event evt = makeEvent(i);
        queue.push_event(evt);
    }

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);
    while ((consumed.load(std::memory_order_acquire) < ITERS) && (std::chrono::steady_clock::now() < deadline))
        std::this_thread::yield();

    queue.trigger_exit();   // stop the consumer (real or watchdog wake-up)
    consumer.join();

    EXPECT_EQ(consumed.load(std::memory_order_acquire), ITERS);
}

// Many producers, single consumer: verify every event is delivered exactly once.
TEST(EventQueueTest, mpsc_stress_no_event_loss)
{
    constexpr uint32_t PRODUCERS{ 4u };
    constexpr uint32_t PER_PRODUCER{ 50000u };
    constexpr uint32_t TOTAL{ PRODUCERS * PER_PRODUCER };

    ReadyQueue queue(0u);   // unlimited capacity
    std::atomic<uint32_t> ready{ 0u };
    std::atomic<bool> go{ false };
    std::atomic<uint32_t> received{ 0u };
    std::atomic<bool> duplicate{ false };
    std::atomic<bool> outOfRange{ false };

    std::vector<std::thread> producers;
    for (uint32_t p = 0u; p < PRODUCERS; ++p)
    {
        producers.emplace_back([&, p]
        {
            ready.fetch_add(1u, std::memory_order_release);
            while (!go.load(std::memory_order_acquire))
                std::this_thread::yield();

            for (uint32_t i = 0u; i < PER_PRODUCER; ++i)
            {
                Event evt = makeEvent((p * PER_PRODUCER) + i);
                queue.push_event(evt);
            }
        });
    }

    std::vector<uint8_t> seen(TOTAL, 0u);   // consumer-thread-only; no data race
    std::thread consumer([&]
    {
        bool exit = false;
        while ((received.load(std::memory_order_relaxed) < TOTAL) && !exit)
        {
            queue.wait_event(areg::WAIT_INFINITE);
            for (;;)
            {
                Event evt = queue.pop_event();
                if (!evt.is_valid())
                    break;
                if (evt.is_exit_prio())
                {
                    exit = true;
                    break;
                }

                const uint32_t tag = evt.event_id();
                if (tag >= TOTAL)
                {
                    outOfRange.store(true, std::memory_order_relaxed);
                }
                else if (seen[tag] != 0u)
                {
                    duplicate.store(true, std::memory_order_relaxed);
                }
                else
                {
                    seen[tag] = 1u;
                    received.fetch_add(1u, std::memory_order_release);
                }
            }
        }
    });

    while (ready.load(std::memory_order_acquire) < PRODUCERS)
        std::this_thread::yield();
    go.store(true, std::memory_order_release);

    for (std::thread& t : producers)
        t.join();

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
    while ((received.load(std::memory_order_acquire) < TOTAL) && (std::chrono::steady_clock::now() < deadline))
        std::this_thread::yield();

    queue.trigger_exit();   // stop the consumer (real or watchdog wake-up)
    consumer.join();

    EXPECT_FALSE(duplicate.load(std::memory_order_relaxed));
    EXPECT_FALSE(outOfRange.load(std::memory_order_relaxed));
    EXPECT_EQ(received.load(std::memory_order_acquire), TOTAL);
}

//////////////////////////////////////////////////////////////////////////
// Lane lifecycle
//////////////////////////////////////////////////////////////////////////

TEST(EventQueueTest, no_lanes_before_acquire)
{
    EventQueue queue(0u);
    Event evt = makeEvent(1u);

    EXPECT_FALSE(queue.push_event(evt));
    EXPECT_TRUE(evt.is_valid());            // rejected, so the caller keeps it
    EXPECT_FALSE(queue.has_pending());
    EXPECT_FALSE(queue.pop_event().is_valid());
}

TEST(EventQueueTest, release_lanes_stops_taking_events)
{
    ReadyQueue queue(0u);
    Event queued = makeEvent(7u);
    EXPECT_TRUE(queue.push_event(queued));

    queue.release_lanes();                  // drops the queued event with the ring

    Event evt = makeEvent(8u);
    EXPECT_FALSE(queue.push_event(evt));
    EXPECT_TRUE(evt.is_valid());
    EXPECT_FALSE(queue.pop_event().is_valid());
}

TEST(EventQueueTest, lanes_reacquired_after_release)
{
    ReadyQueue queue(0u);
    Event first = makeEvent(11u);
    EXPECT_TRUE(queue.push_event(first));

    queue.release_lanes();
    queue.acquire_lanes();
    queue.reset_exit();                     // the owner dispatcher does this when it restarts

    Event second = makeEvent(12u);
    EXPECT_TRUE(queue.push_event(second));

    Event out = queue.pop_event();
    ASSERT_TRUE(out.is_valid());
    EXPECT_EQ(out.event_id(), 12u);         // the pre-release event is gone
    EXPECT_FALSE(queue.pop_event().is_valid());
}

TEST(EventQueueTest, lanes_survive_a_restart_without_release)
{
    // A dispatcher that stops keeps its ring: the acquire on restart is a no-op and
    // the queue works again once the owner cleared the exit flag.
    ReadyQueue queue(0u);
    Event first = makeEvent(11u);
    EXPECT_TRUE(queue.push_event(first));
    EXPECT_TRUE(queue.pop_event().is_valid());

    queue.trigger_exit();
    queue.reset_exit();
    queue.acquire_lanes();

    Event second = makeEvent(12u);
    EXPECT_TRUE(queue.push_event(second));

    Event out = queue.pop_event();
    ASSERT_TRUE(out.is_valid());
    EXPECT_EQ(out.event_id(), 12u);
}

TEST(EventQueueTest, close_lanes_shuts_out_live_producers)
{
    // The guarantee the owner dispatcher needs: once close_lanes() returns, no producer
    // is inside the ring and none can get in, even under a live push storm.
    constexpr uint32_t PRODUCERS{ 4u };

    ReadyQueue               queue(64u, false, 50u);
    std::atomic<bool>        stop { false };
    std::atomic<uint32_t>    ready{ 0u };
    std::atomic<uint32_t>    acceptedAfterClose{ 0u };
    std::atomic<bool>        closed{ false };
    std::vector<std::thread> producers;

    for (uint32_t p = 0u; p < PRODUCERS; ++p)
    {
        producers.emplace_back([&]()
        {
            ready.fetch_add(1u, std::memory_order_release);
            while (!stop.load(std::memory_order_acquire))
            {
                Event evt = makeEvent(1u);
                const bool taken{ queue.push_event(evt) };
                if (taken && closed.load(std::memory_order_acquire))
                    acceptedAfterClose.fetch_add(1u, std::memory_order_relaxed);
            }
        });
    }

    while (ready.load(std::memory_order_acquire) < PRODUCERS)
        std::this_thread::yield();

    // Let the storm build, draining so the producers keep getting slots.
    for (uint32_t i = 0u; i < 2000u; ++i)
        static_cast<void>(queue.pop_event());

    queue.close_lanes();
    closed.store(true, std::memory_order_release);

    // Give the producers ample time to try again against the closed queue.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_TRUE(queue.is_closed());
    Event evt = makeEvent(2u);
    EXPECT_FALSE(queue.push_event(evt));
    EXPECT_TRUE(evt.is_valid());                // refused, so the caller keeps it

    Event high = makeEvent(3u, EventPriority::HighPrio);
    EXPECT_FALSE(queue.push_event(high));       // the priority lane is shut too

    stop.store(true, std::memory_order_release);
    for (std::thread& t : producers)
        t.join();

    EXPECT_EQ(acceptedAfterClose.load(std::memory_order_relaxed), 0u);

    // Reopening restores service on the same ring.
    queue.acquire_lanes();
    EXPECT_FALSE(queue.is_closed());
    queue.remove_all_events();                  // the storm left the small ring full
    Event again = makeEvent(4u);
    EXPECT_TRUE(queue.push_event(again));
    EXPECT_EQ(queue.pop_event().event_id(), 4u);
}

TEST(EventQueueTest, mpsc_stress_across_lane_cycle)
{
    // release_lanes() is quiescent-only. Run a full many-producer round, release the
    // lanes while nothing is inside the queue, acquire them again and repeat: the
    // second round must lose nothing either.
    constexpr uint32_t PRODUCERS   { 4u };
    constexpr uint32_t PER_PRODUCER{ 20000u };
    constexpr uint32_t TOTAL       { PRODUCERS * PER_PRODUCER };

    ReadyQueue queue(0u);

    auto oneRound = [&queue]() -> uint32_t
    {
        std::atomic<uint32_t> received{ 0u };
        std::atomic<bool>     go      { false };

        std::thread consumer([&queue, &received]()
        {
            while (received.load(std::memory_order_relaxed) < TOTAL)
            {
                Event evt{ queue.pop_event() };
                if (evt.is_valid())
                    received.fetch_add(1u, std::memory_order_relaxed);
                else
                    std::this_thread::yield();
            }
        });

        std::vector<std::thread> producers;
        for (uint32_t p = 0u; p < PRODUCERS; ++p)
        {
            producers.emplace_back([&queue, &go]()
            {
                while (!go.load(std::memory_order_acquire))
                    std::this_thread::yield();

                for (uint32_t i = 0u; i < PER_PRODUCER; ++i)
                {
                    Event evt = makeEvent(i);
                    while (!queue.push_event(evt))
                        std::this_thread::yield();
                }
            });
        }

        go.store(true, std::memory_order_release);
        for (std::thread& t : producers)
            t.join();

        consumer.join();
        return received.load(std::memory_order_relaxed);
    };

    EXPECT_EQ(oneRound(), TOTAL);

    // Every producer joined and the consumer drained, so the queue is quiet here.
    queue.release_lanes();
    Event rejected = makeEvent(1u);
    EXPECT_FALSE(queue.push_event(rejected));

    queue.acquire_lanes();
    EXPECT_EQ(oneRound(), TOTAL);
}
