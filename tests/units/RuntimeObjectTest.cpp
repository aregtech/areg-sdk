/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/RuntimeObjectTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of areg::RuntimeObject and areg::RuntimeBase: class identity, runtime checks, casts and allocation.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/RuntimeObject.hpp"
#include "areg/base/private/RuntimeBase.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/String.hpp"

#include <cstdint>
#include <cstring>
#include <string_view>

namespace
{
    //!< The number of living RtAlpha objects, including the derived ones.
    int32_t gAlive{ 0 };

    //!< A runtime class derived from areg::RuntimeObject.
    class RtAlpha : public areg::RuntimeObject
    {
    public:
        AREG_DECLARE_RUNTIME(RtAlpha)

        RtAlpha() { ++gAlive; }
        ~RtAlpha() override { --gAlive; }
    };

    //!< A runtime class derived from RtAlpha.
    class RtBeta : public RtAlpha
    {
    public:
        AREG_DECLARE_RUNTIME(RtBeta)
    };

    //!< A runtime class that has no relation to RtAlpha.
    class RtOther : public areg::RuntimeObject
    {
    public:
        AREG_DECLARE_RUNTIME(RtOther)

        RtOther() = default;
        ~RtOther() override = default;
    };

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4324)
#endif  // _MSC_VER

    //!< A runtime class that requires a bigger alignment.
    class alignas(64) RtAligned : public areg::RuntimeObject
    {
    public:
        AREG_DECLARE_RUNTIME(RtAligned)

        RtAligned() = default;
        ~RtAligned() override = default;
    };

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    //!< A class derived directly from the base of all runtime classes.
    class RtRaw : public areg::RuntimeBase
    {
    public:
        RtRaw() = default;
        ~RtRaw() override = default;
    };

    AREG_IMPLEMENT_RUNTIME(RtAlpha, areg::RuntimeObject)
    AREG_IMPLEMENT_RUNTIME(RtBeta, RtAlpha)
    AREG_IMPLEMENT_RUNTIME(RtOther, areg::RuntimeObject)
    AREG_IMPLEMENT_RUNTIME(RtAligned, areg::RuntimeObject)
}

//////////////////////////////////////////////////////////////////////////
// Class identity
//////////////////////////////////////////////////////////////////////////

TEST(RuntimeObjectTest, class_identity_of_declared_class)
{
    RtAlpha alpha;
    EXPECT_EQ(alpha.class_name(), "RtAlpha");
    EXPECT_STREQ(alpha.class_string(), "RtAlpha");
    EXPECT_EQ(alpha.class_number(), areg::crc32_calculate("RtAlpha"));
    EXPECT_EQ(alpha.class_id().magic(), alpha.class_number());
    EXPECT_TRUE(alpha.class_id() == RtAlpha::CLASS_ID);
    EXPECT_EQ(static_cast<uint32_t>(alpha), alpha.class_number());
}

TEST(RuntimeObjectTest, class_identity_of_derived_class)
{
    RtBeta beta;
    EXPECT_EQ(beta.class_name(), "RtBeta");
    EXPECT_EQ(beta.class_number(), areg::crc32_calculate("RtBeta"));
    EXPECT_NE(beta.class_number(), RtAlpha::CLASS_ID.magic());
}

TEST(RuntimeObjectTest, identity_is_reached_through_the_base_pointer)
{
    RtBeta beta;
    const areg::RuntimeObject * object{ &beta };
    EXPECT_EQ(object->class_name(), "RtBeta");
    EXPECT_EQ(object->class_number(), RtBeta::CLASS_ID.magic());
}

TEST(RuntimeObjectTest, runtime_object_has_its_own_identity)
{
    EXPECT_EQ(areg::RuntimeObject::CLASS_ID.name(), "RuntimeObject");
    EXPECT_EQ(areg::RuntimeObject::CLASS_ID.magic(), areg::crc32_calculate("RuntimeObject"));
}

//////////////////////////////////////////////////////////////////////////
// Runtime checks
//////////////////////////////////////////////////////////////////////////

TEST(RuntimeObjectTest, is_runtime_matches_own_class_in_every_form)
{
    RtAlpha alpha;
    EXPECT_TRUE(alpha.is_runtime("RtAlpha"));
    EXPECT_TRUE(alpha.is_runtime(std::string_view("RtAlpha")));
    EXPECT_TRUE(alpha.is_runtime(areg::String("RtAlpha")));
    EXPECT_TRUE(alpha.is_runtime(RtAlpha::CLASS_ID));
    EXPECT_TRUE(alpha.is_runtime(RtAlpha::CLASS_ID.magic()));
}

