/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StringUnicodeTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of strings with non-ASCII text: UTF-8 in String,
 *              Cyrillic, CJK and emoji in WideString, in memory, on a stream and in a file.
 *              The file is UTF-8 with a byte order mark, so that every compiler reads the
 *              literals the same way; narrow literals are written as UTF-8 byte escapes.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"

#include <cstring>
#include <string>
#include <vector>

namespace
{
    //!< "Привет, 世界 😀" in UTF-8.
    const uint8_t UTF8_TEXT[ ]{ 0xD0, 0x9F, 0xD1, 0x80, 0xD0, 0xB8, 0xD0, 0xB2, 0xD0, 0xB5, 0xD1, 0x82, ',', ' ',
                                0xE4, 0xB8, 0x96, 0xE7, 0x95, 0x8C, ' ', 0xF0, 0x9F, 0x98, 0x80 };

    //!< "Привет, 世界" in UTF-16LE.
    const uint8_t UTF16_TEXT[ ]{ 0x1F, 0x04, 0x40, 0x04, 0x38, 0x04, 0x32, 0x04, 0x35, 0x04, 0x42, 0x04,
                                 ',', 0x00, ' ', 0x00, 0x16, 0x4E, 0x4C, 0x75 };

    //!< Returns the bytes written to the buffer.
    std::vector<uint8_t> bytes_of( const areg::SharedBuffer & buf )
    {
        return std::vector<uint8_t>( buf.buffer( ), buf.buffer( ) + buf.size_used( ) );
    }

    //!< Returns the 4-byte little-endian count followed by the given bytes.
    std::vector<uint8_t> counted( uint32_t count, const uint8_t * data, size_t size )
    {
        std::vector<uint8_t> result{ static_cast<uint8_t>(count), static_cast<uint8_t>(count >> 8),
                                     static_cast<uint8_t>(count >> 16), static_cast<uint8_t>(count >> 24) };
        result.insert( result.end( ), data, data + size );
        return result;
    }
}

/**
 * \brief   A UTF-8 literal in String keeps its bytes; the length counts bytes.
 **/
TEST( StringUnicodeTest, utf8_string_keeps_its_bytes )
{
    const areg::String text( "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82, \xE4\xB8\x96\xE7\x95\x8C \xF0\x9F\x98\x80" );    // Привет, 世界 😀
    ASSERT_EQ( text.length( ), static_cast<areg::CharCount>(sizeof( UTF8_TEXT )) );
    EXPECT_EQ( std::memcmp( text.as_string( ), UTF8_TEXT, sizeof( UTF8_TEXT ) ), 0 );

    const areg::String check( "\xE2\x9C\x93" );    // ✓
    areg::String copy( text );
    copy += " \xE2\x9C\x93";    //  ✓
    EXPECT_TRUE( copy.starts_with( text ) );
    EXPECT_TRUE( copy.ends_with( check ) );
    EXPECT_EQ( copy.length( ), text.length( ) + 4 );
}

/**
 * \brief   Search and comparison of UTF-8 text work on bytes.
 **/
TEST( StringUnicodeTest, utf8_search_and_compare )
{
    const areg::String text( "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82, \xE4\xB8\x96\xE7\x95\x8C \xF0\x9F\x98\x80" );    // Привет, 世界 😀
    const char * const world{ "\xE4\xB8\x96\xE7\x95\x8C" };    // 世界
    const char * const smile{ "\xF0\x9F\x98\x80" };    // 😀
    const char * const peace{ "\xD0\xBC\xD0\xB8\xD1\x80" };    // мир
    const char * const same{ "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82, \xE4\xB8\x96\xE7\x95\x8C \xF0\x9F\x98\x80" };    // Привет, 世界 😀
    const char * const shorter{ "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82, \xE4\xB8\x96\xE7\x95\x8C" };    // Привет, 世界
    const areg::String hello( "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82" );    // Привет
    EXPECT_EQ( text.find_first( world ), 14 );
    EXPECT_EQ( text.find_first( smile ), 21 );
    EXPECT_EQ( text.find_first( peace ), areg::END_POS );
    EXPECT_EQ( text.compare( same ), areg::Ordering::Equal );
    EXPECT_NE( text.compare( shorter ), areg::Ordering::Equal );

    areg::String part;
    text.substring( part, 0, 12 );
    EXPECT_EQ( part, hello );
}

/**
 * \brief   format() passes UTF-8 arguments and format text through unchanged.
 **/
TEST( StringUnicodeTest, utf8_format )
{
    const areg::String expected( "\xD0\x96 \xE2\x80\x94 5 \xE2\x82\xAC" );    // Ж — 5 €
    areg::String text;
    text.format( "%s \xE2\x80\x94 %d \xE2\x82\xAC", "\xD0\x96", 5 );    // %s — %d €, Ж
    EXPECT_EQ( text, expected );
}

/**
 * \brief   A UTF-8 String is streamed as its bytes after the byte count, the same on every platform.
 **/
