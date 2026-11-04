/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SharedBufferTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for SharedBuffer.
 *              Covers: initialization, streaming operators, core methods,
 *              cursor behavior (unified read/write cursor), and object semantics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/MessageEnvelope.hpp"

#include <array>
#include <cstring>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace
{
    constexpr uint32_t VAL_A { 0xAAAAAAAAu };
    constexpr uint32_t VAL_B { 0xBBBBBBBBu };
    constexpr uint32_t VAL_C { 0xCCCCCCCCu };
}

//////////////////////////////////////////////////////////////////////////
// 1. Initialization
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Default-constructed buffer is invalid until first write.
 **/
TEST(SharedBufferTest, default_construction)
{
    areg::SharedBuffer buf;
    EXPECT_FALSE(buf.is_valid());
    EXPECT_TRUE(buf.is_empty());
    EXPECT_EQ(buf.size_used(), 0u);
    EXPECT_EQ(buf.position(), areg::Cursor::INVALID_CURSOR_POSITION);
}

/**
 * \brief   Reserve constructor pre-allocates without writing any data.
 **/
TEST(SharedBufferTest, reserve_construction)
{
    areg::SharedBuffer buf(128u, areg::BLOCK_SIZE);
    EXPECT_TRUE(buf.is_valid());
    EXPECT_TRUE(buf.is_empty());
    EXPECT_EQ(buf.size_used(), 0u);
    EXPECT_GE(buf.size_available(), 128u);
    EXPECT_EQ(buf.position(), 0u);
}

/**
 * \brief   Raw-bytes constructor writes data; cursor is after written data.
 **/
TEST(SharedBufferTest, raw_bytes_construction)
{
    constexpr uint8_t  data[] { 0x01, 0x02, 0x03, 0x04 };
    constexpr uint32_t len    { static_cast<uint32_t>(std::size(data)) };

    areg::SharedBuffer buf(data, len);
    EXPECT_TRUE(buf.is_valid());
    EXPECT_FALSE(buf.is_empty());
    EXPECT_EQ(buf.size_used(), len);
    EXPECT_EQ(buf.position(), len);   // cursor is after the written data

    buf.reset();
    uint8_t out[4] {};
    uint32_t read = buf.read(out, len);
    EXPECT_EQ(read, len);
    EXPECT_EQ(std::memcmp(out, data, len), 0);
}

/**
 * \brief   String constructor writes the stream string format: a uint32 count and the characters.
 **/
TEST(SharedBufferTest, string_construction)
{
    const char* text  = "hello";
    areg::SharedBuffer buf(text);
    EXPECT_TRUE(buf.is_valid());
    EXPECT_FALSE(buf.is_empty());
    EXPECT_EQ(buf.size_used(), static_cast<uint32_t>(sizeof(uint32_t) + std::strlen(text)));

    areg::String read;
    buf.move_to_begin();
    buf >> read;
    EXPECT_EQ(read, text);
}

//////////////////////////////////////////////////////////////////////////
// 2. Streaming operators
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Round-trip: write uint32_t via <<, read back via >>.
 **/
TEST(SharedBufferTest, stream_uint32_roundtrip)
{
    areg::SharedBuffer buf;
    buf << VAL_A;
    EXPECT_EQ(buf.size_used(), sizeof(uint32_t));
    EXPECT_EQ(buf.position(), sizeof(uint32_t));

    buf.reset();
    uint32_t val { 0u };
    buf >> val;
    EXPECT_EQ(val, VAL_A);
}

/**
 * \brief   Multiple heterogeneous types written in sequence are read back in order.
 **/
TEST(SharedBufferTest, stream_multiple_types)
{
    constexpr uint32_t vu32 { 42u };
    constexpr int64_t  vi64 { -7LL };
    constexpr bool     vb   { true };
    constexpr float    vf   { 3.14f };

    areg::SharedBuffer buf;
    buf << vu32 << vi64 << vb << vf;
    EXPECT_EQ(buf.size_used(), sizeof(uint32_t) + sizeof(int64_t) + sizeof(bool) + sizeof(float));

    buf.reset();
    uint32_t r32 {}; int64_t r64 {}; bool rb {}; float rf {};
    buf >> r32 >> r64 >> rb >> rf;
    EXPECT_EQ(r32, vu32);
    EXPECT_EQ(r64, vi64);
    EXPECT_EQ(rb,  vb);
    EXPECT_FLOAT_EQ(rf, vf);
}

/**
 * \brief   SharedBuffer round-trip via operator<< / operator>>: length-prefixed serialization.
 **/
TEST(SharedBufferTest, stream_sharedbuffer_roundtrip)
{
    areg::SharedBuffer inner;
    inner << VAL_A << VAL_B;

    areg::SharedBuffer outer;
    outer << inner;   // serializes length + data
    EXPECT_GT(outer.size_used(), inner.size_used());

    outer.reset();
    areg::SharedBuffer result;
    outer >> result;   // deserializes; result.mReadPos is rewound to 0 by operator>>
    uint32_t a {}, b {};
    result >> a >> b;
    EXPECT_EQ(a, VAL_A);
    EXPECT_EQ(b, VAL_B);
}

//////////////////////////////////////////////////////////////////////////
// 3. Core methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   write() / read() round-trip on raw bytes.
 **/
TEST(SharedBufferTest, write_and_read_raw)
{
    constexpr uint8_t  data[] { 10, 20, 30, 40 };
    constexpr uint32_t len    { 4u };

    areg::SharedBuffer buf;
    uint32_t written = buf.write(data, len);
    EXPECT_EQ(written, len);
    EXPECT_EQ(buf.size_used(), len);

    buf.reset();
    uint8_t out[4] {};
    uint32_t read = buf.read(out, len);
    EXPECT_EQ(read, len);
    EXPECT_EQ(std::memcmp(out, data, len), 0);
}

/**
 * \brief   reset() rewinds the cursor to 0 without erasing data.
 **/
TEST(SharedBufferTest, reset_rewinds_without_clearing)
{
    areg::SharedBuffer buf;
    buf << VAL_A << VAL_B;
    EXPECT_EQ(buf.position(), 8u);

    buf.reset();
    EXPECT_EQ(buf.position(), 0u);
    EXPECT_EQ(buf.size_used(), 8u);   // data still present

    uint32_t a {}, b {};
    buf >> a >> b;
    EXPECT_EQ(a, VAL_A);
    EXPECT_EQ(b, VAL_B);
}

