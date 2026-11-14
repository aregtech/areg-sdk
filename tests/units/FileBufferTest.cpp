/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/FileBufferTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for FileBuffer and the stream functions of FileBase.
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
#include <cwchar>
#include <string>

namespace
{
    using OpenMode = areg::FileBase::OpenMode;

    constexpr uint32_t MODE_BINARY{ static_cast<uint32_t>(OpenMode::Write) | static_cast<uint32_t>(OpenMode::Binary) };
    constexpr uint32_t MODE_TEXT  { static_cast<uint32_t>(OpenMode::Write) | static_cast<uint32_t>(OpenMode::Text) };

    //!< Writes the text as is and moves the cursor to the begin.
    void fill_text( areg::FileBuffer & file, const char * text )
    {
        const uint32_t len{ static_cast<uint32_t>(std::strlen( text )) };
        ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>(text), len ), len );
        ASSERT_EQ( file.set_position( 0, areg::Cursor::SeekOrigin::Begin ), 0u );
    }

    //!< Writes the text as UTF-16LE units and moves the cursor to the begin.
    void fill_units( areg::FileBuffer & file, const std::u16string & units )
    {
        std::string bytes;
        for ( const char16_t unit : units )
        {
            bytes.push_back( static_cast<char>(unit & 0xFFu) );
            bytes.push_back( static_cast<char>((unit >> 8u) & 0xFFu) );
        }

        const uint32_t len{ static_cast<uint32_t>(bytes.size( )) };
        ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>(bytes.data( )), len ), len );
        ASSERT_EQ( file.set_position( 0, areg::Cursor::SeekOrigin::Begin ), 0u );
    }
}

//////////////////////////////////////////////////////////////////////////
// Open, close, state
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A new buffer is closed, reads and writes nothing; after open() it accepts data.
 **/
TEST( FileBufferTest, closed_buffer_does_nothing_and_open_enables_io )
{
    areg::FileBuffer file( MODE_BINARY );
    EXPECT_FALSE( file.is_opened( ) );
    EXPECT_TRUE( file.is_empty( ) );
    EXPECT_EQ( file.data_buffer( ), nullptr );
    EXPECT_EQ( file.position( ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.set_position( 0, areg::Cursor::SeekOrigin::Begin ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.reserve( 16 ), areg::INVALID_SIZE );
    EXPECT_FALSE( file.truncate( ) );
    EXPECT_FALSE( file.ensure_size( 4 ) );

    const uint8_t data[ 4 ]{ 1, 2, 3, 4 };
    uint8_t out[ 4 ]{ };
    EXPECT_EQ( file.write( data, 4 ), 0u );
    EXPECT_EQ( file.read( out, 4 ), 0u );
    EXPECT_EQ( file.insert_at( data, 4, 0 ), 0u );
    int32_t value{ 0 };
    EXPECT_FALSE( file.write_int( 5 ) );
    EXPECT_FALSE( file.read_int( value ) );

    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.is_opened( ) );
    EXPECT_TRUE( file.is_valid( ) );
    EXPECT_EQ( file.write( data, 4 ), 4u );
    EXPECT_EQ( file.length( ), 4u );
    EXPECT_FALSE( file.is_empty( ) );
    EXPECT_NE( file.data_buffer( ), nullptr );
}

/**
 * \brief   A temporary buffer opens empty, keeps the data while it is open and is deleted on close.
 **/
TEST( FileBufferTest, temporary_buffer_is_deleted_when_it_is_closed )
{
    areg::FileBuffer file( static_cast<uint32_t>(OpenMode::CreateTemp) );
    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.is_temporary( ) );
    EXPECT_TRUE( file.is_empty( ) );

    const uint8_t data[ 4 ]{ 1, 2, 3, 4 };
    EXPECT_EQ( file.write( data, 4 ), 4u );
    EXPECT_EQ( file.length( ), 4u );

    file.close( );
    EXPECT_FALSE( file.is_opened( ) );
    EXPECT_FALSE( file.is_valid( ) );
    EXPECT_EQ( file.data_buffer( ), nullptr );
}

/**
 * \brief   An invalid mode cannot open the buffer; a second open() keeps it opened.
 **/
TEST( FileBufferTest, invalid_mode_does_not_open )
{
    areg::FileBuffer file( static_cast<uint32_t>(OpenMode::Invalid) );
    EXPECT_FALSE( file.open( ) );
    EXPECT_FALSE( file.open( areg::String( "name" ), static_cast<uint32_t>(OpenMode::Invalid) ) );
    EXPECT_FALSE( file.is_opened( ) );

    EXPECT_TRUE( file.open( areg::String( "name" ), MODE_BINARY ) );
    EXPECT_TRUE( file.open( ) );
    EXPECT_FALSE( file.open( areg::String( "other" ), MODE_BINARY ) );
    EXPECT_TRUE( file.is_opened( ) );
}

/**
 * \brief   open(name, mode) sets the mode and the name of the buffer.
 **/
TEST( FileBufferTest, open_with_name_sets_name_and_mode )
{
    areg::FileBuffer file( static_cast<uint32_t>(OpenMode::Invalid) );
    ASSERT_TRUE( file.open( areg::String( "memory-file" ), MODE_TEXT ) );
    EXPECT_STREQ( file.name( ).as_string( ), "memory-file" );
    EXPECT_TRUE( file.is_text_mode( ) );
    EXPECT_FALSE( file.is_binary_mode( ) );
    EXPECT_TRUE( file.can_write( ) );
    EXPECT_TRUE( file.can_read( ) );
}

/**
 * \brief   The name masks of the buffer name are replaced.
 **/
TEST( FileBufferTest, open_with_name_replaces_masks )
{
    areg::FileBuffer file( static_cast<uint32_t>(OpenMode::Invalid) );
    ASSERT_TRUE( file.open( areg::String( "x_%time%_y" ), MODE_BINARY ) );
    EXPECT_FALSE( file.name( ).is_valid_position( file.name( ).find_first( "%time%" ) ) );
    EXPECT_TRUE( file.name( ).starts_with( "x_" ) );
    EXPECT_TRUE( file.name( ).ends_with( "_y" ) );
}

/**
 * \brief   The mode flags are reported by the state functions.
 **/
TEST( FileBufferTest, mode_flags_are_reported )
{
    areg::FileBuffer file( static_cast<uint32_t>(OpenMode::Write) | static_cast<uint32_t>(OpenMode::Binary) | static_cast<uint32_t>(OpenMode::ShareWrite) );
    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.is_binary_mode( ) );
    EXPECT_FALSE( file.is_text_mode( ) );
    EXPECT_TRUE( file.is_shared_read( ) );
    EXPECT_TRUE( file.is_shared_write( ) );
    EXPECT_FALSE( file.is_temporary( ) );
    EXPECT_FALSE( file.is_force_delete( ) );
    EXPECT_TRUE( file.is_valid( ) );
}

/**
 * \brief   close() without the delete flag drops the data, the buffer can be opened again and is empty.
 **/
TEST( FileBufferTest, close_drops_data_and_reopen_is_empty )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_int( 77 ) );
    ASSERT_EQ( file.length( ), 4u );

    file.close( );
    EXPECT_FALSE( file.is_opened( ) );

    ASSERT_TRUE( file.open( ) );
    EXPECT_EQ( file.length( ), 0u );
    EXPECT_EQ( file.position( ), 0u );
}

/**
 * \brief   close() of a buffer with the delete flag resets the name and mode.
 **/
TEST( FileBufferTest, close_with_delete_flag_removes_buffer )
{
    areg::FileBuffer file( MODE_BINARY | static_cast<uint32_t>(OpenMode::Delete), "to-delete" );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.is_force_delete( ) );
    ASSERT_TRUE( file.write_int( 1 ) );

    file.close( );
    EXPECT_FALSE( file.is_opened( ) );
    EXPECT_TRUE( file.name( ).is_empty( ) );
    EXPECT_EQ( file.mode( ), static_cast<uint32_t>(OpenMode::Invalid) );
    EXPECT_FALSE( file.open( ) );
}

