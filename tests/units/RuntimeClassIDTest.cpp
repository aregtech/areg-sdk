/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/RuntimeClassIDTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of areg::RuntimeClassID built from strings that are not
 *              compile time constants, so that the code runs at run time.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/String.hpp"
#include "areg/base/MathDefs.hpp"

#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

namespace
{
    //! Two different names with the same CRC32 number.
    const std::string CollisionFirst { "Cls1371838" };
    const std::string CollisionSecond{ "Cls2000402" };
}

/**
 * \brief   Test the identifier made from a run time C string.
 **/
TEST(RuntimeClassIDTest, id_of_run_time_c_string)
{
    const std::string name{ std::string("Runtime") + "ClassName" };
    const areg::RuntimeClassID id(name.c_str());
    EXPECT_TRUE(id.is_valid());
    EXPECT_EQ(id.name(), name);
    EXPECT_EQ(id.magic(), areg::crc32_calculate(name.c_str()));
    EXPECT_EQ(static_cast<uint32_t>(id), id.magic());
    EXPECT_NE(id.magic(), areg::CHECKSUM_IGNORE);
}

/**
 * \brief   Test the identifier made from a run time string view, also from a part of a longer string.
 **/
TEST(RuntimeClassIDTest, id_of_run_time_string_view)
{
    const std::string name{ "ClassNameWithTail" };
    const areg::RuntimeClassID full(std::string_view{ name });
    const areg::RuntimeClassID part(std::string_view{ name }.substr(0u, 9u));
    EXPECT_EQ(full.name(), "ClassNameWithTail");
    EXPECT_EQ(part.name(), "ClassName");
    EXPECT_EQ(part.magic(), areg::crc32_calculate(std::string_view{ "ClassName" }));
    EXPECT_NE(full, part);
    EXPECT_EQ(part, areg::RuntimeClassID(std::string("ClassName").c_str()));
}

/**
 * \brief   Test an empty or null name makes the invalid identifier.
 **/
TEST(RuntimeClassIDTest, empty_and_null_names_make_invalid_id)
{
    const std::string empty;
    const char* const nothing{ empty.empty() ? nullptr : "x" };
    for (const areg::RuntimeClassID& id : { areg::RuntimeClassID(nothing), areg::RuntimeClassID(empty.c_str()), areg::RuntimeClassID(std::string_view{ empty }) })
    {
        EXPECT_FALSE(id.is_valid());
        EXPECT_EQ(id.name(), areg::RuntimeClassID::BAD_CLASS_ID);
        EXPECT_EQ(id.magic(), areg::CHECKSUM_IGNORE);
        EXPECT_EQ(id, areg::RuntimeClassID::empty_id());
    }
}

/**
 * \brief   Test the shared invalid identifier.
 **/
TEST(RuntimeClassIDTest, empty_id_is_shared_and_invalid)
{
    const areg::RuntimeClassID& first{ areg::RuntimeClassID::empty_id() };
    const areg::RuntimeClassID& second{ areg::RuntimeClassID::empty_id() };
    EXPECT_EQ(&first, &second);
    EXPECT_FALSE(first.is_valid());
    EXPECT_EQ(static_cast<uint32_t>(first), areg::CHECKSUM_IGNORE);
}

/**
 * \brief   Reproduces the defect: the shared invalid identifier came back as a writable reference, so
 *          an assignment through it made the sentinel valid for the whole process.
 **/
TEST(RuntimeClassIDTest, empty_id_is_read_only)
{
    using Result = decltype(areg::RuntimeClassID::empty_id());
    EXPECT_TRUE(std::is_lvalue_reference_v<Result>);
    EXPECT_TRUE(std::is_const_v<std::remove_reference_t<Result>>);
    EXPECT_FALSE(areg::RuntimeClassID::empty_id().is_valid());
}

/**
 * \brief   Test copy and move construction and assignment.
 **/
TEST(RuntimeClassIDTest, copy_and_move)
{
    const std::string name{ "CopyName" };
    areg::RuntimeClassID source(name.c_str());
    areg::RuntimeClassID copy(source);
    areg::RuntimeClassID moved(areg::RuntimeClassID(name.c_str()));
    EXPECT_EQ(copy, source);
    EXPECT_EQ(moved, source);
    EXPECT_EQ(copy.name(), name);

    areg::RuntimeClassID other(std::string("Other").c_str());
    other = source;
    EXPECT_EQ(other.name(), name);
    EXPECT_EQ(other.magic(), source.magic());

    areg::RuntimeClassID target(std::string("Target").c_str());
    target = areg::RuntimeClassID(name.c_str());
    EXPECT_EQ(target.name(), name);

    const areg::RuntimeClassID& self{ target };
    target = self;
    EXPECT_EQ(target.name(), name);
    areg::RuntimeClassID& alias{ target };
    target = std::move(alias);
    EXPECT_EQ(target.name(), name);
}

/**
 * \brief   Test the comparison of two identifiers.
 **/
TEST(RuntimeClassIDTest, compare_identifiers)
{
    const areg::RuntimeClassID first(std::string("First").c_str());
    const areg::RuntimeClassID same(std::string("First").c_str());
    const areg::RuntimeClassID second(std::string("Second").c_str());
    EXPECT_TRUE(first == same);
    EXPECT_FALSE(first != same);
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);
}

