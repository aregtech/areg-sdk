/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 *
 * \file        units/BaseContainersCoverageTest.cpp
 * \brief       Areg Platform, unit tests of the base containers and value types: boundary and identity cases.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/HashMap.hpp"
#include "areg/base/KeyValuePair.hpp"
#include "areg/base/LinkedList.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/OrderedMap.hpp"
#include "areg/base/ResourceListMap.hpp"
#include "areg/base/ResourceMap.hpp"
#include "areg/base/RingStack.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/SortedLinkedList.hpp"
#include "areg/base/Stack.hpp"
#include "areg/base/Version.hpp"

#include <array>

//////////////////////////////////////////////////////////////////////////
// Maps
//////////////////////////////////////////////////////////////////////////

// A new key is added with or without the update flag; an existing key is replaced only with the flag.
TEST(BaseContainersCoverageTest, hash_map_add_if_unique_adds_new_keys_and_updates_on_request)
{
    areg::HashMap<int, int> map;
    const int key1{ 1 }, key2{ 2 }, value1{ 10 }, value2{ 20 }, other{ 99 };

    auto added = map.add_if_unique(key1, value1, true);
    EXPECT_TRUE(added.second);
    EXPECT_EQ(added.first->second, 10);
    added = map.add_if_unique(key2, value2, false);
    EXPECT_TRUE(added.second);
    EXPECT_EQ(map.size(), 2u);

    auto kept = map.add_if_unique(key1, other, false);
    EXPECT_FALSE(kept.second);
    EXPECT_EQ(kept.first->second, 10);
    auto updated = map.add_if_unique(key1, other, true);
    EXPECT_FALSE(updated.second);
    EXPECT_EQ(updated.first->second, 99);
    EXPECT_EQ(map.size(), 2u);

    auto moved = map.add_if_unique(3, 30, true);
    EXPECT_TRUE(moved.second);
    EXPECT_EQ(moved.first->second, 30);
    moved = map.add_if_unique(3, 31, true);
    EXPECT_FALSE(moved.second);
    EXPECT_EQ(moved.first->second, 31);
    EXPECT_EQ(map.size(), 3u);
}

// The ordered map adds a new key with the update flag and replaces an existing value only on request.
TEST(BaseContainersCoverageTest, ordered_map_add_if_unique_adds_new_keys_and_updates_on_request)
{
    areg::OrderedMap<int, int> map;
    const int key1{ 1 }, key2{ 2 }, value1{ 10 }, value2{ 20 }, other{ 99 };

    auto added = map.add_if_unique(key1, value1, true);
    EXPECT_TRUE(added.second);
    EXPECT_EQ(added.first->second, 10);
    added = map.add_if_unique(key2, value2, false);
    EXPECT_TRUE(added.second);

    auto kept = map.add_if_unique(key1, other, false);
    EXPECT_FALSE(kept.second);
    EXPECT_EQ(kept.first->second, 10);
    auto updated = map.add_if_unique(key1, other, true);
    EXPECT_FALSE(updated.second);
    EXPECT_EQ(updated.first->second, 99);

    auto moved = map.add_if_unique(3, 30, true);
    EXPECT_TRUE(moved.second);
    EXPECT_EQ(moved.first->second, 30);
    moved = map.add_if_unique(3, 31, true);
    EXPECT_FALSE(moved.second);
    EXPECT_EQ(moved.first->second, 31);
    moved = map.add_if_unique(3, 32, false);
    EXPECT_FALSE(moved.second);
    EXPECT_EQ(moved.first->second, 31);
    EXPECT_EQ(map.size(), 3u);
}

// A key-value pair equals itself, and a pair never differs from itself.
TEST(BaseContainersCoverageTest, key_value_pair_is_equal_to_itself)
{
    const areg::KeyValuePair<int, int> pair(1, 2);
    const areg::KeyValuePair<int, int>& same{ pair };
    const areg::KeyValuePair<int, int> sameKey(1, 3);
    const areg::KeyValuePair<int, int> otherKey(2, 2);
    EXPECT_TRUE(pair == same);
    EXPECT_FALSE(pair != same);
    EXPECT_TRUE(pair == sameKey);
    EXPECT_FALSE(pair != sameKey);
    EXPECT_FALSE(pair == otherKey);
    EXPECT_TRUE(pair != otherKey);
}

