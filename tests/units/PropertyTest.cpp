/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/PropertyTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              The key, the value and the line of the configuration.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/persist/PersistenceDefs.hpp"
#include "areg/persist/Property.hpp"
#include "areg/persist/PropertyKey.hpp"
#include "areg/persist/PropertyValue.hpp"

#include <string>
#include <utility>
#include <vector>

namespace
{
    //!< Returns the string of the text.
    areg::String str(const char* text)
    {
        return areg::String(text);
    }

    //!< Returns the text of the string.
    std::string txt(const areg::String& value)
    {
        return std::string(value.as_string());
    }

    //!< The identifiers used to look up the names in a value list.
    const std::vector<areg::Identifier>& ids()
    {
        static const std::vector<areg::Identifier> list{ areg::Identifier(1u, "alpha")
                                                       , areg::Identifier(2u, "beta")
                                                       , areg::Identifier(4u, "gamma") };
        return list;
    }
}

//////////////////////////////////////////////////////////////////////////
// PropertyKey
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A key with four parts gives every part and the known key type.
 **/
TEST(PropertyKeyTest, four_parts_are_split)
{
    const areg::PropertyKey key(str("log::*::file::location"));
    EXPECT_TRUE(key.is_valid());
    EXPECT_EQ(txt(key.section()), "log");
    EXPECT_EQ(txt(key.module()), "*");
    EXPECT_EQ(txt(key.property()), "file");
    EXPECT_EQ(txt(key.position()), "location");
    EXPECT_TRUE(key.has_section());
    EXPECT_TRUE(key.has_module());
    EXPECT_TRUE(key.has_property());
    EXPECT_TRUE(key.has_position());
    EXPECT_TRUE(key.is_all_modules());
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::LogFileLocation);
}

/**
 * \brief   A key without the position is valid and has no position.
 **/
TEST(PropertyKeyTest, key_without_position)
{
    const areg::PropertyKey key(str("log::app::version"));
    EXPECT_TRUE(key.is_valid());
    EXPECT_FALSE(key.has_position());
    EXPECT_FALSE(key.is_all_modules());
    EXPECT_EQ(txt(key.module()), "app");
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::LogVersion);
}

/**
 * \brief   A key with fewer than three parts is not valid: the missing module is all modules and
 *          the missing property is the wildcard, which the parser accepts as a property name.
 **/
TEST(PropertyKeyTest, key_with_missing_parts)
{
    areg::PropertyKey section(str("log"));
    EXPECT_EQ(txt(section.section()), "log");
    EXPECT_TRUE(section.is_all_modules());
    EXPECT_FALSE(section.has_property());
    EXPECT_FALSE(section.is_valid());

    areg::PropertyKey module(str("log::app"));
    EXPECT_EQ(txt(module.module()), "app");
    EXPECT_EQ(txt(module.property()), "*");
    EXPECT_TRUE(module.is_valid());
}

/**
 * \brief   An empty or blank text gives an invalid key; a failed parse keeps no earlier value.
 **/
TEST(PropertyKeyTest, empty_text_is_invalid)
{
    areg::PropertyKey key;
    EXPECT_FALSE(key.is_valid());
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::Invalid);
    EXPECT_FALSE(key.parse_key(str("")));
    EXPECT_FALSE(key.parse_key(str("   \t ")));
    EXPECT_FALSE(key.has_section());

    EXPECT_TRUE(key.parse_key(str("log::*::version")));
    EXPECT_FALSE(key.parse_key(str("log")));
    EXPECT_FALSE(key.is_valid());
    EXPECT_FALSE(key.has_property());
}

/**
 * \brief   Spaces around the key are removed; a dot is part of a name and is not a separator.
 **/
TEST(PropertyKeyTest, spaces_and_dots)
{
    areg::PropertyKey key;
    EXPECT_TRUE(key.parse_key(str("  \tlog::*::file::location \t ")));
    EXPECT_EQ(txt(key.section()), "log");
    EXPECT_EQ(txt(key.position()), "location");

    areg::PropertyKey dotted(str("net.sec::mod.a::prop.b::pos.c"));
    EXPECT_EQ(txt(dotted.section()), "net.sec");
    EXPECT_EQ(txt(dotted.module()), "mod.a");
    EXPECT_EQ(txt(dotted.property()), "prop.b");
    EXPECT_EQ(txt(dotted.position()), "pos.c");
}

/**
 * \brief   Parts after the fourth are not part of the key.
 **/
TEST(PropertyKeyTest, extra_parts_are_ignored)
{
    const areg::PropertyKey key(str("a::b::c::d::e::f"));
    EXPECT_TRUE(key.is_valid());
    EXPECT_EQ(txt(key.position()), "d");
    EXPECT_EQ(txt(key.to_string()), "a::b::c::d");
}

/**
 * \brief   The move overload of the parser and the assignment from a string parse the text.
 **/
TEST(PropertyKeyTest, parse_by_move_and_assignment)
{
    areg::PropertyKey moved;
    areg::String text(str("log::app::file::location"));
    EXPECT_TRUE(moved.parse_key(std::move(text)));
    EXPECT_EQ(txt(moved.module()), "app");

    areg::PropertyKey emptyMoved;
    EXPECT_FALSE(emptyMoved.parse_key(str("")));

    areg::PropertyKey fromCopy;
    fromCopy = str("log::*::file::append");
    EXPECT_EQ(fromCopy.key_type(), areg::ConfigEntry::LogFileAppend);

    areg::PropertyKey fromMove;
    fromMove = str("log::*::file::location");
    EXPECT_EQ(fromMove.key_type(), areg::ConfigEntry::LogFileLocation);

    areg::PropertyKey ctorMove(str("service::*::list"));
    EXPECT_EQ(ctorMove.key_type(), areg::ConfigEntry::ServiceList);
}

/**
 * \brief   The text of a valid key parses back to an equal key and an equal text.
 **/
