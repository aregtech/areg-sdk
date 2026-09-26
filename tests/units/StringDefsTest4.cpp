/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StringDefsTest4.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan, Jiang Mingpeng
 * \brief       Areg Platform, Areg framework unit test file.
 *              Extends StringDefsTest.cpp / StringDefsTest2.cpp / StringDefsTest3.cpp
 *              with inputs none of them covers:
 *                  - strings far larger than any buffer constant declared in
 *                    StringDefs.hpp (an explicit "there is no MAX_STRING" proof),
 *                  - UTF-8 text: multi-byte Chinese code points and four-byte Emoji,
 *                    exercised through the byte-oriented API only.
 ************************************************************************/
/************************************************************************
 * Scope, and what this file deliberately does NOT assert.  Every statement
 * below is checked against the control flow of StringDefs.hpp, not assumed.
 *
 * 1) SIZE CONSTANTS.
 *    StringDefs.hpp declares no MAX_STRING / MAX_BUFFER / MAX_LEN constant that
 *    bounds the length of a string.  The only size constants it declares are
 *    MSG_MIN_BUF_SIZE, MSG_BUF_SIZE, MSG_BIG_BUF_SIZE and MSG_EXTRA_BUF_SIZE
 *    (128 / 256 / 512 / 1024).  required_char_count() walks those four tiers and
 *    returns -1 past the last one; is_buffer_fit<size> succeeds only while the
 *    formatted length is strictly less than size.  That is a ceiling of the
 *    formatting helper, not of a string -- see large_string_format_ceiling.
 *    Every other function takes an explicit length/capacity.
 *
 * 2) UTF-8 CLASSIFICATION.
 *    utf8_char_def() and every predicate built on it (is_letter, is_numeric,
 *    is_symbol, is_alphanumeric, is_delimited, is_upper, is_lower, is_whitespace,
 *    is_printable, ...) read areg::UTF8_256_Table at index (ch & 0xFF), valid only
 *    for ch in [-128, 255].  It is a 256-entry BYTE table, not a UTF-8 decoder: a
 *    multi-byte code point is seen as its separate bytes.  This file therefore does
 *    NOT assert any specific category for a byte that happens to be part of a
 *    multi-byte sequence; those values belong to the table and are pinned against
 *    it by StringDefsTest1.  Only what the control flow guarantees is asserted here.
 *
 * 3) CASE CONVERSION.
 *    make_lower / make_upper route through make_lower_char / make_upper_char, i.e.
 *    they read the same low-byte table.  Applied to UTF-8 they can rewrite a lead or
 *    continuation byte into an unrelated value and corrupt the byte stream.  Nothing
 *    here asserts a sensible result for UTF-8, on purpose.
 *
 * 4) CROSS-TYPE (char <-> wchar_t) CONVERSION.
 *    copy_string selects the raw-blit path when sizeof(CharDst) == sizeof(CharSrc)
 *    -- a SIZE test, not a type test -- and otherwise widens ONE BYTE AT A TIME via
 *    static_cast<CharDst>(*strSrc++).  It is not a Unicode transcode.  Multibyte
 *    cross-type behaviour is therefore left untested here; only same-type operations
 *    are asserted.
 *
 * 5) DESTINATION SIZING.
 *    The same-size copy_string() path copies min(dstSpace, srcLen) elements and then
 *    writes the terminator at strDst[result], so the destination must hold dstSpace + 1
 *    elements.  Every destination below is sized accordingly (dstSpace == size - 1).
 *
 * 6) All literals are written as explicit hexadecimal byte escapes so this file stays
 *    pure ASCII: it compiles identically regardless of source code page or locale.
 * 
 * 7) DISAMBIGUATION OF areg::compare().
 *    areg::compare(a, b) does not compile when both arguments have the SAME pointer type.
 *    MathDefs.hpp declares a one-parameter compare(const Type&, const Type&) and
 *    StringDefs.hpp declares compare(const CharLhs*, const CharRhs*); with equal argument
 *    types neither is more specialized, so the call is ambiguous
 *    ("compare(const char*, const char*)").  Every comparison below therefore names the
 *    string overload explicitly as areg::compare<char, char>(a, b).  The explicit template
 *    arguments make the one-parameter MathDefs template ineligible and leave exactly the
 *    StringDefs overload, whose defaults keep the original meaning
 *    (charCount = COUNT_ALL, caseSensitive = true).  find_first / find_last / copy_string /
 *    trim_* are either uniquely named or take a count argument, so they need no
 *    disambiguation.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"

#include "areg/base/StringDefs.hpp"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

/************************************************************************
 * Byte sequences used by the tests, kept in an anonymous namespace.
 ************************************************************************/
