#ifndef AREG_BASE_NESTRING_HPP
#define AREG_BASE_NESTRING_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/NEString.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, namespace with UTF String helper methods
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NECommon.hpp"
#include "areg/base/NEMemory.hpp"
#include "areg/base/NEMath.hpp"

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
     *          NEString::eCharDefs
     **/
    typedef enum E_CharDefs   : uint16_t
    {
          CD_Undefined  =   0   //!< Undefined character type
        , CD_EOfS       =   1   //!< End-of+string character
        , CD_EndOfLine  =   2   //!< ENd-of-line character
        , CD_CarReturn  =   4   //!< Carriage return character
        , CD_WhiteSpace =   8   //!< Whitespace character
        , CD_Delimiter  =  16   //!< Syntax Delimiter
        , CD_Control    =  32   //!< Control key / value
        , CD_Printable  =  64   //!< Printable character or character, which change text layout like space or tab
        , CD_Number     = 128   //!< Numeric character
        , CD_Symbol     = 256   //!< Symbol
        , CD_Letter     = 512   //!< Letter

    } eCharDefs;

    /**
     * \brief   Returns the bit-wise value of character definition based on ASCII ISO8859-1
     * \param   ch      The character value to receive defined value.
     */
    AREG_API unsigned short getISO8859CharDef( int ch );

    /**
     * \brief   Returns lower case letter of given character based on ASCII ISO8859-1, it if has any
     * \param   ch      The character value to receive defined value.
     */
    AREG_API unsigned int getISO8859LowerChar( int ch );

    /**
     * \brief   Returns upper case letter of given character based on ASCII ISO8859-1, it if has any
     * \param   ch      The character value to receive defined value.
     */
    AREG_API unsigned int getISO8859UpperChar( int ch );

    //! ASCII locale.
    static const char* const LOCALE_ASCII   = "C";

    //! en_US.UTF8 locale.
    static const char* const LOCALE_US_UTF8 = "en_US.UTF8";

    //! Currently installed C locale.
    static const char* const LOCALE_DEFAULT   {LOCALE_ASCII};

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
     * \brief   The enum used to convert string to digit and vice versa
     *          It defines the bases of conversion.
     * \note    The automatic detection (RadixAutomatic) of conversion works only
     *          when convert from string to digit and has following logic after ignoring white spaces:
     *              - if first character is '0' and second character is not 'x' or 'X', then it is Octal (RadixOctal)
     *              - if first character is '0' and second character is 'x' or 'X', then it is Hexadecimal (RadixHexadecimal)
     *              - if the first character starts between '1' and '9', then Decimal (RadixDecimal)
     *              - There is no way to determine bases 2, and the RadixBinary is ignored.
     **/
    typedef enum class E_Radix : int8_t
    {
          RadixAutomatic    =  0    //!< Detect automatically
        , RadixBinary       =  2    //!< Binary conversion, conversion base is 2
        , RadixOctal        =  8    //!< Octal conversion, conversion base is 8
        , RadixDecimal      = 10    //!< Decimal conversion, conversion base is 10
        , RadixHexadecimal  = 16    //!< Hexadecimal conversion, conversion base is 16
    } eRadix;

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
    int makeString( CharType * strDst, NEString::CharCount charCount, IntType digit, NEString::eRadix radix = NEString::eRadix::RadixDecimal );

    /**
     * \brief   Swaps characters in a given string buffer.
     * \param   strDst      The destination string buffer to make swapping.
     * \param   charCount   The number of characters in buffer to swap. If COUNT_ALL, it swaps all characters until end-of-string
     * \tparam  CharType    The type of characters in buffer.
     */
    template<typename CharType>
    void swapString(CharType * strDst, CharCount charCount = NEString::COUNT_ALL);

    /**
     * \brief   End of String value
     *          NEString::EndofString
     **/
    constexpr char              EndOfString     { static_cast<char>('\0') };  //!< End of String, signed char (ASCII)

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
     * 'brief   Returns true if the specified position is nether invalid, nor end of string value.
     */
    inline bool isPositionValid(NEString::CharPos pos);

    /**
     * \brief   Returns true if given string is empty.
     *          The string is empty if it is nullptr or starts with end-of-string character.
     * \param   strBuffer   The string buffer to check whether it is empty or not.
     * \return  Returns true if given string buffer is empty.
     **/
    template<typename CharType>
    inline bool isEmpty( const CharType * strBuffer );
    
    /**
     * \brief   Returns length of string buffer. The length is calculated until first match of End of String value.
     * \param   theString   The string to count the length.
     * \return  Returns number of characters in the string
     **/
    template <typename CharType>
    inline CharCount getStringLength( const CharType * theString );

    /**
     * \brief   Returns length of string line. The length is calculated until first match of End-of-Line or End-of-string value.
     * \param   theString   The string to count the length.
     * \return  Returns number of characters in the string
     **/
    template <typename CharType>
    inline CharCount getStringLineLength(const CharType* theString);

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
    CharCount copyString( CharDst * strDst, CharCount dstSpace, const CharSrc * strSrc, CharCount charsCopy = NEString::COUNT_ALL );

    /**
     * \brief   Converts given character to lower case. Valid only for ASCII characters.
     * \param   ch      The character to convert to lower case.
     * \return  If character is in ASCII range, it converts character to lower case. 
     *          Otherwise, conversion is ignored and returns same value.
     **/
    template <typename CharType>
    inline CharType makeAsciiLower(CharType ch);

    /**
     * \brief   Converts given character to upper case. Valid only for ASCII characters.
     * \param   ch      The character to convert to upper case.
     * \return  If character is in ASCII range, it converts character to upper case.
     *          Otherwise, conversion is ignored and returns same value.
     **/
    template <typename CharType>
    inline CharType makeAsciiUpper(CharType ch);

    /**
     * \brief   Compares 2 given strings (Left and Right side). The function compares 'charCount' characters
     *          in the strings or until end of string if charCount is equal to NEString::END_POS.
     *          if 'caseSensitive' is false, function ignores capital and lower cases.
     *          The function is valid only for ASCII characters and it returns:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     * \param   leftSide        The Left-side string to compare
     * \param   rightSide       The Right-side string to compare
     * \param   charCount       The characters to compare or until end of string if equal to 'NEString::END_POS'
     * \param   caseSensitive   If false, it will ignore upper and lower cases. 
     * \return  The function returns:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     * \note    The results are based 'charCount' and 'caseSensitive' parameters.
     **/
    template<typename CharLhs, typename CharRhs>
    NEMath::eCompare compareStrings( const CharLhs *leftSide
                                   , const CharRhs * rightSide
                                   , CharCount charCount = NEString::END_POS
                                   , bool caseSensitive  = true);

    /**
     * \brief   Compares 2 given strings (Left and Right side). The function compares complete string and ignores the upper and lower cases.
     *          The function is valid only for ASCII characters and it returns:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     * \param   leftSide        The Left-side string to compare
     * \param   rightSide       The Right-side string to compare
     * \return  The function returns:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     **/
    template<typename CharLhs, typename CharRhs>
    inline NEMath::eCompare compareIgnoreCase( const CharLhs *leftSide, const CharRhs * rightSide );

    /**
     * \brief   Fast compares 2 string. The comparing is done until first match of null-termination
     *          and it is case sensitive.
     *          The function is valid only for ASCII characters and it returns:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     * \param   leftSide        The Left-side string to compare
     * \param   rightSide       The Right-side string to compare
     * \return  The function returns:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     **/
    template<typename CharLhs, typename CharRhs>
    inline NEMath::eCompare compareFast(const CharLhs *leftSide, const CharRhs * rightSide);

    /**
     * \brief   Compares 2 null-terminated strings and returns:
     *          -1 if leftSide string is smaller than the rightSide string;
     *           0 if leftSide and rightSide strings are equal;
     *           1 if leftSide string is bigger than the rightSide string.
     * 
     * \tparam  CharType    Either char or wchar_t.
     * \param   leftSide    The null-terminated string to compare.
     * \param   rightSide   The null-terminated string to compare.
     * \return  Returns -1 if leftSide string is smaller than rightSide string.
     *          Returns 0 if leftSide and rightSide strings are equal.
     *          Returns 1 if leftSide string is bigger than the rightSide string.
     **/
    template<typename CharType>
    inline NEMath::eCompare compare(const CharType * leftSide, const CharType * rightSide);

    /**
     * \brief   Fast compares 2 string. Compares first count characters.
     *          The function is valid only for ASCII characters and it returns:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     * \param   leftSide        The Left-side string to compare
     * \param   rightSide       The Right-side string to compare
     * \return  The function returns:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     **/
    template<typename CharLhs, typename CharRhs>
    inline NEMath::eCompare compareFast( const CharLhs *leftSide, const CharRhs * rightSide, NEString::CharCount count );

    /**
     * \brief   Fast compares first count symbols of 2 string. The comparing case sensitive.
     *          The function is valid only for ASCII characters and it returns:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     * \param   leftSide    The Left-side string to compare
     * \param   rightSide   The Right-side string to compare
     * \param   count       Number of characters to compare.
     * \return  The function returns:
     *              -1 if Left-side string is less than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is bigger than Right-side string.
     **/
    template<typename CharType>
    inline NEMath::eCompare compareFast( const CharType * leftSide, const CharType * rightSide, NEString::CharCount count );

    /**
     * \brief   Fast compares 2 string ignoring sensitivity. The comparing is done until first match of null-termination
     *          and it is case sensitive.
     *          The function is valid only for ASCII characters and it returns:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     * \param   leftSide        The Left-side string to compare
     * \param   rightSide       The Right-side string to compare
     * \return  The function returns:
     *              -1 if Left-side string is bigger than Right-side string
     *               0 if Left-side and Right-side strings are equal
     *               1 if Left-side string is less than Right-side string.
     **/
    template<typename CharLhs, typename CharRhs>
    inline NEMath::eCompare compareFastIgnoreCase(const CharLhs *leftSide, const CharRhs * rightSide);

    /**
     * \brief   Returns true if given character is a letter.
     *          The checkup is done based on ISO8859
     * \param	ch	    ASCII character to check.
     * \return	Returns true if character is a letter.
     **/
    template<typename CharType>
    inline bool isLetter( CharType ch );

    /**
     * \brief   Returns true if given character is a number.
     *          The checkup is done based on ISO8859
     * \param	ch	    ASCII character to check.
     * \return	Returns true if character is numeric.
     **/
    template<typename CharType>
    inline bool isNumeric( CharType ch );

    /**
     * \brief   Returns true if given character is a letter or number.
     *          The checkup is done based on ISO8859
     * \param	ch	    ASCII character to check.
     * \return	Returns true if character is a letter or numeric.
     **/
    template<typename CharType>
    inline bool isAlphanumeric( CharType ch );

    /**
     * \brief   Returns true if given character is a symbol, i.e. not a control key
     *          The checkup is done based on ISO8859
     * \param	ch	    ASCII character to check.
     * \return	Returns true if character is a symbol.
     **/
    template<typename CharType>
    inline bool isSymbol( CharType ch );

    /**
     * \brief   Returns true if given character is a white-space.
     *          The checkup is done based on ISO8859
     * \param	ch	    ASCII character to check.
     * \return	Returns true if character is white-space symbol.
     **/
    template<typename CharType>
    inline bool isWhitespace( CharType ch);

    /**
     * \brief   Returns true if given character is used as delimiter in the syntax.
     *          The checkup is done based on ISO8859
     * \param	ch	    ASCII character to check.
     * \return	Returns true if character is syntax separator symbol.
     **/
    template<typename CharType>
    inline bool isDelimited( CharType ch);

    /**
     * \brief	Checks whether the passed single character is end of line or not.
     *          The checkup is done based on ISO8859
     * \param	ch	    ASCII character to check.
     * \return	Returns true if character is end of line
     **/
    template<typename CharType>
    inline bool isEndOfLine( CharType ch );

    /**
     * \brief	Checks whether the passed single character is a carriage return symbol or not.
     *          The checkup is done based on ISO8859
     * \param	ch	    ASCII character to check.
     * \return	Returns true if character is carriage return symbol.
     **/
    template<typename CharType>
    inline bool isCarriageReturn( CharType ch );

    /**
     * \brief	Checks whether the passed single character is an  end of string symbol.
     *          The checkup is done based on ISO8859
     * \param	ch	    ASCII character to check.
     * \return	Returns true if character is end of string symbol.
     **/
    template<typename CharType>
    inline bool isEndOfString( CharType ch );

    /**
     * \brief   Checks whether the given character is readable or not.
     *          Readable are characters, which can be read by human and white-space.
     * \param   ch      ASCII character to check.
     * \return  Returns true if character is readable.
     * \tparam  The type of ASCII range character. Expecting 8-bit or 16-bit character in range 0 - 0xFF
     * \note    The readable and printable characters differ, which readable character can be read without
     *          additional system support. For example, the horizontal and vertical tabs, or carriage return are not considered as readable.
     * \see     isPrintable
     **/
    template<typename CharType>
    inline bool isReadable( CharType ch );

    /**
     * \brief   Checks whether the given character is printable or not.
     *          Printable are all characters, which can be printed. These include all readable characters, 
     *          plus all other white-spaces like tabs, new-line, carriage return, etc..
     * \param   ch      ASCII character to check.
     * \return  Returns true if character is printable.
     * \tparam  The type of ASCII range character. Expecting 8-bit or 16-bit character in range 0 - 0xFF
     * \note    The printable characters include readable characters, but not vice-versa. In addition, it includes
     *          all other white-spaces like tabs, new-line, carriage return, etc.
     * \see     isPrintable
     **/
    template<typename CharType>
    inline bool isPrintable( CharType ch );

    /**
     * \brief   Checks whether the give character is one of matches in the sequence.
     * \param   ch          The character to check.
     * \param   chSequence  The sequence of characters to have at least one match.
     * \tparam  The type of ASCII range character. Expecting 8-bit or 16-bit character in range 0 - 0xFF
     * \return  Returns true if given character matches one of entries in the given sequence.
     **/
    template<typename CharType>
    inline bool isOneOf(CharType ch, const CharType* chSequence);

    /**
     * \brief   In given string buffer removes white-space characters from left-side (from begin).
     * \param   strBuffer   The string buffer to remove white characters.
     * \param   strLen      The length of the string or NEString:COUNT_ALL if the length should be calculated.
     * \return  Returns number of characters in the buffer.
     **/
    template<typename CharType>
    CharCount trimLeft( CharType * strBuffer, CharCount strLen = NEString::COUNT_ALL );

    /**
     * \brief   In removes white-space characters from left-side (from begin) 
     *          and copies the rest to the destination buffer.
     * \param   strDst  The destination buffer to copy string.
     * \param   lenDst  The destination buffer length to copy characters.
     *                  The length should be big enough to copy.
     *                  Otherwise, it will copy only first (lenDst - 1) characters and set end-of-string at the end.
     * \param   strSrc  The source string buffer.
     * \param   lenSrc  The length of the source string or NEString:COUNT_ALL if the length should be calculated.
     * \return  Returns number of characters copied to the destination buffer.
     **/
    template<typename CharDst, typename CharSrc>
    CharCount trimLeft( CharDst *strDst, CharCount lenDst, const CharSrc * strSrc, CharCount lenSrc = NEString::COUNT_ALL );

    /**
     * \brief   In given string buffer removes white-space characters from right-side (from end).
     * \param   strBuffer   The string buffer to remove white characters.
     * \param   strLen      The length of the string or NEString:COUNT_ALL if the length should be calculated.
     * \return  Returns number of characters in the buffer.
     **/
    template<typename CharType>
    CharCount trimRight( CharType * strBuffer, CharCount strLen = NEString::COUNT_ALL );

    /**
     * \brief   In removes white-space characters from right-side (from end) 
     *          and copies the rest to the destination buffer.
     * \param   strDst  The destination buffer to copy string.
     * \param   lenDst  The destination buffer length to copy characters.
     *                  The length should be big enough to copy.
     *                  Otherwise, it will copy only first (lenDst - 1) characters and set end-of-string at the end.
     * \param   strSrc  The source string buffer.
     * \param   lenSrc  The length of the source string or NEString:COUNT_ALL if the length should be calculated.
     * \return  Returns number of characters copied to the destination buffer.
     **/
    template<typename CharDst, typename CharSrc>
    CharCount trimRight( CharDst *strDst, CharCount lenDst, const CharSrc * strSrc, CharCount lenSrc = NEString::COUNT_ALL );

    /**
     * \brief   In given string buffer removes white-space characters from left- and right-side (from begin and end).
     * \param   strBuffer   The string buffer to remove white characters.
     * \param   strLen      The length of the string or NEString:COUNT_ALL if the length should be calculated.
     * \return  Returns number of characters in buffer.
     **/
    template<typename CharType>
    CharCount trimAll( CharType * strBuffer, NEString::CharCount strLen = NEString::COUNT_ALL );

    /**
     * \brief   In removes white-space characters from left- and right-side (from begin and end)
     *          and copies the rest to the destination buffer.
     * \param   strDst  The destination buffer to copy string.
     * \param   lenDst  The destination buffer length to copy characters.
     *                  The length should be big enough to copy.
     *                  Otherwise, it will copy only first (lenDst - 1) characters and set end-of-string at the end.
     * \param   strSrc  The source string buffer.
     * \param   lenSrc  The length of the source string or NEString:COUNT_ALL if the length should be calculated.
     * \return  Returns number of characters copied to the destination buffer.
     **/
    template<typename CharDst, typename CharSrc>
    CharCount trimAll( CharDst *strDst, NEString::CharCount lenDst, const CharSrc * strSrc, NEString::CharCount lenSrc = NEString::COUNT_ALL );

    /**
     * \brief   Removes specified character from the string.
     * 
     * \param   chRemove    The character to search and remove.
     * \param   strSource   The string where the character should be searched and removed.
     * \param   count       The number of characters to search in the string. By default it searches the complete string.
     * \param   removeAll   Flag, indicating whether it should remove only once or all matches.
     * \return  Returns the number of characters in the modified string.
     **/
    template<typename CharType>
    NEString::CharCount removeChar(const CharType chRemove, CharType* strSource, NEString::CharCount count = NEString::COUNT_ALL, bool removeAll = true);

    /**
     * \brief   Search a string inside of other string. The search starts at given position in the string.
     *          If startPos is equal to NEString::START_POS, the search starts at the begin of string. Otherwise, searches at given position.
     *          If found the phrase in string, returns the position in buffer where could find character,
     *          which is same as number of characters to skip from begin to match the phrase.
     *          Otherwise, returns NEString::INVALID_POS.
     * \param   strPhrase   The phrase to search in the string.
     * \param   strSource   The source of string to search.
     * \param   startPos    The starting position to search phrase. If NEString::START_POS, starts searching at begin of given string buffer.
     * \param   out_next    If the parameter is not nullptr, then:
     *                          - On output, if not nullptr, this contains pointer in source string next after phrase.
     *                          - On output, if nullptr, could not find the phrase.
     *                      If parameter is nullptr, it is ignored.
     * \return  If found the phrase, returns the position in buffer where phrase starts.
     *          If did not find, returns NEString::INVALID_POS
     **/
    template<typename CharType>
    CharPos findFirst( const CharType * strPhrase
                     , const CharType * strSource
                     , CharPos startPos           = NEString::START_POS
                     , const CharType ** out_next = nullptr );

    /**
     * \brief   Search the match of given character inside of string. The search starts at given position
     *          If startPos is equal to NEString::START_POS, the search starts at the begin of string. Otherwise, searches at given position.
     *          If found character in string, returns the position in buffer where phrase starts,
     *          which is same as number of characters to skip from begin to match the character.
     *          Otherwise, returns NEString::INVALID_POS.
     * \param   chSearch    The character to search in the string.
     * \param   strSource   The source of string to search.
     * \param   startPos    The starting position to search phrase. If NEString::START_POS, starts searching at begin of given string buffer.
     * \param   out_next    If the parameter is not nullptr, then:
     *                          - On output, if not nullptr, this contains pointer in source string next after character.
     *                          - On output, if nullptr, could not find the character.
     *                      If parameter is nullptr, it is ignored.
     * \return  If found the character, returns the position in buffer where met character.
     *          If did not find, returns NEString::INVALID_POS
     **/
    template<typename CharType>
    CharPos findFirst( CharType chSearch
                       , const CharType * strSource
                       , CharPos startPos           = NEString::START_POS
                       , const CharType ** out_next = nullptr );

    /**
     * \brief   Reverse search a string inside of other string. The search starts at the given position and moves to begin of string.
     *          If startPos is equal to NEString::END_POS, the search starts at the end of string. Otherwise, searches at given position.
     *          If found the phrase in string, returns the position in buffer where phrase starts,
     *          which is same as number of characters to skip from begin to match the phrase.
     *          Otherwise, returns NEString::INVALID_POS.
     * \param   strPhrase   The phrase to search in the string.
     * \param   strSource   The source of string to search.
     * \param   startPos    The starting position to search phrase. If NEString::END_POS, starts searching at the end of given string buffer.
     * \param   out_next    If the parameter is not nullptr, then:
     *                          - On output, if not nullptr, this contains pointer in source string before phrase starts.
     *                          - On output, if nullptr, could not find the phrase.
     *                      If parameter is nullptr, it is ignored.
     * \return  If found the phrase, returns the position in buffer where phrase starts.
     *          If did not find, returns NEString::INVALID_POS
     * \note    This is reverse search, but the returned position is relevant to begin of string.
     **/
    template<typename CharType>
    CharPos findLast( const CharType * strPhrase
                      , const CharType * strSource
                      , CharPos startPos            = NEString::END_POS
                      , const CharType ** out_next  = nullptr );

    /**
     * \brief   Reverse search the match of given character inside of string. The search starts at the given position and moves to begin of string.
     *          If startPos is equal to NEString::END_POS, the search starts at the end of string. Otherwise, searches at given position.
     *          If found the phrase in string, returns the position in buffer where could find character,
     *          which is same as number of characters to skip from begin to match the character.
     *          Otherwise, returns NEString::INVALID_POS.
     * \param   chSearch    The character to search in the string.
     * \param   strSource   The source of string to search.
     * \param   startPos    The starting position to search phrase. If NEString::END_POS, starts searching at the end of given string buffer.
     * \param   out_next    If the parameter is not nullptr, then:
     *                          - On output, if not nullptr, this contains pointer in source string before character found.
     *                          - On output, if nullptr, could not find the character.
     *                      If parameter is nullptr, it is ignored.
     * \return  If found the character, returns the position in buffer where met character.
     *          If did not find, returns NEString::INVALID_POS
     * \note    This is reverse search, but the returned position is relevant to begin of string.
     **/
    template<typename CharType>
    CharPos findLast( CharType chSearch
                      , const CharType * strSource
                      , CharPos startPos            = NEString::END_POS
                      , const CharType ** out_next  = nullptr );

    /**
     * \brief   Returns true if a give string starts with specified phrase.
     * \param   strString       The string to check the phrase.
     * \param   phrase          The phrase to check.
     * \param   caseSensitive   If false, it with check ignoring upper / lower case. Otherwise, checks exact match.
     * \return  Returns true if the string starts with given phrase.
     **/
    template<typename CharType>
    bool stringStartsWith(const CharType * strString, const CharType * phrase, bool caseSensitive = true);

    /**
     * \brief   Returns true if a give string ends with specified phrase.
     * \param   strString   The string to check the phrase.
     * \param   phrase      The phrase to check.
     * \param   caseSensitive If false, it with check ignoring upper / lower case. Otherwise, checks exact match.
     * \return  Returns true if the string ends with given phrase.
     **/
    template<typename CharType>
    bool stringEndsWith(const CharType * strString, const CharType * phrase, bool caseSensitive = true);

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
     * \example NEString::getPrintable( )
     *
     *          Let's consider example:
     *
     *  char buffer[]   = {'1', '2', '\0', '4', 5, 6, '7', '8', '9', 127 };
     *  int count       = sizeof(buffer);
     *  char * next     = buffer;
     *
     *  const char * result1 = NEString::getPrintable<char>(next, count, &next);  // Results: result1 = "12"   , next = {'4', 5, 6, '7', '8', '9', 127}
     *  count -= getStringLength<char>(result1) + 1;
     *  const char * result2 = NEString::getPrintable<char>(next, count, &next);  // Results: result2 = "4"    , next = {6, '7', '8', '9', 127}
     *  count -= getStringLength<char>(result2) + 1;
     *  const char * result3 = NEString::getPrintable<char>(next, count, &next);  // Results: result3 = ""     , next = {'7', '8', '9', 127}
     *  count -= getStringLength<char>(result3) + 1;
     *  const char * result4 = NEString::getPrintable<char>(next, count, &next);  // Results: result4 = "789"  , next = nullptr
     **/
    template<typename CharType>
    const CharType * getPrintable( CharType * strSource, CharCount charCount, CharType ** out_next = nullptr );

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
    const CharType * getLine( CharType * strSource, CharCount charCount = NEString::COUNT_ALL, CharType ** out_next = nullptr );

    /**
     * \brief   Converts the given string to digit in base 10.
     *          White-space at begin will be ignored. The negative
     */
    template<typename CharType>
    int makeInteger(const CharType * strNumber, const CharType ** remain);
}

