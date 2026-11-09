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
#include <cwchar>
#include <string>
#include <type_traits>

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

namespace
{
    //!< Copies the zero terminated text into the buffer.
    template<typename C, size_t N>
    void put(C (&dst)[N], const C* src)
    {
        size_t i{ 0 };
        for (; (src[i] != 0) && (i + 1 < N); ++i)
            dst[i] = src[i];
        dst[i] = 0;
    }

    //!< Returns the ASCII text as a standard string of the character type.
    template<typename C>
    std::basic_string<C> T(const char* ascii)
    {
        std::basic_string<C> result;
        for (; *ascii != '\0'; ++ascii)
            result += static_cast<C>(static_cast<unsigned char>(*ascii));
        return result;
    }

    //!< The mutable copy of the ASCII text, as a buffer of the character type.
    template<typename C>
    struct Buffer
    {
        explicit Buffer(const char* ascii) : mText(T<C>(ascii)) { mText.resize(mText.size() + 8, C(0)); mText.resize(std::char_traits<C>::length(mText.c_str())); }
        C*                  data() { return &mText[0]; }
        std::basic_string<C> get() const { return std::basic_string<C>(mText.c_str()); }
        std::basic_string<C> mText;
    };
}

template<typename C>
class StringDefsEdgeTyped : public ::testing::Test
{
};

using EdgeCharTypes = ::testing::Types<char, wchar_t>;
TYPED_TEST_SUITE(StringDefsEdgeTyped, EdgeCharTypes);

//////////////////////////////////////////////////////////////////////////
// Search: empty source, case-insensitive phrase and character
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A search in an empty text or behind the last character finds nothing.
 **/
TYPED_TEST( StringDefsEdgeTyped, search_in_empty_text_or_behind_the_last_character_finds_nothing )
{
    using C = TypeParam;
    const auto a = T<C>( "a" );
    const auto abc = T<C>( "abc" );
    const auto none = T<C>( "" );
    EXPECT_EQ( areg::find_first<C>( a.c_str(), none.c_str(), 0 ), areg::INVALID_POS );
    EXPECT_EQ( areg::find_first<C>( a.c_str(), abc.c_str(), 3 ), areg::INVALID_POS );
    EXPECT_EQ( areg::find_first<C>( a.c_str(), abc.c_str(), -1 ), areg::INVALID_POS );
    EXPECT_EQ( areg::find_first<C>( C('a'), abc.c_str(), 3 ), areg::INVALID_POS );
    EXPECT_EQ( areg::find_first<C>( C('a'), abc.c_str(), -1 ), areg::INVALID_POS );
    EXPECT_EQ( areg::find_first<C>( C('\0'), abc.c_str(), 0 ), areg::INVALID_POS );
}

/**
 * \brief   The implementation of the phrase and character search rejects an empty source.
 **/
TYPED_TEST( StringDefsEdgeTyped, search_implementation_rejects_empty_source )
{
    using C = TypeParam;
    const auto a = T<C>( "a" );
    const auto none = T<C>( "" );
    EXPECT_EQ( (areg::str::find_first_phrase_impl<true, C>( a.c_str(), none.c_str(), 0, nullptr )), areg::INVALID_POS );
    EXPECT_EQ( (areg::str::find_first_phrase_impl<false, C>( a.c_str(), none.c_str(), 0, nullptr )), areg::INVALID_POS );
    EXPECT_EQ( (areg::str::find_first_char_impl<true, C>( C('a'), none.c_str(), 0, nullptr )), areg::INVALID_POS );
    EXPECT_EQ( (areg::str::find_first_char_impl<false, C>( C('a'), none.c_str(), 0, nullptr )), areg::INVALID_POS );
    EXPECT_EQ( (areg::str::find_first_phrase_impl<true, C>( a.c_str(), static_cast<const C*>(nullptr), 0, nullptr )), areg::INVALID_POS );
    EXPECT_EQ( (areg::str::find_first_char_impl<false, C>( C('a'), static_cast<const C*>(nullptr), 0, nullptr )), areg::INVALID_POS );
}

/**
 * \brief   The case-insensitive phrase search fails on a partial match and reports the rest after a match.
 **/