namespace
{
    //  code point      UTF-8 bytes        bytes
    //  -------------------------------------------------
    //  U+4E2D           E4 B8 AD           3      中
    //  U+6587           E6 96 87           3      文
    //  U+4F60           E4 BD A0           3      你
    //  U+597D           E5 A5 BD           3      好
    //  U+1F600          F0 9F 98 80        4      (grinning face, above the BMP)
    //  U+1F44D          F0 9F 91 8D        4      (thumbs up, above the BMP)
    constexpr char UTF8_ZHONG      []{ "\xE4\xB8\xAD" };                 // 中
    constexpr char UTF8_WEN        []{ "\xE6\x96\x87" };                 // 文
    constexpr char UTF8_ZHONG_WEN  []{ "\xE4\xB8\xAD\xE6\x96\x87" };     // 中文  (6 bytes, 2 code points)
    constexpr char UTF8_NI_HAO     []{ "\xE4\xBD\xA0\xE5\xA5\xBD" };     // 你好  (6 bytes, 2 code points)
    constexpr char UTF8_EMOJI_GRIN []{ "\xF0\x9F\x98\x80" };             // 😄emoji笑   1 code point, 4 bytes
    constexpr char UTF8_EMOJI_UP   []{ "\xF0\x9F\x91\x8D" };             // 👍emoji点赞   1 code point, 4 bytes

    // One 'A', one three-byte Chinese code point and one four-byte Emoji: 8 bytes.
    constexpr char MIXED_UNIT      []{ "A" "\xE4\xB8\xAD" "\xF0\x9F\x98\x80" };

    // A one-byte ASCII delimiter used between multi-byte sequences.
    constexpr char MARKER{ ',' };
}

/************************************************************************
 * Large strings -- proof that no fixed upper limit exists.
 ************************************************************************/

/**
 * \brief   Length, comparison, truncating copy and in-place rewrite of a one
 *          mebibyte string.
 *
 *          The four MSG_*_BUF_SIZE constants do not apply to these functions;
 *          this test drives a string far beyond all of them.
 **/
