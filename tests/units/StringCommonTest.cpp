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
 * \author      Artak Avetyan, Apoorv Darshan
 * \brief       Areg Platform, Areg framework unit test file.
 *              The unit test of the StringBase API, run for String and WideString.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <iterator>
#include <limits>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace
{
    template<typename S> struct StrChar;
    template<> struct StrChar<areg::String>     { using Type = char;    using Other = areg::WideString; };
    template<> struct StrChar<areg::WideString> { using Type = wchar_t; using Other = areg::String;     };

    //!< The character type of the string type.
    template<typename S> using CharOf  = typename StrChar<S>::Type;
    //!< The string type of the other character width.
    template<typename S> using OtherOf = typename StrChar<S>::Other;

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

    //!< Calls format_list of the string with the variable arguments.
    template<typename S>
    S& format_with_list(S& str, const CharOf<S>* format, ...)
    {
        va_list args;
        va_start(args, format);
        str.format_list(format, args);
        va_end(args);
        return str;
    }

    //!< Calls the 4-argument format_string_list with the variable arguments.
    template<typename S>
    int32_t format_string_with_list(CharOf<S>* dst, int32_t count, const CharOf<S>* format, ...)
    {
        va_list args;
        va_start(args, format);
        const int32_t result{ S::format_string_list(dst, count, format, args) };
        va_end(args);
        return result;
    }

    //!< Calls the 5-argument format_string_list with the variable arguments.
    template<typename S>
    int32_t format_string_with_required(CharOf<S>* dst, int32_t count, int32_t& required, const CharOf<S>* format, ...)
    {
        va_list args;
        va_start(args, format);
        const int32_t result{ S::format_string_list(dst, count, format, args, required) };
        va_end(args);
        return result;
    }

    //!< Opens the protected helpers of the string type.
    template<typename S>
    class StringProbe : public S
    {
    public:
        using S::compare;
        using S::compare_exact;
        using S::compare_ignore_case;
        using S::find_first_phrase;
        using S::find_phrase;
        using S::find_first_word;
        using S::is_name_char;
    };

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
#define TXT(s)  pick<CharOf<TypeParam>>(s, L##s)
//!< The same literal, spelled for the character type of the other width.
#define OTXT(s) pick<CharOf<OtherOf<TypeParam>>>(s, L##s)
//!< The ASCII character as the character type of the string under test.
#define CH(c)   static_cast<CharOf<TypeParam>>(c)

template<typename S>
class StringCommonTest : public ::testing::Test
{
};

template<typename S> using StringCommonConstruct = StringCommonTest<S>;
template<typename S> using StringCommonAssign    = StringCommonTest<S>;
template<typename S> using StringCommonCompare   = StringCommonTest<S>;
template<typename S> using StringCommonConcat    = StringCommonTest<S>;
template<typename S> using StringCommonSearch    = StringCommonTest<S>;
template<typename S> using StringCommonSubstring = StringCommonTest<S>;
template<typename S> using StringCommonModify    = StringCommonTest<S>;
template<typename S> using StringCommonTrim      = StringCommonTest<S>;
template<typename S> using StringCommonNumbers   = StringCommonTest<S>;
template<typename S> using StringCommonFormat    = StringCommonTest<S>;
template<typename S> using StringCommonValidate  = StringCommonTest<S>;
template<typename S> using StringCommonStream    = StringCommonTest<S>;

using StringTypes = ::testing::Types<areg::String, areg::WideString>;
TYPED_TEST_SUITE(StringCommonTest     , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonConstruct, StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonAssign   , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonCompare  , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonConcat   , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonSearch   , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonSubstring, StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonModify   , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonTrim     , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonNumbers  , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonFormat   , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonValidate , StringTypes, StringTypeName);
TYPED_TEST_SUITE(StringCommonStream   , StringTypes, StringTypeName);

//////////////////////////////////////////////////////////////////////////
// StringCommonTest: the derived type and the layout
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Every operation that returns a string returns the derived type.
 **/
TYPED_TEST(StringCommonTest, OperationsReturnDerivedType)
{
    using S = TypeParam;
    using C = CharOf<S>;
    S str(TXT("  a,b  "));
    static_assert(std::is_same_v<decltype(str.trim_all()), S&>);
    static_assert(std::is_same_v<decltype(str.make_upper()), S&>);
    static_assert(std::is_same_v<decltype(str.assign(TXT("x"))), S&>);
    static_assert(std::is_same_v<decltype(str.append(TXT("x"))), S&>);
    static_assert(std::is_same_v<decltype(str.replace(CH('a'), CH('b'))), S&>);
    static_assert(std::is_same_v<decltype(str.remove(CH('a'))), S&>);
    static_assert(std::is_same_v<decltype(str.insert_at(CH('a'), 0)), S&>);
    static_assert(std::is_same_v<decltype(str.substring(0)), S&>);
    static_assert(std::is_same_v<decltype(str.from_int32(1)), S&>);
    static_assert(std::is_same_v<decltype(str.format(TXT("x"))), S&>);
    static_assert(std::is_same_v<decltype(str = TXT("x")), S&>);
    static_assert(std::is_same_v<decltype(str += TXT("x")), S&>);
    static_assert(std::is_same_v<decltype(str.left_side(1)), S>);
    static_assert(std::is_same_v<decltype(str.right_side(1)), S>);
    static_assert(std::is_same_v<decltype(str.split(CH(','))), std::vector<S>>);
    static_assert(std::is_same_v<decltype(str + str), S>);
    static_assert(std::is_same_v<decltype(S::make_string(1)), S>);
    static_assert(std::is_same_v<decltype(S::substr(TXT("a"), TXT("b"))), S>);
    static_assert(sizeof(S) == sizeof(std::basic_string<C>));
    static_assert(std::is_nothrow_move_constructible_v<S>);
    static_assert(std::is_nothrow_move_assignable_v<S>);
    static_assert(std::is_nothrow_default_constructible_v<S>);

    S chained(str.trim_all().make_upper());
    EXPECT_EQ(chained.data(), expected<C>("A,B"));

    const std::vector<S> parts{ chained.split(CH(',')) };
    ASSERT_EQ(parts.size(), 2u);
    EXPECT_EQ(parts[0].data(), expected<C>("A"));
    EXPECT_EQ(parts[1].data(), expected<C>("B"));
}

//////////////////////////////////////////////////////////////////////////
// StringCommonConstruct
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The default constructor creates an empty string with a valid buffer.
 **/
TYPED_TEST(StringCommonConstruct, Default)
{
    TypeParam str;
    EXPECT_TRUE(str.is_empty());
    EXPECT_EQ(str.length(), 0);
    EXPECT_NE(str.as_string(), nullptr);
    EXPECT_STREQ(str.as_string(), TXT(""));
}

/**
 * \brief   The constructors from a null-terminated string, with and without a count.
 **/
TYPED_TEST(StringCommonConstruct, FromPointer)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S str(TXT("Hello"));
    EXPECT_FALSE(str.is_empty());
    EXPECT_EQ(str.length(), 5);
    EXPECT_STREQ(str.as_string(), TXT("Hello"));

    EXPECT_STREQ(S(TXT("Hello World"), 5).as_string(), TXT("Hello"));
    EXPECT_STREQ(S(TXT("Hello"), 0).as_string(), TXT(""));
    EXPECT_TRUE(S(static_cast<const C*>(nullptr)).is_empty());
    EXPECT_TRUE(S(static_cast<const C*>(nullptr), 5).is_empty());
    EXPECT_TRUE(S(TXT("")).is_empty());
}

/**
 * \brief   The constructors from a standard string, an rvalue standard string and a view.
 **/
TYPED_TEST(StringCommonConstruct, FromStandardTypes)
{
    using S = TypeParam;
    using Std = std::basic_string<CharOf<S>>;
    const Std src(TXT("Hello World"));
    EXPECT_STREQ(S(src).as_string(), TXT("Hello World"));

    Std movable(TXT("Moved std string that is long enough to allocate"));
    const S moved(std::move(movable));
    EXPECT_STREQ(moved.as_string(), TXT("Moved std string that is long enough to allocate"));

    const std::basic_string_view<CharOf<S>> view(TXT("Test View"));
    EXPECT_STREQ(S(view).as_string(), TXT("Test View"));
    EXPECT_STREQ(S(view.substr(5, 4)).as_string(), TXT("View"));
    EXPECT_STREQ(S(view.substr(0, 4)).as_string(), TXT("Test"));
}

/**
 * \brief   The constructor from one character, and the reserving constructor.
 **/
TYPED_TEST(StringCommonConstruct, FromCharAndCapacity)
{
    using S = TypeParam;
    const S str(CH('A'));
    EXPECT_EQ(str.length(), 1);
    EXPECT_STREQ(str.as_string(), TXT("A"));
    static_assert(!std::is_convertible_v<CharOf<S>, S>, "The character constructor of the derived type is explicit.");

    const S reserved(static_cast<uint32_t>(100));
    EXPECT_TRUE(reserved.is_empty());
    EXPECT_GE(reserved.capacity(), 100);
    static_assert(!std::is_convertible_v<uint32_t, S>, "The reserving constructor is explicit.");
}

/**
 * \brief   The copy and the move constructors.
 **/
TYPED_TEST(StringCommonConstruct, CopyAndMove)
{
    using S = TypeParam;
    S original(TXT("Copy Me, a string long enough to be on the heap"));
    const S copy(original);
    EXPECT_EQ(copy, original);

    S modified(original);
    modified += TXT("!");
    EXPECT_STREQ(modified.as_string(), TXT("Copy Me, a string long enough to be on the heap!"));
    EXPECT_STREQ(original.as_string(), TXT("Copy Me, a string long enough to be on the heap"));
    EXPECT_STREQ(copy.as_string(), TXT("Copy Me, a string long enough to be on the heap"));

    const S moved(std::move(original));
    EXPECT_STREQ(moved.as_string(), TXT("Copy Me, a string long enough to be on the heap"));
    EXPECT_EQ(moved, copy);
}

/**
 * \brief   The constructors from text of the other character width.
 **/
TYPED_TEST(StringCommonConstruct, FromOtherWidth)
{
    using S = TypeParam;
    using O = OtherOf<S>;
    using OC = CharOf<O>;
    EXPECT_STREQ(S(OTXT("abc")).as_string(), TXT("abc"));
    EXPECT_STREQ(S(OTXT("abcdef"), 3).as_string(), TXT("abc"));
    EXPECT_STREQ(S(std::basic_string<OC>(OTXT("xyz"))).as_string(), TXT("xyz"));
    EXPECT_STREQ(S(O(OTXT("other"))).as_string(), TXT("other"));
    EXPECT_TRUE(S(static_cast<const OC*>(nullptr)).is_empty());
    EXPECT_TRUE(S(static_cast<const OC*>(nullptr), 4).is_empty());
    EXPECT_TRUE(S(O()).is_empty());
}

/**
 * \brief   The shared empty string and the character constants.
 **/
