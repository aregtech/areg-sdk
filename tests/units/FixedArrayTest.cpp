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
 *              Tests of TEFixedArray object.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/TEFixedArray.hpp"
#include "areg/base/SharedBuffer.hpp"

/**
 * \brief   Test TEArrayList constructors.
 **/
TEST(FixedArrayTest, TestConstructors)
{
    using FixedArray = TEFixedArray<int>;

    FixedArray empty, notEmpty(10);

    EXPECT_TRUE(empty.isEmpty());
    EXPECT_FALSE(notEmpty.isEmpty());

    for (uint32_t i = 0; i < notEmpty.getSize(); ++i)
    {
        notEmpty[i] = static_cast<int>(i);
    }

    empty = notEmpty;
    EXPECT_EQ(empty, notEmpty);

    FixedArray toCopy(notEmpty);
    EXPECT_FALSE(notEmpty.isEmpty());
    EXPECT_FALSE(toCopy.isEmpty());
    EXPECT_EQ(empty, toCopy);

    FixedArray toMove(std::move(notEmpty));
    EXPECT_TRUE(notEmpty.isEmpty());
    EXPECT_FALSE(toMove.isEmpty());
    EXPECT_EQ(toCopy, toMove);

    FixedArray src(15);
    for (uint32_t i = 0; i < src.getSize(); ++i)
    {
        src[i] = 10 + static_cast<int>(i);
    }

    toCopy = src;
    EXPECT_FALSE(src.isEmpty());
    EXPECT_FALSE(toCopy.isEmpty());
    EXPECT_EQ(src, toCopy);
    EXPECT_NE(empty, toCopy);

    toMove = std::move(src);
    EXPECT_TRUE(src.isEmpty());
    EXPECT_FALSE(toMove.isEmpty());
    EXPECT_EQ(toCopy, toMove);
}

TEST(FixedArrayTest, TestSearches)
{
    using FixedArray = TEFixedArray<int>;

    FixedArray empty, notEmpty(10);
    for (uint32_t i = 0; i < notEmpty.getSize(); ++i)
    {
        notEmpty[i] = static_cast<int>(i);
    }
}