TEST(PropertyKeyTest, to_string_round_trip)
{
    const char* const keys[] = { "log::*::file::location", "log::app::scope::my_scope_*", "log::*::version"
                               , "net::*::tcpip::sndbuf", "config::*::queue::capacity" };
    for (const char* text : keys)
    {
        const areg::PropertyKey key{ str(text) };
        ASSERT_TRUE(key.is_valid()) << text;
        EXPECT_EQ(txt(key.to_string()), std::string(text));

        const areg::PropertyKey again{ key.to_string() };
        EXPECT_TRUE(again == key) << text;
        EXPECT_EQ(again.key_type(), key.key_type()) << text;
    }

    EXPECT_TRUE(areg::PropertyKey().to_string().is_empty());
}

/**
 * \brief   Every constructor that gets the parts gives the same key; the type is found from the parts
 *          or taken as given.
 **/
TEST(PropertyKeyTest, constructors_from_parts)
{
    const areg::PropertyKey byString(str("log"), str("*"), str("file"), str("location"));
    const areg::PropertyKey byView(std::string_view("log"), std::string_view("*"), std::string_view("file"), std::string_view("location"));
    const areg::PropertyKey typedString(str("log"), str("*"), str("file"), str("location"), areg::ConfigEntry::AnyKey);
    const areg::PropertyKey typedView(std::string_view("log"), std::string_view("*"), std::string_view("file"), std::string_view("location"), areg::ConfigEntry::AnyKey);

    EXPECT_EQ(byString.key_type(), areg::ConfigEntry::LogFileLocation);
    EXPECT_EQ(byView.key_type(), areg::ConfigEntry::LogFileLocation);
    EXPECT_EQ(typedString.key_type(), areg::ConfigEntry::LogFileLocation);
    EXPECT_EQ(typedView.key_type(), areg::ConfigEntry::LogFileLocation);
    EXPECT_TRUE(byString == byView);

    const areg::PropertyKey givenString(str("log"), str("*"), str("file"), str("location"), areg::ConfigEntry::LogFileAppend);
    const areg::PropertyKey givenView(std::string_view("log"), std::string_view("*"), std::string_view("file"), std::string_view("location"), areg::ConfigEntry::LogFileAppend);
    EXPECT_EQ(givenString.key_type(), areg::ConfigEntry::LogFileAppend);
    EXPECT_EQ(givenView.key_type(), areg::ConfigEntry::LogFileAppend);
}

/**
 * \brief   The setters replace every part; defaults are all modules and empty parts.
 **/
TEST(PropertyKeyTest, set_values)
{
    areg::PropertyKey key;
    key.set_values("log", "file", nullptr, "location");
    EXPECT_TRUE(key.is_all_modules());
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::LogFileLocation);

    key.set_values("log", "file", "app", "location");
    EXPECT_FALSE(key.is_all_modules());
    EXPECT_EQ(txt(key.to_string()), "log::app::file::location");

    key.set_values("log", "version");
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::LogVersion);
    EXPECT_FALSE(key.has_position());

    key.set_values(nullptr, nullptr, nullptr, nullptr);
    EXPECT_FALSE(key.is_valid());
    EXPECT_FALSE(key.has_section());

    key.set_values(str("log"), str("file"), str("app"), str("append"));
    EXPECT_EQ(txt(key.module()), "app");
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::LogFileAppend);

    key.set_values(str("x"), str("y"), str("z"), str("w"), areg::ConfigEntry::LogScope);
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::LogScope);
    EXPECT_EQ(txt(key.property()), "y");
    EXPECT_EQ(txt(key.module()), "z");
}

/**
 * \brief   Copy and move keep every part; the source of a copy and of a self-assignment is unchanged.
 **/
TEST(PropertyKeyTest, copy_and_move)
{
    areg::PropertyKey source(str("log::app::file::location"));
    areg::PropertyKey copied(source);
    EXPECT_TRUE(copied == source);
    EXPECT_EQ(copied.key_type(), source.key_type());

    areg::PropertyKey assigned;
    assigned = source;
    EXPECT_EQ(txt(assigned.to_string()), "log::app::file::location");

    areg::PropertyKey& self{ assigned };
    assigned = self;
    EXPECT_EQ(txt(assigned.to_string()), "log::app::file::location");

    areg::PropertyKey moved(std::move(copied));
    EXPECT_EQ(txt(moved.to_string()), "log::app::file::location");

    areg::PropertyKey moveAssigned;
    moveAssigned = std::move(moved);
    EXPECT_EQ(txt(moveAssigned.to_string()), "log::app::file::location");
    EXPECT_EQ(moveAssigned.key_type(), areg::ConfigEntry::LogFileLocation);
}

/**
 * \brief   A key with a missing section, module or property is not valid, whichever part is missing.
 **/
TEST(PropertyKeyTest, every_part_but_position_is_required)
{
    areg::PropertyKey key;
    key.set_values(str("log"), str("file"), str("app"), str(""));
    EXPECT_TRUE(key.is_valid());
    key.set_values(str(""), str("file"), str("app"), str("x"));
    EXPECT_FALSE(key.is_valid());
    key.set_values(str("log"), str(""), str("app"), str("x"));
    EXPECT_FALSE(key.is_valid());
    key.set_values(str("log"), str("file"), str(""), str("x"));
    EXPECT_FALSE(key.is_valid());
}

/**
 * \brief   Text made of separators only is not a key.
 **/
TEST(PropertyKeyTest, separators_only_are_invalid)
{
    areg::PropertyKey key;
    EXPECT_FALSE(key.parse_key(str("::")));
    EXPECT_FALSE(key.parse_key(str("::::::")));
    EXPECT_FALSE(key.parse_key(str(" :: ")));
}

/**
 * \brief   The reset clears every part and the type.
 **/
TEST(PropertyKeyTest, reset_clears_the_key)
{
    areg::PropertyKey key(str("log::app::file::location"));
    key.reset();
    EXPECT_FALSE(key.is_valid());
    EXPECT_FALSE(key.has_section());
    EXPECT_FALSE(key.has_module());
    EXPECT_FALSE(key.has_property());
    EXPECT_FALSE(key.has_position());
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::Invalid);
}

/**
 * \brief   A key is a group when the property or the position ends with the wildcard.
 **/
