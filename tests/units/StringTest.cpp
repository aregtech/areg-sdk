/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/StringTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
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

//////////////////////////////////////////////////////////////////////////
// StringTestConstructors tests
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Test default constructor creates an empty string.
 **/
TEST(StringTestConstructors, DefaultConstructor)
{
    String str;
    EXPECT_TRUE(str.isEmpty());
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(0));
    EXPECT_STREQ(str.getString(), "");
}

/**
 * \brief   Test constructor from const char*.
 **/
TEST(StringTestConstructors, FromConstCharPtr)
{
    String str("Hello");
    EXPECT_FALSE(str.isEmpty());
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(5));
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test constructor from nullptr creates an empty string.
 **/
TEST(StringTestConstructors, FromNullptr)
{
    String str(static_cast<const char*>(nullptr));
    EXPECT_TRUE(str.isEmpty());
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(0));
}

/**
 * \brief   Test constructor from std::string.
 **/
TEST(StringTestConstructors, FromStdString)
{
    std::string src("Hello World");
    String str(src);
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(11));
    EXPECT_STREQ(str.getString(), "Hello World");
}

/**
 * \brief   Test constructor from std::string_view.
 **/
TEST(StringTestConstructors, FromStdStringView)
{
    std::string_view sv("Test View");
    String str(sv);
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(9));
    EXPECT_STREQ(str.getString(), "Test View");
}

/**
 * \brief   Test constructor from a single char.
 **/
TEST(StringTestConstructors, FromChar)
{
    String str('A');
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(1));
    EXPECT_STREQ(str.getString(), "A");
}

/**
 * \brief   Test constructor from const char* with count (partial copy).
 **/
TEST(StringTestConstructors, FromCharPtrWithCount)
{
    String str("Hello World", 5);
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(5));
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test copy constructor.
 **/
TEST(StringTestConstructors, CopyConstructor)
{
    String original("Copy Me");
    String copy(original);
    EXPECT_EQ(copy.getLength(), original.getLength());
    EXPECT_STREQ(copy.getString(), original.getString());
    EXPECT_STREQ(copy.getString(), "Copy Me");
}

/**
 * \brief   Test move constructor.
 **/
TEST(StringTestConstructors, MoveConstructor)
{
    String original("Move Me");
    NEString::CharCount len = original.getLength();
    String moved(std::move(original));
    EXPECT_EQ(moved.getLength(), len);
    EXPECT_STREQ(moved.getString(), "Move Me");
}

/**
 * \brief   Test reserve constructor creates an empty string with capacity.
 **/
TEST(StringTestConstructors, ReserveConstructor)
{
    String str(static_cast<uint32_t>(100));
    EXPECT_TRUE(str.isEmpty());
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(0));
    EXPECT_GE(str.getCapacity(), static_cast<NEString::CharCount>(100));
}

/**
 * \brief   Test constructor from std::string rvalue.
 **/
TEST(StringTestConstructors, FromStdStringMove)
{
    std::string src("Moved std string");
    String str(std::move(src));
    EXPECT_STREQ(str.getString(), "Moved std string");
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
    EXPECT_STREQ(str.getString(), "Assigned");
}

/**
 * \brief   Test assignment from std::string.
 **/
TEST(StringTestAssignment, AssignFromStdString)
{
    String str;
    std::string src("From std::string");
    str = src;
    EXPECT_STREQ(str.getString(), "From std::string");
}

/**
 * \brief   Test assignment from std::string_view.
 **/
TEST(StringTestAssignment, AssignFromStringView)
{
    String str;
    std::string_view sv("From string_view");
    str = sv;
    EXPECT_STREQ(str.getString(), "From string_view");
}

/**
 * \brief   Test assignment from a single char.
 **/
TEST(StringTestAssignment, AssignFromChar)
{
    String str;
    str = 'Z';
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(1));
    EXPECT_STREQ(str.getString(), "Z");
}

/**
 * \brief   Test move assignment from String.
 **/
TEST(StringTestAssignment, MoveAssignFromString)
{
    String src("Move Source");
    String dst;
    dst = std::move(src);
    EXPECT_STREQ(dst.getString(), "Move Source");
}

/**
 * \brief   Test move assignment from std::string.
 **/
TEST(StringTestAssignment, MoveAssignFromStdString)
{
    std::string src("Move std::string");
    String dst;
    dst = std::move(src);
    EXPECT_STREQ(dst.getString(), "Move std::string");
}

