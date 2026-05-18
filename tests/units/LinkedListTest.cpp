/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/LinkedListTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of LinkedList object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/LinkedList.hpp"
#include "areg/base/SharedBuffer.hpp"

 /**
  * \brief   Test LinkedList constructors.
  **/
TEST(LinkedListTest, test_constructors)
{
    using LinkedList = areg::LinkedList<int>;
    constexpr uint32_t count{ 10 };

    LinkedList empty, init;
    EXPECT_TRUE(empty.is_empty() && init.is_empty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        init.push_first(i);
        EXPECT_EQ(init.size(), static_cast<uint32_t>(i + 1));
    }

    EXPECT_EQ(init.size(), count);

    LinkedList copy(init);
    EXPECT_FALSE(copy.is_empty());
    EXPECT_EQ(copy.size(), count);
    EXPECT_EQ(copy, init);

    LinkedList move(std::move(init));
    EXPECT_FALSE(move.is_empty());
    EXPECT_TRUE(init.is_empty());
    EXPECT_EQ(move.size(), count);
    EXPECT_EQ(init.size(), 0u);
    EXPECT_EQ(move, copy);
    EXPECT_NE(move, init);
}

/**
 * \brief   Test LinkedList operators.
 **/
TEST(LinkedListTest, test_operators)
{
    using LinkedList = areg::LinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList src;
    EXPECT_TRUE(src.is_empty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        src.push_last(i);
        EXPECT_EQ(src[static_cast<uint32_t>(i)], i);
    }

    int i = 0;
    for (POS pos = src.first_position(); src.is_valid_position(pos); pos = src.next_position(pos))
    {
        EXPECT_EQ(src[pos], i);
        ++i;
    }

    LinkedList copy;
    copy = src;
    EXPECT_EQ(copy.size(), count);
    EXPECT_EQ(copy, src);
    EXPECT_TRUE(copy == src);

    LinkedList move;
    move = std::move(src);
    EXPECT_TRUE(src.is_empty());
    EXPECT_EQ(move.size(), count);
    EXPECT_EQ(move, copy);
    EXPECT_NE(move, src);
    EXPECT_TRUE(move == copy);
    EXPECT_TRUE(move != src);
}


/**
 * \brief   Test LinkedList positioning attributes.
 **/
TEST(LinkedListTest, test_position_attributes)
{
    using LinkedList = areg::LinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList list;
    POS first = list.first_position();
    POS last = list.last_position();
    EXPECT_TRUE(!list.is_valid_position(first));
    EXPECT_FALSE(list.is_valid_position(last));
    EXPECT_EQ(first, list.invalid_position());
    EXPECT_EQ(last, list.invalid_position());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.push_last(i);
    }

    first = list.first_position();
    last = list.last_position();
    EXPECT_FALSE(!list.is_valid_position(first));
    EXPECT_TRUE(list.is_valid_position(last));

    EXPECT_TRUE(list.is_first_position(first));
    EXPECT_FALSE(list.is_last_position(first));
    EXPECT_TRUE(list.is_last_position(last));
    EXPECT_FALSE(list.is_first_position(last));

    EXPECT_TRUE(list.check_position(first));
    EXPECT_TRUE(list.check_position(last));

    EXPECT_NE(first, list.invalid_position());
    EXPECT_NE(last, list.invalid_position());

    for (POS pos = list.first_position(); list.is_valid_position(pos); pos = list.next_position(pos))
    {
        int value = list[pos];
        EXPECT_EQ(list.value_at(pos), value);
        list[pos] = static_cast<int>(count) + value;
    }

    for (POS pos = list.last_position(); list.is_valid_position(pos); pos = list.prev_position(pos))
    {
        EXPECT_TRUE(list.check_position(pos));
        int value = list.value_at(pos);
        EXPECT_EQ(list[pos], value);
    }

    POS next = list.first_position();
    while (list.is_valid_position(next))
    {
        int value = list.next(next);
        EXPECT_TRUE(!list.is_valid_position(next) || ((value + 1) == list.value_at(next)));
    }

    POS prev = list.last_position();
    while (list.is_valid_position(prev))
    {
        int value = list.prev(prev);
        EXPECT_TRUE(!list.is_valid_position(prev) || ((value - 1) == list.value_at(prev)));
    }
}

/**
 * \brief   Test LinkedList positioning operations.
 **/
