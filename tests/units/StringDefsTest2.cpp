/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/StringDefsTest2.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Unit tests for functions in StringDefs.hpp that are not
 *              covered by StringDefsTest.cpp:
 *                  in_utf8_range, utf8_index, utf8_char_def,
 *                  make_lower_char, make_upper_char (int32_t overloads),
 *                  make_string, revert_string, is_position_valid,
 *                  copy_string_fast,
 *                  is_new_line, is_carriage_return, is_unix_eol, is_dos_eol,
 *                  is_one_of, not_less, not_more,
 *                  required_char_count, is_buffer_fit.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "gtest/gtest-param-test.h"

#include "areg/base/StringDefs.hpp"

#include <cstdarg>
#include <string>
#include <vector>

/************************************************************************
 * in_utf8_range
 ************************************************************************/

/**
 * \brief   Test in_utf8_range boundary conditions.
 *
 *          The function returns true for all values in [-128, 255] and false
 *          for every value outside that range.  We exercise:
 *            - Boundary values: -128, -1, 0, 127, 255
 *            - Out-of-range: -129, 256, INT32_MIN, INT32_MAX
 **/
TEST(StringDefsTest2, in_utf8_range)
{
    // In-range: entire signed-char range [-128, 127]
    for (int32_t i = -128; i <= 127; ++i)
    {
        EXPECT_TRUE(areg::in_utf8_range(i)) << "Expected in-range for i=" << i;
    }

    // In-range: unsigned-byte extension [128, 255]
    for (int32_t i = 128; i <= 255; ++i)
    {
        EXPECT_TRUE(areg::in_utf8_range(i)) << "Expected in-range for i=" << i;
    }

    // Out-of-range: one below and one above the valid window
    EXPECT_FALSE(areg::in_utf8_range(-129));
    EXPECT_FALSE(areg::in_utf8_range(256));
    EXPECT_FALSE(areg::in_utf8_range(1000));
    EXPECT_FALSE(areg::in_utf8_range(INT32_MAX));
    EXPECT_FALSE(areg::in_utf8_range(INT32_MIN));
}

/************************************************************************
 * utf8_index
 ************************************************************************/

/**
 * \brief   Test utf8_index extracts the low 8 bits of any int32_t code point.
 *
 *          For values [0, 255] the index equals the value itself.
 *          For negative signed-char values [-128, -1] the low 8 bits map to
 *          [128, 255] in unsigned space (two's-complement).
 **/
TEST(StringDefsTest2, utf8_index)
{
    // [0, 255]: index == value
    for (int32_t i = 0; i <= 255; ++i)
    {
        EXPECT_EQ(areg::utf8_index(i), static_cast<uint8_t>(i));
    }

    // Signed-char negatives: index must equal (value & 0xFF)
    for (int32_t i = -128; i < 0; ++i)
    {
        EXPECT_EQ(areg::utf8_index(i), static_cast<uint8_t>(i & 0xFF));
    }

    // Spot-check: 'A'=65, 'a'=97, '0'=48
    EXPECT_EQ(areg::utf8_index('A'), static_cast<uint8_t>('A'));
    EXPECT_EQ(areg::utf8_index('a'), static_cast<uint8_t>('a'));
    EXPECT_EQ(areg::utf8_index('0'), static_cast<uint8_t>('0'));
    EXPECT_EQ(areg::utf8_index('\0'), static_cast<uint8_t>(0));
}

/************************************************************************
 * utf8_char_def — consistency with is_* classification functions
 ************************************************************************/

/**
 * \brief   Test utf8_char_def returns a bitmask consistent with all
 *          character-classification functions.
 *
 *          For every code point in [-128, 255] we verify that the bitmask
 *          returned by utf8_char_def agrees with is_letter, is_numeric,
 *          is_whitespace, is_symbol, is_delimited, is_eol, is_carriage_return,
 *          is_eos, and is_control.  Each classification bit is exercised for
 *          both true and false cases.
 **/
TEST(StringDefsTest2, utf8_char_def_consistency)
{
    // Out-of-range must return CharCategory::Undefined (zero).
    EXPECT_EQ(areg::utf8_char_def(-129), static_cast<uint16_t>(areg::CharCategory::Undefined));
    EXPECT_EQ(areg::utf8_char_def(256),  static_cast<uint16_t>(areg::CharCategory::Undefined));

    // For every in-range code point the classification bitmask must be consistent
    // with the high-level predicate functions.
    for (int32_t i = -128; i <= 255; ++i)
    {
        const char ch = static_cast<char>(static_cast<uint8_t>(i));
        const uint16_t def = areg::utf8_char_def(i);

        // A zero bitmask is only valid for the null terminator (EOfS) or unknown
        // extended code points; every other character must have at least one bit.
        // We cannot assert non-zero in general (some extended slots ARE undefined).

        // letter bit agrees with is_letter
        EXPECT_EQ(areg::is_letter<char>(ch),
                  ((def & static_cast<uint16_t>(areg::CharCategory::Letter)) != 0))
            << "Letter mismatch for i=" << i;

        // number bit agrees with is_numeric
        EXPECT_EQ(areg::is_numeric<char>(ch),
                  ((def & static_cast<uint16_t>(areg::CharCategory::Number)) != 0))
            << "Number mismatch for i=" << i;

        // whitespace bit agrees with is_whitespace
        EXPECT_EQ(areg::is_whitespace<char>(ch),
                  ((def & static_cast<uint16_t>(areg::CharCategory::WhiteSpace)) != 0))
            << "WhiteSpace mismatch for i=" << i;

        // symbol bit agrees with is_symbol
        EXPECT_EQ(areg::is_symbol<char>(ch),
                  ((def & static_cast<uint16_t>(areg::CharCategory::Symbol)) != 0))
            << "Symbol mismatch for i=" << i;

        // EOL bit agrees with is_eol
        EXPECT_EQ(areg::is_eol<char>(ch),
                  ((def & static_cast<uint16_t>(areg::CharCategory::EndOfLine)) != 0))
            << "EndOfLine mismatch for i=" << i;

        // EOS bit agrees with is_eos
        EXPECT_EQ(areg::is_eos<char>(ch),
                  ((def & static_cast<uint16_t>(areg::CharCategory::EOfS)) != 0))
            << "EOfS mismatch for i=" << i;

        // control bit agrees with is_control
        EXPECT_EQ(areg::is_control<char>(ch),
                  ((def & static_cast<uint16_t>(areg::CharCategory::Control)) != 0))
            << "Control mismatch for i=" << i;
    }

    // Spot-checks for well-known characters
    EXPECT_TRUE(areg::is_letter<char>('A'));
    EXPECT_TRUE(areg::is_upper<char>('A'));
    EXPECT_FALSE(areg::is_lower<char>('A'));

    EXPECT_TRUE(areg::is_letter<char>('z'));
    EXPECT_TRUE(areg::is_lower<char>('z'));
    EXPECT_FALSE(areg::is_upper<char>('z'));

    EXPECT_TRUE(areg::is_numeric<char>('5'));
    EXPECT_FALSE(areg::is_letter<char>('5'));

    EXPECT_TRUE(areg::is_eos<char>('\0'));
    EXPECT_FALSE(areg::is_letter<char>('\0'));
}

