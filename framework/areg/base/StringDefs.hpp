#ifndef AREG_BASE_STRINGDEFS_HPP
#define AREG_BASE_STRINGDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/StringDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, namespace with UTF String helper methods
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/TableDefs.hpp"

#include <algorithm>
#include <cstring>
#include <cwchar>
#include <limits>
#include <string_view>
#include <type_traits>

#include <stdarg.h>
#include <wchar.h>

/**
 * \brief   String namespace to work with null-terminated strings.
 **/
namespace areg {
/**
 * \brief   areg::CharPos, Definition of character position type in string
 **/
using CharPos   = areg::ElemPos;
/**
 * \brief   areg::CharCount, Definition of character counting type in string
 **/
using CharCount = areg::ElemCount;

//! ASCII locale.
inline constexpr const char* const LOCALE_ASCII       = "C";

//! en_US.UTF8 locale.
inline constexpr const char* const LOCALE_US_UTF8     = "en_US.UTF8";

//! en_US.UTF8 locale.
inline constexpr const char* const LOCALE_UTF8        = "C.UTF8";

//! Currently installed C locale.
inline constexpr const char* const LOCALE_DEFAULT   { LOCALE_UTF8 };

/**
 * \brief   Invalid position in string buffer
 **/
constexpr CharPos   INVALID_POS         { areg::INVALID_INDEX };
/**
 * \brief   Start position in string buffer
 **/
constexpr CharPos   START_POS           { areg::FIRST_INDEX };
/**
 * \brief   Last valid position in string where it is marked 'end of string'
 **/
constexpr CharPos   END_POS             { areg::LAST_INDEX };

/**
 * \brief   The minimum size of a buffer allocated in the stack to format a string.
 **/
constexpr int32_t       MSG_MIN_BUF_SIZE    { 128 };

/**
 * \brief   The size of a buffer allocated in the stack to format a string.
 **/
constexpr int32_t       MSG_BUF_SIZE        { 256 };

/**
 * \brief   The big size of a buffer allocated in the stack to format a large string.
 **/
constexpr int32_t       MSG_BIG_BUF_SIZE    { 512 };

/**
 * \brief   The extra large size of a buffer allocated in the stack to format an extra large string.
 **/
constexpr int32_t       MSG_EXTRA_BUF_SIZE  { 1024 };

/**
 * \brief   The enum used to convert string to digit and vice versa
 *          It defines the bases of conversion.
 * \note    The automatic detection (RadixAutomatic) of conversion works only
 *          when convert from string to digit and has following logic after ignoring white spaces:
 *              - if first character is '0' and second character is not 'x' or 'X', then it is Octal (RadixOctal)
 *              - if first character is '0' and second character is 'x' or 'X', then it is Hexadecimal (RadixHexadecimal)
 *              - if the first character starts between '1' and '9', then Decimal (RadixDecimal)
 *              - There is no way to determine bases 2, and the RadixBinary is ignored.
 **/
enum class Radix    : uint8_t
{
      Automatic    =  0    //!< Detect automatically
    , Binary       =  2    //!< Binary conversion, conversion base is 2
    , Octal        =  8    //!< Octal conversion, conversion base is 8
    , Decimal      = 10    //!< Decimal conversion, conversion base is 10
    , Hexadecimal  = 16    //!< Hexadecimal conversion, conversion base is 16
};

/**
 * \brief   End of String value
 *          areg::EndofString
 **/
constexpr char              EndOfString     { static_cast<char>('\0') };  //!< End of String, signed char

/**
 * \brief   End of String value
 *          areg::EndOfLine
 **/
constexpr char              EndOfLine       { static_cast<char>('\n') };    //!< UNIX End of line character

/**
 * \brief   Empty 8-bit string
 *          areg::EmptyStringA
 **/
constexpr std::string_view  EmptyStringA     { "" };                      //!< Empty 8-bit string

/**
 * \brief   Empty 16-bit string
 *          areg::EmptyStringW
 **/
constexpr std::wstring_view EmptyStringW     { L"" };                     //!< Empty 16-bit string

/**
 * \brief   Returns true if \a ch falls within the range covered by the lookup
 *          tables: all signed-char values [-128, -1] and all unsigned byte
 *          values [0, 255] (equivalent to ch >= -128 && ch <= 255).
 *
 *          The check is branchless and free of signed-integer overflow UB.
 *          Casting \a ch to uint32_t maps negative values to large unsigned
 *          numbers via two's-complement; adding 128u shifts the valid window
 *          to [0, 383] in unsigned space.  Any \a ch outside [-128, 255]
 *          maps to a value greater than 383.
 *
 * \param   ch  Code point value to test.
 **/
[[nodiscard]]
inline constexpr bool in_utf8_range(int32_t ch) noexcept;

/**
 * \brief   Converts \a ch to the corresponding table row index (low 8 bits).
 *          Must only be called after in_utf8_range(ch) returns true.
 **/
[[nodiscard]]
inline constexpr uint8_t utf8_index(int32_t ch) noexcept;

/**
 * \brief   Returns the CharCategory bitmask for code point \a ch.
 *          Looks up areg::UTF8_256_Table.  Code points outside [-128, 255] are classified as CharCategory::Undefined.
 *
 * \param   ch  Code point (int32_t); both signed-char and unsigned-byte values are accepted.
 * \return  uint16_t bitmask of CharCategory flags.
 **/
[[nodiscard]]
inline constexpr uint16_t utf8_char_def(int32_t ch) noexcept;

/**
 * \brief  Returns the uppercase equivalent of code point \a ch.
 *         Looks up areg::UTF8_256_Table_upper.  Code points outside [-128, 255] are returned unchanged.
 *
 * \param   ch  Code point (int32_t).
 * \return  Uppercase code point as uint32_t, or \a ch if no mapping exists.
 **/
[[nodiscard]]
inline constexpr uint32_t make_upper_char(int32_t ch) noexcept;

/**
 * \brief   Returns the lowercase equivalent of code point \a ch.
 *          Looks up areg::UTF8_256_Table_lower.  Code points outside [-128, 255] are returned unchanged.
 *
 * \param   ch  Code point (int32_t).
 * \return  Lowercase code point as uint32_t, or \a ch if no mapping exists.
 **/
[[nodiscard]]
inline constexpr uint32_t make_lower_char(int32_t ch) noexcept;

/**
 * \brief   Converts the given integer digit to a string, writing the result to the specified
 *          buffer. If the buffer is nullptr or charCount is zero, calculates and returns the
 *          space required.
 *
 * \param   strDst          The destination string buffer to write the result.
 * \param   charCount       The number of characters allocated in the buffer, including the null
 *                          terminator.
 * \param   digit           The integer to convert.
 * \param   radix           The conversion base. If equal to RadixAutomatic, defaults to
 *                          RadixDecimal.
 * \return  Returns the number of characters copied to the buffer, including the null
 *          terminator. If strDst is nullptr or charCount is zero, returns the number of
 *          characters required.
 **/
template<typename CharType, typename IntType>
int32_t make_string( CharType*          strDst
                   , areg::CharCount    charCount
                   , IntType            digit
                   , areg::Radix        radix = areg::Radix::Decimal) noexcept;

/**
 * \brief   Reverses the order of characters in the given string buffer.
 *
 * \param   strDst          The string buffer to reverse.
 * \param   charCount       The number of characters to reverse. If COUNT_ALL, reverses all
 *                          characters until the null terminator.
 **/
template<typename CharType>
void revert_string(CharType* strDst, areg::CharCount charCount = areg::COUNT_ALL) noexcept;

/**
 * \brief   Returns true if the given position is a valid string position.
 *
 * \param   pos     The position to validate.
 **/
[[nodiscard]]
inline constexpr bool is_position_valid(areg::CharPos pos) noexcept;

/**
 * \brief   Returns true if the given string is empty (nullptr or starts with null terminator).
 *
 * \param   strBuffer       The string buffer to check.
 * \return  Returns true if the string buffer is empty.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_empty( const CharType * strBuffer ) noexcept;

/**
 * \brief   Returns the length of the string buffer, counting characters until the null
 *          terminator.
 *
 * \param   theString       The string to measure.
 * \return  Returns the number of characters in the string.
 **/
template <typename CharType>
[[nodiscard]]
inline constexpr CharCount string_length( const CharType * theString ) noexcept;

/**
 * \brief   Returns the length of the string line, counting characters until the end-of-line or
 *          null terminator.
 *
 * \param   theString       The string to measure.
 * \return  Returns the number of characters in the line.
 **/
template <typename CharType>
[[nodiscard]]
inline CharCount string_line_length(const CharType* theString) noexcept;

/**
 * \brief   Copies characters from source to destination buffer without reallocating destination
 *          space.
 *
 * \param   strDst          The destination string buffer.
 * \param   dstSpace        The space available in the destination string buffer.
 * \param   strSrc          The source string.
 * \param   charsCopy       The number of characters to copy. By default, copies the entire
 *                          string if space permits.
 * \return  Returns the number of characters copied, which is the minimum of available
 *          destination space and characters to copy. Returns zero if no characters were copied.
 **/
template<typename CharDst, typename CharSrc>
CharCount copy_string( CharDst*       strDst
                     , CharCount      dstSpace
                     , const CharSrc* strSrc
                     , CharCount      charsCopy = areg::COUNT_ALL ) noexcept;

/**
 * \brief   Fast copy of characters from source to destination buffer without reallocating
 *          destination space.
 *
 * \param   strDst          The destination string buffer.
 * \param   strSrc          The source string.
 * \param   charsCopy       The number of characters to copy. By default, copies the entire
 *                          string if space permits.
 * \return  Returns the number of characters copied, which is the minimum of available
 *          destination space and characters to copy. Returns zero if no characters were copied.
 **/
template<typename CharType>
CharCount copy_string_fast( CharType*       strDst
                          , const CharType* strSrc
                          , CharCount       charsCopy = areg::COUNT_ALL) noexcept;

/**
 * \brief   Converts the given character to lower case based on the first 256 UTF-8 code page
 *          symbols.
 *
 * \param   ch      The character to convert.
 * \return  If the character is in range [-128, 127], returns the lower case equivalent.
 *          Otherwise, returns the same value.
 **/
template <typename CharType>
[[nodiscard]]
inline constexpr CharType make_lower(CharType ch) noexcept;

/**
 * \brief   Converts the given null-terminated string to lower case based on the first 256 UTF-8
 *          code page symbols.
 *
 * \param   source      The null-terminated string to convert.
 * \return  Returns the converted string (same buffer as the source parameter).
 **/
template <typename CharType>
inline constexpr const CharType* make_lower(CharType* source) noexcept;

/**
 * \brief   Converts the given character to upper case based on the first 256 UTF-8 code page
 *          symbols.
 *
 * \param   ch      The character to convert.
 * \return  If the character is in range [-128, 127], returns the upper case equivalent.
 *          Otherwise, returns the same value.
 **/
template <typename CharType>
[[nodiscard]] 
inline constexpr CharType make_upper(CharType ch) noexcept;

/**
 * \brief   Converts the given null-terminated string to upper case based on the first 256 UTF-8
 *          code page symbols.
 *
 * \param   source      The null-terminated string to convert.
 * \return  Returns the converted string (same buffer as the source parameter).
 **/
template <typename CharType>
inline constexpr const CharType* make_upper(CharType* source) noexcept;

/**
 * \brief   Returns true if the character is lower case.
 *
 * \param   ch      The character to check.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_lower(CharType ch) noexcept;

/**
 * \brief   Returns true if the character is upper case.
 *
 * \param   ch      The character to check.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_upper(CharType ch) noexcept;

/**
 * \brief   Compares two strings up to a specified number of characters or until null terminator
 *          is reached.
 *
 * \param   left_side           The left-hand side string to compare.
 * \param   right_side          The right-hand side string to compare.
 * \param   charCount           The number of characters to compare, or COUNT_ALL to compare
 *                              complete strings.
 * \param   caseSensitive       If false, comparison ignores case differences.
 * \tparam  CharLhs     The type of characters on left string. Either `char` or `wchar_t`.
 * \tparam  CharRhs     The type of characters on right string. Either `char` or `wchar_t`.
 * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
 *          greater. Valid only for first 256 UTF-8 code page symbols.
 * \note    The results are based on 'charCount' and 'caseSensitive' parameters.
 **/
template<typename CharLhs, typename CharRhs>
[[nodiscard]]
areg::Ordering compare_strings( const CharLhs* left_side
                              , const CharRhs* right_side
                              , CharCount      charCount      = areg::COUNT_ALL
                              , bool           caseSensitive  = true) noexcept;

/**
 * \brief   Compares two strings ignoring case differences.
 *
 * \param   left_side       The left-hand side string to compare.
 * \param   right_side      The right-hand side string to compare.
 * \param   count           The number of characters to compare. By default, compares complete
 *                          strings.
 * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
 *          greater. Valid only for first 256 UTF-8 code page symbols.
 * \tparam  CharLhs     The type of characters on left string. Either `char` or `wchar_t`.
 * \tparam  CharRhs     The type of characters on right string. Either `char` or `wchar_t`.
 **/
template<typename CharLhs, typename CharRhs>
[[nodiscard]]
inline areg::Ordering compare_ignore_case( const CharLhs*  left_side
                                         , const CharRhs*  right_side
                                         , areg::CharCount count      = areg::COUNT_ALL) noexcept;

/**
 * \brief   Compares the first count characters of two strings.
 *
 * \param   left_side       The left-hand side string to compare.
 * \param   right_side      The right-hand side string to compare.
 * \param   count           The number of characters to compare.
 * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
 *          greater. Valid only for first 256 UTF-8 code page symbols.
 * \tparam  CharLhs     The type of characters on left string. Either `char` or `wchar_t`.
 * \tparam  CharRhs     The type of characters on right string. Either `char` or `wchar_t`.
 **/
template<typename CharLhs, typename CharRhs>
[[nodiscard]]
inline areg::Ordering compare(const CharLhs* left_side, const CharRhs* right_side, areg::CharCount count) noexcept;

/**
 * \brief   Compares two strings until the first null terminator, case-sensitive.
 *
 * \param   left_side       The left-hand side string to compare.
 * \param   right_side      The right-hand side string to compare.
 * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
 *          greater. Valid only for first 256 UTF-8 code page symbols.
 **/
template<typename CharLhs, typename CharRhs>
[[nodiscard]]
inline areg::Ordering compare(const CharLhs* left_side, const CharRhs* right_side) noexcept;

/**
 * \brief   Fast comparison of the first count characters of two strings, case-sensitive.
 *
 * \param   left_side       The left-hand side string to compare.
 * \param   right_side      The right-hand side string to compare.
 * \param   count           The number of characters to compare.
 * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
 *          greater. Valid only for first 256 UTF-8 code page symbols.
 **/
template<typename CharType>
[[nodiscard]]
inline areg::Ordering compare_fast( const CharType * left_side, const CharType * right_side, areg::CharCount count ) noexcept;

/**
 * \brief   Returns true if the character is a letter based on the first 256 UTF-8 code page
 *          symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is a letter.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_letter( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is a numeric digit based on the first 256 UTF-8 code
 *          page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is numeric.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_numeric( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is a letter or digit based on the first 256 UTF-8 code
 *          page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is alphanumeric.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_alphanumeric( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is a symbol (not a control character) based on the
 *          first 256 UTF-8 code page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is a symbol.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_symbol( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is a white-space based on the first 256 UTF-8 code
 *          page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is white-space.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_whitespace( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is used as a delimiter in syntax based on the first
 *          256 UTF-8 code page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is a syntax delimiter.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_delimited( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is an end-of-line symbol based on the first 256 UTF-8
 *          code page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is end-of-line.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_eol( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is a carriage return symbol based on the first 256
 *          UTF-8 code page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is a carriage return.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_carriage_return( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is a new line symbol based on the first 256 UTF-8 code
 *          page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is a new line.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_new_line( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is a Unix-style new line ('\n').
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character equals '\n'.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_unix_eol(CharType ch) noexcept;

/**
 * \brief   Returns true if the first character of the string is a Unix-style new line ('\n').
 *
 * \param   source      The string to check.
 * \return  Returns true if the string is not null and the first character equals '\n'.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_unix_eol(const CharType * source) noexcept;

/**
 * \brief   Returns true if the two characters form a DOS-style new line ('\r\n').
 *
 * \param   ch1     The first character in range [-128, 127] to check.
 * \param   ch2     The second character in range [-128, 127] to check.
 * \return  Returns true if ch1 and ch2 form the sequence '\r\n'.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_dos_eol(CharType ch1, CharType ch2) noexcept;

/**
 * \brief   Returns true if the first two characters of the string form a DOS-style new line
 *          ('\r\n').
 *
 * \param   source      The string to check.
 * \return  Returns true if the string is not null and the first two characters form '\r\n'.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_dos_eol(const CharType* source) noexcept;

/**
 * \brief   Returns true if the character is a null terminator based on the first 256 UTF-8 code
 *          page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is the null terminator.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_eos( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is a control symbol based on the first 256 UTF-8 code
 *          page symbols.
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is a control symbol.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_control(CharType ch) noexcept;

/**
 * \brief   Returns true if the character is readable (human-readable characters and
 *          white-space).
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is readable.
 * \note    Readable and printable characters differ. Readable characters can be read without
 *          additional system support, while printable characters include tabs, new-line, and
 *          carriage return.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_readable( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character is printable (all readable characters plus additional
 *          white-space characters).
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character is printable.
 * \note    Printable characters include readable characters plus additional white-space such as
 *          tabs, new-line, and carriage return.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_printable( CharType ch ) noexcept;

/**
 * \brief   Returns true if the character matches one of the characters in the given sequence.
 *
 * \param   ch              The character in range [-128, 127] to check.
 * \param   chSequence      The sequence of characters to check against.
 * \return  Returns true if the character matches at least one entry in the sequence.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_one_of(CharType ch, const CharType* chSequence) noexcept;

/**
 * \brief   Removes white-space characters from the left-side of the string buffer.
 *
 * \param   strBuffer       The string buffer to trim.
 * \param   strLen          The length of the string, or COUNT_ALL to calculate automatically.
 **/
template<typename CharType>
void trim_left( CharType * strBuffer, CharCount strLen = areg::COUNT_ALL ) noexcept;

/**
 * \brief   Removes white-space characters from the left-side and copies the result to the
 *          destination buffer.
 *
 * \param   strDst      The destination buffer to copy trimmed string.
 * \param   lenDst      The destination buffer length. Copies only first (lenDst - 1) characters
 *                      if too small.
 * \param   strSrc      The source string buffer.
 * \param   lenSrc      The length of the source string, or COUNT_ALL to calculate
 *                      automatically.
 **/
template<typename CharDst, typename CharSrc>
constexpr void trim_left( CharDst *strDst, CharCount lenDst, const CharSrc * strSrc, CharCount lenSrc = areg::COUNT_ALL ) noexcept;

/**
 * \brief   Removes white-space characters from the right-side of the string buffer.
 *
 * \param   strBuffer       The string buffer to trim.
 * \param   strLen          The length of the string, or COUNT_ALL to calculate automatically.
 **/
template<typename CharType>
constexpr void trim_right( CharType * strBuffer, CharCount strLen = areg::COUNT_ALL ) noexcept;

/**
 * \brief   Removes white-space characters from the right-side and copies the result to the
 *          destination buffer.
 *
 * \param   strDst      The destination buffer to copy trimmed string.
 * \param   lenDst      The destination buffer length. Copies only first (lenDst - 1) characters
 *                      if too small.
 * \param   strSrc      The source string buffer.
 * \param   lenSrc      The length of the source string, or COUNT_ALL to calculate
 *                      automatically.
 **/
template<typename CharDst, typename CharSrc>
constexpr void trim_right( CharDst *strDst, CharCount lenDst, const CharSrc * strSrc, CharCount lenSrc = areg::COUNT_ALL ) noexcept;

/**
 * \brief   Removes white-space characters from both left- and right-sides of the string buffer.
 *
 * \param   strBuffer       The string buffer to trim.
 * \param   strLen          The length of the string, or COUNT_ALL to calculate automatically.
 **/
template<typename CharType>
void trim_all( CharType * strBuffer, areg::CharCount strLen = areg::COUNT_ALL ) noexcept;

/**
 * \brief   Removes white-space characters from both sides and copies the result to the
 *          destination buffer.
 *
 * \param   strDst      The destination buffer to copy trimmed string.
 * \param   lenDst      The destination buffer length. Copies only first (lenDst - 1) characters
 *                      if too small.
 * \param   strSrc      The source string buffer.
 * \param   lenSrc      The length of the source string, or COUNT_ALL to calculate
 *                      automatically.
 **/
template<typename CharDst, typename CharSrc>
constexpr void trim_all( CharDst *strDst, areg::CharCount lenDst, const CharSrc * strSrc, areg::CharCount lenSrc = areg::COUNT_ALL ) noexcept;

/**
 * \brief   Removes the specified character from the string, with options for multiple
 *          occurrences and case sensitivity.
 *
 * \param   chRemove            The character to search and remove.
 * \param   strSource           The string to search in and remove characters from.
 * \param   removeAll           If true, removes all matches; if false, removes only the first
 *                              match.
 * \param   caseSensitive       If false, ignores case when searching for the character.
 * \return  If removeAll is false, returns the position of the removed character if found. If
 *          removeAll is true or no match found, returns end-of-string position.
 **/
template<typename CharType>
constexpr CharType * remove_char(const CharType chRemove, CharType* strSource, bool removeAll = true, bool caseSensitive = true) noexcept;

/**
 * \brief   Searches for a phrase within a string, starting at the specified position.
 *
 * \param   strPhrase           The phrase to search for.
 * \param   strSource           The source string to search in.
 * \param   startPos            The starting position. If START_POS, searches from the
 *                              beginning.
 * \param   caseSensitive       If false, ignores case differences in the search.
 * \param[out] next            If not nullptr, on output contains the pointer in the source
 *                                 string after the found phrase; nullptr if not found.
 * \return  Returns the position where the phrase starts if found; areg::INVALID_POS
 *          otherwise.
 **/
template<typename CharType>
[[nodiscard]]
constexpr CharPos find_first( const CharType*  strPhrase
                            , const CharType*  strSource
                            , CharPos          startPos      = areg::START_POS
                            , bool             caseSensitive = true
                            , const CharType** next          = nullptr ) noexcept;

/**
 * \brief   Searches for a character within a string, starting at the specified position.
 *
 * \param   chSearch            The character to search for.
 * \param   strSource           The source string to search in.
 * \param   startPos            The starting position. If START_POS, searches from the
 *                              beginning.
 * \param   caseSensitive       If false, ignores case differences in the search.
 * \param[out] next            If not nullptr, on output contains the pointer in the source
 *                                 string after the found character; nullptr if not found.
 * \return  Returns the position where the character is found; areg::INVALID_POS if not
 *          found.
 **/
template<typename CharType>
[[nodiscard]]
constexpr CharPos find_first( CharType chSearch
                            , const CharType*  strSource
                            , CharPos          startPos      = areg::START_POS
                            , bool             caseSensitive = true
                            , const CharType** next          = nullptr ) noexcept;
/**
 * \brief   Reverse searches for a phrase within a string, starting from the specified position
 *          and moving toward the beginning.
 *
 * \param   strPhrase           The phrase to search for.
 * \param   strSource           The source string to search in.
 * \param   startPos            The starting position. If END_POS, searches from the end.
 * \param   caseSensitive       If false, ignores case differences in the search.
 * \param[out] next            If not nullptr, on output contains the pointer in the source
 *                                 string before the found phrase; nullptr if not found.
 * \return  Returns the position where the phrase starts (relative to the beginning);
 *          areg::INVALID_POS if not found.
 * \note    This is a reverse search, but the returned position is relative to the beginning of
 *          the string.
 **/
template<typename CharType>
[[nodiscard]]
constexpr CharPos find_last( const CharType*  strPhrase
                           , const CharType*  strSource
                           , CharPos          startPos      = areg::END_POS
                           , bool             caseSensitive = true
                           , const CharType** next          = nullptr) noexcept;

/**
 * \brief   Reverse searches for a character within a string, starting from the specified
 *          position and moving toward the beginning.
 *
 * \param   chSearch            The character to search for.
 * \param   strSource           The source string to search in.
 * \param   startPos            The starting position. If END_POS, searches from the end.
 * \param   caseSensitive       If false, ignores case differences in the search.
 * \param[out] next            If not nullptr, on output contains the pointer to the found
 *                                 character; nullptr if not found.
 * \return  Returns the position where the character is found (relative to the beginning);
 *          areg::INVALID_POS if not found.
 * \note    This is a reverse search, but the returned position is relative to the beginning of
 *          the string.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr CharPos find_last( CharType         chSearch
                                   , const CharType*  strSource
                                   , CharPos          startPos      = areg::END_POS
                                   , bool             caseSensitive = true
                                   , const CharType** next          = nullptr) noexcept;

/**
 * \brief   Returns true if the string starts with the specified phrase.
 *
 * \param   strString           The string to check.
 * \param   phrase              The phrase to match.
 * \param   caseSensitive       If false, checks with case insensitivity.
 * \return  Returns true if the string starts with the given phrase.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool string_starts_with(const CharType * strString, const CharType * phrase, bool caseSensitive = true) noexcept;

/**
 * \brief   Returns true if the string starts with the specified character.
 *
 * \param   strString           The string to check.
 * \param   ch                  The character to match.
 * \param   caseSensitive       If false, checks with case insensitivity.
 * \return  Returns true if the string starts with the given character.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool string_starts_with(const CharType * strString, const CharType ch, bool caseSensitive = true) noexcept;

/**
 * \brief   Returns true if the string ends with the specified phrase.
 *
 * \param   strString           The string to check.
 * \param   phrase              The phrase to match.
 * \param   caseSensitive       If false, checks with case insensitivity.
 * \return  Returns true if the string ends with the given phrase.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool string_ends_with(const CharType * strString, const CharType * phrase, bool caseSensitive = true) noexcept;

/**
 * \brief   Returns true if the string ends with the specified character.
 *
 * \param   strString           The string to check.
 * \param   ch                  The character to match.
 * \param   caseSensitive       If false, checks with case insensitivity.
 * \return  Returns true if the string ends with the given character.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool string_ends_with(const CharType * strString, const CharType ch, bool caseSensitive = true) noexcept;

/**
 * \brief   Returns the first printable portion of the string. Requires the buffer to be
 *          modifiable.
 *
 * \param   strSource       The source string to check. The buffer must be modifiable.
 * \param   charCount       The number of characters in the buffer.
 * \param[out] next        If not nullptr, on output contains the pointer after the first
 *                             non-printable character; nullptr if all characters are printable.
 * \return  Returns the first printable portion. Returns empty string if the first character is
 *          not printable; nullptr if the buffer is invalid.
 **/
template<typename CharType>
constexpr const CharType* printable( CharType * strSource, CharCount charCount, CharType ** next = nullptr ) noexcept;

/**
 * \brief   Extracts a single line from the string. Requires the buffer to be modifiable.
 *
 * \param   strSource       The source string to extract from. The buffer must be modifiable.
 * \param   charCount       The number of characters in the buffer.
 * \param[out] next        If not nullptr, on output contains the pointer after the first
 *                             end-of-line character; nullptr if no more lines.
 * \return  Returns the first line. Returns nullptr if the buffer is invalid.
 **/
template<typename CharType>    
const CharType * line( CharType * strSource, CharCount charCount = areg::COUNT_ALL, CharType ** next = nullptr ) noexcept;

/**
 * \brief   Converts the given string to an integer in base 10, ignoring leading white-space.
 *
 * \param   strNumber       The string to convert.
 * \param[out] remain          On output, contains the pointer to the first non-digit character.
 **/
template<typename CharType>
[[nodiscard]]
constexpr int32_t make_integer(const CharType * strNumber, const CharType ** remain) noexcept;

/**
 * \brief   Computes the buffer size required to format a string. Works only for char and
 *          wchar_t types.
 *
 * \param   format      The formatting string.
 * \param   argptr      The pointer to the argument list matching the format.
 * \return  Returns an approximate size (128, 256, 512, 1024) or -1 if the required size exceeds
 *          1024 bytes or on error.
 * \note    This method computes approximate buffer size, not exact size. It checks sizes 128,
 *          256, 512, and 1024 bytes. Use is_buffer_fit() to check for larger sizes.
 **/
template<typename CharType>
[[nodiscard]]
inline int32_t required_char_count( const CharType * format, va_list argptr ) noexcept;

/**
 * \brief   Computes the buffer size required to format a string. Works only for char and
 *          wchar_t types.
 *
 * \param   format      The formatting string.
 * \return  Returns an approximate size (128, 256, 512, 1024) or -1 if the required size exceeds
 *          1024 bytes or on error.
 * \note    This method computes approximate buffer size, not exact size. It checks sizes 128,
 *          256, 512, and 1024 bytes. Use is_buffer_fit() to check for larger sizes.
 **/
template<typename CharType>
[[nodiscard]]
inline int32_t required_char_count(const CharType* format, ...) noexcept;

/**
 * \brief   Checks whether a buffer is large enough to format a string with the given arguments.
 *
 * \param   format      The formatting string.
 * \param   argptr      The pointer to the argument list.
 * \return  Returns true if the buffer has sufficient space to format the string.
 **/
template<int32_t size, char dummy>
[[nodiscard]]
inline bool is_buffer_fit( const char * format, va_list argptr ) noexcept;

/**
 * \brief   Checks whether a buffer is large enough to format a string with the given arguments.
 *
 * \param   format      The formatting string.
 * \param   argptr      The pointer to the argument list.
 * \return  Returns true if the buffer has sufficient space to format the string.
 **/
template<int32_t size, wchar_t dummy>
[[nodiscard]]
inline bool is_buffer_fit( const wchar_t * format, va_list argptr ) noexcept;

/**
 * \brief   Checks whether a buffer is large enough to format a string with the given arguments.
 *
 * \param   format      The formatting string.
 * \return  Returns true if the buffer has sufficient space to format the string.
 **/
template<int32_t size, typename CharType>
[[nodiscard]]
inline bool is_buffer_fit(const CharType* format, ...) noexcept;

/**
 * \brief   Returns true if specified null-terminated string has length not less than the specified `minCount`.
 * \param   str         The null-terminated string to check.
 * \param   minCount    The minimum length of the string to check.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool not_less(const CharType* str, const uint32_t minCount) noexcept;

/**
 * \brief   Returns true if specified null-terminated string has length not more than the specified `maxCount`.
 * \param   str         The null-terminated string to check.
 * \param   maxCount    The maximum length of the string to check.
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool not_more(const CharType* str, const uint32_t maxCount) noexcept;

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// areg namespace function templates or inline methods implementation
//////////////////////////////////////////////////////////////////////////

namespace {

    // Inline character equality with case sensitivity resolved at compile time.
    // Non-capturing; the compiler eliminates the dead branch in each instantiation.
    template<bool CaseSensitive, typename CharType>
    [[nodiscard]] inline constexpr bool char_eq(CharType a, CharType b) noexcept;
    // Inline three-way character comparison with case sensitivity resolved at compile time.
    template<bool CaseSensitive, typename CharLhs, typename CharRhs>
    [[nodiscard]] inline constexpr areg::Ordering char_cmp(CharLhs a, CharRhs b) noexcept;

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    constexpr areg::CharPos find_first_phrase_impl(const CharType* strPhrase, const CharType* strSource, areg::CharPos startPos, const CharType** next) noexcept
    {
        const CharType* str = !areg::is_empty(strSource) ? strSource + startPos : nullptr;
        if (areg::is_empty(str))
            return areg::INVALID_POS;

        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::find(string_view) is constexpr in C++17 and SIMD-optimized at runtime
            // via char_traits — equivalent to strstr/wcsstr without losing constexpr eligibility.
            using SV = std::basic_string_view<CharType>;
            const SV sv{ str };
            const SV phrase_sv{ strPhrase };
            const auto pos = sv.find(phrase_sv);
            if (pos == SV::npos)
                return areg::INVALID_POS;

            const CharType* after = str + pos + phrase_sv.size();
            if ((next != nullptr) && (*after != static_cast<CharType>(areg::EndOfString)))
                *next = after;

            return static_cast<areg::CharPos>(startPos + static_cast<areg::CharPos>(pos));
        }
        else
        {
            // Generic path: case-insensitive or non-standard char types.
            while (!areg::is_eos(*str))
            {
                if (char_eq<CaseSensitive>(*str, *strPhrase))
                {
                    const CharType* one = str + 1;
                    const CharType* two = strPhrase + 1;
                    while ((*two != static_cast<CharType>(areg::EndOfString)) && char_eq<CaseSensitive>(*one, *two))
                    {
                        ++one; ++two;
                    }

                    if (areg::is_eos(*two))
                    {
                        if ((next != nullptr) && (*one != static_cast<CharType>(areg::EndOfString)))
                            *next = one;
                        return static_cast<areg::CharPos>(str - strSource);
                    }
                }

                ++str;
            }

            return areg::INVALID_POS;
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    constexpr bool string_starts_with_impl(const CharType* strString, const CharType* phrase) noexcept
    {
        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::compare() is constexpr in C++17 and SIMD-optimized at runtime
            // via char_traits — equivalent to strncmp/wcsncmp without losing constexpr eligibility.
            using SV = std::basic_string_view<CharType>;
            const SV phrase_sv{ phrase };
            const SV str_sv{ strString };
            return str_sv.compare(0, phrase_sv.size(), phrase_sv) == 0;
        }
        else
        {
            while (!areg::is_eos(*phrase) && !areg::is_eos(*strString))
            {
                if (!char_eq<CaseSensitive>(*strString++, *phrase++))
                    return false;
            }

            return areg::is_eos(*phrase);
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    CharType* remove_char_impl(CharType ch1, const CharType* src, CharType* dst, bool removeAll) noexcept
    {
        if constexpr (CaseSensitive && std::is_same_v<CharType, char>)
        {
            // Fast path: use memchr to skip to each matching character, then memmove the
            // gap between consecutive matches in bulk.
            const char* p = src;
            char* out = dst;
            while (true)
            {
                const char* found = static_cast<const char*>(std::memchr(p, static_cast<int>(ch1), std::strlen(p)));
                if (found == nullptr)
                {
                    // No more matches — copy tail.
                    const std::size_t tail = static_cast<std::size_t>(std::strlen(p));
                    if (tail > 0)
                        std::memmove(out, p, tail);
                    out += tail;
                    break;
                }

                const std::size_t chunk = static_cast<std::size_t>(found - p);
                if (chunk > 0)
                    std::memmove(out, p, chunk);
                out += chunk;
                p = found + 1;  // skip the matched character

                if (!removeAll)
                {
                    // Copy the remainder verbatim so the string stays valid, then
                    // return the position right after the removed character.
                    // The caller uses this as the starting point for the next search.
                    const std::size_t tail = static_cast<std::size_t>(std::strlen(p));
                    char* resume = out;
                    if (tail > 0)
                        std::memmove(out, p, tail);
                    out[tail] = '\0';
                    return resume;
                }
            }

            CharType* result = out;
            *out = '\0';
            return result;
        }
        else if constexpr (CaseSensitive && std::is_same_v<CharType, wchar_t>)
        {
            const wchar_t* p = src;
            wchar_t* out = dst;
            while (true)
            {
                const wchar_t* found = static_cast<const wchar_t*>(std::wmemchr(p, static_cast<wchar_t>(ch1), std::wcslen(p)));
                if (found == nullptr)
                {
                    const std::size_t tail = static_cast<std::size_t>(std::wcslen(p));
                    if (tail > 0)
                        std::wmemmove(out, p, tail);
                    out += tail;
                    break;
                }

                const std::size_t chunk = static_cast<std::size_t>(found - p);
                if (chunk > 0)
                    std::wmemmove(out, p, chunk);
                out += chunk;
                p = found + 1;

                if (!removeAll)
                {
                    // Copy the remainder verbatim so the string stays valid, then
                    // return the position right after the removed character.
                    const std::size_t tail = static_cast<std::size_t>(std::wcslen(p));
                    wchar_t* resume = out;
                    if (tail > 0)
                        std::wmemmove(out, p, tail);
                    out[tail] = L'\0';
                    return resume;
                }
            }

            CharType* result = out;
            *out = L'\0';
            return result;
        }
        else
        {
            while (!areg::is_eos(*src))
            {
                if (char_eq<CaseSensitive>(ch1, *src))
                {
                    ++src;
                    if (!removeAll)
                        break;
                }
                else
                {
                    *dst++ = *src++;
                }
            }

            CharType* result = dst;
            while (!areg::is_eos(*src))
                *dst++ = *src++;
            *dst = static_cast<CharType>(areg::EndOfString);
            return result;
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharLhs, typename CharRhs>
    areg::Ordering compare_strings_impl(const CharLhs* left_side, const CharRhs* right_side, areg::CharPos charCount) noexcept
    {
        const bool countAll{ charCount == areg::COUNT_ALL };
        while (countAll || charCount-- > 0)
        {
            const areg::Ordering cmp{ char_cmp<CaseSensitive>(*left_side, *right_side) };
            if (cmp != areg::Ordering::Equal)
                return cmp;

            if (areg::is_eos(*left_side))
                return areg::Ordering::Equal;

            ++left_side;
            ++right_side;
        }
        return areg::Ordering::Equal;
    }

    /** --------------------------------------------------- **/
    // Inline character equality with case sensitivity resolved at compile time.
    // Non-capturing; the compiler eliminates the dead branch in each instantiation.
    template<bool CaseSensitive, typename CharType>
    inline constexpr bool char_eq(CharType a, CharType b) noexcept
    {
        if constexpr (CaseSensitive)
            return a == b;
        else
            return areg::make_lower<CharType>(a) == areg::make_lower<CharType>(b);
    }