/**
 * \brief   Test the comparison with names: C string, string view, areg::String, in both operand orders.
 **/
TEST(RuntimeClassIDTest, compare_with_names)
{
    const std::string name{ "NamedClass" };
    const std::string wrong{ "OtherClass" };
    const areg::RuntimeClassID id(name.c_str());
    const areg::String sameString(name.c_str());
    const areg::String wrongString(wrong.c_str());

    EXPECT_TRUE(id == name.c_str());
    EXPECT_FALSE(id != name.c_str());
    EXPECT_TRUE(name.c_str() == id);
    EXPECT_FALSE(name.c_str() != id);
    EXPECT_FALSE(id == wrong.c_str());
    EXPECT_TRUE(id != wrong.c_str());
    EXPECT_FALSE(wrong.c_str() == id);
    EXPECT_TRUE(wrong.c_str() != id);

    EXPECT_TRUE(id == std::string_view{ name });
    EXPECT_FALSE(id != std::string_view{ name });
    EXPECT_TRUE(std::string_view{ name } == id);
    EXPECT_FALSE(std::string_view{ name } != id);
    EXPECT_FALSE(id == std::string_view{ wrong });
    EXPECT_TRUE(id != std::string_view{ wrong });
    EXPECT_FALSE(std::string_view{ wrong } == id);
    EXPECT_TRUE(std::string_view{ wrong } != id);

    EXPECT_TRUE(id == sameString);
    EXPECT_FALSE(id != sameString);
    EXPECT_TRUE(sameString == id);
    EXPECT_FALSE(sameString != id);
    EXPECT_FALSE(id == wrongString);
    EXPECT_TRUE(id != wrongString);
    EXPECT_FALSE(wrongString == id);
    EXPECT_TRUE(wrongString != id);

    EXPECT_FALSE(id == "NamedClas");
    EXPECT_FALSE(id == "NamedClasss");
    EXPECT_FALSE(id == "namedclass");
}

/**
 * \brief   Test the comparison with the CRC32 number in both operand orders.
 **/
TEST(RuntimeClassIDTest, compare_with_magic_number)
{
    const areg::RuntimeClassID id(std::string("MagicClass").c_str());
    const uint32_t magic{ id.magic() };
    EXPECT_TRUE(magic == id);
    EXPECT_FALSE(magic != id);
    EXPECT_FALSE((magic + 1u) == id);
    EXPECT_TRUE((magic + 1u) != id);
    EXPECT_TRUE(areg::CHECKSUM_IGNORE == areg::RuntimeClassID::empty_id());
}

/**
 * \brief   Test the identifier works as a key of an unordered map by its CRC32 number.
 **/
TEST(RuntimeClassIDTest, identifier_as_map_key)
{
    std::unordered_map<uint32_t, int> map;
    const areg::RuntimeClassID first(std::string("KeyOne").c_str());
    const areg::RuntimeClassID second(std::string("KeyTwo").c_str());
    map[static_cast<uint32_t>(first)] = 1;
    map[static_cast<uint32_t>(second)] = 2;
    EXPECT_EQ(map[first.magic()], 1);
    EXPECT_EQ(map[second.magic()], 2);
}

/**
 * \brief   Reproduces the defect: two identifiers with different names and the same CRC32 number compared
 *          equal, while the comparison with the name said they differ.
 **/
TEST(RuntimeClassIDTest, identifiers_with_same_magic_and_different_names_are_different)
{
    const areg::RuntimeClassID first(CollisionFirst.c_str());
    const areg::RuntimeClassID second(CollisionSecond.c_str());
    ASSERT_EQ(first.magic(), second.magic());
    ASSERT_NE(first.name(), second.name());

    EXPECT_FALSE(first == CollisionSecond.c_str());
    EXPECT_TRUE(first != CollisionSecond.c_str());
    EXPECT_FALSE(first == second);
    EXPECT_TRUE(first != second);

    const areg::RuntimeClassID sameName(CollisionFirst.c_str());
    EXPECT_TRUE(first == sameName);
    EXPECT_FALSE(first != sameName);
}

/**
 * \brief   Reproduces the defect: the comparison with a null C string built a string view from the null
 *          pointer and crashed. A null name is the name of no identifier, the invalid one included.
 **/
TEST(RuntimeClassIDTest, compare_with_null_c_string)
{
    AREG_EXPECT_IN_CHILD(
        const areg::RuntimeClassID id(std::string("NullCompare").c_str());
        const char* const nothing{ std::string().empty() ? nullptr : "x" };
        AREG_CHILD_CHECK((id == nothing) == false);
        AREG_CHILD_CHECK(id != nothing);
        AREG_CHILD_CHECK((nothing == id) == false);
        AREG_CHILD_CHECK(nothing != id);

        const areg::RuntimeClassID& invalid{ areg::RuntimeClassID::empty_id() };
        AREG_CHILD_CHECK((invalid == nothing) == false);
        AREG_CHILD_CHECK(invalid != nothing);
        AREG_CHILD_CHECK((nothing == invalid) == false);
        AREG_CHILD_CHECK(nothing != invalid);
    );
}
