/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/VersionTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of areg::Version: construction, parsing,
 *              comparison, compatibility, text conversion and streaming.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/Version.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"

#include <cstdint>

TEST(VersionTest, default_version_is_invalid)
{
    const areg::Version version;
    EXPECT_EQ(version.major(), 0u);
    EXPECT_EQ(version.minor(), 0u);
    EXPECT_EQ(version.patch(), 0u);
    EXPECT_FALSE(version.is_valid());
    EXPECT_TRUE(version == areg::Version::invalid_version());
}

TEST(VersionTest, invalid_version_is_zero_and_stable)
{
    const areg::Version & invalid{ areg::Version::invalid_version() };
    EXPECT_EQ(invalid.major(), 0u);
    EXPECT_EQ(invalid.minor(), 0u);
    EXPECT_EQ(invalid.patch(), 0u);
    EXPECT_FALSE(invalid.is_valid());
    EXPECT_EQ(&invalid, &areg::Version::invalid_version());
}

TEST(VersionTest, construct_from_numbers)
{
    const areg::Version full(1u, 2u, 3u);
    EXPECT_EQ(full.major(), 1u);
    EXPECT_EQ(full.minor(), 2u);
    EXPECT_EQ(full.patch(), 3u);
    EXPECT_TRUE(full.is_valid());

    const areg::Version noPatch(4u, 5u);
    EXPECT_EQ(noPatch.patch(), 0u);
}

TEST(VersionTest, validity_needs_major_or_minor)
{
    EXPECT_TRUE(areg::Version(1u, 0u, 0u).is_valid());
    EXPECT_TRUE(areg::Version(0u, 1u, 0u).is_valid());
    EXPECT_FALSE(areg::Version(0u, 0u, 7u).is_valid());
}

TEST(VersionTest, set_version_replaces_all_numbers)
{
    areg::Version version(1u, 2u, 3u);
    version.set_version(7u, 8u, 9u);
    EXPECT_EQ(version.major(), 7u);
    EXPECT_EQ(version.minor(), 8u);
    EXPECT_EQ(version.patch(), 9u);
}

TEST(VersionTest, parse_three_numbers)
{
    const areg::Version version("1.2.3");
    EXPECT_EQ(version.major(), 1u);
    EXPECT_EQ(version.minor(), 2u);
    EXPECT_EQ(version.patch(), 3u);
}

TEST(VersionTest, parse_two_numbers_leaves_patch_zero)
{
    const areg::Version version("1.2");
    EXPECT_EQ(version.major(), 1u);
    EXPECT_EQ(version.minor(), 2u);
    EXPECT_EQ(version.patch(), 0u);
}

TEST(VersionTest, parse_one_number_leaves_minor_and_patch_zero)
{
    const areg::Version version("7");
    EXPECT_EQ(version.major(), 7u);
    EXPECT_EQ(version.minor(), 0u);
    EXPECT_EQ(version.patch(), 0u);
    EXPECT_TRUE(version.is_valid());
}

TEST(VersionTest, parse_empty_and_null_give_invalid)
{
    EXPECT_FALSE(areg::Version("").is_valid());
    EXPECT_FALSE(areg::Version(static_cast<const char *>(nullptr)).is_valid());
    EXPECT_FALSE(areg::Version(areg::String()).is_valid());
}

TEST(VersionTest, parse_garbage_gives_invalid)
{
    EXPECT_FALSE(areg::Version("abc").is_valid());
    EXPECT_FALSE(areg::Version("a.b.c").is_valid());
    EXPECT_FALSE(areg::Version("...").is_valid());
    EXPECT_FALSE(areg::Version(".").is_valid());
}

TEST(VersionTest, parse_from_string_object)
{
    const areg::Version version(areg::String("3.4.5"));
    EXPECT_EQ(version.major(), 3u);
    EXPECT_EQ(version.minor(), 4u);
    EXPECT_EQ(version.patch(), 5u);
}

TEST(VersionTest, from_string_returns_itself_and_discards_old_value)
{
    areg::Version version(9u, 9u, 9u);
    areg::Version & ref{ version.from_string("2.3.4") };
    EXPECT_EQ(&ref, &version);
    EXPECT_EQ(version.major(), 2u);
    EXPECT_EQ(version.minor(), 3u);
    EXPECT_EQ(version.patch(), 4u);

    version.from_string("");
    EXPECT_FALSE(version.is_valid());
    EXPECT_EQ(version.patch(), 0u);
}

TEST(VersionTest, assign_from_text)
{
    areg::Version version;
    version = "5.6.7";
    EXPECT_EQ(version, areg::Version(5u, 6u, 7u));

    version = areg::String("8.9.1");
    EXPECT_EQ(version, areg::Version(8u, 9u, 1u));
}