//////////////////////////////////////////////////////////////////////////
// Value types
//////////////////////////////////////////////////////////////////////////

// A version equals itself, and differs from a version with one different part.
TEST(BaseContainersCoverageTest, version_equality_covers_identity_and_every_part)
{
    const areg::Version version(1u, 2u, 3u);
    const areg::Version& same{ version };
    EXPECT_TRUE(version == same);
    EXPECT_FALSE(version != same);
    EXPECT_TRUE(version == areg::Version(1u, 2u, 3u));
    EXPECT_FALSE(version == areg::Version(2u, 2u, 3u));
    EXPECT_FALSE(version == areg::Version(1u, 3u, 3u));
    EXPECT_FALSE(version == areg::Version(1u, 2u, 4u));
}

// Two large integers are equal only when both halves are equal.
TEST(BaseContainersCoverageTest, large_integer_equality_compares_both_halves)
{
    const areg::LargeInteger number(1u, 2u);
    EXPECT_TRUE(number == areg::LargeInteger(1u, 2u));
    EXPECT_FALSE(number != areg::LargeInteger(1u, 2u));
    EXPECT_FALSE(number == areg::LargeInteger(1u, 3u));
    EXPECT_TRUE(number != areg::LargeInteger(1u, 3u));
    EXPECT_FALSE(number == areg::LargeInteger(2u, 2u));
    EXPECT_TRUE(number != areg::LargeInteger(2u, 2u));
}

// A primitive value equals itself and a primitive of the same 64-bit content.
TEST(BaseContainersCoverageTest, primitive_value_is_equal_to_itself_and_to_same_content)
{
    areg::Primitive value{};
    value.valInt64.mElement = 0x1122334455667788;
    areg::Primitive copy{};
    copy.valInt64.mElement = 0x1122334455667788;
    areg::Primitive other{};
    other.valInt64.mElement = 5;
    const areg::Primitive& same{ value };

    EXPECT_TRUE(value == same);
    EXPECT_TRUE(value == copy);
    EXPECT_FALSE(value == other);
}

// set_memory stores the value in exactly the requested number of elements.
TEST(BaseContainersCoverageTest, set_memory_fills_the_requested_elements)
{
    std::array<int32_t, 4> values{ 1, 2, 3, 4 };
    areg::set_memory(values.data(), 7, 3u);
    EXPECT_EQ(values, (std::array<int32_t, 4>{ 7, 7, 7, 4 }));
}

//////////////////////////////////////////////////////////////////////////
// Lists and stacks
//////////////////////////////////////////////////////////////////////////

// An element given as a temporary is added at the tail when it is new, and replaced only on request.
TEST(BaseContainersCoverageTest, linked_list_push_last_unique_takes_new_temporary_element)
{
    areg::LinkedList<int> list;
    EXPECT_TRUE(list.push_last_unique(5));
    EXPECT_TRUE(list.push_last_unique(6));
    EXPECT_FALSE(list.push_last_unique(5));
    ASSERT_EQ(list.size(), 2u);
    EXPECT_EQ(list.value_at(list.last_position()), 6);
}

// Removing at the invalid position removes nothing, returns the invalid position and keeps the output value.
TEST(BaseContainersCoverageTest, linked_list_remove_at_invalid_position_keeps_the_list_and_the_output)
{
    areg::LinkedList<int> list;
    list.push_last(1);
    list.push_last(2);
    int out{ 77 };
    const auto next = list.remove_at(list.invalid_position(), out);
    EXPECT_EQ(next, list.invalid_position());
    EXPECT_EQ(out, 77);
    EXPECT_EQ(list.size(), 2u);

    const auto afterFirst = list.remove_at(list.first_position(), out);
    EXPECT_EQ(out, 1);
    EXPECT_EQ(list.size(), 1u);
    EXPECT_EQ(afterFirst, list.first_position());
}