TEST( StringUnicodeTest, utf8_string_on_the_stream )
{
    const areg::String text( "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82, \xE4\xB8\x96\xE7\x95\x8C \xF0\x9F\x98\x80" );    // Привет, 世界 😀
    areg::SharedBuffer buf;
    buf << text << int32_t{ 3 };
    std::vector<uint8_t> expected{ counted( static_cast<uint32_t>(sizeof( UTF8_TEXT )), UTF8_TEXT, sizeof( UTF8_TEXT ) ) };
    expected.insert( expected.end( ), { 3, 0, 0, 0 } );
    EXPECT_EQ( bytes_of( buf ), expected );

    buf.move_to_begin( );
    areg::String back;
    int32_t value{ 0 };
    buf >> back >> value;
    EXPECT_EQ( back, text );
    EXPECT_EQ( value, 3 );
}

/**
 * \brief   Cyrillic and CJK in WideString are one character each and are streamed as UTF-16LE.
 **/
TEST( StringUnicodeTest, wide_text_on_the_stream )
{
    const areg::WideString text( L"Привет, 世界" );
    const areg::WideString world( L"世界" );
    const std::wstring hedgehogText( L"Ёж" );
    EXPECT_EQ( text.length( ), 10u );
    EXPECT_EQ( text.find_first( world ), 8 );

    areg::SharedBuffer buf;
    buf << text << hedgehogText;
    std::vector<uint8_t> expected{ counted( 10u, UTF16_TEXT, sizeof( UTF16_TEXT ) ) };
    const uint8_t second[ ]{ 0x01, 0x04, 0x36, 0x04 };
    const std::vector<uint8_t> tail{ counted( 2u, second, sizeof( second ) ) };
    expected.insert( expected.end( ), tail.begin( ), tail.end( ) );
    EXPECT_EQ( bytes_of( buf ), expected );

    buf.move_to_begin( );
    areg::WideString back;
    std::wstring hedgehog;
    buf >> back >> hedgehog;
    EXPECT_EQ( back, text );
    EXPECT_EQ( hedgehog, hedgehogText );
}

/**
 * \brief   An emoji in a wide string is a surrogate pair on the stream on every platform.
 **/
TEST( StringUnicodeTest, wide_emoji_is_a_surrogate_pair )
{
    const std::wstring text( L"a😀b" );
    areg::SharedBuffer buf;
    buf << text;
    const uint8_t units[ ]{ 'a', 0, 0x3D, 0xD8, 0x00, 0xDE, 'b', 0 };
    EXPECT_EQ( bytes_of( buf ), counted( 4u, units, sizeof( units ) ) );

    buf.move_to_begin( );
    std::wstring back;
    buf >> back;
    EXPECT_EQ( back, text );
}

/**
 * \brief   Latin-1 letters in a WideString change case.
 **/
TEST( StringUnicodeTest, wide_latin1_case )
{
    const areg::WideString lower( L"äöü äöü" );
    const areg::WideString upper( L"ÄÖÜ ÄÖÜ" );
    const areg::WideString street( L"Straße" );
    const wchar_t * const streetUpper{ L"STRAßE" };
    areg::WideString text( L"ÄÖÜ äöü" );
    text.make_lower( );
    EXPECT_EQ( text, lower );
    text.make_upper( );
    EXPECT_EQ( text, upper );
    EXPECT_EQ( street.compare( streetUpper, areg::START_POS, areg::COUNT_ALL, false ), areg::Ordering::Equal );
}

/**
 * \brief   Non-ASCII strings written to a file with the stream operators are read back unchanged;
 *          wide text written as text is UTF-16LE in the file.
 **/
TEST( StringUnicodeTest, file_keeps_non_ascii_text )
{
    const areg::String narrowText( "Gr\xC3\xBC\xC3\x9F" "e \xE4\xB8\x96\xE7\x95\x8C" );    // Grüße 世界
    const areg::WideString wideText( L"Привет" );
    areg::FileBuffer file;
    ASSERT_TRUE( file.open( ) );
    file << narrowText << wideText;
    ASSERT_TRUE( file.move_to_begin( ) );
    areg::String narrow;
    areg::WideString wide;
    file >> narrow >> wide;
    EXPECT_EQ( narrow, narrowText );
    EXPECT_EQ( wide, wideText );

    constexpr uint32_t textMode{ static_cast<uint32_t>(areg::FileBase::OpenMode::Write) | static_cast<uint32_t>(areg::FileBase::OpenMode::Text) };
    areg::FileBuffer text( textMode );
    ASSERT_TRUE( text.open( ) );
    const wchar_t * const line{ L"Привет, 世界" };
    ASSERT_TRUE( text.write_string( line ) );
    ASSERT_EQ( text.length( ), static_cast<uint32_t>(sizeof( UTF16_TEXT )) );
    ASSERT_TRUE( text.move_to_begin( ) );
    uint8_t bytes[ sizeof( UTF16_TEXT ) ]{ };
    ASSERT_EQ( text.read( bytes, static_cast<uint32_t>(sizeof( bytes )) ), static_cast<uint32_t>(sizeof( bytes )) );
    EXPECT_EQ( std::memcmp( bytes, UTF16_TEXT, sizeof( UTF16_TEXT ) ), 0 );
}
