/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/BufferBaseTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Unit tests for owning byte buffers and streaming boundaries.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/BufferBase.hpp"
#include "areg/base/SharedBuffer.hpp"

#include <array>
#include <cstring>
#include <limits>
#include <string>
#include <utility>
#include <vector>

// Operations on an unallocated buffer leave it invalid and do not touch the output byte.
TEST(BufferBaseTest, empty_operations_preserve_invalid_state)
{
    areg::BufferBase buffer;
    uint8_t output{ 0xA5u };
    EXPECT_EQ(buffer.position(), areg::Cursor::INVALID_CURSOR_POSITION);
    EXPECT_EQ(buffer.type(), areg::BufferType::Unknown);
    EXPECT_EQ(buffer.buffer(), nullptr);
    EXPECT_EQ(std::as_const(buffer).byte_buffer(), nullptr);
    EXPECT_EQ(buffer.size_available(), 0u);
    EXPECT_FALSE(buffer.is_shared());
    EXPECT_FALSE(buffer.is_unique());
    EXPECT_TRUE(buffer.is_begin());
    EXPECT_TRUE(buffer.is_end());
    EXPECT_EQ(buffer.bounded_count(10u), 0u);
    EXPECT_EQ(buffer.read(&output, 1u), 0u);
    EXPECT_EQ(output, 0xA5u);
    EXPECT_EQ(buffer.read_ptr<uint32_t>(), nullptr);
    EXPECT_EQ(buffer.read_array<uint8_t>(0u), nullptr);
    EXPECT_EQ(buffer.set_position(1, areg::Cursor::SeekOrigin::Begin), areg::Cursor::INVALID_CURSOR_POSITION);
    buffer.set_size_used(1u);
    buffer.reset();
    buffer.flush();
    EXPECT_FALSE(buffer.is_valid());
    EXPECT_FALSE(buffer.clone().is_valid());
}

// Constructing from raw bytes expands a small reservation to fit the payload.
TEST(BufferBaseTest, raw_constructor_expands_small_reservation)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u, 5u };
    areg::BufferBase buffer(1u, DATA, sizeof(DATA), areg::BLOCK_SIZE + 1u);
    EXPECT_EQ(buffer.block_size(), 2u * areg::BLOCK_SIZE);
    EXPECT_EQ(buffer.type(), areg::BufferType::Internal);
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
    EXPECT_EQ(buffer.position(), sizeof(DATA));
    ASSERT_GE(buffer.size_available(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(std::as_const(buffer).buffer(), DATA, sizeof(DATA)), 0);
}

// Text constructors round-trip narrow and wide text and accept null input.
TEST(BufferBaseTest, text_constructors_roundtrip_and_accept_null)
{
    areg::BufferBase narrow("buffer");
    areg::BufferBase wide(L"wide buffer");
    areg::BufferBase nullNarrow(static_cast<const char*>(nullptr));
    areg::BufferBase nullWide(static_cast<const wchar_t*>(nullptr));
    std::string text;
    std::wstring wideText;
    narrow.reset();
    wide.reset();
    EXPECT_EQ(narrow.read_text(text), sizeof(uint32_t) + 6u);
    EXPECT_EQ(text, "buffer");
    EXPECT_EQ(wide.read_text(wideText), sizeof(uint32_t) + 22u);
    EXPECT_EQ(wideText, L"wide buffer");
    nullNarrow.reset();
    nullWide.reset();
    EXPECT_EQ(nullNarrow.read_text(text), sizeof(uint32_t));
    EXPECT_TRUE(text.empty());
    EXPECT_EQ(nullWide.read_text(wideText), sizeof(uint32_t));
    EXPECT_TRUE(wideText.empty());
}

// Writing within reserved capacity keeps the allocation; an extra byte grows it safely.
TEST(BufferBaseTest, reservation_keeps_storage_until_growth)
{
    areg::BufferBase buffer(4u * areg::BLOCK_SIZE, areg::BLOCK_SIZE);
    const uint8_t* const allocation{ buffer.buffer() };
    const uint32_t capacity{ buffer.size_available() };
    const std::vector<uint8_t> data(capacity, 0x5Au);
    EXPECT_EQ(buffer.write(data.data(), capacity), capacity);
    EXPECT_EQ(buffer.buffer(), allocation);
    EXPECT_EQ(buffer.size_available(), capacity);
    const uint8_t extra{ 0xE7u };
    EXPECT_EQ(buffer.write(&extra, 1u), 1u);
    EXPECT_GT(buffer.size_available(), capacity);
    EXPECT_EQ(buffer.size_used(), capacity + 1u);
    EXPECT_EQ(std::memcmp(buffer.buffer(), data.data(), capacity), 0);
    EXPECT_EQ(buffer.buffer()[capacity], extra);
}