/************************************************************************
 * make_lower_char / make_upper_char  (int32_t raw overloads)
 ************************************************************************/

/**
 * \brief   Test make_lower_char and make_upper_char for the int32_t overloads.
 *
 *          These return the uint32_t code-point for the case-converted character.
 *          Out-of-range inputs must pass through unchanged.
 **/
TEST(StringDefsTest2, make_lower_upper_char_int)
{
    // Uppercase ASCII -> lowercase
    for (int32_t ch = 'A'; ch <= 'Z'; ++ch)
    {
        EXPECT_EQ(areg::make_lower_char(ch), static_cast<uint32_t>(ch - 'A' + 'a'))
            << "make_lower_char failed for '" << static_cast<char>(ch) << "'";
        EXPECT_EQ(areg::make_upper_char(ch), static_cast<uint32_t>(ch))
            << "make_upper_char changed uppercase char '" << static_cast<char>(ch) << "'";
    }

    // Lowercase ASCII -> uppercase
    for (int32_t ch = 'a'; ch <= 'z'; ++ch)
    {
        EXPECT_EQ(areg::make_upper_char(ch), static_cast<uint32_t>(ch - 'a' + 'A'))
            << "make_upper_char failed for '" << static_cast<char>(ch) << "'";
        EXPECT_EQ(areg::make_lower_char(ch), static_cast<uint32_t>(ch))
            << "make_lower_char changed lowercase char '" << static_cast<char>(ch) << "'";
    }

    // Digits must pass through unchanged for both directions
    for (int32_t ch = '0'; ch <= '9'; ++ch)
    {
        EXPECT_EQ(areg::make_lower_char(ch), static_cast<uint32_t>(ch));
        EXPECT_EQ(areg::make_upper_char(ch), static_cast<uint32_t>(ch));
    }

    // Null terminator must pass through unchanged
    EXPECT_EQ(areg::make_lower_char('\0'), static_cast<uint32_t>('\0'));
    EXPECT_EQ(areg::make_upper_char('\0'), static_cast<uint32_t>('\0'));

    // Out-of-range values must return the input unchanged (no lookup)
    EXPECT_EQ(areg::make_lower_char(256),      static_cast<uint32_t>(256));
    EXPECT_EQ(areg::make_upper_char(256),      static_cast<uint32_t>(256));
    EXPECT_EQ(areg::make_lower_char(INT32_MAX), static_cast<uint32_t>(INT32_MAX));
    EXPECT_EQ(areg::make_upper_char(INT32_MAX), static_cast<uint32_t>(INT32_MAX));

    // Consistency: make_lower_char + make_upper_char are inverses for ASCII letters
    for (int32_t ch = 'a'; ch <= 'z'; ++ch)
    {
        EXPECT_EQ(areg::make_lower_char(static_cast<int32_t>(areg::make_upper_char(ch))),
                  static_cast<uint32_t>(ch));
    }
}

/************************************************************************
 * make_string  (integer -> string conversion)
 ************************************************************************/

/**
 * \brief   Test make_string with decimal radix for both char and wchar_t.
 *
 *          Verifies positive, negative, and zero conversions.  Also checks
 *          that the return value equals the number of characters written
 *          (excluding the null terminator).
 **/
TEST(StringDefsTest2, make_string_decimal)
{
    char    cbuf[64];
    wchar_t wbuf[64];

    // Positive integer
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 123, areg::Radix::Decimal);
        EXPECT_EQ(n, 3);
        EXPECT_EQ(areg::compare(cbuf, "123"), areg::Ordering::Equal);

        n = areg::make_string<wchar_t, int>(wbuf, 64, 123, areg::Radix::Decimal);
        EXPECT_EQ(n, 3);
        EXPECT_EQ(areg::compare(wbuf, L"123"), areg::Ordering::Equal);
    }

    // Negative integer
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, -456, areg::Radix::Decimal);
        EXPECT_EQ(n, 4);
        EXPECT_EQ(areg::compare(cbuf, "-456"), areg::Ordering::Equal);
    }

    // Zero
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 0, areg::Radix::Decimal);
        EXPECT_EQ(n, 1);
        EXPECT_EQ(areg::compare(cbuf, "0"), areg::Ordering::Equal);
    }

    // Single digit
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 7, areg::Radix::Decimal);
        EXPECT_EQ(n, 1);
        EXPECT_EQ(areg::compare(cbuf, "7"), areg::Ordering::Equal);
    }

    // Negative single digit
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, -1, areg::Radix::Decimal);
        EXPECT_EQ(n, 2);
        EXPECT_EQ(areg::compare(cbuf, "-1"), areg::Ordering::Equal);
    }

    // Automatic radix falls back to Decimal
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 99, areg::Radix::Automatic);
        EXPECT_EQ(n, 2);
        EXPECT_EQ(areg::compare(cbuf, "99"), areg::Ordering::Equal);
    }
}

