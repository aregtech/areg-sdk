/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/TEArrayListTest.Cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Tests of TEArrayList object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/TEArrayList.hpp"

TEST(ArrayListTest, TestConstructors)
{
    using Array = TEArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3 };
    constexpr int _len{ MACRO_ARRAYLEN(_arr) };

    constexpr int _capacity{ 5 };

    Array arr1;
    ASSERT_TRUE(arr1.isEmpty());
    EXPECT_EQ(arr1.getSize(), 0);
    EXPECT_EQ(arr1.getCapacity(), NECommon::ARRAY_DEFAULT_CAPACITY);

    Array arr2(_capacity, 0);
    ASSERT_TRUE(arr2.isEmpty());
    EXPECT_EQ(arr2.getSize(), 0);
    EXPECT_EQ(arr2.getCapacity(), _capacity);

    Array arr3(_capacity, 3);
    EXPECT_EQ(arr3.getSize(), 3);
    EXPECT_EQ(arr3.getCapacity(), MACRO_MAX(3, _capacity));

    Array arr4(_arr, _len);
    EXPECT_EQ(arr4.getSize(), _len);
    EXPECT_EQ(arr4.getCapacity(), MACRO_MAX(_len, NECommon::ARRAY_DEFAULT_CAPACITY));

    Array arr5(arr4);
    EXPECT_EQ(arr5.getSize(), _len);
    EXPECT_EQ(arr5.getCapacity(), _len);

    EXPECT_EQ(arr4, arr5);
}

TEST(ArrayListTest, TestIndexValidity)
{
    using Array = TEArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3 };
    constexpr int _len{ MACRO_ARRAYLEN(_arr) };

    constexpr int _capacity{ 5 };

    Array arr1;
    ASSERT_FALSE(arr1.isValidIndex(0));
    ASSERT_FALSE(arr1.isValidIndex(1));

    Array arr2(_capacity, 0);
    ASSERT_FALSE(arr2.isValidIndex(0));
    ASSERT_FALSE(arr2.isValidIndex(1));

    Array arr3(_capacity, 3);
    ASSERT_TRUE(arr3.isValidIndex(0));
    ASSERT_TRUE(arr3.isValidIndex(1));

    Array arr4(_arr, _len);
    ASSERT_TRUE(arr4.isValidIndex(0));
    ASSERT_TRUE(arr4.isValidIndex(1));

    Array arr5(arr4);
    ASSERT_TRUE(arr5.isValidIndex(0));
    ASSERT_TRUE(arr5.isValidIndex(1));
}

TEST(ArrayListTest, TestArrayContent)
{
    using Array = TEArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3 };
    constexpr int _len{ MACRO_ARRAYLEN(_arr) };

    constexpr int _capacity{ 5 };

    Array arr3(_capacity, 3);
    EXPECT_EQ(arr3.getSize(), 3);
    EXPECT_EQ(arr3.getCapacity(), MACRO_MAX(3, _capacity));

    Array arr4(_arr, _len);
    EXPECT_EQ(arr4.getSize(), _len);
    EXPECT_EQ(arr4.getCapacity(), MACRO_MAX(_len, NECommon::ARRAY_DEFAULT_CAPACITY));

    Array arr5(arr4);
    EXPECT_EQ(arr5.getSize(), _len);
    EXPECT_EQ(arr5.getCapacity(), _len);

    std::vector<int> _vec;
    for (int i = 0; i < _len; ++i)
    {
        _vec.push_back(_arr[i]);
        EXPECT_EQ(arr4[i], _arr[i]);
        EXPECT_EQ(arr5[i], _arr[i]);
        ASSERT_FALSE(arr3.contains(_arr[i], 0));
        ASSERT_TRUE(arr4.contains(_arr[i], 0));
        ASSERT_TRUE(arr5.contains(_arr[i], 0));
    }

    EXPECT_EQ(arr4.getData(), _vec);
    EXPECT_EQ(arr5.getData(), _vec);
    EXPECT_EQ(arr4, arr5);

    arr4.freeExtra();
    EXPECT_EQ(arr4.getCapacity(), _len);

    arr4.clear();
    EXPECT_TRUE(arr4.isEmpty());

    EXPECT_NE(arr4, arr5);
}

TEST(ArrayListTest, TestGetSetAndContent)
{
    using Array = TEArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3, 4, 5 };
    constexpr int _len{ MACRO_ARRAYLEN(_arr) };

    Array arr4(_arr, _len);
    EXPECT_EQ(arr4.getSize(), _len);
    EXPECT_EQ(arr4.getCapacity(), MACRO_MAX(_len, NECommon::ARRAY_DEFAULT_CAPACITY));
    const int* values = arr4.getValues();
    ASSERT_TRUE(::memcmp(values, _arr, sizeof(int) * _len) == 0);

    Array arr5(_len, _len);
    EXPECT_EQ(arr5.getSize(), _len);
    for (int i = 0; i < _len; ++i)
    {
        ASSERT_TRUE(arr5.isValidIndex(i));
        arr5.setAt(i, arr4.getAt(i));
    }

    for (int i = 0; i < _len; ++i)
    {
        EXPECT_EQ(arr5[i], arr4[i]);
        EXPECT_EQ(arr5.valueAtPosition(i), _arr[i]);
    }
}

TEST(ArrayListTest, TestAdd)
{
    using Array = TEArrayList<int>;

    constexpr int _unique[]{ 1, 2, 3, 4, 5 };
    constexpr int _mixed[] { 1, 2, 3, 4, 5, 5, 4, 3, 2, 1 };

    constexpr int _lenUnique{ MACRO_ARRAYLEN(_unique) };
    constexpr int _lenMixed{ MACRO_ARRAYLEN(_mixed) };

    Array arrUnique;
    for (int i = 0; i < _lenUnique; ++i)
    {
        arrUnique.add(_unique[i]);
        EXPECT_EQ(arrUnique.getSize(), (i + 1));
    }

    ASSERT_EQ(arrUnique.getSize(), _lenUnique);

    Array arrMixed;
    for (int i = 0; i < _lenUnique; ++i)
    {
        int size{ static_cast<int>(arrMixed.getSize()) };

        if (arrMixed.find(_mixed[i]) == NECommon::INVALID_INDEX)
        {
            ASSERT_TRUE(arrMixed.addIfUnique(_mixed[i]));
            ASSERT_EQ(arrMixed.getSize(), (size + 1));
        }
        else
        {
            ASSERT_FALSE(arrMixed.addIfUnique(_mixed[i]));
            ASSERT_EQ(arrMixed.getSize(), (size + 0));
        }
    }

    ASSERT_NE(arrMixed.getSize(), _lenMixed);
    ASSERT_EQ(arrUnique.getSize(), arrMixed.getSize());

    for (int i = 0; i < static_cast<int>(arrMixed.getSize()); ++i)
    {
        ASSERT_FALSE(arrUnique.addIfUnique(arrMixed[i]));
    }

    ASSERT_EQ(arrUnique.getSize(), arrMixed.getSize());
    const int* values = arrMixed.getValues();
    ASSERT_TRUE(::memcmp(values, _unique, sizeof(int) * _lenUnique) == 0);
    int lenUnq = arrUnique.getSize();
    int lenMix = arrMixed.getSize();

    for (int i = 0; i < static_cast<int>(arrMixed.getSize()); ++i)
    {
        arrUnique.add(arrMixed[i]);
    }

    ASSERT_EQ(arrUnique.getSize(), (lenUnq + lenMix));
}
