/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/FixedArrayTest.cpp
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
TEST(TEFixedArrayTest, TestConstructors)
{
    using FixedArray = TEFixedArray<int>;
    constexpr uint32_t elemCount{ 10u };

    FixedArray empty, notEmpty(elemCount);

    EXPECT_TRUE(empty.isEmpty());
    EXPECT_FALSE(notEmpty.isEmpty());
    EXPECT_EQ(notEmpty.getSize(), elemCount);

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
    EXPECT_FALSE(src.isEmpty());
    EXPECT_FALSE(toMove.isEmpty());
    EXPECT_EQ(toCopy, toMove);

    toMove.clear();
    EXPECT_TRUE(toMove.isEmpty());
    EXPECT_EQ(toMove.getSize(), 0u);
}

/**
 * \brief   Testing copy and move methods of Fixed Array
 **/
TEST(TEFixedArrayTest, TestCopyMove)
{
    using FixedArray = TEFixedArray<int>;
    constexpr uint32_t elemCount{ 10u };

    FixedArray empty, notEmpty(elemCount);

    for (uint32_t i = 0; i < notEmpty.getSize(); ++i)
    {
        notEmpty[i] = static_cast<int>(i);
    }

    empty.copy(notEmpty);
    EXPECT_EQ(empty, notEmpty);

    FixedArray toCopy;
    toCopy.copy(notEmpty);
    EXPECT_FALSE(notEmpty.isEmpty());
    EXPECT_FALSE(toCopy.isEmpty());
    EXPECT_EQ(empty, toCopy);

    FixedArray toMove;
    toMove.move(std::move(notEmpty));
    EXPECT_TRUE(notEmpty.isEmpty());
    EXPECT_FALSE(toMove.isEmpty());
    EXPECT_EQ(toCopy, toMove);

    FixedArray src(15);
    for (uint32_t i = 0; i < src.getSize(); ++i)
    {
        src[i] = 10 + static_cast<int>(i);
    }

    toCopy.copy(src);
    EXPECT_FALSE(src.isEmpty());
    EXPECT_FALSE(toCopy.isEmpty());
    EXPECT_EQ(src, toCopy);
    EXPECT_NE(empty, toCopy);

    toMove.move(std::move(src));
    EXPECT_FALSE(src.isEmpty());
    EXPECT_FALSE(toMove.isEmpty());
    EXPECT_EQ(toCopy, toMove);

    toMove.clear();
    EXPECT_TRUE(toMove.isEmpty());
    EXPECT_EQ(toMove.getSize(), 0u);
}

/**
 * \brief   Test values of the fixed array
 **/
TEST(TEFixedArrayTest, TestValues)
{
    using FixedArray = TEFixedArray<int>;
    constexpr uint32_t elemCount{ 10u };
    constexpr int32_t arr[]{ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    FixedArray notEmpty(elemCount);
    const int* dst = notEmpty.getValues();
    const int* src = arr;
    for (uint32_t i = elemCount; i > 0; -- i)
    {
        EXPECT_TRUE(notEmpty.isValidIndex(elemCount - i));
        notEmpty.setAt(elemCount - i, *src ++);
    }

    EXPECT_TRUE(::memcmp(dst, arr, elemCount * sizeof(int)) == 0);
}

/**
 * \brief   Testing searching methods like 'find' and 'contains' of fixed array
 **/
TEST(TEFixedArrayTest, TestSearchElem)
{
    using FixedArray = TEFixedArray<int>;
    constexpr uint32_t elemCount{ 10u };

    FixedArray notEmpty(elemCount);
    for (uint32_t i = 0; i < notEmpty.getSize(); ++i)
    {
        notEmpty[i] = static_cast<int>(i);
    }

    for (uint32_t i = 0; i < notEmpty.getSize(); ++i)
    {
        EXPECT_TRUE(notEmpty.contains(static_cast<int>(i), 0u));
        EXPECT_TRUE(notEmpty.contains(static_cast<int>(i), i));
        EXPECT_FALSE(notEmpty.contains(static_cast<int>(i), i + 1u));
        EXPECT_FALSE(notEmpty.contains(static_cast<int>(i + elemCount), 0u));

        EXPECT_EQ(notEmpty.find(static_cast<int>(i), 0u), static_cast<int>(i));
        EXPECT_EQ(notEmpty.find(static_cast<int>(i), i), static_cast<int>(i));
        EXPECT_EQ(notEmpty.find(static_cast<int>(i), i + elemCount), NECommon::INVALID_INDEX);
        EXPECT_EQ(notEmpty.find(static_cast<int>(i + elemCount), 0u), NECommon::INVALID_INDEX);
    }
}

/**
 * \brief   Testing the position of the element
 **/
TEST(TEFixedArrayTest, TestElemPosition)
{
    using FixedArray = TEFixedArray<int>;
    constexpr uint32_t elemCount{ 10u };

    FixedArray notEmpty(elemCount);
    for (uint32_t i = 0; i < notEmpty.getSize(); ++i)
    {
        notEmpty[i] = static_cast<int>(i);
        EXPECT_EQ(notEmpty.getAt(i), static_cast<int>(i));
        EXPECT_EQ(notEmpty.getAt(i), notEmpty.valueAtPosition(i));
    }

    EXPECT_EQ(notEmpty.firstEntry(), 0u);
    EXPECT_EQ(notEmpty.lastEntry(), elemCount - 1u);
}