TYPED_TEST( StringDefsEdgeTyped, case_insensitive_phrase_search_rejects_partial_match_and_reports_rest )
{
    using C = TypeParam;
    const auto text = T<C>( "xabcabd" );
    const C* marker{ text.c_str() };
    const C* next{ marker };
    EXPECT_EQ( areg::find_first<C>( T<C>( "ABD" ).c_str(), text.c_str(), 0, false, &next ), 4 );
    EXPECT_EQ( next, nullptr );

    next = marker;
    EXPECT_EQ( areg::find_first<C>( T<C>( "AB" ).c_str(), text.c_str(), 0, false, &next ), 1 );
    EXPECT_EQ( next, text.c_str() + 3 );

    EXPECT_EQ( areg::find_first<C>( T<C>( "abcd" ).c_str(), T<C>( "xabc" ).c_str(), 0, false ), areg::INVALID_POS );
}

/**
 * \brief   The case-insensitive character search reports the rest only if text follows the match.
 **/
TYPED_TEST( StringDefsEdgeTyped, case_insensitive_character_search_reports_rest_only_when_text_follows )
{
    using C = TypeParam;
    const auto text = T<C>( "abc" );
    const C* next{ nullptr };
    EXPECT_EQ( areg::find_first<C>( C('B'), text.c_str(), 0, false, &next ), 1 );
    EXPECT_EQ( next, text.c_str() + 2 );

    next = nullptr;
    EXPECT_EQ( areg::find_first<C>( C('C'), text.c_str(), 0, false, &next ), 2 );
    EXPECT_EQ( next, nullptr );

    EXPECT_EQ( areg::find_first<C>( C('z'), text.c_str(), 0, false, &next ), areg::INVALID_POS );

    next = nullptr;
    EXPECT_EQ( areg::find_first<C>( C('c'), text.c_str(), 0, true, &next ), 2 );
    EXPECT_EQ( next, nullptr );
}

/**
 * \brief   The last-phrase search that ends at the first character finds nothing.
 **/
TYPED_TEST( StringDefsEdgeTyped, last_phrase_search_ending_at_first_character_finds_nothing )
{
    using C = TypeParam;
    EXPECT_EQ( areg::find_last<C>( T<C>( "a" ).c_str(), T<C>( "abc" ).c_str(), 0 ), areg::INVALID_POS );
    EXPECT_EQ( areg::find_last<C>( T<C>( "a" ).c_str(), T<C>( "abca" ).c_str(), areg::END_POS ), 3 );
}

//////////////////////////////////////////////////////////////////////////
// Starts with, remove character
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The case-insensitive prefix check fails when the text is shorter than the phrase.
 **/
TYPED_TEST( StringDefsEdgeTyped, case_insensitive_prefix_check_fails_when_text_is_shorter_than_phrase )
{
    using C = TypeParam;
    EXPECT_FALSE( areg::string_starts_with<C>( T<C>( "ab" ).c_str(), T<C>( "abc" ).c_str(), false ) );
    EXPECT_TRUE( areg::string_starts_with<C>( T<C>( "abc" ).c_str(), T<C>( "AB" ).c_str(), false ) );
    EXPECT_FALSE( areg::string_starts_with<C>( T<C>( "abc" ).c_str(), T<C>( "AX" ).c_str(), false ) );
}

/**
 * \brief   Removing a character keeps the text after the last removal and handles a removal at the end.
 **/
TYPED_TEST( StringDefsEdgeTyped, remove_character_keeps_tail_and_handles_removal_at_the_end )
{
    using C = TypeParam;
    {
        Buffer<C> text( "abcd" );
        areg::remove_char<C>( C('z'), text.data() );
        EXPECT_EQ( text.get(), T<C>( "abcd" ) );
    }
    {
        Buffer<C> text( "abca" );
        areg::remove_char<C>( C('a'), text.data() );
        EXPECT_EQ( text.get(), T<C>( "bc" ) );
    }
    {
        Buffer<C> text( "abcd" );
        areg::remove_char<C>( C('b'), text.data(), false );
        EXPECT_EQ( text.get(), T<C>( "acd" ) );
    }
    {
        Buffer<C> text( "bca" );
        areg::remove_char<C>( C('a'), text.data(), false );
        EXPECT_EQ( text.get(), T<C>( "bc" ) );
    }
}

//////////////////////////////////////////////////////////////////////////
// Numbers
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The size query of a number counts the sign of a negative decimal and rejects an unknown radix.
 **/
