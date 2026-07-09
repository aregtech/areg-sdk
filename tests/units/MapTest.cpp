/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/MapTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of OrderedMap object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/OrderedMap.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/MathDefs.hpp"

/**
 * \brief   Test OrderedMap constructors.
 **/
TEST(MapTest, test_constructors)
{
    using Map = areg::OrderedMap<int, int>;
    constexpr uint32_t count{ 10 };

    // Step 1: test default constructor, initialize maps.
    Map map1, map2;
    EXPECT_TRUE(map1.is_empty() && map2.is_empty());
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map1.set_value_at(i, i);
        map2.set_value_at(i + static_cast<int>(count), i + static_cast<int>(count));
    }

    EXPECT_FALSE(map1.is_empty() && map2.is_empty());

    // Step 2: Test copy-constructor
    //  Result: the elements are copied from the source and the source remains unchanged.
    Map map3(map1);
    EXPECT_FALSE(map1.is_empty());
    EXPECT_EQ(map1, map3);

    // Step 2: test move-constructor
    //  Result: the elements are moved from the source and the source is empty.
    Map map4(std::move(map2));
    EXPECT_TRUE(map2.is_empty());
    EXPECT_FALSE(map4.is_empty());
    EXPECT_NE(map2, map4);

    map3.clear();
    EXPECT_TRUE(map3.is_empty());
    map4.release();
    EXPECT_TRUE(map4.is_empty());
}

/**
 * \brief   Test OrderedMap operators.
 **/
TEST(MapTest, test_operators)
{
    using Map = areg::OrderedMap<int, int>;
    constexpr uint32_t count{ 10 };

    Map map1, map2;
    EXPECT_TRUE(map1 == map2);
    EXPECT_TRUE(map1.is_empty() && map2.is_empty());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map1[i] = i;
        map2[i + static_cast<int>(count)] = i + static_cast<int>(count);
    }

    Map map3;
    EXPECT_TRUE(map3.is_empty());
    map3 = map1;
    EXPECT_FALSE(map1.is_empty());
    EXPECT_FALSE(map3.is_empty());
    EXPECT_EQ(map3.size(), count);
    EXPECT_EQ(map1, map3);
    EXPECT_TRUE(map1 == map3);

    Map map4;
    EXPECT_TRUE(map4.is_empty());
    map4 = std::move(map2);
    EXPECT_TRUE(map2.is_empty());
    EXPECT_FALSE(map4.is_empty());
    EXPECT_EQ(map4.size(), count);
    EXPECT_NE(map2, map4);
    EXPECT_TRUE(map2 != map4);
    EXPECT_TRUE(map3 != map4);
}

/**
 * \brief   Test OrderedMap positioning attributes.
 **/
TEST(MapTest, test_position_attributes)
{
    using Map = areg::OrderedMap<int, int>;
    constexpr uint32_t count{ 10 };

    Map map;
    auto invPos = map.invalid_position();
    EXPECT_TRUE(!map.is_valid_position(invPos));

    // Step 1: in the empty map, take the first position, which must be invalid
    auto invFirst = map.first_position();
    EXPECT_FALSE(map.is_first_position(invFirst));

    // Step 2: search for entries in the map and check the validity of the position.
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        // Before the entry is inserted, the `find` method should return invalid position
        auto invalid = map.find(i);
        EXPECT_FALSE(map.is_valid_position(invalid));
        EXPECT_TRUE(!map.is_valid_position(invalid));
        EXPECT_FALSE(map.check_position(invalid));
        EXPECT_FALSE(map.is_first_position(invalid));

        // Insert entrie
        map[i] = i;

        // After the entry exists, the `find` method should return valid position.
        auto valid = map.find(i);
        EXPECT_TRUE(map.is_valid_position(valid));
        EXPECT_FALSE(!map.is_valid_position(valid));
        EXPECT_TRUE(map.check_position(valid));
    }

    auto first = map.first_position();
    EXPECT_TRUE(map.is_first_position(first));
    EXPECT_TRUE(invPos == map.invalid_position());
}

/**
 * \brief   Test OrderedMap positioning operations.
 **/