// check_position accepts every entry of the container and refuses the invalid position.
TEST(BaseContainersCoverageTest, lists_and_stacks_check_only_their_own_entries)
{
    areg::LinkedList<int> list;
    list.push_last(1);
    list.push_last(2);
    EXPECT_TRUE(list.check_position(list.first_position()));
    EXPECT_TRUE(list.check_position(list.last_position()));
    EXPECT_FALSE(list.check_position(list.invalid_position()));

    areg::SortedLinkedList<int> sorted;
    sorted.add(2);
    sorted.add(1);
    EXPECT_TRUE(sorted.check_position(sorted.first_position()));
    EXPECT_TRUE(sorted.check_position(sorted.last_position()));
    EXPECT_FALSE(sorted.check_position(sorted.invalid_position()));

    areg::Stack<int> stack;
    stack.push_last(1);
    stack.push_last(2);
    EXPECT_TRUE(stack.check_position(stack.first_position()));
    EXPECT_TRUE(stack.check_position(stack.next_position(stack.first_position())));
    EXPECT_FALSE(stack.check_position(stack.invalid_position()));
}

// An empty stack has no last position; elements() never copies more than the stack holds.
TEST(BaseContainersCoverageTest, stack_last_position_and_element_copy_respect_the_size)
{
    areg::Stack<int> stack;
    EXPECT_FALSE(stack.is_last_position(stack.first_position()));
    EXPECT_FALSE(stack.is_last_position(stack.invalid_position()));

    stack.push_last(1);
    stack.push_last(2);
    EXPECT_TRUE(stack.is_last_position(stack.next_position(stack.first_position())));
    EXPECT_FALSE(stack.is_last_position(stack.first_position()));

    std::array<int, 5> list{ 9, 9, 9, 9, 9 };
    EXPECT_EQ(stack.elements(list.data(), 5u), 2u);
    EXPECT_EQ(list, (std::array<int, 5>{ 1, 2, 9, 9, 9 }));
    EXPECT_EQ(stack.elements(list.data(), 1u), 1u);

    areg::Stack<int> none;
    EXPECT_EQ(none.elements(list.data(), 5u), 0u);
}

// Rings of different size are never equal; a ring that grows keeps no stale element; an empty ring streams as empty.
TEST(BaseContainersCoverageTest, ring_stack_compares_sizes_grows_when_empty_and_streams_empty)
{
    areg::RingStack<int> left(4u, areg::OverlapPolicy::Stop), right(4u, areg::OverlapPolicy::Stop);
    EXPECT_TRUE(left == right);
    left.push(1);
    left.push(2);
    right.push(1);
    EXPECT_FALSE(left == right);
    EXPECT_TRUE(left != right);
    right.push(2);
    EXPECT_TRUE(left == right);

    areg::RingStack<int> empty(3u, areg::OverlapPolicy::Stop);
    EXPECT_EQ(empty.reserve(6u), 6u);
    EXPECT_EQ(empty.capacity(), 6u);
    EXPECT_TRUE(empty.is_empty());

    areg::SharedBuffer stream;
    stream << empty;
    stream.move_to_begin();
    stream >> left;
    EXPECT_TRUE(left.is_empty());
    EXPECT_EQ(left.size(), 0u);
}

//////////////////////////////////////////////////////////////////////////
// Resource maps
//////////////////////////////////////////////////////////////////////////

// The next key of an empty map or of a missing key is not found and the key is left as it was.
TEST(BaseContainersCoverageTest, resource_map_next_key_of_missing_key_finds_nothing)
{
    areg::ResourceMap<int, int*> map;
    int key{ 5 };
    EXPECT_EQ(map.resource_next_key(key), nullptr);
    EXPECT_EQ(key, 5);

    int first{ 1 }, second{ 2 };
    map.register_resource_object(1, &first);
    map.register_resource_object(2, &second);
    key = 99;
    EXPECT_EQ(map.resource_next_key(key), nullptr);
    EXPECT_EQ(key, 99);
}

// A resource list is found only for a registered key, in an empty and in a filled map, through both accessors.
TEST(BaseContainersCoverageTest, resource_list_map_finds_a_list_only_for_a_registered_key)
{
    using ListMap = areg::ResourceListMap<int, int*, areg::LinkedList<int*>>;
    ListMap map;
    const ListMap& view{ map };
    EXPECT_EQ(view.find_resource(1), nullptr);
    EXPECT_EQ(view.resource(1), nullptr);

    int object{ 7 };
    map.register_resource_object(1, &object);
    ASSERT_NE(view.find_resource(1), nullptr);
    ASSERT_NE(view.resource(1), nullptr);
    EXPECT_EQ(view.find_resource(1)->size(), 1u);
    EXPECT_EQ(view.resource(1)->size(), 1u);
    EXPECT_EQ(view.find_resource(2), nullptr);
    EXPECT_EQ(view.resource(2), nullptr);
}
