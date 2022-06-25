#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/TEString.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, ASCII string Class.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/NEString.hpp"
#include "areg/base/TETemplateBase.hpp"

#include <string>
#include <locale>

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class IEOutStream;

//////////////////////////////////////////////////////////////////////////
// TEString class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The basic string class template, which contains basic operations 
 *          to set, append, insert, search and replace string, as well to get substring.
 *          When deriving class, should be paid special attention on string structure
 *          validation. The string should be allocated and released via methods 
 *          defined in NEString namespace.
 *          The string class template constructors and destructor are protected.
 *          To be able to instantiate string, the class should be derived to define
 *          basic operations.
 * 
 * \tparam  CharType    The type of character. Normally, either 8-bit character or 16-bit wide character.
 * \tparam  Helper      The name of class where the basic string alphabetic, comparing and 
 *                      stream read/write operations are Helper.
 **/
template<typename CharType>
class TEString
{
//////////////////////////////////////////////////////////////////////////
// Public constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Empty character constant
     **/
    static constexpr CharType   EmptyChar       { static_cast<CharType>('\0') };

    /**
     * \brief   New line constant
     **/
    static constexpr CharType   NewLine         { static_cast<CharType>('\n') };

    /**
     * \brief   DOS format new line
     **/
    static constexpr CharType   NewLineDos[2]   {static_cast<CharType>('\r'), static_cast<CharType>('\n')};

//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Default constructor creates empty string.
     **/
    inline TEString( void );

    /**
     * \brief   Sets the char to string.
     * \param   source    The character to set at the begin of string.
     **/
    inline TEString( const CharType source );
    inline TEString( const CharType * source );
    inline TEString( const std::basic_string<CharType>& source );
    inline TEString( const std::basic_string_view<CharType>& source );


    /**
     * \brief   Copies specified amount of symbols from given string.
     * \param   source  The null-terminated string buffer to initialize. If nullptr, empty string is created.
     * \param   count   The number of character to initialize from given string source. If zero, empty string is created.
     **/
    inline TEString( const CharType * source, NEString::CharCount count );

    /**
     * \brief   Copy constructor.
     * \param   source   The source to copy data.
     **/
    inline TEString( const TEString<CharType> & source );

    /**
     * \brief   Move constructor.
     * \param   source   The source to move data.
     **/
    inline TEString( TEString<CharType> && source ) noexcept;
    inline TEString( std::basic_string<CharType>&& source ) noexcept;

    /**
     * \brief   Destructor.
     **/
    ~TEString( void ) = default;

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Converting operator, converts object to string buffer
     **/
    inline operator const CharType* (void) const;

    /**
     * \brief   Assigning operator, copies data from given string source
     * \param   src     The source of string to copy.
     * \return  Returns the string object.
     **/
    inline TEString<CharType>& operator = (const TEString<CharType>& src);
    inline TEString<CharType>& operator = (const std::basic_string<CharType>& src);
    inline TEString<CharType>& operator = (const std::basic_string_view<CharType>& src);
    inline TEString<CharType>& operator = (const CharType* src);
    inline TEString<CharType>& operator = (TEString<CharType> && src) noexcept;
    inline TEString<CharType>& operator = (std::basic_string<CharType> && src) noexcept;
    inline TEString<CharType>& operator = (const CharType src);

    /**
     * \brief   Determines equality of two strings.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use compare() methods.
     * \param   other   The second string to compare
     * \return  Returns true if 2 strings are equal.
     **/
    inline bool operator == (const TEString<CharType>& other) const;
    inline bool operator == (const std::basic_string<CharType>& other) const;
    inline bool operator == (const std::basic_string_view<CharType>& other) const;
    inline bool operator == (const CharType * other) const;
    inline bool operator != (const TEString<CharType>& other) const;
    inline bool operator != (const std::basic_string<CharType>& other) const;
    inline bool operator != (const std::basic_string_view<CharType>& other) const;
    inline bool operator != (const CharType* other) const;

    /**
     * \brief   Comparing operator. Checks whether strings are bigger or smaller.
     * \remark  The comparing of strings is case-sensitive.
     *          To compare case-insensitive, use Compare() method.
     * \param   other   The second string to compare
     * \return  Returns true if existing string is more than given.
     **/
    inline bool operator > (const TEString<CharType>& other) const;
    inline bool operator > (const std::basic_string<CharType>& other) const;
    inline bool operator > (const std::basic_string_view<CharType>& other) const;
    inline bool operator > (const CharType* other) const;
    inline bool operator < (const TEString<CharType>& other) const;
    inline bool operator < (const CharType* other) const;
    inline bool operator < (const std::basic_string<CharType>& other) const;
    inline bool operator < (const std::basic_string_view<CharType>& other) const;
    inline bool operator >= (const TEString<CharType>& other) const;
    inline bool operator >= (const CharType* other) const;
    inline bool operator >= (const std::basic_string<CharType>& other) const;
    inline bool operator >= (const std::basic_string_view<CharType>& other) const;
    inline bool operator <= (const TEString<CharType>& other) const;
    inline bool operator <= (const CharType* other) const;
    inline bool operator <= (const std::basic_string<CharType>& other) const;
    inline bool operator <= (const std::basic_string_view<CharType>& other) const;

    /**
     * \brief   Appends given string at the end of existing string.
     * \param   src     The source of string to append at the end
     * \return  Returns string object
     **/
    inline TEString<CharType>& operator += (const TEString<CharType>& src);
    inline TEString<CharType>& operator += (const std::basic_string<CharType>& src);
    inline TEString<CharType>& operator += (const std::basic_string_view<CharType>& src);
    inline TEString<CharType>& operator += (const CharType* src);

    /**
     * \brief   Appends given character at the end of existing string.
     * \param   chSource   The source of character to append at the end
     * \return  Returns string object
     **/
    inline TEString<CharType>& operator += (const CharType chSource);

    /**
     * \brief   Operator to concatenate 2 strings.
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    template<typename CharType>
    friend inline TEString<CharType> operator + (const TEString<CharType>& lhs, const TEString<CharType>& rhs);
    template<typename CharType>
    friend inline TEString<CharType> operator + (const TEString<CharType>& lhs, const std::basic_string<CharType>& rhs);
    template<typename CharType>
    friend inline TEString<CharType> operator + (const TEString<CharType>& lhs, const std::basic_string_view<CharType>& rhs);
    template<typename CharType>
    friend inline TEString<CharType> operator + (const TEString<CharType>& lhs, const CharType* rhs);
    template<typename CharType>
    friend inline TEString<CharType> operator + (const TEString<CharType>& lhs, const CharType rhs);
    template<typename CharType>
    friend inline TEString<CharType> operator + (const std::basic_string<CharType>& lhs, const TEString<CharType>& rhs);
    template<typename CharType>
    friend inline TEString<CharType> operator + (const std::basic_string_view<CharType>& lhs, const TEString<CharType>& rhs);
    template<typename CharType>
    friend inline TEString<CharType> operator + (const CharType* lhs, const TEString<CharType>& rhs);
    template<typename CharType>
    friend inline TEString<CharType> operator + (const CharType lhs, const TEString<CharType>& rhs);

    /**
     * \brief   Streams to input object, i.e. reads data from streaming object to string,
     *          and initialize string data.
     * \param    stream  Streaming object to read string data
     * \param    input    String object to initialize and write string data.
     * \return    Reference to stream object.
     **/
    template<typename CharType>
    friend inline const IEInStream & operator >> (const IEInStream & stream, TEString<CharType> & input);
    
    /**
     * \brief    Streams from output object, i.e. write data from string to streaming object.
     * \param    stream    Streaming object to write data.
     * \param    output    String object to read data from
     * \return    Reference to stream object.
     **/
    template<typename CharType>
    friend inline IEOutStream & operator << (IEOutStream & stream, const TEString<CharType> & output);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Checks validity of the string. The valid characters are listed in the passed list, which is null-terminated string.
     * \param   validityList    Null-terminated string of valid characters to check the string.
     * \return  Returns true, if string does not contain any character different in validity list.
     *          Otherwise, it returns false
     **/
    inline bool validate( const CharType * validityList ) const;

    /**
     * \brief   Checks whether the string is numeric. Checks only integer numbers.
     * \param   signIgnore      If true, ignores '-' and '+' signs in front of number
     * \return  Returns true if characters are between '0' and '9', or optional '+' or '-' in front if
     *          signs should be ignored.
     **/
    inline bool isNumeric( bool signIgnore = true ) const;

    /**
     * \brief   Returns true if string is contains only letters and numbers. No symbols or other controls are allowed.
     **/
    inline bool isAlphanumeric(void) const;

    /**
     * \brief   Returns true if string is valid for name.
     *          The name is valid if it contains letters, numbers and following symbols {'-', '_', '.', ','}.
     *          All other symbols are rejected.
     **/
    inline bool isValidName(void) const;

    /**
     * \brief   Returns true if the passed character is valid for the name.
     **/
    inline bool isValidNameChar(const CharType checkChar) const;

    /**
     * \brief   Returns true if string is empty
     **/
    inline bool isEmpty(void) const;

    /**
     * \brief   Returns true if the buffer is non-empty string.
     */
    inline bool isString(void) const;

    /**
     * \brief   Returns the length of string in number of characters without null-terminated character at the and of the string.
     **/
    inline NEString::CharCount getLength(void) const;

    /**
     * \brief   Returns the actual length of allocated buffer for string, which is bigger than the length of string.
     *          The returned value is number of characters that can be written in the string without resizing.
     **/
    inline NEString::CharCount getSize(void) const;

    /**
     * \brief   Return the size of string in bytes including zero-termination at the end of the string.
     **/
    inline uint32_t getSpace( void ) const;

    /**
     * \brief   Returns read-only string buffer starting at specified valid position.
     * \param   startAt     The position in string to return read-only buffer.
     *                      By default it returns begin of string buffer.
     * \return  Returns valid pointer of read-only string buffer if specified position
     *          is valid for read. Otherwise, it returns nullptr
     **/
    inline const CharType* getBuffer(NEString::CharPos startAt = NEString::START_POS) const;
    inline CharType* getBuffer(NEString::CharPos startAt = NEString::START_POS);

    /**
     * \brief   Returns null-terminated buffer of string or nullptr pointer if string is invalid.
     **/
    inline const CharType* getString(void) const;

    /**
     * \brief   Returns the string object.
     **/
    inline const std::basic_string<CharType>& getObject(void) const;

    /**
     * \brief   Returns true if specified character position is valid to read, extract or modify
     *          a character from the string, i.e. is less than the length of the string.
     * \param   pos     Zero indexed character position in the string.
     * \return  Returns true if specified position is valid to read character.
     **/
    inline bool isValidPosition(NEString::CharPos pos) const;

    /**
     * \brief   Returns true if specified character position is invalid.
     * \param   pos     Zero indexed character position in the string.
     * \return  Returns true if specified position is valid to read character.
     **/
    inline bool isInvalidPosition(NEString::CharPos pos) const;

    /**
     * \brief   Returns true if specified position is equal to the end of the string,
     *          i.e. either it is equal to NEString::END_POS or points the last
     *          null-character in the string.
     * \param   pos     The zero-based position in the string.
     **/
    inline bool isEndPosition(NEString::CharPos pos) const;

