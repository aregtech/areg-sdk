/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/FixedArrayTest.Cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Tests of TEHashMap object.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/TEHashMap.hpp"
#include "areg/base/SharedBuffer.hpp"

  /**
   * \brief   Test TEHashMap constructors.
   **/
TEST(TEHashMapTest, TestConstructors)
{
    using HashMap = TEHashMap<int, int>;
    constexpr uint32_t count{ 10 };

    HashMap hashMap1, hashMap2(10u);
    EXPECT_NE(hashMap1.getData().bucket_count(), 0u);
    EXPECT_NE(hashMap2.getData().bucket_count(), 0u);
    EXPECT_TRUE(hashMap1.isEmpty());
    EXPECT_TRUE(hashMap2.isEmpty());

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap1.setAt(i, i);
    }

    EXPECT_FALSE(hashMap1.isEmpty());
    EXPECT_EQ(hashMap1.getSize(), count);

    HashMap hashMap3(hashMap1);
    EXPECT_FALSE(hashMap3.isEmpty());
    EXPECT_FALSE(hashMap1.isEmpty());
    EXPECT_EQ(hashMap3, hashMap1);

    HashMap hashMap4(std::move(hashMap1));
    EXPECT_FALSE(hashMap4.isEmpty());
    EXPECT_NE(hashMap1.getData().bucket_count(), 0u);
    EXPECT_TRUE(hashMap1.isEmpty());
    EXPECT_EQ(hashMap3, hashMap4);
    EXPECT_EQ(hashMap1, hashMap2);
}

/**
 * \brief   Test TEHashMap operators.
 **/
TEST(TEHashMapTest, TestOperators)
{
    using HashMap = TEHashMap<int, int>;
    constexpr uint32_t count{ 10 };

    HashMap hashMap1, hashMap2(10u);
    EXPECT_TRUE(hashMap1 == hashMap2);

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap1.setAt(i, i);
    }

    EXPECT_FALSE(hashMap1.isEmpty());
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

    EXPECT_EQ(hashMap1.getSize(), count * 2);
    for (int i = static_cast<int>(count); i < static_cast<int>(count * 2); ++i)
    {
        hashMap1.removeAt(i);
    }

    HashMap hashMap3;
    hashMap3 = std::move(hashMap1);
    EXPECT_FALSE(hashMap3.isEmpty());
    EXPECT_NE(hashMap1.getData().bucket_count(), 0u);
    EXPECT_TRUE(hashMap1.isEmpty());
    EXPECT_TRUE(hashMap3 == hashMap2);

    for (int i = static_cast<int>(count); i < static_cast<int>(count * 2); ++i)
    {
        hashMap1.setAt(i, i);
        hashMap2.setAt(i, i);
    }

    EXPECT_TRUE(hashMap3 != hashMap1);
    EXPECT_TRUE(hashMap3 != hashMap2);
    EXPECT_TRUE(hashMap1 != hashMap2);

    hashMap1.clear();
    hashMap3.release();
    EXPECT_TRUE(hashMap1.isEmpty());
    EXPECT_TRUE(hashMap3.isEmpty());
    EXPECT_EQ(hashMap1, hashMap3);
}

/**
 * \brief   Test TEHashMap positioning attributes.
 **/
TEST(TEHashMapTest, TestPositionAttributes)
{
    using HashMap = TEHashMap<int, int>;
    constexpr uint32_t count{ 10 };

    HashMap hashMap;
    auto invPos = hashMap.invalidPosition();
    EXPECT_TRUE(hashMap.isInvalidPosition(invPos));

    auto invFirst = hashMap.firstPosition();
    EXPECT_FALSE(hashMap.isStartPosition(invFirst));

    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        auto invalid = hashMap.find(i);
        EXPECT_FALSE(hashMap.isValidPosition(invalid));
        EXPECT_TRUE(hashMap.isInvalidPosition(invalid));
        EXPECT_FALSE(hashMap.checkPosition(invalid));
        EXPECT_FALSE(hashMap.isStartPosition(invalid));

        hashMap[i] = i;
        
        auto valid = hashMap.find(i);
        EXPECT_TRUE(hashMap.isValidPosition(valid));
        EXPECT_FALSE(hashMap.isInvalidPosition(valid));
        EXPECT_TRUE(hashMap.checkPosition(valid));

        if (i == 0u)
        {
            EXPECT_TRUE(hashMap.isStartPosition(valid));
        }
        else
        {
            EXPECT_FALSE(hashMap.isStartPosition(valid));
        }
    }

    auto first = hashMap.firstPosition();
    EXPECT_EQ(hashMap.valueAtPosition(first), 0);
    EXPECT_TRUE(hashMap.isStartPosition(first));

    EXPECT_TRUE(invPos == hashMap.invalidPosition());
}