TYPED_TEST( StringDefsEdgeTyped, number_size_query_counts_sign_of_negative_decimal_and_rejects_unknown_radix )
{
    using C = TypeParam;
    EXPECT_EQ( (areg::make_string<C, int32_t>( nullptr, 0, -123, areg::Radix::Decimal )), 4 );
    EXPECT_EQ( (areg::make_string<C, int32_t>( nullptr, 0, 123, areg::Radix::Decimal )), 3 );
    EXPECT_EQ( (areg::make_string<C, int32_t>( nullptr, 0, -255, areg::Radix::Hexadecimal )), 2 );
    EXPECT_EQ( (areg::make_string<C, int32_t>( nullptr, 0, 42, areg::Radix::Automatic )), 2 );
    EXPECT_EQ( (areg::make_string<C, int32_t>( nullptr, 0, 42, static_cast<areg::Radix>(1) )), 0 );
    EXPECT_EQ( (areg::make_string<C, int32_t>( nullptr, 0, 42, static_cast<areg::Radix>(17) )), 0 );

    C buffer[8]{ C('x'), C('x'), C('x'), C('x'), C('x'), C('x'), C('x'), C('x') };
    EXPECT_EQ( (areg::make_string<C, int32_t>( buffer, 1, -42, areg::Radix::Decimal )), 3 );
    EXPECT_EQ( buffer[0], C('x') );
    EXPECT_EQ( (areg::make_string<C, int32_t>( buffer, 8, -255, areg::Radix::Hexadecimal )), 2 );
    EXPECT_EQ( std::basic_string<C>( buffer ), T<C>( "FF" ) );
}

/**
 * \brief   The integer parser reports the unparsed rest only if it is set and something remains.
 **/
TYPED_TEST( StringDefsEdgeTyped, integer_parser_reports_rest_only_when_set_and_text_remains )
{
    using C = TypeParam;
    const auto whole = T<C>( "123" );
    const C* remain{ whole.c_str() };
    EXPECT_EQ( areg::make_integer<C>( whole.c_str(), &remain ), 123 );
    EXPECT_EQ( remain, nullptr );

    const auto text = T<C>( "12ab" );
    EXPECT_EQ( areg::make_integer<C>( text.c_str(), &remain ), 12 );
    EXPECT_EQ( remain, text.c_str() + 2 );

    EXPECT_EQ( areg::make_integer<C>( text.c_str(), nullptr ), 12 );
    EXPECT_EQ( areg::make_integer<C>( T<C>( "-7" ).c_str(), nullptr ), -7 );
    EXPECT_EQ( areg::make_integer<C>( T<C>( "" ).c_str(), &remain ), 0 );
    EXPECT_EQ( remain, nullptr );
}

//////////////////////////////////////////////////////////////////////////
// Buffer fit, line, printable
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A text fits the buffer only if the formatted result is shorter than the buffer.
 **/
TEST( StringDefsEdgeTest, formatted_text_fits_buffer_only_when_shorter_than_buffer )
{
    EXPECT_TRUE( (areg::is_buffer_fit<8, char>( "%s", "abc" )) );
    EXPECT_FALSE( (areg::is_buffer_fit<8, char>( "%s", "abcdefghij" )) );
#if defined(__linux__)
    const wchar_t invalid[]{ static_cast<wchar_t>(0xd800), L'\0' };
    EXPECT_FALSE( (areg::is_buffer_fit<8, char>( "%ls", invalid )) );
#endif
    EXPECT_TRUE( (areg::is_buffer_fit<8, wchar_t>( L"%ls", L"abc" )) );
    EXPECT_FALSE( (areg::is_buffer_fit<8, wchar_t>( L"%ls", L"abcdefghij" )) );
}

/**
 * \brief   The line splitter works without the next pointer, stops at a non-printable character and honors the count.
 **/
