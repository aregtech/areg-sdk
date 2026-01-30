/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/TELinkedListTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of TELinkedList object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/TELinkedList.hpp"
#include "areg/base/SharedBuffer.hpp"

 /**
  * \brief   Test TELinkedList constructors.
  **/
TEST(TELinkedListTest, TestConstructors)
{
    using LinkedList = TELinkedList<int>;
    constexpr uint32_t count{ 10 };

    LinkedList empty, init;
    EXPECT_TRUE(empty.isEmpty() && init.isEmpty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        init.pushFirst(i);
        EXPECT_EQ(init.getSize(), static_cast<uint32_t>(i + 1));
    }

    EXPECT_EQ(init.getSize(), count);

    LinkedList copy(init);
    EXPECT_FALSE(copy.isEmpty());
    EXPECT_EQ(copy.getSize(), count);
    EXPECT_EQ(copy, init);

    LinkedList move(std::move(init));
    EXPECT_FALSE(move.isEmpty());
    EXPECT_TRUE(init.isEmpty());
    EXPECT_EQ(move.getSize(), count);
    EXPECT_EQ(init.getSize(), 0u);
    EXPECT_EQ(move, copy);
    EXPECT_NE(move, init);
}

/**
 * \brief   Test TELinkedList operators.
 **/
TEST(TELinkedListTest, TestOperators)
{
    using LinkedList = TELinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList src;
    EXPECT_TRUE(src.isEmpty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        src.pushLast(i);
        EXPECT_EQ(src[static_cast<uint32_t>(i)], i);
    }

    int i = 0;
    for (POS pos = src.firstPosition(); src.isValidPosition(pos); pos = src.nextPosition(pos))
    {
        EXPECT_EQ(src[pos], i);
        ++i;
    }

    LinkedList copy;
    copy = src;
    EXPECT_EQ(copy.getSize(), count);
    EXPECT_EQ(copy, src);
    EXPECT_TRUE(copy == src);

    LinkedList move;
    move = std::move(src);
    EXPECT_TRUE(src.isEmpty());
    EXPECT_EQ(move.getSize(), count);
    EXPECT_EQ(move, copy);
    EXPECT_NE(move, src);
    EXPECT_TRUE(move == copy);
    EXPECT_TRUE(move != src);
}


/**
 * \brief   Test TELinkedList positioning attributes.
 **/
TEST(TELinkedListTest, TestPositionAttributes)
{
    using LinkedList = TELinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList list;
    POS first = list.firstPosition();
    POS last = list.lastPosition();
    EXPECT_TRUE(list.isInvalidPosition(first));
    EXPECT_FALSE(list.isValidPosition(last));
    EXPECT_EQ(first, list.invalidPosition());
    EXPECT_EQ(last, list.invalidPosition());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.pushLast(i);
    }

    first = list.firstPosition();
    last = list.lastPosition();
    EXPECT_FALSE(list.isInvalidPosition(first));
    EXPECT_TRUE(list.isValidPosition(last));

    EXPECT_TRUE(list.isFirstPosition(first));
    EXPECT_FALSE(list.isLastPosition(first));
    EXPECT_TRUE(list.isLastPosition(last));
    EXPECT_FALSE(list.isFirstPosition(last));

    EXPECT_TRUE(list.checkPosition(first));
    EXPECT_TRUE(list.checkPosition(last));

    EXPECT_NE(first, list.invalidPosition());
    EXPECT_NE(last, list.invalidPosition());

    for (POS pos = list.firstPosition(); list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        int value = list[pos];
        EXPECT_EQ(list.valueAtPosition(pos), value);
        list[pos] = static_cast<int>(count) + value;
    }

    for (POS pos = list.lastPosition(); list.isValidPosition(pos); pos = list.prevPosition(pos))
    {
        EXPECT_TRUE(list.checkPosition(pos));
        int value = list.valueAtPosition(pos);
        EXPECT_EQ(list[pos], value);
    }

    POS next = list.firstPosition();
    while (list.isValidPosition(next))
    {
        int value = list.getNext(next);
        EXPECT_TRUE(list.isInvalidPosition(next) || ((value + 1) == list.valueAtPosition(next)));
    }

    POS prev = list.lastPosition();
    while (list.isValidPosition(prev))
    {
        int value = list.getPrev(prev);
        EXPECT_TRUE(list.isInvalidPosition(prev) || ((value - 1) == list.valueAtPosition(prev)));
    }
}

/**
 * \brief   Test TELinkedList positioning operations.
 **/