// Reservation preserves or discards existing contents according to its flag.
TEST(BufferBaseTest, reserve_preserves_or_discards_content)
{
    constexpr uint8_t DATA[]{ 9u, 8u, 7u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    const uint32_t requested{ buffer.size_available() + areg::BLOCK_SIZE };
    EXPECT_GE(buffer.reserve(requested, true), requested - sizeof(DATA));
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
    EXPECT_EQ(buffer.position(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(buffer.buffer(), DATA, sizeof(DATA)), 0);
    const uint32_t capacity{ buffer.size_available() };
    EXPECT_EQ(buffer.reserve(1u, true), capacity - sizeof(DATA));
    EXPECT_EQ(buffer.size_available(), capacity);
    const uint8_t* const allocation{ buffer.buffer() };
    EXPECT_GT(buffer.reserve(1u, false), 0u);
    EXPECT_EQ(buffer.buffer(), allocation);
    EXPECT_EQ(buffer.size_used(), 0u);
    EXPECT_EQ(buffer.position(), 0u);
    EXPECT_EQ(buffer.reserve(0u, true), 0u);
    EXPECT_FALSE(buffer.is_valid());
}

// Ensuring space keeps the cursor and contents unchanged and rejects size overflow.
TEST(BufferBaseTest, ensure_size_preserves_cursor_and_rejects_overflow)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    ASSERT_EQ(buffer.set_position(1, areg::Cursor::SeekOrigin::Begin), 1u);
    const uint8_t* const allocation{ buffer.buffer() };
    EXPECT_TRUE(buffer.ensure_size(1u));
    EXPECT_EQ(buffer.buffer(), allocation);
    EXPECT_TRUE(buffer.ensure_size(buffer.size_available() + 1u));
    EXPECT_EQ(buffer.position(), 1u);
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(buffer.buffer(), DATA, sizeof(DATA)), 0);
    const uint32_t capacity{ buffer.size_available() };
    EXPECT_FALSE(buffer.ensure_size(std::numeric_limits<uint32_t>::max()));
    EXPECT_EQ(buffer.size_available(), capacity);
    EXPECT_EQ(buffer.position(), 1u);
}

// Reducing the used size clamps the cursor without reallocating storage.
TEST(BufferBaseTest, resize_used_clamps_cursor_without_reallocating)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    const uint8_t* const allocation{ buffer.buffer() };
    buffer.set_size_used(2u);
    EXPECT_EQ(buffer.size_used(), 2u);
    EXPECT_EQ(buffer.position(), 2u);
    EXPECT_EQ(buffer.buffer(), allocation);
    buffer.set_size_used(buffer.size_available() + 1u);
    EXPECT_EQ(buffer.size_used(), 2u);
    buffer.reset();
    EXPECT_EQ(buffer.position(), 0u);
    EXPECT_EQ(buffer.size_used(), 2u);
    uint8_t output[2]{};
    EXPECT_EQ(buffer.read(output, sizeof(output)), sizeof(output));
    EXPECT_EQ(output[0], DATA[0]);
    EXPECT_EQ(output[1], DATA[1]);
}

// Seeking from each supported origin clamps the cursor to the buffer bounds.
TEST(BufferBaseTest, seek_clamps_all_origins)
{
    constexpr uint8_t DATA[]{ 0u, 1u, 2u, 3u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    using Origin = areg::Cursor::SeekOrigin;
    EXPECT_EQ(buffer.set_position(-10, Origin::Begin), 0u);
    EXPECT_EQ(buffer.set_position(2, Origin::Begin), 2u);
    EXPECT_EQ(buffer.set_position(-1, Origin::Current), 1u);
    EXPECT_EQ(buffer.set_position(-10, Origin::Current), 0u);
    EXPECT_EQ(buffer.set_position(2, Origin::Current), 2u);
    EXPECT_EQ(buffer.set_position(10, Origin::Current), sizeof(DATA));
    EXPECT_EQ(buffer.set_position(10, Origin::Begin), sizeof(DATA));
    EXPECT_EQ(buffer.set_position(-1, Origin::End), 3u);
    EXPECT_EQ(buffer.set_position(-10, Origin::End), 0u);
    EXPECT_EQ(buffer.set_position(10, Origin::End), sizeof(DATA));
}

// Raw reads and writes respect buffer bounds and ignore no-op arguments safely.
TEST(BufferBaseTest, raw_io_bounds_and_noop_arguments)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    buffer.reset();
    std::array<uint8_t, 5> output{ 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu };
    EXPECT_EQ(buffer.read(nullptr, 1u), 0u);
    EXPECT_EQ(buffer.read(output.data(), 0u), 0u);
    EXPECT_EQ(buffer.write(nullptr, 1u), 0u);
    EXPECT_EQ(buffer.write(DATA, 0u), 0u);
    EXPECT_EQ(buffer.position(), 0u);
    EXPECT_EQ(buffer.read(output.data(), static_cast<uint32_t>(output.size())), sizeof(DATA));
    EXPECT_EQ(output, (std::array<uint8_t, 5>{ 1u, 2u, 3u, 0xFFu, 0xFFu }));
    EXPECT_EQ(buffer.read(output.data(), 1u), 0u);
    EXPECT_EQ(buffer.position(), sizeof(DATA));
}

