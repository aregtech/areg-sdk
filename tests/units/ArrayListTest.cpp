/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ArrayListTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of ArrayList object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/ArrayList.hpp"
#include "areg/base/SharedBuffer.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <type_traits>

/**
 * \brief   Test ArrayList constructors.
 **/
TEST(ArrayListTest, test_constructors)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3 };
    constexpr int _len{ std::size(_arr) };
    constexpr int _capacity{ 5 };
    std::vector vec{ 1, 2, 3, 4, 5 };

    Array arr1;
    ASSERT_TRUE(arr1.is_empty());
    EXPECT_EQ(arr1.size(), 0u);
    EXPECT_EQ(arr1.capacity(), areg::ARRAY_DEFAULT_CAPACITY);

    Array arr2(_capacity, 0);
    ASSERT_TRUE(arr2.is_empty());
    EXPECT_EQ(arr2.size(), 0u);
    EXPECT_EQ(arr2.capacity(), static_cast<uint32_t>(_capacity));

    Array arr3(_capacity, 3u);
    EXPECT_EQ(arr3.size(), 3u);
    EXPECT_EQ(arr3.capacity(), static_cast<uint32_t>(std::max(3, _capacity)));

    Array arr4(_arr, _len);
    EXPECT_EQ(arr4.size(), static_cast<uint32_t>(_len));
    EXPECT_EQ(arr4.capacity(), std::max(static_cast<uint32_t>(_len), areg::ARRAY_DEFAULT_CAPACITY));

    Array arr5(arr4);
    EXPECT_EQ(arr5.size(), static_cast<uint32_t>(_len));
    EXPECT_EQ(arr5.capacity(), static_cast<uint32_t>(_len));

    EXPECT_EQ(arr4, arr5);

    Array arr6(vec);
    EXPECT_EQ(arr6, vec);

    Array arr7(std::move(vec));
    EXPECT_NE(arr6, vec);
    EXPECT_TRUE(vec.empty());

    Array arr8(std::move(arr6));
    EXPECT_TRUE(arr6.is_empty());
    EXPECT_EQ(arr8, arr7);
}

/**
 * \brief   Tests the validity of the array index.
 **/
TEST(ArrayListTest, test_index_validity)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3 };
    constexpr int _len{ std::size(_arr) };

    constexpr int _capacity{ 5 };

    Array arr1;
    ASSERT_FALSE(arr1.is_valid_index(0));
    ASSERT_FALSE(arr1.is_valid_index(1));

    Array arr2(_capacity, 0);
    ASSERT_FALSE(arr2.is_valid_index(0));
    ASSERT_FALSE(arr2.is_valid_index(1));

    Array arr3(_capacity, 3);
    ASSERT_TRUE(arr3.is_valid_index(0));
    ASSERT_TRUE(arr3.is_valid_index(1));

    Array arr4(_arr, _len);
    ASSERT_TRUE(arr4.is_valid_index(0));
    ASSERT_TRUE(arr4.is_valid_index(1));

    Array arr5(arr4);
    ASSERT_TRUE(arr5.is_valid_index(0));
    ASSERT_TRUE(arr5.is_valid_index(1));
}

/**
 * \brief   Tests the content of the array.
 **/
TEST(ArrayListTest, test_array_content)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3 };
    constexpr uint32_t _len{ std::size(_arr) };

    constexpr int _capacity{ 5 };

    Array arr3(_capacity, 3);
    EXPECT_EQ(arr3.size(), 3u);
    EXPECT_EQ(arr3.capacity(), static_cast<uint32_t>(std::max(3, _capacity)));

    Array arr4(_arr, _len);
    EXPECT_EQ(arr4.size(), _len);
    EXPECT_EQ(arr4.capacity(), std::max(_len, areg::ARRAY_DEFAULT_CAPACITY));

    Array arr5(arr4);
    EXPECT_EQ(arr5.size(), _len);
    EXPECT_EQ(arr5.capacity(), _len);

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

    EXPECT_EQ(arr4.data(), _vec);
    EXPECT_EQ(arr5.data(), _vec);
    EXPECT_EQ(arr4, arr5);

    arr4.free_extra();
    EXPECT_EQ(arr4.capacity(), _len);

    arr4.clear();
    EXPECT_TRUE(arr4.is_empty());

    EXPECT_NE(arr4, arr5);
}

