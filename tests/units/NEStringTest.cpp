/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/NEStringTest.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              The unit test for methods in NEString namespace.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "gtest/gtest-param-test.h"

#include "areg/base/NEString.hpp"

/**
 * \brief   Test getStringLength and isEmpty methods of NEString
 **/
TEST(NEStringTest, StringLength)
{
    struct SimpleString
    {
        const char* str;
        uint32_t    len;
    };

    constexpr SimpleString params[]
    {
          {"12345"  , 5}
        , {"654987" , 6}
        , {nullptr  , 0}
        , {""       , 0}
        , {"abcdefg", 7}
        , {"ABCDEFG", 7}
        , {"ABCdefg", 7}
        , {"\n"     , 1}
        , {"\r"     , 1}
        , {"\n\r"   , 2}
        , {"\r\n"   , 2}
    };
    constexpr uint32_t count{ MACRO_ARRAYLEN(params) };

    EXPECT_GT(count, 1u);

    for (uint32_t i = 0; i < count; ++i)
    {
        const SimpleString& entry = params[i];
        EXPECT_EQ(NEString::getStringLength<char>(entry.str), static_cast<NEString::CharCount>(entry.len));
        EXPECT_TRUE(NEString::isEmpty<char>(entry.str) == (entry.len == 0));
    }
}


/**
 * \brief   Test getStringLineLength and isEmpty methods of NEString
 **/
TEST(NEStringTest, StringLineLength)
{
    struct SimpleString
    {
        const char* str;
        uint32_t    len;
    };

    constexpr SimpleString params[]
    {
          {"12345"                  , 5}
        , {"12345\n67890\n"         , 5}
        , {"12345\n\r67890\n\r"     , 5}
        , {"12345\r67890\r"         , 5}
        , {"12345\r\n67890\r\n"     , 5}
        , {"12345\n67890\n"         , 5}
        , {nullptr                  , 0}
        , {""                       , 0}
        , {"\n12345\n67890\n"       , 0}
        , {"\n\r12345\n\r67890\n\r" , 0}
        , {"\r12345\r67890\r"       , 0}
        , {"\r\n12345\r\n67890\r\n" , 0}
        , {"\t12345\n67890\n"       , 6}
        , {"\v12345\n\r67890\n\r"   , 6}
        , {"\b12345\r67890\r"       , 0}
        , {"\f12345\r\n67890\r\n"   , 0}
    };
    constexpr uint32_t count{ MACRO_ARRAYLEN(params) };

    EXPECT_GT(count, 1u);

    for (uint32_t i = 0; i < count; ++i)
    {
        const SimpleString& entry = params[i];
        EXPECT_EQ(NEString::getStringLineLength<char>(entry.str), static_cast<NEString::CharCount>(entry.len));
    }
}

/**
 * \brief   Test methods of checking lower and upper cases in NEString
 **/