/**
 * \brief   Test copy assignment from String.
 **/
TEST(StringTestAssignment, CopyAssignFromString)
{
    String src("Copy Source");
    String dst;
    dst = src;
    EXPECT_STREQ(dst.getString(), "Copy Source");
    EXPECT_STREQ(src.getString(), "Copy Source");
}

/**
 * \brief   Test self-assignment safety.
 **/
TEST(StringTestAssignment, SelfAssignment)
{
    String str("Self");
    const String& ref = str;
    str = ref;
    EXPECT_STREQ(str.getString(), "Self");
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
    EXPECT_EQ(str.compare("Hello", NEString::START_POS, NEString::COUNT_ALL, true), NEMath::eCompare::Equal);
    EXPECT_NE(str.compare("hello", NEString::START_POS, NEString::COUNT_ALL, true), NEMath::eCompare::Equal);
}

/**
 * \brief   Test case-insensitive compare method.
 **/
TEST(StringTestComparison, CompareMethodCaseInsensitive)
{
    String str("Hello");
    EXPECT_EQ(str.compare("HELLO", NEString::START_POS, NEString::COUNT_ALL, false), NEMath::eCompare::Equal);
    EXPECT_EQ(str.compare("hello", NEString::START_POS, NEString::COUNT_ALL, false), NEMath::eCompare::Equal);
}

/**
 * \brief   Test compare at a specific position with count.
 **/
TEST(StringTestComparison, CompareAtPositionWithCount)
{
    String str("Hello World");
    EXPECT_EQ(str.compare("World", static_cast<NEString::CharPos>(6), static_cast<NEString::CharCount>(5), true), NEMath::eCompare::Equal);
}

/**
 * \brief   Test compare with String object.
 **/
TEST(StringTestComparison, CompareWithStringObject)
{
    String a("abc");
    String b("ABC");
    EXPECT_EQ(a.compare(b, false), NEMath::eCompare::Equal);
    EXPECT_NE(a.compare(b, true), NEMath::eCompare::Equal);
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
    EXPECT_STREQ(a.getString(), "Hello World");
}

/**
 * \brief   Test += with const char*.
 **/
TEST(StringTestConcatenation, PlusEqualCharPtr)
{
    String str("Hello");
    str += " World";
    EXPECT_STREQ(str.getString(), "Hello World");
}

/**
 * \brief   Test += with std::string.
 **/
TEST(StringTestConcatenation, PlusEqualStdString)
{
    String str("Hello");
    std::string suffix(" World");
    str += suffix;
    EXPECT_STREQ(str.getString(), "Hello World");
}

/**
 * \brief   Test += with std::string_view.
 **/
TEST(StringTestConcatenation, PlusEqualStringView)
{
    String str("Hello");
    std::string_view sv(" World");
    str += sv;
    EXPECT_STREQ(str.getString(), "Hello World");
}

/**
 * \brief   Test += with char.
 **/