/**
 * \brief   Tests setting, getting and the content of the values of the array.
 **/
TEST(ArrayListTest, test_get_set_and_content)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr[]{ 1, 2, 3, 4, 5 };
    constexpr uint32_t _len{ std::size(_arr) };

    Array arr4(_arr, _len);
    EXPECT_EQ(arr4.size(), _len);
    EXPECT_EQ(arr4.capacity(), std::max(_len, areg::ARRAY_DEFAULT_CAPACITY));
    const int* values = arr4.values();
    ASSERT_TRUE(::memcmp(values, _arr, sizeof(int) * _len) == 0);

    Array arr5(_len, _len);
    EXPECT_EQ(arr5.size(), _len);
    for (uint32_t i = 0; i < _len; ++i)
    {
        ASSERT_TRUE(arr5.is_valid_index(i));
        arr5.set_value_at(i, arr4.value_at(i));
    }

    for (uint32_t i = 0; i < _len; ++i)
    {
        EXPECT_EQ(arr5[i], arr4[i]);
        EXPECT_EQ(arr5.value_at(i), _arr[i]);
    }
}

/**
 * \brief   Tests the 'add' method of the array.
 **/
TEST(ArrayListTest, test_add)
{
    using Array = areg::ArrayList<int>;

    constexpr int _unique[]{ 1, 2, 3, 4, 5 };
    constexpr int _mixed[] { 1, 2, 3, 4, 5, 5, 4, 3, 2, 1 };

    constexpr uint32_t _lenUnique{ std::size(_unique) };
    constexpr uint32_t _lenMixed{ std::size(_mixed) };

    Array arrUnique;
    for (uint32_t i = 0; i < _lenUnique; ++i)
    {
        arrUnique.add(_unique[i]);
        EXPECT_EQ(arrUnique.size(), (i + 1));
    }

    ASSERT_EQ(arrUnique.size(), _lenUnique);

    Array arrMixed;
    for (uint32_t i = 0; i < _lenUnique; ++i)
    {
        uint32_t size{ static_cast<uint32_t>(arrMixed.size()) };

        if (arrMixed.find(_mixed[i]) == areg::INVALID_INDEX)
        {
            ASSERT_TRUE(arrMixed.add_if_unique(_mixed[i]));
            ASSERT_EQ(arrMixed.size(), (size + 1));
        }
        else
        {
            ASSERT_FALSE(arrMixed.add_if_unique(_mixed[i]));
            ASSERT_EQ(arrMixed.size(), (size + 0));
        }
    }

    ASSERT_NE(arrMixed.size(), _lenMixed);
    ASSERT_EQ(arrUnique.size(), arrMixed.size());

    for (uint32_t i = 0; i < arrMixed.size(); ++i)
    {
        ASSERT_FALSE(arrUnique.add_if_unique(arrMixed[i]));
    }

    ASSERT_EQ(arrUnique.size(), arrMixed.size());
    const int* values = arrMixed.values();
    ASSERT_TRUE(::memcmp(values, _unique, sizeof(int) * _lenUnique) == 0);
    uint32_t lenUnq = arrUnique.size();
    uint32_t lenMix = arrMixed.size();

    for (uint32_t i = 0; i < arrMixed.size(); ++i)
    {
        arrUnique.add(arrMixed[i]);
    }

    ASSERT_EQ(arrUnique.size(), (lenUnq + lenMix));
}

/**
 * \brief   Tests the 'append' method of the array.
 **/
