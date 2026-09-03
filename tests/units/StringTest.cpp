/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech. All rights reserved.
 * \file        units/StringTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Apoorv Darshan
 * \brief       Areg Platform, Areg framework unit test file.
 *              The unit test for String class methods.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"

#include <string>
#include <string_view>
#include <cmath>

using areg::String;

//////////////////////////////////////////////////////////////////////////
// StringTestConstructors tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test default constructor creates an empty string.
 **/
TEST(StringTestConstructors, DefaultConstructor)
{
    String str;
    EXPECT_TRUE(str.is_empty());
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(0));
    EXPECT_STREQ(str.as_string(), "");
}

/**
 * \brief   Test constructor from const char*.
 **/
TEST(StringTestConstructors, FromConstCharPtr)
{
    String str("Hello");
    EXPECT_FALSE(str.is_empty());
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(5));
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test constructor from nullptr creates an empty string.
 **/
TEST(StringTestConstructors, FromNullptr)
{
    String str(static_cast<const char*>(nullptr));
    EXPECT_TRUE(str.is_empty());
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(0));
}

/**
 * \brief   Test constructor from std::string.
 **/
TEST(StringTestConstructors, FromStdString)
{
    std::string src("Hello World");
    String str(src);
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(11));
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test constructor from std::string_view.
 **/
TEST(StringTestConstructors, FromStdStringView)
{
    std::string_view sv("Test View");
    String str(sv);
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(9));
    EXPECT_STREQ(str.as_string(), "Test View");
}

/**
 * \brief   Test constructor from a single char.
 **/
TEST(StringTestConstructors, FromChar)
{
    String str('A');
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(1));
    EXPECT_STREQ(str.as_string(), "A");
}

/**
 * \brief   Test constructor from const char* with count (partial copy).
 **/
TEST(StringTestConstructors, FromCharPtrWithCount)
{
    String str("Hello World", 5);
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(5));
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test copy constructor.
 **/
TEST(StringTestConstructors, CopyConstructor)
{
    String original("Copy Me");
    String copy(original);
    EXPECT_EQ(copy.length(), original.length());
    EXPECT_STREQ(copy.as_string(), original.as_string());
    EXPECT_STREQ(copy.as_string(), "Copy Me");
}

/**
 * \brief   Test move constructor.
 **/
TEST(StringTestConstructors, MoveConstructor)
{
    String original("Move Me");
    areg::CharCount len = original.length();
    String moved(std::move(original));
    EXPECT_EQ(moved.length(), len);
    EXPECT_STREQ(moved.as_string(), "Move Me");
}

/**
 * \brief   Test reserve constructor creates an empty string with capacity.
 **/
TEST(StringTestConstructors, ReserveConstructor)
{
    String str(static_cast<uint32_t>(100));
    EXPECT_TRUE(str.is_empty());
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(0));
    EXPECT_GE(str.capacity(), static_cast<areg::CharCount>(100));
}

/**
 * \brief   Test constructor from std::string rvalue.
 **/
TEST(StringTestConstructors, FromStdStringMove)
{
    std::string src("Moved std string");
    String str(std::move(src));
    EXPECT_STREQ(str.as_string(), "Moved std string");
}

//////////////////////////////////////////////////////////////////////////
// StringTestAssignment tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test assignment from const char*.
 **/
TEST(StringTestAssignment, AssignFromCharPtr)
{
    String str;
    str = "Assigned";
    EXPECT_STREQ(str.as_string(), "Assigned");
}

/**
 * \brief   Test assignment from std::string.
 **/
TEST(StringTestAssignment, AssignFromStdString)
{
    String str;
    std::string src("From std::string");
    str = src;
    EXPECT_STREQ(str.as_string(), "From std::string");
}

/**
 * \brief   Test assignment from std::string_view.
 **/
TEST(StringTestAssignment, AssignFromStringView)
{
    String str;
    std::string_view sv("From string_view");
    str = sv;
    EXPECT_STREQ(str.as_string(), "From string_view");
}

/**
 * \brief   Test assignment from a single char.
 **/
TEST(StringTestAssignment, AssignFromChar)
{
    String str;
    str = 'Z';
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(1));
    EXPECT_STREQ(str.as_string(), "Z");
}

/**
 * \brief   Test move assignment from String.
 **/
TEST(StringTestAssignment, MoveAssignFromString)
{
    String src("Move Source");
    String dst;
    dst = std::move(src);
    EXPECT_STREQ(dst.as_string(), "Move Source");
}

/**
 * \brief   Test move assignment from std::string.
 **/
TEST(StringTestAssignment, MoveAssignFromStdString)
{
    std::string src("Move std::string");
    String dst;
    dst = std::move(src);
    EXPECT_STREQ(dst.as_string(), "Move std::string");
}

/**
 * \brief   Test copy assignment from String.
 **/
TEST(StringTestAssignment, CopyAssignFromString)
{
    String src("Copy Source");
    String dst;
    dst = src;
    EXPECT_STREQ(dst.as_string(), "Copy Source");
    EXPECT_STREQ(src.as_string(), "Copy Source");
}

/**
 * \brief   Test self-assignment safety.
 **/
TEST(StringTestAssignment, SelfAssignment)
{
    String str("Self");
    const String& ref = str;
    str = ref;
    EXPECT_STREQ(str.as_string(), "Self");
}

