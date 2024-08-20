/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/TEMapTest.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Tests of TEMap object.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/TEMap.hpp"
#include "areg/base/SharedBuffer.hpp"

/**
 * \brief   Test TEMap constructors.
 **/
TEST(TEMapTest, TestConstructors)
{
    using Map = TEMap<int, int>;
    constexpr uint32_t count{ 10 };

    // Step 1: test default constructor, initialize maps.
    Map map1, map2;
    EXPECT_TRUE(map1.isEmpty() && map2.isEmpty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map1.setAt(i, i);
        map2.setAt(i + static_cast<int>(count), i + static_cast<int>(count));
    }

    EXPECT_FALSE(map1.isEmpty() && map2.isEmpty());

    // Step 2: Test copy-constructor
    //  Result: the elements are copied from the source and the source remains unchanged.
    Map map3(map1);
    EXPECT_FALSE(map1.isEmpty());
    EXPECT_EQ(map1, map3);

    // Step 2: test move-constructor
    //  Result: the elements are moved from the source and the source is empty.
    Map map4(std::move(map2));
    EXPECT_TRUE(map2.isEmpty());
    EXPECT_FALSE(map4.isEmpty());
    EXPECT_NE(map2, map4);

    map3.clear();
    EXPECT_TRUE(map3.isEmpty());
    map4.release();
    EXPECT_TRUE(map4.isEmpty());
}

/**
 * \brief   Test TEMap operators.
 **/
TEST(TEMapTest, TestOperators)
{
    using Map = TEMap<int, int>;
    constexpr uint32_t count{ 10 };

    Map map1, map2;
    EXPECT_TRUE(map1 == map2);
    EXPECT_TRUE(map1.isEmpty() && map2.isEmpty());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map1[i] = i;
        map2[i + static_cast<int>(count)] = i + static_cast<int>(count);
    }

    Map map3;
    EXPECT_TRUE(map3.isEmpty());
    map3 = map1;
    EXPECT_FALSE(map1.isEmpty());
    EXPECT_FALSE(map3.isEmpty());
    EXPECT_EQ(map3.getSize(), count);
    EXPECT_EQ(map1, map3);
    EXPECT_TRUE(map1 == map3);

    Map map4;
    EXPECT_TRUE(map4.isEmpty());
    map4 = std::move(map2);
    EXPECT_TRUE(map2.isEmpty());
    EXPECT_FALSE(map4.isEmpty());
    EXPECT_EQ(map4.getSize(), count);
    EXPECT_NE(map2, map4);
    EXPECT_TRUE(map2 != map4);
    EXPECT_TRUE(map3 != map4);
}

/**
 * \brief   Test TEMap positioning attributes.
 **/
TEST(TEMapTest, TestPositionAttributes)
{
    using Map = TEMap<int, int>;
    constexpr uint32_t count{ 10 };

    Map map;
    auto invPos = map.invalidPosition();
    EXPECT_TRUE(map.isInvalidPosition(invPos));

    // Step 1: in the empty map, take the first position, which must be invalid
    auto invFirst = map.firstPosition();
    EXPECT_FALSE(map.isStartPosition(invFirst));

    // Step 2: search for entries in the map and check the validity of the position.
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        // Before the entry is inserted, the `find` method should return invalid position
        auto invalid = map.find(i);
        EXPECT_FALSE(map.isValidPosition(invalid));
        EXPECT_TRUE(map.isInvalidPosition(invalid));
        EXPECT_FALSE(map.checkPosition(invalid));
        EXPECT_FALSE(map.isStartPosition(invalid));

        // Insert entrie
        map[i] = i;

        // After the entry exists, the `find` method should return valid position.
        auto valid = map.find(i);
        EXPECT_TRUE(map.isValidPosition(valid));
        EXPECT_FALSE(map.isInvalidPosition(valid));
        EXPECT_TRUE(map.checkPosition(valid));
    }

    auto first = map.firstPosition();
    EXPECT_TRUE(map.isStartPosition(first));
    EXPECT_TRUE(invPos == map.invalidPosition());
}

/**
 * \brief   Test TEMap positioning operations.
 **/
TEST(TEMapTest, TestPositionOperations)
{
    using Map = TEMap<int, int>;
    using POS = Map::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr int32_t coef{ 2 };

    Map map;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map.setAt(i, i * coef);
        EXPECT_EQ(map.getAt(i), i * coef);
    }

    // Step 1: in the valid and initialized map get first position, which is valid
    POS pos = map.firstPosition();
    EXPECT_TRUE(map.isValidPosition(pos));

    // Step 2: in the loop get the next position and checkup keys and values
    uint32_t cnt{ 0 };
    while (map.isValidPosition(pos))
    {
        int Key, Value;
        map.getAtPosition(pos, Key, Value);
        EXPECT_TRUE(Value == Key * coef);
        EXPECT_TRUE(Value == map.valueAtPosition(pos));
        EXPECT_TRUE(Key == map.keyAtPosition(pos));

        POS cur = pos;
        int nextKey{ -1 }, nextValue{ -1 };

        pos = map.nextPosition(cur, nextKey, nextValue);
        EXPECT_EQ(nextKey, Key);
        EXPECT_EQ(nextValue, Value);
        if (map.isValidPosition(pos))
        {
            EXPECT_EQ(MACRO_DELTA(map.keyAtPosition(pos), nextKey), 1);
            EXPECT_EQ(MACRO_DELTA(map.valueAtPosition(pos), nextValue), coef);
        }

        pos = map.nextPosition(cur);
        ++cnt;
        EXPECT_TRUE((map.isValidPosition(pos)) || (cnt == count));
    }

    EXPECT_TRUE(cnt == count);
    EXPECT_TRUE(map.isInvalidPosition(pos));
}

