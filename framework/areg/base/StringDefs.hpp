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

#include <algorithm>
#include <stdarg.h>
#include <wchar.h>
#include <limits>
#include <string_view>

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

    /**
     * \brief   Character types. Used bits are defining special meaning of characters.
     *          areg::CharCategory
     **/
    enum class CharCategory : uint16_t
    {
          Undefined =   0   //!< bin = 0000 0000 0000 0000, Undefined character type
        , EOfS      =   1   //!< bin = 0000 0000 0000 0001, End-of+string character
        , EndOfLine =   2   //!< bin = 0000 0000 0000 0010, ENd-of-line character
        , CarReturn =   4   //!< bin = 0000 0000 0000 0100, Carriage return character
        , WhiteSpace=   8   //!< bin = 0000 0000 0000 1000, Whitespace character
        , Delimiter =  16   //!< bin = 0000 0000 0001 0000, Syntax Delimiter
        , Control   =  32   //!< bin = 0000 0000 0010 0000, Control key / value
        , Printable =  64   //!< bin = 0000 0000 0100 0000, Printable character or character, which change text layout like space or tab
        , Number    = 128   //!< bin = 0000 0000 1000 0000, Numeric character
        , Symbol    = 256   //!< bin = 0000 0001 0000 0000, Symbol
        , Letter    = 512   //!< bin = 0000 0010 0000 0000, Letter
        , LetterUp  = 1536  //!< bin = 0000 0110 0000 0000, Upper case letter
        , LetterLo  = 2560  //!< bin = 0000 1010 0000 0000, Lower case letter
    };

    /**
     * \brief   Returns the bit-wise value of character definition based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character value to check.
     **/
    AREG_API uint16_t utf8_char_def( int32_t ch );

    /**
     * \brief   Returns the upper case equivalent of a character from the first 256 UTF-8 code page
     *          symbols.
     *
     * \param   ch      The character value to convert.
     **/
    AREG_API uint32_t make_upper_char(int32_t ch);

    /**
     * \brief   Returns the lower case equivalent of a character from the first 256 UTF-8 code page
     *          symbols.
     *
     * \param   ch      The character value to convert.
     **/
    AREG_API uint32_t make_lower_char( int32_t ch );

    //! ASCII locale.
    static const char* const LOCALE_ASCII       = "C";

    //! en_US.UTF8 locale.
    static const char* const LOCALE_US_UTF8     = "en_US.UTF8";

    //! en_US.UTF8 locale.
    static const char* const LOCALE_UTF8        = "C.UTF8";

    //! Currently installed C locale.
    static const char* const LOCALE_DEFAULT   { LOCALE_UTF8 };

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
    int32_t make_string( CharType * strDst, areg::CharCount charCount, IntType digit, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Reverses the order of characters in the given string buffer.
     *
     * \param   strDst          The string buffer to reverse.
     * \param   charCount       The number of characters to reverse. If COUNT_ALL, reverses all
     *                          characters until the null terminator.
     **/
    template<typename CharType>
    void revert_string(CharType * strDst, CharCount charCount = areg::COUNT_ALL);

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
     * \brief   Returns true if the given position is a valid string position.
     *
     * \param   pos     The position to validate.
     **/
    inline bool is_position_valid(areg::CharPos pos);

    /**
     * \brief   Returns true if the given string is empty (nullptr or starts with null terminator).
     *
     * \param   strBuffer       The string buffer to check.
     * \return  Returns true if the string buffer is empty.
     **/
    template<typename CharType>
    inline bool is_empty( const CharType * strBuffer );
    
    /**
     * \brief   Returns the length of the string buffer, counting characters until the null
     *          terminator.
     *
     * \param   theString       The string to measure.
     * \return  Returns the number of characters in the string.
     **/
    template <typename CharType>
    inline CharCount string_length( const CharType * theString );

    /**
     * \brief   Returns the length of the string line, counting characters until the end-of-line or
     *          null terminator.
     *
     * \param   theString       The string to measure.
     * \return  Returns the number of characters in the line.
     **/
    template <typename CharType>
    inline CharCount string_line_length(const CharType* theString);

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
    CharCount copy_string( CharDst * strDst, CharCount dstSpace, const CharSrc * strSrc, CharCount charsCopy = areg::COUNT_ALL );

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
    CharCount copy_string_fast(CharType* strDst, const CharType* strSrc, CharCount charsCopy = areg::COUNT_ALL);

    /**
     * \brief   Converts the given character to lower case based on the first 256 UTF-8 code page
     *          symbols.
     *
     * \param   ch      The character to convert.
     * \return  If the character is in range [-128, 127], returns the lower case equivalent.
     *          Otherwise, returns the same value.
     **/
    template <typename CharType>
    inline CharType make_lower(CharType ch);

    /**
     * \brief   Converts the given null-terminated string to lower case based on the first 256 UTF-8
     *          code page symbols.
     *
     * \param   source      The null-terminated string to convert.
     * \return  Returns the converted string (same buffer as the source parameter).
     **/
    template <typename CharType>
    inline const CharType* make_lower(CharType* source);

    /**
     * \brief   Converts the given character to upper case based on the first 256 UTF-8 code page
     *          symbols.
     *
     * \param   ch      The character to convert.
     * \return  If the character is in range [-128, 127], returns the upper case equivalent.
     *          Otherwise, returns the same value.
     **/
    template <typename CharType>
    inline CharType make_upper(CharType ch);

    /**
     * \brief   Converts the given null-terminated string to upper case based on the first 256 UTF-8
     *          code page symbols.
     *
     * \param   source      The null-terminated string to convert.
     * \return  Returns the converted string (same buffer as the source parameter).
     **/
    template <typename CharType>
    inline const CharType* make_upper(CharType* source);

    /**
     * \brief   Returns true if the character is lower case.
     *
     * \param   ch      The character to check.
     **/
    template<typename CharType>
    inline bool is_lower(CharType ch);

    /**
     * \brief   Returns true if the character is upper case.
     *
     * \param   ch      The character to check.
     **/
    template<typename CharType>
    inline bool is_upper(CharType ch);

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
    areg::Ordering compare_strings( const CharLhs *left_side
                                   , const CharRhs * right_side
                                   , CharCount charCount = areg::END_POS
                                   , bool caseSensitive  = true);

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
    inline areg::Ordering compare_ignore_case( const CharLhs *left_side, const CharRhs * right_side, areg::CharCount count = areg::COUNT_ALL);

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
    inline areg::Ordering compare(const CharLhs* left_side, const CharRhs* right_side, areg::CharCount count);

    /**
     * \brief   Compares two strings until the first null terminator, case-sensitive.
     *
     * \param   left_side       The left-hand side string to compare.
     * \param   right_side      The right-hand side string to compare.
     * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
     *          greater. Valid only for first 256 UTF-8 code page symbols.
     **/
    template<typename CharLhs, typename CharRhs>
    inline areg::Ordering compare(const CharLhs* left_side, const CharRhs* right_side);

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
    inline areg::Ordering compare_fast( const CharType * left_side, const CharType * right_side, areg::CharCount count );

    /**
     * \brief   Returns true if the character is a letter based on the first 256 UTF-8 code page
     *          symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a letter.
     **/
    template<typename CharType>
    inline bool is_letter( CharType ch );

    /**
     * \brief   Returns true if the character is a numeric digit based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is numeric.
     **/
    template<typename CharType>
    inline bool is_numeric( CharType ch );

    /**
     * \brief   Returns true if the character is a letter or digit based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is alphanumeric.
     **/
    template<typename CharType>
    inline bool is_alphanumeric( CharType ch );

    /**
     * \brief   Returns true if the character is a symbol (not a control character) based on the
     *          first 256 UTF-8 code page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a symbol.
     **/
    template<typename CharType>
    inline bool is_symbol( CharType ch );

    /**
     * \brief   Returns true if the character is a white-space based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is white-space.
     **/
    template<typename CharType>
    inline bool is_whitespace( CharType ch);

    /**
     * \brief   Returns true if the character is used as a delimiter in syntax based on the first
     *          256 UTF-8 code page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a syntax delimiter.
     **/
    template<typename CharType>
    inline bool is_delimited( CharType ch);

    /**
     * \brief   Returns true if the character is an end-of-line symbol based on the first 256 UTF-8
     *          code page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is end-of-line.
     **/
    template<typename CharType>
    inline bool is_eol( CharType ch );

    /**
     * \brief   Returns true if the character is a carriage return symbol based on the first 256
     *          UTF-8 code page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a carriage return.
     **/
    template<typename CharType>
    inline bool is_carriage_return( CharType ch );

    /**
     * \brief   Returns true if the character is a new line symbol based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a new line.
     **/
    template<typename CharType>
    inline bool is_new_line( CharType ch );

    /**
     * \brief   Returns true if the character is a Unix-style new line ('\n').
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character equals '\n'.
     **/
    template<typename CharType>
    inline bool is_unix_eol(CharType ch);

    /**
     * \brief   Returns true if the first character of the string is a Unix-style new line ('\n').
     *
     * \param   source      The string to check.
     * \return  Returns true if the string is not null and the first character equals '\n'.
     **/
    template<typename CharType>
    inline bool is_unix_eol(const CharType * source);

    /**
     * \brief   Returns true if the two characters form a DOS-style new line ('\r\n').
     *
     * \param   ch1     The first character in range [-128, 127] to check.
     * \param   ch2     The second character in range [-128, 127] to check.
     * \return  Returns true if ch1 and ch2 form the sequence '\r\n'.
     **/
    template<typename CharType>
    inline bool is_dos_eol(CharType ch1, CharType ch2);

    /**
     * \brief   Returns true if the first two characters of the string form a DOS-style new line
     *          ('\r\n').
     *
     * \param   source      The string to check.
     * \return  Returns true if the string is not null and the first two characters form '\r\n'.
     **/
    template<typename CharType>
    inline bool is_dos_eol(const CharType* source);

    /**
     * \brief   Returns true if the character is a null terminator based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is the null terminator.
     **/
    template<typename CharType>
    inline bool is_eos( CharType ch );

    /**
     * \brief   Returns true if the character is a control symbol based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a control symbol.
     **/
    template<typename CharType>
    inline bool is_control(CharType ch);

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
    inline bool is_readable( CharType ch );

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
    inline bool is_printable( CharType ch );

    /**
     * \brief   Returns true if the character matches one of the characters in the given sequence.
     *
     * \param   ch              The character in range [-128, 127] to check.
     * \param   chSequence      The sequence of characters to check against.
     * \return  Returns true if the character matches at least one entry in the sequence.
     **/
    template<typename CharType>
    inline bool is_one_of(CharType ch, const CharType* chSequence);

    /**
     * \brief   Removes white-space characters from the left-side of the string buffer.
     *
     * \param   strBuffer       The string buffer to trim.
     * \param   strLen          The length of the string, or COUNT_ALL to calculate automatically.
     **/
    template<typename CharType>
    void trim_left( CharType * strBuffer, CharCount strLen = areg::COUNT_ALL );

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
    void trim_left( CharDst *strDst, CharCount lenDst, const CharSrc * strSrc, CharCount lenSrc = areg::COUNT_ALL );

    /**
     * \brief   Removes white-space characters from the right-side of the string buffer.
     *
     * \param   strBuffer       The string buffer to trim.
     * \param   strLen          The length of the string, or COUNT_ALL to calculate automatically.
     **/
    template<typename CharType>
    void trim_right( CharType * strBuffer, CharCount strLen = areg::COUNT_ALL );

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
    void trim_right( CharDst *strDst, CharCount lenDst, const CharSrc * strSrc, CharCount lenSrc = areg::COUNT_ALL );

    /**
     * \brief   Removes white-space characters from both left- and right-sides of the string buffer.
     *
     * \param   strBuffer       The string buffer to trim.
     * \param   strLen          The length of the string, or COUNT_ALL to calculate automatically.
     **/
    template<typename CharType>
    void trim_all( CharType * strBuffer, areg::CharCount strLen = areg::COUNT_ALL );

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
    void trim_all( CharDst *strDst, areg::CharCount lenDst, const CharSrc * strSrc, areg::CharCount lenSrc = areg::COUNT_ALL );

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
    CharType * remove_char(const CharType chRemove, CharType* strSource, bool removeAll = true, bool caseSensitive = true);

    /**
     * \brief   Searches for a phrase within a string, starting at the specified position.
     *
     * \param   strPhrase           The phrase to search for.
     * \param   strSource           The source string to search in.
     * \param   startPos            The starting position. If START_POS, searches from the
     *                              beginning.
     * \param   caseSensitive       If false, ignores case differences in the search.
     * \param[out] out_next            If not nullptr, on output contains the pointer in the source
     *                                 string after the found phrase; nullptr if not found.
     * \return  Returns the position where the phrase starts if found; areg::INVALID_POS
     *          otherwise.
     **/
    template<typename CharType>
    CharPos find_first( const CharType * strPhrase
                     , const CharType * strSource
                     , CharPos startPos             = areg::START_POS
                     , bool caseSensitive           = true
                     , const CharType ** out_next   = nullptr );

    /**
     * \brief   Searches for a character within a string, starting at the specified position.
     *
     * \param   chSearch            The character to search for.
     * \param   strSource           The source string to search in.
     * \param   startPos            The starting position. If START_POS, searches from the
     *                              beginning.
     * \param   caseSensitive       If false, ignores case differences in the search.
     * \param[out] out_next            If not nullptr, on output contains the pointer in the source
     *                                 string after the found character; nullptr if not found.
     * \return  Returns the position where the character is found; areg::INVALID_POS if not
     *          found.
     **/
    template<typename CharType>
    CharPos find_first( CharType chSearch
                     , const CharType * strSource
                     , CharPos startPos             = areg::START_POS
                     , bool caseSensitive           = true
                     , const CharType ** out_next   = nullptr);

    /**
     * \brief   Reverse searches for a phrase within a string, starting from the specified position
     *          and moving toward the beginning.
     *
     * \param   strPhrase           The phrase to search for.
     * \param   strSource           The source string to search in.
     * \param   startPos            The starting position. If END_POS, searches from the end.
     * \param   caseSensitive       If false, ignores case differences in the search.
     * \param[out] out_next            If not nullptr, on output contains the pointer in the source
     *                                 string before the found phrase; nullptr if not found.
     * \return  Returns the position where the phrase starts (relative to the beginning);
     *          areg::INVALID_POS if not found.
     * \note    This is a reverse search, but the returned position is relative to the beginning of
     *          the string.
     **/
    template<typename CharType>
    CharPos find_last( const CharType * strPhrase
                    , const CharType * strSource
                    , CharPos startPos            = areg::END_POS
                    , bool caseSensitive = true
                    , const CharType ** out_next  = nullptr);

    /**
     * \brief   Reverse searches for a character within a string, starting from the specified
     *          position and moving toward the beginning.
     *
     * \param   chSearch            The character to search for.
     * \param   strSource           The source string to search in.
     * \param   startPos            The starting position. If END_POS, searches from the end.
     * \param   caseSensitive       If false, ignores case differences in the search.
     * \param[out] out_next            If not nullptr, on output contains the pointer to the found
     *                                 character; nullptr if not found.
     * \return  Returns the position where the character is found (relative to the beginning);
     *          areg::INVALID_POS if not found.
     * \note    This is a reverse search, but the returned position is relative to the beginning of
     *          the string.
     **/
    template<typename CharType>
    CharPos find_last( CharType chSearch
                    , const CharType * strSource
                    , CharPos startPos            = areg::END_POS
                    , bool caseSensitive = true
                    , const CharType ** out_next  = nullptr);

    /**
     * \brief   Returns true if the string starts with the specified phrase.
     *
     * \param   strString           The string to check.
     * \param   phrase              The phrase to match.
     * \param   caseSensitive       If false, checks with case insensitivity.
     * \return  Returns true if the string starts with the given phrase.
     **/
    template<typename CharType>
    bool string_starts_with(const CharType * strString, const CharType * phrase, bool caseSensitive = true);

    /**
     * \brief   Returns true if the string starts with the specified character.
     *
     * \param   strString           The string to check.
     * \param   ch                  The character to match.
     * \param   caseSensitive       If false, checks with case insensitivity.
     * \return  Returns true if the string starts with the given character.
     **/
    template<typename CharType>
    bool string_starts_with(const CharType * strString, const CharType ch, bool caseSensitive = true);

    /**
     * \brief   Returns true if the string ends with the specified phrase.
     *
     * \param   strString           The string to check.
     * \param   phrase              The phrase to match.
     * \param   caseSensitive       If false, checks with case insensitivity.
     * \return  Returns true if the string ends with the given phrase.
     **/
    template<typename CharType>
    bool string_ends_with(const CharType * strString, const CharType * phrase, bool caseSensitive = true);

    /**
     * \brief   Returns true if the string ends with the specified character.
     *
     * \param   strString           The string to check.
     * \param   ch                  The character to match.
     * \param   caseSensitive       If false, checks with case insensitivity.
     * \return  Returns true if the string ends with the given character.
     **/
    template<typename CharType>
    bool string_ends_with(const CharType * strString, const CharType ch, bool caseSensitive = true);

    /**
     * \brief   Returns the first printable portion of the string. Requires the buffer to be
     *          modifiable.
     *
     * \param   strSource       The source string to check. The buffer must be modifiable.
     * \param   charCount       The number of characters in the buffer.
     * \param[out] out_next        If not nullptr, on output contains the pointer after the first
     *                             non-printable character; nullptr if all characters are printable.
     * \return  Returns the first printable portion. Returns empty string if the first character is
     *          not printable; nullptr if the buffer is invalid.
     **/
    template<typename CharType>
    const CharType * printable( CharType * strSource, CharCount charCount, CharType ** out_next = nullptr );

    /**
     * \brief   Extracts a single line from the string. Requires the buffer to be modifiable.
     *
     * \param   strSource       The source string to extract from. The buffer must be modifiable.
     * \param   charCount       The number of characters in the buffer.
     * \param[out] out_next        If not nullptr, on output contains the pointer after the first
     *                             end-of-line character; nullptr if no more lines.
     * \return  Returns the first line. Returns nullptr if the buffer is invalid.
     **/
    template<typename CharType>    
    const CharType * line( CharType * strSource, CharCount charCount = areg::COUNT_ALL, CharType ** out_next = nullptr );

    /**
     * \brief   Converts the given string to an integer in base 10, ignoring leading white-space.
     *
     * \param   strNumber       The string to convert.
     * \param[out] remain          On output, contains the pointer to the first non-digit character.
     **/
    template<typename CharType>
    int32_t make_integer(const CharType * strNumber, const CharType ** remain);

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
    template<char dummy = '\0'>
    int32_t required_buffer_size( const char * format, va_list argptr );
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
    template<wchar_t dummy = L'\0'>
    int32_t required_buffer_size( const wchar_t * format, va_list argptr );

    /**
     * \brief   Checks whether a buffer is large enough to format a string with the given arguments.
     *
     * \param   format      The formatting string.
     * \param   argptr      The pointer to the argument list.
     * \return  Returns true if the buffer has sufficient space to format the string.
     **/
    template<int32_t size, char dummy>
    bool is_buffer_fit( const char * format, va_list argptr );
    /**
     * \brief   Checks whether a buffer is large enough to format a string with the given arguments.
     *
     * \param   format      The formatting string.
     * \param   argptr      The pointer to the argument list.
     * \return  Returns true if the buffer has sufficient space to format the string.
     **/
    template<int32_t size, wchar_t dummy>
    bool is_buffer_fit( const wchar_t * format, va_list argptr );

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// areg namespace function templates or inline methods implementation
//////////////////////////////////////////////////////////////////////////

template<typename CharType, typename IntType>
int32_t areg::make_string( CharType * strDst, areg::CharCount charCount, IntType digit, areg::Radix radix )
{
    int32_t result = 0;
    IntType num = areg::abs<IntType>(digit);
    if ( (areg::is_empty<CharType>(strDst) == false) && (charCount > 1) )
    {
        static const CharType _valid[]  = {'0', '1', '2', '3', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '\0'};

        CharType * dst = strDst;
        radix = radix != areg::Radix::Automatic ? radix : areg::Radix::Decimal;

        if ((radix >= areg::Radix::Binary) && (radix <= areg::Radix::Hexadecimal) )
        {
            do
            {
                *dst ++ = _valid[num % static_cast<IntType>(radix)];
                num /= static_cast<IntType>(radix);
                -- charCount;
            } while ( (num != 0) && (charCount != 0) );

            if ( (charCount != 0) && (digit < 0) && (radix == areg::Radix::Decimal) )
                *dst ++ = '-';

            *dst = static_cast<CharType>(areg::EndOfString);
            areg::revert_string<CharType>( strDst );
        }

        result = dst - strDst;
    }
    else
    {
        radix = radix != areg::Radix::Automatic ? radix : areg::Radix::Decimal;

        if ( (radix >= areg::Radix::Binary) && (radix <= areg::Radix::Hexadecimal) )
        {
            do
            {
                ++ result;
                num /= static_cast<IntType>(radix);
            } while ( (num != 0) && (charCount != 0) );

            if ( (charCount != 0) && (digit < 0) )
               ++ result;
        }
    }
    
    return result;
}

template<typename CharType>
void areg::revert_string( CharType * strDst, areg::CharCount charCount /*= areg::COUNT_ALL*/ )
{
    if ( areg::is_empty<CharType>( strDst ) == false )
    {
        charCount = charCount == areg::COUNT_ALL ? areg::string_length<CharType>(strDst) : charCount;
        if (charCount > 0)
        {
            CharType * begin    = strDst;
            CharType * end      = strDst + charCount;
            if ( *end == static_cast<CharType>(areg::EndOfString) )
                -- end;

            while (end > begin )
            {
                CharType ch = *begin;
                *begin ++   = *end;
                *end --     = ch;
            }
        }
    }
}

template<typename CharType>
int32_t areg::make_integer(const CharType * strNumber, const CharType ** remain)
{
    areg::NumericSign sign = areg::NumericSign::Undefined;
    uint32_t result = 0;
    if (is_empty<CharType>(strNumber) == false)
    {
        CharType negative = as_char(areg::NumericSign::Negative);
        CharType positive = as_char(areg::NumericSign::Positive);
        for ( CharType ch = *strNumber; *strNumber != static_cast<CharType>(EndOfString); ++ strNumber )
        {
            ch = *strNumber;
            if (is_whitespace<CharType>(ch))
                continue;

            if (sign == areg::NumericSign::Undefined)
            {
                sign = ch == negative ? areg::NumericSign::Negative : areg::NumericSign::Positive;
                if ((ch == negative) || (ch == positive))
                {
                    // the sign is '-' or '+'
                    continue;
                }
            }

            if ( is_numeric(ch) )
            {
                result *= 10u;
                result += static_cast<uint32_t>(ch - static_cast<CharType>('0'));
            }
            else
            {
                break;
            }
        }
    }
    
    if (remain != nullptr)
    {
        *remain = nullptr;
        if (areg::is_empty<CharType>(strNumber) == false)
        {
            *remain = strNumber;
        }
    }
    
    return (static_cast<int32_t>(sign) * static_cast<int32_t>(result));
}

template<char dummy>
int32_t areg::required_buffer_size( const char * format, va_list argptr )
{
    int32_t result{ -1 };

    va_list argcopy;
    va_copy( argcopy, argptr );
    int32_t charCount = vsnprintf( nullptr, 0, format, argcopy );
    va_end( argcopy );

    if ( charCount > 0 )
    {
        if ( charCount < areg::MSG_MIN_BUF_SIZE )
        {
            result = areg::MSG_MIN_BUF_SIZE;
        }
        else if ( charCount < areg::MSG_BUF_SIZE )
        {
            result = areg::MSG_BUF_SIZE;
        }
        else if ( charCount < areg::MSG_BIG_BUF_SIZE )
        {
            result = areg::MSG_BIG_BUF_SIZE;
        }
        else if ( charCount < areg::MSG_EXTRA_BUF_SIZE )
        {
            result = areg::MSG_EXTRA_BUF_SIZE;
        }
    }

    return result;
}

template<wchar_t dummy>
int32_t areg::required_buffer_size( const wchar_t * format, va_list argptr )
{
    int32_t result{ -1 };
    if ( areg::is_buffer_fit< areg::MSG_MIN_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = areg::MSG_MIN_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_MIN_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = areg::MSG_MIN_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = areg::MSG_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_BIG_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = areg::MSG_BIG_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_EXTRA_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = areg::MSG_EXTRA_BUF_SIZE;
    }

    return result;
}

template<int32_t size, char dummy>
bool areg::is_buffer_fit( const char * format, va_list argptr )
{
    char buf[ size ]{ 0 };
    va_list argcopy;
    va_copy( argcopy, argptr );
    int32_t charCount = vsnprintf( buf, size, format, argcopy );
    va_end( argcopy );

    return (charCount < size);
}

template<int32_t size, wchar_t dummy>
bool areg::is_buffer_fit( const wchar_t * format, va_list argptr )
{
    wchar_t buf[ size ]{ 0 };
    va_list argcopy;
    va_copy( argcopy, argptr );
    int32_t charCount = vswprintf( buf, size, format, argcopy );
    va_end( argcopy );

    return (charCount < size);
}

template<typename CharType>
const CharType * areg::line( CharType * strSource, areg::CharCount charCount/*= COUNT_ALL*/, CharType ** out_next /*= nullptr */ )
{
    const CharType* result = strSource;

    if (out_next != nullptr)
        *out_next = nullptr;

    if (areg::is_empty(strSource) == false)
    {
        charCount = charCount == areg::COUNT_ALL ? areg::VALUE_MAX_INT32 : charCount;
        while ((areg::is_eos<CharType>(*strSource) == false) && (charCount > 0))
        {
            if (areg::is_eol<CharType>(*strSource))
            {
                if ( areg::is_dos_eol<CharType>(*strSource, *(strSource + 1)) )
                {
                    *strSource ++ = static_cast<CharType>(areg::EndOfString);
                }

                *strSource ++ = static_cast<CharType>(areg::EndOfString);
                break;
            }

            ++strSource;
        }
    }

    if (out_next != nullptr)
    {
        *out_next = (strSource == nullptr) || areg::is_eos<CharType>(*strSource) ? nullptr : strSource;
    }

    return result;
}

template<typename CharType>
const CharType * areg::printable( CharType * strSource, areg::CharCount charCount, CharType ** out_next /*= nullptr */ )
{
    const CharType * result = nullptr;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( areg::is_empty<CharType>(strSource) == false )
    {
        charCount = charCount == areg::COUNT_ALL ? std::numeric_limits<int32_t>::max() : charCount;
        if ( charCount > 0 )
        {
            result = strSource;

            // move pointer until char is printable
            while ( charCount != 0 )
            {
                if ( areg::is_printable<CharType>(*strSource) == false )
                {
                    // set next position if not EOS.
                    if ( (out_next != nullptr) && (areg::is_eos<CharType>(*strSource) == false) )
                    {
                        *out_next = (strSource + 1);
                    }

                    break;  // break the loop.
                }

                ++ strSource;
                -- charCount;
            }

            // set null-terminated char at the end.
            *strSource = static_cast<CharType>(EndOfString);
        }
    }

    return result;
}

template<typename CharType>
areg::CharPos areg::find_last( CharType   chSearch
                                    , const CharType * strSource
                                    , areg::CharPos startPos /*= areg::END_POS*/
                                    , bool caseSensitive /*= true*/
                                    , const CharType ** out_next /*= nullptr*/ )
{
    areg::CharPos result= areg::INVALID_POS;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (is_empty<CharType>( strSource ) == false) && (chSearch != static_cast<CharType>(areg::EndOfString)) )
    {
        CharPos posSrc = startPos == areg::END_POS ? areg::string_length<CharType>(strSource) - 1 : startPos;
        if ( posSrc >= areg::START_POS )
        {
            const CharType * end = strSource + posSrc;
            if (caseSensitive)
            {
                while (end >= strSource)
                {
                    if (*end == chSearch)
                    {
                        result = static_cast<areg::CharPos>(end - strSource);
                        if ((out_next != nullptr) && (end >= strSource))
                        {
                            *out_next = end;
                        }

                        break;
                    }

                    --end;
                }
            }
            else
            {
                const CharType ch{ areg::make_lower<CharType>(chSearch) };
                while (end >= strSource)
                {
                    if (areg::make_lower<CharType>(*end) == ch)
                    {
                        result = static_cast<areg::CharPos>(end - strSource);
                        if ((out_next != nullptr) && (end >= strSource))
                        {
                            *out_next = end;
                        }

                        break;
                    }

                    --end;
                }
            }
        }
    }

    return result;
}