TEST(MapTest, test_position_operations)
{
    using Map = areg::OrderedMap<int, int>;
    using POS = Map::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr int32_t coef{ 2 };

    Map map;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map.set_value_at(i, i * coef);
        EXPECT_EQ(map.value_at(i), i * coef);
    }

    // Step 1: in the valid and initialized map get first position, which is valid
    POS pos = map.first_position();
    EXPECT_TRUE(map.is_valid_position(pos));

    // Step 2: in the loop get the next position and checkup keys and values
    uint32_t cnt{ 0 };
    while (map.is_valid_position(pos))
    {
        int Key, Value;
        map.at_position(pos, Key, Value);
        EXPECT_TRUE(Value == Key * coef);
        EXPECT_TRUE(Value == map.value_at(pos));
        EXPECT_TRUE(Key == map.key_at(pos));

        POS cur = pos;
        int nextKey{ -1 }, nextValue{ -1 };

        pos = map.next_position(cur, nextKey, nextValue);
        EXPECT_EQ(nextKey, Key);
        EXPECT_EQ(nextValue, Value);
        if (map.is_valid_position(pos))
        {
            EXPECT_EQ(areg::delta(map.key_at(pos), nextKey), 1);
            EXPECT_EQ(areg::delta(map.value_at(pos), nextValue), coef);
        }

        pos = map.next_position(cur);
        ++cnt;
        EXPECT_TRUE((map.is_valid_position(pos)) || (cnt == count));
    }

    EXPECT_TRUE(cnt == count);
    EXPECT_TRUE(!map.is_valid_position(pos));
}

/**
 * \brief   Test OrderedMap positioning manipulation.
 **/
TEST(MapTest, test_position_manipulation)
{
    using Map = areg::OrderedMap<int, int>;
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
    for (POS pos = map.first_position(); map.is_valid_position(pos); ++cnt, pos = map.next_position(pos))
    {
        map.set_value_at(pos, static_cast<int>(map.value_at(pos) * coef));
        int Key = map.key_at(pos);
        int Value = map.value_at(pos);
        EXPECT_EQ(Key, Value);
    }

    // Step 2: change the values by poisition and make sure it is valid / changed.
    EXPECT_EQ(cnt, count);
    uint32_t idx{ 0 };
    for (POS pos = map.first_position(); map.is_valid_position(pos); --cnt, ++idx)
    {
        int Key{ -1 }, Value{ -1 };
        pos = map.remove_at(pos, Key, Value);
        EXPECT_EQ(Key, Value);
    }

    EXPECT_EQ(cnt, 0u);
    EXPECT_EQ(idx, count);
}

/**
 * \brief   Test OrderedMap searching functionalities.
 **/
TEST(MapTest, test_searching)
{
    using Map = areg::OrderedMap<int, int>;
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
            EXPECT_TRUE(map.is_valid_position(pos));
            EXPECT_TRUE(found);
            EXPECT_TRUE(map.contains(Key));
            EXPECT_EQ(map.value_at(pos), Value);
            EXPECT_EQ(map.key_at(pos), Key);
            EXPECT_EQ(Key, Value);
        }
        else
        {
            EXPECT_FALSE(map.is_valid_position(pos));
            EXPECT_FALSE(found);
            EXPECT_FALSE(map.contains(Key));
            EXPECT_NE(Key, Value);
        }
    }
}

/**
 * \brief   Test OrderedMap merging.
 **/
