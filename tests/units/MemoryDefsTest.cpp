/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * \copyright (c) 2017-2026 Aregtech.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/MemoryDefs.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <new>

// Memory copies respect destination capacity and leave it unchanged for invalid operands.
TEST(MemoryDefsTest, boundary_copy_is_bounded_and_invalid_operands_preserve_destination)
{
    const std::array<uint8_t, 4> source{ 1, 2, 3, 4 };
    std::array<uint8_t, 4> output{ 9, 9, 9, 9 };
    EXPECT_EQ(areg::mem_copy(output.data(), 2u, source.data(), 4u), 2u);
    EXPECT_EQ(output, (std::array<uint8_t, 4>{ 1, 2, 9, 9 }));
    EXPECT_EQ(areg::mem_copy(output.data(), 4u, nullptr, 4u), 0u);
    EXPECT_EQ(areg::mem_copy(nullptr, 4u, source.data(), 4u), 0u);
    EXPECT_EQ(areg::mem_copy(output.data(), 0u, source.data(), 4u), 0u);
    EXPECT_EQ(areg::mem_copy(output.data(), 4u, source.data(), 0u), 0u);
    EXPECT_EQ(areg::mem_copy(output.data(), 4u, output.data(), 4u), 4u);
    EXPECT_EQ(output, (std::array<uint8_t, 4>{ 1, 2, 9, 9 }));
}

// A self-copy must report no more bytes than fit in the destination.
TEST(MemoryDefsTest, boundary_self_copy_reports_only_destination_capacity)
{
    std::array<uint8_t, 4> values{ 1, 2, 3, 4 };
    EXPECT_EQ(areg::mem_copy(values.data(), 2u, values.data(), 4u), 2u);
    EXPECT_EQ(areg::mem_copy(values.data(), 0u, values.data(), 4u), 0u);
    EXPECT_EQ(areg::mem_copy(values.data(), 4u, values.data(), 3u), 3u);
    EXPECT_EQ(areg::mem_copy(nullptr, 4u, nullptr, 4u), 0u);
    EXPECT_EQ(values, (std::array<uint8_t, 4>{ 1, 2, 3, 4 }));
}

// Memory moves handle overlap in either direction and leave zero-count moves alone.
TEST(MemoryDefsTest, boundary_move_handles_both_overlap_directions_and_zero_count)
{
    std::array<int32_t, 5> values{ 1, 2, 3, 4, 5 };
    areg::move_elems(values.data() + 1, values.data(), 4u);
    EXPECT_EQ(values, (std::array<int32_t, 5>{ 1, 1, 2, 3, 4 }));
    areg::move_elems(values.data(), values.data() + 1, 4u);
    EXPECT_EQ(values, (std::array<int32_t, 5>{ 1, 2, 3, 4, 4 }));
    areg::move_elems(values.data(), values.data(), 5u);
    areg::move_elems(values.data() + 1, values.data(), 0u);
    areg::mem_move(nullptr, values.data(), sizeof(int32_t));
    areg::mem_move(values.data(), nullptr, sizeof(int32_t));
    areg::mem_move(values.data(), values.data() + 1, 0u);
    EXPECT_EQ(values, (std::array<int32_t, 5>{ 1, 2, 3, 4, 4 }));
}

// Memory comparisons give a consistent order for null and zero-length inputs.
TEST(MemoryDefsTest, boundary_null_and_zero_length_comparisons_have_total_order)
{
    const int32_t value{ 1 };
    EXPECT_EQ(areg::mem_compare(nullptr, nullptr, 4u), areg::Ordering::Equal);
    EXPECT_EQ(areg::mem_compare(&value, nullptr, 4u), areg::Ordering::Bigger);
    EXPECT_EQ(areg::mem_compare(nullptr, &value, 4u), areg::Ordering::Smaller);
    EXPECT_EQ(areg::mem_compare(&value, nullptr, 0u), areg::Ordering::Equal);
    EXPECT_TRUE(areg::equal_elements<int32_t>(nullptr, nullptr, 1u));
    EXPECT_TRUE(areg::equal_elements<int32_t>(&value, nullptr, 0u));
    EXPECT_FALSE(areg::equal_elements<int32_t>(&value, nullptr, 1u));
    EXPECT_FALSE(areg::equal_elements<int32_t>(nullptr, &value, 1u));
}