// Insertion shifts existing bytes or appends the requested bytes exactly.
TEST(BufferBaseTest, insert_shifts_and_appends_exact_bytes)
{
    areg::BufferBase buffer;
    constexpr uint8_t MIDDLE[]{ 3u, 4u };
    constexpr uint8_t FIRST[]{ 1u, 2u };
    constexpr uint8_t LAST[]{ 5u, 6u };
    const uint8_t inserted{ 9u };
    EXPECT_EQ(buffer.insert_at(nullptr, 1u, 0u), 0u);
    EXPECT_EQ(buffer.insert_at(FIRST, 0u, 0u), 0u);
    EXPECT_FALSE(buffer.is_valid());
    EXPECT_EQ(buffer.insert_at(MIDDLE, sizeof(MIDDLE), 10u), sizeof(MIDDLE));
    EXPECT_EQ(buffer.insert_at(FIRST, sizeof(FIRST), 0u), sizeof(FIRST));
    EXPECT_EQ(buffer.insert_at(&inserted, 1u, 2u), 1u);
    EXPECT_EQ(buffer.insert_at(LAST, sizeof(LAST), 100u), sizeof(LAST));
    constexpr uint8_t EXPECTED[]{ 1u, 2u, 9u, 3u, 4u, 5u, 6u };
    ASSERT_EQ(buffer.size_used(), sizeof(EXPECTED));
    EXPECT_EQ(std::memcmp(buffer.buffer(), EXPECTED, sizeof(EXPECTED)), 0);
}

// Copies and moves keep the shared storage alive until its final owner releases it.
TEST(BufferBaseTest, copy_and_move_keep_shared_block_alive)
{
    constexpr uint8_t DATA[]{ 7u, 8u, 9u };
    areg::BufferBase original(DATA, sizeof(DATA));
    areg::BufferBase copied(original);
    EXPECT_EQ(copied.buffer(), original.buffer());
    EXPECT_EQ(copied.position(), 0u);
    EXPECT_EQ(original.share_buffer().use_count(), 2u);
    areg::BufferBase assigned;
    EXPECT_EQ(&(assigned = original), &assigned);
    EXPECT_EQ(original.share_buffer().use_count(), 3u);
    original.invalidate();
    copied.invalidate();
    EXPECT_TRUE(assigned.is_unique());
    areg::BufferBase moved(std::move(assigned));
    EXPECT_FALSE(assigned.is_valid());
    EXPECT_EQ(moved.position(), 0u);
    areg::BufferBase destination;
    EXPECT_EQ(&(destination = std::move(moved)), &destination);
    EXPECT_FALSE(moved.is_valid());
    EXPECT_TRUE(destination.is_unique());
    EXPECT_EQ(std::memcmp(destination.buffer(), DATA, sizeof(DATA)), 0);
}

// Invalid and self-assignments retain the buffer's documented ownership behavior.
TEST(BufferBaseTest, invalid_and_self_assignments_preserve_contract)
{
    constexpr uint8_t DATA[]{ 1u, 2u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    areg::BufferBase& alias{ buffer };
    buffer = alias;
    buffer = std::move(alias);
    EXPECT_EQ(buffer.position(), sizeof(DATA));
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
    EXPECT_TRUE(buffer.is_unique());
    areg::BufferBase empty;
    buffer = empty;
    EXPECT_FALSE(buffer.is_valid());
    areg::BufferBase other(DATA, sizeof(DATA));
    other = std::move(empty);
    EXPECT_FALSE(other.is_valid());
}

// A clone owns independent storage, and equality compares the stored bytes.
TEST(BufferBaseTest, clone_is_independent_and_equality_checks_content)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    areg::BufferBase cloned{ buffer.clone() };
    EXPECT_EQ(buffer, cloned);
    EXPECT_NE(buffer.buffer(), cloned.buffer());
    EXPECT_EQ(cloned.position(), 0u);
    EXPECT_TRUE(buffer.is_equal(buffer));
    areg::BufferBase empty;
    EXPECT_FALSE(buffer.is_equal(empty));
    EXPECT_FALSE(empty.is_equal(buffer));
    EXPECT_TRUE(empty.is_equal(empty));
    const uint8_t different{ 9u };
    EXPECT_EQ(cloned.write(&different, 1u), 1u);
    EXPECT_NE(buffer, cloned);
    cloned.set_size_used(1u);
    EXPECT_NE(buffer, cloned);
    EXPECT_EQ(buffer.buffer()[0], DATA[0]);
}

