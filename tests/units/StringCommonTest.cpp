/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech. All rights reserved.
 * \file        units/StringCommonTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              The unit test of StringBase operations, run for String and WideString.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace
{
    template<typename S> struct StrChar;
    template<> struct StrChar<areg::String>     { using Type = char;    };
    template<> struct StrChar<areg::WideString> { using Type = wchar_t; };

    template<typename C> constexpr const C* pick(const char* narrow, const wchar_t* wide);
    template<> constexpr const char*    pick<char>(const char* narrow, const wchar_t*)  { return narrow; }
    template<> constexpr const wchar_t* pick<wchar_t>(const char*, const wchar_t* wide) { return wide; }

    //!< Returns the ASCII text as a standard string of the character type.
    template<typename C>
    std::basic_string<C> expected(const char* ascii)
    {
        std::basic_string<C> result;
        for (; *ascii != '\0'; ++ascii)
            result += static_cast<C>(static_cast<unsigned char>(*ascii));
        return result;
    }

    //!< Returns the text the printf family writes for the format and the value.
    template<typename C, typename Value>
    std::basic_string<C> printed(const char* format, Value value)
    {
        char buffer[128]{ 0 };
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
        std::snprintf(buffer, sizeof(buffer), format, value);
#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
        return expected<C>(buffer);
    }

    //!< Returns the number written in binary digits, with a minus sign if negative.
    template<typename C, typename UInt>
    std::basic_string<C> binary(UInt magnitude, bool negative)
    {
        std::basic_string<C> digits;
        do
        {
            digits.insert(digits.begin(), static_cast<C>((magnitude & 1u) != 0 ? '1' : '0'));
            magnitude >>= 1;
        } while (magnitude != 0);

        return (negative ? std::basic_string<C>(1, static_cast<C>('-')) + digits : digits);
    }

    struct StringTypeName
    {
        template<typename T>
        static std::string GetName(int)
        {
            return (std::is_same_v<T, areg::String> ? "String" : "WideString");
        }
    };
}

//!< The same literal, spelled for the character type of the string under test.
#define TXT(s)  pick<typename StrChar<TypeParam>::Type>(s, L##s)

template<typename S>
class StringCommonTest : public ::testing::Test
{
};

using StringTypes = ::testing::Types<areg::String, areg::WideString>;
TYPED_TEST_SUITE(StringCommonTest, StringTypes, StringTypeName);

/**
 * \brief   Every operation that returns a string returns the derived type.
 **/
TYPED_TEST(StringCommonTest, OperationsReturnDerivedType)
{
    using S = TypeParam;
    S str(TXT("  a,b  "));
    static_assert(std::is_same_v<decltype(str.trim_all()), S&>);
    static_assert(std::is_same_v<decltype(str.make_upper()), S&>);
    static_assert(std::is_same_v<decltype(str.assign(TXT("x"))), S&>);
    static_assert(std::is_same_v<decltype(str += TXT("x")), S&>);
    static_assert(std::is_same_v<decltype(str.left_side(1)), S>);
    static_assert(std::is_same_v<decltype(str.split(TXT(",")[0])), std::vector<S>>);
    static_assert(std::is_same_v<decltype(str + str), S>);
    static_assert(std::is_same_v<decltype(S::make_string(1)), S>);

    S chained(str.trim_all().make_upper());
    EXPECT_EQ(chained.data(), expected<typename StrChar<S>::Type>("A,B"));

    const std::vector<S> parts{ chained.split(TXT(",")[0]) };
    ASSERT_EQ(parts.size(), 2u);
    EXPECT_EQ(parts[0].data(), expected<typename StrChar<S>::Type>("A"));
    EXPECT_EQ(parts[1].data(), expected<typename StrChar<S>::Type>("B"));
}

/**
 * \brief   make_string writes 32-bit integers as the printf formats did, in every radix.
 **/