TYPED_TEST(StringCommonConstruct, EmptyStringAndConstants)
{
    using S = TypeParam;
    EXPECT_TRUE(S::empty_string().is_empty());
    EXPECT_EQ(&S::empty_string(), &S::empty_string());
    EXPECT_STREQ(S::EmptyString, TXT(""));
    EXPECT_EQ(S::EmptyChar, CH('\0'));
    EXPECT_EQ(S::NewLine, CH('\n'));
    EXPECT_EQ(S::NewLineDos[0], CH('\r'));
    EXPECT_EQ(S::NewLineDos[1], CH('\n'));
}

//////////////////////////////////////////////////////////////////////////
// StringCommonAssign
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The assignment operators of the same character width.
 **/
TYPED_TEST(StringCommonAssign, Operators)
{
    using S = TypeParam;
    using C = CharOf<S>;
    using Std = std::basic_string<C>;
    S str;
    str = TXT("Assigned");
    EXPECT_STREQ(str.as_string(), TXT("Assigned"));

    const Std src(TXT("From std string"));
    str = src;
    EXPECT_STREQ(str.as_string(), TXT("From std string"));

    const std::basic_string_view<C> view(TXT("From string_view"));
    str = view.substr(5);
    EXPECT_STREQ(str.as_string(), TXT("string_view"));

    str = CH('Z');
    EXPECT_EQ(str.length(), 1);
    EXPECT_STREQ(str.as_string(), TXT("Z"));

    Std movable(TXT("Move std string"));
    str = std::move(movable);
    EXPECT_STREQ(str.as_string(), TXT("Move std string"));

    str = static_cast<const C*>(nullptr);
    EXPECT_TRUE(str.is_empty());
}

/**
 * \brief   The copy, move and self assignments.
 **/
TYPED_TEST(StringCommonAssign, CopyMoveAndSelf)
{
    using S = TypeParam;
    const S src(TXT("Copy Source"));
    S dst;
    dst = src;
    EXPECT_STREQ(dst.as_string(), TXT("Copy Source"));
    EXPECT_STREQ(src.as_string(), TXT("Copy Source"));

    S movable(TXT("Move Source"));
    dst = std::move(movable);
    EXPECT_STREQ(dst.as_string(), TXT("Move Source"));

    const S& ref = dst;
    dst = ref;
    EXPECT_STREQ(dst.as_string(), TXT("Move Source"));
}

/**
 * \brief   The assign methods of every source type.
 **/
TYPED_TEST(StringCommonAssign, AssignMethods)
{
    using S = TypeParam;
    using C = CharOf<S>;
    using Std = std::basic_string<C>;
    S str(TXT("old"));
    EXPECT_STREQ(str.assign(TXT("Assigned")).as_string(), TXT("Assigned"));
    EXPECT_STREQ(str.assign(TXT("Hello World"), 5).as_string(), TXT("Hello"));
    EXPECT_STREQ(str.assign(static_cast<const C*>(nullptr)).as_string(), TXT(""));

    const Std src(TXT("Hello World"));
    EXPECT_STREQ(str.assign(src).as_string(), TXT("Hello World"));
    EXPECT_STREQ(str.assign(src, 6).as_string(), TXT("World"));
    EXPECT_STREQ(str.assign(src, 6, 3).as_string(), TXT("Wor"));
    EXPECT_STREQ(str.assign(src, 6, 100).as_string(), TXT("World"));
    EXPECT_STREQ(str.assign(src, 11).as_string(), TXT(""));

    const std::basic_string_view<C> view(TXT("Hello World"));
    EXPECT_STREQ(str.assign(view).as_string(), TXT("Hello World"));
    EXPECT_STREQ(str.assign(view, 6).as_string(), TXT("World"));
    EXPECT_STREQ(str.assign(view, 0, 5).as_string(), TXT("Hello"));
    EXPECT_STREQ(str.assign(view.substr(0, 4)).as_string(), TXT("Hell"));

    const S other(TXT("Hello World"));
    EXPECT_STREQ(str.assign(other).as_string(), TXT("Hello World"));
    EXPECT_STREQ(str.assign(other, 6, 5).as_string(), TXT("World"));

    EXPECT_STREQ(str.assign(CH('X')).as_string(), TXT("X"));
}

/**
 * \brief   The assign and append methods do not read outside the source when the position or
 *          the count is beyond the source.
 **/
TYPED_TEST(StringCommonAssign, PositionBeyondSource)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const std::basic_string<C> src(TXT("abc"));
    const std::basic_string_view<C> view(TXT("abcdef"), 3);
    S str(TXT("keep"));

    EXPECT_TRUE(str.assign(src, 4).is_empty());
    EXPECT_STREQ(str.assign(view, 1, 100).as_string(), TXT("bc"));
    EXPECT_TRUE(str.assign(view, 4).is_empty());
    EXPECT_TRUE(str.assign(S(TXT("abc")), 10, 1).is_empty());

    str = TXT("x");
    EXPECT_STREQ(str.append(src, 4).as_string(), TXT("x"));
    EXPECT_STREQ(str.append(view, 2, 100).as_string(), TXT("xc"));
    EXPECT_STREQ(str.append(view, 5).as_string(), TXT("xc"));
    EXPECT_STREQ(str.append(S(TXT("abc")), 7).as_string(), TXT("xc"));
}

/**
 * \brief   assign, append and insert_at take a part of the string itself as the source.
 **/
TYPED_TEST(StringCommonAssign, SelfSource)
{
    using S = TypeParam;
    S str(TXT("abcdefghijklmnopqrstuvwxyz0123456789"));
    str.append(str, 26);
    EXPECT_STREQ(str.as_string(), TXT("abcdefghijklmnopqrstuvwxyz01234567890123456789"));
    str.insert_at(str, 0, 36);
    EXPECT_STREQ(str.as_string(), TXT("0123456789abcdefghijklmnopqrstuvwxyz01234567890123456789"));
    str.insert_at(str.data(), str.length(), 10, 5);
    EXPECT_STREQ(str.as_string(), TXT("0123456789abcdefghijklmnopqrstuvwxyz01234567890123456789abcde"));
    str.assign(str, 10, 26);
    EXPECT_STREQ(str.as_string(), TXT("abcdefghijklmnopqrstuvwxyz"));
    str.assign(std::basic_string_view<CharOf<S>>(str.data()).substr(23));
    EXPECT_STREQ(str.as_string(), TXT("xyz"));
}

/**
 * \brief   Assignment from text of the other character width.
 **/
TYPED_TEST(StringCommonAssign, FromOtherWidth)
{
    using S = TypeParam;
    using O = OtherOf<S>;
    using OC = CharOf<O>;
    S str;
    str = OTXT("other");
    EXPECT_STREQ(str.as_string(), TXT("other"));
    str = std::basic_string<OC>(OTXT("std"));
    EXPECT_STREQ(str.as_string(), TXT("std"));
    str = static_cast<OC>('q');
    EXPECT_STREQ(str.as_string(), TXT("q"));
    str = O(OTXT("object"));
    EXPECT_STREQ(str.as_string(), TXT("object"));
    str.assign(OTXT("abcdef"), 2);
    EXPECT_STREQ(str.as_string(), TXT("ab"));
    str = static_cast<const OC*>(nullptr);
    EXPECT_TRUE(str.is_empty());
}

//////////////////////////////////////////////////////////////////////////
// StringCommonCompare
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Equality and inequality with every type of the same width.
 **/
TYPED_TEST(StringCommonCompare, EqualityOperators)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S str(TXT("Hello"));
    EXPECT_TRUE(str == S(TXT("Hello")));
    EXPECT_FALSE(str == S(TXT("World")));
    EXPECT_TRUE(str == TXT("Hello"));
    EXPECT_FALSE(str == TXT("Hell"));
    EXPECT_TRUE(str == std::basic_string<C>(TXT("Hello")));
    EXPECT_TRUE(str == std::basic_string_view<C>(TXT("Hello!"), 5));
    EXPECT_TRUE(S(TXT("A")) == CH('A'));
    EXPECT_FALSE(S(TXT("A")) == CH('B'));
    EXPECT_FALSE(S(TXT("AA")) == CH('A'));
    EXPECT_FALSE(S() == CH('A'));

    EXPECT_TRUE(str != S(TXT("World")));
    EXPECT_FALSE(str != S(TXT("Hello")));
    EXPECT_TRUE(str != TXT("World"));
    EXPECT_FALSE(str != TXT("Hello"));
    EXPECT_TRUE(str != std::basic_string<C>(TXT("World")));
    EXPECT_TRUE(str != std::basic_string_view<C>(TXT("Hello"), 4));
    EXPECT_TRUE(S(TXT("A")) != CH('B'));
    EXPECT_FALSE(S(TXT("A")) != CH('A'));
    EXPECT_TRUE(S() != CH('A'));
}

/**
 * \brief   A null pointer compares as the empty string.
 **/
TYPED_TEST(StringCommonCompare, NullPointerIsEmpty)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const C* null{ nullptr };
    EXPECT_TRUE(S() == null);
    EXPECT_FALSE(S(TXT("a")) == null);
    EXPECT_TRUE(S(TXT("a")) != null);
    EXPECT_FALSE(S() != null);
    EXPECT_TRUE(S(TXT("a")) > null);
    EXPECT_FALSE(S() < null);
    EXPECT_TRUE(S() <= null);
    EXPECT_TRUE(S() >= null);
}

/**
 * \brief   The ordering operators with every type of the same width.
 **/
TYPED_TEST(StringCommonCompare, OrderingOperators)
{
    using S = TypeParam;
    using C = CharOf<S>;
    using Std = std::basic_string<C>;
    using View = std::basic_string_view<C>;
    const S a(TXT("Apple"));
    const S b(TXT("Banana"));
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b >= a);
    EXPECT_TRUE(a <= a);
    EXPECT_TRUE(a >= a);
    EXPECT_FALSE(a > a);
    EXPECT_FALSE(a < a);

    EXPECT_TRUE(a < TXT("Banana"));
    EXPECT_TRUE(a > TXT("Aardvark"));
    EXPECT_TRUE(a <= TXT("Apple"));
    EXPECT_TRUE(a >= TXT("Apple"));

    EXPECT_TRUE(a < Std(TXT("Banana")));
    EXPECT_TRUE(a > Std(TXT("App")));
    EXPECT_TRUE(a <= Std(TXT("Apple")));
    EXPECT_TRUE(a >= Std(TXT("Apple")));

    EXPECT_TRUE(a < View(TXT("Banana")));
    EXPECT_TRUE(a > View(TXT("Apple pie"), 3));
    EXPECT_TRUE(a <= View(TXT("Apple")));
    EXPECT_TRUE(a >= View(TXT("Apple")));
}

/**
 * \brief   The compare method, case-sensitive and not, of the whole string.
 **/