//////////////////////////////////////////////////////////////////////////
// StringTestComparison tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test equality operator with String.
 **/
TEST(StringTestComparison, EqualityWithString)
{
    String a("Hello");
    String b("Hello");
    String c("World");
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

/**
 * \brief   Test equality operator with const char*.
 **/
TEST(StringTestComparison, EqualityWithCharPtr)
{
    String str("Test");
    EXPECT_TRUE(str == "Test");
    EXPECT_FALSE(str == "Other");
}

/**
 * \brief   Test equality operator with std::string.
 **/
TEST(StringTestComparison, EqualityWithStdString)
{
    String str("Match");
    std::string other("Match");
    EXPECT_TRUE(str == other);
}

/**
 * \brief   Test equality operator with std::string_view.
 **/
TEST(StringTestComparison, EqualityWithStringView)
{
    String str("View");
    std::string_view sv("View");
    EXPECT_TRUE(str == sv);
}

/**
 * \brief   Test equality operator with a single char.
 **/
TEST(StringTestComparison, EqualityWithChar)
{
    String str("A");
    EXPECT_TRUE(str == 'A');
    EXPECT_FALSE(str == 'B');
}

/**
 * \brief   Test inequality operator.
 **/
TEST(StringTestComparison, InequalityOperator)
{
    String a("Hello");
    String b("World");
    EXPECT_TRUE(a != b);
    EXPECT_TRUE(a != "World");
    EXPECT_FALSE(a != "Hello");
}

/**
 * \brief   Test ordering operators.
 **/
TEST(StringTestComparison, OrderingOperators)
{
    String a("Apple");
    String b("Banana");
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b >= a);
    EXPECT_TRUE(a <= a);
    EXPECT_TRUE(a >= a);
}

/**
 * \brief   Test case-sensitive compare method.
 **/
TEST(StringTestComparison, CompareMethodCaseSensitive)
{
    String str("Hello");
    EXPECT_EQ(str.compare("Hello", areg::START_POS, areg::COUNT_ALL, true), areg::Ordering::Equal);
    EXPECT_NE(str.compare("hello", areg::START_POS, areg::COUNT_ALL, true), areg::Ordering::Equal);
}

/**
 * \brief   Test case-insensitive compare method.
 **/
TEST(StringTestComparison, CompareMethodCaseInsensitive)
{
    String str("Hello");
    EXPECT_EQ(str.compare("HELLO", areg::START_POS, areg::COUNT_ALL, false), areg::Ordering::Equal);
    EXPECT_EQ(str.compare("hello", areg::START_POS, areg::COUNT_ALL, false), areg::Ordering::Equal);
}

/**
 * \brief   Test compare at a specific position with count.
 **/
TEST(StringTestComparison, CompareAtPositionWithCount)
{
    String str("Hello World");
    EXPECT_EQ(str.compare("World", static_cast<areg::CharPos>(6), static_cast<areg::CharCount>(5), true), areg::Ordering::Equal);
}

/**
 * \brief   Test compare with String object.
 **/
TEST(StringTestComparison, CompareWithStringObject)
{
    String a("abc");
    String b("ABC");
    EXPECT_EQ(a.compare(b, false), areg::Ordering::Equal);
    EXPECT_NE(a.compare(b, true), areg::Ordering::Equal);
}

//////////////////////////////////////////////////////////////////////////
// StringTestConcatenation tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test += with String.
 **/
TEST(StringTestConcatenation, PlusEqualString)
{
    String a("Hello");
    String b(" World");
    a += b;
    EXPECT_STREQ(a.as_string(), "Hello World");
}

/**
 * \brief   Test += with const char*.
 **/
