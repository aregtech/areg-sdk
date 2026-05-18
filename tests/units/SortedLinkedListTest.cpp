/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/SortedLinkedListTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of SortedLinkedList object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SortedLinkedList.hpp"
#include "areg/base/SharedBuffer.hpp"

 /**
  * \brief   Test SortedLinkedList constructors.
  **/
TEST(SortedLinkedListTest, test_constructors)
{
    using SortedList = areg::SortedLinkedList<int>;
    constexpr uint32_t count{ 10 };

    SortedList empty, init;
    EXPECT_TRUE(empty.is_empty() && init.is_empty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        init.add(i);
        EXPECT_EQ(init.size(), static_cast<uint32_t>(i + 1));
    }

    EXPECT_EQ(init.size(), count);

    SortedList copy(init);
    EXPECT_FALSE(copy.is_empty());
    EXPECT_EQ(copy.size(), count);
    EXPECT_EQ(copy, init);

    SortedList move(std::move(init));
    EXPECT_FALSE(move.is_empty());
    EXPECT_TRUE(init.is_empty());
    EXPECT_EQ(move.size(), count);
    EXPECT_EQ(init.size(), 0u);
    EXPECT_EQ(move, copy);
    EXPECT_NE(move, init);
}

/**
 * \brief   Test SortedLinkedList operators.
 **/
TEST(SortedLinkedListTest, test_operators)
{
    using SortedList = areg::SortedLinkedList<int>;
    using POS = SortedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    SortedList src(false);
    EXPECT_TRUE(src.is_empty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        src.add(i);
        EXPECT_EQ(src[0u], i);
    }

    int i = static_cast<int>(count) - 1;
    for (POS pos = src.first_position(); src.is_valid_position(pos); pos = src.next_position(pos))
    {
        EXPECT_EQ(src[pos], i);
        --i;
    }

    SortedList copy;
    copy = src;
    EXPECT_EQ(copy.size(), count);
    EXPECT_EQ(copy, src);
    EXPECT_TRUE(copy == src);

    SortedList move;
    move = std::move(src);
    EXPECT_TRUE(src.is_empty());
    EXPECT_EQ(move.size(), count);
    EXPECT_EQ(move, copy);
    EXPECT_NE(move, src);
    EXPECT_TRUE(move == copy);
    EXPECT_TRUE(move != src);
}


/**
 * \brief   Test SortedLinkedList positioning attributes.
 **/
TEST(SortedLinkedListTest, test_position_attributes)
{
    using SortedList = areg::SortedLinkedList<int>;
    using POS = SortedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    SortedList list(false);
    POS first = list.first_position();
    POS last = list.last_position();
    EXPECT_TRUE(!list.is_valid_position(first));
    EXPECT_FALSE(list.is_valid_position(last));
    EXPECT_EQ(first, list.invalid_position());
    EXPECT_EQ(last, list.invalid_position());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.add(i);
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
        EXPECT_TRUE(!list.is_valid_position(next) || ((value - 1) == list.value_at(next)));
    }

    POS prev = list.last_position();
    while (list.is_valid_position(prev))
    {
        int value = list.prev(prev);
        EXPECT_TRUE(!list.is_valid_position(prev) || ((value + 1) == list.value_at(prev)));
    }
}

/**
 * \brief   Test SortedLinkedList searching functionalities.
 **/