/**
 * \brief   Test TEMap positioning manipulation.
 **/
TEST(TEMapTest, TestPositionManipulation)
{
    using Map = TEMap<int, int>;
    using POS = Map::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr int32_t coef{ 2 };

    Map map;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map[i * coef] = i;
    }

    // Step 1: initialize a map
    uint32_t cnt{ 0 };
    for (POS pos = map.firstPosition(); map.isValidPosition(pos); ++cnt, pos = map.nextPosition(pos))
    {
        map.setPosition(pos, static_cast<int>(map.valueAtPosition(pos) * coef));
        int Key = map.keyAtPosition(pos);
        int Value = map.valueAtPosition(pos);
        EXPECT_EQ(Key, Value);
    }

    // Step 2: change the values by poisition and make sure it is valid / changed.
    EXPECT_EQ(cnt, count);
    uint32_t idx{ 0 };
    for (POS pos = map.firstPosition(); map.isValidPosition(pos); --cnt, ++idx)
    {
        int Key{ -1 }, Value{ -1 };
        pos = map.removePosition(pos, Key, Value);
        EXPECT_EQ(Key, Value);
    }

    EXPECT_EQ(cnt, 0);
    EXPECT_EQ(idx, count);
}

/**
 * \brief   Test TEMap searching functionalities.
 **/
TEST(TEMapTest, TestSearching)
{
    using Map = TEMap<int, int>;
    using POS = Map::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr uint32_t listSize{ 20 };

    // Step 1: init map [0 .. 9]
    Map map;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map[i] = i;
    }

    // Step 2: search keys [0 .. 19] and check the position validity:
    //  Result:
    //      - entries with keys [0 .. 9] have valid position
    //      - entries with keys [10 .. 19] have invalid position
    for (int i = 0; i < static_cast<int>(listSize); ++i)
    {
        const int Key{ i };
        int Value{ -1 };
        POS pos = map.find(i);
        bool found = map.find(Key, Value);
        if (i < static_cast<int>(count))
        {
            EXPECT_TRUE(map.isValidPosition(pos));
            EXPECT_TRUE(found);
            EXPECT_TRUE(map.contains(Key));
            EXPECT_EQ(map.valueAtPosition(pos), Value);
            EXPECT_EQ(map.keyAtPosition(pos), Key);
            EXPECT_EQ(Key, Value);
        }
        else
        {
            EXPECT_FALSE(map.isValidPosition(pos));
            EXPECT_FALSE(found);
            EXPECT_FALSE(map.contains(Key));
            EXPECT_NE(Key, Value);
        }
    }
}

/**
 * \brief   Test TEMap merging.
 **/
