/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StreamPrimitivesTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of streaming the primitive types and the standard
 *              containers through a memory stream, including a count that exceeds the data.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/IOStream.hpp"

#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace
{
    constexpr uint32_t MODE{ static_cast<uint32_t>(areg::FileBase::OpenMode::Write) | static_cast<uint32_t>(areg::FileBase::OpenMode::Binary) };

    //!< A memory stream that is opened and rewound for reading.
    struct Memory
    {
        Memory( void )
            : mFile( MODE )
        {
            mFile.open( );
        }

        areg::OutStream & out( void )
        {
            return mFile;
        }

        const areg::InStream & in( void )
        {
            mFile.set_position( 0, areg::Cursor::SeekOrigin::Begin );
            return mFile;
        }

        areg::FileBuffer mFile;
    };
}

/**
 * \brief   A character, a 64-bit integer, a float and a double are read back as written.
 **/
TEST( StreamPrimitivesTest, character_wide_integer_and_real_values_round_trip )
{
    Memory memory;
    memory.out() << 'z' << std::numeric_limits<int64_t>::min() << -0.25f << 1.0e200;

    char ch{ };
    int64_t big{ };
    float real{ };
    double wide{ };
    memory.in() >> ch >> big >> real >> wide;
    EXPECT_EQ( ch, 'z' );
    EXPECT_EQ( big, std::numeric_limits<int64_t>::min() );
    EXPECT_EQ( real, -0.25f );
    EXPECT_EQ( wide, 1.0e200 );
}

/**
 * \brief   A vector is read back with the same elements in the same order.
 **/
TEST( StreamPrimitivesTest, vector_round_trips_in_order )
{
    Memory memory;
    const std::vector<int32_t> source{ 3, -1, 7, 0 };
    memory.out() << source;

    std::vector<int32_t> target{ 99 };
    memory.in() >> target;
    EXPECT_EQ( target, source );
}

/**
 * \brief   A vector count larger than the bytes left is cut to the bytes left, so a damaged stream does not allocate the count.
 **/
TEST( StreamPrimitivesTest, vector_count_larger_than_the_data_is_bounded_by_the_bytes_left )
{
    Memory memory;
    memory.out() << static_cast<uint32_t>(0x7FFFFFFFu) << static_cast<uint8_t>(1) << static_cast<uint8_t>(2);

    std::vector<uint8_t> target;
    memory.in() >> target;
    EXPECT_LE( target.size(), 2u );
}

/**
 * \brief   A map is read back with the same pairs; a count larger than the data is bounded by the bytes left.
 **/
TEST( StreamPrimitivesTest, map_round_trips_and_a_damaged_count_is_bounded )
{
    Memory memory;
    std::map<uint8_t, uint8_t> source;
    source[1] = 10;
    source[2] = 20;
    memory.out() << source;

    std::map<uint8_t, uint8_t> target;
    target[9] = 9;
    memory.in() >> target;
    EXPECT_EQ( target, source );

    Memory damaged;
    damaged.out() << static_cast<uint32_t>(0x7FFFFFFFu) << static_cast<uint8_t>(5) << static_cast<uint8_t>(6);
    std::map<uint8_t, uint8_t> bounded;
    damaged.in() >> bounded;
    EXPECT_LE( bounded.size(), 2u );
}

/**
 * \brief   A null text and an empty text are written as a count of zero and read back empty.
 **/
TEST( StreamPrimitivesTest, null_text_is_written_as_an_empty_text )
{
    Memory memory;
    memory.out() << static_cast<const wchar_t *>(nullptr);
    EXPECT_EQ( memory.mFile.length(), sizeof( uint32_t ) );
}

/**
 * \brief   Bytes left to read bound a count; a count within the data is returned unchanged.
 **/
TEST( StreamPrimitivesTest, bounded_count_returns_the_smaller_of_the_count_and_the_bytes_left )
{
    Memory memory;
    memory.out() << static_cast<uint32_t>(1) << static_cast<uint32_t>(2);

    const areg::InStream & in = memory.in();
    EXPECT_EQ( in.bounded_count( 3u ), 3u );
    EXPECT_EQ( in.bounded_count( 100u ), 8u );
}