// Streaming a nested buffer copies its complete payload into independent storage.
TEST(BufferBaseTest, nested_buffer_roundtrip_copies_payload)
{
    constexpr uint8_t DATA[]{ 11u, 12u, 13u };
    areg::SharedBuffer source(DATA, sizeof(DATA));
    areg::BufferBase stream;
    EXPECT_EQ(stream.write(source), sizeof(DATA));
    EXPECT_EQ(stream.size_used(), sizeof(uint32_t) + sizeof(DATA));
    stream.reset();
    areg::SharedBuffer destination;
    EXPECT_EQ(stream.read(destination), sizeof(DATA));
    EXPECT_EQ(destination.position(), 0u);
    EXPECT_TRUE(destination.is_owner());
    EXPECT_NE(destination.buffer(), source.buffer());
    EXPECT_EQ(std::memcmp(destination.buffer(), DATA, sizeof(DATA)), 0);
    EXPECT_TRUE(stream.is_end());
}

// A truncated nested buffer leaves the destination and read position unchanged.
TEST(BufferBaseTest, truncated_nested_buffer_keeps_destination)
{
    areg::BufferBase stream;
    EXPECT_TRUE(stream.write32_bits(12u));
    const uint8_t byte{ 0xAAu };
    EXPECT_EQ(stream.write(&byte, 1u), 1u);
    stream.reset();
    areg::SharedBuffer destination(&byte, 1u);
    const uint8_t* const allocation{ destination.buffer() };
    EXPECT_EQ(stream.read(destination), 0u);
    EXPECT_EQ(stream.position(), 0u);
    EXPECT_EQ(destination.buffer(), allocation);
    EXPECT_EQ(destination.size_used(), 1u);
    EXPECT_EQ(destination.buffer()[0], byte);
}

// Empty nested data and incomplete length fields report zero bytes read.
TEST(BufferBaseTest, empty_nested_buffer_and_short_prefix_report_zero)
{
    areg::BufferBase stream;
    areg::SharedBuffer invalid;
    EXPECT_EQ(stream.write(invalid), 0u);
    EXPECT_EQ(stream.read(invalid), 0u);
    EXPECT_TRUE(stream.write32_bits(0u));
    stream.reset();
    EXPECT_EQ(stream.read(invalid), 0u);
    EXPECT_EQ(stream.position(), sizeof(uint32_t));
    EXPECT_FALSE(invalid.is_valid());
    stream.set_size_used(2u);
    stream.reset();
    EXPECT_EQ(stream.read(invalid), 0u);
    EXPECT_EQ(stream.position(), 0u);
}

// Primitive stream helpers round-trip values, and flushing leaves the bytes unchanged.
TEST(BufferBaseTest, primitive_helpers_roundtrip_without_flush_changes)
{
    areg::BufferBase buffer;
    EXPECT_TRUE(buffer.write8_bits(0xABu));
    EXPECT_TRUE(buffer.write16_bits(0xCDEFu));
    EXPECT_TRUE(buffer.write32_bits(0x12345678u));
    EXPECT_TRUE(buffer.write64_bits(0x123456789ABCDEF0ull));
    EXPECT_TRUE(buffer.write32_real(1.25F));
    EXPECT_TRUE(buffer.write64_real(-7.5));
    const uint32_t used{ buffer.size_used() };
    buffer.flush();
    EXPECT_EQ(buffer.size_used(), used);
    buffer.reset();
    EXPECT_EQ(buffer.bounded_count(used + 10u), used);
    EXPECT_EQ(buffer.read8_bits(), 0xABu);
    EXPECT_EQ(buffer.read16_bits(), 0xCDEFu);
    EXPECT_EQ(buffer.read32_bits(), 0x12345678u);
    EXPECT_EQ(buffer.read64_bits(), 0x123456789ABCDEF0ull);
    EXPECT_FLOAT_EQ(buffer.read32_real(), 1.25F);
    EXPECT_DOUBLE_EQ(buffer.read64_real(), -7.5);
    EXPECT_EQ(buffer.bounded_count(1u), 0u);
}

// Direct array reads share the buffer's storage and stop at its bounds.
TEST(BufferBaseTest, direct_array_reads_share_storage_and_bound_cursor)
{
    constexpr uint32_t DATA[]{ 10u, 20u, 30u };
    areg::BufferBase buffer;
    EXPECT_EQ(buffer.write_array<uint32_t>(nullptr, 3u), 0u);
    EXPECT_EQ(buffer.write_array(DATA, 0u), 0u);
    EXPECT_EQ(buffer.write_array(DATA, 3u), sizeof(DATA));
    buffer.reset();
    const uint32_t* const values{ buffer.read_array<uint32_t>(3u) };
    ASSERT_NE(values, nullptr);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(values), buffer.buffer());
    EXPECT_EQ(values[0], DATA[0]);
    EXPECT_EQ(values[2], DATA[2]);
    EXPECT_EQ(buffer.position(), sizeof(DATA));
    EXPECT_EQ(buffer.read_ptr<uint32_t>(), nullptr);
    EXPECT_EQ(buffer.position(), sizeof(DATA));
}