TEST(TEMapTest, TestMerging)
{
    using Map = TEMap<int, int>;
    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 maps with entries [0 .. 9] and [10 .. 19]
    Map map1, map2;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map1[i] = i;        //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
        map2[i + 10] = i + 10;   // 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
    }

    // Step 2:  
    //  - create empty map object `map`
    //  - merge [0 .. 1] entries with `map`.
    // Result: the source object is empty, all entries are extraced and merged
    Map map;
    map.merge(map1);
    EXPECT_TRUE(map1.isEmpty());
    EXPECT_FALSE(map.isEmpty());
    EXPECT_NE(map1, map);

    // Step 3: 
    //  - assign entries [0 .. 9] to the empty map
    //  - merge [0 .. 1] entries with `map`, which already contains the keys [0 .. 9]
    //  Result: the source and `map` maps remain unchanged, because it already contained keys [0 .. 9]
    map1 = map;
    EXPECT_EQ(map1, map);
    map.merge(std::move(map1));
    EXPECT_FALSE(map1.isEmpty());
    EXPECT_FALSE(map.isEmpty());
    EXPECT_EQ(map.getSize(), count);
    EXPECT_EQ(map1, map);

    // Step 4:
    //  - merge entries with keys [10 .. 19] by using `move` operation.
    // Result: the source map is empty, the `map` contains entries [0 .. 19]
    map.merge(std::move(map2));
    EXPECT_TRUE(map2.isEmpty());
    EXPECT_FALSE(map.isEmpty());
    EXPECT_NE(map2, map);
    EXPECT_NE(map1, map);

    // Step 5:
    //  - create and initialize 2 maps, wich keys and values are odd and even integers from 1 to 42 and totally have 21 entries each.
    constexpr int32_t odds[]{ 1, 3, 5, 7,  9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41 };
    constexpr int32_t evens[]{ 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42 };
    constexpr uint32_t lenOdd{ MACRO_ARRAYLEN(odds) };
    constexpr uint32_t lenEven{ MACRO_ARRAYLEN(odds) };
    Map mapOdd, mapEven;
    for (uint32_t i = 0u; i < lenOdd; ++i)
    {
        mapOdd[odds[i]] = odds[i];
    }
    for (uint32_t i = 0u; i < lenEven; ++i)
    {
        mapEven[evens[i]] = evens[i];
    }

    // Step 6:
    //  - merge odd map with the `map`
    //  Result:
    //      - the odd and the target `map` maps changed the sizes
    //      - the odd map contains only those entries, which `map` had before merge (10 entries)
    //      - the size of `map` increased by 11
    uint32_t mapSize = map.getSize();
    map.merge(mapOdd);
    EXPECT_FALSE(mapOdd.isEmpty());
    EXPECT_EQ(mapOdd.getSize(), count);
    EXPECT_EQ(map.getSize(), mapSize + 11);

    // Step 6:
    //  - merge even map with the `map`
    //  Result:
    //      - the even and the target `map` maps changed the sizes
    //      - the odd map contains only those entries, which `map` had before merge (9 entries)
    //      - the size of `map` increased by 12
    mapSize = map.getSize();
    map.merge(std::move(mapEven));
    EXPECT_FALSE(mapEven.isEmpty());
    EXPECT_EQ(mapEven.getSize(), count - 1);
    EXPECT_EQ(map.getSize(), mapSize + 11 + 1);

    // Step 7: make checkups to make sure that only elements that were not in `map` where merged.
    for (int i = 0; i < static_cast<int>(map.getSize()); ++i)
    {
        EXPECT_EQ(map.getAt(i), i);
        if ((i > 0) && (i < 20))
        {
            if ((i % 2) != 0)
            {
                EXPECT_TRUE(mapOdd.contains(i));
                EXPECT_FALSE(mapEven.contains(i));
            }
            else
            {
                EXPECT_FALSE(mapOdd.contains(i));
                EXPECT_TRUE(mapEven.contains(i));
            }
        }
    }

    EXPECT_FALSE(map.contains(static_cast<int>(map.getSize())));
}

/**
 * \brief   Test addIfUnique method of the Hash Map object.
 **/
TEST(TEMapTest, TestAddUnique)
{
    using Map = TEMap<int, int>;
    using POS = Map::MAPPOS;
    constexpr uint32_t count{ 10 };

    constexpr int unique[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int notunique[]{ 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };

    Map map;
    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = map.addIfUnique(notunique[i], notunique[i]);
        EXPECT_TRUE(map.isValidPosition(result.first));
        if (i < 5)
        {
            EXPECT_TRUE(result.second);
        }
        else
        {
            EXPECT_FALSE(result.second);
        }
    }

    map.release();
    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = map.addIfUnique(unique[i], unique[i]);
        EXPECT_TRUE(map.isValidPosition(result.first));
        EXPECT_TRUE(result.second);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = map.addIfUnique(notunique[i], notunique[i]);
        EXPECT_TRUE(map.isValidPosition(result.first));
        EXPECT_FALSE(result.second);
    }
}

/**
 * \brief   Test update method of the Hash Map object.
 **/
TEST(TEMapTest, TestUpdate)
{
    using Map = TEMap<int, int>;
    constexpr uint32_t count{ 10 };
    constexpr int arr[]{ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    Map map;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map[i] = i;
    }

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_EQ(map[i], i);
        map.updateAt(i, arr[i]);
        EXPECT_EQ(map[i], arr[i]);
    }
}

/**
 * \brief   Test nextEntry method of the Hash Map object.
 **/
TEST(TEMapTest, TestNextEntry)
{
    using Map = TEMap<int, int>;
    using POS = Map::MAPPOS;
    constexpr uint32_t count{ 10 };

    Map map;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map[i] = i;
    }

    uint32_t idx{ 0 };
    POS pos = map.firstPosition();
    while (map.isValidPosition(pos))
    {
        int Key{ -1 }, Value{ -1 };
        if (idx < 9)
        {
            EXPECT_TRUE(map.nextEntry(pos, Key, Value));
            EXPECT_TRUE((Key == Value) && (Key != -1));
        }
        else
        {
            EXPECT_FALSE(map.nextEntry(pos, Key, Value));
            EXPECT_TRUE((Key == Value) && (Key == -1));
        }

        ++idx;
    }

    EXPECT_EQ(idx, count);
}

/**
 * \brief   Test Map streaming operators.
 **/
TEST(TEMapTest, TestStreaming)
{
    using Map = TEMap<int, int>;
    constexpr uint32_t count{ 10 };

    Map src;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        src[i] = i;
    }

    SharedBuffer stream;
    stream << src;

    stream.moveToBegin();
    Map dst;
    stream >> dst;

    EXPECT_EQ(src, dst);
}
