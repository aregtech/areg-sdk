/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/HashMapTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of HashMap object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/HashMap.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/MathDefs.hpp"

/**
 * \brief   Test HashMap constructors.
 **/
TEST(HashMapTest, test_constructors)
{
    using HashMap = areg::HashMap<int, int>;
    constexpr uint32_t count{ 10 };

    // Step 1: test default constructor and the constructor with the size of hash-table.
    //  Result: the hash-map is initialized and is empty.
    //  Note: bucket_count() behavior varies by STL implementation:
    //        - libstdc++ (GCC) pre-allocates buckets
    //        - libc++ (Apple Clang) uses lazy allocation (0 buckets until insertion)
    HashMap hashMap1, hashMap2(10u);
    EXPECT_TRUE(hashMap1.is_empty());
    EXPECT_TRUE(hashMap2.is_empty());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap1.set_value_at(i, i);
    }

    EXPECT_FALSE(hashMap1.is_empty());
    EXPECT_EQ(hashMap1.size(), count);

    // Step 2: Test copy-constructor
    //  Result: the elements are copied from the source and the source remains unchanged.
    HashMap hashMap3(hashMap1);
    EXPECT_FALSE(hashMap3.is_empty());
    EXPECT_FALSE(hashMap1.is_empty());
    EXPECT_EQ(hashMap3, hashMap1);

    // Step 2: test move-constructor
    //  Result: the elements are moved from the source and the source is empty.
    HashMap hashMap4(std::move(hashMap1));
    EXPECT_FALSE(hashMap4.is_empty());
    // Note: After move, hashMap1 is in a valid but unspecified state.
    // The bucket_count() may be 0 on some implementations (e.g., libc++ on macOS).
    EXPECT_TRUE(hashMap1.is_empty());
    EXPECT_EQ(hashMap3, hashMap4);
    EXPECT_EQ(hashMap1, hashMap2);

    // Step 3: initialize from array
    constexpr int _keys[]  {  1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    constexpr int _values[]{ 10, 9, 8, 7, 6, 5, 4, 3, 2,  1 };
    constexpr uint32_t _len{ std::size(_keys) };
    HashMap hashMap5(_keys, _values, _len);

    EXPECT_EQ(hashMap5.size(), _len);

    int _resKeys[_len]{ };
    int _resValues[_len]{ };
    EXPECT_EQ(hashMap5.elements(_resKeys, _resValues, _len), _len);
}

/**
 * \brief   Test HashMap operators.
 **/
TEST(HashMapTest, test_operators)
{
    using HashMap = areg::HashMap<int, int>;
    constexpr uint32_t count{ 10 };

    HashMap hashMap1, hashMap2(10u);
    EXPECT_TRUE(hashMap1 == hashMap2);

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap1.set_value_at(i, i);
    }

    EXPECT_FALSE(hashMap1.is_empty());
    EXPECT_TRUE(hashMap1 != hashMap2);

    hashMap2 = hashMap1;
    EXPECT_TRUE(hashMap1 == hashMap2);

    for (int i = 0; i < static_cast<int>(count * 2); ++i)
    {
        if (i < static_cast<int>(count))
        {
            EXPECT_EQ(hashMap1[i], i);
        }
        else
        {
            EXPECT_NE(hashMap1[i], i);
        }
    }

    EXPECT_EQ(hashMap1.size(), count * 2);
    for (int i = static_cast<int>(count); i < static_cast<int>(count * 2); ++i)
    {
        hashMap1.remove_at(i);
    }

    HashMap hashMap3;
    hashMap3 = std::move(hashMap1);
    EXPECT_FALSE(hashMap3.is_empty());
    // Note: After move, hashMap1 is in a valid but unspecified state.
    // The bucket_count() may be 0 on some implementations (e.g., libc++ on macOS).
    EXPECT_TRUE(hashMap1.is_empty());
    EXPECT_TRUE(hashMap3 == hashMap2);

    for (int i = static_cast<int>(count); i < static_cast<int>(count * 2); ++i)
    {
        hashMap1.set_value_at(i, i);
        hashMap2.set_value_at(i, i);
    }

    EXPECT_TRUE(hashMap3 != hashMap1);
    EXPECT_TRUE(hashMap3 != hashMap2);
    EXPECT_TRUE(hashMap1 != hashMap2);

    hashMap1.clear();
    hashMap3.release();
    EXPECT_TRUE(hashMap1.is_empty());
    EXPECT_TRUE(hashMap3.is_empty());
    EXPECT_EQ(hashMap1, hashMap3);
}

