#ifndef AREG_BASE_STRINGBASEIMPL_HPP
#define AREG_BASE_STRINGBASEIMPL_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/string_base_impl.hpp
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
#include "areg/base/string_base.h"

#include <algorithm>
#include <iostream>
#include <locale>
#include <string>
#include <vector>
namespace areg {

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

//////////////////////////////////////////////////////////////////////////////
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
#endif