template<typename CharType>
areg::CharPos areg::find_last( const CharType * strPhrase
                                    , const CharType * strSource
                                    , areg::CharPos startPos /*= areg::END_POS*/
                                    , bool caseSensitive /*= true*/
                                    , const CharType ** out_next /*= nullptr*/ )
{
    areg::CharPos result= areg::INVALID_POS;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (is_empty<CharType>( strSource ) == false) && (is_empty<CharType>( strPhrase ) == false) )
    {
        CharPos posSrc = startPos == areg::END_POS ? areg::string_length<CharType>(strSource) : startPos;
        CharPos posPhr = areg::string_length<CharType>(strPhrase);
        if ( (posSrc > areg::START_POS) && (posPhr > areg::START_POS) )
        {
            const CharType * end    = strSource + posSrc - 1;
            const CharType * phrase = strPhrase + posPhr - 1;

            if (caseSensitive)
            {
                while (end >= strSource)
                {
                    if (*end == *phrase)
                    {
                        const CharType* one = end - 1;
                        const CharType* two = phrase - 1;
                        // no need to check (*one != static_cast<CharType>(EndofString))
                        while ((one >= strSource) && (two >= strPhrase) && (*one == *two))
                        {
                            --one;
                            --two;
                        }

                        if (two < strPhrase)
                        {
                            ++one;
                            result = static_cast<areg::CharPos>(one - strSource);
                            if ((out_next != nullptr) && (one >= strSource))
                                *out_next = one;

                            break; // break the loop
                        }
                    }

                    --end;
                }
            }
            else
            {
                CharType ch1{ areg::make_lower<CharType>(*phrase) };
                while (end >= strSource)
                {
                    CharType ch2{ areg::make_lower<CharType>(*end) };
                    if (ch1 == ch2)
                    {
                        const CharType* one = end - 1;
                        const CharType* two = phrase - 1;
                        // no need to check (*one != static_cast<CharType>(EndofString))
                        while ((one >= strSource) && (two >= strPhrase) && (areg::make_lower<CharType>(*one) == areg::make_lower<CharType>(*two)))
                        {
                            --one;
                            --two;
                        }

                        if (two < strPhrase)
                        {
                            ++one;
                            result = static_cast<areg::CharPos>(one - strSource);
                            if ((out_next != nullptr) && (one >= strSource))
                                *out_next = one;

                            break; // break the loop
                        }
                    }

                    --end;
                }
            }
        }
    }

    return result;
}