TEST(RuntimeObjectTest, is_runtime_matches_every_ancestor)
{
    RtBeta beta;
    EXPECT_TRUE(beta.is_runtime("RtBeta"));
    EXPECT_TRUE(beta.is_runtime("RtAlpha"));
    EXPECT_TRUE(beta.is_runtime(std::string_view("RtAlpha")));
    EXPECT_TRUE(beta.is_runtime(areg::String("RtAlpha")));
    EXPECT_TRUE(beta.is_runtime(RtAlpha::CLASS_ID));
    EXPECT_TRUE(beta.is_runtime(RtAlpha::CLASS_ID.magic()));
    EXPECT_TRUE(beta.is_runtime("RuntimeObject"));
    EXPECT_TRUE(beta.is_runtime(areg::RuntimeObject::CLASS_ID));
    EXPECT_TRUE(beta.is_runtime("RuntimeBase"));
    EXPECT_TRUE(beta.is_runtime(std::string_view("RuntimeBase")));
    EXPECT_TRUE(beta.is_runtime(areg::String("RuntimeBase")));
    EXPECT_TRUE(beta.is_runtime(areg::crc32_calculate("RuntimeBase")));
}

TEST(RuntimeObjectTest, is_runtime_rejects_descendants_and_strangers)
{
    RtAlpha alpha;
    EXPECT_FALSE(alpha.is_runtime("RtBeta"));
    EXPECT_FALSE(alpha.is_runtime(std::string_view("RtBeta")));
    EXPECT_FALSE(alpha.is_runtime(areg::String("RtBeta")));
    EXPECT_FALSE(alpha.is_runtime(RtBeta::CLASS_ID));
    EXPECT_FALSE(alpha.is_runtime(RtBeta::CLASS_ID.magic()));
    EXPECT_FALSE(alpha.is_runtime("RtOther"));
    EXPECT_FALSE(alpha.is_runtime(RtOther::CLASS_ID));
    EXPECT_FALSE(alpha.is_runtime(0u));
}

TEST(RuntimeObjectTest, is_runtime_is_case_and_length_sensitive)
{
    RtAlpha alpha;
    EXPECT_FALSE(alpha.is_runtime("rtalpha"));
    EXPECT_FALSE(alpha.is_runtime("RtAlph"));
    EXPECT_FALSE(alpha.is_runtime("RtAlphaX"));
    EXPECT_FALSE(alpha.is_runtime(""));
    EXPECT_FALSE(alpha.is_runtime(std::string_view()));
    EXPECT_FALSE(alpha.is_runtime(areg::String()));
}

TEST(RuntimeObjectTest, sibling_classes_are_not_related)
{
    RtOther other;
    RtAlpha alpha;
    EXPECT_FALSE(other.is_runtime(RtAlpha::CLASS_ID));
    EXPECT_FALSE(alpha.is_runtime(RtOther::CLASS_ID));
    EXPECT_TRUE(other.is_runtime(areg::RuntimeObject::CLASS_ID));
}

//////////////////////////////////////////////////////////////////////////
// RuntimeBase
//////////////////////////////////////////////////////////////////////////

TEST(RuntimeBaseTest, identity_of_the_base_class)
{
    RtRaw raw;
    EXPECT_EQ(raw.class_name(), "RuntimeBase");
    EXPECT_STREQ(raw.class_string(), "RuntimeBase");
    EXPECT_EQ(raw.class_number(), areg::crc32_calculate("RuntimeBase"));
    EXPECT_EQ(raw.class_id().name(), "RuntimeBase");
    EXPECT_EQ(raw.class_id().magic(), raw.class_number());
}

TEST(RuntimeBaseTest, is_runtime_matches_only_the_base_class)
{
    RtRaw raw;
    EXPECT_TRUE(raw.is_runtime("RuntimeBase"));
    EXPECT_TRUE(raw.is_runtime(std::string_view("RuntimeBase")));
    EXPECT_TRUE(raw.is_runtime(areg::String("RuntimeBase")));
    EXPECT_TRUE(raw.is_runtime(raw.class_id()));
    EXPECT_TRUE(raw.is_runtime(raw.class_number()));

    EXPECT_FALSE(raw.is_runtime("RuntimeObject"));
    EXPECT_FALSE(raw.is_runtime(std::string_view("RuntimeObject")));
    EXPECT_FALSE(raw.is_runtime(areg::String("RuntimeObject")));
    EXPECT_FALSE(raw.is_runtime(areg::RuntimeObject::CLASS_ID));
    EXPECT_FALSE(raw.is_runtime(areg::RuntimeObject::CLASS_ID.magic()));
    EXPECT_FALSE(raw.is_runtime(""));
}