TEST(StringDefsTest4, large_string_length_compare_copy_find)
{
    constexpr uint32_t SIZE{ 1024u * 1024u };   // 1 MB


    // Use heap allocation (std::string) to avoid stack overflow; 
    //explicitly avoids any MAX_BUFFER_SIZE limitation.
    std::string big(SIZE, 'a');

    // Plant sentinel bytes at the boundaries to ensure the length/comparison walks the entire buffer
    big[0]        = 'S';                        // distinguishable first byte
    big[SIZE - 1] = 'E';                        // dis last byte

    //length is reported in bytes and is exact
    EXPECT_EQ(areg::string_length<char>(big.c_str()), static_cast<areg::CharCount>(SIZE));
    EXPECT_FALSE(areg::is_empty<char>(big.c_str()));

    //comparison walks the whole buffer
    EXPECT_EQ((areg::compare<char, char>(big.c_str(), big.c_str())), areg::Ordering::Equal);

    std::string almost(big);
    almost[SIZE - 1] = 'X';
    EXPECT_NE((areg::compare<char, char>(big.c_str(), almost.c_str())), areg::Ordering::Equal);

    //compare_fast with an explicit count compares count * sizeof(char) bytes.
    EXPECT_NE(areg::compare_fast<char>(big.c_str(), almost.c_str(), static_cast<areg::CharCount>(SIZE)),
              areg::Ordering::Equal);

    //search reaches the far end
    EXPECT_EQ(areg::find_first<char>('E', big.c_str()), static_cast<areg::CharPos>(SIZE - 1));
    EXPECT_EQ(areg::find_last<char> ('S', big.c_str()), static_cast<areg::CharPos>(0));
    EXPECT_EQ(areg::find_first<char>('Z', big.c_str()), areg::INVALID_POS);

    //        truncating copy 
    // copy_string copies min(dstSpace, srcLen) elements and writes the terminator at
    // strDst[result], so the destination must hold dstSpace + 1 elements.  Here
    // dstSpace = 32 and the buffer is 64, which leaves room for the terminator.
    {
        char dst[64]{};// dst = 32, but buffer = 64 to safely hold the null terminator written at dst[result].
        areg::CharCount n = areg::copy_string<char, char>(dst, 32, big.c_str(), areg::COUNT_ALL);
        EXPECT_EQ(n, static_cast<areg::CharCount>(32));
        EXPECT_EQ(dst[32], '\0');
        EXPECT_EQ(std::strncmp(dst, big.c_str(), 32), 0);
    }

    //    full copy of the whole buffer
    // copy_string_fast writes strDst[charsCopy], i.e. it needs length + 1 elements
    // and does not bound the destination itself.
    {
        std::vector<char> dst(SIZE + 1u, 0);
        areg::CharCount n = areg::copy_string_fast<char>(dst.data(), big.c_str(), areg::COUNT_ALL);
        EXPECT_EQ(n, static_cast<areg::CharCount>(SIZE));
        EXPECT_EQ((areg::compare<char, char>(dst.data(), big.c_str())), areg::Ordering::Equal);
    }

    //in-place reverse of the whole buffer
    {
        std::vector<char> buf(big.begin(), big.end());
        buf.push_back('\0');
        areg::revert_string<char>(buf.data());

        EXPECT_EQ(buf[0], 'E');
        EXPECT_EQ(buf[SIZE - 1], 'S');
        EXPECT_EQ(areg::string_length<char>(buf.data()), static_cast<areg::CharCount>(SIZE));
    }

    //removing a single, rare character must not disturb the rest
    // A character that matches everywhere would cost O(n^2); one match keeps the
    // test fast while still proving the tail is shifted correctly.
    {
        std::vector<char> buf(big.begin(), big.end());
        buf[SIZE / 2u] = 'Q';                   // the only occurrence
        buf.push_back('\0');

        // Keep only one match to avoid O(n^2) worst-case behavior during shifting.
        areg::remove_char<char>('Q', buf.data(), true, true);

        EXPECT_EQ(areg::string_length<char>(buf.data()), static_cast<areg::CharCount>(SIZE - 1u));
        EXPECT_EQ(buf[0], 'S');
        EXPECT_EQ(buf[SIZE / 2u], 'a');         // the gap closed up
        EXPECT_EQ(buf[SIZE - 2u], 'E');         // the last byte moved down by one
    }
}

/**
 * \brief   Pin the one size ceiling that StringDefs.hpp really has.
 *
 *          required_char_count() checks 128, 256, 512 and 1024: a formatted result
 *          of N characters fits a tier of size T exactly when N < T, and a result
 *          that fits no tier is reported as -1 (is_buffer_fit -> false).
 *          This is a property of the formatting helper, not a limit on strings.
 **/