/**
 * \brief   remove() of an owner buffer empties it and invalidates the mode.
 **/
TEST( FileBufferTest, remove_of_owner_resets_buffer )
{
    areg::FileBuffer file( MODE_BINARY, "owned" );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_int( 1 ) );

    EXPECT_TRUE( file.remove( ) );
    EXPECT_FALSE( file.is_opened( ) );
    EXPECT_TRUE( file.name( ).is_empty( ) );
    EXPECT_EQ( file.mode( ), static_cast<uint32_t>(OpenMode::Invalid) );
}

//////////////////////////////////////////////////////////////////////////
// Shared and view buffers
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A buffer made over a shared valid buffer reads its data, does not write, truncate and remove it.
 **/
TEST( FileBufferTest, shared_buffer_is_read_only )
{
    areg::SharedBuffer origin;
    ASSERT_EQ( origin.write( reinterpret_cast<const uint8_t *>("abcdef"), 6 ), 6u );
    areg::SharedBuffer copy( origin );
    ASSERT_TRUE( copy.is_shared( ) );

    areg::FileBuffer file( copy );
    ASSERT_TRUE( file.open( ) );
    EXPECT_FALSE( file.can_write( ) );
    EXPECT_TRUE( file.can_read( ) );
    EXPECT_EQ( file.length( ), 6u );

    const uint8_t extra[ 2 ]{ 'x', 'y' };
    EXPECT_EQ( file.write( extra, 2 ), 0u );
    EXPECT_FALSE( file.truncate( ) );
    EXPECT_FALSE( file.remove( ) );
    EXPECT_EQ( file.insert_at( extra, 2, 0 ), 0u );
    EXPECT_EQ( file.length( ), 6u );
    EXPECT_EQ( origin.size_used( ), 6u );

    file.set_position( 0, areg::Cursor::SeekOrigin::Begin );
    char text[ 8 ]{ };
    EXPECT_EQ( file.read( reinterpret_cast<uint8_t *>(text), 6 ), 6u );
    EXPECT_STREQ( text, "abcdef" );
}

/**
 * \brief   A buffer constructed from a const SharedBuffer is opened at once if the buffer is valid.
 **/
TEST( FileBufferTest, const_shared_buffer_is_opened_when_valid )
{
    areg::SharedBuffer valid;
    ASSERT_EQ( valid.write( reinterpret_cast<const uint8_t *>("ab"), 2 ), 2u );
    const areg::SharedBuffer & constValid{ valid };
    areg::FileBuffer opened( constValid, "valid" );
    EXPECT_TRUE( opened.is_opened( ) );
    EXPECT_EQ( opened.shared_buffer( ).size_used( ), 2u );

    const areg::SharedBuffer invalid;
    areg::FileBuffer closed( invalid );
    EXPECT_FALSE( closed.is_opened( ) );
}


//////////////////////////////////////////////////////////////////////////
// Binary data, positions, growth
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Data larger than the initial block grows the buffer and is read back unchanged.
 **/
TEST( FileBufferTest, growth_beyond_block_keeps_data )
{
    constexpr uint32_t blockSize{ 64 };
    constexpr uint32_t count{ 5 * 1024 };
    areg::FileBuffer file( MODE_BINARY, nullptr, blockSize );
    ASSERT_TRUE( file.open( ) );

    for ( uint32_t i = 0; i < count; ++ i )
    {
        ASSERT_TRUE( file.write_int( static_cast<int32_t>(i) ) );
    }

    EXPECT_EQ( file.length( ), count * sizeof(int32_t) );
    EXPECT_EQ( file.position( ), count * sizeof(int32_t) );

    ASSERT_TRUE( file.move_to_begin( ) );
    for ( uint32_t i = 0; i < count; ++ i )
    {
        int32_t value{ -1 };
        ASSERT_TRUE( file.read_int( value ) );
        ASSERT_EQ( value, static_cast<int32_t>(i) );
    }
}

/**
 * \brief   Reading more than is stored returns the stored size only; reading at the end returns zero.
 **/
TEST( FileBufferTest, read_past_the_end_returns_available_data )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    const uint8_t data[ 5 ]{ 1, 2, 3, 4, 5 };
    ASSERT_EQ( file.write( data, 5 ), 5u );
    ASSERT_TRUE( file.move_to_begin( ) );

    uint8_t out[ 16 ]{ };
    EXPECT_EQ( file.read( out, 16 ), 5u );
    EXPECT_EQ( std::memcmp( out, data, 5 ), 0 );
    EXPECT_TRUE( file.is_eof( ) );
    EXPECT_EQ( file.read( out, 16 ), 0u );
}

/**
 * \brief   The cursor moves from the begin, current and end positions and is clipped by the data.
 **/
TEST( FileBufferTest, set_position_origins )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    const uint8_t data[ 10 ]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    ASSERT_EQ( file.write( data, 10 ), 10u );

    EXPECT_EQ( file.set_position( 3, areg::Cursor::SeekOrigin::Begin ), 3u );
    EXPECT_EQ( file.set_position( 2, areg::Cursor::SeekOrigin::Current ), 5u );
    EXPECT_EQ( file.set_position( -2, areg::Cursor::SeekOrigin::Current ), 3u );
    EXPECT_EQ( file.set_position( -1, areg::Cursor::SeekOrigin::End ), 9u );

    uint8_t one{ 0xFF };
    EXPECT_EQ( file.read( &one, 1 ), 1u );
    EXPECT_EQ( one, 9u );
    EXPECT_TRUE( file.move_to_end( ) );
    EXPECT_EQ( file.position( ), 10u );
    EXPECT_TRUE( file.move_to_begin( ) );
    EXPECT_EQ( file.position( ), areg::FileBase::start_position( ) );
}

/**
 * \brief   insert_at() puts the data into the middle and shifts the tail.
 **/
TEST( FileBufferTest, insert_at_shifts_the_tail )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    const uint8_t data[ 4 ]{ 'a', 'b', 'e', 'f' };
    ASSERT_EQ( file.write( data, 4 ), 4u );

    const uint8_t middle[ 2 ]{ 'c', 'd' };
    EXPECT_EQ( file.insert_at( middle, 2, 2 ), 2u );
    EXPECT_EQ( file.length( ), 6u );

    ASSERT_TRUE( file.move_to_begin( ) );
    char text[ 8 ]{ };
    EXPECT_EQ( file.read( reinterpret_cast<uint8_t *>(text), 6 ), 6u );
    EXPECT_STREQ( text, "abcdef" );
}

/**
 * \brief   reserve() prepares the capacity and truncate() empties an owner buffer.
 **/
TEST( FileBufferTest, reserve_and_truncate )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    EXPECT_NE( file.reserve( 1000 ), areg::INVALID_SIZE );
    EXPECT_EQ( file.length( ), 0u );

    ASSERT_TRUE( file.write_int( 5 ) );
    EXPECT_EQ( file.length( ), 4u );
    EXPECT_TRUE( file.truncate( ) );
    EXPECT_EQ( file.length( ), 0u );
    EXPECT_TRUE( file.is_empty( ) );
}

/**
 * \brief   ensure_size() reports true when the buffer has room for the data to add.
 **/
TEST( FileBufferTest, ensure_size_reports_room )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.ensure_size( 10 ) );
    EXPECT_TRUE( file.ensure_size( 2000 ) );
}

/**
 * \brief   append() writes at the end whatever the cursor is.
 **/