TEST(ArrayListTest, test_append)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 1, 2, 3, 4, 5 };
    constexpr int _arr2[]{ 6, 7, 8, 9, 0 };
    constexpr int _arr3[]{ 1, 3, 5, 7, 9 };

    constexpr uint32_t _len1{ std::size(_arr1) };
    constexpr uint32_t _len2{ std::size(_arr2) };
    constexpr uint32_t _len3{ std::size(_arr3) };

    Array arr1;
    arr1.append(Array(_arr1, _len1)).append(Array(_arr2, _len2));
    EXPECT_EQ(arr1.size(), _len1 + _len2);
    EXPECT_EQ(arr1[0u], _arr1[0]);
    EXPECT_EQ(arr1[_len1 + _len2 - 1], _arr2[_len2 - 1]);

    arr1.append(Array(_arr3, _len3));
    const int* values = arr1.values();
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
    ASSERT_TRUE(arr1.is_empty());
}
/**
 * \brief   Tests the 'copy' and 'move' methods of the array.
 **/

TEST(ArrayListTest, test_copy_move)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 1, 2, 3, 4, 5 };
    constexpr int _arr2[]{ 6, 7, 8, 9, 0, 1, 2, 3, 4, 5 };

    constexpr uint32_t _len1{ std::size(_arr1) };
    constexpr uint32_t _len2{ std::size(_arr2) };

    Array arr1(_arr1, _len1), arr2(_arr2, _len2);
    const int* data1 = arr1.values();
    const int* data2 = arr2.values();
    ASSERT_TRUE(arr1 != arr2);
    ASSERT_TRUE(arr1.size() != arr2.size());
    ASSERT_TRUE(memcmp(data1, data2, std::min(_len1, _len2) * sizeof(int)) != 0);

    arr2.copy(arr1);
    data1 = arr1.values();
    data2 = arr2.values();
    ASSERT_TRUE(arr1 == arr2);
    ASSERT_TRUE(arr1.size() == arr2.size());
    ASSERT_TRUE(memcmp(data1, data2, _len1 * sizeof(int)) == 0);

    arr2.copy(Array(_arr2, _len2));
    EXPECT_EQ(arr2.size(), _len2);
    arr1.move(std::move(arr2));
    EXPECT_EQ(arr1.size(), _len2);
    ASSERT_TRUE(memcmp(arr1.values(), _arr2, _len2 * sizeof(int)) == 0);
    ASSERT_TRUE(arr2.is_empty());
}

/**
 * \brief   Tests the 'insert' method of the array.
 **/
TEST(ArrayListTest, test_insert)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr int _arr2[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int _arr3[]{ 5, 6, 7, 8, 9 };
    constexpr uint32_t _len1{ std::size(_arr1) };
    constexpr uint32_t _len2{ std::size(_arr2) };
    constexpr uint32_t _len3{ std::size(_arr3) };

    Array arr1(_arr1, _len1);
    Array arr2;

    for (uint32_t i = 0; i < _len2; ++i)
    {
        arr2.insert_at(i, static_cast<int>(i), 1);
        EXPECT_EQ(arr2[i], _arr2[i]);
    }

    arr2.clear();
    for (uint32_t i = 0; i < _len2; ++i)
    {
        uint32_t idx = i % 2;
        arr2.insert_at(idx, _arr2[i], 1);
        EXPECT_EQ(arr2[idx], _arr2[i]);
    }

    arr2 = arr1;
    EXPECT_EQ(arr2.size(), arr1.size());
    EXPECT_EQ(arr2, arr1);
    arr2.insert_at(arr1.size(), 0, 5);
    EXPECT_EQ(arr2.size(), arr1.size() + 5);
    arr2.insert_at(arr1.size(), _arr3, _len3);
    ASSERT_TRUE(memcmp(arr2.values(), _arr2, _len2 * sizeof(int)) == 0);
    arr2.insert_at(arr1.size(), arr1);
    ASSERT_TRUE(memcmp(arr2.values() + arr1.size(), arr1.values(), arr1.size() * sizeof(int)) == 0);
}

