/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/TEMapTest.Cpp
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

    // Step 1: test default constructor and the constructor with the size of mao.
    //  Result: the empty map is initialized.
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
 * \brief   Test TEHashMap operators.
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