TEST( FileBufferTest, append_writes_at_the_end )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "abc" );

    const uint8_t tail[ 3 ]{ 'd', 'e', 'f' };
    EXPECT_EQ( file.append( tail, 3 ), 3 );
    EXPECT_EQ( file.append( nullptr, 3 ), 0 );
    EXPECT_EQ( file.append( tail, 0 ), 0 );
    EXPECT_EQ( file.length( ), 6u );

    file.move_to_begin( );
    char text[ 8 ]{ };
    EXPECT_EQ( file.read( reinterpret_cast<uint8_t *>(text), 6 ), 6u );
    EXPECT_STREQ( text, "abcdef" );
}

/**
 * \brief   read_invert() and write_invert() work on the bytes in the reverse order.
 **/
TEST( FileBufferTest, invert_read_and_write_reverse_bytes )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    const uint8_t data[ 4 ]{ 1, 2, 3, 4 };
    EXPECT_EQ( file.write_invert( data, 4 ), 4 );
    EXPECT_EQ( file.write_invert( nullptr, 4 ), 0 );
    EXPECT_EQ( file.write_invert( data, 0 ), 0 );

    file.move_to_begin( );
    uint8_t plain[ 4 ]{ };
    ASSERT_EQ( file.read( plain, 4 ), 4u );
    EXPECT_EQ( plain[ 0 ], 4u );
    EXPECT_EQ( plain[ 3 ], 1u );

    file.move_to_begin( );
    uint8_t inverted[ 4 ]{ };
    EXPECT_EQ( file.read_invert( inverted, 4 ), 4 );
    EXPECT_EQ( std::memcmp( inverted, data, 4 ), 0 );
    EXPECT_EQ( file.read_invert( nullptr, 4 ), 0 );
    EXPECT_EQ( file.read_invert( inverted, 0 ), 0 );
}

/**
 * \brief   resize_and_fill() with zero size keeps the position and the data.
 **/
TEST( FileBufferTest, resize_and_fill_zero_size_keeps_state )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_int( 1 ) );
    EXPECT_EQ( file.resize_and_fill( 0, 0xAB ), 4u );
    EXPECT_EQ( file.length( ), 4u );
}

/**
 * \brief   resize_and_fill() sets the size to the requested one and fills the new space with the byte.
 **/
TEST( FileBufferTest, resize_and_fill_sets_bytes )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    file.resize_and_fill( 8, 0xAB );
    EXPECT_EQ( file.length( ), 8u );

    file.move_to_begin( );
    uint8_t out[ 8 ]{ };
    ASSERT_EQ( file.read( out, 8 ), 8u );
    for ( uint8_t byte : out )
    {
        EXPECT_EQ( byte, 0xABu );
    }
}

/**
 * \brief   resize_and_fill() to the current size fills nothing and keeps the data and the position.
 **/
TEST( FileBufferTest, resize_and_fill_to_the_current_size_keeps_the_data )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_int( 0x01020304 ) );
    EXPECT_EQ( file.resize_and_fill( 4, 0xAB ), 4u );
    EXPECT_EQ( file.length( ), 4u );

    file.move_to_begin( );
    int32_t value{ 0 };
    ASSERT_TRUE( file.read_int( value ) );
    EXPECT_EQ( value, 0x01020304 );
}

/**
 * \brief   append() and read_invert() do nothing on a closed buffer and on a buffer that cannot be written.
 **/
TEST( FileBufferTest, append_and_invert_read_refuse_a_closed_or_read_only_buffer )
{
    const uint8_t data[ 2 ]{ 1, 2 };
    uint8_t out[ 2 ]{ 7, 7 };
    areg::FileBuffer closed( MODE_BINARY );
    EXPECT_EQ( closed.append( data, 2 ), 0 );
    EXPECT_EQ( closed.read_invert( out, 2 ), 0 );
    EXPECT_EQ( out[ 0 ], 7u );

    areg::SharedBuffer origin;
    ASSERT_EQ( origin.write( reinterpret_cast<const uint8_t *>("ab"), 2 ), 2u );
    areg::SharedBuffer copy( origin );
    areg::FileBuffer readOnly( copy );
    ASSERT_TRUE( readOnly.open( ) );
    EXPECT_EQ( readOnly.append( data, 2 ), 0 );
    EXPECT_EQ( readOnly.length( ), 2u );
    EXPECT_EQ( readOnly.read_invert( out, 2 ), 2 );
    EXPECT_EQ( out[ 0 ], static_cast<uint8_t>('b') );
    EXPECT_EQ( out[ 1 ], static_cast<uint8_t>('a') );
}

/**
 * \brief   A search that starts behind the data finds nothing.
 **/
TEST( FileBufferTest, search_data_starting_behind_the_end_finds_nothing )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    const uint8_t data[ 4 ]{ 'a', 'b', 'c', 'd' };
    ASSERT_EQ( file.write( data, 4 ), 4u );
    const uint8_t needle[ 2 ]{ 'c', 'd' };
    EXPECT_EQ( file.search_data( 0, needle, 2 ), 2u );
    EXPECT_EQ( file.search_data( 1000, needle, 2 ), areg::Cursor::INVALID_CURSOR_POSITION );
}

//////////////////////////////////////////////////////////////////////////
// Typed values
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Every simple type is read back as it was written, in the order of writing.
 **/
TEST( FileBufferTest, simple_types_round_trip )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );

    ASSERT_TRUE( file.write_bool( true ) );
    ASSERT_TRUE( file.write_char( 'Q' ) );
    ASSERT_TRUE( file.write_char( L'W' ) );
    ASSERT_TRUE( file.write_short( -1234 ) );
    ASSERT_TRUE( file.write_int( -123456789 ) );
    ASSERT_TRUE( file.write_large( -1234567890123456789ll ) );
    ASSERT_TRUE( file.write_float( 3.5f ) );
    ASSERT_TRUE( file.write_double( -2.25 ) );
    const uint32_t expected{ static_cast<uint32_t>(sizeof(bool) + sizeof(char) + sizeof(uint32_t) + sizeof(int16_t) + sizeof(int32_t) + sizeof(int64_t) + sizeof(float) + sizeof(double)) };
    EXPECT_EQ( file.length( ), expected );

    ASSERT_TRUE( file.move_to_begin( ) );
    bool vBool{ false };
    char vChar{ 0 };
    wchar_t vWide{ 0 };
    int16_t vShort{ 0 };
    int32_t vInt{ 0 };
    int64_t vLarge{ 0 };
    float vFloat{ 0 };
    double vDouble{ 0 };
    EXPECT_TRUE( file.read_bool( vBool ) );
    EXPECT_TRUE( file.read_char( vChar ) );
    EXPECT_TRUE( file.read_char( vWide ) );
    EXPECT_TRUE( file.read_short( vShort ) );
    EXPECT_TRUE( file.read_int( vInt ) );
    EXPECT_TRUE( file.read_large( vLarge ) );
    EXPECT_TRUE( file.read_float( vFloat ) );
    EXPECT_TRUE( file.read_double( vDouble ) );

    EXPECT_TRUE( vBool );
    EXPECT_EQ( vChar, 'Q' );
    EXPECT_EQ( vWide, L'W' );
    EXPECT_EQ( vShort, -1234 );
    EXPECT_EQ( vInt, -123456789 );
    EXPECT_EQ( vLarge, -1234567890123456789ll );
    EXPECT_EQ( vFloat, 3.5f );
    EXPECT_EQ( vDouble, -2.25 );
    EXPECT_EQ( file.position( ), expected );
    EXPECT_TRUE( file.is_eof( ) );
}

/**
 * \brief   A typed read at the end of data fails and leaves the cursor and the output value as they were.
 **/