TYPED_TEST(StringCommonCompare, CompareWholeString)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S str(TXT("Hello"));
    EXPECT_EQ(str.compare(TXT("Hello"), areg::START_POS, areg::COUNT_ALL, true), areg::Ordering::Equal);
    EXPECT_NE(str.compare(TXT("hello"), areg::START_POS, areg::COUNT_ALL, true), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(TXT("HELLO"), areg::START_POS, areg::COUNT_ALL, false), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(TXT("hello"), areg::START_POS, areg::COUNT_ALL, false), areg::Ordering::Equal);

    const S abc(TXT("abc"));
    EXPECT_EQ(abc.compare(S(TXT("abc"))), areg::Ordering::Equal);
    EXPECT_EQ(abc.compare(S(TXT("abd"))), areg::Ordering::Smaller);
    EXPECT_EQ(abc.compare(S(TXT("ABC")), false), areg::Ordering::Equal);
    EXPECT_NE(abc.compare(S(TXT("ABC")), true), areg::Ordering::Equal);
    EXPECT_EQ(abc.compare(std::basic_string<C>(TXT("abc"))), areg::Ordering::Equal);
    EXPECT_EQ(abc.compare(std::basic_string<C>(TXT("abb"))), areg::Ordering::Bigger);
    EXPECT_EQ(abc.compare(std::basic_string_view<C>(TXT("abc"))), areg::Ordering::Equal);
    EXPECT_EQ(abc.compare(TXT("abc")), areg::Ordering::Equal);
    EXPECT_EQ(abc.compare(TXT("ab")), areg::Ordering::Bigger);
    EXPECT_EQ(abc.compare(TXT("abcd")), areg::Ordering::Smaller);
    EXPECT_EQ(S().compare(TXT("a")), areg::Ordering::Smaller);
}

/**
 * \brief   The compare method starting at a position, with a count.
 **/
TYPED_TEST(StringCommonCompare, CompareAtPosition)
{
    using S = TypeParam;
    using C = CharOf<S>;
    using View = std::basic_string_view<C>;
    const S str(TXT("Hello World"));
    EXPECT_EQ(str.compare(TXT("World"), areg::CharPos{ 6 }, areg::CharCount{ 5 }, true), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(S(TXT("World")), 6), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(std::basic_string<C>(TXT("World")), 6), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(S(TXT("WORLD")), 6, areg::COUNT_ALL, false), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(TXT("World"), 100), areg::Ordering::Smaller);

    EXPECT_EQ(str.compare(TXT("World"), 6u), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(S(TXT("World")), 6u), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(std::basic_string<C>(TXT("World")), std::size_t{ 6 }), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(View(TXT("World")), 6), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(View(TXT("WORLD")), 6u, areg::COUNT_ALL, false), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(View(TXT("Worldwide")).substr(0, 5), 6), areg::Ordering::Equal);
}

/**
 * \brief   A bool second argument is 'caseSensitive', an integral one is a position.
 **/
TYPED_TEST(StringCommonCompare, CompareBoolOrPosition)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S str(TXT("abcdef"));
    const C* upper = TXT("ABCDEF");
    EXPECT_EQ(str.compare(TXT("ABCDEF"), false), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(upper, false), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(upper, true), areg::Ordering::Bigger);
    EXPECT_EQ(str.compare(TXT("bcdef"), true), areg::Ordering::Bigger);
    EXPECT_EQ(str.compare(TXT("def"), 3), areg::Ordering::Equal);
}

/**
 * \brief   Case-insensitive compare of a view that is not null-terminated.
 **/
TYPED_TEST(StringCommonCompare, CompareIgnoreCaseView)
{
    using S = TypeParam;
    using C = CharOf<S>;
    using View = std::basic_string_view<C>;
    const S str(TXT("ABC"));
    const C buffer[]{ CH('a'), CH('b'), CH('c') };
    EXPECT_EQ(str.compare(View(TXT("abcdef")).substr(0, 3), false), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(View(buffer, 3), false), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(View(TXT("abd")), false), areg::Ordering::Smaller);
    EXPECT_EQ(str.compare(View(TXT("abb")), false), areg::Ordering::Bigger);
    EXPECT_EQ(str.compare(TXT("abcdef"), 0, 3, false), areg::Ordering::Equal);
}

/**
 * \brief   Equality with text of the other character width.
 **/
TYPED_TEST(StringCommonCompare, OtherWidthEquality)
{
    using S = TypeParam;
    using O = OtherOf<S>;
    using OC = CharOf<O>;
    const S str(TXT("abc"));
    EXPECT_TRUE(str == OTXT("abc"));
    EXPECT_TRUE(str == std::basic_string<OC>(OTXT("abc")));
    EXPECT_TRUE(str == O(OTXT("abc")));
    EXPECT_FALSE(str == OTXT("abd"));
    EXPECT_FALSE(str == OTXT("ab"));
    EXPECT_TRUE(str != OTXT("abd"));
    EXPECT_TRUE(str != std::basic_string<OC>(OTXT("abcd")));
    EXPECT_TRUE(str != O(OTXT("ABC")));
    EXPECT_FALSE(str != O(OTXT("abc")));
    EXPECT_TRUE(S() == static_cast<const OC*>(nullptr));
    EXPECT_TRUE(str != static_cast<const OC*>(nullptr));
}

/**
 * \brief   The protected comparing helpers.
 **/
TYPED_TEST(StringCommonCompare, ProtectedHelpers)
{
    StringProbe<TypeParam> str;
    str.assign(TXT("Hello World"));
    const areg::CharPos six{ 6 };
    EXPECT_EQ(str.compare_exact(six, TXT("World")), areg::Ordering::Equal);
    EXPECT_EQ(str.compare_exact(six, TXT("Worlds")), areg::Ordering::Smaller);
    EXPECT_EQ(str.compare_exact(six, TXT("Wo")), areg::Ordering::Equal);
    EXPECT_EQ(str.compare_exact(six, TXT("Xo")), areg::Ordering::Smaller);
    EXPECT_EQ(str.compare_exact(six, TXT("Va")), areg::Ordering::Bigger);
    EXPECT_EQ(str.compare_exact(areg::END_POS, TXT("W")), areg::Ordering::Smaller);

    EXPECT_EQ(str.compare_ignore_case(six, TXT("WORLD")), areg::Ordering::Equal);
    EXPECT_EQ(str.compare_ignore_case(six, TXT("wo"), 2), areg::Ordering::Equal);
    EXPECT_EQ(str.compare_ignore_case(six, TXT("WORLDS")), areg::Ordering::Smaller);
    EXPECT_EQ(str.compare_ignore_case(six, TXT("x")), areg::Ordering::Smaller);
    EXPECT_EQ(str.compare_ignore_case(six, TXT("a")), areg::Ordering::Bigger);
    EXPECT_EQ(str.compare_ignore_case(areg::INVALID_POS, TXT("a")), areg::Ordering::Smaller);

    EXPECT_EQ(str.compare(six, TXT("WORLD"), areg::COUNT_ALL, false), areg::Ordering::Equal);
    EXPECT_EQ(str.compare(six, TXT("WORLD"), areg::COUNT_ALL, true), areg::Ordering::Bigger);
}

//////////////////////////////////////////////////////////////////////////
// StringCommonConcat
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The += operators of the same width.
 **/
TYPED_TEST(StringCommonConcat, PlusEqual)
{
    using S = TypeParam;
    using C = CharOf<S>;
    S str(TXT("He"));
    str += S(TXT("l"));
    str += std::basic_string<C>(TXT("l"));
    str += std::basic_string_view<C>(TXT("o!"), 1);
    str += TXT(" ");
    str += static_cast<const C*>(nullptr);
    str += CH('W');
    EXPECT_STREQ(str.as_string(), TXT("Hello W"));
}

/**
 * \brief   The += operators of the other width.
 **/
TYPED_TEST(StringCommonConcat, PlusEqualOtherWidth)
{
    using S = TypeParam;
    using O = OtherOf<S>;
    using OC = CharOf<O>;
    S str(TXT("a"));
    str += OTXT("b");
    str += std::basic_string<OC>(OTXT("c"));
    str += static_cast<OC>('d');
    str += O(OTXT("e"));
    str += static_cast<const OC*>(nullptr);
    EXPECT_STREQ(str.as_string(), TXT("abcde"));
    str.append(OTXT("fghi"), 1);
    EXPECT_STREQ(str.as_string(), TXT("abcdef"));
}

/**
 * \brief   Every binary + operator gives the joined text.
 **/
TYPED_TEST(StringCommonConcat, BinaryPlus)
{
    using S = TypeParam;
    using C = CharOf<S>;
    using Std = std::basic_string<C>;
    using View = std::basic_string_view<C>;
    const S lhs(TXT("ab"));
    const S rhs(TXT("cd"));
    const Std joined{ expected<C>("abcd") };

    EXPECT_EQ((lhs + rhs).data(), joined);
    EXPECT_EQ((lhs + Std(TXT("cd"))).data(), joined);
    EXPECT_EQ((lhs + View(TXT("cd"))).data(), joined);
    EXPECT_EQ((lhs + TXT("cd")).data(), joined);
    EXPECT_EQ((S(TXT("abc")) + CH('d')).data(), joined);
    EXPECT_EQ((Std(TXT("ab")) + rhs).data(), joined);
    EXPECT_EQ((View(TXT("ab")) + rhs).data(), joined);
    EXPECT_EQ((TXT("ab") + rhs).data(), joined);
    EXPECT_EQ((CH('a') + S(TXT("bcd"))).data(), joined);
    EXPECT_EQ((lhs + static_cast<const C*>(nullptr)).data(), expected<C>("ab"));
    EXPECT_EQ((static_cast<const C*>(nullptr) + rhs).data(), expected<C>("cd"));
    EXPECT_EQ((S() + S()).data(), Std());
    EXPECT_EQ((lhs + OTXT("cd")).data(), joined);
    EXPECT_EQ((OTXT("ab") + rhs).data(), joined);
    EXPECT_EQ((TXT("a") + S(TXT("b")) + TXT("c") + CH('d')).data(), joined);
}

/**
 * \brief   The append methods of every source type.
 **/
TYPED_TEST(StringCommonConcat, AppendMethods)
{
    using S = TypeParam;
    using C = CharOf<S>;
    S str(TXT("Hello"));
    EXPECT_STREQ(str.append(TXT(" World!!!"), 6).as_string(), TXT("Hello World"));
    EXPECT_STREQ(str.append(CH('!')).as_string(), TXT("Hello World!"));

    str = TXT("a");
    str.append(TXT("b"));
    str.append(std::basic_string<C>(TXT("xcd")), 1);
    str.append(std::basic_string<C>(TXT("xex")), 1, 1);
    str.append(std::basic_string_view<C>(TXT("fg")));
    str.append(std::basic_string_view<C>(TXT("xhx")), 1, 1);
    str.append(S(TXT("ij")));
    str.append(S(TXT("xkx")), 1, 1);
    EXPECT_STREQ(str.as_string(), TXT("abcdefghijk"));
}

//////////////////////////////////////////////////////////////////////////
// StringCommonSearch
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   find_first of a character, case-sensitive and not, from a position.
 **/
TYPED_TEST(StringCommonSearch, FindFirstChar)
{
    using S = TypeParam;
    const S str(TXT("Hello World"));
    EXPECT_EQ(str.find_first(CH('W')), 6);
    EXPECT_EQ(str.find_first(CH('z')), areg::END_POS);
    EXPECT_EQ(str.find_first(CH('w'), areg::START_POS, false), 6);
    EXPECT_EQ(str.find_first(CH('w'), areg::START_POS, true), areg::END_POS);
    EXPECT_EQ(str.find_first(CH('l'), 4), 9);
    EXPECT_EQ(str.find_first(CH('L'), 4, false), 9);
    EXPECT_EQ(str.find_first(CH('H'), 100), areg::INVALID_POS);
    EXPECT_EQ(S(TXT("abcabc")).find_first(CH('a'), 1), 3);
    EXPECT_EQ(S().find_first(CH('a')), areg::INVALID_POS);
}

