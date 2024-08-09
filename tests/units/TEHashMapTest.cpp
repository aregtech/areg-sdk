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
    constexpr uint32_t count{ 10 };

    HashMap hashMap;
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        hashMap.setAt(i, i);
        EXPECT_EQ(hashMap.getAt(i), i);
    }
}