TYPED_TEST( StringDefsEdgeTyped, line_splits_at_non_printable_character_and_honors_count )
{
    using C = TypeParam;
    {
        C text[]{ C('a'), C('b'), C(1), C('c'), C('d'), C(0) };
        EXPECT_EQ( std::basic_string<C>( areg::line<C>( text ) ), T<C>( "ab" ) );
        EXPECT_EQ( std::basic_string<C>( text + 3 ), T<C>( "cd" ) );
    }
    {
        C text[]{ C('a'), C('b'), C(1), C('c'), C('d'), C(0) };
        C* next{ nullptr };
        EXPECT_EQ( std::basic_string<C>( areg::line<C>( text, areg::COUNT_ALL, &next ) ), T<C>( "ab" ) );
        ASSERT_NE( next, nullptr );
        EXPECT_EQ( std::basic_string<C>( next ), T<C>( "cd" ) );
    }
    {
        Buffer<C> text( "abcdef" );
        C* next{ nullptr };
        EXPECT_EQ( std::basic_string<C>( areg::line<C>( text.data(), 3, &next ) ), T<C>( "abc" ) );
        ASSERT_NE( next, nullptr );
        EXPECT_EQ( std::basic_string<C>( next ), T<C>( "ef" ) );
    }
    {
        Buffer<C> text( "abc" );
        Buffer<C> other( "xyz" );
        C* next{ other.data() };
        EXPECT_EQ( std::basic_string<C>( areg::line<C>( text.data(), areg::COUNT_ALL, &next ) ), T<C>( "abc" ) );
        EXPECT_EQ( next, nullptr );
    }
    {
        Buffer<C> text( "ab\ncd" );
        EXPECT_EQ( std::basic_string<C>( areg::line<C>( text.data() ) ), T<C>( "ab" ) );
    }
    {
        Buffer<C> text( "ab\rcd" );
        C* next{ nullptr };
        EXPECT_EQ( std::basic_string<C>( areg::line<C>( text.data(), areg::COUNT_ALL, &next ) ), T<C>( "ab" ) );
        ASSERT_NE( next, nullptr );
        EXPECT_EQ( std::basic_string<C>( next ), T<C>( "cd" ) );
    }
    {
        Buffer<C> text( "ab\r\ncd" );
        C* next{ nullptr };
        EXPECT_EQ( std::basic_string<C>( areg::line<C>( text.data(), areg::COUNT_ALL, &next ) ), T<C>( "ab" ) );
        ASSERT_NE( next, nullptr );
        EXPECT_EQ( std::basic_string<C>( next ), T<C>( "cd" ) );
    }
}

/**
 * \brief   The printable prefix is cut at the count or at the first non-printable character.
 **/
TYPED_TEST( StringDefsEdgeTyped, printable_prefix_is_cut_at_count_or_at_non_printable_character )
{
    using C = TypeParam;
    {
        Buffer<C> text( "abcdef" );
        C* next{ nullptr };
        EXPECT_EQ( std::basic_string<C>( areg::printable<C>( text.data(), 3, &next ) ), T<C>( "abc" ) );
        ASSERT_NE( next, nullptr );
        EXPECT_EQ( std::basic_string<C>( next ), T<C>( "ef" ) );
    }
    {
        Buffer<C> text( "abc" );
        Buffer<C> other( "xyz" );
        C* next{ other.data() };
        EXPECT_EQ( std::basic_string<C>( areg::printable<C>( text.data(), areg::COUNT_ALL, &next ) ), T<C>( "abc" ) );
        EXPECT_EQ( next, nullptr );
    }
    {
        Buffer<C> text( "abc" );
        EXPECT_EQ( std::basic_string<C>( areg::printable<C>( text.data(), areg::COUNT_ALL ) ), T<C>( "abc" ) );
    }
    {
        Buffer<C> text( "abc" );
        EXPECT_EQ( areg::printable<C>( text.data(), 0 ), nullptr );
        EXPECT_EQ( text.get(), T<C>( "abc" ) );
    }
    {
        C* none{ nullptr };
        C* next{ nullptr };
        EXPECT_EQ( areg::printable<C>( none, 3, &next ), nullptr );
        EXPECT_EQ( next, nullptr );
    }
    {
        C text[]{ C('a'), C(1), C('c'), C(0) };
        EXPECT_EQ( std::basic_string<C>( areg::printable<C>( text, areg::COUNT_ALL ) ), T<C>( "a" ) );
    }
}

/**
 * \brief   A wide character above the Latin range is printable and does not end a line.
 **/
