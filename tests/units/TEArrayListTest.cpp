/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/TEArrayListTest.cpp
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
#include "areg/base/SharedBuffer.hpp"

/**
 * \brief   Test TEArrayList constructors.
 **/
TEST(TEArrayListTest, TestConstructors)
{
    using Array = TEArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3 };
    constexpr int _len{ MACRO_ARRAYLEN(_arr) };
    constexpr int _capacity{ 5 };
    std::vector vec{ 1, 2, 3, 4, 5 };

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

    Array arr6(vec);
    EXPECT_EQ(arr6, vec);

    Array arr7(std::move(vec));
    EXPECT_NE(arr6, vec);
    EXPECT_TRUE(vec.empty());

    Array arr8(std::move(arr6));
    EXPECT_TRUE(arr6.isEmpty());
    EXPECT_EQ(arr8, arr7);
}

/**
 * \brief   Tests the validity of the array index.
 **/
TEST(TEArrayListTest, TestIndexValidity)
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

/**
 * \brief   Tests the content of the array.
 **/
TEST(TEArrayListTest, TestArrayContent)
{
    using Array = TEArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3 };
    constexpr uint32_t _len{ MACRO_ARRAYLEN(_arr) };

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
    for (uint32_t i = 0; i < _len; ++i)
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

/**
 * \brief   Tests setting, getting and the content of the values of the array.
 **/
TEST(TEArrayListTest, TestGetSetAndContent)
{
    using Array = TEArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3, 4, 5 };
    constexpr uint32_t _len{ MACRO_ARRAYLEN(_arr) };

    Array arr4(_arr, _len);
    EXPECT_EQ(arr4.getSize(), _len);
    EXPECT_EQ(arr4.getCapacity(), MACRO_MAX(_len, NECommon::ARRAY_DEFAULT_CAPACITY));
    const int* values = arr4.getValues();
    ASSERT_TRUE(::memcmp(values, _arr, sizeof(int) * _len) == 0);

    Array arr5(_len, _len);
    EXPECT_EQ(arr5.getSize(), _len);
    for (uint32_t i = 0; i < _len; ++i)
    {
        ASSERT_TRUE(arr5.isValidIndex(i));
        arr5.setAt(i, arr4.getAt(i));
    }

    for (uint32_t i = 0; i < _len; ++i)
    {
        EXPECT_EQ(arr5[i], arr4[i]);
        EXPECT_EQ(arr5.valueAtPosition(i), _arr[i]);
    }
}

/**
 * \brief   Tests the 'add' method of the array.
 **/
TEST(TEArrayListTest, TestAdd)
{
    using Array = TEArrayList<int>;

    constexpr int _unique[]{ 1, 2, 3, 4, 5 };
    constexpr int _mixed[] { 1, 2, 3, 4, 5, 5, 4, 3, 2, 1 };

    constexpr uint32_t _lenUnique{ MACRO_ARRAYLEN(_unique) };
    constexpr uint32_t _lenMixed{ MACRO_ARRAYLEN(_mixed) };

    Array arrUnique;
    for (uint32_t i = 0; i < _lenUnique; ++i)
    {
        arrUnique.add(_unique[i]);
        EXPECT_EQ(arrUnique.getSize(), (i + 1));
    }

    ASSERT_EQ(arrUnique.getSize(), _lenUnique);

    Array arrMixed;
    for (uint32_t i = 0; i < _lenUnique; ++i)
    {
        uint32_t size{ static_cast<uint32_t>(arrMixed.getSize()) };

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

    for (uint32_t i = 0; i < arrMixed.getSize(); ++i)
    {
        ASSERT_FALSE(arrUnique.addIfUnique(arrMixed[i]));
    }

    ASSERT_EQ(arrUnique.getSize(), arrMixed.getSize());
    const int* values = arrMixed.getValues();
    ASSERT_TRUE(::memcmp(values, _unique, sizeof(int) * _lenUnique) == 0);
    uint32_t lenUnq = arrUnique.getSize();
    uint32_t lenMix = arrMixed.getSize();

    for (uint32_t i = 0; i < arrMixed.getSize(); ++i)
    {
        arrUnique.add(arrMixed[i]);
    }

    ASSERT_EQ(arrUnique.getSize(), (lenUnq + lenMix));
}

/**
 * \brief   Tests the 'append' method of the array.
 **/
TEST(TEArrayListTest, TestAppend)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 1, 2, 3, 4, 5 };
    constexpr int _arr2[]{ 6, 7, 8, 9, 0 };
    constexpr int _arr3[]{ 1, 3, 5, 7, 9 };

    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };
    constexpr uint32_t _len2{ MACRO_ARRAYLEN(_arr2) };
    constexpr uint32_t _len3{ MACRO_ARRAYLEN(_arr3) };

    Array arr1;
    arr1.append(Array(_arr1, _len1)).append(Array(_arr2, _len2));
    EXPECT_EQ(arr1.getSize(), _len1 + _len2);
    EXPECT_EQ(arr1[0u], _arr1[0]);
    EXPECT_EQ(arr1[_len1 + _len2 - 1], _arr2[_len2 - 1]);

    arr1.append(Array(_arr3, _len3));
    const int* values = arr1.getValues();
    ASSERT_TRUE(values != nullptr);
    for (uint32_t i = 0; i < _len1; ++i)
    {
        EXPECT_EQ(values[i], _arr1[i]);
    }

    ASSERT_TRUE(::memcmp(values, _arr1, _len1 * sizeof(int)) == 0);
    values += _len1;

    for (uint32_t i = 0; i < _len2; ++i)
    {
        EXPECT_EQ(values[i], _arr2[i]);
    }

    ASSERT_TRUE(::memcmp(values, _arr2, _len2 * sizeof(int)) == 0);
    values += _len2;

    for (uint32_t i = 0; i < _len3; ++i)
    {
        EXPECT_EQ(values[i], _arr3[i]);
    }

    ASSERT_TRUE(::memcmp(values, _arr3, _len3 * sizeof(int)) == 0);

    arr1.clear();
    ASSERT_TRUE(arr1.isEmpty());
}
/**
 * \brief   Tests the 'copy' and 'move' methods of the array.
 **/