//////////////////////////////////////////////////////////////////////////
// Casts
//////////////////////////////////////////////////////////////////////////

TEST(RuntimeObjectTest, member_cast_returns_the_object_for_a_matching_class)
{
    RtBeta beta;
    EXPECT_EQ(beta.runtime_cast(RtBeta::CLASS_ID), &beta);
    EXPECT_EQ(beta.runtime_cast(RtAlpha::CLASS_ID), &beta);
    EXPECT_EQ(beta.runtime_cast("RtAlpha"), &beta);
    EXPECT_EQ(beta.runtime_cast(areg::String("RtAlpha")), &beta);
    EXPECT_EQ(beta.runtime_cast(RtAlpha::CLASS_ID.magic()), &beta);
}

TEST(RuntimeObjectTest, member_cast_returns_null_for_another_class)
{
    RtAlpha alpha;
    EXPECT_EQ(alpha.runtime_cast(RtBeta::CLASS_ID), nullptr);
    EXPECT_EQ(alpha.runtime_cast("RtBeta"), nullptr);
    EXPECT_EQ(alpha.runtime_cast(areg::String("RtBeta")), nullptr);
    EXPECT_EQ(alpha.runtime_cast(RtBeta::CLASS_ID.magic()), nullptr);
}

TEST(RuntimeObjectTest, free_cast_checks_the_pointer_and_the_class)
{
    RtBeta beta;
    const areg::RuntimeObject * object{ &beta };
    EXPECT_EQ(areg::runtime_cast(object, RtAlpha::CLASS_ID), object);
    EXPECT_EQ(areg::runtime_cast(object, "RtAlpha"), object);
    EXPECT_EQ(areg::runtime_cast(object, areg::String("RtAlpha")), object);
    EXPECT_EQ(areg::runtime_cast(object, RtAlpha::CLASS_ID.magic()), object);

    EXPECT_EQ(areg::runtime_cast(object, RtOther::CLASS_ID), nullptr);
    EXPECT_EQ(areg::runtime_cast(object, "RtOther"), nullptr);
    EXPECT_EQ(areg::runtime_cast(object, areg::String("RtOther")), nullptr);
    EXPECT_EQ(areg::runtime_cast(object, RtOther::CLASS_ID.magic()), nullptr);
}

TEST(RuntimeObjectTest, free_cast_of_null_pointer_is_null)
{
    const areg::RuntimeObject * none{ nullptr };
    EXPECT_EQ(areg::runtime_cast(none, RtAlpha::CLASS_ID), nullptr);
    EXPECT_EQ(areg::runtime_cast(none, "RtAlpha"), nullptr);
    EXPECT_EQ(areg::runtime_cast(none, areg::String("RtAlpha")), nullptr);
    EXPECT_EQ(areg::runtime_cast(none, RtAlpha::CLASS_ID.magic()), nullptr);
}

TEST(RuntimeObjectTest, cast_macro_goes_down_to_the_right_class)
{
    RtBeta beta;
    areg::RuntimeObject * object{ &beta };
    EXPECT_EQ(AREG_RUNTIME_CAST(object, RtBeta), &beta);
    EXPECT_EQ(AREG_RUNTIME_CAST(object, RtAlpha), static_cast<RtAlpha *>(&beta));
    EXPECT_EQ(AREG_RUNTIME_CONST_CAST(object, RtBeta), &beta);
}

TEST(RuntimeObjectTest, cast_macro_rejects_the_wrong_class)
{
    RtAlpha alpha;
    areg::RuntimeObject * object{ &alpha };
    EXPECT_EQ(AREG_RUNTIME_CAST(object, RtBeta), nullptr);
    EXPECT_EQ(AREG_RUNTIME_CAST(object, RtOther), nullptr);
    EXPECT_EQ(AREG_RUNTIME_CONST_CAST(object, RtOther), nullptr);

    areg::RuntimeObject * none{ nullptr };
    EXPECT_EQ(AREG_RUNTIME_CAST(none, RtAlpha), nullptr);
}

TEST(RuntimeObjectTest, required_size_of_runtime_object_is_one_number)
{
    RtAlpha alpha;
    const areg::required_size<areg::RuntimeObject> size{};
    EXPECT_EQ(size(), sizeof(uint32_t));
    EXPECT_EQ(size(alpha), sizeof(uint32_t));
    EXPECT_EQ(static_cast<uint32_t>(size), sizeof(uint32_t));
}

//////////////////////////////////////////////////////////////////////////
// Allocation
//////////////////////////////////////////////////////////////////////////