template<typename CharType>
areg::CharPos areg::find_first( CharType chSearch
                                     , const CharType * strSource
                                     , areg::CharPos startPos /*= areg::START_POS*/
                                     , bool caseSensitive /*= true*/
                                     , const CharType ** out_next /*= nullptr*/ )
{
    areg::CharPos result = areg::INVALID_POS;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( is_empty<CharType>(strSource) || (chSearch == static_cast<CharType>(areg::EndOfString)) )
    {
        if ( startPos >= areg::START_POS )
        {
            const CharType * next = strSource + startPos;
            if (caseSensitive)
            {
                while (*next != static_cast<CharType>(EndOfString))
                {
                    if (*next == chSearch)
                    {
                        result = static_cast<areg::CharPos>(next - strSource);
                        next += 1;
                        if ((out_next != nullptr) && (*next != static_cast<CharType>(EndOfString)))
                            *out_next = next;

                        break; // break the loop
                    }

                    ++next;
                }
            }
            else
            {
                const CharType ch{ areg::make_lower<CharType>(chSearch) };
                while (*next != static_cast<CharType>(EndOfString))
                {
                    if (areg::make_lower<CharType>(*next) == ch)
                    {
                        result = static_cast<areg::CharPos>(next - strSource);
                        next += 1;
                        if ((out_next != nullptr) && (*next != static_cast<CharType>(EndOfString)))
                            *out_next = next;

                        break; // break the loop
                    }

                    ++next;
                }
            }
        }
    }

    return result;
}

