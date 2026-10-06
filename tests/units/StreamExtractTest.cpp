/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
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