TEST(TELinkedListTest, TestPositionOperations)
{
    using LinkedList = TELinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList list;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.pushLast(i);
    }

    EXPECT_EQ(list[0], list.firstEntry());
    EXPECT_EQ(list[static_cast<int>(count) - 1], list.lastEntry());

    POS next = list.firstPosition();
    while (list.isValidPosition(next))
    {
        int value = list.valueAtPosition(next);
        list.getNext(next) = static_cast<int>(count) + value;
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(list[i], static_cast<int>(count + i));
    }

    POS prev = list.lastPosition();
    while (list.isValidPosition(prev))
    {
        int value = list.valueAtPosition(prev);
        list.getPrev(prev) = value - static_cast<int>(count);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_EQ(list.getAt(i), static_cast<int>(i));
    }
}

/**
 * \brief   Test TELinkedList positioning operations.
 **/
TEST(TELinkedListTest, TestPositionManipulation)
{
    using LinkedList = TELinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList list;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.pushLast(i);
    }

    POS next = list.firstPosition();
    POS prev = list.lastPosition();
    uint32_t cycles = 0;
    while (list.isValidPosition(next) && list.isValidPosition(prev))
    {
        ++cycles;
        EXPECT_TRUE(cycles <= count);

        int valNext {-1};
        EXPECT_TRUE(list.nextEntry(next, valNext) || (cycles == count));
        EXPECT_NE(valNext, list.firstEntry());
        EXPECT_TRUE((valNext == static_cast<int>(cycles)) || (cycles == count));

        int valPrev{ -1 };
        EXPECT_TRUE(list.prevEntry(prev, valPrev) || (cycles == count));
        EXPECT_NE(valPrev, list.lastEntry());
        EXPECT_TRUE((valPrev == static_cast<int>(count - (cycles + 1u))) || (cycles == count));
    }

    cycles = 0;
    for (POS pos = list.lastPosition(); list.isValidPosition(pos); pos = list.prevPosition(pos))
    {
        cycles += 1;
        int value = list.getAt(pos);
        list.setAt(pos, static_cast<int>(count) + value);
        EXPECT_EQ(static_cast<int>(count) + value, list.getAt(pos));
    }

    EXPECT_EQ(cycles, count);
    cycles = 0;
    for (int i = static_cast<int>(count - 1); i >= 0; --i)
    {
        POS pos = list.lastPosition();
        EXPECT_TRUE(list.isValidPosition(pos));
        list.removeAt(pos);
    }

    EXPECT_TRUE(list.isEmpty());
}

/**
 * \brief   Test TELinkedList searching functionalities.
 **/
TEST(TELinkedListTest, TestSearching)
{
    using LinkedList = TELinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    LinkedList list;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_FALSE(list.contains(i));
        list.pushLast(i);
        EXPECT_TRUE(list.contains(i));
    }

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        POS valid = list.find(i);
        EXPECT_TRUE(list.isValidPosition(valid));

        POS invalid = list.find(static_cast<int>(count) + i);
        EXPECT_TRUE(list.isInvalidPosition(invalid));
    }

    list.clear();
    EXPECT_TRUE(list.isEmpty());
}

/**
 * \brief   Test TELinkedList merging.
 **/