//////////////////////////////////////////////////////////////////////////
// NEString namespace function templates or inline methods implementation
//////////////////////////////////////////////////////////////////////////

template<typename CharType, typename IntType>
int NEString::makeString( CharType * strDst, NEString::CharCount charCount, IntType digit, NEString::eRadix radix )
{
    int result = 0;
    if ( (NEString::isEmpty<CharType>(strDst) == false) && (charCount > 1) )
    {
        static const CharType _valid[]  = {'0', '1', '2', '3', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', '\0'};

        CharType * dst = strDst;
        IntType num    = MACRO_ABS(digit);
        radix = radix != NEString::eRadix::RadixAutomatic ? radix : NEString::eRadix::RadixDecimal;

        if ((radix >= NEString::eRadix::RadixBinary) && (radix <= NEString::eRadix::RadixHexadecimal) )
        {
            do
            {
                *dst ++ = _valid[num % static_cast<IntType>(radix)];
                num /= static_cast<IntType>(radix);
                -- charCount;
            } while ( (num != 0) && (charCount != 0) );

            if ( (charCount != 0) && (digit < 0) && (radix == NEString::eRadix::RadixDecimal) )
                *dst ++ = '-';

            *dst = static_cast<CharType>(NEString::EndOfString);
            NEString::swapString<CharType>( strDst );
        }

        result = dst - strDst;
    }
    else
    {
        IntType num = MACRO_ABS( digit );
        radix = radix != NEString::eRadix::RadixAutomatic ? radix : NEString::eRadix::RadixDecimal;

        if ( (radix >= NEString::eRadix::RadixBinary) && (radix <= NEString::eRadix::RadixHexadecimal) )
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
void NEString::swapString( CharType * strDst, NEString::CharCount charCount /*= NEString::COUNT_ALL*/ )
{
    if ( NEString::isEmpty<CharType>( strDst ) == false )
    {
        charCount = charCount == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(strDst) : charCount;
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
int NEString::makeInteger(const CharType * strNumber, const CharType ** remain)
{
    NEMath::eDigitSign sign = NEMath::eDigitSign::SignUndefined;
    int result = 0;
    if (isEmpty<CharType>(strNumber) == false)
    {
        CharType negative = getChar(NEMath::eDigitSign::SignNegative);
        CharType positive = getChar(NEMath::eDigitSign::SignPositive);
        for ( CharType ch = *strNumber; *strNumber != static_cast<CharType>(EndOfString); ++ strNumber )
        {
            ch = *strNumber;
            if (sign == NEMath::eDigitSign::SignUndefined)
            {
                if ( isWhitespace(ch) == false )
                {
                    sign = ch == negative ? NEMath::eDigitSign::SignNegative : NEMath::eDigitSign::SignPositive;
                    if ((ch == negative) || (ch == positive))
                    {
                        // the sign is '-' or '+'
                        continue;
                    }
                }
                else
                {
                    continue;
                }
            }

            if ( isNumeric(ch) )
            {
                result *= 10;
                result += ch - static_cast<CharType>('0');
            }
            else
            {
                break;
            }
        }
    }
    
    if ( remain != nullptr )
        *remain = strNumber;
    
    return (static_cast<int>(sign) * result);
}

template<typename CharType>
const CharType * NEString::getLine( CharType * strSource, NEString::CharCount charCount/*= COUNT_ALL*/, CharType ** out_next /*= nullptr */ )
{
    const CharType* result = strSource;

    if (out_next != nullptr)
        *out_next = nullptr;

    if (NEString::isEmpty(strSource) == false)
    {
        charCount = charCount == NEString::COUNT_ALL ? NECommon::VALUE_MAX_INT32 : charCount;
        while ((NEString::isEndOfString<CharType>(*strSource) == false) && (charCount > 0))
        {
            if (NEString::isEndOfLine<CharType>(*strSource))
            {
                *strSource ++ = static_cast<CharType>(NEString::EndOfString);
                break;
            }

            ++strSource;
        }
    }

    if (out_next != nullptr)
    {
        *out_next = (strSource == nullptr) || NEString::isEndOfString<CharType>(*strSource) ? nullptr : strSource;
    }

    return result;
}

template<typename CharType>
const CharType * NEString::getPrintable( CharType * strSource, NEString::CharCount charCount, CharType ** out_next /*= nullptr */ )
{
    const CharType * result = nullptr;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( NEString::isEmpty<CharType>(strSource) == false )
    {
        charCount = charCount == NEString::COUNT_ALL ? MAX_INT_32 : charCount;
        if ( charCount > 0 )
        {
            result = strSource;

            // move pointer until char is printable
            while ( charCount != 0 )
            {
                if ( NEString::isPrintable<CharType>(*strSource) == false )
                {
                    // set next position if not EOS.
                    if ( (out_next != nullptr) && (NEString::isEndOfString<CharType>(*strSource) == false) )
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
NEString::CharPos NEString::findLast( CharType   chSearch
                                     , const CharType * strSource
                                     , NEString::CharPos startPos /*= NEString::END_POS*/
                                     , const CharType ** out_next /*= nullptr*/ )
{
    NEString::CharPos result= NEString::INVALID_POS;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (isEmpty<CharType>( strSource ) == false) && (chSearch != static_cast<CharType>(NEString::EndOfString)) )
    {
        CharPos posSrc = startPos == NEString::END_POS ? NEString::getStringLength<CharType>(strSource) - 1 : startPos;
        if ( posSrc >= NEString::START_POS )
        {
            const CharType * end = strSource + posSrc;
            while (end >= strSource)
            {
                if ( *end == chSearch )
                {
                    result = MACRO_ELEM_COUNT(strSource, end);
                    --end;
                    if ( (out_next != nullptr) && (end >= strSource) )
                        *out_next =  end;

                    break;
                }

                -- end;
            }
        }
    }

    return result;
}

template<typename CharType>
NEString::CharPos NEString::findLast( const CharType * strPhrase
                                      , const CharType * strSource
                                      , NEString::CharPos startPos /*= NEString::END_POS*/
                                      , const CharType ** out_next /*= nullptr*/ )
{
    NEString::CharPos result= NEString::INVALID_POS;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (isEmpty<CharType>( strSource ) == false) && (isEmpty<CharType>( strPhrase ) == false) )
    {
        CharPos posSrc = startPos == NEString::END_POS ? NEString::getStringLength<CharType>(strSource) - 1 : startPos;
        CharPos posPhr = NEString::getStringLength<CharType>(strPhrase) - 1;
        if ( (posSrc >= NEString::START_POS) && (posPhr >= NEString::START_POS) )
        {
            const CharType * end    = strSource + posSrc;
            const CharType * phrase = strPhrase + posPhr;

            while (end >= strSource)
            {
                if ( *end == *phrase )
                {
                    const CharType * one = end    - 1;
                    const CharType * two = phrase - 1;
                    // no need to check (*one != static_cast<CharType>(EndofString))
                    while ( (two >= strPhrase) && (*one == *two) )
                    {
                        -- one;
                        -- two;
                    }

                    if (  two < strPhrase )
                    {
                        result = static_cast<NEString::CharPos>(one + 1 - strSource);
                        if ( (out_next != nullptr) && (one >= strSource) )
                            *out_next = one;

                        break; // break the loop
                    }
                }

                -- end;
            }
        }
    }

    return result;
}

template<typename CharType>
NEString::CharPos NEString::findFirst( CharType chSearch
                                       , const CharType * strSource
                                       , NEString::CharPos startPos /*= NEString::START_POS*/
                                       , const CharType ** out_next /*= nullptr*/ )
{
    NEString::CharPos result = NEString::INVALID_POS;
    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (isEmpty<CharType>(strSource) == false) && (chSearch != static_cast<CharType>(NEString::EndOfString)) )
    {
        if ( startPos >= NEString::START_POS )
        {
            const CharType * next = strSource + startPos;
            while ( *next != static_cast<CharType>(EndOfString) )
            {
                if (*next == chSearch)
                {
                    result = static_cast<NEString::CharPos>(next - strSource);
                    next += 1;
                    if ( (out_next != nullptr) && (*next != static_cast<CharType>(EndOfString)) )
                        *out_next = next;

                    break; // break the loop
                }

                ++ next;
            }
        }
    }

    return result;
}

template<typename CharType>
NEString::CharPos NEString::findFirst( const CharType * strPhrase
                                       , const CharType * strSource
                                       , NEString::CharPos startPos /*= NEString::START_POS*/
                                       , const CharType ** out_next /*= nullptr*/ )
{
    NEString::CharPos result= NEString::INVALID_POS;

    if ( out_next != nullptr )
        *out_next = nullptr;

    if ( (isEmpty<CharType>(strSource) == false) && (isEmpty<CharType>(strPhrase) == false) )
    {
        if ( startPos >= NEString::START_POS )
        {
            const CharType * next = strSource + startPos;
            while ( *next != static_cast<CharType>(NEString::EndOfString) )
            {
                if ( *next == *strPhrase)
                {
                    const CharType * one = next + 1;
                    const CharType * two = strPhrase + 1;
                    // no need to check (*one != static_cast<CharType>(EndofString))

                    while ( (*two != static_cast<CharType>(NEString::EndOfString)) && (*one == *two) )
                    {
                        ++ one;
                        ++ two;
                    }

                    if (*two == static_cast<CharType>(NEString::EndOfString))
                    {
                        result = static_cast<NEString::CharPos>(next - strSource);
                        if ((out_next != nullptr) && (*one != static_cast<CharType>(NEString::EndOfString)))
                            *out_next = one;

                        break; // break the loop
                    }
                }

                ++ next;
            }
        }
    }

    return result;
}

template<typename CharType>
bool NEString::stringStartsWith(const CharType * strString, const CharType * phrase, bool caseSensitive /*= true*/)
{
    bool result = false;
    if ((isEmpty<CharType>(strString) == false) && (isEmpty<CharType>(phrase) == false))
    {
        result = true;
        for ( ; (*phrase != NEString::EndOfString) && (*strString == NEString::EndOfString); ++ strString, ++ phrase )
        {
            CharType ch1 = *strString;
            CharType ch2 = *phrase;
            if ( caseSensitive == false )
            {
                ch1 = NEString::makeAsciiLower<CharType>(ch1);
                ch2 = NEString::makeAsciiLower<CharType>(ch2);
            }

            if (ch1 != ch2)
            {
                result = false;
                break;
            }
        }

        result = result && (*phrase == NEString::EndOfString);
    }

    return result;
}

template<typename CharType>
bool NEString::stringEndsWith(const CharType * strString, const CharType * phrase, bool caseSensitive /*= true*/)
{
    bool result = false;
    if ((isEmpty<CharType>(strString) == false) && (isEmpty<CharType>(phrase) == false))
    {
        int lenString   = NEString::getStringLength<CharType>(strString);
        int lenPhrase   = NEString::getStringLength<CharType>(phrase);
        
        ASSERT(lenString > 0);
        ASSERT(lenPhrase > 0);

        int diff = lenString - lenPhrase;

        result = (diff >= 0) && NEString::stringStartsWith<CharType>(strString + diff, phrase, caseSensitive);
    }

    return result;
}

template<typename CharType>
NEString::CharCount NEString::removeChar(const CharType chRemove, CharType* strSource, NEString::CharCount count /*= NEString::COUNT_ALL*/, bool removeAll /*= true*/)
{
    count = count == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(strSource) : count;
    if (count == 0)
        return 0;

    CharType* dst = strSource;
    const CharType* src = strSource;
    while ((count > 0) && (NEString::isEndOfString(*src) == false))
    {
        --count;
        if (*src == chRemove)
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

    while (NEString::isEndOfString(*src) == false)
        *dst++ = *src++;

    *dst = static_cast<CharType>(NEString::EndOfString);
    
    return static_cast<NEString::CharCount>(dst - strSource);
}


template<typename CharDst, typename CharSrc>
NEString::CharCount NEString::trimAll( CharDst *            strDst
                                     , NEString::CharCount  lenDst
                                     , const CharSrc *      strSrc
                                     , NEString::CharCount  lenSrc /*= NEString::COUNT_ALL*/ )
{
    NEString::CharCount result = 0;

    if ( (strDst != nullptr) && (lenDst > 0) )
    {
        CharDst * dst = strDst;
        *dst    = static_cast<CharDst>(NEString::EndOfString);
        lenSrc  = lenSrc == NEString::COUNT_ALL ? NEString::getStringLength<CharSrc>(strSrc) : lenSrc;

        if ( (lenSrc > 0) && (NEString::isEmpty<CharSrc>(strSrc) == false) )
        {
            const CharSrc * end     = strSrc + lenSrc - 1;
            const CharSrc * begin   = strSrc;

            while ((end > begin) && NEString::isWhitespace<CharSrc>(*end) )
                -- end;

            ++ end;

            while ( (begin < end) && NEString::isWhitespace<CharSrc>(*begin) )
                ++ begin;

            if ( NEString::isWhitespace<CharSrc>(*begin) ==  false )
            {
                for ( ; (begin < end) && (lenDst > 1); -- lenDst)
                    *dst ++ = static_cast<CharDst>(*begin ++);

                result = static_cast<int>(dst - strDst);
                *dst   = static_cast<CharDst>(NEString::EndOfString);
            }
        }
    }

    return result;
}

template<typename CharType>
NEString::CharCount NEString::trimAll( CharType * strBuffer, NEString::CharCount strLen /*= NEString::COUNT_ALL*/ )
{
    NEString::CharCount result = 0;
    if ( NEString::isEmpty<CharType>( strBuffer ) == false )
    {
        strLen = strLen == NEString::COUNT_ALL ? NEString::getStringLength<CharType>( strBuffer ) : strLen;
        if ( strLen > 0 )
        {
            CharType * end  = strBuffer + strLen - 1;
            CharType * begin= strBuffer;
            CharType * buf  = strBuffer;

            while ( (end > begin) && NEString::isWhitespace<CharType>(*end) )
                -- end;

            *(++ end) = static_cast<CharType>(NEString::EndOfString);

            while ( (begin < end) && NEString::isWhitespace<CharType>(*begin) )
                ++ begin;

            result = static_cast<int>(end - begin);
            if (begin != buf)
            {
                while (begin < end)
                    *buf ++ = *begin ++;

                *buf = static_cast<CharType>(NEString::EndOfString);
            }
        }
    }

    return result;
}

template<typename CharDst, typename CharSrc>
NEString::CharCount NEString::trimRight( CharDst *           strDst
                                       , NEString::CharCount lenDst
                                       , const CharSrc *     strSrc
                                       , NEString::CharCount lenSrc /*= NEString::COUNT_ALL*/ )
{
    NEString::CharCount result = 0;
    if ((NEString::isEmpty<CharSrc>(strSrc) == false) && (strDst != nullptr) && (lenDst > 0) )
    {
        lenSrc = lenSrc == NEString::COUNT_ALL ? NEString::getStringLength<CharSrc>(strSrc) : lenSrc;
        if ( lenSrc > 0 )
        {
            result = MACRO_MIN(lenSrc, lenDst - 1);
            const CharSrc * end   = strSrc + lenSrc - 1;
            const CharSrc * begin = strSrc;
            CharDst * dst         = strDst;

            while ( (end != strSrc) && NEString::isWhitespace<CharSrc>(*end) )
                -- end;

            ++ end;

            for ( ; (begin < end) && (lenDst > 1); -- lenDst)
                *dst ++ = static_cast<CharDst>(*begin ++);

            result = static_cast<int>(dst - strDst);
            *dst = static_cast<CharDst>(NEString::EndOfString);
        }
    }

    return result;
}

template<typename CharType>
NEString::CharCount NEString::trimRight( CharType * strBuffer, NEString::CharCount strLen /*= NEString::COUNT_ALL*/ )
{
    NEString::CharCount result = 0;
    if ( NEString::isEmpty<CharType>( strBuffer ) == false )
    {
        strLen = strLen == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(strBuffer) : strLen;
        if ( strLen > 0 )
        {
            CharType * end  = strBuffer + strLen - 1;
            CharType * begin= strBuffer;

            while ( (end > begin) && NEString::isWhitespace<CharType>( *end ) )
                -- end;

            *(++ end) = static_cast<CharType>(NEString::EndOfString);
            result = static_cast<int>(end - begin);
        }
    }

    return result;
}

template<typename CharDst, typename CharSrc>
NEString::CharCount NEString::trimLeft( CharDst *           strDst
                                      , NEString::CharCount lenDst
                                      , const CharSrc *     strSrc
                                      , NEString::CharCount lenSrc /*= NEString::COUNT_ALL*/ )
{
    NEString::CharCount result = 0;
    if ((NEString::isEmpty<CharSrc>(strSrc) == false) && (strDst != nullptr) && (lenDst > 0) )
    {
        lenSrc = lenSrc == NEString::COUNT_ALL ? NEString::getStringLength<CharSrc>(strSrc) : lenSrc;
        if ( lenSrc > 0 )
        {
            const CharSrc * begin = strSrc;
            const CharSrc * end   = strSrc + lenSrc;
            CharDst * dst         = strDst;

            while ((end > begin) && NEString::isWhitespace<CharSrc>(*begin) )
                ++ begin;

            while (begin < end)
                *dst ++ = static_cast<CharDst>(*begin ++);

            result  = static_cast<int>(dst - strDst);
            *dst    = static_cast<CharDst>(NEString::EndOfString);
        }
    }

    return result;
}

template<typename CharType>
NEString::CharCount NEString::trimLeft( CharType * strBuffer, NEString::CharCount strLen /*= NEString::COUNT_ALL*/ )
{
    NEString::CharCount result = 0;
    if ( NEString::isEmpty<CharType>( strBuffer ) == false )
    {
        strLen = strLen == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(strBuffer) : strLen;
        if ( strLen > 0 )
        {
            result = strLen;

            const CharType * begin = strBuffer;
            const CharType * end   = strBuffer + strLen;
            CharType * buf         = strBuffer;

            while ( (end > begin) && NEString::isWhitespace<CharType>(*begin) )
                ++ begin;

            if ( begin != buf )
            {
                while ( begin < end)
                    *buf ++ = *begin ++;

                result  = static_cast<int>(buf - strBuffer);
                *buf    = static_cast<CharType>(NEString::EndOfString);
            }
        }
    }

    return result;
}

template<typename CharType>
inline bool NEString::isOneOf(CharType ch, const CharType* chSequence)
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
inline bool NEString::isReadable( CharType ch )
{
    constexpr unsigned int def = NEString::eCharDefs::CD_Letter | 
                                 NEString::eCharDefs::CD_Number |
                                 NEString::eCharDefs::CD_Symbol;

    return (((NEString::getISO8859CharDef( ch ) & def) != 0) || (ch == ' '));
}

template<typename CharType>
inline bool NEString::isPrintable( CharType ch )
{
    return ((NEString::getISO8859CharDef( ch ) & static_cast<unsigned short>(NEString::eCharDefs::CD_Printable)) != 0);
}

template<typename CharType>
inline bool NEString::isEndOfLine( CharType ch )
{
    return ((NEString::getISO8859CharDef( ch ) & static_cast<unsigned short>(NEString::eCharDefs::CD_EndOfLine)) != 0);
}

template<typename CharType>
inline bool NEString::isCarriageReturn( CharType ch )
{
    return ((NEString::getISO8859CharDef( ch ) & static_cast<unsigned short>(NEString::eCharDefs::CD_CarReturn)) != 0);
}

template<typename CharType>
inline bool NEString::isEndOfString( CharType ch )
{
    return ((NEString::getISO8859CharDef(ch) & static_cast<unsigned short>(NEString::eCharDefs::CD_EOfS)) != 0);
}

template<typename CharType>
inline bool NEString::isSymbol( CharType ch )
{
    return ((NEString::getISO8859CharDef(ch) & static_cast<unsigned short>(NEString::eCharDefs::CD_Symbol)) != 0);
}

template<typename CharType>
inline bool NEString::isWhitespace( CharType ch )
{
    return ((NEString::getISO8859CharDef( ch ) & static_cast<unsigned short>(NEString::eCharDefs::CD_WhiteSpace)) != 0);
}

template<typename CharType>
inline bool NEString::isDelimited( CharType ch )
{
    return ((NEString::getISO8859CharDef( ch ) & static_cast<unsigned short>(NEString::eCharDefs::CD_Delimiter)) != 0);
}

template<typename CharType>
inline bool NEString::isLetter(CharType ch)
{
    return ((NEString::getISO8859CharDef( ch ) & static_cast<unsigned short>(NEString::eCharDefs::CD_Letter)) != 0);
}

template<typename CharType>
inline bool NEString::isNumeric(CharType ch)
{
    return ((NEString::getISO8859CharDef( ch ) & static_cast<unsigned short>(NEString::eCharDefs::CD_Number)) != 0);
}

template<typename CharType>
inline bool NEString::isAlphanumeric(CharType ch)
{
    constexpr unsigned int def = NEString::eCharDefs::CD_Letter | 
                                 NEString::eCharDefs::CD_Number;

    return ((NEString::getISO8859CharDef( ch ) & def) != 0);
}

inline bool NEString::isPositionValid(NEString::CharPos pos)
{
    return (pos >= NEString::START_POS);
}

template<typename CharType>
inline bool NEString::isEmpty( const CharType * strBuffer )
{
    return (strBuffer == nullptr) || (*strBuffer == static_cast<CharType>(NEString::EndOfString));
}

template <typename CharType>
inline NEString::CharCount NEString::getStringLength( const CharType * theString )
{
    NEString::CharCount result = 0;
    if ( NEString::isEmpty<CharType>(theString) == false )
    {
        result = 1;
        while ( * ++ theString != static_cast<CharType>(EndOfString) )
            ++ result;
    }
    return result;
}

template <typename CharType>
inline NEString::CharCount NEString::getStringLineLength(const CharType* theString)
{
    const CharType* start = theString;
    if (theString != nullptr)
    {
        while(!NEString::isEndOfLine<CharType>(*theString) && !NEString::isEndOfString<CharType>(*theString))
        {
            ++theString;
        }
    }

    return MACRO_ELEM_COUNT(start, theString);
}

template<typename CharDst, typename CharSrc>
NEString::CharCount NEString::copyString( CharDst *           strDst
                                        , NEString::CharCount dstSpace
                                        , const CharSrc *     strSrc
                                        , NEString::CharCount charsCopy /*= NEString::COUNT_ALL*/)
{
    constexpr int sizeSrc = sizeof( CharSrc );
    constexpr int sizeDst = sizeof( CharDst );

    NEString::CharCount result = 0;

    if ( sizeSrc == sizeDst )
    {
        if ( strDst != nullptr )
        {
            charsCopy = charsCopy == NEString::COUNT_ALL ? NEString::getStringLength<CharSrc>( strSrc ) : charsCopy;
            result = NEMemory::memCopy( strDst, dstSpace * sizeDst, strSrc, charsCopy * sizeSrc ) / sizeDst;
            strDst[result] = NEString::EndOfString;
        }
    }
    else if ( (dstSpace > 0) && (strDst != nullptr) && (strSrc != nullptr) )
    {
        charsCopy = charsCopy == NEString::COUNT_ALL ? MAX_INT_32 : charsCopy;
        if ( charsCopy > 0 )
        {
            CharDst * dst = strDst;
            dstSpace -= 1;
            charsCopy = MACRO_MIN(dstSpace, charsCopy);
            for ( ; (*strSrc != static_cast<CharDst>(EndOfString)) && (charsCopy != 0); -- charsCopy )
                *dst++ = static_cast<CharDst>(*strSrc++);

            *dst = static_cast<CharDst>(EndOfString);
            result = static_cast<NEString::CharCount>(dst - strDst);
        }
    }
    else if (strDst != nullptr)
    {
        strDst[0] = NEString::EndOfString;
    }

    return result;
}

template <typename CharType>
inline CharType NEString::makeAsciiLower(CharType ch)
{
    // return ((ch >= 'A') && (ch <= 'Z') ? ch - 'A' + 'a' : ch);
    return static_cast<CharType>(NEString::getISO8859LowerChar(ch));
}

template <typename CharType>
inline CharType NEString::makeAsciiUpper(CharType ch)
{
    // return ((ch >= 'a') && (ch <= 'z') ? ch - 'a' + 'A' : ch);
    return static_cast<CharType>(NEString::getISO8859UpperChar(ch));
}

template<typename CharLhs, typename CharRhs>
inline NEMath::eCompare NEString::compareIgnoreCase( const CharLhs *leftSide, const CharRhs * rightSide )
{
    return NEString::compareStrings<CharLhs, CharRhs>(leftSide, rightSide, NEString::COUNT_ALL, false);
}

template<typename CharLhs, typename CharRhs>
NEMath::eCompare NEString::compareStrings( const CharLhs *leftSide
                                         , const CharRhs * rightSide
                                         , NEString::CharCount charCount    /*= COUNT_ALL*/
                                         , bool caseSensitive               /*= true*/)
{
    NEMath::eCompare result = NEMath::eCompare::Bigger;

    if (charCount == NEString::COUNT_ALL)
    {
        result = caseSensitive ? NEString::compareFast<CharLhs, CharRhs>(leftSide, rightSide) : NEString::compareFastIgnoreCase<CharLhs, CharRhs>(leftSide, rightSide);
    }
    else if ( reinterpret_cast<const void *>(leftSide) == reinterpret_cast<const void *>(rightSide) )
    {
        result = NEMath::eCompare::Equal;
    }
    else if ( (leftSide != nullptr) && (rightSide != nullptr) )
    {
        if (caseSensitive)
        {
            result = NEString::compareFast<CharLhs, CharRhs>(leftSide, rightSide, charCount);
        }
        else
        {
            CharLhs chLeft = 0;
            CharRhs chRight = 0;
            while ((charCount-- > 0) && (*leftSide != NEString::EndOfString) && (*rightSide != NEString::EndOfString))
            {
                chLeft = NEString::makeAsciiLower<CharLhs>(*leftSide);
                chRight = NEString::makeAsciiLower<CharRhs>(*rightSide);
                if (chLeft != static_cast<CharLhs>(chRight))
                {
                    break;
                }

                ++leftSide;
                ++rightSide;
            }

            if (chLeft < static_cast<CharLhs>(chRight))
                result = NEMath::eCompare::Smaller;
            else if (chLeft == static_cast<CharLhs>(chRight))
                result = NEMath::eCompare::Equal;
        }
    }
    else if ( rightSide != nullptr )
    {
        result = NEMath::eCompare::Smaller;
    }

    return result;
}

template<typename CharLhs, typename CharRhs>
inline NEMath::eCompare NEString::compareFast(const CharLhs *leftSide, const CharRhs * rightSide)
{
    NEMath::eCompare result = NEMath::eCompare::Bigger;

    if ( reinterpret_cast<const void *>(leftSide) == reinterpret_cast<const void *>(rightSide) )
    {
        result = NEMath::eCompare::Equal;
    }
    else if ( (leftSide != nullptr) && (rightSide != nullptr) )
    {
        while ((*leftSide != NEString::EndOfString) && (*rightSide != NEString::EndOfString))
        {
            if (*leftSide != static_cast<CharLhs>(*rightSide))
            {
                break;
            }

            ++leftSide;
            ++rightSide;
        }

        if (*leftSide == static_cast<CharLhs>(*rightSide))
            result = NEMath::eCompare::Equal;
        else if (*leftSide < static_cast<CharLhs>(*rightSide))
            result = NEMath::eCompare::Smaller;
    }
    else if (rightSide != nullptr)
    {
        result = NEMath::eCompare::Smaller;
    }

    return result;
}

template<typename CharType>
inline NEMath::eCompare NEString::compare(const CharType * leftSide, const CharType * rightSide)
{
    return NEString::compare<CharType, CharType>(leftSide, rightSide);
}


template<typename CharLhs, typename CharRhs>
inline NEMath::eCompare NEString::compareFast( const CharLhs *leftSide, const CharRhs * rightSide, NEString::CharCount count )
{
    NEMath::eCompare result = NEMath::eCompare::Bigger;

    if ( count == NEString::COUNT_ALL )
    {
        result = NEString::compareFast<CharLhs, CharRhs>(leftSide, rightSide);
    }
    else if ( reinterpret_cast<const void *>(leftSide) == reinterpret_cast<const void *>(rightSide) )
    {
        result = NEMath::eCompare::Equal;
    }
    else if ( (leftSide != nullptr) && (rightSide != nullptr) )
    {
        while ((*leftSide != NEString::EndOfString) && (*rightSide != NEString::EndOfString))
        {
            if (*leftSide != static_cast<CharLhs>(*rightSide))
            {
                break;
            }

            ++leftSide;
            ++rightSide;
        }

        if (*leftSide == static_cast<CharLhs>(*rightSide))
            result = NEMath::eCompare::Equal;
        else if (*leftSide < static_cast<CharLhs>(*rightSide))
            result = NEMath::eCompare::Smaller;
    }
    else if ( rightSide != nullptr )
    {
        result = NEMath::eCompare::Smaller;
    }

    return result;
}

template<typename CharType>
inline NEMath::eCompare NEString::compareFast( const CharType * leftSide, const CharType * rightSide, NEString::CharCount count )
{
    NEMath::eCompare result = NEMath::eCompare::Bigger;
    if (count == NEString::COUNT_ALL)
    {
        result = NEString::compareFast<CharType, CharType>(rightSide, leftSide);
    }
    else if ( leftSide == rightSide )
    {
        result = NEMath::eCompare::Equal;
    }
    else if ( (leftSide != nullptr) && (rightSide != nullptr) )
    {
        result = NEMemory::memCompare( leftSide, rightSide, count * sizeof( CharType ) );
    }
    else if ( rightSide != nullptr )
    {
        result = NEMath::eCompare::Smaller;
    }

    return result;
}

template<typename CharLhs, typename CharRhs>
inline NEMath::eCompare NEString::compareFastIgnoreCase(const CharLhs *leftSide, const CharRhs * rightSide)
{
    NEMath::eCompare result = NEMath::eCompare::Bigger;

    if ( reinterpret_cast<const void *>(leftSide) == reinterpret_cast<const void *>(rightSide) )
    {
        result = NEMath::eCompare::Equal;
    }
    else if ( (leftSide != nullptr) && (rightSide != nullptr) )
    {
        CharLhs chLeft = 0;
        CharRhs chRight = 0;
        while ((*leftSide != NEString::EndOfString) && (*rightSide != NEString::EndOfString))
        {
            chLeft = *leftSide;
            chRight = *rightSide;
            if (chLeft != static_cast<CharLhs>(chRight))
            {
                break;
            }

            ++leftSide;
            ++rightSide;
        }

        if (chLeft == static_cast<CharLhs>(chRight))
            result = NEMath::eCompare::Equal;
        else if (chLeft < static_cast<CharLhs>(chRight))
            result = NEMath::eCompare::Smaller;
    }
    else if ( rightSide != nullptr )
    {
        result = NEMath::eCompare::Smaller;
    }

    return result;
}

#endif  // AREG_BASE_NESTRING_HPP
