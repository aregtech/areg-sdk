/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan).
 * Areg SDK is dual-licensed under Apache 2.0 and commercial licenses.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/EventDefs.hpp"
#include "areg/component/private/EventQueue.hpp"

namespace
{
    using areg::Event;
    using areg::EventPriority;
    using areg::EventQueue;
    using areg::EventType;

    //!< An event that carries the tag as its id.
    struct TaggedEvent : public Event
    {
        explicit TaggedEvent(uint32_t tag)
            : Event(EventType::EventCustomExternal, EventPriority::NormalPrio)
        {
            set_event_id(tag);
        }
    };

    //!< Fills the ring of a queue that refuses on full, and returns the number of events it took.
    uint32_t ring_capacity_of(uint32_t requested)
    {
        EventQueue queue(requested, true);
        EXPECT_TRUE(queue.acquire_lanes());
        uint32_t taken{ 0u };
        for (;;)
        {
            Event evt{ TaggedEvent(taken) };
            if (queue.try_push_event(evt) != EventQueue::PushResult::Queued)
                break;

            ++taken;
        }

        return taken;
    }
}

TEST(EventQueueCapacityTest, a_request_of_zero_or_above_the_maximum_gets_the_default_ring)
{
    EXPECT_EQ(ring_capacity_of(0u), areg::QUEUE_DEFAULT_RING_CAPACITY);
    EXPECT_EQ(ring_capacity_of(areg::QUEUE_MAX_RING_CAPACITY + 1u), areg::QUEUE_DEFAULT_RING_CAPACITY);
}

TEST(EventQueueCapacityTest, a_small_request_gets_the_smallest_ring)
{
    EXPECT_EQ(ring_capacity_of(1u), areg::QUEUE_MIN_RING_CAPACITY);
    EXPECT_EQ(ring_capacity_of(areg::QUEUE_MIN_RING_CAPACITY - 1u), areg::QUEUE_MIN_RING_CAPACITY);
}

TEST(EventQueueCapacityTest, a_request_that_is_not_a_power_of_two_gets_the_next_power_of_two)
{
    EXPECT_EQ(ring_capacity_of(areg::QUEUE_MIN_RING_CAPACITY), areg::QUEUE_MIN_RING_CAPACITY);
    EXPECT_EQ(ring_capacity_of(100u), 128u);
    EXPECT_EQ(ring_capacity_of(129u), 256u);
}