TEST(StringDefsTest4, large_string_format_ceiling)
{
    const std::string s1000(1000, 'x');     // below the ceiling

    // 1023 is the last size that fits in 1024 (length < buffer size)  1024 exceeds it.
    const std::string s1023(1023, 'y');     // last size that still fits 1024
    const std::string s1024(1024, 'z');     // first size that does not

    const std::string s2000(2000, 'w');     // far beyond

    EXPECT_EQ(areg::required_char_count("%s", s1000.c_str()), areg::MSG_EXTRA_BUF_SIZE);
    EXPECT_EQ(areg::required_char_count("%s", s1023.c_str()), areg::MSG_EXTRA_BUF_SIZE);

    EXPECT_EQ(areg::required_char_count("%s", s1024.c_str()), -1);
    EXPECT_EQ(areg::required_char_count("%s", s2000.c_str()), -1);

    EXPECT_TRUE (areg::is_buffer_fit<areg::MSG_EXTRA_BUF_SIZE>("%s", s1023.c_str()));
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_EXTRA_BUF_SIZE>("%s", s1024.c_str()));
    EXPECT_FALSE(areg::is_buffer_fit<areg::MSG_EXTRA_BUF_SIZE>("%s", s2000.c_str()));
}

/************************************************************************
 * UTF-8: multi-byte Chinese code points
 ************************************************************************/

/**
 * \brief   A multi-byte string is counted in BYTES; ASCII classification inside it
 *          is unaffected; a BMP string is one wchar_t per code point.
 *
 *          There is no UTF-8 decoding anywhere in StringDefs.hpp, so a caller must
 *          not expect a code point count from string_length<char>.
 **/
TEST(StringDefsTest4, utf8_chinese_length_is_bytes)
{
    // Six bytes, and the C library agrees.
    EXPECT_EQ(areg::string_length<char>(UTF8_ZHONG_WEN), static_cast<areg::CharCount>(6));
    EXPECT_EQ(std::strlen(UTF8_ZHONG_WEN), 6u);
    EXPECT_FALSE(areg::is_empty<char>(UTF8_ZHONG_WEN));

    // Chinese lies entirely in the BMP, so every code point is a single wchar_t on
    // every supported platform: two.
    EXPECT_EQ(areg::string_length<wchar_t>(L"\u4E2D\u6587"), static_cast<areg::CharCount>(2));

    // ASCII classification is unaffected by the presence of multi-byte neighbours,
    // because every predicate takes exactly one code unit.
    EXPECT_TRUE (areg::is_numeric<char>('7'));
    EXPECT_FALSE(areg::is_letter<char>('7'));
    EXPECT_TRUE (areg::is_letter<char>('Z'));
    EXPECT_TRUE (areg::is_upper<char>('Z'));
    EXPECT_FALSE(areg::is_upper<char>('z'));

    // The predicates read areg::UTF8_256_Table by the low byte only, so a byte value
    // classifies the same no matter where it appears: a multi-byte sequence is seen
    // as its individual bytes and never as one character.  The exact category of a
    // byte such as 0xE4 belongs to that table and is pinned by StringDefsTest1, so it
    // is not re-guessed here.
}

/**
 * \brief   Copy, compare and trim of UTF-8 Chinese text.
 *
 *          Comparison is byte-lexicographic (compare() uses strcmp/strncmp for char),
 *          so ordering is decided by the UTF-8 bytes and not by any locale collation.
 *          Trimming is tested only where the boundary characters are ASCII, so the
 *          result does not depend on the white-space mask of the byte table.
 **/
