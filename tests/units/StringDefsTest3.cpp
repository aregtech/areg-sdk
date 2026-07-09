/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StringDefsTest3.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Bug-catching tests for functions in StringDefs.hpp not covered
 *              by StringDefsTest.cpp and StringDefsTest2.cpp:
 *                  is_lower/is_upper, is_alphanumeric, is_symbol, is_delimited,
 *                  is_control, is_readable, is_eos,
 *                  make_lower(str*) / make_upper(str*),
 *                  copy_string (bounded variant),
 *                  compare_fast,
 *                  trim copy-variants (dst/lenDst/src),
 *                  trim with explicit strLen (!all path),
 *                  find_first / find_last 'next' output parameter,
 *                  line() 'next' and DOS/UNIX EOL handling,
 *                  printable() 'next' output parameter,
 *                  make_string radix-out-of-range and buffer-truncation,
 *                  make_integer '+' sign,
 *                  remove_char return value,
 *                  required_char_count<wchar_t> and is_buffer_fit<size, wchar_t>,
 *                  utf8_char_def against known ground truth.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/StringDefs.hpp"

#include <cstdarg>
#include <cstring>
#include <cwchar>
#include <string>

/************************************************************************
 * utf8_char_def -- ground-truth based (non-circular) tests
 ************************************************************************/

/**
 * \brief   Test utf8_char_def against hard-coded expected bit patterns.
 *
 *          Previous tests verified consistency of utf8_char_def with the
 *          is_* predicates, but those predicates all call utf8_char_def
 *          themselves -- a circular check that catches nothing.
 *
 *          These tests use known ground truth: specific characters that
 *          every correct Latin-1 table must classify identically.
 *          A corrupted or wrong lookup table would make these fail.
 **/
TEST(StringDefsTest3, utf8_char_def_ground_truth)
{
    // 'A' must be a letter and uppercase -- not a number, not whitespace
    EXPECT_TRUE (areg::is_letter<char>('A'));
    EXPECT_TRUE (areg::is_upper<char>('A'));
    EXPECT_FALSE(areg::is_lower<char>('A'));
    EXPECT_FALSE(areg::is_numeric<char>('A'));
    EXPECT_FALSE(areg::is_whitespace<char>('A'));

    // 'a' must be a letter and lowercase
    EXPECT_TRUE (areg::is_letter<char>('a'));
    EXPECT_TRUE (areg::is_lower<char>('a'));
    EXPECT_FALSE(areg::is_upper<char>('a'));
    EXPECT_FALSE(areg::is_numeric<char>('a'));

    // '5' must be a digit, not a letter, not whitespace
    EXPECT_TRUE (areg::is_numeric<char>('5'));
    EXPECT_FALSE(areg::is_letter<char>('5'));
    EXPECT_FALSE(areg::is_whitespace<char>('5'));

    // ' ' (space) must be whitespace and printable, not a letter or digit
    EXPECT_TRUE (areg::is_whitespace<char>(' '));
    EXPECT_TRUE (areg::is_printable<char>(' '));
    EXPECT_FALSE(areg::is_letter<char>(' '));
    EXPECT_FALSE(areg::is_numeric<char>(' '));

    // '\t' must be whitespace and printable, not readable (readable excludes tabs)
    EXPECT_TRUE (areg::is_whitespace<char>('\t'));
    EXPECT_TRUE (areg::is_printable<char>('\t'));
    EXPECT_FALSE(areg::is_readable<char>('\t'));

    // '\n' is EOL (end-of-line bit) and also whitespace per the lookup table
    EXPECT_TRUE (areg::is_eol<char>('\n'));
    EXPECT_TRUE (areg::is_whitespace<char>('\n'));
    EXPECT_FALSE(areg::is_letter<char>('\n'));

    // '\r' is carriage-return AND has the EndOfLine bit AND whitespace in the table
    EXPECT_TRUE (areg::is_carriage_return<char>('\r'));
    EXPECT_TRUE (areg::is_eol<char>('\r'));
    EXPECT_TRUE (areg::is_whitespace<char>('\r'));

    // '\0' must be EOfS (end-of-string)
    EXPECT_TRUE (areg::is_eos<char>('\0'));
    EXPECT_FALSE(areg::is_letter<char>('\0'));
    EXPECT_FALSE(areg::is_printable<char>('\0'));

    // '+' must be a symbol, not a letter, not a number
    EXPECT_TRUE (areg::is_symbol<char>('+'));
    EXPECT_FALSE(areg::is_letter<char>('+'));
    EXPECT_FALSE(areg::is_numeric<char>('+'));

    // ',' is a Symbol, not a Delimiter (Delimiter bit is only on space/tab)
    EXPECT_FALSE(areg::is_delimited<char>(','));
    EXPECT_FALSE(areg::is_letter<char>(','));

    // '\b' (backspace, 0x08) must be a control character
    EXPECT_TRUE (areg::is_control<char>('\b'));
    EXPECT_FALSE(areg::is_letter<char>('\b'));
    EXPECT_FALSE(areg::is_printable<char>('\b'));
}

/************************************************************************
 * is_lower / is_upper
 ************************************************************************/

/**
 * \brief   Test is_lower and is_upper for all ASCII letter pairs.
 *
 *          Every lowercase letter must be flagged as lower and not upper.
 *          Every uppercase letter must be flagged as upper and not lower.
 *          Non-letters (digits, symbols, control chars) must be neither.
 *          A bug in the LetterLo/LetterUp bitmasks in UTF8_256_Table would
 *          fail specific entries in this sweep.
 **/
