#ifndef AREG_BASE_STRINGBASE_HPP
#define AREG_BASE_STRINGBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/base/MathDefs.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <iostream>
#include <locale>
#include <string>
#include <string_view>
#include <type_traits>
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
 *          to set, append, insert, search and replace string, as well to get substring,
 *          to convert numbers and to format text. Every operation that returns a string
 *          returns the derived string type.
 *
 * \tparam  CharType    The type of character, either 'char' or 'wchar_t'.
 * \tparam  Derived     The string class derived from this template.
 **/
template<typename CharType, typename Derived>
class StringBase
{
    static_assert(std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>, "StringBase supports only char and wchar_t");

//////////////////////////////////////////////////////////////////////////
// Public constants and types
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The character type of the string with the other character width.
     **/
    using OtherCharType = std::conditional_t<std::is_same_v<CharType, char>, wchar_t, char>;

    /**
     * \brief   Empty character constant
     **/
    static constexpr CharType   EmptyChar       { static_cast<CharType>(areg::EndOfString) };

    /**
     * \brief   The empty string.
     **/
    static constexpr CharType   EmptyString[1]  { static_cast<CharType>(areg::EndOfString) };

    /**
     * \brief   New line constant
     **/
    static constexpr CharType   NewLine         { static_cast<CharType>(areg::EndOfLine) };

    /**
     * \brief   DOS format new line
     **/
    static constexpr CharType   NewLineDos[2]   {static_cast<CharType>('\r'), static_cast<CharType>('\n')};

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:

    inline StringBase() noexcept;

    inline StringBase( const CharType source );

    inline StringBase( const CharType * source );

    inline StringBase( const std::basic_string<CharType>& source );

    inline StringBase( std::basic_string_view<CharType> source );

    inline StringBase( const CharType * source, areg::CharCount count );

    inline StringBase( const StringBase & source );

    inline StringBase( StringBase && source ) noexcept;

    inline StringBase( std::basic_string<CharType>&& source ) noexcept;

    /**
     * \brief   Initializes the string from characters of the other width. A narrow character is
     *          widened as an unsigned value; a wide character is narrowed by truncation.
     **/
    inline StringBase( const OtherCharType * source );

    inline StringBase( const OtherCharType * source, areg::CharCount count );

    inline StringBase( const std::basic_string<OtherCharType>& source );

    template<typename OtherDerived>
    inline StringBase( const StringBase<OtherCharType, OtherDerived>& source );

    /**
     * \brief   Reserves space for a specified number of characters without modifying the size.
     *
     * \param   count       The number of characters to reserve. Size remains zero.
     **/
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
     * \brief   Converts string to a 32-bit unsigned integer hash value.
     **/
    explicit inline operator uint32_t() const noexcept;

    /**
     * \brief   Returns character at the given valid zero-based position. The position should not be neither
     *          negative, nor more than the string length.
     **/
    inline const CharType operator [] (int32_t index) const noexcept;

    inline Derived& operator = (const StringBase& src);
    inline Derived& operator = (const std::basic_string<CharType>& src);
    inline Derived& operator = (const std::basic_string_view<CharType>& src);
    inline Derived& operator = (const CharType* src);
    inline Derived& operator = (StringBase&& src) noexcept;
    inline Derived& operator = (std::basic_string<CharType>&& src) noexcept;
    inline Derived& operator = (const CharType src);
    inline Derived& operator = (const OtherCharType* src);
    inline Derived& operator = (const std::basic_string<OtherCharType>& src);
    inline Derived& operator = (const OtherCharType src);
    template<typename OtherDerived>
    inline Derived& operator = (const StringBase<OtherCharType, OtherDerived>& src);