/**
 * \brief   insert_at() beyond biUsed appends at the logical end.
 **/
TEST(SharedBufferTest, insert_at_end_appends)
{
    constexpr uint8_t first[]  { 1, 2, 3 };
    constexpr uint8_t second[] { 4, 5 };

    areg::SharedBuffer buf;
    buf.write(first, 3u);
    EXPECT_EQ(buf.size_used(), 3u);

    buf.insert_at(second, 2u, 100u);   // 100 > biUsed --> append
    EXPECT_EQ(buf.size_used(), 5u);

    buf.reset();
    uint8_t out[5] {};
    buf.read(out, 5u);
    EXPECT_EQ(out[0], 1); EXPECT_EQ(out[1], 2); EXPECT_EQ(out[2], 3);
    EXPECT_EQ(out[3], 4); EXPECT_EQ(out[4], 5);
}

/**
 * \brief   insert_at() in the middle shifts existing bytes right.
 **/
TEST(SharedBufferTest, insert_at_middle_shifts)
{
    constexpr uint8_t initial[]  { 1, 2, 5, 6 };
    constexpr uint8_t inserted[] { 3, 4 };

    areg::SharedBuffer buf(initial, 4u);
    buf.insert_at(inserted, 2u, 2u);   // insert at offset 2 --> {1,2,3,4,5,6}
    EXPECT_EQ(buf.size_used(), 6u);

    buf.reset();
    uint8_t out[6] {};
    buf.read(out, 6u);
    for (uint8_t i = 0; i < 6; ++i)
        EXPECT_EQ(out[i], static_cast<uint8_t>(i + 1)) << "mismatch at index " << +i;
}

/**
 * \brief   insert_at(0) prepends all existing bytes.
 **/
TEST(SharedBufferTest, insert_at_beginning_prepends)
{
    constexpr uint8_t initial[]   { 3, 4 };
    constexpr uint8_t prepended[] { 1, 2 };

    areg::SharedBuffer buf(initial, 2u);
    buf.insert_at(prepended, 2u, 0u);
    EXPECT_EQ(buf.size_used(), 4u);

    buf.reset();
    uint8_t out[4] {};
    buf.read(out, 4u);
    EXPECT_EQ(out[0], 1); EXPECT_EQ(out[1], 2);
    EXPECT_EQ(out[2], 3); EXPECT_EQ(out[3], 4);
}

//////////////////////////////////////////////////////////////////////////
// 4. Cursor behavior (CRITICAL)
//
// SharedBuffer uses a single unified cursor (mReadPos) for BOTH reads
// and writes.  These tests verify that the cursor advances correctly on
// every write and read, that set_position() with each SeekOrigin moves
// the cursor as documented, and that writing mid-buffer overwrites
// rather than appends.
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Cursor is INVALID before any write; advances by sizeof(T) on each write.
 **/
TEST(SharedBufferTest, cursor_initial_position_and_write_advance)
{
    areg::SharedBuffer buf;
    EXPECT_EQ(buf.position(), areg::Cursor::INVALID_CURSOR_POSITION);   // not yet allocated

    buf << uint8_t(1);
    EXPECT_EQ(buf.position(), 1u);

    buf << uint32_t(2);
    EXPECT_EQ(buf.position(), 1u + sizeof(uint32_t));
}

/**
 * \brief   set_position() with SeekOrigin::Begin moves cursor to an absolute offset.
 **/
TEST(SharedBufferTest, cursor_set_position_begin)
{
    areg::SharedBuffer buf;
    buf << VAL_A << VAL_B;   // 8 bytes written, cursor at 8

    uint32_t pos = buf.set_position(0, areg::Cursor::SeekOrigin::Begin);
    EXPECT_EQ(pos, 0u);
    EXPECT_EQ(buf.position(), 0u);

    pos = buf.set_position(4, areg::Cursor::SeekOrigin::Begin);
    EXPECT_EQ(pos, 4u);
    EXPECT_EQ(buf.position(), 4u);
}

/**
 * \brief   set_position() with SeekOrigin::End places cursor relative to size_used().
 **/
TEST(SharedBufferTest, cursor_set_position_end)
{
    areg::SharedBuffer buf;
    buf << VAL_A << VAL_B;   // size_used = 8

    uint32_t pos = buf.set_position(0, areg::Cursor::SeekOrigin::End);
    EXPECT_EQ(pos, buf.size_used());

    pos = buf.set_position(-4, areg::Cursor::SeekOrigin::End);
    EXPECT_EQ(pos, buf.size_used() - 4u);
}

/**
 * \brief   set_position() with SeekOrigin::Current moves cursor by a signed delta.
 **/
TEST(SharedBufferTest, cursor_set_position_current)
{
    areg::SharedBuffer buf;
    buf << VAL_A << VAL_B;
    buf.reset();   // cursor at 0

    uint32_t pos = buf.set_position(4, areg::Cursor::SeekOrigin::Current);
    EXPECT_EQ(pos, 4u);

    pos = buf.set_position(2, areg::Cursor::SeekOrigin::Current);
    EXPECT_EQ(pos, 6u);
}

/**
 * \brief   read() advances the cursor by the number of bytes consumed.
 **/
TEST(SharedBufferTest, cursor_read_advances)
{
    areg::SharedBuffer buf;
    buf << VAL_A << VAL_B;
    buf.reset();   // cursor at 0

    uint32_t a {};
    buf >> a;
    EXPECT_EQ(buf.position(), sizeof(uint32_t));

    uint32_t b {};
    buf >> b;
    EXPECT_EQ(buf.position(), 2u * sizeof(uint32_t));
}

/**
 * \brief   Writing mid-buffer (cursor < size_used) overwrites in-place without growing size.
 **/