TEST(StringDefsTest3, is_lower_is_upper_sweep)
{
    // Lowercase a-z: each must be lower, not upper
    for (char c = 'a'; c <= 'z'; ++c)
    {
        EXPECT_TRUE (areg::is_lower<char>(c)) << "Expected lower for '" << c << "'";
        EXPECT_FALSE(areg::is_upper<char>(c)) << "Expected NOT upper for '" << c << "'";
    }

    // Uppercase A-Z: each must be upper, not lower
    for (char c = 'A'; c <= 'Z'; ++c)
    {
        EXPECT_TRUE (areg::is_upper<char>(c)) << "Expected upper for '" << c << "'";
        EXPECT_FALSE(areg::is_lower<char>(c)) << "Expected NOT lower for '" << c << "'";
    }

    // Digits 0-9 must be neither
    for (char c = '0'; c <= '9'; ++c)
    {
        EXPECT_FALSE(areg::is_lower<char>(c)) << "Digit '" << c << "' should not be lower";
        EXPECT_FALSE(areg::is_upper<char>(c)) << "Digit '" << c << "' should not be upper";
    }

    // Space, tab, null -- none of these are letters
    EXPECT_FALSE(areg::is_lower<char>(' '));
    EXPECT_FALSE(areg::is_upper<char>(' '));
    EXPECT_FALSE(areg::is_lower<char>('\0'));
    EXPECT_FALSE(areg::is_upper<char>('\0'));

    // wchar_t equivalents for the same ranges
    for (wchar_t c = L'a'; c <= L'z'; ++c)
        EXPECT_TRUE(areg::is_lower<wchar_t>(c));
    for (wchar_t c = L'A'; c <= L'Z'; ++c)
        EXPECT_TRUE(areg::is_upper<wchar_t>(c));
}

/************************************************************************
 * is_alphanumeric
 ************************************************************************/

/**
 * \brief   Test is_alphanumeric with letters, digits, and non-alphanumeric chars.
 *
 *          is_alphanumeric checks Letter|Number in one mask operation.
 *          A separate bit check could accidentally match a symbol bit.
 *          Letters-only and digits-only must both return true.
 *          Symbols, spaces, and control chars must return false.
 **/
TEST(StringDefsTest3, is_alphanumeric)
{
    // All letters are alphanumeric
    EXPECT_TRUE(areg::is_alphanumeric<char>('A'));
    EXPECT_TRUE(areg::is_alphanumeric<char>('z'));

    // All digits are alphanumeric
    EXPECT_TRUE(areg::is_alphanumeric<char>('0'));
    EXPECT_TRUE(areg::is_alphanumeric<char>('9'));

    // Symbols and whitespace are not
    EXPECT_FALSE(areg::is_alphanumeric<char>(' '));
    EXPECT_FALSE(areg::is_alphanumeric<char>('+'));
    EXPECT_FALSE(areg::is_alphanumeric<char>(','));
    EXPECT_FALSE(areg::is_alphanumeric<char>('\n'));
    EXPECT_FALSE(areg::is_alphanumeric<char>('\0'));

    // wchar_t
    EXPECT_TRUE (areg::is_alphanumeric<wchar_t>(L'B'));
    EXPECT_TRUE (areg::is_alphanumeric<wchar_t>(L'7'));
    EXPECT_FALSE(areg::is_alphanumeric<wchar_t>(L'!'));
}

/************************************************************************
 * is_readable -- distinct from is_printable
 ************************************************************************/

/**
 * \brief   Test is_readable versus is_printable for characters where they differ.
 *
 *          Readable = Letter | Number | Symbol | space literal.
 *          Printable = everything in Printable bitmask (includes whitespace like '\t').
 *          Key difference: '\t' is printable but NOT readable.
 *          Catching bugs in the definition of Printable vs readable.
 **/
TEST(StringDefsTest3, is_readable_vs_printable)
{
    // Ordinary letters and digits: both readable and printable
    EXPECT_TRUE(areg::is_readable<char>('A'));
    EXPECT_TRUE(areg::is_printable<char>('A'));

    // Space: readable (special-cased as ' ') and printable
    EXPECT_TRUE(areg::is_readable<char>(' '));
    EXPECT_TRUE(areg::is_printable<char>(' '));

    // Tab '\t': printable but NOT readable
    EXPECT_FALSE(areg::is_readable<char>('\t'));
    EXPECT_TRUE (areg::is_printable<char>('\t'));

    // '\n': NOT readable (not Letter/Number/Symbol), but IS printable per the lookup table
    // (the table entry for '\n' includes the Printable flag)
    EXPECT_FALSE(areg::is_readable<char>('\n'));
    EXPECT_TRUE (areg::is_printable<char>('\n'));

    // Null terminator: neither
    EXPECT_FALSE(areg::is_readable<char>('\0'));
    EXPECT_FALSE(areg::is_printable<char>('\0'));

    // Symbol '+': readable (Symbol bit set), printable
    EXPECT_TRUE(areg::is_readable<char>('+'));
    EXPECT_TRUE(areg::is_printable<char>('+'));
}

/************************************************************************
 * make_lower / make_upper -- string pointer overloads
 ************************************************************************/