TEST(PropertyKeyTest, group_property)
{
    EXPECT_TRUE(areg::PropertyKey(str("log::*::scope::my_*")).is_group_property());
    EXPECT_TRUE(areg::PropertyKey(str("log::*::scope::*")).is_group_property());
    EXPECT_TRUE(areg::PropertyKey(str("net::*::*::sndbuf")).is_group_property());
    EXPECT_FALSE(areg::PropertyKey(str("log::*::scope::my_scope")).is_group_property());
    EXPECT_FALSE(areg::PropertyKey(str("log::*::file::location")).is_group_property());
}

/**
 * \brief   Keys are equal when section, property and position are equal and the modules are equal
 *          or one of them is all modules.
 **/
TEST(PropertyKeyTest, equality_with_wildcard_module)
{
    const areg::PropertyKey all(str("log::*::file::location"));
    const areg::PropertyKey appA(str("log::appa::file::location"));
    const areg::PropertyKey appB(str("log::appb::file::location"));
    const areg::PropertyKey otherPos(str("log::appa::file::append"));
    const areg::PropertyKey otherSection(str("net::appa::file::location"));

    EXPECT_TRUE(all == all);
    EXPECT_TRUE(all == appA);
    EXPECT_TRUE(appB == all);
    EXPECT_FALSE(appA == appB);
    EXPECT_TRUE(appA != appB);
    EXPECT_FALSE(appA == otherPos);
    EXPECT_FALSE(appA == areg::PropertyKey(str("log::appa::other::location")));
    EXPECT_TRUE(appA != otherSection);
    EXPECT_FALSE(appA != appA);
}

/**
 * \brief   The checksum of an invalid key is the ignore value; equal keys give equal checksums
 *          and keys that differ in any part give different ones.
 **/
TEST(PropertyKeyTest, checksum)
{
    EXPECT_EQ(static_cast<uint32_t>(areg::PropertyKey()), areg::CHECKSUM_IGNORE);

    const areg::PropertyKey key(str("log::app::file::location"));
    const areg::PropertyKey same(str("log::app::file::location"));
    EXPECT_EQ(static_cast<uint32_t>(key), static_cast<uint32_t>(same));
    EXPECT_NE(static_cast<uint32_t>(key), areg::CHECKSUM_IGNORE);
    EXPECT_NE(static_cast<uint32_t>(key), static_cast<uint32_t>(areg::PropertyKey(str("log::app::file::append"))));
    EXPECT_NE(static_cast<uint32_t>(key), static_cast<uint32_t>(areg::PropertyKey(str("log::other::file::location"))));
}

/**
 * \brief   The exact checks compare every given part; the module checks accept all modules on
 *          either side and an empty module never matches.
 **/
TEST(PropertyKeyTest, matching_functions)
{
    const areg::PropertyKey key(str("log::app::scope::abc"));
    EXPECT_TRUE(key.is_exact_module(str("log"), str("app")));
    EXPECT_FALSE(key.is_exact_module(str("log"), str("other")));
    EXPECT_FALSE(key.is_exact_module(str("log"), str("*")));
    EXPECT_FALSE(key.is_exact_module(str("net"), str("app")));
    EXPECT_FALSE(key.is_exact_property(str("net"), str("app"), str("scope"), str("abc")));
    EXPECT_FALSE(key.is_exact_property(str("log"), str("other"), str("scope"), str("abc")));
    EXPECT_FALSE(key.is_exact_property(str("log"), str("app"), str("other"), str("abc")));
    EXPECT_TRUE(key.is_exact_property(str("log"), str("app"), str("scope"), str("abc")));
    EXPECT_FALSE(key.is_exact_property(str("log"), str("app"), str("scope"), str("abd")));
    EXPECT_FALSE(areg::PropertyKey().is_exact_module(str(""), str("")));

    EXPECT_TRUE(key.is_module_section(str("log"), str("app")));
    EXPECT_TRUE(key.is_module_section(str("log"), str("*")));
    EXPECT_FALSE(key.is_module_section(str("log"), str("other")));
    EXPECT_FALSE(key.is_module_section(str("net"), str("app")));
    EXPECT_FALSE(key.is_module_section(str("log"), str("")));
    EXPECT_FALSE(areg::PropertyKey().is_module_section(str("log"), str("app")));

    const areg::PropertyKey allKey(str("log::*::scope::abc"));
    EXPECT_TRUE(allKey.is_module_section(str("log"), str("anything")));

    EXPECT_TRUE(key.is_module_property(str("log"), str("app"), str("scope"), str("abc")));
    EXPECT_TRUE(key.is_module_property(str("log"), str("*"), str("scope"), str("abc")));
    EXPECT_TRUE(allKey.is_module_property(str("log"), str("x"), str("scope"), str("abc")));
    EXPECT_FALSE(key.is_module_property(str("log"), str("x"), str("scope"), str("abc")));
    EXPECT_FALSE(key.is_module_property(str("log"), str(""), str("scope"), str("abc")));
    EXPECT_FALSE(key.is_module_property(str("net"), str("app"), str("scope"), str("abc")));
    EXPECT_FALSE(key.is_module_property(str("log"), str("app"), str("other"), str("abc")));
    EXPECT_FALSE(key.is_module_property(str("log"), str("app"), str("scope"), str("abd")));
    EXPECT_FALSE(areg::PropertyKey().is_module_property(str("log"), str("app"), str("scope"), str("abc")));
}

/**
 * \brief   A position compares equal to the same text, to a longer text with the wildcard
 *          and to an empty position of an empty key.
 **/
TEST(PropertyKeyTest, wildcard_position_matching)
{
    const areg::PropertyKey group(str("log::app::scope::my_*"));
    EXPECT_TRUE(group.is_module_property(str("log"), str("app"), str("scope"), str("my_*")));
    EXPECT_TRUE(group.is_module_property(str("log"), str("app"), str("scope"), str("my_scope")));
    EXPECT_FALSE(group.is_module_property(str("log"), str("app"), str("scope"), str("your_scope")));

    const areg::PropertyKey plain(str("log::app::scope::my_scope"));
    EXPECT_TRUE(plain.is_module_property(str("log"), str("app"), str("scope"), str("my_*")));
    EXPECT_FALSE(plain.is_module_property(str("log"), str("app"), str("scope"), str("my_")));
    EXPECT_FALSE(plain.is_module_property(str("log"), str("app"), str("scope"), str("my_scope_x")));
}

//////////////////////////////////////////////////////////////////////////
// PropertyValue
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A value is trimmed and loses the closing semicolons.
 **/
