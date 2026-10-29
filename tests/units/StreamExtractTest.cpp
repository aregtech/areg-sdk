/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StreamExtractTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the stream extractors of strings and containers.
 *              Covers: well-formed round trips, data that ends early, element counts larger
 *              than the data that follows them.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/FixedArray.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/RingStack.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/SortedLinkedList.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <cstring>
#include <deque>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace
{
    //!< An element count far larger than any data that follows it.
    constexpr uint32_t LARGE_COUNT{ 0x40000000u };

    //!< Writes a count and then two uint32_t values, and moves the cursor to the start.
    void count_then_two(areg::SharedBuffer & buf, uint32_t count)
    {
        buf << count << 7u << 9u;
        buf.move_to_begin();
    }
}

//////////////////////////////////////////////////////////////////////////
// 1. std::basic_string
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Strings written by the inserter are read back unchanged, followed by the next field.
 **/
TEST(StreamExtractTest, string_round_trip)
{
    areg::SharedBuffer buf;
    buf << std::string("hello") << std::string() << std::wstring(L"wide") << 42u;
    buf.move_to_begin();

    std::string first{ "x" };
    std::string second{ "y" };
    std::wstring third;
    uint32_t last{ 0u };
    buf >> first >> second >> third >> last;

    EXPECT_EQ(first, "hello");
    EXPECT_TRUE(second.empty());
    EXPECT_EQ(third, L"wide");
    EXPECT_EQ(last, 42u);
}

/**
 * \brief   A string with no terminator ends at the end of the data.
 **/
TEST(StreamExtractTest, string_without_terminator)
{
    constexpr uint8_t data[]{ 'a', 'b', 'c' };
    areg::MessageEnvelope msg;
    msg.init_envelope(areg::EventHeader{ }, 16u);
    msg.write(data, static_cast<uint32_t>(std::size(data)));
    msg.move_to_begin();

    std::string value;
    msg >> value;

    EXPECT_EQ(value, "abc");
}

/**
 * \brief   No data gives an empty string, and a wide string with an odd byte count ends early.
 **/
TEST(StreamExtractTest, string_from_short_data)
{
    areg::SharedBuffer empty;
    std::string value{ "old" };
    empty >> value;
    EXPECT_TRUE(value.empty());

    constexpr uint8_t odd[]{ 0x41 };
    areg::SharedBuffer buf(odd, 1u);
    buf.move_to_begin();
    std::wstring wide{ L"old" };
    buf >> wide;
    EXPECT_TRUE(wide.empty());
}

//////////////////////////////////////////////////////////////////////////
// 2. std:: containers
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Sequence containers are read back unchanged.
 **/
TEST(StreamExtractTest, sequence_round_trip)
{
    const std::vector<uint32_t> vec{ 1u, 2u, 3u };
    const std::deque<std::string> deq{ "a", "", "c" };
    const std::list<uint16_t> lst{ 5u, 6u };

    areg::SharedBuffer buf;
    buf << vec << deq << lst;
    buf.move_to_begin();

    std::vector<uint32_t> vecIn;
    std::deque<std::string> deqIn;
    std::list<uint16_t> lstIn;
    buf >> vecIn >> deqIn >> lstIn;

    EXPECT_EQ(vecIn, vec);
    EXPECT_EQ(deqIn, deq);
    EXPECT_EQ(lstIn, lst);
}

/**
 * \brief   A count larger than the data reads no more elements than there are bytes left.
 **/
TEST(StreamExtractTest, sequence_count_beyond_data)
{
    areg::SharedBuffer vecBuf;
    count_then_two(vecBuf, LARGE_COUNT);
    std::vector<uint32_t> vec;
    vecBuf >> vec;
    ASSERT_LE(vec.size(), 8u);
    EXPECT_EQ(vec[0], 7u);
    EXPECT_EQ(vec[1], 9u);

    areg::SharedBuffer deqBuf;
    count_then_two(deqBuf, LARGE_COUNT);
    std::deque<uint32_t> deq;
    deqBuf >> deq;
    EXPECT_LE(deq.size(), 8u);

    areg::SharedBuffer lstBuf;
    count_then_two(lstBuf, LARGE_COUNT);
    std::list<std::string> lst;
    lstBuf >> lst;
    EXPECT_LE(lst.size(), 8u);
}

/**
 * \brief   Maps are read back unchanged, and a count larger than the data is bounded.
 **/