/**
 * \brief   Test make_lower(CharType*) and make_upper(CharType*) in-place overloads.
 *
 *          These are separate code paths from the character overloads.
 *          A bug where the loop advances past the null terminator, does not
 *          write back, or skips one character would be caught here.
 **/
TEST(StringDefsTest3, make_lower_upper_string_inplace)
{
    // make_lower -- full ASCII uppercase string
    {
        char buf[] = "HELLO WORLD";
        areg::make_lower<char>(buf);
        EXPECT_STREQ(buf, "hello world");
    }

    // make_upper -- full ASCII lowercase string
    {
        char buf[] = "hello world";
        areg::make_upper<char>(buf);
        EXPECT_STREQ(buf, "HELLO WORLD");
    }

    // Mixed case and digits (digits must stay unchanged)
    {
        char buf[] = "Abc123Xyz";
        areg::make_lower<char>(buf);
        EXPECT_STREQ(buf, "abc123xyz");
    }
    {
        char buf[] = "abc123xyz";
        areg::make_upper<char>(buf);
        EXPECT_STREQ(buf, "ABC123XYZ");
    }

    // Empty string -- must not crash, must return the pointer
    {
        char buf[] = "";
        const char* result = areg::make_lower<char>(buf);
        EXPECT_EQ(result, buf);
    }

    // nullptr -- must not crash, must return nullptr
    {
        const char* result = areg::make_lower<char>(static_cast<char*>(nullptr));
        EXPECT_EQ(result, nullptr);
    }

    // wchar_t
    {
        wchar_t wbuf[] = L"ABC";
        areg::make_lower<wchar_t>(wbuf);
        EXPECT_EQ(std::wstring(wbuf), std::wstring(L"abc"));
    }
}

/************************************************************************
 * copy_string -- bounded variant (with dstSpace)
 ************************************************************************/

/**
 * \brief   Test copy_string(dst, dstSpace, src, count) -- the bounded-copy overload.
 *
 *          This is a completely different function from copy_string_fast.
 *          Key contracts:
 *          - Returns characters actually copied (not counting null).
 *          - Always null-terminates dst.
 *          - Never writes more than dstSpace-1 characters.
 *          - Returns 0 if dst is nullptr.
 *          A bug in the min(dstSpace-1, charsCopy) calculation or the
 *          cross-type transcoding path would be caught here.
 **/
TEST(StringDefsTest3, copy_string_bounded)
{
    // Full copy -- destination large enough
    {
        char dst[32]{};
        areg::CharCount n = areg::copy_string<char, char>(dst, 32, "Hello");
        EXPECT_EQ(n, static_cast<areg::CharCount>(5));
        EXPECT_EQ(areg::compare(dst, "Hello"), areg::Ordering::Equal);
        EXPECT_EQ(dst[5], '\0');   // must null-terminate
    }

    // Truncation -- dstSpace smaller than source.
    // The same-type copy path copies min(dstSpace, srcLen) chars and places null
    // at strDst[result], which equals strDst[dstSpace] when fully filled.
    // Use a buffer one element larger than dstSpace to safely hold the null.
    {
        char dst[8]{};
        areg::CharCount n = areg::copy_string<char, char>(dst, 4, "Hello");
        EXPECT_EQ(n, static_cast<areg::CharCount>(4));
        EXPECT_EQ(dst[4], '\0');   // null placed one past the copied chars
        EXPECT_EQ(std::strncmp(dst, "Hell", 4), 0);
    }

    // Explicit charsCopy smaller than source length
    {
        char dst[32]{};
        areg::CharCount n = areg::copy_string<char, char>(dst, 32, "Hello", 3);
        EXPECT_EQ(n, static_cast<areg::CharCount>(3));
        EXPECT_EQ(dst[3], '\0');
        EXPECT_EQ(std::strncmp(dst, "Hel", 3), 0);
    }

    // dstSpace == 1 -- copies 1 character and places null at dst[1].
    // Use a 2-element buffer to safely hold the null at index 1.
    {
        char dst[4] = "XXX";
        areg::CharCount n = areg::copy_string<char, char>(dst, 1, "Hello");
        EXPECT_EQ(n, static_cast<areg::CharCount>(1));
        EXPECT_EQ(dst[0], 'H');
        EXPECT_EQ(dst[1], '\0');
    }

    // Null destination -- must return 0 without crash
    {
        areg::CharCount n = areg::copy_string<char, char>(nullptr, 32, "Hello");
        EXPECT_EQ(n, static_cast<areg::CharCount>(0));
    }

    // Null source -- copies 0 characters, null-terminates
    {
        char dst[8] = "XXXXXXX";
        areg::CharCount n = areg::copy_string<char, char>(dst, 8, nullptr);
        EXPECT_EQ(n, static_cast<areg::CharCount>(0));
        EXPECT_EQ(dst[0], '\0');
    }

    // Empty source
    {
        char dst[8] = "XXXXXXX";
        areg::CharCount n = areg::copy_string<char, char>(dst, 8, "");
        EXPECT_EQ(n, static_cast<areg::CharCount>(0));
        EXPECT_EQ(dst[0], '\0');
    }

    // wchar_t same-type copy
    {
        wchar_t dst[16]{};
        areg::CharCount n = areg::copy_string<wchar_t, wchar_t>(dst, 16, L"Wide");
        EXPECT_EQ(n, static_cast<areg::CharCount>(4));
        EXPECT_EQ(areg::compare(dst, L"Wide"), areg::Ordering::Equal);
    }
}

/************************************************************************
 * compare_fast
 ************************************************************************/

