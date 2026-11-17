/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ChunkQueueTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of the ChunkQueue class template.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/ChunkQueue.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"

#include <utility>

namespace
{
    //!< An element that counts the live instances and remembers its value.
    struct Counted
    {
        static inline int32_t   sAlive  { 0 };  //!< The live instances.
        int32_t                 value   { 0 };  //!< The value, -1 after a move.

        explicit Counted(int32_t init) noexcept
            : value(init)
        {
            ++ sAlive;
        }

        Counted(const Counted & src) noexcept
            : value(src.value)
        {
            ++ sAlive;
        }

        Counted(Counted && src) noexcept
            : value(src.value)
        {
            src.value = -1;
            ++ sAlive;
        }

        ~Counted() noexcept
        {
            -- sAlive;
        }

        Counted & operator = (const Counted &) = delete;
    };

    //!< Small blocks, so that a few elements span several of them.
    using Queue = areg::ChunkQueue<Counted, 4u>;

    //!< Returns true if the queue holds the values first, first + 1, ... in order.
    bool holds_in_order(const Queue & queue, int32_t first, uint32_t count)
    {
        uint32_t index{ 0u };
        for (const Counted & elem : queue)
        {
            if ((index >= count) || (elem.value != first + static_cast<int32_t>(index)))
                return false;

            ++ index;
        }

        return (index == count) && (queue.size() == count);
    }
}

/**
 * \brief   An empty queue allocates nothing, holds nothing and its iteration is empty.
 **/
TEST(ChunkQueueTest, empty_queue_has_no_element)
{
    const Queue queue;
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);
    EXPECT_TRUE(queue.begin() == queue.end());
    EXPECT_FALSE(queue.begin() != queue.end());
}

/**
 * \brief   Elements leave in the order they came in, across block borders, and each is
 *          destroyed exactly once.
 **/
TEST(ChunkQueueTest, elements_leave_in_order_across_blocks)
{
    constexpr uint32_t COUNT{ 3u * 4u + 2u };
    {
        Queue queue;
        for (uint32_t i = 0u; i < COUNT; ++ i)
        {
            const Counted elem{ static_cast<int32_t>(i) };
            ASSERT_TRUE(queue.push_back(elem));
            EXPECT_EQ(elem.value, static_cast<int32_t>(i));
        }

        EXPECT_EQ(Counted::sAlive, static_cast<int32_t>(COUNT));
        EXPECT_TRUE(holds_in_order(queue, 0, COUNT));

        for (uint32_t i = 0u; i < COUNT; ++ i)
        {
            ASSERT_FALSE(queue.empty());
            EXPECT_EQ(queue.front().value, static_cast<int32_t>(i));
            queue.pop_front();
            EXPECT_EQ(queue.size(), COUNT - i - 1u);
        }

        EXPECT_TRUE(queue.empty());
        EXPECT_TRUE(queue.begin() == queue.end());
        EXPECT_EQ(Counted::sAlive, 0);
    }

    EXPECT_EQ(Counted::sAlive, 0);
}

/**
 * \brief   Pushes and pops interleaved keep the order while blocks are emptied and reused,
 *          and a full last block ends the iteration at the block border.
 **/
TEST(ChunkQueueTest, interleaved_pushes_and_pops_keep_the_order)
{
    Queue queue;
    for (int32_t i = 0; i < 4; ++ i)
    {
        ASSERT_TRUE(queue.push_back(Counted{ i }));
    }

    // The only block is full: the end is the border of the block.
    EXPECT_TRUE(holds_in_order(queue, 0, 4u));

    int32_t next{ 4 };
    for (int32_t i = 0; i < 40; ++ i)
    {
        EXPECT_EQ(queue.front().value, i);
        queue.pop_front();
        ASSERT_TRUE(queue.push_back(Counted{ next ++ }));
        EXPECT_TRUE(holds_in_order(queue, i + 1, 4u));
    }

    queue.clear();
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(Counted::sAlive, 0);

    ASSERT_TRUE(queue.push_back(Counted{ 7 }));
    EXPECT_EQ(queue.front().value, 7);
    EXPECT_TRUE(holds_in_order(queue, 7, 1u));
}