template<typename CharType>
areg::CharPos areg::find_first( const CharType * strPhrase
                                     , const CharType * strSource
                                     , areg::CharPos startPos /*= areg::START_POS*/
                                     , bool caseSensitive /*= true*/
                                     , const CharType ** out_next /*= nullptr*/ )
{
    areg::CharPos result= areg::INVALID_POS;

    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (is_empty<CharType>(strSource) == false) && (is_empty<CharType>(strPhrase) == false) )
    {
        if ( startPos >= areg::START_POS )
        {
            const CharType * next = strSource + startPos;
            if (caseSensitive)
            {
                while (*next != static_cast<CharType>(areg::EndOfString))
                {
                    if (*next == *strPhrase)
                    {
                        const CharType* one = next + 1;
                        const CharType* two = strPhrase + 1;
                        // no need to check (*one != static_cast<CharType>(EndofString))

                        while ((*two != static_cast<CharType>(areg::EndOfString)) && (*one == *two))
                        {
                            ++one;
                            ++two;
                        }

                        if (*two == static_cast<CharType>(areg::EndOfString))
                        {
                            result = static_cast<areg::CharPos>(next - strSource);
                            if ((out_next != nullptr) && (*one != static_cast<CharType>(areg::EndOfString)))
                                *out_next = one;

                            break; // break the loop
                        }
                    }

                    ++next;
                }
            }
            else
            {
                const CharType ch{ areg::make_lower<CharType>(*strPhrase) };
                while (*next != static_cast<CharType>(areg::EndOfString))
                {
                    if (areg::make_lower<CharType>(*next) == ch)
                    {
                        const CharType* one = next + 1;
                        const CharType* two = strPhrase + 1;
                        // no need to check (*one != static_cast<CharType>(EndofString))
                        while ((*two != static_cast<CharType>(areg::EndOfString)) && (areg::make_lower<CharType>(*one) == areg::make_lower<CharType>(*two)))
                        {
                            ++one;
                            ++two;
                        }

                        if (*two == static_cast<CharType>(areg::EndOfString))
                        {
                            result = static_cast<areg::CharPos>(next - strSource);
                            if ((out_next != nullptr) && (*one != static_cast<CharType>(areg::EndOfString)))
                                *out_next = one;

                            break; // break the loop
                        }
                    }

                    ++next;
                }
            }
        }
    }

    return result;
}

