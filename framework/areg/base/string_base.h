#ifndef AREG_BASE_STRINGBASE_H
#define AREG_BASE_STRINGBASE_H
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/string_base.h
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, String class template based on first 256 character in UTF-8 code page.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/StringDefs.hpp"

#include <iostream>
#include <locale>
#include <string>


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


}
#endif