/**
 * \brief   Test make_string with hexadecimal, octal, and binary radix.
 *
 *          Verifies that the correct digit characters are produced and that
 *          the return value matches the length of the output.
 **/
TEST(StringDefsTest2, make_string_radix)
{
    char cbuf[64];

    // Hexadecimal: 255 -> "FF"
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 255, areg::Radix::Hexadecimal);
        EXPECT_EQ(n, 2);
        EXPECT_EQ(areg::compare(cbuf, "FF"), areg::Ordering::Equal);
    }

    // Hexadecimal: 16 -> "10"
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 16, areg::Radix::Hexadecimal);
        EXPECT_EQ(n, 2);
        EXPECT_EQ(areg::compare(cbuf, "10"), areg::Ordering::Equal);
    }

    // Hexadecimal: 0 -> "0"
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 0, areg::Radix::Hexadecimal);
        EXPECT_EQ(n, 1);
        EXPECT_EQ(areg::compare(cbuf, "0"), areg::Ordering::Equal);
    }

    // Octal: 8 -> "10"
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 8, areg::Radix::Octal);
        EXPECT_EQ(n, 2);
        EXPECT_EQ(areg::compare(cbuf, "10"), areg::Ordering::Equal);
    }

    // Octal: 7 -> "7"
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 7, areg::Radix::Octal);
        EXPECT_EQ(n, 1);
        EXPECT_EQ(areg::compare(cbuf, "7"), areg::Ordering::Equal);
    }

    // Octal: 64 -> "100"
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 64, areg::Radix::Octal);
        EXPECT_EQ(n, 3);
        EXPECT_EQ(areg::compare(cbuf, "100"), areg::Ordering::Equal);
    }

    // Binary: 5 -> "101"
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 5, areg::Radix::Binary);
        EXPECT_EQ(n, 3);
        EXPECT_EQ(areg::compare(cbuf, "101"), areg::Ordering::Equal);
    }

    // Binary: 8 -> "1000"
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 8, areg::Radix::Binary);
        EXPECT_EQ(n, 4);
        EXPECT_EQ(areg::compare(cbuf, "1000"), areg::Ordering::Equal);
    }

    // Binary: 1 -> "1"
    {
        int32_t n = areg::make_string<char, int>(cbuf, 64, 1, areg::Radix::Binary);
        EXPECT_EQ(n, 1);
        EXPECT_EQ(areg::compare(cbuf, "1"), areg::Ordering::Equal);
    }

    // Negative with non-decimal radix: sign is NOT emitted (only decimal adds '-')
    {
        // -5 in binary: sign ignored, abs(5)=5 -> "101"
        int32_t n = areg::make_string<char, int>(cbuf, 64, -5, areg::Radix::Binary);
        EXPECT_EQ(n, 3);
        EXPECT_EQ(areg::compare(cbuf, "101"), areg::Ordering::Equal);
    }
}

/**
 * \brief   Test make_string with a null buffer: it must return the required size
 *          without writing anything.
 *
 *          When strDst is nullptr or charCount is zero, the function computes
 *          and returns the number of characters needed (excluding null terminator).
 **/
TEST(StringDefsTest2, make_string_size_query)
{
    // nullptr buffer — returns required length.
    // MSVC preprocessor treats '<CharType, IntType>' commas as macro arg separators,
    // so we capture the result in a local variable before passing to EXPECT_EQ.
    {
        int32_t n = areg::make_string<char, int>(nullptr, 64, 123, areg::Radix::Decimal);
        EXPECT_EQ(n, 3);
    }
    {
        int32_t n = areg::make_string<char, int>(nullptr, 64, -456, areg::Radix::Decimal);
        EXPECT_EQ(n, 4);
    }
    {
        int32_t n = areg::make_string<char, int>(nullptr, 64, 0, areg::Radix::Decimal);
        EXPECT_EQ(n, 1);
    }
    {
        int32_t n = areg::make_string<char, int>(nullptr, 64, 255, areg::Radix::Hexadecimal);
        EXPECT_EQ(n, 2);
    }
    {
        int32_t n = areg::make_string<char, int>(nullptr, 64, 8, areg::Radix::Octal);
        EXPECT_EQ(n, 2);
    }
    {
        int32_t n = areg::make_string<char, int>(nullptr, 64, 5, areg::Radix::Binary);
        EXPECT_EQ(n, 3);
    }

    // charCount == 0 — also returns required size
    char dummy[64]{};
    {
        int32_t n = areg::make_string<char, int>(dummy, 0, 999, areg::Radix::Decimal);
        EXPECT_EQ(n, 3);
    }
}

/************************************************************************
 * revert_string
 ************************************************************************/

/**
 * \brief   Test revert_string with COUNT_ALL — reverses the entire string.
 *
 *          Verifies that "hello" becomes "olleh", a single character stays
 *          unchanged, and null / empty inputs are handled safely.
 **/
TEST(StringDefsTest2, revert_string_count_all)
{
    // Multi-character string
    {
        char buf[] = "hello";
        areg::revert_string<char>(buf);
        EXPECT_STREQ(buf, "olleh");
    }

    // Single character: must remain unchanged
    {
        char buf[] = "X";
        areg::revert_string<char>(buf);
        EXPECT_STREQ(buf, "X");
    }

    // Empty string: must be a no-op
    {
        char buf[] = "";
        areg::revert_string<char>(buf);
        EXPECT_STREQ(buf, "");
    }

    // Null pointer: must not crash
    {
        areg::revert_string<char>(nullptr);
    }

    // Palindrome: must remain the same
    {
        char buf[] = "racecar";
        areg::revert_string<char>(buf);
        EXPECT_STREQ(buf, "racecar");
    }

    // Wide character version
    {
        wchar_t wbuf[] = L"world";
        areg::revert_string<wchar_t>(wbuf);
        EXPECT_EQ(std::wstring(wbuf), std::wstring(L"dlrow"));
    }
}

