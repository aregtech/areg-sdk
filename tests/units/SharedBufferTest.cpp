/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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

#include <cstring>

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
 * \brief   String constructor includes NUL terminator in size_used().
 **/
TEST(SharedBufferTest, string_construction)
{
    const char* text  = "hello";
    areg::SharedBuffer buf(text);
    EXPECT_TRUE(buf.is_valid());
    EXPECT_FALSE(buf.is_empty());
    EXPECT_EQ(buf.size_used(), static_cast<uint32_t>(std::strlen(text) + 1u));
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

    // Views are read-only per spec §7 Option A. Write returns 0 and the view is unchanged.
    EXPECT_TRUE(view.is_view());
    const uint32_t used_before = view.size_used();
    view.move_to_begin();
    const uint32_t written = view.write(reinterpret_cast<const uint8_t*>(&VAL_C), sizeof(VAL_C));
    EXPECT_EQ(written, 0u);
    EXPECT_EQ(view.size_used(), used_before);

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
