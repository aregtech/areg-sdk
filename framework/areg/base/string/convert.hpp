#ifndef AREG_BASE_STRINGDEFSCONVERT_HPP
#define AREG_BASE_STRINGDEFSCONVERT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/string/convert.hpp
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
#include "areg/base/MathDefs.hpp"
#include "areg/base/TableDefs.hpp"
#include "areg/base/string/string_defs.h"

#include <cstring>
#include <cwchar>
#include <type_traits>

#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
// areg namespace function templates or inline methods implementation
//////////////////////////////////////////////////////////////////////////
/** --------------------------------------------------- **/
inline constexpr uint16_t areg::utf8_char_def(int32_t ch) noexcept
{
    return in_utf8_range(ch) ? areg::UTF8_256_Table[utf8_index(ch)] : static_cast<uint16_t>(areg::CharCategory::Undefined);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_eos(CharType ch) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::EOfS)) != 0);
}

/** --------------------------------------------------- **/
inline constexpr bool areg::in_utf8_range(int32_t ch) noexcept
{
    return (static_cast<uint32_t>(ch) + 128u) <= 383u;
}

/** --------------------------------------------------- **/
inline constexpr uint8_t areg::utf8_index(int32_t ch) noexcept
{
    return static_cast<uint8_t>(ch & 0xFF);
}

