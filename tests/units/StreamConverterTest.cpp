/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StreamConverterTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for ReadConverter and WriteConverter.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"

#include <cstring>
#include <limits>

namespace
{
    constexpr uint32_t MODE{ static_cast<uint32_t>(areg::FileBase::OpenMode::Write) | static_cast<uint32_t>(areg::FileBase::OpenMode::Binary) };

    //!< A memory stream with the converters bound to it.
    struct Converters
    {
        Converters( void )
            : mStream   ( MODE )
            , mWriter   ( static_cast<areg::OutStream &>(mStream), static_cast<areg::Cursor &>(mStream) )
            , mReader   ( static_cast<areg::InStream &>(mStream), static_cast<areg::Cursor &>(mStream) )
        {
            mStream.open( );
        }

        void rewind( void )
        {
            mStream.set_position( 0, areg::Cursor::SeekOrigin::Begin );
        }

        areg::FileBuffer        mStream;
        areg::WriteConverter    mWriter;
        areg::ReadConverter     mReader;
    };
}

/**
 * \brief   Every simple type is written with its own size and read back unchanged.
 **/
TEST( StreamConverterTest, simple_types_round_trip )
{
    Converters conv;
    EXPECT_TRUE( conv.mWriter.set_bool( true ) );
    EXPECT_TRUE( conv.mWriter.set_bool( false ) );
    EXPECT_TRUE( conv.mWriter.set_char( 'a' ) );
    EXPECT_TRUE( conv.mWriter.set_char( L'b' ) );
    EXPECT_TRUE( conv.mWriter.set_short( std::numeric_limits<int16_t>::min( ) ) );
    EXPECT_TRUE( conv.mWriter.set_int( std::numeric_limits<int32_t>::max( ) ) );
    EXPECT_TRUE( conv.mWriter.set_int64( std::numeric_limits<int64_t>::min( ) ) );
    EXPECT_TRUE( conv.mWriter.set_float( -0.125f ) );
    EXPECT_TRUE( conv.mWriter.set_double( 1.0e300 ) );
    const uint32_t expected{ static_cast<uint32_t>(2 * sizeof(bool) + sizeof(char) + sizeof(uint32_t) + sizeof(int16_t) + sizeof(int32_t) + sizeof(int64_t) + sizeof(float) + sizeof(double)) };
    EXPECT_EQ( conv.mStream.length( ), expected );

    conv.rewind( );
    bool b1{ false }, b2{ true };
    char c{ 0 };
    wchar_t w{ 0 };
    int16_t s{ 0 };
    int32_t i{ 0 };
    int64_t l{ 0 };
    float f{ 0 };
    double d{ 0 };
    EXPECT_TRUE( conv.mReader.as_bool( b1 ) );
    EXPECT_TRUE( conv.mReader.as_bool( b2 ) );
    EXPECT_TRUE( conv.mReader.as_char( c ) );
    EXPECT_TRUE( conv.mReader.as_char( w ) );
    EXPECT_TRUE( conv.mReader.as_short( s ) );
    EXPECT_TRUE( conv.mReader.as_int( i ) );
    EXPECT_TRUE( conv.mReader.int64( l ) );
    EXPECT_TRUE( conv.mReader.as_float( f ) );
    EXPECT_TRUE( conv.mReader.as_double( d ) );

    EXPECT_TRUE( b1 );
    EXPECT_FALSE( b2 );
    EXPECT_EQ( c, 'a' );
    EXPECT_EQ( w, L'b' );
    EXPECT_EQ( s, std::numeric_limits<int16_t>::min( ) );
    EXPECT_EQ( i, std::numeric_limits<int32_t>::max( ) );
    EXPECT_EQ( l, std::numeric_limits<int64_t>::min( ) );
    EXPECT_EQ( f, -0.125f );
    EXPECT_EQ( d, 1.0e300 );
    EXPECT_EQ( conv.mStream.position( ), expected );
}

/**
 * \brief   A read from an empty or exhausted stream fails and does not change the output.
 **/