TEST(LinkedListTest, test_position_operations)
{
    using LinkedList = areg::LinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList list;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.push_last(i);
    }

    EXPECT_EQ(list[0], list.first_entry());
    EXPECT_EQ(list[static_cast<int>(count) - 1], list.last_entry());

    POS next = list.first_position();
    while (list.is_valid_position(next))
    {
        int value = list.value_at(next);
        list.next(next) = static_cast<int>(count) + value;
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(list[i], static_cast<int>(count + i));
    }

    POS prev = list.last_position();
    while (list.is_valid_position(prev))
    {
        int value = list.value_at(prev);
        list.prev(prev) = value - static_cast<int>(count);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(list.value_at(i), static_cast<int>(i));
    }
}

/**
 * \brief   Test LinkedList positioning operations.
 **/
TEST(LinkedListTest, test_position_manipulation)
{
    using LinkedList = areg::LinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList list;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.push_last(i);
    }

    POS next = list.first_position();
    POS prev = list.last_position();
    uint32_t cycles = 0;
    while (list.is_valid_position(next) && list.is_valid_position(prev))
    {
        ++cycles;
        EXPECT_TRUE(cycles <= count);

        int valNext {-1};
        EXPECT_TRUE(list.next_entry(next, valNext) || (cycles == count));
        EXPECT_NE(valNext, list.first_entry());
        EXPECT_TRUE((valNext == static_cast<int>(cycles)) || (cycles == count));

        int valPrev{ -1 };
        EXPECT_TRUE(list.prev_entry(prev, valPrev) || (cycles == count));
        EXPECT_NE(valPrev, list.last_entry());
        EXPECT_TRUE((valPrev == static_cast<int>(count - (cycles + 1u))) || (cycles == count));
    }

    cycles = 0;
    for (POS pos = list.last_position(); list.is_valid_position(pos); pos = list.prev_position(pos))
    {
        cycles += 1;
        int value = list.value_at(pos);
        list.set_value_at(pos, static_cast<int>(count) + value);
        EXPECT_EQ(static_cast<int>(count) + value, list.value_at(pos));
    }

    EXPECT_EQ(cycles, count);
    cycles = 0;
    for (int i = static_cast<int>(count - 1); i >= 0; --i)
    {
        POS pos = list.last_position();
        EXPECT_TRUE(list.is_valid_position(pos));
        list.remove_at(pos);
    }

    EXPECT_TRUE(list.is_empty());
}

/**
 * \brief   Test LinkedList searching functionalities.
 **/
TEST(LinkedListTest, test_searching)
{
    using LinkedList = areg::LinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList list;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_FALSE(list.contains(i));
        list.push_last(i);
        EXPECT_TRUE(list.contains(i));
    }

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        POS valid = list.find(i);
        EXPECT_TRUE(list.is_valid_position(valid));

        POS invalid = list.find(static_cast<int>(count) + i);
        EXPECT_TRUE(!list.is_valid_position(invalid));
    }

    list.clear();
    EXPECT_TRUE(list.is_empty());
}

/**
 * \brief   Test LinkedList merging.
 **/
