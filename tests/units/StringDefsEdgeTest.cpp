/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StringDefsEdgeTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the edge cases of the string helpers: null and empty
 *              input, white-space only text, limited copies and copies between character widths.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/StringDefs.hpp"

#include <cstring>
#include <string>

//////////////////////////////////////////////////////////////////////////
// Starts with, ends with
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   An empty or null text neither starts nor ends with anything.
 **/
TEST( StringDefsEdgeTest, empty_or_null_text_neither_starts_nor_ends_with_a_phrase_or_a_character )
{
    EXPECT_FALSE( areg::string_starts_with<char>( nullptr, 'a' ) );
    EXPECT_FALSE( areg::string_starts_with<char>( "", 'a' ) );
    EXPECT_FALSE( areg::string_ends_with<char>( nullptr, "a" ) );
    EXPECT_FALSE( areg::string_ends_with<char>( "abc", "" ) );
    EXPECT_FALSE( areg::string_ends_with<char>( "", "a" ) );
    EXPECT_FALSE( areg::string_ends_with<char>( nullptr, 'a' ) );
    EXPECT_FALSE( areg::string_ends_with<char>( "", 'a' ) );
}

/**
 * \brief   A phrase longer than the text is not a suffix; a matching suffix is, in both case modes.
 **/
TEST( StringDefsEdgeTest, ends_with_compares_the_tail_in_both_case_modes )
{
    EXPECT_FALSE( areg::string_ends_with<char>( "bc", "abc" ) );
    EXPECT_TRUE( areg::string_ends_with<char>( "abc", "bc" ) );
    EXPECT_FALSE( areg::string_ends_with<char>( "abc", "BC" ) );
    EXPECT_TRUE( areg::string_ends_with<char>( "abc", "BC", false ) );
    EXPECT_TRUE( areg::string_ends_with<char>( "abc", 'c' ) );
    EXPECT_FALSE( areg::string_ends_with<char>( "abc", 'C' ) );
    EXPECT_TRUE( areg::string_ends_with<char>( "abc", 'C', false ) );
    EXPECT_TRUE( areg::string_starts_with<char>( "abc", 'A', false ) );
    EXPECT_FALSE( areg::string_starts_with<char>( "abc", 'A' ) );
}

//////////////////////////////////////////////////////////////////////////
// Trim
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Trimming null, empty and zero-length input changes nothing and does not crash.
 **/
TEST( StringDefsEdgeTest, trim_of_null_or_empty_input_changes_nothing )
{
    areg::trim_all<char>( nullptr );
    areg::trim_left<char>( nullptr );
    areg::trim_right<char>( nullptr );

    char empty[4]{ };
    areg::trim_all( empty );
    areg::trim_left( empty );
    areg::trim_right( empty );
    EXPECT_STREQ( empty, "" );

    char text[8]{ 'a', ' ', 'b', '\0' };
    areg::trim_all( text, 0 );
    areg::trim_left( text, 0 );
    areg::trim_right( text, 0 );
    EXPECT_STREQ( text, "a b" );

    char dst[8]{ 'x', '\0' };
    areg::trim_all( static_cast<char *>(nullptr), 8, " a ", areg::COUNT_ALL );
    areg::trim_left( static_cast<char *>(nullptr), 8, " a", areg::COUNT_ALL );
    areg::trim_right( static_cast<char *>(nullptr), 8, "a ", areg::COUNT_ALL );

    // A missing source or a zero length leaves an empty, terminated destination.
    areg::trim_all( dst, 8, static_cast<const char *>(nullptr), areg::COUNT_ALL );
    EXPECT_STREQ( dst, "" );
    dst[0] = 'x';
    areg::trim_all( dst, 8, "  a  ", 0 );
    EXPECT_STREQ( dst, "" );
    dst[0] = 'x';
    areg::trim_left( dst, 8, static_cast<const char *>(nullptr), areg::COUNT_ALL );
    EXPECT_STREQ( dst, "" );
    dst[0] = 'x';
    areg::trim_right( dst, 8, static_cast<const char *>(nullptr), areg::COUNT_ALL );
    EXPECT_STREQ( dst, "" );
}

/**
 * \brief   In-place trimming removes the white space of the chosen side only.
 **/
TEST( StringDefsEdgeTest, in_place_trim_removes_white_space_of_the_chosen_side )
{
    char both[16]{ };
    areg::copy_string( both, 16, "  a b  " );
    areg::trim_all( both );
    EXPECT_STREQ( both, "a b" );

    char left[16]{ };
    areg::copy_string( left, 16, "  a b  " );
    areg::trim_left( left );
    EXPECT_STREQ( left, "a b  " );

    char right[16]{ };
    areg::copy_string( right, 16, "  a b  " );
    areg::trim_right( right );
    EXPECT_STREQ( right, "  a b" );
}

/**
 * \brief   Text made only of white space trims to an empty text on the side that is trimmed.
 **/
TEST( StringDefsEdgeTest, white_space_only_text_trims_to_empty )
{
    char all[8]{ };
    areg::copy_string( all, 8, "   " );
    areg::trim_all( all );
    EXPECT_STREQ( all, "" );

    char left[8]{ };
    areg::copy_string( left, 8, "   " );
    areg::trim_left( left );
    EXPECT_STREQ( left, "" );

    char right[8]{ };
    areg::copy_string( right, 8, "   " );
    areg::trim_right( right );
    EXPECT_STREQ( right, "" );
}