TEST( FileBufferTest, typed_read_at_the_end_fails )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_short( 7 ) );
    int16_t small{ 0 };
    ASSERT_TRUE( file.move_to_begin( ) );
    ASSERT_TRUE( file.read_short( small ) );
    EXPECT_EQ( small, 7 );

    bool flag{ true };
    char ch{ 'z' };
    int32_t value{ 123 };
    int64_t large{ 5 };
    double dbl{ 1.5 };
    EXPECT_FALSE( file.read_bool( flag ) );
    EXPECT_FALSE( file.read_char( ch ) );
    EXPECT_FALSE( file.read_int( value ) );
    EXPECT_FALSE( file.read_large( large ) );
    EXPECT_FALSE( file.read_double( dbl ) );
    EXPECT_TRUE( flag );
    EXPECT_EQ( ch, 'z' );
    EXPECT_EQ( value, 123 );
    EXPECT_EQ( large, 5 );
    EXPECT_EQ( dbl, 1.5 );
    EXPECT_EQ( file.position( ), 2u );
}

/**
 * \brief   A typed read with fewer bytes than the type needs fails and leaves the cursor in place.
 **/
TEST( FileBufferTest, typed_read_with_too_few_bytes_keeps_position )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_short( 7 ) );
    ASSERT_TRUE( file.move_to_begin( ) );

    int32_t value{ 123 };
    EXPECT_FALSE( file.read_int( value ) );
    EXPECT_EQ( value, 123 );
    EXPECT_EQ( file.position( ), 0u );

    int64_t large{ 5 };
    double real{ 1.5 };
    EXPECT_FALSE( file.read_large( large ) );
    EXPECT_FALSE( file.read_double( real ) );
    EXPECT_EQ( large, 5 );
    EXPECT_EQ( real, 1.5 );
    EXPECT_EQ( file.position( ), 0u );

    int16_t small{ 0 };
    EXPECT_TRUE( file.read_short( small ) );
    EXPECT_EQ( small, 7 );
    EXPECT_EQ( file.position( ), 2u );
}

/**
 * \brief   A wide character read from a buffer that holds a part of it fails and keeps the cursor.
 **/
TEST( FileBufferTest, wide_char_read_of_partial_char_keeps_position )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_char( 'a' ) );
    ASSERT_TRUE( file.move_to_begin( ) );

    wchar_t wide{ L'z' };
    EXPECT_FALSE( file.read_char( wide ) );
    EXPECT_EQ( wide, L'z' );
    EXPECT_EQ( file.position( ), 0u );

    char narrow{ 0 };
    EXPECT_TRUE( file.read_char( narrow ) );
    EXPECT_EQ( narrow, 'a' );
}

/**
 * \brief   A buffer with a view reads typed data through the same API.
 **/
TEST( FileBufferTest, typed_values_survive_shared_copy )
{
    areg::FileBuffer writer( MODE_BINARY );
    ASSERT_TRUE( writer.open( ) );
    ASSERT_TRUE( writer.write_int( 42 ) );
    ASSERT_TRUE( writer.write_double( 0.5 ) );

    areg::SharedBuffer copy( writer.shared_buffer( ) );
    areg::FileBuffer reader( copy );
    ASSERT_TRUE( reader.open( ) );
    int32_t iv{ 0 };
    double dv{ 0 };
    ASSERT_TRUE( reader.move_to_begin( ) );
    EXPECT_TRUE( reader.read_int( iv ) );
    EXPECT_TRUE( reader.read_double( dv ) );
    EXPECT_EQ( iv, 42 );
    EXPECT_EQ( dv, 0.5 );
}

//////////////////////////////////////////////////////////////////////////
// Strings
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A string written in the binary mode includes the terminator; in the text mode it does not.
 **/
TEST( FileBufferTest, write_string_terminator_depends_on_mode )
{
    areg::FileBuffer binary( MODE_BINARY );
    ASSERT_TRUE( binary.open( ) );
    EXPECT_TRUE( binary.write_string( "abc" ) );
    EXPECT_EQ( binary.length( ), 4u );
    EXPECT_TRUE( binary.write_string( L"wd" ) );
    EXPECT_EQ( binary.length( ), 4u + 3u * 2u );

    areg::FileBuffer text( MODE_TEXT );
    ASSERT_TRUE( text.open( ) );
    EXPECT_TRUE( text.write_string( "abc" ) );
    EXPECT_EQ( text.length( ), 3u );
    EXPECT_TRUE( text.write_string( areg::String( "de" ) ) );
    EXPECT_EQ( text.length( ), 5u );
    EXPECT_TRUE( text.write_string( areg::WideString( L"f" ) ) );
    EXPECT_EQ( text.length( ), 5u + 2u );
}

/**
 * \brief   Writing a null string or into a closed buffer fails and writes nothing.
 **/
TEST( FileBufferTest, write_string_rejects_null_and_closed )
{
    areg::FileBuffer closed( MODE_TEXT );
    EXPECT_FALSE( closed.write_string( "abc" ) );
    EXPECT_FALSE( closed.write_line( "abc" ) );

    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    const char * nullAscii{ nullptr };
    const wchar_t * nullWide{ nullptr };
    EXPECT_FALSE( file.write_string( nullAscii ) );
    EXPECT_FALSE( file.write_string( nullWide ) );
    EXPECT_FALSE( file.write_line( nullAscii ) );
    EXPECT_FALSE( file.write_line( nullWide ) );
    EXPECT_EQ( file.length( ), 0u );
}

/**
 * \brief   A string written in the binary mode is read back by every overload.
 **/
TEST( FileBufferTest, binary_string_round_trip )
{
    areg::FileBuffer narrow( MODE_BINARY );
    ASSERT_TRUE( narrow.open( ) );
    ASSERT_TRUE( narrow.write_string( "hello" ) );
    ASSERT_TRUE( narrow.move_to_begin( ) );
    char text[ 32 ]{ };
    EXPECT_GT( narrow.read_string( text, 32 ), 0 );
    EXPECT_STREQ( text, "hello" );

    areg::FileBuffer wide( MODE_BINARY );
    ASSERT_TRUE( wide.open( ) );
    ASSERT_TRUE( wide.write_string( L"world" ) );
    ASSERT_TRUE( wide.move_to_begin( ) );
    wchar_t wtext[ 32 ]{ };
    EXPECT_GT( wide.read_string( wtext, 32 ), 0 );
    EXPECT_EQ( std::wcscmp( wtext, L"world" ), 0 );
}

/**
 * \brief   Two strings written one after another in the binary mode are read back one by one.
 **/
TEST( FileBufferTest, binary_strings_are_read_one_by_one )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_string( "first" ) );
    ASSERT_TRUE( file.write_string( "second" ) );
    ASSERT_TRUE( file.write_int( 77 ) );
    ASSERT_TRUE( file.move_to_begin( ) );

    char text[ 32 ]{ };
    EXPECT_GT( file.read_string( text, 32 ), 0 );
    EXPECT_STREQ( text, "first" );
    EXPECT_GT( file.read_string( text, 32 ), 0 );
    EXPECT_STREQ( text, "second" );
    int32_t value{ 0 };
    EXPECT_TRUE( file.read_int( value ) );
    EXPECT_EQ( value, 77 );
}

/**
 * \brief   Reading a string into a small buffer returns only what fits and leaves the cursor after it.
 **/
TEST( FileBufferTest, read_string_is_limited_by_buffer_size )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "abcdefgh" );

    char text[ 4 ]{ };
    EXPECT_EQ( file.read_string( text, 4 ), 3 );
    EXPECT_STREQ( text, "abc" );
    EXPECT_EQ( file.position( ), 3u );

    EXPECT_EQ( file.read_string( text, 4 ), 3 );
    EXPECT_STREQ( text, "def" );

    EXPECT_EQ( file.read_string( text, 4 ), 2 );
    EXPECT_STREQ( text, "gh" );
    EXPECT_EQ( file.read_string( text, 4 ), 0 );
}