TEST(TEArrayListTest, TestCopyMove)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 1, 2, 3, 4, 5 };
    constexpr int _arr2[]{ 6, 7, 8, 9, 0, 1, 2, 3, 4, 5 };

    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };
    constexpr uint32_t _len2{ MACRO_ARRAYLEN(_arr2) };

    Array arr1(_arr1, _len1), arr2(_arr2, _len2);
    const int* data1 = arr1.getValues();
    const int* data2 = arr2.getValues();
    ASSERT_TRUE(arr1 != arr2);
    ASSERT_TRUE(arr1.getSize() != arr2.getSize());
    ASSERT_TRUE(memcmp(data1, data2, MACRO_MIN(_len1, _len2) * sizeof(int)) != 0);

    arr2.copy(arr1);
    data1 = arr1.getValues();
    data2 = arr2.getValues();
    ASSERT_TRUE(arr1 == arr2);
    ASSERT_TRUE(arr1.getSize() == arr2.getSize());
    ASSERT_TRUE(memcmp(data1, data2, _len1 * sizeof(int)) == 0);

    arr2.copy(Array(_arr2, _len2));
    EXPECT_EQ(arr2.getSize(), _len2);
    arr1.move(std::move(arr2));
    EXPECT_EQ(arr1.getSize(), _len2);
    ASSERT_TRUE(memcmp(arr1.getValues(), _arr2, _len2 * sizeof(int)) == 0);
    ASSERT_TRUE(arr2.isEmpty());
}

/**
 * \brief   Tests the 'insert' method of the array.
 **/
TEST(TEArrayListTest, TestInsert)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr int _arr2[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int _arr3[]{ 5, 6, 7, 8, 9 };
    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };
    constexpr uint32_t _len2{ MACRO_ARRAYLEN(_arr2) };
    constexpr uint32_t _len3{ MACRO_ARRAYLEN(_arr3) };

    Array arr1(_arr1, _len1);
    Array arr2;

    for (uint32_t i = 0; i < _len2; ++i)
    {
        arr2.insertAt(i, static_cast<int>(i), 1);
        EXPECT_EQ(arr2[i], _arr2[i]);
    }

    arr2.clear();
    for (uint32_t i = 0; i < _len2; ++i)
    {
        uint32_t idx = i % 2;
        arr2.insertAt(idx, _arr2[i], 1);
        EXPECT_EQ(arr2[idx], _arr2[i]);
    }

    arr2 = arr1;
    EXPECT_EQ(arr2.getSize(), arr1.getSize());
    EXPECT_EQ(arr2, arr1);
    arr2.insertAt(arr1.getSize(), 0, 5);
    EXPECT_EQ(arr2.getSize(), arr1.getSize() + 5);
    arr2.insertAt(arr1.getSize(), _arr3, _len3);
    ASSERT_TRUE(memcmp(arr2.getValues(), _arr2, _len2 * sizeof(int)) == 0);
    arr2.insertAt(arr1.getSize(), arr1);
    ASSERT_TRUE(memcmp(arr2.getValues() + arr1.getSize(), arr1.getValues(), arr1.getSize() * sizeof(int)) == 0);
}