    [[nodiscard]] inline bool operator == (const StringBase& other) const noexcept;
    [[nodiscard]] inline bool operator == (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator == (const std::basic_string_view<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator == (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator == (const CharType ch ) const noexcept;
    [[nodiscard]] inline bool operator == (const OtherCharType* other) const noexcept;
    [[nodiscard]] inline bool operator == (const std::basic_string<OtherCharType>& other) const noexcept;
    template<typename OtherDerived>
    [[nodiscard]] inline bool operator == (const StringBase<OtherCharType, OtherDerived>& other) const noexcept;

    [[nodiscard]] inline bool operator != (const StringBase& other) const noexcept;
    [[nodiscard]] inline bool operator != (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator != (const std::basic_string_view<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator != (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator != (const CharType ch) const noexcept;
    [[nodiscard]] inline bool operator != (const OtherCharType* other) const noexcept;
    [[nodiscard]] inline bool operator != (const std::basic_string<OtherCharType>& other) const noexcept;
    template<typename OtherDerived>
    [[nodiscard]] inline bool operator != (const StringBase<OtherCharType, OtherDerived>& other) const noexcept;

    [[nodiscard]] inline bool operator > (const StringBase& other) const noexcept;
    [[nodiscard]] inline bool operator > (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator > (const std::basic_string_view<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator > (const CharType* other) const noexcept;

    [[nodiscard]] inline bool operator < (const StringBase& other) const noexcept;
    [[nodiscard]] inline bool operator < (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator < (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator < (const std::basic_string_view<CharType>& other) const noexcept;

    [[nodiscard]] inline bool operator >= (const StringBase& other) const noexcept;
    [[nodiscard]] inline bool operator >= (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator >= (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator >= (const std::basic_string_view<CharType>& other) const noexcept;

    [[nodiscard]] inline bool operator <= (const StringBase& other) const noexcept;
    [[nodiscard]] inline bool operator <= (const CharType* other) const noexcept;
    [[nodiscard]] inline bool operator <= (const std::basic_string<CharType>& other) const noexcept;
    [[nodiscard]] inline bool operator <= (const std::basic_string_view<CharType>& other) const noexcept;

    inline Derived& operator += (const StringBase& src);
    inline Derived& operator += (const std::basic_string<CharType>& src);
    inline Derived& operator += (const std::basic_string_view<CharType>& src);
    inline Derived& operator += (const CharType* src);
    inline Derived& operator += (const CharType chSource);
    inline Derived& operator += (const OtherCharType* src);
    inline Derived& operator += (const std::basic_string<OtherCharType>& src);
    inline Derived& operator += (const OtherCharType chSource);
    template<typename OtherDerived>
    inline Derived& operator += (const StringBase<OtherCharType, OtherDerived>& src);

/************************************************************************/
// Friend operators. Each concatenation allocates the result once.
/************************************************************************/

    friend inline Derived operator + (const Derived& lhs, const Derived& rhs)
    {
        return _concat(lhs.mData.data(), lhs.mData.length(), rhs.mData.data(), rhs.mData.length());
    }

    friend inline Derived operator + (const Derived& lhs, const std::basic_string<CharType>& rhs)
    {
        return _concat(lhs.mData.data(), lhs.mData.length(), rhs.data(), rhs.length());
    }

    friend inline Derived operator + (const Derived& lhs, std::basic_string_view<CharType> rhs)
    {
        return _concat(lhs.mData.data(), lhs.mData.length(), rhs.data(), rhs.length());
    }

    friend inline Derived operator + (const Derived& lhs, const CharType* rhs)
    {
        return _concat(lhs.mData.data(), lhs.mData.length(), rhs, _length(rhs));
    }

    friend inline Derived operator + (const Derived& lhs, const CharType rhs)
    {
        return _concat(lhs.mData.data(), lhs.mData.length(), &rhs, 1u);
    }

    friend inline Derived operator + (const std::basic_string<CharType>& lhs, const Derived& rhs)
    {
        return _concat(lhs.data(), lhs.length(), rhs.mData.data(), rhs.mData.length());
    }

    friend inline Derived operator + (std::basic_string_view<CharType> lhs, const Derived& rhs)
    {
        return _concat(lhs.data(), lhs.length(), rhs.mData.data(), rhs.mData.length());
    }

    friend inline Derived operator + (const CharType* lhs, const Derived& rhs)
    {
        return _concat(lhs, _length(lhs), rhs.mData.data(), rhs.mData.length());
    }

    friend inline Derived operator + (const CharType lhs, const Derived& rhs)
    {
        return _concat(&lhs, 1u, rhs.mData.data(), rhs.mData.length());
    }

    friend inline Derived operator + (const Derived& lhs, const OtherCharType* rhs)
    {
        Derived result(static_cast<uint32_t>(lhs.mData.length() + _length(rhs)));
        result.mData.append(lhs.mData);
        result._append_other(rhs, static_cast<areg::CharCount>(_length(rhs)));
        return result;
    }

    friend inline Derived operator + (const OtherCharType* lhs, const Derived& rhs)
    {
        Derived result(static_cast<uint32_t>(_length(lhs) + rhs.mData.length()));
        result._append_other(lhs, static_cast<areg::CharCount>(_length(lhs)));
        result.mData.append(rhs.mData);
        return result;
    }

    /**
     * \brief   Writes the string to the standard output stream of the same character type.
     *
     * \return  Reference to stream object.
     **/
    friend inline std::basic_ostream<CharType> & operator << ( std::basic_ostream<CharType> & stream, const StringBase & output )
    {
        return (stream << output.mData);
    }

    /**
     * \brief   Reads a whitespace-delimited word from the standard input stream of the same character type.
     *
     * \return  Reference to stream object.
     **/
    friend inline std::basic_istream<CharType> & operator >> ( std::basic_istream<CharType> & stream, StringBase & input )
    {
        return (stream >> input.mData);
    }

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
    void extract_word(StringBase& word, areg::CharPos& startAt, areg::CharPos& endAt) const;

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
     *                              case-sensitive. Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_first( CharType chSearch, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true ) const noexcept;

    /**
     * \brief   Find the first occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   caseSensitive   If true, the search of phrase should be exact, i.e.
     *                          case-sensitive. Otherwise, the search is by upper and lower case.
     * \param   wholeWord       If true, the search of phrase should be the whole word.
     *                          Otherwise, searches by any match. By default, the search is by any match.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_first( const CharType * phrase, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true, bool wholeWord = false ) const noexcept;
    /**
     * \brief   Find the first occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   caseSensitive   If true, the search of phrase should be exact, i.e.
     *                          case-sensitive. Otherwise, the search is by upper and lower case.
     * \param   wholeWord       If true, the search of phrase should be the whole word.
     *                          Otherwise, searches by any match. By default, the search is by any match.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_first( const StringBase & phrase, areg::CharPos startPos = areg::START_POS, bool caseSensitive = true, bool wholeWord = false ) const noexcept;

    /**
     * \brief   Find the last occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   chSearch            The character to search in the string.
     * \param   caseSensitive       If true, the character match should be by exact, i.e.
     *                              case-sensitive. Otherwise, the search is by upper and lower case.
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
     *                              case-sensitive. Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    areg::CharPos find_last( const CharType * phrase, areg::CharCount phraseCount, areg::CharPos startPos, bool caseSensitive) const noexcept;

    /**
     * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    inline areg::CharPos find_last( const CharType* phrase, areg::CharPos startPos = areg::END_POS, bool caseSensitive = true) const noexcept;

    /**
     * \brief   Find the last occurrence of given phrase in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns areg::INVALID_POS value.
     *
     * \param   caseSensitive   If true, the search of phrase should be exact, i.e. case-sensitive.
     * \return  Returns valid string position value, if found given character. Otherwise, returns
     *          areg::INVALID_POS value.
     **/
    [[nodiscard]]
    inline areg::CharPos find_last( const StringBase & phrase, areg::CharPos startPos = areg::END_POS, bool caseSensitive = true ) const noexcept;

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
    inline areg::Ordering compare(const StringBase & strOther, bool caseSensitive = true) const noexcept;

    [[nodiscard]]
    inline areg::Ordering compare(const std::basic_string<CharType>& strOther, bool caseSensitive = true) const noexcept;

    [[nodiscard]]
    inline areg::Ordering compare(const std::basic_string_view<CharType>& strOther, bool caseSensitive = true) const noexcept;

    [[nodiscard]]
    inline areg::Ordering compare( const StringBase& strOther
                                 , areg::CharPos startAt
                                 , areg::CharCount count = areg::COUNT_ALL
                                 , bool caseSensitive = true) const noexcept;

    [[nodiscard]]
    inline areg::Ordering compare( const std::basic_string<CharType>& strOther
                                 , areg::CharPos startAt
                                 , areg::CharCount count = areg::COUNT_ALL
                                 , bool caseSensitive = true) const noexcept;

    /**
     * \brief   Compares with a string starting at 'startAt' of this string, the position given
     *          by any integral type except bool. A bool second argument is 'caseSensitive'.
     **/
    template<typename PosType>
    [[nodiscard]]
    inline std::enable_if_t<std::is_integral_v<PosType> && !std::is_same_v<PosType, bool>, areg::Ordering>
        compare(const CharType* strOther, PosType startAt, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true) const noexcept;

    template<typename PosType>
    [[nodiscard]]
    inline std::enable_if_t<std::is_integral_v<PosType> && !std::is_same_v<PosType, bool>, areg::Ordering>
        compare(const StringBase& strOther, PosType startAt, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true) const noexcept;

    template<typename PosType>
    [[nodiscard]]
    inline std::enable_if_t<std::is_integral_v<PosType> && !std::is_same_v<PosType, bool>, areg::Ordering>
        compare(const std::basic_string<CharType>& strOther, PosType startAt, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true) const noexcept;

    template<typename PosType>
    [[nodiscard]]
    inline std::enable_if_t<std::is_integral_v<PosType> && !std::is_same_v<PosType, bool>, areg::Ordering>
        compare(const std::basic_string_view<CharType>& strOther, PosType startAt, areg::CharCount count = areg::COUNT_ALL, bool caseSensitive = true) const noexcept;

    template<typename BoolType>
    [[nodiscard]]
    inline std::enable_if_t<std::is_same_v<BoolType, bool>, areg::Ordering>
        compare(const CharType* strOther, BoolType caseSensitive) const noexcept;

    /**
     * \brief   Truncates the starting at zero-based 'startPos' given number of characters. If
     *          'startPos' is beginning of the string (areg::START_POS) and 'count' is equal to
     *          the length of the string or areg::COUNT_ALL, the string is not modified.
     *
     * \return  Returns truncated string.
     **/
    inline Derived& substring(areg::CharPos startPos, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Extracts the substring starting at startPos given number of characters.
     **/
    inline void substring( StringBase & outResult, areg::CharPos startPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL ) const;

    /**
     * \brief   Searches given phrase in the string starting from given position until the end of
     *          string. If found, copies the string data into the result until the found position
     *          returns position next after phrase. If not found, will copy complete string until end.
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
    inline areg::CharPos substring(StringBase& outResult, const CharType* strPhrase, areg::CharPos startPos = areg::START_POS) const;

    inline areg::CharPos substring(StringBase& outResult, const StringBase& strPhrase, areg::CharPos startPos = areg::START_POS) const;

    inline areg::CharPos substring(StringBase& outResult, const std::basic_string<CharType>& strPhrase, areg::CharPos startPos = areg::START_POS) const;

    /**
     * \brief   Searches given symbol in the string starting from given position until end of
     *          string. If found, copies the string data into the result until the found position
     *          returns position next after symbol. If not found, will copy complete string until end.
     *
     * \param   chSymbol    The symbol to search in the string.
     * \return  Returns next position after searched symbol and value are followings: - Valid string
     *          position not equal to areg::END_POS, if found phrase and the symbol is not at
     *          the end; - areg::END_POS if found the symbol at end of string; -
     *          areg::INVALID_POS if could not find the phrase.
     **/
    areg::CharPos substring( StringBase & outResult, CharType chSymbol, areg::CharPos startPos = areg::START_POS ) const;

    /**
     * \brief   Returns left side (begin) substring of length 'charCount'.
     **/
    [[nodiscard]]
    inline Derived left_side(areg::CharCount charCount) const;

    /**
     * \brief   Returns right side (end) substring of length 'charCount'.
     **/
    [[nodiscard]]
    inline Derived right_side(areg::CharCount charCount) const;

    /**
     * \brief   Copies given amount of characters of given string returns the amount of copied characters.
     *          If string has not enough space to copy characters, it will reallocate the space.
     **/
    inline Derived& assign(const CharType * source, areg::CharCount count = areg::COUNT_ALL );

    inline Derived& assign(const std::basic_string<CharType> & source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& assign(const std::basic_string_view<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& assign(const StringBase& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& assign(CharType ch);

    /**
     * \brief   Appends given string at the end. The given string can be limited by zero-based valid
     *          position by amount of characters to append.
     **/
    inline Derived& append(const CharType * source, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& append(const std::basic_string<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& append(const std::basic_string_view<CharType>& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& append(const StringBase& source, areg::CharPos pos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& append(CharType ch);

    /**
     * \brief   Moves data inside string buffer starting at specified position. It moves left if
     *          'move_to' value is negative moves right if 'move_to' is positive. If 'move_to' is
     *          zero, no change happens.
     *
     * \param   move_to     Specifies number of characters and direction to move string. If
     *                      negative, moves string to left side. String buffer is not reallocated.
     *                      If positive, moves string to right. if zero, no change happens.
     **/
    Derived& move_to(areg::CharPos startPos, int32_t move_to);

    /**
     * \brief   Inserts given source of character at given valid zero-based position in the string.
     *          The character can be inserted at any position, including begin of string or at the
     *          end of string (areg::END_POS).
     **/
    inline Derived& insert_at( CharType source, areg::CharPos atPos, areg::CharCount count = 1 );

    /**
     * \brief   Inserts given source at given valid zero-based position in the string. The character
     *          can be inserted at any position, including begin of string or at the end of string
     *          (areg::END_POS).
     **/
    inline Derived& insert_at( const CharType * source, areg::CharPos atDstPos, areg::CharCount count = areg::COUNT_ALL );

    inline Derived& insert_at(const std::basic_string<CharType>& source, areg::CharPos atDstPos, areg::CharPos atSrcPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& insert_at(const std::basic_string_view<CharType>& source, areg::CharPos atDstPos, areg::CharPos atSrcPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& insert_at(const StringBase& source, areg::CharPos atDstPos, areg::CharPos atSrcPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Searches replaces given character by another character. The search replacement
     *          starts at given position. The method either replaces only first match or all matches.
     *
     * \param   chSearch        The character to be searched to replace.
     * \param   chReplace       The character to set when found 'chSearch'.
     * \param   replaceAll      If true, replaces all matches in the string. Otherwise, replace
     *                          first match.
     **/
    Derived& replace( CharType chSearch, CharType chReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true );

    /**
     * \brief   Searches replaces given string by another string. The search replacement starts at
     *          given position. The method either replaces only first match or all matches.
     **/
    Derived& replace( const CharType * strSearch, const CharType * strReplace, areg::CharPos startPos = areg::START_POS, areg::CharCount count = areg::COUNT_ALL, bool replaceAll = true );

    Derived& replace(const std::basic_string<CharType>& strSearch, const std::basic_string<CharType>& strReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true);

    Derived& replace(const std::basic_string_view<CharType>& strSearch, const std::basic_string_view<CharType>& strReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true);

    inline Derived& replace(const StringBase& strSearch, const StringBase& strReplace, areg::CharPos startPos = areg::START_POS, bool replaceAll = true);

    /**
     * \brief   At the given position of the string removes specified amount of characters replaces
     *          by the given string.position after replacing string.
     **/
    inline Derived& replace(areg::CharPos startPos, areg::CharCount charsRemove, const CharType* strReplace, areg::CharCount count = areg::COUNT_ALL);

    inline Derived& replace(areg::CharPos startPos, areg::CharCount charsRemove, const std::basic_string<CharType>& strReplace);

    inline Derived& replace(areg::CharPos startPos, areg::CharCount charsRemove, const StringBase& strReplace);

    /**
     * \brief   Removes specified amount of characters in string at specified starting position. If
     *          'count' is areg::COUNT_ALL, it will remove all characters until end of the string.
     **/
    Derived& remove(areg::CharPos startPos, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Starting from the given valid zero-based position searches removes specified
     *          character. If 'removedAll' parameter is true, removes all matches in the string.
     *          Otherwise, it removes only first match.
     *
     * \param   chRemove        The character to search and remove.
     * \param   removeAll       If true, will remove all matches. Otherwise, removes first match of
     *                          the character.
     **/
    Derived& remove(const CharType chRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true);

    /**
     * \brief   Starting from the given valid zero-based position searches removes given phrase of
     *          string. The search starts at given position. If 'removedAll' parameter is true,
     *          removes all matches in the string. Otherwise, it removes only first match.
     **/
    inline Derived& remove( const CharType * strRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true );

    inline Derived& remove(const StringBase& strRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true);

    Derived& remove(const std::basic_string<CharType> & strRemove, areg::CharPos startPos = areg::START_POS, bool removeAll = true);

    /**
     * \brief   Resizes the string to contain count characters. If the current size is less than
     *          count, additional characters are appended. If the current size is greater than
     *          count, the string is reduced to its first count elements.
     *
     * \param   chFill      The characters to fill if new size if longer than the current length.
     **/
    inline Derived& resize(areg::CharCount count, CharType chFill = static_cast<CharType>('\0'));

    /**
     * \brief   If the length of string is bigger than the 'maxChars', it truncated the string. If
     *          'maxChars' is zero, empties the string. If the length of the string is smaller than
     *          the 'maxChars', nothing happens.
     **/
    inline Derived& truncate(areg::CharCount maxChars);

    /**
     * \brief   Reserves the space for the string. Unlike 'resize' this operation does not change
     *          the current length of the string.
     **/
    inline Derived& reserve(areg::CharCount newCapacity);

    /**
     * \brief   Returns character at specified valid zero-based position.
     **/
    [[nodiscard]]
    inline CharType at( areg::CharPos atPos ) const noexcept;

    /**
     * \brief   Sets character at specified valid zero-based position.
     **/
    inline Derived& set_at( CharType ch, areg::CharPos atPos = areg::END_POS ) noexcept;

    /**
     * \brief   Removes whitespace characters from left side, i.e. from the begin of the string
     *          returns the actual modified string.
     **/
    inline Derived& trim_left() noexcept;

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin of the
     *          string. The existing string remain unmodified.
     **/
    inline void trim_left( StringBase & strResult ) const;

    inline void trim_left(std::basic_string<CharType>& strResult) const;

    /**
     * \brief   Removes whitespace characters from right side, i.e. from end of the string returns
     *          the actual modified string.
     **/
    inline Derived& trim_right() noexcept;

    /**
     * \brief   Copies data into given string without trailing whitespace at the end of the string.
     *          The existing string remain unmodified.
     **/
    inline void trim_right( StringBase & strResult ) const;

    inline void trim_right(std::basic_string<CharType>& strResult) const;

    /**
     * \brief   Removes whitespace characters from left right sides, i.e. from the begin end of the
     *          string returns the actual modified string.
     **/
    Derived& trim_all() noexcept;

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin end of the
     *          string. The existing string remain unmodified.
     **/
    inline void trim_all( StringBase & strResult ) const;

    void trim_all(std::basic_string<CharType>& strResult) const;

    /**
     * \brief   Converts the string, makes all letters to lower case returns the instance of
     *          converted string.
     **/
    inline Derived& make_lower() noexcept;

    /**
     * \brief   Converts the string, makes all letters to upper case returns the instance of
     *          converted string.
     **/
    inline Derived& make_upper() noexcept;

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
    inline areg::CharPos read_line(StringBase& strResult, areg::CharPos startPos = areg::START_POS) const;

    areg::CharPos read_line(std::basic_string<CharType>& strResult, areg::CharPos startPos = areg::START_POS) const;

    /**
     * \brief   In the existing string removes all characters, which are not alphanumeric.
     *
     * \return  Returns new length of string after making alphanumeric
     **/
    inline Derived& make_alphanumeric() noexcept;

    /**
     * \brief   Checks returns true if the string starts with the given phrase. Valid only the first
     *          256 characters based on UTF-8 code page.
     **/
    [[nodiscard]]
    inline bool starts_with(const StringBase& phrase, bool isCaseSensitive = true) const noexcept;

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
    inline bool ends_with(const StringBase& phrase, bool isCaseSensitive = true) const noexcept;

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
    inline std::vector<Derived> split(CharType delim) const;

    [[nodiscard]]
    inline std::vector<Derived> split(const StringBase & delim) const;

    /**
     * \brief   Assigns characters of the other width. A narrow character is widened as an
     *          unsigned value; a wide character is narrowed by truncation.
     *
     * \param   source      The source string.
     * \param   count       The number of characters to copy; defaults to all characters.
     **/
    inline Derived& assign(const OtherCharType* source, areg::CharCount count = areg::COUNT_ALL);

    /**
     * \brief   Appends characters of the other width, converted as in assign().
     *
     * \param   source      The source string.
     * \param   count       The number of characters to append; defaults to all characters.
     **/
    inline Derived& append(const OtherCharType* source, areg::CharCount count = areg::COUNT_ALL);

/************************************************************************/
// Conversion and formatting
/************************************************************************/

    /**
     * \brief   Extracts substring up to the specified phrase in a string buffer.
     *
     * \param   src             The source string buffer to search.
     * \param   strPhrase       The phrase to search for in the string.
     * \param[out] out_next     If not nullptr, receives pointer to the position after the phrase
     *                          in the source string, or nullptr if phrase not found.
     * \return  Returns the substring up to the matched phrase.
     *
     * \example substr
     *
     *  String test("0123 456 789 0123");
     *
     *  const char * next = static_cast<const char *>(test);          // next == "0123 456 789 0123"
     *  String result1 = String::substr( next, "0123", &next);        // result1 == ""          , next == " 456 789 0123"
     *  String result2 = String::substr( next, "0123", &next);        // result2 == " 456 789 " , next == ""
     *  String result3 = String::substr( next, "0123", &next);        // result3 == ""          , next == nullptr;
     *
     *  next = static_cast<const char *>(test);                       // next == "0123 456 789 0123"
     *  String result4 = String::substr( next, " ", &next);           // result4 == "0123"      , next == "456 789 0123"
     *  String result5 = String::substr( next, " ", &next);           // result5 == "456"       , next == "789 0123"
     *  String result6 = String::substr( next, " ", &next);           // result6 == "789"       , next == "0123";
     *  String result7 = String::substr( next, " ", &next);           // result7 == "0123"      , next == nullptr;
     **/
    static Derived substr( const CharType * src, const CharType * strPhrase, const CharType ** out_next = nullptr );

    /**
     * \brief   Converts a string of digits to a 32-bit signed integer.
     *
     * \param   strDigit    The string with digits. Can contain optional negative or positive sign.
     * \param   radix       The base for conversion.
     * \param[out] end      If not nullptr, receives pointer to the next character after the numerical value.
     * \return  Returns the 32-bit signed integer.
     **/
    static int32_t make_int32( const CharType * strDigit, areg::Radix radix = areg::Radix::Decimal, const CharType ** end = nullptr ) noexcept;

    /**
     * \brief   Converts a string of digits to a 32-bit unsigned integer.
     *
     * \param   strDigit    The string with digits.
     * \param   radix       The base for conversion.
     * \param[out] end      If not nullptr, receives pointer to the next character after the numerical value.
     * \return  Returns the 32-bit unsigned integer.
     **/
    static uint32_t make_uint32( const CharType * strDigit, areg::Radix radix = areg::Radix::Decimal, const CharType ** end = nullptr ) noexcept;

    /**
     * \brief   Converts a string of digits to a 64-bit signed integer.
     *
     * \param   strDigit    The string with digits. Can contain optional negative or positive sign.
     * \param   radix       The base for conversion.
     * \param[out] end      If not nullptr, receives pointer to the next character after the numerical value.
     * \return  Returns the 64-bit signed integer.
     **/
    static int64_t make_int64( const CharType * strDigit, areg::Radix radix = areg::Radix::Decimal, const CharType ** end = nullptr ) noexcept;

    /**
     * \brief   Converts a string of digits to a 64-bit unsigned integer.
     *
     * \param   strDigit    The string with digits.
     * \param   radix       The base for conversion.
     * \param[out] end      If not nullptr, receives pointer to the next character after the numerical value.
     * \return  Returns the 64-bit unsigned integer.
     **/
    static uint64_t make_uint64( const CharType * strDigit, areg::Radix radix = areg::Radix::Decimal, const CharType ** end = nullptr ) noexcept;

    /**
     * \brief   Converts a string of digits to a 32-bit floating-point number.
     *
     * \param   strDigit    The string with digits.
     * \param[out] end      If not nullptr, receives pointer to the next character after the numerical value.
     * \return  Returns the 32-bit floating-point number.
     **/
    static float make_float( const CharType * strDigit, const CharType ** end = nullptr ) noexcept;

    /**
     * \brief   Converts a string of digits to a 64-bit floating-point number.
     *
     * \param   strDigit    The string with digits.
     * \param[out] end      If not nullptr, receives pointer to the next character after the numerical value.
     * \return  Returns the 64-bit floating-point number.
     **/
    static double make_double( const CharType * strDigit, const CharType ** end = nullptr ) noexcept;

    /**
     * \brief   Converts a string to a boolean value.
     *
     * \param   strBoolean  The string to convert. Recognized as true if equal to "true" (case-insensitive).
     * \param[out] end      If not nullptr, receives pointer to the next character after the value.
     * \return  Returns true if string matches "true"; false otherwise.
     **/
    static bool make_bool( const CharType * strBoolean, const CharType ** end = nullptr ) noexcept;

    /**
     * \brief   Converts a 32-bit signed integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal. Octal has 11 and
     *                      hexadecimal 8 digits at least, hexadecimal is prefixed by "0x".
     * \return  Returns the converted string. A negative number has a minus sign in every base.
     **/
    static Derived make_string( int32_t number, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 32-bit unsigned integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal. Octal has 11 and
     *                      hexadecimal 8 digits at least, hexadecimal is prefixed by "0x".
     * \return  Returns the converted string.
     **/
    static Derived make_string( uint32_t number, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 64-bit signed integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal. Octal has 22 and
     *                      hexadecimal 16 digits at least, hexadecimal is prefixed by "0x".
     * \return  Returns the converted string. A negative number has a minus sign in every base.
     **/
    static Derived make_string( int64_t number, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 64-bit unsigned integer to a string.
     *
     * \param   number      The number to convert.
     * \param   radix       The base for conversion; defaults to decimal. Octal has 22 and
     *                      hexadecimal 16 digits at least, hexadecimal is prefixed by "0x".
     * \return  Returns the converted string.
     **/
    static Derived make_string( uint64_t number, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 32-bit floating-point number to a string, formatted as "%f".
     *
     * \param   number      The floating-point number to convert.
     * \return  Returns the converted string.
     **/
    static Derived make_string( float number );

    /**
     * \brief   Converts a 64-bit floating-point number to a string, formatted as "%g".
     *
     * \param   number      The floating-point number to convert.
     * \return  Returns the converted string.
     **/
    static Derived make_string( double number );

    /**
     * \brief   Converts a boolean value to a string.
     *
     * \param   value       The boolean value to convert.
     * \return  Returns "true" or "false".
     **/
    static Derived make_string( bool value );

    /**
     * \brief   Formats a string using printf-like formatting rules.
     *          The buffer is never written past its end and is always null-terminated.
     *
     * \param[out] strDst   The destination buffer for the formatted string.
     * \param   count       The size of the destination buffer, including the null-character.
     * \param   format      The format string, followed by arguments to format.
     * \return  Returns the number of characters copied, never more than 'count - 1';
     *          negative if 'strDst' is nullptr.
     **/
    static int32_t format_string( CharType * strDst, int32_t count, const CharType * format, ... );

    /**
     * \brief   Formats a string using printf-like formatting rules and a va_list.
     *          The buffer is never written past its end and is always null-terminated.
     *
     * \param[out] strDst   The destination buffer for the formatted string.
     * \param   count       The size of the destination buffer, including the null-character.
     * \param   format      The format string.
     * \param   argptr      The variable argument list.
     * \return  Returns the number of characters copied, never more than 'count - 1';
     *          negative if 'strDst' is nullptr.
     **/
    static int32_t format_string_list( CharType * strDst, int32_t count, const CharType * format, va_list argptr );

    /**
     * \brief   Formats a string using printf-like formatting rules and a va_list, and
     *          reports how long the formatted text is when it does not fit the buffer.
     *          The buffer is never written past its end and is always null-terminated.
     *
     * \param[out]  strDst      The destination buffer for the formatted string.
     * \param       count       The size of the destination buffer, including the null-character.
     * \param       format      The format string.
     * \param       argptr      The variable argument list.
     * \param[out]  required    The number of characters the formatted text needs, without the
     *                          null-character. A value of 'count' or more means the text was cut.
     * \return  Returns the number of characters copied, never more than 'count - 1';
     *          negative if 'strDst' is nullptr.
     **/
    static int32_t format_string_list( CharType * strDst, int32_t count, const CharType * format, va_list argptr, int32_t & required );

    /**
     * \brief   Formats this string using printf-like formatting rules.
     *
     * \param   format      The format string, followed by arguments to format.
     * \note    On POSIX a wide string is cut at 65535 characters.
     **/
    Derived& format(const CharType* format, ...);

    /**
     * \brief   Formats this string using printf-like formatting rules and a va_list.
     *          The string receives the whole formatted text; on POSIX a wide string is cut at
     *          65535 characters.
     *
     * \param   format      The format string.
     * \param   argptr      The variable argument list.
     **/
    inline Derived& format_list(const CharType* format, va_list argptr);

    /**
     * \brief   Converts this string to a 32-bit signed integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the 32-bit signed integer.
     **/
    [[nodiscard]]
    inline int32_t to_int32( areg::Radix radix = areg::Radix::Decimal ) const noexcept;

    /**
     * \brief   Converts string to a 32-bit unsigned integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the 32-bit unsigned integer.
     **/
    [[nodiscard]]
    inline uint32_t to_uint32( areg::Radix radix = areg::Radix::Decimal ) const noexcept;

    /**
     * \brief   Converts string to a 64-bit signed integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the 64-bit signed integer.
     **/
    [[nodiscard]]
    inline int64_t to_int64( areg::Radix radix = areg::Radix::Decimal ) const noexcept;

    /**
     * \brief   Converts string to a 64-bit unsigned integer.
     *
     * \param   radix       The base for conversion; defaults to decimal.
     * \return  Returns the 64-bit unsigned integer.
     **/
    [[nodiscard]]
    inline uint64_t to_uint64( areg::Radix radix = areg::Radix::Decimal ) const noexcept;

    /**
     * \brief   Converts string to a 32-bit floating-point number.
     *
     * \return  Returns the 32-bit floating-point number.
     **/
    [[nodiscard]]
    inline float to_float() const noexcept;

    /**
     * \brief   Converts string to a 64-bit floating-point number.
     *
     * \return  Returns the 64-bit floating-point number.
     **/
    [[nodiscard]]
    inline double to_double() const noexcept;

    /**
     * \brief   Converts string to a boolean value.
     *
     * \return  Returns false if the string is empty or equals "false" (case-insensitive); true otherwise.
     **/
    [[nodiscard]]
    inline bool to_bool() const noexcept;

    /**
     * \brief   Converts a 32-bit signed integer to this string, as make_string() does.
     *
     * \param   value       The integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     **/
    inline Derived & from_int32( int32_t value, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 32-bit unsigned integer to this string, as make_string() does.
     *
     * \param   value       The unsigned integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     **/
    inline Derived & from_uint32( uint32_t value, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 64-bit signed integer to this string, as make_string() does.
     *
     * \param   value       The integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     **/
    inline Derived & from_int64( int64_t value, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 64-bit unsigned integer to this string, as make_string() does.
     *
     * \param   value       The unsigned integer value to convert.
     * \param   radix       The base for conversion; defaults to decimal.
     **/
    inline Derived & from_uint64( uint64_t value, areg::Radix radix = areg::Radix::Decimal );

    /**
     * \brief   Converts a 32-bit floating-point number to this string.
     **/
    inline Derived & from_float( float value );

    /**
     * \brief   Converts a 64-bit floating-point number to this string.
     **/
    inline Derived & from_double( double value );

    /**
     * \brief   Converts a boolean value to this string.
     **/
    inline Derived & from_bool( bool value );

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
     *          exact match of ignoring upper lower cases. It compares upper lower cases by default locale.
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
    static bool is_name_char(const CharType checkChar, std::locale& loc) noexcept;

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

    /**
     * \brief   Returns the length of a null-terminated string, zero for nullptr.
     **/
    template<typename CharSrc>
    static size_t _length(const CharSrc* str) noexcept;

    /**
     * \brief   Returns a new string made of two character sequences, allocated once.
     **/
    static Derived _concat(const CharType* lhs, size_t lhsLen, const CharType* rhs, size_t rhsLen);

    /**
     * \brief   Returns the character as a wide character, a narrow one widened as unsigned value.
     **/
    static constexpr wchar_t _widen(char ch) noexcept;
    static constexpr wchar_t _widen(wchar_t ch) noexcept;

    /**
     * \brief   Appends 'count' characters of the other width.
     **/
    inline void _append_other(const OtherCharType* source, areg::CharCount count);

    /**
     * \brief   Returns true if the string has exactly the 'count' characters of the other width.
     **/
    inline bool _is_equal_other(const OtherCharType* other, size_t count) const noexcept;

    /**
     * \brief   Returns the part of the source starting at 'pos' with at most 'count' characters,
     *          empty if 'pos' is outside the source.
     **/
    static std::basic_string_view<CharType> _sub_view(std::basic_string_view<CharType> source, areg::CharPos pos, areg::CharCount count) noexcept;

    /**
     * \brief   Returns the position to insert at, the length for areg::END_POS, or areg::INVALID_POS
     *          if the position is outside the string.
     **/
    inline areg::CharPos _insert_pos(areg::CharPos pos) const noexcept;

    /**
     * \brief   Returns the number written with digits of the given base, at least 'minDigits'.
     **/
    template<uint32_t Base, typename UInt>
    static Derived _make_integer(UInt magnitude, bool negative, int32_t minDigits, bool prefix);

    /**
     * \brief   Returns the unsigned number written in the given radix.
     **/
    template<typename UInt>
    static Derived _make_unsigned(UInt magnitude, bool negative, areg::Radix radix);

    /**
     * \brief   Formats into the buffer of 'count' characters, never past its end, always null-terminated.
     *          'required' receives the characters needed; 'count' or more means the text was cut.
     *          Returns the characters written.
     **/
    static int32_t _format_buffer(CharType* buffer, int32_t count, const CharType* format, va_list argptr, int32_t& required) noexcept;

    /**
     * \brief   Returns the object as the derived string type.
     **/
    inline Derived& self() noexcept;
    inline const Derived& self() const noexcept;

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

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase() noexcept
    : mData  ( )
{
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(const CharType source)
    : mData  ( &source, 1 )
{
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(const CharType* source)
    : mData  ( source != nullptr ? source : &EmptyChar )
{
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(const std::basic_string<CharType>& source)
    : mData  ( source )
{
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(std::basic_string<CharType>&& source) noexcept
    : mData  ( std::move(source) )
{
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(std::basic_string_view<CharType> source)
    : mData  ( source.data(), source.length() )
{
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(const CharType* source, areg::CharCount count)
    : mData  ( source != nullptr ? source : &EmptyChar
             , source != nullptr ? static_cast<uint32_t>(count != areg::COUNT_ALL ? count : areg::string_length(source)) : 0u )
{
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(const StringBase& source)
    : mData  ( source.mData )
{
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(StringBase&& source) noexcept
    : mData  ( std::move(source.mData) )
{
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(const OtherCharType* source)
    : mData  ( )
{
    _append_other(source, static_cast<areg::CharCount>(_length(source)));
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(const OtherCharType* source, areg::CharCount count)
    : mData  ( )
{
    _append_other(source, count != areg::COUNT_ALL ? count : static_cast<areg::CharCount>(_length(source)));
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(const std::basic_string<OtherCharType>& source)
    : mData  ( )
{
    _append_other(source.data(), static_cast<areg::CharCount>(source.length()));
}

template<typename CharType, typename Derived>
template<typename OtherDerived>
inline StringBase<CharType, Derived>::StringBase(const StringBase<OtherCharType, OtherDerived>& source)
    : mData  ( )
{
    _append_other(source.as_string(), source.length());
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::StringBase(uint32_t count)
    : mData  ( )
{
    mData.reserve(count);
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::operator const CharType* () const noexcept
{
    return mData.c_str();
}

template<typename CharType, typename Derived>
inline StringBase<CharType, Derived>::operator uint32_t() const noexcept
{
    return areg::crc32_calculate(mData.c_str());
}

template<typename CharType, typename Derived>
inline const CharType StringBase<CharType, Derived>::operator [] (int32_t index) const noexcept
{
    ASSERT((mData.c_str() != nullptr) && (index < static_cast<int32_t>(mData.length())));
    return mData.c_str()[index];
}


template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (const StringBase& src)
{
    mData = src.mData;
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (const std::basic_string<CharType>& src)
{
    mData = src;
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (const std::basic_string_view<CharType>& src)
{
    mData.assign( src.data(), src.length() );
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (const CharType* src)
{
    if (src != nullptr)
    {
        mData.assign(src);
    }
    else
    {
        mData.clear();
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (const CharType chSource)
{
    mData = chSource;
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (std::basic_string<CharType>&& src) noexcept
{
    mData = std::move(src);
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (StringBase&& src) noexcept
{
    mData = std::move(src.mData);
    return self();
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator == (const StringBase& other) const noexcept
{
    return (mData == other.mData);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator == (const std::basic_string<CharType>& other) const noexcept
{
    return (mData == other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator == (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData == other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator == (const CharType* other) const noexcept
{
    return (mData == (other != nullptr ? other : &EmptyChar));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator == ( const CharType ch) const noexcept
{
    return ((mData.length() == 1) && (mData[0] == ch));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator != (const StringBase& other) const noexcept
{
    return (mData != other.mData);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator != (const std::basic_string<CharType>& other) const noexcept
{
    return (mData != other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator != (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData != other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator != (const CharType* other) const noexcept
{
    return (mData != (other != nullptr ? other : &EmptyChar));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator != ( const CharType ch ) const noexcept
{
    return ((mData.length( ) != 1) || (mData[0] != ch));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator > (const StringBase& other) const noexcept
{
    return (mData > other.mData);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator > (const std::basic_string<CharType>& other) const noexcept
{
    return (mData > other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator > (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData > other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator > (const CharType* other) const noexcept
{
    return (mData > (other != nullptr ? other : &EmptyChar));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator < (const StringBase& other) const noexcept
{
    return (mData < other.mData);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator < (const std::basic_string<CharType>& other) const noexcept
{
    return (mData < other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator < (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData < other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator < (const CharType* other) const noexcept
{
    return (mData < (other != nullptr ? other : &EmptyChar));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator >= (const StringBase& other) const noexcept
{
    return (mData >= other.mData);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator >= (const std::basic_string<CharType>& other) const noexcept
{
    return (mData >= other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator >= (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData >= other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator >= (const CharType* other) const noexcept
{
    return (mData >= (other != nullptr ? other : &EmptyChar));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator <= (const StringBase& other) const noexcept
{
    return (mData <= other.mData);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator <= (const std::basic_string<CharType>& other) const noexcept
{
    return (mData <= other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator <= (const std::basic_string_view<CharType>& other) const noexcept
{
    return (mData <= other);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator <= (const CharType* other) const noexcept
{
    return (mData <= (other != nullptr ? other : &EmptyChar));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator += (const StringBase& src)
{
    mData += src.mData;
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator += (const std::basic_string<CharType>& src)
{
    mData += src;
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator += (const std::basic_string_view<CharType>& src)
{
    mData.append(src.data(), src.length());
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator += (const CharType* src)
{
    if (!areg::is_empty(src))
        mData += src;
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator += (const CharType chSource)
{
    mData += chSource;
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (const OtherCharType* src)
{
    return assign(src, areg::COUNT_ALL);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (const std::basic_string<OtherCharType>& src)
{
    return assign(src.data(), static_cast<areg::CharCount>(src.length()));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator = (const OtherCharType src)
{
    return assign(&src, 1);
}

template<typename CharType, typename Derived>
template<typename OtherDerived>
inline Derived& StringBase<CharType, Derived>::operator = (const StringBase<OtherCharType, OtherDerived>& src)
{
    return assign(src.as_string(), src.length());
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator == (const OtherCharType* other) const noexcept
{
    return (other != nullptr ? _is_equal_other(other, _length(other)) : mData.empty());
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator == (const std::basic_string<OtherCharType>& other) const noexcept
{
    return _is_equal_other(other.data(), other.length());
}

template<typename CharType, typename Derived>
template<typename OtherDerived>
inline bool StringBase<CharType, Derived>::operator == (const StringBase<OtherCharType, OtherDerived>& other) const noexcept
{
    return _is_equal_other(other.as_string(), static_cast<size_t>(other.length()));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator != (const OtherCharType* other) const noexcept
{
    return (operator == (other) == false);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::operator != (const std::basic_string<OtherCharType>& other) const noexcept
{
    return (operator == (other) == false);
}

template<typename CharType, typename Derived>
template<typename OtherDerived>
inline bool StringBase<CharType, Derived>::operator != (const StringBase<OtherCharType, OtherDerived>& other) const noexcept
{
    return (operator == (other) == false);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator += (const OtherCharType* src)
{
    return append(src, areg::COUNT_ALL);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator += (const std::basic_string<OtherCharType>& src)
{
    _append_other(src.data(), static_cast<areg::CharCount>(src.length()));
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::operator += (const OtherCharType chSource)
{
    _append_other(&chSource, 1);
    return self();
}

template<typename CharType, typename Derived>
template<typename OtherDerived>
inline Derived& StringBase<CharType, Derived>::operator += (const StringBase<OtherCharType, OtherDerived>& src)
{
    _append_other(src.as_string(), src.length());
    return self();
}

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::validate(const CharType* validityList) const noexcept
{
    if (mData.empty() || areg::is_empty<CharType>(validityList))
        return false;

    // find_first_not_of is SIMD-optimized in major STLs; eliminates the per-character loop.
    return mData.find_first_not_of(validityList) == std::basic_string<CharType>::npos;
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_numeric(bool signIgnore /*= true*/) const noexcept
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

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_alphanumeric() const noexcept
{
    const CharType* src = mData.c_str();
    while (areg::is_alphanumeric<CharType>(*src))
        ++src;

    return (*src == EmptyChar); // reached end of the string.
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_name_char(const CharType checkChar) const noexcept
{
    // initialize list of symbols for the valid names.
    constexpr CharType symbols[] = { '_', '\0'};
    return areg::is_alphanumeric<CharType>(checkChar) || areg::is_one_of<CharType>(checkChar, symbols);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_valid_name() const noexcept
{
    if (mData.empty())
        return false;

    const CharType* src = mData.c_str();
    while (is_name_char(*src))
        ++src;

    return (*src == EmptyChar);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_empty() const noexcept
{
    return mData.empty();
}

template<typename CharType, typename Derived>
inline areg::CharCount StringBase<CharType, Derived>::length() const noexcept
{
    return static_cast<areg::CharCount>(mData.length());
}

template<typename CharType, typename Derived>
inline areg::CharCount StringBase<CharType, Derived>::capacity() const noexcept
{
    return static_cast<areg::CharCount>(mData.capacity());
}

template<typename CharType, typename Derived>
inline uint32_t StringBase<CharType, Derived>::space() const noexcept
{
    return static_cast<uint32_t>((mData.length() + 1) * sizeof(CharType));
}

template<typename CharType, typename Derived>
inline const CharType* StringBase<CharType, Derived>::buffer(areg::CharPos startAt /*= areg::START_POS*/) const noexcept
{
    areg::CharCount len = static_cast<areg::CharCount>(mData.size());
    startAt = (startAt == areg::END_POS) || (startAt > len) ? len : startAt;
    return (mData.c_str() + startAt);
}

template<typename CharType, typename Derived>
inline CharType* StringBase<CharType, Derived>::buffer(areg::CharPos startAt /*= areg::START_POS*/) noexcept
{
    areg::CharCount len = static_cast<areg::CharCount>(mData.size());
    startAt = (startAt == areg::END_POS) || (startAt > len) ? len : startAt;
    return (mData.data() + startAt);
}

template<typename CharType, typename Derived>
inline const CharType* StringBase<CharType, Derived>::as_string() const noexcept
{
    return mData.c_str();
}

template<typename CharType, typename Derived>
inline const std::basic_string<CharType>& StringBase<CharType, Derived>::data() const noexcept
{
    return mData;
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_valid_position(areg::CharPos pos) const noexcept
{
    return (pos >= areg::START_POS) && (pos < static_cast<areg::CharPos>(mData.length()));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_invalid_position(areg::CharPos pos) const noexcept
{
    return (pos < areg::START_POS) || (pos > static_cast<areg::CharPos>(mData.length()));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_last_position(areg::CharPos pos) const noexcept
{
    return (pos == areg::END_POS) || (pos >= static_cast<areg::CharPos>(mData.length()));
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_first_position(areg::CharPos pos) const noexcept
{
    return (!mData.empty() && (pos == areg::START_POS));
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::clear() noexcept
{
    mData.clear();
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::free_extra()
{
    areg::shrink_to_fit( mData );
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::release()
{
    mData.clear();
    areg::shrink_to_fit( mData );
}

template<typename CharType, typename Derived>
void StringBase<CharType, Derived>::find_word(areg::CharPos& startAt, areg::CharPos& endAt) const  noexcept
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

template<typename CharType, typename Derived>
void StringBase<CharType, Derived>::extract_word(StringBase& word, areg::CharPos& startAt, areg::CharPos& endAt) const
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

template<typename CharType, typename Derived>
areg::CharPos StringBase<CharType, Derived>::find_one_of( const CharType* chars, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
{
    if (is_invalid_position(startPos) || areg::is_empty<CharType>(chars))
        return areg::INVALID_POS;

    // find_first_of is SIMD-optimized in major STLs; delegates to memchr-class instructions.
    const auto pos = mData.find_first_of(chars, static_cast<uint32_t>(startPos));
    return (pos != std::basic_string<CharType>::npos ? static_cast<areg::CharPos>(pos) : areg::END_POS);
}

template<typename CharType, typename Derived>
areg::CharPos StringBase<CharType, Derived>::find_first( CharType chSearch
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

template<typename CharType, typename Derived>
areg::CharPos StringBase<CharType, Derived>::find_first( const CharType* phrase
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

template<typename CharType, typename Derived>
areg::CharPos StringBase<CharType, Derived>::find_first( const StringBase & phrase
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

template<typename CharType, typename Derived>
areg::CharPos StringBase<CharType, Derived>::find_last(CharType chSearch, areg::CharPos startPos /*= areg::END_POS*/, bool caseSensitive /*= true*/) const noexcept
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

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::find_last(const CharType* phrase, areg::CharCount phraseCount, areg::CharPos startPos, bool caseSensitive) const noexcept
{
    if (((startPos != areg::END_POS) && is_invalid_position(startPos)) || areg::is_empty<CharType>(phrase) || (phraseCount == 0))
        return areg::INVALID_POS;

    const areg::CharCount strLen = length();
    if (strLen < phraseCount)
        return areg::END_POS;

    if (caseSensitive)
    {
        // mData.rfind() is SIMD-optimized
        using SizeT = typename std::basic_string<CharType>::size_type;
        const SizeT searchPos = (startPos == areg::END_POS) ? std::basic_string<CharType>::npos : static_cast<SizeT>(startPos);
        const auto pos = mData.rfind(phrase, searchPos, static_cast<SizeT>(phraseCount));
        return (pos != std::basic_string<CharType>::npos) ? static_cast<areg::CharPos>(pos) : areg::END_POS;
    }

    // Case-insensitive: backward scan using compare()
    // maxStart: last valid phrase-start position
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

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::find_last(const CharType* phrase, areg::CharPos startPos /*= areg::END_POS*/, bool caseSensitive /*= true*/) const noexcept
{
    return find_last(phrase, areg::string_length<CharType>(phrase), startPos, caseSensitive);
}

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::find_last(const StringBase & phrase, areg::CharPos startPos /*= areg::END_POS*/, bool caseSensitive /*= true*/) const noexcept
{
    return find_last(phrase, phrase.length(), startPos, caseSensitive);
}

template<typename CharType, typename Derived>
areg::Ordering StringBase<CharType, Derived>::compare( const CharType* what
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
            // SIMD-optimized
            const int cmp = mData.compare( static_cast<std::size_t>(startAt)
                                         , static_cast<std::size_t>(count)
                                         , what
                                         , static_cast<std::size_t>(count) );
            if (cmp == 0)   return areg::Ordering::Equal;
            return (cmp < 0) ? areg::Ordering::Smaller : areg::Ordering::Bigger;
        }

        // Case-insensitive: char-by-char with fold-to-lower, exactly 'count' characters.
        const CharType* current = buffer(startAt);
        for (areg::CharCount i = 0; i < count; ++i)
        {
            const CharType ch1 = areg::make_lower<CharType>(current[i]);
            const CharType ch2 = areg::make_lower<CharType>(what[i]);
            if (ch1 != ch2)
                return (ch1 < ch2) ? areg::Ordering::Smaller : areg::Ordering::Bigger;
        }

        result = areg::Ordering::Equal;
    }
    else if (length > count)
    {
        result = areg::Ordering::Bigger;
    }

    return result;
}

template<typename CharType, typename Derived>
inline areg::Ordering StringBase<CharType, Derived>::compare(const StringBase& other, bool caseSensitive /*= true*/) const noexcept
{
    return compare(other.mData, caseSensitive);
}

template<typename CharType, typename Derived>
inline areg::Ordering StringBase<CharType, Derived>::compare(const std::basic_string<CharType>& other, bool caseSensitive /*= true*/) const noexcept
{
    return compare(other.c_str(), areg::START_POS, static_cast<areg::CharCount>(other.length()), caseSensitive);
}

template<typename CharType, typename Derived>
inline areg::Ordering StringBase<CharType, Derived>::compare(const std::basic_string_view<CharType>& other, bool caseSensitive /*= true*/) const noexcept
{
    return compare(other.data(), areg::START_POS, static_cast<areg::CharCount>(other.length()), caseSensitive);
}

template<typename CharType, typename Derived>
inline areg::Ordering StringBase<CharType, Derived>::compare( const StringBase& other
                                                   , areg::CharPos startPos
                                                   , areg::CharCount count  /*= areg::COUNT_ALL*/
                                                   , bool caseSensitive         /*= true*/) const noexcept
{
    return compare(other.mData, startPos, count, caseSensitive);
}

template<typename CharType, typename Derived>
inline areg::Ordering StringBase<CharType, Derived>::compare( const std::basic_string<CharType>& other
                                                   , areg::CharPos startAt
                                                   , areg::CharCount count  /*= areg::COUNT_ALL*/
                                                   , bool caseSensitive         /*= true*/) const noexcept
{
    return compare(other.c_str(), startAt, count, caseSensitive);
}

template<typename CharType, typename Derived>
template<typename PosType>
inline std::enable_if_t<std::is_integral_v<PosType> && !std::is_same_v<PosType, bool>, areg::Ordering>
StringBase<CharType, Derived>::compare( const CharType* other
                             , PosType startAt
                             , areg::CharCount count    /*= areg::COUNT_ALL*/
                             , bool caseSensitive       /*= true*/) const noexcept
{
    return compare(other, static_cast<areg::CharPos>(startAt), count, caseSensitive);
}

template<typename CharType, typename Derived>
template<typename PosType>
inline std::enable_if_t<std::is_integral_v<PosType> && !std::is_same_v<PosType, bool>, areg::Ordering>
StringBase<CharType, Derived>::compare( const StringBase& other
                             , PosType startAt
                             , areg::CharCount count    /*= areg::COUNT_ALL*/
                             , bool caseSensitive       /*= true*/) const noexcept
{
    return compare(other.mData.c_str(), static_cast<areg::CharPos>(startAt), count, caseSensitive);
}

template<typename CharType, typename Derived>
template<typename PosType>
inline std::enable_if_t<std::is_integral_v<PosType> && !std::is_same_v<PosType, bool>, areg::Ordering>
StringBase<CharType, Derived>::compare( const std::basic_string<CharType>& other
                             , PosType startAt
                             , areg::CharCount count    /*= areg::COUNT_ALL*/
                             , bool caseSensitive       /*= true*/) const noexcept
{
    return compare(other.c_str(), static_cast<areg::CharPos>(startAt), count, caseSensitive);
}

template<typename CharType, typename Derived>
template<typename PosType>
inline std::enable_if_t<std::is_integral_v<PosType> && !std::is_same_v<PosType, bool>, areg::Ordering>
StringBase<CharType, Derived>::compare( const std::basic_string_view<CharType>& other
                             , PosType startAt
                             , areg::CharCount count    /*= areg::COUNT_ALL*/
                             , bool caseSensitive       /*= true*/) const noexcept
{
    const areg::CharCount length = static_cast<areg::CharCount>(other.length());
    count = ((count == areg::COUNT_ALL) || (count > length)) ? length : count;
    return compare(other.data(), static_cast<areg::CharPos>(startAt), count, caseSensitive);
}

template<typename CharType, typename Derived>
template<typename BoolType>
inline std::enable_if_t<std::is_same_v<BoolType, bool>, areg::Ordering>
StringBase<CharType, Derived>::compare(const CharType* other, BoolType caseSensitive) const noexcept
{
    return compare(other, areg::START_POS, areg::COUNT_ALL, caseSensitive);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::substring(areg::CharPos startPos, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    if (is_valid_position(startPos))
    {
        if ((count >= 0) && (count < static_cast<areg::CharCount>(mData.length()) - startPos))
        {
            mData.erase(static_cast<uint32_t>(startPos + count));
        }

        mData.erase(0, static_cast<uint32_t>(startPos));
    }
    else
    {
        mData.clear();
    }

    return self();
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::substring(StringBase& outResult, areg::CharPos startPos /* = areg::START_POS */, areg::CharCount count /*= areg::COUNT_ALL*/) const
{
    if (is_valid_position(startPos))
    {
        outResult.mData = mData.substr(static_cast<uint32_t>(startPos), count == areg::COUNT_ALL ? std::basic_string<CharType>::npos : static_cast<uint32_t>(count));
    }
}

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::substring(StringBase& outResult, const CharType* strPhrase, areg::CharPos startPos /*= areg::START_POS*/) const
{
    return substring(outResult, std::basic_string<CharType>(strPhrase == nullptr ? &EmptyChar : strPhrase), startPos);
}

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::substring(StringBase& outResult, const StringBase& strPhrase, areg::CharPos startPos /*= areg::START_POS*/) const
{
    return substring(outResult, strPhrase.mData, startPos);
}

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::substring(StringBase& outResult, const std::basic_string<CharType>& strPhrase, areg::CharPos startPos /*= areg::START_POS*/) const
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

template<typename CharType, typename Derived>
areg::CharPos StringBase<CharType, Derived>::substring(StringBase& outResult, CharType chSymbol, areg::CharPos startPos /* = areg::START_POS */) const
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

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::left_side(areg::CharCount charCount) const
{
    Derived result;
    StringBase<CharType, Derived>::substring(result, areg::START_POS, charCount);
    return result;
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::right_side(areg::CharCount charCount) const
{
    Derived result;

    areg::CharCount len = length();
    areg::CharPos pos = charCount < len ? len - charCount : areg::START_POS;
    substring(result, pos, areg::COUNT_ALL);

    return result;
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::assign(const CharType* source, areg::CharCount count /*= areg::COUNT_ALL */)
{
    if ( source != nullptr )
    {
        mData.assign( source, static_cast<uint32_t>(count == areg::COUNT_ALL ? areg::string_length<CharType>( source ) : count) );
    }
    else
    {
        mData.clear( );
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::assign(const std::basic_string<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    const std::basic_string_view<CharType> part{ _sub_view(source, pos, count) };
    mData.assign(part.data(), part.length());
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::assign(const std::basic_string_view<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    const std::basic_string_view<CharType> part{ _sub_view(source, pos, count) };
    mData.assign(part.data(), part.length());
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::assign(const StringBase& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    assign(source.mData, pos, count);
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::assign( CharType ch )
{
    mData = ch;
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::append(const CharType* source, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    count = count == areg::COUNT_ALL ? areg::string_length<CharType>(source) : count;
    mData.append(source, static_cast<uint32_t>(count));
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::append( const std::basic_string<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    const std::basic_string_view<CharType> part{ _sub_view(source, pos, count) };
    mData.append(part.data(), part.length());
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::append(const std::basic_string_view<CharType>& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    const std::basic_string_view<CharType> part{ _sub_view(source, pos, count) };
    mData.append(part.data(), part.length());
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::append( const StringBase& source, areg::CharPos pos /*= areg::START_POS*/, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    return append(source.mData, pos, count);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::append( CharType ch )
{
    mData += ch;
    return self();
}

template<typename CharType, typename Derived>
Derived& StringBase<CharType, Derived>::move_to(areg::CharPos startPos, int32_t move_to)
{
    startPos = _insert_pos(startPos);
    if (startPos == areg::INVALID_POS)
        return self();

    if (move_to < 0)
    {
        areg::CharCount count = static_cast<areg::CharCount>(std::min(-static_cast<int64_t>(move_to), static_cast<int64_t>(startPos)));
        mData.erase(static_cast<uint32_t>(startPos - count), static_cast<uint32_t>(count));
    }
    else if (move_to > 0)
    {
        mData.insert(static_cast<uint32_t>(startPos), static_cast<uint32_t>(move_to), '@');
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::insert_at(CharType source, areg::CharPos atPos, areg::CharCount count /*= 1*/)
{
    atPos = _insert_pos(atPos);
    if ((atPos != areg::INVALID_POS) && (count > 0))
    {
        mData.insert(static_cast<uint32_t>(atPos), static_cast<uint32_t>(count), source);
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::insert_at(const CharType* source, areg::CharPos atPos, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    atPos = _insert_pos(atPos);
    if ((source != nullptr) && (atPos != areg::INVALID_POS))
    {
        mData.insert(static_cast<uint32_t>(atPos), source, count == areg::COUNT_ALL ? static_cast<uint32_t>(areg::string_length(source)) : static_cast<uint32_t>(count));
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::insert_at( const std::basic_string<CharType>& source
                                                       , areg::CharPos atDstPos
                                                       , areg::CharPos atSrcPos /*= areg::START_POS*/
                                                       , areg::CharCount count  /*= areg::COUNT_ALL*/)
{
    atDstPos = _insert_pos(atDstPos);
    if (atDstPos != areg::INVALID_POS)
    {
        const std::basic_string_view<CharType> part{ _sub_view(source, atSrcPos, count) };
        mData.insert(static_cast<uint32_t>(atDstPos), part.data(), static_cast<uint32_t>(part.length()));
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::insert_at( const std::basic_string_view<CharType>& source
                                                       , areg::CharPos atDstPos
                                                       , areg::CharPos atSrcPos /*= areg::START_POS*/
                                                       , areg::CharCount count  /*= areg::COUNT_ALL*/)
{
    atDstPos = _insert_pos(atDstPos);
    if (atDstPos != areg::INVALID_POS)
    {
        const std::basic_string_view<CharType> part{ _sub_view(source, atSrcPos, count) };
        mData.insert(static_cast<uint32_t>(atDstPos), part.data(), static_cast<uint32_t>(part.length()));
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::insert_at( const StringBase& source
                                                            , areg::CharPos atDstPos
                                                            , areg::CharPos atSrcPos /*= areg::START_POS*/
                                                            , areg::CharCount count /*= areg::COUNT_ALL*/)
{
    return insert_at(source.mData, atDstPos, atSrcPos, count);
}

template<typename CharType, typename Derived>
Derived& StringBase<CharType, Derived>::replace(CharType chSearch, CharType chReplace, areg::CharPos startPos /*= areg::START_POS*/, bool replaceAll /*= true*/)
{
    if (!is_valid_position(startPos))
        return self();

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

    return self();
}

template<typename CharType, typename Derived>
Derived& StringBase<CharType, Derived>::replace( const CharType* strSearch
                                                   , const CharType* strReplace
                                                   , areg::CharPos startPos   /*= areg::START_POS*/
                                                   , areg::CharCount count    /*= areg::COUNT_ALL*/
                                                   , bool replaceAll              /*= true*/)
{
    if (!is_valid_position(startPos) || areg::is_empty(strSearch))
        return self();

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

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::replace( const std::basic_string_view<CharType>& strSearch
                                                          , const std::basic_string_view<CharType>& strReplace
                                                          , areg::CharPos startPos  /*= areg::START_POS*/
                                                          , bool replaceAll             /*= true*/)
{
    if (!is_valid_position(startPos) || strSearch.empty())
        return self();
    
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

    return self();
}


template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::replace( const StringBase& strSearch
                                                          , const StringBase& strReplace
                                                          , areg::CharPos startPos /*= areg::START_POS*/
                                                          , bool replaceAll /*= true*/)
{
    return replace(strSearch.mData, strReplace.mData, startPos, replaceAll);
}

template<typename CharType, typename Derived>
Derived& StringBase<CharType, Derived>::replace( const std::basic_string<CharType>& strSearch
                                                   , const std::basic_string<CharType>& strReplace
                                                   , areg::CharPos startPos /*= areg::START_POS*/
                                                   , bool replaceAll /*= true*/)
{
    if (!is_valid_position(startPos) || strSearch.empty())
        return self();
    
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

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::replace( areg::CharPos startPos
                                                          , areg::CharCount charsRemove
                                                          , const CharType* strReplace
                                                          , areg::CharCount count /*= areg::COUNT_ALL*/)
{
    if (is_valid_position(startPos))
    {
        areg::CharPos lenReplace = areg::string_length<CharType>(strReplace);
        mData.replace(static_cast<uint32_t>(startPos), static_cast<uint32_t>(charsRemove), strReplace, (count == areg::COUNT_ALL) || (count > lenReplace) ? static_cast<uint32_t>(lenReplace) : static_cast<uint32_t>(count));
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::replace( areg::CharPos startPos, areg::CharCount charsRemove, const StringBase& strReplace)
{
    return replace(startPos, charsRemove, strReplace.mData);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::replace( areg::CharPos startPos, areg::CharCount charsRemove, const std::basic_string<CharType>& strReplace)
{
    if (is_valid_position(startPos))
    {
        mData.replace(static_cast<uint32_t>(startPos), static_cast<uint32_t>(charsRemove), strReplace);
    }

    return self();
}

template<typename CharType, typename Derived>
Derived& StringBase<CharType, Derived>::remove(areg::CharPos startPos, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    if (!is_valid_position(startPos))
        return self();

    if (count == areg::COUNT_ALL)
    {
        mData.erase(static_cast<uint32_t>(startPos));
    }
    else if (count <= (static_cast<areg::CharCount>(mData.length()) - startPos))
    {
        mData.erase(static_cast<uint32_t>(startPos), static_cast<uint32_t>(count));
    }

    return self();
}

template<typename CharType, typename Derived>
Derived& StringBase<CharType, Derived>::remove(const CharType chRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
{
    if (!is_valid_position(startPos))
        return self();

    uint32_t pos = static_cast<uint32_t>(mData.find(chRemove, static_cast<uint32_t>(startPos)));
    for ( ; pos != static_cast<uint32_t>(std::basic_string<CharType>::npos); pos = static_cast<uint32_t>(mData.find(chRemove, static_cast<uint32_t>(pos))))
    {
        mData.erase(pos, 1);
        if ( !removeAll || (pos >= static_cast<uint32_t>(mData.length( ))) )
            break;
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::remove( const CharType* strRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
{
    if (!areg::is_empty<CharType>(strRemove))
        remove(std::basic_string<CharType>(strRemove), startPos, removeAll);

    return self();
}

template<typename CharType, typename Derived>
Derived& StringBase<CharType, Derived>::remove( const std::basic_string<CharType>& strRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
{
    if (!is_valid_position(startPos) || strRemove.empty())
        return self();
    
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

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::remove( const StringBase& strRemove, areg::CharPos startPos /*= areg::START_POS*/, bool removeAll /*= true*/)
{
    return remove(strRemove.mData, startPos, removeAll);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::resize(areg::CharCount count, CharType chFill /*= static_cast<CharType>('\0')*/)
{
    mData.resize(static_cast<uint32_t>(count), chFill);
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::truncate(areg::CharCount maxChars)
{
    if (maxChars == 0)
    {
        mData.clear();
    }
    else if (maxChars < static_cast<areg::CharCount>(mData.length()))
    {
        mData.erase(static_cast<uint32_t>(maxChars));
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::reserve(areg::CharCount newCapacity)
{
    mData.reserve( static_cast<uint32_t>(newCapacity));
    return self();
}

template<typename CharType, typename Derived>
inline CharType StringBase<CharType, Derived>::at(areg::CharPos atPos) const noexcept
{
    return (is_valid_position(atPos) ? mData.at(static_cast<uint32_t>(atPos)) : static_cast<CharType>(areg::EndOfString));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::set_at(CharType ch, areg::CharPos atPos /*= areg::END_POS*/) noexcept
{
    if ((atPos >= areg::START_POS) && (atPos < static_cast<areg::CharPos>(mData.size())))
    {
        mData.at(static_cast<uint32_t>(atPos)) = ch;
    }
    else if (atPos == areg::END_POS)
    {
        mData.append(1, ch);
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::trim_left() noexcept
{
    if (mData.empty())
        return self();

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

    return self();
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::trim_left(StringBase& strResult) const
{
    trim_left(strResult.mData);
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::trim_left(std::basic_string<CharType>& strResult) const
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

    strResult.assign(mData, count, std::basic_string<CharType>::npos);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::trim_right() noexcept
{
    if (mData.empty())
        return self();

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

    return self();
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::trim_right(StringBase& strResult) const
{
    trim_right(strResult.mData);
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::trim_right(std::basic_string<CharType>& strResult) const
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

    strResult.assign(mData, 0, mData.length() - count);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::trim_all() noexcept
{
    if (mData.empty())
        return self();

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

    return self();
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::trim_all(StringBase& strResult) const
{
    trim_all(strResult.mData);
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::trim_all(std::basic_string<CharType>& strResult) const
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
        strResult = mData.substr(left, length - (left + right));
    }
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::make_lower() noexcept
{
    for (CharType * src = mData.data(); *src != StringBase<CharType, Derived>::EmptyChar; ++src)
    {
        *src = areg::make_lower<CharType>(*src);
    }

    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::make_upper() noexcept
{
    for (CharType * src = mData.data(); *src != StringBase<CharType, Derived>::EmptyChar; ++src)
    {
        *src = areg::make_upper<CharType>(*src);
    }

    return self();
}

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::read_line(StringBase& strResult, areg::CharPos startPos /*= areg::START_POS*/) const
{
    return read_line(strResult.mData, startPos);
}

template<typename CharType, typename Derived>
areg::CharPos StringBase<CharType, Derived>::read_line(std::basic_string<CharType>& strResult, areg::CharPos startPos /*= areg::START_POS*/) const
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
    // move until reach end of line
    while ((areg::is_new_line<CharType>(*str) == false) && (*str != EmptyChar))
        ++str;

    // copy the line without the carriage return of "\r\n", find next line or reach end of string
    const CharType* end = ((str != begin) && (*(str - 1) == NewLineDos[0])) ? str - 1 : str;
    strResult.assign(begin, static_cast<uint32_t>(end - begin));
    while (areg::is_eol<CharType>(*str) && (*str != EmptyChar))
        ++str;

    return (*str == EmptyChar ? areg::END_POS : static_cast<areg::CharPos>(str - mData.c_str()));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::make_alphanumeric() noexcept
{
    if (mData.empty())
        return self();

    CharType* begin = buffer(areg::START_POS);
    CharType* dst = begin;
    for (const CharType* src = begin; *src != static_cast<CharType>(areg::EndOfString); ++src)
    {
        if (areg::is_alphanumeric<CharType>(*src))
            *dst++ = *src;
    }

    *dst = static_cast<CharType>(areg::EndOfString);
    mData.resize(static_cast<uint32_t>(dst - begin));

    return self();
}

//////////////////////////////////////////////////////////////////////////////
// Template class StringBase Helper
//////////////////////////////////////////////////////////////////////////

template<typename CharType, typename Derived>
inline areg::Ordering StringBase<CharType, Derived>::compare( areg::CharPos   startPos
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

template<typename CharType, typename Derived>
inline areg::Ordering StringBase<CharType, Derived>::compare_exact( areg::CharPos   startPos
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

template<typename CharType, typename Derived>
inline areg::Ordering StringBase<CharType, Derived>::compare_ignore_case( areg::CharPos   startPos
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

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::replace_with( areg::CharPos   startPos
                                                       , areg::CharCount count
                                                       , const CharType *    strReplace
                                                       , areg::CharCount lenReplace )
{
    if ((startPos == areg::INVALID_POS) || (startPos == areg::END_POS))
        return areg::INVALID_POS;

    int32_t diff = static_cast<int32_t>(lenReplace - count);
    areg::CharPos endPos = startPos + count;
    move_to( endPos, diff );
    // char_traits::copy is SIMD-optimized
    std::char_traits<CharType>::copy(buffer(startPos), strReplace, static_cast<uint32_t>(lenReplace));

    return (endPos + diff);
}

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::replace_with( const CharType * strOrigin
                                                       , areg::CharCount lenOrigin
                                                       , const CharType * strReplace
                                                       , areg::CharCount lenReplace
                                                       , areg::CharPos startPos )
{
    return replace_with( find_first( strOrigin, startPos, true ), lenOrigin, strReplace, lenReplace);
}

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::find_first_phrase( const CharType* phrase
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

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::find_phrase(const CharType* phrase, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
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

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::find_phrase(const std::basic_string<CharType>& phrase, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
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

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::find_first_word(const CharType* word, bool caseSensitive, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
{
    return (word != nullptr ? find_first_word(std::basic_string<CharType>(word), caseSensitive, startPos) : areg::INVALID_POS);
}

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::find_first_word(const std::basic_string<CharType>& word, bool caseSensitive, areg::CharPos startPos /*= areg::START_POS*/) const noexcept
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
            break;  // end of string

        areg::CharPos pos = static_cast<areg::CharPos>(std::distance(mData.begin(), it));

        CharType chBegin= *(it);
        CharType chPrev = it == mData.begin() ? EmptyChar : *(it - 1);
        bool isBegin    = !is_name_char(chBegin) || !is_name_char(chPrev);

        it += static_cast<int32_t>(word.length());
        CharType chLast = *(it - 1);
        CharType chNext = it != mData.end() ? *it : EmptyChar;
        bool isEnd      = !is_name_char(chLast) || !is_name_char(chNext);

        if (isBegin && isEnd)
        {
            result = pos;   // found
            break;
        }

        startPos = pos + 1;
        if ((mData.length() - static_cast<uint32_t>(startPos)) < word.length())
            break;
    }

    return result;
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::is_name_char(const CharType checkChar, std::locale& loc) noexcept
{
    constexpr CharType symbols[] = { '_', '\0' };
    return std::isalnum(checkChar, loc) || areg::is_one_of<CharType>(checkChar, symbols);
}

template<typename CharType, typename Derived>
inline std::vector<Derived> StringBase<CharType, Derived>::split(CharType delimiter) const
{
    std::vector<Derived> result;
    size_t start{ 0 };
    const size_t len{ mData.length() };
    while (start < len)
    {
        size_t pos = mData.find(delimiter, start);
        if (pos != std::basic_string<CharType>::npos)
        {
            result.emplace_back(mData.c_str() + start, static_cast<areg::CharCount>(pos - start));
            start = pos + 1;
        }
        else
        {
            result.emplace_back(mData.c_str() + start, static_cast<areg::CharCount>(len - start));
            break;
        }
    }

    return result;
}

template<typename CharType, typename Derived>
inline std::vector<Derived> StringBase<CharType, Derived>::split(const StringBase & delimiter) const
{
    std::vector<Derived> result;
    if (delimiter.is_empty())
    {
        result.push_back(self());
    }
    else
    {
        const size_t skip   { static_cast<size_t>(delimiter.length()) };
        const size_t len    { mData.length() };
        size_t start        { 0 };
        size_t pos = mData.find(delimiter.mData, start);
        while (pos != std::basic_string<CharType>::npos)
        {
            result.emplace_back(mData.c_str() + start, static_cast<areg::CharCount>(pos - start));
            start = pos + skip;
            pos = (start < len ? mData.find(delimiter.mData, start) : std::basic_string<CharType>::npos);
        }

        if (start < len)
        {
            result.emplace_back(mData.c_str() + start, static_cast<areg::CharCount>(len - start));
        }
    }

    return result;
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::starts_with(const StringBase& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return starts_with(phrase.mData, isCaseSensitive);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::starts_with(const std::basic_string<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return (phrase.length() <= mData.length() ? starts_with(phrase.c_str(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::starts_with(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return (phrase.length() <= mData.length() ? starts_with(phrase.data(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::starts_with(const CharType* phrase, bool isCaseSensitive /*= true*/, areg::CharCount count /*= areg::COUNT_ALL*/) const noexcept
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

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::ends_with(const StringBase& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return ends_with(phrase.mData, isCaseSensitive);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::ends_with(const std::basic_string<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return (phrase.length() <= mData.length() ? ends_with(phrase.c_str(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::ends_with(const std::basic_string_view<CharType>& phrase, bool isCaseSensitive /*= true*/) const noexcept
{
    return (phrase.length() <= mData.length() ? ends_with(phrase.data(), isCaseSensitive, static_cast<areg::CharCount>(phrase.length())) : false);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::ends_with(const CharType* phrase, bool isCaseSensitive /*= true*/, areg::CharCount count /*= areg::COUNT_ALL*/) const noexcept
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

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::_has_phrase(const CharType* fullString, const CharType* phrase, areg::CharCount count, const char* locale) const noexcept
{
    ASSERT((fullString != nullptr) && (phrase != nullptr));

    bool result{ true };
    if (locale == nullptr)
    {
        result = ::memcmp(fullString, phrase, static_cast<uint32_t>(count) * sizeof(CharType)) == 0;
    }
    else
    {
        for (areg::CharCount i = 0; i < count; ++i)
        {
            if (areg::make_lower<CharType>(phrase[i]) != areg::make_lower<CharType>(fullString[i]))
            {
                result = false;
                break;
            }
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// StringBase conversion and formatting
//////////////////////////////////////////////////////////////////////////

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::assign(const OtherCharType* source, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    mData.clear();
    _append_other(source, count != areg::COUNT_ALL ? count : static_cast<areg::CharCount>(_length(source)));
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::append(const OtherCharType* source, areg::CharCount count /*= areg::COUNT_ALL*/)
{
    _append_other(source, count != areg::COUNT_ALL ? count : static_cast<areg::CharCount>(_length(source)));
    return self();
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::substr(const CharType* src, const CharType* strPhrase, const CharType** out_next /*= nullptr*/)
{
    Derived result;
    if (out_next != nullptr)
    {
        *out_next = nullptr;
    }

    if (areg::is_empty<CharType>(src) == false)
    {
        areg::CharPos pos = areg::find_first<CharType>(strPhrase, src, areg::START_POS, true, out_next);
        result.assign(src, areg::is_position_valid(pos) ? pos : areg::COUNT_ALL);
    }

    return result;
}

template<typename CharType, typename Derived>
inline int32_t StringBase<CharType, Derived>::make_int32(const CharType* strDigit, areg::Radix radix /*= areg::Radix::Decimal*/, const CharType** end /*= nullptr*/) noexcept
{
    CharType* next{ nullptr };
    int32_t result{ 0 };
    if (areg::is_empty<CharType>(strDigit) == false)
    {
        if constexpr (std::is_same_v<CharType, char>)
            result = static_cast<int32_t>(std::strtol(strDigit, &next, static_cast<int32_t>(radix)));
        else
            result = static_cast<int32_t>(std::wcstol(strDigit, &next, static_cast<int32_t>(radix)));
    }

    if (end != nullptr)
    {
        *end = next != nullptr ? next : strDigit;
    }

    return result;
}

template<typename CharType, typename Derived>
inline uint32_t StringBase<CharType, Derived>::make_uint32(const CharType* strDigit, areg::Radix radix /*= areg::Radix::Decimal*/, const CharType** end /*= nullptr*/) noexcept
{
    CharType* next{ nullptr };
    uint32_t result{ 0 };
    if (areg::is_empty<CharType>(strDigit) == false)
    {
        if constexpr (std::is_same_v<CharType, char>)
            result = static_cast<uint32_t>(std::strtoul(strDigit, &next, static_cast<int32_t>(radix)));
        else
            result = static_cast<uint32_t>(std::wcstoul(strDigit, &next, static_cast<int32_t>(radix)));
    }

    if (end != nullptr)
    {
        *end = next != nullptr ? next : strDigit;
    }

    return result;
}

template<typename CharType, typename Derived>
inline int64_t StringBase<CharType, Derived>::make_int64(const CharType* strDigit, areg::Radix radix /*= areg::Radix::Decimal*/, const CharType** end /*= nullptr*/) noexcept
{
    CharType* next{ nullptr };
    int64_t result{ 0 };
    if (areg::is_empty<CharType>(strDigit) == false)
    {
        if constexpr (std::is_same_v<CharType, char>)
            result = static_cast<int64_t>(std::strtoll(strDigit, &next, static_cast<int32_t>(radix)));
        else
            result = static_cast<int64_t>(std::wcstoll(strDigit, &next, static_cast<int32_t>(radix)));
    }

    if (end != nullptr)
    {
        *end = next != nullptr ? next : strDigit;
    }

    return result;
}

template<typename CharType, typename Derived>
inline uint64_t StringBase<CharType, Derived>::make_uint64(const CharType* strDigit, areg::Radix radix /*= areg::Radix::Decimal*/, const CharType** end /*= nullptr*/) noexcept
{
    CharType* next{ nullptr };
    uint64_t result{ 0 };
    if (areg::is_empty<CharType>(strDigit) == false)
    {
        if constexpr (std::is_same_v<CharType, char>)
            result = static_cast<uint64_t>(std::strtoull(strDigit, &next, static_cast<int32_t>(radix)));
        else
            result = static_cast<uint64_t>(std::wcstoull(strDigit, &next, static_cast<int32_t>(radix)));
    }

    if (end != nullptr)
    {
        *end = next != nullptr ? next : strDigit;
    }

    return result;
}

template<typename CharType, typename Derived>
inline float StringBase<CharType, Derived>::make_float(const CharType* strDigit, const CharType** end /*= nullptr*/) noexcept
{
    CharType* next{ nullptr };
    float result{ 0.0f };
    if (areg::is_empty<CharType>(strDigit) == false)
    {
        if constexpr (std::is_same_v<CharType, char>)
            result = std::strtof(strDigit, &next);
        else
            result = std::wcstof(strDigit, &next);
    }

    if (end != nullptr)
    {
        *end = next != nullptr ? next : strDigit;
    }

    return result;
}

template<typename CharType, typename Derived>
inline double StringBase<CharType, Derived>::make_double(const CharType* strDigit, const CharType** end /*= nullptr*/) noexcept
{
    CharType* next{ nullptr };
    double result{ 0.0 };
    if (areg::is_empty<CharType>(strDigit) == false)
    {
        if constexpr (std::is_same_v<CharType, char>)
            result = std::strtod(strDigit, &next);
        else
            result = std::wcstod(strDigit, &next);
    }

    if (end != nullptr)
    {
        *end = next != nullptr ? next : strDigit;
    }

    return result;
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::make_bool(const CharType* strBoolean, const CharType** end /*= nullptr*/) noexcept
{
    bool result{ false };
    int32_t lenSkip{ 0 };
    const int32_t lenTrue{ static_cast<int32_t>(areg::BOOLEAN_TRUE.length()) };
    const int32_t lenFalse{ static_cast<int32_t>(areg::BOOLEAN_FALSE.length()) };
    if (areg::compare_strings<CharType, char>(strBoolean, areg::BOOLEAN_TRUE.data(), lenTrue, false) == areg::Ordering::Equal)
    {
        result = true;
        lenSkip = lenTrue;
    }
    else if (areg::compare_strings<CharType, char>(strBoolean, areg::BOOLEAN_FALSE.data(), lenFalse, false) == areg::Ordering::Equal)
    {
        result = false;
        lenSkip = lenFalse;
    }

    if (end != nullptr)
    {
        *end = strBoolean + lenSkip;
    }

    return result;
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::make_string(int32_t number, areg::Radix radix /*= areg::Radix::Decimal*/)
{
    const bool negative{ number < 0 };
    const uint32_t magnitude{ negative ? 0u - static_cast<uint32_t>(number) : static_cast<uint32_t>(number) };
    return _make_unsigned<uint32_t>(magnitude, negative, radix);
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::make_string(uint32_t number, areg::Radix radix /*= areg::Radix::Decimal*/)
{
    return _make_unsigned<uint32_t>(number, false, radix);
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::make_string(int64_t number, areg::Radix radix /*= areg::Radix::Decimal*/)
{
    const bool negative{ number < 0 };
    const uint64_t magnitude{ negative ? 0u - static_cast<uint64_t>(number) : static_cast<uint64_t>(number) };
    return _make_unsigned<uint64_t>(magnitude, negative, radix);
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::make_string(uint64_t number, areg::Radix radix /*= areg::Radix::Decimal*/)
{
    return _make_unsigned<uint64_t>(number, false, radix);
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::make_string(float number)
{
    char buffer[areg::MSG_MIN_BUF_SIZE];
    const int32_t count{ std::snprintf(buffer, areg::MSG_MIN_BUF_SIZE, "%f", static_cast<double>(number)) };
    return Derived(buffer, count > 0 ? std::min(count, areg::MSG_MIN_BUF_SIZE - 1) : 0);
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::make_string(double number)
{
    char buffer[areg::MSG_MIN_BUF_SIZE];
    const int32_t count{ std::snprintf(buffer, areg::MSG_MIN_BUF_SIZE, "%g", number) };
    return Derived(buffer, count > 0 ? std::min(count, areg::MSG_MIN_BUF_SIZE - 1) : 0);
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::make_string(bool value)
{
    const std::string_view & text{ value ? areg::BOOLEAN_TRUE : areg::BOOLEAN_FALSE };
    return Derived(text.data(), static_cast<areg::CharCount>(text.length()));
}

template<typename CharType, typename Derived>
int32_t StringBase<CharType, Derived>::format_string(CharType* strDst, int32_t count, const CharType* format, ...)
{
    if (strDst == nullptr)
        return -1;

    va_list argptr;
    va_start(argptr, format);
    int32_t required{ 0 };
    const int32_t result{ _format_buffer(strDst, count, format, argptr, required) };
    va_end(argptr);
    return result;
}

template<typename CharType, typename Derived>
inline int32_t StringBase<CharType, Derived>::format_string_list(CharType* strDst, int32_t count, const CharType* format, va_list argptr)
{
    int32_t required{ 0 };
    return (strDst != nullptr ? _format_buffer(strDst, count, format, argptr, required) : -1);
}

template<typename CharType, typename Derived>
inline int32_t StringBase<CharType, Derived>::format_string_list(CharType* strDst, int32_t count, const CharType* format, va_list argptr, int32_t& required)
{
    required = 0;
    return (strDst != nullptr ? _format_buffer(strDst, count, format, argptr, required) : -1);
}

template<typename CharType, typename Derived>
Derived& StringBase<CharType, Derived>::format(const CharType* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    format_list(format, argptr);
    va_end(argptr);
    return self();
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::format_list(const CharType* format, va_list argptr)
{
    // The largest buffer tried for a cut text whose length the C library does not report.
    constexpr int32_t maxGrowth{ areg::MSG_EXTRA_BUF_SIZE * 64 };

    mData.clear();
    if (format == nullptr)
        return self();

    CharType buffer[areg::MSG_BUF_SIZE];
    int32_t required{ 0 };
    va_list args;
    va_copy(args, argptr);
    int32_t count{ _format_buffer(buffer, areg::MSG_BUF_SIZE, format, args, required) };
    va_end(args);

    if (required < areg::MSG_BUF_SIZE)
    {
        mData.assign(buffer, static_cast<size_t>(count));
        return self();
    }

    int32_t size{ required + 1 };
    for ( ; ; )
    {
        mData.resize(static_cast<size_t>(size));
        va_copy(args, argptr);
        count = _format_buffer(mData.data(), size, format, args, required);
        va_end(args);
        if ((required < size) || (size >= maxGrowth))
            break;

        size = std::min(size * 2, maxGrowth);
    }

    mData.resize(static_cast<size_t>(count));
    return self();
}

template<typename CharType, typename Derived>
inline int32_t StringBase<CharType, Derived>::to_int32(areg::Radix radix /*= areg::Radix::Decimal*/) const noexcept
{
    return make_int32(mData.c_str(), radix, nullptr);
}

template<typename CharType, typename Derived>
inline uint32_t StringBase<CharType, Derived>::to_uint32(areg::Radix radix /*= areg::Radix::Decimal*/) const noexcept
{
    return make_uint32(mData.c_str(), radix, nullptr);
}

template<typename CharType, typename Derived>
inline int64_t StringBase<CharType, Derived>::to_int64(areg::Radix radix /*= areg::Radix::Decimal*/) const noexcept
{
    return make_int64(mData.c_str(), radix, nullptr);
}

template<typename CharType, typename Derived>
inline uint64_t StringBase<CharType, Derived>::to_uint64(areg::Radix radix /*= areg::Radix::Decimal*/) const noexcept
{
    return make_uint64(mData.c_str(), radix, nullptr);
}

template<typename CharType, typename Derived>
inline float StringBase<CharType, Derived>::to_float() const noexcept
{
    return make_float(mData.c_str(), nullptr);
}

template<typename CharType, typename Derived>
inline double StringBase<CharType, Derived>::to_double() const noexcept
{
    return make_double(mData.c_str(), nullptr);
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::to_bool() const noexcept
{
    return ((mData.empty() == false) && (areg::compare_ignore_case<CharType, char>(mData.c_str(), areg::BOOLEAN_FALSE.data()) != areg::Ordering::Equal));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::from_int32(int32_t value, areg::Radix radix /*= areg::Radix::Decimal*/)
{
    return (*this = make_string(value, radix));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::from_uint32(uint32_t value, areg::Radix radix /*= areg::Radix::Decimal*/)
{
    return (*this = make_string(value, radix));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::from_int64(int64_t value, areg::Radix radix /*= areg::Radix::Decimal*/)
{
    return (*this = make_string(value, radix));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::from_uint64(uint64_t value, areg::Radix radix /*= areg::Radix::Decimal*/)
{
    return (*this = make_string(value, radix));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::from_float(float value)
{
    return (*this = make_string(value));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::from_double(double value)
{
    return (*this = make_string(value));
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::from_bool(bool value)
{
    return (*this = make_string(value));
}

//////////////////////////////////////////////////////////////////////////
// StringBase hidden helpers
//////////////////////////////////////////////////////////////////////////

template<typename CharType, typename Derived>
inline areg::CharPos StringBase<CharType, Derived>::_insert_pos(areg::CharPos pos) const noexcept
{
    const uint32_t len{ static_cast<uint32_t>(mData.length()) };
    if (pos == areg::END_POS)
        return static_cast<areg::CharPos>(len);

    return (static_cast<uint32_t>(pos) <= len ? pos : areg::INVALID_POS);
}

template<typename CharType, typename Derived>
inline std::basic_string_view<CharType> StringBase<CharType, Derived>::_sub_view(std::basic_string_view<CharType> source, areg::CharPos pos, areg::CharCount count) noexcept
{
    if ((pos < areg::START_POS) || (static_cast<size_t>(pos) > source.length()))
        return std::basic_string_view<CharType>();

    const size_t rest{ source.length() - static_cast<size_t>(pos) };
    const size_t take{ (count == areg::COUNT_ALL) || (count < 0) || (static_cast<size_t>(count) > rest) ? rest : static_cast<size_t>(count) };
    return source.substr(static_cast<size_t>(pos), take);
}

template<typename CharType, typename Derived>
template<typename CharSrc>
inline size_t StringBase<CharType, Derived>::_length(const CharSrc* str) noexcept
{
    return (str != nullptr ? std::char_traits<CharSrc>::length(str) : 0u);
}

template<typename CharType, typename Derived>
inline Derived StringBase<CharType, Derived>::_concat(const CharType* lhs, size_t lhsLen, const CharType* rhs, size_t rhsLen)
{
    Derived result;
    result.mData.reserve(lhsLen + rhsLen);
    if (lhsLen != 0)
        result.mData.append(lhs, lhsLen);
    if (rhsLen != 0)
        result.mData.append(rhs, rhsLen);

    return result;
}

template<typename CharType, typename Derived>
constexpr wchar_t StringBase<CharType, Derived>::_widen(char ch) noexcept
{
    return static_cast<wchar_t>(static_cast<unsigned char>(ch));
}

template<typename CharType, typename Derived>
constexpr wchar_t StringBase<CharType, Derived>::_widen(wchar_t ch) noexcept
{
    return ch;
}

template<typename CharType, typename Derived>
inline void StringBase<CharType, Derived>::_append_other(const OtherCharType* source, areg::CharCount count)
{
    if ((source == nullptr) || (count <= 0))
        return;

    const size_t length{ mData.length() };
    mData.resize(length + static_cast<size_t>(count));
    CharType* dst{ mData.data() + length };
    for (areg::CharCount i = 0; i < count; ++i)
    {
        if constexpr (std::is_same_v<CharType, wchar_t>)
            dst[i] = _widen(source[i]);
        else
            dst[i] = static_cast<CharType>(source[i]);
    }
}

template<typename CharType, typename Derived>
inline bool StringBase<CharType, Derived>::_is_equal_other(const OtherCharType* other, size_t count) const noexcept
{
    if (mData.length() != count)
        return false;

    const CharType* mine{ mData.data() };
    for (size_t i = 0; i < count; ++i)
    {
        if (_widen(mine[i]) != _widen(other[i]))
            return false;
    }

    return true;
}

template<typename CharType, typename Derived>
template<uint32_t Base, typename UInt>
inline Derived StringBase<CharType, Derived>::_make_integer(UInt magnitude, bool negative, int32_t minDigits, bool prefix)
{
    constexpr char digits[]{ "0123456789ABCDEF" };
    constexpr int32_t bufSize{ static_cast<int32_t>(sizeof(UInt) * 8 + 4) };

    CharType buffer[bufSize];
    CharType* const end{ buffer + bufSize };
    CharType* pos{ end };
    do
    {
        *(--pos) = static_cast<CharType>(digits[magnitude % Base]);
        magnitude /= Base;
        --minDigits;
    } while (magnitude != 0);

    for ( ; minDigits > 0; --minDigits)
    {
        *(--pos) = static_cast<CharType>('0');
    }

    if (prefix)
    {
        *(--pos) = static_cast<CharType>('x');
        *(--pos) = static_cast<CharType>('0');
    }

    if (negative)
    {
        *(--pos) = static_cast<CharType>('-');
    }

    return Derived(pos, static_cast<areg::CharCount>(end - pos));
}

template<typename CharType, typename Derived>
template<typename UInt>
inline Derived StringBase<CharType, Derived>::_make_unsigned(UInt magnitude, bool negative, areg::Radix radix)
{
    constexpr int32_t digitsOctal{ sizeof(UInt) == sizeof(uint32_t) ? 11 : 22 };
    constexpr int32_t digitsHex{ static_cast<int32_t>(sizeof(UInt) * 2) };

    switch (radix)
    {
    case areg::Radix::Binary:
        return _make_integer<2u, UInt>(magnitude, negative, 1, false);

    case areg::Radix::Octal:
        return _make_integer<8u, UInt>(magnitude, negative, digitsOctal, false);

    case areg::Radix::Hexadecimal:
        return _make_integer<16u, UInt>(magnitude, negative, digitsHex, true);

    case areg::Radix::Decimal:      // fall through
    case areg::Radix::Automatic:    // fall through
    default:
        return _make_integer<10u, UInt>(magnitude, negative, 1, false);
    }
}

template<typename CharType, typename Derived>
inline int32_t StringBase<CharType, Derived>::_format_buffer(CharType* buffer, int32_t count, const CharType* format, va_list argptr, int32_t& required) noexcept
{
    required = 0;
    if ((buffer == nullptr) || (count <= 0))
        return 0;

    buffer[0] = EmptyChar;
    if (format == nullptr)
        return 0;

    int32_t needed{ -1 };

#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

    if constexpr (std::is_same_v<CharType, char>)
    {
        needed = std::vsnprintf(buffer, static_cast<size_t>(count), format, argptr);
    }
    else
    {
#if defined(_WIN32)
        va_list args;
        va_copy(args, argptr);
        needed = ::_vscwprintf(format, args);
        va_end(args);
        if (needed >= 0)
        {
            ::_vsnwprintf_s(buffer, static_cast<size_t>(count), _TRUNCATE, format, argptr);
        }
#else   // !defined(_WIN32)
        errno = 0;
        needed = areg::str::wide_vprintf(buffer, static_cast<size_t>(count), format, argptr);
        if ((needed < 0) && (errno != EILSEQ))
        {
            // The text is cut and its full length is unknown.
            buffer[count - 1] = EmptyChar;
            required = count;
            return static_cast<int32_t>(std::wcslen(buffer));
        }
#endif  // defined(_WIN32)
    }

#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

    if (needed < 0)
    {
        buffer[0] = EmptyChar;
        return 0;
    }

    required = needed;
    return (needed < count ? needed : count - 1);
}

template<typename CharType, typename Derived>
inline Derived& StringBase<CharType, Derived>::self() noexcept
{
    return static_cast<Derived&>(*this);
}

template<typename CharType, typename Derived>
inline const Derived& StringBase<CharType, Derived>::self() const noexcept
{
    return static_cast<const Derived&>(*this);
}

} // namespace areg
#endif  // AREG_BASE_STRINGBASE_HPP