TEST(StringDefsTest4, utf8_chinese_copy_compare_trim)
{
    //  copy round trip preserves the exact bytes 
    {
        char dst[32]{};
        areg::CharCount n = areg::copy_string<char, char>(dst, sizeof(dst) - 1, UTF8_ZHONG_WEN, areg::COUNT_ALL);
        EXPECT_EQ(n, static_cast<areg::CharCount>(6));
        EXPECT_EQ((areg::compare<char, char>(dst, UTF8_ZHONG_WEN)), areg::Ordering::Equal);
    }

    //   comparison is byte-lexicographic  
    // 中 = E4 B8 AD, 你 = E4 BD A0: first byte equal, second byte B8 < BD.
    EXPECT_EQ((areg::compare<char, char>(UTF8_ZHONG_WEN, UTF8_ZHONG_WEN)), areg::Ordering::Equal);
    EXPECT_EQ((areg::compare<char, char>(UTF8_ZHONG_WEN, UTF8_NI_HAO)), areg::Ordering::Smaller);

    //   phrase search on a code point boundary: one present, one absent  
    constexpr char ZHONG_WEN_XYZ[] { "\xE4\xB8\xAD\xE6\x96\x87xyz" };   // 中文xyz
    EXPECT_EQ(areg::find_first<char>(UTF8_ZHONG_WEN, ZHONG_WEN_XYZ), static_cast<areg::CharPos>(0));
    EXPECT_EQ(areg::find_first<char>(UTF8_NI_HAO,    ZHONG_WEN_XYZ), areg::INVALID_POS);
    // An ASCII phrase found *after* a multi-byte prefix still reports a byte offset.
    EXPECT_EQ(areg::find_first<char>("xyz", ZHONG_WEN_XYZ), static_cast<areg::CharPos>(6));

    //   trim removes surrounding ASCII spaces and leaves the middle alone  
    // The multi-byte sequences sit between the ASCII letters 'X' and 'Y', so the trim
    // stops on bytes it definitely recognises and never has to classify a UTF-8 byte.
    // Result: "X中文Y", 8 bytes.
    {
        char buf[] = "  X\xE4\xB8\xAD\xE6\x96\x87Y  ";
        areg::trim_all<char>(buf);
        EXPECT_EQ((areg::compare<char, char>(buf, "X\xE4\xB8\xAD\xE6\x96\x87Y")), areg::Ordering::Equal);
        EXPECT_EQ(areg::string_length<char>(buf), static_cast<areg::CharCount>(8));
    }

    //   a line break is an ASCII byte and is found wherever it sits  
    // string_line_length<char> uses strcspn(s, "\r\n").
    {
        constexpr char TWO_LINES[] { "\xE4\xB8\xAD\xE6\x96\x87\n\xE4\xBD\xA0\xE5\xA5\xBD" };  // 中文 \n 你好
        EXPECT_EQ(areg::string_line_length<char>(TWO_LINES), static_cast<areg::CharCount>(6));
    }
}

/**
 * \brief   Searching a marker byte between multi-byte sequences, and removing it.
 *
 *          Offsets returned by find_first/find_last are BYTE offsets, not code point
 *          indices.  Note also that find_first/find_last reject a '\0' search value
 *          (they return INVALID_POS), so the terminator can never be located this way.
 **/
TEST(StringDefsTest4, utf8_chinese_marker_search_and_remove)
{
    // 中(0..2) 文(3..5) ,(6) 你(7..9) 好(10..12): 13 bytes, marker at offset 6.
    constexpr char TEXT[] { "\xE4\xB8\xAD\xE6\x96\x87,\xE4\xBD\xA0\xE5\xA5\xBD" };

    EXPECT_EQ(areg::string_length<char>(TEXT), static_cast<areg::CharCount>(13));
    EXPECT_EQ(areg::find_first<char>(MARKER, TEXT), static_cast<areg::CharPos>(6));
    EXPECT_EQ(areg::find_last<char> (MARKER, TEXT), static_cast<areg::CharPos>(6));

    // A whole multi-byte phrase starting on a code point boundary.
    EXPECT_EQ(areg::find_first<char>(UTF8_NI_HAO, TEXT), static_cast<areg::CharPos>(7));

    // A '\0' search value is rejected, not located at the terminator.
    EXPECT_EQ(areg::find_first<char>('\0', TEXT), areg::INVALID_POS);

    //   removing the marker must leave every multi-byte sequence intact  
    {
        char buf[32]{};
        areg::copy_string<char, char>(buf, sizeof(buf) - 1, TEXT, areg::COUNT_ALL);
        areg::remove_char<char>(MARKER, buf, true, true);

        EXPECT_EQ(areg::string_length<char>(buf), static_cast<areg::CharCount>(12));
        EXPECT_EQ((areg::compare<char, char>(buf, "\xE4\xB8\xAD\xE6\x96\x87\xE4\xBD\xA0\xE5\xA5\xBD")), areg::Ordering::Equal);
    }

    //   revert_string reverses BYTES, not code points  
    // The result is deliberately not valid UTF-8; that is the byte-oriented contract
    // of the function, pinned here so a later switch to code point reversal cannot
    // slip through unnoticed.
    {
        char buf[] = "\xE4\xB8\xAD\xE6\x96\x87";                        // 中文
        areg::revert_string<char>(buf);
        EXPECT_EQ((areg::compare<char, char>(buf, "\x87\x96\xE6\xAD\xB8\xE4")), areg::Ordering::Equal);
    }
}