/**
 * \brief   Test TEHashMap positioning operations.
 **/
TEST(TEHashMapTest, TestPositionOperations)
{
    using HashMap = TEHashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr int32_t coef{ 2 };

    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap.setAt(i, i * coef);
        EXPECT_EQ(hashMap.getAt(i), i * coef);
    }

    POS pos = hashMap.firstPosition();
    EXPECT_TRUE(hashMap.isValidPosition(pos));

    uint32_t cnt{ 0 };
    while (hashMap.isValidPosition(pos))
    {
        int Key, Value;
        hashMap.getAtPosition(pos, Key, Value);
        EXPECT_TRUE(Value == Key * coef);
        EXPECT_TRUE(Value == hashMap.valueAtPosition(pos));
        EXPECT_TRUE(Key == hashMap.keyAtPosition(pos));

        POS cur = pos;
        int nextKey, nextValue;

        pos = hashMap.nextPosition(cur, nextKey, nextValue);
        EXPECT_TRUE((nextKey, Key + 1) || hashMap.isInvalidPosition(pos));
        EXPECT_TRUE((nextValue, Value + coef) || hashMap.isInvalidPosition(pos));

        pos = hashMap.nextPosition(cur);
        ++cnt;
        EXPECT_TRUE((hashMap.isValidPosition(pos)) || (cnt == count));
    }

    EXPECT_TRUE(cnt == count);
    EXPECT_TRUE(hashMap.isInvalidPosition(pos));
}

/**
 * \brief   Test TEHashMap positioning manipulation.
 **/
TEST(TEHashMapTest, TestPositionManipolation)
{
    using HashMap = TEHashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr int32_t coef{ 2 };

    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap[i * coef] = i;
    }

    uint32_t cnt{ 0 };
    for (POS pos = hashMap.firstPosition(); hashMap.isValidPosition(pos); ++cnt, pos = hashMap.nextPosition(pos))
    {
        hashMap.setPosition(pos, cnt * coef);
        EXPECT_EQ(cnt * coef, hashMap.keyAtPosition(pos));
        EXPECT_EQ(cnt * coef, hashMap.valueAtPosition(pos));
        EXPECT_EQ(hashMap.keyAtPosition(pos), hashMap.valueAtPosition(pos));
    }

    EXPECT_EQ(cnt, count);
    int idx{ 0 };
    for (POS pos = hashMap.firstPosition(); hashMap.isValidPosition(pos); --cnt, ++ idx)
    {
        int Key, Value;
        pos = hashMap.removePosition(pos, Key, Value);
        EXPECT_EQ(idx * coef, Key);
        EXPECT_EQ(idx * coef, Value);
        EXPECT_EQ(Key, Value);
    }

    EXPECT_EQ(cnt, 0);
}


/**
 * \brief   Test TEHashMap searching functionalities.
 **/
TEST(TEHashMapTest, TestSearching)
{
    using HashMap = TEHashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };
    constexpr uint32_t listSize{ 20 };

    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap[i] = i;
    }

    for (int i = 0; i < listSize; ++i)
    {
        const int Key{ i };
        int Value{ -1 };
        POS pos = hashMap.find(i);
        bool found = hashMap.find(Key, Value);
        if (i < static_cast<int>(count))
        {
            EXPECT_TRUE(hashMap.isValidPosition(pos));
            EXPECT_TRUE(found);
            EXPECT_TRUE(hashMap.contains(Key));
            EXPECT_EQ(hashMap.valueAtPosition(pos), Value);
            EXPECT_EQ(hashMap.keyAtPosition(pos), Key);
            EXPECT_EQ(Key, Value);
        }
        else
        {
            EXPECT_FALSE(hashMap.isValidPosition(pos));
            EXPECT_FALSE(found);
            EXPECT_FALSE(hashMap.contains(Key));
            EXPECT_NE(Key, Value);
        }
    }
}

/**
 * \brief   Test TEHashMap merging.
 **/
