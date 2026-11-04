/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StringStreamTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the stream string format: a uint32_t count of code
 *              units, then the units; narrow text as bytes, wide text as UTF-16LE. The expected
 *              bytes are the same on every platform.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"

#include <cwchar>
#include <string>
#include <string_view>
#include <vector>

namespace
{
    //!< Returns the bytes written to the buffer.
    std::vector<uint8_t> bytes_of( const areg::SharedBuffer & buf )
    {
        return std::vector<uint8_t>( buf.buffer( ), buf.buffer( ) + buf.size_used( ) );
    }

    //!< Returns a wide string with U+1F600 in the platform's wchar_t form.
    std::wstring with_supplementary( )
    {
        std::wstring result{ L"A" };
        if constexpr ( sizeof( wchar_t ) == 4u )
        {
            result += static_cast<wchar_t>(0x1F600);
        }
        else
        {
            result += static_cast<wchar_t>(0xD83D);
            result += static_cast<wchar_t>(0xDE00);
        }

        return result;
    }
}

/**
 * \brief   A String, a WideString and the next field have the same bytes on every platform.
 **/
TEST( StringStreamTest, bytes_are_platform_independent )
{
    areg::SharedBuffer buf;
    buf << areg::String( "hi" ) << areg::WideString( L"hi" ) << int32_t{ 42 };

    const std::vector<uint8_t> expected{ 2, 0, 0, 0, 'h', 'i', 2, 0, 0, 0, 'h', 0, 'i', 0, 42, 0, 0, 0 };
    EXPECT_EQ( bytes_of( buf ), expected );

    buf.move_to_begin( );
    areg::String narrow;
    areg::WideString wide;
    int32_t value{ 0 };
    buf >> narrow >> wide >> value;
    EXPECT_EQ( narrow, "hi" );
    EXPECT_EQ( wide, L"hi" );
    EXPECT_EQ( value, 42 );
}

/**
 * \brief   Every way to write the same text gives the same bytes.
 **/
TEST( StringStreamTest, every_writer_gives_the_same_bytes )
{
    areg::SharedBuffer byString, byStd, byView, byPointer, byMember;
    byString << areg::String( "abc" );
    byStd << std::string( "abc" );
    byView << std::string_view( "abc" );
    byPointer << "abc";
    byMember.write( areg::String( "abc" ) );
    EXPECT_EQ( bytes_of( byString ), bytes_of( byStd ) );
    EXPECT_EQ( bytes_of( byString ), bytes_of( byView ) );
    EXPECT_EQ( bytes_of( byString ), bytes_of( byPointer ) );
    EXPECT_EQ( bytes_of( byString ), bytes_of( byMember ) );

    areg::SharedBuffer wideString, wideStd, wideView, widePointer, wideMember;
    wideString << areg::WideString( L"abc" );
    wideStd << std::wstring( L"abc" );
    wideView << std::wstring_view( L"abc" );
    widePointer << L"abc";
    wideMember.write( areg::WideString( L"abc" ) );
    EXPECT_EQ( bytes_of( wideString ), bytes_of( wideStd ) );
    EXPECT_EQ( bytes_of( wideString ), bytes_of( wideView ) );
    EXPECT_EQ( bytes_of( wideString ), bytes_of( widePointer ) );
    EXPECT_EQ( bytes_of( wideString ), bytes_of( wideMember ) );
}

/**
 * \brief   An empty string, a null pointer and a string with an embedded NUL are read back unchanged.
 **/
TEST( StringStreamTest, empty_null_and_embedded_nul )
{
    areg::SharedBuffer buf;
    const char * none{ nullptr };
    buf << areg::String( ) << none << std::string( "a\0b", 3u ) << int32_t{ 7 };
    EXPECT_EQ( buf.size_used( ), 4u + 4u + (4u + 3u) + 4u );

    buf.move_to_begin( );
    areg::String first{ "x" };
    areg::String second{ "x" };
    std::string third;
    int32_t value{ 0 };
    buf >> first >> second >> third >> value;
    EXPECT_TRUE( first.is_empty( ) );
    EXPECT_TRUE( second.is_empty( ) );
    EXPECT_EQ( third, std::string( "a\0b", 3u ) );
    EXPECT_EQ( value, 7 );
}

/**
 * \brief   A character above U+FFFF is a surrogate pair on the stream, and lone surrogates pass unchanged.
 **/
TEST( StringStreamTest, surrogates_on_the_stream )
{
    std::wstring source{ with_supplementary( ) };
    source += static_cast<wchar_t>(0xD800);
    source += L'B';
    source += static_cast<wchar_t>(0xDC00);

    areg::SharedBuffer buf;
    buf << source << int32_t{ 9 };
    const std::vector<uint8_t> expected{ 6, 0, 0, 0, 'A', 0, 0x3D, 0xD8, 0x00, 0xDE, 0x00, 0xD8, 'B', 0, 0x00, 0xDC, 9, 0, 0, 0 };
    EXPECT_EQ( bytes_of( buf ), expected );

    buf.move_to_begin( );
    std::wstring back;
    int32_t value{ 0 };
    buf >> back >> value;
    EXPECT_EQ( back, source );
    EXPECT_EQ( value, 9 );
}

#if WCHAR_MAX > 0xFFFF
/**
 * \brief   A 4-byte wchar_t above U+10FFFF is written as U+FFFD.
 **/