TEST(SharedBufferTest, cursor_overwrite_in_place)
{
    areg::SharedBuffer buf;
    buf << VAL_A << VAL_B;   // [AA AA AA AA | BB BB BB BB], size=8, cursor=8

    buf.set_position(0, areg::Cursor::SeekOrigin::Begin);
    buf << VAL_C;   // overwrites bytes 0-3
    EXPECT_EQ(buf.size_used(), 8u);   // size must NOT grow
    EXPECT_EQ(buf.position(), sizeof(uint32_t));

    buf.reset();
    uint32_t a {}, b {};
    buf >> a >> b;
    EXPECT_EQ(a, VAL_C);   // overwritten
    EXPECT_EQ(b, VAL_B);   // untouched
}

/**
 * \brief   move_to_begin() / move_to_end() set cursor to 0 / size_used.
 **/
TEST(SharedBufferTest, cursor_move_to_begin_and_end)
{
    areg::SharedBuffer buf;
    buf << VAL_A << VAL_B;

    buf.move_to_begin();
    EXPECT_EQ(buf.position(), 0u);
    EXPECT_TRUE(buf.is_begin());

    buf.move_to_end();
    EXPECT_EQ(buf.position(), buf.size_used());
    EXPECT_TRUE(buf.is_end());
}

/**
 * \brief   After move_to_end() a write appends, growing size_used.
 **/
TEST(SharedBufferTest, cursor_append_after_move_to_end)
{
    areg::SharedBuffer buf;
    buf << VAL_A;   // 4 bytes

    // Seek back to start and overwrite, then move to end and append
    buf.reset();
    buf << VAL_C;   // overwrites at [0..3], cursor=4, size=4 still
    EXPECT_EQ(buf.size_used(), 4u);

    buf.move_to_end();
    buf << VAL_B;   // appends at [4..7]
    EXPECT_EQ(buf.size_used(), 8u);

    buf.reset();
    uint32_t a {}, b {};
    buf >> a >> b;
    EXPECT_EQ(a, VAL_C);
    EXPECT_EQ(b, VAL_B);
}

//////////////////////////////////////////////////////////////////////////
// 5. Object semantics
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Copy constructor shares the underlying block and rewinds to the beginning of the visible slice.
 **/
TEST(SharedBufferTest, copy_constructor_shares)
{
    areg::SharedBuffer original;
    original << VAL_A;
    const uint32_t orig_cursor = original.position();   // 4

    areg::SharedBuffer copy(original);
    EXPECT_TRUE(original.is_shared());
    EXPECT_TRUE(copy.is_shared());
    EXPECT_EQ(original, copy);
    EXPECT_EQ(copy.position(), 0u);
    EXPECT_EQ(original.position(), orig_cursor);    // original cursor unchanged
}

/**
 * \brief   Copy assignment shares the block and rewinds to the beginning of the visible slice.
 **/
TEST(SharedBufferTest, copy_assignment_shares)
{
    areg::SharedBuffer a;
    a << VAL_A;

    areg::SharedBuffer b;
    b << VAL_B;

    b = a;
    EXPECT_TRUE(a.is_shared());
    EXPECT_EQ(a, b);
    EXPECT_EQ(b.position(), 0u);
}

/**
 * \brief   Move constructor transfers ownership; source is left invalid.
 **/
TEST(SharedBufferTest, move_constructor_transfers)
{
    areg::SharedBuffer original;
    original << VAL_A;
    EXPECT_EQ(original.size_used(), sizeof(uint32_t));

    areg::SharedBuffer moved(std::move(original));
    EXPECT_EQ(moved.size_used(), sizeof(uint32_t));
    EXPECT_FALSE(original.is_valid());
}

/**
 * \brief   Move assignment transfers ownership; source is left invalid.
 **/
TEST(SharedBufferTest, move_assignment_transfers)
{
    areg::SharedBuffer src;
    src << VAL_A;

    areg::SharedBuffer dst;
    dst = std::move(src);
    EXPECT_EQ(dst.size_used(), sizeof(uint32_t));
    EXPECT_FALSE(src.is_valid());
}

TEST(SharedBufferTest, view_copy_preserves_slice_and_rewinds)
{
    areg::SharedBuffer inner;
    inner << VAL_A << VAL_B;

    areg::SharedBuffer outer;
    outer << VAL_C << inner;

    outer.reset();
    uint32_t prefix {};
    outer >> prefix;
    EXPECT_EQ(prefix, VAL_C);

    areg::SharedBuffer view;
    outer >> view;
    EXPECT_EQ(view.size_used(), inner.size_used());
    const uint32_t view_begin = view.position();

    uint32_t first {};
    view >> first;
    EXPECT_EQ(first, VAL_A);

    areg::SharedBuffer copy(view);
    EXPECT_EQ(copy.position(), view_begin);

    uint32_t copied_first {}, second {};
    copy >> copied_first >> second;
    EXPECT_EQ(copied_first, VAL_A);
    EXPECT_EQ(second, VAL_B);
}

TEST(SharedBufferTest, view_move_preserves_slice_and_rewinds)
{
    areg::SharedBuffer inner;
    inner << VAL_A << VAL_B;

    areg::SharedBuffer outer;
    outer << VAL_C << inner;

    outer.reset();
    uint32_t prefix {};
    outer >> prefix;
    EXPECT_EQ(prefix, VAL_C);

    areg::SharedBuffer view;
    outer >> view;
    const uint32_t view_begin = view.position();
    uint32_t first {};
    view >> first;

    areg::SharedBuffer moved(std::move(view));
    EXPECT_EQ(moved.position(), view_begin);
    EXPECT_FALSE(view.is_valid());

    uint32_t moved_first {}, moved_second {};
    moved >> moved_first >> moved_second;
    EXPECT_EQ(moved_first, VAL_A);
    EXPECT_EQ(moved_second, VAL_B);
}

TEST(SharedBufferTest, write_to_view_is_blocked)
{
    areg::SharedBuffer inner;
    inner << VAL_A << VAL_B;

    areg::SharedBuffer outer;
    outer << VAL_C << inner;

    outer.reset();
    uint32_t prefix {};
    outer >> prefix;
    EXPECT_EQ(prefix, VAL_C);

    areg::SharedBuffer view;
    outer >> view;

    // Views are read-only; writing to a view is a programming error caught by ASSERT.
    // Only verify is_view() semantics and that detach() produces a correct owner copy.
    EXPECT_TRUE(view.is_view());
    EXPECT_EQ(view.size_used(), inner.size_used());

    // detach() yields a writable owner copy of the view's content.
    areg::SharedBuffer owner = view.detach();
    EXPECT_TRUE(owner.is_owner());
    EXPECT_FALSE(owner.is_view());
    owner.move_to_begin();
    uint32_t da {}, db {};
    owner >> da >> db;
    EXPECT_EQ(da, VAL_A);
    EXPECT_EQ(db, VAL_B);
}