TEST(RuntimeObjectTest, new_and_delete_create_and_destroy_the_object)
{
    const int32_t before{ gAlive };
    RtAlpha * alpha{ new RtAlpha() };
    ASSERT_NE(alpha, nullptr);
    EXPECT_EQ(gAlive, before + 1);
    delete alpha;
    EXPECT_EQ(gAlive, before);
}

TEST(RuntimeObjectTest, self_destruct_deletes_the_object)
{
    const int32_t before{ gAlive };
    RtBeta * beta{ new RtBeta() };
    ASSERT_NE(beta, nullptr);
    EXPECT_EQ(gAlive, before + 1);
    beta->self_destruct();
    EXPECT_EQ(gAlive, before);
}

TEST(RuntimeObjectTest, delete_through_the_base_pointer_destroys_the_derived_object)
{
    const int32_t before{ gAlive };
    RtAlpha * object{ new RtBeta() };
    ASSERT_NE(object, nullptr);
    EXPECT_EQ(gAlive, before + 1);
    delete object;
    EXPECT_EQ(gAlive, before);
}

TEST(RuntimeObjectTest, array_new_creates_every_element)
{
    const int32_t before{ gAlive };
    RtAlpha * items{ new RtAlpha[3] };
    ASSERT_NE(items, nullptr);
    EXPECT_EQ(gAlive, before + 3);
    EXPECT_TRUE(items[2].is_runtime("RtAlpha"));
    delete[] items;
    EXPECT_EQ(gAlive, before);
}

TEST(RuntimeObjectTest, over_aligned_object_is_aligned)
{
    RtAligned * item{ new RtAligned() };
    ASSERT_NE(item, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(item) % 64u, 0u);
    EXPECT_TRUE(item->is_runtime("RtAligned"));
    delete item;

    RtAligned * items{ new RtAligned[2] };
    ASSERT_NE(items, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(items) % 64u, 0u);
    delete[] items;
}

TEST(RuntimeObjectTest, placement_new_uses_the_given_memory)
{
    alignas(RtAlpha) unsigned char storage[sizeof(RtAlpha)];
    const int32_t before{ gAlive };
    RtAlpha * alpha{ new (static_cast<void *>(storage)) RtAlpha() };
    EXPECT_EQ(static_cast<void *>(alpha), static_cast<void *>(storage));
    EXPECT_EQ(gAlive, before + 1);
    alpha->~RtAlpha();
    EXPECT_EQ(gAlive, before);

    alignas(RtAlpha) unsigned char arrayStorage[sizeof(RtAlpha)];
    EXPECT_EQ(RtAlpha::operator new[](sizeof(arrayStorage), static_cast<void *>(arrayStorage)), static_cast<void *>(arrayStorage));
}

TEST(RuntimeObjectTest, raw_allocation_functions_return_usable_memory)
{
    void * single{ RtAlpha::operator new(32u) };
    ASSERT_NE(single, nullptr);
    std::memset(single, 0xA5, 32u);
    RtAlpha::operator delete(single, static_cast<size_t>(32u));

    void * array{ RtAlpha::operator new[](32u) };
    ASSERT_NE(array, nullptr);
    std::memset(array, 0x5A, 32u);
    RtAlpha::operator delete[](array, static_cast<size_t>(32u));

    void * traced{ RtAlpha::operator new(32u, 1, "file.cpp", 10) };
    ASSERT_NE(traced, nullptr);
    std::memset(traced, 0x11, 32u);
    RtAlpha::operator delete(traced, 1, "file.cpp", 10);

    void * tracedArray{ RtAlpha::operator new[](32u, 1, "file.cpp", 10) };
    ASSERT_NE(tracedArray, nullptr);
    std::memset(tracedArray, 0x22, 32u);
    RtAlpha::operator delete[](tracedArray, 1, "file.cpp", 10);
}

TEST(RuntimeObjectTest, raw_aligned_allocation_functions_honor_the_alignment)
{
    constexpr std::align_val_t align{ 128u };

    void * single{ RtAlpha::operator new(256u, align) };
    ASSERT_NE(single, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(single) % 128u, 0u);
    RtAlpha::operator delete(single, align);

    void * sized{ RtAlpha::operator new(256u, align) };
    ASSERT_NE(sized, nullptr);
    RtAlpha::operator delete(sized, static_cast<size_t>(256u), align);

    void * array{ RtAlpha::operator new[](256u, align) };
    ASSERT_NE(array, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(array) % 128u, 0u);
    RtAlpha::operator delete[](array, align);

    void * sizedArray{ RtAlpha::operator new[](256u, align) };
    ASSERT_NE(sizedArray, nullptr);
    RtAlpha::operator delete[](sizedArray, static_cast<size_t>(256u), align);
}