TEST(PropertyValueTest, text_is_trimmed_and_unterminated)
{
    EXPECT_EQ(txt(areg::PropertyValue("  hello  ").as_string()), "hello");
    EXPECT_EQ(txt(areg::PropertyValue("value;").as_string()), "value");
    EXPECT_EQ(txt(areg::PropertyValue(" value;;; ").as_string()), "value");
    EXPECT_TRUE(areg::PropertyValue(";").as_string().is_empty());
    EXPECT_TRUE(areg::PropertyValue("").as_string().is_empty());
    EXPECT_TRUE(areg::PropertyValue(str("  ")).as_string().is_empty());

    areg::PropertyValue value;
    value.parse_value(str(" x; "));
    EXPECT_EQ(txt(value.value()), "x");
    value.parse_value("y;");
    EXPECT_EQ(txt(value.value()), "y");
    value.parse_value(str("z;"));
    EXPECT_EQ(txt(value.value()), "z");
    value.set_value("a;");
    EXPECT_EQ(txt(value.value()), "a");
    value.set_value(str("b;"));
    EXPECT_EQ(txt(value.value()), "b");
    value.set_value(str("c;"));
    EXPECT_EQ(txt(value.value()), "c");
    value.set_string("d;");
    EXPECT_EQ(txt(value.value()), "d");
}

/**
 * \brief   Spaces between the value and its closing semicolon are not part of the value.
 **/
TEST(PropertyValueTest, space_before_terminator_is_trimmed)
{
    EXPECT_EQ(txt(areg::PropertyValue("abc ;").as_string()), "abc");
    EXPECT_EQ(txt(areg::PropertyValue(" abc \t; ").as_string()), "abc");
    EXPECT_EQ(txt(areg::PropertyValue("abc ; ;").as_string()), "abc");
    EXPECT_EQ(txt(areg::PropertyValue("DEBUG | SCOPE ;").as_string()), "DEBUG | SCOPE");
    EXPECT_EQ(txt(areg::PropertyValue(areg::String("a b ;")).as_string()), "a b");
    EXPECT_TRUE(areg::PropertyValue(areg::String(" ; ")).as_string().is_empty());
}

/**
 * \brief   The text of a value ends with the semicolon unless the value is empty.
 **/
TEST(PropertyValueTest, to_string_adds_terminator)
{
    EXPECT_EQ(txt(areg::PropertyValue("abc").to_string()), "abc;");
    EXPECT_TRUE(areg::PropertyValue().to_string().is_empty());
}

/**
 * \brief   The integer set and read back in every radix; text that is not a number gives zero.
 **/
TEST(PropertyValueTest, integer_conversions)
{
    areg::PropertyValue value(static_cast<uint32_t>(255u));
    EXPECT_EQ(txt(value.as_string()), "255");
    EXPECT_EQ(value.as_integer(), 255u);
    EXPECT_EQ(static_cast<uint32_t>(value), 255u);

    value.set_integer(255u, areg::Radix::Hexadecimal);
    EXPECT_EQ(value.as_integer(areg::Radix::Hexadecimal), 255u);

    value.set_integer(10u, areg::Radix::Binary);
    EXPECT_EQ(value.as_integer(areg::Radix::Binary), 10u);

    value = static_cast<uint32_t>(77u);
    EXPECT_EQ(value.as_integer(), 77u);

    value.parse_value("not a number");
    EXPECT_EQ(value.as_integer(), 0u);
}

/**
 * \brief   The double and the boolean convert both ways; text that is not true is false.
 **/
TEST(PropertyValueTest, double_and_boolean_conversions)
{
    areg::PropertyValue number(2.5);
    EXPECT_DOUBLE_EQ(number.as_double(), 2.5);
    EXPECT_DOUBLE_EQ(static_cast<double>(number), 2.5);
    number.set_double(-0.25);
    EXPECT_DOUBLE_EQ(number.as_double(), -0.25);
    number = 1.5;
    EXPECT_DOUBLE_EQ(number.as_double(), 1.5);

    areg::PropertyValue flag(true);
    EXPECT_TRUE(flag.as_boolean());
    EXPECT_TRUE(static_cast<bool>(flag));
    EXPECT_EQ(txt(flag.as_string()), "true");
    flag.set_boolean(false);
    EXPECT_FALSE(flag.as_boolean());
    EXPECT_EQ(txt(flag.as_string()), "false");
    flag = true;
    EXPECT_TRUE(flag.as_boolean());
    flag.parse_value("FALSE");
    EXPECT_FALSE(flag.as_boolean());
    flag.parse_value("anything else");
    EXPECT_TRUE(flag.as_boolean());
    flag.reset();
    EXPECT_FALSE(flag.as_boolean());
}

/**
 * \brief   The string operators and the assignments copy, move and compare by content.
 **/
TEST(PropertyValueTest, copy_move_compare)
{
    areg::PropertyValue first("one");
    areg::PropertyValue second(first);
    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);

    areg::PropertyValue third;
    third = first;
    EXPECT_EQ(txt(third.as_string()), "one");
    third = std::move(second);
    EXPECT_EQ(txt(third.as_string()), "one");

    areg::PropertyValue moved(std::move(third));
    EXPECT_EQ(txt(moved.as_string()), "one");

    moved = str(" two;");
    EXPECT_EQ(txt(moved.as_string()), "two");
    areg::String text(str("three;"));
    moved = std::move(text);
    EXPECT_EQ(txt(moved.as_string()), "three");
    EXPECT_TRUE(moved != first);

    const areg::String& view = moved;
    EXPECT_EQ(txt(view), "three");

    areg::PropertyValue byString(str(" s "));
    EXPECT_EQ(txt(byString.as_string()), "s");
    areg::String movable(str(" m "));
    areg::PropertyValue byMove(std::move(movable));
    EXPECT_EQ(txt(byMove.as_string()), "m");
}

/**
 * \brief   A list of identifiers is written as names separated by a bar and is read back by name.
 **/