/**
 * \brief   Test revert_string with an explicit character count.
 *
 *          When charCount > 1, only the first charCount characters are reversed;
 *          the rest of the string is untouched.
 *          When charCount <= 1, the call is a no-op.
 **/
TEST(StringDefsTest2, revert_string_explicit_count)
{
    // Reverse first 3 characters of "hello" -> "lehlo"
    {
        char buf[] = "hello";
        areg::revert_string<char>(buf, 3);
        EXPECT_STREQ(buf, "lehlo");
    }

    // charCount == 1: no-op
    {
        char buf[] = "hello";
        areg::revert_string<char>(buf, 1);
        EXPECT_STREQ(buf, "hello");
    }

    // charCount == 0: no-op
    {
        char buf[] = "hello";
        areg::revert_string<char>(buf, 0);
        EXPECT_STREQ(buf, "hello");
    }

    // Reverse exactly the whole string using explicit length
    {
        char buf[] = "abcd";
        areg::revert_string<char>(buf, 4);
        EXPECT_STREQ(buf, "dcba");
    }

    // Reversing a 2-character string
    {
        char buf[] = "ab";
        areg::revert_string<char>(buf, 2);
        EXPECT_STREQ(buf, "ba");
    }
}

/************************************************************************
 * is_position_valid
 ************************************************************************/

/**
 * \brief   Test is_position_valid for all boundary cases.
 *
 *          Returns true for any position >= START_POS (0), and false for
 *          INVALID_POS (-1) and END_POS (-2).
 **/
TEST(StringDefsTest2, is_position_valid)
{
    // Valid positions: any non-negative value
    EXPECT_TRUE(areg::is_position_valid(0));
    EXPECT_TRUE(areg::is_position_valid(1));
    EXPECT_TRUE(areg::is_position_valid(100));
    EXPECT_TRUE(areg::is_position_valid(areg::START_POS));

    // Invalid positions: INVALID_POS and END_POS are negative sentinels
    EXPECT_FALSE(areg::is_position_valid(areg::INVALID_POS));
    EXPECT_FALSE(areg::is_position_valid(areg::END_POS));

    // Explicit negative values
    EXPECT_FALSE(areg::is_position_valid(-1));
    EXPECT_FALSE(areg::is_position_valid(-2));
    EXPECT_FALSE(areg::is_position_valid(-100));
}

/************************************************************************
 * copy_string_fast
 ************************************************************************/

/**
 * \brief   Test copy_string_fast for char and wchar_t.
 *
 *          Verifies full copy (COUNT_ALL), partial copy with explicit count,
 *          null destination (returns 0), and empty source.
 **/
TEST(StringDefsTest2, copy_string_fast)
{
    // --- char ---

    // Full copy COUNT_ALL
    {
        char dst[64]{};
        areg::CharCount n = areg::copy_string_fast<char>(dst, "hello", areg::COUNT_ALL);
        EXPECT_EQ(n, 5u);
        EXPECT_EQ(areg::compare(dst, "hello"), areg::Ordering::Equal);
    }

    // Partial copy: copy only first 3 characters
    {
        char dst[64]{};
        areg::CharCount n = areg::copy_string_fast<char>(dst, "hello", 3);
        EXPECT_EQ(n, 3u);
        // dst must be "hel\0"
        EXPECT_EQ(areg::compare(dst, "hel"), areg::Ordering::Equal);
    }

    // Null destination: must return 0 without crashing
    {
        areg::CharCount n = areg::copy_string_fast<char>(nullptr, "hello", areg::COUNT_ALL);
        EXPECT_EQ(n, 0u);
    }

    // Empty source: COUNT_ALL copies 0 characters
    {
        char dst[64]{ 'X', 'X', 'X', '\0' };
        areg::CharCount n = areg::copy_string_fast<char>(dst, "", areg::COUNT_ALL);
        EXPECT_EQ(n, 0u);
        EXPECT_EQ(dst[0], '\0');
    }

    // --- wchar_t ---

    // Full copy COUNT_ALL
    {
        wchar_t dst[64]{};
        areg::CharCount n = areg::copy_string_fast<wchar_t>(dst, L"world", areg::COUNT_ALL);
        EXPECT_EQ(n, 5u);
        EXPECT_EQ(areg::compare(dst, L"world"), areg::Ordering::Equal);
    }

    // Partial copy wchar_t
    {
        wchar_t dst[64]{};
        areg::CharCount n = areg::copy_string_fast<wchar_t>(dst, L"world", 3);
        EXPECT_EQ(n, 3u);
        EXPECT_EQ(areg::compare(dst, L"wor"), areg::Ordering::Equal);
    }
}

/************************************************************************
 * is_new_line, is_unix_eol
 ************************************************************************/

/**
 * \brief   Test is_new_line and is_unix_eol (single-character and pointer overloads).
 *
 *          Both functions treat '\n' as the Unix newline.  '\r' does NOT qualify;
 *          all other printable characters return false.
 **/
TEST(StringDefsTest2, is_new_line_and_unix_eol)
{
    // '\n' is the Unix end-of-line
    EXPECT_TRUE(areg::is_new_line<char>('\n'));
    EXPECT_TRUE(areg::is_unix_eol<char>('\n'));

    // '\r' is a carriage return, not a Unix newline
    EXPECT_FALSE(areg::is_new_line<char>('\r'));
    EXPECT_FALSE(areg::is_unix_eol<char>('\r'));

    // All printable ASCII are not newlines
    for (char ch = ' '; ch <= '~'; ++ch)
    {
        EXPECT_FALSE(areg::is_new_line<char>(ch))
            << "is_new_line returned true for '" << ch << "'";
        EXPECT_FALSE(areg::is_unix_eol<char>(ch))
            << "is_unix_eol returned true for '" << ch << "'";
    }

    // Null terminator is not a newline
    EXPECT_FALSE(areg::is_new_line<char>('\0'));

    // String pointer overload: checks first character
    {
        const char* lf = "\n";
        const char* cr = "\r";
        const char* ab = "ab";

        EXPECT_TRUE(areg::is_unix_eol<char>(lf));
        EXPECT_FALSE(areg::is_unix_eol<char>(cr));
        EXPECT_FALSE(areg::is_unix_eol<char>(ab));
        EXPECT_FALSE(areg::is_unix_eol<char>(nullptr));
    }

    // wchar_t version
    EXPECT_TRUE(areg::is_new_line<wchar_t>(L'\n'));
    EXPECT_FALSE(areg::is_new_line<wchar_t>(L'\r'));
    EXPECT_TRUE(areg::is_unix_eol<wchar_t>(L"\n"));
    EXPECT_FALSE(areg::is_unix_eol<wchar_t>(L"\r"));
}