TEST(LinkedListTest, test_merging)
{
    using LinkedList = areg::LinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 linked lists with entries [0 .. 9] and [10 .. 19]
    LinkedList list1, list2;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list1.push_last(i);      //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
        list2.push_first(i + 10); // 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
    }

    // Step 2:  
    //  - create empty linked-list object `list`
    //  - merge [0 .. 1] entries with `list`.
    // Result: the source object is empty, all entries are extracted and merged
    LinkedList list;
    list.merge(list1);
    EXPECT_TRUE(list1.is_empty());
    EXPECT_FALSE(list.is_empty());
    EXPECT_NE(list1, list);
    EXPECT_EQ(list.size(), count);

    // Step 3: 
    //  - assign entries [0 .. 9] to the empty linked-list
    //  - merge [0 .. 1] entries with `list`, which already contains the keys [0 .. 9]
    //  Result: the entries in the linked-list are doubled.
    list1 = list;
    EXPECT_EQ(list1, list);
    list.merge(std::move(list1));
    EXPECT_TRUE(list1.is_empty());
    EXPECT_FALSE(list.is_empty());
    EXPECT_NE(list1, list);
    EXPECT_EQ(list.size(), 2u * count);
    for (POS pos = list.first_position(); list.is_valid_position(pos); pos = list.next_position(pos))
    {
        int val1 = list.value_at(pos);
        int val2{ -1 };
        EXPECT_TRUE(list.next_entry(pos, val2));
        EXPECT_EQ(val1, val2);
    }

    // To merge, the LinkedList entries should be sorted. Check it.
    LinkedList listUnorder;
    listUnorder.push_first(3);
    listUnorder.push_first(2);
    listUnorder.push_first(5);
    listUnorder.push_first(1);
    listUnorder.push_first(4);
    uint32_t unordSize = listUnorder.size();
    unordSize += list.size();
    listUnorder.merge(list);
    EXPECT_TRUE(list.is_empty());
    EXPECT_EQ(listUnorder.size(), unordSize);
    int valList = listUnorder[0];
    for (POS pos = list.first_position(); list.is_valid_position(pos); pos = list.next_position(pos))
    {
        EXPECT_LE(valList, list.value_at(pos));
        valList = list.value_at(pos);
    }

    // re-initialize the list with entries [0 .. 9]
    list.clear();
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.push_last(i);      //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
    }

    // Step 4:
    //  - merge entries with keys [10 .. 19] by using `move` operation.
    // Result: the source linked-list is empty, the `list` contains entries [0 .. 19]
    list.merge(std::move(list2));
    EXPECT_TRUE(list2.is_empty());
    EXPECT_FALSE(list.is_empty());
    EXPECT_NE(list2, list);
    EXPECT_NE(list1, list);

    // Step 5:
    //  - create and initialize 2 linked-lists, which values are odd and even integers from 1 to 42 and have 21 entries each.
    constexpr int32_t odds[]    { 1, 3, 5, 7,  9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41 };
    constexpr int32_t evens[]   { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42 };
    constexpr uint32_t lenOdd{ std::size(odds) };
    constexpr uint32_t lenEven{ std::size(odds) };
    LinkedList listOdd, listEven;
    for (uint32_t i = 0u; i < lenOdd; ++i)
    {
        listOdd.push_last(odds[i]);
    }
    for (uint32_t i = 0u; i < lenEven; ++i)
    {
        listEven.push_last(evens[i]);
    }

    // Step 6:
    //  - merge odd linked-list with the `list`
    //  Result:
    //      - the odd entries are in the `list`, including all odd entries in the range [1 .. 19]
    uint32_t listSize = list.size();
    list.merge(listOdd);
    EXPECT_TRUE(listOdd.is_empty());
    EXPECT_EQ(listOdd.size(), 0u);
    EXPECT_EQ(list.size(), listSize + lenOdd);

    // Step 6:
    //  - merge even linked-list with the `list`
    //  Result:
    //      - the even entries are in the `list`, including all even entries in the range [1 .. 19]
    list.merge(std::move(listEven));
    EXPECT_TRUE(listEven.is_empty());
    EXPECT_EQ(listEven.size(), 0u);
    EXPECT_EQ(list.size(), listSize + lenOdd + lenEven);

    // Step 7: make checkups. Entries in range [1 .. 19] are doubled. Other entries are not.
    for (uint32_t i = 0; i < list.size(); ++i)
    {
        int value = list.value_at(i);
        POS pos = list.find(value);
        EXPECT_TRUE(list.is_valid_position(pos));
        POS next = list.find(value, pos);
        if ((value > 0) && (value < static_cast<int>(2u * count)))
        {
            EXPECT_TRUE(list.is_valid_position(next));
        }
        else
        {
            EXPECT_TRUE(!list.is_valid_position(next));
        }
    }

    EXPECT_FALSE(list.contains(static_cast<int>(list.size())));
}

/**
 * \brief   Test pushXXXIfUnique method of the LinkedList object.
 **/
TEST(LinkedListTest, test_push_unique)
{
    using LinkedList = areg::LinkedList<int>;
    constexpr uint32_t count{ 10 };

    constexpr int unique[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int notunique[]{ 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };

    EXPECT_EQ(count, std::size(unique));
    EXPECT_EQ(count, std::size(notunique));

    LinkedList list;
    for (uint32_t i = 0; i < count; ++i)
    {
        bool result = list.push_first_unique(notunique[i]);
        if (i < 5)
        {
            EXPECT_TRUE(result);
        }
        else
        {
            EXPECT_FALSE(result);
        }
    }

    list.release();
    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_TRUE(list.push_last_unique(unique[i]));
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_FALSE(list.push_first_unique(notunique[i], true));
    }
}

/**
 * \brief   Test insertAfter and insertBefore methods of the LinkedList object.
 **/