TEST(PropertyValueTest, identifier_list_round_trip)
{
    areg::PropertyValue value(ids());
    EXPECT_EQ(txt(value.as_string()), "alpha | beta | gamma");

    const areg::ArrayList<areg::Identifier> list{ value.identifier_list(ids()) };
    ASSERT_EQ(list.size(), 3u);
    EXPECT_EQ(list[0].value(), 1u);
    EXPECT_EQ(list[1].value(), 2u);
    EXPECT_EQ(list[2].value(), 4u);

    EXPECT_EQ(value.identifier(ids()), 7u);

    areg::PropertyValue assigned;
    assigned = ids();
    EXPECT_TRUE(assigned == value);
}

/**
 * \brief   The bit mask is written from the names whose bits are set and is read back as the same mask.
 **/
TEST(PropertyValueTest, identifier_mask)
{
    areg::PropertyValue value;
    value.set_identifier_list(5u, ids());
    EXPECT_EQ(txt(value.as_string()), "alpha | gamma");
    EXPECT_EQ(value.identifier(ids()), 5u);

    value.set_identifier_list(0u, ids());
    EXPECT_TRUE(value.as_string().is_empty());

    value.set_identifier_list(2u, ids());
    EXPECT_EQ(txt(value.as_string()), "beta");
    EXPECT_EQ(value.identifier(ids()), 2u);
}

/**
 * \brief   Unknown names, an empty value or an empty lookup list give no identifier.
 **/
TEST(PropertyValueTest, identifier_lookup_fails_softly)
{
    areg::PropertyValue value("delta | alpha | alpha | epsilon");
    EXPECT_EQ(value.identifier(ids()), 1u);

    const areg::ArrayList<areg::Identifier> found{ value.identifier_list(ids()) };
    ASSERT_EQ(found.size(), 1u);
    EXPECT_EQ(found[0].value(), 1u);

    value.parse_value("delta");
    EXPECT_EQ(value.identifier(ids()), areg::Identifier::BAD_IDENTIFIER_VALUE);
    EXPECT_EQ(value.identifier_list(ids()).size(), 0u);

    value.parse_value("alpha");
    EXPECT_EQ(value.identifier(std::vector<areg::Identifier>()), areg::Identifier::BAD_IDENTIFIER_VALUE);
    EXPECT_EQ(value.identifier_list(std::vector<areg::Identifier>()).size(), 0u);

    value.reset();
    EXPECT_EQ(value.identifier(ids()), areg::Identifier::BAD_IDENTIFIER_VALUE);
    EXPECT_EQ(value.identifier_list(ids()).size(), 0u);
}

/**
 * \brief   A list of strings is written with bars, is read back trimmed, and duplicates are
 *          removed on request.
 **/
TEST(PropertyValueTest, string_list)
{
    areg::PropertyValue value;
    value.set_value_list(std::vector<areg::String>{ str("a"), str("b"), str("a") });
    EXPECT_EQ(txt(value.as_string()), "a | b | a");

    const areg::ArrayList<areg::String> all{ value.value_list() };
    ASSERT_EQ(all.size(), 3u);
    EXPECT_EQ(txt(all[0]), "a");
    EXPECT_EQ(txt(all[2]), "a");

    const areg::ArrayList<areg::String> unique{ value.value_list(true) };
    ASSERT_EQ(unique.size(), 2u);
    EXPECT_EQ(txt(unique[0]), "a");
    EXPECT_EQ(txt(unique[1]), "b");

    value.set_value_list(std::vector<areg::String>());
    EXPECT_TRUE(value.as_string().is_empty());
    EXPECT_EQ(value.value_list().size(), 0u);
}

//////////////////////////////////////////////////////////////////////////
// Property
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A full line gives the key and the value; the line is not temporary.
 **/
TEST(PropertyTest, parse_full_line)
{
    areg::Property prop;
    EXPECT_TRUE(prop.is_temporary());
    EXPECT_FALSE(prop.is_valid());

    ASSERT_TRUE(prop.parse(str("log::*::file::location = ./logs/a.log;")));
    EXPECT_TRUE(prop.is_valid());
    EXPECT_FALSE(prop.is_temporary());
    EXPECT_EQ(prop.type(), areg::ConfigEntry::LogFileLocation);
    EXPECT_EQ(txt(prop.key_string()), "log::*::file::location");
    EXPECT_EQ(txt(prop.value_string()), "./logs/a.log");
    EXPECT_TRUE(prop.comment().is_empty());
    EXPECT_EQ(static_cast<uint32_t>(prop), static_cast<uint32_t>(prop.key()));
}

/**
 * \brief   A trailing comment is kept apart from the value and written back with the line.
 **/
TEST(PropertyTest, parse_trailing_comment)
{
    areg::Property prop;
    ASSERT_TRUE(prop.parse(str("log::*::file::append = true; # keep the file")));
    EXPECT_TRUE(prop.value().as_boolean());
    EXPECT_EQ(txt(prop.comment()), "# keep the file");
    EXPECT_EQ(txt(prop.to_string()), "log::*::file::append = true; # keep the file");
}

/**
 * \brief   A comment line, a line of one character and an empty line are not properties.
 **/
TEST(PropertyTest, comment_and_short_lines_are_rejected)
{
    areg::Property prop;
    EXPECT_FALSE(prop.parse(str("# a comment line")));
    EXPECT_FALSE(prop.is_valid());
    EXPECT_FALSE(prop.parse(str("#")));
    EXPECT_FALSE(prop.parse(str("")));
    EXPECT_FALSE(prop.parse(str("x")));
}

/**
 * \brief   A line that is not a key and a value is rejected and leaves an empty property.
 **/
TEST(PropertyTest, invalid_line_leaves_property_empty)
{
    areg::Property prop;
    EXPECT_FALSE(prop.parse(str("this is not a property")));
    EXPECT_FALSE(prop.is_valid());
    EXPECT_TRUE(prop.value_string().is_empty());

    EXPECT_FALSE(prop.parse(str("justasection = value")));
    EXPECT_FALSE(prop.is_valid());
    EXPECT_FALSE(prop.parse(str("= value")));
}

/**
 * \brief   Spaces and tabs around the key, the equal sign and the value are removed.
 **/
TEST(PropertyTest, spaces_around_parts)
{
    areg::Property prop;
    ASSERT_TRUE(prop.parse(str("  log::*::file::location\t=\t  ./x.log  ")));
    EXPECT_EQ(txt(prop.key_string()), "log::*::file::location");
    EXPECT_EQ(txt(prop.value_string()), "./x.log");
}