/**
 * \brief   find_first of a phrase, case-sensitive and not, as a whole word and not.
 **/
TYPED_TEST(StringCommonSearch, FindFirstPhrase)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S str(TXT("Hello World Hello"));
    EXPECT_EQ(str.find_first(TXT("World")), 6);
    EXPECT_EQ(str.find_first(TXT("xyz")), areg::END_POS);
    EXPECT_EQ(str.find_first(TXT("Hello"), 1), 12);
    EXPECT_EQ(str.find_first(TXT("WORLD"), areg::START_POS, false), 6);
    EXPECT_EQ(str.find_first(TXT("WORLD"), areg::START_POS, true), areg::END_POS);
    EXPECT_EQ(str.find_first(TXT("")), areg::INVALID_POS);
    EXPECT_EQ(str.find_first(static_cast<const C*>(nullptr)), areg::INVALID_POS);
    EXPECT_EQ(str.find_first(TXT("World"), 100), areg::INVALID_POS);

    EXPECT_EQ(str.find_first(S(TXT("World"))), 6);
    EXPECT_EQ(str.find_first(S(TXT("hello")), 1, false), 12);
    EXPECT_EQ(str.find_first(S()), areg::INVALID_POS);
}

/**
 * \brief   find_first of a whole word skips a match inside a longer word.
 **/
TYPED_TEST(StringCommonSearch, FindFirstWholeWord)
{
    using S = TypeParam;
    const S str(TXT("abcd xabc abc_d abc, end"));
    EXPECT_EQ(str.find_first(TXT("abc"), areg::START_POS, true, true), 16);
    EXPECT_EQ(str.find_first(TXT("ABC"), areg::START_POS, false, true), 16);
    EXPECT_EQ(str.find_first(S(TXT("abc")), areg::START_POS, true, true), 16);
    EXPECT_EQ(str.find_first(TXT("end"), areg::START_POS, true, true), 21);
    EXPECT_EQ(str.find_first(TXT("abcd"), areg::START_POS, true, true), 0);
    EXPECT_EQ(str.find_first(TXT("bc"), areg::START_POS, true, true), areg::END_POS);
    EXPECT_EQ(S(TXT("abcde")).find_first(TXT("abc"), areg::START_POS, true, true), areg::END_POS);
    EXPECT_EQ(S(TXT("abc")).find_first(TXT("abc"), areg::START_POS, true, true), 0);
    EXPECT_EQ(S(TXT("x abc")).find_first(TXT("abc"), areg::START_POS, true, true), 2);
    EXPECT_EQ(S(TXT("a-b")).find_first(TXT("-"), areg::START_POS, true, true), 1);
}

/**
 * \brief   find_last of a character and of a phrase, case-sensitive and not, from a position.
 **/
TYPED_TEST(StringCommonSearch, FindLast)
{
    using S = TypeParam;
    const S str(TXT("Hello World"));
    EXPECT_EQ(str.find_last(CH('l')), 9);
    EXPECT_EQ(str.find_last(CH('z')), areg::END_POS);
    EXPECT_EQ(str.find_last(CH('l'), 5), 3);
    EXPECT_EQ(str.find_last(CH('L'), areg::END_POS, false), 9);
    EXPECT_EQ(str.find_last(CH('L'), 5, false), 3);
    EXPECT_EQ(str.find_last(CH('Z'), areg::END_POS, false), areg::END_POS);
    EXPECT_EQ(S().find_last(CH('a')), areg::INVALID_POS);

    const S abc(TXT("abcABCabc"));
    EXPECT_EQ(abc.find_last(TXT("abc")), 6);
    EXPECT_EQ(abc.find_last(TXT("abc"), 5), 0);
    EXPECT_EQ(abc.find_last(TXT("ABC")), 3);
    EXPECT_EQ(abc.find_last(TXT("ABC"), areg::END_POS, false), 6);
    EXPECT_EQ(abc.find_last(TXT("ABC"), 5, false), 3);
    EXPECT_EQ(abc.find_last(TXT("xyz")), areg::END_POS);
    EXPECT_EQ(abc.find_last(TXT("abcABCabcabc")), areg::END_POS);
    EXPECT_EQ(abc.find_last(TXT("")), areg::INVALID_POS);
    EXPECT_EQ(abc.find_last(S(TXT("abc"))), 6);
    EXPECT_EQ(abc.find_last(S(TXT("Abc")), areg::END_POS, false), 6);
    EXPECT_EQ(abc.find_last(TXT("abcd"), 3, areg::END_POS, true), 6);
}

/**
 * \brief   find_one_of finds the first of any listed character.
 **/
TYPED_TEST(StringCommonSearch, FindOneOf)
{
    using S = TypeParam;
    const S str(TXT("Hello World"));
    EXPECT_EQ(str.find_one_of(TXT("xWz")), 6);
    EXPECT_EQ(str.find_one_of(TXT("xyz")), areg::END_POS);
    EXPECT_EQ(str.find_one_of(TXT("lo"), 5), 7);
    EXPECT_EQ(str.find_one_of(TXT("")), areg::INVALID_POS);
    EXPECT_EQ(str.find_one_of(TXT("H"), 100), areg::INVALID_POS);
}

/**
 * \brief   find_word and extract_word walk the words of the string.
 **/
TYPED_TEST(StringCommonSearch, FindAndExtractWord)
{
    using S = TypeParam;
    const S str(TXT("  hello, big_world"));
    areg::CharPos start{ areg::START_POS };
    areg::CharPos end{ areg::INVALID_POS };
    str.find_word(start, end);
    EXPECT_EQ(start, 2);
    EXPECT_EQ(end, 7);

    S word;
    start = areg::START_POS;
    str.extract_word(word, start, end);
    EXPECT_STREQ(word.as_string(), TXT("hello"));
    start = end;
    str.extract_word(word, start, end);
    EXPECT_STREQ(word.as_string(), TXT("big_world"));
    EXPECT_EQ(start, 9);
    EXPECT_EQ(end, areg::END_POS);
    start = end;
    str.extract_word(word, start, end);
    EXPECT_TRUE(word.is_empty());
    EXPECT_EQ(start, areg::END_POS);
    EXPECT_EQ(end, areg::INVALID_POS);

    start = areg::START_POS;
    S(TXT(" ,; ")).find_word(start, end);
    EXPECT_EQ(start, areg::END_POS);
    EXPECT_EQ(end, areg::INVALID_POS);
}

/**
 * \brief   The protected searching helpers.
 **/
TYPED_TEST(StringCommonSearch, ProtectedHelpers)
{
    using S = TypeParam;
    using C = CharOf<S>;
    StringProbe<S> str;
    str.assign(TXT("one Two three two"));
    EXPECT_EQ(str.find_first_phrase(TXT("two")), 14);
    EXPECT_EQ(str.find_first_phrase(TXT("twoX"), 3), 14);
    EXPECT_EQ(str.find_first_phrase(TXT("xyz")), areg::END_POS);
    EXPECT_EQ(str.find_first_phrase(TXT("")), areg::INVALID_POS);

    EXPECT_EQ(str.find_phrase(TXT("TWO")), 4);
    EXPECT_EQ(str.find_phrase(TXT("TWO"), 5), 14);
    EXPECT_EQ(str.find_phrase(std::basic_string<C>(TXT("THREE"))), 8);
    EXPECT_EQ(str.find_phrase(TXT("four")), areg::END_POS);
    EXPECT_EQ(str.find_phrase(static_cast<const C*>(nullptr)), areg::INVALID_POS);

    EXPECT_EQ(str.find_first_word(TXT("two"), true), 14);
    EXPECT_EQ(str.find_first_word(TXT("two"), false), 4);
    EXPECT_EQ(str.find_first_word(std::basic_string<C>(TXT("thre")), false), areg::END_POS);
    EXPECT_EQ(str.find_first_word(static_cast<const C*>(nullptr), true), areg::INVALID_POS);

    EXPECT_TRUE(str.is_name_char(CH('_')));
    EXPECT_TRUE(str.is_name_char(CH('a')));
    EXPECT_TRUE(str.is_name_char(CH('7')));
    EXPECT_FALSE(str.is_name_char(CH('-')));
    std::locale loc;
    EXPECT_TRUE(StringProbe<S>::is_name_char(CH('Q'), loc));
    EXPECT_FALSE(StringProbe<S>::is_name_char(CH(' '), loc));
}

//////////////////////////////////////////////////////////////////////////
// StringCommonSubstring
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   substring in place, and into another string, by position and count.
 **/
TYPED_TEST(StringCommonSubstring, ByPosition)
{
    using S = TypeParam;
    S str(TXT("Hello World"));
    EXPECT_STREQ(str.substring(6).as_string(), TXT("World"));
    str = TXT("Hello World");
    EXPECT_STREQ(str.substring(0, 5).as_string(), TXT("Hello"));
    str = TXT("Hello World");
    EXPECT_TRUE(str.substring(11).is_empty());
    str = TXT("Hello World");
    EXPECT_TRUE(str.substring(100).is_empty());
    str = TXT("Hello World");
    EXPECT_TRUE(str.substring(areg::END_POS).is_empty());

    const S src(TXT("Hello World"));
    S result;
    src.substring(result, 6, 5);
    EXPECT_STREQ(result.as_string(), TXT("World"));
    src.substring(result, 6);
    EXPECT_STREQ(result.as_string(), TXT("World"));
    src.substring(result);
    EXPECT_STREQ(result.as_string(), TXT("Hello World"));
    EXPECT_STREQ(src.as_string(), TXT("Hello World"));

    S fromEmpty;
    S().substring(fromEmpty, areg::START_POS, 5);
    EXPECT_TRUE(fromEmpty.is_empty());
}

/**
 * \brief   substring until a phrase or a character, returning the next position.
 **/
TYPED_TEST(StringCommonSubstring, UntilDelimiter)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S str(TXT("key=value;next"));
    S out;
    EXPECT_EQ(str.substring(out, TXT("=")), 4);
    EXPECT_STREQ(out.as_string(), TXT("key"));
    EXPECT_EQ(str.substring(out, S(TXT(";")), 4), 10);
    EXPECT_STREQ(out.as_string(), TXT("value"));
    EXPECT_EQ(str.substring(out, std::basic_string<C>(TXT(";")), 10), areg::END_POS);
    EXPECT_STREQ(out.as_string(), TXT("next"));
    EXPECT_EQ(str.substring(out, TXT("")), areg::INVALID_POS);
    EXPECT_TRUE(out.is_empty());
    EXPECT_EQ(str.substring(out, static_cast<const C*>(nullptr)), areg::INVALID_POS);
    EXPECT_EQ(str.substring(out, TXT("="), 100), areg::INVALID_POS);

    EXPECT_EQ(str.substring(out, CH('=')), 4);
    EXPECT_STREQ(out.as_string(), TXT("key"));
    EXPECT_EQ(str.substring(out, CH('#'), 4), areg::END_POS);
    EXPECT_STREQ(out.as_string(), TXT("value;next"));
    EXPECT_EQ(str.substring(out, CH('='), 100), areg::INVALID_POS);
}