TEST(LinkedListTest, test_inserting)
{
    using LinkedList = areg::LinkedList<int>;
    using POS = LinkedList::LISTPOS;

    constexpr int notunique[]{ 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5 };
    constexpr uint32_t count{ std::size(notunique) };
    constexpr uint32_t len{ count / 2u };

    LinkedList list;
    for (uint32_t i = 0; i < len; ++i)
    {
        POS pos = list.invalid_position();
        bool before = (i % 2u) == 0;
        if (before)
        {
            pos = list.insert_before(list.last_position(), notunique[i]);
        }
        else
        {
            pos = list.insert_after(list.first_position(), notunique[i]);
        }

        EXPECT_TRUE(list.check_position(pos));
        EXPECT_TRUE(list.is_valid_position(pos));
        EXPECT_EQ(notunique[i], list.value_at(pos));
    }

    for (uint32_t i = len; i < count; ++i)
    {
        POS found = list.find(notunique[i]);
        POS pos = list.invalid_position();
        EXPECT_TRUE(list.is_valid_position(found));
        bool before = (i % 2u) == 0;
        if (before)
        {
            pos = list.insert_before(found, notunique[i]);
            EXPECT_TRUE(list.value_at(pos) == list.value_at(found));
        }
        else
        {
            pos = list.insert_after(found, notunique[i]);
            EXPECT_TRUE(list.value_at(pos) == list.value_at(found));
        }

        EXPECT_TRUE(list.check_position(pos));
        EXPECT_TRUE(list.is_valid_position(pos));
        EXPECT_EQ(notunique[i], list.value_at(pos));
    }
}

/**
 * \brief   Test remove first / last methods of the LinkedList object.
 **/
TEST(LinkedListTest, test_remove_entries)
{
    using LinkedList = areg::LinkedList<int>;
    constexpr int32_t count{ 10 };

    LinkedList list;
    for (int i = 0; i < count; ++i)
    {
        list.push_last(i);
    }

    EXPECT_EQ(list.size(), static_cast<uint32_t>(count));
    for (int i = 0; i < (count / 2); ++i)
    {
        int first{ -1 }, last{ -1 };
        EXPECT_TRUE(list.remove_first(first));
        EXPECT_TRUE(list.remove_last(last));
        EXPECT_EQ(i, first);
        EXPECT_EQ(count - (i + 1), last);
    }

    EXPECT_TRUE(list.is_empty());
}

/**
 * \brief   Test LinkedList streaming operators.
 **/
TEST(LinkedListTest, test_streaming)
{
    using LinkedList = areg::LinkedList<int>;
    constexpr int32_t count{ 10 };

    LinkedList src;
    for (int i = 0; i < count; ++i)
    {
        src.push_last(i);
    }

    areg::SharedBuffer stream;
    stream << src;

    EXPECT_FALSE(stream.is_empty());

    stream.move_to_begin();
    LinkedList dst;
    stream >> dst;

    EXPECT_EQ(src, dst);
}

/**
 * \brief   Test LinkedList streaming operators.
 **/
TEST(LinkedListTest, test_ascending)
{
    using LinkedList = areg::LinkedList<int>;
    constexpr int _arr1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr int _res1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr uint32_t _len{ std::size(_arr1) };
    int _dat1[_len]{};

    LinkedList list1(_arr1, _len), res1(_res1, _len);
    list1.sort([](const int elem1, const int elem2) { return (elem1 < elem2); });
    list1.elements(_dat1, _len);
    EXPECT_NE(areg::mem_compare(_dat1, _arr1, _len), areg::Ordering::Equal);
    EXPECT_EQ(list1.size(), _len);
    EXPECT_EQ(areg::mem_compare(_dat1, _res1, _len), areg::Ordering::Equal);
}

/**
 * \brief   Test LinkedList streaming operators.
 **/
TEST(LinkedListTest, test_descending)
{
    using LinkedList = areg::LinkedList<int>;
    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int _res1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr uint32_t _len{ std::size(_arr1) };
    int _dat1[_len]{};

    LinkedList list1(_arr1, _len), res1(_res1, _len);
    list1.sort([](const int elem1, const int elem2) { return (elem1 > elem2); });
    list1.elements(_dat1, _len);
    EXPECT_NE(areg::mem_compare(_dat1, _arr1, _len), areg::Ordering::Equal);
    EXPECT_EQ(list1.size(), _len);
    EXPECT_EQ(areg::mem_compare(_dat1, _res1, _len), areg::Ordering::Equal);
}