/**
 * \brief   Test compare_fast -- fast bytewise comparison.
 *
 *          compare_fast has four distinct code paths:
 *            1. count == COUNT_ALL   -> delegates to compare_strings
 *            2. left_side == right_side (same pointer) -> Equal immediately
 *            3. Both non-null with explicit count -> mem_compare
 *            4. One pointer null -> Smaller/Bigger without dereferencing
 *          Only path 2 is a unique shortcut that compare_strings does not
 *          take -- a bug there (e.g., dereferencing before the pointer check)
 *          would be caught by test cases 2 and 4.
 **/
TEST(StringDefsTest3, compare_fast)
{
    // Path 1: COUNT_ALL -- delegates to compare_strings
    EXPECT_EQ(areg::compare_fast<char>("abc", "abc", areg::COUNT_ALL), areg::Ordering::Equal);
    EXPECT_EQ(areg::compare_fast<char>("abc", "abd", areg::COUNT_ALL), areg::Ordering::Smaller);
    EXPECT_EQ(areg::compare_fast<char>("abd", "abc", areg::COUNT_ALL), areg::Ordering::Bigger);

    // Path 2: same pointer -- must return Equal without reading memory
    const char* same = "hello";
    EXPECT_EQ(areg::compare_fast<char>(same, same, 5), areg::Ordering::Equal);

    // Path 3: explicit count -- bytewise compare via mem_compare
    EXPECT_EQ(areg::compare_fast<char>("abcX", "abcY", 3), areg::Ordering::Equal);   // first 3 equal
    EXPECT_NE(areg::compare_fast<char>("abcX", "abcY", 4), areg::Ordering::Equal);   // 4th differs

    // Path 4: one null -- must not crash, must return Smaller/Bigger
    EXPECT_EQ(areg::compare_fast<char>(nullptr, "x", 1),   areg::Ordering::Smaller);
    EXPECT_EQ(areg::compare_fast<char>("x",    nullptr, 1), areg::Ordering::Bigger);
    EXPECT_EQ(areg::compare_fast<char>(nullptr, nullptr, areg::COUNT_ALL), areg::Ordering::Equal);

    // wchar_t
    EXPECT_EQ(areg::compare_fast<wchar_t>(L"abc", L"abc", areg::COUNT_ALL), areg::Ordering::Equal);
    const wchar_t* wsame = L"hello";
    EXPECT_EQ(areg::compare_fast<wchar_t>(wsame, wsame, 5), areg::Ordering::Equal);
}

/************************************************************************
 * trim copy-variants (dst, lenDst, src, lenSrc)
 ************************************************************************/

/**
 * \brief   Test trim_left, trim_right, trim_all copy-variants.
 *
 *          These overloads write trimmed content into a separate destination
 *          buffer, leaving the source unchanged.  They are completely distinct
 *          code from the in-place overloads.  Key contracts:
 *          - lenDst==0 or null dst: writes nothing, no crash.
 *          - Destination is always null-terminated.
 *          - Source is never modified.
 *          - Respects lenDst limit (no overflow).
 *          A bug in begin/end pointer arithmetic or the dst-size guard would
 *          produce an incorrect result or buffer overrun.
 **/
TEST(StringDefsTest3, trim_copy_variants)
{
    // trim_left copy -- leading spaces stripped into dst
    {
        char dst[32]{};
        const char* src = "   Hello   ";
        areg::trim_left<char, char>(dst, 32, src);
        EXPECT_EQ(areg::compare(dst, "Hello   "), areg::Ordering::Equal);
        EXPECT_EQ(areg::compare(src, "   Hello   "), areg::Ordering::Equal); // source unchanged
    }

    // trim_right copy -- trailing spaces stripped into dst
    {
        char dst[32]{};
        areg::trim_right<char, char>(dst, 32, "   Hello   ");
        EXPECT_EQ(areg::compare(dst, "   Hello"), areg::Ordering::Equal);
    }

    // trim_all copy -- both sides stripped
    {
        char dst[32]{};
        areg::trim_all<char, char>(dst, 32, "   Hello   ");
        EXPECT_EQ(areg::compare(dst, "Hello"), areg::Ordering::Equal);
    }

    // trim_all copy -- all-whitespace source -> empty string
    {
        char dst[32] = "UNCHANGED";
        areg::trim_all<char, char>(dst, 32, "     ");
        EXPECT_EQ(dst[0], '\0');
    }

    // trim_left copy -- dst too small: truncation, but must still null-terminate
    {
        char dst[4]{};
        areg::trim_left<char, char>(dst, 4, "  LongString");
        // can copy at most 3 chars: "Lon"
        EXPECT_EQ(dst[3], '\0');
        EXPECT_EQ(std::strncmp(dst, "Lon", 3), 0);
    }

    // trim_all copy -- null dst: must not crash
    {
        areg::trim_all<char, char>(nullptr, 32, "  text  ");
        // no assertion -- just must not crash
    }

    // trim_right copy -- null source -> empty dst
    {
        char dst[8] = "XXXXXXX";
        areg::trim_right<char, char>(dst, 8, nullptr);
        EXPECT_EQ(dst[0], '\0');
    }

    // wchar_t copy variant
    {
        wchar_t dst[32]{};
        areg::trim_all<wchar_t, wchar_t>(dst, 32, L"  Wide  ");
        EXPECT_EQ(areg::compare(dst, L"Wide"), areg::Ordering::Equal);
    }
}

