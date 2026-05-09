#ifndef AREG_BASE_STRINGDEFSFORMAT_HPP
#define AREG_BASE_STRINGDEFSFORMAT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/string/format.hpp
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
#include "areg/base/TableDefs.hpp"

#include <algorithm>
#include <cstring>
#include <cwchar>
#include <limits>
#include <type_traits>

#include <stdarg.h>
#include <wchar.h>
#include "areg/base/string/string_defs.h"

//////////////////////////////////////////////////////////////////////////
// areg namespace function templates or inline methods implementation
//////////////////////////////////////////////////////////////////////////
/** --------------------------------------------------- **/
template<typename CharType>
void areg::revert_string(CharType* strDst, areg::CharCount charCount /*= areg::COUNT_ALL*/) noexcept
{
    if (areg::is_empty<CharType>(strDst))
        return;

    charCount = (charCount == areg::COUNT_ALL) ? areg::string_length<CharType>(strDst) : charCount;
    if (charCount > 1)
        std::reverse(strDst, strDst + charCount);
}

/** --------------------------------------------------- **/
inline constexpr uint32_t areg::make_lower_char(int32_t ch) noexcept
{
    return in_utf8_range(ch) ? static_cast<uint32_t>(areg::UTF8_256_Table_lower[utf8_index(ch)]) : static_cast<uint32_t>(ch);
}

/** --------------------------------------------------- **/
inline constexpr uint32_t areg::make_upper_char(int32_t ch) noexcept
{
    return in_utf8_range(ch) ? static_cast<uint32_t>(areg::UTF8_256_Table_upper[utf8_index(ch)]) : static_cast<uint32_t>(ch);
}