TEST(TELinkedListTest, TestMerging)
{
    using LinkedList = TELinkedList<int>;
    using POS = LinkedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 linked lists with entries [0 .. 9] and [10 .. 19]
    LinkedList list1, list2;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list1.pushLast(i);      //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
        list2.pushFirst(i + 10); // 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
    }

    // Step 2:  
    //  - create empty linked-list object `list`
    //  - merge [0 .. 1] entries with `list`.
    // Result: the source object is empty, all entries are extracted and merged
    LinkedList list;
    list.merge(list1);
    EXPECT_TRUE(list1.isEmpty());
    EXPECT_FALSE(list.isEmpty());
    EXPECT_NE(list1, list);
    EXPECT_EQ(list.getSize(), count);

    // Step 3: 
    //  - assign entries [0 .. 9] to the empty linked-list
    //  - merge [0 .. 1] entries with `list`, which already contains the keys [0 .. 9]
    //  Result: the entries in the linked-list are doubled.
    list1 = list;
    EXPECT_EQ(list1, list);
    list.merge(std::move(list1));
    EXPECT_TRUE(list1.isEmpty());
    EXPECT_FALSE(list.isEmpty());
    EXPECT_NE(list1, list);
    EXPECT_EQ(list.getSize(), 2u * count);
    for (POS pos = list.firstPosition(); list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        int val1 = list.valueAtPosition(pos);
        int val2{ -1 };
        EXPECT_TRUE(list.nextEntry(pos, val2));
        EXPECT_EQ(val1, val2);
    }

    // To merge, the LinkedList entries should be sorted. Check it.
    LinkedList listUnorder;
    listUnorder.pushFirst(3);
    listUnorder.pushFirst(2);
    listUnorder.pushFirst(5);
    listUnorder.pushFirst(1);
    listUnorder.pushFirst(4);
    uint32_t unordSize = listUnorder.getSize();
    unordSize += list.getSize();
    listUnorder.merge(list);
    EXPECT_TRUE(list.isEmpty());
    EXPECT_EQ(listUnorder.getSize(), unordSize);
    int valList = listUnorder[0];
    for (POS pos = list.firstPosition(); list.isValidPosition(pos); pos = list.nextPosition(pos))
    {
        EXPECT_LE(valList, list.valueAtPosition(pos));
        valList = list.valueAtPosition(pos);
    }

    // re-initialize the list with entries [0 .. 9]
    list.clear();
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.pushLast(i);      //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
    }

    // Step 4:
    //  - merge entries with keys [10 .. 19] by using `move` operation.
    // Result: the source linked-list is empty, the `list` contains entries [0 .. 19]
    list.merge(std::move(list2));
    EXPECT_TRUE(list2.isEmpty());
    EXPECT_FALSE(list.isEmpty());
    EXPECT_NE(list2, list);
    EXPECT_NE(list1, list);

    // Step 5:
    //  - create and initialize 2 linked-lists, which values are odd and even integers from 1 to 42 and have 21 entries each.
    constexpr int32_t odds[]    { 1, 3, 5, 7,  9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41 };
    constexpr int32_t evens[]   { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42 };
    constexpr uint32_t lenOdd{ MACRO_ARRAYLEN(odds) };
    constexpr uint32_t lenEven{ MACRO_ARRAYLEN(odds) };
    LinkedList listOdd, listEven;
    for (uint32_t i = 0u; i < lenOdd; ++i)
    {
        listOdd.pushLast(odds[i]);
    }
    for (uint32_t i = 0u; i < lenEven; ++i)
    {
        listEven.pushLast(evens[i]);
    }

    // Step 6:
    //  - merge odd linked-list with the `list`
    //  Result:
    //      - the odd entries are in the `list`, including all odd entries in the range [1 .. 19]
    uint32_t listSize = list.getSize();
    list.merge(listOdd);
    EXPECT_TRUE(listOdd.isEmpty());
    EXPECT_EQ(listOdd.getSize(), 0u);
    EXPECT_EQ(list.getSize(), listSize + lenOdd);

    // Step 6:
    //  - merge even linked-list with the `list`
    //  Result:
    //      - the even entries are in the `list`, including all even entries in the range [1 .. 19]
    list.merge(std::move(listEven));
    EXPECT_TRUE(listEven.isEmpty());
    EXPECT_EQ(listEven.getSize(), 0u);
    EXPECT_EQ(list.getSize(), listSize + lenOdd + lenEven);

    // Step 7: make checkups. Entries in range [1 .. 19] are doubled. Other entries are not.
    for (uint32_t i = 0; i < list.getSize(); ++i)
    {
        int value = list.getAt(i);
        POS pos = list.find(value);
        EXPECT_TRUE(list.isValidPosition(pos));
        POS next = list.find(value, pos);
        if ((value > 0) && (value < static_cast<int>(2u * count)))
        {
            EXPECT_TRUE(list.isValidPosition(next));
        }
        else
        {
            EXPECT_TRUE(list.isInvalidPosition(next));
        }
    }

    EXPECT_FALSE(list.contains(static_cast<int>(list.getSize())));
}

/**
 * \brief   Test pushXXXIfUnique method of the TELinkedList object.
 **/