/**
 * \brief   clone() returns a deep-independent copy; modifying one does not affect the other.
 **/
TEST(SharedBufferTest, clone_independent)
{
    areg::SharedBuffer original;
    original << VAL_A << VAL_B;

    areg::SharedBuffer cloned = original.clone();
    EXPECT_EQ(original, cloned);
    EXPECT_FALSE(original.is_shared());
    EXPECT_FALSE(cloned.is_shared());

    // Overwrite first uint32_t in original -- clone must be unaffected
    original.set_position(0, areg::Cursor::SeekOrigin::Begin);
    original << VAL_C;

    cloned.reset();
    uint32_t ca {}, cb {};
    cloned >> ca >> cb;
    EXPECT_EQ(ca, VAL_A);   // clone still has the original first value
    EXPECT_EQ(cb, VAL_B);
}

//////////////////////////////////////////////////////////////////////////
// 6. Edge cases
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   read() at the end of buffer returns 0 bytes read.
 **/
TEST(SharedBufferTest, read_past_end_returns_zero)
{
    areg::SharedBuffer buf;
    buf << VAL_A;
    buf.move_to_end();

    uint32_t val {};
    uint32_t read = buf.read(reinterpret_cast<uint8_t*>(&val), sizeof(uint32_t));
    EXPECT_EQ(read, 0u);
}

/**
 * \brief   invalidate() makes the buffer invalid and frees the block.
 **/
TEST(SharedBufferTest, invalidate_clears_buffer)
{
    areg::SharedBuffer buf;
    buf << VAL_A;
    EXPECT_TRUE(buf.is_valid());

    buf.invalidate();
    EXPECT_FALSE(buf.is_valid());
    EXPECT_TRUE(buf.is_empty());
    EXPECT_EQ(buf.position(), areg::Cursor::INVALID_CURSOR_POSITION);
}

/**
 * \brief   operator== compares data content; operator!= is the complement.
 **/
TEST(SharedBufferTest, equality_operators)
{
    areg::SharedBuffer a, b;
    a << VAL_A << VAL_B;
    b << VAL_A << VAL_B;
    EXPECT_EQ(a, b);

    areg::SharedBuffer c;
    c << VAL_C;
    EXPECT_NE(a, c);
}

/**
 * \brief   Write to a grown buffer works; write to a shared buffer that must reallocate is silently ignored.
 **/
TEST(SharedBufferTest, write_to_shared_buffer)
{
    // Fill a small buffer exactly
    areg::SharedBuffer original(areg::BLOCK_SIZE, areg::BLOCK_SIZE);
    EXPECT_TRUE(original.is_valid());
    const uint32_t capacity_before = original.size_available();

    // Share it
    areg::SharedBuffer copy(original);
    EXPECT_TRUE(original.is_shared());

    // Write enough bytes to REQUIRE reallocation --> should be silently dropped
    const uint32_t too_many = capacity_before + 1u;
    const std::vector<uint8_t> big(too_many, 0xABu);
    original.write(big.data(), too_many);

    EXPECT_EQ(original.size_used(), too_many * sizeof(uint8_t));
    EXPECT_TRUE(copy.is_valid());
    EXPECT_EQ(copy.size_used(), 0u);
}

// Logical shrinking and discarding contents retain the existing allocation.
TEST(SharedBufferTest, shrink_and_reserve_discard_keep_allocation)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    areg::SharedBuffer buffer(128u, DATA, sizeof(DATA));
    const uint8_t* const allocation{ buffer.buffer() };
    buffer.set_size_used(2u);
    EXPECT_EQ(buffer.position(), 2u);
    EXPECT_EQ(buffer.size_used(), 2u);
    buffer.set_size_used(buffer.size_available() + 1u);
    EXPECT_EQ(buffer.size_used(), 2u);
    buffer.reset();
    EXPECT_EQ(buffer.position(), 0u);
    EXPECT_EQ(buffer.size_used(), 2u);
    EXPECT_GT(buffer.reserve(1u, false), 0u);
    EXPECT_EQ(buffer.buffer(), allocation);
    EXPECT_TRUE(buffer.is_empty());
    EXPECT_EQ(buffer.reserve(0u, false), 0u);
    EXPECT_FALSE(buffer.is_valid());
    EXPECT_EQ(buffer.buffer(), nullptr);
    EXPECT_EQ(std::as_const(buffer).buffer(), nullptr);
    buffer.set_size_used(1u);
    EXPECT_EQ(buffer.size_available(), 0u);
    EXPECT_TRUE(buffer.is_begin());
    EXPECT_TRUE(buffer.is_end());
}

// Seeks stay within bounds, and a short read leaves the output tail untouched.
TEST(SharedBufferTest, seek_clamps_and_raw_reads_leave_tail_untouched)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::SharedBuffer buffer(DATA, sizeof(DATA));
    using Origin = areg::Cursor::SeekOrigin;
    EXPECT_EQ(buffer.set_position(-10, Origin::Begin), 0u);
    EXPECT_EQ(buffer.set_position(100, Origin::Begin), sizeof(DATA));
    EXPECT_EQ(buffer.set_position(-100, Origin::Current), 0u);
    EXPECT_EQ(buffer.set_position(100, Origin::Current), sizeof(DATA));
    EXPECT_EQ(buffer.set_position(-100, Origin::End), 0u);
    EXPECT_EQ(buffer.set_position(100, Origin::End), sizeof(DATA));
    buffer.reset();
    std::array<uint8_t, 5> output{ 9u, 9u, 9u, 9u, 9u };
    EXPECT_EQ(buffer.read(nullptr, 1u), 0u);
    EXPECT_EQ(buffer.read(output.data(), 0u), 0u);
    EXPECT_EQ(buffer.read(output.data(), static_cast<uint32_t>(output.size())), sizeof(DATA));
    EXPECT_EQ(output, (std::array<uint8_t, 5>{ 1u, 2u, 3u, 9u, 9u }));
    EXPECT_EQ(buffer.position(), sizeof(DATA));
    buffer.invalidate();
    EXPECT_EQ(buffer.read(output.data(), 1u), 0u);
    EXPECT_EQ(buffer.set_position(0, Origin::Begin), areg::Cursor::INVALID_CURSOR_POSITION);
}