// Zero-count and null element operations do not touch storage.
TEST(MemoryDefsTest, boundary_zero_count_and_null_element_operations_are_noops)
{
    std::array<int32_t, 2> values{ 1, 2 };
    const int32_t source[]{ 3, 4 };
    areg::copy_elems<int32_t, int32_t>(values.data(), nullptr, 2u);
    areg::copy_elems<int32_t>(nullptr, source, 2u);
    areg::copy_elems(values.data(), source, 0u);
    areg::copy_elems(values.data(), values.data(), 2u);
    areg::set_memory<int32_t>(nullptr, 9, 2u);
    areg::set_memory(values.data(), 9, 0u);
    areg::mem_set(nullptr, 4u, 9u);
    areg::mem_set(values.data(), 0u, 9u);
    EXPECT_EQ(values, (std::array<int32_t, 2>{ 1, 2 }));
    EXPECT_EQ(areg::construct_elems<int32_t>(nullptr, 2u), nullptr);
    EXPECT_EQ((areg::construct_with_arg<int32_t, int32_t>(nullptr, 2u, 9)), nullptr);
    areg::destroy_elems<int32_t>(nullptr, 2u);
}

// Element construction and destruction visit exactly the requested number of objects.
TEST(MemoryDefsTest, boundary_construct_and_destroy_exactly_the_requested_objects)
{
    struct Counted
    {
        explicit Counted(int32_t* live) : mLive(live)
        {
            ++*mLive;
        }
        ~Counted()
        {
            --*mLive;
        }
        int32_t* mLive;
    };
    alignas(Counted) uint8_t storage[sizeof(Counted) * 3u]{};
    int32_t live{ 0 };
    Counted* values = areg::construct_with_arg<Counted>(storage, 3u, &live);
    EXPECT_EQ(live, 3);
    EXPECT_EQ(values, reinterpret_cast<Counted*>(storage));
    areg::destroy_elems(values, 0u);
    EXPECT_EQ(live, 3);
    areg::destroy_elems(values, 3u);
    EXPECT_EQ(live, 0);
}

// Buffer-data access skips the header correctly and accepts a null buffer.
TEST(MemoryDefsTest, boundary_buffer_data_uses_header_offset_and_accepts_null)
{
    areg::RawBuffer buffer;
    buffer.bufHeader.biOffset = static_cast<uint32_t>(offsetof(areg::RawBuffer, bufData));
    EXPECT_EQ(areg::buffer_data_write(&buffer), buffer.bufData);
    EXPECT_EQ(areg::buffer_data_read(&buffer), buffer.bufData);
    EXPECT_EQ(areg::buffer_data_write(nullptr), nullptr);
    EXPECT_EQ(areg::buffer_data_read(nullptr), nullptr);
}

namespace
{
    //! The one place the tests call the inlined copy, so that a size is a run time value.
    void _exact_copy(void * target, const void * source, uint32_t size)
    {
        areg::mem_copy_exact(target, source, size);
    }

    //! The same call with the size of the destination.
    void _exact_copy_known(void * target, uint32_t space, const void * source, uint32_t size)
    {
        areg::mem_copy_exact(target, space, source, size);
    }
}

// A destination of a known size gets every count whole, and the bytes after it stay.
TEST(MemoryDefsTest, exact_copy_into_a_known_size)
{
    uint8_t source[32]{ };
    for (uint32_t i = 0u; i < sizeof(source); ++i)
    {
        source[i] = static_cast<uint8_t>(i + 1u);
    }

    for (uint32_t size = 0u; size <= 24u; ++size)
    {
        for (const uint32_t space : { size, static_cast<uint32_t>(sizeof(source)) })
        {
            uint8_t target[32]{ };
            _exact_copy_known(target, space, source, size);
            EXPECT_EQ(std::memcmp(target, source, size), 0) << "size " << size << ", space " << space;
            for (uint32_t i = size; i < sizeof(target); ++i)
            {
                EXPECT_EQ(target[i], 0u) << "size " << size << ", space " << space;
            }
        }
    }
}

// Every size is copied whole, and the bytes after it stay.
TEST(MemoryDefsTest, exact_copy_of_every_small_size)
{
    uint8_t source[32]{ };
    for (uint32_t i = 0u; i < sizeof(source); ++i)
    {
        source[i] = static_cast<uint8_t>(i + 1u);
    }

    for (uint32_t size = 0u; size <= 24u; ++size)
    {
        uint8_t target[32]{ };
        _exact_copy(target, source, size);
        for (uint32_t i = 0u; i < sizeof(target); ++i)
        {
            EXPECT_EQ(target[i], (i < size) ? source[i] : 0u) << "size " << size << " index " << i;
        }
    }
}