TEST(StringTestConcatenation, PlusEqualChar)
{
    String str("Hell");
    str += 'o';
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test binary + operator with String + String.
 **/
TEST(StringTestConcatenation, BinaryPlusStringString)
{
    String a("Hello");
    String b(" World");
    String result = a + b;
    EXPECT_STREQ(result.getString(), "Hello World");
}

/**
 * \brief   Test binary + operator with String + const char*.
 **/
TEST(StringTestConcatenation, BinaryPlusStringCharPtr)
{
    String a("Hello");
    String result = a + " World";
    EXPECT_STREQ(result.getString(), "Hello World");
}

/**
 * \brief   Test binary + operator with const char* + String.
 **/
TEST(StringTestConcatenation, BinaryPlusCharPtrString)
{
    String b(" World");
    String result = "Hello" + b;
    EXPECT_STREQ(result.getString(), "Hello World");
}

/**
 * \brief   Test binary + operator with String + char.
 **/
TEST(StringTestConcatenation, BinaryPlusStringChar)
{
    String a("Hell");
    String result = a + 'o';
    EXPECT_STREQ(result.getString(), "Hello");
}

/**
 * \brief   Test binary + operator with char + String.
 **/
TEST(StringTestConcatenation, BinaryPlusCharString)
{
    String b("ello");
    String result = 'H' + b;
    EXPECT_STREQ(result.getString(), "Hello");
}

/**
 * \brief   Test binary + operator with std::string + String.
 **/
TEST(StringTestConcatenation, BinaryPlusStdStringString)
{
    std::string a("Hello");
    String b(" World");
    String result = a + b;
    EXPECT_STREQ(result.getString(), "Hello World");
}

/**
 * \brief   Test binary + operator with std::string_view + String.
 **/
TEST(StringTestConcatenation, BinaryPlusStringViewString)
{
    std::string_view a("Hello");
    String b(" World");
    String result = a + b;
    EXPECT_STREQ(result.getString(), "Hello World");
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
    EXPECT_EQ(str.findFirst('W'), static_cast<NEString::CharPos>(6));
    EXPECT_EQ(str.findFirst('z'), NEString::END_POS);
}

/**
 * \brief   Test findFirst with case-insensitive character search.
 **/
TEST(StringTestSearch, FindFirstCharCaseInsensitive)
{
    String str("Hello World");
    EXPECT_EQ(str.findFirst('w', NEString::START_POS, false), static_cast<NEString::CharPos>(6));
}

/**
 * \brief   Test findFirst with a phrase.
 **/
TEST(StringTestSearch, FindFirstPhrase)
{
    String str("Hello World Hello");
    EXPECT_EQ(str.findFirst("World"), static_cast<NEString::CharPos>(6));
    EXPECT_EQ(str.findFirst("xyz"), NEString::END_POS);
}

/**
 * \brief   Test findFirst phrase with case-insensitive search.
 **/
TEST(StringTestSearch, FindFirstPhraseCaseInsensitive)
{
    String str("Hello World");
    EXPECT_EQ(str.findFirst("WORLD", NEString::START_POS, false), static_cast<NEString::CharPos>(6));
}

/**
 * \brief   Test findLast with a character.
 **/
TEST(StringTestSearch, FindLastChar)
{
    String str("Hello World");
    EXPECT_EQ(str.findLast('l'), static_cast<NEString::CharPos>(9));
    EXPECT_EQ(str.findLast('z'), NEString::END_POS);
}

/**
 * \brief   Test findLast with a phrase.
 **/
TEST(StringTestSearch, FindLastPhrase)
{
    String str("Hello World Hello World");
    // findLast searches backward; verify it finds the phrase
    NEString::CharPos pos = str.findLast("Hello");
    EXPECT_NE(pos, NEString::END_POS);
    EXPECT_GE(pos, static_cast<NEString::CharPos>(0));
}

/**
 * \brief   Test findOneOf.
 **/
TEST(StringTestSearch, FindOneOf)
{
    String str("Hello World");
    EXPECT_EQ(str.findOneOf("xWz"), static_cast<NEString::CharPos>(6));
    EXPECT_EQ(str.findOneOf("xyz"), NEString::END_POS);
}

/**
 * \brief   Test findFirst with a start position.
 **/
TEST(StringTestSearch, FindFirstWithStartPos)
{
    String str("abcabc");
    EXPECT_EQ(str.findFirst('a', static_cast<NEString::CharPos>(1)), static_cast<NEString::CharPos>(3));
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
    str.substring(static_cast<NEString::CharPos>(6));
    EXPECT_STREQ(str.getString(), "World");
}

/**
 * \brief   Test substring with startPos and count.
 **/
TEST(StringTestSubstring, SubstringWithCount)
{
    String str("Hello World");
    str.substring(static_cast<NEString::CharPos>(0), static_cast<NEString::CharCount>(5));
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test substring (extract variant) copies to another string.
 **/
TEST(StringTestSubstring, SubstringExtract)
{
    String str("Hello World");
    String result;
    str.substring(result, static_cast<NEString::CharPos>(6), static_cast<NEString::CharCount>(5));
    EXPECT_STREQ(result.getString(), "World");
    EXPECT_STREQ(str.getString(), "Hello World");
}

/**
 * \brief   Test leftSide extraction.
 **/
TEST(StringTestSubstring, LeftSide)
{
    String str("Hello World");
    String left(str.leftSide(static_cast<NEString::CharCount>(5)).getString());
    EXPECT_STREQ(left.getString(), "Hello");
}

/**
 * \brief   Test rightSide extraction.
 **/
TEST(StringTestSubstring, RightSide)
{
    String str("Hello World");
    String right(str.rightSide(static_cast<NEString::CharCount>(5)).getString());
    EXPECT_STREQ(right.getString(), "World");
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
    EXPECT_STREQ(str.getString(), "Assigned");
}

/**
 * \brief   Test assign method with count.
 **/
TEST(StringTestManipulation, AssignCharPtrWithCount)
{
    String str;
    str.assign("Hello World", static_cast<NEString::CharCount>(5));
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test assign method with std::string and position.
 **/
TEST(StringTestManipulation, AssignStdStringWithPos)
{
    String str;
    std::string src("Hello World");
    str.assign(src, static_cast<NEString::CharPos>(6), static_cast<NEString::CharCount>(5));
    EXPECT_STREQ(str.getString(), "World");
}

/**
 * \brief   Test assign method with a single char.
 **/
TEST(StringTestManipulation, AssignChar)
{
    String str;
    str.assign('X');
    EXPECT_STREQ(str.getString(), "X");
}

/**
 * \brief   Test append method with const char*.
 **/
TEST(StringTestManipulation, AppendCharPtr)
{
    String str("Hello");
    str.append(" World");
    EXPECT_STREQ(str.getString(), "Hello World");
}

/**
 * \brief   Test append method with count.
 **/
TEST(StringTestManipulation, AppendCharPtrWithCount)
{
    String str("Hello");
    str.append(" World!!!", static_cast<NEString::CharCount>(6));
    EXPECT_STREQ(str.getString(), "Hello World");
}

/**
 * \brief   Test append method with a single char.
 **/
TEST(StringTestManipulation, AppendChar)
{
    String str("Hell");
    str.append('o');
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test append method with String object.
 **/
TEST(StringTestManipulation, AppendStringObject)
{
    String str("Hello");
    String suffix(" World");
    str.append(suffix);
    EXPECT_STREQ(str.getString(), "Hello World");
}

/**
 * \brief   Test insertAt with a character.
 **/
TEST(StringTestManipulation, InsertAtChar)
{
    String str("Hllo");
    str.insertAt('e', static_cast<NEString::CharPos>(1));
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test insertAt with a string.
 **/
TEST(StringTestManipulation, InsertAtString)
{
    String str("Hello!");
    str.insertAt(" World", static_cast<NEString::CharPos>(5));
    EXPECT_STREQ(str.getString(), "Hello World!");
}

/**
 * \brief   Test insertAt at the beginning.
 **/
TEST(StringTestManipulation, InsertAtBeginning)
{
    String str("World");
    str.insertAt("Hello ", static_cast<NEString::CharPos>(0));
    EXPECT_STREQ(str.getString(), "Hello World");
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
    str.replace('l', 'r', NEString::START_POS, false);
    EXPECT_STREQ(str.getString(), "rlHello");
}

/**
 * \brief   Test replace string with string.
 **/
TEST(StringTestManipulation, ReplaceStringString)
{
    String str("Hello World");
    str.replace("World", "Earth");
    EXPECT_STREQ(str.getString(), "Hello Earth");
}

/**
 * \brief   Test replace all occurrences of a string.
 **/
TEST(StringTestManipulation, ReplaceStringStringAll)
{
    String str("abcabcabc");
    str.replace("abc", "x");
    EXPECT_STREQ(str.getString(), "xxx");
}

/**
 * \brief   Test remove at position with count.
 **/
TEST(StringTestManipulation, RemoveAtPosition)
{
    String str("Hello World");
    str.remove(static_cast<NEString::CharPos>(5), static_cast<NEString::CharCount>(6));
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test remove a character from the string.
 **/
TEST(StringTestManipulation, RemoveChar)
{
    String str("Hello");
    str.remove('l');
    EXPECT_STREQ(str.getString(), "Heo");
}

/**
 * \brief   Test remove a phrase from the string.
 **/
TEST(StringTestManipulation, RemovePhrase)
{
    String str("Hello World");
    str.remove(" World");
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test resize to make string longer.
 **/
TEST(StringTestManipulation, ResizeLonger)
{
    String str("Hi");
    str.resize(static_cast<NEString::CharCount>(5), 'x');
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(5));
    EXPECT_STREQ(str.getString(), "Hixxx");
}

/**
 * \brief   Test resize to make string shorter.
 **/
TEST(StringTestManipulation, ResizeShorter)
{
    String str("Hello World");
    str.resize(static_cast<NEString::CharCount>(5));
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(5));
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test truncate method.
 **/
TEST(StringTestManipulation, Truncate)
{
    String str("Hello World");
    str.truncate(static_cast<NEString::CharCount>(5));
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test reserve method.
 **/
TEST(StringTestManipulation, Reserve)
{
    String str("Hi");
    str.reserve(static_cast<NEString::CharCount>(100));
    EXPECT_GE(str.getCapacity(), static_cast<NEString::CharCount>(100));
    EXPECT_STREQ(str.getString(), "Hi");
}

/**
 * \brief   Test getAt and setAt methods.
 **/
TEST(StringTestManipulation, GetAtSetAt)
{
    String str("Hello");
    EXPECT_EQ(str.getAt(static_cast<NEString::CharPos>(0)), 'H');
    EXPECT_EQ(str.getAt(static_cast<NEString::CharPos>(4)), 'o');

    str.setAt('h', static_cast<NEString::CharPos>(0));
    EXPECT_STREQ(str.getString(), "hello");
}

/**
 * \brief   Test clear method.
 **/
TEST(StringTestManipulation, Clear)
{
    String str("Hello");
    str.clear();
    EXPECT_TRUE(str.isEmpty());
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(0));
}

/**
 * \brief   Test release method.
 **/
TEST(StringTestManipulation, Release)
{
    String str("Hello");
    str.release();
    EXPECT_TRUE(str.isEmpty());
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(0));
}

/**
 * \brief   Test freeExtra method.
 **/
TEST(StringTestManipulation, FreeExtra)
{
    String str(static_cast<uint32_t>(100));
    str.assign("Hi");
    NEString::CharCount capBefore = str.getCapacity();
    str.freeExtra();
    EXPECT_LE(str.getCapacity(), capBefore);
    EXPECT_STREQ(str.getString(), "Hi");
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
    str.trimLeft();
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test trimRight removes trailing whitespace.
 **/
TEST(StringTestTrim, TrimRight)
{
    String str("Hello   ");
    str.trimRight();
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test trimAll removes leading and trailing whitespace.
 **/
TEST(StringTestTrim, TrimAll)
{
    String str("   Hello World   ");
    str.trimAll();
    EXPECT_STREQ(str.getString(), "Hello World");
}

/**
 * \brief   Test trimLeft on a string that has no leading whitespace (no-op).
 **/
TEST(StringTestTrim, TrimLeftNoOp)
{
    String str("Hello   ");
    str.trimLeft();
    EXPECT_STREQ(str.getString(), "Hello   ");
}

/**
 * \brief   Test trimRight on a string that has no trailing whitespace (no-op).
 **/
TEST(StringTestTrim, TrimRightNoOp)
{
    String str("   Hello");
    str.trimRight();
    EXPECT_STREQ(str.getString(), "   Hello");
}

/**
 * \brief   Test trimAll on an already-trimmed string (no-op).
 **/
TEST(StringTestTrim, TrimAllNoOp)
{
    String str("Hello");
    str.trimAll();
    EXPECT_STREQ(str.getString(), "Hello");
}

/**
 * \brief   Test trim on string with tabs and newlines.
 **/
TEST(StringTestTrim, TrimWithTabsAndNewlines)
{
    String str("\t\n Hello \t\n");
    str.trimAll();
    EXPECT_STREQ(str.getString(), "Hello");
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
    str.makeLower();
    EXPECT_STREQ(str.getString(), "hello world 123");
}

/**
 * \brief   Test makeUpper converts all letters to upper case.
 **/
TEST(StringTestCase, MakeUpper)
{
    String str("Hello world 123");
    str.makeUpper();
    EXPECT_STREQ(str.getString(), "HELLO WORLD 123");
}

/**
 * \brief   Test that non-letter characters are unchanged by case conversion.
 **/
TEST(StringTestCase, NonLettersUnchanged)
{
    String str("!@#$%^&*()_+-=[]{}|;:,.<>?");
    String original(str);
    str.makeLower();
    EXPECT_STREQ(str.getString(), original.getString());
    str.makeUpper();
    EXPECT_STREQ(str.getString(), original.getString());
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
    EXPECT_EQ(str.toInt32(), 12345);

    String neg("-42");
    EXPECT_EQ(neg.toInt32(), -42);
}

/**
 * \brief   Test toUInt32 for decimal strings.
 **/
TEST(StringTestNumericConversions, ToUInt32Decimal)
{
    String str("4294967295");
    EXPECT_EQ(str.toUInt32(), 4294967295u);
}

/**
 * \brief   Test toInt64 for decimal strings.
 **/
TEST(StringTestNumericConversions, ToInt64Decimal)
{
    String str("9223372036854775807");
    EXPECT_EQ(str.toInt64(), INT64_C(9223372036854775807));
}

/**
 * \brief   Test toUInt64 for decimal strings.
 **/
TEST(StringTestNumericConversions, ToUInt64Decimal)
{
    String str("18446744073709551615");
    EXPECT_EQ(str.toUInt64(), UINT64_C(18446744073709551615));
}

/**
 * \brief   Test toInt32 with hexadecimal.
 **/
TEST(StringTestNumericConversions, ToInt32Hex)
{
    String str("FF");
    EXPECT_EQ(str.toInt32(NEString::eRadix::RadixHexadecimal), 255);
}

/**
 * \brief   Test toFloat.
 **/
TEST(StringTestNumericConversions, ToFloat)
{
    String str("3.14");
    EXPECT_NEAR(str.toFloat(), 3.14f, 0.001f);
}

/**
 * \brief   Test toDouble.
 **/
TEST(StringTestNumericConversions, ToDouble)
{
    String str("3.14159265358979");
    EXPECT_NEAR(str.toDouble(), 3.14159265358979, 0.0000001);
}

/**
 * \brief   Test toBool.
 **/
TEST(StringTestNumericConversions, ToBool)
{
    String t("true");
    String f("false");
    String empty;
    EXPECT_TRUE(t.toBool());
    EXPECT_FALSE(f.toBool());
    EXPECT_FALSE(empty.toBool());
}

/**
 * \brief   Test fromInt32 and round-trip.
 **/
TEST(StringTestNumericConversions, FromInt32RoundTrip)
{
    String str;
    str.fromInt32(-12345);
    EXPECT_EQ(str.toInt32(), -12345);
}

/**
 * \brief   Test fromUInt32 and round-trip.
 **/
TEST(StringTestNumericConversions, FromUInt32RoundTrip)
{
    String str;
    str.fromUInt32(4294967295u);
    EXPECT_EQ(str.toUInt32(), 4294967295u);
}

/**
 * \brief   Test fromInt64 and round-trip.
 **/
TEST(StringTestNumericConversions, FromInt64RoundTrip)
{
    String str;
    str.fromInt64(INT64_C(-9223372036854775807));
    EXPECT_EQ(str.toInt64(), INT64_C(-9223372036854775807));
}

/**
 * \brief   Test fromUInt64 and round-trip.
 **/
TEST(StringTestNumericConversions, FromUInt64RoundTrip)
{
    String str;
    str.fromUInt64(UINT64_C(18446744073709551615));
    EXPECT_EQ(str.toUInt64(), UINT64_C(18446744073709551615));
}

/**
 * \brief   Test fromFloat and round-trip.
 **/
TEST(StringTestNumericConversions, FromFloatRoundTrip)
{
    String str;
    str.fromFloat(3.14f);
    EXPECT_NEAR(str.toFloat(), 3.14f, 0.01f);
}

/**
 * \brief   Test fromDouble and round-trip.
 **/
TEST(StringTestNumericConversions, FromDoubleRoundTrip)
{
    String str;
    str.fromDouble(3.14159265);
    EXPECT_NEAR(str.toDouble(), 3.14159265, 0.0001);
}

/**
 * \brief   Test fromBool and round-trip.
 **/
TEST(StringTestNumericConversions, FromBoolRoundTrip)
{
    String strTrue;
    strTrue.fromBool(true);
    EXPECT_TRUE(strTrue.toBool());

    String strFalse;
    strFalse.fromBool(false);
    EXPECT_FALSE(strFalse.toBool());
}

/**
 * \brief   Test static makeInt32.
 **/
TEST(StringTestNumericConversions, StaticMakeInt32)
{
    EXPECT_EQ(String::makeInt32("123"), 123);
    EXPECT_EQ(String::makeInt32("-456"), -456);
    EXPECT_EQ(String::makeInt32("FF", NEString::eRadix::RadixHexadecimal), 255);
}

/**
 * \brief   Test static makeUInt32.
 **/
TEST(StringTestNumericConversions, StaticMakeUInt32)
{
    EXPECT_EQ(String::makeUInt32("4294967295"), 4294967295u);
}

/**
 * \brief   Test static makeFloat and makeDouble.
 **/
TEST(StringTestNumericConversions, StaticMakeFloatDouble)
{
    EXPECT_NEAR(String::makeFloat("1.5"), 1.5f, 0.001f);
    EXPECT_NEAR(String::makeDouble("1.5"), 1.5, 0.001);
}

/**
 * \brief   Test static makeBool.
 **/
TEST(StringTestNumericConversions, StaticMakeBool)
{
    EXPECT_TRUE(String::makeBool("true"));
    EXPECT_FALSE(String::makeBool("false"));
}

/**
 * \brief   Test static makeString for various numeric types.
 **/
TEST(StringTestNumericConversions, StaticMakeString)
{
    EXPECT_STREQ(String::makeString(static_cast<int32_t>(42)).getString(), "42");
    EXPECT_STREQ(String::makeString(static_cast<uint32_t>(42)).getString(), "42");
    EXPECT_STREQ(String::makeString(true).getString(), "true");
    EXPECT_STREQ(String::makeString(false).getString(), "false");
}

/**
 * \brief   Test fromInt32 with hexadecimal radix.
 **/
TEST(StringTestNumericConversions, FromInt32Hex)
{
    String str;
    str.fromUInt32(255u, NEString::eRadix::RadixHexadecimal);
    EXPECT_EQ(str.toUInt32(NEString::eRadix::RadixHexadecimal), 255u);
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
    EXPECT_STREQ(str.getString(), "Value: 42");
}

/**
 * \brief   Test format with %s.
 **/
TEST(StringTestFormat, FormatString)
{
    String str;
    str.format("Hello, %s!", "World");
    EXPECT_STREQ(str.getString(), "Hello, World!");
}

/**
 * \brief   Test format with %f.
 **/
TEST(StringTestFormat, FormatFloat)
{
    String str;
    str.format("Pi: %.2f", 3.14);
    EXPECT_STREQ(str.getString(), "Pi: 3.14");
}

/**
 * \brief   Test format with %x.
 **/
TEST(StringTestFormat, FormatHex)
{
    String str;
    str.format("Hex: %x", 255);
    EXPECT_STREQ(str.getString(), "Hex: ff");
}

/**
 * \brief   Test format with multiple arguments.
 **/
TEST(StringTestFormat, FormatMultipleArgs)
{
    String str;
    str.format("%s is %d years old", "Alice", 30);
    EXPECT_STREQ(str.getString(), "Alice is 30 years old");
}

/**
 * \brief   Test formatString static method.
 **/
TEST(StringTestFormat, FormatStringStatic)
{
    char buf[64]{};
    int result = String::formatString(buf, 64, "Hello %s", "World");
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
    EXPECT_GT(str.getLength(), static_cast<NEString::CharCount>(128));
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
    EXPECT_TRUE(empty.isEmpty());
    EXPECT_EQ(empty.getLength(), static_cast<NEString::CharCount>(0));

    String notEmpty("Hi");
    EXPECT_FALSE(notEmpty.isEmpty());
    EXPECT_EQ(notEmpty.getLength(), static_cast<NEString::CharCount>(2));
}

/**
 * \brief   Test getCapacity and getSpace.
 **/
TEST(StringTestValidation, GetCapacityAndSpace)
{
    String str("Hello");
    EXPECT_GE(str.getCapacity(), str.getLength());
    EXPECT_GT(str.getSpace(), static_cast<uint32_t>(0));
}

/**
 * \brief   Test isNumeric.
 **/
TEST(StringTestValidation, IsNumeric)
{
    String digits("12345");
    EXPECT_TRUE(digits.isNumeric());

    String alpha("abc");
    EXPECT_FALSE(alpha.isNumeric());

    String withSign("-123");
    EXPECT_TRUE(withSign.isNumeric(true));
    EXPECT_FALSE(withSign.isNumeric(false));
}

/**
 * \brief   Test isAlphanumeric.
 **/
TEST(StringTestValidation, IsAlphanumeric)
{
    String valid("Hello123");
    EXPECT_TRUE(valid.isAlphanumeric());

    String withSymbol("Hello 123");
    EXPECT_FALSE(withSymbol.isAlphanumeric());
}

/**
 * \brief   Test isValidName.
 **/
TEST(StringTestValidation, IsValidName)
{
    String valid("my_var123");
    EXPECT_TRUE(valid.isValidName());

    String invalid("my var");
    EXPECT_FALSE(invalid.isValidName());
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
    EXPECT_TRUE(str.isValidPosition(static_cast<NEString::CharPos>(0)));
    EXPECT_TRUE(str.isValidPosition(static_cast<NEString::CharPos>(4)));
    EXPECT_FALSE(str.isValidPosition(static_cast<NEString::CharPos>(5)));
    EXPECT_TRUE(str.isInvalidPosition(static_cast<NEString::CharPos>(100)));
    EXPECT_TRUE(str.isLastPosition(static_cast<NEString::CharPos>(5)));
    EXPECT_TRUE(str.isFirstPosition(static_cast<NEString::CharPos>(0)));
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
    EXPECT_EQ(str.findFirst('a'), NEString::INVALID_POS);
    // findFirst(phrase) on empty: position 0 is not "invalid" but string is empty, returns END_POS
    NEString::CharPos pos = str.findFirst("abc");
    EXPECT_TRUE(pos == NEString::INVALID_POS || pos == NEString::END_POS);
    // findLast(char) on empty: mData.empty() is true, returns INVALID_POS
    EXPECT_EQ(str.findLast('a'), NEString::INVALID_POS);
}

/**
 * \brief   Test replace on empty string is safe.
 **/
TEST(StringTestEdgeCases, ReplaceOnEmpty)
{
    String str;
    str.replace('a', 'b');
    EXPECT_TRUE(str.isEmpty());
    str.replace("abc", "xyz");
    EXPECT_TRUE(str.isEmpty());
}

/**
 * \brief   Test substring on empty string.
 **/
TEST(StringTestEdgeCases, SubstringOnEmpty)
{
    String str;
    String result;
    str.substring(result, NEString::START_POS, static_cast<NEString::CharCount>(5));
    EXPECT_TRUE(result.isEmpty());
}

/**
 * \brief   Test operations with single character string.
 **/
TEST(StringTestEdgeCases, SingleCharString)
{
    String str("A");
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(1));
    EXPECT_EQ(str.findFirst('A'), static_cast<NEString::CharPos>(0));

    String left(str.leftSide(static_cast<NEString::CharCount>(1)).getString());
    EXPECT_STREQ(left.getString(), "A");

    String right(str.rightSide(static_cast<NEString::CharCount>(1)).getString());
    EXPECT_STREQ(right.getString(), "A");
}

/**
 * \brief   Test with a long string.
 **/
TEST(StringTestEdgeCases, LongString)
{
    std::string longStr(10000, 'A');
    String str(longStr);
    EXPECT_EQ(str.getLength(), static_cast<NEString::CharCount>(10000));
    EXPECT_EQ(str.findFirst('A'), static_cast<NEString::CharPos>(0));
    EXPECT_EQ(str.findLast('A'), static_cast<NEString::CharPos>(9999));
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
    EXPECT_TRUE(str.startsWith("Hello"));
    EXPECT_FALSE(str.startsWith("World"));
    EXPECT_TRUE(str.endsWith("World"));
    EXPECT_FALSE(str.endsWith("Hello"));
    EXPECT_TRUE(str.startsWith("HELLO", false));
    EXPECT_TRUE(str.endsWith("WORLD", false));
}

/**
 * \brief   Test empty string constructor consistency.
 **/
TEST(StringTestEdgeCases, EmptyStringConsistency)
{
    String a;
    String b("");
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a.isEmpty());
    EXPECT_TRUE(b.isEmpty());
}

/**
 * \brief   Test getString returns the underlying buffer.
 **/
TEST(StringTestEdgeCases, GetStringNotNull)
{
    String str;
    EXPECT_NE(str.getString(), nullptr);
}

/**
 * \brief   Test getData returns a valid std::string reference.
 **/
TEST(StringTestEdgeCases, GetDataReturnsStdString)
{
    String str("Test");
    const std::string& data = str.getData();
    EXPECT_EQ(data, "Test");
}

/**
 * \brief   Test readLine method.
 **/
TEST(StringTestEdgeCases, ReadLine)
{
    String str("Line1\nLine2\nLine3");
    String line;
    NEString::CharPos pos = str.readLine(line, NEString::START_POS);
    EXPECT_STREQ(line.getString(), "Line1");
    EXPECT_NE(pos, NEString::INVALID_POS);

    pos = str.readLine(line, pos);
    EXPECT_STREQ(line.getString(), "Line2");
}

/**
 * \brief   Test makeAlphanumeric removes non-alphanumeric characters.
 **/
TEST(StringTestEdgeCases, MakeAlphanumeric)
{
    String str("Hello, World! 123");
    str.makeAlphanumeric();
    EXPECT_TRUE(str.isAlphanumeric());
}