template<typename CharType>
bool areg::string_starts_with(const CharType * strString, const CharType * phrase, bool caseSensitive /*= true*/)
{
    bool result{ false };
    if ((is_empty<CharType>(strString) == false) && (is_empty<CharType>(phrase) == false))
    {
        result = true;
        if (caseSensitive)
        {
            for ( ; (*phrase != areg::EndOfString) && (*strString != areg::EndOfString); ++strString, ++phrase)
            {
                if (*strString != *phrase)
                {
                    result = false;
                    break;
                }
            }
        }
        else
        {
            for (; (*phrase != areg::EndOfString) && (*strString != areg::EndOfString); ++strString, ++phrase)
            {
                CharType ch1{ areg::make_lower<CharType>(*strString) };
                CharType ch2{ areg::make_lower<CharType>(*phrase) };
                if (ch1 != ch2)
                {
                    result = false;
                    break;
                }
            }
        }

        result = result && (*phrase == areg::EndOfString);
    }

    return result;
}

template<typename CharType>
bool areg::string_starts_with(const CharType* strString, const CharType ch, bool caseSensitive)
{
    bool result{ false };
    if (is_empty<CharType>(strString) == false)
    {
        if (caseSensitive)
        {
            result = *strString == ch;
        }
        else
        {
            result = areg::make_lower<CharType>(*strString) == areg::make_lower<CharType>(ch);
        }
    }

    return result;
}