TEST( StreamConverterTest, read_past_the_end_fails_without_change )
{
    Converters conv;
    bool b{ true };
    char c{ 'k' };
    wchar_t w{ L'k' };
    int16_t s{ 11 };
    int32_t i{ 12 };
    int64_t l{ 13 };
    float f{ 1.5f };
    double d{ 2.5 };
    EXPECT_FALSE( conv.mReader.as_bool( b ) );
    EXPECT_FALSE( conv.mReader.as_char( c ) );
    EXPECT_FALSE( conv.mReader.as_char( w ) );
    EXPECT_FALSE( conv.mReader.as_short( s ) );
    EXPECT_FALSE( conv.mReader.as_int( i ) );
    EXPECT_FALSE( conv.mReader.int64( l ) );
    EXPECT_FALSE( conv.mReader.as_float( f ) );
    EXPECT_FALSE( conv.mReader.as_double( d ) );
    EXPECT_TRUE( b );
    EXPECT_EQ( c, 'k' );
    EXPECT_EQ( w, L'k' );
    EXPECT_EQ( s, 11 );
    EXPECT_EQ( i, 12 );
    EXPECT_EQ( l, 13 );
    EXPECT_EQ( f, 1.5f );
    EXPECT_EQ( d, 2.5 );

    // fewer bytes than the type needs
    EXPECT_TRUE( conv.mWriter.set_short( 3 ) );
    conv.rewind( );
    EXPECT_FALSE( conv.mReader.as_int( i ) );
    EXPECT_FALSE( conv.mReader.as_double( d ) );
}

/**
 * \brief   A write to a stream that does not accept data fails.
 **/
TEST( StreamConverterTest, write_into_closed_stream_fails )
{
    areg::FileBuffer closed( MODE );
    areg::WriteConverter writer( static_cast<areg::OutStream &>(closed), static_cast<areg::Cursor &>(closed) );
    EXPECT_FALSE( writer.set_bool( true ) );
    EXPECT_FALSE( writer.set_char( 'a' ) );
    EXPECT_FALSE( writer.set_char( L'a' ) );
    EXPECT_FALSE( writer.set_short( 1 ) );
    EXPECT_FALSE( writer.set_int( 1 ) );
    EXPECT_FALSE( writer.set_int64( 1 ) );
    EXPECT_FALSE( writer.set_float( 1.f ) );
    EXPECT_FALSE( writer.set_double( 1. ) );
    EXPECT_FALSE( writer.set_string( "text" ) );
    EXPECT_FALSE( writer.append_string( "text" ) );
}

/**
 * \brief   set_string() writes the terminator, append_string() does not; null and empty text handled.
 **/
TEST( StreamConverterTest, set_and_append_string_sizes )
{
    Converters conv;
    EXPECT_TRUE( conv.mWriter.set_string( "abc" ) );
    EXPECT_EQ( conv.mStream.length( ), 4u );
    EXPECT_TRUE( conv.mWriter.append_string( "de" ) );
    EXPECT_EQ( conv.mStream.length( ), 6u );
    EXPECT_TRUE( conv.mWriter.set_string( areg::String( "f" ) ) );
    EXPECT_EQ( conv.mStream.length( ), 8u );

    EXPECT_TRUE( conv.mWriter.set_string( L"wd" ) );
    EXPECT_EQ( conv.mStream.length( ), 8u + 3u * 2u );
    EXPECT_TRUE( conv.mWriter.append_string( L"x" ) );
    EXPECT_EQ( conv.mStream.length( ), 8u + 4u * 2u );
    EXPECT_TRUE( conv.mWriter.set_string( areg::WideString( L"y" ) ) );
    EXPECT_EQ( conv.mStream.length( ), 8u + 6u * 2u );

    const uint32_t before{ static_cast<uint32_t>(conv.mStream.length( )) };
    const char * nullAscii{ nullptr };
    const wchar_t * nullWide{ nullptr };
    EXPECT_FALSE( conv.mWriter.set_string( nullAscii ) );
    EXPECT_FALSE( conv.mWriter.set_string( nullWide ) );
    EXPECT_FALSE( conv.mWriter.append_string( nullAscii ) );
    EXPECT_FALSE( conv.mWriter.append_string( nullWide ) );
    EXPECT_FALSE( conv.mWriter.append_string( "" ) );
    EXPECT_FALSE( conv.mWriter.append_string( L"" ) );
    EXPECT_EQ( conv.mStream.length( ), before );
}

/**
 * \brief   A string written with its terminator is read back up to the terminator, the next value follows.
 **/
TEST( StreamConverterTest, string_round_trip_with_next_value )
{
    Converters conv;
    ASSERT_TRUE( conv.mWriter.set_string( "narrow" ) );
    ASSERT_TRUE( conv.mWriter.set_string( L"wide" ) );
    ASSERT_TRUE( conv.mWriter.set_int( 99 ) );
    conv.rewind( );

    areg::String narrow;
    areg::WideString wide;
    int32_t value{ 0 };
    EXPECT_TRUE( conv.mReader.as_string( narrow ) );
    EXPECT_STREQ( narrow.as_string( ), "narrow" );
    EXPECT_TRUE( conv.mReader.as_string( wide ) );
    EXPECT_EQ( wide, areg::WideString( L"wide" ) );
    EXPECT_TRUE( conv.mReader.as_int( value ) );
    EXPECT_EQ( value, 99 );
}