// A copy reads and writes at any alignment.
TEST(MemoryDefsTest, exact_copy_at_any_alignment)
{
    uint8_t source[16]{ 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u };
    for (uint32_t size : { 2u, 4u, 8u })
    {
        for (uint32_t offset = 0u; offset < 8u; ++offset)
        {
            uint8_t target[16]{ };
            _exact_copy(target + offset, source + 8u - offset, size);
            for (uint32_t i = 0u; i < size; ++i)
            {
                EXPECT_EQ(target[offset + i], source[8u - offset + i]);
            }
        }
    }
}

// Overlapping ranges are copied as if through a temporary, for a primitive size and a larger one.
TEST(MemoryDefsTest, exact_copy_of_overlapping_ranges)
{
    uint8_t small[9]{ 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u };
    _exact_copy(small + 1u, small, 8u);
    EXPECT_EQ(small[1], 1u);
    EXPECT_EQ(small[8], 8u);

    uint8_t large[20]{ 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u };
    _exact_copy(large + 2u, large, 16u);
    EXPECT_EQ(large[2], 1u);
    EXPECT_EQ(large[17], 16u);
}

namespace
{
    //! Returns a block with an empty buffer header for an owner to adopt.
    areg::RawBuffer * _new_block()
    {
        uint8_t * const block{ new uint8_t[sizeof(areg::RawBuffer)] };
        areg::RawBuffer * const raw{ reinterpret_cast<areg::RawBuffer *>(block) };
        raw->bufHeader = areg::BufferHeader(0u, static_cast<uint32_t>(sizeof(areg::BufferHeader)), areg::BufferType::Internal);
        return raw;
    }
}

// An owner adopting nothing is empty and counts no owner.
TEST(MemoryDefsTest, owner_of_nothing_is_empty)
{
    areg::RawBufferPtr owner(nullptr);
    EXPECT_EQ(owner.get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(owner));
    EXPECT_EQ(owner.use_count(), 0u);
}

// Copies count as owners, and the last one frees the block.
TEST(MemoryDefsTest, owners_count_up_and_down)
{
    areg::RawBufferPtr first(_new_block());
    EXPECT_EQ(first.use_count(), 1u);
    {
        areg::RawBufferPtr second(first);
        EXPECT_EQ(first.use_count(), 2u);
        areg::RawBufferPtr third;
        third = second;
        EXPECT_EQ(first.use_count(), 3u);
    }

    EXPECT_EQ(first.use_count(), 1u);
}

// A copy of an empty owner is empty.
TEST(MemoryDefsTest, copy_of_an_empty_owner_is_empty)
{
    const areg::RawBufferPtr empty;
    const areg::RawBufferPtr copy(empty);
    EXPECT_EQ(copy.get(), nullptr);
}

// Assigning nothing releases the block, assigning itself keeps it.
TEST(MemoryDefsTest, owner_assignment_of_nothing_and_of_itself)
{
    areg::RawBufferPtr owner(_new_block());
    const areg::RawBufferPtr & same{ owner };
    owner = same;
    EXPECT_EQ(owner.use_count(), 1u);

    const areg::RawBufferPtr empty;
    owner = empty;
    EXPECT_EQ(owner.get(), nullptr);
}

// A move leaves the source empty and keeps the count, a move into itself keeps the block.
TEST(MemoryDefsTest, owner_move_and_move_into_itself)
{
    areg::RawBufferPtr source(_new_block());
    areg::RawBufferPtr target;
    target = std::move(source);
    EXPECT_EQ(source.get(), nullptr);
    EXPECT_EQ(target.use_count(), 1u);

    areg::RawBufferPtr & alias{ target };
    target = std::move(alias);
    EXPECT_EQ(target.use_count(), 1u);
}

/**
 * \brief   zero_element clears every byte of one value; zero_elements clears the given count
 *          and leaves the rest, and a count of zero changes nothing.
 **/
TEST(MemoryDefsTest, zero_elements_clears_only_the_given_count)
{
    struct Pair
    {
        uint32_t first;
        uint32_t second;
    };

    Pair one{ 7u, 9u };
    areg::zero_element<Pair>(one);
    EXPECT_EQ(one.first, 0u);
    EXPECT_EQ(one.second, 0u);

    uint32_t values[4]{ 1u, 2u, 3u, 4u };
    areg::zero_elements<uint32_t>(values, 0u);
    EXPECT_EQ(values[0], 1u);
    areg::zero_elements<uint32_t>(values, 3u);
    EXPECT_EQ(values[0], 0u);
    EXPECT_EQ(values[1], 0u);
    EXPECT_EQ(values[2], 0u);
    EXPECT_EQ(values[3], 4u);
}