/************************************************************************
 * UTF-8: four-byte Emoji (code points above the BMP)
 ************************************************************************/

/**
 * \brief   Byte-oriented operations on Emoji, and the platform-dependent wide
 *          length.
 *
 *          An Emoji is one code point, four UTF-8 bytes and no ASCII byte at all, so
 *          it is the strongest case for the "bytes, not characters" rule.  The wide
 *          length is not portable: Windows wchar_t is UTF-16 (a surrogate pair, two
 *          code units), POSIX wchar_t is usually UTF-32 (one code unit).
 **/
TEST(StringDefsTest4, utf8_emoji_byte_ops)
{
    //   one Emoji is four bytes  
    EXPECT_EQ(areg::string_length<char>(UTF8_EMOJI_GRIN), static_cast<areg::CharCount>(4));
    EXPECT_EQ(areg::string_length<char>(UTF8_EMOJI_UP),   static_cast<areg::CharCount>(4));

    //   copy round trip  
    {
        char dst[16]{};
        areg::CharCount n = areg::copy_string<char, char>(dst, sizeof(dst) - 1, UTF8_EMOJI_GRIN, areg::COUNT_ALL);
        EXPECT_EQ(n, static_cast<areg::CharCount>(4));
        EXPECT_EQ((areg::compare<char, char>(dst, UTF8_EMOJI_GRIN)), areg::Ordering::Equal);
    }

    //   two different Emoji are two different byte strings  
    {
        char both[16]{};
        areg::copy_string<char, char>(both,        8, UTF8_EMOJI_GRIN, areg::COUNT_ALL);
        areg::copy_string<char, char>(both + 4,    8, UTF8_EMOJI_UP,   areg::COUNT_ALL);

        EXPECT_EQ(areg::string_length<char>(both), static_cast<areg::CharCount>(8));
        EXPECT_NE((areg::compare<char, char>(both, UTF8_EMOJI_GRIN)), areg::Ordering::Equal);

        EXPECT_EQ(areg::find_first<char>(UTF8_EMOJI_UP,   both), static_cast<areg::CharPos>(4));
        EXPECT_EQ(areg::find_last<char> (UTF8_EMOJI_GRIN, both), static_cast<areg::CharPos>(0));
    }

    //   iterating matches through the 'next' output  
    // The buffer is short on purpose: walking a full 1 MiB buffer one match at a time
    // is O(n^2), a property of the search API a caller must respect.
    {
        constexpr char MANY[] { "\xF0\x9F\x98\x80\xF0\x9F\x98\x80\xF0\x9F\x98\x80" };  // three Emoji
        const char* next{ nullptr };
        uint32_t found{ 0u };

        areg::CharPos pos = areg::find_first<char>(UTF8_EMOJI_GRIN, MANY, areg::START_POS, true, &next);
        while (pos != areg::INVALID_POS)
        {
            ++found;
            pos = areg::find_first<char>(UTF8_EMOJI_GRIN, next, areg::START_POS, true, &next);
        }

        EXPECT_EQ(found, 3u);
        EXPECT_EQ(next, nullptr);
    }

    //   the wide length is platform dependent  
    // Windows: UTF-16, the code point is a surrogate pair -> 2 code units.
    // POSIX with 4-byte wchar_t: UTF-32 -> 1 code unit.
    {
#if defined(_WIN32)
        EXPECT_EQ(areg::string_length<wchar_t>(L"\U0001F600"), static_cast<areg::CharCount>(2));
#else
        EXPECT_EQ(areg::string_length<wchar_t>(L"\U0001F600"), static_cast<areg::CharCount>(1));
#endif  // defined(_WIN32)
    }
}