/**
 * \brief   Tests the 'removeAt' method of the array.
 **/
TEST(TEArrayListTest, TestRemoveAt)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };

    Array arr(_arr1, _len1);
    arr.removeAt(2, 1);
    EXPECT_EQ(arr.getSize(), _len1 - 1u);
    EXPECT_NE(arr[2u], 2);
    arr.removeAt(2, _len1);
    EXPECT_EQ(arr.getSize(), 2u);
    arr.removeAt(0, _len1);
    ASSERT_TRUE(arr.isEmpty());
}

/**
 * \brief   Tests the 'position' method of the array.
 **/
TEST(TEArrayListTest, TestRemovePosition)
{
    using Array = TEArrayList<int>;

    constexpr int _arr2[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr uint32_t _len2{ MACRO_ARRAYLEN(_arr2) };

    Array arr(_arr2, _len2);

    for (int32_t i = static_cast<int>(_len2 - 1u); i >= 0; --i)
    {
        EXPECT_EQ(arr.removePosition(static_cast<uint32_t>(i)), i);
    }

    arr.insertAt(0, _arr2, _len2);
    EXPECT_EQ(arr.getSize(), _len2);
    EXPECT_EQ(arr.removePosition(5u), 5);
    EXPECT_EQ(arr.getSize(), _len2 - 1u);
}

/**
 * \brief   Tests the 'removeElem' method of the array.
 **/
TEST(TEArrayListTest, TestRemoveElement)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };

    Array arr(_arr1, _len1);
    for (int i = static_cast<int>(_len1 - 1u); i >= 0; --i)
    {
        ASSERT_TRUE(arr.removeElem(i));
    }

    ASSERT_TRUE(arr.isEmpty());
}

/**
 * \brief   Tests the 'find' method (searching) of the array.
 **/
TEST(TEArrayListTest, TestFind)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };

    Array arr(_arr1, _len1);
    for (int i = 10; i >= 0; --i)
    {
        if (_len1 <= static_cast<uint32_t>(i))
        {
            EXPECT_EQ(arr.find(i), NECommon::INVALID_INDEX);
        }
        else
        {
            EXPECT_EQ(arr.find(i), i);
        }
    }

    for (int i = 0; i < static_cast<int>(_len1); ++i)
    {
        EXPECT_TRUE(arr.contains(i, 0));
        EXPECT_EQ(arr.find(i, 0), i);
        
        EXPECT_TRUE(arr.contains(i, static_cast<uint32_t>(i)));
        EXPECT_EQ(arr.find(i, static_cast<uint32_t>(i)), i);

        EXPECT_FALSE(arr.contains(i, static_cast<uint32_t>(i) + 1u));
        EXPECT_EQ(arr.find(i, static_cast<uint32_t>(i) + 1u), NECommon::INVALID_INDEX);
    }

    EXPECT_FALSE(arr.contains(0, _len1 * 2));
}

/**
 * \brief   Tests the 'resize' method of the array.
 **/
TEST(TEArrayListTest, TestResize)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };

    Array arr;
    ASSERT_TRUE(arr.isEmpty());
    arr.resize(_len1);
    ASSERT_FALSE(arr.isEmpty());
    EXPECT_EQ(arr.getSize(), _len1);
    for (uint32_t i = 0; i < _len1; ++i)
    {
        arr[i] = _arr1[i];
    }

    arr.resize(_len1 * 2);
    EXPECT_EQ(arr.getSize(), _len1 * 2);

    for (uint32_t i = 0; i < _len1; ++i)
    {
        EXPECT_EQ(arr[i], _arr1[i]);
        arr.setAt(i + _len1, _arr1[i]);
    }

    const int* values = arr.getValues();
    const uint32_t len = arr.getSize();
    EXPECT_EQ(len, _len1 * 2);
    ASSERT_TRUE(::memcmp(values, values + _len1, _len1) == 0);
}

/**
 * \brief   Tests the 'reserve' method of the array.
 **/