TYPED_TEST(StringCommonTest, MakeString32MatchesPrintf)
{
    using S = TypeParam;
    using C = typename StrChar<S>::Type;
    const int32_t signedValues[]{ 0, 1, -1, 42, -42, 1234567, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::min() };
    for (int32_t value : signedValues)
    {
        const bool negative{ value < 0 };
        const uint32_t magnitude{ negative ? 0u - static_cast<uint32_t>(value) : static_cast<uint32_t>(value) };
        const std::basic_string<C> sign(negative ? 1u : 0u, static_cast<C>('-'));
        EXPECT_EQ(S::make_string(value).data(), printed<C>("%d", value));
        EXPECT_EQ(S::make_string(value, areg::Radix::Automatic).data(), printed<C>("%d", value));
        EXPECT_EQ(S::make_string(value, areg::Radix::Octal).data(), sign + printed<C>("%.11o", magnitude));
        EXPECT_EQ(S::make_string(value, areg::Radix::Hexadecimal).data(), sign + printed<C>("0x%.8X", magnitude));
        EXPECT_EQ(S::make_string(value, areg::Radix::Binary).data(), binary<C>(magnitude, negative));
    }

    const uint32_t unsignedValues[]{ 0u, 1u, 42u, 0x80000000u, std::numeric_limits<uint32_t>::max() };
    for (uint32_t value : unsignedValues)
    {
        EXPECT_EQ(S::make_string(value).data(), printed<C>("%u", value));
        EXPECT_EQ(S::make_string(value, areg::Radix::Octal).data(), printed<C>("%.11o", value));
        EXPECT_EQ(S::make_string(value, areg::Radix::Hexadecimal).data(), printed<C>("0x%.8X", value));
        EXPECT_EQ(S::make_string(value, areg::Radix::Binary).data(), binary<C>(value, false));
    }
}

/**
 * \brief   make_string writes 64-bit integers as the printf formats did, in every radix.
 **/
TYPED_TEST(StringCommonTest, MakeString64MatchesPrintf)
{
    using S = TypeParam;
    using C = typename StrChar<S>::Type;
    const int64_t signedValues[]{ 0, 1, -1, 1234567890123LL, -1234567890123LL, std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min() };
    for (int64_t value : signedValues)
    {
        const bool negative{ value < 0 };
        const uint64_t magnitude{ negative ? 0u - static_cast<uint64_t>(value) : static_cast<uint64_t>(value) };
        const std::basic_string<C> sign(negative ? 1u : 0u, static_cast<C>('-'));
        EXPECT_EQ(S::make_string(value).data(), printed<C>("%lld", static_cast<long long>(value)));
        EXPECT_EQ(S::make_string(value, areg::Radix::Octal).data(), sign + printed<C>("%.22llo", static_cast<unsigned long long>(magnitude)));
        EXPECT_EQ(S::make_string(value, areg::Radix::Hexadecimal).data(), sign + printed<C>("0x%.16llX", static_cast<unsigned long long>(magnitude)));
        EXPECT_EQ(S::make_string(value, areg::Radix::Binary).data(), binary<C>(magnitude, negative));
    }

    const uint64_t unsignedValues[]{ 0u, 1u, 0x8000000000000000ull, std::numeric_limits<uint64_t>::max() };
    for (uint64_t value : unsignedValues)
    {
        EXPECT_EQ(S::make_string(value).data(), printed<C>("%llu", static_cast<unsigned long long>(value)));
        EXPECT_EQ(S::make_string(value, areg::Radix::Octal).data(), printed<C>("%.22llo", static_cast<unsigned long long>(value)));
        EXPECT_EQ(S::make_string(value, areg::Radix::Hexadecimal).data(), printed<C>("0x%.16llX", static_cast<unsigned long long>(value)));
        EXPECT_EQ(S::make_string(value, areg::Radix::Binary).data(), binary<C>(value, false));
    }
}

/**
 * \brief   make_string writes real numbers and booleans; the from_ methods assign the same text.
 **/
TYPED_TEST(StringCommonTest, MakeStringRealAndBool)
{
    using S = TypeParam;
    using C = typename StrChar<S>::Type;
    EXPECT_EQ(S::make_string(3.25f).data(), printed<C>("%f", 3.25));
    EXPECT_EQ(S::make_string(-1.5e300).data(), printed<C>("%g", -1.5e300));
    EXPECT_EQ(S::make_string(std::numeric_limits<float>::max()).data(), printed<C>("%f", static_cast<double>(std::numeric_limits<float>::max())));
    EXPECT_EQ(S::make_string(true).data(), expected<C>("true"));
    EXPECT_EQ(S::make_string(false).data(), expected<C>("false"));

    S str;
    EXPECT_EQ(str.from_int32(-7).data(), expected<C>("-7"));
    EXPECT_EQ(str.from_uint64(255u, areg::Radix::Hexadecimal).data(), expected<C>("0x00000000000000FF"));
    EXPECT_EQ(str.from_bool(true).data(), expected<C>("true"));
    EXPECT_EQ(str.from_double(0.5).data(), expected<C>("0.5"));
}

/**
 * \brief   The make_ methods parse numbers and booleans and report where parsing stopped.
 **/