/**
 * \brief   A value may hold an equal sign; only the first one splits the line.
 **/
TEST(PropertyTest, value_with_equal_sign)
{
    areg::Property prop;
    ASSERT_TRUE(prop.parse(str("log::*::layout::message = a=b")));
    EXPECT_EQ(txt(prop.value_string()), "a=b");
}

/**
 * \brief   The text of a property reads back as the same property.
 **/
TEST(PropertyTest, to_string_round_trip)
{
    const areg::Property original("log::app::file::location", "./logs/o.log");
    areg::Property parsed;
    ASSERT_TRUE(parsed.parse(original.to_string()));
    EXPECT_TRUE(parsed == original);
    EXPECT_EQ(txt(parsed.value_string()), "./logs/o.log");

    EXPECT_TRUE(areg::Property().to_string().is_empty());

    const areg::Property bare("log::app::file::location", "");
    areg::Property bareParsed;
    ASSERT_TRUE(bareParsed.parse(bare.to_string()));
    EXPECT_TRUE(bareParsed.value_string().is_empty());
    EXPECT_TRUE(bareParsed.key() == bare.key());
}

/**
 * \brief   A comment that is long or has several lines is written above the property.
 **/
TEST(PropertyTest, long_comment_goes_above)
{
    areg::Property multi("log::*::file::append", "true");
    multi.set_comment(str("# first\n# second"));
    EXPECT_EQ(txt(multi.to_string()), "# first\n# second\nlog::*::file::append = true;");

    areg::Property longer("log::*::file::append", "true");
    const std::string longComment{ "# " + std::string(70, 'x') };
    longer.set_comment(str(longComment.c_str()));
    EXPECT_EQ(txt(longer.to_string()), longComment + "\nlog::*::file::append = true;");
}

/**
 * \brief   The comment gets the comment mark once, whatever the text is.
 **/
TEST(PropertyTest, add_comment)
{
    areg::Property prop;
    prop.add_comment(str("plain"));
    EXPECT_EQ(txt(prop.comment()), "# plain");
    prop.add_comment(str("# marked"));
    EXPECT_EQ(txt(prop.comment()), "# plain# marked");
}

/**
 * \brief   Every constructor gives the same property.
 **/
TEST(PropertyTest, constructors)
{
    const areg::Property fromLine{ str("log::app::file::location = ./a.log") };
    EXPECT_TRUE(fromLine.is_valid());
    EXPECT_FALSE(fromLine.is_temporary());

    const areg::Property fromStrings(str("log::app::file::location"), str("./a.log"), str("# c"), true);
    const areg::Property fromChars("log::app::file::location", "./a.log", "# c", true);
    const areg::Property fromViews(std::string_view("log::app::file::location"), std::string_view("./a.log"), std::string_view("# c"), true);
    const areg::Property fromParts(std::string_view("log"), std::string_view("app"), std::string_view("file"), std::string_view("location"), std::string_view("./a.log"), std::string_view("# c"), true);
    const areg::Property fromKey(areg::PropertyKey(str("log::app::file::location")), areg::PropertyValue("./a.log"), str("# c"), true);
    const areg::Property fromEntry(fromKey.pair(), str("# c"), true);

    for (const areg::Property* prop : { &fromStrings, &fromChars, &fromViews, &fromParts, &fromKey, &fromEntry })
    {
        EXPECT_TRUE(*prop == fromLine);
        EXPECT_EQ(txt(prop->value_string()), "./a.log");
        EXPECT_EQ(txt(prop->comment()), "# c");
        EXPECT_TRUE(prop->is_temporary());
        EXPECT_EQ(prop->type(), areg::ConfigEntry::LogFileLocation);
    }

    areg::PropertyKey key(str("log::app::file::append"));
    areg::PropertyValue value("true");
    const areg::Property moved(std::move(key), std::move(value));
    EXPECT_FALSE(moved.is_temporary());
    EXPECT_TRUE(moved.comment().is_empty());
    EXPECT_TRUE(moved.value().as_boolean());

    areg::PropertyKey key2(str("log::app::file::append"));
    areg::PropertyValue value2("true");
    areg::String comment(str("# moved"));
    const areg::Property movedAll(std::move(key2), std::move(value2), std::move(comment));
    EXPECT_EQ(txt(movedAll.comment()), "# moved");
    EXPECT_FALSE(movedAll.is_temporary());
}

/**
 * \brief   Copy and move keep the key, the value, the comment and the temporary flag.
 **/
TEST(PropertyTest, copy_and_move)
{
    areg::Property source("log::app::file::location", "./a.log", "# c", true);
    areg::Property copied(source);
    EXPECT_TRUE(copied == source);
    EXPECT_TRUE(copied.is_temporary());
    EXPECT_EQ(txt(copied.comment()), "# c");

    EXPECT_TRUE(source == source);
    EXPECT_FALSE(source != source);

    areg::Property assigned;
    assigned = source;
    EXPECT_FALSE(assigned != source);
    EXPECT_EQ(txt(assigned.value_string()), "./a.log");
    areg::Property& self{ assigned };
    assigned = self;
    EXPECT_TRUE(assigned == source);

    areg::Property moved(std::move(copied));
    EXPECT_EQ(txt(moved.key_string()), "log::app::file::location");
    EXPECT_TRUE(moved.is_temporary());

    areg::Property moveAssigned;
    moveAssigned = std::move(moved);
    EXPECT_EQ(txt(moveAssigned.value_string()), "./a.log");
    EXPECT_EQ(txt(moveAssigned.comment()), "# c");
}

/**
 * \brief   The setters replace the key, the value, the pair and the flag; the reset empties the property.
 **/