    /**
     * \brief   Returns true if string is not empty and the specified position is zero.
     **/
    inline bool isStartPosition(NEString::CharPos pos) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Clears the string, makes it empty and invalid.
     **/
    inline void clear(void);

    /**
     * \brief   Delete unused capacity in the string.
     **/
    inline void freeExtra(void);

    /**
     * \brief   Sets the size of string to zero and deletes all unused capacity of the string.
     */
    inline void release(void);

    /**
     * \brief   Searches the word in the string. On output, the 'startAt' and 'endAt' contain
     *          start and end of position where each word start.
     * 
     * \param startAt [in, out] On input, this should be valid position in the string to start to search.
     *                          On output, it contains the next position of the next word.
     * \param endAt [out]       On output, it contains the position where the next word finishes.
     **/
    void findWord(NEString::CharPos& IN OUT startAt, NEString::CharPos& OUT endAt) const;

    /**
     * \brief   Searches the word in the string. On output, the 'startAt' and 'endAt' contain
     *          start and end of position where each word start.
     *
     * \param   word [out]          On output contains the word that it could find.
     * \param   startAt [in, out]   On input, this should be valid position in the string to start to search.
     *                              On output, it contains the next position of the next word.
     * \param   endAt [out]         On output, it contains the position where the next word finishes.
     * 
     * \return  Returns the next word that it would. It is same object as 'word'.
     **/
    TEString<CharType>& getWord(TEString<CharType>& OUT word, NEString::CharPos& IN OUT startAt, NEString::CharPos& OUT endAt) const;

    /**
     * \brief   Find the first occurrence of any of the characters in string buffer 'chars'. 
     *          The search starts at given 'startPos'. If any character is found, returns valid position.
     *          Otherwise, returns NEString::INVALID_POS.
     * \param   chars       The one or more characters as set in null-terminated string to search.
     * \param   startPos    The start position in string to search. Should be valid.
     *                      By default, starts to search at begin of string, i.e. at 'NEString::START_POS'
     * \return  Returns valid string position value, if any occurrence of given characters found.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findOneOf( const CharType * chars, NEString::CharPos startPos = NEString::START_POS ) const;

    /**
     * \brief   Find the first occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string,
     *                          i.e. at 'NEString::START_POS'
     * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findFirst( CharType chSearch, NEString::CharPos startPos = NEString::START_POS, bool caseSensitive = true ) const;

    /**
     * \brief   Find the first occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string,
     *                          i.e. at 'NEString::START_POS'
     * \param   caseSensitive   If true, the search of phrase should be match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \param   wholeWord       If true, the search of phrase should match the whole word.
     *                          Otherwise, the match is by any match. By default, the search is by any phrase.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findFirst( const CharType * phrase, NEString::CharPos startPos = NEString::START_POS, bool caseSensitive = true, bool wholeWord = false ) const;
    
    /**
     * \brief   Find the last occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the end of string,
     *                          i.e. at 'NEString::END_POS'
     * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findLast( CharType chSearch, NEString::CharPos startPos = NEString::END_POS, bool caseSensitive = true ) const;

    /**
     * \brief   Find the last occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the end of string,
     *                          i.e. at 'NEString::END_POS'
     * \param   caseSensitive   If true, the search of phrase should be match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findLast( const CharType * phrase, NEString::CharPos startPos = NEString::END_POS, bool caseSensitive = true ) const;

    /**
     * \brief   Compares with the given string at given position. The comparing is either complete string or
     *          given number of characters. Comparing either by exact match (case-sensitive) or ignore case.
     *          The function returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     * \param   strOther        The given other string to compare
     * \param   startAt         The position of string to start comparing. 
     *                          By default, it compares at the begin of string.
     * \param   count       The number of characters to compare.
     *                          By default, it compares until end of string
     * \param   caseSensitive   If true, compares by exact match, i.e. case-sensitive.
     *                          Otherwise, it ignores upper and lower cases.
     * \return  Returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     **/
    inline NEMath::eCompare compare( const CharType * strOther, NEString::CharPos startAt = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL, bool caseSensitive = true) const;
    
    /**
     * \brief   Compares with the given string at given position. The comparing is either complete string or
     *          given number of characters. Comparing either by exact match (case-sensitive) or ignore case.
     *          The function returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     * \param   other           The string to compare with.
     * \param   startAt         The positions in the existing string to start to compare.
     * \param   count           The number of characters in the 
     * \param   caseSensitive   If true, compares by exact match, i.e. case-sensitive.
     *                          Otherwise, it ignores upper and lower cases.
     * \return  Returns:
     *              NEMath::Smaller if the existing string is smaller than the given string.
     *              NEMath::Equal   if the existing and given strings are equal.
     *              NEMath::Bigger  if the existing string is bugger than the given string.
     **/
    inline NEMath::eCompare compare(const TEString<CharType> & other, bool caseSensitive = true) const;
    inline NEMath::eCompare compare(const std::basic_string<CharType>& other, bool caseSensitive = true) const;
    inline NEMath::eCompare compare(const std::basic_string_view<CharType>& other, bool caseSensitive = true) const;
    inline NEMath::eCompare compare(const TEString<CharType>& other, NEString::CharPos startAt = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL, bool caseSensitive = true) const;
    inline NEMath::eCompare compare(const std::basic_string<CharType>& other, NEString::CharPos startAt = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL, bool caseSensitive = true) const;