template<typename CharType>
bool areg::string_ends_with(const CharType * strString, const CharType * phrase, bool caseSensitive /*= true*/)
{
    bool result{ false };
    if ((is_empty<CharType>(strString) == false) && (is_empty<CharType>(phrase) == false))
    {
        int32_t lenString   = areg::string_length<CharType>(strString);
        int32_t lenPhrase   = areg::string_length<CharType>(phrase);
        
        ASSERT(lenString > 0);
        ASSERT(lenPhrase > 0);

        int32_t diff = lenString - lenPhrase;

        result = (diff >= 0) && areg::string_starts_with<CharType>(strString + diff, phrase, caseSensitive);
    }

    return result;
}

template<typename CharType>
bool areg::string_ends_with(const CharType* strString, const CharType ch, bool caseSensitive /*= true*/)
{
    bool result{ false };
    if (is_empty<CharType>(strString) == false)
    {
        int32_t len = areg::string_length<CharType>(strString);
        ASSERT(len != 0);
        if (caseSensitive)
        {
            result = strString[len - 1] == ch;
        }
        else
        {
            result = areg::make_lower<CharType>(strString[len - 1]) == areg::make_lower<CharType>(ch);
        }
    }

    return result;
}

template<typename CharType>
CharType * areg::remove_char(const CharType chRemove, CharType* strSource, bool removeAll /*= true*/, bool caseSensitive /*= true*/)
{
    CharType* dst = strSource;
    const CharType* src = strSource;

    CharType ch1{ caseSensitive ? chRemove : areg::make_lower<CharType>(chRemove) };

    while (areg::is_eos(*src) == false)
    {
        CharType ch2{ caseSensitive ? *src : areg::make_lower<CharType>(*src) };
        if (ch1 == ch2)
        {
            ++src;
            if (removeAll == false)
                break;
        }
        else
        {
            *dst++ = *src++;
        }
    }

    CharType* result{ dst };
    while (areg::is_eos(*src) == false)
        *dst++ = *src++;

    *dst = static_cast<CharType>(areg::EndOfString);    
    return result;
}

template<typename CharDst, typename CharSrc>
void areg::trim_all( CharDst *            strDst
                      , areg::CharCount  lenDst
                      , const CharSrc *      strSrc
                      , areg::CharCount  lenSrc /*= areg::COUNT_ALL*/ )
{
    if ( (strDst != nullptr) && (lenDst > 0) )
    {
        CharDst * dst = strDst;
        *dst    = static_cast<CharDst>(areg::EndOfString);
        lenSrc  = lenSrc == areg::COUNT_ALL ? areg::string_length<CharSrc>(strSrc) : lenSrc;

        if ( (lenSrc > 0) && (areg::is_empty<CharSrc>(strSrc) == false) )
        {
            const CharSrc * end     = strSrc + lenSrc - 1;
            const CharSrc * begin   = strSrc;

            while ((end > begin) && areg::is_whitespace<CharSrc>(*end) )
                -- end;

            if (areg::is_whitespace<CharSrc>(*end) == false)
                ++end;

            while ( (begin < end) && areg::is_whitespace<CharSrc>(*begin) )
                ++ begin;

            for (; (begin < end) && (lenDst > 1); --lenDst)
                *dst++ = static_cast<CharDst>(*begin++);

            *dst = static_cast<CharDst>(areg::EndOfString);
        }
    }
    else if (strDst != nullptr)
    {
        *strDst = static_cast<CharDst>(areg::EndOfString);
    }
}

template<typename CharType>
void areg::trim_all( CharType * strBuffer, areg::CharCount strLen /*= areg::COUNT_ALL*/ )
{
    if ( areg::is_empty<CharType>( strBuffer ) == false )
    {
        areg::CharCount shift = strLen == areg::COUNT_ALL ? areg::string_length<CharType>( strBuffer ) : strLen;
        if ( shift > 0 )
        {
            CharType * end  = strBuffer + shift - 1;
            CharType * begin= strBuffer;
            CharType * buf  = strBuffer;
            CharType * next = strBuffer + shift;

            while ( (end > begin) && areg::is_whitespace<CharType>(*end) )
                -- end;

            if (areg::is_whitespace<CharType>(*end) == false)
                ++end;

            while ( (begin < end) && areg::is_whitespace<CharType>(*begin) )
                ++ begin;

            if (begin != buf)
            {
                while (begin < end)
                    *buf ++ = *begin ++;
            }
            else
            {
                buf = end;
            }

            if (strLen != areg::COUNT_ALL)
            {
                while (*next != static_cast<CharType>(areg::EndOfString))
                    *buf ++ = *next ++;
            }

            *buf = static_cast<CharType>(areg::EndOfString);
        }
    }
}

template<typename CharDst, typename CharSrc>
void areg::trim_right( CharDst *           strDst
                        , areg::CharCount lenDst
                        , const CharSrc *     strSrc
                        , areg::CharCount lenSrc /*= areg::COUNT_ALL*/ )
{
    if ((areg::is_empty<CharSrc>(strSrc) == false) && (strDst != nullptr) && (lenDst > 0) )
    {
        lenSrc = lenSrc == areg::COUNT_ALL ? areg::string_length<CharSrc>(strSrc) : lenSrc;
        if ( lenSrc > 0 )
        {
            const CharSrc * end   = strSrc + lenSrc - 1;
            const CharSrc * begin = strSrc;
            CharDst * dst         = strDst;

            while ( (end != strSrc) && areg::is_whitespace<CharSrc>(*end) )
                -- end;

            if (areg::is_whitespace<CharSrc>(*end) == false)
                ++ end;

            for ( ; (begin < end) && (lenDst > 1); -- lenDst)
                *dst ++ = static_cast<CharDst>(*begin ++);

            *dst = static_cast<CharDst>(areg::EndOfString);
        }
    }
    else if (strDst != nullptr)
    {
        *strDst = static_cast<CharDst>(areg::EndOfString);
    }
}

template<typename CharType>
void areg::trim_right( CharType * strBuffer, areg::CharCount strLen /*= areg::COUNT_ALL*/ )
{
    if ( areg::is_empty<CharType>( strBuffer ) == false )
    {
        areg::CharCount shift = strLen = strLen == areg::COUNT_ALL ? areg::string_length<CharType>(strBuffer) : strLen;
        if ( shift > 0 )
        {
            CharType * end  = strBuffer + shift - 1;
            CharType * begin= strBuffer;
            CharType * next = strBuffer + shift;

            while ( (end > begin) && areg::is_whitespace<CharType>( *end ) )
                -- end;

            if (strLen == areg::COUNT_ALL)
            {
                if (areg::is_whitespace<CharType>(*end))
                {
                    *(end) = static_cast<CharType>(areg::EndOfString);
                }
                else
                {
                    *(++end) = static_cast<CharType>(areg::EndOfString);
                }
            }
            else
            {
                CharType * dst = areg::is_whitespace<CharType>(*end) ? end : ++end;
                while(*next != static_cast<CharType>(areg::EndOfString))
                    *dst ++ = *next ++;
                *(dst) = static_cast<CharType>(areg::EndOfString);
            }
        }
    }
}

