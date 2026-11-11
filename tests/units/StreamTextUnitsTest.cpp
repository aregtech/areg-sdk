/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/StreamTextUnitsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the text units conversion of the stream: count_units(),
 *              encode(), decode(), and write_text() / read_text() through a stream with and
 *              without direct access to its memory.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/SharedBuffer.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace
{
    constexpr char32_t  EMOJI       { 0x1F600u };
    constexpr uint16_t  EMOJI_HIGH  { 0xD83Du };
    constexpr uint16_t  EMOJI_LOW   { 0xDE00u };
    constexpr uint32_t  HEAD        { 4u };

    //! A memory stream. Direct access to the memory, a limit of the written and of the read bytes are switchable.
    class MemoryStream : public areg::IOStream
    {
    public:
        explicit MemoryStream(bool views) : mViews(views) { }

        std::vector<uint8_t>    mData;
        mutable uint32_t        mPos    { 0u };
        bool                    mViews  { true };
        uint32_t                mWriteLimit { 0xFFFFFFFFu };
        uint32_t                mReadCap    { 0xFFFFFFFFu };

        uint32_t read(uint8_t* buffer, uint32_t size) const noexcept override
        {
            const uint32_t count{ std::min(std::min(size, mReadCap), size_readable()) };
            if (count != 0u)
                std::memcpy(buffer, mData.data() + mPos, count);
            mPos += count;
            return count;
        }

        uint32_t read(areg::SharedBuffer&) const override { return 0u; }
        void reset() const noexcept override { mPos = 0u; }

        uint32_t write(const uint8_t* buffer, uint32_t size) override
        {
            const uint32_t count{ std::min(size, mWriteLimit) };
            mData.insert(mData.end(), buffer, buffer + count);
            mWriteLimit -= count;
            return count;
        }

        uint32_t write(const areg::SharedBuffer&) override { return 0u; }
        void flush() noexcept override { }
        bool ensure_size(uint32_t) override { return true; }

        uint32_t size_readable() const noexcept override { return static_cast<uint32_t>(mData.size()) - mPos; }

        const uint8_t* read_view(uint32_t size) const noexcept override
        {
            if ((mViews == false) || (size > size_readable()))
                return nullptr;
            const uint8_t* result{ mData.data() + mPos };
            mPos += size;
            return result;
        }

        uint8_t* write_view(uint32_t size) noexcept override
        {
            if ((mViews == false) || (size > mWriteLimit))
                return nullptr;
            const size_t at{ mData.size() };
            mData.resize(at + size);
            mWriteLimit -= size;
            return mData.data() + at;
        }
    };

    std::vector<uint8_t> to_bytes(const std::vector<uint16_t>& units)
    {
        std::vector<uint8_t> result(units.size() * 2u);
        for (size_t i = 0; i < units.size(); ++i)
            std::memcpy(result.data() + i * 2u, &units[i], 2u);
        return result;
    }

    std::u32string make_text(uint32_t length, uint32_t emojiAt = 0xFFFFFFFFu)
    {
        std::u32string text(length, U'a');
        for (uint32_t i = 0; i < length; ++i)
            text[i] = U'a' + static_cast<char32_t>(i % 20u);
        if (emojiAt < length)
            text[emojiAt] = EMOJI;
        return text;
    }

    //! Writes the text and reads it back, returns the read string.
    std::u32string round_trip(const std::u32string& text, bool writeViews, bool readViews, uint32_t* written = nullptr, uint32_t* read = nullptr)
    {
        MemoryStream stream(writeViews);
        const uint32_t w{ stream.write_text(text.data(), static_cast<uint32_t>(text.size())) };
        stream.mViews = readViews;
        stream.reset();
        std::u32string result;
        const uint32_t r{ stream.read_text(result) };
        if (written != nullptr)
            *written = w;
        if (read != nullptr)
            *read = r;
        return result;
    }
}

/**
 * \brief   Test count_units(): the simple flag and the number of units.
 **/