/**
 * \brief   left_side and right_side.
 **/
TYPED_TEST(StringCommonSubstring, LeftAndRightSide)
{
    using S = TypeParam;
    const S str(TXT("Hello World"));
    EXPECT_STREQ(str.left_side(5).as_string(), TXT("Hello"));
    EXPECT_STREQ(str.right_side(5).as_string(), TXT("World"));
    EXPECT_STREQ(str.left_side(100).as_string(), TXT("Hello World"));
    EXPECT_STREQ(str.right_side(100).as_string(), TXT("Hello World"));
    EXPECT_TRUE(str.left_side(0).is_empty());
    EXPECT_TRUE(str.right_side(0).is_empty());

    const S one(TXT("A"));
    EXPECT_STREQ(one.left_side(1).as_string(), TXT("A"));
    EXPECT_STREQ(one.right_side(1).as_string(), TXT("A"));
}

/**
 * \brief   The static substr walks a text by a phrase.
 **/
TYPED_TEST(StringCommonSubstring, StaticSubstr)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S test(TXT("0123 456 789 0123"));
    const C* next{ test.as_string() };
    EXPECT_STREQ(S::substr(next, TXT(" "), &next).as_string(), TXT("0123"));
    EXPECT_STREQ(S::substr(next, TXT(" "), &next).as_string(), TXT("456"));
    EXPECT_STREQ(S::substr(next, TXT(" "), &next).as_string(), TXT("789"));
    EXPECT_STREQ(S::substr(next, TXT(" "), &next).as_string(), TXT("0123"));
    EXPECT_TRUE(next == nullptr);

    next = test.as_string();
    EXPECT_TRUE(S::substr(next, TXT("0123"), &next).is_empty());
    EXPECT_STREQ(next, TXT(" 456 789 0123"));
    EXPECT_STREQ(S::substr(next, TXT("0123"), &next).as_string(), TXT(" 456 789 "));
    EXPECT_STREQ(S::substr(test.as_string(), TXT("789")).as_string(), TXT("0123 456 "));
    EXPECT_TRUE(S::substr(nullptr, TXT(" "), &next).is_empty());
    EXPECT_TRUE(next == nullptr);
}

/**
 * \brief   split by a character and by a phrase.
 **/
TYPED_TEST(StringCommonSubstring, Split)
{
    using S = TypeParam;
    const S str(TXT("a,b,,c"));
    const std::vector<S> byChar{ str.split(CH(',')) };
    ASSERT_EQ(byChar.size(), 4u);
    EXPECT_STREQ(byChar[0].as_string(), TXT("a"));
    EXPECT_STREQ(byChar[1].as_string(), TXT("b"));
    EXPECT_TRUE(byChar[2].is_empty());
    EXPECT_STREQ(byChar[3].as_string(), TXT("c"));
    EXPECT_TRUE(S().split(CH(',')).empty());
    EXPECT_EQ(S(TXT("abc")).split(CH(',')).size(), 1u);

    const S phrases(TXT("one::two::::three"));
    const std::vector<S> byPhrase{ phrases.split(S(TXT("::"))) };
    ASSERT_EQ(byPhrase.size(), 4u);
    EXPECT_STREQ(byPhrase[0].as_string(), TXT("one"));
    EXPECT_STREQ(byPhrase[1].as_string(), TXT("two"));
    EXPECT_TRUE(byPhrase[2].is_empty());
    EXPECT_STREQ(byPhrase[3].as_string(), TXT("three"));

    const std::vector<S> whole{ phrases.split(S()) };
    ASSERT_EQ(whole.size(), 1u);
    EXPECT_EQ(whole[0], phrases);
}

//////////////////////////////////////////////////////////////////////////
// StringCommonModify
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   insert_at of every source type, including the end of the string.
 **/
TYPED_TEST(StringCommonModify, InsertAt)
{
    using S = TypeParam;
    using C = CharOf<S>;
    S str(TXT("Hllo"));
    EXPECT_STREQ(str.insert_at(CH('e'), 1).as_string(), TXT("Hello"));
    EXPECT_STREQ(str.insert_at(CH('-'), 0, 2).as_string(), TXT("--Hello"));
    EXPECT_STREQ(str.insert_at(CH('!'), areg::END_POS).as_string(), TXT("--Hello!"));
    EXPECT_STREQ(str.insert_at(CH('?'), str.length()).as_string(), TXT("--Hello!?"));
    EXPECT_STREQ(str.insert_at(CH('x'), 100).as_string(), TXT("--Hello!?"));

    str = TXT("Hello!");
    EXPECT_STREQ(str.insert_at(TXT(" World"), 5).as_string(), TXT("Hello World!"));
    EXPECT_STREQ(str.insert_at(TXT(">> "), 0).as_string(), TXT(">> Hello World!"));
    EXPECT_STREQ(str.insert_at(TXT("abc"), 0, 1).as_string(), TXT("a>> Hello World!"));
    EXPECT_STREQ(str.insert_at(TXT("."), areg::END_POS).as_string(), TXT("a>> Hello World!."));
    EXPECT_STREQ(str.insert_at(static_cast<const C*>(nullptr), 0).as_string(), TXT("a>> Hello World!."));

    str = TXT("ad");
    EXPECT_STREQ(str.insert_at(std::basic_string<C>(TXT("xbc")), 1, 1).as_string(), TXT("abcd"));
    EXPECT_STREQ(str.insert_at(std::basic_string_view<C>(TXT("e")), areg::END_POS).as_string(), TXT("abcde"));
    EXPECT_STREQ(str.insert_at(S(TXT("0123")), 0, 1, 2).as_string(), TXT("12abcde"));
    EXPECT_STREQ(str.insert_at(S(TXT("f")), str.length()).as_string(), TXT("12abcdef"));
    EXPECT_STREQ(str.insert_at(S(TXT("abc")), 0, 3).as_string(), TXT("12abcdef"));
}

/**
 * \brief   replace of a character, of a phrase, and of a range.
 **/
TYPED_TEST(StringCommonModify, Replace)
{
    using S = TypeParam;
    using C = CharOf<S>;
    using Std = std::basic_string<C>;
    using View = std::basic_string_view<C>;
    EXPECT_STREQ(S(TXT("banana")).replace(CH('a'), CH('o')).as_string(), TXT("bonono"));
    EXPECT_STREQ(S(TXT("banana")).replace(CH('a'), CH('o'), 2).as_string(), TXT("banono"));
    EXPECT_STREQ(S(TXT("banana")).replace(CH('a'), CH('o'), 0, false).as_string(), TXT("bonana"));
    EXPECT_STREQ(S(TXT("llHello")).replace(CH('l'), CH('r'), areg::START_POS, false).as_string(), TXT("rlHello"));
    EXPECT_STREQ(S(TXT("banana")).replace(CH('a'), CH('o'), 100).as_string(), TXT("banana"));
    EXPECT_TRUE(S().replace(CH('a'), CH('b')).is_empty());

    EXPECT_STREQ(S(TXT("Hello World")).replace(TXT("World"), TXT("Earth")).as_string(), TXT("Hello Earth"));
    EXPECT_STREQ(S(TXT("abcabcabc")).replace(TXT("abc"), TXT("x")).as_string(), TXT("xxx"));
    EXPECT_STREQ(S(TXT("a-b-c")).replace(TXT("-"), TXT("")).as_string(), TXT("abc"));
    EXPECT_STREQ(S(TXT("a-b-c")).replace(TXT("-"), static_cast<const C*>(nullptr)).as_string(), TXT("abc"));
    EXPECT_STREQ(S(TXT("a-b-c")).replace(TXT("-"), TXT("=+"), 0, 1).as_string(), TXT("a=b=c"));
    EXPECT_STREQ(S(TXT("a-b-c")).replace(TXT("-"), TXT("+"), 2, areg::COUNT_ALL, false).as_string(), TXT("a-b+c"));
    EXPECT_STREQ(S(TXT("aaa")).replace(TXT("a"), TXT("aa")).as_string(), TXT("aaaaaa"));
    EXPECT_STREQ(S(TXT("abc")).replace(TXT(""), TXT("x")).as_string(), TXT("abc"));
    EXPECT_TRUE(S().replace(TXT("abc"), TXT("xyz")).is_empty());

    EXPECT_STREQ(S(TXT("a-b-c")).replace(Std(TXT("-")), Std(TXT("::"))).as_string(), TXT("a::b::c"));
    EXPECT_STREQ(S(TXT("a-b-c")).replace(Std(TXT("-")), Std(), 0, false).as_string(), TXT("ab-c"));
    EXPECT_STREQ(S(TXT("a-b-c")).replace(View(TXT("-")), View(TXT("+"))).as_string(), TXT("a+b+c"));
    EXPECT_STREQ(S(TXT("a-b-c")).replace(View(TXT("-")), View(), 2).as_string(), TXT("a-bc"));
    EXPECT_STREQ(S(TXT("a-b-c")).replace(S(TXT("b")), S(TXT("BB"))).as_string(), TXT("a-BB-c"));

    EXPECT_STREQ(S(TXT("Hello World")).replace(6, 5, TXT("Earth")).as_string(), TXT("Hello Earth"));
    EXPECT_STREQ(S(TXT("Hello World")).replace(6, 5, TXT("Earth"), 3).as_string(), TXT("Hello Ear"));
    EXPECT_STREQ(S(TXT("Hello World")).replace(0, 5, Std(TXT("Hi"))).as_string(), TXT("Hi World"));
    EXPECT_STREQ(S(TXT("Hello World")).replace(5, 1, S(TXT(", "))).as_string(), TXT("Hello, World"));
    EXPECT_STREQ(S(TXT("Hello")).replace(100, 1, TXT("x")).as_string(), TXT("Hello"));
}

/**
 * \brief   replace_with and move_to.
 **/
TYPED_TEST(StringCommonModify, ReplaceWithAndMoveTo)
{
    using S = TypeParam;
    S str(TXT("Hello World"));
    EXPECT_EQ(str.replace_with(6, 5, TXT("Earth!"), 6), 12);
    EXPECT_STREQ(str.as_string(), TXT("Hello Earth!"));
    EXPECT_EQ(str.replace_with(6, 6, TXT("Ea"), 2), 8);
    EXPECT_STREQ(str.as_string(), TXT("Hello Ea"));
    EXPECT_EQ(str.replace_with(areg::END_POS, 1, TXT("x"), 1), areg::INVALID_POS);

    str = TXT("a=1;b=2");
    EXPECT_EQ(str.replace_with(TXT("="), 1, TXT(":="), 2, areg::START_POS), 3);
    EXPECT_STREQ(str.as_string(), TXT("a:=1;b=2"));
    EXPECT_EQ(str.replace_with(TXT("="), 1, TXT(":="), 2, 4), 8);
    EXPECT_STREQ(str.as_string(), TXT("a:=1;b:=2"));
    EXPECT_EQ(str.replace_with(TXT("#"), 1, TXT("x"), 1, areg::START_POS), areg::INVALID_POS);

    str = TXT("abc");
    EXPECT_EQ(str.move_to(1, 2).length(), 5);
    EXPECT_EQ(str.at(0), CH('a'));
    EXPECT_EQ(str.at(3), CH('b'));
    EXPECT_STREQ(str.move_to(3, -2).as_string(), TXT("abc"));
    EXPECT_STREQ(str.move_to(1, 0).as_string(), TXT("abc"));
    EXPECT_STREQ(str.move_to(1, -5).as_string(), TXT("bc"));
    EXPECT_STREQ(str.move_to(100, 1).as_string(), TXT("bc"));
    EXPECT_STREQ(str.move_to(areg::END_POS, -1).as_string(), TXT("b"));
}