TEST(TELinkedListTest, TestPushUnique)
{
    using LinkedList = TELinkedList<int>;
    constexpr uint32_t count{ 10 };

    constexpr int unique[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int notunique[]{ 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };

    EXPECT_EQ(count, MACRO_ARRAYLEN(unique));
    EXPECT_EQ(count, MACRO_ARRAYLEN(notunique));

    LinkedList list;
    for (uint32_t i = 0; i < count; ++i)
    {
        bool result = list.pushFirstIfUnique(notunique[i]);
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
        EXPECT_TRUE(list.pushLastIfUnique(unique[i]));
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_FALSE(list.pushFirstIfUnique(notunique[i], true));
    }
}

/**
 * \brief   Test insertAfter and insertBefore methods of the TELinkedList object.
 **/
TEST(TELinkedListTest, TestInserting)
{
    using LinkedList = TELinkedList<int>;
    using POS = LinkedList::LISTPOS;

    constexpr int notunique[]{ 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5 };
    constexpr uint32_t count{ MACRO_ARRAYLEN(notunique) };
    constexpr uint32_t len{ count / 2u };

    LinkedList list;
    for (uint32_t i = 0; i < len; ++i)
    {
        POS pos = list.invalidPosition();
        bool before = (i % 2u) == 0;
        if (before)
        {
            pos = list.insertBefore(list.lastPosition(), notunique[i]);
        }
        else
        {
            pos = list.insertAfter(list.firstPosition(), notunique[i]);
        }

        EXPECT_TRUE(list.checkPosition(pos));
        EXPECT_TRUE(list.isValidPosition(pos));
        EXPECT_EQ(notunique[i], list.valueAtPosition(pos));
    }

    for (uint32_t i = len; i < count; ++i)
    {
        POS found = list.find(notunique[i]);
        POS pos = list.invalidPosition();
        EXPECT_TRUE(list.isValidPosition(found));
        bool before = (i % 2u) == 0;
        if (before)
        {
            pos = list.insertBefore(found, notunique[i]);
            EXPECT_TRUE(list.valueAtPosition(pos) == list.valueAtPosition(found));
        }
        else
        {
            pos = list.insertAfter(found, notunique[i]);
            EXPECT_TRUE(list.valueAtPosition(pos) == list.valueAtPosition(found));
        }

        EXPECT_TRUE(list.checkPosition(pos));
        EXPECT_TRUE(list.isValidPosition(pos));
        EXPECT_EQ(notunique[i], list.valueAtPosition(pos));
    }
}

/**
 * \brief   Test remove first / last methods of the TELinkedList object.
 **/
TEST(TELinkedListTest, TestRemoveEntries)
{
    using LinkedList = TELinkedList<int>;
    constexpr int32_t count{ 10 };

    LinkedList list;
    for (int i = 0; i < count; ++i)
    {
        list.pushLast(i);
    }

    EXPECT_EQ(list.getSize(), static_cast<uint32_t>(count));
    for (int i = 0; i < (count / 2); ++i)
    {
        int first{ -1 }, last{ -1 };
        EXPECT_TRUE(list.removeFirst(first));
        EXPECT_TRUE(list.removeLast(last));
        EXPECT_EQ(i, first);
        EXPECT_EQ(count - (i + 1), last);
    }

    EXPECT_TRUE(list.isEmpty());
}

/**
 * \brief   Test TELinkedList streaming operators.
 **/
TEST(TELinkedListTest, TestStreaming)
{
    using LinkedList = TELinkedList<int>;
    constexpr int32_t count{ 10 };

    LinkedList src;
    for (int i = 0; i < count; ++i)
    {
        src.pushLast(i);
    }

    SharedBuffer stream;
    stream << src;

    EXPECT_FALSE(stream.isEmpty());

    stream.moveToBegin();
    LinkedList dst;
    stream >> dst;

    EXPECT_EQ(src, dst);
}

/**
 * \brief   Test TELinkedList streaming operators.
 **/
TEST(TELinkedListTest, TestSortAscending)
{
    using LinkedList = TELinkedList<int>;
    constexpr int _arr1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr int _res1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr uint32_t _len{ MACRO_ARRAYLEN(_arr1) };
    int _dat1[_len]{};

    LinkedList list1(_arr1, _len), res1(_res1, _len);
    list1.sort([](const int elem1, const int elem2) { return (elem1 < elem2); });
    list1.getElements(_dat1, _len);
    EXPECT_NE(NEMemory::memCompare(_dat1, _arr1, _len), NEMath::eCompare::Equal);
    EXPECT_EQ(list1.getSize(), _len);
    EXPECT_EQ(NEMemory::memCompare(_dat1, _res1, _len), NEMath::eCompare::Equal);
}

/**
 * \brief   Test TELinkedList streaming operators.
 **/
TEST(TELinkedListTest, TestSortDescending)
{
    using LinkedList = TELinkedList<int>;
    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int _res1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr uint32_t _len{ MACRO_ARRAYLEN(_arr1) };
    int _dat1[_len]{};

    LinkedList list1(_arr1, _len), res1(_res1, _len);
    list1.sort([](const int elem1, const int elem2) { return (elem1 > elem2); });
    list1.getElements(_dat1, _len);
    EXPECT_NE(NEMemory::memCompare(_dat1, _arr1, _len), NEMath::eCompare::Equal);
    EXPECT_EQ(list1.getSize(), _len);
    EXPECT_EQ(NEMemory::memCompare(_dat1, _res1, _len), NEMath::eCompare::Equal);
}