/************************************************************************
 * Everything at once: a large string built from ASCII, Chinese and Emoji
 ************************************************************************/

/**
 * \brief   A large mixed-encoding string, exercising length, comparison, search
 *          and copy in one buffer.
 *
 *          Each 8-byte unit is 'A' + 中 + 😀.  The offsets below are byte offsets,
 *          the only unit the byte-oriented API knows about.
 **/
TEST(StringDefsTest4, mixed_large_utf8_and_emoji)
{
    constexpr uint32_t UNITS     { 100000u };                       // 100 000 units
    constexpr uint32_t UNIT_SIZE { 8u };                            // 'A'(1) + 中(3) + Emoji(4)
    constexpr uint32_t TOTAL     { UNITS * UNIT_SIZE };             // 800 000 bytes

    std::string mixed;
    mixed.reserve(TOTAL);
    for (uint32_t i = 0u; i < UNITS; ++i)
        mixed.append(MIXED_UNIT);

    ASSERT_EQ(mixed.size(), TOTAL);

    //   length  
    EXPECT_EQ(areg::string_length<char>(mixed.c_str()), static_cast<areg::CharCount>(TOTAL));

    //   search: the Emoji occupies bytes 4..7 of every unit  
    EXPECT_EQ(areg::find_first<char>(UTF8_EMOJI_GRIN, mixed.c_str()), static_cast<areg::CharPos>(4));
    EXPECT_EQ(areg::find_last<char> (UTF8_EMOJI_GRIN, mixed.c_str()), static_cast<areg::CharPos>(TOTAL - UNIT_SIZE + 4u));
    EXPECT_EQ(areg::find_last<char> ('A', mixed.c_str()), static_cast<areg::CharPos>(TOTAL - UNIT_SIZE));

    //   comparison over the whole mixed buffer  
    {
        std::string same(mixed);
        EXPECT_EQ((areg::compare<char, char>(mixed.c_str(), same.c_str())), areg::Ordering::Equal);

        // Flip one byte that belongs to a Chinese sequence in the middle unit.
        std::string diff(mixed);
        diff[(UNITS / 2u) * UNIT_SIZE + 1u] = 'Z';// Corrupt the second byte of a Chinese sequence 
                                                  //in the middle unit to force early exit on compare
        EXPECT_NE((areg::compare<char, char>(mixed.c_str(), diff.c_str())), areg::Ordering::Equal);
    }

    //   full copy preserves every byte  
    // dstSpace is the buffer size minus one, per the destination-sizing rule above.
    {
        std::vector<char> dst(TOTAL + 1u, 0);
        areg::CharCount n = areg::copy_string<char, char>(dst.data(),
                                                          static_cast<areg::CharCount>(TOTAL),
                                                          mixed.c_str(),
                                                          areg::COUNT_ALL);
        EXPECT_EQ(n, static_cast<areg::CharCount>(TOTAL));
        EXPECT_EQ((areg::compare<char, char>(dst.data(), mixed.c_str())), areg::Ordering::Equal);
    }
}