    /**
     * \brief   Extracts the substring starting at startPos and given number of characters.
     * \param   startPos    Starting position of the substring to create.
     *                      By default, the substring starts at the begin, i.e. NEString::START_POS
     * \param   count       The number of characters in substring, starting at given starting position.
     *                      By default, it gets characters until end of string.
     * \return  Returns reference to the string as a result.
     **/
    TEString<CharType>& substring(NEString::CharPos startPos, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Extracts the substring starting at startPos and given number of characters.
     * \param   outResult   On output, this parameter contains resulting substring.
     * \param   startPos    Starting position of the substring to create.
     *                      By default, the substring starts at the begin, i.e. NEString::START_POS
     * \param   count   The number of characters in substring, starting at given starting position.
     *                      By default, it gets characters until end of string.
     * \return  Returns true if could create substring. Otherwise, returns false.
     *          The substring can fail if method is called for invalid string, if given position is invalid.
     **/
    void substring( TEString<CharType> & outResult, NEString::CharPos startPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL ) const;

    /**
     * \brief   Searches given phrase in the string starting from given position until the end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after phrase. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   strPhrase   The phrase to search in the string.
     * \param   startPos    The starting position to search the string.
     * \return  Returns next position after searched phrase and value are followings:
     *              -   Valid string position not equal to NEString::END_POS,
     *                  if found phrase and the phrase is not at the end;
     *              -   NEString::END_POS if found the phrase at end of string;
     *              -   NEString::INVALID_POS if could not find the phrase.
     *
     * \example     The example of Substr()
     * \code [cpp]
     *
     *  String test("0123 456 789 0123");
     *  String result;
     *  NEString::CharPos next = NEString::START_POS;
     *  next = test.substr(result, "0123", next);   // results: next == 4, result == ""
     *  next = test.substr(result, "0123", next);   // results: next == NEString::END_POS, result == " 456 789 "
     *
     *  next = NEString::START_POS;
     *  next = test.Substr(result, " ", next);      // results: next == 5, result == "0123"
     *  next = test.Substr(result, " ", next);      // results: next == 9, result == "456"
     *  next = test.Substr(result, " ", next);      // results: next == NEString::INVALID_POS, result == "0123"
     * \endcode
     **/
    NEString::CharPos substring(TEString<CharType>& outResult, const CharType* strPhrase, NEString::CharPos startPos = NEString::START_POS) const;
    NEString::CharPos substring(TEString<CharType>& outResult, const TEString<CharType>& strPhrase, NEString::CharPos startPos = NEString::START_POS) const;
    NEString::CharPos substring(TEString<CharType>& outResult, const std::basic_string<CharType>& strPhrase, NEString::CharPos startPos = NEString::START_POS) const;

    /**
     * \brief   Searches given symbol in the string starting from given position until end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after symbol. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   chSymbol    The symbol to search in the string.
     * \param   startPos    The starting position to search the symbol.
     * \return  Returns next position after searched symbol and value are followings:
     *              -   Valid string position not equal to NEString::END_POS,
     *                  if found phrase and the symbol is not at the end;
     *              -   NEString::END_POS if found the symbol at end of string;
     *              -   NEString::INVALID_POS if could not find the phrase.
     **/
    NEString::CharPos substring( TEString<CharType> & outResult, CharType chSymbol, NEString::CharPos startPos = NEString::START_POS ) const;

    /**
     * \brief   Returns left side (begin) substring of length 'charCount'
     * \param   charCount   The number of characters to extract.
     * \return  Returns the left side substring of length 'charCount' or empty string is string is empty.
     **/
    inline TEString<CharType> leftSide(NEString::CharCount charCount) const;

    /**
     * \brief   Returns right side (end) substring of length 'charCount'
     * \param   charCount   The number of characters to extract.
     * \return  Returns the right side substring of length 'charCount' or empty string is string is empty.
     **/
    inline TEString<CharType> rightSide(NEString::CharCount charCount) const;

    /**
     * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
     *          If string has not enough space to copy characters, it will reallocate the space.
     * \param   source  The source of string to copy characters.
     * \param   pos     The position in source string to start to copy.
     * \param   count   The number of characters to copy. By default, it copies all characters.
     **/
    void assign(const CharType * source, NEString::CharCount count = NEString::COUNT_ALL );
    void assign(const std::basic_string<CharType> & source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    void assign(const std::basic_string_view<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    void assign(const TEString<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends given amount of characters of given string at the end of the string
     *          and returns the amount of appended characters. If string has not enough space
     *          to append characters, it will reallocate the space.
     * \param   source  The source of string to append characters.
     * \param   pos     The position in source string to start to copy.
     * \param   count   The number of characters to append. By default, it copies all characters.
     * \return  Returns number of append characters. If zero, nothing was added.
     **/
    inline void append(const CharType * source, NEString::CharCount count = NEString::COUNT_ALL);
    inline void append(const std::basic_string<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline void append(const std::basic_string_view<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    inline void append(const TEString<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Moves string inside string buffer starting at specified position.
     *          It moves left if 'moveTo' value is negative and moves right if 'moveTo' is positive.
     *          If 'moveTo' is zero, no change happens. The method is reallocating new space if
     *          string requires more space to move right.
     * \param   startPos    The position to start moving string. The specified position should be valid for reading.
     * \param   moveTo      Specifies number of characters and direction to move string.
     *                      If negative, moves string to left side. String buffer is not reallocated.
     *                      If positive, moves string to right. And if needed, reallocates string buffer.
     *                      if zero, no change happens.
     **/
    void move(NEString::CharPos startPos, int moveTo);

    /**
     * \brief   Swaps characters in string buffer starting at given position and given number of characters.
     *          By default, it swaps all characters in the string buffer.
     * \param   startPos    The starting position in string buffer to swap.
     *                      By default, it starts at the begin of string.
     * \param   count   The amount of characters to swap.
     *                      By default, swaps all characters until end of string.
     **/
    void swap(NEString::CharPos startPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Inserts given source of character at given position in the string. 
     *          The character can be inserted at any position, including
     *          begin of string or at the end of string (append). The caller should make sure
     *          that the specified position is valid and not more end-of-string.
     *          If string has not enough space to insert character, it will reallocate the space.
     * \param   source  The source of character to insert.
     * \param   atPos   The position in the string to insert.
     * \param   count   The number of characters to insert. By default, it is one character.
     **/
    void insertAt( CharType source, NEString::CharPos atPos, NEString::CharCount count = 1 );

    /**
     * \brief   Inserts given source of string at given position in the string. 
     *          The character can be inserted at any position, including
     *          begin of string or at the end of string (append). The caller should make sure
     *          that the specified position is valid and not more end-of-string.
     *          If string has not enough space to insert character, it will reallocate the space.
     * \param   source      The source of character to insert.
     * \param   atDstPos    The position in the destination string to insert.
     * \param   atSrcPos    The position in source string, By default, it is the beginning of source string.
     * \param   count       The number of characters in the source string to insert.
     *                      By default it inserts the complete source of string.
     **/
    void insertAt( const CharType * source, NEString::CharPos atDstPos, NEString::CharCount count = NEString::COUNT_ALL );
    void insertAt(const std::basic_string<CharType>& source, NEString::CharPos atDstPos, NEString::CharPos atSrcPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);
    void insertAt(const TEString<CharType>& source, NEString::CharPos atDstPos, NEString::CharPos atSrcPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Searches and replaces given character by another given. The search and replacement starts at given position.
     *          The method either replaces first match of character and returns next position,
     *          or replaces all matches of given characters and returns end-of-string position (NEString::END_POS).
     * \param   chSearch    The character to be searched to replace.
     * \param   chReplace   The character to set when found 'chSearch'.
     * \param   startPos    The position to start searching character to replace.
     *                      By default, it searches at the begin of string.
     * \param   replaceAll  If true, replaces all matches in the string and returns End Position (NEString::END_POS).
     *                      Otherwise, replace first match.
     * \return  Returns:
     *          - valid next position value if found 'chSearch' character and replaced, where 'replaceAll' parameter is false.
     *          - NEString::END_POS if replaced character was at the end of the string or could replace at least once, where 'replaceAll' is true.
     *          - NEString::INVALID_POS if could not find and replace any 'chSearch' character.
     **/
    NEString::CharPos replace( CharType chSearch, CharType chReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true );

    /**
     * \brief   Searches and replaces given string by another string. The search and replacement starts at given position.
     *          The method either replaces the first match of the string and returns next position,
     *          or replaces all matches of given string and returns end-of-string position (NEString::END_POS).
     * \param   strSearch   The string to be searched to replace.
     * \param   strReplace  The string to set when found.
     * \param   startPos    The position to start searching string to replace.
     *                      By default, it searches at the begin of string.
     * \param   count       The amount of characters in the 'strReplace' to use to replace 'strSearch'.
     * \param   replaceAll  If true, replaces all matches of the string and returns End Position (NEString::END_POS).
     *                      Otherwise, replace only first matched.
     * \return  Returns:
     *          - valid position value if found 'strSearch' string and replaced, where 'replaceAll' parameter is false.
     *          - NEString::END_POS if replaced searching string was at the end of data or could replace at least once, where 'replaceAll' is true.
     *          - NEString::INVALID_POS if could not find and replace any match of the searching string.
     **/
    NEString::CharPos replace( const CharType * strSearch, const CharType * strReplace, NEString::CharPos startPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL, bool replaceAll = true );
    NEString::CharPos replace(const std::basic_string<CharType>& strSearch, const std::basic_string<CharType>& strReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true);
    NEString::CharPos replace(const TEString<CharType>& strSearch, const TEString<CharType>& strReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true);

    /**
     * \brief   At the given position of the string removes specified amount of characters, which are replaced 
     *          by the given string. The function returns the next position after replacing string.
     * \param   startPos    The position to start setting string.
     * \param   charsRemove Amount of characters to replace in string.
     * \param   strReplace  The string to set.
     * \param   count       The number of characters in the given string to use when replace .
     * \return  Returns next position after replacing string. If specified position in invalid
     *          returns NEString::INVALID_POS. If string was inserted at the end, returns NEString::END_POS.
     **/
    NEString::CharPos replace(NEString::CharPos startPos, NEString::CharCount charsRemove, const CharType* strReplace, NEString::CharCount count = NEString::COUNT_ALL);
    NEString::CharPos replace(NEString::CharPos startPos, NEString::CharCount charsRemove, const std::basic_string<CharType>& strReplace);
    NEString::CharPos replace(NEString::CharPos startPos, NEString::CharCount charsRemove, const TEString<CharType>& strReplace);

    /**
     * \brief   Removes specified amount of characters in string at specified starting position.
     *          If 'count' is NEString::COUNT_ALL, it will remove all until end, i.e. sets
     *          end-of-string character at specified position.
     * \param   startPos    The starting position to remove string. Operation is ignored if not valid position.
     * \param   count   The amount of characters to remove. If 'NEString::COUNT_ALL' removes all until end of string,
     *                      i.e. sets end-of-string value at specified position.
     **/
    inline void remove(NEString::CharPos startPos, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Searches and removes specified character in the string starting from the given position.
     *          Searches the complete string if 'removedAll' parameter is true.
     *          Either returns the next position where the character was removed when 'removeAll' is false,
     *          or returns NEString::END_POS if removed last character of when removed at least one time
     *          and 'removeAll' flag is true.
     * \param chRemove      The character to search and remove.
     * \param startPos      Searching start from specify position.
     * \param removeAll     If true, will remove all matches. Otherwise, removes first match of the character.
     * \return  Returns:
     *          - valid next position value if found 'chRemove' character and where 'removeAll' parameter is false.
     *          - NEString::END_POS if removed character was at the end of the string or could remove at least once, where 'removeAll' is true.
     *          - NEString::INVALID_POS if could not find any match of character to remove.
     */
    NEString::CharPos remove(const CharType chRemove, NEString::CharPos startPos = NEString::START_POS, bool removeAll = true);

    /**
     * \brief   Searches and removes given phrase of string. The search starts at given position.
     *          The method either removes only first match of string and returns next position,
     *          or removes all matches of given string and returns end-of-string position (NEString::END_POS).
     * \param   strRemove   The string to search and remove.
     * \param   startPos    The position to start searching string to replace.
     *                      By default, it searches at the begin of string.
     * \param   removeAll   If true, removes all matches of string and returns End Position (NEString::END_POS).
     *                      Otherwise, removes only first matched.
     * \return  Returns:
     *          - valid next position value if found 'strRemove' string phrase and removed, where 'removeAll' parameter is false.
     *          - NEString::END_POS if removed string was at the end of data or could remove at least once, where 'removeAll' is true.
     *          - NEString::INVALID_POS if could not find any match of string.
     **/
    NEString::CharPos remove( const CharType * strRemove, NEString::CharPos startPos = NEString::START_POS, bool removeAll = true );
    NEString::CharPos remove(const std::basic_string<CharType> & strRemove, NEString::CharPos startPos = NEString::START_POS, bool removeAll = true);
    NEString::CharPos remove(const TEString<CharType>& strRemove, NEString::CharPos startPos = NEString::START_POS, bool removeAll = true);

    /**
     * \brief   Resize the string and sets new size. If the size is less than the current length, it truncates.
     *          If the size is bigger than the length, it sets new length and fills space with the specified characters.
     * \param   maxChars    The maximum length of string.
     * \param   chFill      The characters to fill if new size if longer than the current length.
     **/
    inline void resize(NEString::CharCount maxChars, CharType chFill = static_cast<CharType>('\0'));

    /**
     * \brief   If the length of string is bigger than the 'maxChars', it truncated the string.
     *          If the length of the string is smaller than the 'maxChars, nothing happens.
     *          If 'maxChars' is zero, it empties the string.
     * \param   maxChars    The maximum characters in the string.
     */
    inline void truncate(NEString::CharCount maxChars);

    /**
     * \brief   Reserves the space for the string. Unlike 'resize' this operation does not change the current length of the string.
     * \param   newCapacity     New capacity of string to set.
     */
    inline void reserve(NEString::CharCount newCapacity);

    /**
     * \brief   Returns character at specified string position. The position must be valid.
     * \param   atPos   The position to return character value.
     * \return  Returns character at specified position of string buffer.
     *          Returns NEString::EndOfString if position is invalid or
     *          equal to string length.
     **/
    inline CharType getAt( NEString::CharPos atPos ) const;

    /**
     * \brief   Sets character at specified position.
     * \param   ch      The character to set.
     * \param   atPos   The position where to set character.
     *                  If the position is valid index, it sets the character at the specified position.
     *                  If the position is NEString::END_POS, it appends a character to the end of the string.
     *                  In all other cases, the operation is ignored.
     * \return  Returns true if operation succeeded and character either is set or append.
     *          Otherwise, returns false.
     **/
    inline bool setAt( CharType ch, NEString::CharPos atPos = NEString::END_POS );

    /**
     * \brief   Removes whitespace characters from left side, i.e. from begin of string
     *          and returns the actual length of the string.
     **/
    inline TEString<CharType>& trimLeft( void );

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin of string, 
     *          i.e. removes all whitespace characters at the begin of string, and returns number of
     *          characters copied to the resulting destination string, i.e. the length of resulting string.
     *          The existing string remain unmodified.
     * \param   strResult   The destination string to copy result.
     * \return  Returns number of characters copied to the resulting string.
     **/
    inline void trimLeft( TEString<CharType> & OUT strResult ) const;
    inline void trimLeft(std::basic_string<CharType>& OUT strResult) const;

    /**
     * \brief   Removes whitespace characters from right side, i.e. from the end of string
     *          and returns the actual length of the string.
     **/
    inline TEString<CharType>& trimRight( void );

    /**
     * \brief   Copies data into given string without trailing whitespace at the end of string, 
     *          i.e. removes all whitespace characters at the end of string, and returns number of
     *          characters copied to the resulting destination string, i.e. the length of resulting string.
     *          The existing string remain unmodified.
     * \param   strResult   The destination string to copy result.
     * \return  Returns number of characters copied to the resulting string.
     **/
    inline void trimRight( TEString<CharType> & OUT strResult ) const;
    inline void trimRight(std::basic_string<CharType>& OUT strResult) const;

    /**
     * \brief   Removes whitespace characters from left and right sides, i.e. from begin and end of string
     *          and returns the actual length of the string.
     **/
    inline TEString<CharType>& trimAll( void );

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin and end of string, 
     *          i.e. removes all whitespace characters at the begin and end of string, and returns number of
     *          characters copied to the resulting destination string, i.e. the length of resulting string.
     *          The existing string remain unmodified.
     * \param   strResult   The destination string to copy result.
     * \return  Returns number of characters copied to the resulting string.
     **/
    inline void trimAll( TEString<CharType> & OUT strResult ) const;
    inline void trimAll(std::basic_string<CharType>& OUT strResult) const;

    /**
     * \brief   Reads the string starting from specified position until end of line, 
     *          copies the result into the 'strResult' and returns the next position
     *          where new line begins or returns NEString::END_POS if reached end of string.
     *          NOTE:   If after reading the line there are still empty new lines,
     *                  they are escaped by return position value.
     * \param   strResult   On output the string contains a line.
     * \param   startPos    The position to start reading the line. By default it starts from the begging of string.
     * \return  Returns next position where starts non-empty new line or NEString::END_POS if reached end of string.
     **/
    inline NEString::CharPos readLine(TEString<CharType>& OUT strResult, NEString::CharPos startPos = NEString::START_POS) const;
    inline NEString::CharPos readLine(std::basic_string<CharType>& OUT strResult, NEString::CharPos startPos = NEString::START_POS) const;

    /**
     * \brief   In existing string removes all characters, which are not alphanumeric.
     *          Returns zero if string is not valid or all characters are removed.
     * \return  Returns the length of string after making alphanumeric
     **/
    inline NEString::CharCount makeAlphanumeric( void );

/************************************************************************/
// Methods that can be overwritten in Helper class template
/************************************************************************/
protected:

    /**
     * \brief   Compares the existing string with given string and returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     *          The comparing starts from given position, compares given
     *          amount of character of until the end if 'count' is NEString::COUNT_ALL,
     *          and the comparing is either by exact match or ignores cases sensitive.
     * \param   startPos        The position in string buffer to start comparing.
     *                          If equal to NEString::START_POS, compares from beginning of string.
     * \param   strOther        The given string to compare
     * \param   count           The amount of characters to compare.
     *                          If equal to NEString::COUNT_ALL compares until end of string.
     * \param   caseSensitive   If true, compares exact match of string. Otherwise, compares lower and upper cases.
     * \return  Return:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     **/
    inline NEMath::eCompare compareString( NEString::CharPos startPos, const CharType * strOther, NEString::CharCount count = NEString::COUNT_ALL, bool caseSensitive = true ) const;

/************************************************************************/
// Protected methods, can be assessed only from derived class
/************************************************************************/

    /**
     * \brief   Searches string and if found, replace by another.
     * \param   strOrigin   The string to search.
     * \param   lenOrigin   The length of string to search or amount of characters to search.
     * \param   strReplace  The string to replace
     * \param   lenReplace  The amount of characters to replace
     * \param   startPos    The starting position to start searching.
     * \return  Returns next position after replacing string. Returns NEString::INVALID_POS if could not find specified string.
     **/
    inline NEString::CharPos replaceWith( const CharType * strOrigin
                                        , NEString::CharCount lenOrigin
                                        , const CharType * strReplace
                                        , NEString::CharCount lenReplace
                                        , NEString::CharPos startPos);

    /**
     * \brief   Replaces string in string buffer starting at specified position.
     * \param   startPos    The position in string buffer to start replacing characters. Should be valid position for reading.
     * \param   count   The amount characters to replace is string buffer.
     * \param   strReplace  The string to replace
     * \param   lenReplace  The amount of characters to replace
     * \return  Returns next position after replacing string. Returns NEString::INVALID_POS if could not find specified string.
     **/
    inline NEString::CharPos replaceWith( NEString::CharPos startPos
                                        , NEString::CharCount count
                                        , const CharType * strReplace
                                        , NEString::CharCount lenReplace);

    inline NEString::CharPos findFirstPhrase(const CharType* phrase, NEString::CharPos startPos = NEString::START_POS) const;

    inline NEString::CharPos findFirstPhraseIgnoreCase(const CharType* phrase, NEString::CharPos startPos = NEString::START_POS) const;

    inline NEString::CharPos findFirstPhraseIgnoreCase(const std::basic_string<CharType>& phrase, NEString::CharPos startPos = NEString::START_POS) const;

    inline NEString::CharPos findFirstWord(const CharType* word, bool caseSensetive, NEString::CharPos startPos = NEString::START_POS) const;

    inline NEString::CharPos findFirstWord(const std::basic_string<CharType>& word, bool caseSensetive, NEString::CharPos startPos = NEString::START_POS) const;

    inline NEString::CharPos findLastPhrase(const CharType* phrase, NEString::CharPos startPos = NEString::START_POS) const;

    inline bool isValidNameChar(const CharType checkChar, std::locale& loc) const;

//////////////////////////////////////////////////////////////////////////
// Protected member variables
//////////////////////////////////////////////////////////////////////////
protected:
    //! The string object
    std::basic_string<CharType> mData;
};


//////////////////////////////////////////////////////////////////////////
// TEString class template implementation
//////////////////////////////////////////////////////////////////////////

template<typename CharType>
inline TEString<CharType>::TEString(void)
    : mData  (&EmptyChar)
{
}

template<typename CharType>
inline TEString<CharType>::TEString(const CharType source)
    : mData  ( &source, 1 )
{
}

template<typename CharType>
inline TEString<CharType>::TEString(const CharType* source)
    : mData  ( source != nullptr ? source : &EmptyChar )
{
}

template<typename CharType>
inline TEString<CharType>::TEString(const std::basic_string<CharType>& source)
    : mData  ( source )
{
}

template<typename CharType>
inline TEString<CharType>::TEString(std::basic_string<CharType>&& source) noexcept
    : mData  ( std::move(source) )
{
}

template<typename CharType>
inline TEString<CharType>::TEString(const std::basic_string_view<CharType>& source)
    : mData  ( source.data(), source.length() )
{
}

template<typename CharType>
inline TEString<CharType>::TEString(const CharType* source, NEString::CharCount count)
    : mData  ( source != nullptr ? source : &EmptyChar, count )
{
}

template<typename CharType>
inline TEString<CharType>::TEString(const TEString<CharType>& source)
    : mData  ( source.mData )
{
}

template<typename CharType>
inline TEString<CharType>::TEString(TEString<CharType>&& source) noexcept
    : mData  ( std::move(source.mData) )
{
}

template<typename CharType>
inline TEString<CharType>::operator const CharType* (void) const
{
    return mData.c_str();
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator = (const TEString<CharType>& src)
{
    mData = src.mData;
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator = (const std::basic_string<CharType>& src)
{
    mData = src;
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator = (const std::basic_string_view<CharType>& src)
{
    mData = src.data();
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator = (const CharType* src)
{
    if (src != nullptr)
    {
        mData = src;
    }
    else
    {
        mData.clear();
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator = (const CharType chSource)
{
    mData = chSource;
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator = (std::basic_string<CharType>&& src) noexcept
{
    mData = std::move(mData);
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator = (TEString<CharType>&& src) noexcept
{
    mData = std::move(src.mData);
    return (*this);
}

template<typename CharType>
inline bool TEString<CharType>::operator == (const TEString<CharType>& other) const
{
    return (mData == other.mData);
}

template<typename CharType>
inline bool TEString<CharType>::operator == (const std::basic_string<CharType>& other) const
{
    return (mData == other);
}

template<typename CharType>
inline bool TEString<CharType>::operator == (const std::basic_string_view<CharType>& other) const
{
    return (mData == other.data());
}

template<typename CharType>
inline bool TEString<CharType>::operator == (const CharType* other) const
{
    return (mData == other);
}

template<typename CharType>
inline bool TEString<CharType>::operator != (const TEString<CharType>& other) const
{
    return (mData != other.mData);
}

template<typename CharType>
inline bool TEString<CharType>::operator != (const std::basic_string<CharType>& other) const
{
    return (mData != other);
}

template<typename CharType>
inline bool TEString<CharType>::operator != (const std::basic_string_view<CharType>& other) const
{
    return (mData != other.data());
}

template<typename CharType>
inline bool TEString<CharType>::operator != (const CharType* other) const
{
    return (mData != other);
}

template<typename CharType>
inline bool TEString<CharType>::operator > (const TEString<CharType>& other) const
{
    return (mData > other.mData);
}

template<typename CharType>
inline bool TEString<CharType>::operator > (const std::basic_string<CharType>& other) const
{
    return (mData > other);
}

template<typename CharType>
inline bool TEString<CharType>::operator > (const std::basic_string_view<CharType>& other) const
{
    return (mData > other.data());
}

template<typename CharType>
inline bool TEString<CharType>::operator > (const CharType* other) const
{
    return (mData > other);
}

template<typename CharType>
inline bool TEString<CharType>::operator < (const TEString<CharType>& other) const
{
    return (mData < other.mData);
}

template<typename CharType>
inline bool TEString<CharType>::operator < (const std::basic_string<CharType>& other) const
{
    return (mData < other);
}

template<typename CharType>
inline bool TEString<CharType>::operator < (const std::basic_string_view<CharType>& other) const
{
    return (mData < other.data());
}

template<typename CharType>
inline bool TEString<CharType>::operator < (const CharType* other) const
{
    return (mData < other);
}

template<typename CharType>
inline bool TEString<CharType>::operator >= (const TEString<CharType>& other) const
{
    return (mData >= other.mData);
}

template<typename CharType>
inline bool TEString<CharType>::operator >= (const std::basic_string<CharType>& other) const
{
    return (mData >= other);
}

template<typename CharType>
inline bool TEString<CharType>::operator >= (const std::basic_string_view<CharType>& other) const
{
    return (mData >= other.data());
}

template<typename CharType>
inline bool TEString<CharType>::operator >= (const CharType* other) const
{
    return (mData >= other);
}

template<typename CharType>
inline bool TEString<CharType>::operator <= (const TEString<CharType>& other) const
{
    return (mData <= other.mData);
}

template<typename CharType>
inline bool TEString<CharType>::operator <= (const std::basic_string<CharType>& other) const
{
    return (mData <= other);
}

template<typename CharType>
inline bool TEString<CharType>::operator <= (const std::basic_string_view<CharType>& other) const
{
    return (mData <= other.data());
}

template<typename CharType>
inline bool TEString<CharType>::operator <= (const CharType* other) const
{
    return (mData <= other);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator += (const TEString<CharType>& src)
{
    mData += src.mData;
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator += (const std::basic_string<CharType>& src)
{
    mData += src;
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator += (const std::basic_string_view<CharType>& src)
{
    mData += src.data();
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator += (const CharType* src)
{
    mData += src;
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator += (const CharType chSource)
{
    mData += chSource;
    return (*this);
}

template<typename CharType>
inline TEString<CharType> operator + (const TEString<CharType>& lhs, const TEString<CharType>& rhs)
{
    TEString<CharType> result(lhs);
    return (result += rhs);
}

template<typename CharType>
inline TEString<CharType> operator + (const TEString<CharType>& lhs, const std::basic_string<CharType>& rhs)
{
    TEString<CharType> result(lhs);
    return (result += rhs);
}

template<typename CharType>
inline TEString<CharType> operator + (const TEString<CharType>& lhs, const std::basic_string_view<CharType>& rhs)
{
    TEString<CharType> result(lhs);
    return (result += rhs);
}

template<typename CharType>
inline TEString<CharType> operator + (const TEString<CharType>& lhs, const CharType* rhs)
{
    TEString<CharType> result(lhs);
    return (result += rhs);
}

template<typename CharType>
inline TEString<CharType> operator + (const TEString<CharType>& lhs, const CharType rhs)
{
    TEString<CharType> result(lhs);
    return (result += rhs);
}

template<typename CharType>
inline TEString<CharType> operator + (const std::basic_string<CharType>& lhs, const TEString<CharType>& rhs)
{
    TEString<CharType> result(lhs);
    return (result += rhs);
}

template<typename CharType>
inline TEString<CharType> operator + (const std::basic_string_view<CharType>& lhs, const TEString<CharType>& rhs)
{
    TEString<CharType> result(lhs);
    return (result += rhs);
}

template<typename CharType>
inline TEString<CharType> operator + (const CharType* lhs, const TEString<CharType>& rhs)
{
    TEString<CharType> result(lhs);
    return (result += rhs);
}

template<typename CharType>
inline TEString<CharType> operator + (const CharType lhs, const TEString<CharType>& rhs)
{
    TEString<CharType> result(lhs);
    return (result += rhs);
}

template<typename CharType>
inline const IEInStream& operator >> (const IEInStream& stream, TEString<CharType>& input)
{
    return (stream >> input.mData);
}

template<typename CharType>
inline IEOutStream& operator << (IEOutStream& stream, const TEString<CharType>& output)
{
    return (stream << output.mData);
}

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

template<typename CharType>
inline bool TEString<CharType>::validate(const CharType* validityList) const
{
    bool result = false;
    if ((mData.empty() == false) && (NEString::isEmpty<CharType>(validityList) == false))
    {
        result = true;
        for (const CharType* src = mData.c_str(); *src != EmptyChar; ++src)
        {
            if (NEString::findFirst<CharType>(*src, validityList) == NEString::INVALID_POS)
            {
                result = false;
                break;
            }
        }
    }

    return result;
}

template<typename CharType>
inline bool TEString<CharType>::isNumeric(bool signIgnore /*= true*/) const
{
    bool result = false;
    const CharType* src = mData.c_str();

    // escape if first symbol is a sign
    if (signIgnore && ((*src == '-') || (*src == '+')))
        ++src; // Escape sign, if it should.

    // make sure the string is not empty
    if (*src != EmptyChar)
    {
        while (isNumeric(*src))
            ++src;

        result = *src == EmptyChar;
    }

    return result;
}

template<typename CharType>
inline bool TEString<CharType>::isAlphanumeric(void) const
{
    std::locale loc(NEString::DEFAULT_LOCALE);
    const CharType* src = mData.c_str();
    while (std::isalnum<CharType>(*src, loc))
        ++src;

    return (*src == EmptyChar); // reached end of the string.
}

template<typename CharType>
inline bool TEString<CharType>::isValidNameChar(const CharType checkChar) const
{
    // initialize list of symbols for the valid names.
    constexpr CharType symbols[] = { '_', '\0'};
    return std::isalnum(checkChar, std::locale(NEString::DEFAULT_LOCALE)) || NEString::isOneOf<CharType>(checkChar, symbols);
}

template<typename CharType>
inline bool TEString<CharType>::isValidName(void) const
{
    if (mData.empty())
        return false;

    const CharType* src = mData.c_str();
    while (isValidNameChar(*src))
        ++src;

    return (*src == EmptyChar);
}

template<typename CharType>
inline bool TEString<CharType>::isEmpty(void) const
{
    return mData.empty();
}

template<typename CharType>
inline bool TEString<CharType>::isString(void) const
{
    return (mData.empty() == false);
}

template<typename CharType>
inline NEString::CharCount TEString<CharType>::getLength(void) const
{
    return static_cast<NEString::CharCount>(mData.length());
}

template<typename CharType>
inline NEString::CharCount TEString<CharType>::getSize(void) const
{
    return static_cast<NEString::CharCount>(mData.max_size());
}

template<typename CharType>
inline uint32_t TEString<CharType>::getSpace(void) const
{
    return static_cast<uint32_t>((mData.length() + 1) * sizeof(CharType));
}

template<typename CharType>
inline const CharType* TEString<CharType>::getBuffer(NEString::CharPos startAt /*= NEString::START_POS*/) const
{
    NEString::CharCount len = static_cast<NEString::CharCount>(mData.size());
    startAt = (startAt == NEString::END_POS) || (startAt > len) ? len : startAt;
    return (mData.c_str() + startAt);
}

template<typename CharType>
inline CharType* TEString<CharType>::getBuffer(NEString::CharPos startAt /*= NEString::START_POS*/)
{
    NEString::CharCount len = static_cast<NEString::CharCount>(mData.size());
    startAt = (startAt == NEString::END_POS) || (startAt > len) ? len : startAt;
    return (mData.data() + startAt);
}

template<typename CharType>
inline const CharType* TEString<CharType>::getString(void) const
{
    return mData.c_str();
}

template<typename CharType>
inline const std::basic_string<CharType>& TEString<CharType>::getObject(void) const
{
    return mData;
}

template<typename CharType>
inline bool TEString<CharType>::isValidPosition(NEString::CharPos pos) const
{
    return (pos >= NEString::START_POS) && (pos < static_cast<NEString::CharPos>(mData.length()));
}

template<typename CharType>
inline bool TEString<CharType>::isInvalidPosition(NEString::CharPos pos) const
{
    return (pos < NEString::START_POS) || (pos > static_cast<NEString::CharPos>(mData.length()));
}

template<typename CharType>
inline bool TEString<CharType>::isEndPosition(NEString::CharPos pos) const
{
    return (pos == NEString::END_POS) || (pos >= static_cast<NEString::CharPos>(mData.length()));
}

template<typename CharType>
inline bool TEString<CharType>::isStartPosition(NEString::CharPos pos) const
{
    return (mData.empty() == false) && (pos == NEString::START_POS);
}

template<typename CharType>
inline void TEString<CharType>::clear(void)
{
    mData.clear();
}

template<typename CharType>
inline void TEString<CharType>::freeExtra(void)
{
    mData.shrink_to_fit();
}

template<typename CharType>
inline void TEString<CharType>::release(void)
{
    mData.clear();
    mData.shrink_to_fit();
}

template<typename CharType>
void TEString<CharType>::findWord(NEString::CharPos& IN OUT startAt, NEString::CharPos& OUT endAt) const
{
    NEString::CharPos pos = startAt;
    endAt   = NEString::INVALID_POS;
    startAt = NEString::END_POS;

    if (isInvalidPosition(pos) || mData.empty())
        return; // invalid position, return nothing

    const CharType* buffer = getBuffer(startAt);
    while ((*buffer != EmptyChar) && isValidNameChar(*buffer) == false)
        ++buffer;   // escape invalid chars at the begin

    if (*buffer == EmptyChar)
        return; // reached EOS, do nothing.

    // Remember the position of starting valid word.
    startAt = static_cast<NEString::CharPos>(getString() - buffer);

    while (isValidNameChar(*buffer))
        ++buffer;   // escape invalid chars at the begin

    // the word ends at position.
    endAt = *buffer != EmptyChar ? static_cast<NEString::CharPos>(getString() - buffer) : NEString::END_POS;
}

template<typename CharType>
TEString<CharType>& TEString<CharType>::getWord(TEString<CharType>& OUT word, NEString::CharPos& IN OUT startAt, NEString::CharPos& OUT endAt) const
{
    findWord(startAt, endAt);
    if (startAt != NEString::END_POS)
    {
        ASSERT(endAt != NEString::INVALID_POS);
        substring(word, startAt, endAt == NEString::END_POS ? std::basic_string<CharType>::npos : endAt - startAt);
    }
    else
    {
        word.clear();
    }

    return word;
}

template<typename CharType>
NEString::CharPos TEString<CharType>::findOneOf( const CharType* chars, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    if (isInvalidPosition(startPos) || NEString::isEmpty<CharType>(chars))
        return NEString::INVALID_POS;

    const CharType* strBegin = getBuffer(startPos);
    while (*strBegin != EmptyChar)
    {
        if (NEString::isOneOf(*strBegin, chars))
        {
            return static_cast<NEString::CharPos>(strBegin - getString());
        }

        ++strBegin;
    }

    return NEString::END_POS;
}

template<typename CharType>
NEString::CharPos TEString<CharType>::findFirst( CharType chSearch
                                                 , NEString::CharPos startPos /*= NEString::START_POS*/
                                                 , bool caseSensitive /*= true*/) const
{
    if (isValidPosition(startPos) == false)
        return NEString::INVALID_POS;

    std::locale loc(NEString::DEFAULT_LOCALE);
    const CharType* str = getBuffer(startPos);
    CharType chUpper = caseSensitive ? chSearch : std::toupper(chSearch, loc);
    CharType chLower = caseSensitive ? chSearch : std::tolower(chSearch, loc);

    while ((*str != EmptyChar) && (*str != chUpper) && (*str != chLower))
    {
        ++str;
    }

    return (*str != EmptyChar ? static_cast<NEString::CharPos>(str - getString()) : NEString::END_POS);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::findFirst( const CharType* phrase
                                               , NEString::CharPos startPos /*= NEString::START_POS*/
                                               , bool caseSensitive   /*= true*/
                                               , bool wholeWord       /*= false*/) const
{
    if (isInvalidPosition(startPos) || NEString::isEmpty<CharType>(phrase))
        return NEString::INVALID_POS;

    if (caseSensitive && !wholeWord)
    {
        return findFirstPhrase(phrase, startPos);
    }
    else if (!wholeWord)
    {
        return findFirstPhraseIgnoreCase(phrase, startPos);
    }
    else
    {
        return findFirstWord(phrase, caseSensitive, startPos);
    }
}

template<typename CharType>
NEString::CharPos TEString<CharType>::findLast(CharType chSearch, NEString::CharPos startPos /*= NEString::END_POS*/, bool caseSensitive /*= true*/) const
{
    if (mData.empty())
        return NEString::INVALID_POS;

    startPos = startPos == NEString::END_POS ? getLength() - 1 : startPos;

    const CharType* begin = getString();
    const CharType* end = getBuffer(startPos);
    std::locale loc(NEString::DEFAULT_LOCALE);
    CharType chUpper = caseSensitive ? chSearch : std::toupper(chSearch, loc);
    CharType chLower = caseSensitive ? chSearch : std::tolower(chSearch, loc);

    while ((end >= begin) && (*end != chUpper) && (*end != chLower))
    {
        --end;
    }

    return (end >= begin ? static_cast<NEString::CharPos>(end - begin) : NEString::END_POS);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::findLast(const CharType* phrase, NEString::CharPos startPos /*= NEString::END_POS*/, bool caseSensitive /*= true*/) const
{
    if (isInvalidPosition(startPos) || NEString::isEmpty<CharType>(phrase))
        return NEString::INVALID_POS;

    if (caseSensitive == false)
        return findFirstPhrase(phrase, startPos);

    NEString::CharPos result{ NEString::END_POS };
    NEString::CharCount count = NEString::getStringLength<CharType>(phrase);
    NEString::CharCount strLen = getLength();

    startPos = (startPos == NEString::END_POS) && (strLen != 0) ? strLen - 1 : 0;

    const CharType* begin = getBuffer(NEString::START_POS);
    const CharType* end = getBuffer(startPos);

    for (NEString::CharPos pos = startPos; end >= begin; --end, --pos)
    {
        if ((compareString(pos, phrase, count, caseSensitive) == NEMath::eCompare::Equal))
        {
            result = pos;
            break;
        }
    }

    return result;
}

template<typename CharType>
inline NEMath::eCompare TEString<CharType>::compare(const CharType* what, NEString::CharPos startAt /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/, bool caseSensitive /*= true*/) const
{
    NEMath::eCompare result{ NEMath::eCompare::Smaller };
    if (isValidPosition(startAt) == false)
        return result;

    NEString::CharPos length = static_cast<NEString::CharPos>(mData.length()) - startAt;
    count = count == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(what) : count;
    if (length == count)
    {
        const CharType* current = getBuffer(startAt);
        const CharType* other = what;

        std::locale loc;
        result = NEMath::eCompare::Equal;

        CharType ch1{ EmptyChar };
        CharType ch2{ EmptyChar };
        do
        {
            ch1 = *current++;
            ch2 = *other++;
            if (caseSensitive == false)
            {
                ch1 = std::tolower(ch1, loc);
                ch2 = std::tolower(ch2, loc);
            }

            if (ch1 != ch2)
            {
                break;
            }

        } while (ch1 != EmptyChar);


        if (ch1 < ch2)
            result = NEMath::eCompare::Smaller;
        else if (ch1 > ch2)
            result = NEMath::eCompare::Bigger;
    }
    else if (length > count)
    {
        result = NEMath::eCompare::Bigger;
    }

    return result;
}

template<typename CharType>
inline NEMath::eCompare TEString<CharType>::compare(const TEString<CharType>& other, bool caseSensitive /*= true*/) const
{
    return compare(other.mData, caseSensitive);
}

template<typename CharType>
inline NEMath::eCompare TEString<CharType>::compare(const std::basic_string<CharType>& other, bool caseSensitive /*= true*/) const
{
    return compare(other.c_str(), NEString::START_POS, static_cast<NEString::CharCount>(other.length()), caseSensitive);
}

template<typename CharType>
inline NEMath::eCompare TEString<CharType>::compare(const std::basic_string_view<CharType>& other, bool caseSensitive /*= true*/) const
{
    return compare(other.data(), NEString::START_POS, static_cast<NEString::CharCount>(other.length()), caseSensitive);
}

template<typename CharType>
inline NEMath::eCompare TEString<CharType>::compare( const TEString<CharType>& other
                                                   , NEString::CharPos startPos /*= NEString::START_POS*/
                                                   , NEString::CharCount count  /*= NEString::COUNT_ALL*/
                                                   , bool caseSensitive         /*= true*/) const
{
    return compare(other.mData, startPos, count, caseSensitive);
}

template<typename CharType>
inline NEMath::eCompare TEString<CharType>::compare( const std::basic_string<CharType>& other
                                                   , NEString::CharPos startAt  /*= NEString::START_POS*/
                                                   , NEString::CharCount count  /*= NEString::COUNT_ALL*/
                                                   , bool caseSensitive         /*= true*/) const
{
    return compare(other.c_str(), startAt, count, caseSensitive);
}

template<typename CharType>
TEString<CharType>& TEString<CharType>::substring(NEString::CharPos startPos, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(startPos))
    {
        mData = mData.substr(startPos, count == NEString::COUNT_ALL ? std::basic_string<CharType>::npos : count);
    }
    else
    {
        mData.clear();
    }

    return (*this);
}

template<typename CharType>
inline void TEString<CharType>::substring(TEString<CharType>& outResult, NEString::CharPos startPos /* = NEString::START_POS */, NEString::CharCount count /*= NEString::COUNT_ALL*/) const
{
    if (isValidPosition(startPos))
    {
        outResult.mData = mData.substr(startPos, count == NEString::COUNT_ALL ? std::basic_string<CharType>::npos : count);
    }
}

template<typename CharType>
NEString::CharPos TEString<CharType>::substring(TEString<CharType>& outResult, const CharType* strPhrase, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    return substring(outResult, std::basic_string<CharType>(strPhrase == nullptr ? &EmptyChar : strPhrase), startPos);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::substring(TEString<CharType>& outResult, const TEString<CharType>& strPhrase, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    return substring(outResult, strPhrase.mData, startPos);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::substring(TEString<CharType>& outResult, const std::basic_string<CharType>& strPhrase, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    if (isInvalidPosition(startPos) || strPhrase.empty())
        return NEString::INVALID_POS;

    uint32_t len = static_cast<uint32_t>(strPhrase.length());
    uint32_t pos = static_cast<uint32_t>(mData.find(strPhrase, static_cast<uint32_t>(startPos)));
    outResult.mData = mData.substr(startPos, static_cast<uint32_t>(pos - startPos));

    return (pos == std::basic_string<CharType>::npos ? NEString::END_POS : pos + len);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::substring(TEString<CharType>& outResult, CharType chSymbol, NEString::CharPos startPos /* = NEString::START_POS */) const
{
    NEString::CharPos result = NEString::INVALID_POS;
    const CharType* buffer = getBuffer(startPos);
    const CharType* next = nullptr;

    NEString::CharPos pos = NEString::findFirst<CharType>(chSymbol, buffer, NEString::START_POS, &next);
    NEString::CharPos nextPos = next != nullptr ? startPos + static_cast<NEString::CharPos>(next - buffer) : NEString::INVALID_POS;
    outResult.assign(buffer, isValidPosition(pos) ? pos : NEString::COUNT_ALL);

    if (nextPos < getLength())
        result = nextPos;
    else if (nextPos == getLength())
        result = NEString::END_POS;
    return result;
}

template<typename CharType>
inline TEString<CharType> TEString<CharType>::leftSide(NEString::CharCount charCount) const
{
    TEString<CharType> result;
    TEString<CharType>::substring(result, NEString::START_POS, charCount);
    return result;
}

template<typename CharType>
inline TEString<CharType> TEString<CharType>::rightSide(NEString::CharCount charCount) const
{
    TEString<CharType> result;

    NEString::CharCount len = getLength();
    NEString::CharPos pos = charCount < len ? len - charCount : NEString::START_POS;
    substring(result, pos, NEString::COUNT_ALL);

    return result;
}

template<typename CharType>
void TEString<CharType>::assign(const CharType* source, NEString::CharCount count /*= NEString::COUNT_ALL */)
{
    count = count == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(source) : count;
    mData.resize(count);
    if (mData.empty() == false)
    {
        memcpy_s(mData.data(), mData.size() * sizeof(CharType), source, count * sizeof(CharType));
    }
}

template<typename CharType>
void TEString<CharType>::assign(const std::basic_string<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(pos))
    {
        if ((count == NEString::COUNT_ALL) || (count >= static_cast<NEString::CharPos>(source.length() - pos)))
        {
            assign(source.c_str() + pos, static_cast<NEString::CharCount>(source.length() - pos));
        }
        else
        {
            assign(source.c_str() + pos, count);
        }
    }
}

template<typename CharType>
void TEString<CharType>::assign(const std::basic_string_view<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(pos))
    {
        if ((count == NEString::COUNT_ALL) || (count >= static_cast<NEString::CharPos>(source.length() - pos)))
        {
            assign(source.data() + pos, static_cast<NEString::CharCount>(source.length() - pos));
        }
        else
        {
            assign(source.data() + pos, count);
        }
    }
}

template<typename CharType>
inline void TEString<CharType>::assign(const TEString<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    assign(source.mData, pos, count);
}

template<typename CharType>
inline void TEString<CharType>::append(const CharType* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    count = count == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(source) : count;
    uint32_t length = static_cast<uint32_t>(mData.length());
    mData.resize(length + count);

    if ((mData.empty() == false) && (length < static_cast<uint32_t>(mData.length())))
    {
        memcpy_s(mData.data() + length, count, source, count);
    }
}

template<typename CharType>
inline void TEString<CharType>::append( const std::basic_string<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(pos))
    {
        if ((count == NEString::COUNT_ALL) || (count >= static_cast<NEString::CharPos>(source.length() - pos)))
        {
            append(source.c_str() + pos, static_cast<NEString::CharCount>(source.length() - pos));
        }
        else
        {
            append(source.c_str() + pos, count);
        }
    }
}

template<typename CharType>
inline void TEString<CharType>::append(const std::basic_string_view<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(pos))
    {
        if ((count == NEString::COUNT_ALL) || (count >= static_cast<NEString::CharPos>(source.length() - pos)))
        {
            append(source.data() + pos, static_cast<NEString::CharCount>(source.length() - pos));
        }
        else
        {
            append(source.data() + pos, count);
        }
    }
}

template<typename CharType>
inline void TEString<CharType>::append( const TEString<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    append(source.mData, pos, count);
}

template<typename CharType>
void TEString<CharType>::move(NEString::CharPos startPos, int moveTo)
{
    if (moveTo < 0)
    {
        NEString::CharCount count = static_cast<NEString::CharCount>(-1 * moveTo);
        NEString::CharPos dstPos  = startPos >= count ? startPos + moveTo : NEString::START_POS;
        mData.erase(dstPos, count);
    }
    else if (moveTo > 0)
    {
        uint32_t length = static_cast<uint32_t>(mData.length());
        mData.insert(startPos, moveTo, '@');
    }
}

template<typename CharType>
void TEString<CharType>::swap(NEString::CharPos startPos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(startPos))
    {
        NEString::CharCount len = getLength();
        ASSERT(len > 0);
        ASSERT(len > startPos);
        NEString::CharCount remain = len - startPos - 1;
        count = MACRO_MIN(count, remain);
        CharType* begin = getBuffer(startPos);
        CharType* end   = getBuffer(startPos + count);
        for (; begin < end; ++begin, --end)
        {
            CharType ch = *begin;
            *begin = *end;
            *end = ch;
        }
    }
}

template<typename CharType>
void TEString<CharType>::insertAt(CharType source, NEString::CharPos atPos, NEString::CharCount count /*= 1*/)
{
    move(atPos, 1);
    if (isValidPosition(atPos))
        mData[atPos] = source;

    if (count > 0)
    {
        uint32_t length = static_cast<uint32_t>(mData.length());
        if (isValidPosition(atPos) || (atPos == static_cast<NEString::CharPos>(length)))
        {
            mData.insert(atPos, count, source);
        }
    }
}

template<typename CharType>
void TEString<CharType>::insertAt(const CharType* source, NEString::CharPos atPos, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (source != nullptr)
    {
        count = count == NEString::COUNT_ALL ? NEString::getStringLength(source) : count;
        if (isValidPosition(atPos))
        {
            mData.insert(atPos, source, count);
        }
    }
}

template<typename CharType>
void TEString<CharType>::insertAt( const std::basic_string<CharType>& source
                                         , NEString::CharPos atDstPos
                                         , NEString::CharPos atSrcPos /*= NEString::START_POS*/
                                         , NEString::CharCount count  /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(atDstPos) && (atSrcPos < static_cast<NEString::CharPos>(source.length())))
    {
        count = count == NEString::COUNT_ALL ? static_cast<NEString::CharPos>(source.length() - atSrcPos) : count;
        mData.insert(atDstPos, source, atSrcPos, count);
    }
}

template<typename CharType>
void TEString<CharType>::insertAt( const TEString<CharType>& source
                                 , NEString::CharPos atDstPos
                                 , NEString::CharPos atSrcPos /*= NEString::START_POS*/
                                 , NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    insertAt(source.mData, atDstPos, atSrcPos, count);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::replace(CharType chSearch, CharType chReplace, NEString::CharPos startPos /*= NEString::START_POS*/, bool replaceAll /*= true*/)
{
    NEString::CharPos result = NEString::INVALID_POS;
    if (isValidPosition(startPos))
    {
        CharType* begin = mData.data();
        CharType* dst   = begin + startPos;
        while (*dst != EmptyChar)
        {
            if (*dst == chSearch)
            {
                *dst ++ = chReplace;
                result = static_cast<NEString::CharPos>(dst - begin);
                if (replaceAll == false)
                {
                    break;
                }
            }
        }

        if ((result != NEString::INVALID_POS) && (*dst == EmptyChar))
        {
            result = NEString::END_POS;
        }
    }

    return result;
}

template<typename CharType>
NEString::CharPos TEString<CharType>::replace( const CharType* strSearch
                                                     , const CharType* strReplace
                                                     , NEString::CharPos startPos   /*= NEString::START_POS*/
                                                     , NEString::CharCount count    /*= NEString::COUNT_ALL*/
                                                     , bool replaceAll              /*= true*/)
{
    NEString::CharPos result = NEString::INVALID_POS;
    if (isValidPosition(startPos) && (NEString::isEmpty(strSearch) == false))
    {
        NEString::CharPos lenSearch  = NEString::getStringLength<CharType>(strSearch);
        NEString::CharPos lenReplace = NEString::getStringLength<CharType>(strReplace);
        count       = (count == NEString::COUNT_ALL) || (count > static_cast<NEString::CharCount>(lenReplace)) ? lenReplace : count;
        strReplace  = strReplace != nullptr ? strReplace : &EmptyChar;
        uint32_t pos = static_cast<uint32_t>(mData.find(strSearch, startPos));
        while (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
        {
            mData.replace(pos, static_cast<uint32_t>(lenSearch), strReplace, static_cast<uint32_t>(count));

            pos += static_cast<uint32_t>(count);
            result = static_cast<NEString::CharPos>(pos);
            if (replaceAll == false)
            {
                break;
            }

            pos = static_cast<uint32_t>(mData.find(strSearch, pos));
        }

        if ((result != NEString::INVALID_POS) && (pos == std::basic_string<CharType>::npos))
        {
            result = NEString::END_POS;
        }
    }

    return result;
}

template<typename CharType>
NEString::CharPos TEString<CharType>::replace( const TEString<CharType>& strSearch
                                                     , const TEString<CharType>& strReplace
                                                     , NEString::CharPos startPos /*= NEString::START_POS*/
                                                     , bool replaceAll /*= true*/)
{
    return replace(strSearch.mData, strReplace.mData, startPos, replaceAll);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::replace( const std::basic_string<CharType>& strSearch
                                             , const std::basic_string<CharType>& strReplace
                                             , NEString::CharPos startPos /*= NEString::START_POS*/
                                             , bool replaceAll /*= true*/)
{
    NEString::CharPos result = NEString::INVALID_POS;
    if (isValidPosition(startPos) && (strSearch.empty() == false))
    {
        NEString::CharPos lenSearch = static_cast<NEString::CharPos>(strSearch.length());
        NEString::CharPos lenReplace = static_cast<NEString::CharPos>(strReplace.length());
        uint32_t pos = static_cast<uint32_t>(mData.find(strSearch, startPos));
        while (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
        {
            mData.replace(pos, static_cast<uint32_t>(lenSearch), strReplace);

            pos += lenReplace;
            result = static_cast<NEString::CharPos>(pos);
            if (replaceAll == false)
            {
                break;
            }

            pos = static_cast<uint32_t>(mData.find(strSearch, pos));
        }

        if ((result != NEString::INVALID_POS) && (pos == std::basic_string<CharType>::npos))
        {
            result = NEString::END_POS;
        }
    }
    
    return result;
}

template<typename CharType>
NEString::CharPos TEString<CharType>::replace( NEString::CharPos startPos
                                                     , NEString::CharCount charsRemove
                                                     , const CharType* strReplace
                                                     , NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    NEString::CharPos result = NEString::INVALID_POS;
    if (isValidPosition(startPos))
    {
        NEString::CharPos lenReplace = NEString::getStringLength<CharType>(strReplace);
        count = (count == NEString::COUNT_ALL) || (count > lenReplace) ? lenReplace : count;
        mData.replace(static_cast<uint32_t>(startPos), charsRemove, strReplace, count);

        startPos += count;
    }

    return result;
}

template<typename CharType>
NEString::CharPos TEString<CharType>::replace( NEString::CharPos startPos
                                                     , NEString::CharCount charsRemove
                                                     , const TEString<CharType>& strReplace)
{
    return replace(startPos, charsRemove, strReplace.mData);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::replace( NEString::CharPos startPos
                                                     , NEString::CharCount charsRemove
                                                     , const std::basic_string<CharType>& strReplace)
{
    NEString::CharPos result = NEString::INVALID_POS;
    if (isValidPosition(startPos))
    {
        mData.replace(static_cast<uint32_t>(startPos), charsRemove, strReplace);
        startPos += static_cast<NEString::CharCount>(strReplace.length());
    }

    return result;
}

template<typename CharType>
inline void TEString<CharType>::remove(NEString::CharPos startPos, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(startPos))
    {
        if (count == NEString::COUNT_ALL)
        {
            mData.erase(static_cast<uint32_t>(startPos));
        }
        else if (count <= (static_cast<NEString::CharCount>(mData.length()) - startPos))
        {
            mData.erase(static_cast<uint32_t>(startPos), static_cast<uint32_t>(count));
        }
    }
}

template<typename CharType>
NEString::CharPos TEString<CharType>::remove(const CharType chRemove, NEString::CharPos startPos /*= NEString::START_POS*/, bool removeAll /*= true*/)
{
    NEString::CharPos result = NEString::INVALID_POS;
    if (isValidPosition(startPos))
    {
        uint32_t pos = static_cast<uint32_t>(mData.find(chRemove, static_cast<uint32_t>(startPos)));
        for ( ; pos != static_cast<uint32_t>(std::basic_string<CharType>::npos); pos = static_cast<uint32_t>(mData.find(chRemove, static_cast<uint32_t>(pos))))
        {
            mData.erase(pos, 1);
            result = static_cast<NEString::CharPos>(pos);
            if (removeAll == false)
            {
                break;
            }
        }

        if (pos == static_cast<uint32_t>(std::basic_string<CharType>::npos))
        {
            result = NEString::END_POS;
        }
    }

    return result;
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::remove( const CharType* strRemove
                                                           , NEString::CharPos startPos /*= NEString::START_POS*/
                                                           , bool removeAll             /*= true*/)
{
    return (NEString::isEmpty<CharType>(strRemove) == false ? remove(std::basic_string<CharType>(strRemove), startPos, removeAll) : NEString::INVALID_POS);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::remove( const std::basic_string<CharType>& strRemove
                                                    , NEString::CharPos startPos /*= NEString::START_POS*/
                                                    , bool removeAll             /*= true*/)
{
    NEString::CharPos result = NEString::INVALID_POS;
    if (isValidPosition(startPos))
    {
        uint32_t len = static_cast<uint32_t>(strRemove.length());
        uint32_t pos = static_cast<uint32_t>(mData.find(strRemove, static_cast<uint32_t>(startPos)));
        for (; pos != static_cast<uint32_t>(std::basic_string<CharType>::npos); pos = static_cast<uint32_t>(mData.find(strRemove, pos)))
        {
            mData.erase(pos, len);
            result = static_cast<NEString::CharPos>(pos);
            if (removeAll == false)
            {
                break;
            }
        }

        if (pos == static_cast<uint32_t>(std::basic_string<CharType>::npos))
        {
            result = NEString::END_POS;
        }
    }

    return result;
}

template<typename CharType>
NEString::CharPos TEString<CharType>::remove( const TEString<CharType>& strRemove
                                                    , NEString::CharPos startPos /*= NEString::START_POS*/
                                                    , bool removeAll             /*= true*/)
{
    return remove(strRemove.mData, startPos, removeAll);
}

template<typename CharType>
inline void TEString<CharType>::resize(NEString::CharCount maxChars, CharType chFill /*= static_cast<CharType>('\0')*/)
{
    mData.resize(maxChars, chFill);
}

template<typename CharType>
inline void TEString<CharType>::truncate(NEString::CharCount maxChars)
{
    if (maxChars == 0)
    {
        mData.clear();
    }
    else if (maxChars < static_cast<NEString::CharCount>(mData.length()))
    {
        mData.erase(maxChars);
    }
}

template<typename CharType>
inline void TEString<CharType>::reserve(NEString::CharCount newCapacity)
{
    mData.reserve( static_cast<uint32_t>(newCapacity));
}

template<typename CharType>
inline CharType TEString<CharType>::getAt(NEString::CharPos atPos) const
{
    return (isValidPosition(atPos) ? mData.at(static_cast<uint32_t>(atPos)) : static_cast<CharType>(NEString::EndOfString));
}

template<typename CharType>
inline bool TEString<CharType>::setAt(CharType ch, NEString::CharPos atPos /*= NEString::END_POS*/)
{
    bool result = false;
    if (isValidPosition(atPos))
    {
        mData.at(static_cast<uint32_t>(atPos)) = ch;
        result = true;
    }
    else if (atPos == NEString::END_POS)
    {
        mData.append(1, ch);
        result = true;
    }

    return result;
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::trimLeft(void)
{
    if (mData.empty() == false)
    {
        uint32_t count = 0;
        std::locale loc(NEString::DEFAULT_LOCALE);
        for (const auto& ch : mData)
        {
            if (std::isspace<CharType>(ch, loc) == false)
                break;

            ++count;
        }

        if (count != 0)
        {
            mData.erase(0, count);
        }
    }

    return (*this);
}

template<typename CharType>
inline void TEString<CharType>::trimLeft(TEString<CharType>& OUT strResult) const
{
    trimLeft(strResult.mData);
}

template<typename CharType>
inline void TEString<CharType>::trimLeft(std::basic_string<CharType>& OUT strResult) const
{
    strResult.clear();
    if (mData.empty() == false)
    {
        uint32_t count = 0;
        std::locale loc(NEString::DEFAULT_LOCALE);
        for (const auto& ch : mData)
        {
            if (std::isspace<CharType>(ch, loc) == false)
                break;

            ++count;
        }

        if (count != 0)
        {
            strResult.assign(getBuffer(count), static_cast<NEString::CharCount>(strResult.length() - count));
        }
    }
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::trimRight(void)
{
    if (mData.empty() == false)
    {
        uint32_t count = 0;
        std::locale loc(NEString::DEFAULT_LOCALE);
        for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
        {
            if (std::isspace<CharType>(*cit, loc) == false)
                break;

            ++count;
        }

        if (count != 0)
        {
            mData.erase(mData.length() - count, std::basic_string<CharType>::npos);
        }
    }

    return (*this);
}

template<typename CharType>
inline void TEString<CharType>::trimRight(TEString<CharType>& OUT strResult) const
{
    trimRight(strResult.mData);
}

template<typename CharType>
inline void TEString<CharType>::trimRight(std::basic_string<CharType>& OUT strResult) const
{
    strResult.clear();
    if (mData.empty() == false)
    {
        uint32_t count = 0;
        std::locale loc(NEString::DEFAULT_LOCALE);
        for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
        {
            if (std::isspace<CharType>(*cit, loc) == false)
                break;

            ++count;
        }

        if (count != 0)
        {
            strResult.assign(mData, 0, mData.length() - count);
        }
    }
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::trimAll(void)
{
    if (mData.empty() == false)
    {
        uint32_t length = static_cast<uint32_t>(mData.length());
        std::locale loc(NEString::DEFAULT_LOCALE);

        uint32_t left = 0;
        for (auto cit = mData.cbegin(); cit != mData.cend(); ++cit)
        {
            if (std::isspace<CharType>(*cit, loc) == false)
                break;

            ++left;
        }

        uint32_t right = 0;
        for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
        {
            if (std::isspace<CharType>(*cit, loc) == false)
                break;

            ++right;
        }

        if ((left + right) < length)
        {
            mData = mData.substr(left, length - (left + right));
        }
        else
        {
            mData.clear();
        }
    }

    return (*this);
}

template<typename CharType>
inline void TEString<CharType>::trimAll(TEString<CharType>& OUT strResult) const
{
    trimAll(strResult.mData);
}

template<typename CharType>
inline void TEString<CharType>::trimAll(std::basic_string<CharType>& OUT strResult) const
{
    strResult.clear();
    if (mData.empty() == false)
    {
        uint32_t length = static_cast<uint32_t>(mData.length());
        std::locale loc(NEString::DEFAULT_LOCALE);

        uint32_t left = 0;
        for (auto cit = mData.cbegin(); cit != mData.cend(); ++cit)
        {
            if (std::isspace<CharType>(*cit, loc) == false)
                break;

            ++left;
        }

        uint32_t right = 0;
        for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
        {
            if (std::isspace<CharType>(*cit, loc) == false)
                break;

            ++right;
        }

        if ((left + right) < length)
        {
            strResult = mData.substr(left, length - right);
        }
    }
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::readLine(TEString<CharType>& OUT strResult, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    return readLine(strResult.mData, startPos);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::readLine(std::basic_string<CharType>& OUT strResult, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    NEString::CharPos result = NEString::END_POS;
    if (isValidPosition(startPos))
    {
        const CharType* begin = getBuffer(startPos);
        while (NEString::isEndOfLine(*begin) && (*begin != EmptyChar))
        {
            // escape end of line symbols at the begin.
            ++begin;
        }

        if (*begin != EmptyChar)
        {
            const CharType* str = begin;
            while ((NEString::isEndOfLine(*str) == false) && (*str != EmptyChar))
            {
                // move until reach end of line
                ++str;
            }

            // copy the line
            strResult.assign(begin, static_cast<uint32_t>(str - begin));

            while (NEString::isEndOfLine(*str) && (*str != EmptyChar))
            {
                // find next line or reach end of string
                ++str;
            }

            // if reached end of string, return END_POS, otherwise, return the next position in the string where new not empty line starts.
            result = *str == EmptyChar ? NEString::END_POS : static_cast<NEString::CharPos>(str - mData.c_str());
        }
    }

    return result;
}

template<typename CharType>
inline NEString::CharCount TEString<CharType>::makeAlphanumeric(void)
{
    NEString::CharCount result = 0;
    if (mData.empty() == false)
    {
        std::locale loc(NEString::LOCALE_ASCII);
        CharType* begin = getBuffer(NEString::START_POS);
        CharType* dst = begin;
        for (const CharType* src = begin; *src != static_cast<CharType>(NEString::EndOfString); ++src)
        {
            if (std::isalnum<CharType>(*src, loc))
                *dst++ = *src;
        }
        *dst = static_cast<CharType>(NEString::EndOfString);
        mData.resize(static_cast<uint32_t>(dst - begin));
        result = static_cast<NEString::CharCount>(mData.length());
    }

    return result;
}

/// //////////////////////////////////////////////////////////////////////////
// Template class TEString Helper
//////////////////////////////////////////////////////////////////////////

template<typename CharType>
inline NEMath::eCompare TEString<CharType>::compareString( NEString::CharPos startPos
                                                         , const CharType * strOther
                                                         , NEString::CharCount count/*= NEString::COUNT_ALL */
                                                         , bool caseSensitive       /*= true                */ ) const
{
    NEMath::eCompare result = NEMath::eCompare::Smaller;
    count = count == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(strOther) : count;
    if (isValidPosition(startPos))
    {
        NEString::CharCount len = static_cast<NEString::CharCount>(getLength() - startPos);
        if (count <= len)
        {
            CharType chLeft{ '\0' };
            CharType chRight{ '\0' };

            const CharType* leftSide = getBuffer(startPos);
            const CharType* rightSide = strOther;

            if (caseSensitive)
            {
                while (count-- > 0)
                {
                    if (*leftSide != *rightSide)
                    {
                        chLeft = *leftSide;
                        chRight = *rightSide;
                        break;
                    }

                    ++leftSide;
                    ++rightSide;
                }
            }
            else
            {
                std::locale loc(NEString::DEFAULT_LOCALE);

                while (count-- > 0)
                {
                    chLeft = std::tolower(*leftSide, loc);
                    chRight = std::tolower(*rightSide, loc);
                    if (chLeft != chRight)
                    {
                        break;
                    }

                    ++leftSide;
                    ++rightSide;
                }
            }

            if (chLeft == chRight)
                result = NEMath::eCompare::Equal;
            else if (chLeft > chRight)
                result = NEMath::eCompare::Bigger;
        }
    }

    return result;
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::replaceWith( NEString::CharPos   startPos
                                                        , NEString::CharCount count
                                                        , const CharType *    strReplace
                                                        , NEString::CharCount lenReplace )
{
    NEString::CharPos nextPos = NEString::INVALID_POS;
    if ( (startPos != NEString::INVALID_POS) && (startPos != NEString::END_POS) )
    {
        int diff = static_cast<int>(lenReplace - count);
        NEString::CharPos endPos = startPos + count;
        move( endPos, diff );
        CharType * dst = getBuffer( startPos );
        while ( *strReplace != static_cast<CharType>(NEString::EndOfString) )
            *dst ++ = *strReplace ++;

        nextPos = endPos + diff;
    }

    return nextPos;
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::replaceWith( const CharType * strOrigin
                                                        , NEString::CharCount lenOrigin
                                                        , const CharType * strReplace
                                                        , NEString::CharCount lenReplace
                                                        , NEString::CharPos startPos )
{
    return replaceWith( findFirst( strOrigin, startPos, true ), lenOrigin, strReplace, lenReplace);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::findFirstPhrase(const CharType* phrase, NEString::CharPos startPos /* = NEString::START_POS */) const
{
    if (isValidPosition(startPos) && !NEString::isEmpty<CharType>(phrase))
    {
        uint32_t pos = static_cast<uint32_t>(mData.find(phrase, static_cast<uint32_t>(startPos)));
        return (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos) ? static_cast<NEString::CharPos>(pos) : NEString::END_POS);
    }
    else
    {
        return NEString::INVALID_POS;
    }
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::findFirstPhraseIgnoreCase(const CharType* phrase, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    return (phrase != nullptr ? findFirstPhraseIgnoreCase(std::basic_string<CharType>(phrase), startPos) : NEString::INVALID_POS);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::findFirstPhraseIgnoreCase(const std::basic_string<CharType>& phrase, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    if (isInvalidPosition(startPos) || phrase.empty())
        return NEString::INVALID_POS;
    else if (mData.empty())
        return NEString::END_POS;
    else if ((mData.length() - startPos) < phrase.length())
        return NEString::END_POS;

    std::locale loc(NEString::DEFAULT_LOCALE);
    auto it = std::search( mData.begin() + static_cast<uint32_t>(startPos), mData.end()
                         , phrase.begin(), phrase.end()
                         , [&](const CharType& ch1, const CharType& ch2) { return (std::tolower(ch1, loc) == std::tolower(ch2, loc)); }
                         );

    return (it != mData.end() ? static_cast<NEString::CharPos>(std::distance(mData.begin(), it)) : NEString::END_POS);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::findFirstWord(const CharType* word, bool caseSensetive, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    return (word != nullptr ? findFirstWord(std::basic_string<CharType>(word), caseSensetive, startPos) : NEString::INVALID_POS);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::findFirstWord(const std::basic_string<CharType>& word, bool caseSensetive, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    if (isInvalidPosition(startPos) || word.empty())
        return NEString::INVALID_POS;
    else if (mData.empty())
        return NEString::END_POS;
    else if ((mData.length() - startPos) < word.length())
        return NEString::END_POS;

    std::locale loc(NEString::DEFAULT_LOCALE);

    NEString::CharPos result = NEString::END_POS;
    while (result == NEString::END_POS)
    {
        auto it = std::search( mData.begin() + static_cast<uint32_t>(startPos), mData.end()
                             , word.begin(), word.end()
                             , [&](const CharType& ch1, const CharType& ch2){ return (caseSensetive ? ch1 == ch2 : std::tolower(ch1, loc) == std::tolower(ch2, loc)); }
                             );

        if (it == mData.end())
        {
            // we reached end of string, nothing found, break!
            break;
        }

        NEString::CharPos pos = static_cast<NEString::CharPos>(std::distance(mData.begin(), it));

        // Take firs char
        CharType chBegin = *(it);
        // Take previous char or empty char
        CharType chPrev  = it == mData.begin() ? EmptyChar : *(it - 1);
        // One of them must be beginning of the word.
        // Example: search "word" or " word" in sentence "This is a word";
        bool isBegin = !isValidNameChar(chBegin, loc) || !isValidNameChar(chPrev, loc);

        // Take last char
        it += word.length();
        CharType chEnd  = it != mData.end() ? *it : EmptyChar;
        // Take next char if not EOS
        CharType chNext = (it != mData.end()) && ((it + 1) != mData.end()) ? *(it + 1) : EmptyChar;
        // One of them must be end of the word.
        // Example: search "word" or "word!" in sentence "This is a word!";
        bool isEnd      = !isValidNameChar(chEnd, loc) || !isValidNameChar(chNext, loc);

        if (isBegin && isEnd)
        {
            // We found, break!
            result = pos;
            break;
        }

        ++startPos;
        if ((mData.length() - startPos) < word.length())
        {
            // if the remaining length is smaller, break, because makes no sense to search
            break;
        }
    }

    return result;
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::findLastPhrase(const CharType* phrase, NEString::CharPos startPos /* = NEString::START_POS */) const
{
    if (isValidPosition(startPos) && !NEString::isEmpty<CharType>(phrase))
    {
        uint32_t pos = static_cast<uint32_t>(mData.rfind(phrase, static_cast<uint32_t>(startPos)));
        return (pos != std::basic_string<CharType>::npos ? static_cast<NEString::CharPos>(pos) : NEString::END_POS);
    }
    else
    {
        return NEString::INVALID_POS;
    }
}

template<typename CharType>
inline bool TEString<CharType>::isValidNameChar(const CharType checkChar, std::locale& loc) const
{
    // initialize list of symbols for the valid names.
    constexpr CharType symbols[] = { '_', '\0' };
    return std::isalnum(checkChar, loc) || NEString::isOneOf<CharType>(checkChar, symbols);
}
