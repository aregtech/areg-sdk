/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/IdentifierTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of areg::Identifier: construction, comparison, lookup by value and by name, invalidation and streaming.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"

#include <string_view>
#include <vector>

namespace
{
    //!< A lookup table of three identifiers; the first is the default.
    const std::vector<areg::Identifier> & lookup_list()
    {
        static const std::vector<areg::Identifier> list{
              areg::Identifier(0u, "Unknown")
            , areg::Identifier(10u, "Ten")
            , areg::Identifier(20u, std::string_view("Twenty"))
            , areg::Identifier(30u, areg::String("Thirty"))
        };
        return list;
    }
}

TEST(IdentifierTest, default_identifier_is_invalid)
{
    const areg::Identifier id;
    EXPECT_FALSE(id.is_valid());
    EXPECT_EQ(id.value(), areg::Identifier::BAD_IDENTIFIER_VALUE);
    EXPECT_EQ(id.name(), areg::Identifier::BAD_IDENTIFIER_NAME.data());
    EXPECT_TRUE(id == areg::Identifier::BAD_IDENTIFIER);
}

TEST(IdentifierTest, construct_from_every_name_type)
{
    const areg::Identifier fromChars(1u, "One");
    const areg::Identifier fromView(2u, std::string_view("Two"));
    const areg::Identifier fromString(3u, areg::String("Three"));
    EXPECT_EQ(fromChars.value(), 1u);
    EXPECT_EQ(fromChars.name(), "One");
    EXPECT_EQ(fromView.value(), 2u);
    EXPECT_EQ(fromView.name(), "Two");
    EXPECT_EQ(fromString.value(), 3u);
    EXPECT_EQ(fromString.name(), "Three");
    EXPECT_TRUE(fromChars.is_valid());
    EXPECT_TRUE(fromView.is_valid());
    EXPECT_TRUE(fromString.is_valid());
}

TEST(IdentifierTest, copy_and_move_keep_the_value_and_the_name)
{
    areg::Identifier source(5u, "Five");
    const areg::Identifier copy(source);
    EXPECT_EQ(copy.value(), 5u);
    EXPECT_EQ(copy.name(), "Five");

    areg::Identifier assigned;
    assigned = copy;
    EXPECT_TRUE(assigned == copy);

    areg::Identifier moved(std::move(source));
    EXPECT_EQ(moved.value(), 5u);
    EXPECT_EQ(moved.name(), "Five");

    areg::Identifier moveAssigned;
    moveAssigned = std::move(moved);
    EXPECT_EQ(moveAssigned.value(), 5u);
    EXPECT_EQ(moveAssigned.name(), "Five");
}

TEST(IdentifierTest, identifiers_are_equal_when_value_and_name_are_equal)
{
    const areg::Identifier base(7u, "Seven");
    EXPECT_TRUE(base == areg::Identifier(7u, "Seven"));
    EXPECT_FALSE(base != areg::Identifier(7u, "Seven"));
    EXPECT_TRUE(base != areg::Identifier(8u, "Seven"));
    EXPECT_TRUE(base != areg::Identifier(7u, "Eight"));
    EXPECT_TRUE(base == base);
    EXPECT_FALSE(base != base);
}

TEST(IdentifierTest, compare_with_a_name)
{
    const areg::Identifier id(7u, "Seven");
    EXPECT_TRUE(id == "Seven");
    EXPECT_FALSE(id == "Six");
    EXPECT_TRUE(id != "Six");
    EXPECT_FALSE(id != "Seven");
    EXPECT_TRUE(id == areg::String("Seven"));
    EXPECT_FALSE(id == areg::String("Six"));
    EXPECT_TRUE(id != areg::String("Six"));
    EXPECT_FALSE(id != areg::String("Seven"));
}