/**
 * \brief   remove of a range, of a character and of a phrase.
 **/
TYPED_TEST(StringCommonModify, Remove)
{
    using S = TypeParam;
    using C = CharOf<S>;
    EXPECT_STREQ(S(TXT("Hello World")).remove(5).as_string(), TXT("Hello"));
    EXPECT_STREQ(S(TXT("Hello World")).remove(0, 6).as_string(), TXT("World"));
    EXPECT_STREQ(S(TXT("Hello World")).remove(5, 6).as_string(), TXT("Hello"));
    EXPECT_STREQ(S(TXT("Hello")).remove(100).as_string(), TXT("Hello"));

    EXPECT_STREQ(S(TXT("Hello")).remove(CH('l')).as_string(), TXT("Heo"));
    EXPECT_STREQ(S(TXT("banana")).remove(CH('a'), 2).as_string(), TXT("bann"));
    EXPECT_STREQ(S(TXT("banana")).remove(CH('a'), 0, false).as_string(), TXT("bnana"));
    EXPECT_STREQ(S(TXT("aaa")).remove(CH('a')).as_string(), TXT(""));

    EXPECT_STREQ(S(TXT("Hello World")).remove(TXT(" World")).as_string(), TXT("Hello"));
    EXPECT_STREQ(S(TXT("a--b--c")).remove(TXT("--")).as_string(), TXT("abc"));
    EXPECT_STREQ(S(TXT("a--b--c")).remove(TXT("--"), 0, false).as_string(), TXT("ab--c"));
    EXPECT_STREQ(S(TXT("a--b--c")).remove(S(TXT("--")), 2).as_string(), TXT("a--bc"));
    EXPECT_STREQ(S(TXT("a--b--c")).remove(std::basic_string<C>(TXT("-"))).as_string(), TXT("abc"));
    EXPECT_STREQ(S(TXT("abc")).remove(TXT("")).as_string(), TXT("abc"));
    EXPECT_STREQ(S(TXT("abc")).remove(static_cast<const C*>(nullptr)).as_string(), TXT("abc"));
    EXPECT_STREQ(S(TXT("abc")).remove(S()).as_string(), TXT("abc"));
    EXPECT_STREQ(S(TXT("abc")).remove(std::basic_string<C>()).as_string(), TXT("abc"));
}

/**
 * \brief   resize, truncate, reserve, clear, release and free_extra.
 **/
TYPED_TEST(StringCommonModify, SizeAndCapacity)
{
    using S = TypeParam;
    S str(TXT("Hi"));
    EXPECT_STREQ(str.resize(5, CH('x')).as_string(), TXT("Hixxx"));
    EXPECT_EQ(str.length(), 5);
    EXPECT_STREQ(str.resize(1).as_string(), TXT("H"));
    EXPECT_EQ(str.resize(3).length(), 3);
    EXPECT_EQ(str.at(2), CH('\0'));

    str = TXT("Hello World");
    EXPECT_STREQ(str.truncate(100).as_string(), TXT("Hello World"));
    EXPECT_STREQ(str.truncate(5).as_string(), TXT("Hello"));
    EXPECT_TRUE(str.truncate(0).is_empty());

    str = TXT("Hi");
    EXPECT_GE(str.reserve(100).capacity(), 100);
    EXPECT_STREQ(str.as_string(), TXT("Hi"));
    const areg::CharCount before{ str.capacity() };
    str.free_extra();
    EXPECT_LE(str.capacity(), before);
    EXPECT_STREQ(str.as_string(), TXT("Hi"));

    str.clear();
    EXPECT_TRUE(str.is_empty());
    str = TXT("a string long enough to be allocated on the heap");
    str.release();
    EXPECT_TRUE(str.is_empty());
    EXPECT_STREQ(str.as_string(), TXT(""));
}

/**
 * \brief   at, set_at, operator [] and buffer.
 **/
TYPED_TEST(StringCommonModify, CharacterAccess)
{
    using S = TypeParam;
    using C = CharOf<S>;
    S str(TXT("Hello"));
    EXPECT_EQ(str[0], CH('H'));
    EXPECT_EQ(str[4], CH('o'));
    EXPECT_EQ(str.at(0), CH('H'));
    EXPECT_EQ(str.at(5), CH('\0'));
    EXPECT_EQ(str.at(areg::END_POS), CH('\0'));

    EXPECT_STREQ(str.set_at(CH('h'), 0).as_string(), TXT("hello"));
    EXPECT_STREQ(str.set_at(CH('!')).as_string(), TXT("hello!"));
    EXPECT_STREQ(str.set_at(CH('?'), 100).as_string(), TXT("hello!"));

    EXPECT_STREQ(str.buffer(), TXT("hello!"));
    EXPECT_STREQ(str.buffer(2), TXT("llo!"));
    EXPECT_STREQ(str.buffer(areg::END_POS), TXT(""));
    EXPECT_STREQ(str.buffer(100), TXT(""));
    str.buffer(1)[0] = CH('E');
    EXPECT_STREQ(str.as_string(), TXT("hEllo!"));

    const S& cref = str;
    const C* cbuf{ cref.buffer(1) };
    EXPECT_STREQ(cbuf, TXT("Ello!"));
    EXPECT_STREQ(static_cast<const C*>(str), TXT("hEllo!"));
    EXPECT_EQ(str.data(), expected<C>("hEllo!"));
}

/**
 * \brief   make_lower, make_upper and make_alphanumeric.
 **/
TYPED_TEST(StringCommonModify, CaseAndAlphanumeric)
{
    using S = TypeParam;
    EXPECT_STREQ(S(TXT("Hello WORLD 123")).make_lower().as_string(), TXT("hello world 123"));
    EXPECT_STREQ(S(TXT("Hello world 123")).make_upper().as_string(), TXT("HELLO WORLD 123"));
    EXPECT_TRUE(S().make_upper().is_empty());

    const S symbols(TXT("!@#$%^&*()_+-=[]{}|;:,.<>?"));
    EXPECT_EQ(S(symbols).make_lower(), symbols);
    EXPECT_EQ(S(symbols).make_upper(), symbols);

    S str(TXT("Hello, World! 123"));
    EXPECT_STREQ(str.make_alphanumeric().as_string(), TXT("HelloWorld123"));
    EXPECT_TRUE(str.is_alphanumeric());
    EXPECT_TRUE(S().make_alphanumeric().is_empty());
    EXPECT_TRUE(S(TXT("!?")).make_alphanumeric().is_empty());
}

//////////////////////////////////////////////////////////////////////////
// StringCommonTrim
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The trims in place.
 **/
TYPED_TEST(StringCommonTrim, InPlace)
{
    using S = TypeParam;
    EXPECT_STREQ(S(TXT("   Hello")).trim_left().as_string(), TXT("Hello"));
    EXPECT_STREQ(S(TXT("Hello   ")).trim_left().as_string(), TXT("Hello   "));
    EXPECT_STREQ(S(TXT("Hello   ")).trim_right().as_string(), TXT("Hello"));
    EXPECT_STREQ(S(TXT("   Hello")).trim_right().as_string(), TXT("   Hello"));
    EXPECT_STREQ(S(TXT("   Hello World   ")).trim_all().as_string(), TXT("Hello World"));
    EXPECT_STREQ(S(TXT("Hello")).trim_all().as_string(), TXT("Hello"));
    EXPECT_STREQ(S(TXT("\t\n Hello \t\n")).trim_all().as_string(), TXT("Hello"));
    EXPECT_TRUE(S(TXT(" \t ")).trim_left().is_empty());
    EXPECT_TRUE(S(TXT(" \t ")).trim_right().is_empty());
    EXPECT_TRUE(S(TXT(" \t ")).trim_all().is_empty());
    EXPECT_TRUE(S().trim_all().is_empty());
}

/**
 * \brief   The trims into another string leave the source and give the trimmed text.
 **/
TYPED_TEST(StringCommonTrim, IntoResult)
{
    using S = TypeParam;
    using C = CharOf<S>;
    using Std = std::basic_string<C>;
    const S both(TXT("  ab  "));
    const S none(TXT("ab"));
    S out(TXT("old"));
    Std stdOut(TXT("old"));

    both.trim_left(out);
    EXPECT_STREQ(out.as_string(), TXT("ab  "));
    none.trim_left(out);
    EXPECT_STREQ(out.as_string(), TXT("ab"));
    both.trim_left(stdOut);
    EXPECT_EQ(stdOut, expected<C>("ab  "));

    both.trim_right(out);
    EXPECT_STREQ(out.as_string(), TXT("  ab"));
    none.trim_right(out);
    EXPECT_STREQ(out.as_string(), TXT("ab"));
    both.trim_right(stdOut);
    EXPECT_EQ(stdOut, expected<C>("  ab"));

    both.trim_all(out);
    EXPECT_STREQ(out.as_string(), TXT("ab"));
    S(TXT("  ab")).trim_all(out);
    EXPECT_STREQ(out.as_string(), TXT("ab"));
    S(TXT("ab  ")).trim_all(out);
    EXPECT_STREQ(out.as_string(), TXT("ab"));
    none.trim_all(stdOut);
    EXPECT_EQ(stdOut, expected<C>("ab"));

    S(TXT("   ")).trim_all(out);
    EXPECT_TRUE(out.is_empty());
    S().trim_left(out);
    EXPECT_TRUE(out.is_empty());
    EXPECT_STREQ(both.as_string(), TXT("  ab  "));
}

//////////////////////////////////////////////////////////////////////////
// StringCommonNumbers
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   make_string writes 32-bit integers as the printf formats do, in every radix.
 **/