template<typename CharDst, typename CharSrc>
void areg::trim_left( CharDst *           strDst
                       , areg::CharCount lenDst
                       , const CharSrc *     strSrc
                       , areg::CharCount lenSrc /*= areg::COUNT_ALL*/ )
{
    if ((areg::is_empty<CharSrc>(strSrc) == false) && (strDst != nullptr) && (lenDst > 0) )
    {
        lenSrc = lenSrc == areg::COUNT_ALL ? areg::string_length<CharSrc>(strSrc) : lenSrc;
        if ( lenSrc > 0 )
        {
            const CharSrc * begin = strSrc;
            const CharSrc * end   = strSrc + lenSrc;
            CharDst * dst         = strDst;

            while ((end > begin) && areg::is_whitespace<CharSrc>(*begin) )
                ++ begin;

            for (; (begin < end) && (lenDst > 1); --lenDst)
                *dst++ = static_cast<CharDst>(*begin++);

            *dst    = static_cast<CharDst>(areg::EndOfString);
        }
    }
    else if (strDst != nullptr)
    {
        *strDst = static_cast<CharDst>(areg::EndOfString);
    }
}

template<typename CharType>
void areg::trim_left( CharType * strBuffer, areg::CharCount strLen /*= areg::COUNT_ALL*/ )
{
    if ( areg::is_empty<CharType>( strBuffer ) == false )
    {
        areg::CharCount shift = strLen == areg::COUNT_ALL ? areg::string_length<CharType>(strBuffer) : strLen;
        if (shift > 0 )
        {
            const CharType * begin = strBuffer;
            const CharType * end   = strBuffer + shift;
            CharType * buf         = strBuffer;

            while ( (end > begin) && areg::is_whitespace<CharType>(*begin) )
                ++ begin;

            if ( begin != buf )
            {
                while ( begin < end)
                    *buf ++ = *begin ++;

                if (strLen != areg::COUNT_ALL)
                {
                    const CharType* src = strBuffer + strLen;
                    while (*src != static_cast<CharType>(areg::EndOfString))
                        *buf ++ = *src ++;
                }

                *buf = static_cast<CharType>(areg::EndOfString);
            }
        }
    }
}

template<typename CharType>
inline bool areg::is_one_of(CharType ch, const CharType* chSequence)
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

template<typename CharType>
inline bool areg::is_readable( CharType ch )
{
    constexpr uint32_t def{ static_cast<uint32_t>(areg::CharCategory::Letter) |
                            static_cast<uint32_t>(areg::CharCategory::Number) |
                            static_cast<uint32_t>(areg::CharCategory::Symbol) };

    return (((areg::utf8_char_def( ch ) & def) != 0) || (ch == ' '));
}

template<typename CharType>
inline bool areg::is_printable( CharType ch )
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Printable)) != 0);
}

template<typename CharType>
inline bool areg::is_eol( CharType ch )
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::EndOfLine)) != 0);
}

template<typename CharType>
inline bool areg::is_carriage_return( CharType ch )
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::CarReturn)) != 0);
}

template<typename CharType>
bool areg::is_new_line( CharType ch )
{
    return (ch == static_cast<CharType>(areg::EndOfLine));
}

template<typename CharType>
bool areg::is_unix_eol(CharType ch)
{
    return areg::is_new_line<CharType>(ch);
}

template<typename CharType>
bool areg::is_unix_eol(const CharType * source)
{
    return (source != nullptr ? areg::is_unix_eol<CharType>(*source) : false);
}

template<typename CharType>
bool areg::is_dos_eol(CharType ch1, CharType ch2)
{
    return (areg::is_carriage_return<CharType>(ch1) && areg::is_unix_eol<CharType>(ch2));
}

template<typename CharType>
bool areg::is_dos_eol(const CharType * source)
{
    return (source != nullptr ? areg::is_dos_eol<CharType>(*source, *(source + 1)) : false);
}

template<typename CharType>
inline bool areg::is_eos( CharType ch )
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::EOfS)) != 0);
}

template<typename CharType>
inline bool areg::is_control(CharType ch)
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::Control)) != 0);
}

template<typename CharType>
inline bool areg::is_symbol( CharType ch )
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::Symbol)) != 0);
}

template<typename CharType>
inline bool areg::is_whitespace( CharType ch )
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::WhiteSpace)) != 0);
}

template<typename CharType>
inline bool areg::is_delimited( CharType ch )
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Delimiter)) != 0);
}

template<typename CharType>
inline bool areg::is_letter(CharType ch)
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Letter)) != 0);
}

template<typename CharType>
inline bool areg::is_numeric(CharType ch)
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Number)) != 0);
}

template<typename CharType>
inline bool areg::is_alphanumeric(CharType ch)
{
    constexpr uint32_t def{ static_cast<uint32_t>(areg::CharCategory::Letter) | static_cast<uint32_t>(areg::CharCategory::Number) };

    return ((areg::utf8_char_def( ch ) & def) != 0);
}

template <typename CharType>
inline CharType areg::make_lower(CharType ch)
{
    // return ((ch >= 'A') && (ch <= 'Z') ? ch - 'A' + 'a' : ch);
    return static_cast<CharType>(areg::make_lower_char(ch));
}

template <typename CharType>
inline const CharType* areg::make_lower(CharType* source)
{
    const CharType* result{ source };
    if (areg::is_empty<CharType>(source) == false)
    {
        while (areg::is_eos<CharType>(*source) == false)
        {
            *source = areg::make_lower<CharType>(*source);
            ++source;
        }
    }

    return result;
}

template <typename CharType>
inline CharType areg::make_upper(CharType ch)
{
    // return ((ch >= 'a') && (ch <= 'z') ? ch - 'a' + 'A' : ch);
    return static_cast<CharType>(areg::make_upper_char(ch));
}

template <typename CharType>
inline const CharType* areg::make_upper(CharType* source)
{
    const CharType* result{ source };
    if (areg::is_empty<CharType>(source) == false)
    {
        while (areg::is_eos<CharType>(*source) == false)
        {
            *source = areg::make_upper<CharType>(*source);
            ++source;
        }
    }

    return result;
}

template<typename CharType>
inline bool areg::is_lower(CharType ch)
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::LetterLo)) == static_cast<uint16_t>(areg::CharCategory::LetterLo));
}

template<typename CharType>
inline bool areg::is_upper(CharType ch)
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::LetterUp)) == static_cast<uint16_t>(areg::CharCategory::LetterUp));
}

inline bool areg::is_position_valid(areg::CharPos pos)
{
    return (pos >= areg::START_POS);
}