TEST(StreamExtractTest, map_round_trip_and_count_beyond_data)
{
    const std::map<uint32_t, std::string> ordered{ { 1u, "one" }, { 2u, "two" } };
    const std::unordered_map<std::string, uint32_t> hashed{ { "x", 10u } };

    areg::SharedBuffer buf;
    buf << ordered << hashed;
    buf.move_to_begin();
    std::map<uint32_t, std::string> orderedIn;
    std::unordered_map<std::string, uint32_t> hashedIn;
    buf >> orderedIn >> hashedIn;
    EXPECT_EQ(orderedIn, ordered);
    EXPECT_EQ(hashedIn, hashed);

    areg::SharedBuffer mapBuf;
    count_then_two(mapBuf, LARGE_COUNT);
    std::map<uint32_t, uint32_t> mapIn;
    mapBuf >> mapIn;
    EXPECT_LE(mapIn.size(), 8u);

    areg::SharedBuffer hashBuf;
    count_then_two(hashBuf, LARGE_COUNT);
    std::unordered_map<uint32_t, uint32_t> hashIn;
    hashBuf >> hashIn;
    EXPECT_LE(hashIn.size(), 8u);
}

//////////////////////////////////////////////////////////////////////////
// 3. areg containers
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   FixedArray, RingStack and SortedLinkedList bound a count larger than the data.
 **/
TEST(StreamExtractTest, areg_containers_count_beyond_data)
{
    areg::SharedBuffer arrBuf;
    count_then_two(arrBuf, LARGE_COUNT);
    areg::FixedArray<uint32_t> arr;
    arrBuf >> arr;
    EXPECT_LE(arr.size(), 8u);

    areg::SharedBuffer ringBuf;
    count_then_two(ringBuf, LARGE_COUNT);
    areg::ConcurrentRingStack<uint32_t> ring(0, areg::OverlapPolicy::Shift);
    ringBuf >> ring;
    EXPECT_LE(ring.size(), 8u);

    areg::SharedBuffer sortBuf;
    sortBuf << LARGE_COUNT << static_cast<uint8_t>(0u) << 7u << 9u;
    sortBuf.move_to_begin();
    areg::SortedLinkedList<uint32_t> sorted;
    sortBuf >> sorted;
    EXPECT_LE(sorted.size(), 8u);
}

//////////////////////////////////////////////////////////////////////////
// 4. LogEntry
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A log entry whose length exceeds its text buffer is written and read within that buffer.
 **/
TEST(StreamExtractTest, log_entry_length_beyond_buffer)
{
    areg::LogEntry out;
    std::memset(out.logMessage, 'm', areg::LOG_MSG_SIZE - 1u);
    out.logMessage[areg::LOG_MSG_SIZE - 1u] = '\0';
    out.logMessageLen = 100000u;

    areg::SharedBuffer buf;
    buf << out;
    EXPECT_LE(buf.size_used(), static_cast<uint32_t>(sizeof(areg::LogEntry)));
    buf.move_to_begin();

    areg::LogEntry in;
    buf >> in;
    EXPECT_EQ(in.logMessageLen, 100000u);
    EXPECT_EQ(in.logMessage[areg::LOG_MSG_SIZE - 1u], '\0');
    EXPECT_EQ(std::strlen(in.logMessage), static_cast<size_t>(areg::LOG_MSG_SIZE - 1u));
}

namespace
{
    constexpr uint32_t  PAGE_BYTES      { 4096u };  //!< The size of a memory page.
    constexpr uint32_t  BEFORE_PAGE_END { 23u };    //!< The data starts this many bytes before a page end, at an odd address.
    constexpr uint32_t  MARKER          { 42u };    //!< The field written after the string.

    /**
     * \brief   Reserves the buffer and returns the number of bytes to write first, so that the next byte
     *          sits BEFORE_PAGE_END bytes before a page end. The address is odd, so no wchar_t is aligned there.
     **/
    uint32_t prefix_before_page_end(areg::SharedBuffer & buf)
    {
        buf.reserve(3u * PAGE_BYTES, false);
        const uintptr_t base{ reinterpret_cast<uintptr_t>(buf.buffer()) };
        uintptr_t pageEnd{ (base + PAGE_BYTES) & ~static_cast<uintptr_t>(PAGE_BYTES - 1u) };
        if ((pageEnd - base) < BEFORE_PAGE_END)
        {
            pageEnd += PAGE_BYTES;
        }

        return static_cast<uint32_t>(pageEnd - base) - BEFORE_PAGE_END;
    }