// Streaming a buffer to or from itself leaves its cursor unchanged.
TEST(BufferBaseTest, base_stream_self_operations_do_not_change_cursor)
{
    areg::SharedBuffer storage;
    storage << uint32_t{ 42u };
    areg::BufferBase& base{ storage };
    const uint32_t position{ base.position() };
    EXPECT_EQ(base.areg::BufferBase::write(storage), sizeof(uint32_t));
    EXPECT_EQ(base.areg::BufferBase::read(storage), sizeof(uint32_t));
    EXPECT_EQ(base.position(), position);
    EXPECT_EQ(base.size_used(), sizeof(uint32_t));
}

// Serializing a view copies its visible slice, not the surrounding owner's bytes.
TEST(BufferBaseTest, serializing_view_copies_only_its_visible_slice)
{
    constexpr uint8_t DATA[]{ 10u, 20u, 30u };
    areg::SharedBuffer inner(DATA, sizeof(DATA));
    areg::SharedBuffer outer;
    outer << uint32_t{ 99u } << inner << uint32_t{ 100u };
    outer.reset();
    EXPECT_EQ(outer.read32_bits(), 99u);
    areg::SharedBuffer view;
    ASSERT_EQ(outer.read(view), sizeof(DATA));
    areg::BufferBase stream;
    EXPECT_EQ(stream.write(view), sizeof(DATA));
    stream.reset();
    EXPECT_EQ(stream.read32_bits(), sizeof(DATA));
    uint8_t output[sizeof(DATA)]{};
    EXPECT_EQ(stream.read(output, sizeof(output)), sizeof(DATA));
    EXPECT_EQ(std::memcmp(output, DATA, sizeof(DATA)), 0);
    EXPECT_TRUE(stream.is_end());
}

// A text length larger than the payload consumes only the available bytes.
TEST(BufferBaseTest, oversized_text_length_consumes_only_available_bytes)
{
    areg::BufferBase buffer;
    std::string text{ "unchanged" };
    EXPECT_EQ(buffer.read_text(text), 0u);
    EXPECT_TRUE(text.empty());
    ASSERT_TRUE(buffer.write32_bits(std::numeric_limits<uint32_t>::max()));
    constexpr uint8_t DATA[]{ 'a', 'b', 'c' };
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    buffer.reset();
    text = "unchanged";
    EXPECT_EQ(buffer.read_text(text), sizeof(uint32_t) + sizeof(DATA));
    EXPECT_TRUE(text.empty());
    EXPECT_EQ(buffer.position(), buffer.size_used());
    EXPECT_EQ(buffer.bounded_count(1u), 0u);
}

// Reserving an already available total capacity must not reallocate the buffer.
TEST(BufferBaseTest, reserve_total_capacity_does_not_grow_sufficient_storage)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    const uint8_t* const allocation{ buffer.buffer() };
    const uint32_t capacity{ buffer.size_available() };
    EXPECT_EQ(buffer.reserve(capacity, true), capacity - sizeof(DATA));
    EXPECT_EQ(buffer.size_available(), capacity);
    EXPECT_EQ(buffer.buffer(), allocation);
}

// A typed-array read must reject an element count whose byte size overflows.
TEST(BufferBaseTest, direct_array_rejects_overflowing_element_count)
{
    constexpr uint32_t VALUE{ 42u };
    areg::BufferBase buffer;
    ASSERT_EQ(buffer.write_pod(VALUE), sizeof(VALUE));
    buffer.reset();
    EXPECT_EQ(buffer.read_array<uint32_t>(0x40000000u), nullptr);
    EXPECT_EQ(buffer.position(), 0u);
    EXPECT_EQ(buffer.write_array(&VALUE, 0x40000001u), 0u);
    EXPECT_EQ(buffer.position(), 0u);
}

// Reserving more than the capacity grows to the requested total and keeps the data.
TEST(BufferBaseTest, reserve_grows_to_requested_total)
{
    const std::vector<uint8_t> data(4096u, 3u);
    const uint32_t used{ static_cast<uint32_t>(data.size()) };
    areg::BufferBase buffer(data.data(), used);
    const uint32_t requested{ buffer.size_available() + 1u };
    EXPECT_GE(buffer.reserve(requested, true), requested - used);
    EXPECT_GE(buffer.size_available(), requested);
    EXPECT_LT(buffer.size_available(), requested + used);
    EXPECT_EQ(buffer.size_used(), used);
    EXPECT_EQ(std::memcmp(buffer.buffer(), data.data(), used), 0);
}

