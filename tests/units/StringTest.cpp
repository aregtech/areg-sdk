/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech. All rights reserved.
 * \file        units/StringTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Apoorv Darshan
 * \brief       Areg Platform, Areg framework unit test file.
 *              The unit test of what differs between String and WideString: the
 *              conversion between the widths, the string formats and the sizes of
 *              the standard strings. The common API is tested in StringCommonTest.cpp.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"

#include <string>
#include <string_view>

using areg::String;
using areg::WideString;

/**
 * \brief   Conversion between widths maps a narrow character as an unsigned value, and
 *          equality gives the same answer in both directions.
 **/
TEST(StringTest, ConversionBetweenWidths)
{
    const String narrow("A\xE9z");
    const WideString wide(L"A\x00E9z");

    EXPECT_EQ(WideString(narrow).data(), std::wstring(L"A\x00E9z"));
    EXPECT_EQ(String(wide).data(), std::string("A\xE9z"));
    EXPECT_TRUE(narrow == wide);
    EXPECT_TRUE(wide == narrow);
    EXPECT_TRUE(narrow == L"A\x00E9z");
    EXPECT_TRUE(wide == "A\xE9z");
    EXPECT_TRUE(narrow == std::wstring(L"A\x00E9z"));
    EXPECT_TRUE(wide == std::string("A\xE9z"));

    const WideString notLatin(L"\x0141");
    EXPECT_TRUE(String("A") != notLatin);
    EXPECT_TRUE(notLatin != String("A"));
    EXPECT_TRUE(notLatin != "A");
    EXPECT_FALSE(String("A") == L"\x0141");
    EXPECT_FALSE(String("AB") == WideString(L"AC"));
    EXPECT_FALSE(WideString(L"AB") == "AC");
    EXPECT_TRUE(String("AB") != L"AC");
}

/**
 * \brief   A wide character beyond one byte is narrowed by truncation.
 **/
TEST(StringTest, NarrowingTruncates)
{
    EXPECT_EQ(String(WideString(L"\x0141")).data(), std::string("\x41"));
    EXPECT_EQ(String(L"\x01FF").data(), std::string("\xFF"));

    String str;
    str = L'\x0142';
    EXPECT_EQ(str.data(), std::string("\x42"));
}

/**
 * \brief   The assignments and appends of the other width, mixing both directions.
 **/
TEST(StringTest, MixedWidthOperations)
{
    const WideString wide(L"A\x00E9z");
    String str;
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

    WideString wstr("narrow");
    EXPECT_EQ(wstr.data(), std::wstring(L"narrow"));
    wstr = 'n';
    EXPECT_EQ(wstr.data(), std::wstring(L"n"));
    wstr += "\xE9";
    EXPECT_EQ(wstr.data(), std::wstring(L"n\x00E9"));
    EXPECT_EQ((String("a") + L"b").data(), std::string("ab"));
    EXPECT_EQ((L"a" + String("b")).data(), std::string("ab"));
    EXPECT_EQ((WideString(L"a") + "b").data(), std::wstring(L"ab"));
    EXPECT_EQ(("a" + WideString(L"b")).data(), std::wstring(L"ab"));
    EXPECT_TRUE(String(static_cast<const wchar_t*>(nullptr)).is_empty());
    EXPECT_TRUE(String("") == static_cast<const wchar_t*>(nullptr));
}

/**
 * \brief   The shared empty strings of both types are separate objects.
 **/
TEST(StringTest, EmptyStringsOfBothTypes)
{
    EXPECT_TRUE(String::empty_string().is_empty());
    EXPECT_TRUE(WideString::empty_string().is_empty());
    EXPECT_TRUE(String::empty_string() == WideString::empty_string());
    EXPECT_NE(static_cast<const void*>(&String::empty_string()), static_cast<const void*>(&WideString::empty_string()));
}

/**
 * \brief   format writes a narrow and a wide string argument into a narrow string.
 **/
