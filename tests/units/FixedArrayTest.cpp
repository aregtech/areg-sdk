/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/FixedArrayTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of FixedArray object.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/FixedArray.hpp"
#include "areg/base/SharedBuffer.hpp"

#include <cstddef>
#include <type_traits>

/**
 * \brief   Test ArrayList constructors.
 **/
TEST(FixedArrayTest, test_constructors)
{
    using FixedArray = areg::FixedArray<int>;
    constexpr uint32_t elemCount{ 10u };

    FixedArray empty, notEmpty(elemCount);

    EXPECT_TRUE(empty.is_empty());
    EXPECT_FALSE(notEmpty.is_empty());
    EXPECT_EQ(notEmpty.size(), elemCount);

    for (uint32_t i = 0; i < notEmpty.size(); ++i)
    {
        notEmpty[i] = static_cast<int>(i);
    }

    empty = notEmpty;
    EXPECT_EQ(empty, notEmpty);

    FixedArray toCopy(notEmpty);
    EXPECT_FALSE(notEmpty.is_empty());
    EXPECT_FALSE(toCopy.is_empty());
    EXPECT_EQ(empty, toCopy);

    FixedArray toMove(std::move(notEmpty));
    EXPECT_TRUE(notEmpty.is_empty());
    EXPECT_FALSE(toMove.is_empty());
    EXPECT_EQ(toCopy, toMove);

    FixedArray src(15);
    for (uint32_t i = 0; i < src.size(); ++i)
    {
        src[i] = 10 + static_cast<int>(i);
    }

    toCopy = src;
    EXPECT_FALSE(src.is_empty());
    EXPECT_FALSE(toCopy.is_empty());
    EXPECT_EQ(src, toCopy);
    EXPECT_NE(empty, toCopy);

    toMove = std::move(src);
    EXPECT_FALSE(src.is_empty());
    EXPECT_FALSE(toMove.is_empty());
    EXPECT_EQ(toCopy, toMove);

    toMove.clear();
    EXPECT_TRUE(toMove.is_empty());
    EXPECT_EQ(toMove.size(), 0u);
}

/**
 * \brief   Testing copy and move methods of Fixed Array
 **/
TEST(FixedArrayTest, test_copy_move)
{
    using FixedArray = areg::FixedArray<int>;
    constexpr uint32_t elemCount{ 10u };

    FixedArray empty, notEmpty(elemCount);

    for (uint32_t i = 0; i < notEmpty.size(); ++i)
    {
        notEmpty[i] = static_cast<int>(i);
    }

    empty.copy(notEmpty);
    EXPECT_EQ(empty, notEmpty);

    FixedArray toCopy;
    toCopy.copy(notEmpty);
    EXPECT_FALSE(notEmpty.is_empty());
    EXPECT_FALSE(toCopy.is_empty());
    EXPECT_EQ(empty, toCopy);

    FixedArray toMove;
    toMove.move(std::move(notEmpty));
    EXPECT_TRUE(notEmpty.is_empty());
    EXPECT_FALSE(toMove.is_empty());
    EXPECT_EQ(toCopy, toMove);

    FixedArray src(15);
    for (uint32_t i = 0; i < src.size(); ++i)
    {
        src[i] = 10 + static_cast<int>(i);
    }

    toCopy.copy(src);
    EXPECT_FALSE(src.is_empty());
    EXPECT_FALSE(toCopy.is_empty());
    EXPECT_EQ(src, toCopy);
    EXPECT_NE(empty, toCopy);

    toMove.move(std::move(src));
    EXPECT_FALSE(src.is_empty());
    EXPECT_FALSE(toMove.is_empty());
    EXPECT_EQ(toCopy, toMove);

    toMove.clear();
    EXPECT_TRUE(toMove.is_empty());
    EXPECT_EQ(toMove.size(), 0u);
}

/**
 * \brief   Test values of the fixed array
 **/