/** --------------------------------------------------- **/
template<typename CharType, typename IntType>
int32_t areg::make_string( CharType*        strDst
                         , areg::CharCount  charCount
                         , IntType          digit
                         , areg::Radix      radix ) noexcept
{
    static constexpr char _valid[] = "0123456789ABCDEF";

    radix = (radix != areg::Radix::Automatic) ? radix : areg::Radix::Decimal;
    if (radix < areg::Radix::Binary || radix > areg::Radix::Hexadecimal)
        return 0;

    const IntType base = static_cast<IntType>(radix);
    IntType       num  = areg::abs<IntType>(digit);

    if ((strDst != nullptr) && (charCount > 1))
    {
        // Writing path
        CharType* dst = strDst;
        --charCount; // reserve space for EOS

        do
        {
            *dst++ = static_cast<CharType>(_valid[num % base]);
            num /= base;
            --charCount;
        } while ((num != 0) && (charCount > 0));

        if ((charCount > 0) && (digit < 0) && (radix == areg::Radix::Decimal))
            *dst++ = static_cast<CharType>('-');

        *dst = static_cast<CharType>(areg::EndOfString);
        areg::revert_string<CharType>(strDst);
        return static_cast<int32_t>(dst - strDst);
    }
    else
    {
        int32_t result = (digit < 0 && radix == areg::Radix::Decimal) ? 1 : 0;
        do
        {
            ++result;
            num /= base;
        } while (num != 0);

        return result;
    }
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr int32_t areg::make_integer(const CharType* strNumber, const CharType** remain) noexcept
{
    if (remain != nullptr)
        *remain = nullptr;

    if (areg::is_empty<CharType>(strNumber))
        return 0;

    const CharType negative = areg::as_char(areg::NumericSign::Negative);
    const CharType positive = areg::as_char(areg::NumericSign::Positive);

    areg::NumericSign sign = areg::NumericSign::Undefined;
    uint32_t result = 0;

    for (; *strNumber != static_cast<CharType>(areg::EndOfString); ++strNumber)
    {
        const CharType ch = *strNumber;

        if (areg::is_whitespace<CharType>(ch))
            continue;

        if (sign == areg::NumericSign::Undefined)
        {
            sign = (ch == negative) ? areg::NumericSign::Negative : areg::NumericSign::Positive;
            if ((ch == negative) || (ch == positive))
                continue;
        }

        if (areg::is_numeric<CharType>(ch))
        {
            result = result * 10u + static_cast<uint32_t>(ch - static_cast<CharType>('0'));
        }
        else
        {
            break;
        }
    }

    if ((remain != nullptr) && !areg::is_eos<CharType>(*strNumber))
        *remain = strNumber;

    return static_cast<int32_t>(sign) * static_cast<int32_t>(result);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_eol( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::EndOfLine)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_carriage_return( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::CarReturn)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_new_line( CharType ch ) noexcept
{
    return (ch == static_cast<CharType>(areg::EndOfLine));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_unix_eol(CharType ch) noexcept
{
    return areg::is_new_line<CharType>(ch);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_unix_eol(const CharType * source) noexcept
{
    return (source != nullptr ? areg::is_unix_eol<CharType>(*source) : false);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_dos_eol(CharType ch1, CharType ch2) noexcept
{
    return (areg::is_carriage_return<CharType>(ch1) && areg::is_unix_eol<CharType>(ch2));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_dos_eol(const CharType * source) noexcept
{
    return (source != nullptr ? areg::is_dos_eol<CharType>(*source, *(source + 1)) : false);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_control(CharType ch) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::Control)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_symbol( CharType ch ) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::Symbol)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_whitespace( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::WhiteSpace)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_delimited( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Delimiter)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_letter(CharType ch) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Letter)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_numeric(CharType ch) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Number)) != 0);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_alphanumeric(CharType ch) noexcept
{
    constexpr uint16_t def{ static_cast<uint16_t>(areg::CharCategory::Letter) | static_cast<uint16_t>(areg::CharCategory::Number) };

    return ((areg::utf8_char_def( ch ) & def) != 0);
}

/** --------------------------------------------------- **/
inline constexpr bool areg::is_position_valid(areg::CharPos pos) noexcept
{
    return (pos >= areg::START_POS);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_empty( const CharType * strBuffer ) noexcept
{
    return (strBuffer == nullptr) || (*strBuffer == static_cast<CharType>(areg::EndOfString));
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr areg::CharCount areg::string_length(const CharType* theString) noexcept
{
    if (areg::is_empty<CharType>(theString))
        return 0;

    // std::char_traits<T>::length() is constexpr since C++17 (P0254R2) for char and wchar_t,
    // and delegates to the same SIMD-optimized strlen/wcslen at runtime — no overhead.
    if constexpr (std::is_same_v<CharType, char> || std::is_same_v<CharType, wchar_t>)
    {
        return static_cast<areg::CharCount>(std::char_traits<CharType>::length(theString));
    }
    else
    {
        const CharType* p = theString;
        while (*p != static_cast<CharType>(areg::EndOfString))
            ++p;
        return static_cast<areg::CharCount>(p - theString);
    }
}

/** --------------------------------------------------- **/
template <typename CharType>
inline areg::CharCount areg::string_line_length(const CharType* theString) noexcept
{
    if (areg::is_empty<CharType>(theString))
        return 0;

    auto line_length = [](const CharType* s) -> areg::CharCount
        {
            if constexpr (std::is_same_v<CharType, char>)
            {
                return static_cast<areg::CharCount>(std::strcspn(s, "\r\n"));
            }
            else if constexpr (std::is_same_v<CharType, wchar_t>)
            {
                return static_cast<areg::CharCount>(std::wcscspn(s, L"\r\n"));
            }
            else
            {
                const CharType* p = s;
                while (!areg::is_eol<CharType>(*p) && !areg::is_eos<CharType>(*p))
                    ++p;
                return static_cast<areg::CharCount>(p - s);
            }
        };

    return line_length(theString);
}


/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::not_less(const CharType* str, const uint32_t minCount) noexcept
{
    if (minCount == 0)
        return true;

    if (is_empty(str))
        return false;

    for (uint32_t i = 0u; i < minCount; ++i)
    {
        if (str[i] == static_cast<CharType>(areg::EndOfString))
            return false;
    }

    return true;
}

// Constexpr specializations: char_traits::find() is constexpr in C++17 and delegates to
// memchr/wmemchr (SIMD-optimized) at runtime. Searches for null in first minCount positions —
// if found, the string is shorter than minCount; if not found, it has at least minCount chars.
template<>
inline constexpr bool areg::not_less<char>(const char* str, const uint32_t minCount) noexcept
{
    if (minCount == 0)
        return true;

    if (is_empty(str))
        return false;

    return std::char_traits<char>::find(str, static_cast<std::size_t>(minCount), '\0') == nullptr;
}

template<>
inline constexpr bool areg::not_less<wchar_t>(const wchar_t* str, const uint32_t minCount) noexcept
{
    if (minCount == 0)
        return true;

    if (is_empty(str))
        return false;

    return std::char_traits<wchar_t>::find(str, static_cast<std::size_t>(minCount), L'\0') == nullptr;
}

/** --------------------------------------------------- **/
// Bug fix: original loop ran i < maxCount, missing the null at str[maxCount] for strings of
// exactly maxCount characters. Changed to i <= maxCount to correctly handle that boundary.
template<typename CharType>
inline constexpr bool areg::not_more(const CharType* str, const uint32_t maxCount) noexcept
{
    if (maxCount == 0)
        return is_empty(str);

    if (is_empty(str))
        return true;

    for (uint32_t i = 0u; i <= maxCount; ++i)
    {
        if (str[i] == static_cast<CharType>(areg::EndOfString))
            return true;
    }

    return false;
}

// Constexpr specializations: char_traits::find() is constexpr in C++17, SIMD at runtime.
// Checks maxCount+1 positions so a string of exactly maxCount chars (null at index maxCount)
// correctly returns true — fixes the off-by-one present in naive < maxCount loop.
template<>
inline constexpr bool areg::not_more<char>(const char* str, const uint32_t maxCount) noexcept
{
    if (maxCount == 0)
        return is_empty(str);

    if (is_empty(str))
        return true;

    return std::char_traits<char>::find(str, static_cast<std::size_t>(maxCount) + 1u, '\0') != nullptr;
}

template<>
inline constexpr bool areg::not_more<wchar_t>(const wchar_t* str, const uint32_t maxCount) noexcept
{
    if (maxCount == 0)
        return is_empty(str);

    if (is_empty(str))
        return true;

    return std::char_traits<wchar_t>::find(str, static_cast<std::size_t>(maxCount) + 1u, L'\0') != nullptr;
}


#endif // AREG_BASE_STRINGDEFSCONVERT_HPP