/************************************************************************
 * is_carriage_return
 ************************************************************************/

/**
 * \brief   Test is_carriage_return for char and wchar_t.
 *
 *          Only '\r' returns true.  '\n' and all printable characters return false.
 **/
TEST(StringDefsTest2, is_carriage_return)
{
    EXPECT_TRUE(areg::is_carriage_return<char>('\r'));
    EXPECT_FALSE(areg::is_carriage_return<char>('\n'));
    EXPECT_FALSE(areg::is_carriage_return<char>('\0'));

    for (char ch = ' '; ch <= '~'; ++ch)
    {
        EXPECT_FALSE(areg::is_carriage_return<char>(ch))
            << "is_carriage_return returned true for '" << ch << "'";
    }

    EXPECT_TRUE(areg::is_carriage_return<wchar_t>(L'\r'));
    EXPECT_FALSE(areg::is_carriage_return<wchar_t>(L'\n'));
}

/************************************************************************
 * is_dos_eol
 ************************************************************************/

/**
 * \brief   Test is_dos_eol for both two-character and pointer overloads.
 *
 *          DOS line endings are '\r\n'.  Any other two-character pair must
 *          return false, including '\n\r' (reversed order).
 **/
TEST(StringDefsTest2, is_dos_eol)
{
    // Two-character overload
    EXPECT_TRUE(areg::is_dos_eol<char>('\r', '\n'));
    EXPECT_FALSE(areg::is_dos_eol<char>('\n', '\r'));   // wrong order
    EXPECT_FALSE(areg::is_dos_eol<char>('\r', '\r'));
    EXPECT_FALSE(areg::is_dos_eol<char>('\n', '\n'));
    EXPECT_FALSE(areg::is_dos_eol<char>('\r', '\0'));
    EXPECT_FALSE(areg::is_dos_eol<char>('a', 'b'));

    // String pointer overload
    {
        const char* crlf = "\r\n";
        const char* lf   = "\n";
        const char* cr   = "\r";
        const char* ab   = "ab";

        EXPECT_TRUE(areg::is_dos_eol<char>(crlf));
        EXPECT_FALSE(areg::is_dos_eol<char>(lf));
        EXPECT_FALSE(areg::is_dos_eol<char>(cr));
        EXPECT_FALSE(areg::is_dos_eol<char>(ab));
        EXPECT_FALSE(areg::is_dos_eol<char>(nullptr));
    }

    // wchar_t overloads
    EXPECT_TRUE(areg::is_dos_eol<wchar_t>(L'\r', L'\n'));
    EXPECT_FALSE(areg::is_dos_eol<wchar_t>(L'\n', L'\r'));
    EXPECT_TRUE(areg::is_dos_eol<wchar_t>(L"\r\n"));
    EXPECT_FALSE(areg::is_dos_eol<wchar_t>(L"\n\r"));
}

/************************************************************************
 * is_one_of
 ************************************************************************/

/**
 * \brief   Test is_one_of for char, wchar_t, and edge cases.
 *
 *          Returns true iff the character is present in the sequence.
 *          Null character, null sequence, and empty sequence all yield false.
 **/
TEST(StringDefsTest2, is_one_of)
{
    // --- char (SIMD-optimised specialisation via string_view::find) ---

    EXPECT_TRUE(areg::is_one_of<char>('a', "abc"));
    EXPECT_TRUE(areg::is_one_of<char>('b', "abc"));
    EXPECT_TRUE(areg::is_one_of<char>('c', "abc"));
    EXPECT_FALSE(areg::is_one_of<char>('d', "abc"));
    EXPECT_FALSE(areg::is_one_of<char>('A', "abc"));   // case-sensitive

    // Single-element sequence
    EXPECT_TRUE(areg::is_one_of<char>('x', "x"));
    EXPECT_FALSE(areg::is_one_of<char>('y', "x"));

    // Null character: always false regardless of sequence
    EXPECT_FALSE(areg::is_one_of<char>('\0', "abc"));
    EXPECT_FALSE(areg::is_one_of<char>('\0', ""));

    // Null sequence pointer: always false
    EXPECT_FALSE(areg::is_one_of<char>('a', nullptr));

    // Empty sequence ("" has no non-null chars): always false
    EXPECT_FALSE(areg::is_one_of<char>('a', ""));

    // Digits in digit string
    for (char ch = '0'; ch <= '9'; ++ch)
    {
        EXPECT_TRUE(areg::is_one_of<char>(ch, "0123456789"))
            << "Expected '" << ch << "' to be found in digit sequence";
    }
    EXPECT_FALSE(areg::is_one_of<char>('a', "0123456789"));

    // --- wchar_t (SIMD-optimised specialisation via wstring_view::find) ---

    EXPECT_TRUE(areg::is_one_of<wchar_t>(L'x', L"xyz"));
    EXPECT_TRUE(areg::is_one_of<wchar_t>(L'y', L"xyz"));
    EXPECT_FALSE(areg::is_one_of<wchar_t>(L'w', L"xyz"));
    EXPECT_FALSE(areg::is_one_of<wchar_t>(L'\0', L"xyz"));
    EXPECT_FALSE(areg::is_one_of<wchar_t>(L'x', nullptr));
    EXPECT_FALSE(areg::is_one_of<wchar_t>(L'x', L""));
}

