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
#include "areg/base/GEGlobal.h"
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
namespace NEString
{
    /**
     * \brief   NEString::CharPos, Definition of character position type in string
     **/
    using CharPos   = NECommon::ElemPos;
    /**
     * \brief   NEString::CharCount, Definition of character counting type in string
     **/
    using CharCount = NECommon::ElemCount;

    /**
     * \brief   Character types. Used bits are defining special meaning of characters.
     *          NEString::CharCategory
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
     * \brief   Constant, defines count all characters in the string
     **/
    constexpr CharCount COUNT_ALL           { NECommon::COUNT_ALL };
    /**
     * \brief   Invalid position in string buffer
     **/
    constexpr CharPos   INVALID_POS         { NECommon::INVALID_INDEX };
    /**
     * \brief   Start position in string buffer
     **/
    constexpr CharPos   START_POS           { NECommon::FIRST_INDEX };
    /**
     * \brief   Last valid position in string where it is marked 'end of string'
     **/
    constexpr CharPos   END_POS             { NECommon::LAST_INDEX };

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
     * \brief   Converts given integer digit to string. The data is written in given buffer.
     *          If the buffer is nullptr or the charCount is zero, the function calculates and 
     *          returns the spare required to make conversion.
     * \param   strDst      The destination string buffer to write result.
     * \param   charCount   The number of characters allocated in buffer, including null-terminated character at the end of string.
     * \param   digit       The integer digit to convert.
     * \param   radix       The conversion base. If equal to RadixAutomatic, it sets RadixDecimal, which is default value.
     * \return  Returns number of character copied in the buffer, including null-terminated character.
     *          If strDst is nullptr or empty, or charCount is zero, the function calculates and returns the number of characters
     *          required to allocated to make conversion.
     * \tparam  CharType    The type of character. Normally, either char or wchar_t.
     * \tparam  IntType     The type of integer.
     * \remark  Be careful if passing negative value and converting with base, which is not RadixDecimal.
     *          In this case the conversion sets '-' sign at the begin of string, which can be confusing.
     *          If base differs or RadixDecimal, use conversion of unsigned integer.
     **/
    template<typename CharType, typename IntType>
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
    int32_t make_string( CharType * strDst, NEString::CharCount charCount, IntType digit, NEString::Radix radix = NEString::Radix::Decimal );

    /**
     * \brief   Swaps characters in a given string buffer.
     * \param   strDst      The destination string buffer to make swapping.
     * \param   charCount   The number of characters in buffer to swap. If COUNT_ALL, it swaps all characters until end-of-string
     * \tparam  CharType    The type of characters in buffer.
     */
    template<typename CharType>
    /**
     * \brief   Reverses the order of characters in the given string buffer.
     *
     * \param   strDst          The string buffer to reverse.
     * \param   charCount       The number of characters to reverse. If COUNT_ALL, reverses all
     *                          characters until the null terminator.
     **/
    void revert_string(CharType * strDst, CharCount charCount = NEString::COUNT_ALL);

    /**
     * \brief   End of String value
     *          NEString::EndofString
     **/
    constexpr char              EndOfString     { static_cast<char>('\0') };  //!< End of String, signed char

    /**
     * \brief   End of String value
     *          NEString::EndOfLine
     **/
    constexpr char              EndOfLine       { static_cast<char>('\n') };    //!< UNIX End of line character

    /**
     * \brief   Empty 8-bit string
     *          NEString::EmptyStringA
     **/
    constexpr std::string_view  EmptyStringA     { "" };                      //!< Empty 8-bit string

    /**
     * \brief   Empty 16-bit string
     *          NEString::EmptyStringW
     **/
    constexpr std::wstring_view EmptyStringW     { L"" };                     //!< Empty 16-bit string
    
    /**
     * \brief   Returns true if the given position is a valid string position.
     *
     * \param   pos     The position to validate.
     **/
    inline bool is_position_valid(NEString::CharPos pos);

    /**
     * \brief   Returns true if given string is empty.
     *          The string is empty if it is nullptr or starts with end-of-string character.
     * \param   strBuffer   The string buffer to check whether it is empty or not.
     * \return  Returns true if given string buffer is empty.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the given string is empty (nullptr or starts with null terminator).
     *
     * \param   strBuffer       The string buffer to check.
     * \return  Returns true if the string buffer is empty.
     **/
    inline bool is_empty( const CharType * strBuffer );
    
    /**
     * \brief   Returns length of string buffer. The length is calculated until first match of End of String value.
     * \param   theString   The string to count the length.
     * \return  Returns number of characters in the string
     **/
    template <typename CharType>
    /**
     * \brief   Returns the length of the string buffer, counting characters until the null
     *          terminator.
     *
     * \param   theString       The string to measure.
     * \return  Returns the number of characters in the string.
     **/
    inline CharCount string_length( const CharType * theString );

    /**
     * \brief   Returns length of string line. The length is calculated until first match of End-of-Line or End-of-string value.
     * \param   theString   The string to count the length.
     * \return  Returns number of characters in the string
     **/
    template <typename CharType>
    /**
     * \brief   Returns the length of the string line, counting characters until the end-of-line or
     *          null terminator.
     *
     * \param   theString       The string to measure.
     * \return  Returns the number of characters in the line.
     **/
    inline CharCount string_line_length(const CharType* theString);

    /**
     * \brief   Copies string from source to destination string.
     *          The method does not reallocates space for destination string
     *          if there is no enough space available in destination.
     *          characters, the function will no
     * \param   strDst      The destination string to copy characters.
     * \param   dstSpace    The space available at destination string buffer
     * \param   strSrc      The source of string
     * \param   charsCopy   The number of characters to copy.
     *                      By default, it will try to copy all string if there is 
     *                      enough space available at destination.
     * \return  Returns number of characters copied to destination string, which is minimum
     *          of destination space and characters to copy. Returns zero if no character was copied.
     **/
    template<typename CharDst, typename CharSrc>
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
    CharCount copy_string( CharDst * strDst, CharCount dstSpace, const CharSrc * strSrc, CharCount charsCopy = NEString::COUNT_ALL );

    /**
     * \brief   Copies string from source to destination string.
     *          The method does not reallocates space for destination string
     *          if there is no enough space available in destination.
     *          characters, the function will no
     * \param   strDst      The destination string to copy characters.
     * \param   strSrc      The source of string
     * \param   charsCopy   The number of characters to copy.
     *                      By default, it will try to copy all string if there is
     *                      enough space available at destination.
     * \return  Returns number of characters copied to destination string, which is minimum
     *          of destination space and characters to copy. Returns zero if no character was copied.
     **/
    template<typename CharType>
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
    CharCount copy_string_fast(CharType* strDst, const CharType* strSrc, CharCount charsCopy = NEString::COUNT_ALL);

    /**
     * \brief   Converts given character to lower case based on first 256 of UTF-8 code page..
     * \param   ch      The character to convert to lower case.
     * \return  If character is in range [-128 .. 127], it converts character to lower case. 
     *          Otherwise, conversion is ignored and returns same value.
     **/
    template <typename CharType>
    /**
     * \brief   Converts the given character to lower case based on the first 256 UTF-8 code page
     *          symbols.
     *
     * \param   ch      The character to convert.
     * \return  If the character is in range [-128, 127], returns the lower case equivalent.
     *          Otherwise, returns the same value.
     **/
    inline CharType make_lower(CharType ch);

    /**
     * \brief   Converts given null-terminated string to lower case based on first 256 of UTF-8 code page..
     * \param   source  The null-terminated string to convert to lower case.
     * \return  Returns the converted string. The return string and `source` parameter indicate same string buffer.
     **/
    template <typename CharType>
    /**
     * \brief   Converts the given null-terminated string to lower case based on the first 256 UTF-8
     *          code page symbols.
     *
     * \param   source      The null-terminated string to convert.
     * \return  Returns the converted string (same buffer as the source parameter).
     **/
    inline const CharType* make_lower(CharType* source);

    /**
     * \brief   Converts given character to upper case based on first 256 of UTF-8 code page.
     * \param   ch      The character to convert to upper case.
     * \return  If character is in range [-128 .. 127], it converts character to upper case.
     *          Otherwise, conversion is ignored and returns same value.
     **/
    template <typename CharType>
    /**
     * \brief   Converts the given character to upper case based on the first 256 UTF-8 code page
     *          symbols.
     *
     * \param   ch      The character to convert.
     * \return  If the character is in range [-128, 127], returns the upper case equivalent.
     *          Otherwise, returns the same value.
     **/
    inline CharType make_upper(CharType ch);

    /**
     * \brief   Converts given null-terminated string to lower case based on first 256 of UTF-8 code page..
     * \param   source  The null-terminated string to convert to lower case.
     * \return  Returns the converted string. The return string and `source` parameter indicate same string buffer.
     **/
    template <typename CharType>
    /**
     * \brief   Converts the given null-terminated string to upper case based on the first 256 UTF-8
     *          code page symbols.
     *
     * \param   source      The null-terminated string to convert.
     * \return  Returns the converted string (same buffer as the source parameter).
     **/
    inline const CharType* make_upper(CharType* source);

    /**
     * \brief   Returns true if the specified character is lower case.
     * \param   ch      The character to check.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is lower case.
     *
     * \param   ch      The character to check.
     **/
    inline bool is_lower(CharType ch);

    /**
     * \brief   Returns true if the specified character is upper case.
     * \param   ch      The character to check.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is upper case.
     *
     * \param   ch      The character to check.
     **/
    inline bool is_upper(CharType ch);