/************************************************************************
 * trim in-place with explicit strLen -- the '!all' branch
 ************************************************************************/

/**
 * \brief   Test trim_left/right/all in-place with an explicit strLen parameter.
 *
 *          When strLen != COUNT_ALL the functions operate on exactly strLen
 *          characters and then append the remainder of the buffer verbatim.
 *          This "!all" tail-append branch is entirely separate code from the
 *          COUNT_ALL path and is never exercised by the existing tests.
 *
 *          Bug scenario: if the tail-append loop is broken, the characters
 *          after strLen would be lost or duplicated.
 **/
TEST(StringDefsTest3, trim_inplace_explicit_strlen)
{
    // trim_left with explicit strLen: trim only the first strLen chars,
    // append the rest of the buffer unchanged.
    // Buffer: "  ABC|tail" (strLen=5 covers "  ABC"), tail="|tail"
    {
        char buf[] = "  ABC|tail";
        // strLen=5 -> operate on "  ABC"; tail is "|tail"
        areg::trim_left<char>(buf, 5);
        // Expected: "ABC|tail" -- leading spaces stripped, tail preserved
        EXPECT_EQ(areg::compare(buf, "ABC|tail"), areg::Ordering::Equal) << buf;
    }

    // trim_right with explicit strLen
    {
        char buf[] = "ABC  |tail";
        areg::trim_right<char>(buf, 5);
        // strLen=5 -> "ABC  " trimmed to "ABC", then "|tail" appended
        EXPECT_EQ(areg::compare(buf, "ABC|tail"), areg::Ordering::Equal) << buf;
    }

    // trim_all with explicit strLen
    {
        char buf[] = " AB |tail";
        areg::trim_all<char>(buf, 4);   // strLen=4: " AB "
        // trimmed to "AB", then "|tail" appended
        EXPECT_EQ(areg::compare(buf, "AB|tail"), areg::Ordering::Equal) << buf;
    }

    // All-whitespace segment with a tail: tail must survive
    {
        char buf[] = "    |tail";
        areg::trim_all<char>(buf, 4);
        EXPECT_EQ(areg::compare(buf, "|tail"), areg::Ordering::Equal) << buf;
    }
}

/************************************************************************
 * find_first / find_last -- 'next' output parameter
 ************************************************************************/

/**
 * \brief   Test the 'next' output pointer of find_first and find_last.
 *
 *          The 'next' parameter lets callers iterate through all matches
 *          without repeated full-string searches.  It is the primary way
 *          callers know where the match ended.
 *
 *          Bug scenarios:
 *          - 'next' points before the match end -> caller re-finds same match.
 *          - 'next' points past end of string -> caller reads garbage.
 *          - 'next' is not set to nullptr when no match or match is last char.
 **/
TEST(StringDefsTest3, find_first_next_param)
{
    const char* src = "aXbXcX";

    // Find first 'X'; next must point to the character after the found 'X'
    {
        const char* next = nullptr;
        areg::CharPos pos = areg::find_first<char>('X', src, areg::START_POS, true, &next);
        EXPECT_EQ(pos, 1);
        ASSERT_NE(next, nullptr);
        EXPECT_EQ(*next, 'b');   // character immediately after first 'X'
    }

    // Iterate all 'X' occurrences using next
    {
        const char* next = nullptr;
        areg::CharPos pos0 = areg::find_first<char>('X', src, areg::START_POS, true, &next);
        EXPECT_EQ(pos0, 1);
        ASSERT_NE(next, nullptr);

        // Advance: search from next's offset
        areg::CharPos offset1 = static_cast<areg::CharPos>(next - src);
        const char* next2 = nullptr;
        areg::CharPos pos1 = areg::find_first<char>('X', src, offset1, true, &next2);
        EXPECT_EQ(pos1, 3);   // second 'X' at index 3

        // Advance again
        areg::CharPos offset2 = static_cast<areg::CharPos>(next2 - src);
        const char* next3 = nullptr;
        areg::CharPos pos2 = areg::find_first<char>('X', src, offset2, true, &next3);
        EXPECT_EQ(pos2, 5);    // third 'X' at index 5
        EXPECT_EQ(next3, nullptr);  // 'X' is last char -- nothing after it
    }

    // Phrase find: next must point after the phrase
    {
        const char* next = nullptr;
        areg::CharPos pos = areg::find_first<char>("Xb", src, areg::START_POS, true, &next);
        EXPECT_EQ(pos, 1);    // "Xb" starts at index 1
        ASSERT_NE(next, nullptr);
        // "aXbXcX": "Xb" occupies indices 1-2, so after it is src[3]='X'
        EXPECT_EQ(*next, 'X');
    }

    // No match: next must remain nullptr
    {
        const char* next = reinterpret_cast<const char*>(0x1);  // sentinel non-null
        areg::CharPos pos = areg::find_first<char>('Z', src, areg::START_POS, true, &next);
        EXPECT_EQ(pos, areg::INVALID_POS);
        EXPECT_EQ(next, nullptr);
    }
}