TEST( StringStreamTest, invalid_code_point_is_replaced )
{
    const wchar_t source[ ]{ static_cast<wchar_t>(0x110000), L'z' };
    areg::SharedBuffer buf;
    buf << std::wstring( source, 2u );
    const std::vector<uint8_t> expected{ 2, 0, 0, 0, 0xFD, 0xFF, 'z', 0 };
    EXPECT_EQ( bytes_of( buf ), expected );
}
#endif // WCHAR_MAX > 0xFFFF

/**
 * \brief   A surrogate pair that crosses the internal conversion chunk and a long text are read back whole.
 **/
TEST( StringStreamTest, long_wide_text_with_pair_across_chunks )
{
    for ( size_t before : { size_t{ 126 }, size_t{ 127 }, size_t{ 128 } } )
    {
        std::wstring source( before, L'x' );
        source += with_supplementary( );
        source += std::wstring( 300u, L'y' );

        areg::SharedBuffer buf;
        buf << source;
        buf.move_to_begin( );
        std::wstring back;
        buf >> back;
        EXPECT_EQ( back, source ) << "pair after " << before << " characters";
    }
}

/**
 * \brief   A count larger than the data gives an empty string and consumes the rest of the data.
 **/
TEST( StringStreamTest, count_beyond_data_consumes_the_rest )
{
    const uint8_t raw[ ]{ 0xFF, 0xFF, 0, 0, 'a', 'b', 'c' };
    areg::SharedBuffer buf( raw, static_cast<uint32_t>(sizeof( raw )) );
    buf.move_to_begin( );
    areg::String text{ "x" };
    EXPECT_EQ( buf.read( text ), static_cast<uint32_t>(sizeof( raw )) );
    EXPECT_TRUE( text.is_empty( ) );
    EXPECT_TRUE( buf.is_end( ) );

    const uint8_t oddWide[ ]{ 2, 0, 0, 0, 'a', 0, 'b' };
    areg::SharedBuffer wideBuf( oddWide, static_cast<uint32_t>(sizeof( oddWide )) );
    wideBuf.move_to_begin( );
    areg::WideString wide{ L"x" };
    wideBuf >> wide;
    EXPECT_TRUE( wide.is_empty( ) );
    EXPECT_TRUE( wideBuf.is_end( ) );

    const uint8_t shortHead[ ]{ 3, 0 };
    areg::SharedBuffer headBuf( shortHead, static_cast<uint32_t>(sizeof( shortHead )) );
    headBuf.move_to_begin( );
    std::string head{ "x" };
    headBuf >> head;
    EXPECT_TRUE( head.empty( ) );
}

/**
 * \brief   A single wchar_t is 4 bytes on the stream on every platform.
 **/
TEST( StringStreamTest, single_wide_char_is_four_bytes )
{
    areg::SharedBuffer buf;
    buf << L'Z';
    ASSERT_EQ( buf.size_used( ), 4u );
    EXPECT_EQ( bytes_of( buf ), (std::vector<uint8_t>{ 'Z', 0, 0, 0 }) );

    buf.move_to_begin( );
    wchar_t ch{ 0 };
    buf >> ch;
    EXPECT_EQ( ch, L'Z' );

    const uint8_t big[ ]{ 0x00, 0xF6, 0x01, 0x00 };
    areg::SharedBuffer bigBuf( big, static_cast<uint32_t>(sizeof( big )) );
    bigBuf.move_to_begin( );
    bigBuf >> ch;
    EXPECT_EQ( static_cast<uint32_t>(ch), sizeof( wchar_t ) == 4u ? 0x1F600u : 0xFFFDu );
}

/**
 * \brief   The text constructor and a file buffer use the stream string format.
 **/
TEST( StringStreamTest, text_constructor_and_file_buffer )
{
    areg::SharedBuffer text( "hey" );
    EXPECT_EQ( bytes_of( text ), (std::vector<uint8_t>{ 3, 0, 0, 0, 'h', 'e', 'y' }) );
    text.move_to_begin( );
    areg::String narrow;
    text >> narrow;
    EXPECT_EQ( narrow, "hey" );

    areg::SharedBuffer wideText( L"ok" );
    EXPECT_EQ( bytes_of( wideText ), (std::vector<uint8_t>{ 2, 0, 0, 0, 'o', 0, 'k', 0 }) );

    areg::FileBuffer file;
    ASSERT_TRUE( file.open( ) );
    file << areg::String( "file" ) << areg::WideString( L"wide" );
    ASSERT_TRUE( file.move_to_begin( ) );
    areg::String fileNarrow;
    areg::WideString fileWide;
    file >> fileNarrow >> fileWide;
    EXPECT_EQ( fileNarrow, "file" );
    EXPECT_EQ( fileWide, L"wide" );
}

/**
 * \brief   The required size is the count and the text; for wide text it is an upper bound.
 **/
TEST( StringStreamTest, required_size_covers_the_stream_size )
{
    const areg::String narrow( std::string( 5000u, 'q' ).c_str( ) );
    const areg::WideString wide( std::wstring( 5000u, L'q' ).c_str( ) );
    areg::SharedBuffer buf;
    buf << narrow;
    EXPECT_EQ( buf.size_used( ), areg::required_size<areg::String>{ }(narrow) );
    buf << wide;
    EXPECT_EQ( buf.size_used( ), 4u + 5000u + 4u + 10000u );
    EXPECT_GE( areg::required_size<areg::WideString>{ }(wide), 4u + 10000u );

    buf.move_to_begin( );
    areg::String narrowBack;
    areg::WideString wideBack;
    buf >> narrowBack >> wideBack;
    EXPECT_EQ( narrowBack, narrow );
    EXPECT_EQ( wideBack, wide );
}
