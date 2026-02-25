#ifndef AREG_BASE_STRINGBASE_HPP
#define AREG_BASE_STRINGBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/StringBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, String class template based on first 256 character in UTF-8 code page.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/TemplateBase.hpp"
#include "areg/base/StringDefs.hpp"

#include <algorithm>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

/************************************************************************
 * Dependencies
 ************************************************************************/
class InStream;
class OutStream;

namespace areg
{

    //////////////////////////////////////////////////////////////////////////
    // StringBase class template declaration
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
    class StringBase
    {
    //////////////////////////////////////////////////////////////////////////
    // Public constants
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Empty character constant
         **/
        static constexpr CharType   EmptyChar       { static_cast<CharType>(areg::EndOfString) };

        /**
         * \brief   New line constant
         **/
        static constexpr CharType   NewLine         { static_cast<CharType>(areg::EndOfLine) };

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
        inline StringBase();

        /**
         * \brief   Sets the char to string.
         * \param   source    The character to set at the begin of string.
         **/
        inline StringBase( const CharType source );
        inline StringBase( const CharType * source );
        inline StringBase( const std::basic_string<CharType>& source );
        inline StringBase( const std::basic_string_view<CharType>& source );


        /**
         * \brief   Copies specified amount of symbols from given string.
         * \param   source  The null-terminated string buffer to initialize. If nullptr, empty string is created.
         * \param   count   The number of character to initialize from given string source. If zero, empty string is created.
         **/
        inline StringBase( const CharType * source, areg::CharCount count );

        /**
         * \brief   Copy constructor.
         * \param   source   The source to copy data.
         **/
        inline StringBase( const StringBase<CharType> & source );

        /**
         * \brief   Move constructor.
         * \param   source   The source to move data.
         **/
        inline StringBase( StringBase<CharType> && source ) noexcept;
        inline StringBase( std::basic_string<CharType>&& source ) noexcept;

        /**
         * \brief   Initializes string and reserves a space for specified amount of characters.
         *          Note, the size of string remains zero.
         *
         * \param   count   The space to reserve.
         */
        explicit inline StringBase(uint32_t count);

        /**
         * \brief   Destructor.
         **/
        ~StringBase() = default;

    //////////////////////////////////////////////////////////////////////////
    // operators
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Converting operator, converts object to string buffer
         **/
        inline operator const CharType* () const;

        /**
         * \brief   Returns character at the given valid position. The position should not be
         *          neither negative, nor more than the string length.
         * 
         * \param index     Valid zero-based index, which value should not be more than the length of string.
         **/
        inline const CharType operator[ ](int32_t index) const;

        /**
         * \brief   Assigning operator, copies data from given string source
         * \param   src     The source of string to copy.
         * \return  Returns the string object.
         **/
        inline StringBase<CharType>& operator = (const StringBase<CharType>& src);
        inline StringBase<CharType>& operator = (const std::basic_string<CharType>& src);
        inline StringBase<CharType>& operator = (const std::basic_string_view<CharType>& src);
        inline StringBase<CharType>& operator = (const CharType* src);
        inline StringBase<CharType>& operator = (StringBase<CharType> && src) noexcept;
        inline StringBase<CharType>& operator = (std::basic_string<CharType> && src) noexcept;
        inline StringBase<CharType>& operator = (const CharType src);

        /**
         * \brief   Determines equality of two strings.
         * \remark  The comparing of strings is case-sensitive.
         *          To compare case-insensitive, use compare() methods.
         * \param   other   The second string to compare
         * \return  Returns true if 2 strings are equal.
         **/
        inline bool operator == (const StringBase<CharType>& other) const;
        inline bool operator == (const std::basic_string<CharType>& other) const;
        inline bool operator == (const std::basic_string_view<CharType>& other) const;
        inline bool operator == (const CharType * other) const;
        inline bool operator == (const CharType ch ) const;
        inline bool operator != (const StringBase<CharType>& other) const;
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
        inline bool operator > (const StringBase<CharType>& other) const;
        inline bool operator > (const std::basic_string<CharType>& other) const;
        inline bool operator > (const std::basic_string_view<CharType>& other) const;
        inline bool operator > (const CharType* other) const;
        inline bool operator < (const StringBase<CharType>& other) const;
        inline bool operator < (const CharType* other) const;
        inline bool operator < (const std::basic_string<CharType>& other) const;
        inline bool operator < (const std::basic_string_view<CharType>& other) const;
        inline bool operator >= (const StringBase<CharType>& other) const;
        inline bool operator >= (const CharType* other) const;
        inline bool operator >= (const std::basic_string<CharType>& other) const;
        inline bool operator >= (const std::basic_string_view<CharType>& other) const;
        inline bool operator <= (const StringBase<CharType>& other) const;
        inline bool operator <= (const CharType* other) const;
        inline bool operator <= (const std::basic_string<CharType>& other) const;
        inline bool operator <= (const std::basic_string_view<CharType>& other) const;

        /**
         * \brief   Appends given string at the end of existing string.
         * \param   src     The source of string to append at the end
         * \return  Returns string object
         **/
        inline StringBase<CharType>& operator += (const StringBase<CharType>& src);
        inline StringBase<CharType>& operator += (const std::basic_string<CharType>& src);
        inline StringBase<CharType>& operator += (const std::basic_string_view<CharType>& src);
        inline StringBase<CharType>& operator += (const CharType* src);

        /**
         * \brief   Appends given character at the end of existing string.
         * \param   chSource   The source of character to append at the end
         * \return  Returns string object
         **/
        inline StringBase<CharType>& operator += (const CharType chSource);

        /**
         * \brief   Operator to concatenate 2 strings.
         *
         * \param   lhs     Left-hand side string object
         * \param   rhs     Right-hand side string object
         * \return  Returns newly constructed string object by concatenating 2 strings
         **/
        template<typename CT>
        friend inline StringBase<CT> operator + (const StringBase<CT>& lhs, const StringBase<CT>& rhs);
        template<typename CT>
        friend inline StringBase<CT> operator + (const StringBase<CT>& lhs, const std::basic_string<CT>& rhs);
        template<typename CT>
        friend inline StringBase<CT> operator + (const StringBase<CT>& lhs, const std::basic_string_view<CT>& rhs);
        template<typename CT>
        friend inline StringBase<CT> operator + (const StringBase<CT>& lhs, const CT* rhs);
        template<typename CT>
        friend inline StringBase<CT> operator + (const StringBase<CT>& lhs, const CT rhs);
        template<typename CT>
        friend inline StringBase<CT> operator + (const std::basic_string<CT>& lhs, const StringBase<CT>& rhs);
        template<typename CT>
        friend inline StringBase<CT> operator + (const std::basic_string_view<CT>& lhs, const StringBase<CT>& rhs);
        template<typename CT>
        friend inline StringBase<CT> operator + (const CT* lhs, const StringBase<CT>& rhs);
        template<typename CT>
        friend inline StringBase<CT> operator + (const CT lhs, const StringBase<CT>& rhs);

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
        friend inline const InStream & operator >> (const InStream & stream, StringBase<CT> & input);

        /**
         * \brief   Streams from output object, i.e. write data from string to streaming object.
         * \tparam  CT      The type of character.
         * \param   stream  Streaming object to write data.
         * \param   output  String object to read data from
         * \return  Reference to stream object.
         **/
        template<typename CT>
        friend inline OutStream & operator << (OutStream & stream, const StringBase<CT> & output);

        /**
         * \brief   Prints string message to the standard output stream.
         * \tparam  CT      The type of character.
         * \param   stream  Standard streaming object to print message.
         * \param   output  String object with the message.
         * \return  Reference to stream object.
         **/
        template<typename CT>
        friend inline std::ostream & operator << ( std::ostream & stream, const StringBase<CT> & output );

        /**
         * \brief   Reads string message from the standard input stream.
         * \tparam  CT      The type of character.
         * \param   stream  Standard streaming object to read message.
         * \param   input   String object to save the message.
         * \return  Reference to stream object.
         **/
        template<typename CT>
        friend inline const std::istream & operator >> ( const std::istream & stream, StringBase<CT> & input );

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
        inline bool isAlphanumeric() const;

        /**
         * \brief   Returns true if string is valid for name.
         *          The name is valid if it contains letters, numbers and following symbols {'_'}.
         *          All other symbols are rejected.
         **/
        inline bool isValidName() const;

        /**
         * \brief   Returns true if the passed character is valid for the name.
         **/
        inline bool isValidNameChar(const CharType checkChar) const;

        /**
         * \brief   Returns true if string is empty
         **/
        inline bool isEmpty() const;

        /**
         * \brief   Returns the length of the string.
         **/
        inline areg::CharCount getLength() const;

        /**
         * \brief   Returns the number of characters that can store in the string.
         **/
        inline areg::CharCount getCapacity() const;

        /**
         * \brief   Return the size of string in bytes including the end of the string character.
         **/
        inline uint32_t getSpace() const;

        /**
         * \brief   Returns string buffer starting at specified valid position.
         *
         * \param   startAt     The offset of position in string to return buffer.
         *                      By default it returns begin of string buffer.
         * \return  Returns valid pointer of the string buffer at specified position.
         **/
        inline const CharType* getBuffer(areg::CharPos startAt = areg::START_POS) const;
        inline CharType* getBuffer(areg::CharPos startAt = areg::START_POS);

        /**
         * \brief   Returns the buffer of string.
         **/
        inline const CharType* getString() const;

        /**
         * \brief   Returns the string object.
         **/
        inline const std::basic_string<CharType>& getData() const;

        /**
         * \brief   Returns true if specified character position is valid in the string.
         * \param   pos     Zero-based position in the string.
         * \return  Returns true if specified position is valid to read character.
         **/
        inline bool isValidPosition(areg::CharPos pos) const;

        /**
         * \brief   Returns true if specified character position is invalid in the string.
         * \param   pos     Zero-based position in the string.
         * \return  Returns true if specified position is invalid.
         **/
        inline bool isInvalidPosition(areg::CharPos pos) const;

        /**
         * \brief   Returns true if specified position is equal to the end of the string.
         * \param   pos     The zero-based position in the string.
         **/
        inline bool isLastPosition(areg::CharPos pos) const;

        /**
         * \brief   Returns true if string is not empty and the specified position is zero.
         **/
        inline bool isFirstPosition(areg::CharPos pos) const;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Clears the string, makes it empty and invalid.
         **/
        inline void clear();

        /**
         * \brief   Delete unused space in the string.
         **/
        inline void freeExtra();

        /**
         * \brief   Sets the size of string to zero and deletes the space.
         */
        inline void release();

        /**
         * \brief   Searches the whole word in the string at specified 'startAt' position.
         *          On output, the 'startAt' and 'endAt' contain start and end of position where each
         *          word start and end. The length of the word calculated by 'endAt - startAt' formula.
         *          The word is a collection of characters and digits, and allowed in the valid name
         *          characters ('_'), which are surrounded by other symbols (whitespace, comma, dot, etc),
         *          or if it starts at the beginning or end of the string.
         *
         * \param[in,out]   startAt On input, this should be valid position in the string to start to search.
         *                          On output, it contains the next position of the next word.
         * \param[out]      endAt   On output, it contains the position where the next word finishes.
         **/
        void findWord(areg::CharPos& startAt, areg::CharPos& endAt) const;

        /**
         * \brief   Searches the whole word in the string at specified 'startAt' position.
         *          On output, the 'startAt' and 'endAt' contain start and end of position where each
         *          word start and end. On output, the 'word' is the word with length 'endAt - startAt'.
         *          The word is a collection of characters and digits, and allowed in the valid name
         *          characters ('_'), which are surrounded by other symbols (whitespace, comma, dot, etc),
         *          or if it starts at the beginning or end of the string.
         *
         * \param[out]      word        On output contains the word that it could find.
         * \param[in,out]   startAt     On input, this should be valid position in the string to start to search.
         *                              On output, it contains the next position of the next word.
         * \param[out]      endAt       On output, it contains the position where the next word finishes.
         **/
        void getWord(StringBase<CharType>& word, areg::CharPos& startAt, areg::CharPos& endAt) const;