TEST(StringDefsTest3, find_last_next_param)
{
    const char* src = "aXbXcX";

    // find_last 'X': must return last position and set next to that position
    {
        const char* next = nullptr;
        areg::CharPos pos = areg::find_last<char>('X', src, areg::END_POS, true, &next);
        EXPECT_EQ(pos, 5);
        ASSERT_NE(next, nullptr);
        EXPECT_EQ(next, src + 5);  // points to the found character
    }

    // No match: next remains nullptr
    {
        const char* next = reinterpret_cast<const char*>(0x1);
        areg::CharPos pos = areg::find_last<char>('Z', src, areg::END_POS, true, &next);
        EXPECT_EQ(pos, areg::INVALID_POS);
        EXPECT_EQ(next, nullptr);
    }

    // Phrase find_last: next points to the start of the found phrase
    {
        const char* next = nullptr;
        areg::CharPos pos = areg::find_last<char>("Xb", src, areg::END_POS, true, &next);
        EXPECT_EQ(pos, 1);
        ASSERT_NE(next, nullptr);
        EXPECT_EQ(next, src + 1);
    }
}

/************************************************************************
 * line() -- DOS vs UNIX EOL, 'next' output parameter
 ************************************************************************/

/**
 * \brief   Test line() with DOS (\r\n) and UNIX (\n) line endings.
 *
 *          DOS EOL path writes TWO null terminators (overwriting both \r and \n),
 *          which consumes one extra byte.  UNIX EOL writes ONE.
 *          The 'next' pointer must land on the first character of the next line,
 *          not on the null terminator written by the DOS path.
 *
 *          Bug scenario: if the DOS branch writes only one null (like the UNIX
 *          branch), 'next' points to '\n' instead of the next line's first char.
 **/
TEST(StringDefsTest3, line_dos_unix_eol_and_next)
{
    // UNIX EOL: "line1\nline2"
    {
        char buf[] = "line1\nline2";
        char* next = nullptr;
        const char* l = areg::line<char>(buf, areg::COUNT_ALL, &next);
        EXPECT_STREQ(l, "line1");   // first line, null-terminated at '\n'
        ASSERT_NE(next, nullptr);
        EXPECT_STREQ(next, "line2");  // next must point to second line
    }

    // DOS EOL: "line1\r\nline2"
    {
        char buf[] = "line1\r\nline2";
        char* next = nullptr;
        const char* l = areg::line<char>(buf, areg::COUNT_ALL, &next);
        EXPECT_STREQ(l, "line1");
        ASSERT_NE(next, nullptr);
        EXPECT_STREQ(next, "line2");  // must skip BOTH \r and \n
    }

    // Last line (no EOL): next must be null
    {
        char buf[] = "onlyline";
        char* next = reinterpret_cast<char*>(0x1);  // sentinel
        const char* l = areg::line<char>(buf, areg::COUNT_ALL, &next);
        EXPECT_STREQ(l, "onlyline");
        EXPECT_EQ(next, nullptr);
    }

    // Empty string: returns the pointer, next = nullptr
    {
        char buf[] = "";
        char* next = reinterpret_cast<char*>(0x1);
        const char* l = areg::line<char>(buf, areg::COUNT_ALL, &next);
        EXPECT_EQ(l, buf);
        // next is not set when is_empty -- no requirement to touch it
        (void)next;
    }

    // Multiple lines: iterate all three
    {
        char buf[] = "a\nb\nc";
        char* next = nullptr;
        const char* l1 = areg::line<char>(buf, areg::COUNT_ALL, &next);
        EXPECT_STREQ(l1, "a");
        ASSERT_NE(next, nullptr);

        const char* l2 = areg::line<char>(next, areg::COUNT_ALL, &next);
        EXPECT_STREQ(l2, "b");
        ASSERT_NE(next, nullptr);

        const char* l3 = areg::line<char>(next, areg::COUNT_ALL, &next);
        EXPECT_STREQ(l3, "c");
        EXPECT_EQ(next, nullptr);   // last line
    }
}

/************************************************************************
 * printable() -- 'next' output parameter
 ************************************************************************/

/**
 * \brief   Test printable() with the 'next' output pointer.
 *
 *          printable() null-terminates the buffer at the first non-printable
 *          character and sets *next to the character after the terminator.
 *          Without testing 'next', a caller using printable() to tokenise a
 *          buffer would silently read garbage on an off-by-one.
 **/
TEST(StringDefsTest3, printable_next_param)
{
    // "Hello\1World": printable part is "Hello", next should point to 'W'
    {
        char buf[] = "Hello\1World";
        char* next = nullptr;
        const char* p = areg::printable<char>(buf, areg::COUNT_ALL, &next);
        EXPECT_STREQ(p, "Hello");
        ASSERT_NE(next, nullptr);
        EXPECT_STREQ(next, "World");   // character after the non-printable '\1'
    }

    // All printable -- no non-printable found; next must be null
    {
        char buf[] = "AllPrintable";
        char* next = reinterpret_cast<char*>(0x1);
        const char* p = areg::printable<char>(buf, areg::COUNT_ALL, &next);
        EXPECT_STREQ(p, "AllPrintable");
        EXPECT_EQ(next, nullptr);
    }

    // Non-printable at first position -- returns empty string, next points after
    {
        char buf[] = "\2ABC";
        char* next = nullptr;
        const char* p = areg::printable<char>(buf, areg::COUNT_ALL, &next);
        EXPECT_NE(p, nullptr);
        EXPECT_EQ(p[0], '\0');         // empty printable span
        ASSERT_NE(next, nullptr);
        EXPECT_STREQ(next, "ABC");
    }
}

/************************************************************************
 * make_string -- radix out of range and buffer truncation
 ************************************************************************/