/**
 * \brief   Test HashMap positioning attributes.
 **/
TEST(HashMapTest, test_position_attributes)
{
    using HashMap = areg::HashMap<int, int>;
    constexpr uint32_t count{ 10 };

    HashMap hashMap;
    auto invPos = hashMap.invalid_position();
    EXPECT_TRUE(!hashMap.is_valid_position(invPos));

    // Step 1: in the empty hash-map, take the first position, which must be invalid
    auto invFirst = hashMap.first_position();
    EXPECT_FALSE(hashMap.is_first_position(invFirst));

    // Step 2: search for entries in the hash-map and check the validity of the position.
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        // Before the entry is inserted, the `find` method should return invalid position
        auto invalid = hashMap.find(i);
        EXPECT_FALSE(hashMap.is_valid_position(invalid));
        EXPECT_TRUE(!hashMap.is_valid_position(invalid));
        EXPECT_FALSE(hashMap.check_position(invalid));
        EXPECT_FALSE(hashMap.is_first_position(invalid));

        // Insert entry
        hashMap[i] = i;
        
        // After the entry exists, the `find` method should return valid position.
        auto valid = hashMap.find(i);
        EXPECT_TRUE(hashMap.is_valid_position(valid));
        EXPECT_FALSE(!hashMap.is_valid_position(valid));
        EXPECT_TRUE(hashMap.check_position(valid));
    }

    auto first = hashMap.first_position();
    EXPECT_TRUE(hashMap.is_first_position(first));
    EXPECT_TRUE(invPos == hashMap.invalid_position());
}

/**
 * \brief   Test HashMap positioning operations.
 **/
TEST(HashMapTest, test_position_operations)
{
    using HashMap = areg::HashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr int32_t coef{ 2 };

    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap.set_value_at(i, i * coef);
        EXPECT_EQ(hashMap.value_at(i), i * coef);
    }

    // Step 1: in the valid and initialized hash-map get first position, which is valid
    POS pos = hashMap.first_position();
    EXPECT_TRUE(hashMap.is_valid_position(pos));

    // Step 2: in the loop get the next position and checkup keys and values
    uint32_t cnt{ 0 };
    while (hashMap.is_valid_position(pos))
    {
        int Key, Value;
        hashMap.at_position(pos, Key, Value);
        EXPECT_TRUE(Value == Key * coef);
        EXPECT_TRUE(Value == hashMap.value_at(pos));
        EXPECT_TRUE(Key == hashMap.key_at(pos));

        POS cur = pos;
        int nextKey{ -1 }, nextValue{ -1 };

        pos = hashMap.next_position(cur, nextKey, nextValue);
        EXPECT_EQ(nextKey, Key);
        EXPECT_EQ(nextValue, Value);
        if (hashMap.is_valid_position(pos))
        {
            EXPECT_EQ(areg::delta(hashMap.key_at(pos), nextKey), 1);
            EXPECT_EQ(areg::delta(hashMap.value_at(pos), nextValue), coef);
        }

        pos = hashMap.next_position(cur);
        ++cnt;
        EXPECT_TRUE((hashMap.is_valid_position(pos)) || (cnt == count));
    }

    EXPECT_TRUE(cnt == count);
    EXPECT_TRUE(!hashMap.is_valid_position(pos));
}

/**
 * \brief   Test HashMap positioning manipulation.
 **/
TEST(HashMapTest, test_position_manipulation)
{
    using HashMap = areg::HashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr int32_t coef{ 2 };

    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap[i * coef] = i;
    }

    // Step 1: initialize a hash-map
    uint32_t cnt{ 0 };
    for (POS pos = hashMap.first_position(); hashMap.is_valid_position(pos); ++cnt, pos = hashMap.next_position(pos))
    {
        hashMap.set_value_at(pos, static_cast<int>(hashMap.value_at(pos) * coef));
        int Key = hashMap.key_at(pos);
        int Value = hashMap.value_at(pos);
        EXPECT_EQ(Key, Value);
    }

    // Step 2: change the values by position and make sure it is valid / changed.
    EXPECT_EQ(cnt, count);
    uint32_t idx{ 0 };
    for (POS pos = hashMap.first_position(); hashMap.is_valid_position(pos); --cnt, ++ idx)
    {
        int Key{-1}, Value{-1};
        pos = hashMap.remove_at(pos, Key, Value);
        EXPECT_EQ(Key, Value);
    }

    EXPECT_EQ(cnt, 0u);
    EXPECT_EQ(idx, count);
}