TYPED_TEST(StringCommonTest, ParseNumbersAndBool)
{
    using S = TypeParam;
    using C = typename StrChar<S>::Type;
    const C* end{ nullptr };
    EXPECT_EQ(S::make_int32(TXT("-123xyz"), areg::Radix::Decimal, &end), -123);
    EXPECT_EQ(std::basic_string<C>(end), expected<C>("xyz"));
    EXPECT_EQ(S::make_uint32(TXT("ff"), areg::Radix::Hexadecimal), 255u);
    EXPECT_EQ(S::make_int64(TXT("-9000000000")), -9000000000LL);
    EXPECT_EQ(S::make_uint64(TXT("18446744073709551615")), std::numeric_limits<uint64_t>::max());
    EXPECT_FLOAT_EQ(S::make_float(TXT("2.5")), 2.5f);
    EXPECT_DOUBLE_EQ(S::make_double(TXT("-0.125")), -0.125);
    EXPECT_EQ(S::make_int32(nullptr, areg::Radix::Decimal, &end), 0);
    EXPECT_TRUE(end == nullptr);

    EXPECT_TRUE(S::make_bool(TXT("TRUE rest"), &end));
    EXPECT_EQ(std::basic_string<C>(end), expected<C>(" rest"));
    EXPECT_FALSE(S::make_bool(TXT("false")));
    EXPECT_FALSE(S::make_bool(TXT("other")));

    EXPECT_EQ(S(TXT("77")).to_int32(), 77);
    EXPECT_EQ(S(TXT("0x10")).to_uint32(areg::Radix::Automatic), 16u);
    EXPECT_TRUE(S(TXT("yes")).to_bool());
    EXPECT_FALSE(S(TXT("FALSE")).to_bool());
    EXPECT_FALSE(S().to_bool());
}

/**
 * \brief   format writes short and long text in full.
 **/
TYPED_TEST(StringCommonTest, FormatShortAndLongText)
{
    using S = TypeParam;
    using C = typename StrChar<S>::Type;
    S str;
    EXPECT_EQ(str.format(TXT("%d-%d"), 1, 2).data(), expected<C>("1-2"));

    for (int width : { 255, 256, 1023, 1024, 5000 })
    {
        str.format(TXT("%0*d"), width, 7);
        ASSERT_EQ(str.length(), static_cast<areg::CharCount>(width));
        EXPECT_EQ(str.data().front(), static_cast<C>('0'));
        EXPECT_EQ(str.data().back(), static_cast<C>('7'));
    }

    str.format(nullptr);
    EXPECT_TRUE(str.is_empty());
}

/**
 * \brief   format_string never writes past the buffer, terminates it, and reports a cut text.
 **/
TYPED_TEST(StringCommonTest, FormatStringTruncates)
{
    using S = TypeParam;
    using C = typename StrChar<S>::Type;
    C buffer[8];
    EXPECT_EQ(S::format_string(buffer, 8, TXT("%d"), 1234567), 7);
    EXPECT_EQ(std::basic_string<C>(buffer), expected<C>("1234567"));

    std::fill(std::begin(buffer), std::end(buffer), static_cast<C>('#'));
    EXPECT_EQ(S::format_string(buffer, 8, TXT("%d"), 123456789), 7);
    EXPECT_EQ(std::basic_string<C>(buffer), expected<C>("1234567"));

    int32_t required{ 0 };
    EXPECT_EQ(S::format_string(nullptr, 8, TXT("%d"), 1), -1);
    EXPECT_EQ(S::format_string(buffer, 0, TXT("%d"), 1), 0);

    struct Call
    {
        static int32_t list(C* dst, int32_t count, int32_t& needed, const C* format, ...)
        {
            va_list args;
            va_start(args, format);
            const int32_t result{ S::format_string_list(dst, count, format, args, needed) };
            va_end(args);
            return result;
        }
    };

    EXPECT_EQ(Call::list(buffer, 8, required, TXT("%d"), 123), 3);
    EXPECT_EQ(required, 3);
    EXPECT_EQ(Call::list(buffer, 8, required, TXT("%d"), 123456789), 7);
    EXPECT_GE(required, 8);
    EXPECT_EQ(std::basic_string<C>(buffer), expected<C>("1234567"));
}

/**
 * \brief   Conversion between widths maps a narrow character as an unsigned value, and
 *          equality gives the same answer in both directions.
 **/