template<typename CharType>
inline bool areg::is_empty( const CharType * strBuffer )
{
    return (strBuffer == nullptr) || (*strBuffer == static_cast<CharType>(areg::EndOfString));
}

template <typename CharType>
inline areg::CharCount areg::string_length( const CharType * theString )
{
    areg::CharCount result = 0;
    if ( areg::is_empty<CharType>(theString) == false )
    {
        result = 1;
        while ( *++theString != static_cast<CharType>(EndOfString) )
        {
            ++ result;
        }
    }

    return result;
}

template <typename CharType>
inline areg::CharCount areg::string_line_length(const CharType* theString)
{
    const CharType* start = theString;
    if (theString != nullptr)
    {
        while(!areg::is_eol<CharType>(*theString) && !areg::is_eos<CharType>(*theString))
        {
            ++theString;
        }
    }

    return static_cast<areg::CharCount>( theString - start );
}

template<typename CharDst, typename CharSrc>
areg::CharCount areg::copy_string( CharDst *           strDst
                                        , areg::CharCount dstSpace
                                        , const CharSrc *     strSrc
                                        , areg::CharCount charsCopy /*= areg::COUNT_ALL*/)
{
    uint32_t result { 0 };

    if constexpr (sizeof(CharSrc) == sizeof(CharDst))
    {
        if ( strDst != nullptr )
        {
            charsCopy = charsCopy == areg::COUNT_ALL ? areg::string_length<CharSrc>( strSrc ) : charsCopy;
            result = areg::mem_copy( strDst, static_cast<uint32_t>(dstSpace) * sizeof(CharDst), strSrc, static_cast<uint32_t>(charsCopy) * sizeof(CharSrc)) / sizeof(CharDst);
            strDst[result] = areg::EndOfString;
        }
    }
    else if ( (dstSpace > 0) && (strDst != nullptr) && (strSrc != nullptr) )
    {
        charsCopy = charsCopy == areg::COUNT_ALL ? std::numeric_limits<int32_t>::max() : charsCopy;
        if ( charsCopy > 0 )
        {
            CharDst * dst = strDst;
            dstSpace -= 1;
            charsCopy = std::min(dstSpace, charsCopy);
            for ( ; (*strSrc != static_cast<CharDst>(EndOfString)) && (charsCopy != 0); -- charsCopy )
                *dst++ = static_cast<CharDst>(*strSrc++);

            *dst = static_cast<CharDst>(EndOfString);
            result = static_cast<uint32_t>(dst - strDst);
        }
    }
    else if (strDst != nullptr)
    {
        strDst[0] = areg::EndOfString;
    }

    return static_cast<areg::CharCount>(result);
}

template<typename CharType>
areg::CharCount areg::copy_string_fast(CharType*            strDst
                                            , const CharType*     strSrc
                                            , areg::CharCount charsCopy /*= areg::COUNT_ALL*/)
{
    uint32_t result { 0 };
    if (strDst != nullptr)
    {
        charsCopy = charsCopy == areg::COUNT_ALL ? areg::string_length<CharType>(strSrc) : charsCopy;
        result = areg::mem_copy(strDst, static_cast<uint32_t>(charsCopy) * sizeof(CharType), strSrc, static_cast<uint32_t>(charsCopy) * sizeof(CharType)) / sizeof(CharType);
        strDst[result] = static_cast<CharType>(areg::EndOfString);
    }

    return static_cast<areg::CharCount>(result);
}

template<typename CharLhs, typename CharRhs>
areg::Ordering areg::compare_strings( const CharLhs *left_side
                                         , const CharRhs * right_side
                                         , areg::CharCount charCount    /*= COUNT_ALL*/
                                         , bool caseSensitive               /*= true*/)
{
    areg::Ordering result = areg::Ordering::Bigger;

    if (reinterpret_cast<const void*>(left_side) == reinterpret_cast<const void*>(right_side))
    {
        result = areg::Ordering::Equal;
    }
    else if ((left_side != nullptr) && (right_side != nullptr))
    {
        if (charCount == areg::COUNT_ALL)
        {
            CharLhs chLeft{ 0 };
            CharRhs chRight{ 0 };
            if (caseSensitive)
            {
                while (true)
                {
                    chLeft = *left_side;
                    chRight = *right_side;
                    if ((chLeft != static_cast<CharLhs>(chRight)) || (chLeft == areg::EndOfString) || (chRight == areg::EndOfString))
                    {
                        break;
                    }

                    ++left_side;
                    ++right_side;
                }
            }
            else
            {
                while (true)
                {
                    chLeft = areg::make_lower<CharLhs>(*left_side);
                    chRight = areg::make_lower<CharRhs>(*right_side);
                    if ((chLeft != static_cast<CharLhs>(chRight)) || (chLeft == areg::EndOfString) || (chRight == areg::EndOfString))
                    {
                        break;
                    }

                    ++left_side;
                    ++right_side;
                }
            }

            if (chLeft == static_cast<CharLhs>(chRight))
                result = areg::Ordering::Equal;
            else if (chLeft < static_cast<CharLhs>(chRight))
                result = areg::Ordering::Smaller;
        }
        else
        {
            CharLhs chLeft{ 0 };
            CharRhs chRight{ 0 };
            if (caseSensitive)
            {
                while (charCount > 0)
                {
                    chLeft = *left_side;
                    chRight = *right_side;
                    if ((chLeft != static_cast<CharLhs>(chRight)) || (chLeft == areg::EndOfString) || (chRight == areg::EndOfString))
                    {
                        break;
                    }

                    ++left_side;
                    ++right_side;
                    --charCount;
                }
            }
            else
            {
                while (charCount > 0)
                {
                    chLeft = areg::make_lower<CharLhs>(*left_side);
                    chRight = areg::make_lower<CharRhs>(*right_side);
                    if ((chLeft != static_cast<CharLhs>(chRight)) || (chLeft == areg::EndOfString) || (chRight == areg::EndOfString))
                    {
                        break;
                    }

                    ++left_side;
                    ++right_side;
                    --charCount;
                }
            }

            if (charCount == 0)
                result = areg::Ordering::Equal;
            else if (chLeft < static_cast<CharLhs>(chRight))
                result = areg::Ordering::Smaller;
        }
    }
    else if ( right_side != nullptr )
    {
        result = areg::Ordering::Smaller;
    }

    return result;
}

template<typename CharLhs, typename CharRhs>
inline areg::Ordering areg::compare_ignore_case(const CharLhs* left_side, const CharRhs* right_side, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    return areg::compare_strings<CharLhs, CharRhs>(left_side, right_side, count, false);
}

template<typename CharLhs, typename CharRhs>
inline areg::Ordering areg::compare(const CharLhs* left_side, const CharRhs* right_side)
{
    return areg::compare_strings<CharLhs, CharRhs>(left_side, right_side, areg::COUNT_ALL, true);
}

template<typename CharLhs, typename CharRhs>
inline areg::Ordering areg::compare(const CharLhs* left_side, const CharRhs* right_side, areg::CharCount count)
{
    return areg::compare_strings<CharLhs, CharRhs>(left_side, right_side, count, true);
}

template<typename CharType>
inline areg::Ordering areg::compare_fast(const CharType* left_side, const CharType* right_side, areg::CharCount count)
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

#endif  // AREG_BASE_STRINGDEFS_HPP
