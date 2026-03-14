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
#include "areg/base/areg_global.h"

#include "areg/base/TemplateBase.hpp"
#include "areg/base/StringDefs.hpp"

#include <algorithm>
#include <iostream>
#include <locale>
#include <string>
#include <vector>
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class InStream;
class OutStream;

//////////////////////////////////////////////////////////////////////////
// StringBase class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The basic string class template, which contains basic operations
 *          to set, append, insert, search and replace string, as well to get substring.
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

    inline StringBase();

    inline StringBase( const CharType source );

    inline StringBase( const CharType * source );

    inline StringBase( const std::basic_string<CharType>& source );

    inline StringBase( const std::basic_string_view<CharType>& source );

    inline StringBase( const CharType * source, areg::CharCount count );

    inline StringBase( const StringBase<CharType> & source );

    inline StringBase( StringBase<CharType> && source ) noexcept;

    inline StringBase( std::basic_string<CharType>&& source ) noexcept;

    explicit inline StringBase(uint32_t count);

    ~StringBase() = default;

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Converting operator, converts object to string buffer.
     **/
    inline operator const CharType* () const noexcept;

    /**
     * \brief   Returns character at the given valid zero-based position. The position should not be neither
     *          negative, nor more than the string length.
     **/
    inline const CharType operator [] (int32_t index) const noexcept;

    inline StringBase<CharType>& operator = (const StringBase<CharType>& src);
    inline StringBase<CharType>& operator = (const std::basic_string<CharType>& src);
    inline StringBase<CharType>& operator = (const std::basic_string_view<CharType>& src);
    inline StringBase<CharType>& operator = (const CharType* src);
    inline StringBase<CharType>& operator = (StringBase<CharType>&& src) noexcept;
    inline StringBase<CharType>& operator = (std::basic_string<CharType>&& src) noexcept;
    inline StringBase<CharType>& operator = (const CharType src);

    [[nodiscard]] inline bool operator == (const StringBase<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator == (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator == (const std::basic_string_view<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator == (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator == (const CharType ch ) const noexcept;

    [[nodiscard]] inline bool operator != (const StringBase<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator != (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator != (const std::basic_string_view<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator != (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator != (const CharType ch) const noexcept;

    [[nodiscard]] inline bool operator > (const StringBase<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator > (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator > (const std::basic_string_view<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator > (const CharType* other) const noexcept;

    [[nodiscard]] inline bool operator < (const StringBase<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator < (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator < (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator < (const std::basic_string_view<CharType>& other) const noexcept;

    [[nodiscard]] inline bool operator >= (const StringBase<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator >= (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator >= (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator >= (const std::basic_string_view<CharType>& other) const noexcept;

    [[nodiscard]] inline bool operator <= (const StringBase<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator <= (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator <= (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator <= (const std::basic_string_view<CharType>& other) const noexcept;

    inline StringBase<CharType>& operator += (const StringBase<CharType>& src);
    inline StringBase<CharType>& operator += (const std::basic_string<CharType>& src);
    inline StringBase<CharType>& operator += (const std::basic_string_view<CharType>& src);
    inline StringBase<CharType>& operator += (const CharType* src);
    inline StringBase<CharType>& operator += (const CharType chSource);

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
     * \brief   Streams to input object, i.e. reads data from streaming object to string, initialize
     *          string data.
     *
     * \return  Reference to stream object.
     **/
    template<typename CT>
    friend inline const InStream & operator >> (const InStream & stream, StringBase<CT> & input);

    /**
     * \brief   Streams from output object, i.e. write data from string to streaming object.
     *
     * \return  Reference to stream object.
     **/
    template<typename CT>
    friend inline OutStream & operator << (OutStream & stream, const StringBase<CT> & output);

    /**
     * \brief   Prints string message to the standard output stream.
     *
     * \return  Reference to stream object.
     **/
    template<typename CT>
    friend inline std::ostream & operator << ( std::ostream & stream, const StringBase<CT> & output );

    /**
     * \brief   Reads string message from the standard input stream.
     *
     * \return  Reference to stream object.
     **/
    template<typename CT>
    friend inline const std::istream & operator >> ( const std::istream & stream, StringBase<CT> & input );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Checks validity of the string. The valid characters are listed in the passed list,
     *          which is null-terminated string.
     **/
    [[nodiscard]]
    inline bool validate( const CharType * validityList ) const noexcept;

    /**
     * \brief   Checks whether the string is numeric. Checks only integer numbers.
     *
     * \param   signIgnore      If true, ignores '-' and '+' signs in front of number
     **/
    [[nodiscard]]
    inline bool is_numeric( bool signIgnore = true ) const noexcept;

    /**
     * \brief   Returns true if string is contains only letters numbers. No symbols or other
     *          controls are allowed.
     **/
    [[nodiscard]]
    inline bool is_alphanumeric() const noexcept;

    /**
     * \brief   Returns true if string is valid for name. The name is valid if it contains letters,
     *          numbers following symbols {'_'}. All other symbols are rejected.
     **/
    [[nodiscard]]
    inline bool is_valid_name() const noexcept;

    /**
     * \brief   Returns true if the passed character is valid for the name.
     **/
    [[nodiscard]]
    inline bool is_name_char(const CharType checkChar) const noexcept;

    /**
     * \brief   Returns true if string is empty.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Returns the length of the string.
     **/
    [[nodiscard]]
    inline areg::CharCount length() const noexcept;

    /**
     * \brief   Returns the number of characters that can store in the string.
     **/
    [[nodiscard]]
    inline areg::CharCount capacity() const noexcept;

    /**
     * \brief   Return the size of string in bytes including the end of the string character.
     **/
    [[nodiscard]]
    inline uint32_t space() const noexcept;

    /**
     * \brief   Returns string buffer starting at specified valid position.
     **/
    [[nodiscard]]
    inline const CharType* buffer(areg::CharPos startAt = areg::START_POS) const noexcept;
    /**
     * \brief   Returns string buffer starting at specified valid position.
     **/
    [[nodiscard]]
    inline CharType* buffer(areg::CharPos startAt = areg::START_POS) noexcept;

    /**
     * \brief   Returns the buffer of string.
     **/
    [[nodiscard]]
    inline const CharType* as_string() const noexcept;

    /**
     * \brief   Returns the string object.
     **/
    [[nodiscard]]
    inline const std::basic_string<CharType>& data() const  noexcept;

    /**
     * \brief   Returns true if specified character position is valid in the string.
     **/
    [[nodiscard]]
    inline bool is_valid_position(areg::CharPos pos) const noexcept;

    /**
     * \brief   Returns true if specified character position is invalid in the string.
     **/
    [[nodiscard]]
    inline bool is_invalid_position(areg::CharPos pos) const noexcept;

    /**
     * \brief   Returns true if specified position is equal to the end of the string.
     **/
    [[nodiscard]]
    inline bool is_last_position(areg::CharPos pos) const noexcept;

    /**
     * \brief   Returns true if string is not empty the specified position is zero.
     **/
    [[nodiscard]]
    inline bool is_first_position(areg::CharPos pos) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Clears the string, makes it empty invalid.
     **/
    inline void clear() noexcept;

    /**
     * \brief   Delete unused space in the string.
     **/
    inline void free_extra();

    /**
     * \brief   Sets the size of string to zero deletes the space.
     **/
    inline void release();

    /**
     * \brief   Searches the whole word in the string at specified 'startAt' position. On output,
     *          the 'startAt' and 'endAt' contain start end of position where each word start end.
     *          The length of the word calculated by 'endAt - startAt' formula. The word is a
     *          collection of characters digits, allowed in the valid name characters ('_'), which
     *          are surrounded by other symbols (whitespace, comma, dot, etc), or if it starts at
     *          the beginning or end of the string.
     **/
    void find_word(areg::CharPos& startAt, areg::CharPos& endAt) const noexcept;

    /**
     * \brief   Searches the whole word in the string at specified 'startAt' position. On output,
     *          the 'startAt' and 'endAt' contain start end of position where each word start end.
     *          On output, the 'word' is the word with length 'endAt - startAt'. The word is a
     *          collection of characters digits, allowed in the valid name characters ('_'), which
     *          are surrounded by other symbols (whitespace, comma, dot, etc), or if it starts at
     *          the beginning or end of the string.
     **/
    void extract_word(StringBase<CharType>& word, areg::CharPos& startAt, areg::CharPos& endAt) const;

    /**
     * \brief   Find the first occurrence of any of the characters in string buffer 'chars'. The
     *          search starts at given 'startPos'. Returns valid position if found any. Otherwise,
     *          returns areg::INVALID_POS.
     *
     * \return  Returns valid string position value, if any occurrence of given characters found.
     *          Otherwise, returns areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_one_of( const CharType * chars, areg::CharPos startPos = areg::START_POS ) const noexcept;

    /**
     * \brief   Find the first occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   chSearch            The character to search in the string.
     * \param   caseSensitive       If true, the character match should be by exact, i.e.
     *                              case-sensitive. Otherwise, the search is by upper and lower
     *                              case.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_first( CharType chSearch, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true ) const noexcept;

    /**
     * \brief   Find the first occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   caseSensitive       If true, the search of phrase should be exact, i.e.
     *                              case-sensitive. Otherwise, the search is by upper and lower
     *                              case.
     * \param   wholeWord           If true, the search of phrase should be the whole word.
     *                              Otherwise, searches by any match. By default, the search is by
     *                              any match.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_first( const CharType * phrase, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true, bool wholeWord = false ) const noexcept;
    /**
     * \brief   Find the first occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   caseSensitive       If true, the search of phrase should be exact, i.e.
     *                              case-sensitive. Otherwise, the search is by upper and lower
     *                              case.
     * \param   wholeWord           If true, the search of phrase should be the whole word.
     *                              Otherwise, searches by any match. By default, the search is by
     *                              any match.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_first( const StringBase<CharType> & phrase, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true, bool wholeWord = false ) const noexcept;

    /**
     * \brief   Find the last occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   chSearch            The character to search in the string.
     * \param   caseSensitive       If true, the character match should be by exact, i.e.
     *                              case-sensitive. Otherwise, the search is by upper and lower
     *                              case.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_last( CharType chSearch, areg::CharPos startPos = areg::END_POS, bool caseSensitive = true ) const noexcept;

    /**
     * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   caseSensitive       If true, the search of phrase should be exact, i.e.
     *                              case-sensitive. Otherwise, the search is by upper and lower
     *                              case.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_last( const CharType * phrase, areg::CharCount phraseCount, areg::CharPos startPos, bool caseSensitive) const noexcept;

    /**
     * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   caseSensitive       If true, the search of phrase should be exact, i.e.
     *                              case-sensitive.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    inline areg::CharPos find_last( const CharType* phrase, areg::CharPos startPos = areg::END_POS, bool caseSensitive = true) const noexcept;

    /**
     * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   caseSensitive       If true, the search of phrase should be exact, i.e.
     *                              case-sensitive.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    inline areg::CharPos find_last( const StringBase<CharType> & phrase, areg::CharPos startPos = areg::END_POS, bool caseSensitive = true ) const noexcept;

    /**
     * \brief   Compares the given string. The comparing is done by certain position, certain amount
     *          of characters or by ignoring upper lower cases, depending parameters are passed in
     *          the function.
     *
     * \param   caseSensitive       If true, compares by exact match, i.e. case-sensitive.
     *                              Otherwise, it ignores upper and lower cases.
     * \return  Returns: areg::Smaller if string is less than given string areg::Equal if
     *          strings have equal areg::Bigger if string is more than given string
     **/
    [[nodiscard]]
    areg::Ordering compare( const CharType * strOther, areg::CharPos startAt = areg::START_POS, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true) const noexcept;

    [[nodiscard]]
    inline areg::Ordering compare(const StringBase<CharType> & strOther, bool caseSensitive = true) const noexcept;

    [[nodiscard]]
    inline areg::Ordering compare(const std::basic_string<CharType>& strOther, bool caseSensitive = true) const noexcept;

    [[nodiscard]]
    inline areg::Ordering compare(const std::basic_string_view<CharType>& strOther, bool caseSensitive = true) const noexcept;

    [[nodiscard]]
    inline areg::Ordering compare( const StringBase<CharType>& strOther
                                 , areg::CharPos startAt = areg::START_POS
                                 , areg::CharCount count = areg::COUNT_ALL
                                 , bool caseSensitive = true) const noexcept;

    [[nodiscard]]
    inline areg::Ordering compare( const std::basic_string<CharType>& strOther
                                 , areg::CharPos startAt = areg::START_POS
                                 , areg::CharCount count = areg::COUNT_ALL
                                 , bool caseSensitive = true) const noexcept;

    /**
     * \brief   Truncates the starting at zero-based 'startPos' given number of characters. If
     *          'startPos' is beginning of the string (areg::START_POS) and 'count' is equal to
     *          the length of the string or areg::COUNT_ALL, the string is not modified.
     *
     * \return  Returns truncated string.
     **/
    inline StringBase<CharType>& substring(areg::CharPos startPos, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Extracts the substring starting at startPos given number of characters.
     **/
    inline void substring( StringBase<CharType> & outResult, areg::CharPos startPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL ) const;

    /**
     * \brief   Searches given phrase in the string starting from given position until the end of
     *          string. If found, copies the string data into the result until the found position
     *          returns position next after phrase. If not found, will copy complete string until
     *          end.
     *
     * \return  Returns next position after searched phrase and value are followings: - Valid string
     *          position not equal to areg::END_POS, if found phrase and the phrase is not at
     *          the end; - areg::END_POS if found the phrase at end of string; -
     *          areg::INVALID_POS if could not find the phrase.
     * 
     * \code[cpp]
     *
     *  String test("0123 456 789 0123");
     *  String result;
     *  areg::CharPos next = areg::START_POS;
     *  next = test.substr(result, "0123", next);   // results: next == 4, result == ""
     *  next = test.substr(result, "0123", next);   // results: next == areg::END_POS, result == " 456 789 "
     *
     *  next = areg::START_POS;
     *  next = test.substr(result, " ", next);      // results: next == 5, result == "0123"
     *  next = test.substr(result, " ", next);      // results: next == 9, result == "456"
     *  next = test.substr(result, " ", next);      // results: next == areg::INVALID_POS, result == "0123"
     * \endcode
     **/
    inline areg::CharPos substring(StringBase<CharType>& outResult, const CharType* strPhrase, areg::CharPos startPos = areg::START_POS) const;

    inline areg::CharPos substring(StringBase<CharType>& outResult, const StringBase<CharType>& strPhrase, areg::CharPos startPos = areg::START_POS) const;

    inline areg::CharPos substring(StringBase<CharType>& outResult, const std::basic_string<CharType>& strPhrase, areg::CharPos startPos = areg::START_POS) const;

    /**
     * \brief   Searches given symbol in the string starting from given position until end of
     *          string. If found, copies the string data into the result until the found position
     *          returns position next after symbol. If not found, will copy complete string until
     *          end.
     *
     * \param   chSymbol    The symbol to search in the string.
     * \return  Returns next position after searched symbol and value are followings: - Valid string
     *          position not equal to areg::END_POS, if found phrase and the symbol is not at
     *          the end; - areg::END_POS if found the symbol at end of string; -
     *          areg::INVALID_POS if could not find the phrase.
     **/
    areg::CharPos substring( StringBase<CharType> & outResult, CharType chSymbol, areg::CharPos startPos = areg::START_POS ) const;

    /**
     * \brief   Returns left side (begin) substring of length 'charCount'.
     **/
    [[nodiscard]]
    inline StringBase<CharType> left_side(areg::CharCount charCount) const;

    /**
     * \brief   Returns right side (end) substring of length 'charCount'.
     **/
    [[nodiscard]]
    inline StringBase<CharType> right_side(areg::CharCount charCount) const;

    /**
     * \brief   Copies given amount of characters of given string returns the amount of copied characters.
     *          If string has not enough space to copy characters, it will reallocate the space.
     **/
    inline StringBase<CharType>& assign(const CharType * source, areg::CharCount count = areg::COUNT_ALL );

    inline StringBase<CharType>& assign(const std::basic_string<CharType> & source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& assign(const std::basic_string_view<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& assign(const StringBase<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& assign(CharType ch);

    /**
     * \brief   Appends given string at the end. The given string can be limited by zero-based valid
     *          position by amount of characters to append.
     **/
    inline StringBase<CharType>& append(const CharType * source, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& append(const std::basic_string<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& append(const std::basic_string_view<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& append(const StringBase<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& append(CharType ch);

    /**
     * \brief   Moves data inside string buffer starting at specified position. It moves left if
     *          'move_to' value is negative moves right if 'move_to' is positive. If 'move_to' is
     *          zero, no change happens.
     *
     * \param   move_to     Specifies number of characters and direction to move string. If
     *                      negative, moves string to left side. String buffer is not reallocated.
     *                      If positive, moves string to right. if zero, no change happens.
     **/
    StringBase<CharType>& move_to(areg::CharPos startPos, int32_t move_to);

    /**
     * \brief   Inserts given source of character at given valid zero-based position in the string.
     *          The character can be inserted at any position, including begin of string or at the
     *          end of string (areg::END_POS).
     **/
    inline StringBase<CharType>& insert_at( CharType source, areg::CharPos atPos, areg::CharCount count = 1 );

    /**
     * \brief   Inserts given source at given valid zero-based position in the string. The character
     *          can be inserted at any position, including begin of string or at the end of string
     *          (areg::END_POS).
     **/
    inline StringBase<CharType>& insert_at( const CharType * source, areg::CharPos atDstPos, areg::CharCount count = areg::COUNT_ALL );

    inline StringBase<CharType>& insert_at(const std::basic_string<CharType>& source, areg::CharPos atDstPos, areg::CharPos atSrcPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& insert_at(const std::basic_string_view<CharType>& source, areg::CharPos atDstPos, areg::CharPos atSrcPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& insert_at(const StringBase<CharType>& source, areg::CharPos atDstPos, areg::CharPos atSrcPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Searches replaces given character by another character. The search replacement
     *          starts at given position. The method either replaces only first match or all
     *          matches.
     *
     * \param   chSearch        The character to be searched to replace.
     * \param   chReplace       The character to set when found 'chSearch'.
     * \param   replaceAll      If true, replaces all matches in the string. Otherwise, replace
     *                          first match.
     **/
    StringBase<CharType>& replace( CharType chSearch, CharType chReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true );

    /**
     * \brief   Searches replaces given string by another string. The search replacement starts at
     *          given position. The method either replaces only first match or all matches.
     **/
    StringBase<CharType>& replace( const CharType * strSearch, const CharType * strReplace, areg::CharPos startPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL, bool replaceAll = true );

    StringBase<CharType>& replace(const std::basic_string<CharType>& strSearch, const std::basic_string<CharType>& strReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true);

    StringBase<CharType>& replace(const std::basic_string_view<CharType>& strSearch, const std::basic_string_view<CharType>& strReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true);

    inline StringBase<CharType>& replace(const StringBase<CharType>& strSearch, const StringBase<CharType>& strReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true);

    /**
     * \brief   At the given position of the string removes specified amount of characters replaces
     *          by the given string.position after replacing string.
     **/
    inline StringBase<CharType>& replace(areg::CharPos startPos, areg::CharCount charsRemove, const CharType* strReplace, areg::CharCount count = areg::COUNT_ALL);

    inline StringBase<CharType>& replace(areg::CharPos startPos, areg::CharCount charsRemove, const std::basic_string<CharType>& strReplace);

    inline StringBase<CharType>& replace(areg::CharPos startPos, areg::CharCount charsRemove, const StringBase<CharType>& strReplace);

    /**
     * \brief   Removes specified amount of characters in string at specified starting position. If
     *          'count' is areg::COUNT_ALL, it will remove all characters until end of the string.
     **/
    StringBase<CharType>& remove(areg::CharPos startPos, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Starting from the given valid zero-based position searches removes specified
     *          character. If 'removedAll' parameter is true, removes all matches in the string.
     *          Otherwise, it removes only first match.
     *
     * \param   chRemove        The character to search and remove.
     * \param   removeAll       If true, will remove all matches. Otherwise, removes first match of
     *                          the character.
     **/
    StringBase<CharType>& remove(const CharType chRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true);

    /**
     * \brief   Starting from the given valid zero-based position searches removes given phrase of
     *          string. The search starts at given position. If 'removedAll' parameter is true,
     *          removes all matches in the string. Otherwise, it removes only first match.
     **/
    inline StringBase<CharType>& remove( const CharType * strRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true );

    inline StringBase<CharType>& remove(const StringBase<CharType>& strRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true);

    StringBase<CharType>& remove(const std::basic_string<CharType> & strRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true);

    /**
     * \brief   Resizes the string to contain count characters. If the current size is less than
     *          count, additional characters are appended. If the current size is greater than
     *          count, the string is reduced to its first count elements.
     *
     * \param   chFill      The characters to fill if new size if longer than the current length.
     **/
    inline StringBase<CharType>& resize(areg::CharCount count, CharType chFill = static_cast<CharType>('\0'));

    /**
     * \brief   If the length of string is bigger than the 'maxChars', it truncated the string. If
     *          'maxChars' is zero, empties the string. If the length of the string is smaller than
     *          the 'maxChars', nothing happens.
     **/
    inline StringBase<CharType>& truncate(areg::CharCount maxChars);

    /**
     * \brief   Reserves the space for the string. Unlike 'resize' this operation does not change
     *          the current length of the string.
     **/
    inline StringBase<CharType>& reserve(areg::CharCount newCapacity);

    /**
     * \brief   Returns character at specified valid zero-based position.
     **/
    [[nodiscard]]
    inline CharType at( areg::CharPos atPos ) const noexcept;

    /**
     * \brief   Sets character at specified valid zero-based position.
     **/
    inline StringBase<CharType>& set_at( CharType ch, areg::CharPos atPos = areg::END_POS ) noexcept;

    /**
     * \brief   Removes whitespace characters from left side, i.e. from the begin of the string
     *          returns the actual modified string.
     **/
    inline StringBase<CharType>& trim_left() noexcept;

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin of the
     *          string. The existing string remain unmodified.
     **/
    inline void trim_left( StringBase<CharType> & strResult ) const;

    inline void trim_left(std::basic_string<CharType>& strResult) const;

    /**
     * \brief   Removes whitespace characters from right side, i.e. from end of the string returns
     *          the actual modified string.
     **/
    inline StringBase<CharType>& trim_right() noexcept;

    /**
     * \brief   Copies data into given string without trailing whitespace at the end of the string.
     *          The existing string remain unmodified.
     **/
    inline void trim_right( StringBase<CharType> & strResult ) const;

    inline void trim_right(std::basic_string<CharType>& strResult) const;

    /**
     * \brief   Removes whitespace characters from left right sides, i.e. from the begin end of the
     *          string returns the actual modified string.
     **/
    StringBase<CharType>& trim_all() noexcept;

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin end of the
     *          string. The existing string remain unmodified.
     **/
    inline void trim_all( StringBase<CharType> & strResult ) const;

    void trim_all(std::basic_string<CharType>& strResult) const;

    /**
     * \brief   Converts the string, makes all letters to lower case returns the instance of
     *          converted string.
     **/
    inline StringBase<CharType> & make_lower() noexcept;

    /**
     * \brief   Converts the string, makes all letters to upper case returns the instance of
     *          converted string.
     **/
    inline StringBase<CharType> & make_upper() noexcept;

    /**
     * \brief   Reads the string starting from specified position until end of line, copies the
     *          result into the 'strResult' returns the next position where new line begins or
     *          returns areg::END_POS if reached end of string.
     *
     * \return  Returns next position where starts non-empty new line or areg::END_POS if
     *          reached end of string.
     * \note    If after reading the line there are still empty new lines. The returned position
     *          value escapes followed empty new lines.
     **/
    inline areg::CharPos read_line(StringBase<CharType>& strResult, areg::CharPos startPos = areg::START_POS) const;

    areg::CharPos read_line(std::basic_string<CharType>& strResult, areg::CharPos startPos = areg::START_POS) const;

    /**
     * \brief   In the existing string removes all characters, which are not alphanumeric.
     *
     * \return  Returns new length of string after making alphanumeric
     **/
    inline StringBase<CharType>& make_alphanumeric() noexcept;

    /**
     * \brief   Checks returns true if the string starts with the given phrase. Valid only the first
     *          256 characters based on UTF-8 code page.
     **/
    [[nodiscard]]
    inline bool starts_with(const StringBase<CharType>& phrase, bool isCaseSensitive = true) const noexcept;

    [[nodiscard]]
    inline bool starts_with(const std::basic_string<CharType>& phrase, bool isCaseSensitive = true) const noexcept;

    [[nodiscard]]
    inline bool starts_with(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive = true) const noexcept;

    [[nodiscard]]
    inline bool starts_with(const CharType* phrase, bool isCaseSensitive = true, areg::CharCount count = areg::COUNT_ALL) const noexcept;

    /**
     * \brief   Checks returns true if the string ends with the given phrase. Valid only the first
     *          256 characters based on UTF-8 code page.
     **/
    [[nodiscard]]
    inline bool ends_with(const StringBase<CharType>& phrase, bool isCaseSensitive = true) const noexcept;

    [[nodiscard]]
    inline bool ends_with(const std::basic_string<CharType>& phrase, bool isCaseSensitive = true) const noexcept;

    [[nodiscard]]
    inline bool ends_with(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive = true) const noexcept;

    [[nodiscard]]
    inline bool ends_with(const CharType* phrase, bool isCaseSensitive = true, areg::CharCount count = areg::COUNT_ALL) const noexcept;

    /**
     * \brief   Searches string if found, replace by another.
     *
     * \return  Returns next position after replacing string. Returns areg::INVALID_POS if could
     *          not find specified string.
     **/
    inline areg::CharPos replace_with( const CharType * strOrigin
                                     , areg::CharCount lenOrigin
                                     , const CharType * strReplace
                                     , areg::CharCount lenReplace
                                     , areg::CharPos startPos);

    inline areg::CharPos replace_with( areg::CharPos startPos
                                     , areg::CharCount count
                                     , const CharType * strReplace
                                     , areg::CharCount lenReplace);

    /**
     * \brief   Splits the given string into multiple parts considering specified delimiter.
     *
     * \param   delim       The delimiter character that should be searched to split the string.
     * \return  Returns list of strings that are split be specified delimiter.
     **/
    [[nodiscard]]
    inline std::vector<StringBase<CharType>> split(CharType delim) const;

    [[nodiscard]]
    inline std::vector<StringBase<CharType>> split(const StringBase<CharType> & delim) const;

/************************************************************************/
// Protected methods, can be assessed only from derived class
/************************************************************************/
protected:

    /**
     * \brief   Compares the existing string at the specified valid zero-based position with another
     *          string, returns: - areg::Smaller if string is less than given string -
     *          areg::Equal if strings have equal - areg::Bigger if string is more than given
     *          string. The comparing can be done by ignoring case sensitivity.
     *
     * \param   caseSensitive       If true, compares exact match of string. Otherwise, ignores
     *                              lower and upper cases.
     * \return  Return: areg::Smaller if string is less than given string areg::Equal if strings
     *          have equal areg::Bigger if string is more than given string
     **/
    [[nodiscard]]
    inline areg::Ordering compare( areg::CharPos startPos, const CharType * strOther, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true ) const noexcept;

    /**
     * \brief   Compares the existing string at the specified valid zero-based position with another
     *          string, returns: - areg::Smaller if string is less than given string -
     *          areg::Equal if strings have equal - areg::Bigger if string is more than given
     *          string. The comparing is done by exact match.
     *
     * \return  Return: areg::Smaller if string is less than given string areg::Equal if strings
     *          have equal areg::Bigger if string is more than given string
     **/
    [[nodiscard]]
    inline areg::Ordering compare_exact(areg::CharPos startPos, const CharType* strOther, areg::CharCount count = areg::COUNT_ALL) const noexcept;

    /**
     * \brief   Compares the existing string at the specified valid zero-based position with another
     *          string, returns: - areg::Smaller if string is less than given string -
     *          areg::Equal if strings have equal - areg::Bigger if string is more than given
     *          string. The comparing is done by ignoring upper / lower case.
     *
     * \return  Return: areg::Smaller if string is less than given string areg::Equal if strings
     *          have equal areg::Bigger if string is more than given string
     **/
    [[nodiscard]]
    inline areg::Ordering compare_ignore_case(areg::CharPos startPos, const CharType* strOther, areg::CharCount count = areg::COUNT_ALL) const noexcept;

    /**
     * \brief   Searches the first phrase in string. The comparing is done by exact match.
     *
     * \return  If found, returns valid position in the string. If not found, it returns
     *          areg::END_POS.
     **/
    [[nodiscard]]
    inline areg::CharPos find_first_phrase(const CharType* phrase, areg::CharCount count = areg::COUNT_ALL, areg::CharPos startPos = areg::START_POS ) const noexcept;

    /**
     * \brief   Searches the first phrase in string. The comparing is done by ignoring upper lower
     *          cases. It compares upper lower cases by default locale.
     *
     * \return  If found, returns valid position in the string. If not found, it returns
     *          areg::END_POS.
     **/
    [[nodiscard]]
    inline areg::CharPos find_phrase(const CharType* phrase, areg::CharPos startPos = areg::START_POS) const noexcept;

    [[nodiscard]]
    inline areg::CharPos find_phrase(const std::basic_string<CharType>& phrase, areg::CharPos startPos = areg::START_POS) const noexcept;

    /**
     * \brief   Searches the first match of the entire word in string. The comparing can be done by
     *          exact match of ignoring upper lower cases. It compares upper lower cases by default
     *          locale.
     *
     * \param   caseSensitive       If true, it searches by exact match. Otherwise, ignores upper
     *                              and lower cases.
     * \return  If found, returns valid position in the string. If not found, it returns
     *          areg::END_POS.
     **/
    [[nodiscard]]
    inline areg::CharPos find_first_word(const CharType* word, bool caseSensitive, areg::CharPos startPos = areg::START_POS) const noexcept;

    [[nodiscard]]
    inline areg::CharPos find_first_word(const std::basic_string<CharType>& word, bool caseSensitive, areg::CharPos startPos = areg::START_POS) const noexcept;

    /**
     * \brief   Returns true if specified character is valid for the names. The names may contain
     *          characters, digits symbols like '_'.
     **/
    [[nodiscard]]
    static inline bool is_name_char(const CharType checkChar, std::locale& loc) noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Compares whether full string starts with the given phrase.
     *
     * \return  Returns true if 'count' characters in the full string are equal to the phrase.
     **/
    [[nodiscard]]
    inline bool _has_phrase(const CharType* fullString, const CharType* phrase, areg::CharCount count, const char* locale) const noexcept;

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
    : mData  ( source != nullptr ? source : &EmptyChar, static_cast<uint32_t>(count != areg::COUNT_ALL ? count : areg::string_length(source)) )
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
inline StringBase<CharType>::operator const CharType* () const noexcept
{
    return mData.c_str();
}

template<typename CharType>
inline const CharType StringBase<CharType>::operator [] (int32_t index) const noexcept
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
    mData.assign( src.data(), src.length() );
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
inline bool StringBase<CharType>::operator == (const StringBase<CharType>& other) const noexcept
{
    return (mData == other.mData);
}

template<typename CharType>
inline bool StringBase<CharType>::operator == (const std::basic_string<CharType>& other) const noexcept
{
    return (mData == other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator == (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData == other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator == (const CharType* other) const noexcept
{
    return (mData == other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator == ( const CharType ch) const noexcept
{
    return ((mData.length() == 1) && (mData[0] == ch));
}

template<typename CharType>
inline bool StringBase<CharType>::operator != (const StringBase<CharType>& other) const noexcept
{
    return (mData != other.mData);
}

template<typename CharType>
inline bool StringBase<CharType>::operator != (const std::basic_string<CharType>& other) const noexcept
{
    return (mData != other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator != (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData != other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator != (const CharType* other) const noexcept
{
    return (mData != other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator != ( const CharType ch ) const noexcept
{
    return ((mData.length( ) != 1) || (mData[0] != ch));
}

template<typename CharType>
inline bool StringBase<CharType>::operator > (const StringBase<CharType>& other) const noexcept
{
    return (mData > other.mData);
}

template<typename CharType>
inline bool StringBase<CharType>::operator > (const std::basic_string<CharType>& other) const noexcept
{
    return (mData > other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator > (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData > other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator > (const CharType* other) const noexcept
{
    return (mData > other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator < (const StringBase<CharType>& other) const noexcept
{
    return (mData < other.mData);
}

template<typename CharType>
inline bool StringBase<CharType>::operator < (const std::basic_string<CharType>& other) const noexcept
{
    return (mData < other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator < (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData < other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator < (const CharType* other) const noexcept
{
    return (mData < other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator >= (const StringBase<CharType>& other) const noexcept
{
    return (mData >= other.mData);
}

template<typename CharType>
inline bool StringBase<CharType>::operator >= (const std::basic_string<CharType>& other) const noexcept
{
    return (mData >= other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator >= (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData >= other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator >= (const CharType* other) const noexcept
{
    return (mData >= other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator <= (const StringBase<CharType>& other) const noexcept
{
    return (mData <= other.mData);
}

template<typename CharType>
inline bool StringBase<CharType>::operator <= (const std::basic_string<CharType>& other) const noexcept
{
    return (mData <= other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator <= (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData <= other);
}

template<typename CharType>
inline bool StringBase<CharType>::operator <= (const CharType* other) const noexcept
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
    mData.append(src.data(), src.length());
    return (*this);
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::operator += (const CharType* src)
{
    if (!areg::is_empty(src))
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
inline bool StringBase<CharType>::validate(const CharType* validityList) const noexcept
{
    if (mData.empty() || areg::is_empty<CharType>(validityList))
        return false;

    // find_first_not_of is SIMD-optimized in major STLs; eliminates the per-character loop.
    return mData.find_first_not_of(validityList) == std::basic_string<CharType>::npos;
}

template<typename CharType>
inline bool StringBase<CharType>::is_numeric(bool signIgnore /*= true*/) const noexcept
{
    bool result = false;
    const CharType* src = mData.c_str();

    // escape if first symbol is a sign
    if (signIgnore && ((*src == '-') || (*src == '+')))
        ++src; // Escape sign, if it should.

    // make sure the string is not empty
    if (*src != EmptyChar)
    {
        while (areg::is_numeric<CharType>(*src))
            ++src;

        result = *src == EmptyChar;
    }

    return result;
}

template<typename CharType>
inline bool StringBase<CharType>::is_alphanumeric() const noexcept
{
    const CharType* src = mData.c_str();
    while (areg::is_alphanumeric<CharType>(*src))
        ++src;

    return (*src == EmptyChar); // reached end of the string.
}

template<typename CharType>
inline bool StringBase<CharType>::is_name_char(const CharType checkChar) const noexcept
{
    // initialize list of symbols for the valid names.
    constexpr CharType symbols[] = { '_', '\0'};
    return areg::is_alphanumeric<CharType>(checkChar) || areg::is_one_of<CharType>(checkChar, symbols);
}

template<typename CharType>
inline bool StringBase<CharType>::is_valid_name() const noexcept
{
    if (mData.empty())
        return false;

    const CharType* src = mData.c_str();
    while (is_name_char(*src))
        ++src;

    return (*src == EmptyChar);
}

template<typename CharType>
inline bool StringBase<CharType>::is_empty() const noexcept
{
    return mData.empty();
}

template<typename CharType>
inline areg::CharCount StringBase<CharType>::length() const noexcept
{
    return static_cast<areg::CharCount>(mData.length());
}

template<typename CharType>
inline areg::CharCount StringBase<CharType>::capacity() const noexcept
{
    return static_cast<areg::CharCount>(mData.capacity());
}

template<typename CharType>
inline uint32_t StringBase<CharType>::space() const noexcept
{
    return static_cast<uint32_t>((mData.length() + 1) * sizeof(CharType));
}

template<typename CharType>
inline const CharType* StringBase<CharType>::buffer(areg::CharPos startAt /*= areg::START_POS*/) const noexcept
{
    areg::CharCount len = static_cast<areg::CharCount>(mData.size());
    startAt = (startAt == areg::END_POS) || (startAt > len) ? len : startAt;
    return (mData.c_str() + startAt);
}

template<typename CharType>
inline CharType* StringBase<CharType>::buffer(areg::CharPos startAt /*= areg::START_POS*/) noexcept
{
    areg::CharCount len = static_cast<areg::CharCount>(mData.size());
    startAt = (startAt == areg::END_POS) || (startAt > len) ? len : startAt;
    return (mData.data() + startAt);
}

template<typename CharType>
inline const CharType* StringBase<CharType>::as_string() const noexcept
{
    return mData.c_str();
}

template<typename CharType>
inline const std::basic_string<CharType>& StringBase<CharType>::data() const noexcept
{
    return mData;
}

template<typename CharType>
inline bool StringBase<CharType>::is_valid_position(areg::CharPos pos) const noexcept
{
    return (pos >= areg::START_POS) && (pos < static_cast<areg::CharPos>(mData.length()));
}

template<typename CharType>
inline bool StringBase<CharType>::is_invalid_position(areg::CharPos pos) const noexcept
{
    return (pos < areg::START_POS) || (pos > static_cast<areg::CharPos>(mData.length()));
}

template<typename CharType>
inline bool StringBase<CharType>::is_last_position(areg::CharPos pos) const noexcept
{
    return (pos == areg::END_POS) || (pos >= static_cast<areg::CharPos>(mData.length()));
}

template<typename CharType>
inline bool StringBase<CharType>::is_first_position(areg::CharPos pos) const noexcept
{
    return (!mData.empty() && (pos == areg::START_POS));
}

template<typename CharType>
inline void StringBase<CharType>::clear() noexcept
{
    mData.clear();
}

template<typename CharType>
inline void StringBase<CharType>::free_extra()
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
void StringBase<CharType>::find_word(areg::CharPos& startAt, areg::CharPos& endAt) const  noexcept
{
    areg::CharPos pos = startAt;

    if (is_invalid_position(pos) || mData.empty())
    {
        endAt   = areg::INVALID_POS;
        startAt = areg::END_POS;
        return;
    }

    const CharType* base = as_string();
    const CharType* buf  = buffer(pos);

    // Skip leading non-word characters.
    while ((*buf != EmptyChar) && !is_name_char(*buf))
        ++buf;

    if (*buf == EmptyChar)
    {
        endAt   = areg::INVALID_POS;
        startAt = areg::END_POS;
        return;
    }

    // Mark the word start, then advance past word characters to find the end.
    startAt = static_cast<areg::CharPos>(buf - base);
    while (is_name_char(*buf))
        ++buf;

    endAt = (*buf != EmptyChar) ? static_cast<areg::CharPos>(buf - base) : areg::END_POS;
}

template<typename CharType>
void StringBase<CharType>::extract_word(StringBase<CharType>& word, areg::CharPos& startAt, areg::CharPos& endAt) const
{
    find_word(startAt, endAt);
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
areg::CharPos StringBase<CharType>::find_one_of( const CharType* chars, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
{
    if (is_invalid_position(startPos) || areg::is_empty<CharType>(chars))
        return areg::INVALID_POS;

    // find_first_of is SIMD-optimized in major STLs; delegates to memchr-class instructions.
    const auto pos = mData.find_first_of(chars, static_cast<uint32_t>(startPos));
    return (pos != std::basic_string<CharType>::npos ? static_cast<areg::CharPos>(pos) : areg::END_POS);
}

template<typename CharType>
areg::CharPos StringBase<CharType>::find_first( CharType chSearch
                                               , areg::CharPos startPos /*= areg::START_POS*/
                                               , bool caseSensitive /*= true*/) const noexcept
{
    if (is_valid_position(startPos) == false)
        return areg::INVALID_POS;

    if (caseSensitive)
    {
        // std::basic_string::find() is SIMD-optimized in major STLs.
        const auto pos = mData.find(chSearch, static_cast<uint32_t>(startPos));
        return (pos != std::basic_string<CharType>::npos ? static_cast<areg::CharPos>(pos) : areg::END_POS);
    }

    const CharType* str = buffer(startPos);
    const CharType chUpper = areg::make_upper<CharType>(chSearch);
    const CharType chLower = areg::make_lower<CharType>(chSearch);

    while ((*str != EmptyChar) && (*str != chUpper) && (*str != chLower))
    {
        ++str;
    }

    return (*str != EmptyChar ? static_cast<areg::CharPos>(str - as_string()) : areg::END_POS);
}

template<typename CharType>
areg::CharPos StringBase<CharType>::find_first( const CharType* phrase
                                               , areg::CharPos startPos /*= areg::START_POS*/
                                               , bool caseSensitive   /*= true*/
                                               , bool wholeWord       /*= false*/) const noexcept
{
    if (is_invalid_position(startPos) || areg::is_empty<CharType>(phrase))
        return areg::INVALID_POS;

    if (caseSensitive && !wholeWord)
    {
        return find_first_phrase(phrase, areg::COUNT_ALL, startPos);
    }
    else if (!wholeWord)
    {
        return find_phrase(phrase, startPos);
    }
    else
    {
        return find_first_word(phrase, caseSensitive, startPos);
    }
}

template<typename CharType>
areg::CharPos StringBase<CharType>::find_first( const StringBase<CharType> & phrase
                                               , areg::CharPos startPos /*= areg::START_POS*/
                                               , bool caseSensitive         /*= true*/
                                               , bool wholeWord             /*= false*/) const noexcept
{
    if (is_invalid_position(startPos) || phrase.is_empty())
        return areg::INVALID_POS;

    if (caseSensitive && !wholeWord)
    {
        return find_first_phrase(phrase.as_string(), phrase.length(), startPos);
    }
    else if (!wholeWord)
    {
        return find_phrase(phrase.mData, startPos);
    }
    else
    {
        return find_first_word(phrase.mData, caseSensitive, startPos);
    }
}

template<typename CharType>
areg::CharPos StringBase<CharType>::find_last(CharType chSearch, areg::CharPos startPos /*= areg::END_POS*/, bool caseSensitive /*= true*/) const noexcept
{
    if (mData.empty())
        return areg::INVALID_POS;

    if (caseSensitive)
    {
        // std::basic_string::rfind() is SIMD-optimized in major STLs.
        const uint32_t rpos = (startPos == areg::END_POS) ? static_cast<uint32_t>(std::basic_string<CharType>::npos) : static_cast<uint32_t>(startPos);
        const auto pos = mData.rfind(chSearch, rpos);
        return (pos != std::basic_string<CharType>::npos ? static_cast<areg::CharPos>(pos) : areg::END_POS);
    }

    startPos = (startPos == areg::END_POS) ? length() - 1 : startPos;

    const CharType* begin = as_string();
    const CharType* end = buffer(startPos);
    const CharType chUpper = areg::make_upper<CharType>(chSearch);
    const CharType chLower = areg::make_lower<CharType>(chSearch);

    while ((end >= begin) && (*end != chUpper) && (*end != chLower))
    {
        --end;
    }

    return (end >= begin ? static_cast<areg::CharPos>(end - begin) : areg::END_POS);
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::find_last(const CharType* phrase, areg::CharCount phraseCount, areg::CharPos startPos, bool caseSensitive) const noexcept
{
    if (((startPos != areg::END_POS) && is_invalid_position(startPos)) || areg::is_empty<CharType>(phrase) || (phraseCount == 0))
        return areg::INVALID_POS;

    const areg::CharCount strLen = length();
    if (strLen < phraseCount)
        return areg::END_POS;

    if (caseSensitive)
    {
        // mData.rfind() is SIMD-optimized (delegates to memmem/wmemmem) and handles
        // all CharType; no constexpr guard needed since mData is always available.
        using SizeT = typename std::basic_string<CharType>::size_type;
        const SizeT searchPos = (startPos == areg::END_POS)
            ? std::basic_string<CharType>::npos
            : static_cast<SizeT>(startPos);
        const auto pos = mData.rfind(phrase, searchPos, static_cast<SizeT>(phraseCount));
        return (pos != std::basic_string<CharType>::npos) ? static_cast<areg::CharPos>(pos) : areg::END_POS;
    }

    // Case-insensitive: backward scan using compare().
    // maxStart: last valid phrase-start position (strLen - phraseCount).
    const areg::CharPos maxStart = static_cast<areg::CharPos>(strLen - phraseCount);
    const areg::CharPos searchFrom = (startPos == areg::END_POS) ? maxStart
                                   : (startPos < maxStart)       ? startPos
                                                                  : maxStart;
    for (areg::CharPos pos = searchFrom; pos >= 0; --pos)
    {
        if (compare(pos, phrase, phraseCount, false) == areg::Ordering::Equal)
            return pos;
    }

    return areg::END_POS;
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::find_last(const CharType* phrase, areg::CharPos startPos /*= areg::END_POS*/, bool caseSensitive /*= true*/) const noexcept
{
    return find_last(phrase, areg::string_length<CharType>(phrase), startPos, caseSensitive);
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::find_last(const StringBase<CharType> & phrase, areg::CharPos startPos /*= areg::END_POS*/, bool caseSensitive /*= true*/) const noexcept
{
    return find_last(phrase, phrase.length(), startPos, caseSensitive);
}

template<typename CharType>
areg::Ordering StringBase<CharType>::compare( const CharType* what
                                            , areg::CharPos startAt /*= areg::START_POS*/
                                            , areg::CharCount count /*= areg::COUNT_ALL*/
                                            , bool caseSensitive /*= true*/) const noexcept
{
    areg::Ordering result{ areg::Ordering::Smaller };
    if (is_valid_position(startAt) == false)
        return result;

    if (count == areg::COUNT_ALL)
    {
        count = areg::string_length<CharType>(what);
    }

    const areg::CharPos length = static_cast<areg::CharPos>(mData.length()) - startAt;
    if ((length == count) && (what != nullptr))
    {
        if (caseSensitive)
        {
            // mData.compare() delegates to memcmp/wmemcmp — SIMD-optimized.
            const int cmp = mData.compare( static_cast<std::size_t>(startAt)
                                         , static_cast<std::size_t>(count)
                                         , what
                                         , static_cast<std::size_t>(count) );
            if (cmp == 0)   return areg::Ordering::Equal;
            return (cmp < 0) ? areg::Ordering::Smaller : areg::Ordering::Bigger;
        }

        // Case-insensitive: char-by-char with fold-to-lower.
        const CharType* current = buffer(startAt);
        const CharType* other = what;
        result = areg::Ordering::Equal;

        CharType ch1{ EmptyChar };
        CharType ch2{ EmptyChar };
        do
        {
            ch1 = areg::make_lower<CharType>(*current++);
            ch2 = areg::make_lower<CharType>(*other++);
            if (ch1 != ch2)
                break;
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
inline areg::Ordering StringBase<CharType>::compare(const StringBase<CharType>& other, bool caseSensitive /*= true*/) const noexcept
{
    return compare(other.mData, caseSensitive);
}

template<typename CharType>
inline areg::Ordering StringBase<CharType>::compare(const std::basic_string<CharType>& other, bool caseSensitive /*= true*/) const noexcept
{
    return compare(other.c_str(), areg::START_POS, static_cast<areg::CharCount>(other.length()), caseSensitive);
}

template<typename CharType>
inline areg::Ordering StringBase<CharType>::compare(const std::basic_string_view<CharType>& other, bool caseSensitive /*= true*/) const noexcept
{
    return compare(other.data(), areg::START_POS, static_cast<areg::CharCount>(other.length()), caseSensitive);
}

template<typename CharType>
inline areg::Ordering StringBase<CharType>::compare( const StringBase<CharType>& other
                                                   , areg::CharPos startPos /*= areg::START_POS*/
                                                   , areg::CharCount count  /*= areg::COUNT_ALL*/
                                                   , bool caseSensitive         /*= true*/) const noexcept
{
    return compare(other.mData, startPos, count, caseSensitive);
}

template<typename CharType>
inline areg::Ordering StringBase<CharType>::compare( const std::basic_string<CharType>& other
                                                   , areg::CharPos startAt  /*= areg::START_POS*/
                                                   , areg::CharCount count  /*= areg::COUNT_ALL*/
                                                   , bool caseSensitive         /*= true*/) const noexcept
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
    if (is_valid_position(startPos))
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

    if (is_invalid_position(startPos) || strPhrase.empty())
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
    if (is_invalid_position(startPos))
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
inline StringBase<CharType> StringBase<CharType>::left_side(areg::CharCount charCount) const
{
    StringBase<CharType> result;
    StringBase<CharType>::substring(result, areg::START_POS, charCount);
    return result;
}

template<typename CharType>
inline StringBase<CharType> StringBase<CharType>::right_side(areg::CharCount charCount) const
{
    StringBase<CharType> result;

    areg::CharCount len = length();
    areg::CharPos pos = charCount < len ? len - charCount : areg::START_POS;
    substring(result, pos, areg::COUNT_ALL);

    return result;
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::assign(const CharType* source, areg::CharCount count /*= areg::COUNT_ALL */)
{
    if ( source != nullptr )
    {
        mData.assign( source, static_cast<uint32_t>(count == areg::COUNT_ALL ? areg::string_length<CharType>( source ) : count) );
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
    count = count == areg::COUNT_ALL ? areg::string_length<CharType>(source) : count;
    mData.append(source, static_cast<uint32_t>(count));
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
StringBase<CharType>& StringBase<CharType>::move_to(areg::CharPos startPos, int32_t move_to)
{
    if (move_to < 0)
    {
        areg::CharCount count = static_cast<areg::CharCount>(-1 * move_to);
        areg::CharPos dstPos  = startPos >= count ? startPos + move_to : areg::START_POS;
        mData.erase(static_cast<uint32_t>(dstPos), static_cast<uint32_t>(count));
    }
    else if (move_to > 0)
    {
        mData.insert(static_cast<uint32_t>(startPos), static_cast<uint32_t>(move_to), '@');
    }

    return (*this);
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::insert_at(CharType source, areg::CharPos atPos, areg::CharCount count /*= 1*/)
{
    mData.insert(static_cast<uint32_t>(atPos), static_cast<uint32_t>(count), source);
    return (*this);
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::insert_at(const CharType* source, areg::CharPos atPos, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    if (source != nullptr && is_valid_position(atPos))
    {
        mData.insert(static_cast<uint32_t>(atPos), source, count == areg::COUNT_ALL ? static_cast<uint32_t>(areg::string_length(source)) : static_cast<uint32_t>(count));
    }

    return (*this);
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::insert_at( const std::basic_string<CharType>& source
                                                       , areg::CharPos atDstPos
                                                       , areg::CharPos atSrcPos /*= areg::START_POS*/
                                                       , areg::CharCount count  /*= areg::COUNT_ALL*/)
{
    if (is_valid_position(atDstPos) && (atSrcPos < static_cast<areg::CharPos>(source.length())))
    {
        count = count == areg::COUNT_ALL ? static_cast<areg::CharPos>(source.length() - static_cast<uint32_t>(atSrcPos)) : count;
        mData.insert(static_cast<uint32_t>(atDstPos), source, static_cast<uint32_t>(atSrcPos), static_cast<uint32_t>(count));
    }

    return (*this);
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::insert_at( const std::basic_string_view<CharType>& source
                                                       , areg::CharPos atDstPos
                                                       , areg::CharPos atSrcPos /*= areg::START_POS*/
                                                       , areg::CharCount count  /*= areg::COUNT_ALL*/)
{
    if (is_valid_position(atDstPos) && (atSrcPos < static_cast<areg::CharPos>(source.length())))
    {
        count = count == areg::COUNT_ALL ? static_cast<areg::CharPos>(source.length() - static_cast<uint32_t>(atSrcPos)) : count;
        mData.insert(static_cast<uint32_t>(atDstPos), source, static_cast<uint32_t>(atSrcPos), static_cast<uint32_t>(count));
    }

    return (*this);
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::insert_at( const StringBase<CharType>& source
                                                            , areg::CharPos atDstPos
                                                            , areg::CharPos atSrcPos /*= areg::START_POS*/
                                                            , areg::CharCount count /*= areg::COUNT_ALL*/)
{
    return insert_at(source.mData, atDstPos, atSrcPos, count);
}

template<typename CharType>
StringBase<CharType>& StringBase<CharType>::replace(CharType chSearch, CharType chReplace, areg::CharPos startPos /*= areg::START_POS*/, bool replaceAll /*= true*/)
{
    if (!is_valid_position(startPos))
        return (*this);

    CharType* dst = mData.data() + startPos;
    while (*dst != EmptyChar)
    {
        if (*dst == chSearch)
        {
            *dst = chReplace;
            if (!replaceAll)
                break;
        }

        ++dst;
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
    if (!is_valid_position(startPos) || areg::is_empty(strSearch))
        return (*this);

    areg::CharPos lenSearch  = areg::string_length<CharType>(strSearch);
    areg::CharPos lenReplace = areg::string_length<CharType>(strReplace);
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

        if ( !replaceAll || (pos >= static_cast<uint32_t>(mData.length())) )
        {
            break;
        }

        pos = static_cast<uint32_t>(mData.find(strSearch, pos));
    }

    return (*this);
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::replace( const std::basic_string_view<CharType>& strSearch
                                                          , const std::basic_string_view<CharType>& strReplace
                                                          , areg::CharPos startPos  /*= areg::START_POS*/
                                                          , bool replaceAll             /*= true*/)
{
    if (!is_valid_position(startPos) || strSearch.empty())
        return (*this);
    
    areg::CharPos lenSearch  = static_cast<areg::CharPos>(strSearch.length());
    areg::CharPos lenReplace = static_cast<areg::CharPos>(strReplace.length());
    uint32_t pos = static_cast<uint32_t>(mData.find(strSearch, static_cast<uint32_t>(startPos)));
    while (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos))
    {
        if ( lenReplace != 0 )
        {
            mData.replace( pos, static_cast<uint32_t>(lenSearch), strReplace.data(), static_cast<uint32_t>(lenReplace) );
            pos += static_cast<uint32_t>(lenReplace);
        }
        else
        {
            mData.erase( pos, static_cast<uint32_t>(lenSearch) );
        }

        if (!replaceAll || (pos >= static_cast<uint32_t>(mData.length())) )
        {
            break;
        }

        pos = static_cast<uint32_t>(mData.find(strSearch, pos));
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
    if (!is_valid_position(startPos) || strSearch.empty())
        return (*this);
    
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

        if ( !replaceAll || (pos >= static_cast<uint32_t>(mData.length( ))) )
        {
            break;
        }

        pos = static_cast<uint32_t>(mData.find(strSearch, pos));
    }

    return (*this);
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::replace( areg::CharPos startPos
                                                          , areg::CharCount charsRemove
                                                          , const CharType* strReplace
                                                          , areg::CharCount count /*= areg::COUNT_ALL*/)
{
    if (is_valid_position(startPos))
    {
        areg::CharPos lenReplace = areg::string_length<CharType>(strReplace);
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
    if (is_valid_position(startPos))
    {
        mData.replace(static_cast<uint32_t>(startPos), static_cast<uint32_t>(charsRemove), strReplace);
    }

    return (*this);
}

template<typename CharType>
StringBase<CharType>& StringBase<CharType>::remove(areg::CharPos startPos, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    if (!is_valid_position(startPos))
        return (*this);

    if (count == areg::COUNT_ALL)
    {
        mData.erase(static_cast<uint32_t>(startPos));
    }
    else if (count <= (static_cast<areg::CharCount>(mData.length()) - startPos))
    {
        mData.erase(static_cast<uint32_t>(startPos), static_cast<uint32_t>(count));
    }

    return (*this);
}

template<typename CharType>
StringBase<CharType>& StringBase<CharType>::remove(const CharType chRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
{
    if (!is_valid_position(startPos))
        return (*this);

    uint32_t pos = static_cast<uint32_t>(mData.find(chRemove, static_cast<uint32_t>(startPos)));
    for ( ; pos != static_cast<uint32_t>(std::basic_string<CharType>::npos); pos = static_cast<uint32_t>(mData.find(chRemove, static_cast<uint32_t>(pos))))
    {
        mData.erase(pos, 1);
        if ( !removeAll || (pos >= static_cast<uint32_t>(mData.length( ))) )
            break;
    }

    return (*this);
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::remove( const CharType* strRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
{
    if (!areg::is_empty<CharType>(strRemove))
        remove(std::basic_string<CharType>(strRemove), startPos, removeAll);

    return (*this);
}

template<typename CharType>
StringBase<CharType>& StringBase<CharType>::remove( const std::basic_string<CharType>& strRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
{
    if (!is_valid_position(startPos))
        return (*this);
    
    uint32_t len = static_cast<uint32_t>(strRemove.length());
    uint32_t pos = static_cast<uint32_t>(mData.find(strRemove, static_cast<uint32_t>(startPos)));
    for (; pos != static_cast<uint32_t>(std::basic_string<CharType>::npos); pos = static_cast<uint32_t>(mData.find(strRemove, pos)))
    {
        mData.erase(pos, len);
        if ( !removeAll || (pos >= static_cast<uint32_t>(mData.length( ))) )
        {
            break;
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
inline CharType StringBase<CharType>::at(areg::CharPos atPos) const noexcept
{
    return (is_valid_position(atPos) ? mData.at(static_cast<uint32_t>(atPos)) : static_cast<CharType>(areg::EndOfString));
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::set_at(CharType ch, areg::CharPos atPos /*= areg::END_POS*/) noexcept
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
inline StringBase<CharType>& StringBase<CharType>::trim_left() noexcept
{
    if (mData.empty())
        return (*this);

    uint32_t count = 0;
    for (const auto& ch : mData)
    {
        if (!areg::is_whitespace<CharType>(ch))
            break;

        ++count;
    }

    if (count != 0)
    {
        mData.erase(0, count);
    }

    return (*this);
}

template<typename CharType>
inline void StringBase<CharType>::trim_left(StringBase<CharType>& strResult) const
{
    trim_left(strResult.mData);
}

template<typename CharType>
inline void StringBase<CharType>::trim_left(std::basic_string<CharType>& strResult) const
{
    strResult.clear();
    if (mData.empty())
        return;

    uint32_t count = 0;
    for (const auto& ch : mData)
    {
        if (!areg::is_whitespace<CharType>(ch))
            break;

        ++count;
    }

    if (count != 0)
    {
        strResult.assign(buffer(static_cast<areg::CharCount>(count)), static_cast<uint32_t>(mData.length() - count));
    }
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::trim_right() noexcept
{
    if (mData.empty())
        return (*this);

    uint32_t count = 0;
    for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
    {
        if (!areg::is_whitespace<CharType>(*cit))
            break;

        ++count;
    }

    if (count != 0)
    {
        mData.erase(mData.length() - count, std::basic_string<CharType>::npos);
    }

    return (*this);
}

template<typename CharType>
inline void StringBase<CharType>::trim_right(StringBase<CharType>& strResult) const
{
    trim_right(strResult.mData);
}

template<typename CharType>
inline void StringBase<CharType>::trim_right(std::basic_string<CharType>& strResult) const
{
    strResult.clear();
    if (mData.empty())
        return;

    uint32_t count = 0;
    for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
    {
        if (!areg::is_whitespace<CharType>(*cit))
            break;

        ++count;
    }

    if (count != 0)
    {
        strResult.assign(mData, 0, mData.length() - count);
    }
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::trim_all() noexcept
{
    if (mData.empty())
        return (*this);

    uint32_t length = static_cast<uint32_t>(mData.length());
    uint32_t left = 0;
    for (auto cit = mData.cbegin(); cit != mData.cend(); ++cit)
    {
        if (!areg::is_whitespace<CharType>(*cit))
            break;

        ++left;
    }

    uint32_t right = 0;
    for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
    {
        if (!areg::is_whitespace<CharType>(*cit))
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

    return (*this);
}

template<typename CharType>
inline void StringBase<CharType>::trim_all(StringBase<CharType>& strResult) const
{
    trim_all(strResult.mData);
}

template<typename CharType>
inline void StringBase<CharType>::trim_all(std::basic_string<CharType>& strResult) const
{
    strResult.clear();
    if (mData.empty())
        return;

    uint32_t length = static_cast<uint32_t>(mData.length());
    uint32_t left = 0;
    for (auto cit = mData.cbegin(); cit != mData.cend(); ++cit)
    {
        if (!areg::is_whitespace<CharType>(*cit))
            break;

        ++left;
    }

    uint32_t right = 0;
    for (auto cit = mData.crbegin(); cit != mData.crend(); ++cit)
    {
        if (!areg::is_whitespace<CharType>(*cit))
            break;

        ++right;
    }

    if ((left + right) < length)
    {
        strResult = mData.substr(left, length - right);
    }
}

template<typename CharType>
inline StringBase<CharType> & StringBase<CharType>::make_lower() noexcept
{
    for (CharType * src = mData.data(); *src != StringBase<CharType>::EmptyChar; ++src)
    {
        *src = areg::make_lower<CharType>(*src);
    }

    return (*this);
}

template<typename CharType>
inline StringBase<CharType> & StringBase<CharType>::make_upper() noexcept
{
    for (CharType * src = mData.data(); *src != StringBase<CharType>::EmptyChar; ++src)
    {
        *src = areg::make_upper<CharType>(*src);
    }

    return (*this);
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::read_line(StringBase<CharType>& strResult, areg::CharPos startPos /*= areg::START_POS*/) const
{
    return read_line(strResult.mData, startPos);
}

template<typename CharType>
areg::CharPos StringBase<CharType>::read_line(std::basic_string<CharType>& strResult, areg::CharPos startPos /*= areg::START_POS*/) const
{
    if (!is_valid_position(startPos))
        return areg::END_POS;

    const CharType* begin = buffer(startPos);
    while (areg::is_eol<CharType>(*begin) && (*begin != EmptyChar))
    {
        // escape end of line symbols at the begin.
        ++begin;
    }

    if (*begin == EmptyChar)
        return areg::END_POS;

    const CharType* str = begin;
    while ((areg::is_new_line<CharType>(*str) == false) && (*str != EmptyChar))
    {
        // move until reach end of line
        ++str;
    }

    // copy the line
    strResult.assign(begin, static_cast<uint32_t>(str - begin));
    while (areg::is_eol<CharType>(*str) && (*str != EmptyChar))
    {
        // find next line or reach end of string
        ++str;
    }

    // if reached end of string, return END_POS, otherwise, return the next position in the string where new not empty line starts.
    return (*str == EmptyChar ? areg::END_POS : static_cast<areg::CharPos>(str - mData.c_str()));
}

template<typename CharType>
inline StringBase<CharType>& StringBase<CharType>::make_alphanumeric() noexcept
{
    if (mData.empty())
        return (*this);

    CharType* begin = buffer(areg::START_POS);
    CharType* dst = begin;
    for (const CharType* src = begin; *src != static_cast<CharType>(areg::EndOfString); ++src)
    {
        if (areg::is_alphanumeric<CharType>(*src))
            *dst++ = *src;
    }

    *dst = static_cast<CharType>(areg::EndOfString);
    mData.resize(static_cast<uint32_t>(dst - begin));

    return (*this);
}

/// //////////////////////////////////////////////////////////////////////////
// Template class StringBase Helper
//////////////////////////////////////////////////////////////////////////

template<typename CharType>
inline areg::Ordering StringBase<CharType>::compare( areg::CharPos   startPos
                                                   , const CharType* strOther
                                                   , areg::CharCount count          /*= areg::COUNT_ALL */
                                                   , bool            caseSensitive  /*= true            */ ) const noexcept
{
    if (caseSensitive)
    {
        return compare_exact(startPos, strOther, count);
    }
    else
    {
        return compare_ignore_case(startPos, strOther, count);
    }
}

template<typename CharType>
inline areg::Ordering StringBase<CharType>::compare_exact( areg::CharPos   startPos
                                                         , const CharType* strOther
                                                         , areg::CharCount count    /*= areg::COUNT_ALL */ ) const noexcept
{
    areg::Ordering result = areg::Ordering::Smaller;
    count = count == areg::COUNT_ALL ? areg::string_length<CharType>(strOther) : count;
    if (is_valid_position(startPos))
    {
        areg::CharCount len = static_cast<areg::CharCount>(length() - startPos);
        if (count <= len)
        {
            result = areg::mem_compare(buffer(startPos), strOther, static_cast<uint32_t>(count));
        }
    }

    return result;
}

template<typename CharType>
inline areg::Ordering StringBase<CharType>::compare_ignore_case( areg::CharPos   startPos
                                                               , const CharType* strOther
                                                               , areg::CharCount count      /*= areg::COUNT_ALL */ ) const noexcept
{
    if (!is_valid_position(startPos))
        return areg::Ordering::Smaller;

    count = count == areg::COUNT_ALL ? areg::string_length<CharType>(strOther) : count;
    areg::CharCount len = static_cast<areg::CharCount>(length() - startPos);
    if (count > len)
        return areg::Ordering::Smaller;

    CharType chLeft{ '\0' };
    CharType chRight{ '\0' };

    const CharType* left_side = buffer(startPos);
    const CharType* right_side = strOther;

    while (count-- > 0)
    {
        chLeft  = areg::make_lower<CharType>(*left_side++);
        chRight = areg::make_lower<CharType>(*right_side++);
        if (chLeft != chRight)
        {
            break;
        }
    }

    if (chLeft == chRight)
        return areg::Ordering::Equal;
    else if (chLeft > chRight)
        return areg::Ordering::Bigger;
    else
        return areg::Ordering::Smaller;
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::replace_with( areg::CharPos   startPos
                                                       , areg::CharCount count
                                                       , const CharType *    strReplace
                                                       , areg::CharCount lenReplace )
{
    if ((startPos == areg::INVALID_POS) || (startPos == areg::END_POS))
        return areg::INVALID_POS;

    int32_t diff = static_cast<int32_t>(lenReplace - count);
    areg::CharPos endPos = startPos + count;
    move_to( endPos, diff );
    // char_traits::copy is SIMD-optimized in major STLs (equivalent to memcpy).
    std::char_traits<CharType>::copy(buffer(startPos), strReplace, static_cast<uint32_t>(lenReplace));

    return (endPos + diff);
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::replace_with( const CharType * strOrigin
                                                       , areg::CharCount lenOrigin
                                                       , const CharType * strReplace
                                                       , areg::CharCount lenReplace
                                                       , areg::CharPos startPos )
{
    return replace_with( find_first( strOrigin, startPos, true ), lenOrigin, strReplace, lenReplace);
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::find_first_phrase( const CharType* phrase
                                                            , areg::CharCount count     /* = areg::COUNT_ALL */
                                                            , areg::CharPos startPos    /* = areg::START_POS */) const noexcept
{
    if (!is_valid_position(startPos) || areg::is_empty<CharType>(phrase))
        return areg::INVALID_POS;

    uint32_t pos = (count == areg::COUNT_ALL) ? 
                    static_cast<uint32_t>(mData.find(phrase, static_cast<uint32_t>(startPos))) :
                    static_cast<uint32_t>(mData.find(phrase, static_cast<uint32_t>(startPos), static_cast<uint32_t>(count)));

    return (pos != static_cast<uint32_t>(std::basic_string<CharType>::npos) ? static_cast<areg::CharPos>(pos) : areg::END_POS);
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::find_phrase(const CharType* phrase, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
{
    if (phrase == nullptr || is_invalid_position(startPos) || mData.empty())
        return areg::INVALID_POS;

    // Search directly on raw pointers to avoid constructing a temporary std::basic_string.
    const CharType* phraseEnd = phrase;
    while (*phraseEnd != static_cast<CharType>(areg::EndOfString))
        ++phraseEnd;

    if (phraseEnd == phrase)
        return areg::INVALID_POS;

    if ((mData.length() - static_cast<uint32_t>(startPos)) < static_cast<uint32_t>(phraseEnd - phrase))
        return areg::END_POS;

    auto it = std::search( mData.begin() + static_cast<int32_t>(startPos), mData.end()
                         , phrase, phraseEnd
                         , [](const CharType& ch1, const CharType& ch2)
                           { return (areg::make_lower<CharType>(ch1) == areg::make_lower<CharType>(ch2)); });

    return (it != mData.end() ? static_cast<areg::CharPos>(std::distance(mData.begin(), it)) : areg::END_POS);
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::find_phrase(const std::basic_string<CharType>& phrase, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
{
    if (is_invalid_position(startPos) || phrase.empty())
        return areg::INVALID_POS;
    else if (mData.empty())
        return areg::END_POS;
    else if ((mData.length() - static_cast<uint32_t>(startPos)) < phrase.length())
        return areg::END_POS;

    auto it = std::search( mData.begin() + static_cast<int32_t>(startPos), mData.end()
                         , phrase.begin(), phrase.end()
                         , [](const CharType& ch1, const CharType& ch2) { return (areg::make_lower<CharType>(ch1) == areg::make_lower<CharType>(ch2)); }
                         );

    return (it != mData.end() ? static_cast<areg::CharPos>(std::distance(mData.begin(), it)) : areg::END_POS);
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::find_first_word(const CharType* word, bool caseSensitive, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
{
    return (word != nullptr ? find_first_word(std::basic_string<CharType>(word), caseSensitive, startPos) : areg::INVALID_POS);
}

template<typename CharType>
inline areg::CharPos StringBase<CharType>::find_first_word(const std::basic_string<CharType>& word, bool caseSensitive, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
{
    if (is_invalid_position(startPos) || word.empty())
        return areg::INVALID_POS;
    else if (mData.empty())
        return areg::END_POS;
    else if ((mData.length() - static_cast<uint32_t>(startPos)) < word.length())
        return areg::END_POS;


    auto search = [caseSensitive](const CharType& ch1, const CharType& ch2) { return (caseSensitive ? ch1 == ch2 : areg::make_lower<CharType>(ch1) == areg::make_lower<CharType>(ch2)); };
    areg::CharPos result = areg::END_POS;
    while (result == areg::END_POS)
    {
        auto it = std::search( mData.begin() + static_cast<int32_t>(startPos), mData.end()
                             , word.begin(), word.end()
                             , search);

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
        bool isBegin = !is_name_char(chBegin) || !is_name_char(chPrev);

        // Take last char
        it += static_cast<int32_t>(word.length());
        CharType chEnd  = it != mData.end() ? *it : EmptyChar;
        // Take next char if not EOS
        CharType chNext = (it != mData.end()) && ((it + 1) != mData.end()) ? *(it + 1) : EmptyChar;
        // One of them must be end of the word.
        // Example: search "word" or "word!" in sentence "This is a word!";
        bool isEnd      = !is_name_char(chEnd) || !is_name_char(chNext);

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
inline bool StringBase<CharType>::is_name_char(const CharType checkChar, std::locale& loc) noexcept
{
    // initialize list of symbols for the valid names.
    constexpr CharType symbols[] = { '_', '\0' };
    return std::isalnum(checkChar, loc) || areg::is_one_of<CharType>(checkChar, symbols);
}

template<typename CharType>
inline std::vector<StringBase<CharType>> StringBase<CharType>::split(CharType delimiter) const
{
    std::vector<StringBase<CharType>> result;
    size_t start{ 0 };
    const size_t len{ mData.length() };
    while (start < len)
    {
        size_t pos = mData.find(delimiter, start);
        if (pos != std::basic_string<CharType>::npos)
        {
            result.push_back(StringBase<CharType>(mData.c_str() + start, static_cast<areg::CharCount>(pos - start)));
            start = pos + 1;
        }
        else
        {
            result.push_back(StringBase<CharType>(mData.c_str() + start, static_cast<areg::CharCount>(len - start)));
            break;
        }
    }

    return result;
}

template<typename CharType>
inline std::vector<StringBase<CharType>> StringBase<CharType>::split(const StringBase<CharType> & delimiter) const
{
    std::vector<StringBase<CharType>> result;
    if (delimiter.is_empty())
    {
        result.push_back(*this);
    }
    else
    {
        const size_t skip   { static_cast<size_t>(delimiter.length()) };
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

    return result;
}

template<typename CharType>
inline bool StringBase<CharType>::starts_with(const StringBase<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return starts_with(phrase.mData, isCaseSensitive);
}

template<typename CharType>
inline bool StringBase<CharType>::starts_with(const std::basic_string<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return (phrase.length() <= mData.length() ? starts_with(phrase.c_str(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
}

template<typename CharType>
inline bool StringBase<CharType>::starts_with(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return (phrase.length() <= mData.length() ? starts_with(phrase.data(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
}

template<typename CharType>
inline bool StringBase<CharType>::starts_with(const CharType* phrase, bool isCaseSensitive /*= true*/, areg::CharCount count /*= areg::COUNT_ALL*/) const noexcept
{
    count = count != areg::COUNT_ALL ? count : areg::string_length<CharType>(phrase);
    if (areg::is_empty<CharType>(phrase) || (count == 0) || (count > static_cast<areg::CharCount>(mData.length())))
    {
        return false;
    }
    else
    {
        return _has_phrase(mData.c_str(), phrase, count, isCaseSensitive ? nullptr : areg::LOCALE_DEFAULT);
    }
}

template<typename CharType>
inline bool StringBase<CharType>::ends_with(const StringBase<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return ends_with(phrase.mData, isCaseSensitive);
}

template<typename CharType>
inline bool StringBase<CharType>::ends_with(const std::basic_string<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return (phrase.length() <= mData.length() ? ends_with(phrase.c_str(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
}

template<typename CharType>
inline bool StringBase<CharType>::ends_with(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return (phrase.length() <= mData.length() ? ends_with(phrase.data(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
}

template<typename CharType>
inline bool StringBase<CharType>::ends_with(const CharType* phrase, bool isCaseSensitive /*= true*/, areg::CharCount count /*= areg::COUNT_ALL*/) const noexcept
{
    count = count != areg::COUNT_ALL ? count : areg::string_length<CharType>(phrase);
    if (areg::is_empty<CharType>(phrase) || (count == 0) || (count > static_cast<areg::CharCount>(mData.length())))
    {
        return false;
    }
    else
    {
        areg::CharCount skip = static_cast<areg::CharCount>(mData.length() - static_cast<uint32_t>(count));
        ASSERT(skip >= 0);
        return _has_phrase(mData.c_str() + skip, phrase, count, isCaseSensitive ? nullptr : areg::LOCALE_DEFAULT);
    }
}

template<typename CharType>
inline bool StringBase<CharType>::_has_phrase(const CharType* fullString, const CharType* phrase, areg::CharCount count, const char* locale) const noexcept
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
            if (areg::make_lower<CharType>(*phrase) != areg::make_lower<CharType>(*fullString))
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
    return (stream << output.data( ));
}

template<typename CT>
inline const std::istream & operator >> ( const std::istream & stream, StringBase<CT> & input )
{
    return (stream >> input.data( ));
}

} // namespace areg
#endif  // AREG_BASE_STRINGBASE_HPP