/**
 * \brief   Tests the 'removeAt' method of the array.
 **/
TEST(ArrayListTest, test_remove_at)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ std::size(_arr1) };

    Array arr(_arr1, _len1);
    arr.remove_at(2, 1);
    EXPECT_EQ(arr.size(), _len1 - 1u);
    EXPECT_NE(arr[2u], 2);
    arr.remove_at(2, _len1);
    EXPECT_EQ(arr.size(), 2u);
    arr.remove_at(0, _len1);
    ASSERT_TRUE(arr.is_empty());
}

/**
 * \brief   Tests the 'position' method of the array.
 **/
TEST(ArrayListTest, test_remove_position)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr2[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr uint32_t _len2{ std::size(_arr2) };

    Array arr(_arr2, _len2);

    for (int32_t i = static_cast<int>(_len2 - 1u); i >= 0; --i)
    {
        EXPECT_EQ(arr.remove_at(static_cast<uint32_t>(i)), i);
    }

    arr.insert_at(0, _arr2, _len2);
    EXPECT_EQ(arr.size(), _len2);
    EXPECT_EQ(arr.remove_at(5u), 5);
    EXPECT_EQ(arr.size(), _len2 - 1u);
}

/**
 * \brief   Tests the 'removeElem' method of the array.
 **/
TEST(ArrayListTest, test_remove_element)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ std::size(_arr1) };

    Array arr(_arr1, _len1);
    for (int i = static_cast<int>(_len1 - 1u); i >= 0; --i)
    {
        ASSERT_TRUE(arr.remove_elem(i));
    }

    ASSERT_TRUE(arr.is_empty());
}

/**
 * \brief   Tests the 'find' method (searching) of the array.
 **/
TEST(ArrayListTest, test_find)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ std::size(_arr1) };

    Array arr(_arr1, _len1);
    for (int i = 10; i >= 0; --i)
    {
        if (_len1 <= static_cast<uint32_t>(i))
        {
            EXPECT_EQ(arr.find(i), areg::INVALID_INDEX);
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
        EXPECT_EQ(arr.find(i, static_cast<uint32_t>(i) + 1u), areg::INVALID_INDEX);
    }

    EXPECT_FALSE(arr.contains(0, _len1 * 2));
}

/**
 * \brief   Tests the 'resize' method of the array.
 **/
TEST(ArrayListTest, test_resize)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ std::size(_arr1) };

    Array arr;
    ASSERT_TRUE(arr.is_empty());
    arr.resize(_len1);
    ASSERT_FALSE(arr.is_empty());
    EXPECT_EQ(arr.size(), _len1);
    for (uint32_t i = 0; i < _len1; ++i)
    {
        arr[i] = _arr1[i];
    }

    arr.resize(_len1 * 2);
    EXPECT_EQ(arr.size(), _len1 * 2);

    for (uint32_t i = 0; i < _len1; ++i)
    {
        EXPECT_EQ(arr[i], _arr1[i]);
        arr.set_value_at(i + _len1, _arr1[i]);
    }

    const int* values = arr.values();
    const uint32_t len = arr.size();
    EXPECT_EQ(len, _len1 * 2);
    ASSERT_TRUE(::memcmp(values, values + _len1, _len1) == 0);
}

/**
 * \brief   Tests the 'reserve' method of the array.
 **/
TEST(ArrayListTest, test_reserve)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4 };
    constexpr uint32_t _len1{ std::size(_arr1) };

    Array arr;
    ASSERT_TRUE(arr.is_empty());

    arr.reserve(_len1);
    ASSERT_TRUE(arr.is_empty());
    ASSERT_TRUE(arr.capacity() >= _len1);
    arr.copy(Array(_arr1, _len1));
    arr.free_extra();
    ASSERT_TRUE(arr.capacity() == _len1);

    EXPECT_EQ(arr.size(), _len1);
    arr.reserve(_len1 * 100);
    EXPECT_EQ(arr.size(), _len1);
    ASSERT_TRUE(arr.capacity() >= (_len1 * 100));
    arr.free_extra();
    ASSERT_TRUE(arr.capacity() == _len1);
    arr.release();
    ASSERT_TRUE(arr.is_empty());
}

