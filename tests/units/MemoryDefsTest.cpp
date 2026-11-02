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