/************************************************************************
 * not_less / not_more
 ************************************************************************/

/**
 * \brief   Test not_less for char and wchar_t.
 *
 *          Returns true when string length >= minCount.
 *          minCount == 0 always returns true.
 *          Null or empty strings return false for minCount > 0.
 **/
TEST(StringDefsTest2, not_less)
{
    // Exact match: length == minCount
    EXPECT_TRUE(areg::not_less<char>("hello", 5));

    // Length > minCount
    EXPECT_TRUE(areg::not_less<char>("hello", 0));
    EXPECT_TRUE(areg::not_less<char>("hello", 1));
    EXPECT_TRUE(areg::not_less<char>("hello", 4));

    // Length < minCount
    EXPECT_FALSE(areg::not_less<char>("hello", 6));
    EXPECT_FALSE(areg::not_less<char>("hi",    5));

    // minCount == 0 always true regardless of string
    EXPECT_TRUE(areg::not_less<char>("",      0));
    EXPECT_TRUE(areg::not_less<char>(nullptr, 0));

    // Null string with minCount > 0 -> false
    EXPECT_FALSE(areg::not_less<char>(nullptr, 1));
    EXPECT_FALSE(areg::not_less<char>(nullptr, 100));

    // Empty string with minCount > 0 -> false
    EXPECT_FALSE(areg::not_less<char>("", 1));

    // wchar_t version
    EXPECT_TRUE(areg::not_less<wchar_t>(L"hello", 5));
    EXPECT_TRUE(areg::not_less<wchar_t>(L"hello", 3));
    EXPECT_FALSE(areg::not_less<wchar_t>(L"hello", 6));
    EXPECT_TRUE(areg::not_less<wchar_t>(L"hello", 0));
    EXPECT_FALSE(areg::not_less<wchar_t>(nullptr, 1));
}

/**
 * \brief   Test not_more for char and wchar_t.
 *
 *          Returns true when string length <= maxCount.
 *          Null or empty strings always return true.
 *          maxCount == 0 returns true only for null/empty strings.
 **/
TEST(StringDefsTest2, not_more)
{
    // Length == maxCount: boundary must return true
    EXPECT_TRUE(areg::not_more<char>("hello", 5));

    // Length < maxCount
    EXPECT_TRUE(areg::not_more<char>("hi",    5));
    EXPECT_TRUE(areg::not_more<char>("hi",    3));
    EXPECT_TRUE(areg::not_more<char>("hi",    2));

    // Length > maxCount
    EXPECT_FALSE(areg::not_more<char>("hello", 4));
    EXPECT_FALSE(areg::not_more<char>("hello", 0));

    // Null string always satisfies not_more
    EXPECT_TRUE(areg::not_more<char>(nullptr, 0));
    EXPECT_TRUE(areg::not_more<char>(nullptr, 100));

    // Empty string always satisfies not_more
    EXPECT_TRUE(areg::not_more<char>("", 0));
    EXPECT_TRUE(areg::not_more<char>("", 5));

    // maxCount == 0: only empty/null qualifies
    EXPECT_FALSE(areg::not_more<char>("a",    0));
    EXPECT_TRUE(areg::not_more<char>("",     0));
    EXPECT_TRUE(areg::not_more<char>(nullptr, 0));

    // wchar_t version
    EXPECT_TRUE(areg::not_more<wchar_t>(L"hello", 5));
    EXPECT_TRUE(areg::not_more<wchar_t>(L"hello", 6));
    EXPECT_FALSE(areg::not_more<wchar_t>(L"hello", 4));
    EXPECT_TRUE(areg::not_more<wchar_t>(nullptr, 0));
    EXPECT_TRUE(areg::not_more<wchar_t>(L"",     0));
}

/************************************************************************
 * required_char_count / is_buffer_fit
 ************************************************************************/

/**
 * \brief   Test required_char_count for char.
 *
 *          The function returns the smallest of {128, 256, 512, 1024} that
 *          is larger than the formatted output, or -1 if the output exceeds 1024.
 **/
TEST(StringDefsTest2, required_char_count_char)
{
    // Short string fitting in MSG_MIN_BUF_SIZE (128): output length < 128.
    // "%s" with a 10-character string produces exactly 10 characters.
    const std::string s10(10, 'a');
    EXPECT_EQ(areg::required_char_count("%s", s10.c_str()), areg::MSG_MIN_BUF_SIZE);

    // String just below 128 characters.
    const std::string s127(127, 'b');
    EXPECT_EQ(areg::required_char_count("%s", s127.c_str()), areg::MSG_MIN_BUF_SIZE);

    // String of 128 characters: output length == 128 -> next tier 256.
    const std::string s128(128, 'c');
    EXPECT_EQ(areg::required_char_count("%s", s128.c_str()), areg::MSG_BUF_SIZE);

    // String fitting in MSG_BUF_SIZE (256): 200 characters.
    const std::string s200(200, 'd');
    EXPECT_EQ(areg::required_char_count("%s", s200.c_str()), areg::MSG_BUF_SIZE);

    // String fitting in MSG_BIG_BUF_SIZE (512): 300 characters.
    const std::string s300(300, 'e');
    EXPECT_EQ(areg::required_char_count("%s", s300.c_str()), areg::MSG_BIG_BUF_SIZE);

    // String fitting in MSG_EXTRA_BUF_SIZE (1024): 600 characters.
    const std::string s600(600, 'f');
    EXPECT_EQ(areg::required_char_count("%s", s600.c_str()), areg::MSG_EXTRA_BUF_SIZE);

    // String exceeding 1024 characters: must return -1.
    const std::string s1100(1100, 'g');
    EXPECT_EQ(areg::required_char_count("%s", s1100.c_str()), -1);
}

/**
 * \brief   Test is_buffer_fit for char.
 *
 *          Returns true when a buffer of the template-specified size is large
 *          enough to hold the formatted output (including the null terminator).
 **/