TEST(StreamTextUnitsTest, count_units_counts_surrogate_pairs)
{
    bool simple{ false };
    const char32_t plain[]{ U'a', U'b', 0xFFFFu };
    EXPECT_EQ(areg::text_units::count_units(plain, 3u, simple), 3u);
    EXPECT_TRUE(simple);

    const char32_t mixed[]{ U'a', EMOJI, 0x10000u, 0x10FFFFu };
    EXPECT_EQ(areg::text_units::count_units(mixed, 4u, simple), 7u);
    EXPECT_FALSE(simple);

    const char32_t invalid[]{ U'a', 0x110000u, 0xFFFFFFFFu };
    EXPECT_EQ(areg::text_units::count_units(invalid, 3u, simple), 3u);
    EXPECT_FALSE(simple);

    EXPECT_EQ(areg::text_units::count_units(plain, 0u, simple), 0u);
    EXPECT_TRUE(simple);
}

/**
 * \brief   Test encode() of characters in the BMP, with and without the simple flag.
 **/
TEST(StreamTextUnitsTest, encode_writes_utf16_units)
{
    const char32_t text[]{ U'a', 0x20ACu, 0xFFFFu, 0xD800u };
    const std::vector<uint8_t> expected{ to_bytes({ 0x0061u, 0x20ACu, 0xFFFFu, 0xD800u }) };
    for (bool simple : { true, false })
    {
        uint8_t out[8]{ };
        EXPECT_EQ(areg::text_units::encode(text, 4u, out, simple), 8u);
        EXPECT_EQ(std::memcmp(out, expected.data(), 8u), 0);
    }
}

/**
 * \brief   Test encode() of the characters above U+FFFF, the first and the last valid one, and the invalid ones.
 **/
TEST(StreamTextUnitsTest, encode_writes_surrogate_pairs_and_replaces_invalid)
{
    const char32_t text[]{ EMOJI, 0x10000u, 0x10FFFFu, 0x110000u, 0xFFFFFFFFu, U'z' };
    const std::vector<uint8_t> expected{ to_bytes({ EMOJI_HIGH, EMOJI_LOW, 0xD800u, 0xDC00u, 0xDBFFu, 0xDFFFu, 0xFFFDu, 0xFFFDu, 0x007Au }) };
    uint8_t out[18]{ };
    EXPECT_EQ(areg::text_units::encode(text, 6u, out, false), 18u);
    EXPECT_EQ(std::memcmp(out, expected.data(), 18u), 0);
}

/**
 * \brief   Test encode() writes to an address of any alignment and writes nothing for no characters.
 **/
TEST(StreamTextUnitsTest, encode_accepts_unaligned_destination)
{
    const char32_t text[]{ EMOJI, U'a' };
    uint8_t out[8]{ };
    EXPECT_EQ(areg::text_units::encode(text, 2u, out + 1, false), 6u);
    const std::vector<uint8_t> expected{ to_bytes({ EMOJI_HIGH, EMOJI_LOW, 0x0061u }) };
    EXPECT_EQ(std::memcmp(out + 1, expected.data(), 6u), 0);
    EXPECT_EQ(areg::text_units::encode(text, 0u, out, true), 0u);
    EXPECT_EQ(areg::text_units::encode(text, 0u, out, false), 0u);
}

/**
 * \brief   Test decode() of units without surrogates.
 **/
TEST(StreamTextUnitsTest, decode_copies_units_without_surrogates)
{
    const std::vector<uint8_t> src{ to_bytes({ 0x0061u, 0x20ACu, 0xFFFFu, 0xD7FFu, 0xE000u }) };
    char32_t dst[5]{ };
    uint32_t high{ 0u };
    EXPECT_EQ(areg::text_units::decode(src.data(), 5u, dst, high), 5u);
    EXPECT_EQ(high, 0u);
    EXPECT_EQ(dst[0], U'a');
    EXPECT_EQ(dst[1], 0x20ACu);
    EXPECT_EQ(dst[2], 0xFFFFu);
    EXPECT_EQ(dst[3], 0xD7FFu);
    EXPECT_EQ(dst[4], 0xE000u);
}

/**
 * \brief   Test decode() combines a surrogate pair, also from an address of any alignment.
 **/