/**
 * \brief   Reading into an invalid buffer or with a too small size returns zero.
 **/
TEST( FileBufferTest, read_string_invalid_arguments )
{
    areg::FileBuffer file( MODE_TEXT );
    char text[ 8 ]{ };
    wchar_t wide[ 8 ]{ };
    areg::String ascii;
    EXPECT_EQ( file.read_string( text, 8 ), 0 );
    EXPECT_EQ( file.read_string( ascii ), 0 );
    EXPECT_EQ( file.read_line( text, 8 ), 0 );
    EXPECT_EQ( file.read_line( ascii ), 0 );

    ASSERT_TRUE( file.open( ) );
    fill_text( file, "abc" );
    EXPECT_EQ( file.read_string( static_cast<char *>(nullptr), 8 ), 0 );
    EXPECT_EQ( file.read_string( text, 1 ), 0 );
    EXPECT_EQ( file.read_string( text, 0 ), 0 );
    EXPECT_EQ( file.read_string( wide, 1 ), 0 );
    EXPECT_EQ( file.read_line( static_cast<char *>(nullptr), 8 ), 0 );
    EXPECT_EQ( file.read_line( text, 1 ), 0 );
    EXPECT_EQ( file.read_line( wide, 1 ), 0 );
    EXPECT_EQ( file.position( ), 0u );
}

/**
 * \brief   A string read stops at a non-printable character and the data behind it is read by the next call.
 **/
TEST( FileBufferTest, read_string_stops_at_non_printable )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    const uint8_t data[ 7 ]{ 'a', 'b', 'c', 0x01, 'd', 'e', 'f' };
    ASSERT_EQ( file.write( data, 7 ), 7u );
    ASSERT_TRUE( file.move_to_begin( ) );

    areg::String text;
    EXPECT_GT( file.read_string( text ), 0 );
    EXPECT_STREQ( text.as_string( ), "abc" );
    EXPECT_GT( file.read_string( text ), 0 );
    EXPECT_STREQ( text.as_string( ), "def" );
}

/**
 * \brief   Wide lines are read one by one with every line ending; the last line needs none.
 **/
TEST( FileBufferTest, wide_lines_are_read_with_every_line_ending )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    fill_units( file, u"first\nsecond\r\n\nlast" );

    areg::WideString line;
    EXPECT_EQ( file.read_line( line ), 6 );
    EXPECT_EQ( line, areg::WideString( L"first" ) );
    EXPECT_EQ( file.read_line( line ), 8 );
    EXPECT_EQ( line, areg::WideString( L"second" ) );
    EXPECT_EQ( file.read_line( line ), 1 );
    EXPECT_TRUE( line.is_empty( ) );
    EXPECT_EQ( file.read_line( line ), 4 );
    EXPECT_EQ( line, areg::WideString( L"last" ) );
    EXPECT_EQ( file.read_line( line ), 0 );
    EXPECT_TRUE( file.is_eof( ) );
}

/**
 * \brief   A wide CR that fills the buffer is one line end with the LF behind it, alone before other text
 *          or at the end of the data.
 **/
TEST( FileBufferTest, wide_line_into_a_buffer_takes_a_cr_at_its_end_as_one_line_end )
{
    wchar_t text[ 4 ]{ };

    areg::FileBuffer crlf( MODE_TEXT );
    ASSERT_TRUE( crlf.open( ) );
    fill_units( crlf, u"ab\r\nc" );
    EXPECT_EQ( crlf.read_line( text, 4 ), 4 );
    EXPECT_EQ( std::wcscmp( text, L"ab" ), 0 );
    EXPECT_EQ( crlf.read_line( text, 4 ), 1 );
    EXPECT_EQ( std::wcscmp( text, L"c" ), 0 );

    areg::FileBuffer cr( MODE_TEXT );
    ASSERT_TRUE( cr.open( ) );
    fill_units( cr, u"ab\rc" );
    EXPECT_EQ( cr.read_line( text, 4 ), 3 );
    EXPECT_EQ( std::wcscmp( text, L"ab" ), 0 );
    EXPECT_EQ( cr.read_line( text, 4 ), 1 );
    EXPECT_EQ( std::wcscmp( text, L"c" ), 0 );

    areg::FileBuffer last( MODE_TEXT );
    ASSERT_TRUE( last.open( ) );
    fill_units( last, u"ab\r" );
    EXPECT_EQ( last.read_line( text, 4 ), 3 );
    EXPECT_EQ( std::wcscmp( text, L"ab" ), 0 );
    EXPECT_EQ( last.read_line( text, 4 ), 0 );
}

/**
 * \brief   Wide text longer than one internal chunk is read whole, also when a surrogate pair crosses
 *          the chunk border; a string keeps the line end, a non-printable character ends it, a lone
 *          surrogate is kept.
 **/
TEST( FileBufferTest, wide_text_longer_than_a_chunk_keeps_a_surrogate_pair_on_the_chunk_border )
{
    std::u16string units( 255u, u'x' );
    units += u"\U0001F600yyy\n";
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    fill_units( file, units );

    std::wstring expected( 255u, L'x' );
    expected += L"\U0001F600yyy";
    areg::WideString line;
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_EQ( line, areg::WideString( expected.c_str( ) ) );
    EXPECT_EQ( file.read_line( line ), 0 );

    ASSERT_EQ( file.set_position( 0, areg::Cursor::SeekOrigin::Begin ), 0u );
    areg::WideString text;
    EXPECT_GT( file.read_string( text ), 0 );
    EXPECT_EQ( text, areg::WideString( (expected + L"\n").c_str( ) ) );

    areg::FileBuffer mixed( MODE_TEXT );
    ASSERT_TRUE( mixed.open( ) );
    fill_units( mixed, std::u16string( u"ab" ) + u'\x0001' + u"c" + static_cast<char16_t>(0xD83Du) + u"d" );
    EXPECT_EQ( mixed.read_string( text ), 3 );
    EXPECT_EQ( text, areg::WideString( L"ab" ) );
    EXPECT_EQ( mixed.read_string( text ), 3 );
    const wchar_t lone[ 4 ]{ L'c', static_cast<wchar_t>(0xD83Du), L'd', L'\0' };
    EXPECT_EQ( text, areg::WideString( lone ) );
}

/**
 * \brief   read_string() into a String returns a printable text that fills most of one chunk.
 **/
TEST( FileBufferTest, read_string_into_string_reads_long_text )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    const std::string expected( 1000, 'x' );
    fill_text( file, expected.c_str( ) );

    areg::String text;
    EXPECT_EQ( file.read_string( text ), static_cast<int32_t>(expected.size( )) );
    EXPECT_STREQ( text.as_string( ), expected.c_str( ) );
    EXPECT_TRUE( file.is_eof( ) );
}

/**
 * \brief   read_string() into a String reads the whole printable text, longer than one internal chunk.
 **/
TEST( FileBufferTest, read_string_into_string_reads_text_longer_than_a_chunk )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    const std::string expected( 3000, 'x' );
    fill_text( file, expected.c_str( ) );

    areg::String text;
    EXPECT_EQ( file.read_string( text ), static_cast<int32_t>(expected.size( )) );
    EXPECT_EQ( text.length( ), expected.size( ) );
    EXPECT_TRUE( file.is_eof( ) );
}

/**
 * \brief   read_string() into a WideString reads UTF-16LE text, whatever the size of wchar_t.
 **/
TEST( FileBufferTest, read_string_into_wide_string )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    const uint8_t source[ ]{ 'w', 0, 'i', 0, 'd', 0, 'e', 0 };
    ASSERT_EQ( file.write( source, static_cast<uint32_t>(sizeof( source )) ), static_cast<uint32_t>(sizeof( source )) );
    ASSERT_TRUE( file.move_to_begin( ) );

    areg::WideString text;
    EXPECT_EQ( file.read_string( text ), 4 );
    EXPECT_EQ( text, areg::WideString( L"wide" ) );
    EXPECT_TRUE( file.is_eof( ) );
}