TEST(IdentifierTest, compare_with_a_value)
{
    const areg::Identifier id(7u, "Seven");
    EXPECT_TRUE(id == 7u);
    EXPECT_FALSE(id == 8u);
    EXPECT_TRUE(id != 8u);
    EXPECT_FALSE(id != 7u);
}

TEST(IdentifierTest, invalidate_turns_into_the_bad_identifier)
{
    areg::Identifier id(7u, "Seven");
    ASSERT_TRUE(id.is_valid());
    id.invalidate();
    EXPECT_FALSE(id.is_valid());
    EXPECT_EQ(id.value(), areg::Identifier::BAD_IDENTIFIER_VALUE);
    EXPECT_EQ(id.name(), areg::Identifier::BAD_IDENTIFIER_NAME.data());
    EXPECT_TRUE(id == areg::Identifier::BAD_IDENTIFIER);
}

TEST(IdentifierTest, lookup_name_by_value)
{
    const auto & list{ lookup_list() };
    EXPECT_EQ(areg::Identifier::to_string(10u, list, 0u), "Ten");
    EXPECT_EQ(areg::Identifier::to_string(20u, list, 0u), "Twenty");
    EXPECT_EQ(areg::Identifier::to_string(30u, list, 0u), "Thirty");
    EXPECT_EQ(areg::Identifier::to_string(0u, list, 3u), "Unknown");
}

TEST(IdentifierTest, lookup_name_of_a_missing_value_gives_the_default_entry)
{
    const auto & list{ lookup_list() };
    EXPECT_EQ(areg::Identifier::to_string(99u, list, 0u), "Unknown");
    EXPECT_EQ(areg::Identifier::to_string(99u, list, 2u), "Twenty");
}

TEST(IdentifierTest, lookup_name_of_a_missing_value_with_bad_default_is_empty)
{
    const auto & list{ lookup_list() };
    EXPECT_TRUE(areg::Identifier::to_string(99u, list, 4u).is_empty());
    EXPECT_TRUE(areg::Identifier::to_string(99u, list, UINT32_MAX).is_empty());

    const std::vector<areg::Identifier> none;
    EXPECT_TRUE(areg::Identifier::to_string(10u, none, 0u).is_empty());
}

TEST(IdentifierTest, lookup_value_by_name)
{
    const auto & list{ lookup_list() };
    EXPECT_EQ(areg::Identifier::from_string(areg::String("Ten"), list, 0u), 10u);
    EXPECT_EQ(areg::Identifier::from_string(areg::String("Twenty"), list, 0u), 20u);
    EXPECT_EQ(areg::Identifier::from_string(areg::String("Thirty"), list, 0u), 30u);
}

TEST(IdentifierTest, lookup_value_of_a_missing_name_gives_the_default_entry)
{
    const auto & list{ lookup_list() };
    EXPECT_EQ(areg::Identifier::from_string(areg::String("Nothing"), list, 1u), 10u);
    EXPECT_EQ(areg::Identifier::from_string(areg::String(), list, 3u), 30u);
    EXPECT_EQ(areg::Identifier::from_string(areg::String("ten"), list, 0u), 0u);
}

TEST(IdentifierTest, lookup_value_of_a_missing_name_with_bad_default_is_all_ones)
{
    const auto & list{ lookup_list() };
    EXPECT_EQ(areg::Identifier::from_string(areg::String("Nothing"), list, 4u), static_cast<uint32_t>(~0));

    const std::vector<areg::Identifier> none;
    EXPECT_EQ(areg::Identifier::from_string(areg::String("Ten"), none, 0u), static_cast<uint32_t>(~0));
}

TEST(IdentifierTest, stream_round_trip)
{
    const areg::Identifier original(42u, "Answer");
    areg::SharedBuffer buffer;
    buffer << original;

    buffer.reset();
    areg::Identifier read;
    buffer >> read;
    EXPECT_TRUE(read == original);
    EXPECT_EQ(read.value(), 42u);
    EXPECT_EQ(read.name(), "Answer");
}