TEST( StringDefsEdgeTest, wide_character_above_latin_range_is_printable )
{
    EXPECT_TRUE( areg::is_printable<wchar_t>( static_cast<wchar_t>(0x0100) ) );
    wchar_t text[]{ L'a', static_cast<wchar_t>(0x0100), L'b', L'\0' };
    const wchar_t* result = areg::line<wchar_t>( text );
    EXPECT_EQ( result, text );
    EXPECT_EQ( std::wstring( result ), std::wstring( text ) );
    EXPECT_EQ( std::wcslen( result ), 3u );

    if constexpr (std::is_signed_v<wchar_t>)
    {
        EXPECT_FALSE( areg::is_printable<wchar_t>( static_cast<wchar_t>(-200) ) );
        wchar_t cut[]{ L'a', static_cast<wchar_t>(-200), L'b', L'\0' };
        EXPECT_EQ( std::wstring( areg::line<wchar_t>( cut ) ), std::wstring( L"a" ) );
    }
}

//////////////////////////////////////////////////////////////////////////
// Case conversion, comparison, length limit
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Case conversion of a null or empty text returns the text unchanged.
 **/
TYPED_TEST( StringDefsEdgeTyped, case_conversion_of_null_or_empty_text_returns_the_text )
{
    using C = TypeParam;
    C* none{ nullptr };
    C empty[]{ C(0) };
    EXPECT_EQ( areg::make_upper<C>( none ), nullptr );
    EXPECT_EQ( areg::make_lower<C>( none ), nullptr );
    EXPECT_EQ( areg::make_upper<C>( empty ), empty );
    EXPECT_EQ( areg::make_lower<C>( empty ), empty );
}

/**
 * \brief   Comparison of strings of different widths and of a null string follows the character order.
 **/
TEST( StringDefsEdgeTest, comparison_of_strings_with_null_side_and_different_widths_follows_order )
{
    EXPECT_EQ( (areg::compare_strings<char, char>( nullptr, "a" )), areg::Ordering::Smaller );
    EXPECT_EQ( (areg::compare_strings<char, char>( "a", nullptr )), areg::Ordering::Bigger );
    EXPECT_EQ( (areg::compare_strings<char, char>( nullptr, nullptr )), areg::Ordering::Equal );
    EXPECT_EQ( (areg::compare_strings<char, char>( "ABC", "abd", areg::COUNT_ALL, false )), areg::Ordering::Smaller );
    EXPECT_EQ( (areg::compare_strings<char, char>( "abcd", "abce", 3, true )), areg::Ordering::Equal );
    EXPECT_EQ( (areg::compare_strings<wchar_t, wchar_t>( L"abcd", L"abce", 3, true )), areg::Ordering::Equal );
    EXPECT_EQ( (areg::compare_strings<wchar_t, wchar_t>( L"abcd", L"abce", areg::COUNT_ALL, true )), areg::Ordering::Smaller );
    EXPECT_EQ( (areg::compare_strings<wchar_t, wchar_t>( L"ABC", L"abd", areg::COUNT_ALL, false )), areg::Ordering::Smaller );

    EXPECT_EQ( (areg::compare_strings<char, wchar_t>( "abc", L"abc", areg::COUNT_ALL, true )), areg::Ordering::Equal );
    EXPECT_EQ( (areg::compare_strings<char, wchar_t>( "abd", L"abc", areg::COUNT_ALL, true )), areg::Ordering::Bigger );
    EXPECT_EQ( (areg::compare_strings<char, wchar_t>( "abd", L"abc", 2, true )), areg::Ordering::Equal );
    EXPECT_EQ( (areg::compare_strings<char, wchar_t>( "ABC", L"abd", areg::COUNT_ALL, false )), areg::Ordering::Smaller );
    EXPECT_EQ( (areg::compare_strings<wchar_t, char>( L"abd", "abc", areg::COUNT_ALL, true )), areg::Ordering::Bigger );
    EXPECT_EQ( (areg::compare_strings<wchar_t, char>( L"abc", "abc", 2, true )), areg::Ordering::Equal );
    EXPECT_EQ( (areg::compare_strings<wchar_t, char>( L"ABC", "abc", areg::COUNT_ALL, false )), areg::Ordering::Equal );
    EXPECT_EQ( (areg::compare_strings<wchar_t, char>( nullptr, "a" )), areg::Ordering::Smaller );
    EXPECT_EQ( (areg::compare_strings<char, wchar_t>( nullptr, L"a" )), areg::Ordering::Smaller );
}

/**
 * \brief   The length limit check of a wide text with a zero limit accepts only an empty or null text.
 **/