// An assigned view keeps its slice alive after the original owner releases it.
TEST(SharedBufferTest, view_assignment_retains_slice_after_owner_release)
{
    constexpr uint8_t DATA[]{ 11u, 12u, 13u };
    areg::SharedBuffer inner(DATA, sizeof(DATA));
    areg::SharedBuffer outer;
    outer << VAL_C << inner << VAL_B;
    outer.reset();
    EXPECT_EQ(outer.read32_bits(), VAL_C);
    areg::SharedBuffer view;
    ASSERT_EQ(outer.read(view), sizeof(DATA));
    const uint8_t* const slice{ std::as_const(view).buffer() };
    areg::SharedBuffer copied;
    copied = view;
    areg::SharedBuffer moved;
    moved = std::move(view);
    EXPECT_FALSE(view.is_valid());
    EXPECT_TRUE(view.is_owner());
    EXPECT_TRUE(copied.is_view());
    EXPECT_TRUE(moved.is_view());
    EXPECT_EQ(copied.position(), 0u);
    outer.invalidate();
    copied.invalidate();
    EXPECT_TRUE(moved.is_unique());
    EXPECT_EQ(std::as_const(moved).buffer(), slice);
    std::array<uint8_t, 5> output{};
    EXPECT_EQ(moved.read(output.data(), static_cast<uint32_t>(output.size())), sizeof(DATA));
    EXPECT_EQ(output, (std::array<uint8_t, 5>{ 11u, 12u, 13u, 0u, 0u }));
    EXPECT_TRUE(moved.is_end());
    moved.reset();
    EXPECT_EQ(moved.position(), 0u);
    EXPECT_EQ(moved.bounded_count(100u), sizeof(DATA));
}

