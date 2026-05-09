#ifndef AREG_BASE_STRINGDEFSSEARCH_HPP
#define AREG_BASE_STRINGDEFSSEARCH_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/string/search.hpp
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

#include "areg/base/string/string_defs.h"

#include <cstring>
#include <cwchar>
#include <string_view>
#include <type_traits>

#include <stdarg.h>

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_one_of(CharType ch, const CharType* chSequence) noexcept
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

//////////////////////////////////////////////////////////////////////////
// areg namespace function templates or inline methods implementation
//////////////////////////////////////////////////////////////////////////

namespace areg::str {

    // Inline character equality with case sensitivity resolved at compile time.
    // Non-capturing; the compiler eliminates the dead branch in each instantiation.
    template<bool CaseSensitive, typename CharType>
    [[nodiscard]] inline constexpr bool char_eq(CharType a, CharType b) noexcept;
    // Inline three-way character comparison with case sensitivity resolved at compile time.
    template<bool CaseSensitive, typename CharLhs, typename CharRhs>
    [[nodiscard]] inline constexpr areg::Ordering char_cmp(CharLhs a, CharRhs b) noexcept;

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    [[nodiscard]] constexpr areg::CharPos find_first_phrase_impl(const CharType* strPhrase, const CharType* strSource, areg::CharPos startPos, const CharType** next) noexcept
    {
        const CharType* str = !areg::is_empty(strSource) ? strSource + startPos : nullptr;
        if (areg::is_empty(str))
            return areg::INVALID_POS;

        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::find(string_view) is constexpr in C++17 and SIMD-optimized at runtime
            // via char_traits — equivalent to strstr/wcsstr without losing constexpr eligibility.
            using SV = std::basic_string_view<CharType>;
            const SV sv{ str };
            const SV phrase_sv{ strPhrase };
            const auto pos = sv.find(phrase_sv);
            if (pos == SV::npos)
                return areg::INVALID_POS;

            const CharType* after = str + pos + phrase_sv.size();
            if ((next != nullptr) && (*after != static_cast<CharType>(areg::EndOfString)))
                *next = after;

            return static_cast<areg::CharPos>(startPos + static_cast<areg::CharPos>(pos));
        }
        else
        {
            // Generic path: case-insensitive or non-standard char types.
            while (!areg::is_eos(*str))
            {
                if (char_eq<CaseSensitive, CharType>(*str, *strPhrase))
                {
                    const CharType* one = str + 1;
                    const CharType* two = strPhrase + 1;
                    while ((*two != static_cast<CharType>(areg::EndOfString)) && char_eq<CaseSensitive, CharType>(*one, *two))
                    {
                        ++one; ++two;
                    }

                    if (areg::is_eos(*two))
                    {
                        if ((next != nullptr) && (*one != static_cast<CharType>(areg::EndOfString)))
                            *next = one;
                        return static_cast<areg::CharPos>(str - strSource);
                    }
                }

                ++str;
            }

            return areg::INVALID_POS;
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    [[nodiscard]] constexpr bool string_starts_with_impl(const CharType* strString, const CharType* phrase) noexcept
    {
        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::compare() is constexpr in C++17 and SIMD-optimized at runtime
            // via char_traits — equivalent to strncmp/wcsncmp without losing constexpr eligibility.
            using SV = std::basic_string_view<CharType>;
            const SV phrase_sv{ phrase };
            const SV str_sv{ strString };
            return str_sv.compare(0, phrase_sv.size(), phrase_sv) == 0;
        }
        else
        {
            while (!areg::is_eos(*phrase) && !areg::is_eos(*strString))
            {
                if (!char_eq<CaseSensitive, CharType>(*strString++, *phrase++))
                    return false;
            }

            return areg::is_eos(*phrase);
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    [[nodiscard]] CharType* remove_char_impl(CharType ch1, const CharType* src, CharType* dst, bool removeAll) noexcept
    {
        if constexpr (CaseSensitive && std::is_same_v<CharType, char>)
        {
            // Fast path: use memchr to skip to each matching character, then memmove the
            // gap between consecutive matches in bulk.
            const char* p = src;
            char* out = dst;
            while (true)
            {
                const char* found = static_cast<const char*>(std::memchr(p, static_cast<int>(ch1), std::strlen(p)));
                if (found == nullptr)
                {
                    // No more matches — copy tail.
                    const std::size_t tail = static_cast<std::size_t>(std::strlen(p));
                    if (tail > 0)
                        std::memmove(out, p, tail);
                    out += tail;
                    break;
                }

                const std::size_t chunk = static_cast<std::size_t>(found - p);
                if (chunk > 0)
                    std::memmove(out, p, chunk);
                out += chunk;
                p = found + 1;  // skip the matched character

                if (!removeAll)
                {
                    // Copy the remainder verbatim so the string stays valid, then
                    // return the position right after the removed character.
                    // The caller uses this as the starting point for the next search.
                    const std::size_t tail = static_cast<std::size_t>(std::strlen(p));
                    char* resume = out;
                    if (tail > 0)
                        std::memmove(out, p, tail);
                    out[tail] = '\0';
                    return resume;
                }
            }

            CharType* result = out;
            *out = '\0';
            return result;
        }
        else if constexpr (CaseSensitive && std::is_same_v<CharType, wchar_t>)
        {
            const wchar_t* p = src;
            wchar_t* out = dst;
            while (true)
            {
                const wchar_t* found = static_cast<const wchar_t*>(std::wmemchr(p, static_cast<wchar_t>(ch1), std::wcslen(p)));
                if (found == nullptr)
                {
                    const std::size_t tail = static_cast<std::size_t>(std::wcslen(p));
                    if (tail > 0)
                        std::wmemmove(out, p, tail);
                    out += tail;
                    break;
                }

                const std::size_t chunk = static_cast<std::size_t>(found - p);
                if (chunk > 0)
                    std::wmemmove(out, p, chunk);
                out += chunk;
                p = found + 1;

                if (!removeAll)
                {
                    // Copy the remainder verbatim so the string stays valid, then
                    // return the position right after the removed character.
                    const std::size_t tail = static_cast<std::size_t>(std::wcslen(p));
                    wchar_t* resume = out;
                    if (tail > 0)
                        std::wmemmove(out, p, tail);
                    out[tail] = L'\0';
                    return resume;
                }
            }

            CharType* result = out;
            *out = L'\0';
            return result;
        }
        else
        {
            while (!areg::is_eos(*src))
            {
                if (char_eq<CaseSensitive, CharType>(ch1, *src))
                {
                    ++src;
                    if (!removeAll)
                        break;
                }
                else
                {
                    *dst++ = *src++;
                }
            }

            CharType* result = dst;
            while (!areg::is_eos(*src))
                *dst++ = *src++;
            *dst = static_cast<CharType>(areg::EndOfString);
            return result;
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharLhs, typename CharRhs>
    [[nodiscard]] areg::Ordering compare_strings_impl(const CharLhs* left_side, const CharRhs* right_side, areg::CharPos charCount) noexcept
    {
        const bool countAll{ charCount == areg::COUNT_ALL };
        while (countAll || charCount-- > 0)
        {
            const areg::Ordering cmp{ char_cmp<CaseSensitive, CharLhs, CharRhs>(*left_side, *right_side) };
            if (cmp != areg::Ordering::Equal)
                return cmp;

            if (areg::is_eos(*left_side))
                return areg::Ordering::Equal;

            ++left_side;
            ++right_side;
        }
        return areg::Ordering::Equal;
    }

    /** --------------------------------------------------- **/
    // Inline character equality with case sensitivity resolved at compile time.
    // Non-capturing; the compiler eliminates the dead branch in each instantiation.
    template<bool CaseSensitive, typename CharType>
    [[nodiscard]] inline constexpr bool char_eq(CharType a, CharType b) noexcept
    {
        if constexpr (CaseSensitive)
            return a == b;
        else
            return areg::make_lower<CharType>(a) == areg::make_lower<CharType>(b);
    }

    /** --------------------------------------------------- **/
    // Inline three-way character comparison with case sensitivity resolved at compile time.
    template<bool CaseSensitive, typename CharLhs, typename CharRhs>
    [[nodiscard]] inline constexpr areg::Ordering char_cmp(CharLhs a, CharRhs b) noexcept
    {
        if constexpr (CaseSensitive)
        {
            const auto rb = static_cast<CharLhs>(b);
            return (a == rb) ? areg::Ordering::Equal : (a < rb ? areg::Ordering::Smaller : areg::Ordering::Bigger);
        }
        else
        {
            const CharLhs la = areg::make_lower<CharLhs>(a);
            const CharLhs lb = static_cast<CharLhs>(areg::make_lower<CharRhs>(b));
            return (la == lb) ? areg::Ordering::Equal : (la < lb ? areg::Ordering::Smaller : areg::Ordering::Bigger);
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    [[nodiscard]] constexpr areg::CharPos find_last_char_impl(CharType ch, const CharType* strSource, areg::CharPos pos, const CharType** next) noexcept
    {
        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::rfind() is constexpr in C++17 and SIMD-optimized at runtime.
            using SV = std::basic_string_view<CharType>;
            const SV sv{ strSource };
            const auto found = sv.rfind(ch, static_cast<std::size_t>(pos));
            if (found == SV::npos)
                return areg::INVALID_POS;

            if (next != nullptr)
                *next = strSource + found;

            return static_cast<areg::CharPos>(found);
        }
        else
        {
            for (const CharType* p = strSource + pos; p >= strSource; --p)
            {
                if (areg::str::char_eq<CaseSensitive, CharType>(*p, ch))
                {
                    if (next != nullptr)
                        *next = p;

                    return static_cast<areg::CharPos>(p - strSource);
                }
            }

            return areg::INVALID_POS;
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    [[nodiscard]] constexpr areg::CharPos find_last_phrase_impl(const CharType* strPhrase, const CharType* strSource, areg::CharPos pos, areg::CharPos lenPhr, const CharType** next) noexcept
    {
        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::rfind() is constexpr in C++17 and SIMD-optimized at runtime.
            // Search start is capped at (pos - lenPhr): the last valid phrase-start position.
            if (static_cast<areg::CharPos>(lenPhr) > pos)
                return areg::INVALID_POS;

            using SV = std::basic_string_view<CharType>;
            const SV sv{ strSource };
            const SV phrase_sv{ strPhrase, static_cast<typename SV::size_type>(lenPhr) };
            const auto found = sv.rfind(phrase_sv, static_cast<typename SV::size_type>(pos - lenPhr));
            if (found == SV::npos)
                return areg::INVALID_POS;

            if (next != nullptr)
                *next = strSource + found;

            return static_cast<areg::CharPos>(found);
        }
        else
        {
            const CharType* end = strSource + pos - 1;
            const CharType* phrase = strPhrase + lenPhr - 1;

            for (; end >= strSource; --end)
            {
                if (areg::str::char_eq<CaseSensitive, CharType>(*end, *phrase))
                {
                    const CharType* one = end - 1;
                    const CharType* two = phrase - 1;
                    while ((one >= strSource) && (two >= strPhrase) && areg::str::char_eq<CaseSensitive, CharType>(*one, *two))
                    {
                        --one; --two;
                    }

                    if (two < strPhrase)
                    {
                        ++one;
                        if (next != nullptr)
                            *next = one;

                        return static_cast<areg::CharPos>(one - strSource);
                    }
                }
            }
            return areg::INVALID_POS;
        }
    }

    /** --------------------------------------------------- **/
    template<bool CaseSensitive, typename CharType>
    [[nodiscard]] constexpr areg::CharPos find_first_char_impl(CharType ch, const CharType* strSource, areg::CharPos startPos, const CharType** next) noexcept
    {
        const CharType* str = !areg::is_empty(strSource) ? strSource + startPos : nullptr;
        if (areg::is_empty(str))
            return areg::INVALID_POS;

        if constexpr (CaseSensitive && (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>))
        {
            // string_view::find(char) is constexpr in C++17 and SIMD-optimized at runtime
            // via char_traits::find() — equivalent to strchr/wcschr without losing constexpr eligibility.
            using SV = std::basic_string_view<CharType>;
            const SV sv{ str };
            const auto pos = sv.find(ch);
            if (pos == SV::npos)
                return areg::INVALID_POS;

            if ((next != nullptr) && (pos + 1 < sv.size()))
                *next = str + pos + 1;

            return static_cast<areg::CharPos>(startPos + static_cast<areg::CharPos>(pos));
        }
        else
        {
            while (*str != static_cast<CharType>(areg::EndOfString))
            {
                if (areg::str::char_eq<CaseSensitive, CharType>(*str, ch))
                {
                    const areg::CharPos result = static_cast<areg::CharPos>(str - strSource);
                    ++str;
                    if ((next != nullptr) && (*str != static_cast<CharType>(areg::EndOfString)))
                        *next = str;
                    return result;
                }

                ++str;
            }

            return areg::INVALID_POS;
        }
    }
} // namespace areq::str

/** --------------------------------------------------- **/
template<typename CharType>
constexpr areg::CharPos areg::find_last( CharType         chSearch
                                       , const CharType*  strSource
                                       , areg::CharPos    startPos      /*= areg::END_POS*/
                                       , bool             caseSensitive /*= true*/
                                       , const CharType** next      /*= nullptr*/) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (is_empty<CharType>(strSource) || (chSearch == static_cast<CharType>(areg::EndOfString)))
        return areg::INVALID_POS;

    const areg::CharPos len = areg::string_length<CharType>(strSource);
    const areg::CharPos pos = (startPos == areg::END_POS || startPos >= len) ? len - 1 : startPos;
    if (pos < areg::START_POS)
        return areg::INVALID_POS;

    const CharType ch = caseSensitive ? chSearch : areg::make_lower<CharType>(chSearch);
    return  ( caseSensitive
            ? areg::str::find_last_char_impl<true, CharType>(ch, strSource, pos, next)
            : areg::str::find_last_char_impl<false, CharType>(ch, strSource, pos, next));
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr areg::CharPos areg::find_last( const CharType*   strPhrase
                                       , const CharType*   strSource
                                       , areg::CharPos     startPos      /*= areg::END_POS*/
                                       , bool              caseSensitive /*= true*/
                                       , const CharType**  next      /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (is_empty<CharType>(strSource) || is_empty<CharType>(strPhrase))
        return areg::INVALID_POS;

    const areg::CharPos lenSrc = areg::string_length<CharType>(strSource);
    const areg::CharPos lenPhr = areg::string_length<CharType>(strPhrase);
    const areg::CharPos pos    = (startPos == areg::END_POS) ? lenSrc : startPos;

    if (pos <= areg::START_POS || lenPhr <= areg::START_POS)
        return areg::INVALID_POS;

    return (  caseSensitive 
           ? areg::str::find_last_phrase_impl<true, CharType>(strPhrase, strSource, pos, lenPhr, next)
           : areg::str::find_last_phrase_impl<false, CharType>(strPhrase, strSource, pos, lenPhr, next));
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr areg::CharPos areg::find_first( CharType          chSearch
                                        , const CharType*   strSource
                                        , areg::CharPos     startPos      /*= areg::START_POS*/
                                        , bool              caseSensitive /*= true*/
                                        , const CharType**  next      /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (areg::is_empty<CharType>(strSource) || (chSearch == static_cast<CharType>(areg::EndOfString)) || startPos < areg::START_POS)
        return areg::INVALID_POS;

    const CharType ch = caseSensitive ? chSearch : areg::make_lower<CharType>(chSearch);
    return  ( caseSensitive 
            ? areg::str::find_first_char_impl<true, CharType>(ch, strSource, startPos, next)
            : areg::str::find_first_char_impl<false, CharType>(ch, strSource, startPos, next));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr areg::CharPos areg::find_first( const CharType*   strPhrase
                                        , const CharType*   strSource
                                        , areg::CharPos     startPos      /*= areg::START_POS*/
                                        , bool              caseSensitive /*= true*/
                                        , const CharType**  next      /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (areg::is_empty<CharType>(strSource) || areg::is_empty<CharType>(strPhrase) || startPos < areg::START_POS)
        return areg::INVALID_POS;

    return  ( caseSensitive 
            ? areg::str::find_first_phrase_impl<true>(strPhrase, strSource, startPos, next)
            : areg::str::find_first_phrase_impl<false>(strPhrase, strSource, startPos, next));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::string_starts_with(const CharType* strString, const CharType* phrase, bool caseSensitive /*= true*/) noexcept
{
    if (is_empty<CharType>(strString) || is_empty<CharType>(phrase))
        return false;

    return caseSensitive ? areg::str::string_starts_with_impl<true>(strString, phrase) : areg::str::string_starts_with_impl<false>(strString, phrase);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::string_starts_with(const CharType* strString, const CharType ch, bool caseSensitive /*= true*/) noexcept
{
    if (is_empty<CharType>(strString))
        return false;

    return caseSensitive ? (*strString == ch) : (areg::make_lower<CharType>(*strString) == areg::make_lower<CharType>(ch));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::string_ends_with(const CharType* strString, const CharType* phrase, bool caseSensitive /*= true*/) noexcept
{
    if (is_empty<CharType>(strString) || is_empty<CharType>(phrase))
        return false;

    const int32_t diff = areg::string_length<CharType>(strString) - areg::string_length<CharType>(phrase);
    return (diff >= 0) && areg::string_starts_with<CharType>(strString + diff, phrase, caseSensitive);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::string_ends_with(const CharType* strString, const CharType ch, bool caseSensitive /*= true*/) noexcept
{
    if (is_empty<CharType>(strString))
        return false;

    const CharType last = strString[areg::string_length<CharType>(strString) - 1];
    return caseSensitive ? (last == ch) : (areg::make_lower<CharType>(last) == areg::make_lower<CharType>(ch));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr CharType* areg::remove_char(const CharType chRemove, CharType* strSource, bool removeAll /*= true*/, bool caseSensitive /*= true*/) noexcept
{
    const CharType ch1 = caseSensitive ? chRemove : areg::make_lower<CharType>(chRemove);
    return  ( caseSensitive
            ? areg::str::remove_char_impl<true, CharType>(ch1, strSource, strSource, removeAll)
            : areg::str::remove_char_impl<false, CharType>(ch1, strSource, strSource, removeAll));
}

/** --------------------------------------------------- **/
template<typename CharLhs, typename CharRhs>
areg::Ordering areg::compare_strings( const CharLhs*   left_side
                                    , const CharRhs*   right_side
                                    , areg::CharCount  charCount    /*= COUNT_ALL*/
                                    , bool             caseSensitive /*= true*/ ) noexcept
{
    if (reinterpret_cast<const void*>(left_side) == reinterpret_cast<const void*>(right_side))
        return areg::Ordering::Equal;

    if (left_side == nullptr)
        return (right_side != nullptr) ? areg::Ordering::Smaller : areg::Ordering::Equal;

    if (right_side == nullptr)
        return areg::Ordering::Bigger;

    // Fast path: same char type + case-sensitive, SIMD-optimized stdlib
    if constexpr (std::is_same_v<CharLhs, CharRhs>)
    {
        if (caseSensitive)
        {
            int cmp = 0;
            if constexpr (std::is_same_v<CharLhs, char>)
            {
                cmp = (charCount == areg::COUNT_ALL)
                    ? std::strcmp(left_side, right_side)
                    : std::strncmp(left_side, right_side, static_cast<std::size_t>(charCount));
            }
            else if constexpr (std::is_same_v<CharLhs, wchar_t>)
            {
                cmp = (charCount == areg::COUNT_ALL)
                    ? std::wcscmp(left_side, right_side)
                    : std::wcsncmp(left_side, right_side, static_cast<std::size_t>(charCount));
            }

            if (cmp == 0) return areg::Ordering::Equal;
            return (cmp < 0) ? areg::Ordering::Smaller : areg::Ordering::Bigger;
        }
    }

    // Generic path: different char types or case-insensitive
    return (caseSensitive 
                ? areg::str::compare_strings_impl<true, CharLhs, CharRhs>(left_side, right_side, charCount) 
                : areg::str::compare_strings_impl<false, CharLhs, CharRhs>(left_side, right_side, charCount));
}

/** --------------------------------------------------- **/
template<typename CharLhs, typename CharRhs>
inline areg::Ordering areg::compare_ignore_case(const CharLhs* left_side, const CharRhs* right_side, areg::CharCount count /*= areg::COUNT_ALL*/) noexcept
{
    return areg::compare_strings<CharLhs, CharRhs>(left_side, right_side, count, false);
}

/** --------------------------------------------------- **/
template<typename CharLhs, typename CharRhs>
inline areg::Ordering areg::compare(const CharLhs* left_side, const CharRhs* right_side) noexcept
{
    return areg::compare_strings<CharLhs, CharRhs>(left_side, right_side, areg::COUNT_ALL, true);
}

/** --------------------------------------------------- **/
template<typename CharLhs, typename CharRhs>
inline areg::Ordering areg::compare(const CharLhs* left_side, const CharRhs* right_side, areg::CharCount count) noexcept
{
    return areg::compare_strings<CharLhs, CharRhs>(left_side, right_side, count, true);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline areg::Ordering areg::compare_fast(const CharType* left_side, const CharType* right_side, areg::CharCount count) noexcept
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

// Constexpr specializations: string_view::find() is constexpr in C++17 and delegates to
// char_traits::find() which is SIMD-optimized at runtime — best of both worlds.
template<>
inline constexpr bool areg::is_one_of<char>(char ch, const char* chSequence) noexcept
{
    if ((chSequence == nullptr) || (ch == static_cast<char>(EndOfString)))
        return false;

    return std::string_view{ chSequence }.find(ch) != std::string_view::npos;
}

template<>
inline constexpr bool areg::is_one_of<wchar_t>(wchar_t ch, const wchar_t* chSequence) noexcept
{
    if ((chSequence == nullptr) || (ch == static_cast<wchar_t>(EndOfString)))
        return false;

    return std::wstring_view{ chSequence }.find(ch) != std::wstring_view::npos;
}


#endif // AREG_BASE_STRINGDEFSSEARCH_HPP