/**
 * \brief   Tests the 'shift' method of the array.
 **/
TEST(ArrayListTest, test_shift)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5 };
    constexpr uint32_t _len1{ std::size(_arr1) };

    Array arr;
    ASSERT_TRUE(arr.is_empty());

    arr.shift(0, _len1);
    ASSERT_TRUE(arr.is_empty());
    arr.shift(_len1, _len1);
    ASSERT_TRUE(arr.is_empty());

    Array origin(_arr1, _len1);
    arr = std::move(origin);
    ASSERT_TRUE(origin.is_empty());
    origin = arr;

    EXPECT_EQ(arr.size(), _len1);
    arr.shift(0, 1);
    EXPECT_EQ(arr.size(), _len1 + 1);
    arr.shift(3, 1);
    EXPECT_EQ(arr.size(), _len1 + 2);
    arr.shift(arr.size() - 1, 1);
    EXPECT_EQ(arr.size(), _len1 + 3);
    arr.shift(arr.size(), 1);
    EXPECT_EQ(arr.size(), _len1 + 3);

    arr.shift(arr.size(), -1);
    EXPECT_EQ(arr.size(), _len1 + 3);
    arr.shift(arr.size() - 1, -1);
    EXPECT_EQ(arr.size(), _len1 + 2);
    arr.shift(3 + 1, -1);
    EXPECT_EQ(arr.size(), _len1 + 1);
    arr.shift(0 + 1, -1);
    EXPECT_EQ(arr.size(), _len1);

    EXPECT_EQ(arr, origin);

    arr.shift(0, -1);
    EXPECT_EQ(arr.size(), _len1);
    arr.shift(arr.size(), -1);
    EXPECT_EQ(arr.size(), _len1);
    arr.shift(4, -5);
    EXPECT_EQ(arr.size(), _len1 - 4);
}

/**
 * \brief   Tests the 'first' and 'last' methods of the array to access elements by reference.
 **/
TEST(ArrayListTest, test_entries)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5 };
    constexpr uint32_t _len1{ std::size(_arr1) };

    Array arr, temp(_arr1, _len1);
    ASSERT_TRUE(arr.is_empty());
    ASSERT_FALSE(temp.is_empty());

    arr = std::move(temp);
    ASSERT_FALSE(arr.is_empty());
    ASSERT_TRUE(temp.is_empty());


    const int& first = arr.first_entry();
    EXPECT_EQ(first, _arr1[0u]);
    EXPECT_EQ(first, arr[0u]);
    EXPECT_EQ(&first, &arr[0u]);
    EXPECT_NE(first, arr[1u]);

    const int& last = arr.last_entry();
    EXPECT_EQ(last, _arr1[_len1 - 1u]);
    EXPECT_EQ(last, arr[_len1 - 1u]);
    EXPECT_EQ(&last, &arr[_len1 - 1u]);
    EXPECT_NE(last, arr[_len1 - 2u]);

    arr.first_entry() = 5;
    EXPECT_EQ(arr[0u], 5);
    arr.last_entry() = 0;
    EXPECT_EQ(arr[_len1 - 1u], 0);
}

/**
 * \brief   Tests the streaming operators of the array.
 **/
TEST(ArrayListTest, test_stream)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5 };
    constexpr uint32_t _len1{ std::size(_arr1) };

    Array dst, src(_arr1, _len1);
    ASSERT_TRUE(dst.is_empty());
    ASSERT_FALSE(src.is_empty());

    areg::SharedBuffer stream;
    stream << src;
    stream.move_to_begin();
    stream >> dst;

    EXPECT_EQ(dst.size(), _len1);
    EXPECT_EQ(src, dst);
}