TEST(TEHashMapTest, TestMerging)
{
    using HashMap = TEHashMap<int, int>;
    constexpr uint32_t count{ 10 };

    HashMap hashMap1, hashMap2;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap1[i]     = i;        //  0,  1,  2,  3,  4,  5,  6,  7,  8,  9
        hashMap2[i + 10]= i + 10;   // 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
    }

    HashMap hashMap;
    hashMap.merge(hashMap1);
    EXPECT_TRUE(hashMap1.isEmpty());
    EXPECT_FALSE(hashMap.isEmpty());
    EXPECT_NE(hashMap1, hashMap);

    hashMap1 = hashMap;
    EXPECT_EQ(hashMap1, hashMap);

    hashMap.merge(std::move(hashMap1));
    EXPECT_FALSE(hashMap1.isEmpty());
    EXPECT_FALSE(hashMap.isEmpty());
    EXPECT_EQ(hashMap.getSize(), count);
    EXPECT_EQ(hashMap1, hashMap);

    hashMap.merge(std::move(hashMap2));
    EXPECT_TRUE(hashMap2.isEmpty());
    EXPECT_FALSE(hashMap.isEmpty());
    EXPECT_NE(hashMap2, hashMap);
    EXPECT_NE(hashMap1, hashMap);

    constexpr int32_t odds []   { 1, 3, 5, 7,  9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41 };
    constexpr int32_t evens[]   { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42 };
    constexpr uint32_t lenOdd   { MACRO_ARRAYLEN(odds) };
    constexpr uint32_t lenEven  { MACRO_ARRAYLEN(odds) };
    constexpr uint32_t mrg      { 11 };
    constexpr uint32_t rem      {  9 };

    HashMap mapOdd, mapEven;
    for (uint32_t i = 0u; i < lenOdd; ++i)
    {
        mapOdd[odds[i]] = odds[i];
    }
    for (uint32_t i = 0u; i < lenEven; ++i)
    {
        mapEven[evens[i]] = evens[i];
    }

    uint32_t mapSize = hashMap.getSize();
    hashMap.merge(mapOdd);
    EXPECT_FALSE(mapOdd.isEmpty());
    EXPECT_EQ(mapOdd.getSize(), count);
    EXPECT_EQ(hashMap.getSize(), mapSize + 11);

    mapSize = hashMap.getSize();
    hashMap.merge(std::move(mapEven));
    EXPECT_FALSE(mapEven.isEmpty());
    EXPECT_EQ(mapEven.getSize(), count - 1);
    EXPECT_EQ(hashMap.getSize(), mapSize + 11 + 1);

    for (int i = 0; i < static_cast<int>(hashMap.getSize()); ++i)
    {
        EXPECT_EQ(hashMap.getAt(i), i);
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

    EXPECT_FALSE(hashMap.contains(static_cast<int>(hashMap.getSize())));
}

/**
 * \brief   Test addIfUnique method of the Hash Map object.
 **/
TEST(TEHashMapTest, TestAddUnique)
{
    using HashMap = TEHashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };

    constexpr int unique    []{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int notunique []{ 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };

    HashMap hashMap;
    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = hashMap.addIfUnique(notunique[i], notunique[i]);
        EXPECT_TRUE(hashMap.isValidPosition(result.first));
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
        std::pair<POS, bool> result = hashMap.addIfUnique(unique[i], unique[i]);
        EXPECT_TRUE(hashMap.isValidPosition(result.first));
        EXPECT_TRUE(result.second);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        std::pair<POS, bool> result = hashMap.addIfUnique(notunique[i], notunique[i]);
        EXPECT_TRUE(hashMap.isValidPosition(result.first));
        EXPECT_FALSE(result.second);
    }
}

/**
 * \brief   Test update method of the Hash Map object.
 **/
TEST(TEHashMapTest, TestUpdate)
{
    using HashMap = TEHashMap<int, int>;
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
        hashMap.updateAt(i, arr[i]);
        EXPECT_EQ(hashMap[i], arr[i]);
    }
}

/**
 * \brief   Test nextEntry method of the Hash Map object.
 **/
TEST(TEHashMapTest, TestNextEntry)
{
    using HashMap = TEHashMap<int, int>;
    using POS = HashMap::MAPPOS;
    constexpr uint32_t count{ 10 };

    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap[i] = i;
    }

    int idx{ 0 };
    POS pos = hashMap.firstPosition();
    while (hashMap.isValidPosition(pos))
    {
        int Key{-1}, Value{-1};
        if (idx < 9)
        {
            EXPECT_TRUE(hashMap.nextEntry(pos, Key, Value));
            EXPECT_EQ(Key, idx + 1);
            EXPECT_EQ(Value, idx + 1);
        }
        else
        {
            EXPECT_FALSE(hashMap.nextEntry(pos, Key, Value));
            EXPECT_EQ(Key, -1);
            EXPECT_EQ(Value, -1);
        }

        ++idx;
    }
}

/**
 * \brief   Test Hash Map streaming operators.
 **/
TEST(TEHashMapTest, TestStreaming)
{
    using HashMap = TEHashMap<int, int>;
    constexpr uint32_t count{ 10 };

    HashMap src;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        src[i] = i;
    }

    SharedBuffer stream;
    stream << src;

    stream.moveToBegin();
    HashMap dst;
    stream >> dst;

    EXPECT_EQ(src, dst);
}