TEST(SortedLinkedListTest, test_searching)
{
    using SortedList = areg::SortedLinkedList<int>;
    using POS = SortedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    SortedList list(true);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_FALSE(list.contains(i));
        list.add(i);
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
 * \brief   Test SortedLinkedList merging.
 **/
TEST(SortedLinkedListTest, test_merging)
{
    using SortedList = areg::SortedLinkedList<int>;
    using POS = SortedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 linked lists with entries [0 .. 9] and [10 .. 19]
    SortedList list1(true), list2(false);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list1.add(i);      //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
        list2.add(i + 10); // 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
    }

    // Step 2:  
    //  - create empty linked-list object `list`
    //  - merge [0 .. 1] entries with `list`.
    // Result: the source object is empty, all entries are extracted and merged
    SortedList list(true);
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

    SortedList listUnorder(!list.is_ascending());
    listUnorder.add(3);
    listUnorder.add(2);
    listUnorder.add(5);
    listUnorder.add(1);
    listUnorder.add(4);
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
    EXPECT_TRUE(list.is_empty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.add(i);      //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
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
    SortedList listOdd(true), listEven(false);
    for (uint32_t i = 0u; i < lenOdd; ++i)
    {
        listOdd.add(odds[i]);
    }
    for (uint32_t i = 0u; i < lenEven; ++i)
    {
        listEven.add(evens[i]);
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
 * \brief   Test addIfUnique method of the SortedLinkedList object.
 **/
TEST(SortedLinkedListTest, test_add_if_unique)
{
    using SortedList = areg::SortedLinkedList<int>;
    constexpr uint32_t count{ 10 };

    constexpr int unique[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int notunique[]{ 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };

    EXPECT_EQ(count, std::size(unique));
    EXPECT_EQ(count, std::size(notunique));

    SortedList list(true);
    for (uint32_t i = 0; i < count; ++i)
    {
        bool result = list.add_if_unique(notunique[i]).second;
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
        EXPECT_TRUE(list.add_if_unique(unique[i]).second);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_FALSE(list.add_if_unique(notunique[i], true).second);
    }
}

/**
 * \brief   Test remove first / last methods of the SortedLinkedList object.
 **/
TEST(SortedLinkedListTest, test_remove_entries)
{
    using SortedList = areg::SortedLinkedList<int>;
    constexpr int32_t count{ 10 };

    SortedList asc(true);
    SortedList desc(false);
    for (int i = 0; i < count; ++i)
    {
        asc.add(i);
        desc.add(i);
    }

    EXPECT_NE(asc, desc);
    EXPECT_EQ(asc.size(), static_cast<uint32_t>(count));
    EXPECT_EQ(desc.size(), static_cast<uint32_t>(count));

    for (int i = 0; i < (count / 2); ++i)
    {
        int ascFirst{ -1 }, ascLast{ -1 };
        EXPECT_TRUE(asc.remove_first(ascFirst));
        EXPECT_TRUE(asc.remove_last(ascLast));
        EXPECT_EQ(i, ascFirst);
        EXPECT_EQ(count - (i + 1), ascLast);

        int descFirst{ -1 }, descLast{ -1 };
        EXPECT_TRUE(desc.remove_first(descFirst));
        EXPECT_TRUE(desc.remove_last(descLast));
        EXPECT_EQ(count - (i + 1), descFirst);
        EXPECT_EQ(i, descLast);

    }

    EXPECT_TRUE(asc.is_empty());
    EXPECT_TRUE(desc.is_empty());
}

/**
 * \brief   Test ascending and descending behavior of the SortedLinkedList object.
 **/
TEST(SortedLinkedListTest, test_sorting_entries)
{
    using SortedList = areg::SortedLinkedList<int>;
    using POS = SortedList::LISTPOS;

    constexpr int _list[]{ 5, 1, 0, 8, 3, 5, 6, 1, 5, 8, 2, 8, 9, 4, 7, 0 };
    constexpr int _asc[] { 0, 0, 1, 1, 2, 3, 4, 5, 5, 5, 6, 7, 8, 8, 8, 9 };
    constexpr int _desc[]{ 9, 8, 8, 8, 7, 6, 5, 5, 5, 4, 3, 2, 1, 1, 0, 0 };
    constexpr int32_t count{ std::size(_list) };

    SortedList asc(true);
    SortedList desc(false);
    for (int i = 0; i < count; ++i)
    {
        asc.add(_list[static_cast<uint32_t>(i)]);
        desc.add(_list[static_cast<uint32_t>(i)]);
    }

    EXPECT_NE(asc, desc);
    EXPECT_EQ(asc.size(), static_cast<uint32_t>(count));
    EXPECT_EQ(desc.size(), static_cast<uint32_t>(count));

    POS posAsc = asc.first_position();
    POS revAsc = asc.last_position();

    POS posDesc = desc.first_position();
    POS revDesc = desc.last_position();

    for (int i = 0; i < count; ++i)
    {
        EXPECT_EQ(asc[static_cast<uint32_t>(i)], _asc[static_cast<uint32_t>(i)]);
        EXPECT_EQ(desc[static_cast<uint32_t>(i)], _desc[static_cast<uint32_t>(i)]);

        EXPECT_TRUE(asc.is_valid_position(posAsc));
        EXPECT_TRUE(asc.is_valid_position(revAsc));
        EXPECT_TRUE(desc.is_valid_position(posDesc));
        EXPECT_TRUE(desc.is_valid_position(revDesc));

        EXPECT_EQ(asc.value_at(posAsc), _asc[static_cast<uint32_t>(i)]);
        EXPECT_EQ(desc.value_at(posDesc), _desc[static_cast<uint32_t>(i)]);
        EXPECT_EQ(desc.value_at(revDesc), asc.value_at(posAsc));
        EXPECT_EQ(asc.value_at(revAsc), desc.value_at(posDesc));

        posAsc = asc.next_position(posAsc);
        revAsc = asc.prev_position(revAsc);

        posDesc = desc.next_position(posDesc);
        revDesc = desc.prev_position(revDesc);
    }
}

/**
 * \brief   Test SortedLinkedList streaming operators.
 **/
TEST(SortedLinkedListTest, test_streaming)
{
    using SortedList = areg::SortedLinkedList<int>;
    constexpr int32_t count{ 10 };

    SortedList src(false);
    for (int i = 0; i < count; ++i)
    {
        src.add(i);
    }

    areg::SharedBuffer stream;
    stream << src;

    EXPECT_FALSE(stream.is_empty());

    stream.move_to_begin();
    SortedList dst(true);
    stream >> dst;

    EXPECT_EQ(src, dst);
}