    /**
     * \brief   Compares 2 given strings (Left and Right side). The function compares 'charCount' characters
     *          in the strings or until end of string is reached. If the number of characters is equal to `NEString::COUNT_ALL`
     *          if compares the complete 2 string.
     *          if 'caseSensitive' is false, function ignores capital and lower cases.
     *          The function is valid only for first 256 character based on UTF-8 code page:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     * \param   left_side        The Left-side string to compare
     * \param   right_side       The Right-side string to compare
     * \param   charCount       The number of characters to compare or until end of string is reached.
     *                          If the number of characters is `NEString::COUNT_ALL`, compares the complete string.
     * \param   caseSensitive   If false, it will ignore upper and lower cases. 
     * \return  The function returns:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     * \tparam  CharLhs     The type of characters on left string. Either `char` or `wchar_t`.
     * \tparam  CharRhs     The type of characters on right string. Either `char` or `wchar_t`.
     * \note    The results are based 'charCount' and 'caseSensitive' parameters.
     **/
    template<typename CharLhs, typename CharRhs>
    /**
     * \brief   Compares two strings up to a specified number of characters or until null terminator
     *          is reached.
     *
     * \param   left_side           The left-hand side string to compare.
     * \param   right_side          The right-hand side string to compare.
     * \param   charCount           The number of characters to compare, or COUNT_ALL to compare
     *                              complete strings.
     * \param   caseSensitive       If false, comparison ignores case differences.
     * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
     *          greater. Valid only for first 256 UTF-8 code page symbols.
     **/
    NEMath::Ordering compare_strings( const CharLhs *left_side
                                   , const CharRhs * right_side
                                   , CharCount charCount = NEString::END_POS
                                   , bool caseSensitive  = true);