TEST(PropertyTest, setters_and_reset)
{
    areg::Property prop;
    prop.parse_key(str("log::app::file::location"));
    prop.parse_value(str("./b.log;"));
    EXPECT_TRUE(prop.is_valid());
    EXPECT_EQ(txt(prop.value_string()), "./b.log");

    prop.set_key(areg::PropertyKey(str("log::app::file::append")));
    EXPECT_EQ(prop.type(), areg::ConfigEntry::LogFileAppend);
    areg::PropertyKey movedKey(str("log::app::version"));
    prop.set_key(std::move(movedKey));
    EXPECT_EQ(prop.type(), areg::ConfigEntry::LogVersion);

    prop.set_value(areg::PropertyValue("1.0.0"));
    EXPECT_EQ(txt(prop.value_string()), "1.0.0");
    prop.set_value(areg::PropertyValue("2.0.0"));
    prop.value() = areg::PropertyValue("3.0.0");
    EXPECT_EQ(txt(prop.value_string()), "3.0.0");

    areg::Property other("log::app::file::location", "./o.log");
    prop.set_pair(other.pair());
    EXPECT_EQ(txt(prop.value_string()), "./o.log");
    areg::Property third("log::app::file::append", "true");
    prop.set_pair(areg::Property::Entry(third.pair()));
    EXPECT_EQ(prop.type(), areg::ConfigEntry::LogFileAppend);

    prop.set_temporary(true);
    EXPECT_TRUE(prop.is_temporary());
    prop.set_comment(str("# c"));
    prop.reset();
    EXPECT_FALSE(prop.is_valid());
    EXPECT_FALSE(prop.is_temporary());
    EXPECT_TRUE(prop.comment().is_empty());
    EXPECT_TRUE(prop.value_string().is_empty());
}

/**
 * \brief   A property belongs to a module when the key has this module or all modules.
 **/
TEST(PropertyTest, module_property)
{
    const areg::Property all("log::*::file::location", "./a.log");
    const areg::Property app("log::app::file::location", "./a.log");
    EXPECT_TRUE(all.is_module_property(str("anything")));
    EXPECT_TRUE(app.is_module_property(str("app")));
    EXPECT_FALSE(app.is_module_property(str("other")));
}

/**
 * \brief   The hash of a property is the checksum of its key.
 **/
TEST(PropertyTest, hash_follows_the_key)
{
    const areg::Property first("log::app::file::location", "./a.log");
    const areg::Property second("log::app::file::location", "./other.log");
    const areg::Property different("log::app::file::append", "true");
    std::hash<areg::Property> hasher;
    EXPECT_EQ(hasher(first), hasher(second));
    EXPECT_NE(hasher(first), hasher(different));
}

//////////////////////////////////////////////////////////////////////////
// Key table
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Every key accessor gives the key of the configuration file that its name says.
 **/
TEST(PersistenceDefsTest, key_accessors_give_the_documented_keys)
{
    struct Case
    {
        const areg::ConfigKey&  key;
        const char*             text;
        areg::ConfigEntry       entry;
    };

    const Case cases[] = {
          { areg::config_version()          , "config::*::version"              , areg::ConfigEntry::ConfigVersion }
        , { areg::log_version()             , "log::*::version"                 , areg::ConfigEntry::LogVersion }
        , { areg::log_target()              , "log::*::target"                  , areg::ConfigEntry::LogTarget }
        , { areg::log_status()              , "log::*::enable"                  , areg::ConfigEntry::LogStatus }
        , { areg::log_enable()              , "log::*::enable::*"               , areg::ConfigEntry::LogEnable }
        , { areg::log_file_location()       , "log::*::file::location"          , areg::ConfigEntry::LogFileLocation }
        , { areg::log_file_append()         , "log::*::file::append"            , areg::ConfigEntry::LogFileAppend }
        , { areg::remote_queue_size()       , "log::*::remote::queue"           , areg::ConfigEntry::LogRemoteQueueSize }
        , { areg::log_database_engine()     , "log::*::db::engine"              , areg::ConfigEntry::LogDatabaseEngine }
        , { areg::log_database_name()       , "log::*::db::name"                , areg::ConfigEntry::LogDatabaseName }
        , { areg::log_database_location()   , "log::*::db::location"            , areg::ConfigEntry::LogDatabaseLocation }
        , { areg::log_database_driver()     , "log::*::db::driver"              , areg::ConfigEntry::LogDatabaseDriver }
        , { areg::log_database_address()    , "log::*::db::address"             , areg::ConfigEntry::LogDatabaseAddress }
        , { areg::log_database_port()       , "log::*::db::port"                , areg::ConfigEntry::LogDatabasePort }
        , { areg::log_database_user()       , "log::*::db::username"            , areg::ConfigEntry::LogDatabaseUsername }
        , { areg::log_database_password()   , "log::*::db::password"            , areg::ConfigEntry::LogDatabasePassword }
        , { areg::log_layout_enter()        , "log::*::layout::enter"           , areg::ConfigEntry::LogLayoutEnter }
        , { areg::log_layout_message()      , "log::*::layout::message"         , areg::ConfigEntry::LogLayoutMessage }
        , { areg::log_layout_exit()         , "log::*::layout::exit"            , areg::ConfigEntry::LogLayoutExit }
        , { areg::log_scope()               , "log::*::scope::*"                , areg::ConfigEntry::LogScope }
        , { areg::service_list()            , "service::*::list"                , areg::ConfigEntry::ServiceList }
        , { areg::service_name()            , "*::*::service"                   , areg::ConfigEntry::ServiceName }
        , { areg::service_connection()      , "*::*::connect"                   , areg::ConfigEntry::ServiceConnection }
        , { areg::service_enable()          , "*::*::enable::*"                 , areg::ConfigEntry::ServiceEnable }
        , { areg::service_address()         , "*::*::address::*"                , areg::ConfigEntry::ServiceAddress }
        , { areg::service_port()            , "*::*::port::*"                   , areg::ConfigEntry::ServicePort }
        , { areg::buffer_block_size()       , "config::*::default::blocksize"   , areg::ConfigEntry::DefaultBufferBlock }
        , { areg::net_socket_sndbuf()       , "net::*::*::sndbuf"               , areg::ConfigEntry::NetSocketSndbuf }
        , { areg::net_socket_rcvbuf()       , "net::*::*::rcvbuf"               , areg::ConfigEntry::NetSocketRcvbuf }
        , { areg::net_socket_drain()        , "net::*::*::drain"                , areg::ConfigEntry::NetSocketDrain }
        , { areg::net_pool_pairs()          , "net::*::*::pairs"                , areg::ConfigEntry::NetPoolPairs }
        , { areg::net_socket_timeout()      , "net::*::*::timeout"              , areg::ConfigEntry::NetSocketTimeout }
        , { areg::net_thread_cache()        , "net::*::*::cache"                , areg::ConfigEntry::NetThreadCache }
        , { areg::queue_capacity()          , "config::*::queue::capacity"      , areg::ConfigEntry::QueueCapacity }
        , { areg::queue_wait_timeout()      , "config::*::queue::timeout"       , areg::ConfigEntry::QueueWaitTimeout }
        , { areg::queue_drop_on_full()      , "config::*::queue::drop"          , areg::ConfigEntry::QueueDropOnFull }
        , { areg::net_keepalive()           , "net::*::*::keepalive"            , areg::ConfigEntry::NetKeepAlive }
    };

    for (const Case& entry : cases)
    {
        std::string text{ std::string(entry.key.section) + "::" + std::string(entry.key.module) + "::" + std::string(entry.key.property) };
        if (entry.key.position.empty() == false)
        {
            text += "::" + std::string(entry.key.position);
        }

        EXPECT_EQ(text, std::string(entry.text));
        EXPECT_EQ(&entry.key, &areg::DefaultPropertyKeys[static_cast<int32_t>(entry.entry)]);
    }
}