// Reserving less than the written bytes with copy keeps all of them.
TEST(BufferBaseTest, reserve_below_used_keeps_data)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u, 5u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    EXPECT_EQ(buffer.reserve(2u, true), buffer.size_available() - sizeof(DATA));
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(buffer.buffer(), DATA, sizeof(DATA)), 0);
}

// A write whose end overflows 32 bits is rejected and the cursor stays.
TEST(BufferBaseTest, write_with_wrapping_size_is_rejected)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    areg::BufferBase buffer(DATA, sizeof(DATA));
    const uint32_t huge{ std::numeric_limits<uint32_t>::max() - 1u };
    EXPECT_EQ(buffer.write(DATA, huge), 0u);
    EXPECT_EQ(buffer.position(), sizeof(DATA));
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
    EXPECT_EQ(buffer.insert_at(DATA, huge, 0u), 0u);
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(buffer.buffer(), DATA, sizeof(DATA)), 0);
}

// Writing a SharedBuffer into a block sized for it keeps the block.
TEST(BufferBaseTest, presized_shared_buffer_write_keeps_allocation)
{
    const std::vector<uint8_t> head(2000u, 5u);
    const std::vector<uint8_t> data(4000u, 7u);
    areg::SharedBuffer source(data.data(), static_cast<uint32_t>(data.size()));
    const uint32_t needed{ static_cast<uint32_t>(head.size()) + areg::required_size<areg::SharedBuffer>{}(source) };
    areg::BufferBase stream(needed, areg::BLOCK_SIZE);
    ASSERT_EQ(stream.write(head.data(), static_cast<uint32_t>(head.size())), head.size());
    const uint8_t* const allocation{ stream.buffer() };
    ASSERT_GE(stream.size_available(), needed);
    ASSERT_LT(stream.size_available(), 2u * needed - static_cast<uint32_t>(head.size()));
    EXPECT_EQ(stream.write(source), data.size());
    EXPECT_EQ(stream.buffer(), allocation);
    EXPECT_EQ(stream.size_used(), needed);
}

// An invalid SharedBuffer is serialized as length 0, so the next field stays aligned.
TEST(BufferBaseTest, invalid_shared_buffer_writes_zero_length)
{
    const areg::SharedBuffer invalid;
    ASSERT_FALSE(invalid.is_valid());
    areg::BufferBase stream;
    areg::OutStream& out{ stream };
    out << invalid << uint32_t{ 77u };
    EXPECT_EQ(stream.size_used(), 2u * sizeof(uint32_t));
    stream.reset();
    constexpr uint8_t STALE[]{ 9u, 9u };
    areg::SharedBuffer destination(STALE, sizeof(STALE));
    const areg::InStream& in{ stream };
    in >> destination;
    EXPECT_FALSE(destination.is_valid());
    EXPECT_EQ(stream.read32_bits(), 77u);
}

// A nested length beyond the data is rejected before any allocation and the cursor stays.
TEST(BufferBaseTest, nested_length_overflow_is_rejected)
{
    areg::BufferBase stream;
    ASSERT_TRUE(stream.write32_bits(std::numeric_limits<uint32_t>::max() - 1u));
    stream.reset();
    constexpr uint8_t STALE[]{ 5u };
    areg::SharedBuffer destination(STALE, sizeof(STALE));
    EXPECT_EQ(stream.read(destination), 0u);
    EXPECT_EQ(stream.position(), 0u);
    EXPECT_EQ(destination.size_used(), sizeof(STALE));
}

// A truncated nested length leaves the cursor where it was.
TEST(BufferBaseTest, truncated_nested_length_keeps_cursor)
{
    constexpr uint8_t DATA[]{ 1u, 2u };
    areg::BufferBase stream(DATA, sizeof(DATA));
    stream.reset();
    areg::SharedBuffer destination;
    EXPECT_EQ(stream.read(destination), 0u);
    EXPECT_EQ(stream.position(), 0u);
    EXPECT_FALSE(destination.is_valid());
}

namespace
{
    //! Makes the protected interface of the buffer reachable for a test.
    class BufferProbe : public areg::BufferBase
    {
    public:
        using areg::BufferBase::BufferBase;
        using areg::BufferBase::buffer_to_read;
        using areg::BufferBase::buffer_to_write;
        using areg::BufferBase::end_of_buffer;
        using areg::BufferBase::header;
        using areg::BufferBase::init_buffer;
        using areg::BufferBase::read_view;
        using areg::BufferBase::size_writable;
        using areg::BufferBase::write_data;
        using areg::BufferBase::write_view;
    };
}