    /** --------------------------------------------------- **/
    // Inline three-way character comparison with case sensitivity resolved at compile time.
    template<bool CaseSensitive, typename CharLhs, typename CharRhs>
    inline constexpr areg::Ordering char_cmp(CharLhs a, CharRhs b) noexcept
    {
        if constexpr (CaseSensitive)
        {
            const auto rb = static_cast<CharLhs>(b);
            return (a == rb) ? areg::Ordering::Equal : (a < rb ? areg::Ordering::Smaller : areg::Ordering::Bigger);
        }
        else
        {
            const CharLhs la = areg::make_lower<CharLhs>(a);
            const CharLhs lb = static_cast<CharLhs>(areg::make_lower<CharRhs>(b));
            return (la == lb) ? areg::Ordering::Equal : (la < lb ? areg::Ordering::Smaller : areg::Ordering::Bigger);
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    constexpr areg::CharPos find_last_char_impl(CharType ch, const CharType* strSource, areg::CharPos pos, const CharType** next) noexcept
    {
        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::rfind() is constexpr in C++17 and SIMD-optimized at runtime.
            using SV = std::basic_string_view<CharType>;
            const SV sv{ strSource };
            const auto found = sv.rfind(ch, static_cast<std::size_t>(pos));
            if (found == SV::npos)
                return areg::INVALID_POS;

            if (next != nullptr)
                *next = strSource + found;

            return static_cast<areg::CharPos>(found);
        }
        else
        {
            for (const CharType* p = strSource + pos; p >= strSource; --p)
            {
                if (char_eq<CaseSensitive>(*p, ch))
                {
                    if (next != nullptr)
                        *next = p;

                    return static_cast<areg::CharPos>(p - strSource);
                }
            }

            return areg::INVALID_POS;
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    constexpr areg::CharPos find_last_phrase_impl(const CharType* strPhrase, const CharType* strSource, areg::CharPos pos, areg::CharPos lenPhr, const CharType** next) noexcept
    {
        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::rfind() is constexpr in C++17 and SIMD-optimized at runtime.
            // Search start is capped at (pos - lenPhr): the last valid phrase-start position.
            if (static_cast<areg::CharPos>(lenPhr) > pos)
                return areg::INVALID_POS;

            using SV = std::basic_string_view<CharType>;
            const SV sv{ strSource };
            const SV phrase_sv{ strPhrase, static_cast<typename SV::size_type>(lenPhr) };
            const auto found = sv.rfind(phrase_sv, static_cast<typename SV::size_type>(pos - lenPhr));
            if (found == SV::npos)
                return areg::INVALID_POS;

            if (next != nullptr)
                *next = strSource + found;

            return static_cast<areg::CharPos>(found);
        }
        else
        {
            const CharType* end = strSource + pos - 1;
            const CharType* phrase = strPhrase + lenPhr - 1;

            for (; end >= strSource; --end)
            {
                if (char_eq<CaseSensitive>(*end, *phrase))
                {
                    const CharType* one = end - 1;
                    const CharType* two = phrase - 1;
                    while ((one >= strSource) && (two >= strPhrase) && char_eq<CaseSensitive>(*one, *two))
                    {
                        --one; --two;
                    }

                    if (two < strPhrase)
                    {
                        ++one;
                        if (next != nullptr)
                            *next = one;

                        return static_cast<areg::CharPos>(one - strSource);
                    }
                }
            }
            return areg::INVALID_POS;
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    constexpr areg::CharPos find_first_char_impl(CharType ch, const CharType* strSource, areg::CharPos startPos, const CharType** next) noexcept
    {
        const CharType* str = !areg::is_empty(strSource) ? strSource + startPos : nullptr;
        if (areg::is_empty(str))
            return areg::INVALID_POS;

        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::find(char) is constexpr in C++17 and SIMD-optimized at runtime
            // via char_traits::find() — equivalent to strchr/wcschr without losing constexpr eligibility.
            using SV = std::basic_string_view<CharType>;
            const SV sv{ str };
            const auto pos = sv.find(ch);
            if (pos == SV::npos)
                return areg::INVALID_POS;

            if ((next != nullptr) && (pos + 1 < sv.size()))
                *next = str + pos + 1;

            return static_cast<areg::CharPos>(startPos + static_cast<areg::CharPos>(pos));
        }
        else
        {
            while (*str != static_cast<CharType>(areg::EndOfString))
            {
                if (char_eq<CaseSensitive>(*str, ch))
                {
                    const areg::CharPos result = static_cast<areg::CharPos>(str - strSource);
                    ++str;
                    if ((next != nullptr) && (*str != static_cast<CharType>(areg::EndOfString)))
                        *next = str;
                    return result;
                }

                ++str;
            }

            return areg::INVALID_POS;
        }
    }

} // namespace

//////////////////////////////////////////////////////////////////////////
// areg namespace function templates or inline methods implementation
//////////////////////////////////////////////////////////////////////////
/** --------------------------------------------------- **/
inline constexpr uint16_t areg::utf8_char_def(int32_t ch) noexcept
{
    return in_utf8_range(ch) ? areg::UTF8_256_Table[utf8_index(ch)] : static_cast<uint16_t>(areg::CharCategory::Undefined);
}

/** --------------------------------------------------- **/
inline constexpr uint32_t areg::make_lower_char(int32_t ch) noexcept
{
    return in_utf8_range(ch) ? static_cast<uint32_t>(areg::UTF8_256_Table_lower[utf8_index(ch)]) : static_cast<uint32_t>(ch);
}

/** --------------------------------------------------- **/
inline constexpr uint32_t areg::make_upper_char(int32_t ch) noexcept
{
    return in_utf8_range(ch) ? static_cast<uint32_t>(areg::UTF8_256_Table_upper[utf8_index(ch)]) : static_cast<uint32_t>(ch);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_eos(CharType ch) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::EOfS)) != 0);
}