        /**
         * \brief   Find the first occurrence of any of the characters in string buffer 'chars'.
         *          The search starts at given 'startPos'. Returns valid position if found any.
         *          Otherwise, returns areg::INVALID_POS.
         *
         * \param   chars       The one or more characters as null-terminated string to search.
         * \param   startPos    The starting position in string to search. Should be valid.
         *                      By default, starts to search at begin of string, i.e. at 'areg::START_POS'
         * \return  Returns valid string position value, if any occurrence of given characters found.
         *          Otherwise, returns areg::INVALID_POS value.
         **/
        areg::CharPos findOneOf( const CharType * chars, areg::CharPos startPos = areg::START_POS ) const;

        /**
         * \brief   Find the first occurrence of given character in the string. If found, returns valid
         *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
         *
         * \param   chSearch        The character to search in the string.
         * \param   startPos        The starting position in string to search. Should be valid.
         *                          By default, starts to search at the begin of string, i.e. at 'areg::START_POS'
         * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
         *                          Otherwise, the search is by upper and lower case.
         * \return  Returns valid string position value, if found given character.
         *          Otherwise, returns areg::INVALID_POS value.
         **/
        areg::CharPos findFirst( CharType chSearch, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true ) const;

        /**
         * \brief   Find the first occurrence of given phrase in the string. If found, returns valid
         *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
         *
         * \param   phrase          The phrase to search in the string.
         * \param   startPos        The starting position in string to search. Should be valid.
         *                          By default, starts to search at the begin of string, i.e. at 'areg::START_POS'
         * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
         *                          Otherwise, the search is by upper and lower case.
         * \param   wholeWord       If true, the search of phrase should be the whole word.
         *                          Otherwise, searches by any match. By default, the search is by any match.
         * \return  Returns valid string position value, if found given character.
         *          Otherwise, returns areg::INVALID_POS value.
         **/
        areg::CharPos findFirst( const CharType * phrase, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true, bool wholeWord = false ) const;
        areg::CharPos findFirst( const StringBase<CharType> & phrase, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true, bool wholeWord = false ) const;

        /**
         * \brief   Find the last occurrence of given character in the string. If found, returns valid
         *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
         *
         * \param   chSearch        The character to search in the string.
         * \param   startPos        The starting position in string to search. Should be valid.
         *                          By default, starts to search at the end of string, i.e. at 'areg::END_POS'
         * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
         *                          Otherwise, the search is by upper and lower case.
         * \return  Returns valid string position value, if found given character.
         *          Otherwise, returns areg::INVALID_POS value.
         **/
        areg::CharPos findLast( CharType chSearch, areg::CharPos startPos = areg::END_POS, bool caseSensitive = true ) const;

        /**
         * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
         *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
         * \param   phrase          The phrase to search in the string.
         * \param   phraseCount     The length of the phrase or the number of characters in the phrase to search.
         * \param   startPos        The starting position in string to search. Should be valid.
         *                          By default, starts to search at the begin of string, i.e. at 'areg::END_POS'
         * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
         *                          Otherwise, the search is by upper and lower case.
         * \return  Returns valid string position value, if found given character.
         *          Otherwise, returns areg::INVALID_POS value.
         **/
        areg::CharPos findLast( const CharType * phrase, areg::CharCount phraseCount, areg::CharPos startPos, bool caseSensitive) const;

        /**
         * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
         *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
         * \param   phrase          The phrase to search in the string.
         * \param   startPos        The starting position in string to search. Should be valid.
         *                          By default, starts to search at the begin of string, i.e. at 'areg::END_POS'
         * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
         * \return  Returns valid string position value, if found given character.
         *          Otherwise, returns areg::INVALID_POS value.
         **/
        areg::CharPos findLast( const CharType* phrase, areg::CharPos startPos = areg::END_POS, bool caseSensitive = true) const;

        /**
         * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
         *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
         * \param   phrase          The phrase to search in the string.
         * \param   startPos        The starting position in string to search. Should be valid.
         *                          By default, starts to search at the begin of string, i.e. at 'areg::END_POS'
         * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
         * \return  Returns valid string position value, if found given character.
         *          Otherwise, returns areg::INVALID_POS value.
         **/
        areg::CharPos findLast( const StringBase<CharType> & phrase, areg::CharPos startPos = areg::END_POS, bool caseSensitive = true ) const;

        /**
         * \brief   Compares the given string. The comparing is done by certain position, certain amount of characters
         *          or by ignoring upper and lower cases, depending parameters are passed in the function.
         *
         * \param   strOther        The given string to compare.
         * \param   startAt         If given, compares at specified zero-based position of the string.
         *                          Otherwise, it compares at the begin of string (areg::START_POS).
         * \param   count           The number of characters to compare. By default, it compares until end of string
         * \param   caseSensitive   If true, compares by exact match, i.e. case-sensitive.
         *                          Otherwise, it ignores upper and lower cases.
         * \return  Returns:
         *              areg::Smaller if string is less than given string
         *              areg::Equal   if strings have equal
         *              areg::Bigger  if string is more than given string
         **/
        areg::Ordering compare( const CharType * strOther, areg::CharPos startAt = areg::START_POS, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true) const;
        inline areg::Ordering compare(const StringBase<CharType> & strOther, bool caseSensitive = true) const;
        inline areg::Ordering compare(const std::basic_string<CharType>& strOther, bool caseSensitive = true) const;
        inline areg::Ordering compare(const std::basic_string_view<CharType>& strOther, bool caseSensitive = true) const;
        inline areg::Ordering compare(const StringBase<CharType>& strOther, areg::CharPos startAt = areg::START_POS, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true) const;
        inline areg::Ordering compare(const std::basic_string<CharType>& strOther, areg::CharPos startAt = areg::START_POS, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true) const;

