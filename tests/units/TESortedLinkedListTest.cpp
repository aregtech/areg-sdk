/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/TESortedLinkedListTest.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Tests of TESortedLinkedList object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/TESortedLinkedList.hpp"
#include "areg/base/SharedBuffer.hpp"

 /**
  * \brief   Test TESortedLinkedList constructors.
  **/
TEST(TESortedLinkedListTest, TestConstructors)
{
    using SortedList = TESortedLinkedList<int>;
    constexpr uint32_t count{ 10 };

    SortedList empty, init;
    EXPECT_TRUE(empty.isEmpty() && init.isEmpty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        init.add(i);
        EXPECT_EQ(init.getSize(), static_cast<uint32_t>(i + 1));
    }

    EXPECT_EQ(init.getSize(), count);

    SortedList copy(init);
    EXPECT_FALSE(copy.isEmpty());
    EXPECT_EQ(copy.getSize(), count);
    EXPECT_EQ(copy, init);

    SortedList move(std::move(init));
    EXPECT_FALSE(move.isEmpty());
    EXPECT_TRUE(init.isEmpty());
    EXPECT_EQ(move.getSize(), count);
    EXPECT_EQ(init.getSize(), 0u);
    EXPECT_EQ(move, copy);
    EXPECT_NE(move, init);
}

/**
 * \brief   Test TESortedLinkedList operators.
 **/
TEST(TESortedLinkedListTest, TestOperators)
{
    using SortedList = TESortedLinkedList<int>;
    using POS = SortedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    SortedList src(false);
    EXPECT_TRUE(src.isEmpty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        src.add(i);
        EXPECT_EQ(src[0u], i);
    }

    int i = static_cast<int>(count) - 1;
    for (POS pos = src.firstPosition(); src.isValidPosition(pos); pos = src.nextPosition(pos))
    {
        EXPECT_EQ(src[pos], i);
        --i;
    }

    SortedList copy;
    copy = src;
    EXPECT_EQ(copy.getSize(), count);
    EXPECT_EQ(copy, src);
    EXPECT_TRUE(copy == src);

    SortedList move;
    move = std::move(src);
    EXPECT_TRUE(src.isEmpty());
    EXPECT_EQ(move.getSize(), count);
    EXPECT_EQ(move, copy);
    EXPECT_NE(move, src);
    EXPECT_TRUE(move == copy);
    EXPECT_TRUE(move != src);
}


/**
 * \brief   Test TESortedLinkedList positioning attributes.
 **/
TEST(TESortedLinkedListTest, TestPositionAttributes)
{
    using SortedList = TESortedLinkedList<int>;
    using POS = SortedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    SortedList list(false);
    POS first = list.firstPosition();
    POS last = list.lastPosition();
    EXPECT_TRUE(list.isInvalidPosition(first));
    EXPECT_FALSE(list.isValidPosition(last));
    EXPECT_EQ(first, list.invalidPosition());
    EXPECT_EQ(last, list.invalidPosition());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.add(i);
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
        EXPECT_TRUE(list.isInvalidPosition(next) || ((value - 1) == list.valueAtPosition(next)));
    }

    POS prev = list.lastPosition();
    while (list.isValidPosition(prev))
    {
        int value = list.getPrev(prev);
        EXPECT_TRUE(list.isInvalidPosition(prev) || ((value + 1) == list.valueAtPosition(prev)));
    }
}

/**
 * \brief   Test TESortedLinkedList positioning operations.
 **/
TEST(TESortedLinkedListTest, TestPositionManipulation)
{
    using SortedList = TESortedLinkedList<int>;
    using POS = SortedList::LISTPOS;
    constexpr uint32_t count{ 10 };

    SortedList list(true);
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.add(i);
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
 * \brief   Test TESortedLinkedList searching functionalities.
 **/
TEST(TESortedLinkedListTest, TestSearching)
{
    using SortedList = TESortedLinkedList<int>;
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
        EXPECT_TRUE(list.isValidPosition(valid));

        POS invalid = list.find(static_cast<int>(count) + i);
        EXPECT_TRUE(list.isInvalidPosition(invalid));
    }

    list.clear();
    EXPECT_TRUE(list.isEmpty());
}

/**
 * \brief   Test TESortedLinkedList merging.
 **/
