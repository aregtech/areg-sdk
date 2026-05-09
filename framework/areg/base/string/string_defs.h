#ifndef AREG_BASE_STRINGDEFS_H
#define AREG_BASE_STRINGDEFS_H
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/string/string_defs.hpp
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
#include "areg/base/MathDefs.hpp"

#include <stdarg.h>

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
 * \brief   Reverses the order of characters in the given string buffer.
 *
 * \param   strDst          The string buffer to reverse.
 * \param   charCount       The number of characters to reverse. If COUNT_ALL, reverses all
 *                          characters until the null terminator.
 **/
template<typename CharType>
void revert_string(CharType* strDst, areg::CharCount charCount = areg::COUNT_ALL) noexcept;

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
 * \brief   Returns true if the character is a Unix-style new line ("\n").
 *
 * \param   ch      The character in range [-128, 127] to check.
 * \return  Returns true if the character equals "\n".
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_unix_eol(CharType ch) noexcept;

/**
 * \brief   Returns true if the first character of the string is a Unix-style new line ("\n").
 *
 * \param   source      The string to check.
 * \return  Returns true if the string is not null and the first character equals "\n".
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_unix_eol(const CharType * source) noexcept;

/**
 * \brief   Returns true if the two characters form a DOS-style new line ("\r\n").
 *
 * \param   ch1     The first character in range [-128, 127] to check.
 * \param   ch2     The second character in range [-128, 127] to check.
 * \return  Returns true if ch1 and ch2 form the sequence "\r\n".
 **/
template<typename CharType>
[[nodiscard]]
inline constexpr bool is_dos_eol(CharType ch1, CharType ch2) noexcept;

/**
 * \brief   Returns true if the first two characters of the string form a DOS-style new line ("\r\n").
 *
 * \param   source      The string to check.
 * \return  Returns true if the string is not null and the first two characters form "\r\n".
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
 * \brief   Converts the given string to an integer in base 10, ignoring leading white-space.
 *
 * \param   strNumber       The string to convert.
 * \param[out] remain          On output, contains the pointer to the first non-digit character.
 **/
template<typename CharType>
[[nodiscard]]
constexpr int32_t make_integer(const CharType * strNumber, const CharType ** remain) noexcept;

} // namespace areq

#endif // AREG_BASE_STRINGDEFS_H