TEST(StringCommonTest, ConversionBetweenWidths)
{
    const areg::String narrow("A\xE9z");
    const areg::WideString wide(L"A\x00E9z");

    EXPECT_EQ(areg::WideString(narrow).data(), std::wstring(L"A\x00E9z"));
    EXPECT_EQ(areg::String(wide).data(), std::string("A\xE9z"));
    EXPECT_TRUE(narrow == wide);
    EXPECT_TRUE(wide == narrow);
    EXPECT_TRUE(narrow == L"A\x00E9z");
    EXPECT_TRUE(wide == "A\xE9z");
    EXPECT_TRUE(narrow == std::wstring(L"A\x00E9z"));
    EXPECT_TRUE(wide == std::string("A\xE9z"));

    const areg::WideString notLatin(L"\x0141");
    EXPECT_TRUE(areg::String("A") != notLatin);
    EXPECT_TRUE(notLatin != areg::String("A"));
    EXPECT_TRUE(notLatin != "A");
    EXPECT_FALSE(areg::String("A") == L"\x0141");
    EXPECT_FALSE(areg::String("AB") == areg::WideString(L"AC"));
    EXPECT_FALSE(areg::WideString(L"AB") == "AC");
    EXPECT_TRUE(areg::String("AB") != L"AC");

    areg::String str;
    str = wide;
    EXPECT_EQ(str.data(), std::string("A\xE9z"));
    str += L"!";
    str += L'?';
    str += std::wstring(L"+");
    str += wide;
    EXPECT_EQ(str.data(), std::string("A\xE9z!?+A\xE9z"));
    str.assign(L"abcdef", 3);
    EXPECT_EQ(str.data(), std::string("abc"));
    str.append(L"xyz", 2);
    EXPECT_EQ(str.data(), std::string("abcxy"));

    areg::WideString wstr("narrow");
    wstr = 'n';
    EXPECT_EQ(wstr.data(), std::wstring(L"n"));
    EXPECT_EQ((areg::String("a") + L"b").data(), std::string("ab"));
    EXPECT_EQ((L"a" + areg::String("b")).data(), std::string("ab"));
    EXPECT_EQ((areg::WideString(L"a") + "b").data(), std::wstring(L"ab"));
    EXPECT_TRUE(areg::String(static_cast<const wchar_t*>(nullptr)).is_empty());
    EXPECT_TRUE(areg::String("") == static_cast<const wchar_t*>(nullptr));
}

/**
 * \brief   Every concatenation operator gives the joined text.
 **/
TYPED_TEST(StringCommonTest, Concatenation)
{
    using S = TypeParam;
    using C = typename StrChar<S>::Type;
    const S lhs(TXT("ab"));
    const S rhs(TXT("cd"));
    const std::basic_string<C> stdStr(TXT("cd"));
    const std::basic_string_view<C> view(TXT("cd"));
    const std::basic_string<C> joined{ expected<C>("abcd") };

    EXPECT_EQ((lhs + rhs).data(), joined);
    EXPECT_EQ((lhs + stdStr).data(), joined);
    EXPECT_EQ((lhs + view).data(), joined);
    EXPECT_EQ((lhs + TXT("cd")).data(), joined);
    EXPECT_EQ((S(TXT("abc")) + static_cast<C>('d')).data(), joined);
    EXPECT_EQ((std::basic_string<C>(TXT("ab")) + rhs).data(), joined);
    EXPECT_EQ((std::basic_string_view<C>(TXT("ab")) + rhs).data(), joined);
    EXPECT_EQ((TXT("ab") + rhs).data(), joined);
    EXPECT_EQ((static_cast<C>('a') + S(TXT("bcd"))).data(), joined);
    EXPECT_EQ((lhs + static_cast<const C*>(nullptr)).data(), expected<C>("ab"));
}

/**
 * \brief   The remaining moved operations: substr, the hash value and a counted null source.
 **/
TYPED_TEST(StringCommonTest, SubstrHashAndNullSource)
{
    using S = TypeParam;
    using C = typename StrChar<S>::Type;
    const S test(TXT("0123 456 789 0123"));
    const C* next{ test.as_string() };
    EXPECT_EQ(S::substr(next, TXT(" "), &next).data(), expected<C>("0123"));
    EXPECT_EQ(S::substr(next, TXT(" "), &next).data(), expected<C>("456"));
    EXPECT_EQ(S::substr(next, TXT(" "), &next).data(), expected<C>("789"));
    EXPECT_EQ(S::substr(next, TXT(" "), &next).data(), expected<C>("0123"));
    EXPECT_TRUE(next == nullptr);

    EXPECT_EQ(static_cast<uint32_t>(test), areg::crc32_calculate(test.as_string()));
    EXPECT_TRUE(S(static_cast<const C*>(nullptr), 5).is_empty());
}