TEST(StreamTextUnitsTest, decode_combines_surrogate_pairs)
{
    const std::vector<uint8_t> pairs{ to_bytes({ 0x0061u, EMOJI_HIGH, EMOJI_LOW, 0xD800u, 0xDC00u, 0xDBFFu, 0xDFFFu, 0x0062u }) };
    std::vector<uint8_t> shifted(pairs.size() + 1u);
    std::memcpy(shifted.data() + 1, pairs.data(), pairs.size());
    const uint8_t* const sources[]{ pairs.data(), shifted.data() + 1 };
    for (const uint8_t* src : sources)
    {
        char32_t dst[8]{ };
        uint32_t high{ 0u };
        EXPECT_EQ(areg::text_units::decode(src, 8u, dst, high), 5u);
        EXPECT_EQ(high, 0u);
        EXPECT_EQ(dst[0], U'a');
        EXPECT_EQ(dst[1], EMOJI);
        EXPECT_EQ(dst[2], 0x10000u);
        EXPECT_EQ(dst[3], 0x10FFFFu);
        EXPECT_EQ(dst[4], U'b');
    }
}

/**
 * \brief   Test decode() keeps a lone surrogate as it is when no valid partner follows.
 **/
TEST(StreamTextUnitsTest, decode_keeps_lone_surrogates)
{
    const std::vector<uint8_t> src{ to_bytes({ 0xDC00u, 0x0061u, 0xD800u, 0x0062u, 0xD800u, 0xD800u, 0xDC00u }) };
    char32_t dst[7]{ };
    uint32_t high{ 0u };
    EXPECT_EQ(areg::text_units::decode(src.data(), 7u, dst, high), 6u);
    EXPECT_EQ(high, 0u);
    EXPECT_EQ(dst[0], 0xDC00u);
    EXPECT_EQ(dst[1], U'a');
    EXPECT_EQ(dst[2], 0xD800u);
    EXPECT_EQ(dst[3], U'b');
    EXPECT_EQ(dst[4], 0xD800u);
    EXPECT_EQ(dst[5], 0x10000u);
}

/**
 * \brief   Test decode() carries a high surrogate at the end of the units to the next call.
 **/
TEST(StreamTextUnitsTest, decode_carries_high_surrogate_to_next_call)
{
    const std::vector<uint8_t> first{ to_bytes({ 0x0061u, EMOJI_HIGH }) };
    const std::vector<uint8_t> second{ to_bytes({ EMOJI_LOW, 0x0062u }) };
    char32_t dst[4]{ };
    uint32_t high{ 0u };
    EXPECT_EQ(areg::text_units::decode(first.data(), 2u, dst, high), 1u);
    EXPECT_EQ(high, static_cast<uint32_t>(EMOJI_HIGH));
    EXPECT_EQ(dst[0], U'a');

    EXPECT_EQ(areg::text_units::decode(second.data(), 2u, dst, high), 2u);
    EXPECT_EQ(high, 0u);
    EXPECT_EQ(dst[0], EMOJI);
    EXPECT_EQ(dst[1], U'b');
}

/**
 * \brief   Test decode() writes a carried high surrogate as it is when the next unit is not a low surrogate.
 **/
TEST(StreamTextUnitsTest, decode_writes_carried_high_surrogate_before_other_units)
{
    const std::vector<uint8_t> second{ to_bytes({ 0x0062u, EMOJI_HIGH, EMOJI_LOW }) };
    char32_t dst[4]{ };
    uint32_t high{ EMOJI_HIGH };
    EXPECT_EQ(areg::text_units::decode(second.data(), 3u, dst, high), 3u);
    EXPECT_EQ(high, 0u);
    EXPECT_EQ(dst[0], static_cast<char32_t>(EMOJI_HIGH));
    EXPECT_EQ(dst[1], U'b');
    EXPECT_EQ(dst[2], EMOJI);
}

/**
 * \brief   Reproduces the defect: decode() with no units and a carried high surrogate read the unit
 *          behind the given ones as the low surrogate, wrote one character and lost the carried value.
 **/
TEST(StreamTextUnitsTest, decode_of_no_units_keeps_carried_high_surrogate)
{
    const std::vector<uint8_t> behind{ to_bytes({ EMOJI_LOW }) };
    char32_t dst[2]{ U'x', U'x' };
    uint32_t high{ EMOJI_HIGH };
    EXPECT_EQ(areg::text_units::decode(behind.data(), 0u, dst, high), 0u);
    EXPECT_EQ(high, static_cast<uint32_t>(EMOJI_HIGH));
    EXPECT_EQ(dst[0], U'x');
}