    /**
     * \brief   Compares 2 given strings (Left and Right side). The function compares complete string and ignores the upper and lower cases.
     *          The function is valid only for first 256 character based on UTF-8 code page:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     * \param   left_side    The Left-side string to compare.
     * \param   right_side   The Right-side string to compare.
     * \param   count       The number of characters to compare. By default, it compares the complete strings.
     * \return  The function returns:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     * \tparam  CharLhs     The type of characters on left string. Either `char` or `wchar_t`.
     * \tparam  CharRhs     The type of characters on right string. Either `char` or `wchar_t`.
     **/
    template<typename CharLhs, typename CharRhs>
    /**
     * \brief   Compares two strings ignoring case differences.
     *
     * \param   left_side       The left-hand side string to compare.
     * \param   right_side      The right-hand side string to compare.
     * \param   count           The number of characters to compare. By default, compares complete
     *                          strings.
     * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
     *          greater. Valid only for first 256 UTF-8 code page symbols.
     **/
    inline NEMath::Ordering compare_ignore_case( const CharLhs *left_side, const CharRhs * right_side, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Compares 2 string. Compares first count characters.
     *          The function is valid only for first 256 character based on UTF-8 code page:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     * \param   left_side    The Left-side string to compare.
     * \param   right_side   The Right-side string to compare.
     * \param   count       The number of characters to compare.
     * \return  The function returns:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     * \tparam  CharLhs     The type of characters on left string. Either `char` or `wchar_t`.
     * \tparam  CharRhs     The type of characters on right string. Either `char` or `wchar_t`.
     **/
    template<typename CharLhs, typename CharRhs>
    /**
     * \brief   Compares the first count characters of two strings.
     *
     * \param   left_side       The left-hand side string to compare.
     * \param   right_side      The right-hand side string to compare.
     * \param   count           The number of characters to compare.
     * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
     *          greater. Valid only for first 256 UTF-8 code page symbols.
     **/
    inline NEMath::Ordering compare(const CharLhs* left_side, const CharRhs* right_side, NEString::CharCount count);

    /**
     * \brief   Compares 2 string. The comparing is done until first match of null-termination
     *          and it is case sensitive.
     *          The function is valid only for first 256 character based on UTF-8 code page:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     * \param   left_side        The Left-side string to compare
     * \param   right_side       The Right-side string to compare
     * \return  The function returns:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     * \tparam  CharLhs     The type of characters on left string. Either `char` or `wchar_t`.
     * \tparam  CharRhs     The type of characters on right string. Either `char` or `wchar_t`.
     **/
    template<typename CharLhs, typename CharRhs>
    /**
     * \brief   Compares two strings until the first null terminator, case-sensitive.
     *
     * \param   left_side       The left-hand side string to compare.
     * \param   right_side      The right-hand side string to compare.
     * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
     *          greater. Valid only for first 256 UTF-8 code page symbols.
     **/
    inline NEMath::Ordering compare(const CharLhs* left_side, const CharRhs* right_side);

    /**
     * \brief   Fast compares first count symbols of 2 string. The comparing case sensitive.
     *          The function is valid only for first 256 character based on UTF-8 code page:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     * \param   left_side    The Left-side string to compare
     * \param   right_side   The Right-side string to compare
     * \param   count       The number of characters to compare.
     * \return  The function returns:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     * \tparam  CharType    The type of characters. Either `char` or `wchar_t`.
     **/
    template<typename CharType>
    /**
     * \brief   Fast comparison of the first count characters of two strings, case-sensitive.
     *
     * \param   left_side       The left-hand side string to compare.
     * \param   right_side      The right-hand side string to compare.
     * \param   count           The number of characters to compare.
     * \return  Returns -1 if left-side is less than right-side, 0 if equal, 1 if left-side is
     *          greater. Valid only for first 256 UTF-8 code page symbols.
     **/
    inline NEMath::Ordering compare_fast( const CharType * left_side, const CharType * right_side, NEString::CharCount count );

    /**
     * \brief   Returns true if given character is a letter.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is a letter.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a letter based on the first 256 UTF-8 code page
     *          symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a letter.
     **/
    inline bool is_letter( CharType ch );

    /**
     * \brief   Returns true if given character is a number.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is numeric.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a numeric digit based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is numeric.
     **/
    inline bool is_numeric( CharType ch );

    /**
     * \brief   Returns true if given character is a letter or number.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is a letter or numeric.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a letter or digit based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is alphanumeric.
     **/
    inline bool is_alphanumeric( CharType ch );

    /**
     * \brief   Returns true if given character is a symbol, i.e. not a control key
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is a symbol.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a symbol (not a control character) based on the
     *          first 256 UTF-8 code page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a symbol.
     **/
    inline bool is_symbol( CharType ch );

    /**
     * \brief   Returns true if given character is a white-space.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is white-space symbol.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a white-space based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is white-space.
     **/
    inline bool is_whitespace( CharType ch);

    /**
     * \brief   Returns true if given character is used as delimiter in the syntax.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is syntax separator symbol.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is used as a delimiter in syntax based on the first
     *          256 UTF-8 code page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a syntax delimiter.
     **/
    inline bool is_delimited( CharType ch);

    /**
     * \brief	Checks whether the passed single character is end of line or not.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is end of line
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is an end-of-line symbol based on the first 256 UTF-8
     *          code page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is end-of-line.
     **/
    inline bool is_eol( CharType ch );

    /**
     * \brief	Checks whether the passed single character is a carriage return symbol or not.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is carriage return symbol.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a carriage return symbol based on the first 256
     *          UTF-8 code page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a carriage return.
     **/
    inline bool is_carriage_return( CharType ch );

    /**
     * \brief	Checks whether the passed single character is a new line symbol or not.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is carriage return symbol.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a new line symbol based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a new line.
     **/
    inline bool is_new_line( CharType ch );

    /**
     * \brief   Checks whether the specified character is a Unix style 'new line', i.e. the symbol is equal to `'\n'`.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return  Returns `true` if specified character is equal to `'\n'`.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a Unix-style new line ('\n').
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character equals '\n'.
     **/
    inline bool is_unix_eol(CharType ch);

    /**
     * \brief   Checks whether the first character of the specified null-terminated string 
     *          is a Unix style 'new line', i.e. the symbol is equal to `'\n'`.
     * \param   source  The string to check.
     * \return  Returns `true` if specified string is not null and the first character is equal to `'\n'`.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the first character of the string is a Unix-style new line ('\n').
     *
     * \param   source      The string to check.
     * \return  Returns true if the string is not null and the first character equals '\n'.
     **/
    inline bool is_unix_eol(const CharType * source);

    /**
     * \brief   Checks whether the specified characters define DOS style new line, i.e. the symbol is equal to `"\r\n"` sequence.
     * \param   ch1     The character in range [-128 .. 127] to check.
     * \param   ch2     The character in range [-128 .. 127] to check.
     * \return  Returns `true` if specified character `ch1` and `ch2` chars are appropriately are equal to sequence `"\r\n"`.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the two characters form a DOS-style new line ('\r\n').
     *
     * \param   ch1     The first character in range [-128, 127] to check.
     * \param   ch2     The second character in range [-128, 127] to check.
     * \return  Returns true if ch1 and ch2 form the sequence '\r\n'.
     **/
    inline bool is_dos_eol(CharType ch1, CharType ch2);

    /**
     * \brief   Checks whether the first 2 characters of the specified null-terminated string
     *          is DOS style new line, i.e. the symbol is equal to `"\r\n"`.
     * \param   source  The string to check.
     * \return  Returns `true` if specified string is not null and the first two characters are equal to `"\r\n"`.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the first two characters of the string form a DOS-style new line
     *          ('\r\n').
     *
     * \param   source      The string to check.
     * \return  Returns true if the string is not null and the first two characters form '\r\n'.
     **/
    inline bool is_dos_eol(const CharType* source);

    /**
     * \brief	Checks whether the passed single character is an  end of string symbol.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is end of string symbol.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a null terminator based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is the null terminator.
     **/
    inline bool is_eos( CharType ch );

    /**
     * \brief	Checks whether the passed single character is a control symbol.
     *          The checkup is done based on first 256 symbols based on UTF-8 code page.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return	Returns true if character is a control symbol.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is a control symbol based on the first 256 UTF-8 code
     *          page symbols.
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is a control symbol.
     **/
    inline bool is_control(CharType ch);

    /**
     * \brief   Checks whether the given character is readable or not.
     *          Readable are characters, which can be read by human and white-space.
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return  Returns true if character is readable.
     * \tparam  CharType    The type of character (char or wchar_t). Expecting 8-bit or 16-bit character in range 0 - 0xFF
     * \note    The readable and printable characters differ, which readable character can be read without
     *          additional system support. For example, the horizontal and vertical tabs, or carriage return are not considered as readable.
     * \see     is_printable
     **/
    template<typename CharType>
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
    inline bool is_readable( CharType ch );

    /**
     * \brief   Checks whether the given character is printable or not.
     *          Printable are all characters, which can be printed. These include all readable characters, 
     *          plus all other white-spaces like tabs, new-line, carriage return, etc..
     * \param   ch      The character in range [-128 .. 127] to check.
     * \return  Returns true if character is printable.
     * \tparam  CharType    The type of character. Expecting 8-bit or 16-bit character in range 0 - 0xFF
     * \note    The printable characters include readable characters, but not vice-versa. In addition, it includes
     *          all other white-spaces like tabs, new-line, carriage return, etc.
     * \see     is_printable
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character is printable (all readable characters plus additional
     *          white-space characters).
     *
     * \param   ch      The character in range [-128, 127] to check.
     * \return  Returns true if the character is printable.
     * \note    Printable characters include readable characters plus additional white-space such as
     *          tabs, new-line, and carriage return.
     **/
    inline bool is_printable( CharType ch );

    /**
     * \brief   Checks whether the character is one of matches in the sequence.
     * \param   ch          The character in range [-128 .. 127] to check.
     * \param   chSequence  The sequence of characters to have at least one match.
     * \tparam  CharType    The type of character. Expecting 8-bit or 16-bit character in range 0 - 0xFF
     * \return  Returns true if given character matches one of entries in the given sequence.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the character matches one of the characters in the given sequence.
     *
     * \param   ch              The character in range [-128, 127] to check.
     * \param   chSequence      The sequence of characters to check against.
     * \return  Returns true if the character matches at least one entry in the sequence.
     **/
    inline bool is_one_of(CharType ch, const CharType* chSequence);

    /**
     * \brief   In given string buffer removes white-space characters from left-side (from begin).
     * \param   strBuffer   The string buffer to remove white characters.
     * \param   strLen      The length of the string or NEString:COUNT_ALL if the length should be calculated.
     **/
    template<typename CharType>
    /**
     * \brief   Removes white-space characters from the left-side of the string buffer.
     *
     * \param   strBuffer       The string buffer to trim.
     * \param   strLen          The length of the string, or COUNT_ALL to calculate automatically.
     **/
    void trim_left( CharType * strBuffer, CharCount strLen = NEString::COUNT_ALL );

    /**
     * \brief   In removes white-space characters from left-side (from begin) 
     *          and copies the rest to the destination buffer.
     * \param   strDst  The destination buffer to copy string.
     * \param   lenDst  The destination buffer length to copy characters.
     *                  The length should be big enough to copy.
     *                  Otherwise, it will copy only first (lenDst - 1) characters and set end-of-string at the end.
     * \param   strSrc  The source string buffer.
     * \param   lenSrc  The length of the source string or NEString:COUNT_ALL if the length should be calculated.
     **/
    template<typename CharDst, typename CharSrc>
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
    void trim_left( CharDst *strDst, CharCount lenDst, const CharSrc * strSrc, CharCount lenSrc = NEString::COUNT_ALL );

    /**
     * \brief   In given string buffer removes white-space characters from right-side (from end).
     * \param   strBuffer   The string buffer to remove white characters.
     * \param   strLen      The length of the string or NEString:COUNT_ALL if the length should be calculated.
     **/
    template<typename CharType>
    /**
     * \brief   Removes white-space characters from the right-side of the string buffer.
     *
     * \param   strBuffer       The string buffer to trim.
     * \param   strLen          The length of the string, or COUNT_ALL to calculate automatically.
     **/
    void trim_right( CharType * strBuffer, CharCount strLen = NEString::COUNT_ALL );

    /**
     * \brief   In removes white-space characters from right-side (from end) 
     *          and copies the rest to the destination buffer.
     * \param   strDst  The destination buffer to copy string.
     * \param   lenDst  The destination buffer length to copy characters.
     *                  The length should be big enough to copy.
     *                  Otherwise, it will copy only first (lenDst - 1) characters and set end-of-string at the end.
     * \param   strSrc  The source string buffer.
     * \param   lenSrc  The length of the source string or NEString:COUNT_ALL if the length should be calculated.
     **/
    template<typename CharDst, typename CharSrc>
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
    void trim_right( CharDst *strDst, CharCount lenDst, const CharSrc * strSrc, CharCount lenSrc = NEString::COUNT_ALL );

    /**
     * \brief   In given string buffer removes white-space characters from left- and right-side (from begin and end).
     * \param   strBuffer   The string buffer to remove white characters.
     * \param   strLen      The length of the string or NEString:COUNT_ALL if the length should be calculated.
     **/
    template<typename CharType>
    /**
     * \brief   Removes white-space characters from both left- and right-sides of the string buffer.
     *
     * \param   strBuffer       The string buffer to trim.
     * \param   strLen          The length of the string, or COUNT_ALL to calculate automatically.
     **/
    void trim_all( CharType * strBuffer, NEString::CharCount strLen = NEString::COUNT_ALL );

    /**
     * \brief   In removes white-space characters from left- and right-side (from begin and end)
     *          and copies the rest to the destination buffer.
     * \param   strDst  The destination buffer to copy string.
     * \param   lenDst  The destination buffer length to copy characters.
     *                  The length should be big enough to copy.
     *                  Otherwise, it will copy only first (lenDst - 1) characters and set end-of-string at the end.
     * \param   strSrc  The source string buffer.
     * \param   lenSrc  The length of the source string or NEString:COUNT_ALL if the length should be calculated.
     **/
    template<typename CharDst, typename CharSrc>
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
    void trim_all( CharDst *strDst, NEString::CharCount lenDst, const CharSrc * strSrc, NEString::CharCount lenSrc = NEString::COUNT_ALL );

    /**
     * \brief   Removes specified character from the string.
     * 
     * \param   chRemove        The character to search and remove.
     * \param   strSource       The string where the character should be searched and removed.
     * \param   removeAll       Flag, indicating whether it should remove only once or all matches.
     * \param   caseSensitive   Flag, indicating whether the remove character is case sensitive or not.
     *                          If `caseSensitive` is `false` and `removeAll` parameter is `true`, all capital and small
     *                          letters of specified `chRemove` character is removed in the string.
     *                          If `caseSensitive` is false and `removeAll` parameter is `false`, the first matching capital or small
     *                          letter of specified `chRemove` character is removed in the string.
     * \return  Returns one of following values:
     *              1. If `removeAll` is `false`, returns the position of removed character if found.
     *              2. If `removeAll` is `true`, returns end of string, indicating that there is no more character to remove;
     *              3. If did not find any matching, returns end of string, indicating that there is no character to remove.
     **/
    template<typename CharType>
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
    CharType * remove_char(const CharType chRemove, CharType* strSource, bool removeAll = true, bool caseSensitive = true);

    /**
     * \brief   Search a string inside of other string. The search starts at given position in the string.
     *          If startPos is equal to NEString::START_POS, the search starts at the begin of string. Otherwise, searches at given position.
     *          If found the phrase in string, returns the position in buffer where could find character,
     *          which is same as number of characters to skip from begin to match the phrase.
     *          Otherwise, returns NEString::INVALID_POS.
     * \param   strPhrase       The phrase to search in the string.
     * \param   strSource       The source of string to search.
     * \param   startPos        The starting position to search phrase. If NEString::START_POS, starts searching at begin of given string buffer.
     * \param   caseSensitive   The flag, indicating whether the searching should be case sensitive or not.
     *                          If the flag is `true`, the exact match is searched.
     *                          If the flag is `false`, the capital and small letters are ignored.
     * \param   out_next        If the parameter is not nullptr, then:
     *                              - On output, if not nullptr, this contains pointer in source string next after phrase.
     *                              - On output, if nullptr, could not find the phrase.
     *                          If parameter is nullptr, it is ignored.
     * \return  If found the phrase, returns the position in buffer where phrase starts.
     *          If did not find, returns NEString::INVALID_POS
     **/
    template<typename CharType>
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
     * \return  Returns the position where the phrase starts if found; NEString::INVALID_POS
     *          otherwise.
     **/
    CharPos find_first( const CharType * strPhrase
                     , const CharType * strSource
                     , CharPos startPos             = NEString::START_POS
                     , bool caseSensitive           = true
                     , const CharType ** out_next   = nullptr );

    /**
     * \brief   Search the match of given character inside of string. The search starts at given position
     *          If startPos is equal to NEString::START_POS, the search starts at the begin of string. Otherwise, searches at given position.
     *          If found character in string, returns the position in buffer where phrase starts,
     *          which is same as number of characters to skip from begin to match the character.
     *          Otherwise, returns NEString::INVALID_POS.
     * \param   chSearch        The character to search in the string.
     * \param   strSource       The source of string to search.
     * \param   startPos        The starting position to search phrase. If NEString::START_POS, starts searching at begin of given string buffer.
     * \param   caseSensitive   The flag, indicating whether the searching should be case sensitive or not.
     *                          If the flag is `true`, the exact match is searched.
     *                          If the flag is `false`, the capital and small letters are ignored.
     * \param   out_next        If the parameter is not nullptr, then:
     *                              - On output, if not nullptr, this contains pointer in source string next after character.
     *                              - On output, if nullptr, could not find the character.
     *                          If parameter is nullptr, it is ignored.
     * \return  If found the character, returns the position in buffer where met character.
     *          If did not find, returns NEString::INVALID_POS
     **/
    template<typename CharType>
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
     * \return  Returns the position where the character is found; NEString::INVALID_POS if not
     *          found.
     **/
    CharPos find_first( CharType chSearch
                     , const CharType * strSource
                     , CharPos startPos             = NEString::START_POS
                     , bool caseSensitive           = true
                     , const CharType ** out_next   = nullptr);

    /**
     * \brief   Reverse search a string inside of other string. The search starts at the given position and moves to begin of string.
     *          If startPos is equal to NEString::END_POS, the search starts at the end of string. Otherwise, searches at given position.
     *          If found the phrase in string, returns the position in buffer where phrase starts,
     *          which is same as number of characters to skip from begin to match the phrase.
     *          Otherwise, returns NEString::INVALID_POS.
     * \param   strPhrase       The phrase to search in the string.
     * \param   strSource       The source of string to search.
     * \param   startPos        The starting position to search phrase. If NEString::END_POS, starts searching at the end of given string buffer.
     * \param   caseSensitive   The flag, indicating whether the searching should be case sensitive or not.
     *                          If the flag is `true`, the exact match is searched.
     *                          If the flag is `false`, the capital and small letters are ignored.
     * \param   out_next        If the parameter is not nullptr, then:
     *                              - On output, if not nullptr, this contains pointer in source string before phrase starts.
     *                              - On output, if nullptr, could not find the phrase.
     *                          If parameter is nullptr, it is ignored.
     * \return  If found the phrase, returns the position in buffer where phrase starts.
     *          If did not find, returns NEString::INVALID_POS
     * \note    This is reverse search, but the returned position is relevant to begin of string.
     **/
    template<typename CharType>
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
     *          NEString::INVALID_POS if not found.
     * \note    This is a reverse search, but the returned position is relative to the beginning of
     *          the string.
     **/
    CharPos find_last( const CharType * strPhrase
                    , const CharType * strSource
                    , CharPos startPos            = NEString::END_POS
                    , bool caseSensitive = true
                    , const CharType ** out_next  = nullptr);

    /**
     * \brief   Reverse search the match of given character inside of string. The search starts at the given position and moves to begin of string.
     *          If `startPos` is equal to `NEString::END_POS`, the search starts at the end of string. Otherwise, searches at the given position.
     *          If found the character in the string, returns the position (zero-based index) in the give source buffer,
     *          Otherwise, returns `NEString::INVALID_POS`.
     * \param   chSearch        The character to search in the string.
     * \param   strSource       The source of string to search.
     * \param   startPos        The starting position to search phrase. If `NEString::END_POS`, starts reverse searching from the end of given string buffer.
     * \param   caseSensitive   The flag, indicating whether the searching should be case sensitive or not.
     *                          If the flag is `true`, the exact match is searched.
     *                          If the flag is `false`, the capital and small letters are ignored.
     * \param   out_next        It is an optional parameter. If the parameter is not `nullptr`, then:
     *                              - If on output it is not `nullptr`, it indicates to the source of last match of character,
     *                                i.e. it points to the searching character.
     *                              - if on output it is `nullptr`, it indicates that could not find the character.
     *                          This parameter is ignored if `nullptr`.
     * \return  If found the character, returns the position in buffer where met character.
     *          If did not find, returns `NEString::INVALID_POS`.
     * \note    This is reverse search, but the returned position is relevant to begin of string.
     **/
    template<typename CharType>
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
     *          NEString::INVALID_POS if not found.
     * \note    This is a reverse search, but the returned position is relative to the beginning of
     *          the string.
     **/
    CharPos find_last( CharType chSearch
                    , const CharType * strSource
                    , CharPos startPos            = NEString::END_POS
                    , bool caseSensitive = true
                    , const CharType ** out_next  = nullptr);

    /**
     * \brief   Returns true if a give string starts with specified phrase.
     * \param   strString       The string to check the phrase.
     * \param   phrase          The phrase to check.
     * \param   caseSensitive   If false, it with check ignoring upper / lower case. Otherwise, checks exact match.
     * \return  Returns true if the string starts with given phrase.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the string starts with the specified phrase.
     *
     * \param   strString           The string to check.
     * \param   phrase              The phrase to match.
     * \param   caseSensitive       If false, checks with case insensitivity.
     * \return  Returns true if the string starts with the given phrase.
     **/
    bool string_starts_with(const CharType * strString, const CharType * phrase, bool caseSensitive = true);

    /**
     * \brief   Returns true if a give string starts with specified phrase.
     * \param   strString       The string to check the phrase.
     * \param   ch              The character to check.
     * \param   caseSensitive   If false, it with check ignoring upper / lower case. Otherwise, checks exact match.
     * \return  Returns true if the string starts with given phrase.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the string starts with the specified character.
     *
     * \param   strString           The string to check.
     * \param   ch                  The character to match.
     * \param   caseSensitive       If false, checks with case insensitivity.
     * \return  Returns true if the string starts with the given character.
     **/
    bool string_starts_with(const CharType * strString, const CharType ch, bool caseSensitive = true);

    /**
     * \brief   Returns true if a give string ends with specified phrase.
     * \param   strString   The string to check the phrase.
     * \param   phrase      The phrase to check.
     * \param   caseSensitive If false, it with check ignoring upper / lower case. Otherwise, checks exact match.
     * \return  Returns true if the string ends with given phrase.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the string ends with the specified phrase.
     *
     * \param   strString           The string to check.
     * \param   phrase              The phrase to match.
     * \param   caseSensitive       If false, checks with case insensitivity.
     * \return  Returns true if the string ends with the given phrase.
     **/
    bool string_ends_with(const CharType * strString, const CharType * phrase, bool caseSensitive = true);

    /**
     * \brief   Returns true if a give string ends with specified phrase.
     * \param   strString   The string to check the phrase.
     * \param   ch              The character to check.
     * \param   caseSensitive If false, it with check ignoring upper / lower case. Otherwise, checks exact match.
     * \return  Returns true if the string ends with given phrase.
     **/
    template<typename CharType>
    /**
     * \brief   Returns true if the string ends with the specified character.
     *
     * \param   strString           The string to check.
     * \param   ch                  The character to match.
     * \param   caseSensitive       If false, checks with case insensitivity.
     * \return  Returns true if the string ends with the given character.
     **/
    bool string_ends_with(const CharType * strString, const CharType ch, bool caseSensitive = true);

    /**
     * \brief   Returns printable string of given buffer. The buffer should be possible to modify.
     *          Returns end-of-string if the first position of given string is not printable
     *          and returns nullptr if given source is nullptr.
     * \param   strSource   The source of string to make printable. The string buffer should be possible to modify
     * \param   charCount   The number of characters in string buffer.
     * \param   out_next    If not nullptr, on output this contains pointer to next position
     *                      after first match of non-printable character in source buffer.
     *                      If nullptr, there are no more non-printable characters in buffer.
     * \return  Returns portion of printable string if found.
     *          Returns empty string (end-of-string) if there is a non-printable character at first position.
     *          Returns nullptr if given buffer is invalid.
     *
     * \example NEString::printable( )
     *
     *          Let's consider example:
     *
     *  char buffer[]   = {'1', '2', '\0', '4', 5, 6, '7', '8', '9', 127 };
     *  int32_t count       = sizeof(buffer);
     *  char * next     = buffer;
     *
     *  const char * result1 = NEString::printable<char>(next, count, &next);  // Results: result1 = "12"   , next = {'4', 5, 6, '7', '8', '9', 127}
     *  count -= string_length<char>(result1) + 1;
     *  const char * result2 = NEString::printable<char>(next, count, &next);  // Results: result2 = "4"    , next = {6, '7', '8', '9', 127}
     *  count -= string_length<char>(result2) + 1;
     *  const char * result3 = NEString::printable<char>(next, count, &next);  // Results: result3 = ""     , next = {'7', '8', '9', 127}
     *  count -= string_length<char>(result3) + 1;
     *  const char * result4 = NEString::printable<char>(next, count, &next);  // Results: result4 = "789"  , next = nullptr
     **/
    template<typename CharType>
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
    const CharType * printable( CharType * strSource, CharCount charCount, CharType ** out_next = nullptr );

    /**
     * \brief   Returns line of string of given buffer. The buffer should be possible to modify.
     *          Returns end-of-string if end-of-line symbol is at first position of given string
     *          and returns nullptr if given source is invalid.
     * \param   strSource   The source of string to get line. The string buffer should be possible to modify
     * \param   charCount   The number of characters in string buffer.
     * \param   out_next    If not nullptr, on output this contains pointer to next position
     *                      after first match of end-of-line character in source buffer.
     *                      If nullptr, there are no more characters in buffer.
     * \return  Returns portion of line of string.
     *          Returns nullptr if given buffer is invalid.
     **/
    template<typename CharType>    
    /**
     * \brief   Extracts a single line from the string. Requires the buffer to be modifiable.
     *
     * \param   strSource       The source string to extract from. The buffer must be modifiable.
     * \param   charCount       The number of characters in the buffer.
     * \param[out] out_next        If not nullptr, on output contains the pointer after the first
     *                             end-of-line character; nullptr if no more lines.
     * \return  Returns the first line. Returns nullptr if the buffer is invalid.
     **/
    const CharType * line( CharType * strSource, CharCount charCount = NEString::COUNT_ALL, CharType ** out_next = nullptr );

    /**
     * \brief   Converts the given string to digit in base 10.
     *          White-space at begin will be ignored. The negative
     */
    template<typename CharType>
    /**
     * \brief   Converts the given string to an integer in base 10, ignoring leading white-space.
     *
     * \param   strNumber       The string to convert.
     * \param[out] remain          On output, contains the pointer to the first non-digit character.
     **/
    int32_t make_integer(const CharType * strNumber, const CharType ** remain);

    /**
     * \brief   Computes and returns the buffer size required to allocate to format the string.
     *          This function works only for 'char' and 'wchar_t' types.
     * \note    Note:   This method does not compute the exact size required to allocate, rather
     *                  than the approximate size of buffer. The method start checking for buffer
     *                  sizes 128, 256, 512 and 1024. If it is required to allocate more than
     *                  1024 bytes, the function returns -1.
     * \param   format  The formatting string to calculate required size.
     * \param   argptr  The pointer to the argument list that fits the format.
     * \return  Returned values are 128, 256, 512, 1024 or -1 if failed to format or the required
     *          buffer size is 0 or larger than 1024. The return value includes null-character at
     *          the end of the string. If the buffer size could be bigger than 1024, check with
     *          function 'is_buffer_fit'
     **/
    template<char dummy = '\0'>
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
    int32_t required_buffer_size( const char * format, va_list argptr );
    template<wchar_t dummy = L'\0'>
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
    int32_t required_buffer_size( const wchar_t * format, va_list argptr );

    /**
     * \brief   Checks whether the buffer size fits to format a string.
     * \tparam  size    A constant expression to check the size.
     * \tparam  dummy   Is not used.
     * \param   format  The formatting string.
     * \param   argptr  The pointer to the argument list.
     * \return  Returns true if the checking size is enough to format the string.
     **/
    template<int32_t size, char dummy>
    /**
     * \brief   Checks whether a buffer is large enough to format a string with the given arguments.
     *
     * \param   format      The formatting string.
     * \param   argptr      The pointer to the argument list.
     * \return  Returns true if the buffer has sufficient space to format the string.
     **/
    bool is_buffer_fit( const char * format, va_list argptr );
    template<int32_t size, wchar_t dummy>
    /**
     * \brief   Checks whether a buffer is large enough to format a string with the given arguments.
     *
     * \param   format      The formatting string.
     * \param   argptr      The pointer to the argument list.
     * \return  Returns true if the buffer has sufficient space to format the string.
     **/
    bool is_buffer_fit( const wchar_t * format, va_list argptr );
}

//////////////////////////////////////////////////////////////////////////
// NEString namespace function templates or inline methods implementation
//////////////////////////////////////////////////////////////////////////

template<typename CharType, typename IntType>
int32_t NEString::make_string( CharType * strDst, NEString::CharCount charCount, IntType digit, NEString::Radix radix )
{
    int32_t result = 0;
    IntType num = NEMath::abs<IntType>(digit);
    if ( (NEString::is_empty<CharType>(strDst) == false) && (charCount > 1) )
    {
        static const CharType _valid[]  = {'0', '1', '2', '3', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '\0'};

        CharType * dst = strDst;
        radix = radix != NEString::Radix::Automatic ? radix : NEString::Radix::Decimal;

        if ((radix >= NEString::Radix::Binary) && (radix <= NEString::Radix::Hexadecimal) )
        {
            do
            {
                *dst ++ = _valid[num % static_cast<IntType>(radix)];
                num /= static_cast<IntType>(radix);
                -- charCount;
            } while ( (num != 0) && (charCount != 0) );

            if ( (charCount != 0) && (digit < 0) && (radix == NEString::Radix::Decimal) )
                *dst ++ = '-';

            *dst = static_cast<CharType>(NEString::EndOfString);
            NEString::revert_string<CharType>( strDst );
        }

        result = dst - strDst;
    }
    else
    {
        radix = radix != NEString::Radix::Automatic ? radix : NEString::Radix::Decimal;

        if ( (radix >= NEString::Radix::Binary) && (radix <= NEString::Radix::Hexadecimal) )
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
void NEString::revert_string( CharType * strDst, NEString::CharCount charCount /*= NEString::COUNT_ALL*/ )
{
    if ( NEString::is_empty<CharType>( strDst ) == false )
    {
        charCount = charCount == NEString::COUNT_ALL ? NEString::string_length<CharType>(strDst) : charCount;
        if (charCount > 0)
        {
            CharType * begin    = strDst;
            CharType * end      = strDst + charCount;
            if ( *end == static_cast<CharType>(NEString::EndOfString) )
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
int32_t NEString::make_integer(const CharType * strNumber, const CharType ** remain)
{
    NEMath::NumericSign sign = NEMath::NumericSign::Undefined;
    uint32_t result = 0;
    if (is_empty<CharType>(strNumber) == false)
    {
        CharType negative = as_char(NEMath::NumericSign::Negative);
        CharType positive = as_char(NEMath::NumericSign::Positive);
        for ( CharType ch = *strNumber; *strNumber != static_cast<CharType>(EndOfString); ++ strNumber )
        {
            ch = *strNumber;
            if (is_whitespace<CharType>(ch))
                continue;

            if (sign == NEMath::NumericSign::Undefined)
            {
                sign = ch == negative ? NEMath::NumericSign::Negative : NEMath::NumericSign::Positive;
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
        if (NEString::is_empty<CharType>(strNumber) == false)
        {
            *remain = strNumber;
        }
    }
    
    return (static_cast<int32_t>(sign) * static_cast<int32_t>(result));
}

template<char dummy>
int32_t NEString::required_buffer_size( const char * format, va_list argptr )
{
    int32_t result{ -1 };

    va_list argcopy;
    va_copy( argcopy, argptr );
    int32_t charCount = vsnprintf( nullptr, 0, format, argcopy );
    va_end( argcopy );

    if ( charCount > 0 )
    {
        if ( charCount < NEString::MSG_MIN_BUF_SIZE )
        {
            result = NEString::MSG_MIN_BUF_SIZE;
        }
        else if ( charCount < NEString::MSG_BUF_SIZE )
        {
            result = NEString::MSG_BUF_SIZE;
        }
        else if ( charCount < NEString::MSG_BIG_BUF_SIZE )
        {
            result = NEString::MSG_BIG_BUF_SIZE;
        }
        else if ( charCount < NEString::MSG_EXTRA_BUF_SIZE )
        {
            result = NEString::MSG_EXTRA_BUF_SIZE;
        }
    }

    return result;
}

template<wchar_t dummy>
int32_t NEString::required_buffer_size( const wchar_t * format, va_list argptr )
{
    int32_t result{ -1 };
    if ( NEString::is_buffer_fit< NEString::MSG_MIN_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = NEString::MSG_MIN_BUF_SIZE;
    }
    else if ( NEString::is_buffer_fit< NEString::MSG_MIN_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = NEString::MSG_MIN_BUF_SIZE;
    }
    else if ( NEString::is_buffer_fit< NEString::MSG_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = NEString::MSG_BUF_SIZE;
    }
    else if ( NEString::is_buffer_fit< NEString::MSG_BIG_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = NEString::MSG_BIG_BUF_SIZE;
    }
    else if ( NEString::is_buffer_fit< NEString::MSG_EXTRA_BUF_SIZE, dummy >( format, argptr ) )
    {
        result = NEString::MSG_EXTRA_BUF_SIZE;
    }

    return result;
}

template<int32_t size, char dummy>
bool NEString::is_buffer_fit( const char * format, va_list argptr )
{
    char buf[ size ]{ 0 };
    va_list argcopy;
    va_copy( argcopy, argptr );
    int32_t charCount = vsnprintf( buf, size, format, argcopy );
    va_end( argcopy );

    return (charCount < size);
}

template<int32_t size, wchar_t dummy>
bool NEString::is_buffer_fit( const wchar_t * format, va_list argptr )
{
    wchar_t buf[ size ]{ 0 };
    va_list argcopy;
    va_copy( argcopy, argptr );
    int32_t charCount = vswprintf( buf, size, format, argcopy );
    va_end( argcopy );

    return (charCount < size);
}

template<typename CharType>
const CharType * NEString::line( CharType * strSource, NEString::CharCount charCount/*= COUNT_ALL*/, CharType ** out_next /*= nullptr */ )
{
    const CharType* result = strSource;

    if (out_next != nullptr)
        *out_next = nullptr;

    if (NEString::is_empty(strSource) == false)
    {
        charCount = charCount == NEString::COUNT_ALL ? NECommon::VALUE_MAX_INT32 : charCount;
        while ((NEString::is_eos<CharType>(*strSource) == false) && (charCount > 0))
        {
            if (NEString::is_eol<CharType>(*strSource))
            {
                if ( NEString::is_dos_eol<CharType>(*strSource, *(strSource + 1)) )
                {
                    *strSource ++ = static_cast<CharType>(NEString::EndOfString);
                }

                *strSource ++ = static_cast<CharType>(NEString::EndOfString);
                break;
            }

            ++strSource;
        }
    }

    if (out_next != nullptr)
    {
        *out_next = (strSource == nullptr) || NEString::is_eos<CharType>(*strSource) ? nullptr : strSource;
    }

    return result;
}

template<typename CharType>
const CharType * NEString::printable( CharType * strSource, NEString::CharCount charCount, CharType ** out_next /*= nullptr */ )
{
    const CharType * result = nullptr;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( NEString::is_empty<CharType>(strSource) == false )
    {
        charCount = charCount == NEString::COUNT_ALL ? std::numeric_limits<int32_t>::max() : charCount;
        if ( charCount > 0 )
        {
            result = strSource;

            // move pointer until char is printable
            while ( charCount != 0 )
            {
                if ( NEString::is_printable<CharType>(*strSource) == false )
                {
                    // set next position if not EOS.
                    if ( (out_next != nullptr) && (NEString::is_eos<CharType>(*strSource) == false) )
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
NEString::CharPos NEString::find_last( CharType   chSearch
                                    , const CharType * strSource
                                    , NEString::CharPos startPos /*= NEString::END_POS*/
                                    , bool caseSensitive /*= true*/
                                    , const CharType ** out_next /*= nullptr*/ )
{
    NEString::CharPos result= NEString::INVALID_POS;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (is_empty<CharType>( strSource ) == false) && (chSearch != static_cast<CharType>(NEString::EndOfString)) )
    {
        CharPos posSrc = startPos == NEString::END_POS ? NEString::string_length<CharType>(strSource) - 1 : startPos;
        if ( posSrc >= NEString::START_POS )
        {
            const CharType * end = strSource + posSrc;
            if (caseSensitive)
            {
                while (end >= strSource)
                {
                    if (*end == chSearch)
                    {
                        result = static_cast<NEString::CharPos>(end - strSource);
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
                const CharType ch{ NEString::make_lower<CharType>(chSearch) };
                while (end >= strSource)
                {
                    if (NEString::make_lower<CharType>(*end) == ch)
                    {
                        result = static_cast<NEString::CharPos>(end - strSource);
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
NEString::CharPos NEString::find_last( const CharType * strPhrase
                                    , const CharType * strSource
                                    , NEString::CharPos startPos /*= NEString::END_POS*/
                                    , bool caseSensitive /*= true*/
                                    , const CharType ** out_next /*= nullptr*/ )
{
    NEString::CharPos result= NEString::INVALID_POS;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (is_empty<CharType>( strSource ) == false) && (is_empty<CharType>( strPhrase ) == false) )
    {
        CharPos posSrc = startPos == NEString::END_POS ? NEString::string_length<CharType>(strSource) : startPos;
        CharPos posPhr = NEString::string_length<CharType>(strPhrase);
        if ( (posSrc > NEString::START_POS) && (posPhr > NEString::START_POS) )
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
                            result = static_cast<NEString::CharPos>(one - strSource);
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
                CharType ch1{ NEString::make_lower<CharType>(*phrase) };
                while (end >= strSource)
                {
                    CharType ch2{ NEString::make_lower<CharType>(*end) };
                    if (ch1 == ch2)
                    {
                        const CharType* one = end - 1;
                        const CharType* two = phrase - 1;
                        // no need to check (*one != static_cast<CharType>(EndofString))
                        while ((one >= strSource) && (two >= strPhrase) && (NEString::make_lower<CharType>(*one) == NEString::make_lower<CharType>(*two)))
                        {
                            --one;
                            --two;
                        }

                        if (two < strPhrase)
                        {
                            ++one;
                            result = static_cast<NEString::CharPos>(one - strSource);
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
NEString::CharPos NEString::find_first( CharType chSearch
                                     , const CharType * strSource
                                     , NEString::CharPos startPos /*= NEString::START_POS*/
                                     , bool caseSensitive /*= true*/
                                     , const CharType ** out_next /*= nullptr*/ )
{
    NEString::CharPos result = NEString::INVALID_POS;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (is_empty<CharType>(strSource) == false) && (chSearch != static_cast<CharType>(NEString::EndOfString)) )
    {
        if ( startPos >= NEString::START_POS )
        {
            const CharType * next = strSource + startPos;
            if (caseSensitive)
            {
                while (*next != static_cast<CharType>(EndOfString))
                {
                    if (*next == chSearch)
                    {
                        result = static_cast<NEString::CharPos>(next - strSource);
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
                const CharType ch{ NEString::make_lower<CharType>(chSearch) };
                while (*next != static_cast<CharType>(EndOfString))
                {
                    if (NEString::make_lower<CharType>(*next) == ch)
                    {
                        result = static_cast<NEString::CharPos>(next - strSource);
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
NEString::CharPos NEString::find_first( const CharType * strPhrase
                                     , const CharType * strSource
                                     , NEString::CharPos startPos /*= NEString::START_POS*/
                                     , bool caseSensitive /*= true*/
                                     , const CharType ** out_next /*= nullptr*/ )
{
    NEString::CharPos result= NEString::INVALID_POS;

    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (is_empty<CharType>(strSource) == false) && (is_empty<CharType>(strPhrase) == false) )
    {
        if ( startPos >= NEString::START_POS )
        {
            const CharType * next = strSource + startPos;
            if (caseSensitive)
            {
                while (*next != static_cast<CharType>(NEString::EndOfString))
                {
                    if (*next == *strPhrase)
                    {
                        const CharType* one = next + 1;
                        const CharType* two = strPhrase + 1;
                        // no need to check (*one != static_cast<CharType>(EndofString))

                        while ((*two != static_cast<CharType>(NEString::EndOfString)) && (*one == *two))
                        {
                            ++one;
                            ++two;
                        }

                        if (*two == static_cast<CharType>(NEString::EndOfString))
                        {
                            result = static_cast<NEString::CharPos>(next - strSource);
                            if ((out_next != nullptr) && (*one != static_cast<CharType>(NEString::EndOfString)))
                                *out_next = one;

                            break; // break the loop
                        }
                    }

                    ++next;
                }
            }
            else
            {
                const CharType ch{ NEString::make_lower<CharType>(*strPhrase) };
                while (*next != static_cast<CharType>(NEString::EndOfString))
                {
                    if (NEString::make_lower<CharType>(*next) == ch)
                    {
                        const CharType* one = next + 1;
                        const CharType* two = strPhrase + 1;
                        // no need to check (*one != static_cast<CharType>(EndofString))
                        while ((*two != static_cast<CharType>(NEString::EndOfString)) && (NEString::make_lower<CharType>(*one) == NEString::make_lower<CharType>(*two)))
                        {
                            ++one;
                            ++two;
                        }

                        if (*two == static_cast<CharType>(NEString::EndOfString))
                        {
                            result = static_cast<NEString::CharPos>(next - strSource);
                            if ((out_next != nullptr) && (*one != static_cast<CharType>(NEString::EndOfString)))
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
bool NEString::string_starts_with(const CharType * strString, const CharType * phrase, bool caseSensitive /*= true*/)
{
    bool result{ false };
    if ((is_empty<CharType>(strString) == false) && (is_empty<CharType>(phrase) == false))
    {
        result = true;
        if (caseSensitive)
        {
            for ( ; (*phrase != NEString::EndOfString) && (*strString != NEString::EndOfString); ++strString, ++phrase)
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
            for (; (*phrase != NEString::EndOfString) && (*strString != NEString::EndOfString); ++strString, ++phrase)
            {
                CharType ch1{ NEString::make_lower<CharType>(*strString) };
                CharType ch2{ NEString::make_lower<CharType>(*phrase) };
                if (ch1 != ch2)
                {
                    result = false;
                    break;
                }
            }
        }

        result = result && (*phrase == NEString::EndOfString);
    }

    return result;
}

template<typename CharType>
bool NEString::string_starts_with(const CharType* strString, const CharType ch, bool caseSensitive)
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
            result = NEString::make_lower<CharType>(*strString) == NEString::make_lower<CharType>(ch);
        }
    }

    return result;
}

template<typename CharType>
bool NEString::string_ends_with(const CharType * strString, const CharType * phrase, bool caseSensitive /*= true*/)
{
    bool result{ false };
    if ((is_empty<CharType>(strString) == false) && (is_empty<CharType>(phrase) == false))
    {
        int32_t lenString   = NEString::string_length<CharType>(strString);
        int32_t lenPhrase   = NEString::string_length<CharType>(phrase);
        
        ASSERT(lenString > 0);
        ASSERT(lenPhrase > 0);

        int32_t diff = lenString - lenPhrase;

        result = (diff >= 0) && NEString::string_starts_with<CharType>(strString + diff, phrase, caseSensitive);
    }

    return result;
}

template<typename CharType>
bool NEString::string_ends_with(const CharType* strString, const CharType ch, bool caseSensitive /*= true*/)
{
    bool result{ false };
    if (is_empty<CharType>(strString) == false)
    {
        int32_t len = NEString::string_length<CharType>(strString);
        ASSERT(len != 0);
        if (caseSensitive)
        {
            result = strString[len - 1] == ch;
        }
        else
        {
            result = NEString::make_lower<CharType>(strString[len - 1]) == NEString::make_lower<CharType>(ch);
        }
    }

    return result;
}

template<typename CharType>
CharType * NEString::remove_char(const CharType chRemove, CharType* strSource, bool removeAll /*= true*/, bool caseSensitive /*= true*/)
{
    CharType* dst = strSource;
    const CharType* src = strSource;

    CharType ch1{ caseSensitive ? chRemove : NEString::make_lower<CharType>(chRemove) };

    while (NEString::is_eos(*src) == false)
    {
        CharType ch2{ caseSensitive ? *src : NEString::make_lower<CharType>(*src) };
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
    while (NEString::is_eos(*src) == false)
        *dst++ = *src++;

    *dst = static_cast<CharType>(NEString::EndOfString);    
    return result;
}

template<typename CharDst, typename CharSrc>
void NEString::trim_all( CharDst *            strDst
                      , NEString::CharCount  lenDst
                      , const CharSrc *      strSrc
                      , NEString::CharCount  lenSrc /*= NEString::COUNT_ALL*/ )
{
    if ( (strDst != nullptr) && (lenDst > 0) )
    {
        CharDst * dst = strDst;
        *dst    = static_cast<CharDst>(NEString::EndOfString);
        lenSrc  = lenSrc == NEString::COUNT_ALL ? NEString::string_length<CharSrc>(strSrc) : lenSrc;

        if ( (lenSrc > 0) && (NEString::is_empty<CharSrc>(strSrc) == false) )
        {
            const CharSrc * end     = strSrc + lenSrc - 1;
            const CharSrc * begin   = strSrc;

            while ((end > begin) && NEString::is_whitespace<CharSrc>(*end) )
                -- end;

            if (NEString::is_whitespace<CharSrc>(*end) == false)
                ++end;

            while ( (begin < end) && NEString::is_whitespace<CharSrc>(*begin) )
                ++ begin;

            for (; (begin < end) && (lenDst > 1); --lenDst)
                *dst++ = static_cast<CharDst>(*begin++);

            *dst = static_cast<CharDst>(NEString::EndOfString);
        }
    }
    else if (strDst != nullptr)
    {
        *strDst = static_cast<CharDst>(NEString::EndOfString);
    }
}

template<typename CharType>
void NEString::trim_all( CharType * strBuffer, NEString::CharCount strLen /*= NEString::COUNT_ALL*/ )
{
    if ( NEString::is_empty<CharType>( strBuffer ) == false )
    {
        NEString::CharCount shift = strLen == NEString::COUNT_ALL ? NEString::string_length<CharType>( strBuffer ) : strLen;
        if ( shift > 0 )
        {
            CharType * end  = strBuffer + shift - 1;
            CharType * begin= strBuffer;
            CharType * buf  = strBuffer;
            CharType * next = strBuffer + shift;

            while ( (end > begin) && NEString::is_whitespace<CharType>(*end) )
                -- end;

            if (NEString::is_whitespace<CharType>(*end) == false)
                ++end;

            while ( (begin < end) && NEString::is_whitespace<CharType>(*begin) )
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

            if (strLen != NEString::COUNT_ALL)
            {
                while (*next != static_cast<CharType>(NEString::EndOfString))
                    *buf ++ = *next ++;
            }

            *buf = static_cast<CharType>(NEString::EndOfString);
        }
    }
}

template<typename CharDst, typename CharSrc>
void NEString::trim_right( CharDst *           strDst
                        , NEString::CharCount lenDst
                        , const CharSrc *     strSrc
                        , NEString::CharCount lenSrc /*= NEString::COUNT_ALL*/ )
{
    if ((NEString::is_empty<CharSrc>(strSrc) == false) && (strDst != nullptr) && (lenDst > 0) )
    {
        lenSrc = lenSrc == NEString::COUNT_ALL ? NEString::string_length<CharSrc>(strSrc) : lenSrc;
        if ( lenSrc > 0 )
        {
            const CharSrc * end   = strSrc + lenSrc - 1;
            const CharSrc * begin = strSrc;
            CharDst * dst         = strDst;

            while ( (end != strSrc) && NEString::is_whitespace<CharSrc>(*end) )
                -- end;

            if (NEString::is_whitespace<CharSrc>(*end) == false)
                ++ end;

            for ( ; (begin < end) && (lenDst > 1); -- lenDst)
                *dst ++ = static_cast<CharDst>(*begin ++);

            *dst = static_cast<CharDst>(NEString::EndOfString);
        }
    }
    else if (strDst != nullptr)
    {
        *strDst = static_cast<CharDst>(NEString::EndOfString);
    }
}

template<typename CharType>
void NEString::trim_right( CharType * strBuffer, NEString::CharCount strLen /*= NEString::COUNT_ALL*/ )
{
    if ( NEString::is_empty<CharType>( strBuffer ) == false )
    {
        NEString::CharCount shift = strLen = strLen == NEString::COUNT_ALL ? NEString::string_length<CharType>(strBuffer) : strLen;
        if ( shift > 0 )
        {
            CharType * end  = strBuffer + shift - 1;
            CharType * begin= strBuffer;
            CharType * next = strBuffer + shift;

            while ( (end > begin) && NEString::is_whitespace<CharType>( *end ) )
                -- end;

            if (strLen == NEString::COUNT_ALL)
            {
                if (NEString::is_whitespace<CharType>(*end))
                {
                    *(end) = static_cast<CharType>(NEString::EndOfString);
                }
                else
                {
                    *(++end) = static_cast<CharType>(NEString::EndOfString);
                }
            }
            else
            {
                CharType * dst = NEString::is_whitespace<CharType>(*end) ? end : ++end;
                while(*next != static_cast<CharType>(NEString::EndOfString))
                    *dst ++ = *next ++;
                *(dst) = static_cast<CharType>(NEString::EndOfString);
            }
        }
    }
}

template<typename CharDst, typename CharSrc>
void NEString::trim_left( CharDst *           strDst
                       , NEString::CharCount lenDst
                       , const CharSrc *     strSrc
                       , NEString::CharCount lenSrc /*= NEString::COUNT_ALL*/ )
{
    if ((NEString::is_empty<CharSrc>(strSrc) == false) && (strDst != nullptr) && (lenDst > 0) )
    {
        lenSrc = lenSrc == NEString::COUNT_ALL ? NEString::string_length<CharSrc>(strSrc) : lenSrc;
        if ( lenSrc > 0 )
        {
            const CharSrc * begin = strSrc;
            const CharSrc * end   = strSrc + lenSrc;
            CharDst * dst         = strDst;

            while ((end > begin) && NEString::is_whitespace<CharSrc>(*begin) )
                ++ begin;

            for (; (begin < end) && (lenDst > 1); --lenDst)
                *dst++ = static_cast<CharDst>(*begin++);

            *dst    = static_cast<CharDst>(NEString::EndOfString);
        }
    }
    else if (strDst != nullptr)
    {
        *strDst = static_cast<CharDst>(NEString::EndOfString);
    }
}

template<typename CharType>
void NEString::trim_left( CharType * strBuffer, NEString::CharCount strLen /*= NEString::COUNT_ALL*/ )
{
    if ( NEString::is_empty<CharType>( strBuffer ) == false )
    {
        NEString::CharCount shift = strLen == NEString::COUNT_ALL ? NEString::string_length<CharType>(strBuffer) : strLen;
        if (shift > 0 )
        {
            const CharType * begin = strBuffer;
            const CharType * end   = strBuffer + shift;
            CharType * buf         = strBuffer;

            while ( (end > begin) && NEString::is_whitespace<CharType>(*begin) )
                ++ begin;

            if ( begin != buf )
            {
                while ( begin < end)
                    *buf ++ = *begin ++;

                if (strLen != NEString::COUNT_ALL)
                {
                    const CharType* src = strBuffer + strLen;
                    while (*src != static_cast<CharType>(NEString::EndOfString))
                        *buf ++ = *src ++;
                }

                *buf = static_cast<CharType>(NEString::EndOfString);
            }
        }
    }
}

template<typename CharType>
inline bool NEString::is_one_of(CharType ch, const CharType* chSequence)
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
inline bool NEString::is_readable( CharType ch )
{
    constexpr uint32_t def{ static_cast<uint32_t>(NEString::CharCategory::Letter) |
                            static_cast<uint32_t>(NEString::CharCategory::Number) |
                            static_cast<uint32_t>(NEString::CharCategory::Symbol) };

    return (((NEString::utf8_char_def( ch ) & def) != 0) || (ch == ' '));
}

template<typename CharType>
inline bool NEString::is_printable( CharType ch )
{
    return ((NEString::utf8_char_def( ch ) & static_cast<uint16_t>(NEString::CharCategory::Printable)) != 0);
}

template<typename CharType>
inline bool NEString::is_eol( CharType ch )
{
    return ((NEString::utf8_char_def( ch ) & static_cast<uint16_t>(NEString::CharCategory::EndOfLine)) != 0);
}

template<typename CharType>
inline bool NEString::is_carriage_return( CharType ch )
{
    return ((NEString::utf8_char_def( ch ) & static_cast<uint16_t>(NEString::CharCategory::CarReturn)) != 0);
}

template<typename CharType>
bool NEString::is_new_line( CharType ch )
{
    return (ch == static_cast<CharType>(NEString::EndOfLine));
}

template<typename CharType>
bool NEString::is_unix_eol(CharType ch)
{
    return NEString::is_new_line<CharType>(ch);
}

template<typename CharType>
bool NEString::is_unix_eol(const CharType * source)
{
    return (source != nullptr ? NEString::is_unix_eol<CharType>(*source) : false);
}

template<typename CharType>
bool NEString::is_dos_eol(CharType ch1, CharType ch2)
{
    return (NEString::is_carriage_return<CharType>(ch1) && NEString::is_unix_eol<CharType>(ch2));
}

template<typename CharType>
bool NEString::is_dos_eol(const CharType * source)
{
    return (source != nullptr ? NEString::is_dos_eol<CharType>(*source, *(source + 1)) : false);
}

template<typename CharType>
inline bool NEString::is_eos( CharType ch )
{
    return ((NEString::utf8_char_def(ch) & static_cast<uint16_t>(NEString::CharCategory::EOfS)) != 0);
}

template<typename CharType>
inline bool NEString::is_control(CharType ch)
{
    return ((NEString::utf8_char_def(ch) & static_cast<uint16_t>(NEString::CharCategory::Control)) != 0);
}

template<typename CharType>
inline bool NEString::is_symbol( CharType ch )
{
    return ((NEString::utf8_char_def(ch) & static_cast<uint16_t>(NEString::CharCategory::Symbol)) != 0);
}

template<typename CharType>
inline bool NEString::is_whitespace( CharType ch )
{
    return ((NEString::utf8_char_def( ch ) & static_cast<uint16_t>(NEString::CharCategory::WhiteSpace)) != 0);
}

template<typename CharType>
inline bool NEString::is_delimited( CharType ch )
{
    return ((NEString::utf8_char_def( ch ) & static_cast<uint16_t>(NEString::CharCategory::Delimiter)) != 0);
}

template<typename CharType>
inline bool NEString::is_letter(CharType ch)
{
    return ((NEString::utf8_char_def( ch ) & static_cast<uint16_t>(NEString::CharCategory::Letter)) != 0);
}

template<typename CharType>
inline bool NEString::is_numeric(CharType ch)
{
    return ((NEString::utf8_char_def( ch ) & static_cast<uint16_t>(NEString::CharCategory::Number)) != 0);
}

template<typename CharType>
inline bool NEString::is_alphanumeric(CharType ch)
{
    constexpr uint32_t def{ static_cast<uint32_t>(NEString::CharCategory::Letter) | static_cast<uint32_t>(NEString::CharCategory::Number) };

    return ((NEString::utf8_char_def( ch ) & def) != 0);
}

template <typename CharType>
inline CharType NEString::make_lower(CharType ch)
{
    // return ((ch >= 'A') && (ch <= 'Z') ? ch - 'A' + 'a' : ch);
    return static_cast<CharType>(NEString::make_lower_char(ch));
}

template <typename CharType>
inline const CharType* NEString::make_lower(CharType* source)
{
    const CharType* result{ source };
    if (NEString::is_empty<CharType>(source) == false)
    {
        while (NEString::is_eos<CharType>(*source) == false)
        {
            *source = NEString::make_lower<CharType>(*source);
            ++source;
        }
    }

    return result;
}

template <typename CharType>
inline CharType NEString::make_upper(CharType ch)
{
    // return ((ch >= 'a') && (ch <= 'z') ? ch - 'a' + 'A' : ch);
    return static_cast<CharType>(NEString::make_upper_char(ch));
}

template <typename CharType>
inline const CharType* NEString::make_upper(CharType* source)
{
    const CharType* result{ source };
    if (NEString::is_empty<CharType>(source) == false)
    {
        while (NEString::is_eos<CharType>(*source) == false)
        {
            *source = NEString::make_upper<CharType>(*source);
            ++source;
        }
    }

    return result;
}

template<typename CharType>
inline bool NEString::is_lower(CharType ch)
{
    return ((NEString::utf8_char_def(ch) & static_cast<uint16_t>(NEString::CharCategory::LetterLo)) == static_cast<uint16_t>(NEString::CharCategory::LetterLo));
}

template<typename CharType>
inline bool NEString::is_upper(CharType ch)
{
    return ((NEString::utf8_char_def(ch) & static_cast<uint16_t>(NEString::CharCategory::LetterUp)) == static_cast<uint16_t>(NEString::CharCategory::LetterUp));
}

inline bool NEString::is_position_valid(NEString::CharPos pos)
{
    return (pos >= NEString::START_POS);
}

template<typename CharType>
inline bool NEString::is_empty( const CharType * strBuffer )
{
    return (strBuffer == nullptr) || (*strBuffer == static_cast<CharType>(NEString::EndOfString));
}

template <typename CharType>
inline NEString::CharCount NEString::string_length( const CharType * theString )
{
    NEString::CharCount result = 0;
    if ( NEString::is_empty<CharType>(theString) == false )
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
inline NEString::CharCount NEString::string_line_length(const CharType* theString)
{
    const CharType* start = theString;
    if (theString != nullptr)
    {
        while(!NEString::is_eol<CharType>(*theString) && !NEString::is_eos<CharType>(*theString))
        {
            ++theString;
        }
    }

    return static_cast<NEString::CharCount>( theString - start );
}

template<typename CharDst, typename CharSrc>
NEString::CharCount NEString::copy_string( CharDst *           strDst
                                        , NEString::CharCount dstSpace
                                        , const CharSrc *     strSrc
                                        , NEString::CharCount charsCopy /*= NEString::COUNT_ALL*/)
{
    uint32_t result { 0 };

    if constexpr (sizeof(CharSrc) == sizeof(CharDst))
    {
        if ( strDst != nullptr )
        {
            charsCopy = charsCopy == NEString::COUNT_ALL ? NEString::string_length<CharSrc>( strSrc ) : charsCopy;
            result = NEMemory::mem_copy( strDst, static_cast<uint32_t>(dstSpace) * sizeof(CharDst), strSrc, static_cast<uint32_t>(charsCopy) * sizeof(CharSrc)) / sizeof(CharDst);
            strDst[result] = NEString::EndOfString;
        }
    }
    else if ( (dstSpace > 0) && (strDst != nullptr) && (strSrc != nullptr) )
    {
        charsCopy = charsCopy == NEString::COUNT_ALL ? std::numeric_limits<int32_t>::max() : charsCopy;
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
        strDst[0] = NEString::EndOfString;
    }

    return static_cast<NEString::CharCount>(result);
}

template<typename CharType>
NEString::CharCount NEString::copy_string_fast(CharType*            strDst
                                            , const CharType*     strSrc
                                            , NEString::CharCount charsCopy /*= NEString::COUNT_ALL*/)
{
    uint32_t result { 0 };
    if (strDst != nullptr)
    {
        charsCopy = charsCopy == NEString::COUNT_ALL ? NEString::string_length<CharType>(strSrc) : charsCopy;
        result = NEMemory::mem_copy(strDst, static_cast<uint32_t>(charsCopy) * sizeof(CharType), strSrc, static_cast<uint32_t>(charsCopy) * sizeof(CharType)) / sizeof(CharType);
        strDst[result] = static_cast<CharType>(NEString::EndOfString);
    }

    return static_cast<NEString::CharCount>(result);
}

template<typename CharLhs, typename CharRhs>
NEMath::Ordering NEString::compare_strings( const CharLhs *left_side
                                         , const CharRhs * right_side
                                         , NEString::CharCount charCount    /*= COUNT_ALL*/
                                         , bool caseSensitive               /*= true*/)
{
    NEMath::Ordering result = NEMath::Ordering::Bigger;

    if (reinterpret_cast<const void*>(left_side) == reinterpret_cast<const void*>(right_side))
    {
        result = NEMath::Ordering::Equal;
    }
    else if ((left_side != nullptr) && (right_side != nullptr))
    {
        if (charCount == NEString::COUNT_ALL)
        {
            CharLhs chLeft{ 0 };
            CharRhs chRight{ 0 };
            if (caseSensitive)
            {
                while (true)
                {
                    chLeft = *left_side;
                    chRight = *right_side;
                    if ((chLeft != static_cast<CharLhs>(chRight)) || (chLeft == NEString::EndOfString) || (chRight == NEString::EndOfString))
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
                    chLeft = NEString::make_lower<CharLhs>(*left_side);
                    chRight = NEString::make_lower<CharRhs>(*right_side);
                    if ((chLeft != static_cast<CharLhs>(chRight)) || (chLeft == NEString::EndOfString) || (chRight == NEString::EndOfString))
                    {
                        break;
                    }

                    ++left_side;
                    ++right_side;
                }
            }

            if (chLeft == static_cast<CharLhs>(chRight))
                result = NEMath::Ordering::Equal;
            else if (chLeft < static_cast<CharLhs>(chRight))
                result = NEMath::Ordering::Smaller;
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
                    if ((chLeft != static_cast<CharLhs>(chRight)) || (chLeft == NEString::EndOfString) || (chRight == NEString::EndOfString))
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
                    chLeft = NEString::make_lower<CharLhs>(*left_side);
                    chRight = NEString::make_lower<CharRhs>(*right_side);
                    if ((chLeft != static_cast<CharLhs>(chRight)) || (chLeft == NEString::EndOfString) || (chRight == NEString::EndOfString))
                    {
                        break;
                    }

                    ++left_side;
                    ++right_side;
                    --charCount;
                }
            }

            if (charCount == 0)
                result = NEMath::Ordering::Equal;
            else if (chLeft < static_cast<CharLhs>(chRight))
                result = NEMath::Ordering::Smaller;
        }
    }
    else if ( right_side != nullptr )
    {
        result = NEMath::Ordering::Smaller;
    }

    return result;
}

template<typename CharLhs, typename CharRhs>
inline NEMath::Ordering NEString::compare_ignore_case(const CharLhs* left_side, const CharRhs* right_side, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    return NEString::compare_strings<CharLhs, CharRhs>(left_side, right_side, count, false);
}

template<typename CharLhs, typename CharRhs>
inline NEMath::Ordering NEString::compare(const CharLhs* left_side, const CharRhs* right_side)
{
    return NEString::compare_strings<CharLhs, CharRhs>(left_side, right_side, NEString::COUNT_ALL, true);
}

template<typename CharLhs, typename CharRhs>
inline NEMath::Ordering NEString::compare(const CharLhs* left_side, const CharRhs* right_side, NEString::CharCount count)
{
    return NEString::compare_strings<CharLhs, CharRhs>(left_side, right_side, count, true);
}

template<typename CharType>
inline NEMath::Ordering NEString::compare_fast(const CharType* left_side, const CharType* right_side, NEString::CharCount count)
{
    NEMath::Ordering result = NEMath::Ordering::Bigger;
    if (count == NEString::COUNT_ALL)
    {
        result = NEString::compare_strings<CharType, CharType>(left_side, right_side, NEString::COUNT_ALL, true);
    }
    else if (left_side == right_side)
    {
        result = NEMath::Ordering::Equal;
    }
    else if ((left_side != nullptr) && (right_side != nullptr))
    {
        result = NEMemory::mem_compare(left_side, right_side, static_cast<uint32_t>(count) * sizeof(CharType));
    }
    else if (right_side != nullptr)
    {
        result = NEMath::Ordering::Smaller;
    }

    return result;
}

#endif  // AREG_BASE_STRINGDEFS_HPP