TEST(TESortedLinkedListTest, TestMerging)
{
    using SortedList = TESortedLinkedList<int>;
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

    SortedList listUnorder(!list.isAscending());
    listUnorder.add(3);
    listUnorder.add(2);
    listUnorder.add(5);
    listUnorder.add(1);
    listUnorder.add(4);
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
    EXPECT_TRUE(list.isEmpty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        list.add(i);      //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
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
 * \brief   Test addIfUnique method of the TESortedLinkedList object.
 **/
TEST(TESortedLinkedListTest, TestAddIfUnique)
{
    using SortedList = TESortedLinkedList<int>;
    constexpr uint32_t count{ 10 };

    constexpr int unique[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int notunique[]{ 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };

    EXPECT_EQ(count, MACRO_ARRAYLEN(unique));
    EXPECT_EQ(count, MACRO_ARRAYLEN(notunique));

    SortedList list(true);
    for (uint32_t i = 0; i < count; ++i)
    {
        bool result = list.addIfUnique(notunique[i]).second;
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
        EXPECT_TRUE(list.addIfUnique(unique[i]).second);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        EXPECT_FALSE(list.addIfUnique(notunique[i], true).second);
    }
}

/**
 * \brief   Test remove first / last methods of the TESortedLinkedList object.
 **/
TEST(TESortedLinkedListTest, TestRemoveEntries)
{
    using SortedList = TESortedLinkedList<int>;
    constexpr int32_t count{ 10 };

    SortedList asc(true);
    SortedList desc(false);
    for (int i = 0; i < count; ++i)
    {
        asc.add(i);
        desc.add(i);
    }

    EXPECT_NE(asc, desc);
    EXPECT_EQ(asc.getSize(), static_cast<uint32_t>(count));
    EXPECT_EQ(desc.getSize(), static_cast<uint32_t>(count));

    for (int i = 0; i < (count / 2); ++i)
    {
        int ascFirst{ -1 }, ascLast{ -1 };
        EXPECT_TRUE(asc.removeFirst(ascFirst));
        EXPECT_TRUE(asc.removeLast(ascLast));
        EXPECT_EQ(i, ascFirst);
        EXPECT_EQ(count - (i + 1), ascLast);

        int descFirst{ -1 }, descLast{ -1 };
        EXPECT_TRUE(desc.removeFirst(descFirst));
        EXPECT_TRUE(desc.removeLast(descLast));
        EXPECT_EQ(count - (i + 1), descFirst);
        EXPECT_EQ(i, descLast);

    }

    EXPECT_TRUE(asc.isEmpty());
    EXPECT_TRUE(desc.isEmpty());
}

/**
 * \brief   Test ascending and descending behavior of the TESortedLinkedList object.
 **/
TEST(TESortedLinkedListTest, TestSortingEntries)
{
    using SortedList = TESortedLinkedList<int>;
    using POS = SortedList::LISTPOS;

    constexpr int _list[]{ 5, 1, 0, 8, 3, 5, 6, 1, 5, 8, 2, 8, 9, 4, 7, 0 };
    constexpr int _asc[] { 0, 0, 1, 1, 2, 3, 4, 5, 5, 5, 6, 7, 8, 8, 8, 9 };
    constexpr int _desc[]{ 9, 8, 8, 8, 7, 6, 5, 5, 5, 4, 3, 2, 1, 1, 0, 0 };
    constexpr int32_t count{ MACRO_ARRAYLEN(_list) };

    SortedList asc(true);
    SortedList desc(false);
    for (int i = 0; i < count; ++i)
    {
        asc.add(_list[static_cast<uint32_t>(i)]);
        desc.add(_list[static_cast<uint32_t>(i)]);
    }

    EXPECT_NE(asc, desc);
    EXPECT_EQ(asc.getSize(), static_cast<uint32_t>(count));
    EXPECT_EQ(desc.getSize(), static_cast<uint32_t>(count));

    POS posAsc = asc.firstPosition();
    POS revAsc = asc.lastPosition();

    POS posDesc = desc.firstPosition();
    POS revDesc = desc.lastPosition();

    for (int i = 0; i < count; ++i)
    {
        EXPECT_EQ(asc[static_cast<uint32_t>(i)], _asc[static_cast<uint32_t>(i)]);
        EXPECT_EQ(desc[static_cast<uint32_t>(i)], _desc[static_cast<uint32_t>(i)]);

        EXPECT_TRUE(asc.isValidPosition(posAsc));
        EXPECT_TRUE(asc.isValidPosition(revAsc));
        EXPECT_TRUE(desc.isValidPosition(posDesc));
        EXPECT_TRUE(desc.isValidPosition(revDesc));

        EXPECT_EQ(asc.valueAtPosition(posAsc), _asc[static_cast<uint32_t>(i)]);
        EXPECT_EQ(desc.valueAtPosition(posDesc), _desc[static_cast<uint32_t>(i)]);
        EXPECT_EQ(desc.valueAtPosition(revDesc), asc.valueAtPosition(posAsc));
        EXPECT_EQ(asc.valueAtPosition(revAsc), desc.valueAtPosition(posDesc));

        posAsc = asc.nextPosition(posAsc);
        revAsc = asc.prevPosition(revAsc);

        posDesc = desc.nextPosition(posDesc);
        revDesc = desc.prevPosition(revDesc);
    }
}

/**
 * \brief   Test TESortedLinkedList streaming operators.
 **/
TEST(TESortedLinkedListTest, TestStreaming)
{
    using SortedList = TESortedLinkedList<int>;
    constexpr int32_t count{ 10 };

    SortedList src(false);
    for (int i = 0; i < count; ++i)
    {
        src.add(i);
    }

    SharedBuffer stream;
    stream << src;

    EXPECT_FALSE(stream.isEmpty());

    stream.moveToBegin();
    SortedList dst(true);
    stream >> dst;

    EXPECT_EQ(src, dst);
}