TYPED_TEST(StringCommonNumbers, MakeString32MatchesPrintf)
{
    using S = TypeParam;
    using C = CharOf<S>;
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
 * \brief   make_string writes 64-bit integers as the printf formats do, in every radix.
 **/
TYPED_TEST(StringCommonNumbers, MakeString64MatchesPrintf)
{
    using S = TypeParam;
    using C = CharOf<S>;
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
 * \brief   make_string writes real numbers and booleans.
 **/
TYPED_TEST(StringCommonNumbers, MakeStringRealAndBool)
{
    using S = TypeParam;
    using C = CharOf<S>;
    EXPECT_EQ(S::make_string(3.25f).data(), printed<C>("%f", 3.25));
    EXPECT_EQ(S::make_string(-1.5e300).data(), printed<C>("%g", -1.5e300));
    EXPECT_EQ(S::make_string(0.5).data(), printed<C>("%g", 0.5));
    EXPECT_EQ(S::make_string(std::numeric_limits<float>::max()).data(), printed<C>("%f", static_cast<double>(std::numeric_limits<float>::max())));
    EXPECT_STREQ(S::make_string(true).as_string(), TXT("true"));
    EXPECT_STREQ(S::make_string(false).as_string(), TXT("false"));
    EXPECT_STREQ(S::make_string(static_cast<int32_t>(42)).as_string(), TXT("42"));
    EXPECT_STREQ(S::make_string(static_cast<uint32_t>(42)).as_string(), TXT("42"));
}

/**
 * \brief   The from_ methods assign the text the make_string methods write.
 **/
TYPED_TEST(StringCommonNumbers, FromMethods)
{
    using S = TypeParam;
    S str;
    EXPECT_STREQ(str.from_int32(-7).as_string(), TXT("-7"));
    EXPECT_STREQ(str.from_int32(255, areg::Radix::Hexadecimal).as_string(), TXT("0x000000FF"));
    EXPECT_STREQ(str.from_uint32(7u).as_string(), TXT("7"));
    EXPECT_STREQ(str.from_uint32(5u, areg::Radix::Binary).as_string(), TXT("101"));
    EXPECT_STREQ(str.from_int64(-9000000000LL).as_string(), TXT("-9000000000"));
    EXPECT_STREQ(str.from_int64(8, areg::Radix::Octal).as_string(), TXT("0000000000000000000010"));
    EXPECT_STREQ(str.from_uint64(255u, areg::Radix::Hexadecimal).as_string(), TXT("0x00000000000000FF"));
    EXPECT_STREQ(str.from_uint64(18446744073709551615ull).as_string(), TXT("18446744073709551615"));
    EXPECT_STREQ(str.from_bool(true).as_string(), TXT("true"));
    EXPECT_STREQ(str.from_bool(false).as_string(), TXT("false"));
    EXPECT_STREQ(str.from_double(0.5).as_string(), TXT("0.5"));
    EXPECT_STREQ(str.from_float(0.5f).as_string(), TXT("0.500000"));
}

/**
 * \brief   The from_ and to_ methods round-trip every type.
 **/
TYPED_TEST(StringCommonNumbers, RoundTrip)
{
    using S = TypeParam;
    S str;
    EXPECT_EQ(str.from_int32(-12345).to_int32(), -12345);
    EXPECT_EQ(str.from_uint32(4294967295u).to_uint32(), 4294967295u);
    EXPECT_EQ(str.from_int64(INT64_C(-9223372036854775807)).to_int64(), INT64_C(-9223372036854775807));
    EXPECT_EQ(str.from_uint64(UINT64_C(18446744073709551615)).to_uint64(), UINT64_C(18446744073709551615));
    EXPECT_NEAR(str.from_float(3.14f).to_float(), 3.14f, 0.01f);
    EXPECT_NEAR(str.from_double(3.14159265).to_double(), 3.14159265, 0.0001);
    EXPECT_TRUE(str.from_bool(true).to_bool());
    EXPECT_FALSE(str.from_bool(false).to_bool());
    EXPECT_EQ(str.from_uint32(255u, areg::Radix::Hexadecimal).to_uint32(areg::Radix::Hexadecimal), 255u);
    EXPECT_EQ(str.from_uint64(UINT64_C(0xFFFFFFFFFF), areg::Radix::Hexadecimal).to_uint64(areg::Radix::Hexadecimal), UINT64_C(0xFFFFFFFFFF));
    EXPECT_EQ(str.from_int64(-64, areg::Radix::Octal).to_int64(areg::Radix::Octal), -64);
    EXPECT_EQ(str.from_uint32(10u, areg::Radix::Binary).to_uint32(areg::Radix::Binary), 10u);
}

/**
 * \brief   The to_ methods parse the string.
 **/
TYPED_TEST(StringCommonNumbers, ToMethods)
{
    using S = TypeParam;
    EXPECT_EQ(S(TXT("12345")).to_int32(), 12345);
    EXPECT_EQ(S(TXT("-42")).to_int32(), -42);
    EXPECT_EQ(S(TXT("FF")).to_int32(areg::Radix::Hexadecimal), 255);
    EXPECT_EQ(S(TXT("0x10")).to_uint32(areg::Radix::Automatic), 16u);
    EXPECT_EQ(S(TXT("010")).to_uint32(areg::Radix::Automatic), 8u);
    EXPECT_EQ(S(TXT("4294967295")).to_uint32(), 4294967295u);
    EXPECT_EQ(S(TXT("9223372036854775807")).to_int64(), INT64_C(9223372036854775807));
    EXPECT_EQ(S(TXT("18446744073709551615")).to_uint64(), UINT64_C(18446744073709551615));
    EXPECT_NEAR(S(TXT("3.14")).to_float(), 3.14f, 0.001f);
    EXPECT_NEAR(S(TXT("3.14159265358979")).to_double(), 3.14159265358979, 0.0000001);
    EXPECT_EQ(S().to_int32(), 0);
    EXPECT_EQ(S(TXT("abc")).to_int32(), 0);

    EXPECT_TRUE(S(TXT("true")).to_bool());
    EXPECT_TRUE(S(TXT("yes")).to_bool());
    EXPECT_FALSE(S(TXT("false")).to_bool());
    EXPECT_FALSE(S(TXT("FALSE")).to_bool());
    EXPECT_FALSE(S().to_bool());
}

/**
 * \brief   The static make_ methods parse numbers and booleans and report where parsing stopped.
 **/
TYPED_TEST(StringCommonNumbers, StaticParse)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const C* end{ nullptr };
    EXPECT_EQ(S::make_int32(TXT("123")), 123);
    EXPECT_EQ(S::make_int32(TXT("-456")), -456);
    EXPECT_EQ(S::make_int32(TXT("FF"), areg::Radix::Hexadecimal), 255);
    EXPECT_EQ(S::make_int32(TXT("-123xyz"), areg::Radix::Decimal, &end), -123);
    EXPECT_STREQ(end, TXT("xyz"));
    EXPECT_EQ(S::make_uint32(TXT("ff"), areg::Radix::Hexadecimal), 255u);
    EXPECT_EQ(S::make_uint32(TXT("4294967295")), 4294967295u);
    EXPECT_EQ(S::make_uint32(TXT("12 rest"), areg::Radix::Decimal, &end), 12u);
    EXPECT_STREQ(end, TXT(" rest"));
    EXPECT_EQ(S::make_int64(TXT("-9000000000")), -9000000000LL);
    EXPECT_EQ(S::make_int64(TXT("777"), areg::Radix::Octal, &end), 511);
    EXPECT_STREQ(end, TXT(""));
    EXPECT_EQ(S::make_uint64(TXT("18446744073709551615")), std::numeric_limits<uint64_t>::max());
    EXPECT_EQ(S::make_uint64(TXT("1010b"), areg::Radix::Binary, &end), 10u);
    EXPECT_STREQ(end, TXT("b"));
    EXPECT_FLOAT_EQ(S::make_float(TXT("2.5")), 2.5f);
    EXPECT_FLOAT_EQ(S::make_float(TXT("1.5e1x"), &end), 15.0f);
    EXPECT_STREQ(end, TXT("x"));
    EXPECT_DOUBLE_EQ(S::make_double(TXT("-0.125")), -0.125);
    EXPECT_DOUBLE_EQ(S::make_double(TXT("2.25;"), &end), 2.25);
    EXPECT_STREQ(end, TXT(";"));

    EXPECT_EQ(S::make_int32(nullptr, areg::Radix::Decimal, &end), 0);
    EXPECT_TRUE(end == nullptr);
    EXPECT_EQ(S::make_uint64(TXT(""), areg::Radix::Decimal, &end), 0u);
    EXPECT_DOUBLE_EQ(S::make_double(nullptr), 0.0);

    EXPECT_TRUE(S::make_bool(TXT("true")));
    EXPECT_TRUE(S::make_bool(TXT("TRUE rest"), &end));
    EXPECT_STREQ(end, TXT(" rest"));
    EXPECT_FALSE(S::make_bool(TXT("false;"), &end));
    EXPECT_STREQ(end, TXT(";"));
    EXPECT_FALSE(S::make_bool(TXT("other"), &end));
    EXPECT_STREQ(end, TXT("other"));
}

//////////////////////////////////////////////////////////////////////////
// StringCommonFormat
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   format writes the formatted text.
 **/
TYPED_TEST(StringCommonFormat, Format)
{
    using S = TypeParam;
    S str;
    EXPECT_STREQ(str.format(TXT("Value: %d"), 42).as_string(), TXT("Value: 42"));
    EXPECT_STREQ(str.format(TXT("Pi: %.2f"), 3.14).as_string(), TXT("Pi: 3.14"));
    EXPECT_STREQ(str.format(TXT("Hex: %x"), 255).as_string(), TXT("Hex: ff"));
    EXPECT_STREQ(str.format(TXT("%d-%u-%lld"), -1, 2u, 3LL).as_string(), TXT("-1-2-3"));
    EXPECT_STREQ(str.format(TXT("100%%")).as_string(), TXT("100%"));
    EXPECT_TRUE(str.format(TXT("")).is_empty());
    str = TXT("keep");
    EXPECT_TRUE(str.format(nullptr).is_empty());
}

/**
 * \brief   format writes short and long text in full.
 **/
TYPED_TEST(StringCommonFormat, ShortAndLongText)
{
    using S = TypeParam;
    S str;
    for (int width : { 1, 255, 256, 257, 1023, 1024, 5000, 65535 })
    {
        str.format(TXT("%0*d"), width, 7);
        ASSERT_EQ(str.length(), static_cast<areg::CharCount>(width));
        EXPECT_EQ(str.data().front(), CH(width == 1 ? '7' : '0'));
        EXPECT_EQ(str.data().back(), CH('7'));
    }
}

/**
 * \brief   format_list writes the formatted text of a va_list.
 **/
TYPED_TEST(StringCommonFormat, FormatList)
{
    using S = TypeParam;
    S str;
    EXPECT_STREQ(format_with_list(str, TXT("%d+%d=%d"), 1, 2, 3).as_string(), TXT("1+2=3"));
    format_with_list(str, TXT("%0*d"), 600, 1);
    EXPECT_EQ(str.length(), 600);
    EXPECT_EQ(str.data().back(), CH('1'));
}

/**
 * \brief   format_string and format_string_list never write past the buffer, terminate it, and
 *          report a cut text.
 **/
TYPED_TEST(StringCommonFormat, FormatStringTruncates)
{
    using S = TypeParam;
    using C = CharOf<S>;
    C buffer[8];
    EXPECT_EQ(S::format_string(buffer, 8, TXT("%d"), 1234567), 7);
    EXPECT_STREQ(buffer, TXT("1234567"));

    std::fill(std::begin(buffer), std::end(buffer), CH('#'));
    EXPECT_EQ(S::format_string(buffer, 8, TXT("%d"), 123456789), 7);
    EXPECT_STREQ(buffer, TXT("1234567"));

    EXPECT_EQ(S::format_string(nullptr, 8, TXT("%d"), 1), -1);
    EXPECT_EQ(S::format_string(buffer, 0, TXT("%d"), 1), 0);

    C large[64]{};
    EXPECT_EQ(S::format_string(large, 64, TXT("Hello %d"), 7), 7);
    EXPECT_STREQ(large, TXT("Hello 7"));

    EXPECT_EQ(format_string_with_list<S>(buffer, 8, TXT("%d"), 42), 2);
    EXPECT_STREQ(buffer, TXT("42"));
    EXPECT_EQ(format_string_with_list<S>(buffer, 8, TXT("%d"), 123456789), 7);
    EXPECT_STREQ(buffer, TXT("1234567"));

    int32_t required{ 0 };
    EXPECT_EQ(format_string_with_required<S>(buffer, 8, required, TXT("%d"), 123), 3);
    EXPECT_EQ(required, 3);
    EXPECT_EQ(format_string_with_required<S>(buffer, 8, required, TXT("%d"), 123456789), 7);
    EXPECT_GE(required, 8);
    EXPECT_STREQ(buffer, TXT("1234567"));
}

//////////////////////////////////////////////////////////////////////////
// StringCommonValidate
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   is_empty, length, capacity and space.
 **/
TYPED_TEST(StringCommonValidate, LengthAndSpace)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S empty;
    EXPECT_TRUE(empty.is_empty());
    EXPECT_EQ(empty.length(), 0);
    EXPECT_EQ(empty.space(), static_cast<uint32_t>(sizeof(C)));

    const S str(TXT("Hello"));
    EXPECT_FALSE(str.is_empty());
    EXPECT_EQ(str.length(), 5);
    EXPECT_GE(str.capacity(), str.length());
    EXPECT_EQ(str.space(), static_cast<uint32_t>(6 * sizeof(C)));

    const std::basic_string<C> longText(10000, CH('A'));
    const S large(longText);
    EXPECT_EQ(large.length(), 10000);
    EXPECT_EQ(large.find_first(CH('A')), 0);
    EXPECT_EQ(large.find_last(CH('A')), 9999);
}