// Growing a view detaches only the bytes visible through that view.
TEST(SharedBufferTest, view_reserve_detaches_only_visible_bytes)
{
    constexpr uint8_t DATA[]{ 11u, 12u, 13u };
    areg::SharedBuffer inner(DATA, sizeof(DATA));
    areg::SharedBuffer outer;
    outer << VAL_A << inner << VAL_B;
    outer.reset();
    EXPECT_EQ(outer.read32_bits(), VAL_A);
    areg::SharedBuffer view;
    ASSERT_EQ(outer.read(view), sizeof(DATA));
    EXPECT_FALSE(view.ensure_size(1u));
    EXPECT_EQ(view.insert_at(DATA, 1u, 0u), 0u);
    EXPECT_EQ(view.position(), 0u);
    EXPECT_GE(view.reserve(64u, false), 64u);
    EXPECT_TRUE(view.is_owner());
    EXPECT_FALSE(view.is_shared());
    EXPECT_EQ(view.size_used(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(view.buffer(), DATA, sizeof(DATA)), 0);
    view.reset();
    const uint8_t replacement{ 99u };
    EXPECT_EQ(view.write(&replacement, 1u), 1u);
    outer.reset();
    EXPECT_EQ(outer.read32_bits(), VAL_A);
    areg::SharedBuffer unchanged;
    ASSERT_EQ(outer.read(unchanged), sizeof(DATA));
    EXPECT_EQ(std::as_const(unchanged).buffer()[0], DATA[0]);
}

// Seeking and direct reads cannot escape a shared-buffer slice.
TEST(SharedBufferTest, view_seek_and_direct_reads_cannot_escape_slice)
{
    constexpr uint32_t DATA[]{ 21u, 22u };
    areg::SharedBuffer inner;
    ASSERT_EQ(inner.write_array(DATA, 2u), sizeof(DATA));
    areg::SharedBuffer outer;
    outer << VAL_A << inner << VAL_B;
    outer.reset();
    EXPECT_EQ(outer.read32_bits(), VAL_A);
    areg::SharedBuffer view;
    ASSERT_EQ(outer.read(view), sizeof(DATA));
    using Origin = areg::Cursor::SeekOrigin;
    EXPECT_EQ(view.set_position(-100, Origin::End), 0u);
    EXPECT_EQ(view.set_position(100, Origin::Current), sizeof(DATA));
    EXPECT_EQ(view.set_position(-4, Origin::Current), sizeof(uint32_t));
    EXPECT_EQ(view.read_array<uint32_t>(2u), nullptr);
    EXPECT_EQ(view.position(), sizeof(uint32_t));
    const uint32_t* const last{ view.read_ptr<uint32_t>() };
    ASSERT_NE(last, nullptr);
    EXPECT_EQ(*last, DATA[1]);
    EXPECT_EQ(view.read_ptr<uint32_t>(), nullptr);
    EXPECT_EQ(view.position(), sizeof(DATA));
}

// A truncated nested payload restores the read position before its length field.
TEST(SharedBufferTest, truncated_nested_payload_rolls_back_length)
{
    areg::SharedBuffer outer;
    ASSERT_TRUE(outer.write32_bits(20u));
    ASSERT_TRUE(outer.write8_bits(0xAAu));
    outer.reset();
    areg::SharedBuffer destination;
    EXPECT_EQ(outer.read(destination), 0u);
    EXPECT_EQ(outer.position(), 0u);
    EXPECT_FALSE(destination.is_valid());
    EXPECT_EQ(outer.read32_bits(), 20u);
}

// Self-streaming is harmless, and invalid assignments clear the buffer as specified.
TEST(SharedBufferTest, self_streaming_and_invalid_assignments)
{
    areg::SharedBuffer buffer;
    buffer << VAL_A;
    const uint32_t position{ buffer.position() };
    EXPECT_EQ(buffer.write(buffer), sizeof(VAL_A));
    EXPECT_EQ(buffer.read(buffer), sizeof(VAL_A));
    buffer << buffer;
    buffer >> buffer;
    areg::SharedBuffer& alias{ buffer };
    buffer = alias;
    buffer = std::move(alias);
    EXPECT_EQ(buffer.position(), position);
    EXPECT_EQ(buffer.size_used(), sizeof(VAL_A));
    areg::SharedBuffer invalid;
    EXPECT_EQ(buffer.write(invalid), 0u);
    EXPECT_TRUE(buffer.is_equal(buffer));
    EXPECT_FALSE(buffer.is_equal(invalid));
    EXPECT_FALSE(invalid.is_equal(buffer));
    EXPECT_TRUE(invalid.is_equal(invalid));
    EXPECT_FALSE(invalid.clone().is_valid());
    buffer = invalid;
    EXPECT_FALSE(buffer.is_valid());
    buffer << VAL_B;
    buffer = std::move(invalid);
    EXPECT_FALSE(buffer.is_valid());
}

// Converting between an envelope and a shared buffer retains the payload and ownership.
TEST(SharedBufferTest, envelope_conversions_keep_payload_and_ownership)
{
    constexpr uint8_t DATA[]{ 31u, 32u, 33u };
    areg::MessageEnvelope envelope(true);
    ASSERT_EQ(envelope.write(DATA, sizeof(DATA)), sizeof(DATA));
    const uint8_t* const allocation{ envelope.buffer() };
    areg::SharedBuffer copied(envelope);
    areg::SharedBuffer assigned;
    assigned = envelope;
    EXPECT_EQ(copied.buffer(), allocation);
    EXPECT_EQ(assigned.buffer(), allocation);
    EXPECT_EQ(copied.position(), 0u);
    EXPECT_TRUE(copied.is_owner());
    EXPECT_EQ(envelope.share_buffer().use_count(), 3u);
    areg::SharedBuffer moved(std::move(envelope));
    EXPECT_FALSE(envelope.is_valid());
    EXPECT_EQ(moved.buffer(), allocation);
    areg::MessageEnvelope second(true);
    ASSERT_EQ(second.write(DATA, sizeof(DATA)), sizeof(DATA));
    areg::SharedBuffer moveAssigned;
    moveAssigned = std::move(second);
    EXPECT_FALSE(second.is_valid());
    EXPECT_EQ(moveAssigned.position(), 0u);
    copied.invalidate();
    assigned.invalidate();
    EXPECT_TRUE(moved.is_unique());
    EXPECT_EQ(std::memcmp(moved.buffer(), DATA, sizeof(DATA)), 0);
    EXPECT_EQ(std::memcmp(moveAssigned.buffer(), DATA, sizeof(DATA)), 0);
}

// Assigning an invalid envelope clears a previously held slice.
TEST(SharedBufferTest, invalid_envelope_assignment_clears_previous_slice)
{
    areg::SharedBuffer outer;
    areg::SharedBuffer inner;
    inner << VAL_A;
    outer << inner;
    outer.reset();
    areg::SharedBuffer view;
    ASSERT_EQ(outer.read(view), sizeof(VAL_A));
    areg::MessageEnvelope empty;
    view = empty;
    EXPECT_FALSE(view.is_valid());
    EXPECT_TRUE(view.is_owner());
    view << VAL_B;
    view = std::move(empty);
    EXPECT_FALSE(view.is_valid());
    EXPECT_TRUE(view.is_owner());
    areg::SharedBuffer copied(empty);
    areg::SharedBuffer moved(std::move(empty));
    EXPECT_FALSE(copied.is_valid());
    EXPECT_FALSE(moved.is_valid());
}

// Growing one shared owner leaves the other owner's bytes intact.
TEST(SharedBufferTest, growth_releases_one_owner_without_changing_other)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::SharedBuffer original(DATA, sizeof(DATA));
    areg::SharedBuffer copy(original);
    const uint8_t* const allocation{ copy.buffer() };
    const std::vector<uint8_t> extra(original.size_available() + 1u, 0xA5u);
    ASSERT_EQ(original.write(extra.data(), static_cast<uint32_t>(extra.size())), extra.size());
    EXPECT_EQ(copy.buffer(), allocation);
    EXPECT_EQ(copy.size_used(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(copy.buffer(), DATA, sizeof(DATA)), 0);
    EXPECT_TRUE(copy.is_unique());
    original.invalidate();
    EXPECT_EQ(copy.read8_bits(), DATA[0]);
}

// An empty or incomplete nested-buffer prefix reports no payload.
TEST(SharedBufferTest, empty_and_short_nested_prefixes_report_no_payload)
{
    areg::SharedBuffer source;
    areg::SharedBuffer destination;
    EXPECT_EQ(source.read(destination), 0u);
    EXPECT_EQ(source.read_array<uint8_t>(1u), nullptr);
    EXPECT_EQ(source.read_array<uint8_t>(0u), nullptr);
    EXPECT_TRUE(source.write32_bits(0u));
    source.reset();
    EXPECT_EQ(source.read(destination), 0u);
    EXPECT_EQ(source.position(), sizeof(uint32_t));
    EXPECT_FALSE(destination.is_valid());
    source.set_size_used(2u);
    source.reset();
    EXPECT_EQ(source.read(destination), 0u);
    EXPECT_EQ(source.position(), 0u);
}

// Ensuring capacity and flushing keep the stored bytes intact.
TEST(SharedBufferTest, ensure_size_and_flush_preserve_existing_content)
{
    areg::SharedBuffer buffer;
    EXPECT_TRUE(buffer.ensure_size(64u));
    EXPECT_TRUE(buffer.is_valid());
    EXPECT_TRUE(buffer.is_empty());
    EXPECT_EQ(buffer.position(), 0u);
    buffer << VAL_A << VAL_B;
    ASSERT_EQ(buffer.set_position(1, areg::Cursor::SeekOrigin::Begin), 1u);
    const uint8_t* const allocation{ buffer.buffer() };
    EXPECT_TRUE(buffer.ensure_size(1u));
    EXPECT_EQ(buffer.buffer(), allocation);
    EXPECT_TRUE(buffer.ensure_size(buffer.size_available() + 1u));
    EXPECT_EQ(buffer.position(), 1u);
    EXPECT_EQ(buffer.size_used(), 2u * sizeof(uint32_t));
    buffer.flush();
    EXPECT_EQ(buffer.position(), 1u);
    buffer.reset();
    EXPECT_EQ(buffer.read32_bits(), VAL_A);
    EXPECT_EQ(buffer.read32_bits(), VAL_B);
}

// Text parsing through a view cannot read bytes beyond its slice.
TEST(SharedBufferTest, view_text_parser_cannot_read_trailing_owner_bytes)
{
    areg::SharedBuffer inner;
    ASSERT_TRUE(inner.write32_bits(6u));
    constexpr uint8_t DATA[]{ 'a', 'b', 'c' };
    ASSERT_EQ(inner.write(DATA, sizeof(DATA)), sizeof(DATA));
    areg::SharedBuffer outer;
    outer << VAL_A << inner << VAL_B;
    outer.reset();
    EXPECT_EQ(outer.read32_bits(), VAL_A);
    areg::SharedBuffer view;
    ASSERT_EQ(outer.read(view), inner.size_used());
    std::string text{ "unchanged" };
    EXPECT_EQ(view.read_text(text), sizeof(uint32_t) + sizeof(DATA));
    EXPECT_TRUE(text.empty());
    EXPECT_EQ(view.position(), view.size_used());
    EXPECT_TRUE(view.is_end());
    EXPECT_EQ(outer.read32_bits(), VAL_B);
}

// Reserving an already available total capacity must not reallocate the buffer.
TEST(SharedBufferTest, reserve_total_capacity_does_not_grow_sufficient_storage)
{
    areg::SharedBuffer buffer;
    buffer << VAL_A;
    const uint8_t* const allocation{ buffer.buffer() };
    const uint32_t capacity{ buffer.size_available() };
    EXPECT_EQ(buffer.reserve(capacity, true), capacity - sizeof(VAL_A));
    EXPECT_EQ(buffer.size_available(), capacity);
    EXPECT_EQ(buffer.buffer(), allocation);
}

// An overflowing nested-buffer length must be rejected before creating a view.
TEST(SharedBufferTest, nested_length_overflow_is_rejected)
{
    areg::SharedBuffer stream;
    ASSERT_TRUE(stream.write32_bits(std::numeric_limits<uint32_t>::max()));
    stream.reset();
    areg::SharedBuffer destination;
    ASSERT_EQ(stream.read(destination), 0u);
    EXPECT_FALSE(destination.is_valid());
    EXPECT_EQ(stream.position(), 0u);
}

// A typed-array read must reject an element count whose byte size overflows.
TEST(SharedBufferTest, direct_array_rejects_overflowing_element_count)
{
    constexpr uint32_t VALUE{ 42u };
    areg::SharedBuffer buffer;
    ASSERT_EQ(buffer.write_pod(VALUE), sizeof(VALUE));
    buffer.reset();
    EXPECT_EQ(buffer.read_array<uint32_t>(0x40000000u), nullptr);
    EXPECT_EQ(buffer.position(), 0u);
    EXPECT_EQ(buffer.write_array(&VALUE, 0x40000001u), 0u);
    EXPECT_EQ(buffer.position(), 0u);
}

// A nested length that fits 32 bits but exceeds the data is rejected.
TEST(SharedBufferTest, nested_length_beyond_data_is_rejected)
{
    areg::SharedBuffer stream;
    ASSERT_TRUE(stream.write32_bits(5u));
    ASSERT_TRUE(stream.write32_bits(VAL_A));
    stream.reset();
    areg::SharedBuffer destination;
    EXPECT_EQ(stream.read(destination), 0u);
    EXPECT_FALSE(destination.is_valid());
    EXPECT_EQ(stream.position(), 0u);
}

// An empty nested buffer replaces stale destination content.
TEST(SharedBufferTest, empty_nested_buffer_clears_destination)
{
    areg::SharedBuffer empty;
    areg::SharedBuffer stream;
    stream << VAL_A << empty << VAL_B;
    EXPECT_EQ(stream.size_used(), 3u * sizeof(uint32_t));
    stream.reset();
    EXPECT_EQ(stream.read32_bits(), VAL_A);
    areg::SharedBuffer destination;
    destination << VAL_C;
    stream >> destination;
    EXPECT_FALSE(destination.is_valid());
    EXPECT_EQ(stream.read32_bits(), VAL_B);
}

// A view cannot be the target of a nested write.
TEST(SharedBufferTest, view_rejects_nested_write)
{
    areg::SharedBuffer inner;
    inner << VAL_A;
    areg::SharedBuffer outer;
    outer << inner;
    outer.reset();
    areg::SharedBuffer view;
    ASSERT_EQ(outer.read(view), sizeof(VAL_A));
    ASSERT_TRUE(view.is_view());
    EXPECT_EQ(view.write(inner), 0u);
    EXPECT_TRUE(view.is_view());
    EXPECT_EQ(view.size_used(), sizeof(VAL_A));
}

namespace
{
    //! Makes the protected interface of the buffer reachable for a test.
    class SharedProbe : public areg::SharedBuffer
    {
    public:
        using areg::SharedBuffer::SharedBuffer;
        using areg::SharedBuffer::buffer_to_read;
        using areg::SharedBuffer::init_buffer;
        using areg::SharedBuffer::read_view;
        using areg::SharedBuffer::size_readable;
        using areg::SharedBuffer::size_writable;
        using areg::SharedBuffer::write_data;
        using areg::SharedBuffer::write_view;
    };

    //! Fills target with a view of the bytes in source.
    void _make_view(areg::SharedBuffer & holder, areg::SharedBuffer & target, const uint8_t * data, uint32_t size)
    {
        areg::SharedBuffer inner;
        ASSERT_EQ(inner.write(data, size), size);
        ASSERT_EQ(holder.write(inner), size);
        holder.move_to_begin();
        ASSERT_EQ(holder.read(target), size);
        ASSERT_TRUE(target.is_view());
    }
}

// Every way to write is refused on a view, and the shared block stays as it was.
TEST(SharedBufferTest, view_refuses_every_write)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    constexpr uint32_t VALUE{ 0xAABBCCDDu };
    areg::SharedBuffer holder;
    SharedProbe view;
    _make_view(holder, view, DATA, sizeof(DATA));

    EXPECT_EQ(view.write(DATA, sizeof(DATA)), 0u);
    EXPECT_EQ(view.write_data(DATA, sizeof(DATA)), 0u);
    EXPECT_EQ(view.write_view(sizeof(DATA)), nullptr);
    EXPECT_EQ(view.write_pod(VALUE), 0u);
    EXPECT_EQ(view.write_array(&VALUE, 1u), 0u);
    EXPECT_EQ(view.size_writable(), 0u);
    areg::OutStream & stream{ view };
    stream << VALUE;
    view.set_size_used(1u);

    EXPECT_EQ(view.size_used(), sizeof(DATA));
    EXPECT_EQ(view.buffer()[0], 1u);
    EXPECT_EQ(view.buffer()[3], 4u);
}