TEST(NEStringTest, LowerUpperNumericCase)
{
    constexpr char chNumMin{ '0' };
    constexpr char chNumMax{ '9' };

    constexpr char chUpperMin{ 'A' };
    constexpr char chUpperMax{ 'Z' };
    constexpr char chUpperMinSpec1{ static_cast<char>(0xC0)};
    constexpr char chUpperMaxSpec1{ static_cast<char>(0xD6) };
    constexpr char chUpperMinSpec2{ static_cast<char>(0xD8) };
    constexpr char chUpperMaxSpec2{ static_cast<char>(0xDE) };

    constexpr char chLowerMin{ 'a' };
    constexpr char chLowerMax{ 'z' };
    constexpr char chLowerMinSpec1{ static_cast<char>(0xE0) };
    constexpr char chLowerMaxSpec1{ static_cast<char>(0xF6) };
    constexpr char chLowerMinSpec2{ static_cast<char>(0xF8) };
    constexpr char chLowerMaxSpec2{ static_cast<char>(0xFF) };

    for (uint32_t i = 0; i <= 0xFFu; ++i)
    {
        char ch    = static_cast<char>(i);
        char lower = NEString::makeLower<char>(ch);
        char upper = NEString::makeUpper<char>(ch);

        if ((ch >= chNumMin) && (ch <= chNumMax))
        {
            // is numeric
            EXPECT_TRUE(NEString::isNumeric<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_EQ(lower, upper);
            EXPECT_EQ(ch, lower);
            EXPECT_FALSE(NEString::isLetter<char>(ch));
        }
        else if ((ch >= chUpperMin) && (ch <= chUpperMax))
        {
            // is upper
            EXPECT_TRUE(NEString::isUpper<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chUpperMinSpec1) && (ch <= chUpperMaxSpec1))
        {
            // is upper special 1
            EXPECT_TRUE(NEString::isUpper<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chUpperMinSpec2) && (ch <= chUpperMaxSpec2))
        {
            // is upper special 2
            EXPECT_TRUE(NEString::isUpper<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chLowerMin) && (ch <= chLowerMax))
        {
            // is lower
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chLowerMinSpec1) && (ch <= chLowerMaxSpec1))
        {
            // is lower special 1
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chLowerMinSpec2) && (ch <= chLowerMaxSpec2))
        {
            // is lower special 2
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch == static_cast<char>(0x8A)) ||
                 (ch == static_cast<char>(0x8C)) ||
                 (ch == static_cast<char>(0x8E)) ||
                 (ch == static_cast<char>(0x9F)) )
        {
            // is upper, control --> 138 (Š), 140 (Œ), 142 (Ž), 159 (Ÿ)
            EXPECT_TRUE(NEString::isUpper<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
            EXPECT_TRUE(NEString::isControl<char>(ch));
        }
        else if ((ch == static_cast<char>(0x9A)) ||
                 (ch == static_cast<char>(0x9C)) ||
                 (ch == static_cast<char>(0x9E)) )
        {
            // is lower, control --> 154 (š), 156 (œ), 158 (ž)
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
            EXPECT_TRUE(NEString::isControl<char>(ch));
        }
        else if (ch == static_cast<char>(0xDF))
        {
            // is German letter 'ss' --> 223 (ß), has no upper case
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_EQ(ch, upper);
        }
        else
        {
            // is symbol or special char, there is no upper or lower case
            EXPECT_FALSE(NEString::isNumeric<char>(ch))     << "Check char: \'" << static_cast<wchar_t>(ch) << "\'";
            EXPECT_FALSE(NEString::isLetter<char>(ch))      << "Check char: \'" << static_cast<wchar_t>(ch) << "\'";
            EXPECT_FALSE(NEString::isAlphanumeric<char>(ch))<< "Check char: \'" << static_cast<wchar_t>(ch) << "\'";
            EXPECT_TRUE(NEString::isSymbol<char>(ch)        || 
                        NEString::isWhitespace<char>(ch)    ||
                        NEString::isDelimited<char>(ch)     ||
                        NEString::isEndOfLine<char>(ch)     ||
                        NEString::isEndOfString<char>(ch)   ||
                        NEString::isControl<char>(ch));
            EXPECT_EQ(lower, upper);
            EXPECT_EQ(ch, lower);
        }
    }
}

struct CompareStringParams
{
    std::string_view left;
    std::string_view right;
    int              count;
    bool             sensitive;
    int              resSensitive;
    int              resIgnore;
};

struct StringTestCompare : public ::testing::TestWithParam<CompareStringParams>
{
    CompareStringParams params;
};

CompareStringParams StringTestCompare_params[]
{
      { {"12345"}, {"12345"}, -2, false, 0, 0 }
    , { {"12345"}, {"12345"}, -2, true , 0, 0 }
};

INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestCompare, ::testing::ValuesIn<CompareStringParams>(StringTestCompare_params));

TEST_P(StringTestCompare, CompareStrings)
{
    const CompareStringParams& param = GetParam();
    const char* left{ param.left.data() };
    const char* right{ param.right.data() };

    EXPECT_EQ(NEString::compare<char>(left, right), static_cast<NEMath::eCompare>(param.resSensitive));
    EXPECT_EQ(NEString::compareFastIgnoreCase<char>(left, right), static_cast<NEMath::eCompare>(param.resIgnore));
    EXPECT_EQ(NEString::compareFast<char>(left, right), static_cast<NEMath::eCompare>(param.resSensitive));
    EXPECT_EQ(NEString::compareFast<char>(left, right, param.count), static_cast<NEMath::eCompare>(param.resSensitive));
    EXPECT_TRUE(NEString::compareStrings(left, right, param.count, param.sensitive) == static_cast<NEMath::eCompare>(param.resSensitive));
    EXPECT_TRUE(NEString::compareIgnoreCase(left, right) == static_cast<NEMath::eCompare>(param.resIgnore));
}