/**
 * \brief   Test HashMap searching functionalities.
 **/
TEST(HashMapTest, test_searching)
{
    using HashMap = areg::HashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr uint32_t listSize{ 20 };

    // Step 1: init hash-map [0 .. 9]
    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap[i] = i;
    }

    // Step 2: search keys [0 .. 19] and check the position validity:
    //  Result:
    //      - entries with keys [0 .. 9] have valid position
    //      - entries with keys [10 .. 19] have invalid position
    for (int i = 0; i < static_cast<int>(listSize); ++i)
    {
        const int Key{ i };
        int Value{ -1 };
        POS pos = hashMap.find(i);
        bool found = hashMap.find(Key, Value);
        if (i < static_cast<int>(count))
        {
            EXPECT_TRUE(hashMap.is_valid_position(pos));
            EXPECT_TRUE(found);
            EXPECT_TRUE(hashMap.contains(Key));
            EXPECT_EQ(hashMap.value_at(pos), Value);
            EXPECT_EQ(hashMap.key_at(pos), Key);
            EXPECT_EQ(Key, Value);
        }
        else
        {
            EXPECT_FALSE(hashMap.is_valid_position(pos));
            EXPECT_FALSE(found);
            EXPECT_FALSE(hashMap.contains(Key));
            EXPECT_NE(Key, Value);
        }
    }
}

/**
 * \brief   Test HashMap merging.
 **/
TEST(HashMapTest, test_merging)
{
    using HashMap = areg::HashMap<int, int>;
    constexpr uint32_t count{ 10 };

    // Step 1: initialize 2 hash-maps with entries [0 .. 9] and [10 .. 19]
    HashMap hashMap1, hashMap2;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap1[i]     = i;        //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
        hashMap2[i + 10]= i + 10;   // 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
    }

    // Step 2:  
    //  - create empty hash-map object `hashMap`
    //  - merge [0 .. 1] entries with `hashMap`.
    // Result: the source object is empty, all entries are extracted and merged
    HashMap hashMap;
    hashMap.merge(hashMap1);
    EXPECT_TRUE(hashMap1.is_empty());
    EXPECT_FALSE(hashMap.is_empty());
    EXPECT_NE(hashMap1, hashMap);

    // Step 3: 
    //  - assign entries [0 .. 9] to the empty hash-map
    //  - merge [0 .. 1] entries with `hashMap`, which already contains the keys [0 .. 9]
    //  Result: the source and `hashMap` hash-maps remain unchanged, because it already contained keys [0 .. 9]
    hashMap1 = hashMap;
    EXPECT_EQ(hashMap1, hashMap);
    hashMap.merge(std::move(hashMap1));
    EXPECT_FALSE(hashMap1.is_empty());
    EXPECT_FALSE(hashMap.is_empty());
    EXPECT_EQ(hashMap.size(), count);
    EXPECT_EQ(hashMap1, hashMap);

    // Step 4:
    //  - merge entries with keys [10 .. 19] by using `move` operation.
    // Result: the source hash-map is empty, the `hashMap` contains entries [0 .. 19]
    hashMap.merge(std::move(hashMap2));
    EXPECT_TRUE(hashMap2.is_empty());
    EXPECT_FALSE(hashMap.is_empty());
    EXPECT_NE(hashMap2, hashMap);
    EXPECT_NE(hashMap1, hashMap);

    // Step 5:
    //  - create and initialize 2 hash-maps, which keys and values are odd and even integers from 1 to 42 and totally have 21 entries each.
    constexpr int32_t odds []   { 1, 3, 5, 7,  9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41 };
    constexpr int32_t evens[]   { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42 };
    constexpr uint32_t lenOdd   { std::size(odds) };
    constexpr uint32_t lenEven  { std::size(odds) };
    HashMap mapOdd, mapEven;
    for (uint32_t i = 0u; i < lenOdd; ++i)
    {
        mapOdd[odds[i]] = odds[i];
    }
    for (uint32_t i = 0u; i < lenEven; ++i)
    {
        mapEven[evens[i]] = evens[i];
    }

    // Step 6:
    //  - merge odd hash-map with the `hashMap`
    //  Result:
    //      - the odd and the target `hashMap` hash-maps changed the sizes
    //      - the odd hash-map contains only those entries, which `hashMap` had before merge (10 entries)
    //      - the size of `hashMap` increased by 11
    uint32_t mapSize = hashMap.size();
    hashMap.merge(mapOdd);
    EXPECT_FALSE(mapOdd.is_empty());
    EXPECT_EQ(mapOdd.size(), count);
    EXPECT_EQ(hashMap.size(), mapSize + 11);

    // Step 6:
    //  - merge even hash-map with the `hashMap`
    //  Result:
    //      - the even and the target `hashMap` hash-maps changed the sizes
    //      - the odd hash-map contains only those entries, which `hashMap` had before merge (9 entries)
    //      - the size of `hashMap` increased by 12
    mapSize = hashMap.size();
    hashMap.merge(std::move(mapEven));
    EXPECT_FALSE(mapEven.is_empty());
    EXPECT_EQ(mapEven.size(), count - 1);
    EXPECT_EQ(hashMap.size(), mapSize + 11 + 1);

    // Step 7: make checkups to make sure that only elements that were not in `hashMap` where merged.
    for (int i = 0; i < static_cast<int>(hashMap.size()); ++i)
    {
        EXPECT_EQ(hashMap.value_at(i), i);
        if ((i > 0) && (i < 20))
        {
            if ((i % 2) != 0)
            {
                EXPECT_TRUE( mapOdd.contains(i));
                EXPECT_FALSE(mapEven.contains(i));
            }
            else
            {
                EXPECT_FALSE(mapOdd.contains(i));
                EXPECT_TRUE(mapEven.contains(i));
            }
        }
    }

    EXPECT_FALSE(hashMap.contains(static_cast<int>(hashMap.size())));
}