/**
 * \brief   String objects use the stream string format in both file modes: a 4-byte count, then the
 *          characters, wide ones as 2-byte units.
 **/
TEST( FileBufferTest, string_objects_write_and_read )
{
    for ( const uint32_t mode : { MODE_TEXT, MODE_BINARY } )
    {
        areg::FileBuffer file( mode );
        ASSERT_TRUE( file.open( ) );
        EXPECT_EQ( file.write( areg::String( "abcd" ) ), 8u );
        EXPECT_EQ( file.write( areg::WideString( L"ef" ) ), 8u );
        EXPECT_EQ( file.length( ), 16u );

        ASSERT_TRUE( file.move_to_begin( ) );
        areg::String ascii;
        areg::WideString wide;
        EXPECT_EQ( file.read( ascii ), 8u );
        EXPECT_EQ( file.read( wide ), 8u );
        EXPECT_STREQ( ascii.as_string( ), "abcd" );
        EXPECT_EQ( wide, areg::WideString( L"ef" ) );
    }
}

/**
 * \brief   A string and a wide string written by the stream operators are read back by them.
 **/
TEST( FileBufferTest, stream_operators_round_trip )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    areg::FileBase & out{ file };
    out << areg::String( "first" );
    ASSERT_GT( file.length( ), 0u );

    ASSERT_TRUE( file.move_to_begin( ) );
    areg::String s1;
    const areg::FileBase & in{ file };
    in >> s1;
    EXPECT_STREQ( s1.as_string( ), "first" );

    areg::FileBuffer wide( MODE_BINARY );
    ASSERT_TRUE( wide.open( ) );
    areg::FileBase & wideOut{ wide };
    wideOut << areg::WideString( L"second" ) << L"third";
    ASSERT_GT( wide.length( ), 0u );
}

/**
 * \brief   Several strings written by the stream operators in the binary mode are read back one by one.
 **/
TEST( FileBufferTest, stream_operators_round_trip_of_several_strings )
{
    for ( const uint32_t mode : { MODE_TEXT, MODE_BINARY } )
    {
        areg::FileBuffer file( mode );
        ASSERT_TRUE( file.open( ) );
        areg::FileBase & out{ file };
        out << areg::String( "first" ) << areg::String( "second" ) << areg::WideString( L"third" ) << L"fourth" << "fifth";

        ASSERT_TRUE( file.move_to_begin( ) );
        areg::String s1, s2, s5;
        areg::WideString s3, s4;
        const areg::FileBase & in{ file };
        in >> s1 >> s2 >> s3 >> s4 >> s5;
        EXPECT_STREQ( s1.as_string( ), "first" );
        EXPECT_STREQ( s2.as_string( ), "second" );
        EXPECT_EQ( s3, areg::WideString( L"third" ) );
        EXPECT_EQ( s4, areg::WideString( L"fourth" ) );
        EXPECT_STREQ( s5.as_string( ), "fifth" );
        EXPECT_TRUE( file.is_eof( ) );
    }
}

/**
 * \brief   A shared buffer is written with its size.
 **/
TEST( FileBufferTest, shared_buffer_is_written_with_its_size )
{
    areg::SharedBuffer source;
    const uint8_t data[ 6 ]{ 9, 8, 7, 6, 5, 4 };
    ASSERT_EQ( source.write( data, 6 ), 6u );

    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    EXPECT_EQ( file.write( source ), 6u + sizeof(int32_t) );
    EXPECT_EQ( file.length( ), 6u + sizeof(int32_t) );
    ASSERT_TRUE( file.move_to_begin( ) );
    int32_t size{ 0 };
    ASSERT_TRUE( file.read_int( size ) );
    EXPECT_EQ( size, 6 );
}

/**
 * \brief   A shared buffer is read back with the same content.
 **/
TEST( FileBufferTest, shared_buffer_round_trip )
{
    areg::SharedBuffer source;
    const uint8_t data[ 6 ]{ 9, 8, 7, 6, 5, 4 };
    ASSERT_EQ( source.write( data, 6 ), 6u );

    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.write( source ), 6u + sizeof(int32_t) );
    ASSERT_TRUE( file.move_to_begin( ) );

    areg::SharedBuffer result;
    EXPECT_EQ( file.read( result ), 6u + sizeof(int32_t) );
    ASSERT_EQ( result.size_used( ), 6u );
    EXPECT_EQ( std::memcmp( result.buffer( ), data, 6 ), 0 );
}

/**
 * \brief   Reading a shared buffer with a size field that exceeds the stored data fails and gives an invalid buffer.
 **/
TEST( FileBufferTest, shared_buffer_read_with_truncated_data_fails )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_int( 100 ) );
    const uint8_t few[ 3 ]{ 1, 2, 3 };
    ASSERT_EQ( file.write( few, 3 ), 3u );
    ASSERT_TRUE( file.move_to_begin( ) );

    areg::SharedBuffer result;
    EXPECT_EQ( file.read( result ), 0u );

    ASSERT_TRUE( file.move_to_begin( ) );
    ASSERT_TRUE( file.write_int( -5 ) );
    ASSERT_TRUE( file.move_to_begin( ) );
    EXPECT_EQ( file.read( result ), 0u );
}

/**
 * \brief   Reading a shared buffer from a closed buffer returns zero.
 **/
TEST( FileBufferTest, shared_buffer_closed_read_and_write_return_zero )
{
    areg::FileBuffer file( MODE_BINARY );
    areg::SharedBuffer buf;
    EXPECT_EQ( file.read( buf ), 0u );
    EXPECT_EQ( file.write( buf ), 0u );
}

//////////////////////////////////////////////////////////////////////////
// Lines
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Lines separated by LF, CRLF and a last line without the line end are read one by one.
 **/
TEST( FileBufferTest, read_line_into_string_handles_line_ends )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "one\ntwo\r\nthree" );

    areg::String line;
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "one" );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "two" );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "three" );
    EXPECT_TRUE( file.is_eof( ) );
    EXPECT_EQ( file.read_line( line ), 0 );
    EXPECT_TRUE( line.is_empty( ) );
}

/**
 * \brief   An empty buffer has no line; an empty line is returned as an empty string between two lines.
 **/
TEST( FileBufferTest, read_line_empty_buffer_and_empty_line )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    areg::String line( "stale" );
    EXPECT_EQ( file.read_line( line ), 0 );
    EXPECT_TRUE( line.is_empty( ) );

    fill_text( file, "a\n\nb" );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "a" );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_TRUE( line.is_empty( ) );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "b" );
}

/**
 * \brief   read_line() into a character buffer returns one line per call and consumes its line end.
 **/
TEST( FileBufferTest, read_line_into_char_buffer )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "first\nsecond\r\nlast" );

    char text[ 32 ]{ };
    EXPECT_EQ( file.read_line( text, 32 ), 6 );
    EXPECT_STREQ( text, "first" );
    EXPECT_EQ( file.position( ), 6u );
    EXPECT_EQ( file.read_line( text, 32 ), 8 );
    EXPECT_STREQ( text, "second" );
    EXPECT_EQ( file.read_line( text, 32 ), 4 );
    EXPECT_STREQ( text, "last" );
    EXPECT_EQ( file.read_line( text, 32 ), 0 );
}

/**
 * \brief   read_line() into a short buffer returns the part of the line that fits; the line end is a line of its own.
 **/