/** --------------------------------------------------- **/
inline constexpr bool areg::in_utf8_range(int32_t ch) noexcept
{
    return (static_cast<uint32_t>(ch) + 128u) <= 383u;
}

/** --------------------------------------------------- **/
inline constexpr uint8_t areg::utf8_index(int32_t ch) noexcept
{
    return static_cast<uint8_t>(ch & 0xFF);
}

/** --------------------------------------------------- **/
template<typename CharType>
void areg::revert_string(CharType* strDst, areg::CharCount charCount /*= areg::COUNT_ALL*/) noexcept
{
    if (areg::is_empty<CharType>(strDst))
        return;

    charCount = (charCount == areg::COUNT_ALL) ? areg::string_length<CharType>(strDst) : charCount;
    if (charCount > 1)
        std::reverse(strDst, strDst + charCount);
}

/** --------------------------------------------------- **/
template<typename CharType, typename IntType>
int32_t areg::make_string( CharType*        strDst
                         , areg::CharCount  charCount
                         , IntType          digit
                         , areg::Radix      radix ) noexcept
{
    static constexpr char _valid[] = "0123456789ABCDEF";

    radix = (radix != areg::Radix::Automatic) ? radix : areg::Radix::Decimal;
    if (radix < areg::Radix::Binary || radix > areg::Radix::Hexadecimal)
        return 0;

    const IntType base = static_cast<IntType>(radix);
    IntType       num  = areg::abs<IntType>(digit);

    if (!areg::is_empty<CharType>(strDst) && (charCount > 1))
    {
        // Writing path
        CharType* dst = strDst;
        --charCount; // reserve space for EOS

        do
        {
            *dst++ = static_cast<CharType>(_valid[num % base]);
            num /= base;
            --charCount;
        } while ((num != 0) && (charCount > 0));

        if ((charCount > 0) && (digit < 0) && (radix == areg::Radix::Decimal))
            *dst++ = static_cast<CharType>('-');

        *dst = static_cast<CharType>(areg::EndOfString);
        areg::revert_string<CharType>(strDst);
        return static_cast<int32_t>(dst - strDst);
    }
    else
    {
        int32_t result = (digit < 0 && radix == areg::Radix::Decimal) ? 1 : 0;
        do
        {
            ++result;
            num /= base;
        } while (num != 0);

        return result;
    }
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr int32_t areg::make_integer(const CharType* strNumber, const CharType** remain) noexcept
{
    if (remain != nullptr)
        *remain = nullptr;

    if (areg::is_empty<CharType>(strNumber))
        return 0;

    const CharType negative = areg::as_char(areg::NumericSign::Negative);
    const CharType positive = areg::as_char(areg::NumericSign::Positive);

    areg::NumericSign sign = areg::NumericSign::Undefined;
    uint32_t result = 0;

    for (; *strNumber != static_cast<CharType>(areg::EndOfString); ++strNumber)
    {
        const CharType ch = *strNumber;

        if (areg::is_whitespace<CharType>(ch))
            continue;

        if (sign == areg::NumericSign::Undefined)
        {
            sign = (ch == negative) ? areg::NumericSign::Negative : areg::NumericSign::Positive;
            if ((ch == negative) || (ch == positive))
                continue;
        }

        if (areg::is_numeric<CharType>(ch))
        {
            result = result * 10u + static_cast<uint32_t>(ch - static_cast<CharType>('0'));
        }
        else
        {
            break;
        }
    }

    if ((remain != nullptr) && !areg::is_eos<CharType>(*strNumber))
        *remain = strNumber;

    return static_cast<int32_t>(sign) * static_cast<int32_t>(result);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline int32_t areg::required_char_count( const CharType * format, va_list argptr ) noexcept
{
    if ( areg::is_buffer_fit< areg::MSG_MIN_BUF_SIZE, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0' >(format, argptr))
    {
        return areg::MSG_MIN_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_BUF_SIZE, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0' >( format, argptr ) )
    {
        return areg::MSG_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_BIG_BUF_SIZE, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0' >( format, argptr ) )
    {
        return areg::MSG_BIG_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_EXTRA_BUF_SIZE, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0' >( format, argptr ) )
    {
        return areg::MSG_EXTRA_BUF_SIZE;
    }

    return -1;
}

/** --------------------------------------------------- **/
template<typename CharType>
int32_t areg::required_char_count(const CharType* format, ...) noexcept
{
    va_list argptr;
    va_start(argptr, format);
    int32_t charCount = areg::required_char_count(format, argptr);
    va_end(argptr);
    return charCount;
}

/** --------------------------------------------------- **/
template<int32_t size, char dummy>
inline bool areg::is_buffer_fit( const char * format, va_list argptr ) noexcept
{
    char buf[ size ]{ 0 };
    va_list argcopy;
    va_copy( argcopy, argptr );
    int32_t charCount = vsnprintf( buf, size, format, argcopy );
    va_end( argcopy );

    return (charCount >= 0) && (charCount < size);
}

/** --------------------------------------------------- **/
template<int32_t size, wchar_t dummy>
inline bool areg::is_buffer_fit( const wchar_t * format, va_list argptr ) noexcept
{
    wchar_t buf[ size ]{ 0 };
    va_list argcopy;
    va_copy( argcopy, argptr );
    int32_t charCount = vswprintf( buf, size, format, argcopy );
    va_end( argcopy );

    return (charCount >= 0) && (charCount < size);
}

template<int32_t size, typename CharType>
inline bool areg::is_buffer_fit(const CharType* format, ... ) noexcept
{
    va_list args;
    va_start(args, format);
    bool result = is_buffer_fit<size, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0'>(format, args);
    va_end(args);

    return result;
}

/** --------------------------------------------------- **/
template<typename CharType>
const CharType* areg::line( CharType*         strSource
                          , areg::CharCount  charCount /*= COUNT_ALL*/
                          , CharType**       next  /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (areg::is_empty<CharType>(strSource))
        return strSource;

    const CharType* result = strSource;

    // Scan to the first end-of-line or non-printable character.
    // \r and \n are end-of-line (is_eol); control characters are non-printable (!is_printable).
    // Note: \r and \n ARE printable in the areg table, so is_eol must be checked separately.
    auto find_eol = [](CharType* s, areg::CharCount n) -> CharType*
    {
        areg::CharCount count = (n == areg::COUNT_ALL) ? areg::VALUE_MAX_INT32 : n;
        while (   (count-- > 0)
               && !areg::is_eos<CharType>(*s)
               && !areg::is_eol<CharType>(*s)
               && areg::is_printable<CharType>(*s))
        {
            ++s;
        }

        return s;
    };

    strSource = find_eol(strSource, charCount);

    if (!areg::is_eos<CharType>(*strSource))
    {
        if (areg::is_dos_eol<CharType>(*strSource, *(strSource + 1)))
            *strSource++ = static_cast<CharType>(areg::EndOfString);  // overwrite \r

        *strSource++ = static_cast<CharType>(areg::EndOfString);       // overwrite \n or \r
    }

    if (next != nullptr)
        *next = areg::is_eos<CharType>(*strSource) ? nullptr : strSource;

    return result;
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr const CharType* areg::printable( CharType*       strSource
                                         , areg::CharCount charCount
                                         , CharType**      next /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (areg::is_empty<CharType>(strSource) || charCount == 0)
        return nullptr;

    const CharType* result = strSource;
    areg::CharCount count  = (charCount == areg::COUNT_ALL) ? std::numeric_limits<areg::CharCount>::max() : charCount;

    while ((count-- > 0) && areg::is_printable<CharType>(*strSource))
        ++strSource;

    if ((next != nullptr) && !areg::is_eos<CharType>(*strSource))
        *next = strSource + 1;

    *strSource = static_cast<CharType>(areg::EndOfString);
    return result;
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr areg::CharPos areg::find_last( CharType         chSearch
                                       , const CharType*  strSource
                                       , areg::CharPos    startPos      /*= areg::END_POS*/
                                       , bool             caseSensitive /*= true*/
                                       , const CharType** next      /*= nullptr*/) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (is_empty<CharType>(strSource) || (chSearch == static_cast<CharType>(areg::EndOfString)))
        return areg::INVALID_POS;

    const areg::CharPos len = areg::string_length<CharType>(strSource);
    const areg::CharPos pos = (startPos == areg::END_POS || startPos >= len) ? len - 1 : startPos;
    if (pos < areg::START_POS)
        return areg::INVALID_POS;

    const CharType ch = caseSensitive ? chSearch : areg::make_lower<CharType>(chSearch);
    return caseSensitive ? find_last_char_impl<true>(ch, strSource, pos, next) : find_last_char_impl<false>(ch, strSource, pos, next);
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr areg::CharPos areg::find_last( const CharType*   strPhrase
                                       , const CharType*   strSource
                                       , areg::CharPos     startPos      /*= areg::END_POS*/
                                       , bool              caseSensitive /*= true*/
                                       , const CharType**  next      /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (is_empty<CharType>(strSource) || is_empty<CharType>(strPhrase))
        return areg::INVALID_POS;

    const areg::CharPos lenSrc = areg::string_length<CharType>(strSource);
    const areg::CharPos lenPhr = areg::string_length<CharType>(strPhrase);
    const areg::CharPos pos    = (startPos == areg::END_POS) ? lenSrc : startPos;

    if (pos <= areg::START_POS || lenPhr <= areg::START_POS)
        return areg::INVALID_POS;

    return caseSensitive ? find_last_phrase_impl<true>(strPhrase, strSource, pos, lenPhr, next) : find_last_phrase_impl<false>(strPhrase, strSource, pos, lenPhr, next);
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr areg::CharPos areg::find_first( CharType          chSearch
                                        , const CharType*   strSource
                                        , areg::CharPos     startPos      /*= areg::START_POS*/
                                        , bool              caseSensitive /*= true*/
                                        , const CharType**  next      /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (areg::is_empty<CharType>(strSource) || (chSearch == static_cast<CharType>(areg::EndOfString)) || startPos < areg::START_POS)
        return areg::INVALID_POS;

    const CharType ch = caseSensitive ? chSearch : areg::make_lower<CharType>(chSearch);
    return caseSensitive ? find_first_char_impl<true>(ch, strSource, startPos, next) : find_first_char_impl<false>(ch, strSource, startPos, next);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr areg::CharPos areg::find_first( const CharType*   strPhrase
                                        , const CharType*   strSource
                                        , areg::CharPos     startPos      /*= areg::START_POS*/
                                        , bool              caseSensitive /*= true*/
                                        , const CharType**  next      /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (areg::is_empty<CharType>(strSource) || areg::is_empty<CharType>(strPhrase) || startPos < areg::START_POS)
        return areg::INVALID_POS;

    return caseSensitive ? find_first_phrase_impl<true>(strPhrase, strSource, startPos, next) : find_first_phrase_impl<false>(strPhrase, strSource, startPos, next);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::string_starts_with(const CharType* strString, const CharType* phrase, bool caseSensitive /*= true*/) noexcept
{
    if (is_empty<CharType>(strString) || is_empty<CharType>(phrase))
        return false;

    return caseSensitive ? string_starts_with_impl<true>(strString, phrase) : string_starts_with_impl<false>(strString, phrase);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::string_starts_with(const CharType* strString, const CharType ch, bool caseSensitive /*= true*/) noexcept
{
    if (is_empty<CharType>(strString))
        return false;

    return caseSensitive ? (*strString == ch) : (areg::make_lower<CharType>(*strString) == areg::make_lower<CharType>(ch));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::string_ends_with(const CharType* strString, const CharType* phrase, bool caseSensitive /*= true*/) noexcept
{
    if (is_empty<CharType>(strString) || is_empty<CharType>(phrase))
        return false;

    const int32_t diff = areg::string_length<CharType>(strString) - areg::string_length<CharType>(phrase);
    return (diff >= 0) && areg::string_starts_with<CharType>(strString + diff, phrase, caseSensitive);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::string_ends_with(const CharType* strString, const CharType ch, bool caseSensitive /*= true*/) noexcept
{
    if (is_empty<CharType>(strString))
        return false;

    const CharType last = strString[areg::string_length<CharType>(strString) - 1];
    return caseSensitive ? (last == ch) : (areg::make_lower<CharType>(last) == areg::make_lower<CharType>(ch));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr CharType* areg::remove_char(const CharType chRemove, CharType* strSource, bool removeAll /*= true*/, bool caseSensitive /*= true*/) noexcept
{
    const CharType ch1 = caseSensitive ? chRemove : areg::make_lower<CharType>(chRemove);
    return caseSensitive
        ? remove_char_impl<true>(ch1, strSource, strSource, removeAll)
        : remove_char_impl<false>(ch1, strSource, strSource, removeAll);
}

/** --------------------------------------------------- **/
template<typename CharDst, typename CharSrc>
constexpr void areg::trim_all( CharDst*           strDst
                    , areg::CharCount   lenDst
                    , const CharSrc*    strSrc
                    , areg::CharCount   lenSrc /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return;

    *strDst = static_cast<CharDst>(areg::EndOfString);
    if (lenDst <= 0 || is_empty<CharSrc>(strSrc))
        return;

    const areg::CharCount len = (lenSrc == areg::COUNT_ALL) ? areg::string_length<CharSrc>(strSrc) : lenSrc;
    if (len == 0)
        return;

    const CharSrc* begin = strSrc;
    const CharSrc* end   = strSrc + len - 1;

    while ((end > begin) && areg::is_whitespace<CharSrc>(*end))
        --end;

    if (!areg::is_whitespace<CharSrc>(*end))
        ++end;               // exclusive end

    while ((begin < end) && areg::is_whitespace<CharSrc>(*begin))
        ++begin;

    CharDst* dst = strDst;
    for (; (begin < end) && (lenDst > 1); --lenDst)
        *dst++ = static_cast<CharDst>(*begin++);

    *dst = static_cast<CharDst>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharType>
void areg::trim_all(CharType* strBuffer, areg::CharCount strLen /*= areg::COUNT_ALL*/) noexcept
{
    if (is_empty<CharType>(strBuffer))
        return;

    const bool all  = (strLen == areg::COUNT_ALL);
    const areg::CharCount len  = all ? areg::string_length<CharType>(strBuffer) : strLen;
    if (len == 0)
        return;

    const CharType* tail  = strBuffer + len;
    CharType*       begin = strBuffer;
    CharType*       end   = strBuffer + len - 1;

    while ((end > begin) && areg::is_whitespace<CharType>(*end))
        --end;

    if (!areg::is_whitespace<CharType>(*end))
        ++end; // exclusive end

    while ((begin < end) && areg::is_whitespace<CharType>(*begin))
        ++begin;

    CharType* dst = strBuffer;
    if (begin != dst)
    {
        const std::size_t copyLen = static_cast<std::size_t>(end - begin);
        if constexpr (std::is_same_v<CharType, char>)
        {
            std::memmove(dst, begin, copyLen);
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            std::wmemmove(dst, begin, copyLen);
        }
        else
        {
            while (begin < end)
                *dst++ = *begin++;
        }
        dst += copyLen;
    }
    else
    {
        dst = end;
    }

    if (!all)
    {
        while (*tail != static_cast<CharType>(areg::EndOfString))
            *dst++ = *tail++;
    }

    *dst = static_cast<CharType>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharDst, typename CharSrc>
constexpr void areg::trim_right( CharDst*          strDst
                     , areg::CharCount  lenDst
                     , const CharSrc*   strSrc
                     , areg::CharCount  lenSrc /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return;

    *strDst = static_cast<CharDst>(areg::EndOfString);

    if (lenDst <= 0 || is_empty<CharSrc>(strSrc))
        return;

    const areg::CharCount len = (lenSrc == areg::COUNT_ALL) ? areg::string_length<CharSrc>(strSrc) : lenSrc;
    if (len == 0)
        return;

    const CharSrc* begin = strSrc;
    const CharSrc* end   = strSrc + len - 1;

    while ((end != strSrc) && areg::is_whitespace<CharSrc>(*end))
        --end;

    if (!areg::is_whitespace<CharSrc>(*end))
        ++end; // exclusive end

    CharDst* dst = strDst;
    for (; (begin < end) && (lenDst > 1); --lenDst)
        *dst++ = static_cast<CharDst>(*begin++);

    *dst = static_cast<CharDst>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr void areg::trim_right(CharType* strBuffer, areg::CharCount strLen /*= areg::COUNT_ALL*/) noexcept
{
    if (is_empty<CharType>(strBuffer))
        return;

    const bool            all = (strLen == areg::COUNT_ALL);
    const areg::CharCount len = all ? areg::string_length<CharType>(strBuffer) : strLen;
    if (len == 0)
        return;

    const CharType* tail  = strBuffer + len;
    CharType* begin = strBuffer;
    CharType* end   = strBuffer + len - 1;

    while ((end > begin) && areg::is_whitespace<CharType>(*end))
        --end;

    CharType* dst = areg::is_whitespace<CharType>(*end) ? end : ++end;
    if (!all)
    {
        while (*tail != static_cast<CharType>(areg::EndOfString))
            *dst++ = *tail++;
    }

    *dst = static_cast<CharType>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharDst, typename CharSrc>
constexpr void areg::trim_left( CharDst*          strDst
                    , areg::CharCount  lenDst
                    , const CharSrc*   strSrc
                    , areg::CharCount  lenSrc /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return;

    *strDst = static_cast<CharDst>(areg::EndOfString);
    if (lenDst <= 0 || is_empty<CharSrc>(strSrc))
        return;

    const areg::CharCount len = (lenSrc == areg::COUNT_ALL) ? areg::string_length<CharSrc>(strSrc) : lenSrc;
    if (len == 0)
        return;

    const CharSrc* begin = strSrc;
    const CharSrc* end   = strSrc + len;

    while ((begin < end) && areg::is_whitespace<CharSrc>(*begin))
        ++begin;

    CharDst* dst = strDst;
    for (; (begin < end) && (lenDst > 1); --lenDst)
        *dst++ = static_cast<CharDst>(*begin++);

    *dst = static_cast<CharDst>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharType>
void areg::trim_left(CharType* strBuffer, areg::CharCount strLen /*= areg::COUNT_ALL*/) noexcept
{
    if (is_empty<CharType>(strBuffer))
        return;

    const bool all  = (strLen == areg::COUNT_ALL);
    const areg::CharCount len  = all ? areg::string_length<CharType>(strBuffer) : strLen;
    if (len == 0)
        return;

    const CharType* tail  = strBuffer + len;
    const CharType* begin = strBuffer;
    const CharType* end   = tail;

    while ((begin < end) && areg::is_whitespace<CharType>(*begin))
        ++begin;

    if (begin != strBuffer)
    {
        const std::size_t copyLen = static_cast<std::size_t>(end - begin);
        CharType* dst = strBuffer;
        if constexpr (std::is_same_v<CharType, char>)
        {
            std::memmove(dst, begin, copyLen);
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            std::wmemmove(dst, begin, copyLen);
        }
        else
        {
            while (begin < end)
                *dst++ = *begin++;
        }
        dst += copyLen;

        if (!all)
        {
            while (*tail != static_cast<CharType>(areg::EndOfString))
                *dst++ = *tail++;
        }

        *dst = static_cast<CharType>(areg::EndOfString);
    }
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_one_of(CharType ch, const CharType* chSequence) noexcept
{
    if ((chSequence == nullptr) || (ch == static_cast<CharType>(EndOfString)))
        return false;

    while (*chSequence != static_cast<CharType>(EndOfString))
    {
        if (*chSequence == ch)
            return true;

        ++chSequence;
    }

    return false;
}

// Constexpr specializations: string_view::find() is constexpr in C++17 and delegates to
// char_traits::find() which is SIMD-optimized at runtime — best of both worlds.
template<>
inline constexpr bool areg::is_one_of<char>(char ch, const char* chSequence) noexcept
{
    if ((chSequence == nullptr) || (ch == static_cast<char>(EndOfString)))
        return false;

    return std::string_view{ chSequence }.find(ch) != std::string_view::npos;
}

template<>
inline constexpr bool areg::is_one_of<wchar_t>(wchar_t ch, const wchar_t* chSequence) noexcept
{
    if ((chSequence == nullptr) || (ch == static_cast<wchar_t>(EndOfString)))
        return false;

    return std::wstring_view{ chSequence }.find(ch) != std::wstring_view::npos;
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_readable( CharType ch ) noexcept
{
    constexpr uint16_t def{ static_cast<uint16_t>(areg::CharCategory::Letter) |
                            static_cast<uint16_t>(areg::CharCategory::Number) |
                            static_cast<uint16_t>(areg::CharCategory::Symbol) };

    return (((areg::utf8_char_def( ch ) & def) != 0) || (ch == ' '));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_printable( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Printable)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_eol( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::EndOfLine)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_carriage_return( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::CarReturn)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_new_line( CharType ch ) noexcept
{
    return (ch == static_cast<CharType>(areg::EndOfLine));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_unix_eol(CharType ch) noexcept
{
    return areg::is_new_line<CharType>(ch);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_unix_eol(const CharType * source) noexcept
{
    return (source != nullptr ? areg::is_unix_eol<CharType>(*source) : false);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_dos_eol(CharType ch1, CharType ch2) noexcept
{
    return (areg::is_carriage_return<CharType>(ch1) && areg::is_unix_eol<CharType>(ch2));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_dos_eol(const CharType * source) noexcept
{
    return (source != nullptr ? areg::is_dos_eol<CharType>(*source, *(source + 1)) : false);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_control(CharType ch) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::Control)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_symbol( CharType ch ) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::Symbol)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_whitespace( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::WhiteSpace)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_delimited( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Delimiter)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_letter(CharType ch) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Letter)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_numeric(CharType ch) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Number)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_alphanumeric(CharType ch) noexcept
{
    constexpr uint16_t def{ static_cast<uint16_t>(areg::CharCategory::Letter) | static_cast<uint16_t>(areg::CharCategory::Number) };

    return ((areg::utf8_char_def( ch ) & def) != 0);
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr CharType areg::make_lower(CharType ch) noexcept
{
    // return ((ch >= 'A') && (ch <= 'Z') ? ch - 'A' + 'a' : ch);
    return static_cast<CharType>(areg::make_lower_char(ch));
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr const CharType* areg::make_lower(CharType* source) noexcept
{
    if (areg::is_empty<CharType>(source))
        return source;

    const CharType* result{ source };
    while (areg::is_eos<CharType>(*source) == false)
    {
        *source = areg::make_lower<CharType>(*source);
        ++source;
    }

    return result;
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr CharType areg::make_upper(CharType ch) noexcept
{
    // return ((ch >= 'a') && (ch <= 'z') ? ch - 'a' + 'A' : ch);
    return static_cast<CharType>(areg::make_upper_char(ch));
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr const CharType* areg::make_upper(CharType* source) noexcept
{
    if (areg::is_empty<CharType>(source))
        return source;

    const CharType* result{ source };
    while (areg::is_eos<CharType>(*source) == false)
    {
        *source = areg::make_upper<CharType>(*source);
        ++source;
    }

    return result;
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_lower(CharType ch) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::LetterLo)) == static_cast<uint16_t>(areg::CharCategory::LetterLo));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_upper(CharType ch) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::LetterUp)) == static_cast<uint16_t>(areg::CharCategory::LetterUp));
}

/** --------------------------------------------------- **/
inline constexpr bool areg::is_position_valid(areg::CharPos pos) noexcept
{
    return (pos >= areg::START_POS);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_empty( const CharType * strBuffer ) noexcept
{
    return (strBuffer == nullptr) || (*strBuffer == static_cast<CharType>(areg::EndOfString));
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr areg::CharCount areg::string_length(const CharType* theString) noexcept
{
    if (areg::is_empty<CharType>(theString))
        return 0;

    // std::char_traits<T>::length() is constexpr since C++17 (P0254R2) for char and wchar_t,
    // and delegates to the same SIMD-optimized strlen/wcslen at runtime — no overhead.
    if constexpr (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>)
    {
        return static_cast<areg::CharCount>(std::char_traits<CharType>::length(theString));
    }
    else
    {
        const CharType* p = theString;
        while (*p != static_cast<CharType>(areg::EndOfString))
            ++p;
        return static_cast<areg::CharCount>(p - theString);
    }
}

/** --------------------------------------------------- **/
template <typename CharType>
inline areg::CharCount areg::string_line_length(const CharType* theString) noexcept
{
    if (areg::is_empty<CharType>(theString))
        return 0;

    auto line_length = [](const CharType* s) -> areg::CharCount
        {
            if constexpr (std::is_same_v<CharType, char>)
            {
                return static_cast<areg::CharCount>(std::strcspn(s, "\r\n"));
            }
            else if constexpr (std::is_same_v<CharType, wchar_t>)
            {
                return static_cast<areg::CharCount>(std::wcscspn(s, L"\r\n"));
            }
            else
            {
                const CharType* p = s;
                while (!areg::is_eol<CharType>(*p) && !areg::is_eos<CharType>(*p))
                    ++p;
                return static_cast<areg::CharCount>(p - s);
            }
        };

    return line_length(theString);
}

/** --------------------------------------------------- **/
template<typename CharDst, typename CharSrc>
areg::CharCount areg::copy_string( CharDst*          strDst
                                 , areg::CharCount   dstSpace
                                 , const CharSrc*    strSrc
                                 , areg::CharCount   charsCopy /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return 0;

    if constexpr (sizeof(CharSrc) == sizeof(CharDst))
    {
        charsCopy = (charsCopy == areg::COUNT_ALL) ? areg::string_length<CharSrc>(strSrc) : charsCopy;
        const areg::CharCount result = areg::mem_copy( strDst
                                                     , static_cast<uint32_t>(dstSpace)  * sizeof(CharDst)
                                                     , strSrc
                                                     , static_cast<uint32_t>(charsCopy) * sizeof(CharSrc) ) / sizeof(CharDst);
        strDst[result] = static_cast<CharDst>(areg::EndOfString);
        return result;
    }
    else
    {
        if (dstSpace <= 0 || strSrc == nullptr)
        {
            strDst[0] = static_cast<CharDst>(areg::EndOfString);
            return 0;
        }

        charsCopy = (charsCopy == areg::COUNT_ALL) ? std::numeric_limits<areg::CharCount>::max() : charsCopy;
        charsCopy = std::min(dstSpace - 1, charsCopy);

        CharDst* dst = strDst;
        while ((*strSrc != static_cast<CharSrc>(areg::EndOfString)) && (charsCopy-- > 0))
            *dst++ = static_cast<CharDst>(*strSrc++);

        *dst = static_cast<CharDst>(areg::EndOfString);
        return static_cast<areg::CharCount>(dst - strDst);
    }
}

/** --------------------------------------------------- **/
template<typename CharType>
areg::CharCount areg::copy_string_fast( CharType*         strDst
                                      , const CharType*   strSrc
                                      , areg::CharCount   charsCopy /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return 0;

    charsCopy = (charsCopy == areg::COUNT_ALL) ? areg::string_length<CharType>(strSrc) : charsCopy;

    auto do_copy = [&]() -> areg::CharCount
    {
        if constexpr (std::is_same_v<CharType, char>)
            std::memcpy(strDst, strSrc, charsCopy);
        else if constexpr (std::is_same_v<CharType, wchar_t>)
            std::wmemcpy(strDst, strSrc, charsCopy);
        else
            areg::mem_copy(strDst, charsCopy * sizeof(CharType), strSrc, charsCopy * sizeof(CharType));

        strDst[charsCopy] = static_cast<CharType>(areg::EndOfString);
        return charsCopy;
    };

    return do_copy();
}

/** --------------------------------------------------- **/
template<typename CharLhs, typename CharRhs>
areg::Ordering areg::compare_strings( const CharLhs*   left_side
                                    , const CharRhs*   right_side
                                    , areg::CharCount  charCount    /*= COUNT_ALL*/
                                    , bool             caseSensitive /*= true*/ ) noexcept
{
    if (reinterpret_cast<const void*>(left_side) == reinterpret_cast<const void*>(right_side))
        return areg::Ordering::Equal;

    if (left_side == nullptr)
        return (right_side != nullptr) ? areg::Ordering::Smaller : areg::Ordering::Equal;

    if (right_side == nullptr)
        return areg::Ordering::Bigger;

    // Fast path: same char type + case-sensitive, SIMD-optimized stdlib
    if constexpr (std::is_same_v<CharLhs, CharRhs>)
    {
        if (caseSensitive)
        {
            int cmp = 0;
            if constexpr (std::is_same_v<CharLhs, char>)
            {
                cmp = (charCount == areg::COUNT_ALL)
                    ? std::strcmp(left_side, right_side)
                    : std::strncmp(left_side, right_side, static_cast<std::size_t>(charCount));
            }
            else if constexpr (std::is_same_v<CharLhs, wchar_t>)
            {
                cmp = (charCount == areg::COUNT_ALL)
                    ? std::wcscmp(left_side, right_side)
                    : std::wcsncmp(left_side, right_side, static_cast<std::size_t>(charCount));
            }

            if (cmp == 0) return areg::Ordering::Equal;
            return (cmp < 0) ? areg::Ordering::Smaller : areg::Ordering::Bigger;
        }
    }

    // Generic path: different char types or case-insensitive
    return caseSensitive
        ? compare_strings_impl<true>(left_side, right_side, charCount)
        : compare_strings_impl<false>(left_side, right_side, charCount);
}

/** --------------------------------------------------- **/
template<typename CharLhs, typename CharRhs>
inline areg::Ordering areg::compare_ignore_case(const CharLhs* left_side, const CharRhs* right_side, areg::CharCount count /*= areg::COUNT_ALL*/) noexcept
{
    return areg::compare_strings<CharLhs, CharRhs>(left_side, right_side, count, false);
}

/** --------------------------------------------------- **/
template<typename CharLhs, typename CharRhs>
inline areg::Ordering areg::compare(const CharLhs* left_side, const CharRhs* right_side) noexcept
{
    return areg::compare_strings<CharLhs, CharRhs>(left_side, right_side, areg::COUNT_ALL, true);
}

/** --------------------------------------------------- **/
template<typename CharLhs, typename CharRhs>
inline areg::Ordering areg::compare(const CharLhs* left_side, const CharRhs* right_side, areg::CharCount count) noexcept
{
    return areg::compare_strings<CharLhs, CharRhs>(left_side, right_side, count, true);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline areg::Ordering areg::compare_fast(const CharType* left_side, const CharType* right_side, areg::CharCount count) noexcept
{
    areg::Ordering result = areg::Ordering::Bigger;
    if (count == areg::COUNT_ALL)
    {
        result = areg::compare_strings<CharType, CharType>(left_side, right_side, areg::COUNT_ALL, true);
    }
    else if (left_side == right_side)
    {
        result = areg::Ordering::Equal;
    }
    else if ((left_side != nullptr) && (right_side != nullptr))
    {
        result = areg::mem_compare(left_side, right_side, static_cast<uint32_t>(count) * sizeof(CharType));
    }
    else if (right_side != nullptr)
    {
        result = areg::Ordering::Smaller;
    }

    return result;
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::not_less(const CharType* str, const uint32_t minCount) noexcept
{
    if (minCount == 0)
        return true;

    if (is_empty(str))
        return false;

    for (uint32_t i = 0u; i < minCount; ++i)
    {
        if (str[i] == static_cast<CharType>(areg::EndOfString))
            return false;
    }

    return true;
}

// Constexpr specializations: char_traits::find() is constexpr in C++17 and delegates to
// memchr/wmemchr (SIMD-optimized) at runtime. Searches for null in first minCount positions —
// if found, the string is shorter than minCount; if not found, it has at least minCount chars.
template<>
inline constexpr bool areg::not_less<char>(const char* str, const uint32_t minCount) noexcept
{
    if (minCount == 0)
        return true;

    if (is_empty(str))
        return false;

    return std::char_traits<char>::find(str, static_cast<std::size_t>(minCount), '\0') == nullptr;
}

template<>
inline constexpr bool areg::not_less<wchar_t>(const wchar_t* str, const uint32_t minCount) noexcept
{
    if (minCount == 0)
        return true;

    if (is_empty(str))
        return false;

    return std::char_traits<wchar_t>::find(str, static_cast<std::size_t>(minCount), L'\0') == nullptr;
}

/** --------------------------------------------------- **/
// Bug fix: original loop ran i < maxCount, missing the null at str[maxCount] for strings of
// exactly maxCount characters. Changed to i <= maxCount to correctly handle that boundary.
template<typename CharType>
inline constexpr bool areg::not_more(const CharType* str, const uint32_t maxCount) noexcept
{
    if (maxCount == 0)
        return is_empty(str);

    if (is_empty(str))
        return true;

    for (uint32_t i = 0u; i <= maxCount; ++i)
    {
        if (str[i] == static_cast<CharType>(areg::EndOfString))
            return true;
    }

    return false;
}

// Constexpr specializations: char_traits::find() is constexpr in C++17, SIMD at runtime.
// Checks maxCount+1 positions so a string of exactly maxCount chars (null at index maxCount)
// correctly returns true — fixes the off-by-one present in naive < maxCount loop.
template<>
inline constexpr bool areg::not_more<char>(const char* str, const uint32_t maxCount) noexcept
{
    if (maxCount == 0)
        return is_empty(str);

    if (is_empty(str))
        return true;

    return std::char_traits<char>::find(str, static_cast<std::size_t>(maxCount) + 1u, '\0') != nullptr;
}

template<>
inline constexpr bool areg::not_more<wchar_t>(const wchar_t* str, const uint32_t maxCount) noexcept
{
    if (maxCount == 0)
        return is_empty(str);

    if (is_empty(str))
        return true;

    return std::char_traits<wchar_t>::find(str, static_cast<std::size_t>(maxCount) + 1u, L'\0') != nullptr;
}

#endif  // AREG_BASE_STRINGDEFS_HPP