/**
 * \brief   Test write_text() and read_text() of the text with a character above U+FFFF.
 **/
TEST(StreamTextUnitsTest, text_with_surrogate_pair_round_trips_on_every_stream_kind)
{
    const std::u32string text{ U'a', EMOJI, U'b', 0x10000u, 0x10FFFFu };
    for (bool writeViews : { true, false })
    {
        for (bool readViews : { true, false })
        {
            uint32_t written{ 0u }, read{ 0u };
            EXPECT_EQ(round_trip(text, writeViews, readViews, &written, &read), text);
            EXPECT_EQ(written, HEAD + 8u * 2u);
            EXPECT_EQ(read, written);
        }
    }
}

/**
 * \brief   Test a surrogate pair at every position around the chunk borders of the writer (128 characters)
 *          and of the reader (256 units).
 **/
TEST(StreamTextUnitsTest, surrogate_pair_at_chunk_borders_round_trips)
{
    for (uint32_t at : { 0u, 126u, 127u, 128u, 129u, 253u, 254u, 255u, 256u, 257u, 298u, 299u })
    {
        const std::u32string text{ make_text(300u, at) };
        for (bool writeViews : { true, false })
        {
            for (bool readViews : { true, false })
            {
                uint32_t written{ 0u }, read{ 0u };
                EXPECT_EQ(round_trip(text, writeViews, readViews, &written, &read), text) << "pair at " << at;
                EXPECT_EQ(written, HEAD + 301u * 2u) << "pair at " << at;
                EXPECT_EQ(read, written) << "pair at " << at;
            }
        }
    }
}

/**
 * \brief   Test many surrogate pairs across several chunks.
 **/
TEST(StreamTextUnitsTest, text_of_only_surrogate_pairs_round_trips)
{
    for (uint32_t length : { 1u, 127u, 128u, 129u, 255u, 256u, 257u, 600u })
    {
        const std::u32string text(length, EMOJI);
        for (bool writeViews : { true, false })
        {
            for (bool readViews : { true, false })
            {
                EXPECT_EQ(round_trip(text, writeViews, readViews), text) << "length " << length;
            }
        }
    }
}

/**
 * \brief   Test lone surrogates in the text stay as they are, also when the high one is the last character
 *          of a chunk or of the text.
 **/
TEST(StreamTextUnitsTest, lone_surrogates_round_trip)
{
    for (uint32_t at : { 0u, 254u, 255u, 298u, 299u })
    {
        std::u32string high{ make_text(300u) };
        std::u32string low{ make_text(300u) };
        high[at] = 0xD83Du;
        low[at] = 0xDE00u;
        for (bool readViews : { true, false })
        {
            EXPECT_EQ(round_trip(high, readViews, readViews), high) << "high at " << at;
            EXPECT_EQ(round_trip(low, readViews, readViews), low) << "low at " << at;
        }
    }
}

/**
 * \brief   Test an invalid code point is replaced with U+FFFD.
 **/
TEST(StreamTextUnitsTest, invalid_code_points_are_replaced)
{
    const std::u32string text{ U'a', 0x110000u, 0xFFFFFFFFu, U'b' };
    const std::u32string expected{ U'a', 0xFFFDu, 0xFFFDu, U'b' };
    for (bool views : { true, false })
        EXPECT_EQ(round_trip(text, views, views), expected);
}

/**
 * \brief   Test an empty and a null text.
 **/
TEST(StreamTextUnitsTest, empty_and_null_text)
{
    for (bool views : { true, false })
    {
        MemoryStream stream(views);
        EXPECT_EQ(stream.write_text(static_cast<const char32_t*>(nullptr), 5u), HEAD);
        EXPECT_EQ(stream.write_text(U"abc", 0u), HEAD);
        stream.reset();
        std::u32string text{ U"previous" };
        EXPECT_EQ(stream.read_text(text), HEAD);
        EXPECT_TRUE(text.empty());
        text = U"previous";
        EXPECT_EQ(stream.read_text(text), HEAD);
        EXPECT_TRUE(text.empty());
    }
}