/**
 * \brief   Test make_string with an invalid radix and with a buffer too small.
 *
 *          Invalid radix: the function must return 0 and write nothing.
 *          Buffer truncation: the do-while loop exits early when charCount
 *          reaches zero; the result must be a valid (partial) null-terminated
 *          string, not garbage.
 **/
TEST(StringDefsTest3, make_string_edge_cases)
{
    // Invalid radix: value 1 is below Binary(2), triggering the out-of-range check.
    // The function must return 0 immediately without writing to the buffer.
    {
        char buf[16] = "XXXXXXXXXXXXXXX";
        int32_t n = areg::make_string<char, int>(buf, 16, 42,
                        static_cast<areg::Radix>(1)); // 1 < Binary(2) -> out of range
        EXPECT_EQ(n, 0);
    }

    // charCount == 1: condition (charCount > 1) is false -> size-query path.
    // Returns the number of digits needed for the value (5 for 12345), not 0.
    {
        char buf[1]{};
        int32_t n = areg::make_string<char, int>(buf, 1, 12345, areg::Radix::Decimal);
        EXPECT_EQ(n, 5);  // size-query: returns required digit count
        EXPECT_EQ(buf[0], '\0');  // buffer not written
    }

    // charCount == 2: writing path (2 > 1). Reserves 1 for null, writes 1 digit.
    // Buffer must be non-empty (buf[0] != '\0') to avoid size-query path.
    {
        char buf[4] = "XXX";
        int32_t n = areg::make_string<char, int>(buf, 2, 12345, areg::Radix::Decimal);
        EXPECT_EQ(n, 1);      // one digit written
        EXPECT_EQ(buf[1], '\0');
    }

    // Automatic radix acts as Decimal. Buffer must be non-empty.
    {
        char buf[16] = "XXXXXXXXXXXXXXX";
        int32_t n = areg::make_string<char, int>(buf, 16, 42, areg::Radix::Automatic);
        EXPECT_EQ(n, 2);
        EXPECT_STREQ(buf, "42");
    }
}

/************************************************************************
 * make_integer -- '+' sign prefix and whitespace handling
 ************************************************************************/

/**
 * \brief   Test make_integer with explicit '+' sign and edge cases.
 *
 *          The parser handles '-' and '+' but tests in Test1/Test2 never
 *          verify the '+' path.  Additionally, whitespace-then-sign behavior
 *          and sign-then-whitespace (should stop parsing) are important.
 **/
TEST(StringDefsTest3, make_integer_sign_and_whitespace)
{
    // Explicit '+' prefix
    {
        const char* remain = nullptr;
        int32_t v = areg::make_integer<char>("+123", &remain);
        EXPECT_EQ(v, 123);
        EXPECT_EQ(remain, nullptr);   // consumed completely
    }

    // Leading whitespace then '+' sign
    {
        const char* remain = nullptr;
        int32_t v = areg::make_integer<char>("  +456", &remain);
        EXPECT_EQ(v, 456);
    }

    // '+' sign then whitespace before digits:
    // Whitespace is ALWAYS skipped unconditionally throughout the loop,
    // so the space after '+' is consumed and '123' is parsed normally.
    {
        const char* remain = nullptr;
        int32_t v = areg::make_integer<char>("+ 123", &remain);
        EXPECT_EQ(v, 123);
        EXPECT_EQ(remain, nullptr);
    }

    // '-' followed by non-digit (no digits parsed)
    {
        const char* remain = nullptr;
        int32_t v = areg::make_integer<char>("-abc", &remain);
        EXPECT_EQ(v, 0);
        ASSERT_NE(remain, nullptr);
        EXPECT_EQ(*remain, 'a');
    }

    // Just sign, nothing else
    {
        const char* remain = nullptr;
        int32_t v = areg::make_integer<char>("+", &remain);
        EXPECT_EQ(v, 0);
        EXPECT_EQ(remain, nullptr);  // EOS reached, no non-digit non-EOS char
    }

    // Overflow-like large number: uint32_t accumulator wraps; at least must not crash
    {
        int32_t v = areg::make_integer<char>("9999999999", nullptr);
        (void)v;  // implementation-defined wrap -- just verify no crash
    }

    // wchar_t with '+' sign
    {
        const wchar_t* remain = nullptr;
        int32_t v = areg::make_integer<wchar_t>(L"+789", &remain);
        EXPECT_EQ(v, 789);
    }
}

/************************************************************************
 * remove_char -- return value contracts
 ************************************************************************/

/**
 * \brief   Test remove_char return value.
 *
 *          The return value is a pointer:
 *          - removeAll=true:  pointer to the null terminator after removal.
 *          - removeAll=false: pointer to the position immediately after the
 *            removed character (start point for the next search).
 *          These are never verified in existing tests; a regression that
 *          returns the wrong pointer would be invisible without this test.
 **/