TEST(FixedArrayTest, test_values)
{
    using FixedArray = areg::FixedArray<int>;
    constexpr uint32_t elemCount{ 10u };
    constexpr int32_t arr[]{ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    FixedArray notEmpty(elemCount);
    const int* dst = notEmpty.values();
    const int* src = arr;
    for (uint32_t i = elemCount; i > 0; -- i)
    {
        EXPECT_TRUE(notEmpty.is_valid_index(elemCount - i));
        notEmpty.set_value_at(elemCount - i, *src ++);
    }

    EXPECT_TRUE(::memcmp(dst, arr, elemCount * sizeof(int)) == 0);
}

/**
 * \brief   Testing searching methods like 'find' and 'contains' of fixed array
 **/
TEST(FixedArrayTest, test_search_elem)
{
    using FixedArray = areg::FixedArray<int>;
    constexpr uint32_t elemCount{ 10u };

    FixedArray notEmpty(elemCount);
    for (uint32_t i = 0; i < notEmpty.size(); ++i)
    {
        notEmpty[i] = static_cast<int>(i);
    }

    for (uint32_t i = 0; i < notEmpty.size(); ++i)
    {
        EXPECT_TRUE(notEmpty.contains(static_cast<int>(i), 0u));
        EXPECT_TRUE(notEmpty.contains(static_cast<int>(i), i));
        EXPECT_FALSE(notEmpty.contains(static_cast<int>(i), i + 1u));
        EXPECT_FALSE(notEmpty.contains(static_cast<int>(i + elemCount), 0u));

        EXPECT_EQ(notEmpty.find(static_cast<int>(i), 0u), static_cast<int>(i));
        EXPECT_EQ(notEmpty.find(static_cast<int>(i), i), static_cast<int>(i));
        EXPECT_EQ(notEmpty.find(static_cast<int>(i), i + elemCount), areg::INVALID_INDEX);
        EXPECT_EQ(notEmpty.find(static_cast<int>(i + elemCount), 0u), areg::INVALID_INDEX);
    }
}

/**
 * \brief   Testing the position of the element
 **/
TEST(FixedArrayTest, test_elem_position)
{
    using FixedArray = areg::FixedArray<int>;
    constexpr uint32_t elemCount{ 10u };

    FixedArray notEmpty(elemCount);
    for (uint32_t i = 0; i < notEmpty.size(); ++i)
    {
        notEmpty[i] = static_cast<int>(i);
        EXPECT_EQ(notEmpty.value_at(i), static_cast<int>(i));
        EXPECT_EQ(notEmpty.value_at(i), notEmpty.value_at(i));
    }

    EXPECT_EQ(notEmpty.first_entry(), 0);
    EXPECT_EQ(notEmpty.last_entry(), static_cast<int>(elemCount - 1u));
}

/**
 * \brief   Tests ascending sorting of fixed array.
 **/
TEST(FixedArrayTest, test_ascending)
{
    using FixedArray = areg::FixedArray<int>;

    constexpr int _arr1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr int _res1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr uint32_t _len{ std::size(_arr1) };

    FixedArray arr1(_arr1, _len), res1(_res1, _len);
    arr1.sort([](const int elem1, const int elem2) { return (elem1 < elem2); });

    EXPECT_NE(arr1, FixedArray(_arr1, _len));
    EXPECT_EQ(arr1.size(), _len);
    EXPECT_EQ(arr1, res1);
}

/**
 * \brief   Tests descending sorting of fixed array.
 **/
TEST(FixedArrayTest, test_descending)
{
    using FixedArray = areg::FixedArray<int>;

    constexpr int _arr1[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr int _res1[]{ 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    constexpr uint32_t _len{ std::size(_arr1) };

    FixedArray arr1(_arr1, _len), res1(_res1, _len);
    arr1.sort([](const int elem1, const int elem2) { return (elem1 > elem2); });

    EXPECT_NE(arr1, FixedArray(_arr1, _len));
    EXPECT_EQ(arr1.size(), _len);
    EXPECT_EQ(arr1, res1);
}

/**
 * \brief   The index operator takes any integer type and returns a writable element.
 **/
TEST(FixedArrayTest, test_index_any_integer_type)
{
    using Array = areg::FixedArray<int>;
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

// An empty fixed array stays safe to copy, resize, and inspect through its pointer.
TEST(FixedArrayTest, boundary_empty_copy_resize_and_pointer_conversion)
{
    const areg::FixedArray<int32_t> empty;
    areg::FixedArray<int32_t> copy(empty);
    EXPECT_TRUE(copy.is_empty());
    EXPECT_EQ(static_cast<const int32_t*>(copy), nullptr);
    const int32_t source[]{ 4, 5 };
    areg::FixedArray<int32_t> values(source, 2u);
    EXPECT_NE(values, empty);
    EXPECT_FALSE(values == empty);
    const int32_t* pointer{ values };
    EXPECT_EQ(pointer, values.values());
    values.copy(values);
    values.move(std::move(values));
    EXPECT_EQ(values[0], 4);
    EXPECT_EQ(values[1], 5);
    values.copy(empty);
    EXPECT_TRUE(values.is_empty());
    values.resize(0u);
    EXPECT_EQ(static_cast<const int32_t*>(values), nullptr);
    EXPECT_EQ(values, copy);
    EXPECT_FALSE(values != copy);
}

// Copying an equally sized array replaces every destination value.
TEST(FixedArrayTest, boundary_equal_size_copy_replaces_values)
{
    const int32_t first[]{ 1, 2 };
    const int32_t second[]{ 3, 4 };
    areg::FixedArray<int32_t> values(first, 2u);
    const areg::FixedArray<int32_t> source(second, 2u);
    EXPECT_NE(values, source);
    values.copy(source);
    EXPECT_EQ(values, source);
    values.resize(1u);
    ASSERT_EQ(values.size(), 1u);
    EXPECT_EQ(values[0], 3);
}

/**
 * \brief   Test FixedArray const access, rvalue setter and export of the elements.
 **/
TEST(FixedArrayTest, test_const_access_rvalue_set_and_elements)
{
    using FixedArray = areg::FixedArray<int>;
    constexpr int values[]{ 10, 20, 30, 40 };

    FixedArray array(values, 4u);
    ASSERT_EQ(array.size(), 4u);

    array.set_value_at(1u, 21);
    array.set_value_at(2u, int{ 31 });
    const FixedArray& constArray{ array };
    EXPECT_EQ(constArray.value_at(0u), 10);
    EXPECT_EQ(constArray.value_at(1u), 21);
    EXPECT_EQ(constArray.value_at(2u), 31);
    EXPECT_EQ(constArray.first_entry(), 10);
    EXPECT_EQ(constArray.last_entry(), 40);

    int all[6]{ -1, -1, -1, -1, -1, -1 };
    EXPECT_EQ(array.elements(all, 6u), 4u);
    EXPECT_EQ(all[0], 10);
    EXPECT_EQ(all[3], 40);
    EXPECT_EQ(all[4], -1);

    int some[2]{ -1, -1 };
    EXPECT_EQ(array.elements(some, 2u), 2u);
    EXPECT_EQ(some[1], 21);
}

/**
 * \brief   Test FixedArray construction from an empty list and sort of an empty array.
 **/
TEST(FixedArrayTest, test_empty_list_and_empty_sort)
{
    using FixedArray = areg::FixedArray<int>;
    constexpr int values[]{ 1 };

    FixedArray empty(values, 0u);
    EXPECT_TRUE(empty.is_empty());
    EXPECT_EQ(empty.size(), 0u);

    empty.sort([](int a, int b) { return a < b; });
    EXPECT_TRUE(empty.is_empty());
}