/**
 * \brief   Test write_text() rejects a count above the supported maximum before it reads the text.
 **/
TEST(StreamTextUnitsTest, write_text_rejects_count_above_maximum)
{
    const char32_t text[]{ U'a' };
    const char16_t text16[]{ u'a' };
    const char text8[]{ 'a' };
    for (bool views : { true, false })
    {
        MemoryStream stream(views);
        EXPECT_EQ(stream.write_text(text, 0x40000000u), 0u);
        EXPECT_EQ(stream.write_text(text16, 0x40000000u), 0u);
        EXPECT_EQ(stream.write_text(text8, 0x40000000u), 0u);
        EXPECT_EQ(stream.write_text(text, 0xFFFFFFFFu), 0u);
        EXPECT_TRUE(stream.mData.empty());
    }
}

/**
 * \brief   Test write_text() reports 0 when the header cannot be written and the written bytes when the units cannot.
 **/
TEST(StreamTextUnitsTest, write_text_reports_short_writes)
{
    const std::u32string text{ make_text(10u, 3u) };
    {
        MemoryStream stream(false);
        stream.mWriteLimit = 3u;
        EXPECT_EQ(stream.write_text(text.data(), 10u), 0u);
    }
    {
        MemoryStream stream(false);
        stream.mWriteLimit = HEAD + 6u;
        EXPECT_EQ(stream.write_text(text.data(), 10u), HEAD + 6u);
    }
    {
        MemoryStream stream(true);
        stream.mWriteLimit = HEAD + 6u;
        EXPECT_EQ(stream.write_text(text.data(), 10u), HEAD + 6u);
        EXPECT_EQ(stream.mData.size(), static_cast<size_t>(HEAD + 6u));
    }
}

/**
 * \brief   Test write_units() writes the units without header, for every character size, and nothing for no text.
 **/
TEST(StreamTextUnitsTest, write_units_writes_units_without_header)
{
    MemoryStream stream(false);
    const char32_t wide[]{ EMOJI, U'a' };
    EXPECT_EQ(stream.write_units(wide, 2u), 6u);
    EXPECT_EQ(stream.write_units(wide, 0u), 0u);
    EXPECT_EQ(stream.write_units(static_cast<const char32_t*>(nullptr), 2u), 0u);
    const char16_t mid[]{ u'a', u'b' };
    EXPECT_EQ(stream.write_units(mid, 2u), 4u);
    const char narrow[]{ 'a', 'b', 'c' };
    EXPECT_EQ(stream.write_units(narrow, 3u), 3u);
    EXPECT_EQ(stream.mData.size(), 13u);
}

/**
 * \brief   Test read_text() of a declared count above the readable bytes: the text is empty and the rest is dropped.
 **/
TEST(StreamTextUnitsTest, read_text_drops_data_when_declared_count_is_too_large)
{
    for (bool views : { true, false })
    {
        for (uint32_t declared : { 6u, 100u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu })
        {
            MemoryStream stream(views);
            stream.write32_bits(declared);
            const std::vector<uint8_t> body{ to_bytes({ 0x0061u, 0x0062u }) };
            stream.write(body.data(), 4u);
            stream.write(body.data(), 4u);
            stream.reset();

            std::u32string text{ U"previous" };
            EXPECT_EQ(stream.read_text(text), HEAD + 8u) << "declared " << declared;
            EXPECT_TRUE(text.empty());
            EXPECT_EQ(stream.size_readable(), 0u);
        }
    }
}

/**
 * \brief   Test read_text() of a declared count above the readable bytes for narrow and 2-byte text.
 **/
TEST(StreamTextUnitsTest, read_text_of_narrow_and_wide_drops_data_when_declared_count_is_too_large)
{
    for (bool views : { true, false })
    {
        MemoryStream stream(views);
        stream.write32_bits(50u);
        const uint8_t body[]{ 1u, 2u, 3u, 4u };
        stream.write(body, 4u);
        stream.reset();
        std::string narrow{ "previous" };
        EXPECT_EQ(stream.read_text(narrow), HEAD + 4u);
        EXPECT_TRUE(narrow.empty());

        stream.reset();
        std::u16string wide{ u"previous" };
        EXPECT_EQ(stream.read_text(wide), HEAD + 4u);
        EXPECT_TRUE(wide.empty());
    }
}