// The position decides is_begin() of a valid buffer.
TEST(BufferBaseTest, is_begin_follows_the_position)
{
    constexpr uint8_t DATA[]{ 1u, 2u };
    areg::BufferBase buffer;
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    EXPECT_FALSE(buffer.is_begin());
    buffer.reset();
    EXPECT_TRUE(buffer.is_begin());
}

// A read of no element gives nothing, a read of one element gives its address.
TEST(BufferBaseTest, read_array_of_no_element_and_of_one_element)
{
    constexpr uint32_t VALUE{ 7u };
    areg::BufferBase buffer;
    ASSERT_EQ(buffer.write_pod(VALUE), sizeof(VALUE));
    buffer.reset();
    EXPECT_EQ(buffer.read_array<uint32_t>(0u), nullptr);
    const uint32_t * const read{ buffer.read_array<uint32_t>(1u) };
    ASSERT_NE(read, nullptr);
    EXPECT_EQ(*read, VALUE);
}

// The read address of a buffer without a block is null, the writable size is 0.
TEST(BufferBaseTest, buffer_without_a_block_gives_no_address_and_no_room)
{
    BufferProbe buffer;
    EXPECT_EQ(buffer.buffer_to_read(), nullptr);
    EXPECT_EQ(buffer.size_writable(), 0u);
}

// The writable size is what the block has after the used bytes.
TEST(BufferBaseTest, writable_size_is_the_rest_of_the_block)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    BufferProbe buffer;
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    EXPECT_EQ(buffer.size_writable(), buffer.size_available() - sizeof(DATA));
}

// Without a new block init_buffer() reports an invalid position.
TEST(BufferBaseTest, init_buffer_without_a_block_is_invalid)
{
    BufferProbe buffer;
    EXPECT_EQ(buffer.init_buffer(nullptr, 64u, false), areg::Cursor::INVALID_CURSOR_POSITION);
}

// A view of no bytes and a view past the end of the data are refused.
TEST(BufferBaseTest, read_view_refuses_no_bytes_and_a_range_past_the_data)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    BufferProbe buffer;
    EXPECT_EQ(buffer.read_view(1u), nullptr);
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    buffer.reset();
    EXPECT_EQ(buffer.read_view(0u), nullptr);
    EXPECT_EQ(buffer.read_view(sizeof(DATA) + 1u), nullptr);
    ASSERT_NE(buffer.read_view(3u), nullptr);

    // Another owner of the block cuts the data before the position of this one.
    BufferProbe other(buffer);
    other.set_size_used(1u);
    EXPECT_EQ(buffer.read_view(1u), nullptr);
}

// A write view of no bytes and one that wraps the position are refused.
TEST(BufferBaseTest, write_view_refuses_no_bytes_and_a_wrapping_size)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    BufferProbe buffer;
    EXPECT_EQ(buffer.write_view(0u), nullptr);
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    EXPECT_EQ(buffer.write_view(0xFFFFFFFFu), nullptr);
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
}

// A write view before the end of the data keeps the used size.
TEST(BufferBaseTest, write_view_inside_the_data_keeps_the_used_size)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    BufferProbe buffer;
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    buffer.reset();
    uint8_t * const view{ buffer.write_view(2u) };
    ASSERT_NE(view, nullptr);
    view[0] = 9u;
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
    EXPECT_EQ(buffer.buffer()[0], 9u);
}

// A write view grows the block and the used size.
TEST(BufferBaseTest, write_view_grows_the_block)
{
    BufferProbe buffer;
    uint8_t * const view{ buffer.write_view(10000u) };
    ASSERT_NE(view, nullptr);
    EXPECT_EQ(buffer.size_used(), 10000u);
}

// A write that wraps the position, and one past the limit of a buffer, write nothing.
TEST(BufferBaseTest, write_refuses_a_wrapping_size_and_a_size_past_the_limit)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    areg::BufferBase buffer;
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    EXPECT_EQ(buffer.write(DATA, 0xFFFFFFFFu), 0u);
    EXPECT_EQ(buffer.write(DATA, areg::MAX_BUF_LENGTH + 1u), 0u);
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
}

// A write of nothing and a write from no source are refused on the fast path too.
TEST(BufferBaseTest, write_refuses_no_bytes_and_no_source)
{
    constexpr uint8_t DATA[]{ 1u };
    BufferProbe buffer;
    ASSERT_EQ(buffer.write_data(DATA, sizeof(DATA)), sizeof(DATA));
    EXPECT_EQ(buffer.write(DATA, 0u), 0u);
    EXPECT_EQ(buffer.write(nullptr, 1u), 0u);
    EXPECT_EQ(buffer.size_used(), sizeof(DATA));
}

// Reproduces the defect: ensure_size(0) reported no room on a buffer without a block, so an empty value was never sent.
TEST(BufferBaseTest, ensure_size_of_nothing_is_always_true)
{
    areg::BufferBase buffer;
    EXPECT_TRUE(buffer.ensure_size(0u));
    EXPECT_FALSE(buffer.is_valid());
}