/**
 * \brief   A key that is written in the file with a concrete position gets the type of the matching table row.
 **/
TEST(PersistenceDefsTest, parsed_key_gets_the_type_of_its_row)
{
    EXPECT_EQ(areg::PropertyKey(str("log::*::enable::file")).key_type(), areg::ConfigEntry::LogEnable);
    EXPECT_EQ(areg::PropertyKey(str("log::*::enable")).key_type(), areg::ConfigEntry::LogStatus);
    EXPECT_EQ(areg::PropertyKey(str("router::*::enable::tcpip")).key_type(), areg::ConfigEntry::ServiceEnable);
    EXPECT_EQ(areg::PropertyKey(str("router::*::address::tcpip")).key_type(), areg::ConfigEntry::ServiceAddress);
    EXPECT_EQ(areg::PropertyKey(str("router::*::port::tcpip")).key_type(), areg::ConfigEntry::ServicePort);
    EXPECT_EQ(areg::PropertyKey(str("log::*::scope::any_name")).key_type(), areg::ConfigEntry::LogScope);
    EXPECT_EQ(areg::PropertyKey(str("net::*::tcpip::keepalive")).key_type(), areg::ConfigEntry::NetKeepAlive);
    EXPECT_EQ(areg::PropertyKey(str("unknown::*::thing::here")).key_type(), areg::ConfigEntry::AnyKey);
}

/**
 * \brief   The setters that take a named key or value copy it, and the text assignments parse it.
 **/
TEST(PropertyTest, setters_copy_a_named_key_and_value)
{
    areg::PropertyKey key;
    key = str("log::app::file::location");
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::LogFileLocation);
    EXPECT_TRUE(key.is_exact_property(str("log"), str("app"), str("file"), str("location")));
    EXPECT_FALSE(key.is_exact_property(str("log"), str("app"), str("file"), str("append")));
    EXPECT_FALSE(key.is_exact_property(str("log"), str("other"), str("file"), str("location")));
    EXPECT_FALSE(areg::PropertyKey().is_exact_property(str(""), str(""), str(""), str("")));

    areg::PropertyValue value;
    value.set_value(str("./named.log;"));
    EXPECT_EQ(txt(value.as_string()), "./named.log");

    areg::Property prop;
    prop.set_key(key);
    prop.set_value(value);
    EXPECT_TRUE(prop.is_valid());
    EXPECT_EQ(prop.type(), areg::ConfigEntry::LogFileLocation);
    EXPECT_EQ(txt(prop.value_string()), "./named.log");
    EXPECT_EQ(key.key_type(), areg::ConfigEntry::LogFileLocation);
}

/**
 * \brief   An empty line is not a property.
 **/
TEST(PropertyTest, empty_line_is_not_a_property)
{
    areg::Property prop;
    EXPECT_FALSE(prop.parse(areg::String()));
    EXPECT_FALSE(prop.is_valid());
}

/**
 * \brief   Keys without a section match by property when one property is a prefix ending in the
 *          wildcard; a plain prefix or a different text does not match.
 **/
TEST(PropertyKeyTest, keys_without_section_match_by_wildcard_property)
{
    areg::PropertyKey file;
    file.set_values(str(""), str("file"), str("*"), str(""));
    areg::PropertyKey wildcard;
    wildcard.set_values(str(""), str("fi*"), str("*"), str(""));
    areg::PropertyKey prefix;
    prefix.set_values(str(""), str("fil"), str("*"), str(""));
    areg::PropertyKey other;
    other.set_values(str(""), str("flag"), str("*"), str(""));

    EXPECT_TRUE(file == wildcard);
    EXPECT_TRUE(wildcard == file);
    EXPECT_FALSE(file == prefix);
    EXPECT_FALSE(file == other);
}

/**
 * \brief   A key assigned from a string is parsed like a key built from it; a value set from a
 *          string replaces the value, the same as from a literal.
 **/
TEST(PropertyKeyTest, key_assigned_from_a_string_and_value_set_from_a_string_are_parsed)
{
    areg::PropertyKey key(str("log::*::scope::*"));
    const areg::String text{ str("router::*::address::tcpip") };
    key = text;
    const areg::PropertyKey built(str("router::*::address::tcpip"));
    EXPECT_TRUE(key.is_valid());
    EXPECT_EQ(txt(key.section()), "router");
    EXPECT_EQ(txt(key.property()), "address");
    EXPECT_EQ(txt(key.position()), "tcpip");
    EXPECT_EQ(key.key_type(), built.key_type());
    EXPECT_EQ(txt(key.to_string()), txt(built.to_string()));

    areg::PropertyValue value;
    value.set_value(str("first"));
    EXPECT_EQ(txt(value.as_string()), "first");
    const areg::String next{ str("second") };
    value.set_value(next);
    EXPECT_EQ(txt(value.as_string()), "second");
    areg::PropertyValue literal;
    literal.set_value("second");
    EXPECT_EQ(txt(literal.as_string()), txt(value.as_string()));
}