TEST( FileBufferTest, read_line_into_short_buffer_splits_the_line )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "abcdefgh\nij" );

    char text[ 5 ]{ };
    EXPECT_EQ( file.read_line( text, 5 ), 4 );
    EXPECT_STREQ( text, "abcd" );
    EXPECT_EQ( file.read_line( text, 5 ), 4 );
    EXPECT_STREQ( text, "efgh" );
    EXPECT_GT( file.read_line( text, 5 ), 0 );
    EXPECT_STREQ( text, "" );
    EXPECT_EQ( file.read_line( text, 5 ), 2 );
    EXPECT_STREQ( text, "ij" );
}

/**
 * \brief   A line is written with the line end and read back without it, for narrow and wide text.
 **/
TEST( FileBufferTest, write_line_round_trip )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.write_line( "alpha" ) );
    EXPECT_TRUE( file.write_line( areg::String( "beta" ) ) );
    EXPECT_EQ( file.length( ), 6u + 5u );

    ASSERT_TRUE( file.move_to_begin( ) );
    areg::String line;
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "alpha" );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "beta" );

    areg::FileBuffer wide( MODE_TEXT );
    ASSERT_TRUE( wide.open( ) );
    EXPECT_TRUE( wide.write_line( L"gamma" ) );
    EXPECT_TRUE( wide.write_line( areg::WideString( L"delta" ) ) );
    EXPECT_EQ( wide.length( ), 2u * 6u * 2u );

    ASSERT_TRUE( wide.move_to_begin( ) );
    areg::WideString wideLine;
    EXPECT_GT( wide.read_line( wideLine ), 0 );
    EXPECT_EQ( wideLine, areg::WideString( L"gamma" ) );
    EXPECT_GT( wide.read_line( wideLine ), 0 );
    EXPECT_EQ( wideLine, areg::WideString( L"delta" ) );
}

/**
 * \brief   A wide line is read into a wide buffer.
 **/
TEST( FileBufferTest, read_line_into_wide_buffer )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_line( L"one" ) );
    ASSERT_TRUE( file.write_line( L"two" ) );
    ASSERT_TRUE( file.move_to_begin( ) );

    wchar_t text[ 16 ]{ };
    EXPECT_EQ( file.read_line( text, 16 ), 4 );
    EXPECT_EQ( std::wcscmp( text, L"one" ), 0 );
    EXPECT_EQ( file.read_line( text, 16 ), 4 );
    EXPECT_EQ( std::wcscmp( text, L"two" ), 0 );
    EXPECT_EQ( file.read_line( text, 16 ), 0 );
}

//////////////////////////////////////////////////////////////////////////
// Search
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   search_data() finds bytes at the begin, in the middle, across a window border and at the end.
 **/
TEST( FileBufferTest, search_data_finds_position )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    std::string content( 1000, 'a' );
    content.replace( 0, 3, "XYZ" );
    content.replace( 499, 4, "WXYZ" );
    content.replace( 997, 3, "END" );
    ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>(content.data( )), static_cast<uint32_t>(content.size( )) ), content.size( ) );

    const uint8_t xyz[ 3 ]{ 'X', 'Y', 'Z' };
    EXPECT_EQ( file.search_data( 0, xyz, 3 ), 0u );
    EXPECT_EQ( file.search_data( 1, xyz, 3 ), 500u );
    EXPECT_EQ( file.search_data( 501, xyz, 3 ), areg::Cursor::INVALID_CURSOR_POSITION );

    const uint8_t end[ 3 ]{ 'E', 'N', 'D' };
    EXPECT_EQ( file.search_data( 0, end, 3 ), 997u );

    const uint8_t missing[ 3 ]{ 'q', 'q', 'q' };
    EXPECT_EQ( file.search_data( 0, missing, 3 ), areg::Cursor::INVALID_CURSOR_POSITION );
}

/**
 * \brief   search_data() with invalid arguments returns the invalid position.
 **/
TEST( FileBufferTest, search_data_invalid_arguments )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "abcdef" );

    const uint8_t cd[ 2 ]{ 'c', 'd' };
    EXPECT_EQ( file.search_data( 0, cd, 2 ), 2u );
    EXPECT_EQ( file.search_data( areg::Cursor::INVALID_CURSOR_POSITION, cd, 2 ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.search_data( 0, nullptr, 2 ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.search_data( 0, cd, 0 ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.search_data( 100, cd, 2 ), areg::Cursor::INVALID_CURSOR_POSITION );

    areg::SharedBuffer pattern;
    ASSERT_EQ( pattern.write( cd, 2 ), 2u );
    EXPECT_EQ( file.search_data( 0, pattern ), 2u );
}

/**
 * \brief   search_text() finds narrow and wide text, case sensitive and insensitive, and reports a miss.
 **/
TEST( FileBufferTest, search_text_finds_position )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "Hello World, hello areg" );

    EXPECT_EQ( file.search_text( 0, "hello", true ), 13u );
    EXPECT_EQ( file.search_text( 0, "hello", false ), 0u );
    EXPECT_EQ( file.search_text( 1, "hello", false ), 13u );
    EXPECT_EQ( file.search_text( 0, areg::String( "areg" ), true ), 19u );
    EXPECT_EQ( file.search_text( 0, "absent", true ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.search_text( 0, "", true ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.search_text( areg::Cursor::INVALID_CURSOR_POSITION, "areg", true ), areg::Cursor::INVALID_CURSOR_POSITION );

    areg::FileBuffer wide( MODE_TEXT );
    ASSERT_TRUE( wide.open( ) );
    ASSERT_TRUE( wide.write_string( L"Wide Text Here" ) );
    EXPECT_EQ( wide.search_text( 0, L"Text", true ), 5u * 2u );
    EXPECT_EQ( wide.search_text( 0, areg::WideString( L"text" ), false ), 5u * 2u );
    EXPECT_EQ( wide.search_text( 0, areg::WideString( L"text" ), true ), areg::Cursor::INVALID_CURSOR_POSITION );
}

//////////////////////////////////////////////////////////////////////////
// Flags
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   flush() on a buffer keeps the data.
 **/
TEST( FileBufferTest, flush_keeps_data )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_int( 3 ) );
    file.flush( );
    EXPECT_EQ( file.length( ), 4u );
    EXPECT_EQ( file.position( ), 4u );
}

/**
 * \brief   A wide string written in the text mode is read back as the same wide string.
 **/
TEST( FileBufferTest, wide_string_text_round_trip )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_string( areg::WideString( L"wide text" ) ) );
    ASSERT_TRUE( file.move_to_begin( ) );

    areg::WideString text;
    EXPECT_GT( file.read_string( text ), 0 );
    EXPECT_EQ( text, areg::WideString( L"wide text" ) );
}

/**
 * \brief   Wide text is stored as UTF-16LE: 2 bytes per character, a surrogate pair above U+FFFF.
 **/
TEST( FileBufferTest, wide_text_is_utf16_in_the_file )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    std::wstring source{ L"A" };
    if constexpr ( sizeof( wchar_t ) == 4u )
    {
        source += static_cast<wchar_t>(0x1F600);
    }
    else
    {
        source += static_cast<wchar_t>(0xD83D);
        source += static_cast<wchar_t>(0xDE00);
    }

    ASSERT_TRUE( file.write_string( source.c_str( ) ) );
    ASSERT_EQ( file.length( ), 6u );
    ASSERT_TRUE( file.move_to_begin( ) );
    uint8_t bytes[ 6 ]{ };
    ASSERT_EQ( file.read( bytes, 6u ), 6u );
    const uint8_t expected[ 6 ]{ 'A', 0, 0x3D, 0xD8, 0x00, 0xDE };
    EXPECT_EQ( std::memcmp( bytes, expected, 6u ), 0 );
}

/**
 * \brief   A wide text longer than one read chunk is read whole.
 **/