/**
 * \brief   A pushed temporary is moved in, an element is modified in place through the
 *          front and the iterators, and an element keeps its address while it is queued.
 **/
TEST(ChunkQueueTest, elements_are_moved_in_and_modified_in_place)
{
    Queue queue;
    Counted source{ 5 };
    ASSERT_TRUE(queue.push_back(std::move(source)));
    EXPECT_EQ(source.value, -1);
    EXPECT_EQ(Counted::sAlive, 2);

    const Counted * const first{ &queue.front() };
    for (int32_t i = 6; i < 15; ++ i)
    {
        ASSERT_TRUE(queue.push_back(Counted{ i }));
    }

    EXPECT_EQ(&queue.front(), first);
    queue.front().value = 100;
    for (Queue::Iterator pos = queue.begin(); pos != queue.end(); ++ pos)
    {
        pos->value += 1;
    }

    const Queue & view{ queue };
    EXPECT_EQ(view.front().value, 101);
    EXPECT_EQ((*view.begin()).value, 101);
    EXPECT_TRUE(holds_in_order(queue, 101, 1u) == false);
    queue.pop_front();
    EXPECT_TRUE(holds_in_order(queue, 7, 9u));
}

/**
 * \brief   A move gives the elements to the target and leaves the source empty; a move
 *          assignment destroys the elements of the target first. A self move changes nothing.
 **/
TEST(ChunkQueueTest, move_gives_the_elements_and_empties_the_source)
{
    Queue source;
    for (int32_t i = 0; i < 6; ++ i)
    {
        ASSERT_TRUE(source.push_back(Counted{ i }));
    }

    Queue moved{ std::move(source) };
    EXPECT_TRUE(source.empty());
    EXPECT_TRUE(source.begin() == source.end());
    EXPECT_TRUE(holds_in_order(moved, 0, 6u));
    EXPECT_EQ(Counted::sAlive, 6);

    Queue assigned;
    ASSERT_TRUE(assigned.push_back(Counted{ 50 }));
    assigned = std::move(moved);
    EXPECT_TRUE(moved.empty());
    EXPECT_TRUE(holds_in_order(assigned, 0, 6u));
    EXPECT_EQ(Counted::sAlive, 6);

    Queue & self{ assigned };
    assigned = std::move(self);
    EXPECT_TRUE(holds_in_order(assigned, 0, 6u));

    ASSERT_TRUE(source.push_back(Counted{ 9 }));
    EXPECT_TRUE(holds_in_order(source, 9, 1u));
    source.clear();
    assigned.clear();
    EXPECT_EQ(Counted::sAlive, 0);
}

/**
 * \brief   A queued element shares the buffer of a message, and popping or clearing it
 *          releases the share.
 **/
TEST(ChunkQueueTest, queued_message_shares_its_buffer_until_it_leaves)
{
    areg::MessageEnvelope message;
    ASSERT_NE(message.init_envelope(areg::notify_client_connection(), 16u), nullptr);
    EXPECT_TRUE(message.is_unique());

    areg::ChunkQueue<areg::MessageEnvelope> queue;
    ASSERT_TRUE(queue.push_back(message));
    EXPECT_TRUE(message.is_shared());
    EXPECT_EQ(queue.front().header(), message.header());
    queue.pop_front();
    EXPECT_TRUE(message.is_unique());

    for (uint32_t i = 0u; i < 20u; ++ i)
    {
        ASSERT_TRUE(queue.push_back(message));
    }

    EXPECT_TRUE(message.is_shared());
    queue.clear();
    EXPECT_TRUE(queue.empty());
    EXPECT_TRUE(message.is_unique());
}