        /**
         * \brief   Truncates the starting at zero-based 'startPos' and given number of characters.
         *          If 'startPos' is beginning of the string (areg::START_POS) and 'count' is
         *          equal to the length of the string or areg::COUNT_ALL, the string is not modified.
         *
         * \param   startPos    Starting position of the substring to create.
         *                      By default, the substring starts at the begin, i.e. areg::START_POS
         * \param   count       The number of characters in substring, starting at given starting position.
         *                      By default, it gets characters until end of string.
         * \return  Returns truncated string.
         **/
        inline StringBase<CharType>& substring(areg::CharPos startPos, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Extracts the substring starting at startPos and given number of characters.
         * \param[out]  outResult   On output, this parameter contains resulting substring.
         * \param[in]   startPos    Starting position of the substring to create.
         *                          By default, the substring starts at the begin, i.e. areg::START_POS
         * \param[in]   count       The number of characters in substring, starting at given starting position.
         *                          By default, it gets characters until end of string.
         **/
        inline void substring( StringBase<CharType> & outResult, areg::CharPos startPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL ) const;

        /**
         * \brief   Searches given phrase in the string starting from given position until the end of string.
         *          If found, copies the string data into the result until the found position and returns position
         *          next after phrase. If not found, will copy complete string until end.
         * \param[out]  outResult   On output, the string contains result of copied data until found position
         *                          or contains the complete string until the end.
         * \param[in]   strPhrase   The phrase to search in the string.
         * \param[in]   startPos    The starting position to search the string.
         * \return  Returns next position after searched phrase and value are followings:
         *              -   Valid string position not equal to areg::END_POS,
         *                  if found phrase and the phrase is not at the end;
         *              -   areg::END_POS if found the phrase at end of string;
         *              -   areg::INVALID_POS if could not find the phrase.
         *
         * \example     The example of substring
         * \code [cpp]
         *
         *  String test("0123 456 789 0123");
         *  String result;
         *  areg::CharPos next = areg::START_POS;
         *  next = test.substr(result, "0123", next);   // results: next == 4, result == ""
         *  next = test.substr(result, "0123", next);   // results: next == areg::END_POS, result == " 456 789 "
         *
         *  next = areg::START_POS;
         *  next = test.Substr(result, " ", next);      // results: next == 5, result == "0123"
         *  next = test.Substr(result, " ", next);      // results: next == 9, result == "456"
         *  next = test.Substr(result, " ", next);      // results: next == areg::INVALID_POS, result == "0123"
         * \endcode
         **/
        inline areg::CharPos substring(StringBase<CharType>& outResult, const CharType* strPhrase, areg::CharPos startPos = areg::START_POS) const;
        inline areg::CharPos substring(StringBase<CharType>& outResult, const StringBase<CharType>& strPhrase, areg::CharPos startPos = areg::START_POS) const;
        inline areg::CharPos substring(StringBase<CharType>& outResult, const std::basic_string<CharType>& strPhrase, areg::CharPos startPos = areg::START_POS) const;

        /**
         * \brief   Searches given symbol in the string starting from given position until end of string.
         *          If found, copies the string data into the result until the found position and returns position
         *          next after symbol. If not found, will copy complete string until end.
         * \param[out]  outResult   On output, the string contains result of copied data until found position
         *                          or contains the complete string until the end.
         * \param[in]   chSymbol    The symbol to search in the string.
         * \param[in]   startPos    The starting position to search the symbol.
         * \return  Returns next position after searched symbol and value are followings:
         *              -   Valid string position not equal to areg::END_POS,
         *                  if found phrase and the symbol is not at the end;
         *              -   areg::END_POS if found the symbol at end of string;
         *              -   areg::INVALID_POS if could not find the phrase.
         **/
        areg::CharPos substring( StringBase<CharType> & outResult, CharType chSymbol, areg::CharPos startPos = areg::START_POS ) const;

        /**
         * \brief   Returns left side (begin) substring of length 'charCount'
         * \param   charCount   The number of characters to extract.
         * \return  Returns the left side substring of length 'charCount' or empty string is string is empty.
         **/
        inline StringBase<CharType> leftSide(areg::CharCount charCount) const;

        /**
         * \brief   Returns right side (end) substring of length 'charCount'
         * \param   charCount   The number of characters to extract.
         * \return  Returns the right side substring of length 'charCount' or empty string is string is empty.
         **/
        inline StringBase<CharType> rightSide(areg::CharCount charCount) const;

        /**
         * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
         *          If string has not enough space to copy characters, it will reallocate the space.
         * \param   source  The source of string to copy characters.
         * \param   count   The number of characters to copy. By default, it copies all characters.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& assign(const CharType * source, areg::CharCount count = areg::COUNT_ALL );

        /**
         * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
         *          If string has not enough space to copy characters, it will reallocate the space.
         * \param   source  The source of string to copy characters.
         * \param   pos     The position in source string to start to copy.
         * \param   count   The number of characters to copy. By default, it copies all characters.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& assign(const std::basic_string<CharType> & source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
         *          If string has not enough space to copy characters, it will reallocate the space.
         * \param   source  The source of string to copy characters.
         * \param   pos     The position in source string to start to copy.
         * \param   count   The number of characters to copy. By default, it copies all characters.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& assign(const std::basic_string_view<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
         *          If string has not enough space to copy characters, it will reallocate the space.
         * \param   source  The source of string to copy characters.
         * \param   pos     The position in source string to start to copy.
         * \param   count   The number of characters to copy. By default, it copies all characters.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& assign(const StringBase<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
         *          If string has not enough space to copy characters, it will reallocate the space.
         * \param   ch      A character to assign.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& assign(CharType ch);

        /**
         * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
         *          and by amount of characters to append.
         * \param   source  The source of string to append characters.
         * \param   count   If specified, the number of characters to append. By default, it appends all characters.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& append(const CharType * source, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
         *          and by amount of characters to append.
         * \param   source  The source of string to append characters.
         * \param   pos     If specified the valid zero-based position in the given string to append.
         *                  Otherwise, it append starting from the beginning.
         * \param   count   If specified, the number of characters to append. By default, it appends all characters.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& append(const std::basic_string<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
         *          and by amount of characters to append.
         * \param   source  The source of string to append characters.
         * \param   pos     If specified the valid zero-based position in the given string to append.
         *                  Otherwise, it append starting from the beginning.
         * \param   count   If specified, the number of characters to append. By default, it appends all characters.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& append(const std::basic_string_view<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

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
        inline StringBase<CharType>& append(const StringBase<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Appends given string at the end. The given string can be limited by zero-based valid position
         *          and by amount of characters to append.
         * \param   ch      A character to append.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& append(CharType ch);

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
        StringBase<CharType>& moveTo(areg::CharPos startPos, int32_t moveTo);

        /**
         * \brief   Inserts given source of character at given valid zero-based position in the string.
         *          The character can be inserted at any position, including begin of string or
         *          at the end of string (areg::END_POS).
         * \param   source  The character to insert.
         * \param   atPos   The position in the string to insert.
         * \param   count   The number of characters to insert. By default, it is one character.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& insertAt( CharType source, areg::CharPos atPos, areg::CharCount count = 1 );

        /**
         * \brief   Inserts given source at given valid zero-based position in the string.
         *          The character can be inserted at any position, including begin of string or
         *          at the end of string (areg::END_POS).
         * \param   source      The source of character to insert.
         * \param   atDstPos    The position in the destination string to insert.
         *                      By default, it is the beginning of source string.
         * \param   count       If given, the number of characters in the source string to insert.
         *                      By default it inserts the complete source of string.
         **/
        inline StringBase<CharType>& insertAt( const CharType * source, areg::CharPos atDstPos, areg::CharCount count = areg::COUNT_ALL );

        /**
         * \brief   Inserts given source at given valid zero-based position in the string.
         *          The character can be inserted at any position, including begin of string or
         *          at the end of string (areg::END_POS).
         *
         * \param   source      The source of character to insert.
         * \param   atDstPos    The position in the destination string to insert.
         * \param   atSrcPos    If given, the position in source string to take for inserting.
         *                      By default, it is the beginning of source string.
         * \param   count       If given, the number of characters in the source string to insert.
         *                      By default it inserts the complete source of string.
         **/
        inline StringBase<CharType>& insertAt(const std::basic_string<CharType>& source, areg::CharPos atDstPos, areg::CharPos atSrcPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Inserts given source at given valid zero-based position in the string.
         *          The character can be inserted at any position, including begin of string or
         *          at the end of string (areg::END_POS).
         *
         * \param   source      The source of character to insert.
         * \param   atDstPos    The position in the destination string to insert.
         * \param   atSrcPos    If given, the position in source string to take for inserting.
         *                      By default, it is the beginning of source string.
         * \param   count       If given, the number of characters in the source string to insert.
         *                      By default it inserts the complete source of string.
         **/
        inline StringBase<CharType>& insertAt(const std::basic_string_view<CharType>& source, areg::CharPos atDstPos, areg::CharPos atSrcPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Inserts given source at given valid zero-based position in the string.
         *          The character can be inserted at any position, including begin of string or
         *          at the end of string (areg::END_POS).
         *
         * \param   source      The source of character to insert.
         * \param   atDstPos    The position in the destination string to insert.
         * \param   atSrcPos    If given, the position in source string to take for inserting.
         *                      By default, it is the beginning of source string.
         * \param   count       If given, the number of characters in the source string to insert.
         *                      By default it inserts the complete source of string.
         **/
        inline StringBase<CharType>& insertAt(const StringBase<CharType>& source, areg::CharPos atDstPos, areg::CharPos atSrcPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

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
        StringBase<CharType>& replace( CharType chSearch, CharType chReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true );

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
        StringBase<CharType>& replace( const CharType * strSearch, const CharType * strReplace, areg::CharPos startPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL, bool replaceAll = true );
        StringBase<CharType>& replace(const std::basic_string<CharType>& strSearch, const std::basic_string<CharType>& strReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true);
        StringBase<CharType>& replace(const std::basic_string_view<CharType>& strSearch, const std::basic_string_view<CharType>& strReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true);
        inline StringBase<CharType>& replace(const StringBase<CharType>& strSearch, const StringBase<CharType>& strReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true);

        /**
         * \brief   At the given position of the string removes specified amount of characters and replaces by the given string.position after replacing string.
         *
         * \param   startPos    The valid zero-based position to start modifying string.
         * \param   charsRemove Amount of characters to remove to replace in string.
         * \param   strReplace  The string to set.
         * \param   count       The number of characters in the given string to use to replace .
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& replace(areg::CharPos startPos, areg::CharCount charsRemove, const CharType* strReplace, areg::CharCount count = areg::COUNT_ALL);
        inline StringBase<CharType>& replace(areg::CharPos startPos, areg::CharCount charsRemove, const std::basic_string<CharType>& strReplace);
        inline StringBase<CharType>& replace(areg::CharPos startPos, areg::CharCount charsRemove, const StringBase<CharType>& strReplace);

        /**
         * \brief   Removes specified amount of characters in string at specified starting position.
         *          If 'count' is areg::COUNT_ALL, it will remove all characters until end of the string.
         *
         * \param   startPos    The valid zero-based starting position to remove string.
         * \param   count       The amount of characters to remove or until end of the string if areg::COUNT_ALL.
         * \return  Returns modified string.
         **/
        StringBase<CharType>& remove(areg::CharPos startPos, areg::CharCount count = areg::COUNT_ALL);

        /**
         * \brief   Starting from the given valid zero-based position searches and removes specified character.
         *          If 'removedAll' parameter is true, removes all matches in the string. Otherwise, it removes only first match.
         *
         * \param   chRemove    The character to search and remove.
         * \param   startPos    The valid zero-based position to start to search character.
         * \param   removeAll   If true, will remove all matches. Otherwise, removes first match of the character.
         * \return  Returns modified string.
         **/
        StringBase<CharType>& remove(const CharType chRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true);

        /**
         * \brief   Starting from the given valid zero-based position searches and removes given phrase of string. The search starts at given position.
         *          If 'removedAll' parameter is true, removes all matches in the string. Otherwise, it removes only first match.
         *
         * \param   strRemove   The string to search and remove.
         * \param   startPos    The valid zero-based position to start to search character.
         * \param   removeAll   If true, will remove all matches. Otherwise, removes first match of the character.
         * \return  Returns modified string.
         **/
        inline StringBase<CharType>& remove( const CharType * strRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true );
        inline StringBase<CharType>& remove(const StringBase<CharType>& strRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true);
        StringBase<CharType>& remove(const std::basic_string<CharType> & strRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true);

        /**
         * \brief   Resizes the string to contain count characters.
         *          If the current size is less than count, additional characters are appended.
         *          If the current size is greater than count, the string is reduced to its first count elements.
         *
         * \param   count   The maximum length of string.
         * \param   chFill  The characters to fill if new size if longer than the current length.
         **/
        inline StringBase<CharType>& resize(areg::CharCount count, CharType chFill = static_cast<CharType>('\0'));

        /**
         * \brief   If the length of string is bigger than the 'maxChars', it truncated the string.
         *          If 'maxChars' is zero, empties the string.
         *          If the length of the string is smaller than the 'maxChars', nothing happens.
         * \param   maxChars    The maximum characters in the string.
         */
        inline StringBase<CharType>& truncate(areg::CharCount maxChars);

        /**
         * \brief   Reserves the space for the string. Unlike 'resize' this operation does not change the current length of the string.
         * \param   newCapacity     New capacity of the string to set.
         */
        inline StringBase<CharType>& reserve(areg::CharCount newCapacity);

        /**
         * \brief   Returns character at specified valid zero-based position.
         *
         * \param   atPos   The position to return character value.
         * \return  Returns character at specified position of string buffer.
         *          Returns areg::EndOfString if position is invalid or
         *          equal to string length.
         **/
        inline CharType getAt( areg::CharPos atPos ) const;

        /**
         * \brief   Sets character at specified valid zero-based position.
         *
         * \param   ch      The character to set.
         * \param   atPos   The position where to set character.
         *                  If the position is valid, it sets the character at the specified position.
         *                  If the position is areg::END_POS, it appends a character at the end of the string.
         *                  In all other cases, the operation is ignored.
         **/
        inline StringBase<CharType>& setAt( CharType ch, areg::CharPos atPos = areg::END_POS );

        /**
         * \brief   Removes whitespace characters from left side, i.e. from the begin of the string
         *          and returns the actual modified string.
         **/
        inline StringBase<CharType>& trimLeft();

        /**
         * \brief   Copies data into given string without trailing whitespace at the begin of the string.
         *          The existing string remain unmodified.
         *
         * \param[out]  strResult   The destination string to copy result.
         **/
        inline void trimLeft( StringBase<CharType> & strResult ) const;
        inline void trimLeft(std::basic_string<CharType>& strResult) const;

        /**
         * \brief   Removes whitespace characters from right side, i.e. from end of the string
         *          and returns the actual modified string.
         **/
        inline StringBase<CharType>& trimRight();

        /**
         * \brief   Copies data into given string without trailing whitespace at the end of the string.
         *          The existing string remain unmodified.
         *
         * \param[out]  strResult   The destination string to copy result.
         **/
        inline void trimRight( StringBase<CharType> & strResult ) const;
        inline void trimRight(std::basic_string<CharType>& strResult) const;

        /**
         * \brief   Removes whitespace characters from left and right sides, i.e. from the begin and end of the string
         *          and returns the actual modified string.
         **/
        StringBase<CharType>& trimAll();

        /**
         * \brief   Copies data into given string without trailing whitespace at the begin and end of the string.
         *          The existing string remain unmodified.
         *
         * \param[out]  strResult   The destination string to copy result.
         **/
        inline void trimAll( StringBase<CharType> & strResult ) const;
        void trimAll(std::basic_string<CharType>& strResult) const;

        /**
         * \brief   Converts the string, makes all letters to lower case and returns the instance of converted string.
         **/
        inline StringBase<CharType> & makeLower();

        /**
         * \brief   Converts the string, makes all letters to upper case and returns the instance of converted string.
         **/
        inline StringBase<CharType> & makeUpper();

        /**
         * \brief   Reads the string starting from specified position until end of line,
         *          copies the result into the 'strResult' and returns the next position
         *          where new line begins or returns areg::END_POS if reached end of string.
         * \note    If after reading the line there are still empty new lines.
         *          The returned position value escapes followed empty new lines.
         * \param[out]  strResult   On output the string contains a line.
         * \param[in]   startPos    The valid zero-based position to start copying the line. By default it starts from the begging of string.
         * \return  Returns next position where starts non-empty new line or areg::END_POS if reached end of string.
         **/
        inline areg::CharPos readLine(StringBase<CharType>& strResult, areg::CharPos startPos = areg::START_POS) const;
        areg::CharPos readLine(std::basic_string<CharType>& strResult, areg::CharPos startPos = areg::START_POS) const;

        /**
         * \brief   In the existing string removes all characters, which are not alphanumeric.
         * \return  Returns new length of string after making alphanumeric
         **/
        inline StringBase<CharType>& makeAlphanumeric();

        /**
         * \brief   Checks and returns true if the string starts with the given phrase.
         *          Valid only the first 256 characters based on UTF-8 code page.
         * \param phrase            The phrase to check.
         * \param isCaseSensitive   If false, ignores the upper and lower cases.
         **/
        inline bool startsWith(const StringBase<CharType>& phrase, bool isCaseSensitive = true) const;

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
         * \param count             The number of character in the phase. If areg::COUNT_ALL it compare the complete phase.
         **/
        inline bool startsWith(const CharType* phrase, bool isCaseSensitive = true, areg::CharCount count = areg::COUNT_ALL) const;

        /**
         * \brief   Checks and returns true if the string ends with the given phrase.
         *          Valid only the first 256 characters based on UTF-8 code page.
         * \param phrase            The phrase to check.
         * \param isCaseSensitive   If false, ignores the upper and lower cases.
         **/
        inline bool endsWith(const StringBase<CharType>& phrase, bool isCaseSensitive = true) const;

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
         * \param count             The number of character in the phase. If areg::COUNT_ALL it compare the complete phase.
         **/
        inline bool endsWith(const CharType* phrase, bool isCaseSensitive = true, areg::CharCount count = areg::COUNT_ALL) const;

        /**
         * \brief   Searches string and if found, replace by another.
         * \param   strOrigin   The string to search.
         * \param   lenOrigin   The length of string to search or amount of characters to search.
         * \param   strReplace  The string to replace
         * \param   lenReplace  The amount of characters to replace
         * \param   startPos    The starting position to start searching.
         * \return  Returns next position after replacing string. Returns areg::INVALID_POS if could not find specified string.
         **/
        inline areg::CharPos replaceWith( const CharType * strOrigin
                                            , areg::CharCount lenOrigin
                                            , const CharType * strReplace
                                            , areg::CharCount lenReplace
                                            , areg::CharPos startPos);

        /**
         * \brief   Replaces string in the buffer starting at specified position.
         * \param   startPos    The position in string buffer to start replacing characters. Should be valid position for reading.
         * \param   count       The amount characters to replace is string buffer.
         * \param   strReplace  The string to replace
         * \param   lenReplace  The amount of characters to replace
         * \return  Returns next position after replacing string. Returns areg::INVALID_POS if could not find specified string.
         **/
        inline areg::CharPos replaceWith( areg::CharPos startPos
                                            , areg::CharCount count
                                            , const CharType * strReplace
                                            , areg::CharCount lenReplace);

        /**
         * \brief   Splits the given string into multiple parts considering specified delimiter.
         * \param   delim   The delimiter character that should be searched to split the string.
         * \return  Returns list of strings that are split be specified delimiter.
         **/
        inline std::vector<StringBase<CharType>> split(CharType delim) const;

        /**
         * \brief   Splits the given string into multiple parts considering specified delimiter.
         * \param   delim   The delimiter string that should be searched to split the string.
         * \return  Returns list of strings that are split be specified delimiter.
         **/
        inline std::vector<StringBase<CharType>> split(const StringBase<CharType> & delim) const;

    /************************************************************************/
    // Protected methods, can be assessed only from derived class
    /************************************************************************/
    protected:

        /**
         * \brief   Compares the existing string at the specified valid zero-based position with another string, and returns:
         *              - areg::Smaller   if string is less than given string
         *              - areg::Equal     if strings have equal
         *              - areg::Bigger    if string is more than given string.
         *          The comparing can be done by ignoring case sensitivity.
         *
         * \param   startPos        The Valid zero-based position in the string buffer to start comparing.
         * \param   strOther        The string to compare
         * \param   count           The amount of characters to compare or areg::COUNT_ALL to compare the complete string.
         * \param   caseSensitive   If true, compares exact match of string. Otherwise, ignores lower and upper cases.
         * \return  Return:
         *              areg::Smaller if string is less than given string
         *              areg::Equal   if strings have equal
         *              areg::Bigger  if string is more than given string
         **/
        inline areg::Ordering compareString( areg::CharPos startPos, const CharType * strOther, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true ) const;

        /**
         * \brief   Compares the existing string at the specified valid zero-based position with another string, and returns:
         *              - areg::Smaller   if string is less than given string
         *              - areg::Equal     if strings have equal
         *              - areg::Bigger    if string is more than given string.
         *          The comparing is done by exact match.
         *
         * \param   startPos        The Valid zero-based position in the string buffer to start comparing.
         * \param   strOther        The string to compare
         * \param   count           The amount of characters to compare or areg::COUNT_ALL to compare the complete string.
         * \return  Return:
         *              areg::Smaller if string is less than given string
         *              areg::Equal   if strings have equal
         *              areg::Bigger  if string is more than given string
         **/
        inline areg::Ordering compareStringExact(areg::CharPos startPos, const CharType* strOther, areg::CharCount count = areg::COUNT_ALL) const;

        /**
         * \brief   Compares the existing string at the specified valid zero-based position with another string, and returns:
         *              - areg::Smaller   if string is less than given string
         *              - areg::Equal     if strings have equal
         *              - areg::Bigger    if string is more than given string.
         *          The comparing is done by ignoring upper / lower case.
         *
         * \param   startPos        The Valid zero-based position in the string buffer to start comparing.
         * \param   strOther        The string to compare
         * \param   count           The amount of characters to compare or areg::COUNT_ALL to compare the complete string.
         * \return  Return:
         *              areg::Smaller if string is less than given string
         *              areg::Equal   if strings have equal
         *              areg::Bigger  if string is more than given string
         **/
        inline areg::Ordering compareStringIgnoreCase(areg::CharPos startPos, const CharType * strOther, areg::CharCount count = areg::COUNT_ALL) const;

        /**
         * \brief   Searches the first phrase in string. The comparing is done by exact match.
         *
         * \param   phrase      The phrase to search.
         * \param   count       The number of characters in the phrase. The value areg::COUNT_ALL searches the complete phrase.
         * \param   startPos    The valid zero-based position in the string to start searching.
         * \return  If found, returns valid position in the string. If not found, it returns areg::END_POS.
         **/
        inline areg::CharPos findFirstPhrase(const CharType* phrase, areg::CharCount count = areg::COUNT_ALL, areg::CharPos startPos = areg::START_POS ) const;

        /**
         * \brief   Searches the first phrase in string. The comparing is done by ignoring upper and lower cases.
         *          It compares upper and lower cases by default locale.
         *
         * \param   phrase      The phrase to search.
         * \param   startPos    The valid zero-based position in the string to start searching.
         * \return  If found, returns valid position in the string. If not found, it returns areg::END_POS.
         **/
        inline areg::CharPos findFirstPhraseIgnoreCase(const CharType* phrase, areg::CharPos startPos = areg::START_POS) const;
        inline areg::CharPos findFirstPhraseIgnoreCase(const std::basic_string<CharType>& phrase, areg::CharPos startPos = areg::START_POS) const;

        /**
         * \brief   Searches the first match of the entire word in string. The comparing can be done by exact match of ignoring upper and lower cases.
         *          It compares upper and lower cases by default locale.
         *
         * \param   word            The word to search.
         * \param   caseSensitive   If true, it searches by exact match. Otherwise, ignores upper and lower cases.
         * \param   startPos        The valid zero-based position in the string to start searching.
         * \return  If found, returns valid position in the string. If not found, it returns areg::END_POS.
         **/
        inline areg::CharPos findFirstWord(const CharType* word, bool caseSensitive, areg::CharPos startPos = areg::START_POS) const;
        inline areg::CharPos findFirstWord(const std::basic_string<CharType>& word, bool caseSensitive, areg::CharPos startPos = areg::START_POS) const;

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
        inline bool _hasPhrase(const CharType* fullString, const CharType* phrase, areg::CharCount count, const char* locale) const;

    //////////////////////////////////////////////////////////////////////////
    // Protected member variables
    //////////////////////////////////////////////////////////////////////////
    protected:
        //! The string object
        std::basic_string<CharType> mData;
    };


    //////////////////////////////////////////////////////////////////////////
    // StringBase class template implementation
    //////////////////////////////////////////////////////////////////////////

    template<typename CharType>
    inline StringBase<CharType>::StringBase()
        : mData  (&EmptyChar)
    {
    }

    template<typename CharType>
    inline StringBase<CharType>::StringBase(const CharType source)
        : mData  ( &source, 1 )
    {
    }

    template<typename CharType>
    inline StringBase<CharType>::StringBase(const CharType* source)
        : mData  ( source != nullptr ? source : &EmptyChar )
    {
    }

    template<typename CharType>
    inline StringBase<CharType>::StringBase(const std::basic_string<CharType>& source)
        : mData  ( source )
    {
    }

    template<typename CharType>
    inline StringBase<CharType>::StringBase(std::basic_string<CharType>&& source) noexcept
        : mData  ( std::move(source) )
    {
    }

    template<typename CharType>
    inline StringBase<CharType>::StringBase(const std::basic_string_view<CharType>& source)
        : mData  ( source.data(), source.length() )
    {
    }

    template<typename CharType>
    inline StringBase<CharType>::StringBase(const CharType* source, areg::CharCount count)
        : mData  ( source != nullptr ? source : &EmptyChar, static_cast<uint32_t>(count != areg::COUNT_ALL ? count : areg::getStringLength(source)) )
    {
    }

    template<typename CharType>
    inline StringBase<CharType>::StringBase(const StringBase<CharType>& source)
        : mData  ( source.mData )
    {
    }

    template<typename CharType>
    inline StringBase<CharType>::StringBase(StringBase<CharType>&& source) noexcept
        : mData  ( std::move(source.mData) )
    {
    }

    template<typename CharType>
    inline StringBase<CharType>::StringBase(uint32_t count)
        : mData  ( )
    {
        mData.reserve(count);
    }

    template<typename CharType>
    inline StringBase<CharType>::operator const CharType* () const
    {
        return mData.c_str();
    }

    template<typename CharType>
    inline const CharType StringBase<CharType>::operator[ ](int32_t index) const
    {
        ASSERT((mData.c_str() != nullptr) && (index < static_cast<int32_t>(mData.length())));
        return mData.c_str()[index];
    }


    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator = (const StringBase<CharType>& src)
    {
        mData = src.mData;
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator = (const std::basic_string<CharType>& src)
    {
        mData = src;
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator = (const std::basic_string_view<CharType>& src)
    {
        mData.assign( src.data( ) );
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator = (const CharType* src)
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
    inline StringBase<CharType>& StringBase<CharType>::operator = (const CharType chSource)
    {
        mData = chSource;
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator = (std::basic_string<CharType>&& src) noexcept
    {
        mData = std::move(src);
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator = (StringBase<CharType>&& src) noexcept
    {
        mData = std::move(src.mData);
        return (*this);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator == (const StringBase<CharType>& other) const
    {
        return (mData == other.mData);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator == (const std::basic_string<CharType>& other) const
    {
        return (mData == other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator == (const std::basic_string_view<CharType>& other) const
    {
        return (mData == other.data());
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator == (const CharType* other) const
    {
        return (mData == other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator == ( const CharType ch) const
    {
        return ((mData.length() == 1) && (mData[0] == ch));
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator != (const StringBase<CharType>& other) const
    {
        return (mData != other.mData);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator != (const std::basic_string<CharType>& other) const
    {
        return (mData != other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator != (const std::basic_string_view<CharType>& other) const
    {
        return (mData != other.data());
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator != (const CharType* other) const
    {
        return (mData != other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator != ( const CharType ch ) const
    {
        return ((mData.length( ) != 1) || (mData[0] != ch));
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator > (const StringBase<CharType>& other) const
    {
        return (mData > other.mData);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator > (const std::basic_string<CharType>& other) const
    {
        return (mData > other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator > (const std::basic_string_view<CharType>& other) const
    {
        return (mData > other.data());
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator > (const CharType* other) const
    {
        return (mData > other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator < (const StringBase<CharType>& other) const
    {
        return (mData < other.mData);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator < (const std::basic_string<CharType>& other) const
    {
        return (mData < other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator < (const std::basic_string_view<CharType>& other) const
    {
        return (mData < other.data());
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator < (const CharType* other) const
    {
        return (mData < other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator >= (const StringBase<CharType>& other) const
    {
        return (mData >= other.mData);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator >= (const std::basic_string<CharType>& other) const
    {
        return (mData >= other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator >= (const std::basic_string_view<CharType>& other) const
    {
        return (mData >= other.data());
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator >= (const CharType* other) const
    {
        return (mData >= other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator <= (const StringBase<CharType>& other) const
    {
        return (mData <= other.mData);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator <= (const std::basic_string<CharType>& other) const
    {
        return (mData <= other);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator <= (const std::basic_string_view<CharType>& other) const
    {
        return (mData <= other.data());
    }

    template<typename CharType>
    inline bool StringBase<CharType>::operator <= (const CharType* other) const
    {
        return (mData <= other);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator += (const StringBase<CharType>& src)
    {
        mData += src.mData;
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator += (const std::basic_string<CharType>& src)
    {
        mData += src;
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator += (const std::basic_string_view<CharType>& src)
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
    inline StringBase<CharType>& StringBase<CharType>::operator += (const CharType* src)
    {
        if (areg::isEmpty(src) == false)
            mData += src;
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::operator += (const CharType chSource)
    {
        mData += chSource;
        return (*this);
    }

    //////////////////////////////////////////////////////////////////////////
    // Attributes and operations
    //////////////////////////////////////////////////////////////////////////

    template<typename CharType>
    inline bool StringBase<CharType>::validate(const CharType* validityList) const
    {
        if (mData.empty() || areg::isEmpty<CharType>(validityList))
            return false;

        const CharType* src = mData.c_str();
        while (*src != EmptyChar)
        {
            if (areg::findFirst<CharType>(*src, validityList) == areg::INVALID_POS)
                break;

            ++src;
        }

        return (*src == EmptyChar);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::isNumeric(bool signIgnore /*= true*/) const
    {
        bool result = false;
        const CharType* src = mData.c_str();

        // escape if first symbol is a sign
        if (signIgnore && ((*src == '-') || (*src == '+')))
            ++src; // Escape sign, if it should.

        // make sure the string is not empty
        if (*src != EmptyChar)
        {
            while (std::isdigit(static_cast<int>(*src)) != 0)
                ++src;

            result = *src == EmptyChar;
        }

        return result;
    }

    template<typename CharType>
    inline bool StringBase<CharType>::isAlphanumeric() const
    {
        const CharType* src = mData.c_str();
        while (std::isalnum(static_cast<int>(*src)) != 0)
            ++src;

        return (*src == EmptyChar); // reached end of the string.
    }

    template<typename CharType>
    inline bool StringBase<CharType>::isValidNameChar(const CharType checkChar) const
    {
        // initialize list of symbols for the valid names.
        constexpr CharType symbols[] = { '_', '\0'};
        return (std::isalnum(static_cast<int32_t>(checkChar)) != 0) || areg::isOneOf<CharType>(checkChar, symbols);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::isValidName() const
    {
        if (mData.empty())
            return false;

        const CharType* src = mData.c_str();
        while (isValidNameChar(*src))
            ++src;

        return (*src == EmptyChar);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::isEmpty() const
    {
        return mData.empty();
    }

    template<typename CharType>
    inline areg::CharCount StringBase<CharType>::getLength() const
    {
        return static_cast<areg::CharCount>(mData.length());
    }

    template<typename CharType>
    inline areg::CharCount StringBase<CharType>::getCapacity() const
    {
        return static_cast<areg::CharCount>(mData.capacity());
    }

    template<typename CharType>
    inline uint32_t StringBase<CharType>::getSpace() const
    {
        return static_cast<uint32_t>((mData.length() + 1) * sizeof(CharType));
    }

    template<typename CharType>
    inline const CharType* StringBase<CharType>::getBuffer(areg::CharPos startAt /*= areg::START_POS*/) const
    {
        areg::CharCount len = static_cast<areg::CharCount>(mData.size());
        startAt = (startAt == areg::END_POS) || (startAt > len) ? len : startAt;
        return (mData.c_str() + startAt);
    }

    template<typename CharType>
    inline CharType* StringBase<CharType>::getBuffer(areg::CharPos startAt /*= areg::START_POS*/)
    {
        areg::CharCount len = static_cast<areg::CharCount>(mData.size());
        startAt = (startAt == areg::END_POS) || (startAt > len) ? len : startAt;
        return (mData.data() + startAt);
    }

    template<typename CharType>
    inline const CharType* StringBase<CharType>::getString() const
    {
        return mData.c_str();
    }

    template<typename CharType>
    inline const std::basic_string<CharType>& StringBase<CharType>::getData() const
    {
        return mData;
    }

    template<typename CharType>
    inline bool StringBase<CharType>::isValidPosition(areg::CharPos pos) const
    {
        return (pos >= areg::START_POS) && (pos < static_cast<areg::CharPos>(mData.length()));
    }

    template<typename CharType>
    inline bool StringBase<CharType>::isInvalidPosition(areg::CharPos pos) const
    {
        return (pos < areg::START_POS) || (pos > static_cast<areg::CharPos>(mData.length()));
    }

    template<typename CharType>
    inline bool StringBase<CharType>::isLastPosition(areg::CharPos pos) const
    {
        return (pos == areg::END_POS) || (pos >= static_cast<areg::CharPos>(mData.length()));
    }

    template<typename CharType>
    inline bool StringBase<CharType>::isFirstPosition(areg::CharPos pos) const
    {
        return (mData.empty() == false) && (pos == areg::START_POS);
    }

    template<typename CharType>
    inline void StringBase<CharType>::clear()
    {
        mData.clear();
    }

    template<typename CharType>
    inline void StringBase<CharType>::freeExtra()
    {
        mData.shrink_to_fit();
    }

    template<typename CharType>
    inline void StringBase<CharType>::release()
    {
        mData.clear();
        mData.shrink_to_fit();
    }

    template<typename CharType>
    void StringBase<CharType>::findWord(areg::CharPos& startAt, areg::CharPos& endAt) const
    {
        areg::CharPos pos = startAt;

        if (isInvalidPosition(pos) || mData.empty())
        {
            endAt = areg::INVALID_POS;
            startAt = areg::END_POS;
            return; // invalid position, return nothing
        }

        if (startAt != areg::START_POS)
        {
            endAt = areg::INVALID_POS;
            startAt = areg::END_POS;
        }
        else
        {
            endAt = areg::END_POS;
        }

        const CharType* buffer = getBuffer(pos);
        while ((*buffer != EmptyChar) && isValidNameChar(*buffer) == false)
            ++buffer;   // escape invalid chars at the begin

        if (*buffer == EmptyChar)
            return; // reached EOS, do nothing.

        if (startAt != areg::START_POS)
        {
            // Remember the position of starting valid word.
            startAt = static_cast<areg::CharPos>(getString() - buffer);
            while (isValidNameChar(*buffer))
                ++buffer;   // escape invalid chars at the begin
        }

        // the word ends at position.
        endAt = *buffer != EmptyChar ? static_cast<areg::CharPos>(getString() - buffer) : areg::END_POS;
    }

    template<typename CharType>
    void StringBase<CharType>::getWord(StringBase<CharType>& word, areg::CharPos& startAt, areg::CharPos& endAt) const
    {
        findWord(startAt, endAt);
        if (startAt != areg::END_POS)
        {
            ASSERT(endAt != areg::INVALID_POS);
            substring(word, startAt, static_cast<areg::CharCount>(endAt == areg::END_POS ? areg::COUNT_ALL : endAt - startAt));
        }
        else
        {
            word.clear();
        }
    }

    template<typename CharType>
    areg::CharPos StringBase<CharType>::findOneOf( const CharType* chars, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        if (isInvalidPosition(startPos) || areg::isEmpty<CharType>(chars))
            return areg::INVALID_POS;

        const CharType* strBegin = getBuffer(startPos);
        while (*strBegin != EmptyChar)
        {
            if (areg::isOneOf(*strBegin, chars))
            {
                return static_cast<areg::CharPos>(strBegin - getString());
            }

            ++strBegin;
        }

        return areg::END_POS;
    }

    template<typename CharType>
    areg::CharPos StringBase<CharType>::findFirst( CharType chSearch
                                                , areg::CharPos startPos /*= areg::START_POS*/
                                                , bool caseSensitive /*= true*/) const
    {
        if (isValidPosition(startPos) == false)
            return areg::INVALID_POS;

        const CharType* str = getBuffer(startPos);
        CharType chUpper = caseSensitive ? chSearch : static_cast<CharType>(std::toupper(static_cast<int32_t>(chSearch)));
        CharType chLower = caseSensitive ? chSearch : static_cast<CharType>(std::tolower(static_cast<int32_t>(chSearch)));
    
        while ((*str != EmptyChar) && (*str != chUpper) && (*str != chLower))
        {
            ++str;
        }

        return (*str != EmptyChar ? static_cast<areg::CharPos>(str - getString()) : areg::END_POS);
    }

    template<typename CharType>
    areg::CharPos StringBase<CharType>::findFirst( const CharType* phrase
                                                , areg::CharPos startPos /*= areg::START_POS*/
                                                , bool caseSensitive   /*= true*/
                                                , bool wholeWord       /*= false*/) const
    {
        if (isInvalidPosition(startPos) || areg::isEmpty<CharType>(phrase))
            return areg::INVALID_POS;

        if (caseSensitive && !wholeWord)
        {
            return findFirstPhrase(phrase, areg::COUNT_ALL, startPos);
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
    areg::CharPos StringBase<CharType>::findFirst( const StringBase<CharType> & phrase
                                                , areg::CharPos startPos /*= areg::START_POS*/
                                                , bool caseSensitive         /*= true*/
                                                , bool wholeWord             /*= false*/) const
    {
        if (isInvalidPosition(startPos) || areg::isEmpty<CharType>(phrase))
            return areg::INVALID_POS;

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
    areg::CharPos StringBase<CharType>::findLast(CharType chSearch, areg::CharPos startPos /*= areg::END_POS*/, bool caseSensitive /*= true*/) const
    {
        if (mData.empty())
            return areg::INVALID_POS;

        startPos = startPos == areg::END_POS ? getLength() - 1 : startPos;

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
            CharType chUpper = static_cast<CharType>(std::toupper(static_cast<int32_t>(chSearch)));
            CharType chLower = static_cast<CharType>(std::tolower(static_cast<int32_t>(chSearch)));

            while ((end >= begin) && (*end != chUpper) && (*end != chLower))
            {
                --end;
            }
        }

        return (end >= begin ? static_cast<areg::CharPos>(end - begin) : areg::END_POS);
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::findLast(const CharType* phrase, areg::CharCount phraseCount, areg::CharPos startPos, bool caseSensitive) const
    {
        if (((startPos != areg::END_POS) && isInvalidPosition(startPos)) || areg::isEmpty<CharType>(phrase) || (phraseCount == 0))
            return areg::INVALID_POS;

        areg::CharPos result{ areg::END_POS };
        areg::CharCount count = phraseCount > 0 ? phraseCount : areg::getStringLength<CharType>(phrase);
        areg::CharCount strLen = getLength();

        startPos = (startPos == areg::END_POS) && (strLen >= count) ? strLen - 1 - count : 0;
        for (areg::CharPos pos = startPos; pos >= 0; --pos)
        {
            if ((compareString(pos, phrase, count, caseSensitive) == areg::Ordering::Equal))
            {
                result = pos;
                break;
            }
        }

        return result;
    }

    template<typename CharType>
    areg::CharPos StringBase<CharType>::findLast(const CharType* phrase, areg::CharPos startPos /*= areg::END_POS*/, bool caseSensitive /*= true*/) const
    {
        return findLast(phrase, areg::getStringLength<CharType>(phrase), startPos, caseSensitive);
    }

    template<typename CharType>
    areg::CharPos StringBase<CharType>::findLast(const StringBase<CharType> & phrase, areg::CharPos startPos /*= areg::END_POS*/, bool caseSensitive /*= true*/) const
    {
        return findLast(phrase, phrase.getLength(), startPos, caseSensitive);
    }

    template<typename CharType>
    areg::Ordering StringBase<CharType>::compare(const CharType* what, areg::CharPos startAt /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/, bool caseSensitive /*= true*/) const
    {
        areg::Ordering result{ areg::Ordering::Smaller };
        if (isValidPosition(startAt) == false)
            return result;

        if (count == areg::COUNT_ALL)
        {
            count = areg::getStringLength<CharType>(what);
        }

        areg::CharPos length = static_cast<areg::CharPos>(mData.length()) - startAt;
        if ((length == count) && (what != nullptr))
        {
            const CharType* current = getBuffer(startAt);
            const CharType* other = what;

            result = areg::Ordering::Equal;

            CharType ch1{ EmptyChar };
            CharType ch2{ EmptyChar };
            do
            {
                ch1 = *current++;
                ch2 = *other++;
                if (caseSensitive == false)
                {
                    ch1 = static_cast<CharType>(std::tolower(static_cast<int32_t>(ch1)));
                    ch2 = static_cast<CharType>(std::tolower(static_cast<int32_t>(ch2)));
                }

                if (ch1 != ch2)
                {
                    break;
                }

            } while (ch1 != EmptyChar);


            if (ch1 < ch2)
                result = areg::Ordering::Smaller;
            else if (ch1 > ch2)
                result = areg::Ordering::Bigger;
        }
        else if (length > count)
        {
            result = areg::Ordering::Bigger;
        }

        return result;
    }

    template<typename CharType>
    inline areg::Ordering StringBase<CharType>::compare(const StringBase<CharType>& other, bool caseSensitive /*= true*/) const
    {
        return compare(other.mData, caseSensitive);
    }

    template<typename CharType>
    inline areg::Ordering StringBase<CharType>::compare(const std::basic_string<CharType>& other, bool caseSensitive /*= true*/) const
    {
        return compare(other.c_str(), areg::START_POS, static_cast<areg::CharCount>(other.length()), caseSensitive);
    }

    template<typename CharType>
    inline areg::Ordering StringBase<CharType>::compare(const std::basic_string_view<CharType>& other, bool caseSensitive /*= true*/) const
    {
        return compare(other.data(), areg::START_POS, static_cast<areg::CharCount>(other.length()), caseSensitive);
    }

    template<typename CharType>
    inline areg::Ordering StringBase<CharType>::compare( const StringBase<CharType>& other
                                                    , areg::CharPos startPos /*= areg::START_POS*/
                                                    , areg::CharCount count  /*= areg::COUNT_ALL*/
                                                    , bool caseSensitive         /*= true*/) const
    {
        return compare(other.mData, startPos, count, caseSensitive);
    }

    template<typename CharType>
    inline areg::Ordering StringBase<CharType>::compare( const std::basic_string<CharType>& other
                                                    , areg::CharPos startAt  /*= areg::START_POS*/
                                                    , areg::CharCount count  /*= areg::COUNT_ALL*/
                                                    , bool caseSensitive         /*= true*/) const
    {
        return compare(other.c_str(), startAt, count, caseSensitive);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::substring(areg::CharPos startPos, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        mData = mData.substr(static_cast<uint32_t>(startPos), count == areg::COUNT_ALL ? std::basic_string<CharType>::npos : static_cast<uint32_t>(count));
        return (*this);
    }

    template<typename CharType>
    inline void StringBase<CharType>::substring(StringBase<CharType>& outResult, areg::CharPos startPos /* = areg::START_POS */, areg::CharCount count /*= areg::COUNT_ALL*/) const
    {
        if (isValidPosition(startPos))
        {
            outResult.mData = mData.substr(static_cast<uint32_t>(startPos), count == areg::COUNT_ALL ? std::basic_string<CharType>::npos : static_cast<uint32_t>(count));
        }
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::substring(StringBase<CharType>& outResult, const CharType* strPhrase, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        return substring(outResult, std::basic_string<CharType>(strPhrase == nullptr ? &EmptyChar : strPhrase), startPos);
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::substring(StringBase<CharType>& outResult, const StringBase<CharType>& strPhrase, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        return substring(outResult, strPhrase.mData, startPos);
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::substring(StringBase<CharType>& outResult, const std::basic_string<CharType>& strPhrase, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        outResult.mData.clear();

        if (isInvalidPosition(startPos) || strPhrase.empty())
        {
            return areg::INVALID_POS;
        }

        uint32_t len = static_cast<uint32_t>(strPhrase.length());
        uint32_t pos = static_cast<uint32_t>(mData.find(strPhrase, static_cast<uint32_t>(startPos)));

        if (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
        {
            uint32_t newCount = pos - static_cast<uint32_t>(startPos);
            outResult.mData = mData.substr(static_cast<uint32_t>(startPos), static_cast<uint32_t>(newCount));
            return static_cast<areg::CharPos>(pos + len);
        }
        else
        {
            outResult.mData = mData.substr(static_cast<uint32_t>(startPos));
            return areg::END_POS;
        }
    }

    template<typename CharType>
    areg::CharPos StringBase<CharType>::substring(StringBase<CharType>& outResult, CharType chSymbol, areg::CharPos startPos /* = areg::START_POS */) const
    {
        if (isInvalidPosition(startPos))
        {
            return areg::INVALID_POS;
        }

        uint32_t pos = static_cast<uint32_t>(mData.find(chSymbol, static_cast<uint32_t>(startPos)));
        if (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
        {
            uint32_t newCount = pos - static_cast<uint32_t>(startPos);
            outResult.mData = mData.substr(static_cast<uint32_t>(startPos), newCount);
            return static_cast<areg::CharPos>(pos + 1);
        }
        else
        {
            outResult.mData = mData.substr(static_cast<uint32_t>(startPos));
            return areg::END_POS;
        }
    }

    template<typename CharType>
    inline StringBase<CharType> StringBase<CharType>::leftSide(areg::CharCount charCount) const
    {
        StringBase<CharType> result;
        StringBase<CharType>::substring(result, areg::START_POS, charCount);
        return result;
    }

    template<typename CharType>
    inline StringBase<CharType> StringBase<CharType>::rightSide(areg::CharCount charCount) const
    {
        StringBase<CharType> result;

        areg::CharCount len = getLength();
        areg::CharPos pos = charCount < len ? len - charCount : areg::START_POS;
        substring(result, pos, areg::COUNT_ALL);

        return result;
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::assign(const CharType* source, areg::CharCount count /*= areg::COUNT_ALL */)
    {
        if ( source != nullptr )
        {
            mData.assign( source, static_cast<uint32_t>(count == areg::COUNT_ALL ? areg::getStringLength<CharType>( source ) : count) );
        }
        else
        {
            mData.clear( );
        }

        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::assign(const std::basic_string<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        mData.assign(source, static_cast<uint32_t>(pos), count == areg::COUNT_ALL ? std::basic_string<CharType>::npos : static_cast<uint32_t>(count));
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::assign(const std::basic_string_view<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        assign(source.data() + pos, count == areg::COUNT_ALL ? static_cast<areg::CharCount>(source.length() - static_cast<uint32_t>(pos)) : count);
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::assign(const StringBase<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        assign(source.mData, pos, count);
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::assign( CharType ch )
    {
        mData = ch;
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::append(const CharType* source, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        count = count == areg::COUNT_ALL ? areg::getStringLength<CharType>(source) : count;
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
    inline StringBase<CharType>& StringBase<CharType>::append( const std::basic_string<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        mData.append(source, static_cast<uint32_t>(pos), count == areg::COUNT_ALL ? std::basic_string<CharType>::npos : static_cast<uint32_t>(count));
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::append(const std::basic_string_view<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        append(source.data() + pos, count == areg::COUNT_ALL ? static_cast<areg::CharCount>(source.length() - static_cast<uint32_t>(pos)) : count);
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::append( const StringBase<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        return append(source.mData, pos, count);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::append( CharType ch )
    {
        mData += ch;
        return (*this);
    }

    template<typename CharType>
    StringBase<CharType>& StringBase<CharType>::moveTo(areg::CharPos startPos, int32_t moveTo)
    {
        if (moveTo < 0)
        {
            areg::CharCount count = static_cast<areg::CharCount>(-1 * moveTo);
            areg::CharPos dstPos  = startPos >= count ? startPos + moveTo : areg::START_POS;
            mData.erase(static_cast<uint32_t>(dstPos), static_cast<uint32_t>(count));
        }
        else if (moveTo > 0)
        {
            mData.insert(static_cast<uint32_t>(startPos), static_cast<uint32_t>(moveTo), '@');
        }

        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::insertAt(CharType source, areg::CharPos atPos, areg::CharCount count /*= 1*/)
    {
        mData.insert(static_cast<uint32_t>(atPos), static_cast<uint32_t>(count), source);
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::insertAt(const CharType* source, areg::CharPos atPos, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        if (source != nullptr && isValidPosition(atPos))
        {
            mData.insert(static_cast<uint32_t>(atPos), source, count == areg::COUNT_ALL ? static_cast<uint32_t>(areg::getStringLength(source)) : static_cast<uint32_t>(count));
        }

        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::insertAt( const std::basic_string<CharType>& source
                                                        , areg::CharPos atDstPos
                                                        , areg::CharPos atSrcPos /*= areg::START_POS*/
                                                        , areg::CharCount count  /*= areg::COUNT_ALL*/)
    {
        if (isValidPosition(atDstPos) && (atSrcPos < static_cast<areg::CharPos>(source.length())))
        {
            count = count == areg::COUNT_ALL ? static_cast<areg::CharPos>(source.length() - static_cast<uint32_t>(atSrcPos)) : count;
            mData.insert(static_cast<uint32_t>(atDstPos), source, static_cast<uint32_t>(atSrcPos), static_cast<uint32_t>(count));
        }

        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::insertAt( const std::basic_string_view<CharType>& source
                                                        , areg::CharPos atDstPos
                                                        , areg::CharPos atSrcPos /*= areg::START_POS*/
                                                        , areg::CharCount count  /*= areg::COUNT_ALL*/)
    {
        if (isValidPosition(atDstPos) && (atSrcPos < static_cast<areg::CharPos>(source.length())))
        {
            count = count == areg::COUNT_ALL ? static_cast<areg::CharPos>(source.length() - static_cast<uint32_t>(atSrcPos)) : count;
            mData.insert(static_cast<uint32_t>(atDstPos), source, static_cast<uint32_t>(atSrcPos), static_cast<uint32_t>(count));
        }

        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::insertAt( const StringBase<CharType>& source
                                                        , areg::CharPos atDstPos
                                                        , areg::CharPos atSrcPos /*= areg::START_POS*/
                                                        , areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        return insertAt(source.mData, atDstPos, atSrcPos, count);
    }

    template<typename CharType>
    StringBase<CharType>& StringBase<CharType>::replace(CharType chSearch, CharType chReplace, areg::CharPos startPos /*= areg::START_POS*/, bool replaceAll /*= true*/)
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
    StringBase<CharType>& StringBase<CharType>::replace( const CharType* strSearch
                                                , const CharType* strReplace
                                                , areg::CharPos startPos   /*= areg::START_POS*/
                                                , areg::CharCount count    /*= areg::COUNT_ALL*/
                                                , bool replaceAll              /*= true*/)
    {
        if (isValidPosition(startPos) && (areg::isEmpty(strSearch) == false))
        {
            areg::CharPos lenSearch  = areg::getStringLength<CharType>(strSearch);
            areg::CharPos lenReplace = areg::getStringLength<CharType>(strReplace);
            count       = (count == areg::COUNT_ALL) || (count > static_cast<areg::CharCount>(lenReplace)) ? lenReplace : count;
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
    inline StringBase<CharType>& StringBase<CharType>::replace( const std::basic_string_view<CharType>& strSearch
                                                        , const std::basic_string_view<CharType>& strReplace
                                                        , areg::CharPos startPos  /*= areg::START_POS*/
                                                        , bool replaceAll             /*= true*/)
    {
        if (isValidPosition(startPos) && (strSearch.empty() == false))
        {
            areg::CharPos lenSearch  = static_cast<areg::CharPos>(strSearch.length());
            areg::CharPos lenReplace = static_cast<areg::CharPos>(strReplace.length());
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
    inline StringBase<CharType>& StringBase<CharType>::replace( const StringBase<CharType>& strSearch
                                                        , const StringBase<CharType>& strReplace
                                                        , areg::CharPos startPos /*= areg::START_POS*/
                                                        , bool replaceAll /*= true*/)
    {
        return replace(strSearch.mData, strReplace.mData, startPos, replaceAll);
    }

    template<typename CharType>
    StringBase<CharType>& StringBase<CharType>::replace( const std::basic_string<CharType>& strSearch
                                                , const std::basic_string<CharType>& strReplace
                                                , areg::CharPos startPos /*= areg::START_POS*/
                                                , bool replaceAll /*= true*/)
    {
        if (isValidPosition(startPos) && (strSearch.empty() == false))
        {
            areg::CharPos lenSearch = static_cast<areg::CharPos>(strSearch.length());
            areg::CharPos lenReplace = static_cast<areg::CharPos>(strReplace.length());
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
    inline StringBase<CharType>& StringBase<CharType>::replace( areg::CharPos startPos, areg::CharCount charsRemove, const CharType* strReplace, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        if (isValidPosition(startPos))
        {
            areg::CharPos lenReplace = areg::getStringLength<CharType>(strReplace);
            mData.replace(static_cast<uint32_t>(startPos), static_cast<uint32_t>(charsRemove), strReplace, (count == areg::COUNT_ALL) || (count > lenReplace) ? static_cast<uint32_t>(lenReplace) : static_cast<uint32_t>(count));
        }

        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::replace( areg::CharPos startPos, areg::CharCount charsRemove, const StringBase<CharType>& strReplace)
    {
        return replace(startPos, charsRemove, strReplace.mData);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::replace( areg::CharPos startPos, areg::CharCount charsRemove, const std::basic_string<CharType>& strReplace)
    {
        if (isValidPosition(startPos))
        {
            mData.replace(static_cast<uint32_t>(startPos), static_cast<uint32_t>(charsRemove), strReplace);
        }

        return (*this);
    }

    template<typename CharType>
    StringBase<CharType>& StringBase<CharType>::remove(areg::CharPos startPos, areg::CharCount count /*= areg::COUNT_ALL*/)
    {
        if (isValidPosition(startPos))
        {
            if (count == areg::COUNT_ALL)
            {
                mData.erase(static_cast<uint32_t>(startPos));
            }
            else if (count <= (static_cast<areg::CharCount>(mData.length()) - startPos))
            {
                mData.erase(static_cast<uint32_t>(startPos), static_cast<uint32_t>(count));
            }
        }

        return (*this);
    }

    template<typename CharType>
    StringBase<CharType>& StringBase<CharType>::remove(const CharType chRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
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
    inline StringBase<CharType>& StringBase<CharType>::remove( const CharType* strRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
    {
        if (areg::isEmpty<CharType>(strRemove) == false)
            remove(std::basic_string<CharType>(strRemove), startPos, removeAll);

        return (*this);
    }

    template<typename CharType>
    StringBase<CharType>& StringBase<CharType>::remove( const std::basic_string<CharType>& strRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
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
    inline StringBase<CharType>& StringBase<CharType>::remove( const StringBase<CharType>& strRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
    {
        return remove(strRemove.mData, startPos, removeAll);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::resize(areg::CharCount count, CharType chFill /*= static_cast<CharType>('\0')*/)
    {
        mData.resize(static_cast<uint32_t>(count), chFill);
        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::truncate(areg::CharCount maxChars)
    {
        if (maxChars == 0)
        {
            mData.clear();
        }
        else if (maxChars < static_cast<areg::CharCount>(mData.length()))
        {
            mData.erase(static_cast<uint32_t>(maxChars));
        }

        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::reserve(areg::CharCount newCapacity)
    {
        mData.reserve( static_cast<uint32_t>(newCapacity));
        return (*this);
    }

    template<typename CharType>
    inline CharType StringBase<CharType>::getAt(areg::CharPos atPos) const
    {
        return (isValidPosition(atPos) ? mData.at(static_cast<uint32_t>(atPos)) : static_cast<CharType>(areg::EndOfString));
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::setAt(CharType ch, areg::CharPos atPos /*= areg::END_POS*/)
    {
        if ((atPos >= areg::START_POS) && (atPos < static_cast<areg::CharPos>(mData.size())))
        {
            mData.at(static_cast<uint32_t>(atPos)) = ch;
        }
        else if (atPos == areg::END_POS)
        {
            mData.append(1, ch);
        }

        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::trimLeft()
    {
        if (mData.empty() == false)
        {
            uint32_t count = 0;
            for (const auto& ch : mData)
            {
                if (std::isspace(static_cast<int32_t>(ch)) == 0)
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
    inline void StringBase<CharType>::trimLeft(StringBase<CharType>& strResult) const
    {
        trimLeft(strResult.mData);
    }

    template<typename CharType>
    inline void StringBase<CharType>::trimLeft(std::basic_string<CharType>& strResult) const
    {
        strResult.clear();
        if (mData.empty() == false)
        {
            uint32_t count = 0;
            for (const auto& ch : mData)
            {
                if (std::isspace(static_cast<int32_t>(ch)) == 0)
                    break;

                ++count;
            }

            if (count != 0)
            {
                strResult.assign(getBuffer(static_cast<areg::CharCount>(count)), static_cast<uint32_t>(strResult.length() - count));
            }
        }
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::trimRight()
    {
        if (mData.empty() == false)
        {
            uint32_t count = 0;
            for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
            {
                if (std::isspace(static_cast<int32_t>(*cit)) == 0)
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
    inline void StringBase<CharType>::trimRight(StringBase<CharType>& strResult) const
    {
        trimRight(strResult.mData);
    }

    template<typename CharType>
    inline void StringBase<CharType>::trimRight(std::basic_string<CharType>& strResult) const
    {
        strResult.clear();
        if (mData.empty() == false)
        {
            uint32_t count = 0;
            for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
            {
                int32_t ch = static_cast<int32_t>(*cit);
                if ((ch <= 0xFF) && (std::isspace(ch) == 0))
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
    inline StringBase<CharType>& StringBase<CharType>::trimAll()
    {
        if (mData.empty() == false)
        {
            uint32_t length = static_cast<uint32_t>(mData.length());

            uint32_t left = 0;
            for (auto cit = mData.cbegin(); cit != mData.cend(); ++cit)
            {
                int32_t ch = static_cast<int32_t>(*cit);
                if ((ch <= 0xFF) && (std::isspace(ch) == 0))
                    break;

                ++left;
            }

            uint32_t right = 0;
            for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
            {
                int32_t ch = static_cast<int32_t>(*cit);
                if ((ch <= 0xFF) && (std::isspace(ch) == 0))
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
    inline void StringBase<CharType>::trimAll(StringBase<CharType>& strResult) const
    {
        trimAll(strResult.mData);
    }

    template<typename CharType>
    inline void StringBase<CharType>::trimAll(std::basic_string<CharType>& strResult) const
    {
        strResult.clear();
        if (mData.empty() == false)
        {
            uint32_t length = static_cast<uint32_t>(mData.length());

            uint32_t left = 0;
            for (auto cit = mData.cbegin(); cit != mData.cend(); ++cit)
            {
                int32_t ch = static_cast<int32_t>(*cit);
                if ((ch <= 0xFF) && (std::isspace(ch) == 0))
                    break;

                ++left;
            }

            uint32_t right = 0;
            for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
            {
                int32_t ch = static_cast<int32_t>(*cit);
                if ((ch <= 0xFF) && (std::isspace(ch) == 0))
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
    inline StringBase<CharType> & StringBase<CharType>::makeLower()
    {
        for (CharType * src = mData.data(); *src != StringBase<CharType>::EmptyChar; ++src)
        {
            *src = static_cast<CharType>(std::tolower(static_cast<int32_t>(*src)));
        }

        return (*this);
    }

    template<typename CharType>
    inline StringBase<CharType> & StringBase<CharType>::makeUpper()
    {
        for (CharType * src = mData.data(); *src != StringBase<CharType>::EmptyChar; ++src)
        {
            *src = static_cast<CharType>(std::toupper(static_cast<int32_t>(*src)));
        }

        return (*this);
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::readLine(StringBase<CharType>& strResult, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        return readLine(strResult.mData, startPos);
    }

    template<typename CharType>
    areg::CharPos StringBase<CharType>::readLine(std::basic_string<CharType>& strResult, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        areg::CharPos result = areg::END_POS;
        if (isValidPosition(startPos))
        {
            const CharType* begin = getBuffer(startPos);
            while (areg::isEndOfLine<CharType>(*begin) && (*begin != EmptyChar))
            {
                // escape end of line symbols at the begin.
                ++begin;
            }

            if (*begin != EmptyChar)
            {
                const CharType* str = begin;
                while ((areg::isNewLine<CharType>(*str) == false) && (*str != EmptyChar))
                {
                    // move until reach end of line
                    ++str;
                }

                // copy the line
                strResult.assign(begin, static_cast<uint32_t>(str - begin));

                while (areg::isEndOfLine<CharType>(*str) && (*str != EmptyChar))
                {
                    // find next line or reach end of string
                    ++str;
                }

                // if reached end of string, return END_POS, otherwise, return the next position in the string where new not empty line starts.
                result = *str == EmptyChar ? areg::END_POS : static_cast<areg::CharPos>(str - mData.c_str());
            }
        }

        return result;
    }

    template<typename CharType>
    inline StringBase<CharType>& StringBase<CharType>::makeAlphanumeric()
    {
        if (mData.empty() == false)
        {
            CharType* begin = getBuffer(areg::START_POS);
            CharType* dst = begin;
            for (const CharType* src = begin; *src != static_cast<CharType>(areg::EndOfString); ++src)
            {
                if (std::isalnum(static_cast<int32_t>(*src)) != 0)
                    *dst++ = *src;
            }

            *dst = static_cast<CharType>(areg::EndOfString);
            mData.resize(static_cast<uint32_t>(dst - begin));
        }

        return (*this);
    }

    /// //////////////////////////////////////////////////////////////////////////
    // Template class StringBase Helper
    //////////////////////////////////////////////////////////////////////////

    template<typename CharType>
    inline areg::Ordering StringBase<CharType>::compareString( areg::CharPos startPos
                                                            , const CharType * strOther
                                                            , areg::CharCount count/*= areg::COUNT_ALL */
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
    inline areg::Ordering StringBase<CharType>::compareStringExact( areg::CharPos startPos
                                                                , const CharType * strOther
                                                                , areg::CharCount count/*= areg::COUNT_ALL */ ) const
    {
        areg::Ordering result = areg::Ordering::Smaller;
        count = count == areg::COUNT_ALL ? areg::getStringLength<CharType>(strOther) : count;
        if (isValidPosition(startPos))
        {
            areg::CharCount len = static_cast<areg::CharCount>(getLength() - startPos);
            if (count <= len)
            {
                result = areg::memCompare(getBuffer(startPos), strOther, static_cast<uint32_t>(count));
            }
        }

        return result;
    }

    template<typename CharType>
    inline areg::Ordering StringBase<CharType>::compareStringIgnoreCase( areg::CharPos startPos
                                                                    , const CharType * strOther
                                                                    , areg::CharCount count/*= areg::COUNT_ALL */ ) const
    {
        areg::Ordering result = areg::Ordering::Smaller;
        count = count == areg::COUNT_ALL ? areg::getStringLength<CharType>(strOther) : count;
        if (isValidPosition(startPos))
        {
            areg::CharCount len = static_cast<areg::CharCount>(getLength() - startPos);
            if (count <= len)
            {
                CharType chLeft{ '\0' };
                CharType chRight{ '\0' };

                const CharType* leftSide = getBuffer(startPos);
                const CharType* rightSide = strOther;

                while (count-- > 0)
                {
                    chLeft  = static_cast<CharType>(std::tolower(static_cast<int32_t>(*leftSide ++)));
                    chRight = static_cast<CharType>(std::tolower(static_cast<int32_t>(*rightSide ++)));
                    if (chLeft != chRight)
                    {
                        break;
                    }
                }

                if (chLeft == chRight)
                    result = areg::Ordering::Equal;
                else if (chLeft > chRight)
                    result = areg::Ordering::Bigger;
            }
        }

        return result;
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::replaceWith( areg::CharPos   startPos
                                                            , areg::CharCount count
                                                            , const CharType *    strReplace
                                                            , areg::CharCount lenReplace )
    {
        areg::CharPos nextPos = areg::INVALID_POS;
        if ( (startPos != areg::INVALID_POS) && (startPos != areg::END_POS) )
        {
            int32_t diff = static_cast<int32_t>(lenReplace - count);
            areg::CharPos endPos = startPos + count;
            moveTo( endPos, diff );
            CharType * dst = getBuffer( startPos );
            while ( *strReplace != static_cast<CharType>(areg::EndOfString) )
                *dst ++ = *strReplace ++;

            nextPos = endPos + diff;
        }

        return nextPos;
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::replaceWith( const CharType * strOrigin
                                                            , areg::CharCount lenOrigin
                                                            , const CharType * strReplace
                                                            , areg::CharCount lenReplace
                                                            , areg::CharPos startPos )
    {
        return replaceWith( findFirst( strOrigin, startPos, true ), lenOrigin, strReplace, lenReplace);
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::findFirstPhrase( const CharType* phrase
                                                                , areg::CharCount count     /* = areg::COUNT_ALL */
                                                                , areg::CharPos startPos    /* = areg::START_POS */) const
    {
        if (isValidPosition(startPos) && !areg::isEmpty<CharType>(phrase))
        {
            uint32_t pos = static_cast<uint32_t>(std::basic_string<CharType>::npos);
            
            if (count == areg::COUNT_ALL)
            {
                pos = static_cast<uint32_t>(mData.find(phrase, static_cast<uint32_t>(startPos)));
            }
            else
            {
                pos = static_cast<uint32_t>(mData.find(phrase, static_cast<uint32_t>(startPos), static_cast<uint32_t>(count)));
            }

            return (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos) ? static_cast<areg::CharPos>(pos) : areg::END_POS);
        }
        else
        {
            return areg::INVALID_POS;
        }
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::findFirstPhraseIgnoreCase(const CharType* phrase, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        return (phrase != nullptr ? findFirstPhraseIgnoreCase(std::basic_string<CharType>(phrase), startPos) : areg::INVALID_POS);
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::findFirstPhraseIgnoreCase(const std::basic_string<CharType>& phrase, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        if (isInvalidPosition(startPos) || phrase.empty())
            return areg::INVALID_POS;
        else if (mData.empty())
            return areg::END_POS;
        else if ((mData.length() - static_cast<uint32_t>(startPos)) < phrase.length())
            return areg::END_POS;

        auto it = std::search( mData.begin() + static_cast<int32_t>(startPos), mData.end()
                            , phrase.begin(), phrase.end()
                            , [&](const CharType& ch1, const CharType& ch2) { return (std::tolower(static_cast<int32_t>(ch1)) == std::tolower(static_cast<int32_t>(ch2))); }
                            );

        return (it != mData.end() ? static_cast<areg::CharPos>(std::distance(mData.begin(), it)) : areg::END_POS);
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::findFirstWord(const CharType* word, bool caseSensitive, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        return (word != nullptr ? findFirstWord(std::basic_string<CharType>(word), caseSensitive, startPos) : areg::INVALID_POS);
    }

    template<typename CharType>
    inline areg::CharPos StringBase<CharType>::findFirstWord(const std::basic_string<CharType>& word, bool caseSensitive, areg::CharPos startPos /*= areg::START_POS*/) const
    {
        if (isInvalidPosition(startPos) || word.empty())
            return areg::INVALID_POS;
        else if (mData.empty())
            return areg::END_POS;
        else if ((mData.length() - static_cast<uint32_t>(startPos)) < word.length())
            return areg::END_POS;


        areg::CharPos result = areg::END_POS;
        while (result == areg::END_POS)
        {
            auto it = std::search( mData.begin() + static_cast<int32_t>(startPos), mData.end()
                                , word.begin(), word.end()
                                , [&](const CharType& ch1, const CharType& ch2){ return (caseSensitive ? ch1 == ch2 : std::tolower(static_cast<int32_t>(ch1)) == std::tolower(static_cast<int32_t>(ch2))); }
                                );

            if (it == mData.end())
            {
                // we reached end of string, nothing found, break!
                break;
            }

            areg::CharPos pos = static_cast<areg::CharPos>(std::distance(mData.begin(), it));

            // Take firs char
            CharType chBegin = *(it);
            // Take previous char or empty char
            CharType chPrev  = it == mData.begin() ? EmptyChar : *(it - 1);
            // One of them must be beginning of the word.
            // Example: search "word" or " word" in sentence "This is a word";
            bool isBegin = !isValidNameChar(chBegin) || !isValidNameChar(chPrev);

            // Take last char
            it += static_cast<int32_t>(word.length());
            CharType chEnd  = it != mData.end() ? *it : EmptyChar;
            // Take next char if not EOS
            CharType chNext = (it != mData.end()) && ((it + 1) != mData.end()) ? *(it + 1) : EmptyChar;
            // One of them must be end of the word.
            // Example: search "word" or "word!" in sentence "This is a word!";
            bool isEnd      = !isValidNameChar(chEnd) || !isValidNameChar(chNext);

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
    inline bool StringBase<CharType>::isValidNameChar(const CharType checkChar, std::locale& loc) const
    {
        // initialize list of symbols for the valid names.
        constexpr CharType symbols[] = { '_', '\0' };
        return std::isalnum(checkChar, loc) || areg::isOneOf<CharType>(checkChar, symbols);
    }

    template<typename CharType>
    inline std::vector<StringBase<CharType>> StringBase<CharType>::split(CharType delimiter) const
    {
        std::vector<StringBase<CharType>> result;
        size_t start{ 0 };
        while (mData.at(start) != static_cast<CharType>('\0'))
        {
            size_t pos = mData.find_first_of(delimiter, start);
            if (pos != std::basic_string<CharType>::npos)
            {
                result.push_back(StringBase<CharType>(mData.c_str() + start, static_cast<areg::CharCount>(pos - start)));
                start = pos + 1;
            }
            else
            {
                result.push_back(StringBase<CharType>(mData.c_str() + start));
                break;
            }
        }

        return result;
    }

    template<typename CharType>
    inline std::vector<StringBase<CharType>> StringBase<CharType>::split(const StringBase<CharType> & delimiter) const
    {
        std::vector<StringBase<CharType>> result;
        if (delimiter.isEmpty() == false)
        {
            const size_t skip   { static_cast<size_t>(delimiter.getLength()) };
            const size_t len    { mData.length() };
            size_t start        { 0 };
            size_t pos = mData.find(delimiter.mData, start);
            while (pos != std::basic_string<CharType>::npos)
            {
                result.push_back(StringBase<CharType>(mData.c_str() + start, static_cast<areg::CharCount>(pos - start)));
                start = pos + skip;
                pos = (start < len ? mData.find(delimiter.mData, start) : std::basic_string<CharType>::npos);
            }

            if (start < len)
            {
                result.push_back(StringBase<CharType>(mData.c_str() + start, static_cast<areg::CharCount>(len - start)));
            }
        }
        else
        {
            result.push_back(*this);
        }

        return result;
    }

    template<typename CharType>
    inline bool StringBase<CharType>::startsWith(const StringBase<CharType>& phrase, bool /*isCaseSensitive*/ /*= true*/) const
    {
        return startsWith(phrase.mData);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::startsWith(const std::basic_string<CharType>& phrase, bool isCaseSensitive /*= true*/) const
    {
        return (phrase.length() <= mData.length() ? startsWith(phrase.c_str(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::startsWith(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive /*= true*/) const
    {
        return (phrase.length() <= mData.length() ? startsWith(phrase.data(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::startsWith(const CharType* phrase, bool isCaseSensitive /*= true*/, areg::CharCount count /*= areg::COUNT_ALL*/) const
    {
        count = count != areg::COUNT_ALL ? count : areg::getStringLength<CharType>(phrase);
        if (areg::isEmpty<CharType>(phrase) || (count == 0) || (count > static_cast<areg::CharCount>(mData.length())))
        {
            return false;
        }
        else
        {
            return _hasPhrase(mData.c_str(), phrase, count, isCaseSensitive ? nullptr : areg::LOCALE_DEFAULT);
        }
    }

    template<typename CharType>
    inline bool StringBase<CharType>::endsWith(const StringBase<CharType>& phrase, bool isCaseSensitive /*= true*/) const
    {
        return endsWith(phrase.mData, isCaseSensitive);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::endsWith(const std::basic_string<CharType>& phrase, bool isCaseSensitive /*= true*/) const
    {
        return (phrase.length() <= mData.length() ? endsWith(phrase.c_str(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::endsWith(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive /*= true*/) const
    {
        return (phrase.length() <= mData.length() ? endsWith(phrase.data(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
    }

    template<typename CharType>
    inline bool StringBase<CharType>::endsWith(const CharType* phrase, bool isCaseSensitive /*= true*/, areg::CharCount count /*= areg::COUNT_ALL*/) const
    {
        count = count != areg::COUNT_ALL ? count : areg::getStringLength<CharType>(phrase);
        if (areg::isEmpty<CharType>(phrase) || (count == 0) || (count > static_cast<areg::CharCount>(mData.length())))
        {
            return false;
        }
        else
        {
            areg::CharCount skip = static_cast<areg::CharCount>(mData.length() - static_cast<uint32_t>(count));
            ASSERT(skip >= 0);
            return _hasPhrase(mData.c_str() + skip, phrase, count, isCaseSensitive ? nullptr : areg::LOCALE_DEFAULT);
        }
    }

    template<typename CharType>
    inline bool StringBase<CharType>::_hasPhrase(const CharType* fullString, const CharType* phrase, areg::CharCount count, const char* locale) const
    {
        ASSERT((fullString != nullptr) && (phrase != nullptr));

        bool result{ true };
        if (locale == nullptr)
        {
            result = ::memcmp(fullString, phrase, static_cast<uint32_t>(count) * sizeof(CharType)) == 0;
        }
        else
        {
            for (; *phrase != StringBase<CharType>::EmptyChar; ++phrase, ++fullString)
            {
                if (std::tolower(static_cast<int32_t>(*phrase)) != std::tolower(static_cast<int32_t>(*fullString)))
                {
                    result = false;
                    break;
                }
            }
        }

        return result;
    }

    //////////////////////////////////////////////////////////////////////////
    // StringBase friend operators
    //////////////////////////////////////////////////////////////////////////
    template<typename CT>
    inline StringBase<CT> operator + (const StringBase<CT>& lhs, const StringBase<CT>& rhs)
    {
        StringBase<CT> result(lhs);
        return (result += rhs);
    }

    template<typename CT>
    inline StringBase<CT> operator + (const StringBase<CT>& lhs, const std::basic_string<CT>& rhs)
    {
        StringBase<CT> result(lhs);
        return (result += rhs);
    }

    template<typename CT>
    inline StringBase<CT> operator + (const StringBase<CT>& lhs, const std::basic_string_view<CT>& rhs)
    {
        StringBase<CT> result(lhs);
        return (result += rhs);
    }

    template<typename CT>
    inline StringBase<CT> operator + (const StringBase<CT>& lhs, const CT* rhs)
    {
        StringBase<CT> result(lhs);
        return (result += rhs);
    }

    template<typename CT>
    inline StringBase<CT> operator + (const StringBase<CT>& lhs, const CT rhs)
    {
        StringBase<CT> result(lhs);
        return (result += rhs);
    }

    template<typename CT>
    inline StringBase<CT> operator + (const std::basic_string<CT>& lhs, const StringBase<CT>& rhs)
    {
        StringBase<CT> result(lhs);
        return (result += rhs);
    }

    template<typename CT>
    inline StringBase<CT> operator + (const std::basic_string_view<CT>& lhs, const StringBase<CT>& rhs)
    {
        StringBase<CT> result(lhs);
        return (result += rhs);
    }

    template<typename CT>
    inline StringBase<CT> operator + (const CT* lhs, const StringBase<CT>& rhs)
    {
        StringBase<CT> result(lhs);
        return (result += rhs);
    }

    template<typename CT>
    inline StringBase<CT> operator + (const CT lhs, const StringBase<CT>& rhs)
    {
        StringBase<CT> result(lhs);
        return (result += rhs);
    }

    template<typename CT>
    inline const InStream& operator >> (const InStream& stream, StringBase<CT>& input)
    {
        return (stream >> input.mData);
    }

    template<typename CT>
    inline OutStream& operator << (OutStream& stream, const StringBase<CT>& output)
    {
        return (stream << output.mData);
    }

    template<typename CT>
    inline std::ostream & operator << ( std::ostream & stream, const StringBase<CT> & output )
    {
        return (stream << output.getData( ));
    }

    template<typename CT>
    inline const std::istream & operator >> ( const std::istream & stream, StringBase<CT> & input )
    {
        return (stream >> input.getData( ));
    }

} // namespace areg

#endif  // AREG_BASE_STRINGBASE_HPP