/**
 * \brief   A string read from an empty stream fails; a string without terminator is read up to the end of data.
 **/
TEST( StreamConverterTest, string_read_without_terminator_and_from_empty )
{
    Converters conv;
    areg::String narrow( "old" );
    areg::WideString wide( L"old" );
    EXPECT_FALSE( conv.mReader.as_string( narrow ) );
    EXPECT_FALSE( conv.mReader.as_string( wide ) );
    EXPECT_TRUE( narrow.is_empty( ) );
    EXPECT_TRUE( wide.is_empty( ) );

    ASSERT_TRUE( conv.mWriter.append_string( "tail" ) );
    conv.rewind( );
    EXPECT_TRUE( conv.mReader.as_string( narrow ) );
    EXPECT_STREQ( narrow.as_string( ), "tail" );
    EXPECT_TRUE( conv.mStream.is_eof( ) );
}

/**
 * \brief   A terminator alone is read as an empty string and the read succeeds.
 **/
TEST( StreamConverterTest, string_of_terminator_only_is_empty )
{
    Converters conv;
    ASSERT_TRUE( conv.mWriter.set_string( "" ) );
    ASSERT_TRUE( conv.mWriter.set_string( L"" ) );
    conv.rewind( );

    areg::String narrow( "x" );
    areg::WideString wide( L"x" );
    EXPECT_TRUE( conv.mReader.as_string( narrow ) );
    EXPECT_TRUE( narrow.is_empty( ) );
    EXPECT_TRUE( conv.mReader.as_string( wide ) );
    EXPECT_TRUE( wide.is_empty( ) );
}

/**
 * \brief   Lines ended by LF and the end of data are read one by one.
 **/
TEST( StreamConverterTest, read_line_narrow )
{
    Converters conv;
    ASSERT_TRUE( conv.mWriter.append_string( "one\ntwo\nthree" ) );
    conv.rewind( );

    areg::String line;
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_STREQ( line.as_string( ), "one" );
    line.clear( );
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_STREQ( line.as_string( ), "two" );
    line.clear( );
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_STREQ( line.as_string( ), "three" );
    line.clear( );
    EXPECT_FALSE( conv.mReader.read_line( line ) );
    EXPECT_TRUE( line.is_empty( ) );
}

/**
 * \brief   A line ended by CR and LF is one line.
 **/
TEST( StreamConverterTest, read_line_dos_line_end_is_one_line )
{
    Converters conv;
    ASSERT_TRUE( conv.mWriter.append_string( "one\r\ntwo\r\nthree" ) );
    conv.rewind( );

    areg::String line;
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_STREQ( line.as_string( ), "one" );
    line.clear( );
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_STREQ( line.as_string( ), "two" );
    line.clear( );
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_STREQ( line.as_string( ), "three" );
    line.clear( );
    EXPECT_FALSE( conv.mReader.read_line( line ) );
}

/**
 * \brief   Empty lines are returned as empty strings and the next line is not lost.
 **/
TEST( StreamConverterTest, read_line_keeps_empty_lines )
{
    Converters conv;
    ASSERT_TRUE( conv.mWriter.append_string( "a\n\nb\n" ) );
    conv.rewind( );

    areg::String line;
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_STREQ( line.as_string( ), "a" );
    line.clear( );
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_TRUE( line.is_empty( ) );
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_STREQ( line.as_string( ), "b" );
}

/**
 * \brief   Wide lines are read the same way as narrow lines.
 **/
TEST( StreamConverterTest, read_line_wide )
{
    Converters conv;
    ASSERT_TRUE( conv.mWriter.append_string( L"first\nsecond\nlast" ) );
    conv.rewind( );

    areg::WideString line;
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_EQ( line, areg::WideString( L"first" ) );
    line.clear( );
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_EQ( line, areg::WideString( L"second" ) );
    line.clear( );
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_EQ( line, areg::WideString( L"last" ) );
    line.clear( );
    EXPECT_FALSE( conv.mReader.read_line( line ) );
}

/**
 * \brief   A wide line ended by CR and LF is one line.
 **/
TEST( StreamConverterTest, read_line_wide_dos_line_end_is_one_line )
{
    Converters conv;
    ASSERT_TRUE( conv.mWriter.append_string( L"first\r\nlast" ) );
    conv.rewind( );

    areg::WideString line;
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_EQ( line, areg::WideString( L"first" ) );
    line.clear( );
    EXPECT_TRUE( conv.mReader.read_line( line ) );
    EXPECT_EQ( line, areg::WideString( L"last" ) );
}