/**
 * \brief   Test addIfUnique method of the Hash Map object.
 **/
TEST(HashMapTest, test_add_unique)
{
    using HashMap = areg::HashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };

    constexpr int unique    []{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int notunique []{ 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };

    HashMap hashMap;
    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = hashMap.add_if_unique(notunique[i], notunique[i]);
        EXPECT_TRUE(hashMap.is_valid_position(result.first));
        if (i < 5)
        {
            EXPECT_TRUE(result.second);
        }
        else
        {
            EXPECT_FALSE(result.second);
        }
    }

    hashMap.release();
    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = hashMap.add_if_unique(unique[i], unique[i]);
        EXPECT_TRUE(hashMap.is_valid_position(result.first));
        EXPECT_TRUE(result.second);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = hashMap.add_if_unique(notunique[i], notunique[i]);
        EXPECT_TRUE(hashMap.is_valid_position(result.first));
        EXPECT_FALSE(result.second);
    }
}

/**
 * \brief   Test update method of the Hash Map object.
 **/
TEST(HashMapTest, test_update)
{
    using HashMap = areg::HashMap<int, int>;
    constexpr uint32_t count{ 10 };
    constexpr int arr[]{ 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap[i] = i;
    }

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        EXPECT_EQ(hashMap[i], i);
        hashMap.update_at(i, arr[i]);
        EXPECT_EQ(hashMap[i], arr[i]);
    }
}

/**
 * \brief   Test nextEntry method of the Hash Map object.
 **/
TEST(HashMapTest, test_next_entry)
{
    using HashMap = areg::HashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };

    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap[i] = i;
    }

    uint32_t idx{ 0 };
    POS pos = hashMap.first_position();
    while (hashMap.is_valid_position(pos))
    {
        int Key{-1}, Value{-1};
        if (idx < 9)
        {
            EXPECT_TRUE(hashMap.next_entry(pos, Key, Value));
            EXPECT_TRUE((Key == Value) && (Key != -1));
        }
        else
        {
            EXPECT_FALSE(hashMap.next_entry(pos, Key, Value));
            EXPECT_TRUE((Key == Value) && (Key == -1));
        }

        ++idx;
    }

    EXPECT_EQ(idx, count);
}

/**
 * \brief   Test Hash Map streaming operators.
 **/
TEST(HashMapTest, test_streaming)
{
    using HashMap = areg::HashMap<int, int>;
    constexpr uint32_t count{ 10 };

    HashMap src;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        src[i] = i;
    }

    areg::SharedBuffer stream;
    stream << src;

    stream.move_to_begin();
    HashMap dst;
    stream >> dst;

    EXPECT_EQ(src, dst);
}