// ensure_size() keeps a block that has the room and grows one that has not.
TEST(BufferBaseTest, ensure_size_keeps_or_grows_the_block)
{
    constexpr uint8_t DATA[]{ 5u, 6u };
    areg::BufferBase buffer;
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    const uint32_t capacity{ buffer.size_available() };
    EXPECT_TRUE(buffer.ensure_size(1u));
    EXPECT_EQ(buffer.size_available(), capacity);
    EXPECT_TRUE(buffer.ensure_size(capacity * 2u));
    EXPECT_GT(buffer.size_available(), capacity);
    EXPECT_EQ(buffer.position(), sizeof(DATA));
    EXPECT_EQ(buffer.buffer()[1], 6u);
}

// A length prefix that claims more than the rest of the source reads nothing.
TEST(BufferBaseTest, read_of_a_buffer_refuses_a_length_past_the_data)
{
    const uint32_t length{ 100u };
    areg::BufferBase source;
    ASSERT_EQ(source.write_pod(length), sizeof(length));
    source.reset();
    areg::SharedBuffer target;
    EXPECT_EQ(source.read(target), 0u);
    EXPECT_FALSE(target.is_valid());
    EXPECT_EQ(source.position(), 0u);
}

// A write view of no bytes on a valid buffer is refused and changes nothing.
TEST(BufferBaseTest, write_view_of_no_bytes_on_a_valid_buffer)
{
    constexpr uint8_t DATA[]{ 1u };
    BufferProbe buffer;
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    EXPECT_EQ(buffer.write_view(0u), nullptr);
    EXPECT_EQ(buffer.position(), sizeof(DATA));
}

// Every copy size goes through the fast write, the growing write and the read of a buffer.
TEST(BufferBaseTest, every_copy_size_writes_and_reads)
{
    uint8_t source[16]{ };
    for (uint32_t i = 0u; i < sizeof(source); ++i)
    {
        source[i] = static_cast<uint8_t>(0xA0u + i);
    }

    for (uint32_t size : { 1u, 2u, 3u, 4u, 8u, 16u })
    {
        areg::BufferBase grown;
        ASSERT_EQ(grown.write(source, size), size);

        areg::BufferBase fast(4096u, areg::BLOCK_SIZE);
        ASSERT_EQ(fast.write(source, size), size);

        uint8_t target[16]{ };
        grown.reset();
        ASSERT_EQ(grown.read(target, size), size);
        EXPECT_TRUE(areg::mem_equal(target, source, size)) << "size " << size;

        fast.reset();
        ASSERT_EQ(fast.read(target, size), size);
        EXPECT_TRUE(areg::mem_equal(target, source, size)) << "size " << size;
    }
}

// A buffer without a block is empty and has no header, no end and no writable address.
TEST(BufferBaseTest, buffer_without_a_block_is_empty_and_has_no_header)
{
    BufferProbe buffer;
    EXPECT_TRUE(buffer.is_empty());
    EXPECT_EQ(buffer.header(), nullptr);
    EXPECT_EQ(std::as_const(buffer).header(), nullptr);
    EXPECT_EQ(buffer.end_of_buffer(), nullptr);
    EXPECT_EQ(std::as_const(buffer).end_of_buffer(), nullptr);
}

// A buffer with data is not empty, its header holds the used size and the end follows the data.
TEST(BufferBaseTest, buffer_with_data_gives_header_end_and_writable_address)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u, 5u };
    BufferProbe buffer(1u, DATA, sizeof(DATA), areg::BLOCK_SIZE);
    EXPECT_FALSE(buffer.is_empty());
    ASSERT_NE(buffer.header(), nullptr);
    EXPECT_EQ(buffer.header()->biUsed, sizeof(DATA));
    EXPECT_EQ(std::as_const(buffer).header(), buffer.header());

    const uint8_t * const begin{ std::as_const(buffer).buffer() };
    ASSERT_NE(begin, nullptr);
    EXPECT_EQ(buffer.end_of_buffer(), begin + sizeof(DATA));
    EXPECT_EQ(std::as_const(buffer).end_of_buffer(), begin + sizeof(DATA));
    EXPECT_EQ(buffer.buffer_to_write(), begin + buffer.position());
}

/**
 * \brief   A buffer without data has no valid position; a buffer with data has one.
 **/
TEST(BufferBaseTest, position_is_valid_only_for_a_buffer_with_data)
{
    areg::SharedBuffer empty;
    EXPECT_FALSE(empty.is_position_valid());
    areg::SharedBuffer filled;
    filled << uint32_t{ 42u };
    EXPECT_TRUE(filled.is_position_valid());
    filled.move_to_begin();
    EXPECT_TRUE(filled.is_position_valid());
    EXPECT_EQ(filled.position(), 0u);
}