TEST(VersionTest, patch_is_kept_when_minor_is_zero)
{
    const areg::Version version("1.0.5");
    EXPECT_EQ(version.major(), 1u);
    EXPECT_EQ(version.minor(), 0u);
    EXPECT_EQ(version.patch(), 5u);
    EXPECT_EQ(areg::Version("0.9.3").patch(), 3u);
    EXPECT_EQ(areg::Version("0.0.5"), areg::Version::invalid_version());
}

TEST(VersionTest, to_string_uses_dotted_numbers)
{
    EXPECT_EQ(areg::Version(1u, 2u, 3u).to_string(), "1.2.3");
    EXPECT_EQ(areg::Version().to_string(), "0.0.0");
    EXPECT_EQ(areg::Version(10u, 20u, 30u).to_string(), "10.20.30");
}

TEST(VersionTest, text_round_trip_keeps_numbers)
{
    const areg::Version original(12u, 34u, 56u);
    const areg::Version parsed(original.to_string());
    EXPECT_EQ(parsed, original);
}

TEST(VersionTest, to_string_prints_large_numbers_unsigned)
{
    EXPECT_EQ(areg::Version(0x80000000u, 1u, 0u).to_string(), "2147483648.1.0");
}

TEST(VersionTest, equality_compares_all_numbers)
{
    const areg::Version base(1u, 2u, 3u);
    EXPECT_TRUE(base == areg::Version(1u, 2u, 3u));
    EXPECT_FALSE(base != areg::Version(1u, 2u, 3u));
    EXPECT_TRUE(base != areg::Version(2u, 2u, 3u));
    EXPECT_TRUE(base != areg::Version(1u, 3u, 3u));
    EXPECT_TRUE(base != areg::Version(1u, 2u, 4u));
    EXPECT_FALSE(base == areg::Version(1u, 2u, 4u));
    EXPECT_TRUE(base == base);
    EXPECT_FALSE(base != base);
}

TEST(VersionTest, order_by_major)
{
    const areg::Version low(1u, 9u, 9u);
    const areg::Version high(2u, 0u, 0u);
    EXPECT_TRUE(low < high);
    EXPECT_FALSE(high < low);
    EXPECT_TRUE(high > low);
    EXPECT_FALSE(low > high);
}

TEST(VersionTest, order_by_minor)
{
    const areg::Version low(1u, 2u, 9u);
    const areg::Version high(1u, 3u, 0u);
    EXPECT_TRUE(low < high);
    EXPECT_FALSE(high < low);
    EXPECT_TRUE(high > low);
    EXPECT_FALSE(low > high);
}

TEST(VersionTest, order_by_patch)
{
    const areg::Version low(1u, 2u, 3u);
    const areg::Version high(1u, 2u, 4u);
    EXPECT_TRUE(low < high);
    EXPECT_FALSE(high < low);
    EXPECT_TRUE(high > low);
    EXPECT_FALSE(low > high);
}

TEST(VersionTest, equal_versions_are_neither_smaller_nor_bigger)
{
    const areg::Version one(3u, 4u, 5u);
    const areg::Version two(3u, 4u, 5u);
    EXPECT_FALSE(one < two);
    EXPECT_FALSE(one > two);
    EXPECT_FALSE(one < one);
    EXPECT_FALSE(one > one);
}

TEST(VersionTest, compatible_needs_same_major_and_not_older_minor)
{
    const areg::Version required(1u, 2u, 0u);
    EXPECT_TRUE(areg::Version(1u, 2u, 0u).is_compatible(required));
    EXPECT_TRUE(areg::Version(1u, 5u, 0u).is_compatible(required));
    EXPECT_FALSE(areg::Version(1u, 1u, 9u).is_compatible(required));
    EXPECT_FALSE(areg::Version(2u, 2u, 0u).is_compatible(required));
    EXPECT_FALSE(areg::Version(0u, 9u, 0u).is_compatible(required));
}

TEST(VersionTest, compatibility_ignores_patch)
{
    EXPECT_TRUE(areg::Version(1u, 2u, 0u).is_compatible(areg::Version(1u, 2u, 99u)));
}

TEST(VersionTest, stream_round_trip)
{
    const areg::Version original(11u, 22u, 33u);
    areg::SharedBuffer buffer;
    buffer << original;
    EXPECT_EQ(buffer.size_used(), 3u * sizeof(uint32_t));

    buffer.reset();
    areg::Version read;
    buffer >> read;
    EXPECT_EQ(read, original);
}

TEST(VersionTest, construct_from_stream)
{
    areg::SharedBuffer buffer;
    buffer << areg::Version(4u, 5u, 6u);

    buffer.reset();
    const areg::Version read(buffer);
    EXPECT_EQ(read.major(), 4u);
    EXPECT_EQ(read.minor(), 5u);
    EXPECT_EQ(read.patch(), 6u);
}