TEST(StringDefsTest2, is_buffer_fit_char)
{
    // 50 characters fits in 128, 256, 512, 1024
    const std::string s50(50, 'a');
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_MIN_BUF_SIZE>  ("%s", s50.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_BUF_SIZE>      ("%s", s50.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_BIG_BUF_SIZE>  ("%s", s50.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_EXTRA_BUF_SIZE>("%s", s50.c_str()));

    // 200 characters does NOT fit in 128, but fits in 256, 512, 1024
    const std::string s200(200, 'b');
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_MIN_BUF_SIZE>("%s", s200.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_BUF_SIZE>     ("%s", s200.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_BIG_BUF_SIZE> ("%s", s200.c_str()));

    // 400 characters does NOT fit in 128 or 256, but fits in 512 and 1024
    const std::string s400(400, 'c');
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_MIN_BUF_SIZE>("%s", s400.c_str()));
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_BUF_SIZE>    ("%s", s400.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_BIG_BUF_SIZE> ("%s", s400.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_EXTRA_BUF_SIZE>("%s", s400.c_str()));

    // 800 characters does NOT fit in 128, 256, or 512, but fits in 1024
    const std::string s800(800, 'd');
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_MIN_BUF_SIZE>  ("%s", s800.c_str()));
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_BUF_SIZE>      ("%s", s800.c_str()));
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_BIG_BUF_SIZE>  ("%s", s800.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_EXTRA_BUF_SIZE> ("%s", s800.c_str()));
}

/************************************************************************
 * wchar_t path tests for functions already tested for char above
 ************************************************************************/

/**
 * \brief   Test compare_strings, compare, compare_ignore_case with wchar_t.
 *
 *          Exercises the SIMD-optimised wcscmp/wcsncmp fast path and the
 *          generic case-insensitive path via compare_strings_impl.
 **/
TEST(StringDefsTest2, wchar_compare_strings)
{
    // MSVC: two-argument template calls inside macros confuse the preprocessor.
    // Capture result in a local variable before passing to EXPECT_EQ/NE.

    // Identical strings
    { areg::Ordering r = areg::compare<wchar_t, wchar_t>(L"hello", L"hello");   EXPECT_EQ(r, areg::Ordering::Equal); }

    // Left < right
    { areg::Ordering r = areg::compare<wchar_t, wchar_t>(L"abc", L"abd");       EXPECT_EQ(r, areg::Ordering::Smaller); }

    // Left > right
    { areg::Ordering r = areg::compare<wchar_t, wchar_t>(L"abd", L"abc");       EXPECT_EQ(r, areg::Ordering::Bigger); }

    // Case-sensitive: different case strings are NOT equal
    { areg::Ordering r = areg::compare<wchar_t, wchar_t>(L"Hello", L"hello");   EXPECT_NE(r, areg::Ordering::Equal); }

    // Case-insensitive: same letters, different case -> equal
    { areg::Ordering r = areg::compare_ignore_case<wchar_t, wchar_t>(L"Hello", L"hello"); EXPECT_EQ(r, areg::Ordering::Equal); }
    { areg::Ordering r = areg::compare_ignore_case<wchar_t, wchar_t>(L"WORLD", L"world"); EXPECT_EQ(r, areg::Ordering::Equal); }

    // compare with explicit count: only first N characters compared
    { areg::Ordering r = areg::compare<wchar_t, wchar_t>(L"abcXXX", L"abcYYY", 3); EXPECT_EQ(r, areg::Ordering::Equal); }
    { areg::Ordering r = areg::compare<wchar_t, wchar_t>(L"abcXXX", L"abcYYY", 4); EXPECT_NE(r, areg::Ordering::Equal); }

    // Null pointers
    { areg::Ordering r = areg::compare_strings<wchar_t, wchar_t>(nullptr, nullptr); EXPECT_EQ(r, areg::Ordering::Equal); }
    { areg::Ordering r = areg::compare_strings<wchar_t, wchar_t>(nullptr, L"a");    EXPECT_EQ(r, areg::Ordering::Smaller); }
    { areg::Ordering r = areg::compare_strings<wchar_t, wchar_t>(L"a",    nullptr); EXPECT_EQ(r, areg::Ordering::Bigger); }
}

/**
 * \brief   Test find_first and find_last for wchar_t strings.
 *
 *          Exercises the wide-character SIMD path (wstring_view::find/rfind) for
 *          both character and phrase overloads, case-sensitive and insensitive.
 **/
TEST(StringDefsTest2, wchar_find_first_last)
{
    const wchar_t* src = L"Hello World Hello";

    // find_first character — case-sensitive
    EXPECT_EQ(areg::find_first<wchar_t>(L'H', src), 0);
    EXPECT_EQ(areg::find_first<wchar_t>(L'o', src), 4);
    EXPECT_EQ(areg::find_first<wchar_t>(L'z', src), areg::INVALID_POS);

    // find_first character — case-insensitive: 'h' finds 'H' at position 0
    EXPECT_EQ(areg::find_first<wchar_t>(L'h', src, areg::START_POS, false), 0);

    // find_first phrase — case-sensitive
    EXPECT_EQ(areg::find_first<wchar_t>(L"Hello", src), 0);
    EXPECT_EQ(areg::find_first<wchar_t>(L"World", src), 6);
    EXPECT_EQ(areg::find_first<wchar_t>(L"xyz",   src), areg::INVALID_POS);

    // find_first phrase from offset: skip the first "Hello"
    {
        areg::CharPos pos = areg::find_first<wchar_t>(L"Hello", src, 1);
        EXPECT_EQ(pos, 12);   // second "Hello" starts at index 12
    }

    // find_first phrase case-insensitive
    EXPECT_EQ(areg::find_first<wchar_t>(L"hello", src, areg::START_POS, false), 0);

    // find_last character — case-sensitive: last 'l' in "Hello World Hello"
    EXPECT_EQ(areg::find_last<wchar_t>(L'l', src), 15);

    // find_last character — case-insensitive: last 'h' or 'H'
    EXPECT_EQ(areg::find_last<wchar_t>(L'h', src, areg::END_POS, false), 12);

    // find_last phrase — case-sensitive: last "Hello"
    EXPECT_EQ(areg::find_last<wchar_t>(L"Hello", src), 12);

    // find_last phrase not found
    EXPECT_EQ(areg::find_last<wchar_t>(L"xyz", src), areg::INVALID_POS);

    // find_last phrase case-insensitive
    EXPECT_EQ(areg::find_last<wchar_t>(L"hello", src, areg::END_POS, false), 12);
}

/**
 * \brief   Test remove_char for wchar_t strings.
 *
 *          Exercises the wmemchr/wmemmove fast path (case-sensitive) and the
 *          generic case-insensitive path for wide characters.
 **/
TEST(StringDefsTest2, wchar_remove_char)
{
    // Case-sensitive, remove all occurrences of L'l'
    {
        wchar_t buf[] = L"Hello World";
        areg::remove_char<wchar_t>(L'l', buf, true, true);
        EXPECT_EQ(std::wstring(buf), std::wstring(L"Heo Word"));
    }

    // Case-sensitive, remove first occurrence of L'l'
    {
        wchar_t buf[] = L"Hello World";
        areg::remove_char<wchar_t>(L'l', buf, false, true);
        EXPECT_EQ(std::wstring(buf), std::wstring(L"Helo World"));
    }

    // Case-insensitive, remove all L'h'/'H'
    {
        wchar_t buf[] = L"Hello hWorld";
        areg::remove_char<wchar_t>(L'h', buf, true, false);
        EXPECT_EQ(std::wstring(buf), std::wstring(L"ello World"));
    }

    // Character not present: string unchanged
    {
        wchar_t buf[] = L"Hello";
        areg::remove_char<wchar_t>(L'z', buf, true, true);
        EXPECT_EQ(std::wstring(buf), std::wstring(L"Hello"));
    }
}

/**
 * \brief   Test string_starts_with and string_ends_with for wchar_t.
 *
 *          Exercises the wstring_view::compare fast path and the generic
 *          case-insensitive path.
 **/
TEST(StringDefsTest2, wchar_starts_ends_with)
{
    // starts_with phrase — case-sensitive
    EXPECT_TRUE(areg::string_starts_with<wchar_t>(L"Hello World", L"Hello"));
    EXPECT_FALSE(areg::string_starts_with<wchar_t>(L"Hello World", L"World"));
    EXPECT_FALSE(areg::string_starts_with<wchar_t>(L"Hello World", L"hello"));

    // starts_with phrase — case-insensitive
    EXPECT_TRUE(areg::string_starts_with<wchar_t>(L"Hello World", L"hello", false));
    EXPECT_TRUE(areg::string_starts_with<wchar_t>(L"HELLO World", L"hello", false));

    // starts_with character — case-sensitive
    EXPECT_TRUE(areg::string_starts_with<wchar_t>(L"Hello", L'H'));
    EXPECT_FALSE(areg::string_starts_with<wchar_t>(L"Hello", L'h'));

    // starts_with character — case-insensitive
    EXPECT_TRUE(areg::string_starts_with<wchar_t>(L"Hello", L'h', false));

    // ends_with phrase — case-sensitive
    EXPECT_TRUE(areg::string_ends_with<wchar_t>(L"Hello World", L"World"));
    EXPECT_FALSE(areg::string_ends_with<wchar_t>(L"Hello World", L"world"));

    // ends_with phrase — case-insensitive
    EXPECT_TRUE(areg::string_ends_with<wchar_t>(L"Hello World", L"world", false));

    // ends_with character — case-sensitive
    EXPECT_TRUE(areg::string_ends_with<wchar_t>(L"Hello", L'o'));
    EXPECT_FALSE(areg::string_ends_with<wchar_t>(L"Hello", L'O'));

    // ends_with character — case-insensitive
    EXPECT_TRUE(areg::string_ends_with<wchar_t>(L"Hello", L'O', false));

    // Null and empty edge cases
    EXPECT_FALSE(areg::string_starts_with<wchar_t>(nullptr,  L"Hello"));
    EXPECT_FALSE(areg::string_starts_with<wchar_t>(L"Hello", nullptr));
    EXPECT_FALSE(areg::string_ends_with<wchar_t>(nullptr,  L"Hello"));
    EXPECT_FALSE(areg::string_ends_with<wchar_t>(L"Hello", nullptr));
    EXPECT_FALSE(areg::string_starts_with<wchar_t>(L"", L"Hello"));
    EXPECT_FALSE(areg::string_ends_with<wchar_t>(L"", L"Hello"));
}

/**
 * \brief   Test trim_left, trim_right, trim_all for wchar_t in-place overloads.
 *
 *          Exercises the wmemmove fast path used in the wide-char in-place trims.
 **/
TEST(StringDefsTest2, wchar_trim_inplace)
{
    // trim_left
    {
        wchar_t buf[] = L"   Hello   ";
        areg::trim_left<wchar_t>(buf);
        EXPECT_EQ(std::wstring(buf), std::wstring(L"Hello   "));
    }

    // trim_right
    {
        wchar_t buf[] = L"   Hello   ";
        areg::trim_right<wchar_t>(buf);
        EXPECT_EQ(std::wstring(buf), std::wstring(L"   Hello"));
    }

    // trim_all
    {
        wchar_t buf[] = L"   Hello   ";
        areg::trim_all<wchar_t>(buf);
        EXPECT_EQ(std::wstring(buf), std::wstring(L"Hello"));
    }

    // All whitespace -> empty string
    {
        wchar_t buf[] = L"     ";
        areg::trim_all<wchar_t>(buf);
        EXPECT_EQ(std::wstring(buf), std::wstring(L""));
    }

    // No whitespace -> unchanged
    {
        wchar_t buf[] = L"NoSpace";
        areg::trim_all<wchar_t>(buf);
        EXPECT_EQ(std::wstring(buf), std::wstring(L"NoSpace"));
    }
}