/**
 * \brief   Trimming a counted prefix leaves the rest of the buffer as it was.
 **/
TEST( StringDefsEdgeTest, trim_of_a_counted_prefix_keeps_the_tail )
{
    char text[16]{ };
    areg::copy_string( text, 16, " a  tail" );
    areg::trim_left( text, 3 );
    EXPECT_STREQ( text, "a  tail" );

    char right[16]{ };
    areg::copy_string( right, 16, "ab  tail" );
    areg::trim_right( right, 4 );
    EXPECT_STREQ( right, "abtail" );
}

/**
 * \brief   Trimming into another buffer stores the trimmed text, cut to the buffer size.
 **/
TEST( StringDefsEdgeTest, trim_into_another_buffer_stores_the_trimmed_text_cut_to_the_space )
{
    char dst[16]{ };
    areg::trim_all( dst, 16, "  hello  ", areg::COUNT_ALL );
    EXPECT_STREQ( dst, "hello" );

    areg::trim_left( dst, 16, "  hello  ", areg::COUNT_ALL );
    EXPECT_STREQ( dst, "hello  " );

    areg::trim_right( dst, 16, "  hello  ", areg::COUNT_ALL );
    EXPECT_STREQ( dst, "  hello" );

    char small[4]{ };
    areg::trim_all( small, 4, "  hello  ", areg::COUNT_ALL );
    EXPECT_STREQ( small, "hel" );
}

/**
 * \brief   Trimming wide text into a narrow buffer converts each character.
 **/
TEST( StringDefsEdgeTest, trim_converts_between_character_widths )
{
    char dst[16]{ };
    areg::trim_all( dst, 16, L"  wide  ", areg::COUNT_ALL );
    EXPECT_STREQ( dst, "wide" );
}

//////////////////////////////////////////////////////////////////////////
// Copy
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   copy_string() with a null destination or no space copies nothing; a null source terminates.
 **/
TEST( StringDefsEdgeTest, copy_string_handles_null_and_empty_arguments )
{
    char dst[4]{ 'x', 'x', 'x', 'x' };
    EXPECT_EQ( (areg::copy_string<char, char>( nullptr, 4, "ab" )), 0 );
    EXPECT_EQ( (areg::copy_string<char, char>( dst, 0, "ab" )), 0 );
    EXPECT_EQ( dst[0], 'x' );
    EXPECT_EQ( (areg::copy_string<char, char>( dst, 4, nullptr )), 0 );
    EXPECT_STREQ( dst, "" );
}

/**
 * \brief   Copying between character widths converts each character, stops at the space and terminates.
 **/
TEST( StringDefsEdgeTest, copy_string_between_character_widths_converts_and_terminates )
{
    char narrow[8]{ };
    EXPECT_EQ( areg::copy_string( narrow, 8, L"abc" ), 3 );
    EXPECT_STREQ( narrow, "abc" );

    EXPECT_EQ( areg::copy_string( narrow, 3, L"abcdef" ), 2 );
    EXPECT_STREQ( narrow, "ab" );

    EXPECT_EQ( areg::copy_string( narrow, 8, L"abcdef", 4 ), 4 );
    EXPECT_STREQ( narrow, "abcd" );

    wchar_t wide[8]{ };
    EXPECT_EQ( areg::copy_string( wide, 8, "xyz" ), 3 );
    EXPECT_EQ( std::wstring( wide ), std::wstring( L"xyz" ) );
}

/**
 * \brief   Reproduces the defect: trim_all(), trim_left() and trim_right() with a destination of zero capacity wrote the terminator.
 **/
TEST( StringDefsEdgeTest, trim_into_a_destination_without_room_does_not_write )
{
    char dst[2]{ 'x', 'y' };
    areg::trim_all( dst, 0, " a ", areg::COUNT_ALL );
    EXPECT_EQ( dst[0], 'x' );

    areg::trim_left( dst, 0, " a ", areg::COUNT_ALL );
    EXPECT_EQ( dst[0], 'x' );

    areg::trim_right( dst, 0, " a ", areg::COUNT_ALL );
    EXPECT_EQ( dst[0], 'x' );

    areg::trim_all( dst, -1, " a ", areg::COUNT_ALL );
    EXPECT_EQ( dst[0], 'x' );
}

/**
 * \brief   A destination of one character holds the terminator only, and an empty source still clears a destination with room.
 **/
TEST( StringDefsEdgeTest, trim_into_a_small_destination_terminates_it )
{
    char dst[2]{ 'x', 'y' };
    areg::trim_all( dst, 1, " a ", areg::COUNT_ALL );
    EXPECT_EQ( dst[0], '\0' );
    EXPECT_EQ( dst[1], 'y' );

    dst[0] = 'x';
    areg::trim_left( dst, 1, " a ", areg::COUNT_ALL );
    EXPECT_EQ( dst[0], '\0' );
    EXPECT_EQ( dst[1], 'y' );

    dst[0] = 'x';
    areg::trim_right( dst, 1, " a ", areg::COUNT_ALL );
    EXPECT_EQ( dst[0], '\0' );
    EXPECT_EQ( dst[1], 'y' );

    dst[0] = 'x';
    areg::trim_all( dst, 2, "", areg::COUNT_ALL );
    EXPECT_EQ( dst[0], '\0' );

    dst[0] = 'x';
    areg::trim_left( dst, 2, static_cast<const char *>(nullptr), areg::COUNT_ALL );
    EXPECT_EQ( dst[0], '\0' );
}