/**
 * \brief   Tests ascending sorting of array.
 **/
TEST(ArrayListTest, test_ascending)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr int _res1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr uint32_t _len{ std::size(_arr1) };

    Array arr1(_arr1, _len), res1(_res1, _len);
    arr1.sort([](const int elem1, const int elem2) { return (elem1 < elem2); });

    EXPECT_NE(arr1, Array(_arr1, _len));
    EXPECT_EQ(arr1.size(), _len);
    EXPECT_EQ(arr1, res1);
}

/**
 * \brief   Tests descending sorting of array.
 **/
TEST(ArrayListTest, test_descending)
{
    using Array = areg::ArrayList<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int _res1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr uint32_t _len{ std::size(_arr1) };

    Array arr1(_arr1, _len), res1(_res1, _len);
    arr1.sort([](const int elem1, const int elem2) { return (elem1 > elem2); });

    EXPECT_NE(arr1, Array(_arr1, _len));
    EXPECT_EQ(arr1.size(), _len);
    EXPECT_EQ(arr1, res1);
}

/**
 * \brief   The index operator takes any integer type and returns a writable element.
 **/
TEST(ArrayListTest, test_index_any_integer_type)
{
    using Array = areg::ArrayList<int>;
    enum ePos { PosFirst, PosSecond };

    constexpr int _arr[]{ 10, 20, 30, 40 };
    constexpr uint32_t _len{ std::size(_arr) };

    Array arr(_arr, _len);
    const Array & carr{ arr };

    static_assert(std::is_same_v<decltype(arr[0]), int &>, "An int index selects the member operator.");
    static_assert(std::is_same_v<decltype(arr[std::ptrdiff_t{ 0 }]), int &>, "A ptrdiff_t index selects the member operator.");
    static_assert(std::is_same_v<decltype(arr[std::size_t{ 0 }]), int &>, "A size_t index selects the member operator.");
    static_assert(std::is_same_v<decltype(carr[0]), const int &>, "A const array returns a const element.");

    EXPECT_EQ(arr[0], 10);
    EXPECT_EQ(arr[1u], 20);
    EXPECT_EQ(arr[std::size_t{ 2 }], 30);
    EXPECT_EQ(arr[std::ptrdiff_t{ 3 }], 40);
    EXPECT_EQ(arr[int64_t{ 0 }], 10);
    EXPECT_EQ(arr[uint64_t{ 3 }], 40);
    EXPECT_EQ(arr[uint8_t{ 1 }], 20);
    EXPECT_EQ(arr[int16_t{ 2 }], 30);
    EXPECT_EQ(arr[PosSecond], 20);
    EXPECT_EQ(carr[0], 10);
    EXPECT_EQ(carr[std::ptrdiff_t{ 3 }], 40);
    EXPECT_EQ(carr[PosFirst], 10);

    arr[0] = 11;
    arr[std::ptrdiff_t{ 1 }] = 21;
    arr[std::size_t{ 2 }] = 31;
    arr[PosFirst] += 1;
    EXPECT_EQ(carr[0], 12);
    EXPECT_EQ(carr[1], 21);
    EXPECT_EQ(carr[2], 31);
    EXPECT_EQ(carr[3], 40);
}

// A null source or zero capacity creates an empty array.
TEST(ArrayListTest, boundary_null_source_and_zero_capacity_are_empty)
{
    const areg::ArrayList<int32_t> nullSource(static_cast<const int32_t*>(nullptr), 3u);
    EXPECT_TRUE(nullSource.is_empty());
    areg::ArrayList<int32_t> zero(0u, 0u);
    EXPECT_TRUE(zero.is_empty());
    EXPECT_EQ(zero.capacity(), 0u);
    EXPECT_FALSE(zero.contains(1, 0u));
}