    //!< Writes \a count filler bytes.
    void write_prefix(areg::SharedBuffer & buf, uint32_t count)
    {
        const uint8_t fill{ 0x5Au };
        for (uint32_t i = 0u; i < count; ++i)
        {
            buf.write(&fill, 1u);
        }
    }

    //!< Moves the read position to the data written after \a prefix filler bytes.
    void skip_prefix(areg::SharedBuffer & buf, uint32_t prefix)
    {
        buf.move_to_begin();
        buf.set_position(static_cast<int32_t>(prefix), areg::Cursor::SeekOrigin::Begin);
    }

    //!< Returns true if the data written after \a prefix filler bytes starts at an odd address before a page end.
    bool is_odd_before_page_end(const areg::SharedBuffer & buf, uint32_t prefix)
    {
        const uintptr_t at{ reinterpret_cast<uintptr_t>(buf.buffer()) + prefix };
        return ((at % PAGE_BYTES) == (PAGE_BYTES - BEFORE_PAGE_END)) && ((at % alignof(wchar_t)) != 0u);
    }
}

/**
 * \brief   A std::wstring at an odd address just before a page end is read back whole, followed by the next field.
 **/
TEST(StreamExtractTest, std_wide_string_at_odd_address_before_page_end)
{
    areg::SharedBuffer buf;
    const uint32_t prefix{ prefix_before_page_end(buf) };
    write_prefix(buf, prefix);
    buf << std::wstring(L"wide") << MARKER;
    ASSERT_TRUE(is_odd_before_page_end(buf, prefix));

    skip_prefix(buf, prefix);
    std::wstring text;
    uint32_t last{ 0u };
    buf >> text >> last;

    EXPECT_EQ(text, L"wide");
    EXPECT_EQ(last, MARKER);
}

/**
 * \brief   A WideString at an odd address just before a page end is read back whole, followed by the next field.
 **/
TEST(StreamExtractTest, areg_wide_string_at_odd_address_before_page_end)
{
    areg::SharedBuffer buf;
    const uint32_t prefix{ prefix_before_page_end(buf) };
    write_prefix(buf, prefix);
    buf.write(areg::WideString(L"wide"));
    buf << MARKER;
    ASSERT_TRUE(is_odd_before_page_end(buf, prefix));

    skip_prefix(buf, prefix);
    areg::WideString text;
    uint32_t last{ 0u };
    buf.read(text);
    buf >> last;

    EXPECT_EQ(text.data(), std::wstring(L"wide"));
    EXPECT_EQ(last, MARKER);
}

/**
 * \brief   A length-prefixed wide string at an odd address just before a page end is read back whole.
 **/
TEST(StreamExtractTest, binary_wide_string_at_odd_address_before_page_end)
{
    areg::SharedBuffer buf;
    const uint32_t prefix{ prefix_before_page_end(buf) };
    write_prefix(buf, prefix);
    buf.write_string_bin(areg::WideString(L"wide"));
    buf << MARKER;
    ASSERT_TRUE(is_odd_before_page_end(buf, prefix));

    skip_prefix(buf, prefix);
    areg::WideString text;
    uint32_t last{ 0u };
    EXPECT_NE(buf.read_string_bin(text), 0u);
    buf >> last;

    EXPECT_EQ(text.data(), std::wstring(L"wide"));
    EXPECT_EQ(last, MARKER);
}

/**
 * \brief   Wide strings start at every byte offset and are read back whole by each of the three readers.
 **/
TEST(StreamExtractTest, wide_strings_at_every_byte_offset)
{
    const std::wstring longText{ L"abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
    for (uint32_t offset = 0u; offset < 8u; ++offset)
    {
        areg::SharedBuffer buf;
        write_prefix(buf, offset);
        buf << longText;
        buf.write(areg::WideString(L"wide"));
        buf.write_string_bin(areg::WideString(L"binary"));
        buf << std::wstring() << MARKER;

        skip_prefix(buf, offset);
        std::wstring first;
        areg::WideString second;
        areg::WideString third;
        std::wstring empty{ L"x" };
        uint32_t last{ 0u };
        buf >> first;
        buf.read(second);
        EXPECT_NE(buf.read_string_bin(third), 0u) << "offset " << offset;
        buf >> empty >> last;

        EXPECT_EQ(first, longText) << "offset " << offset;
        EXPECT_EQ(second.data(), std::wstring(L"wide")) << "offset " << offset;
        EXPECT_EQ(third.data(), std::wstring(L"binary")) << "offset " << offset;
        EXPECT_TRUE(empty.empty()) << "offset " << offset;
        EXPECT_EQ(last, MARKER) << "offset " << offset;
    }
}