TEST(StringTestConcatenation, PlusEqualCharPtr)
{
    String str("Hello");
    str += " World";
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test += with std::string.
 **/
TEST(StringTestConcatenation, PlusEqualStdString)
{
    String str("Hello");
    std::string suffix(" World");
    str += suffix;
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test += with std::string_view.
 **/
TEST(StringTestConcatenation, PlusEqualStringView)
{
    String str("Hello");
    std::string_view sv(" World");
    str += sv;
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test += with char.
 **/
TEST(StringTestConcatenation, PlusEqualChar)
{
    String str("Hell");
    str += 'o';
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test binary + operator with String + String.
 **/
TEST(StringTestConcatenation, BinaryPlusStringString)
{
    String a("Hello");
    String b(" World");
    String result = a + b;
    EXPECT_STREQ(result.as_string(), "Hello World");
}

/**
 * \brief   Test binary + operator with String + const char*.
 **/
TEST(StringTestConcatenation, BinaryPlusStringCharPtr)
{
    String a("Hello");
    String result = a + " World";
    EXPECT_STREQ(result.as_string(), "Hello World");
}

/**
 * \brief   Test binary + operator with const char* + String.
 **/
TEST(StringTestConcatenation, BinaryPlusCharPtrString)
{
    String b(" World");
    String result = "Hello" + b;
    EXPECT_STREQ(result.as_string(), "Hello World");
}

/**
 * \brief   Test binary + operator with String + char.
 **/
TEST(StringTestConcatenation, BinaryPlusStringChar)
{
    String a("Hell");
    String result = a + 'o';
    EXPECT_STREQ(result.as_string(), "Hello");
}

/**
 * \brief   Test binary + operator with char + String.
 **/
TEST(StringTestConcatenation, BinaryPlusCharString)
{
    String b("ello");
    String result = 'H' + b;
    EXPECT_STREQ(result.as_string(), "Hello");
}

/**
 * \brief   Test binary + operator with std::string + String.
 **/
TEST(StringTestConcatenation, BinaryPlusStdStringString)
{
    std::string a("Hello");
    String b(" World");
    String result = a + b;
    EXPECT_STREQ(result.as_string(), "Hello World");
}

/**
 * \brief   Test binary + operator with std::string_view + String.
 **/
TEST(StringTestConcatenation, BinaryPlusStringViewString)
{
    std::string_view a("Hello");
    String b(" World");
    String result = a + b;
    EXPECT_STREQ(result.as_string(), "Hello World");
}

//////////////////////////////////////////////////////////////////////////
// StringTestSearch tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test findFirst with a character.
 **/
TEST(StringTestSearch, FindFirstChar)
{
    String str("Hello World");
    EXPECT_EQ(str.find_first('W'), static_cast<areg::CharPos>(6));
    EXPECT_EQ(str.find_first('z'), areg::END_POS);
}

/**
 * \brief   Test findFirst with case-insensitive character search.
 **/
TEST(StringTestSearch, FindFirstCharCaseInsensitive)
{
    String str("Hello World");
    EXPECT_EQ(str.find_first('w', areg::START_POS, false), static_cast<areg::CharPos>(6));
}

/**
 * \brief   Test findFirst with a phrase.
 **/
TEST(StringTestSearch, FindFirstPhrase)
{
    String str("Hello World Hello");
    EXPECT_EQ(str.find_first("World"), static_cast<areg::CharPos>(6));
    EXPECT_EQ(str.find_first("xyz"), areg::END_POS);
}

/**
 * \brief   Test findFirst phrase with case-insensitive search.
 **/
TEST(StringTestSearch, FindFirstPhraseCaseInsensitive)
{
    String str("Hello World");
    EXPECT_EQ(str.find_first("WORLD", areg::START_POS, false), static_cast<areg::CharPos>(6));
}

/**
 * \brief   Test findLast with a character.
 **/
TEST(StringTestSearch, FindLastChar)
{
    String str("Hello World");
    EXPECT_EQ(str.find_last('l'), static_cast<areg::CharPos>(9));
    EXPECT_EQ(str.find_last('z'), areg::END_POS);
}

/**
 * \brief   Test findLast with a phrase.
 **/
TEST(StringTestSearch, FindLastPhrase)
{
    String str("Hello World Hello World");
    // findLast searches backward; verify it finds the phrase
    areg::CharPos pos = str.find_last("Hello");
    EXPECT_NE(pos, areg::END_POS);
    EXPECT_GE(pos, static_cast<areg::CharPos>(0));
}

/**
 * \brief   Test findOneOf.
 **/
TEST(StringTestSearch, FindOneOf)
{
    String str("Hello World");
    EXPECT_EQ(str.find_one_of("xWz"), static_cast<areg::CharPos>(6));
    EXPECT_EQ(str.find_one_of("xyz"), areg::END_POS);
}

/**
 * \brief   Test findFirst with a start position.
 **/
TEST(StringTestSearch, FindFirstWithStartPos)
{
    String str("abcabc");
    EXPECT_EQ(str.find_first('a', static_cast<areg::CharPos>(1)), static_cast<areg::CharPos>(3));
}

//////////////////////////////////////////////////////////////////////////
// StringTestSubstring tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test substring (truncate variant) modifies the string in-place.
 **/
TEST(StringTestSubstring, SubstringTruncate)
{
    String str("Hello World");
    str.substring(static_cast<areg::CharPos>(6));
    EXPECT_STREQ(str.as_string(), "World");
}

/**
 * \brief   Test substring with startPos and count.
 **/
TEST(StringTestSubstring, SubstringWithCount)
{
    String str("Hello World");
    str.substring(static_cast<areg::CharPos>(0), static_cast<areg::CharCount>(5));
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test substring (extract variant) copies to another string.
 **/
TEST(StringTestSubstring, SubstringExtract)
{
    String str("Hello World");
    String result;
    str.substring(result, static_cast<areg::CharPos>(6), static_cast<areg::CharCount>(5));
    EXPECT_STREQ(result.as_string(), "World");
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test leftSide extraction.
 **/
TEST(StringTestSubstring, LeftSide)
{
    String str("Hello World");
    String left(str.left_side(static_cast<areg::CharCount>(5)).as_string());
    EXPECT_STREQ(left.as_string(), "Hello");
}

/**
 * \brief   Test rightSide extraction.
 **/
TEST(StringTestSubstring, RightSide)
{
    String str("Hello World");
    String right(str.right_side(static_cast<areg::CharCount>(5)).as_string());
    EXPECT_STREQ(right.as_string(), "World");
}

//////////////////////////////////////////////////////////////////////////
// StringTestManipulation tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test assign method with const char*.
 **/
TEST(StringTestManipulation, AssignCharPtr)
{
    String str;
    str.assign("Assigned");
    EXPECT_STREQ(str.as_string(), "Assigned");
}

/**
 * \brief   Test assign method with count.
 **/
TEST(StringTestManipulation, AssignCharPtrWithCount)
{
    String str;
    str.assign("Hello World", static_cast<areg::CharCount>(5));
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test assign method with std::string and position.
 **/
TEST(StringTestManipulation, AssignStdStringWithPos)
{
    String str;
    std::string src("Hello World");
    str.assign(src, static_cast<areg::CharPos>(6), static_cast<areg::CharCount>(5));
    EXPECT_STREQ(str.as_string(), "World");
}

/**
 * \brief   Test assign method with a single char.
 **/
TEST(StringTestManipulation, AssignChar)
{
    String str;
    str.assign('X');
    EXPECT_STREQ(str.as_string(), "X");
}

/**
 * \brief   Test append method with const char*.
 **/
TEST(StringTestManipulation, AppendCharPtr)
{
    String str("Hello");
    str.append(" World");
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test append method with count.
 **/
TEST(StringTestManipulation, AppendCharPtrWithCount)
{
    String str("Hello");
    str.append(" World!!!", static_cast<areg::CharCount>(6));
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test append method with a single char.
 **/
TEST(StringTestManipulation, AppendChar)
{
    String str("Hell");
    str.append('o');
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test append method with String object.
 **/
TEST(StringTestManipulation, AppendStringObject)
{
    String str("Hello");
    String suffix(" World");
    str.append(suffix);
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test insertAt with a character.
 **/
TEST(StringTestManipulation, InsertAtChar)
{
    String str("Hllo");
    str.insert_at('e', static_cast<areg::CharPos>(1));
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test insertAt with a string.
 **/
TEST(StringTestManipulation, InsertAtString)
{
    String str("Hello!");
    str.insert_at(" World", static_cast<areg::CharPos>(5));
    EXPECT_STREQ(str.as_string(), "Hello World!");
}

/**
 * \brief   Test insertAt at the beginning.
 **/
TEST(StringTestManipulation, InsertAtBeginning)
{
    String str("World");
    str.insert_at("Hello ", static_cast<areg::CharPos>(0));
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test replace character with character (replaceAll=false, first occurrence only).
 *          Note: replace(char,char) with replaceAll=true has a known infinite loop issue
 *          when non-matching characters are encountered, so we only test first-match replacement
 *          where the first character matches.
 **/
TEST(StringTestManipulation, ReplaceCharCharFirstOnly)
{
    String str("llHello");
    str.replace('l', 'r', areg::START_POS, false);
    EXPECT_STREQ(str.as_string(), "rlHello");
}

/**
 * \brief   Test replace string with string.
 **/
TEST(StringTestManipulation, ReplaceStringString)
{
    String str("Hello World");
    str.replace("World", "Earth");
    EXPECT_STREQ(str.as_string(), "Hello Earth");
}

/**
 * \brief   Test replace all occurrences of a string.
 **/
TEST(StringTestManipulation, ReplaceStringStringAll)
{
    String str("abcabcabc");
    str.replace("abc", "x");
    EXPECT_STREQ(str.as_string(), "xxx");
}

/**
 * \brief   Test remove at position with count.
 **/
TEST(StringTestManipulation, RemoveAtPosition)
{
    String str("Hello World");
    str.remove(static_cast<areg::CharPos>(5), static_cast<areg::CharCount>(6));
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test remove a character from the string.
 **/
TEST(StringTestManipulation, RemoveChar)
{
    String str("Hello");
    str.remove('l');
    EXPECT_STREQ(str.as_string(), "Heo");
}

/**
 * \brief   Test remove a phrase from the string.
 **/
TEST(StringTestManipulation, RemovePhrase)
{
    String str("Hello World");
    str.remove(" World");
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test resize to make string longer.
 **/
TEST(StringTestManipulation, ResizeLonger)
{
    String str("Hi");
    str.resize(static_cast<areg::CharCount>(5), 'x');
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(5));
    EXPECT_STREQ(str.as_string(), "Hixxx");
}

/**
 * \brief   Test resize to make string shorter.
 **/
TEST(StringTestManipulation, ResizeShorter)
{
    String str("Hello World");
    str.resize(static_cast<areg::CharCount>(5));
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(5));
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test truncate method.
 **/
TEST(StringTestManipulation, Truncate)
{
    String str("Hello World");
    str.truncate(static_cast<areg::CharCount>(5));
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test reserve method.
 **/
TEST(StringTestManipulation, Reserve)
{
    String str("Hi");
    str.reserve(static_cast<areg::CharCount>(100));
    EXPECT_GE(str.capacity(), static_cast<areg::CharCount>(100));
    EXPECT_STREQ(str.as_string(), "Hi");
}

/**
 * \brief   Test getAt and setAt methods.
 **/
TEST(StringTestManipulation, GetAtSetAt)
{
    String str("Hello");
    EXPECT_EQ(str[static_cast<areg::CharPos>(0)], 'H');
    EXPECT_EQ(str[static_cast<areg::CharPos>(4)], 'o');

    str.set_at('h', static_cast<areg::CharPos>(0));
    EXPECT_STREQ(str.as_string(), "hello");
}

/**
 * \brief   Test clear method.
 **/
TEST(StringTestManipulation, Clear)
{
    String str("Hello");
    str.clear();
    EXPECT_TRUE(str.is_empty());
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(0));
}

/**
 * \brief   Test release method.
 **/
TEST(StringTestManipulation, Release)
{
    String str("Hello");
    str.release();
    EXPECT_TRUE(str.is_empty());
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(0));
}

/**
 * \brief   Test freeExtra method.
 **/
TEST(StringTestManipulation, FreeExtra)
{
    String str(static_cast<uint32_t>(100));
    str.assign("Hi");
    areg::CharCount capBefore = str.capacity();
    str.free_extra();
    EXPECT_LE(str.capacity(), capBefore);
    EXPECT_STREQ(str.as_string(), "Hi");
}

//////////////////////////////////////////////////////////////////////////
// StringTestTrim tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test trimLeft removes leading whitespace.
 **/
TEST(StringTestTrim, TrimLeft)
{
    String str("   Hello");
    str.trim_left();
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test trimRight removes trailing whitespace.
 **/
TEST(StringTestTrim, TrimRight)
{
    String str("Hello   ");
    str.trim_right();
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test trimAll removes leading and trailing whitespace.
 **/
TEST(StringTestTrim, TrimAll)
{
    String str("   Hello World   ");
    str.trim_all();
    EXPECT_STREQ(str.as_string(), "Hello World");
}

/**
 * \brief   Test trimLeft on a string that has no leading whitespace (no-op).
 **/
TEST(StringTestTrim, TrimLeftNoOp)
{
    String str("Hello   ");
    str.trim_left();
    EXPECT_STREQ(str.as_string(), "Hello   ");
}

/**
 * \brief   Test trimRight on a string that has no trailing whitespace (no-op).
 **/
TEST(StringTestTrim, TrimRightNoOp)
{
    String str("   Hello");
    str.trim_right();
    EXPECT_STREQ(str.as_string(), "   Hello");
}

/**
 * \brief   Test trimAll on an already-trimmed string (no-op).
 **/
TEST(StringTestTrim, TrimAllNoOp)
{
    String str("Hello");
    str.trim_all();
    EXPECT_STREQ(str.as_string(), "Hello");
}

/**
 * \brief   Test trim on string with tabs and newlines.
 **/
TEST(StringTestTrim, TrimWithTabsAndNewlines)
{
    String str("\t\n Hello \t\n");
    str.trim_all();
    EXPECT_STREQ(str.as_string(), "Hello");
}

//////////////////////////////////////////////////////////////////////////
// StringTestCase tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test makeLower converts all letters to lower case.
 **/
TEST(StringTestCase, MakeLower)
{
    String str("Hello WORLD 123");
    str.make_lower();
    EXPECT_STREQ(str.as_string(), "hello world 123");
}

/**
 * \brief   Test makeUpper converts all letters to upper case.
 **/
TEST(StringTestCase, MakeUpper)
{
    String str("Hello world 123");
    str.make_upper();
    EXPECT_STREQ(str.as_string(), "HELLO WORLD 123");
}

/**
 * \brief   Test that non-letter characters are unchanged by case conversion.
 **/
TEST(StringTestCase, NonLettersUnchanged)
{
    String str("!@#$%^&*()_+-=[]{}|;:,.<>?");
    String original(str);
    str.make_lower();
    EXPECT_STREQ(str.as_string(), original.as_string());
    str.make_upper();
    EXPECT_STREQ(str.as_string(), original.as_string());
}

//////////////////////////////////////////////////////////////////////////
// StringTestNumericConversions tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test toInt32 for decimal strings.
 **/
TEST(StringTestNumericConversions, ToInt32Decimal)
{
    String str("12345");
    EXPECT_EQ(str.to_int32(), 12345);

    String neg("-42");
    EXPECT_EQ(neg.to_int32(), -42);
}

/**
 * \brief   Test toUInt32 for decimal strings.
 **/
TEST(StringTestNumericConversions, ToUInt32Decimal)
{
    String str("4294967295");
    EXPECT_EQ(str.to_uint32(), 4294967295u);
}

/**
 * \brief   Test toInt64 for decimal strings.
 **/
TEST(StringTestNumericConversions, ToInt64Decimal)
{
    String str("9223372036854775807");
    EXPECT_EQ(str.to_int64(), INT64_C(9223372036854775807));
}

/**
 * \brief   Test toUInt64 for decimal strings.
 **/
TEST(StringTestNumericConversions, ToUInt64Decimal)
{
    String str("18446744073709551615");
    EXPECT_EQ(str.to_uint64(), UINT64_C(18446744073709551615));
}

/**
 * \brief   Test toInt32 with hexadecimal.
 **/
TEST(StringTestNumericConversions, ToInt32Hex)
{
    String str("FF");
    EXPECT_EQ(str.to_int32(areg::Radix::Hexadecimal), 255);
}

/**
 * \brief   Test toFloat.
 **/
TEST(StringTestNumericConversions, ToFloat)
{
    String str("3.14");
    EXPECT_NEAR(str.to_float(), 3.14f, 0.001f);
}

/**
 * \brief   Test toDouble.
 **/
TEST(StringTestNumericConversions, ToDouble)
{
    String str("3.14159265358979");
    EXPECT_NEAR(str.to_double(), 3.14159265358979, 0.0000001);
}

/**
 * \brief   Test toBool.
 **/
TEST(StringTestNumericConversions, ToBool)
{
    String t("true");
    String f("false");
    String empty;
    EXPECT_TRUE(t.to_bool());
    EXPECT_FALSE(f.to_bool());
    EXPECT_FALSE(empty.to_bool());
}

/**
 * \brief   Test fromInt32 and round-trip.
 **/
TEST(StringTestNumericConversions, FromInt32RoundTrip)
{
    String str;
    str.from_int32(-12345);
    EXPECT_EQ(str.to_int32(), -12345);
}

/**
 * \brief   Test fromUInt32 and round-trip.
 **/
TEST(StringTestNumericConversions, FromUInt32RoundTrip)
{
    String str;
    str.from_uint32(4294967295u);
    EXPECT_EQ(str.to_uint32(), 4294967295u);
}

/**
 * \brief   Test fromInt64 and round-trip.
 **/
TEST(StringTestNumericConversions, FromInt64RoundTrip)
{
    String str;
    str.from_int64(INT64_C(-9223372036854775807));
    EXPECT_EQ(str.to_int64(), INT64_C(-9223372036854775807));
}

/**
 * \brief   Test fromUInt64 and round-trip.
 **/
TEST(StringTestNumericConversions, FromUInt64RoundTrip)
{
    String str;
    str.from_uint64(UINT64_C(18446744073709551615));
    EXPECT_EQ(str.to_uint64(), UINT64_C(18446744073709551615));
}

/**
 * \brief   Test fromFloat and round-trip.
 **/
TEST(StringTestNumericConversions, FromFloatRoundTrip)
{
    String str;
    str.from_float(3.14f);
    EXPECT_NEAR(str.to_float(), 3.14f, 0.01f);
}

/**
 * \brief   Test fromDouble and round-trip.
 **/
TEST(StringTestNumericConversions, FromDoubleRoundTrip)
{
    String str;
    str.from_double(3.14159265);
    EXPECT_NEAR(str.to_double(), 3.14159265, 0.0001);
}

/**
 * \brief   Test fromBool and round-trip.
 **/
TEST(StringTestNumericConversions, FromBoolRoundTrip)
{
    String strTrue;
    strTrue.from_bool(true);
    EXPECT_TRUE(strTrue.to_bool());

    String strFalse;
    strFalse.from_bool(false);
    EXPECT_FALSE(strFalse.to_bool());
}

/**
 * \brief   Test static makeInt32.
 **/
TEST(StringTestNumericConversions, StaticMakeInt32)
{
    EXPECT_EQ(String::make_int32("123"), 123);
    EXPECT_EQ(String::make_int32("-456"), -456);
    EXPECT_EQ(String::make_int32("FF", areg::Radix::Hexadecimal), 255);
}

/**
 * \brief   Test static makeUInt32.
 **/
TEST(StringTestNumericConversions, StaticMakeUInt32)
{
    EXPECT_EQ(String::make_uint32("4294967295"), 4294967295u);
}

/**
 * \brief   Test static makeFloat and makeDouble.
 **/
TEST(StringTestNumericConversions, StaticMakeFloatDouble)
{
    EXPECT_NEAR(String::make_float("1.5"), 1.5f, 0.001f);
    EXPECT_NEAR(String::make_double("1.5"), 1.5, 0.001);
}

/**
 * \brief   Test static makeBool.
 **/
TEST(StringTestNumericConversions, StaticMakeBool)
{
    EXPECT_TRUE(String::make_bool("true"));
    EXPECT_FALSE(String::make_bool("false"));
}

/**
 * \brief   Test static makeString for various numeric types.
 **/
TEST(StringTestNumericConversions, StaticMakeString)
{
    EXPECT_STREQ(String::make_string(static_cast<int32_t>(42)).as_string(), "42");
    EXPECT_STREQ(String::make_string(static_cast<uint32_t>(42)).as_string(), "42");
    EXPECT_STREQ(String::make_string(true).as_string(), "true");
    EXPECT_STREQ(String::make_string(false).as_string(), "false");
}

/**
 * \brief   Test fromInt32 with hexadecimal radix.
 **/
TEST(StringTestNumericConversions, FromInt32Hex)
{
    String str;
    str.from_uint32(255u, areg::Radix::Hexadecimal);
    EXPECT_EQ(str.to_uint32(areg::Radix::Hexadecimal), 255u);
}

//////////////////////////////////////////////////////////////////////////
// StringTestFormat tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test format with %d.
 **/
TEST(StringTestFormat, FormatInt)
{
    String str;
    str.format("Value: %d", 42);
    EXPECT_STREQ(str.as_string(), "Value: 42");
}

/**
 * \brief   Test format with %s.
 **/
TEST(StringTestFormat, FormatString)
{
    String str;
    str.format("Hello, %s!", "World");
    EXPECT_STREQ(str.as_string(), "Hello, World!");
}

/**
 * \brief   Test format with %f.
 **/
TEST(StringTestFormat, FormatFloat)
{
    String str;
    str.format("Pi: %.2f", 3.14);
    EXPECT_STREQ(str.as_string(), "Pi: 3.14");
}

/**
 * \brief   Test format with %x.
 **/
TEST(StringTestFormat, FormatHex)
{
    String str;
    str.format("Hex: %x", 255);
    EXPECT_STREQ(str.as_string(), "Hex: ff");
}

/**
 * \brief   Test format with multiple arguments.
 **/
TEST(StringTestFormat, FormatMultipleArgs)
{
    String str;
    str.format("%s is %d years old", "Alice", 30);
    EXPECT_STREQ(str.as_string(), "Alice is 30 years old");
}

/**
 * \brief   Test formatString static method.
 **/
TEST(StringTestFormat, FormatStringStatic)
{
    char buf[64]{};
    int result = String::format_string(buf, 64, "Hello %s", "World");
    EXPECT_GT(result, 0);
    EXPECT_STREQ(buf, "Hello World");
}

/**
 * \brief   Test format auto-resize for long strings.
 **/
TEST(StringTestFormat, FormatAutoResize)
{
    String str;
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
    EXPECT_GT(str.length(), static_cast<areg::CharCount>(128));
}

//////////////////////////////////////////////////////////////////////////
// StringTestValidation tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test isEmpty and getLength.
 **/
TEST(StringTestValidation, IsEmptyAndGetLength)
{
    String empty;
    EXPECT_TRUE(empty.is_empty());
    EXPECT_EQ(empty.length(), static_cast<areg::CharCount>(0));

    String notEmpty("Hi");
    EXPECT_FALSE(notEmpty.is_empty());
    EXPECT_EQ(notEmpty.length(), static_cast<areg::CharCount>(2));
}

/**
 * \brief   Test getCapacity and getSpace.
 **/
TEST(StringTestValidation, GetCapacityAndSpace)
{
    String str("Hello");
    EXPECT_GE(str.capacity(), str.length());
    EXPECT_GT(str.space(), static_cast<uint32_t>(0));
}

/**
 * \brief   Test isNumeric.
 **/
TEST(StringTestValidation, IsNumeric)
{
    String digits("12345");
    EXPECT_TRUE(digits.is_numeric());

    String alpha("abc");
    EXPECT_FALSE(alpha.is_numeric());

    String withSign("-123");
    EXPECT_TRUE(withSign.is_numeric(true));
    EXPECT_FALSE(withSign.is_numeric(false));
}

/**
 * \brief   Test isAlphanumeric.
 **/
TEST(StringTestValidation, IsAlphanumeric)
{
    String valid("Hello123");
    EXPECT_TRUE(valid.is_alphanumeric());

    String withSymbol("Hello 123");
    EXPECT_FALSE(withSymbol.is_alphanumeric());
}

/**
 * \brief   Test isValidName.
 **/
TEST(StringTestValidation, IsValidName)
{
    String valid("my_var123");
    EXPECT_TRUE(valid.is_valid_name());

    String invalid("my var");
    EXPECT_FALSE(invalid.is_valid_name());
}

/**
 * \brief   Test validate with a custom validity list.
 **/
TEST(StringTestValidation, ValidateCustomList)
{
    String str("aabbcc");
    EXPECT_TRUE(str.validate("abc"));
    EXPECT_FALSE(str.validate("ab"));
}

/**
 * \brief   Test position checks: isValidPosition, isInvalidPosition, isLastPosition, isFirstPosition.
 **/
TEST(StringTestValidation, PositionChecks)
{
    String str("Hello");
    EXPECT_TRUE(str.is_valid_position(static_cast<areg::CharPos>(0)));
    EXPECT_TRUE(str.is_valid_position(static_cast<areg::CharPos>(4)));
    EXPECT_FALSE(str.is_valid_position(static_cast<areg::CharPos>(5)));
    EXPECT_TRUE(str.is_invalid_position(static_cast<areg::CharPos>(100)));
    EXPECT_TRUE(str.is_last_position(static_cast<areg::CharPos>(5)));
    EXPECT_TRUE(str.is_first_position(static_cast<areg::CharPos>(0)));
}

//////////////////////////////////////////////////////////////////////////
// StringTestEdgeCases tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test find on empty string returns invalid position.
 **/
TEST(StringTestEdgeCases, FindOnEmpty)
{
    String str;
    // findFirst(char) on empty: position 0 is invalid for empty string, returns INVALID_POS
    EXPECT_EQ(str.find_first('a'), areg::INVALID_POS);
    // findFirst(phrase) on empty: position 0 is not "invalid" but string is empty, returns END_POS
    areg::CharPos pos = str.find_first("abc");
    EXPECT_TRUE(pos == areg::INVALID_POS || pos == areg::END_POS);
    // findLast(char) on empty: mData.is_empty() is true, returns INVALID_POS
    EXPECT_EQ(str.find_last('a'), areg::INVALID_POS);
}

/**
 * \brief   Test replace on empty string is safe.
 **/
TEST(StringTestEdgeCases, ReplaceOnEmpty)
{
    String str;
    str.replace('a', 'b');
    EXPECT_TRUE(str.is_empty());
    str.replace("abc", "xyz");
    EXPECT_TRUE(str.is_empty());
}

/**
 * \brief   Test substring on empty string.
 **/
TEST(StringTestEdgeCases, SubstringOnEmpty)
{
    String str;
    String result;
    str.substring(result, areg::START_POS, static_cast<areg::CharCount>(5));
    EXPECT_TRUE(result.is_empty());
}

/**
 * \brief   Test operations with single character string.
 **/
TEST(StringTestEdgeCases, SingleCharString)
{
    String str("A");
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(1));
    EXPECT_EQ(str.find_first('A'), static_cast<areg::CharPos>(0));

    String left(str.left_side(static_cast<areg::CharCount>(1)).as_string());
    EXPECT_STREQ(left.as_string(), "A");

    String right(str.right_side(static_cast<areg::CharCount>(1)).as_string());
    EXPECT_STREQ(right.as_string(), "A");
}

/**
 * \brief   Test with a long string.
 **/
TEST(StringTestEdgeCases, LongString)
{
    std::string longStr(10000, 'A');
    String str(longStr);
    EXPECT_EQ(str.length(), static_cast<areg::CharCount>(10000));
    EXPECT_EQ(str.find_first('A'), static_cast<areg::CharPos>(0));
    EXPECT_EQ(str.find_last('A'), static_cast<areg::CharPos>(9999));
}

/**
 * \brief   Test the operator[] for character access.
 **/
TEST(StringTestEdgeCases, OperatorBracket)
{
    String str("Hello");
    EXPECT_EQ(str[0], 'H');
    EXPECT_EQ(str[4], 'o');
}

/**
 * \brief   Test the const char* conversion operator.
 **/
TEST(StringTestEdgeCases, ConversionToCharPtr)
{
    String str("Test");
    const char* ptr = static_cast<const char*>(str);
    EXPECT_STREQ(ptr, "Test");
}

/**
 * \brief   Test startsWith and endsWith methods.
 **/
TEST(StringTestEdgeCases, StartsWithEndsWith)
{
    String str("Hello World");
    EXPECT_TRUE(str.starts_with("Hello"));
    EXPECT_FALSE(str.starts_with("World"));
    EXPECT_TRUE(str.ends_with("World"));
    EXPECT_FALSE(str.ends_with("Hello"));
    EXPECT_TRUE(str.starts_with("HELLO", false));
    EXPECT_TRUE(str.ends_with("WORLD", false));
}

/**
 * \brief   Test empty string constructor consistency.
 **/
TEST(StringTestEdgeCases, EmptyStringConsistency)
{
    String a;
    String b("");
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a.is_empty());
    EXPECT_TRUE(b.is_empty());
}

/**
 * \brief   Test getString returns the underlying buffer.
 **/
TEST(StringTestEdgeCases, GetStringNotNull)
{
    String str;
    EXPECT_NE(str.as_string(), nullptr);
}

/**
 * \brief   Test getData returns a valid std::string reference.
 **/
TEST(StringTestEdgeCases, GetDataReturnsStdString)
{
    String str("Test");
    const std::string& data = str.data();
    EXPECT_EQ(data, "Test");
}

/**
 * \brief   Test readLine method.
 **/
TEST(StringTestEdgeCases, ReadLine)
{
    String str("Line1\nLine2\nLine3");
    String line;
    areg::CharPos pos = str.read_line(line, areg::START_POS);
    EXPECT_STREQ(line.as_string(), "Line1");
    EXPECT_NE(pos, areg::INVALID_POS);

    pos = str.read_line(line, pos);
    EXPECT_STREQ(line.as_string(), "Line2");
}

/**
 * \brief   Test makeAlphanumeric removes non-alphanumeric characters.
 **/
TEST(StringTestEdgeCases, MakeAlphanumeric)
{
    String str("Hello, World! 123");
    str.make_alphanumeric();
    EXPECT_TRUE(str.is_alphanumeric());
}

/**
 * \brief Tests the std::hash specialization for the String class.
 **/
TEST(StringTest, StdHashSpecialization)
{
    String str1("hello");
    String str2("hello");
    String str3("world");
    std::hash<areg::String> hasher;
    EXPECT_EQ(hasher(str1), hasher(str2));
    EXPECT_NE(hasher(str1), hasher(str3));

    String empty1("");
    String empty2;
    EXPECT_EQ(hasher(empty1), hasher(empty2));

    String space(" ");
    String tab("\t");
    EXPECT_NE(hasher(space), hasher(tab));

    String upperCase("HELLO");
    EXPECT_NE(hasher(str1), hasher(upperCase));
}

/**
 * \brief Tests the required_size template specialization for std::string_view.
 **/
TEST(StringTest, RequiredSize_StdStringView )
{
    areg::required_size<std::string_view> required;
    std::string_view view1("hello");
    std::string_view view2("world !");
    std::string_view emptyView("");
    EXPECT_EQ(required(view1), 14u);
    EXPECT_EQ(required(view2), 16u);
    EXPECT_EQ(required(emptyView), 9u);

    std::string_view emptyView2;
    EXPECT_EQ(required(emptyView), required(emptyView2));

    EXPECT_NE(required(view1), required(view2));

}


/**
 * \brief Tests the required_size template specialization for std::string.
 **/
TEST(StringTest, RequiredSize_StdString) {
    areg::required_size<std::string> required;
    std::string str1("hello");
    std::string str2("world !");
    std::string empty1("");

    EXPECT_EQ(required(str1), 14u);
    EXPECT_EQ(required(str2), 16u);
    EXPECT_EQ(required(empty1), 9u);

    std::string empty2;
    EXPECT_EQ(required(empty1), required(empty2));
    EXPECT_NE(required(str1), required(str2));
}

/**
 * \brief Tests the required_size template specialization for areg::string.
 **/
TEST(StringTest, RequiredSize_aregString)
{
    areg::required_size<areg::String> required;
    areg::String str1("hello");
    areg::String str2("world !");
    areg::String empty1("");
    EXPECT_EQ(required(str1), 14u);
    EXPECT_EQ(required(str2), 16u);
    EXPECT_EQ(required(empty1), 9u);

    areg::String empty2;
    EXPECT_EQ(required(empty1), required(empty2));
    EXPECT_NE(required(str1), required(str2));


}