TEST( StringDefsEdgeTest, wide_length_limit_with_zero_limit_accepts_only_empty_or_null_text )
{
    EXPECT_TRUE( areg::not_more<wchar_t>( L"", 0u ) );
    EXPECT_TRUE( areg::not_more<wchar_t>( nullptr, 0u ) );
    EXPECT_FALSE( areg::not_more<wchar_t>( L"a", 0u ) );
    EXPECT_TRUE( areg::not_more<wchar_t>( nullptr, 3u ) );
    EXPECT_TRUE( areg::not_more<wchar_t>( L"abc", 3u ) );
    EXPECT_FALSE( areg::not_more<wchar_t>( L"abcd", 3u ) );
    EXPECT_TRUE( areg::not_more<char>( nullptr, 3u ) );
    EXPECT_FALSE( areg::not_more<char>( "abcd", 3u ) );
}

//////////////////////////////////////////////////////////////////////////
// In-place trim with a counted prefix
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A counted trim keeps the tail behind the prefix, or nothing if the prefix is the whole text.
 **/
TYPED_TEST( StringDefsEdgeTyped, counted_trim_keeps_tail_behind_prefix_or_nothing )
{
    using C = TypeParam;
    {
        Buffer<C> text( " ab  cd" );
        areg::trim_all<C>( text.data(), 5 );
        EXPECT_EQ( text.get(), T<C>( "abcd" ) );
    }
    {
        Buffer<C> text( " ab " );
        areg::trim_all<C>( text.data(), 4 );
        EXPECT_EQ( text.get(), T<C>( "ab" ) );
    }
    {
        Buffer<C> text( "ab  cd" );
        areg::trim_all<C>( text.data(), 4 );
        EXPECT_EQ( text.get(), T<C>( "abcd" ) );
    }
    {
        Buffer<C> text( "ab  " );
        areg::trim_all<C>( text.data(), 4 );
        EXPECT_EQ( text.get(), T<C>( "ab" ) );
    }
    {
        Buffer<C> text( " ab " );
        areg::trim_all<C>( text.data() );
        EXPECT_EQ( text.get(), T<C>( "ab" ) );
    }
    {
        Buffer<C> text( "ab  cd" );
        areg::trim_right<C>( text.data(), 4 );
        EXPECT_EQ( text.get(), T<C>( "abcd" ) );
    }
    {
        Buffer<C> text( "ab  " );
        areg::trim_right<C>( text.data(), 4 );
        EXPECT_EQ( text.get(), T<C>( "ab" ) );
    }
    {
        Buffer<C> text( "ab  " );
        areg::trim_right<C>( text.data() );
        EXPECT_EQ( text.get(), T<C>( "ab" ) );
    }
    {
        Buffer<C> text( "  ab  cd" );
        areg::trim_left<C>( text.data(), 4 );
        EXPECT_EQ( text.get(), T<C>( "ab  cd" ) );
    }
    {
        Buffer<C> text( "  ab" );
        areg::trim_left<C>( text.data(), 4 );
        EXPECT_EQ( text.get(), T<C>( "ab" ) );
    }
    {
        Buffer<C> text( "  ab" );
        areg::trim_left<C>( text.data() );
        EXPECT_EQ( text.get(), T<C>( "ab" ) );
    }
    {
        Buffer<C> text( "ab " );
        areg::trim_left<C>( text.data() );
        EXPECT_EQ( text.get(), T<C>( "ab " ) );
    }
    {
        Buffer<C> text( "ab " );
        areg::trim_left<C>( text.data(), 3 );
        EXPECT_EQ( text.get(), T<C>( "ab " ) );
    }
}

/**
 * \brief   A trim into a destination with a zero source length leaves the destination empty.
 **/
TYPED_TEST( StringDefsEdgeTyped, trim_into_destination_from_zero_length_source_leaves_it_empty )
{
    using C = TypeParam;
    const auto src = T<C>( " a " );
    C dst[4]{ C('x'), C('x'), C('x'), C('x') };
    areg::trim_right( dst, 4, src.c_str(), 0 );
    EXPECT_EQ( dst[0], C('\0') );

    dst[0] = C('x');
    areg::trim_left( dst, 4, src.c_str(), 0 );
    EXPECT_EQ( dst[0], C('\0') );

    dst[0] = C('x');
    areg::trim_all( dst, 4, src.c_str(), 0 );
    EXPECT_EQ( dst[0], C('\0') );
}
