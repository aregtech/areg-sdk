#ifndef AREG_BASE_TESTRING_HPP
#define AREG_BASE_TESTRING_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/TEString.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, String class template based on first 256 character in UTF-8 code page.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TETemplateBase.hpp"
#include "areg/base/NEString.hpp"

#include <algorithm>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

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
    static constexpr CharType   EmptyChar       { static_cast<CharType>(NEString::EndOfString) };

    /**
     * \brief   New line constant
     **/
    static constexpr CharType   NewLine         { static_cast<CharType>(NEString::EndOfLine) };

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
     * \brief   Initializes string and reserves a space for specified amount of characters.
     *          Note, the size of string remains zero.
     *
     * \param   count   The space to reserve.
     */
    explicit inline TEString(uint32_t count);

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
     * \brief   Returns character at the given valid position. The position should not be
     *          neither negative, nor more than the string length.
     * 
     * \param index     Valid zero-based index, which value should not be more than the length of string.
     **/
    inline const CharType operator[ ](int index) const;

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
    inline bool operator == (const CharType ch ) const;
    inline bool operator != (const TEString<CharType>& other) const;
    inline bool operator != (const std::basic_string<CharType>& other) const;
    inline bool operator != (const std::basic_string_view<CharType>& other) const;
    inline bool operator != (const CharType* other) const;
    inline bool operator != (const CharType ch) const;

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
     *
     * \param   lhs     Left-hand side string object
     * \param   rhs     Right-hand side string object
     * \return  Returns newly constructed string object by concatenating 2 strings
     **/
    template<typename CT>
    friend inline TEString<CT> operator + (const TEString<CT>& lhs, const TEString<CT>& rhs);
    template<typename CT>
    friend inline TEString<CT> operator + (const TEString<CT>& lhs, const std::basic_string<CT>& rhs);
    template<typename CT>
    friend inline TEString<CT> operator + (const TEString<CT>& lhs, const std::basic_string_view<CT>& rhs);
    template<typename CT>
    friend inline TEString<CT> operator + (const TEString<CT>& lhs, const CT* rhs);
    template<typename CT>
    friend inline TEString<CT> operator + (const TEString<CT>& lhs, const CT rhs);
    template<typename CT>
    friend inline TEString<CT> operator + (const std::basic_string<CT>& lhs, const TEString<CT>& rhs);
    template<typename CT>
    friend inline TEString<CT> operator + (const std::basic_string_view<CT>& lhs, const TEString<CT>& rhs);
    template<typename CT>
    friend inline TEString<CT> operator + (const CT* lhs, const TEString<CT>& rhs);
    template<typename CT>
    friend inline TEString<CT> operator + (const CT lhs, const TEString<CT>& rhs);

    /**
     * \brief   Streams to input object, i.e. reads data from streaming object to string,
     *          and initialize string data.
     *
     * \tparam  CT      The type of character.
     * \param   stream  Streaming object to read string data
     * \param   input   String object to initialize and write string data.
     * \return  Reference to stream object.
     **/
    template<typename CT>
    friend inline const IEInStream & operator >> (const IEInStream & stream, TEString<CT> & input);

    /**
     * \brief   Streams from output object, i.e. write data from string to streaming object.
     * \tparam  CT      The type of character.
     * \param   stream  Streaming object to write data.
     * \param   output  String object to read data from
     * \return  Reference to stream object.
     **/
    template<typename CT>
    friend inline IEOutStream & operator << (IEOutStream & stream, const TEString<CT> & output);

    /**
     * \brief   Prints string message to the standard output stream.
     * \tparam  CT      The type of character.
     * \param   stream  Standard streaming object to print message.
     * \param   output  String object with the message.
     * \return  Reference to stream object.
     **/
    template<typename CT>
    friend inline std::ostream & operator << ( std::ostream & stream, const TEString<CT> & output );

    /**
     * \brief   Reads string message from the standard input stream.
     * \tparam  CT      The type of character.
     * \param   stream  Standard streaming object to read message.
     * \param   input   String object to save the message.
     * \return  Reference to stream object.
     **/
    template<typename CT>
    friend inline const std::istream & operator >> ( const std::istream & stream, TEString<CT> & input );

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
     *          The name is valid if it contains letters, numbers and following symbols {'_'}.
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
     * \brief   Returns the length of the string.
     **/
    inline NEString::CharCount getLength(void) const;

    /**
     * \brief   Returns the number of characters that can store in the string.
     **/
    inline NEString::CharCount getCapacity(void) const;

    /**
     * \brief   Return the size of string in bytes including the end of the string character.
     **/
    inline uint32_t getSpace( void ) const;

    /**
     * \brief   Returns string buffer starting at specified valid position.
     *
     * \param   startAt     The offset of position in string to return buffer.
     *                      By default it returns begin of string buffer.
     * \return  Returns valid pointer of the string buffer at specified position.
     **/
    inline const CharType* getBuffer(NEString::CharPos startAt = NEString::START_POS) const;
    inline CharType* getBuffer(NEString::CharPos startAt = NEString::START_POS);

    /**
     * \brief   Returns the buffer of string.
     **/
    inline const CharType* getString(void) const;

    /**
     * \brief   Returns the string object.
     **/
    inline const std::basic_string<CharType>& getData(void) const;

    /**
     * \brief   Returns true if specified character position is valid in the string.
     * \param   pos     Zero-based position in the string.
     * \return  Returns true if specified position is valid to read character.
     **/
    inline bool isValidPosition(NEString::CharPos pos) const;

    /**
     * \brief   Returns true if specified character position is invalid in the string.
     * \param   pos     Zero-based position in the string.
     * \return  Returns true if specified position is invalid.
     **/
    inline bool isInvalidPosition(NEString::CharPos pos) const;

    /**
     * \brief   Returns true if specified position is equal to the end of the string.
     * \param   pos     The zero-based position in the string.
     **/
    inline bool isLastPosition(NEString::CharPos pos) const;

    /**
     * \brief   Returns true if string is not empty and the specified position is zero.
     **/
    inline bool isFirstPosition(NEString::CharPos pos) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Clears the string, makes it empty and invalid.
     **/
    inline void clear(void);

    /**
     * \brief   Delete unused space in the string.
     **/
    inline void freeExtra(void);

    /**
     * \brief   Sets the size of string to zero and deletes the space.
     */
    inline void release(void);

    /**
     * \brief   Searches the whole word in the string at specified 'startAt' position.
     *          On output, the 'startAt' and 'endAt' contain start and end of position where each
     *          word start and end. The length of the word calculated by 'endAt - startAt' formula.
     *          The word is a collection of characters and digits, and allowed in the valid name
     *          characters ('_'), which are surrounded by other symbols (whitespace, comma, dot, etc),
     *          or if it starts at the beginning or end of the string.
     *
     * \param startAt [in, out] On input, this should be valid position in the string to start to search.
     *                          On output, it contains the next position of the next word.
     * \param endAt [out]       On output, it contains the position where the next word finishes.
     **/
    void findWord(NEString::CharPos& IN OUT startAt, NEString::CharPos& OUT endAt) const;

    /**
     * \brief   Searches the whole word in the string at specified 'startAt' position.
     *          On output, the 'startAt' and 'endAt' contain start and end of position where each
     *          word start and end. On output, the 'word' is the word with length 'endAt - startAt'.
     *          The word is a collection of characters and digits, and allowed in the valid name
     *          characters ('_'), which are surrounded by other symbols (whitespace, comma, dot, etc),
     *          or if it starts at the beginning or end of the string.
     *
     * \param   word [out]          On output contains the word that it could find.
     * \param   startAt [in, out]   On input, this should be valid position in the string to start to search.
     *                              On output, it contains the next position of the next word.
     * \param   endAt [out]         On output, it contains the position where the next word finishes.
     **/
    void getWord(TEString<CharType>& OUT word, NEString::CharPos& IN OUT startAt, NEString::CharPos& OUT endAt) const;

    /**
     * \brief   Find the first occurrence of any of the characters in string buffer 'chars'.
     *          The search starts at given 'startPos'. Returns valid position if found any.
     *          Otherwise, returns NEString::INVALID_POS.
     *
     * \param   chars       The one or more characters as null-terminated string to search.
     * \param   startPos    The starting position in string to search. Should be valid.
     *                      By default, starts to search at begin of string, i.e. at 'NEString::START_POS'
     * \return  Returns valid string position value, if any occurrence of given characters found.
     *          Otherwise, returns NEString::INVALID_POS value.
     **/
    NEString::CharPos findOneOf( const CharType * chars, NEString::CharPos startPos = NEString::START_POS ) const;

    /**
     * \brief   Find the first occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     *
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The starting position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string, i.e. at 'NEString::START_POS'
     * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::INVALID_POS value.
     **/
    NEString::CharPos findFirst( CharType chSearch, NEString::CharPos startPos = NEString::START_POS, bool caseSensitive = true ) const;

    /**
     * \brief   Find the first occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     *
     * \param   phrase          The phrase to search in the string.
     * \param   startPos        The starting position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string, i.e. at 'NEString::START_POS'
     * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \param   wholeWord       If true, the search of phrase should be the whole word.
     *                          Otherwise, searches by any match. By default, the search is by any match.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::INVALID_POS value.
     **/
    NEString::CharPos findFirst( const CharType * phrase, NEString::CharPos startPos = NEString::START_POS, bool caseSensitive = true, bool wholeWord = false ) const;
    NEString::CharPos findFirst( const TEString<CharType> & phrase, NEString::CharPos startPos = NEString::START_POS, bool caseSensitive = true, bool wholeWord = false ) const;

    /**
     * \brief   Find the last occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     *
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The starting position in string to search. Should be valid.
     *                          By default, starts to search at the end of string, i.e. at 'NEString::END_POS'
     * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::INVALID_POS value.
     **/
    NEString::CharPos findLast( CharType chSearch, NEString::CharPos startPos = NEString::END_POS, bool caseSensitive = true ) const;

    /**
     * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   phrase          The phrase to search in the string.
     * \param   phraseCount     The length of the phrase or the number of characters in the phrase to search.
     * \param   startPos        The starting position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string, i.e. at 'NEString::END_POS'
     * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::INVALID_POS value.
     **/
    NEString::CharPos findLast( const CharType * phrase, NEString::CharCount phraseCount, NEString::CharPos startPos, bool caseSensitive) const;

    /**
     * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   phrase          The phrase to search in the string.
     * \param   startPos        The starting position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string, i.e. at 'NEString::END_POS'
     * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::INVALID_POS value.
     **/
    NEString::CharPos findLast( const CharType* phrase, NEString::CharPos startPos = NEString::END_POS, bool caseSensitive = true) const;

    /**
     * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   phrase          The phrase to search in the string.
     * \param   startPos        The starting position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string, i.e. at 'NEString::END_POS'
     * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::INVALID_POS value.
     **/
    NEString::CharPos findLast( const TEString<CharType> & phrase, NEString::CharPos startPos = NEString::END_POS, bool caseSensitive = true ) const;

    /**
     * \brief   Compares the given string. The comparing is done by certain position, certain amount of characters
     *          or by ignoring upper and lower cases, depending parameters are passed in the function.
     *
     * \param   strOther        The given string to compare.
     * \param   startAt         If given, compares at specified zero-based position of the string.
     *                          Otherwise, it compares at the begin of string (NEString::START_POS).
     * \param   count           The number of characters to compare. By default, it compares until end of string
     * \param   caseSensitive   If true, compares by exact match, i.e. case-sensitive.
     *                          Otherwise, it ignores upper and lower cases.
     * \return  Returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger  if string is more than given string
     **/
    NEMath::eCompare compare( const CharType * strOther, NEString::CharPos startAt = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL, bool caseSensitive = true) const;
    inline NEMath::eCompare compare(const TEString<CharType> & strOther, bool caseSensitive = true) const;
    inline NEMath::eCompare compare(const std::basic_string<CharType>& strOther, bool caseSensitive = true) const;
    inline NEMath::eCompare compare(const std::basic_string_view<CharType>& strOther, bool caseSensitive = true) const;
    inline NEMath::eCompare compare(const TEString<CharType>& strOther, NEString::CharPos startAt = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL, bool caseSensitive = true) const;
    inline NEMath::eCompare compare(const std::basic_string<CharType>& strOther, NEString::CharPos startAt = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL, bool caseSensitive = true) const;

    /**
     * \brief   Truncates the starting at zero-based 'startPos' and given number of characters.
     *          If 'startPos' is beginning of the string (NEString::START_POS) and 'count' is
     *          equal to the length of the string or NEString::COUNT_ALL, the string is not modified.
     *
     * \param   startPos    Starting position of the substring to create.
     *                      By default, the substring starts at the begin, i.e. NEString::START_POS
     * \param   count       The number of characters in substring, starting at given starting position.
     *                      By default, it gets characters until end of string.
     * \return  Returns truncated string.
     **/
    inline TEString<CharType>& substring(NEString::CharPos startPos, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Extracts the substring starting at startPos and given number of characters.
     * \param   outResult   On output, this parameter contains resulting substring.
     * \param   startPos    Starting position of the substring to create.
     *                      By default, the substring starts at the begin, i.e. NEString::START_POS
     * \param   count       The number of characters in substring, starting at given starting position.
     *                      By default, it gets characters until end of string.
     **/
    inline void substring( TEString<CharType> & outResult, NEString::CharPos startPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL ) const;

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
    inline NEString::CharPos substring(TEString<CharType>& outResult, const CharType* strPhrase, NEString::CharPos startPos = NEString::START_POS) const;
    inline NEString::CharPos substring(TEString<CharType>& outResult, const TEString<CharType>& strPhrase, NEString::CharPos startPos = NEString::START_POS) const;
    inline NEString::CharPos substring(TEString<CharType>& outResult, const std::basic_string<CharType>& strPhrase, NEString::CharPos startPos = NEString::START_POS) const;

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
     * \param   count   The number of characters to copy. By default, it copies all characters.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& assign(const CharType * source, NEString::CharCount count = NEString::COUNT_ALL );

    /**
     * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
     *          If string has not enough space to copy characters, it will reallocate the space.
     * \param   source  The source of string to copy characters.
     * \param   pos     The position in source string to start to copy.
     * \param   count   The number of characters to copy. By default, it copies all characters.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& assign(const std::basic_string<CharType> & source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
     *          If string has not enough space to copy characters, it will reallocate the space.
     * \param   source  The source of string to copy characters.
     * \param   pos     The position in source string to start to copy.
     * \param   count   The number of characters to copy. By default, it copies all characters.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& assign(const std::basic_string_view<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
     *          If string has not enough space to copy characters, it will reallocate the space.
     * \param   source  The source of string to copy characters.
     * \param   pos     The position in source string to start to copy.
     * \param   count   The number of characters to copy. By default, it copies all characters.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& assign(const TEString<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
     *          If string has not enough space to copy characters, it will reallocate the space.
     * \param   ch      A character to assign.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& assign(CharType ch);

    /**
     * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
     *          and by amount of characters to append.
     * \param   source  The source of string to append characters.
     * \param   count   If specified, the number of characters to append. By default, it appends all characters.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& append(const CharType * source, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
     *          and by amount of characters to append.
     * \param   source  The source of string to append characters.
     * \param   pos     If specified the valid zero-based position in the given string to append.
     *                  Otherwise, it append starting from the beginning.
     * \param   count   If specified, the number of characters to append. By default, it appends all characters.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& append(const std::basic_string<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
     *          and by amount of characters to append.
     * \param   source  The source of string to append characters.
     * \param   pos     If specified the valid zero-based position in the given string to append.
     *                  Otherwise, it append starting from the beginning.
     * \param   count   If specified, the number of characters to append. By default, it appends all characters.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& append(const std::basic_string_view<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
     *          and by amount of characters to append.
     *
     * \param   source  The source of string to append characters.
     * \param   pos     If specified the valid zero-based position in the given string to append.
     *                  Otherwise, it append starting from the beginning.
     * \param   count   If specified, the number of characters to append. By default, it appends all characters.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& append(const TEString<CharType>& source, NEString::CharPos pos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
     *          and by amount of characters to append.
     * \param   ch      A character to append.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& append(CharType ch);

    /**
     * \brief   Moves data inside string buffer starting at specified position.
     *          It moves left if 'moveTo' value is negative and moves right if 'moveTo' is positive.
     *          If 'moveTo' is zero, no change happens.
     * \param   startPos    The valid zero-based position to start moving string.
     * \param   moveTo      Specifies number of characters and direction to move string.
     *                      If negative, moves string to left side. String buffer is not reallocated.
     *                      If positive, moves string to right.
     *                      if zero, no change happens.
     * \return  Returns modified string.
     **/
    TEString<CharType>& moveTo(NEString::CharPos startPos, int moveTo);

    /**
     * \brief   Inserts given source of character at given valid zero-based position in the string.
     *          The character can be inserted at any position, including begin of string or
     *          at the end of string (NEString::END_POS).
     * \param   source  The character to insert.
     * \param   atPos   The position in the string to insert.
     * \param   count   The number of characters to insert. By default, it is one character.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& insertAt( CharType source, NEString::CharPos atPos, NEString::CharCount count = 1 );

    /**
     * \brief   Inserts given source at given valid zero-based position in the string.
     *          The character can be inserted at any position, including begin of string or
     *          at the end of string (NEString::END_POS).
     * \param   source      The source of character to insert.
     * \param   atDstPos    The position in the destination string to insert.
     *                      By default, it is the beginning of source string.
     * \param   count       If given, the number of characters in the source string to insert.
     *                      By default it inserts the complete source of string.
     **/
    inline TEString<CharType>& insertAt( const CharType * source, NEString::CharPos atDstPos, NEString::CharCount count = NEString::COUNT_ALL );

    /**
     * \brief   Inserts given source at given valid zero-based position in the string.
     *          The character can be inserted at any position, including begin of string or
     *          at the end of string (NEString::END_POS).
     *
     * \param   source      The source of character to insert.
     * \param   atDstPos    The position in the destination string to insert.
     * \param   atSrcPos    If given, the position in source string to take for inserting.
     *                      By default, it is the beginning of source string.
     * \param   count       If given, the number of characters in the source string to insert.
     *                      By default it inserts the complete source of string.
     **/
    inline TEString<CharType>& insertAt(const std::basic_string<CharType>& source, NEString::CharPos atDstPos, NEString::CharPos atSrcPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Inserts given source at given valid zero-based position in the string.
     *          The character can be inserted at any position, including begin of string or
     *          at the end of string (NEString::END_POS).
     *
     * \param   source      The source of character to insert.
     * \param   atDstPos    The position in the destination string to insert.
     * \param   atSrcPos    If given, the position in source string to take for inserting.
     *                      By default, it is the beginning of source string.
     * \param   count       If given, the number of characters in the source string to insert.
     *                      By default it inserts the complete source of string.
     **/
    inline TEString<CharType>& insertAt(const std::basic_string_view<CharType>& source, NEString::CharPos atDstPos, NEString::CharPos atSrcPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Inserts given source at given valid zero-based position in the string.
     *          The character can be inserted at any position, including begin of string or
     *          at the end of string (NEString::END_POS).
     *
     * \param   source      The source of character to insert.
     * \param   atDstPos    The position in the destination string to insert.
     * \param   atSrcPos    If given, the position in source string to take for inserting.
     *                      By default, it is the beginning of source string.
     * \param   count       If given, the number of characters in the source string to insert.
     *                      By default it inserts the complete source of string.
     **/
    inline TEString<CharType>& insertAt(const TEString<CharType>& source, NEString::CharPos atDstPos, NEString::CharPos atSrcPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Searches and replaces given character by another character. The search and replacement starts at given position.
     *          The method either replaces only first match or all matches.
     *
     * \param   chSearch    The character to be searched to replace.
     * \param   chReplace   The character to set when found 'chSearch'.
     * \param   startPos    The valid zero-based position to start searching character to replace.
     *                      By default, it searches at the begin of string.
     * \param   replaceAll  If true, replaces all matches in the string. Otherwise, replace first match.
     * \return  Returns modified string.
     **/
    TEString<CharType>& replace( CharType chSearch, CharType chReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true );

    /**
     * \brief   Searches and replaces given string by another string. The search and replacement starts at given position.
     *          The method either replaces only first match or all matches.
     *
     * \param   strSearch   The string to be searched to replace.
     * \param   strReplace  The string to set when found 'strSearch'.
     * \param   startPos    The valid zero-based position to start searching string to replace.
     *                      By default, it searches at the begin of string.
     * \param   count       The amount of characters in the 'strReplace' to use to replace 'strSearch'.
     * \param   replaceAll  If true, replaces all matches in the string. Otherwise, replace first match.
     * \return  Returns modified string.
     **/
    TEString<CharType>& replace( const CharType * strSearch, const CharType * strReplace, NEString::CharPos startPos = NEString::START_POS, NEString::CharCount count = NEString::COUNT_ALL, bool replaceAll = true );
    TEString<CharType>& replace(const std::basic_string<CharType>& strSearch, const std::basic_string<CharType>& strReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true);
    TEString<CharType>& replace(const std::basic_string_view<CharType>& strSearch, const std::basic_string_view<CharType>& strReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true);
    inline TEString<CharType>& replace(const TEString<CharType>& strSearch, const TEString<CharType>& strReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true);

    /**
     * \brief   At the given position of the string removes specified amount of characters and replaces by the given string.position after replacing string.
     *
     * \param   startPos    The valid zero-based position to start modifying string.
     * \param   charsRemove Amount of characters to remove to replace in string.
     * \param   strReplace  The string to set.
     * \param   count       The number of characters in the given string to use to replace .
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& replace(NEString::CharPos startPos, NEString::CharCount charsRemove, const CharType* strReplace, NEString::CharCount count = NEString::COUNT_ALL);
    inline TEString<CharType>& replace(NEString::CharPos startPos, NEString::CharCount charsRemove, const std::basic_string<CharType>& strReplace);
    inline TEString<CharType>& replace(NEString::CharPos startPos, NEString::CharCount charsRemove, const TEString<CharType>& strReplace);

    /**
     * \brief   Removes specified amount of characters in string at specified starting position.
     *          If 'count' is NEString::COUNT_ALL, it will remove all characters until end of the string.
     *
     * \param   startPos    The valid zero-based starting position to remove string.
     * \param   count       The amount of characters to remove or until end of the string if NEString::COUNT_ALL.
     * \return  Returns modified string.
     **/
    TEString<CharType>& remove(NEString::CharPos startPos, NEString::CharCount count = NEString::COUNT_ALL);

    /**
     * \brief   Starting from the given valid zero-based position searches and removes specified character.
     *          If 'removedAll' parameter is true, removes all matches in the string. Otherwise, it removes only first match.
     *
     * \param   chRemove    The character to search and remove.
     * \param   startPos    The valid zero-based position to start to search character.
     * \param   removeAll   If true, will remove all matches. Otherwise, removes first match of the character.
     * \return  Returns modified string.
     **/
    TEString<CharType>& remove(const CharType chRemove, NEString::CharPos startPos = NEString::START_POS, bool removeAll = true);

    /**
     * \brief   Starting from the given valid zero-based position searches and removes given phrase of string. The search starts at given position.
     *          If 'removedAll' parameter is true, removes all matches in the string. Otherwise, it removes only first match.
     *
     * \param   strRemove   The string to search and remove.
     * \param   startPos    The valid zero-based position to start to search character.
     * \param   removeAll   If true, will remove all matches. Otherwise, removes first match of the character.
     * \return  Returns modified string.
     **/
    inline TEString<CharType>& remove( const CharType * strRemove, NEString::CharPos startPos = NEString::START_POS, bool removeAll = true );
    inline TEString<CharType>& remove(const TEString<CharType>& strRemove, NEString::CharPos startPos = NEString::START_POS, bool removeAll = true);
    TEString<CharType>& remove(const std::basic_string<CharType> & strRemove, NEString::CharPos startPos = NEString::START_POS, bool removeAll = true);

    /**
     * \brief   Resizes the string to contain count characters.
     *          If the current size is less than count, additional characters are appended.
     *          If the current size is greater than count, the string is reduced to its first count elements.
     *
     * \param   count   The maximum length of string.
     * \param   chFill  The characters to fill if new size if longer than the current length.
     **/
    inline TEString<CharType>& resize(NEString::CharCount count, CharType chFill = static_cast<CharType>('\0'));

    /**
     * \brief   If the length of string is bigger than the 'maxChars', it truncated the string.
     *          If 'maxChars' is zero, empties the string.
     *          If the length of the string is smaller than the 'maxChars', nothing happens.
     * \param   maxChars    The maximum characters in the string.
     */
    inline TEString<CharType>& truncate(NEString::CharCount maxChars);

    /**
     * \brief   Reserves the space for the string. Unlike 'resize' this operation does not change the current length of the string.
     * \param   newCapacity     New capacity of the string to set.
     */
    inline TEString<CharType>& reserve(NEString::CharCount newCapacity);

    /**
     * \brief   Returns character at specified valid zero-based position.
     *
     * \param   atPos   The position to return character value.
     * \return  Returns character at specified position of string buffer.
     *          Returns NEString::EndOfString if position is invalid or
     *          equal to string length.
     **/
    inline CharType getAt( NEString::CharPos atPos ) const;

    /**
     * \brief   Sets character at specified valid zero-based position.
     *
     * \param   ch      The character to set.
     * \param   atPos   The position where to set character.
     *                  If the position is valid, it sets the character at the specified position.
     *                  If the position is NEString::END_POS, it appends a character at the end of the string.
     *                  In all other cases, the operation is ignored.
     **/
    inline TEString<CharType>& setAt( CharType ch, NEString::CharPos atPos = NEString::END_POS );

    /**
     * \brief   Removes whitespace characters from left side, i.e. from the begin of the string
     *          and returns the actual modified string.
     **/
    inline TEString<CharType>& trimLeft( void );

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin of the string.
     *          The existing string remain unmodified.
     *
     * \param   strResult   The destination string to copy result.
     **/
    inline void trimLeft( TEString<CharType> & OUT strResult ) const;
    inline void trimLeft(std::basic_string<CharType>& OUT strResult) const;

    /**
     * \brief   Removes whitespace characters from right side, i.e. from end of the string
     *          and returns the actual modified string.
     **/
    inline TEString<CharType>& trimRight( void );

    /**
     * \brief   Copies data into given string without trailing whitespace at the end of the string.
     *          The existing string remain unmodified.
     *
     * \param   strResult   The destination string to copy result.
     **/
    inline void trimRight( TEString<CharType> & OUT strResult ) const;
    inline void trimRight(std::basic_string<CharType>& OUT strResult) const;

    /**
     * \brief   Removes whitespace characters from left and right sides, i.e. from the begin and end of the string
     *          and returns the actual modified string.
     **/
    TEString<CharType>& trimAll( void );

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin and end of the string.
     *          The existing string remain unmodified.
     *
     * \param   strResult   The destination string to copy result.
     **/
    inline void trimAll( TEString<CharType> & OUT strResult ) const;
    void trimAll(std::basic_string<CharType>& OUT strResult) const;

    /**
     * \brief   Converts the string, makes all letters to lower case and returns the instance of converted string.
     **/
    inline TEString<CharType> & makeLower( void );

    /**
     * \brief   Converts the string, makes all letters to upper case and returns the instance of converted string.
     **/
    inline TEString<CharType> & makeUpper( void );

    /**
     * \brief   Reads the string starting from specified position until end of line,
     *          copies the result into the 'strResult' and returns the next position
     *          where new line begins or returns NEString::END_POS if reached end of string.
     * \note    If after reading the line there are still empty new lines.
     *          The returned position value escapes followed empty new lines.
     * \param   strResult   On output the string contains a line.
     * \param   startPos    The valid zero-based position to start copying the line. By default it starts from the begging of string.
     * \return  Returns next position where starts non-empty new line or NEString::END_POS if reached end of string.
     **/
    inline NEString::CharPos readLine(TEString<CharType>& OUT strResult, NEString::CharPos startPos = NEString::START_POS) const;
    NEString::CharPos readLine(std::basic_string<CharType>& OUT strResult, NEString::CharPos startPos = NEString::START_POS) const;

    /**
     * \brief   In the existing string removes all characters, which are not alphanumeric.
     * \return  Returns new length of string after making alphanumeric
     **/
    inline TEString<CharType>& makeAlphanumeric( void );

    /**
     * \brief   Checks and returns true if the string starts with the given phrase.
     *          Valid only the first 256 characters based on UTF-8 code page.
     * \param phrase            The phrase to check.
     * \param isCaseSensitive   If false, ignores the upper and lower cases.
     **/
    inline bool startsWith(const TEString<CharType>& phrase, bool isCaseSensitive = true) const;

    /**
     * \brief   Checks and returns true if the string starts with the given phrase.
     *          Valid only the first 256 characters based on UTF-8 code page.
     * \param phrase            The phrase to check.
     * \param isCaseSensitive   If false, ignores the upper and lower cases.
     **/
    inline bool startsWith(const std::basic_string<CharType>& phrase, bool isCaseSensitive = true) const;

    /**
     * \brief   Checks and returns true if the string starts with the given phrase.
     *          Valid only the first 256 characters based on UTF-8 code page.
     * \param phrase            The phrase to check.
     * \param isCaseSensitive   If false, ignores the upper and lower cases.
     **/
    inline bool startsWith(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive = true) const;

    /**
     * \brief   Checks and returns true if the string starts with the given phrase.
     *          Valid only the first 256 characters based on UTF-8 code page.
     * \param phrase            The phrase to check.
     * \param isCaseSensitive   If false, ignores the upper and lower cases.
     * \param count             The number of character in the phase. If NEString::COUNT_ALL it compare the complete phase.
     **/
    inline bool startsWith(const CharType* phrase, bool isCaseSensitive = true, NEString::CharCount count = NEString::COUNT_ALL) const;

    /**
     * \brief   Checks and returns true if the string ends with the given phrase.
     *          Valid only the first 256 characters based on UTF-8 code page.
     * \param phrase            The phrase to check.
     * \param isCaseSensitive   If false, ignores the upper and lower cases.
     **/
    inline bool endsWith(const TEString<CharType>& phrase, bool isCaseSensitive = true) const;

    /**
     * \brief   Checks and returns true if the string ends with the given phrase.
     *          Valid only the first 256 characters based on UTF-8 code page.
     * \param phrase            The phrase to check.
     * \param isCaseSensitive   If false, ignores the upper and lower cases.
     **/
    inline bool endsWith(const std::basic_string<CharType>& phrase, bool isCaseSensitive = true) const;

    /**
     * \brief   Checks and returns true if the string ends with the given phrase.
     *          Valid only the first 256 characters based on UTF-8 code page.
     * \param phrase            The phrase to check.
     * \param isCaseSensitive   If false, ignores the upper and lower cases.
     **/
    inline bool endsWith(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive = true) const;

    /**
     * \brief   Checks and returns true if the string ends with the given phrase.
     *          Valid only the first 256 characters based on UTF-8 code page.
     * \param phrase            The phrase to check.
     * \param isCaseSensitive   If false, ignores the upper and lower cases.
     * \param count             The number of character in the phase. If NEString::COUNT_ALL it compare the complete phase.
     **/
    inline bool endsWith(const CharType* phrase, bool isCaseSensitive = true, NEString::CharCount count = NEString::COUNT_ALL) const;

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
     * \brief   Replaces string in the buffer starting at specified position.
     * \param   startPos    The position in string buffer to start replacing characters. Should be valid position for reading.
     * \param   count       The amount characters to replace is string buffer.
     * \param   strReplace  The string to replace
     * \param   lenReplace  The amount of characters to replace
     * \return  Returns next position after replacing string. Returns NEString::INVALID_POS if could not find specified string.
     **/
    inline NEString::CharPos replaceWith( NEString::CharPos startPos
                                        , NEString::CharCount count
                                        , const CharType * strReplace
                                        , NEString::CharCount lenReplace);

    /**
     * \brief   Splits the given string into multiple parts considering specified delimiter.
     * \param   delim   The delimiter character that should be searched to split the string.
     * \return  Returns list of strings that are split be specified delimiter.
     **/
    inline std::vector<TEString<CharType>> split(CharType delim) const;

    /**
     * \brief   Splits the given string into multiple parts considering specified delimiter.
     * \param   delim   The delimiter string that should be searched to split the string.
     * \return  Returns list of strings that are split be specified delimiter.
     **/
    inline std::vector<TEString<CharType>> split(const TEString<CharType> & delim) const;

/************************************************************************/
// Protected methods, can be assessed only from derived class
/************************************************************************/
protected:

    /**
     * \brief   Compares the existing string at the specified valid zero-based position with another string, and returns:
     *              - NEMath::Smaller   if string is less than given string
     *              - NEMath::Equal     if strings have equal
     *              - NEMath::Bigger    if string is more than given string.
     *          The comparing can be done by ignoring case sensitivity.
     *
     * \param   startPos        The Valid zero-based position in the string buffer to start comparing.
     * \param   strOther        The string to compare
     * \param   count           The amount of characters to compare or NEString::COUNT_ALL to compare the complete string.
     * \param   caseSensitive   If true, compares exact match of string. Otherwise, ignores lower and upper cases.
     * \return  Return:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger  if string is more than given string
     **/
    inline NEMath::eCompare compareString( NEString::CharPos startPos, const CharType * strOther, NEString::CharCount count = NEString::COUNT_ALL, bool caseSensitive = true ) const;

    /**
     * \brief   Compares the existing string at the specified valid zero-based position with another string, and returns:
     *              - NEMath::Smaller   if string is less than given string
     *              - NEMath::Equal     if strings have equal
     *              - NEMath::Bigger    if string is more than given string.
     *          The comparing is done by exact match.
     *
     * \param   startPos        The Valid zero-based position in the string buffer to start comparing.
     * \param   strOther        The string to compare
     * \param   count           The amount of characters to compare or NEString::COUNT_ALL to compare the complete string.
     * \return  Return:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger  if string is more than given string
     **/
    inline NEMath::eCompare compareStringExact(NEString::CharPos startPos, const CharType* strOther, NEString::CharCount count = NEString::COUNT_ALL) const;

    /**
     * \brief   Compares the existing string at the specified valid zero-based position with another string, and returns:
     *              - NEMath::Smaller   if string is less than given string
     *              - NEMath::Equal     if strings have equal
     *              - NEMath::Bigger    if string is more than given string.
     *          The comparing is done by ignoring upper / lower case.
     *
     * \param   startPos        The Valid zero-based position in the string buffer to start comparing.
     * \param   strOther        The string to compare
     * \param   count           The amount of characters to compare or NEString::COUNT_ALL to compare the complete string.
     * \return  Return:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger  if string is more than given string
     **/
    inline NEMath::eCompare compareStringIgnoreCase(NEString::CharPos startPos, const CharType * strOther, NEString::CharCount count = NEString::COUNT_ALL) const;

    /**
     * \brief   Searches the first phrase in string. The comparing is done by exact match.
     *
     * \param   phrase      The phrase to search.
     * \param   count       The number of characters in the phrase. The value NEString::COUNT_ALL searches the complete phrase.
     * \param   startPos    The valid zero-based position in the string to start searching.
     * \return  If found, returns valid position in the string. If not found, it returns NEString::END_POS.
     **/
    inline NEString::CharPos findFirstPhrase(const CharType* phrase, NEString::CharCount count = NEString::COUNT_ALL, NEString::CharPos startPos = NEString::START_POS ) const;

    /**
     * \brief   Searches the first phrase in string. The comparing is done by ignoring upper and lower cases.
     *          It compares upper and lower cases by default locale.
     *
     * \param   phrase      The phrase to search.
     * \param   startPos    The valid zero-based position in the string to start searching.
     * \return  If found, returns valid position in the string. If not found, it returns NEString::END_POS.
     **/
    inline NEString::CharPos findFirstPhraseIgnoreCase(const CharType* phrase, NEString::CharPos startPos = NEString::START_POS) const;
    inline NEString::CharPos findFirstPhraseIgnoreCase(const std::basic_string<CharType>& phrase, NEString::CharPos startPos = NEString::START_POS) const;

    /**
     * \brief   Searches the first match of the entire word in string. The comparing can be done by exact match of ignoring upper and lower cases.
     *          It compares upper and lower cases by default locale.
     *
     * \param   word            The word to search.
     * \param   caseSensitive   If true, it searches by exact match. Otherwise, ignores upper and lower cases.
     * \param   startPos        The valid zero-based position in the string to start searching.
     * \return  If found, returns valid position in the string. If not found, it returns NEString::END_POS.
     **/
    inline NEString::CharPos findFirstWord(const CharType* word, bool caseSensitive, NEString::CharPos startPos = NEString::START_POS) const;
    inline NEString::CharPos findFirstWord(const std::basic_string<CharType>& word, bool caseSensitive, NEString::CharPos startPos = NEString::START_POS) const;

    /**
     * \brief   Returns true if specified character is valid for the names. The names may contain
     *          characters, digits and symbols like '_'.
     *
     * \param   checkChar   Character to check.
     * \param   loc         The locale to use when validating.
     * \return  Returns true if specified character is valid for the names.
     */
    inline bool isValidNameChar(const CharType checkChar, std::locale& loc) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Compares whether full string starts with the given phrase.
     * 
     * \param fullString    The Full string.
     * \param phrase        The phrase to compare
     * \param count         Number of characters.
     * \param locale        Pointer to the locale string. Should be nullptr if ignore locale.
     * \return  Returns true if 'count' characters in the full string are equal to the phrase.
     **/
    inline bool _hasPhrase(const CharType* fullString, const CharType* phrase, NEString::CharCount count, const char* locale) const;

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
    : mData  ( source != nullptr ? source : &EmptyChar, static_cast<uint32_t>(count != NECommon::COUNT_ALL ? count : NEString::getStringLength(source)) )
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
inline TEString<CharType>::TEString(uint32_t count)
    : mData  ( )
{
    mData.reserve(count);
}

template<typename CharType>
inline TEString<CharType>::operator const CharType* (void) const
{
    return mData.c_str();
}

template<typename CharType>
inline const CharType TEString<CharType>::operator[ ](int index) const
{
    ASSERT((mData.c_str() != nullptr) && (index < static_cast<int>(mData.length())));
    return mData.c_str()[index];
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
    mData.assign( src.data( ) );
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator = (const CharType* src)
{
    if (src != nullptr)
    {
        mData.assign(src);
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
    mData = std::move(src);
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
inline bool TEString<CharType>::operator == ( const CharType ch) const
{
    return ((mData.length() == 1) && (mData[0] == ch));
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
inline bool TEString<CharType>::operator != ( const CharType ch ) const
{
    return ((mData.length( ) != 1) || (mData[0] != ch));
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
    if (mData.empty())
    {
        mData.assign(src.data(), src.length());
    }
    else
    {
        mData.append(src.data(), src.length());
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator += (const CharType* src)
{
    if (NEString::isEmpty(src) == false)
        mData += src;
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::operator += (const CharType chSource)
{
    mData += chSource;
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

template<typename CharType>
inline bool TEString<CharType>::validate(const CharType* validityList) const
{
    if (mData.empty() || NEString::isEmpty<CharType>(validityList))
        return false;

    const CharType* src = mData.c_str();
    while (*src != EmptyChar)
    {
        if (NEString::findFirst<CharType>(*src, validityList) == NEString::INVALID_POS)
            break;

        ++src;
    }

    return (*src == EmptyChar);
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
        std::locale loc(NEString::LOCALE_DEFAULT);
        while (std::isdigit<CharType>(*src, loc))
            ++src;

        result = *src == EmptyChar;
    }

    return result;
}

template<typename CharType>
inline bool TEString<CharType>::isAlphanumeric(void) const
{
    std::locale loc(NEString::LOCALE_DEFAULT );
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
    return std::isalnum(checkChar, std::locale(NEString::LOCALE_DEFAULT)) || NEString::isOneOf<CharType>(checkChar, symbols);
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
inline NEString::CharCount TEString<CharType>::getLength(void) const
{
    return static_cast<NEString::CharCount>(mData.length());
}

template<typename CharType>
inline NEString::CharCount TEString<CharType>::getCapacity(void) const
{
    return static_cast<NEString::CharCount>(mData.capacity());
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
inline const std::basic_string<CharType>& TEString<CharType>::getData(void) const
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
inline bool TEString<CharType>::isLastPosition(NEString::CharPos pos) const
{
    return (pos == NEString::END_POS) || (pos >= static_cast<NEString::CharPos>(mData.length()));
}

template<typename CharType>
inline bool TEString<CharType>::isFirstPosition(NEString::CharPos pos) const
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

    if (isInvalidPosition(pos) || mData.empty())
    {
        endAt = NEString::INVALID_POS;
        startAt = NEString::END_POS;
        return; // invalid position, return nothing
    }

    if (startAt != NEString::START_POS)
    {
        endAt = NEString::INVALID_POS;
        startAt = NEString::END_POS;
    }
    else
    {
        endAt = NEString::END_POS;
    }

    const CharType* buffer = getBuffer(pos);
    while ((*buffer != EmptyChar) && isValidNameChar(*buffer) == false)
        ++buffer;   // escape invalid chars at the begin

    if (*buffer == EmptyChar)
        return; // reached EOS, do nothing.

    if (startAt != NEString::START_POS)
    {
        // Remember the position of starting valid word.
        startAt = static_cast<NEString::CharPos>(getString() - buffer);
        while (isValidNameChar(*buffer))
            ++buffer;   // escape invalid chars at the begin
    }

    // the word ends at position.
    endAt = *buffer != EmptyChar ? static_cast<NEString::CharPos>(getString() - buffer) : NEString::END_POS;
}

template<typename CharType>
void TEString<CharType>::getWord(TEString<CharType>& OUT word, NEString::CharPos& IN OUT startAt, NEString::CharPos& OUT endAt) const
{
    findWord(startAt, endAt);
    if (startAt != NEString::END_POS)
    {
        ASSERT(endAt != NEString::INVALID_POS);
        substring(word, startAt, static_cast<NEString::CharCount>(endAt == NEString::END_POS ? NEString::COUNT_ALL : endAt - startAt));
    }
    else
    {
        word.clear();
    }
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

    std::locale loc(NEString::LOCALE_DEFAULT);
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
        return findFirstPhrase(phrase, NEString::COUNT_ALL, startPos);
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
NEString::CharPos TEString<CharType>::findFirst( const TEString<CharType> & phrase
                                               , NEString::CharPos startPos /*= NEString::START_POS*/
                                               , bool caseSensitive         /*= true*/
                                               , bool wholeWord             /*= false*/) const
{
    if (isInvalidPosition(startPos) || NEString::isEmpty<CharType>(phrase))
        return NEString::INVALID_POS;

    if (caseSensitive && !wholeWord)
    {
        return findFirstPhrase(phrase.getString(), phrase.getLength(), startPos);
    }
    else if (!wholeWord)
    {
        return findFirstPhraseIgnoreCase(phrase.mData, startPos);
    }
    else
    {
        return findFirstWord(phrase.mData, caseSensitive, startPos);
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
    if (caseSensitive)
    {
        while ((end >= begin) && (*end != chSearch))
        {
            --end;
        }

    }
    else
    {
        std::locale loc(NEString::LOCALE_DEFAULT);
        CharType chUpper = std::toupper(chSearch, loc);
        CharType chLower = std::tolower(chSearch, loc);

        while ((end >= begin) && (*end != chUpper) && (*end != chLower))
        {
            --end;
        }
    }

    return (end >= begin ? static_cast<NEString::CharPos>(end - begin) : NEString::END_POS);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::findLast(const CharType* phrase, NEString::CharCount phraseCount, NEString::CharPos startPos, bool caseSensitive) const
{
    if (((startPos != NEString::END_POS) && isInvalidPosition(startPos)) || NEString::isEmpty<CharType>(phrase) || (phraseCount == 0))
        return NEString::INVALID_POS;

    NEString::CharPos result{ NEString::END_POS };
    NEString::CharCount count = phraseCount > 0 ? phraseCount : NEString::getStringLength<CharType>(phrase);
    NEString::CharCount strLen = getLength();

    startPos = (startPos == NEString::END_POS) && (strLen >= count) ? strLen - 1 - count : 0;
    for (NEString::CharPos pos = startPos; pos >= 0; --pos)
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
NEString::CharPos TEString<CharType>::findLast(const CharType* phrase, NEString::CharPos startPos /*= NEString::END_POS*/, bool caseSensitive /*= true*/) const
{
    return findLast(phrase, NEString::getStringLength<CharType>(phrase), startPos, caseSensitive);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::findLast(const TEString<CharType> & phrase, NEString::CharPos startPos /*= NEString::END_POS*/, bool caseSensitive /*= true*/) const
{
    return findLast(phrase, phrase.getLength(), startPos, caseSensitive);
}

template<typename CharType>
NEMath::eCompare TEString<CharType>::compare(const CharType* what, NEString::CharPos startAt /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/, bool caseSensitive /*= true*/) const
{
    NEMath::eCompare result{ NEMath::eCompare::Smaller };
    if (isValidPosition(startAt) == false)
        return result;

    if (count == NEString::COUNT_ALL)
    {
        count = NEString::getStringLength<CharType>(what);
    }

    NEString::CharPos length = static_cast<NEString::CharPos>(mData.length()) - startAt;
    if ((length == count) && (what != nullptr))
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
inline TEString<CharType>& TEString<CharType>::substring(NEString::CharPos startPos, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    mData = mData.substr(static_cast<uint32_t>(startPos), count == NEString::COUNT_ALL ? std::basic_string<CharType>::npos : static_cast<uint32_t>(count));
    return (*this);
}

template<typename CharType>
inline void TEString<CharType>::substring(TEString<CharType>& outResult, NEString::CharPos startPos /* = NEString::START_POS */, NEString::CharCount count /*= NEString::COUNT_ALL*/) const
{
    if (isValidPosition(startPos))
    {
        outResult.mData = mData.substr(static_cast<uint32_t>(startPos), count == NEString::COUNT_ALL ? std::basic_string<CharType>::npos : static_cast<uint32_t>(count));
    }
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::substring(TEString<CharType>& outResult, const CharType* strPhrase, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    return substring(outResult, std::basic_string<CharType>(strPhrase == nullptr ? &EmptyChar : strPhrase), startPos);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::substring(TEString<CharType>& outResult, const TEString<CharType>& strPhrase, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    return substring(outResult, strPhrase.mData, startPos);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::substring(TEString<CharType>& outResult, const std::basic_string<CharType>& strPhrase, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    outResult.mData.clear();

    if (isInvalidPosition(startPos) || strPhrase.empty())
    {
        return NEString::INVALID_POS;
    }

    uint32_t len = static_cast<uint32_t>(strPhrase.length());
    uint32_t pos = static_cast<uint32_t>(mData.find(strPhrase, static_cast<uint32_t>(startPos)));

    if (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
    {
        uint32_t newCount = pos - static_cast<uint32_t>(startPos);
        outResult.mData = mData.substr(static_cast<uint32_t>(startPos), static_cast<uint32_t>(newCount));
        return static_cast<NEString::CharPos>(pos + len);
    }
    else
    {
        outResult.mData = mData.substr(static_cast<uint32_t>(startPos));
        return NEString::END_POS;
    }
}

template<typename CharType>
NEString::CharPos TEString<CharType>::substring(TEString<CharType>& outResult, CharType chSymbol, NEString::CharPos startPos /* = NEString::START_POS */) const
{
    if (isInvalidPosition(startPos))
    {
        return NEString::INVALID_POS;
    }

    uint32_t pos = static_cast<uint32_t>(mData.find(chSymbol, static_cast<uint32_t>(startPos)));
    if (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
    {
        uint32_t newCount = pos - static_cast<uint32_t>(startPos);
        outResult.mData = mData.substr(static_cast<uint32_t>(startPos), newCount);
        return static_cast<NEString::CharPos>(pos + 1);
    }
    else
    {
        outResult.mData = mData.substr(static_cast<uint32_t>(startPos));
        return NEString::END_POS;
    }
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
inline TEString<CharType>& TEString<CharType>::assign(const CharType* source, NEString::CharCount count /*= NEString::COUNT_ALL */)
{
    if ( source != nullptr )
    {
        mData.assign( source, static_cast<uint32_t>(count == NEString::COUNT_ALL ? NEString::getStringLength<CharType>( source ) : count) );
    }
    else
    {
        mData.clear( );
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::assign(const std::basic_string<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    mData.assign(source, static_cast<uint32_t>(pos), count == NEString::COUNT_ALL ? std::basic_string<CharType>::npos : static_cast<uint32_t>(count));
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::assign(const std::basic_string_view<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    assign(source.data() + pos, count == NEString::COUNT_ALL ? static_cast<NEString::CharCount>(source.length() - static_cast<uint32_t>(pos)) : count);
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::assign(const TEString<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    assign(source.mData, pos, count);
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::assign( CharType ch )
{
    mData = ch;
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::append(const CharType* source, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    count = count == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(source) : count;
    if (mData.empty())
    {
        mData.assign(source, static_cast<uint32_t>(count));
    }
    else
    {
        mData.append(source, static_cast<uint32_t>(count));
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::append( const std::basic_string<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    mData.append(source, static_cast<uint32_t>(pos), count == NEString::COUNT_ALL ? std::basic_string<CharType>::npos : static_cast<uint32_t>(count));
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::append(const std::basic_string_view<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    append(source.data() + pos, count == NEString::COUNT_ALL ? static_cast<NEString::CharCount>(source.length() - static_cast<uint32_t>(pos)) : count);
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::append( const TEString<CharType>& source, NEString::CharPos pos /*= NEString::START_POS*/, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    return append(source.mData, pos, count);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::append( CharType ch )
{
    mData += ch;
    return (*this);
}

template<typename CharType>
TEString<CharType>& TEString<CharType>::moveTo(NEString::CharPos startPos, int moveTo)
{
    if (moveTo < 0)
    {
        NEString::CharCount count = static_cast<NEString::CharCount>(-1 * moveTo);
        NEString::CharPos dstPos  = startPos >= count ? startPos + moveTo : NEString::START_POS;
        mData.erase(static_cast<uint32_t>(dstPos), static_cast<uint32_t>(count));
    }
    else if (moveTo > 0)
    {
        mData.insert(static_cast<uint32_t>(startPos), static_cast<uint32_t>(moveTo), '@');
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::insertAt(CharType source, NEString::CharPos atPos, NEString::CharCount count /*= 1*/)
{
    mData.insert(static_cast<uint32_t>(atPos), static_cast<uint32_t>(count), source);
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::insertAt(const CharType* source, NEString::CharPos atPos, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (source != nullptr && isValidPosition(atPos))
    {
        mData.insert(static_cast<uint32_t>(atPos), source, count == NEString::COUNT_ALL ? static_cast<uint32_t>(NEString::getStringLength(source)) : static_cast<uint32_t>(count));
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::insertAt( const std::basic_string<CharType>& source
                                                       , NEString::CharPos atDstPos
                                                       , NEString::CharPos atSrcPos /*= NEString::START_POS*/
                                                       , NEString::CharCount count  /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(atDstPos) && (atSrcPos < static_cast<NEString::CharPos>(source.length())))
    {
        count = count == NEString::COUNT_ALL ? static_cast<NEString::CharPos>(source.length() - static_cast<uint32_t>(atSrcPos)) : count;
        mData.insert(static_cast<uint32_t>(atDstPos), source, static_cast<uint32_t>(atSrcPos), static_cast<uint32_t>(count));
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::insertAt( const std::basic_string_view<CharType>& source
                                                       , NEString::CharPos atDstPos
                                                       , NEString::CharPos atSrcPos /*= NEString::START_POS*/
                                                       , NEString::CharCount count  /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(atDstPos) && (atSrcPos < static_cast<NEString::CharPos>(source.length())))
    {
        count = count == NEString::COUNT_ALL ? static_cast<NEString::CharPos>(source.length() - static_cast<uint32_t>(atSrcPos)) : count;
        mData.insert(static_cast<uint32_t>(atDstPos), source, static_cast<uint32_t>(atSrcPos), static_cast<uint32_t>(count));
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::insertAt( const TEString<CharType>& source
                                                       , NEString::CharPos atDstPos
                                                       , NEString::CharPos atSrcPos /*= NEString::START_POS*/
                                                       , NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    return insertAt(source.mData, atDstPos, atSrcPos, count);
}

template<typename CharType>
TEString<CharType>& TEString<CharType>::replace(CharType chSearch, CharType chReplace, NEString::CharPos startPos /*= NEString::START_POS*/, bool replaceAll /*= true*/)
{
    if (isValidPosition(startPos))
    {
        CharType* begin = mData.data();
        CharType* dst   = begin + startPos;
        while (*dst != EmptyChar)
        {
            if (*dst == chSearch)
            {
                *dst ++ = chReplace;
                if (replaceAll == false)
                {
                    break;
                }
            }
        }
    }

    return (*this);
}

template<typename CharType>
TEString<CharType>& TEString<CharType>::replace( const CharType* strSearch
                                               , const CharType* strReplace
                                               , NEString::CharPos startPos   /*= NEString::START_POS*/
                                               , NEString::CharCount count    /*= NEString::COUNT_ALL*/
                                               , bool replaceAll              /*= true*/)
{
    if (isValidPosition(startPos) && (NEString::isEmpty(strSearch) == false))
    {
        NEString::CharPos lenSearch  = NEString::getStringLength<CharType>(strSearch);
        NEString::CharPos lenReplace = NEString::getStringLength<CharType>(strReplace);
        count       = (count == NEString::COUNT_ALL) || (count > static_cast<NEString::CharCount>(lenReplace)) ? lenReplace : count;
        strReplace  = strReplace != nullptr ? strReplace : &EmptyChar;
        uint32_t pos = static_cast<uint32_t>(mData.find(strSearch, static_cast<uint32_t>(startPos)));
        while (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
        {
            if ( count != 0 )
            {
                mData.replace( pos, static_cast<uint32_t>(lenSearch), strReplace, static_cast<uint32_t>(count) );
                pos += static_cast<uint32_t>(count);
            }
            else
            {
                mData.erase( pos, static_cast<uint32_t>(lenSearch) );
            }

            if ( (replaceAll == false) || (pos >= static_cast<uint32_t>(mData.length())) )
            {
                break;
            }

            pos = static_cast<uint32_t>(mData.find(strSearch, pos));
        }
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::replace( const std::basic_string_view<CharType>& strSearch
                                                      , const std::basic_string_view<CharType>& strReplace
                                                      , NEString::CharPos startPos  /*= NEString::START_POS*/
                                                      , bool replaceAll             /*= true*/)
{
    if (isValidPosition(startPos) && (strSearch.empty() == false))
    {
        NEString::CharPos lenSearch  = static_cast<NEString::CharPos>(strSearch.length());
        NEString::CharPos lenReplace = static_cast<NEString::CharPos>(strReplace.length());
        uint32_t pos = static_cast<uint32_t>(mData.find(strSearch.data(), static_cast<uint32_t>(startPos)));
        while (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
        {
            if ( lenReplace != 0 )
            {
                mData.replace( pos, static_cast<uint32_t>(lenSearch), strReplace.data( ), static_cast<uint32_t>(lenReplace) );
                pos += static_cast<uint32_t>(lenReplace);
            }
            else
            {
                mData.erase( pos, static_cast<uint32_t>(lenSearch) );
            }

            if ((replaceAll == false) || (pos >= static_cast<uint32_t>(mData.length( ))) )
            {
                break;
            }

            pos = static_cast<uint32_t>(mData.find(strSearch.data(), pos));
        }
    }

    return (*this);
}


template<typename CharType>
inline TEString<CharType>& TEString<CharType>::replace( const TEString<CharType>& strSearch
                                                       , const TEString<CharType>& strReplace
                                                       , NEString::CharPos startPos /*= NEString::START_POS*/
                                                       , bool replaceAll /*= true*/)
{
    return replace(strSearch.mData, strReplace.mData, startPos, replaceAll);
}

template<typename CharType>
TEString<CharType>& TEString<CharType>::replace( const std::basic_string<CharType>& strSearch
                                               , const std::basic_string<CharType>& strReplace
                                               , NEString::CharPos startPos /*= NEString::START_POS*/
                                               , bool replaceAll /*= true*/)
{
    if (isValidPosition(startPos) && (strSearch.empty() == false))
    {
        NEString::CharPos lenSearch = static_cast<NEString::CharPos>(strSearch.length());
        NEString::CharPos lenReplace = static_cast<NEString::CharPos>(strReplace.length());
        uint32_t pos = static_cast<uint32_t>(mData.find(strSearch, static_cast<uint32_t>(startPos)));
        while (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
        {
            if ( lenReplace != 0 )
            {
                mData.replace( pos, static_cast<uint32_t>(lenSearch), strReplace );
                pos += static_cast<uint32_t>(lenReplace);
            }
            else
            {
                mData.erase( pos, static_cast<uint32_t>(lenSearch) );
            }

            if ( (replaceAll == false) || (pos >= static_cast<uint32_t>(mData.length( ))) )
            {
                break;
            }

            pos = static_cast<uint32_t>(mData.find(strSearch, pos));
        }
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::replace( NEString::CharPos startPos, NEString::CharCount charsRemove, const CharType* strReplace, NEString::CharCount count /*= NEString::COUNT_ALL*/)
{
    if (isValidPosition(startPos))
    {
        NEString::CharPos lenReplace = NEString::getStringLength<CharType>(strReplace);
        mData.replace(static_cast<uint32_t>(startPos), static_cast<uint32_t>(charsRemove), strReplace, (count == NEString::COUNT_ALL) || (count > lenReplace) ? static_cast<uint32_t>(lenReplace) : static_cast<uint32_t>(count));
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::replace( NEString::CharPos startPos, NEString::CharCount charsRemove, const TEString<CharType>& strReplace)
{
    return replace(startPos, charsRemove, strReplace.mData);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::replace( NEString::CharPos startPos, NEString::CharCount charsRemove, const std::basic_string<CharType>& strReplace)
{
    if (isValidPosition(startPos))
    {
        mData.replace(static_cast<uint32_t>(startPos), static_cast<uint32_t>(charsRemove), strReplace);
    }

    return (*this);
}

template<typename CharType>
TEString<CharType>& TEString<CharType>::remove(NEString::CharPos startPos, NEString::CharCount count /*= NEString::COUNT_ALL*/)
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

    return (*this);
}

template<typename CharType>
TEString<CharType>& TEString<CharType>::remove(const CharType chRemove, NEString::CharPos startPos /*= NEString::START_POS*/, bool removeAll /*= true*/)
{
    if (isValidPosition(startPos))
    {
        uint32_t pos = static_cast<uint32_t>(mData.find(chRemove, static_cast<uint32_t>(startPos)));
        for ( ; pos != static_cast<uint32_t>(std::basic_string<CharType>::npos); pos = static_cast<uint32_t>(mData.find(chRemove, static_cast<uint32_t>(pos))))
        {
            mData.erase(pos, 1);
            if ( (removeAll == false) || (pos >= static_cast<uint32_t>(mData.length( ))) )
            {
                break;
            }
        }
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::remove( const CharType* strRemove, NEString::CharPos startPos /*= NEString::START_POS*/, bool removeAll /*= true*/)
{
    if (NEString::isEmpty<CharType>(strRemove) == false)
        remove(std::basic_string<CharType>(strRemove), startPos, removeAll);

    return (*this);
}

template<typename CharType>
TEString<CharType>& TEString<CharType>::remove( const std::basic_string<CharType>& strRemove, NEString::CharPos startPos /*= NEString::START_POS*/, bool removeAll /*= true*/)
{
    if (isValidPosition(startPos))
    {
        uint32_t len = static_cast<uint32_t>(strRemove.length());
        uint32_t pos = static_cast<uint32_t>(mData.find(strRemove, static_cast<uint32_t>(startPos)));
        for (; pos != static_cast<uint32_t>(std::basic_string<CharType>::npos); pos = static_cast<uint32_t>(mData.find(strRemove, pos)))
        {
            mData.erase(pos, len);
            if ( (removeAll == false) || (pos >= static_cast<uint32_t>(mData.length( ))) )
            {
                break;
            }
        }
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::remove( const TEString<CharType>& strRemove, NEString::CharPos startPos /*= NEString::START_POS*/, bool removeAll /*= true*/)
{
    return remove(strRemove.mData, startPos, removeAll);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::resize(NEString::CharCount count, CharType chFill /*= static_cast<CharType>('\0')*/)
{
    mData.resize(static_cast<uint32_t>(count), chFill);
    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::truncate(NEString::CharCount maxChars)
{
    if (maxChars == 0)
    {
        mData.clear();
    }
    else if (maxChars < static_cast<NEString::CharCount>(mData.length()))
    {
        mData.erase(static_cast<uint32_t>(maxChars));
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::reserve(NEString::CharCount newCapacity)
{
    mData.reserve( static_cast<uint32_t>(newCapacity));
    return (*this);
}

template<typename CharType>
inline CharType TEString<CharType>::getAt(NEString::CharPos atPos) const
{
    return (isValidPosition(atPos) ? mData.at(static_cast<uint32_t>(atPos)) : static_cast<CharType>(NEString::EndOfString));
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::setAt(CharType ch, NEString::CharPos atPos /*= NEString::END_POS*/)
{
    if ((atPos >= NEString::START_POS) && (atPos < static_cast<NEString::CharPos>(mData.size())))
    {
        mData.at(static_cast<uint32_t>(atPos)) = ch;
    }
    else if (atPos == NEString::END_POS)
    {
        mData.append(1, ch);
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::trimLeft(void)
{
    if (mData.empty() == false)
    {
        uint32_t count = 0;
        std::locale loc(NEString::LOCALE_DEFAULT);
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
        std::locale loc(NEString::LOCALE_DEFAULT);
        for (const auto& ch : mData)
        {
            if (std::isspace<CharType>(ch, loc) == false)
                break;

            ++count;
        }

        if (count != 0)
        {
            strResult.assign(getBuffer(static_cast<NEString::CharCount>(count)), static_cast<uint32_t>(strResult.length() - count));
        }
    }
}

template<typename CharType>
inline TEString<CharType>& TEString<CharType>::trimRight(void)
{
    if (mData.empty() == false)
    {
        uint32_t count = 0;
        std::locale loc(NEString::LOCALE_DEFAULT);
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
        std::locale loc(NEString::LOCALE_DEFAULT);
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
        std::locale loc(NEString::LOCALE_DEFAULT);

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
        std::locale loc(NEString::LOCALE_DEFAULT);

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
inline TEString<CharType> & TEString<CharType>::makeLower( void )
{
    std::locale loc(NEString::LOCALE_DEFAULT);
    for (CharType * src = mData.data(); *src != TEString<CharType>::EmptyChar; ++src)
    {
        *src = std::tolower(*src, loc);
    }

    return (*this);
}

template<typename CharType>
inline TEString<CharType> & TEString<CharType>::makeUpper( void )
{
    std::locale loc(NEString::LOCALE_DEFAULT);
    for (CharType * src = mData.data(); *src != TEString<CharType>::EmptyChar; ++src)
    {
        *src = std::toupper(*src, loc);
    }

    return (*this);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::readLine(TEString<CharType>& OUT strResult, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    return readLine(strResult.mData, startPos);
}

template<typename CharType>
NEString::CharPos TEString<CharType>::readLine(std::basic_string<CharType>& OUT strResult, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    NEString::CharPos result = NEString::END_POS;
    if (isValidPosition(startPos))
    {
        const CharType* begin = getBuffer(startPos);
        while (NEString::isEndOfLine<CharType>(*begin) && (*begin != EmptyChar))
        {
            // escape end of line symbols at the begin.
            ++begin;
        }

        if (*begin != EmptyChar)
        {
            const CharType* str = begin;
            while ((NEString::isNewLine<CharType>(*str) == false) && (*str != EmptyChar))
            {
                // move until reach end of line
                ++str;
            }

            // copy the line
            strResult.assign(begin, static_cast<uint32_t>(str - begin));

            while (NEString::isEndOfLine<CharType>(*str) && (*str != EmptyChar))
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
inline TEString<CharType>& TEString<CharType>::makeAlphanumeric(void)
{
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
    }

    return (*this);
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
    if (caseSensitive)
    {
        return compareStringExact(startPos, strOther, count);
    }
    else
    {
        return compareStringIgnoreCase(startPos, strOther, count);
    }
}

template<typename CharType>
inline NEMath::eCompare TEString<CharType>::compareStringExact( NEString::CharPos startPos
                                                              , const CharType * strOther
                                                              , NEString::CharCount count/*= NEString::COUNT_ALL */ ) const
{
    NEMath::eCompare result = NEMath::eCompare::Smaller;
    count = count == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(strOther) : count;
    if (isValidPosition(startPos))
    {
        NEString::CharCount len = static_cast<NEString::CharCount>(getLength() - startPos);
        if (count <= len)
        {
            result = NEMemory::memCompare(getBuffer(startPos), strOther, static_cast<uint32_t>(count));
        }
    }

    return result;
}

template<typename CharType>
inline NEMath::eCompare TEString<CharType>::compareStringIgnoreCase( NEString::CharPos startPos
                                                                   , const CharType * strOther
                                                                   , NEString::CharCount count/*= NEString::COUNT_ALL */ ) const
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

            std::locale loc(NEString::LOCALE_DEFAULT);

            while (count-- > 0)
            {
                chLeft  = std::tolower(*leftSide ++, loc);
                chRight = std::tolower(*rightSide ++, loc);
                if (chLeft != chRight)
                {
                    break;
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
        moveTo( endPos, diff );
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
inline NEString::CharPos TEString<CharType>::findFirstPhrase( const CharType* phrase
                                                            , NEString::CharCount count     /* = NEString::COUNT_ALL */
                                                            , NEString::CharPos startPos    /* = NEString::START_POS */) const
{
    if (isValidPosition(startPos) && !NEString::isEmpty<CharType>(phrase))
    {
        uint32_t pos = static_cast<uint32_t>(std::basic_string<CharType>::npos);
        
        if (count == NEString::COUNT_ALL)
        {
            pos = static_cast<uint32_t>(mData.find(phrase, static_cast<uint32_t>(startPos)));
        }
        else
        {
            pos = static_cast<uint32_t>(mData.find(phrase, static_cast<uint32_t>(startPos), static_cast<uint32_t>(count)));
        }

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
    else if ((mData.length() - static_cast<uint32_t>(startPos)) < phrase.length())
        return NEString::END_POS;

    std::locale loc(NEString::LOCALE_DEFAULT);
    auto it = std::search( mData.begin() + static_cast<int32_t>(startPos), mData.end()
                         , phrase.begin(), phrase.end()
                         , [&](const CharType& ch1, const CharType& ch2) { return (std::tolower(ch1, loc) == std::tolower(ch2, loc)); }
                         );

    return (it != mData.end() ? static_cast<NEString::CharPos>(std::distance(mData.begin(), it)) : NEString::END_POS);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::findFirstWord(const CharType* word, bool caseSensitive, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    return (word != nullptr ? findFirstWord(std::basic_string<CharType>(word), caseSensitive, startPos) : NEString::INVALID_POS);
}

template<typename CharType>
inline NEString::CharPos TEString<CharType>::findFirstWord(const std::basic_string<CharType>& word, bool caseSensitive, NEString::CharPos startPos /*= NEString::START_POS*/) const
{
    if (isInvalidPosition(startPos) || word.empty())
        return NEString::INVALID_POS;
    else if (mData.empty())
        return NEString::END_POS;
    else if ((mData.length() - static_cast<uint32_t>(startPos)) < word.length())
        return NEString::END_POS;

    std::locale loc(NEString::LOCALE_DEFAULT);

    NEString::CharPos result = NEString::END_POS;
    while (result == NEString::END_POS)
    {
        auto it = std::search( mData.begin() + static_cast<int32_t>(startPos), mData.end()
                             , word.begin(), word.end()
                             , [&](const CharType& ch1, const CharType& ch2){ return (caseSensitive ? ch1 == ch2 : std::tolower(ch1, loc) == std::tolower(ch2, loc)); }
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
        it += static_cast<int32_t>(word.length());
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
        if ((mData.length() - static_cast<uint32_t>(startPos)) < word.length())
        {
            // if the remaining length is smaller, break, because makes no sense to search
            break;
        }
    }

    return result;
}

template<typename CharType>
inline bool TEString<CharType>::isValidNameChar(const CharType checkChar, std::locale& loc) const
{
    // initialize list of symbols for the valid names.
    constexpr CharType symbols[] = { '_', '\0' };
    return std::isalnum(checkChar, loc) || NEString::isOneOf<CharType>(checkChar, symbols);
}

template<typename CharType>
inline std::vector<TEString<CharType>> TEString<CharType>::split(CharType delimiter) const
{
    std::vector<TEString<CharType>> result;
    size_t start{ 0 };
    while (mData.at(start) != static_cast<CharType>('\0'))
    {
        size_t pos = mData.find_first_of(delimiter, start);
        if (pos != std::basic_string<CharType>::npos)
        {
            result.push_back(TEString<CharType>(mData.c_str() + start, static_cast<NEString::CharCount>(pos - start)));
            start = pos + 1;
        }
        else
        {
            result.push_back(TEString<CharType>(mData.c_str() + start));
            break;
        }
    }

    return result;
}

template<typename CharType>
inline std::vector<TEString<CharType>> TEString<CharType>::split(const TEString<CharType> & delimiter) const
{
    std::vector<TEString<CharType>> result;
    if (delimiter.isEmpty() == false)
    {
        const size_t skip   { static_cast<size_t>(delimiter.getLength()) };
        const size_t len    { mData.length() };
        size_t start        { 0 };
        size_t pos = mData.find(delimiter.mData, start);
        while (pos != std::basic_string<CharType>::npos)
        {
            result.push_back(TEString<CharType>(mData.c_str() + start, static_cast<NEString::CharCount>(pos - start)));
            start = pos + skip;
            pos = (start < len ? mData.find(delimiter.mData, start) : std::basic_string<CharType>::npos);
        }

        if (start < len)
        {
            result.push_back(TEString<CharType>(mData.c_str() + start, static_cast<NEString::CharCount>(len - start)));
        }
    }
    else
    {
        result.push_back(*this);
    }

    return result;
}

template<typename CharType>
inline bool TEString<CharType>::startsWith(const TEString<CharType>& phrase, bool /*isCaseSensitive*/ /*= true*/) const
{
    return startsWith(phrase.mData);
}

template<typename CharType>
inline bool TEString<CharType>::startsWith(const std::basic_string<CharType>& phrase, bool isCaseSensitive /*= true*/) const
{
    return (phrase.length() <= mData.length() ? startsWith(phrase.c_str(), isCaseSensitive, static_cast<NEString::CharCount>(phrase.length())) : false);
}

template<typename CharType>
inline bool TEString<CharType>::startsWith(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive /*= true*/) const
{
    return (phrase.length() <= mData.length() ? startsWith(phrase.data(), isCaseSensitive, static_cast<NEString::CharCount>(phrase.length())) : false);
}

template<typename CharType>
inline bool TEString<CharType>::startsWith(const CharType* phrase, bool isCaseSensitive /*= true*/, NEString::CharCount count /*= NEString::COUNT_ALL*/) const
{
    count = count != NEString::COUNT_ALL ? count : NEString::getStringLength<CharType>(phrase);
    if (NEString::isEmpty<CharType>(phrase) || (count == 0) || (count > static_cast<NEString::CharCount>(mData.length())))
    {
        return false;
    }
    else
    {
        return _hasPhrase(mData.c_str(), phrase, count, isCaseSensitive ? nullptr : NEString::LOCALE_DEFAULT);
    }
}

template<typename CharType>
inline bool TEString<CharType>::endsWith(const TEString<CharType>& phrase, bool isCaseSensitive /*= true*/) const
{
    return endsWith(phrase.mData, isCaseSensitive);
}

template<typename CharType>
inline bool TEString<CharType>::endsWith(const std::basic_string<CharType>& phrase, bool isCaseSensitive /*= true*/) const
{
    return (phrase.length() <= mData.length() ? endsWith(phrase.c_str(), isCaseSensitive, static_cast<NEString::CharCount>(phrase.length())) : false);
}

template<typename CharType>
inline bool TEString<CharType>::endsWith(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive /*= true*/) const
{
    return (phrase.length() <= mData.length() ? endsWith(phrase.data(), isCaseSensitive, static_cast<NEString::CharCount>(phrase.length())) : false);
}

template<typename CharType>
inline bool TEString<CharType>::endsWith(const CharType* phrase, bool isCaseSensitive /*= true*/, NEString::CharCount count /*= NEString::COUNT_ALL*/) const
{
    count = count != NEString::COUNT_ALL ? count : NEString::getStringLength<CharType>(phrase);
    if (NEString::isEmpty<CharType>(phrase) || (count == 0) || (count > static_cast<NEString::CharCount>(mData.length())))
    {
        return false;
    }
    else
    {
        NEString::CharCount skip = static_cast<NEString::CharCount>(mData.length() - static_cast<uint32_t>(count));
        ASSERT(skip >= 0);
        return _hasPhrase(mData.c_str() + skip, phrase, count, isCaseSensitive ? nullptr : NEString::LOCALE_DEFAULT);
    }
}

template<typename CharType>
inline bool TEString<CharType>::_hasPhrase(const CharType* fullString, const CharType* phrase, NEString::CharCount count, const char* locale) const
{
    ASSERT((fullString != nullptr) && (phrase != nullptr));

    bool result{ true };
    if (locale == nullptr)
    {
        result = ::memcmp(fullString, phrase, static_cast<uint32_t>(count) * sizeof(CharType)) == 0;
    }
    else
    {
        std::locale loc(locale);
        for (; *phrase != TEString<CharType>::EmptyChar; ++phrase, ++fullString)
        {
            if (std::tolower<CharType>(*phrase, loc) != std::tolower<CharType>(*fullString, loc))
            {
                result = false;
                break;
            }
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// TEString friend operators
//////////////////////////////////////////////////////////////////////////
template<typename CT>
inline TEString<CT> operator + (const TEString<CT>& lhs, const TEString<CT>& rhs)
{
    TEString<CT> result(lhs);
    return (result += rhs);
}

template<typename CT>
inline TEString<CT> operator + (const TEString<CT>& lhs, const std::basic_string<CT>& rhs)
{
    TEString<CT> result(lhs);
    return (result += rhs);
}

template<typename CT>
inline TEString<CT> operator + (const TEString<CT>& lhs, const std::basic_string_view<CT>& rhs)
{
    TEString<CT> result(lhs);
    return (result += rhs);
}

template<typename CT>
inline TEString<CT> operator + (const TEString<CT>& lhs, const CT* rhs)
{
    TEString<CT> result(lhs);
    return (result += rhs);
}

template<typename CT>
inline TEString<CT> operator + (const TEString<CT>& lhs, const CT rhs)
{
    TEString<CT> result(lhs);
    return (result += rhs);
}

template<typename CT>
inline TEString<CT> operator + (const std::basic_string<CT>& lhs, const TEString<CT>& rhs)
{
    TEString<CT> result(lhs);
    return (result += rhs);
}

template<typename CT>
inline TEString<CT> operator + (const std::basic_string_view<CT>& lhs, const TEString<CT>& rhs)
{
    TEString<CT> result(lhs);
    return (result += rhs);
}

template<typename CT>
inline TEString<CT> operator + (const CT* lhs, const TEString<CT>& rhs)
{
    TEString<CT> result(lhs);
    return (result += rhs);
}

template<typename CT>
inline TEString<CT> operator + (const CT lhs, const TEString<CT>& rhs)
{
    TEString<CT> result(lhs);
    return (result += rhs);
}

template<typename CT>
inline const IEInStream& operator >> (const IEInStream& stream, TEString<CT>& input)
{
    return (stream >> input.mData);
}

template<typename CT>
inline IEOutStream& operator << (IEOutStream& stream, const TEString<CT>& output)
{
    return (stream << output.mData);
}

template<typename CT>
inline std::ostream & operator << ( std::ostream & stream, const TEString<CT> & output )
{
    return (stream << output.getData( ));
}

template<typename CT>
inline const std::istream & operator >> ( const std::istream & stream, TEString<CT> & input )
{
    return (stream >> input.getData( ));
}

#endif  // AREG_BASE_TESTRING_HPP