/** --------------------------------------------------- **/
template<typename CharType>
inline int32_t areg::required_char_count( const CharType * format, va_list argptr ) noexcept
{
    if ( areg::is_buffer_fit< areg::MSG_MIN_BUF_SIZE, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0' >(format, argptr))
    {
        return areg::MSG_MIN_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_BUF_SIZE, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0' >( format, argptr ) )
    {
        return areg::MSG_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_BIG_BUF_SIZE, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0' >( format, argptr ) )
    {
        return areg::MSG_BIG_BUF_SIZE;
    }
    else if ( areg::is_buffer_fit< areg::MSG_EXTRA_BUF_SIZE, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0' >( format, argptr ) )
    {
        return areg::MSG_EXTRA_BUF_SIZE;
    }

    return -1;
}

/** --------------------------------------------------- **/
template<typename CharType>
int32_t areg::required_char_count(const CharType* format, ...) noexcept
{
    va_list argptr;
    va_start(argptr, format);
    int32_t charCount = areg::required_char_count(format, argptr);
    va_end(argptr);
    return charCount;
}

/** --------------------------------------------------- **/
template<int32_t size, char dummy>
inline bool areg::is_buffer_fit( const char * format, va_list argptr ) noexcept
{
    char buf[ size ]{ 0 };
    va_list argcopy;
    va_copy( argcopy, argptr );
    // format is intentionally a runtime parameter — suppress -Wformat-nonliteral
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
    int32_t charCount = vsnprintf( buf, static_cast<size_t>(size), format, argcopy );
#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
    va_end( argcopy );

    return (charCount >= 0) && (charCount < size);
}

/** --------------------------------------------------- **/
template<int32_t size, wchar_t dummy>
inline bool areg::is_buffer_fit( const wchar_t * format, va_list argptr ) noexcept
{
    wchar_t buf[ size ]{ 0 };
    va_list argcopy;
    va_copy( argcopy, argptr );
    // format is intentionally a runtime parameter — suppress -Wformat-nonliteral
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
    int32_t charCount = vswprintf( buf, size, format, argcopy );
#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
    va_end( argcopy );

    return (charCount >= 0) && (charCount < size);
}

template<int32_t size, typename CharType>
inline bool areg::is_buffer_fit(const CharType* format, ... ) noexcept
{
    va_list args;
    va_start(args, format);
    bool result = is_buffer_fit<size, sizeof(CharType) == sizeof(wchar_t) ? L'\0' : '\0'>(format, args);
    va_end(args);

    return result;
}

/** --------------------------------------------------- **/
template<typename CharType>
const CharType* areg::line( CharType*         strSource
                          , areg::CharCount  charCount /*= COUNT_ALL*/
                          , CharType**       next  /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (areg::is_empty<CharType>(strSource))
        return strSource;

    const CharType* result = strSource;

    // Scan to the first end-of-line or non-printable character.
    // \r and \n are end-of-line (is_eol); control characters are non-printable (!is_printable).
    // Note: \r and \n ARE printable in the areg table, so is_eol must be checked separately.
    auto find_eol = [](CharType* s, areg::CharCount n) -> CharType*
    {
        areg::CharCount count = (n == areg::COUNT_ALL) ? areg::VALUE_MAX_INT32 : n;
        while (   (count-- > 0)
               && !areg::is_eos<CharType>(*s)
               && !areg::is_eol<CharType>(*s)
               && areg::is_printable<CharType>(*s))
        {
            ++s;
        }

        return s;
    };

    strSource = find_eol(strSource, charCount);

    if (!areg::is_eos<CharType>(*strSource))
    {
        if (areg::is_dos_eol<CharType>(*strSource, *(strSource + 1)))
            *strSource++ = static_cast<CharType>(areg::EndOfString);  // overwrite \r

        *strSource++ = static_cast<CharType>(areg::EndOfString);       // overwrite \n or \r
    }

    if (next != nullptr)
        *next = areg::is_eos<CharType>(*strSource) ? nullptr : strSource;

    return result;
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr const CharType* areg::printable( CharType*       strSource
                                         , areg::CharCount charCount
                                         , CharType**      next /*= nullptr*/ ) noexcept
{
    if (next != nullptr)
        *next = nullptr;

    if (areg::is_empty<CharType>(strSource) || charCount == 0)
        return nullptr;

    const CharType* result = strSource;
    areg::CharCount count  = (charCount == areg::COUNT_ALL) ? std::numeric_limits<areg::CharCount>::max() : charCount;

    while ((count-- > 0) && areg::is_printable<CharType>(*strSource))
        ++strSource;

    if ((next != nullptr) && !areg::is_eos<CharType>(*strSource))
        *next = strSource + 1;

    *strSource = static_cast<CharType>(areg::EndOfString);
    return result;
}



/** --------------------------------------------------- **/
template<typename CharDst, typename CharSrc>
constexpr void areg::trim_all( CharDst*           strDst
                            , areg::CharCount   lenDst
                            , const CharSrc*    strSrc
                            , areg::CharCount   lenSrc /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return;

    *strDst = static_cast<CharDst>(areg::EndOfString);
    if (lenDst <= 0 || is_empty<CharSrc>(strSrc))
        return;

    const areg::CharCount len = (lenSrc == areg::COUNT_ALL) ? areg::string_length<CharSrc>(strSrc) : lenSrc;
    if (len == 0)
        return;

    const CharSrc* begin = strSrc;
    const CharSrc* end   = strSrc + len - 1;

    while ((end > begin) && areg::is_whitespace<CharSrc>(*end))
        --end;

    if (!areg::is_whitespace<CharSrc>(*end))
        ++end;               // exclusive end

    while ((begin < end) && areg::is_whitespace<CharSrc>(*begin))
        ++begin;

    CharDst* dst = strDst;
    for (; (begin < end) && (lenDst > 1); --lenDst)
        *dst++ = static_cast<CharDst>(*begin++);

    *dst = static_cast<CharDst>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharType>
void areg::trim_all(CharType* strBuffer, areg::CharCount strLen /*= areg::COUNT_ALL*/) noexcept
{
    if (is_empty<CharType>(strBuffer))
        return;

    const bool all  = (strLen == areg::COUNT_ALL);
    const areg::CharCount len  = all ? areg::string_length<CharType>(strBuffer) : strLen;
    if (len == 0)
        return;

    const CharType* tail  = strBuffer + len;
    CharType*       begin = strBuffer;
    CharType*       end   = strBuffer + len - 1;

    while ((end > begin) && areg::is_whitespace<CharType>(*end))
        --end;

    if (!areg::is_whitespace<CharType>(*end))
        ++end; // exclusive end

    while ((begin < end) && areg::is_whitespace<CharType>(*begin))
        ++begin;

    CharType* dst = strBuffer;
    if (begin != dst)
    {
        const std::size_t copyLen = static_cast<std::size_t>(end - begin);
        if constexpr (std::is_same_v<CharType, char>)
        {
            std::memmove(dst, begin, copyLen);
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            std::wmemmove(dst, begin, copyLen);
        }
        else
        {
            while (begin < end)
                *dst++ = *begin++;
        }
        dst += copyLen;
    }
    else
    {
        dst = end;
    }

    if (!all)
    {
        while (*tail != static_cast<CharType>(areg::EndOfString))
            *dst++ = *tail++;
    }

    *dst = static_cast<CharType>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharDst, typename CharSrc>
constexpr void areg::trim_right( CharDst*          strDst
                               , areg::CharCount  lenDst
                               , const CharSrc*   strSrc
                               , areg::CharCount  lenSrc /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return;

    *strDst = static_cast<CharDst>(areg::EndOfString);

    if (lenDst <= 0 || is_empty<CharSrc>(strSrc))
        return;

    const areg::CharCount len = (lenSrc == areg::COUNT_ALL) ? areg::string_length<CharSrc>(strSrc) : lenSrc;
    if (len == 0)
        return;

    const CharSrc* begin = strSrc;
    const CharSrc* end   = strSrc + len - 1;

    while ((end != strSrc) && areg::is_whitespace<CharSrc>(*end))
        --end;

    if (!areg::is_whitespace<CharSrc>(*end))
        ++end; // exclusive end

    CharDst* dst = strDst;
    for (; (begin < end) && (lenDst > 1); --lenDst)
        *dst++ = static_cast<CharDst>(*begin++);

    *dst = static_cast<CharDst>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharType>
constexpr void areg::trim_right(CharType* strBuffer, areg::CharCount strLen /*= areg::COUNT_ALL*/) noexcept
{
    if (is_empty<CharType>(strBuffer))
        return;

    const bool            all = (strLen == areg::COUNT_ALL);
    const areg::CharCount len = all ? areg::string_length<CharType>(strBuffer) : strLen;
    if (len == 0)
        return;

    const CharType* tail  = strBuffer + len;
    CharType* begin = strBuffer;
    CharType* end   = strBuffer + len - 1;

    while ((end > begin) && areg::is_whitespace<CharType>(*end))
        --end;

    CharType* dst = areg::is_whitespace<CharType>(*end) ? end : ++end;
    if (!all)
    {
        while (*tail != static_cast<CharType>(areg::EndOfString))
            *dst++ = *tail++;
    }

    *dst = static_cast<CharType>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharDst, typename CharSrc>
constexpr void areg::trim_left( CharDst*          strDst
                              , areg::CharCount  lenDst
                              , const CharSrc*   strSrc
                              , areg::CharCount  lenSrc /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return;

    *strDst = static_cast<CharDst>(areg::EndOfString);
    if (lenDst <= 0 || is_empty<CharSrc>(strSrc))
        return;

    const areg::CharCount len = (lenSrc == areg::COUNT_ALL) ? areg::string_length<CharSrc>(strSrc) : lenSrc;
    if (len == 0)
        return;

    const CharSrc* begin = strSrc;
    const CharSrc* end   = strSrc + len;

    while ((begin < end) && areg::is_whitespace<CharSrc>(*begin))
        ++begin;

    CharDst* dst = strDst;
    for (; (begin < end) && (lenDst > 1); --lenDst)
        *dst++ = static_cast<CharDst>(*begin++);

    *dst = static_cast<CharDst>(areg::EndOfString);
}

/** --------------------------------------------------- **/
template<typename CharType>
void areg::trim_left(CharType* strBuffer, areg::CharCount strLen /*= areg::COUNT_ALL*/) noexcept
{
    if (is_empty<CharType>(strBuffer))
        return;

    const bool all  = (strLen == areg::COUNT_ALL);
    const areg::CharCount len  = all ? areg::string_length<CharType>(strBuffer) : strLen;
    if (len == 0)
        return;

    const CharType* tail  = strBuffer + len;
    const CharType* begin = strBuffer;
    const CharType* end   = tail;

    while ((begin < end) && areg::is_whitespace<CharType>(*begin))
        ++begin;

    if (begin != strBuffer)
    {
        const std::size_t copyLen = static_cast<std::size_t>(end - begin);
        CharType* dst = strBuffer;
        if constexpr (std::is_same_v<CharType, char>)
        {
            std::memmove(dst, begin, copyLen);
        }
        else if constexpr (std::is_same_v<CharType, wchar_t>)
        {
            std::wmemmove(dst, begin, copyLen);
        }
        else
        {
            while (begin < end)
                *dst++ = *begin++;
        }
        dst += copyLen;

        if (!all)
        {
            while (*tail != static_cast<CharType>(areg::EndOfString))
                *dst++ = *tail++;
        }

        *dst = static_cast<CharType>(areg::EndOfString);
    }
}


/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_readable( CharType ch ) noexcept
{
    constexpr uint16_t def{ static_cast<uint16_t>(areg::CharCategory::Letter) |
                            static_cast<uint16_t>(areg::CharCategory::Number) |
                            static_cast<uint16_t>(areg::CharCategory::Symbol) };

    return (((areg::utf8_char_def( ch ) & def) != 0) || (ch == ' '));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_printable( CharType ch ) noexcept
{
    return ((areg::utf8_char_def( ch ) & static_cast<uint16_t>(areg::CharCategory::Printable)) != 0);
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr CharType areg::make_lower(CharType ch) noexcept
{
    // return ((ch >= 'A') && (ch <= 'Z') ? ch - 'A' + 'a' : ch);
    return static_cast<CharType>(areg::make_lower_char(ch));
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr const CharType* areg::make_lower(CharType* source) noexcept
{
    if (areg::is_empty<CharType>(source))
        return source;

    const CharType* result{ source };
    while (areg::is_eos<CharType>(*source) == false)
    {
        *source = areg::make_lower<CharType>(*source);
        ++source;
    }

    return result;
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr CharType areg::make_upper(CharType ch) noexcept
{
    // return ((ch >= 'a') && (ch <= 'z') ? ch - 'a' + 'A' : ch);
    return static_cast<CharType>(areg::make_upper_char(ch));
}

/** --------------------------------------------------- **/
template <typename CharType>
inline constexpr const CharType* areg::make_upper(CharType* source) noexcept
{
    if (areg::is_empty<CharType>(source))
        return source;

    const CharType* result{ source };
    while (areg::is_eos<CharType>(*source) == false)
    {
        *source = areg::make_upper<CharType>(*source);
        ++source;
    }

    return result;
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_lower(CharType ch) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::LetterLo)) == static_cast<uint16_t>(areg::CharCategory::LetterLo));
}

/** --------------------------------------------------- **/
template<typename CharType>
inline constexpr bool areg::is_upper(CharType ch) noexcept
{
    return ((areg::utf8_char_def(ch) & static_cast<uint16_t>(areg::CharCategory::LetterUp)) == static_cast<uint16_t>(areg::CharCategory::LetterUp));
}

/** --------------------------------------------------- **/
template<typename CharDst, typename CharSrc>
areg::CharCount areg::copy_string( CharDst*          strDst
                                 , areg::CharCount   dstSpace
                                 , const CharSrc*    strSrc
                                 , areg::CharCount   charsCopy /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return 0;

    if constexpr (sizeof(CharSrc) == sizeof(CharDst))
    {
        charsCopy = (charsCopy == areg::COUNT_ALL) ? areg::string_length<CharSrc>(strSrc) : charsCopy;
        const uint32_t result = areg::mem_copy( strDst
                                              , static_cast<uint32_t>(static_cast<uint32_t>(dstSpace)  * sizeof(CharDst))
                                              , strSrc
                                              , static_cast<uint32_t>(static_cast<uint32_t>(charsCopy) * sizeof(CharSrc))) / static_cast<uint32_t>(sizeof(CharDst));
        strDst[result] = static_cast<CharDst>(areg::EndOfString);
        return static_cast<areg::CharCount>(result);
    }
    else
    {
        if (dstSpace <= 0 || strSrc == nullptr)
        {
            strDst[0] = static_cast<CharDst>(areg::EndOfString);
            return 0;
        }

        charsCopy = (charsCopy == areg::COUNT_ALL) ? std::numeric_limits<areg::CharCount>::max() : charsCopy;
        charsCopy = std::min(dstSpace - 1, charsCopy);

        CharDst* dst = strDst;
        while ((*strSrc != static_cast<CharSrc>(areg::EndOfString)) && (charsCopy-- > 0))
            *dst++ = static_cast<CharDst>(*strSrc++);

        *dst = static_cast<CharDst>(areg::EndOfString);
        return static_cast<areg::CharCount>(dst - strDst);
    }
}

/** --------------------------------------------------- **/
template<typename CharType>
areg::CharCount areg::copy_string_fast( CharType*         strDst
                                      , const CharType*   strSrc
                                      , areg::CharCount   charsCopy /*= areg::COUNT_ALL*/ ) noexcept
{
    if (strDst == nullptr)
        return 0;

    charsCopy = (charsCopy == areg::COUNT_ALL) ? areg::string_length<CharType>(strSrc) : charsCopy;

    auto do_copy = [&]() -> areg::CharCount
    {
        if constexpr (std::is_same_v<CharType, char>)
            std::memcpy(strDst, strSrc, static_cast<size_t>(charsCopy));
        else if constexpr (std::is_same_v<CharType, wchar_t>)
            std::wmemcpy(strDst, strSrc, static_cast<size_t>(charsCopy));
        else
            areg::mem_copy(strDst, charsCopy * sizeof(CharType), strSrc, static_cast<size_t>(charsCopy * sizeof(CharType)));

        strDst[charsCopy] = static_cast<CharType>(areg::EndOfString);
        return charsCopy;
    };

    return do_copy();
}


#endif // AREG_BASE_STRINGDEFSFORMAT_HPP