// A write through the base class reference of a view is refused too.
TEST(SharedBufferTest, view_refuses_a_write_through_the_base_class)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    areg::SharedBuffer holder;
    areg::SharedBuffer view;
    _make_view(holder, view, DATA, sizeof(DATA));

    areg::BufferBase & base{ view };
    EXPECT_EQ(base.write(DATA, sizeof(DATA)), 0u);
    EXPECT_EQ(view.buffer()[0], 1u);
}

// is_unique() of a buffer without a block is false, of a sole owner true, of two owners false.
TEST(SharedBufferTest, is_unique_counts_the_owners)
{
    constexpr uint8_t DATA[]{ 1u };
    areg::SharedBuffer buffer;
    EXPECT_FALSE(buffer.is_unique());
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    EXPECT_TRUE(buffer.is_unique());
    areg::SharedBuffer other(buffer);
    EXPECT_FALSE(buffer.is_unique());
    EXPECT_TRUE(buffer.is_shared());
}

// The position decides is_begin() of a valid buffer, and of a view.
TEST(SharedBufferTest, is_begin_follows_the_position)
{
    constexpr uint8_t DATA[]{ 1u, 2u };
    areg::SharedBuffer buffer;
    EXPECT_TRUE(buffer.is_begin());
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    EXPECT_FALSE(buffer.is_begin());
    buffer.reset();
    EXPECT_TRUE(buffer.is_begin());
}