// Setting the next element appends it without leaving gaps.
TEST(ArrayListTest, boundary_lvalue_set_appends_without_creating_index_gaps)
{
    areg::ArrayList<int32_t> list(0u, 0u);
    const int32_t value{ 17 };
    list.set_value_at(12u, value);
    ASSERT_EQ(list.size(), 1u);
    EXPECT_EQ(list[0], value);
    list.set_value_at(0u, value);
    EXPECT_EQ(list.size(), 1u);
    const int32_t* values{ list };
    EXPECT_EQ(values, list.data().data());
    EXPECT_EQ(values[0], value);
}

// Zero-count changes leave the contents and reserved storage alone.
TEST(ArrayListTest, boundary_zero_count_mutations_preserve_reserved_storage)
{
    const int32_t source[]{ 1, 2, 3 };
    areg::ArrayList<int32_t> list(source, 3u);
    list.reserve(16u);
    const uint32_t capacity{ list.capacity() };
    const int32_t* values{ list.data().data() };
    list.insert_at(0u, 9, 0u);
    list.insert_at(0u, static_cast<const int32_t*>(nullptr), 3u);
    list.insert_at(0u, source, 0u);
    list.insert_at(0u, std::vector<int32_t>{});
    list.remove_at(list.size(), 0u);
    list.shift(0u, 0);
    list.shift(list.size(), 1);
    EXPECT_EQ(list.data(), (std::vector<int32_t>{ 1, 2, 3 }));
    EXPECT_EQ(list.capacity(), capacity);
    EXPECT_EQ(list.data().data(), values);
}

// Shifting within reserved storage keeps the remaining elements in order.
TEST(ArrayListTest, boundary_shift_with_reserved_space_and_clamped_removal)
{
    const int32_t source[]{ 1, 2, 3, 4 };
    areg::ArrayList<int32_t> list(source, 4u);
    list.reserve(16u);
    const uint32_t capacity{ list.capacity() };
    list.shift(2u, 2);
    ASSERT_EQ(list.size(), 6u);
    EXPECT_EQ(list[0], 1);
    EXPECT_EQ(list[1], 2);
    EXPECT_EQ(list[4], 3);
    EXPECT_EQ(list[5], 4);
    list.shift(4u, -20);
    EXPECT_EQ(list.data(), (std::vector<int32_t>{ 3, 4 }));
    list.remove_at(1u, 50u);
    EXPECT_EQ(list.data(), (std::vector<int32_t>{ 3 }));
    EXPECT_FALSE(list.remove_elem(3, list.size()));
    EXPECT_FALSE(list.remove_elem(9));
    EXPECT_EQ(list.capacity(), capacity);
}

// Growing during a shift must preserve the elements after the insertion point.
TEST(ArrayListTest, DISABLED_boundary_shift_reallocation_preserves_tail)
{
    areg::ArrayList<int32_t> list(1u, 1u);
    list[0] = 17;
    const uint32_t count{ list.capacity() + 1u };
    list.shift(0u, static_cast<int32_t>(count));
    ASSERT_EQ(list.size(), count + 1u);
    EXPECT_EQ(list[count], 17);
}

// The most negative shift must remove only the available prefix.
TEST(ArrayListTest, DISABLED_boundary_minimum_negative_shift_clamps_to_prefix)
{
    const int32_t source[]{ 1, 2, 3 };
    areg::ArrayList<int32_t> list(source, 3u);
    list.shift(1u, std::numeric_limits<int32_t>::min());
    EXPECT_EQ(list.data(), (std::vector<int32_t>{ 2, 3 }));
}

// The reported stream size matches the bytes written for the array.
TEST(ArrayListTest, boundary_required_size_matches_serialized_values)
{
    const int32_t source[]{ 1, 2, 3 };
    const areg::ArrayList<int32_t> list(source, 3u);
    EXPECT_EQ(areg::required_size<areg::ArrayList<int32_t>>{}(list), 4u + 3u * sizeof(int32_t));
    EXPECT_EQ(areg::required_size<std::vector<int32_t>>{}({}), 4u);
}