TEST(StringTest, FormatNarrowString)
{
    String str;
    str.format("Hello, %s!", "World");
    EXPECT_STREQ(str.as_string(), "Hello, World!");
    str.format("%s is %d years old", "Alice", 30);
    EXPECT_STREQ(str.as_string(), "Alice is 30 years old");
    str.format("[%ls]", L"wide");
    EXPECT_STREQ(str.as_string(), "[wide]");
    str.format("%c%c", 'o', 'k');
    EXPECT_STREQ(str.as_string(), "ok");

    char buf[64]{};
    EXPECT_EQ(String::format_string(buf, 64, "Hello %s", "World"), 11);
    EXPECT_STREQ(buf, "Hello World");

    str.format("I want to try some extra [ %s ], [ %s ], [ %s ], [ %s ], [ %s ], [ %s ], [ %s ], [ %s ], [ %s ] string"
                , "extra"
                , "very extra"
                , "very-very extra"
                , "very-very-very extra"
                , "very-very-very-very extra"
                , "very-very-very-very-very extra"
                , "very-very-very-very-very-very extra"
                , "very-very-very-very-very-very-very extra"
                , "very-very-very-very-very-very-very-very extra"
    );
    EXPECT_GT(str.length(), 256);
    EXPECT_TRUE(str.starts_with("I want to try some extra [ extra ]"));
    EXPECT_TRUE(str.ends_with("[ very-very-very-very-very-very-very-very extra ] string"));
}

/**
 * \brief   format writes a wide string argument into a wide string.
 **/
TEST(StringTest, FormatWideString)
{
    WideString str;
    str.format(L"Hello, %ls!", L"World");
    EXPECT_STREQ(str.as_string(), L"Hello, World!");
    str.format(L"%ls is %d years old", L"Alice", 30);
    EXPECT_STREQ(str.as_string(), L"Alice is 30 years old");
    str.format(L"[%ls]", L"\x00E9\x0141");
    EXPECT_STREQ(str.as_string(), L"[\x00E9\x0141]");

    wchar_t buf[64]{};
    EXPECT_EQ(WideString::format_string(buf, 64, L"Hello %ls", L"World"), 11);
    EXPECT_STREQ(buf, L"Hello World");

    const std::wstring part(300, L'w');
    str.format(L"<%ls>", part.c_str());
    EXPECT_EQ(str.length(), 302);
    EXPECT_EQ(str.data(), L"<" + part + L">");
}

/**
 * \brief   format writes a narrow text of any length in full, and a wide text in full up to
 *          the limit of the platform.
 **/
TEST(StringTest, FormatLongText)
{
    String narrow;
    narrow.format("%0*d", 70000, 7);
    EXPECT_EQ(narrow.length(), 70000);
    EXPECT_EQ(narrow.data().back(), '7');

    WideString wide;
    wide.format(L"%0*d", 70000, 7);
#if defined(_WIN32)
    EXPECT_EQ(wide.length(), 70000);
    EXPECT_EQ(wide.data().back(), L'7');
#else
    EXPECT_EQ(wide.length(), 65535);
    EXPECT_EQ(wide.data().front(), L'0');
#endif
}

/**
 * \brief   required_size of the standard narrow strings.
 **/
TEST(StringTest, RequiredSizeStdNarrow)
{
    const areg::required_size<std::string_view> requiredView;
    EXPECT_EQ(requiredView(std::string_view("hello")), 14u);
    EXPECT_EQ(requiredView(std::string_view("world !")), 16u);
    EXPECT_EQ(requiredView(std::string_view("")), 9u);
    EXPECT_EQ(requiredView(std::string_view()), 9u);

    const areg::required_size<std::string> requiredStd;
    EXPECT_EQ(requiredStd(std::string("hello")), 14u);
    EXPECT_EQ(requiredStd(std::string("world !")), 16u);
    EXPECT_EQ(requiredStd(std::string()), 9u);

    const areg::required_size<String> required;
    EXPECT_EQ(required(String("hello")), 14u);
    EXPECT_EQ(required(String()), 9u);
}

/**
 * \brief   required_size of the standard wide strings.
 **/
TEST(StringTest, RequiredSizeStdWide)
{
    constexpr uint32_t header{ 2 * sizeof(uint32_t) };
    constexpr uint32_t ch{ sizeof(wchar_t) };

    const areg::required_size<std::wstring_view> requiredView;
    EXPECT_EQ(requiredView(std::wstring_view(L"hello")), header + 6 * ch);
    EXPECT_EQ(requiredView(std::wstring_view()), header + ch);

    const areg::required_size<std::wstring> requiredStd;
    EXPECT_EQ(requiredStd(std::wstring(L"world !")), header + 8 * ch);
    EXPECT_EQ(requiredStd(std::wstring()), header + ch);

    const areg::required_size<WideString> required;
    EXPECT_EQ(required(WideString(L"hello")), header + 6 * ch);
    EXPECT_EQ(required(WideString()), header + ch);
}