TEST(MapTest, test_merging)
{
    using Map = areg::OrderedMap<int, int>;
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
    // Result: the source object is empty, all entries are extracted and merged
    Map map;
    map.merge(map1);
    EXPECT_TRUE(map1.is_empty());
    EXPECT_FALSE(map.is_empty());
    EXPECT_NE(map1, map);

    // Step 3: 
    //  - assign entries [0 .. 9] to the empty map
    //  - merge [0 .. 1] entries with `map`, which already contains the keys [0 .. 9]
    //  Result: the source and `map` maps remain unchanged, because it already contained keys [0 .. 9]
    map1 = map;
    EXPECT_EQ(map1, map);
    map.merge(std::move(map1));
    EXPECT_FALSE(map1.is_empty());
    EXPECT_FALSE(map.is_empty());
    EXPECT_EQ(map.size(), count);
    EXPECT_EQ(map1, map);

    // Step 4:
    //  - merge entries with keys [10 .. 19] by using `move` operation.
    // Result: the source map is empty, the `map` contains entries [0 .. 19]
    map.merge(std::move(map2));
    EXPECT_TRUE(map2.is_empty());
    EXPECT_FALSE(map.is_empty());
    EXPECT_NE(map2, map);
    EXPECT_NE(map1, map);

    // Step 5:
    //  - create and initialize 2 maps, wich keys and values are odd and even integers from 1 to 42 and totally have 21 entries each.
    constexpr int32_t odds[]{ 1, 3, 5, 7,  9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41 };
    constexpr int32_t evens[]{ 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42 };
    constexpr uint32_t lenOdd{ std::size(odds) };
    constexpr uint32_t lenEven{ std::size(odds) };
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
    uint32_t mapSize = map.size();
    map.merge(mapOdd);
    EXPECT_FALSE(mapOdd.is_empty());
    EXPECT_EQ(mapOdd.size(), count);
    EXPECT_EQ(map.size(), mapSize + 11);

    // Step 6:
    //  - merge even map with the `map`
    //  Result:
    //      - the even and the target `map` maps changed the sizes
    //      - the odd map contains only those entries, which `map` had before merge (9 entries)
    //      - the size of `map` increased by 12
    mapSize = map.size();
    map.merge(std::move(mapEven));
    EXPECT_FALSE(mapEven.is_empty());
    EXPECT_EQ(mapEven.size(), count - 1);
    EXPECT_EQ(map.size(), mapSize + 11 + 1);

    // Step 7: make checkups to make sure that only elements that were not in `map` where merged.
    for (int i = 0; i < static_cast<int>(map.size()); ++i)
    {
        EXPECT_EQ(map.value_at(i), i);
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

    EXPECT_FALSE(map.contains(static_cast<int>(map.size())));
}

/**
 * \brief   Test addIfUnique method of the Hash Map object.
 **/
TEST(MapTest, test_add_unique)
{
    using Map = areg::OrderedMap<int, int>;
    using POS = Map::MAPPOS;
    constexpr uint32_t count{ 10 };

    constexpr int unique[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int notunique[]{ 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };

    Map map;
    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = map.add_if_unique(notunique[i], notunique[i]);
        EXPECT_TRUE(map.is_valid_position(result.first));
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
        std::pair<POS, bool> result = map.add_if_unique(unique[i], unique[i]);
        EXPECT_TRUE(map.is_valid_position(result.first));
        EXPECT_TRUE(result.second);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = map.add_if_unique(notunique[i], notunique[i]);
        EXPECT_TRUE(map.is_valid_position(result.first));
        EXPECT_FALSE(result.second);
    }
}

/**
 * \brief   Test update method of the Hash Map object.
 **/
TEST(MapTest, test_update)
{
    using Map = areg::OrderedMap<int, int>;
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
        map.update_at(i, arr[i]);
        EXPECT_EQ(map[i], arr[i]);
    }
}

/**
 * \brief   Test nextEntry method of the Hash Map object.
 **/
TEST(MapTest, test_next_entry)
{
    using Map = areg::OrderedMap<int, int>;
    using POS = Map::MAPPOS;
    constexpr uint32_t count{ 10 };

    Map map;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        map[i] = i;
    }

    uint32_t idx{ 0 };
    POS pos = map.first_position();
    while (map.is_valid_position(pos))
    {
        int Key{ -1 }, Value{ -1 };
        if (idx < 9)
        {
            EXPECT_TRUE(map.next_entry(pos, Key, Value));
            EXPECT_TRUE((Key == Value) && (Key != -1));
        }
        else
        {
            EXPECT_FALSE(map.next_entry(pos, Key, Value));
            EXPECT_TRUE((Key == Value) && (Key == -1));
        }

        ++idx;
    }

    EXPECT_EQ(idx, count);
}

/**
 * \brief   Test Map streaming operators.
 **/
TEST(MapTest, test_streaming)
{
    using Map = areg::OrderedMap<int, int>;
    constexpr uint32_t count{ 10 };

    Map src;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        src[i] = i;
    }

    areg::SharedBuffer stream;
    stream << src;

    stream.move_to_begin();
    Map dst;
    stream >> dst;

    EXPECT_EQ(src, dst);
}