/**
 * \brief   validate, is_numeric, is_alphanumeric, is_valid_name and is_name_char.
 **/
TYPED_TEST(StringCommonValidate, Content)
{
    using S = TypeParam;
    using C = CharOf<S>;
    EXPECT_TRUE(S(TXT("aabbcc")).validate(TXT("abc")));
    EXPECT_FALSE(S(TXT("aabbcc")).validate(TXT("ab")));
    EXPECT_FALSE(S().validate(TXT("abc")));
    EXPECT_FALSE(S(TXT("abc")).validate(TXT("")));
    EXPECT_FALSE(S(TXT("abc")).validate(static_cast<const C*>(nullptr)));

    EXPECT_TRUE(S(TXT("12345")).is_numeric());
    EXPECT_FALSE(S(TXT("abc")).is_numeric());
    EXPECT_FALSE(S(TXT("12a")).is_numeric());
    EXPECT_TRUE(S(TXT("-123")).is_numeric(true));
    EXPECT_TRUE(S(TXT("+123")).is_numeric(true));
    EXPECT_FALSE(S(TXT("-123")).is_numeric(false));
    EXPECT_FALSE(S(TXT("-")).is_numeric());
    EXPECT_FALSE(S().is_numeric());

    EXPECT_TRUE(S(TXT("Hello123")).is_alphanumeric());
    EXPECT_FALSE(S(TXT("Hello 123")).is_alphanumeric());
    EXPECT_FALSE(S(TXT("my_var")).is_alphanumeric());

    EXPECT_TRUE(S(TXT("my_var123")).is_valid_name());
    EXPECT_TRUE(S(TXT("_")).is_valid_name());
    EXPECT_FALSE(S(TXT("my var")).is_valid_name());
    EXPECT_FALSE(S(TXT("my-var")).is_valid_name());
    EXPECT_FALSE(S().is_valid_name());

    const S str;
    EXPECT_TRUE(str.is_name_char(CH('_')));
    EXPECT_TRUE(str.is_name_char(CH('z')));
    EXPECT_TRUE(str.is_name_char(CH('0')));
    EXPECT_FALSE(str.is_name_char(CH('.')));
    EXPECT_FALSE(str.is_name_char(CH('\0')));
}

/**
 * \brief   The position checks.
 **/
TYPED_TEST(StringCommonValidate, Positions)
{
    using S = TypeParam;
    const S str(TXT("Hello"));
    EXPECT_TRUE(str.is_valid_position(0));
    EXPECT_TRUE(str.is_valid_position(4));
    EXPECT_FALSE(str.is_valid_position(5));
    EXPECT_FALSE(str.is_valid_position(areg::END_POS));
    EXPECT_FALSE(str.is_valid_position(areg::INVALID_POS));
    EXPECT_TRUE(str.is_invalid_position(100));
    EXPECT_TRUE(str.is_invalid_position(areg::INVALID_POS));
    EXPECT_FALSE(str.is_invalid_position(5));
    EXPECT_TRUE(str.is_last_position(5));
    EXPECT_TRUE(str.is_last_position(areg::END_POS));
    EXPECT_FALSE(str.is_last_position(4));
    EXPECT_TRUE(str.is_first_position(0));
    EXPECT_FALSE(str.is_first_position(1));
    EXPECT_FALSE(S().is_first_position(0));
}

/**
 * \brief   starts_with and ends_with of every phrase type, case-sensitive and not.
 **/
TYPED_TEST(StringCommonValidate, StartsAndEndsWith)
{
    using S = TypeParam;
    using C = CharOf<S>;
    using Std = std::basic_string<C>;
    using View = std::basic_string_view<C>;
    const S str(TXT("Hello World"));
    EXPECT_TRUE(str.starts_with(TXT("Hello")));
    EXPECT_FALSE(str.starts_with(TXT("World")));
    EXPECT_TRUE(str.starts_with(TXT("HELLO"), false));
    EXPECT_FALSE(str.starts_with(TXT("HELLO"), true));
    EXPECT_TRUE(str.starts_with(S(TXT("Hel"))));
    EXPECT_TRUE(str.starts_with(Std(TXT("hel")), false));
    EXPECT_TRUE(str.starts_with(View(TXT("Help")).substr(0, 3)));
    EXPECT_TRUE(str.starts_with(View(TXT("HELP")).substr(0, 3), false));
    EXPECT_TRUE(str.starts_with(TXT("Hexagon"), true, 2));
    EXPECT_TRUE(str.starts_with(TXT("HEXAGON"), false, 2));
    EXPECT_FALSE(str.starts_with(TXT("")));
    EXPECT_FALSE(str.starts_with(static_cast<const C*>(nullptr)));
    EXPECT_FALSE(str.starts_with(TXT("Hello World and more")));
    EXPECT_FALSE(S().starts_with(TXT("a")));

    EXPECT_TRUE(str.ends_with(TXT("World")));
    EXPECT_FALSE(str.ends_with(TXT("Hello")));
    EXPECT_TRUE(str.ends_with(TXT("WORLD"), false));
    EXPECT_FALSE(str.ends_with(TXT("WORLD"), true));
    EXPECT_TRUE(str.ends_with(S(TXT("rld"))));
    EXPECT_TRUE(str.ends_with(Std(TXT("RLD")), false));
    EXPECT_TRUE(str.ends_with(View(TXT("ldx")).substr(0, 2)));
    EXPECT_TRUE(str.ends_with(View(TXT("LDX")).substr(0, 2), false));
    EXPECT_TRUE(str.ends_with(TXT("ldx"), true, 2));
    EXPECT_TRUE(str.ends_with(TXT("LDX"), false, 2));
    EXPECT_TRUE(str.ends_with(CH('d')));
    EXPECT_FALSE(str.ends_with(TXT("")));
    EXPECT_FALSE(str.ends_with(TXT("more Hello World")));
}

/**
 * \brief   read_line walks the lines and skips the empty ones.
 **/
TYPED_TEST(StringCommonValidate, ReadLine)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S str(TXT("Line1\nLine2\r\n\r\nLine3"));
    S line;
    areg::CharPos pos{ str.read_line(line) };
    EXPECT_STREQ(line.as_string(), TXT("Line1"));
    EXPECT_EQ(pos, 6);
    pos = str.read_line(line, pos);
    EXPECT_STREQ(line.as_string(), TXT("Line2"));
    std::basic_string<C> stdLine;
    pos = str.read_line(stdLine, pos);
    EXPECT_EQ(stdLine, expected<C>("Line3"));
    EXPECT_EQ(pos, areg::END_POS);
    EXPECT_EQ(str.read_line(line, pos), areg::END_POS);

    EXPECT_EQ(S(TXT("a\rb\nc")).read_line(line), 4);
    EXPECT_STREQ(line.as_string(), TXT("a\rb"));
    EXPECT_EQ(S(TXT("\n\n")).read_line(line), areg::END_POS);
    EXPECT_EQ(S().read_line(line), areg::END_POS);
}

//////////////////////////////////////////////////////////////////////////
// StringCommonStream
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The string round-trips through a shared buffer.
 **/
TYPED_TEST(StringCommonStream, SharedBufferRoundTrip)
{
    using S = TypeParam;
    const S src(TXT("streamed text"));
    const S empty;
    areg::SharedBuffer stream;
    stream << src;
    stream << empty;
    stream << src;
    stream.move_to_begin();

    S dst(TXT("old"));
    stream >> dst;
    EXPECT_EQ(dst, src);
    stream >> dst;
    EXPECT_TRUE(dst.is_empty());
    const S constructed(static_cast<const areg::InStream&>(stream));
    EXPECT_EQ(constructed, src);
}

/**
 * \brief   The string writes to and reads from the standard streams of its width.
 **/
TYPED_TEST(StringCommonStream, StandardStreams)
{
    using S = TypeParam;
    using C = CharOf<S>;
    std::basic_ostringstream<C> out;
    out << S(TXT("one two"));
    EXPECT_EQ(out.str(), expected<C>("one two"));

    std::basic_istringstream<C> in(std::basic_string<C>(TXT("first second")));
    S word;
    in >> word;
    EXPECT_STREQ(word.as_string(), TXT("first"));
    in >> word;
    EXPECT_STREQ(word.as_string(), TXT("second"));
}

/**
 * \brief   The hash value, the std::hash specialization and the required size.
 **/
TYPED_TEST(StringCommonStream, HashAndRequiredSize)
{
    using S = TypeParam;
    using C = CharOf<S>;
    const S hello(TXT("hello"));
    EXPECT_EQ(static_cast<uint32_t>(hello), areg::crc32_calculate(hello.as_string()));

    const std::hash<S> hasher;
    EXPECT_EQ(hasher(hello), hasher(S(TXT("hello"))));
    EXPECT_NE(hasher(hello), hasher(S(TXT("world"))));
    EXPECT_NE(hasher(hello), hasher(S(TXT("HELLO"))));
    EXPECT_EQ(hasher(S()), hasher(S(TXT(""))));
    EXPECT_NE(hasher(S(TXT(" "))), hasher(S(TXT("\t"))));

    const std::unordered_set<S> set{ S(TXT("a")), S(TXT("b")), S(TXT("a")) };
    EXPECT_EQ(set.size(), 2u);
    EXPECT_EQ(set.count(S(TXT("b"))), 1u);

    const areg::required_size<S> required;
    const uint32_t header{ static_cast<uint32_t>(2 * sizeof(uint32_t)) };
    EXPECT_EQ(required(hello), header + static_cast<uint32_t>(6 * sizeof(C)));
    EXPECT_EQ(required(S()), header + static_cast<uint32_t>(sizeof(C)));
}