/**
 * \brief   Test read_text() of a stream shorter than the header.
 **/
TEST(StreamTextUnitsTest, read_text_of_truncated_header)
{
    for (bool views : { true, false })
    {
        for (uint32_t size : { 0u, 1u, 3u })
        {
            MemoryStream stream(views);
            stream.mData.assign(size, 0x05u);
            std::u32string text{ U"previous" };
            EXPECT_EQ(stream.read_text(text), size);
            EXPECT_TRUE(text.empty());
        }
    }
}

/**
 * \brief   Test read_text() when the stream returns fewer bytes than it announced.
 **/
TEST(StreamTextUnitsTest, read_text_with_short_reads)
{
    const std::u32string text{ make_text(20u, 5u) };
    MemoryStream stream(false);
    stream.write_text(text.data(), 20u);
    stream.reset();
    stream.mReadCap = 6u;

    std::u32string result;
    const uint32_t read{ stream.read_text(result) };
    EXPECT_EQ(read, HEAD + 6u);
    EXPECT_EQ(result, text.substr(0, 3u));

    stream.reset();
    std::u16string wide;
    MemoryStream wideStream(false);
    const std::u16string wideText{ u"abcdefghij" };
    wideStream.write_text(wideText.data(), 10u);
    wideStream.reset();
    wideStream.mReadCap = 6u;
    EXPECT_EQ(wideStream.read_text(wide), HEAD + 6u);
    EXPECT_EQ(wide, wideText.substr(0, 3u));
}

/**
 * \brief   Test a stream that ends with a high surrogate: the lone surrogate is kept as the last character.
 **/
TEST(StreamTextUnitsTest, read_text_keeps_high_surrogate_at_end_of_stream)
{
    for (bool views : { true, false })
    {
        MemoryStream stream(views);
        stream.write32_bits(2u);
        const std::vector<uint8_t> body{ to_bytes({ 0x0061u, EMOJI_HIGH }) };
        stream.write(body.data(), 4u);
        stream.reset();
        std::u32string text;
        EXPECT_EQ(stream.read_text(text), HEAD + 4u);
        ASSERT_EQ(text.size(), 2u);
        EXPECT_EQ(text[0], U'a');
        EXPECT_EQ(text[1], static_cast<char32_t>(EMOJI_HIGH));
    }
}

/**
 * \brief   Test narrow and 2-byte text round trips on every stream kind.
 **/
TEST(StreamTextUnitsTest, narrow_and_two_byte_text_round_trip)
{
    const std::string narrow{ "narrow text" };
    const std::u16string mid{ u"mid text \xD83D\xDE00" };
    for (bool writeViews : { true, false })
    {
        for (bool readViews : { true, false })
        {
            MemoryStream stream(writeViews);
            EXPECT_EQ(stream.write_text(narrow.data(), static_cast<uint32_t>(narrow.size())), HEAD + narrow.size());
            EXPECT_EQ(stream.write_text(mid.data(), static_cast<uint32_t>(mid.size())), HEAD + mid.size() * 2u);
            stream.mViews = readViews;
            stream.reset();
            std::string narrowResult;
            std::u16string midResult;
            EXPECT_EQ(stream.read_text(narrowResult), HEAD + narrow.size());
            EXPECT_EQ(stream.read_text(midResult), HEAD + mid.size() * 2u);
            EXPECT_EQ(narrowResult, narrow);
            EXPECT_EQ(midResult, mid);
        }
    }
}

/**
 * \brief   Test the wide string of the platform round trips through the stream operators.
 **/
TEST(StreamTextUnitsTest, wide_string_operators_round_trip)
{
    const std::wstring text{ L"wide \x20AC text" };
    for (bool views : { true, false })
    {
        MemoryStream stream(views);
        static_cast<areg::OutStream&>(stream) << text << L"literal";
        stream.reset();
        std::wstring first, second;
        static_cast<const areg::InStream&>(stream) >> first >> second;
        EXPECT_EQ(first, text);
        EXPECT_EQ(second, L"literal");
    }
}