// A buffer without a block has no read address, no element to read and no room.
TEST(SharedBufferTest, buffer_without_a_block_gives_no_address)
{
    SharedProbe buffer;
    EXPECT_EQ(buffer.buffer_to_read(), nullptr);
    EXPECT_EQ(buffer.read_array<uint32_t>(1u), nullptr);
    EXPECT_EQ(buffer.size_writable(), 0u);
    EXPECT_EQ(buffer.size_readable(), 0u);
    EXPECT_EQ(buffer.read_view(1u), nullptr);
}

// The writable size of an owner is the rest of the block.
TEST(SharedBufferTest, writable_size_is_the_rest_of_the_block)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    SharedProbe buffer;
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    EXPECT_EQ(buffer.size_writable(), buffer.size_available() - sizeof(DATA));
}

// An array write refuses no data, no elements and a count past the limit.
TEST(SharedBufferTest, write_array_refuses_a_bad_request)
{
    constexpr uint32_t VALUE{ 1u };
    areg::SharedBuffer buffer;
    EXPECT_EQ(buffer.write_array<uint32_t>(nullptr, 1u), 0u);
    EXPECT_EQ(buffer.write_array(&VALUE, 0u), 0u);
    EXPECT_EQ(buffer.write_array(&VALUE, areg::MAX_BUF_LENGTH), 0u);
    EXPECT_EQ(buffer.write_array(&VALUE, 1u), sizeof(VALUE));
}

// A read view refuses no bytes and a range past the data, and returns what is in range.
TEST(SharedBufferTest, read_view_stays_inside_the_data)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u, 4u };
    SharedProbe buffer;
    ASSERT_EQ(buffer.write(DATA, sizeof(DATA)), sizeof(DATA));
    buffer.reset();
    EXPECT_EQ(buffer.read_view(0u), nullptr);
    EXPECT_EQ(buffer.read_view(sizeof(DATA) + 1u), nullptr);
    ASSERT_NE(buffer.read_view(3u), nullptr);

    SharedProbe other(buffer);
    other.set_size_used(1u);
    EXPECT_EQ(buffer.read_view(1u), nullptr);
}

// A length prefix that claims more than the rest of the source reads nothing.
TEST(SharedBufferTest, read_of_a_buffer_refuses_a_length_past_the_data)
{
    const uint32_t length{ 100u };
    areg::SharedBuffer source;
    ASSERT_EQ(source.write_pod(length), sizeof(length));
    source.reset();
    areg::SharedBuffer target;
    EXPECT_EQ(source.read(target), 0u);
    EXPECT_FALSE(target.is_valid());
    EXPECT_EQ(source.position(), 0u);
}

// A nested sequence read from a view stays inside the view.
TEST(SharedBufferTest, nested_read_from_a_view_stays_inside_the_view)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::SharedBuffer payload;
    ASSERT_EQ(payload.write(DATA, sizeof(DATA)), sizeof(DATA));

    areg::SharedBuffer middle;
    ASSERT_EQ(middle.write(payload), sizeof(DATA));

    areg::SharedBuffer outer;
    ASSERT_EQ(outer.write(middle), sizeof(uint32_t) + sizeof(DATA));
    outer.move_to_begin();

    areg::SharedBuffer view;
    ASSERT_EQ(outer.read(view), sizeof(uint32_t) + sizeof(DATA));
    ASSERT_TRUE(view.is_view());

    areg::SharedBuffer inner;
    ASSERT_EQ(view.read(inner), sizeof(DATA));
    EXPECT_EQ(inner.size_used(), sizeof(DATA));
    EXPECT_EQ(inner.buffer()[2], 3u);
}

// Every copy size goes through the read of a shared buffer.
TEST(SharedBufferTest, every_copy_size_writes_and_reads)
{
    uint8_t source[16]{ };
    for (uint32_t i = 0u; i < sizeof(source); ++i)
    {
        source[i] = static_cast<uint8_t>(0xB0u + i);
    }

    for (uint32_t size : { 1u, 2u, 3u, 4u, 8u, 16u })
    {
        areg::SharedBuffer buffer(4096u, areg::BLOCK_SIZE);
        ASSERT_EQ(buffer.write(source, size), size);
        buffer.reset();
        uint8_t target[16]{ };
        ASSERT_EQ(buffer.read(target, size), size);
        EXPECT_TRUE(areg::mem_equal(target, source, size)) << "size " << size;
    }
}