TEST(TEArrayListTest, TestReserve)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };

    Array arr;
    ASSERT_TRUE(arr.isEmpty());

    arr.reserve(_len1);
    ASSERT_TRUE(arr.isEmpty());
    ASSERT_TRUE(arr.getCapacity() >= _len1);
    arr.copy(Array(_arr1, _len1));
    arr.freeExtra();
    ASSERT_TRUE(arr.getCapacity() == _len1);

    EXPECT_EQ(arr.getSize(), _len1);
    arr.reserve(_len1 * 100);
    EXPECT_EQ(arr.getSize(), _len1);
    ASSERT_TRUE(arr.getCapacity() >= (_len1 * 100));
    arr.freeExtra();
    ASSERT_TRUE(arr.getCapacity() == _len1);
    arr.release();
    ASSERT_TRUE(arr.isEmpty());
}

/**
 * \brief   Tests the 'shift' method of the array.
 **/
TEST(TEArrayListTest, TestShift)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5 };
    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };

    Array arr;
    ASSERT_TRUE(arr.isEmpty());

    arr.shift(0, _len1);
    ASSERT_TRUE(arr.isEmpty());
    arr.shift(_len1, _len1);
    ASSERT_TRUE(arr.isEmpty());

    Array origin(_arr1, _len1);
    arr = std::move(origin);
    ASSERT_TRUE(origin.isEmpty());
    origin = arr;

    EXPECT_EQ(arr.getSize(), _len1);
    arr.shift(0, 1);
    EXPECT_EQ(arr.getSize(), _len1 + 1);
    arr.shift(3, 1);
    EXPECT_EQ(arr.getSize(), _len1 + 2);
    arr.shift(arr.getSize() - 1, 1);
    EXPECT_EQ(arr.getSize(), _len1 + 3);
    arr.shift(arr.getSize(), 1);
    EXPECT_EQ(arr.getSize(), _len1 + 3);

    arr.shift(arr.getSize(), -1);
    EXPECT_EQ(arr.getSize(), _len1 + 3);
    arr.shift(arr.getSize() - 1, -1);
    EXPECT_EQ(arr.getSize(), _len1 + 2);
    arr.shift(3 + 1, -1);
    EXPECT_EQ(arr.getSize(), _len1 + 1);
    arr.shift(0 + 1, -1);
    EXPECT_EQ(arr.getSize(), _len1);

    EXPECT_EQ(arr, origin);

    arr.shift(0, -1);
    EXPECT_EQ(arr.getSize(), _len1);
    arr.shift(arr.getSize(), -1);
    EXPECT_EQ(arr.getSize(), _len1);
    arr.shift(4, -5);
    EXPECT_EQ(arr.getSize(), _len1 - 4);
}

/**
 * \brief   Tests the 'first' and 'last' methods of the array to access elements by reference.
 **/
TEST(TEArrayListTest, TestEntries)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5 };
    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };

    Array arr, temp(_arr1, _len1);
    ASSERT_TRUE(arr.isEmpty());
    ASSERT_FALSE(temp.isEmpty());

    arr = std::move(temp);
    ASSERT_FALSE(arr.isEmpty());
    ASSERT_TRUE(temp.isEmpty());


    const int& first = arr.firstEntry();
    EXPECT_EQ(first, _arr1[0u]);
    EXPECT_EQ(first, arr[0u]);
    EXPECT_EQ(&first, &arr[0u]);
    EXPECT_NE(first, arr[1u]);

    const int& last = arr.lastEntry();
    EXPECT_EQ(last, _arr1[_len1 - 1u]);
    EXPECT_EQ(last, arr[_len1 - 1u]);
    EXPECT_EQ(&last, &arr[_len1 - 1u]);
    EXPECT_NE(last, arr[_len1 - 2u]);

    arr.firstEntry() = 5;
    EXPECT_EQ(arr[0u], 5);
    arr.lastEntry() = 0;
    EXPECT_EQ(arr[_len1 - 1u], 0);
}

/**
 * \brief   Tests the streaming operators of the array.
 **/
TEST(TEArrayListTest, TestStream)
{
    using Array = TEArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5 };
    constexpr uint32_t _len1{ MACRO_ARRAYLEN(_arr1) };

    Array dst, src(_arr1, _len1);
    ASSERT_TRUE(dst.isEmpty());
    ASSERT_FALSE(src.isEmpty());

    SharedBuffer stream;
    stream << src;
    stream.moveToBegin();
    stream >> dst;

    EXPECT_EQ(dst.getSize(), _len1);
    EXPECT_EQ(src, dst);
}