TEST( FileBufferTest, wide_text_longer_than_a_chunk )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    const std::wstring source( 700u, L'y' );
    ASSERT_TRUE( file.write_string( source.c_str( ) ) );
    ASSERT_TRUE( file.move_to_begin( ) );

    areg::WideString text;
    EXPECT_EQ( file.read_string( text ), 700 );
    EXPECT_EQ( text.length( ), 700u );
    EXPECT_TRUE( file.is_eof( ) );
}

/**
 * \brief   A CR LF that is split by the end of a read chunk is one line end.
 **/
TEST( FileBufferTest, read_line_dos_line_end_across_chunks )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    const std::string first( 255u, 'a' );
    fill_text( file, (first + "\r\nnext").c_str( ) );

    areg::String line;
    EXPECT_EQ( file.read_line( line ), 257 );
    EXPECT_EQ( std::string( line.as_string( ) ), first );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "next" );
}

/**
 * \brief   An empty line is written as the line end alone.
 **/
TEST( FileBufferTest, write_empty_line )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.write_line( "" ) );
    EXPECT_TRUE( file.write_line( L"" ) );
    EXPECT_EQ( file.length( ), 1u + 2u );
}

/**
 * \brief   A typed wide character is a 4-byte code on every platform.
 **/
TEST( FileBufferTest, typed_wide_char_is_four_bytes )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_char( L'Z' ) );
    EXPECT_EQ( file.length( ), 4u );
    ASSERT_TRUE( file.move_to_begin( ) );
    wchar_t ch{ 0 };
    EXPECT_TRUE( file.read_char( ch ) );
    EXPECT_EQ( ch, L'Z' );
}

/**
 * \brief   A read-only buffer refuses every kind of write and keeps its data.
 **/
TEST( FileBufferTest, read_only_buffer_refuses_every_write )
{
    areg::SharedBuffer origin;
    ASSERT_EQ( origin.write( reinterpret_cast<const uint8_t *>("abcdef"), 6 ), 6u );
    areg::SharedBuffer copy( origin );
    areg::FileBuffer file( copy );
    ASSERT_TRUE( file.open( ) );
    ASSERT_FALSE( file.can_write( ) );

    const uint8_t extra[ 2 ]{ 'x', 'y' };
    areg::SharedBuffer block;
    ASSERT_EQ( block.write( extra, 2 ), 2u );
    EXPECT_EQ( file.append( extra, 2 ), 0u );
    EXPECT_EQ( file.write_invert( extra, 2 ), 0u );
    EXPECT_EQ( file.write( block ), 0u );
    EXPECT_FALSE( file.write_string( "text" ) );
    EXPECT_FALSE( file.write_line( "line" ) );
    EXPECT_FALSE( file.write_string( L"text" ) );
    EXPECT_FALSE( file.write_line( L"line" ) );
    EXPECT_EQ( file.length( ), 6u );
    EXPECT_EQ( origin.size_used( ), 6u );
}

/**
 * \brief   A stored shared buffer of size zero is read as an empty buffer; a size field that is cut
 *          is not read and keeps the position.
 **/
TEST( FileBufferTest, shared_buffer_of_size_zero_and_cut_size_field )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_int( 0 ) );
    const uint8_t cut[ 2 ]{ 1, 2 };
    ASSERT_EQ( file.write( cut, 2 ), 2u );
    ASSERT_TRUE( file.move_to_begin( ) );

    areg::SharedBuffer result;
    EXPECT_EQ( file.read( result ), sizeof(int32_t) );
    EXPECT_EQ( result.size_used( ), 0u );

    const uint32_t before{ file.position( ) };
    EXPECT_EQ( file.read( result ), 0u );
    EXPECT_EQ( file.position( ), before );
}

/**
 * \brief   A search that starts beyond the end, or finds nothing, gives the invalid position.
 **/
TEST( FileBufferTest, search_beyond_the_end_finds_nothing )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "abcdefabc" );

    const uint8_t phrase[ 3 ]{ 'a', 'b', 'c' };
    const uint8_t missing[ 2 ]{ 'x', 'y' };
    EXPECT_EQ( file.search_data( 0u, phrase, 3u ), 0u );
    EXPECT_EQ( file.search_data( 1u, phrase, 3u ), 6u );
    EXPECT_EQ( file.search_data( 0u, missing, 2u ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.search_data( 100u, phrase, 3u ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.search_text( 0u, "def", true ), 3u );
    EXPECT_EQ( file.search_text( 100u, "def", true ), areg::Cursor::INVALID_CURSOR_POSITION );
}

/**
 * \brief   A search for a phrase longer than half the 32-bit range finds nothing.
 **/
TEST( FileBufferTest, search_for_an_overlong_phrase_finds_nothing )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "abcdefabc" );

    const uint8_t phrase[ 3 ]{ 'a', 'b', 'c' };
    EXPECT_EQ( file.search_data( 0u, phrase, 0x8000'0000u ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.search_data( 0u, phrase, 3u ), 0u );
}

/**
 * \brief   The writable space is reported only for a buffer that is opened for writing, and it
 *          shrinks by what is written into the reserved space.
 **/
TEST( FileBufferTest, writable_space_follows_open_state_and_written_bytes )
{
    areg::FileBuffer file( MODE_BINARY );
    EXPECT_EQ( file.size_writable( ), 0u );
    ASSERT_TRUE( file.open( ) );
    ASSERT_NE( file.reserve( 64u ), areg::INVALID_SIZE );
    const uint32_t before{ file.size_writable( ) };
    EXPECT_GE( before, 64u );

    const uint8_t data[ 10 ]{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    ASSERT_EQ( file.write( data, 10u ), 10u );
    EXPECT_EQ( file.size_writable( ), before - 10u );
    file.close( );
    EXPECT_EQ( file.size_writable( ), 0u );

    areg::SharedBuffer origin;
    ASSERT_EQ( origin.write( data, 10u ), 10u );
    areg::SharedBuffer copy( origin );
    areg::FileBuffer readOnly( copy );
    ASSERT_TRUE( readOnly.open( ) );
    ASSERT_FALSE( readOnly.can_write( ) );
    EXPECT_EQ( readOnly.size_writable( ), 0u );
}

/**
 * \brief   The stream views of an opened file write and read the same values, and the invalid
 *          position is the cursor's invalid position.
 **/
TEST( FileBufferTest, stream_views_round_trip_values )
{
    EXPECT_EQ( areg::FileBase::invalid_position( ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( areg::FileBase::start_position( ), areg::Cursor::START_CURSOR_POSITION );

    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    file.write_stream( ) << static_cast<uint32_t>(0xA5A5'0102u) << areg::String( "stream" );
    ASSERT_TRUE( file.move_to_begin( ) );

    uint32_t number{ 0u };
    areg::String text;
    file.read_stream( ) >> number >> text;
    EXPECT_EQ( number, 0xA5A5'0102u );
    EXPECT_EQ( text, "stream" );
}

/**
 * \brief   A search that starts beyond the end finds nothing, wherever the cursor is; a search from
 *          inside the data finds the first match after the start.
 **/
TEST( FileBufferTest, text_search_beyond_the_end_ignores_the_current_cursor )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    fill_text( file, "abcdefabc" );

    ASSERT_TRUE( file.move_to_begin( ) );
    EXPECT_EQ( file.search_text( 100u, "def", true ), areg::Cursor::INVALID_CURSOR_POSITION );
    ASSERT_TRUE( file.move_to_begin( ) );
    EXPECT_EQ( file.search_text( 100u, L"def", true ), areg::Cursor::INVALID_CURSOR_POSITION );
    ASSERT_TRUE( file.move_to_begin( ) );
    EXPECT_EQ( file.search_data( 100u, reinterpret_cast<const uint8_t *>("def"), 3u ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.search_text( 4u, "abc", true ), 6u );
}