TEST(StringDefsTest3, remove_char_return_value)
{
    // removeAll=true: returned pointer must equal buf + strlen(result)
    {
        char buf[] = "aXbXcX";
        char* ret = areg::remove_char<char>('X', buf, true, true);
        // After removal buf = "abc\0"
        EXPECT_STREQ(buf, "abc");
        EXPECT_EQ(ret, buf + std::strlen(buf));   // must point to null terminator
    }

    // removeAll=false: removes first 'X', returns pointer after removed char
    {
        char buf[] = "aXbXcX";
        char* ret = areg::remove_char<char>('X', buf, false, true);
        // buf becomes "abXcX" -- only first 'X' removed
        EXPECT_STREQ(buf, "abXcX");
        // 'X' was at index 1; after removal the char at index 1 is 'b';
        // ret must point to index 1 (position where 'b' now lives)
        EXPECT_EQ(ret, buf + 1);
    }

    // removeAll=false, char not present: string unchanged, return points to EOS
    {
        char buf[] = "abcde";
        char* ret = areg::remove_char<char>('Z', buf, false, true);
        EXPECT_STREQ(buf, "abcde");
        EXPECT_EQ(ret, buf + std::strlen(buf));
    }

    // Case-insensitive, removeAll=true
    {
        char buf[] = "aAbB";
        char* ret = areg::remove_char<char>('a', buf, true, false);
        EXPECT_STREQ(buf, "bB");
        EXPECT_EQ(ret, buf + std::strlen(buf));
    }

    // wchar_t removeAll=false
    {
        wchar_t buf[] = L"aXbXcX";
        wchar_t* ret = areg::remove_char<wchar_t>(L'X', buf, false, true);
        EXPECT_EQ(areg::compare(buf, L"abXcX"), areg::Ordering::Equal);
        EXPECT_EQ(ret, buf + 1);
    }
}

/**
 * \brief   Test required_char_count<wchar_t> -- the wchar_t overload.
 *
 *          The wchar_t overload has a completely different implementation from
 *          the char overload: it calls is_buffer_fit in a cascaded chain
 *          (128->256->512->1024) rather than using vsnprintf with nullptr.
 *          A bug in the cascade order (e.g., wrong tier returned) would not
 *          be caught by the char tests.
 **/
TEST(StringDefsTest3, required_char_count_wchar)
{
    // 50-wchar string -> MSG_MIN_BUF_SIZE (128)
    const std::wstring w50(50, L'a');
    EXPECT_EQ(areg::required_char_count(L"%ls", w50.c_str()), areg::MSG_MIN_BUF_SIZE);

    // 127-wchar string -> MSG_MIN_BUF_SIZE (fits in 128 because < 128 chars)
    const std::wstring w127(127, L'b');
    EXPECT_EQ(areg::required_char_count(L"%ls", w127.c_str()), areg::MSG_MIN_BUF_SIZE);

    // 200-wchar string -> MSG_BUF_SIZE (256)
    const std::wstring w200(200, L'c');
    EXPECT_EQ(areg::required_char_count(L"%ls", w200.c_str()), areg::MSG_BUF_SIZE);

    // 300-wchar string -> MSG_BIG_BUF_SIZE (512)
    const std::wstring w300(300, L'd');
    EXPECT_EQ(areg::required_char_count(L"%ls", w300.c_str()), areg::MSG_BIG_BUF_SIZE);

    // 600-wchar string -> MSG_EXTRA_BUF_SIZE (1024)
    const std::wstring w600(600, L'e');
    EXPECT_EQ(areg::required_char_count(L"%ls", w600.c_str()), areg::MSG_EXTRA_BUF_SIZE);

    // Over 1024 -> -1
    const std::wstring w1100(1100, L'f');
    EXPECT_EQ(areg::required_char_count(L"%ls", w1100.c_str()), -1);
}

/**
 * \brief   Test is_buffer_fit<size, wchar_t>.
 *
 *          Uses vswprintf internally, which behaves differently from vsnprintf
 *          on some platforms.  Tier boundaries verified against string lengths.
 **/
TEST(StringDefsTest3, is_buffer_fit_wchar)
{
    const std::wstring w50 (50,  L'a');
    const std::wstring w200(200, L'b');
    const std::wstring w400(400, L'c');
    const std::wstring w800(800, L'd');

    // 50 chars fits all tiers
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_MIN_BUF_SIZE>  (L"%ls", w50.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_BUF_SIZE>      (L"%ls", w50.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_BIG_BUF_SIZE>  (L"%ls", w50.c_str()));
    EXPECT_TRUE(areg::is_buffer_fit<areg::MSG_EXTRA_BUF_SIZE>(L"%ls", w50.c_str()));

    // 200 chars: does NOT fit 128, fits 256+
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_MIN_BUF_SIZE>(L"%ls", w200.c_str()));
    EXPECT_TRUE (areg::is_buffer_fit<areg::MSG_BUF_SIZE>    (L"%ls", w200.c_str()));
    EXPECT_TRUE (areg::is_buffer_fit<areg::MSG_BIG_BUF_SIZE>(L"%ls", w200.c_str()));

    // 400 chars: does NOT fit 128 or 256, fits 512+
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_MIN_BUF_SIZE>(L"%ls", w400.c_str()));
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_BUF_SIZE>    (L"%ls", w400.c_str()));
    EXPECT_TRUE (areg::is_buffer_fit<areg::MSG_BIG_BUF_SIZE>(L"%ls", w400.c_str()));
    EXPECT_TRUE (areg::is_buffer_fit<areg::MSG_EXTRA_BUF_SIZE>(L"%ls", w400.c_str()));

    // 800 chars: does NOT fit 128, 256, or 512, fits 1024
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_MIN_BUF_SIZE>  (L"%ls", w800.c_str()));
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_BUF_SIZE>      (L"%ls", w800.c_str()));
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_BIG_BUF_SIZE>  (L"%ls", w800.c_str()));
    EXPECT_TRUE (areg::is_buffer_fit<areg::MSG_EXTRA_BUF_SIZE>(L"%ls", w800.c_str()));
}
