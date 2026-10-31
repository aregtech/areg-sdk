/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ServiceAddressTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the address classes: Channel, ServiceItem,
 *              ServiceRole, ServiceAddress, ProxyAddress, StubAddress and ComponentAddress.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/Version.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/ComponentAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ServiceAddress.hpp"
#include "areg/component/ServiceItem.hpp"
#include "areg/component/StubAddress.hpp"

#include <functional>
#include <string>
#include <utility>

namespace
{
    constexpr UniqueNumber    SVC_NUM     { 0x1000u };
    constexpr UniqueNumber    ROLE_NUM    { 0x2000u };
    constexpr UniqueNumber    THREAD_NUM  { 0x3000u };
    constexpr UniqueNumber    ADDR_NUM    { 0x4000u };
    constexpr areg::ServiceType     SVC_TYPE    { areg::ServiceType::Public };

    //!< Version used by the tested services.
    areg::Version svc_version()
    {
        return areg::Version(1, 2, 3);
    }

    //!< A valid service item with a name.
    areg::ServiceItem make_item(const char* name = "SvcItem")
    {
        return areg::ServiceItem(areg::String(name), svc_version(), SVC_TYPE);
    }

    //!< A valid service address with names.
    areg::ServiceAddress make_service(const char* name = "SvcAddr", const char* role = "RoleAddr")
    {
        return areg::ServiceAddress(areg::String(name), svc_version(), SVC_TYPE, areg::String(role));
    }

    //!< A valid proxy built of numbers, so that no dispatcher thread is needed.
    areg::ProxyAddress make_proxy(UniqueNumber role = ROLE_NUM, const areg::Version& ver = areg::Version(1, 2, 3))
    {
        return areg::ProxyAddress(SVC_NUM, ver, SVC_TYPE, role, THREAD_NUM, ADDR_NUM);
    }

    //!< A valid stub built of numbers.
    areg::StubAddress make_stub(UniqueNumber role = ROLE_NUM, const areg::Version& ver = areg::Version(1, 2, 3))
    {
        return areg::StubAddress(SVC_NUM, ver, SVC_TYPE, role, THREAD_NUM, ADDR_NUM);
    }

    //!< Returns the first 'size' used bytes of the buffer as a new buffer to read from.
    areg::SharedBuffer cut(const areg::SharedBuffer& buf, uint32_t size)
    {
        return areg::SharedBuffer(buf.buffer(), size);
    }
}

////////////////////////////////////////////////////////////////////////////
// Channel
////////////////////////////////////////////////////////////////////////////

/**
 * \brief   A default channel is all zero and is not valid; the invalid channel is not valid either.
 **/
TEST(ChannelTest, default_and_invalid_are_not_valid)
{
    const areg::Channel ch;
    EXPECT_EQ(ch.source(), 0u);
    EXPECT_EQ(ch.target(), 0u);
    EXPECT_EQ(ch.cookie(), 0u);
    EXPECT_FALSE(ch.is_valid());
    EXPECT_FALSE(areg::Channel::invalid_channel().is_valid());
    EXPECT_EQ(areg::Channel::invalid_channel().cookie(), areg::COOKIE_UNKNOWN);
}

/**
 * \brief   Constructor parts and setters are returned by the getters; a channel is valid with a cookie.
 **/
TEST(ChannelTest, parts_setters_and_validity)
{
    areg::Channel ch(11u, 22u, 33u);
    EXPECT_EQ(ch.source(), 11u);
    EXPECT_EQ(ch.target(), 22u);
    EXPECT_EQ(ch.cookie(), 33u);
    EXPECT_EQ(static_cast<uint32_t>(ch), 11u);
    EXPECT_TRUE(ch.is_valid());

    ch.set_source(1u);
    ch.set_target(2u);
    ch.set_cookie(0u);
    EXPECT_EQ(ch.source(), 1u);
    EXPECT_EQ(ch.target(), 2u);
    EXPECT_FALSE(ch.is_valid());

    ch.set_cookie(5u);
    EXPECT_TRUE(ch.is_valid());
    ch.invalidate();
    EXPECT_EQ(ch, areg::Channel());
}

/**
 * \brief   Equality compares all three fields.
 **/
TEST(ChannelTest, compare)
{
    const areg::Channel a(1u, 2u, 3u);
    const areg::Channel same(1u, 2u, 3u);
    EXPECT_TRUE(a == same);
    EXPECT_FALSE(a != same);
    EXPECT_TRUE(a == a);
    EXPECT_FALSE(a != a);
    EXPECT_TRUE(a != areg::Channel(9u, 2u, 3u));
    EXPECT_TRUE(a != areg::Channel(1u, 9u, 3u));
    EXPECT_TRUE(a != areg::Channel(1u, 2u, 9u));
    EXPECT_FALSE(a == areg::Channel(1u, 2u, 9u));
}

/**
 * \brief   The text form is "source.target.cookie" and parses back to the same channel.
 **/
TEST(ChannelTest, string_round_trip)
{
    const areg::Channel ch(7u, 8u, 9u);
    EXPECT_STREQ(ch.to_string().as_string(), "7.8.9");

    areg::Channel parsed;
    parsed.from_string(ch.to_string());
    EXPECT_EQ(parsed, ch);
}

/**
 * \brief   Malformed text never leaves old values: missing or non-numeric parts become zero.
 **/
TEST(ChannelTest, from_malformed_string)
{
    areg::Channel ch(1u, 2u, 3u);
    ch.from_string(areg::String());
    EXPECT_EQ(ch, areg::Channel());

    ch.from_string(areg::String("5.6"));
    EXPECT_EQ(ch.source(), 5u);
    EXPECT_EQ(ch.target(), 6u);
    EXPECT_EQ(ch.cookie(), 0u);

    ch.from_string(areg::String("abc.def.ghi"));
    EXPECT_EQ(ch, areg::Channel());
}

/**
 * \brief   A channel written to a stream is read back equal; a truncated stream does not give the channel.
 **/
TEST(ChannelTest, stream_round_trip_and_truncated)
{
    const areg::Channel ch(1u, 2u, 3u);
    areg::SharedBuffer buf;
    buf << ch;
    EXPECT_EQ(buf.size_used(), 3u * sizeof(uint32_t));

    buf.reset();
    areg::Channel back;
    buf >> back;
    EXPECT_EQ(back, ch);

    areg::SharedBuffer part = cut(buf, sizeof(uint32_t));
    areg::Channel partial;
    part >> partial;
    EXPECT_NE(partial, ch);
    EXPECT_EQ(partial.cookie(), 0u);
}

////////////////////////////////////////////////////////////////////////////
// ServiceItem
////////////////////////////////////////////////////////////////////////////

/**
 * \brief   A default item is not valid, has the invalid name and version and a local type.
 **/
TEST(ServiceItemTest, default_is_invalid)
{
    const areg::ServiceItem item;
    EXPECT_FALSE(item.is_valid());
    EXPECT_STREQ(item.service_name().as_string(), "INVALID_SERVICE");
    EXPECT_FALSE(item.service_version().is_valid());
    EXPECT_EQ(item.service_type(), areg::ServiceType::Local);
    EXPECT_FALSE(item.is_service_public());
}

/**
 * \brief   An item built of name, version and type returns the parts and is valid.
 **/
TEST(ServiceItemTest, construct_from_parts)
{
    const areg::ServiceItem item = make_item();
    EXPECT_TRUE(item.is_valid());
    EXPECT_STREQ(item.service_name().as_string(), "SvcItem");
    EXPECT_EQ(item.service_version(), svc_version());
    EXPECT_EQ(item.service_type(), SVC_TYPE);
    EXPECT_TRUE(item.is_service_public());
    EXPECT_NE(static_cast<uint32_t>(item), 0u);
}

/**
 * \brief   The name-only constructor gives a valid local item; a number-only item takes the number.
 **/
TEST(ServiceItemTest, construct_from_name_and_number)
{
    const areg::ServiceItem named(areg::String("OnlyName"));
    EXPECT_FALSE(named.is_valid());     // the version is not set
    EXPECT_STREQ(named.service_name().as_string(), "OnlyName");
    EXPECT_EQ(named.service_type(), areg::ServiceType::Local);
    EXPECT_FALSE(named.service_version().is_valid());

    const areg::ServiceItem numbered(SVC_NUM);
    EXPECT_TRUE(numbered.is_valid());
    EXPECT_EQ(static_cast<uint32_t>(numbered), SVC_NUM);
    EXPECT_TRUE(numbered.service_name().is_empty());

    const areg::ServiceItem numbered2(SVC_NUM, svc_version(), SVC_TYPE);
    EXPECT_EQ(static_cast<uint32_t>(numbered2), SVC_NUM);
    EXPECT_EQ(numbered2.service_version(), svc_version());
    EXPECT_EQ(numbered2.service_type(), SVC_TYPE);
}

/**
 * \brief   An invalid service type or a version-less name does not give a valid item with a name and version.
 **/
TEST(ServiceItemTest, invalid_parts_make_invalid_item)
{
    EXPECT_FALSE(areg::ServiceItem(areg::String("Name"), svc_version(), areg::ServiceType::Invalid).is_valid());
    EXPECT_FALSE(areg::ServiceItem(areg::String(), svc_version(), SVC_TYPE).is_valid());
    EXPECT_FALSE(areg::ServiceItem(areg::String("Name"), areg::Version(), SVC_TYPE).is_valid());
}

/**
 * \brief   A name longer than the allowed length is truncated.
 **/
TEST(ServiceItemTest, long_name_is_truncated)
{
    const std::string longName(areg::ITEM_NAMES_MAX_LENGTH + 40u, 'x');
    const areg::ServiceItem item(areg::String(longName.c_str()), svc_version(), SVC_TYPE);
    EXPECT_LE(item.service_name().length(), static_cast<areg::CharCount>(areg::ITEM_NAMES_MAX_LENGTH));
    EXPECT_TRUE(item.is_valid());
}

/**
 * \brief   Copy keeps the identity; move keeps it in the target.
 **/
TEST(ServiceItemTest, copy_and_move)
{
    const areg::ServiceItem item = make_item();
    areg::ServiceItem copy(item);
    EXPECT_EQ(copy, item);
    EXPECT_STREQ(copy.service_name().as_string(), "SvcItem");

    areg::ServiceItem assigned;
    assigned = item;
    EXPECT_EQ(assigned, item);
    {
        const auto& self = assigned;
        assigned = self;
    }
    EXPECT_EQ(assigned, item);

    areg::ServiceItem moved(std::move(copy));
    EXPECT_EQ(moved, item);

    areg::ServiceItem moveAssigned;
    moveAssigned = std::move(moved);
    EXPECT_EQ(moveAssigned, item);
    EXPECT_EQ(moveAssigned.service_version(), svc_version());
}

/**
 * \brief   Items compare by identity (name and type) and hash to the same value when equal.
 **/
TEST(ServiceItemTest, compare_and_hash)
{
    const areg::ServiceItem a = make_item("A");
    const areg::ServiceItem a2 = make_item("A");
    const areg::ServiceItem b = make_item("B");
    EXPECT_TRUE(a == a2);
    EXPECT_FALSE(a != a2);
    EXPECT_TRUE(a != b);
    EXPECT_EQ(std::hash<areg::ServiceItem>{}(a), std::hash<areg::ServiceItem>{}(a2));
    EXPECT_EQ(std::hash<areg::ServiceItem>{}(a), static_cast<uint32_t>(a));
    EXPECT_NE(std::hash<areg::ServiceItem>{}(a), std::hash<areg::ServiceItem>{}(b));
}

/**
 * \brief   Setting the name or the type recalculates the identity; an invalid type makes the item invalid.
 **/
TEST(ServiceItemTest, setters_update_identity)
{
    areg::ServiceItem item = make_item("First");
    const uint32_t first = static_cast<uint32_t>(item);

    item.set_service_name(areg::String("Second"));
    EXPECT_STREQ(item.service_name().as_string(), "Second");
    EXPECT_NE(static_cast<uint32_t>(item), first);
    EXPECT_EQ(item, make_item("Second"));

    item.set_service_type(areg::ServiceType::Local);
    EXPECT_EQ(item.service_type(), areg::ServiceType::Local);
    EXPECT_FALSE(item.is_service_public());
    EXPECT_TRUE(item.is_valid());

    item.set_service_version(areg::Version(5, 6, 7));
    EXPECT_EQ(item.service_version(), areg::Version(5, 6, 7));

    item.set_service_type(areg::ServiceType::Invalid);
    EXPECT_FALSE(item.is_valid());
}

/**
 * \brief   Items are compatible when identity matches and the other minor version is not higher.
 **/
TEST(ServiceItemTest, compatibility)
{
    const areg::ServiceItem v12(areg::String("Svc"), areg::Version(1, 2, 0), SVC_TYPE);
    const areg::ServiceItem v11(areg::String("Svc"), areg::Version(1, 1, 0), SVC_TYPE);
    const areg::ServiceItem v22(areg::String("Svc"), areg::Version(2, 2, 0), SVC_TYPE);
    const areg::ServiceItem other(areg::String("Other"), areg::Version(1, 2, 0), SVC_TYPE);

    EXPECT_TRUE(v12.is_service_compatible(v12));
    EXPECT_TRUE(v12.is_service_compatible(v11));
    EXPECT_FALSE(v11.is_service_compatible(v12));
    EXPECT_FALSE(v12.is_service_compatible(v22));
    EXPECT_FALSE(v12.is_service_compatible(other));
}

/**
 * \brief   The path of an item is "name::version::type" and parses back to an equal item.
 **/
TEST(ServiceItemTest, path_round_trip)
{
    const areg::ServiceItem item = make_item("PathSvc");
    const areg::String path = areg::ServiceItem::to_path(item);
    EXPECT_STREQ(path.as_string(), "PathSvc::1.2.3::128");

    const areg::ServiceItem back = areg::ServiceItem::from_path(path.as_string());
    EXPECT_EQ(back, item);
    EXPECT_EQ(back.service_version(), svc_version());
    EXPECT_EQ(back.service_type(), SVC_TYPE);
    EXPECT_STREQ(back.service_name().as_string(), "PathSvc");
}

/**
 * \brief   A '#<number>' item in the path is the identity with no name; the next part points after the item.
 **/
TEST(ServiceItemTest, path_numeric_name_and_next_part)
{
    const char* next{ nullptr };
    const areg::ServiceItem item = areg::ServiceItem::from_path("#4660::1.0.0::128::tail", &next);
    EXPECT_EQ(static_cast<uint32_t>(item), 4660u);
    EXPECT_TRUE(item.service_name().is_empty());
    EXPECT_EQ(item.service_type(), areg::ServiceType::Public);
    ASSERT_NE(next, nullptr);
    EXPECT_STREQ(next, "tail");

    EXPECT_EQ(areg::ServiceItem::to_path(item), areg::String("#4660::1.0.0::128"));
}

/**
 * \brief   A service item of numbers is written with the marker and read back equal, with no name.
 **/
TEST(ServiceItemTest, path_numeric_round_trip)
{
    const areg::ServiceItem item(SVC_NUM, svc_version(), SVC_TYPE);
    const areg::String text = areg::ServiceItem::to_path(item);
    EXPECT_STREQ(text.as_string(), "#4096::1.2.3::128");

    const areg::ServiceItem back = areg::ServiceItem::from_path(text.as_string());
    EXPECT_EQ(back, item);
    EXPECT_EQ(static_cast<uint32_t>(back), SVC_NUM);
    EXPECT_TRUE(back.service_name().is_empty());
    EXPECT_EQ(back.service_version(), svc_version());
    EXPECT_EQ(back.service_type(), SVC_TYPE);
    EXPECT_EQ(areg::ServiceItem::to_path(back), text);
}

/**
 * \brief   A name of digits is a name: its identity is computed as for any other name and survives the text.
 **/
TEST(ServiceItemTest, path_digit_name_is_a_name)
{
    const areg::ServiceItem item = make_item("4660");
    EXPECT_NE(static_cast<uint32_t>(item), 4660u);

    const areg::ServiceItem back = areg::ServiceItem::from_path(areg::ServiceItem::to_path(item).as_string());
    EXPECT_EQ(back, item);
    EXPECT_STREQ(back.service_name().as_string(), "4660");
}

/**
 * \brief   A service item read from a stream has no name, so its text carries the streamed identity.
 **/
TEST(ServiceItemTest, stream_read_item_text_round_trip)
{
    const areg::ServiceItem item = make_item("Streamed");
    areg::SharedBuffer buf;
    buf << item;
    buf.reset();

    areg::ServiceItem back = make_item("Stale");
    buf >> back;
    EXPECT_EQ(back, item);
    EXPECT_TRUE(back.service_name().is_empty());

    const areg::ServiceItem again = areg::ServiceItem::from_path(areg::ServiceItem::to_path(back).as_string());
    EXPECT_EQ(again, item);
}

/**
 * \brief   Empty and malformed paths do not give a valid item.
 **/
TEST(ServiceItemTest, path_empty_and_malformed)
{
    const areg::ServiceItem empty = areg::ServiceItem::from_path("");
    EXPECT_FALSE(empty.is_valid());

    const areg::ServiceItem garbage = areg::ServiceItem::from_path("garbage");
    EXPECT_FALSE(garbage.is_valid());

    const areg::ServiceItem noType = areg::ServiceItem::from_path("Svc::1.0.0::0");
    EXPECT_FALSE(noType.is_valid());
}

/**
 * \brief   Endpoint conversion keeps the identity, version and type.
 **/
TEST(ServiceItemTest, endpoint_round_trip)
{
    const areg::ServiceItem item = make_item();
    areg::RawService raw;
    areg::Endpoint end;
    item.to_endpoint(raw, end);
    EXPECT_EQ(raw.service, static_cast<uint32_t>(item));
    EXPECT_EQ(end.major, 1u);
    EXPECT_EQ(end.minor, 2u);
    EXPECT_EQ(end.patch, 3u);
    EXPECT_EQ(end.type, static_cast<uint16_t>(SVC_TYPE));

    const areg::ServiceItem fromEnd(raw, end);
    EXPECT_EQ(fromEnd, item);
    EXPECT_EQ(fromEnd.service_version(), svc_version());
    EXPECT_EQ(fromEnd.service_type(), SVC_TYPE);

    areg::ServiceItem assigned = make_item("Different");
    assigned.from_endpoint(raw, end);
    EXPECT_EQ(assigned, item);
    EXPECT_TRUE(assigned.service_name().is_empty());
}

/**
 * \brief   An item written to a stream is read back with the same identity, version and type; a truncated stream is not equal.
 **/
TEST(ServiceItemTest, stream_round_trip_and_truncated)
{
    const areg::ServiceItem item = make_item();
    areg::SharedBuffer buf;
    buf << item;
    const uint32_t used = buf.size_used();
    EXPECT_GT(used, 0u);

    buf.reset();
    areg::ServiceItem back;
    buf >> back;
    EXPECT_EQ(back, item);
    EXPECT_EQ(back.service_version(), svc_version());
    EXPECT_EQ(back.service_type(), SVC_TYPE);

    areg::SharedBuffer part = cut(buf, used - 1u);
    areg::ServiceItem partial;
    part >> partial;
    EXPECT_FALSE(partial.service_type() == SVC_TYPE && partial.service_version() == svc_version() && partial == item);
}

////////////////////////////////////////////////////////////////////////////
// ServiceRole
////////////////////////////////////////////////////////////////////////////

/**
 * \brief   A default role is not valid and not numeric.
 **/
TEST(ServiceRoleTest, default_is_invalid)
{
    areg::ServiceRole role;
    EXPECT_FALSE(role.is_valid());
    EXPECT_EQ(role.number(), areg::CHECKSUM_IGNORE);
    EXPECT_FALSE(role.is_numeric());
}

/**
 * \brief   A role of a name takes the checksum of the name; a role of a number keeps the number.
 **/
TEST(ServiceRoleTest, construct_from_name_and_number)
{
    const areg::ServiceRole named(areg::String("Role"));
    EXPECT_TRUE(named.is_valid());
    EXPECT_EQ(named.number(), areg::crc32_calculate("Role"));
    EXPECT_STREQ(named.as_string(), "Role");
    EXPECT_STREQ(named.role_name().as_string(), "Role");
    EXPECT_EQ(static_cast<uint32_t>(named), named.number());
    EXPECT_STREQ(static_cast<const areg::String&>(named).as_string(), "Role");

    areg::ServiceRole numbered(ROLE_NUM);
    EXPECT_TRUE(numbered.is_valid());
    EXPECT_EQ(numbered.number(), ROLE_NUM);
    EXPECT_FALSE(numbered.is_empty());
    EXPECT_TRUE(numbered.is_numeric());

    const areg::ServiceRole both(areg::String("Both"), 77u);
    EXPECT_EQ(both.number(), 77u);
    EXPECT_STREQ(both.as_string(), "Both");
}

/**
 * \brief   Roles compare by number; copy and move keep it, and the moved role is reset.
 **/
TEST(ServiceRoleTest, compare_copy_move)
{
    const areg::ServiceRole a(areg::String("A"));
    const areg::ServiceRole a2(areg::String("A"));
    const areg::ServiceRole b(areg::String("B"));
    EXPECT_TRUE(a == a2);
    EXPECT_FALSE(a != a2);
    EXPECT_TRUE(a != b);

    areg::ServiceRole copy(a);
    EXPECT_EQ(copy, a);
    areg::ServiceRole assigned;
    assigned = a;
    EXPECT_EQ(assigned, a);
    {
        const auto& self = assigned;
        assigned = self;
    }
    EXPECT_EQ(assigned, a);

    areg::ServiceRole moved(std::move(copy));
    EXPECT_EQ(moved, a);
    EXPECT_FALSE(copy.is_valid());

    areg::ServiceRole moveAssigned;
    moveAssigned = std::move(moved);
    EXPECT_EQ(moveAssigned, a);
    EXPECT_FALSE(moved.is_valid());
}

/**
 * \brief   Changing the name recalculates the number; a numeric name is taken as the number; clear resets.
 **/
TEST(ServiceRoleTest, set_name_number_and_clear)
{
    areg::ServiceRole role;
    role.set_role_name(areg::String("Named"));
    EXPECT_EQ(role.number(), areg::crc32_calculate("Named"));

    role.set_role_name(areg::String("1234"));
    EXPECT_EQ(role.number(), 1234u);
    EXPECT_TRUE(role.is_numeric());

    role.set_number(55u);
    EXPECT_EQ(role.number(), 55u);

    role.clear();
    EXPECT_FALSE(role.is_valid());
    EXPECT_TRUE(role.role_name().is_empty());
}

////////////////////////////////////////////////////////////////////////////
// ServiceAddress
////////////////////////////////////////////////////////////////////////////

/**
 * \brief   A default service address is not valid and has an empty role.
 **/
TEST(ServiceAddressTest, default_is_invalid)
{
    const areg::ServiceAddress addr;
    EXPECT_FALSE(addr.is_valid());
    EXPECT_TRUE(addr.role_name().is_empty());
    EXPECT_EQ(static_cast<uint32_t>(addr), areg::CHECKSUM_IGNORE);
}

/**
 * \brief   An address built of names is valid and returns its parts.
 **/
TEST(ServiceAddressTest, construct_from_names)
{
    const areg::ServiceAddress addr = make_service();
    EXPECT_TRUE(addr.is_valid());
    EXPECT_STREQ(addr.role_name().as_string(), "RoleAddr");
    EXPECT_STREQ(addr.service().service_name().as_string(), "SvcAddr");
    EXPECT_EQ(addr.service_version(), svc_version());
    EXPECT_EQ(addr.service_type(), SVC_TYPE);
}

/**
 * \brief   An address without a role, with an invalid type or without a service name is not valid.
 **/
TEST(ServiceAddressTest, missing_parts_make_invalid_address)
{
    EXPECT_FALSE(areg::ServiceAddress(areg::String("S"), svc_version(), SVC_TYPE, areg::String()).is_valid());
    EXPECT_FALSE(areg::ServiceAddress(areg::String("S"), svc_version(), areg::ServiceType::Invalid, areg::String("R")).is_valid());
    EXPECT_FALSE(areg::ServiceAddress(areg::String(), svc_version(), SVC_TYPE, areg::String("R")).is_valid());
}

/**
 * \brief   Addresses built of numbers or of an item and a role keep the numbers.
 **/
TEST(ServiceAddressTest, construct_from_numbers_and_item)
{
    const areg::ServiceAddress byNum(SVC_NUM, svc_version(), SVC_TYPE, ROLE_NUM);
    EXPECT_TRUE(byNum.is_valid());
    EXPECT_EQ(static_cast<uint32_t>(byNum), ROLE_NUM);
    EXPECT_EQ(static_cast<uint32_t>(byNum.service()), SVC_NUM);

    const areg::ServiceAddress byItemNum(areg::ServiceItem(SVC_NUM), ROLE_NUM);
    EXPECT_EQ(static_cast<uint32_t>(byItemNum), ROLE_NUM);
    EXPECT_EQ(byItemNum, byNum);

    const areg::ServiceAddress byItemName(make_item("SvcAddr"), areg::String("RoleAddr"));
    EXPECT_EQ(byItemName, make_service());

    const areg::ServiceAddress byNumericRole(make_item("SvcAddr"), areg::String("#8192"));
    EXPECT_EQ(static_cast<uint32_t>(byNumericRole), 8192u);
    EXPECT_TRUE(byNumericRole.role_name().is_empty());

    const areg::ServiceAddress byDigitRole(make_item("SvcAddr"), areg::String("8192"));
    EXPECT_EQ(byDigitRole, make_service("SvcAddr", "8192"));
    EXPECT_NE(static_cast<uint32_t>(byDigitRole), 8192u);
}

/**
 * \brief   set_role_name takes '#<number>' as the identity with no name, and any other text as a name.
 **/
TEST(ServiceAddressTest, set_role_name_marker_and_digits)
{
    areg::ServiceAddress addr = make_service("SvcAddr", "Old");
    addr.set_role_name(areg::String("#77"));
    EXPECT_EQ(static_cast<uint32_t>(addr), 77u);
    EXPECT_TRUE(addr.role_name().is_empty());

    addr.set_role_name(areg::String("77"));
    EXPECT_EQ(addr, make_service("SvcAddr", "77"));
    EXPECT_STREQ(addr.role_name().as_string(), "77");
}

/**
 * \brief   An address built of a proxy or of a stub keeps the service and the role.
 **/
TEST(ServiceAddressTest, construct_from_proxy_and_stub)
{
    const areg::ProxyAddress proxy = make_proxy();
    const areg::StubAddress stub = make_stub();

    const areg::ServiceAddress fromProxy(proxy);
    const areg::ServiceAddress fromStub(stub);
    EXPECT_EQ(fromProxy, fromStub);
    EXPECT_EQ(static_cast<uint32_t>(fromProxy), ROLE_NUM);
    EXPECT_EQ(static_cast<uint32_t>(fromProxy.service()), SVC_NUM);
    EXPECT_EQ(fromStub.service_version(), svc_version());
}

/**
 * \brief   Copy and move keep the identity; a moved address is reset.
 **/
TEST(ServiceAddressTest, copy_and_move)
{
    const areg::ServiceAddress addr = make_service();
    areg::ServiceAddress copy(addr);
    EXPECT_EQ(copy, addr);

    areg::ServiceAddress assigned;
    assigned = addr;
    EXPECT_EQ(assigned, addr);
    {
        const auto& self = assigned;
        assigned = self;
    }
    EXPECT_EQ(assigned, addr);

    areg::ServiceAddress moved(std::move(copy));
    EXPECT_EQ(moved, addr);
    EXPECT_FALSE(copy.is_valid());

    areg::ServiceAddress moveAssigned;
    moveAssigned = std::move(moved);
    EXPECT_EQ(moveAssigned, addr);
    EXPECT_STREQ(moveAssigned.role_name().as_string(), "RoleAddr");
    EXPECT_FALSE(moved.is_valid());
}

/**
 * \brief   Addresses with a different role, service or type differ; equal ones hash the same.
 **/
TEST(ServiceAddressTest, compare_and_hash)
{
    const areg::ServiceAddress a = make_service("S", "R1");
    const areg::ServiceAddress same = make_service("S", "R1");
    const areg::ServiceAddress otherRole = make_service("S", "R2");
    const areg::ServiceAddress otherSvc = make_service("T", "R1");
    EXPECT_TRUE(a == same);
    EXPECT_FALSE(a != same);
    EXPECT_TRUE(a != otherRole);
    EXPECT_TRUE(a != otherSvc);
    EXPECT_EQ(std::hash<areg::ServiceAddress>{}(a), std::hash<areg::ServiceAddress>{}(same));
    EXPECT_NE(std::hash<areg::ServiceAddress>{}(a), std::hash<areg::ServiceAddress>{}(otherRole));
}

/**
 * \brief   The text form is the item path plus the role and parses back to an equal address.
 **/
TEST(ServiceAddressTest, string_round_trip)
{
    const areg::ServiceAddress addr = make_service();
    const areg::String text = areg::ServiceAddress::to_path(addr);
    EXPECT_STREQ(text.as_string(), "SvcAddr::1.2.3::128::RoleAddr");

    const areg::ServiceAddress back = areg::ServiceAddress::from_path(text.as_string());
    EXPECT_EQ(back, addr);
    EXPECT_STREQ(back.role_name().as_string(), "RoleAddr");
    EXPECT_TRUE(back.is_valid());
}

/**
 * \brief   An address of numbers writes the numbers as text and takes them back.
 **/
TEST(ServiceAddressTest, string_with_numbers)
{
    const areg::ServiceAddress addr(SVC_NUM, svc_version(), SVC_TYPE, ROLE_NUM);
    EXPECT_STREQ(addr.to_string().as_string(), "#4096::1.2.3::128::#8192");

    const char* next{ nullptr };
    const areg::ServiceAddress back = areg::ServiceAddress::from_path("#4096::1.2.3::128::#8192::rest", &next);
    EXPECT_EQ(static_cast<uint32_t>(back.service()), SVC_NUM);
    EXPECT_EQ(back.service_version(), svc_version());
    ASSERT_NE(next, nullptr);
    EXPECT_STREQ(next, "rest");
}

/**
 * \brief   A numeric role in the text is the identity of the address, as written by to_string().
 **/
TEST(ServiceAddressTest, string_numeric_role_round_trip)
{
    const areg::ServiceAddress addr(SVC_NUM, svc_version(), SVC_TYPE, ROLE_NUM);
    const areg::String text = addr.to_string();
    const areg::ServiceAddress back = areg::ServiceAddress::from_path(text.as_string());
    EXPECT_EQ(static_cast<uint32_t>(back), ROLE_NUM);
    EXPECT_EQ(back, addr);
    EXPECT_EQ(static_cast<uint32_t>(back.service()), SVC_NUM);
    EXPECT_EQ(back.service_version(), svc_version());
    EXPECT_EQ(back.service_type(), SVC_TYPE);
    EXPECT_TRUE(back.role_name().is_empty());
    EXPECT_TRUE(back.service_name().is_empty());
    EXPECT_EQ(back.to_string(), text);
}

/**
 * \brief   The limits of the identity number survive the text.
 **/
TEST(ServiceAddressTest, string_numeric_role_limits)
{
    for (const UniqueNumber num : { areg::CHECKSUM_IGNORE, 1u, areg::CHECKSUM_INVALID })
    {
        const areg::ServiceAddress addr(num, svc_version(), SVC_TYPE, num);
        const areg::ServiceAddress back = areg::ServiceAddress::from_path(addr.to_string().as_string());
        EXPECT_EQ(static_cast<uint32_t>(back), num);
        EXPECT_EQ(static_cast<uint32_t>(back.service()), num);
    }
}

/**
 * \brief   A role name of digits is a name: the identity is computed from the names and survives the text.
 **/
TEST(ServiceAddressTest, string_digit_role_is_a_name)
{
    const areg::ServiceAddress addr = make_service("SvcAddr", "8192");
    EXPECT_NE(static_cast<uint32_t>(addr), 8192u);

    const areg::ServiceAddress back = areg::ServiceAddress::from_path(addr.to_string().as_string());
    EXPECT_EQ(back, addr);
    EXPECT_STREQ(back.role_name().as_string(), "8192");
}

/**
 * \brief   A named role of a service of numbers is invalid before and after the text, as built.
 **/
TEST(ServiceAddressTest, string_numeric_service_named_role)
{
    const areg::ServiceAddress addr(areg::ServiceItem(SVC_NUM, svc_version(), SVC_TYPE), areg::String("Role"));
    const areg::String text = addr.to_string();
    EXPECT_STREQ(text.as_string(), "#4096::1.2.3::128::Role");

    const areg::ServiceAddress back = areg::ServiceAddress::from_path(text.as_string());
    EXPECT_EQ(back, addr);
    EXPECT_EQ(back.is_valid(), addr.is_valid());
}

/**
 * \brief   An address read from a stream or an endpoint has no names, so its text carries the identity.
 **/
TEST(ServiceAddressTest, stream_and_endpoint_read_text_round_trip)
{
    const areg::ServiceAddress addr = make_service("SvcAddr", "RoleAddr");

    areg::SharedBuffer buf;
    buf << addr;
    buf.reset();
    areg::ServiceAddress streamed = make_service("Stale", "StaleRole");
    buf >> streamed;
    EXPECT_EQ(streamed, addr);
    EXPECT_TRUE(streamed.role_name().is_empty());
    EXPECT_EQ(areg::ServiceAddress::from_path(streamed.to_string().as_string()), addr);

    areg::RawService raw;
    areg::Endpoint end;
    addr.to_endpoint(raw, end);
    areg::ServiceAddress fromEnd = make_service("Stale", "StaleRole");
    fromEnd.from_endpoint(raw, end);
    EXPECT_EQ(fromEnd, addr);
    EXPECT_TRUE(fromEnd.role_name().is_empty());
    EXPECT_TRUE(fromEnd.service_name().is_empty());
    EXPECT_EQ(areg::ServiceAddress::from_path(fromEnd.to_string().as_string()), addr);
}

/**
 * \brief   Only '#' followed by 1 to 10 digits with a value that fits 32 bits is a marked number.
 **/
TEST(ServiceAddressTest, parse_address_number_forms)
{
    uint32_t num{ 5u };
    EXPECT_TRUE(areg::parse_address_number("#0", num));
    EXPECT_EQ(num, 0u);
    EXPECT_TRUE(areg::parse_address_number("#4294967295", num));
    EXPECT_EQ(num, 4294967295u);
    EXPECT_TRUE(areg::parse_address_number("#0000000001", num));
    EXPECT_EQ(num, 1u);

    num = 5u;
    for (const char* text : { "", "#", "12", "#4294967296", "#99999999999", "#00000000001", "#-1", "#+1", "#1a", "# 1", "a#1", "##1" })
    {
        EXPECT_FALSE(areg::parse_address_number(text, num)) << text;
        EXPECT_EQ(num, 5u) << text;
    }

    static_assert([]() { uint32_t n{ 0u }; return areg::parse_address_number("#42", n) && (n == 42u); }(), "parse_address_number is constexpr");
}

/**
 * \brief   Empty or malformed text does not give a valid address.
 **/
TEST(ServiceAddressTest, string_empty_and_malformed)
{
    EXPECT_FALSE(areg::ServiceAddress::from_path("").is_valid());
    EXPECT_FALSE(areg::ServiceAddress::from_path("garbage").is_valid());
    EXPECT_FALSE(areg::ServiceAddress::from_path("Svc::1.0.0::128").is_valid());
    EXPECT_FALSE(areg::ServiceAddress::from_path("Svc::1.0.0::0::Role").is_valid());
}

/**
 * \brief   Endpoint conversion keeps the service, the role, the version and the type.
 **/
TEST(ServiceAddressTest, endpoint_round_trip)
{
    const areg::ServiceAddress addr(SVC_NUM, svc_version(), SVC_TYPE, ROLE_NUM);
    areg::RawService raw;
    areg::Endpoint end;
    addr.to_endpoint(raw, end);
    EXPECT_EQ(raw.service, SVC_NUM);
    EXPECT_EQ(raw.role, ROLE_NUM);

    const areg::ServiceAddress back(raw, end);
    EXPECT_EQ(back, addr);
    EXPECT_EQ(back.service_version(), svc_version());

    areg::ServiceAddress assigned;
    assigned.from_endpoint(raw, end);
    EXPECT_EQ(assigned, addr);
    EXPECT_TRUE(assigned.is_valid());
}

/**
 * \brief   An address written to a stream is read back equal; a truncated stream is not equal.
 **/
TEST(ServiceAddressTest, stream_round_trip_and_truncated)
{
    const areg::ServiceAddress addr(SVC_NUM, svc_version(), SVC_TYPE, ROLE_NUM);
    areg::SharedBuffer buf;
    buf << addr;
    const uint32_t used = buf.size_used();

    buf.reset();
    areg::ServiceAddress back;
    buf >> back;
    EXPECT_EQ(back, addr);
    EXPECT_EQ(back.service_version(), svc_version());
    EXPECT_EQ(static_cast<uint32_t>(back.service()), SVC_NUM);

    areg::SharedBuffer part = cut(buf, used - 1u);
    areg::ServiceAddress partial;
    part >> partial;
    EXPECT_NE(partial, addr);
}

////////////////////////////////////////////////////////////////////////////
// ComponentAddress
////////////////////////////////////////////////////////////////////////////

/**
 * \brief   The invalid component address is not valid and has the invalid name.
 **/
TEST(ComponentAddressTest, invalid_address)
{
    const areg::ComponentAddress& invalid = areg::ComponentAddress::invalid_component_address();
    EXPECT_FALSE(invalid.is_valid());
    EXPECT_STREQ(invalid.role_name().as_string(), "INVALID_COMPONENT_NAME");
    EXPECT_FALSE(invalid.thread_address().is_valid());
}

/**
 * \brief   An address of a thread address only keeps the thread and has the invalid role.
 **/
TEST(ComponentAddressTest, construct_from_thread_only)
{
    const areg::ThreadAddress thread{ areg::String("Thr") };
    const areg::ComponentAddress addr(thread);
    EXPECT_FALSE(addr.is_valid());
    EXPECT_EQ(addr.thread_address(), thread);
    EXPECT_STREQ(addr.role_name().as_string(), "INVALID_COMPONENT_NAME");
}

/**
 * \brief   An address of a thread and a role is valid and returns both.
 **/
TEST(ComponentAddressTest, construct_from_thread_and_role)
{
    const areg::ThreadAddress thread{ areg::String("Thr") };
    const areg::ComponentAddress addr(thread, areg::String("CompRole"));
    EXPECT_TRUE(addr.is_valid());
    EXPECT_EQ(addr.thread_address(), thread);
    EXPECT_STREQ(addr.role_name().as_string(), "CompRole");
    EXPECT_NE(static_cast<uint32_t>(addr), 0u);
}

/**
 * \brief   An empty role becomes the invalid name and the address is not valid; so is an invalid thread.
 **/
TEST(ComponentAddressTest, empty_role_or_invalid_thread)
{
    const areg::ThreadAddress thread{ areg::String("Thr") };
    const areg::ComponentAddress noRole(thread, areg::String());
    EXPECT_FALSE(noRole.is_valid());
    EXPECT_STREQ(noRole.role_name().as_string(), "INVALID_COMPONENT_NAME");

    const areg::ComponentAddress noThread(areg::ThreadAddress::invalid_thread_address(), areg::String("Role"));
    EXPECT_FALSE(noThread.is_valid());

    const areg::ComponentAddress invalidName(thread, areg::String("INVALID_COMPONENT_NAME"));
    EXPECT_FALSE(invalidName.is_valid());
}

/**
 * \brief   A role and a thread name that is not a known dispatcher give the invalid thread and an invalid address.
 **/
TEST(ComponentAddressTest, construct_from_role_and_unknown_thread_name)
{
    const areg::ComponentAddress addr(areg::String("Role"), areg::String("NoSuchDispatcherThread"));
    EXPECT_STREQ(addr.role_name().as_string(), "Role");

    const areg::ComponentAddress emptyThread(areg::String("Role"), areg::String());
    EXPECT_FALSE(emptyThread.is_valid());
    EXPECT_FALSE(emptyThread.thread_address().is_valid());
}

/**
 * \brief   A long role name is truncated to the allowed length.
 **/
TEST(ComponentAddressTest, long_role_is_truncated)
{
    const std::string longName(areg::ITEM_NAMES_MAX_LENGTH + 30u, 'r');
    const areg::ComponentAddress addr(areg::ThreadAddress(areg::String("Thr")), areg::String(longName.c_str()));
    EXPECT_LE(addr.role_name().length(), areg::ITEM_NAMES_MAX_LENGTH);
    EXPECT_TRUE(addr.is_valid());
}

/**
 * \brief   Copy keeps the identity; move keeps it, assignment of a move resets the source.
 **/
TEST(ComponentAddressTest, copy_and_move)
{
    const areg::ComponentAddress addr(areg::ThreadAddress(areg::String("Thr")), areg::String("Role"));
    areg::ComponentAddress copy(addr);
    EXPECT_EQ(copy, addr);
    EXPECT_TRUE(copy.is_valid());

    areg::ComponentAddress assigned(areg::ThreadAddress(areg::String("Other")), areg::String("Other"));
    assigned = addr;
    EXPECT_EQ(assigned, addr);

    areg::ComponentAddress moved(std::move(copy));
    EXPECT_EQ(moved, addr);

    areg::ComponentAddress target(areg::ThreadAddress(areg::String("Other")), areg::String("Other"));
    target = std::move(moved);
    EXPECT_EQ(target, addr);
    EXPECT_FALSE(moved.is_valid());
    {
        auto& self = target;
        target = std::move(self);
    }
    EXPECT_EQ(target, addr);
}

/**
 * \brief   Addresses differing by role or thread are not equal; equal ones hash the same.
 **/
TEST(ComponentAddressTest, compare_and_hash)
{
    const areg::ThreadAddress thr1{ areg::String("Thr1") };
    const areg::ThreadAddress thr2{ areg::String("Thr2") };
    const areg::ComponentAddress a(thr1, areg::String("Role"));
    const areg::ComponentAddress same(thr1, areg::String("Role"));
    const areg::ComponentAddress otherRole(thr1, areg::String("Role2"));
    const areg::ComponentAddress otherThread(thr2, areg::String("Role"));
    EXPECT_TRUE(a == same);
    EXPECT_FALSE(a != same);
    EXPECT_TRUE(a != otherRole);
    EXPECT_TRUE(a != otherThread);
    EXPECT_EQ(std::hash<areg::ComponentAddress>{}(a), std::hash<areg::ComponentAddress>{}(same));
    EXPECT_NE(std::hash<areg::ComponentAddress>{}(a), std::hash<areg::ComponentAddress>{}(otherRole));
}

/**
 * \brief   The text form is "role::thread" and parses back to an equal address.
 **/
TEST(ComponentAddressTest, string_round_trip)
{
    const areg::ComponentAddress addr(areg::ThreadAddress(areg::String("Thr")), areg::String("Role"));
    const areg::String path = areg::ComponentAddress::to_path(addr);
    EXPECT_STREQ(path.as_string(), "Role::Thr");

    const char* next{ nullptr };
    const areg::ComponentAddress back = areg::ComponentAddress::from_path(path.as_string(), &next);
    EXPECT_EQ(back, addr);
    EXPECT_TRUE(back.is_valid());
    EXPECT_STREQ(back.role_name().as_string(), "Role");
    EXPECT_STREQ(back.thread_address().name().as_string(), "Thr");
    EXPECT_TRUE((next == nullptr) || (*next == '\0'));
}

/**
 * \brief   Empty or malformed text does not give a valid component address.
 **/
TEST(ComponentAddressTest, string_empty_and_malformed)
{
    EXPECT_FALSE(areg::ComponentAddress::from_path("").is_valid());
    EXPECT_FALSE(areg::ComponentAddress::from_path("OnlyRole").is_valid());
    EXPECT_FALSE(areg::ComponentAddress::from_path("::Thr").is_valid());
}

////////////////////////////////////////////////////////////////////////////
// ProxyAddress
////////////////////////////////////////////////////////////////////////////

/**
 * \brief   A default proxy is not valid and neither local nor remote.
 **/
TEST(ProxyAddressTest, default_is_invalid)
{
    const areg::ProxyAddress proxy;
    EXPECT_FALSE(proxy.is_valid());
    EXPECT_FALSE(proxy.is_local_address());
    EXPECT_FALSE(proxy.is_remote_address());
    EXPECT_FALSE(proxy.thread().is_valid());
    EXPECT_FALSE(areg::ProxyAddress::invalid_proxy_address().is_valid());
}

/**
 * \brief   A proxy of numbers is valid and local; the parts are returned.
 **/
TEST(ProxyAddressTest, construct_from_numbers)
{
    const areg::ProxyAddress proxy = make_proxy();
    EXPECT_TRUE(proxy.is_valid());
    EXPECT_TRUE(proxy.is_local_address());
    EXPECT_FALSE(proxy.is_remote_address());
    EXPECT_EQ(proxy.cookie(), areg::COOKIE_LOCAL);
    EXPECT_EQ(static_cast<uint32_t>(proxy), ADDR_NUM);
    EXPECT_EQ(static_cast<uint32_t>(static_cast<const areg::ServiceAddress&>(proxy)), ROLE_NUM);
    EXPECT_EQ(static_cast<uint32_t>(proxy.thread()), THREAD_NUM);

    const areg::ProxyAddress byItem(areg::ServiceItem(SVC_NUM, svc_version(), SVC_TYPE), ROLE_NUM, THREAD_NUM, ADDR_NUM);
    EXPECT_EQ(byItem, proxy);
}

/**
 * \brief   A proxy of names without a running dispatcher thread is not valid, but keeps the service.
 **/
TEST(ProxyAddressTest, construct_from_names_without_dispatcher)
{
    const areg::ProxyAddress byNames(areg::String("PrxSvc"), svc_version(), SVC_TYPE, areg::String("PrxRole"), areg::String("NoSuchThread"));
    EXPECT_FALSE(byNames.is_valid());
    EXPECT_STREQ(byNames.role_name().as_string(), "PrxRole");
    EXPECT_FALSE(byNames.thread().is_valid());
    EXPECT_TRUE(byNames.is_local_address());

    const areg::ProxyAddress byItem(make_item("PrxSvc"), areg::String("PrxRole"), areg::String("NoSuchThread"));
    EXPECT_FALSE(byItem.is_valid());
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(byItem), static_cast<const areg::ServiceAddress&>(byNames));

    areg::InterfaceData data;
    data.idServiceName = "PrxSvc";
    data.idVersion = svc_version();
    data.idServiceType = SVC_TYPE;
    const areg::ProxyAddress byData(data, areg::String("PrxRole"), areg::String("NoSuchThread"));
    EXPECT_FALSE(byData.is_valid());
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(byData), static_cast<const areg::ServiceAddress&>(byNames));
}

/**
 * \brief   A proxy of an invalid service has the unknown cookie and is neither local nor remote.
 **/
TEST(ProxyAddressTest, invalid_service_has_unknown_cookie)
{
    const areg::ProxyAddress proxy(areg::String("S"), svc_version(), SVC_TYPE, areg::String(), areg::String());
    EXPECT_EQ(proxy.cookie(), areg::COOKIE_UNKNOWN);
    EXPECT_FALSE(proxy.is_local_address());
    EXPECT_FALSE(proxy.is_valid());
}

/**
 * \brief   A proxy of a service address, a stub or a moved service address keeps the service and has no thread.
 **/
TEST(ProxyAddressTest, construct_from_service_address_and_stub)
{
    const areg::ServiceAddress service(SVC_NUM, svc_version(), SVC_TYPE, ROLE_NUM);
    const areg::ProxyAddress fromService(service);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(fromService), service);
    EXPECT_FALSE(fromService.thread().is_valid());
    EXPECT_FALSE(fromService.is_valid());
    EXPECT_TRUE(fromService.is_local_address());

    const areg::ProxyAddress fromStub(make_stub());
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(fromStub), service);
    EXPECT_FALSE(fromStub.is_valid());

    areg::ServiceAddress movable(service);
    const areg::ProxyAddress fromMoved(std::move(movable));
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(fromMoved), service);
    EXPECT_FALSE(fromMoved.is_valid());
}

/**
 * \brief   Copy keeps all parts; move resets the source identity; self assignment changes nothing.
 **/
TEST(ProxyAddressTest, copy_and_move)
{
    areg::ProxyAddress proxy = make_proxy();
    proxy.set_source(5u);
    proxy.set_target(6u);

    areg::ProxyAddress copy(proxy);
    EXPECT_EQ(copy, proxy);
    EXPECT_EQ(copy.channel(), proxy.channel());

    areg::ProxyAddress assigned;
    assigned = proxy;
    EXPECT_EQ(assigned, proxy);
    EXPECT_EQ(assigned.source(), 5u);
    EXPECT_EQ(assigned.target(), 6u);
    {
        const auto& self = assigned;
        assigned = self;
    }
    EXPECT_EQ(assigned, proxy);

    areg::ProxyAddress moved(std::move(copy));
    EXPECT_EQ(moved, proxy);
    EXPECT_FALSE(copy.is_valid());

    areg::ProxyAddress moveAssigned;
    moveAssigned = std::move(moved);
    EXPECT_EQ(moveAssigned, proxy);
    EXPECT_EQ(moveAssigned.channel(), proxy.channel());
    EXPECT_FALSE(moved.is_valid());
}

/**
 * \brief   The source and target getters follow the cookie: local proxies and remote proxies differ.
 **/
TEST(ProxyAddressTest, local_and_remote_flags)
{
    areg::ProxyAddress proxy = make_proxy();
    EXPECT_FALSE(proxy.is_source_local());
    EXPECT_FALSE(proxy.is_target_local());
    EXPECT_FALSE(proxy.is_target_public());
    EXPECT_FALSE(proxy.is_source_public());

    proxy.set_source(10u);
    proxy.set_target(20u);
    EXPECT_TRUE(proxy.is_local_address());
    EXPECT_TRUE(proxy.is_source_local());
    EXPECT_TRUE(proxy.is_target_local());
    EXPECT_TRUE(proxy.is_target_public());
    EXPECT_FALSE(proxy.is_source_public());

    proxy.set_cookie(areg::COOKIE_REMOTE_SERVICE);
    EXPECT_FALSE(proxy.is_local_address());
    EXPECT_TRUE(proxy.is_remote_address());
    EXPECT_FALSE(proxy.is_source_local());
    EXPECT_FALSE(proxy.is_target_local());
    EXPECT_TRUE(proxy.is_source_public());
    EXPECT_TRUE(proxy.is_target_public());
    EXPECT_EQ(proxy.cookie(), areg::COOKIE_REMOTE_SERVICE);
}

/**
 * \brief   Invalidating the channel makes the proxy not valid; setting a channel makes it valid again.
 **/
TEST(ProxyAddressTest, channel_validity)
{
    areg::ProxyAddress proxy = make_proxy();
    ASSERT_TRUE(proxy.is_valid());
    const areg::Channel saved = proxy.channel();

    proxy.invalidate_channel();
    EXPECT_FALSE(proxy.is_valid());
    EXPECT_FALSE(proxy.is_local_address());

    proxy.set_channel(saved);
    EXPECT_TRUE(proxy.is_valid());
    EXPECT_EQ(proxy.channel(), saved);
}

/**
 * \brief   Proxies are equal when identity and cookie match; the hash is the identity.
 **/
TEST(ProxyAddressTest, compare_and_hash)
{
    const areg::ProxyAddress a = make_proxy();
    areg::ProxyAddress same = make_proxy();
    EXPECT_TRUE(a == same);
    EXPECT_FALSE(a != same);

    same.set_cookie(areg::COOKIE_REMOTE_SERVICE);
    EXPECT_TRUE(a != same);
    EXPECT_FALSE(a == same);

    const areg::ProxyAddress otherRole = make_proxy(ROLE_NUM + 1u);
    EXPECT_TRUE(a == otherRole);   // identity of a numeric proxy is the proxy number
    EXPECT_EQ(std::hash<areg::ProxyAddress>{}(a), ADDR_NUM);
}

/**
 * \brief   A proxy is compatible with a valid stub of the same service, role and a not lower minor version.
 **/
TEST(ProxyAddressTest, provider_compatibility)
{
    const areg::ProxyAddress proxy = make_proxy();
    EXPECT_TRUE(proxy.is_provider_compatible(make_stub()));
    EXPECT_TRUE(proxy == make_stub());
    EXPECT_TRUE(proxy.is_provider_compatible(make_stub(ROLE_NUM, areg::Version(1, 5, 0))));

    EXPECT_FALSE(proxy.is_provider_compatible(make_stub(ROLE_NUM + 1u)));
    EXPECT_FALSE(proxy.is_provider_compatible(make_stub(ROLE_NUM, areg::Version(2, 2, 3))));
    EXPECT_FALSE(proxy.is_provider_compatible(make_stub(ROLE_NUM, areg::Version(1, 1, 0))));
    EXPECT_FALSE(proxy.is_provider_compatible(areg::StubAddress::invalid_stub_address()));

    areg::StubAddress noChannel = make_stub();
    noChannel.invalidate_channel();
    EXPECT_FALSE(proxy.is_provider_compatible(noChannel));

    areg::ProxyAddress invalidProxy = make_proxy();
    invalidProxy.invalidate_channel();
    EXPECT_FALSE(invalidProxy.is_provider_compatible(make_stub()));
}

/**
 * \brief   The text of a proxy starts with the kind and parses back to an equal proxy.
 **/
TEST(ProxyAddressTest, string_round_trip_with_thread_name)
{
    const char* text = "proxy::12345::PrxSvc::1.2.3::128::PrxRole::PrxThread::7.8.9";
    const char* next{ nullptr };
    const areg::ProxyAddress proxy = areg::ProxyAddress::from_path(text, &next);
    EXPECT_EQ(static_cast<uint32_t>(proxy), 12345u);
    EXPECT_STREQ(proxy.role_name().as_string(), "PrxRole");
    EXPECT_STREQ(proxy.thread().name().as_string(), "PrxThread");
    EXPECT_EQ(proxy.service_version(), svc_version());
    EXPECT_EQ(proxy.source(), 7u);
    EXPECT_EQ(proxy.target(), 8u);
    EXPECT_EQ(proxy.cookie(), 9u);
    EXPECT_TRUE((next == nullptr) || (*next == '\0'));

    EXPECT_STREQ(areg::ProxyAddress::to_path(proxy).as_string(), text);

    areg::ProxyAddress again;
    again.from_string(areg::ProxyAddress::to_path(proxy).as_string());
    EXPECT_EQ(again, proxy);
    EXPECT_EQ(again.channel(), proxy.channel());
}

/**
 * \brief   Text of another kind, empty text or truncated text does not give a valid proxy.
 **/
TEST(ProxyAddressTest, string_empty_and_malformed)
{
    EXPECT_FALSE(areg::ProxyAddress::from_path("").is_valid());
    EXPECT_FALSE(areg::ProxyAddress::from_path("garbage").is_valid());
    EXPECT_FALSE(areg::ProxyAddress::from_path("stub::12345::S::1.0.0::128::R::T::1.2.3").is_valid());
    EXPECT_FALSE(areg::ProxyAddress::from_path("proxy::12345").is_valid());
    EXPECT_FALSE(areg::ProxyAddress::from_path("proxy::12345::S::1.0.0::128::R::T").is_valid());

    const char* next{ nullptr };
    const areg::ProxyAddress wrongKind = areg::ProxyAddress::from_path("stub::1::S", &next);
    ASSERT_NE(next, nullptr);
    EXPECT_STREQ(next, "1::S");
    EXPECT_FALSE(wrongKind.is_valid());
}

/**
 * \brief   Endpoint and event header conversion keep the service, role, identity and channel parts.
 **/
TEST(ProxyAddressTest, endpoint_and_event_round_trip)
{
    areg::ProxyAddress proxy = make_proxy();
    proxy.set_source(11u);

    areg::RawService raw;
    areg::Endpoint end;
    proxy.to_endpoint(raw, end);
    EXPECT_EQ(end.id, areg::COOKIE_LOCAL);
    EXPECT_EQ(end.number, ADDR_NUM);
    EXPECT_EQ(raw.role, ROLE_NUM);
    EXPECT_EQ(raw.service, SVC_NUM);

    const areg::ProxyAddress fromEnd(raw, end);
    EXPECT_EQ(fromEnd, proxy);
    EXPECT_EQ(fromEnd.cookie(), areg::COOKIE_LOCAL);

    areg::EventHeader header;
    proxy.to_event(header);
    EXPECT_EQ(header.consumer.number, ADDR_NUM);
    EXPECT_EQ(header.rawService.role, ROLE_NUM);

    const areg::ProxyAddress fromHeader(header);
    EXPECT_EQ(fromHeader, proxy);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(fromHeader), static_cast<const areg::ServiceAddress&>(proxy));

    areg::ProxyAddress assigned;
    assigned.from_event(header);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(assigned), static_cast<const areg::ServiceAddress&>(proxy));
    EXPECT_EQ(assigned.source(), THREAD_NUM);
    EXPECT_EQ(assigned.cookie(), areg::COOKIE_LOCAL);
    areg::ProxyAddress assigned2;
    assigned2.from_endpoint(raw, end);
    EXPECT_EQ(assigned2.source(), THREAD_NUM);
    EXPECT_EQ(assigned2.cookie(), areg::COOKIE_LOCAL);
}

/**
 * \brief   Reading the endpoint restores the identity number that the endpoint carries.
 **/
TEST(ProxyAddressTest, from_endpoint_restores_identity)
{
    areg::ProxyAddress proxy = make_proxy();
    areg::RawService raw;
    areg::Endpoint end;
    proxy.to_endpoint(raw, end);
    const areg::ProxyAddress built(raw, end);

    areg::ProxyAddress assigned(make_service("OldSvc", "OldRole"));
    assigned.set_cookie(77u);
    assigned.from_endpoint(raw, end);
    EXPECT_EQ(static_cast<uint32_t>(assigned), ADDR_NUM);
    EXPECT_EQ(assigned, proxy);
    EXPECT_EQ(assigned, built);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(assigned), static_cast<const areg::ServiceAddress&>(proxy));
    EXPECT_EQ(static_cast<uint32_t>(assigned.service()), SVC_NUM);
    EXPECT_EQ(assigned.service_version(), proxy.service_version());
    EXPECT_EQ(assigned.service_type(), proxy.service_type());
    EXPECT_EQ(static_cast<uint32_t>(assigned.thread()), THREAD_NUM);
    EXPECT_EQ(assigned.cookie(), proxy.cookie());
    EXPECT_EQ(assigned.source(), built.source());
    EXPECT_TRUE(assigned.service_name().is_empty());
    EXPECT_TRUE(assigned.role_name().is_empty());
    EXPECT_EQ(areg::ProxyAddress::to_path(assigned), areg::ProxyAddress::to_path(built));

    areg::EventHeader header;
    proxy.to_event(header);
    areg::ProxyAddress fromEvent(make_service("OldSvc", "OldRole"));
    fromEvent.from_event(header);
    EXPECT_EQ(fromEvent, proxy);
    EXPECT_EQ(fromEvent, areg::ProxyAddress(header));
}

/**
 * \brief   A proxy of numbers keeps every part, the thread included, through the text.
 **/
TEST(ProxyAddressTest, string_numeric_round_trip)
{
    const areg::ProxyAddress proxy = make_proxy();
    const areg::String text = areg::ProxyAddress::to_path(proxy);
    EXPECT_STREQ(text.as_string(), "proxy::16384::#4096::1.2.3::128::#8192::#12288::0.0.1");

    const areg::ProxyAddress back = areg::ProxyAddress::from_path(text.as_string());
    EXPECT_EQ(back, proxy);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(back), static_cast<const areg::ServiceAddress&>(proxy));
    EXPECT_EQ(static_cast<uint32_t>(back.service()), SVC_NUM);
    EXPECT_EQ(static_cast<uint32_t>(back.thread()), THREAD_NUM);
    EXPECT_EQ(back.channel(), proxy.channel());
    EXPECT_EQ(areg::ProxyAddress::to_path(back), text);
}

/**
 * \brief   A proxy written to a stream is read back equal; a truncated stream is not equal.
 **/
TEST(ProxyAddressTest, stream_round_trip_and_truncated)
{
    const areg::ProxyAddress proxy = make_proxy();
    areg::SharedBuffer buf;
    buf << proxy;
    const uint32_t used = buf.size_used();
    EXPECT_GT(used, 0u);

    buf.reset();
    areg::ProxyAddress back;
    buf >> back;
    EXPECT_EQ(back, proxy);
    EXPECT_EQ(back.cookie(), areg::COOKIE_LOCAL);
    EXPECT_EQ(static_cast<uint32_t>(back.thread()), THREAD_NUM);
    EXPECT_EQ(back.service_version(), svc_version());

    areg::SharedBuffer part = cut(buf, used - 1u);
    areg::ProxyAddress partial;
    part >> partial;
    EXPECT_NE(partial, proxy);

    areg::SharedBuffer none;
    areg::ProxyAddress nothing;
    none >> nothing;
    EXPECT_FALSE(nothing.is_valid());
}

/**
 * \brief   Without a running dispatcher thread the delivery of an event fails and no thread is set.
 **/
TEST(ProxyAddressTest, set_unknown_thread_invalidates_proxy)
{
    areg::ProxyAddress proxy = make_proxy();
    ASSERT_TRUE(proxy.is_valid());

    proxy.set_thread(areg::String("NoSuchDispatcherThread"));
    EXPECT_FALSE(proxy.thread().is_valid());
    EXPECT_FALSE(proxy.is_valid());

    proxy = make_proxy();
    proxy.set_thread(areg::ThreadAddress(areg::String("NoSuchDispatcherThread")));
    EXPECT_FALSE(proxy.thread().is_valid());
    EXPECT_FALSE(proxy.is_valid());
}

////////////////////////////////////////////////////////////////////////////
// StubAddress
////////////////////////////////////////////////////////////////////////////

/**
 * \brief   A default stub is not valid and neither local nor remote.
 **/
TEST(StubAddressTest, default_is_invalid)
{
    const areg::StubAddress stub;
    EXPECT_FALSE(stub.is_valid());
    EXPECT_FALSE(stub.is_local_address());
    EXPECT_FALSE(stub.is_remote_address());
    EXPECT_FALSE(stub.thread().is_valid());
    EXPECT_FALSE(areg::StubAddress::invalid_stub_address().is_valid());
}

/**
 * \brief   A stub of numbers is valid and local; the parts are returned.
 **/
TEST(StubAddressTest, construct_from_numbers)
{
    const areg::StubAddress stub = make_stub();
    EXPECT_TRUE(stub.is_valid());
    EXPECT_TRUE(stub.is_local_address());
    EXPECT_FALSE(stub.is_remote_address());
    EXPECT_EQ(stub.cookie(), areg::COOKIE_LOCAL);
    EXPECT_EQ(static_cast<uint32_t>(stub), ADDR_NUM);
    EXPECT_EQ(static_cast<uint32_t>(static_cast<const areg::ServiceAddress&>(stub)), ROLE_NUM);
    EXPECT_EQ(static_cast<uint32_t>(stub.thread()), THREAD_NUM);

    const areg::StubAddress byService(areg::ServiceAddress(SVC_NUM, svc_version(), SVC_TYPE, ROLE_NUM), THREAD_NUM, ADDR_NUM);
    EXPECT_EQ(byService, stub);
    EXPECT_TRUE(byService.is_valid());
}

/**
 * \brief   A stub of names without a running dispatcher thread is not valid, but keeps the service.
 **/
TEST(StubAddressTest, construct_from_names_without_dispatcher)
{
    const areg::StubAddress byNames(areg::String("StbSvc"), svc_version(), SVC_TYPE, areg::String("StbRole"), areg::String("NoSuchThread"));
    EXPECT_FALSE(byNames.is_valid());
    EXPECT_STREQ(byNames.role_name().as_string(), "StbRole");
    EXPECT_FALSE(byNames.thread().is_valid());
    EXPECT_TRUE(byNames.is_local_address());

    const areg::StubAddress byItem(make_item("StbSvc"), areg::String("StbRole"), areg::String("NoSuchThread"));
    EXPECT_FALSE(byItem.is_valid());
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(byItem), static_cast<const areg::ServiceAddress&>(byNames));

    areg::InterfaceData data;
    data.idServiceName = "StbSvc";
    data.idVersion = svc_version();
    data.idServiceType = SVC_TYPE;
    const areg::StubAddress byData(data, areg::String("StbRole"), areg::String("NoSuchThread"));
    EXPECT_FALSE(byData.is_valid());
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(byData), static_cast<const areg::ServiceAddress&>(byNames));
}

/**
 * \brief   A stub of a service address, a proxy or a moved service address keeps the service and has no thread.
 **/
TEST(StubAddressTest, construct_from_service_address_and_proxy)
{
    const areg::ServiceAddress service(SVC_NUM, svc_version(), SVC_TYPE, ROLE_NUM);
    const areg::StubAddress fromService(service);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(fromService), service);
    EXPECT_FALSE(fromService.thread().is_valid());
    EXPECT_FALSE(fromService.is_valid());
    EXPECT_TRUE(fromService.is_local_address());

    const areg::StubAddress fromProxy(make_proxy());
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(fromProxy), service);
    EXPECT_FALSE(fromProxy.is_valid());

    areg::ServiceAddress movable(service);
    const areg::StubAddress fromMoved(std::move(movable));
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(fromMoved), service);
    EXPECT_FALSE(fromMoved.thread().is_valid());
}

/**
 * \brief   Assigning a service address replaces the service and drops the thread, the channel and the identity.
 **/
TEST(StubAddressTest, assign_service_address_resets_stub_parts)
{
    const areg::ServiceAddress other(SVC_NUM + 1u, svc_version(), SVC_TYPE, ROLE_NUM + 1u);

    areg::StubAddress stub = make_stub();
    stub = other;
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(stub), other);
    EXPECT_FALSE(stub.thread().is_valid());
    EXPECT_FALSE(stub.is_valid());
    EXPECT_FALSE(stub.channel().is_valid());

    areg::StubAddress stub2 = make_stub();
    areg::ServiceAddress movable(other);
    stub2 = std::move(movable);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(stub2), other);
    EXPECT_FALSE(stub2.is_valid());
}

/**
 * \brief   Copy keeps all parts; move keeps them in the target; self assignment changes nothing.
 **/
TEST(StubAddressTest, copy_and_move)
{
    areg::StubAddress stub = make_stub();
    stub.set_source(5u);

    areg::StubAddress copy(stub);
    EXPECT_EQ(copy, stub);
    EXPECT_EQ(copy.channel(), stub.channel());

    areg::StubAddress assigned;
    assigned = stub;
    EXPECT_EQ(assigned, stub);
    EXPECT_EQ(assigned.source(), 5u);
    {
        const auto& self = assigned;
        assigned = self;
    }
    EXPECT_EQ(assigned, stub);

    areg::StubAddress moved(std::move(copy));
    EXPECT_EQ(moved, stub);
    EXPECT_FALSE(copy.is_valid());

    areg::StubAddress moveAssigned;
    moveAssigned = std::move(moved);
    EXPECT_EQ(moveAssigned, stub);
    EXPECT_EQ(moveAssigned.channel(), stub.channel());
}

/**
 * \brief   The source flags follow the cookie and the source; a stub has no target accessors but the flags exist.
 **/
TEST(StubAddressTest, local_and_remote_flags)
{
    areg::StubAddress stub = make_stub();
    EXPECT_FALSE(stub.is_source_local());
    EXPECT_FALSE(stub.is_source_public());
    EXPECT_FALSE(stub.is_target_local());
    EXPECT_FALSE(stub.is_target_public());

    stub.set_source(10u);
    EXPECT_EQ(stub.source(), 10u);
    EXPECT_TRUE(stub.is_source_local());
    EXPECT_FALSE(stub.is_source_public());

    areg::Channel ch = stub.channel();
    ch.set_target(20u);
    stub.set_channel(ch);
    EXPECT_TRUE(stub.is_target_local());
    EXPECT_TRUE(stub.is_target_public());

    stub.set_cookie(areg::COOKIE_REMOTE_SERVICE);
    EXPECT_FALSE(stub.is_local_address());
    EXPECT_TRUE(stub.is_remote_address());
    EXPECT_FALSE(stub.is_source_local());
    EXPECT_TRUE(stub.is_source_public());
    EXPECT_TRUE(stub.is_target_public());
}

/**
 * \brief   Invalidating the channel makes the stub not valid.
 **/
TEST(StubAddressTest, channel_validity)
{
    areg::StubAddress stub = make_stub();
    ASSERT_TRUE(stub.is_valid());
    stub.invalidate_channel();
    EXPECT_FALSE(stub.is_valid());
    EXPECT_FALSE(stub.is_local_address());
}

/**
 * \brief   Stubs are equal when identity and cookie match; hash and equal_to follow the service address.
 **/
TEST(StubAddressTest, compare_and_hash)
{
    const areg::StubAddress a = make_stub();
    areg::StubAddress same = make_stub();
    EXPECT_TRUE(a == same);
    EXPECT_FALSE(a != same);
    EXPECT_TRUE(std::equal_to<areg::StubAddress>{}(a, same));
    EXPECT_EQ(std::hash<areg::StubAddress>{}(a), ADDR_NUM);

    same.set_cookie(areg::COOKIE_REMOTE_SERVICE);
    EXPECT_TRUE(a != same);
    EXPECT_FALSE(a == same);
    EXPECT_TRUE(std::equal_to<areg::StubAddress>{}(a, same));

    EXPECT_FALSE(std::equal_to<areg::StubAddress>{}(a, make_stub(ROLE_NUM + 1u)));
}

/**
 * \brief   A stub serves a valid proxy of the same service, role and a not higher minor version.
 **/
TEST(StubAddressTest, proxy_compatibility)
{
    const areg::StubAddress stub = make_stub();
    EXPECT_TRUE(stub.is_proxy_compatible(make_proxy()));
    EXPECT_TRUE(stub == make_proxy());
    EXPECT_TRUE(stub.is_proxy_compatible(make_proxy(ROLE_NUM, areg::Version(1, 1, 0))));

    EXPECT_FALSE(stub.is_proxy_compatible(make_proxy(ROLE_NUM + 1u)));
    EXPECT_FALSE(stub.is_proxy_compatible(make_proxy(ROLE_NUM, areg::Version(2, 2, 3))));
    EXPECT_FALSE(stub.is_proxy_compatible(make_proxy(ROLE_NUM, areg::Version(1, 5, 0))));
    EXPECT_FALSE(stub.is_proxy_compatible(areg::ProxyAddress::invalid_proxy_address()));
    EXPECT_FALSE(areg::StubAddress::invalid_stub_address().is_proxy_compatible(make_proxy()));
}

/**
 * \brief   The text of a stub starts with the kind and parses back to an equal stub.
 **/
TEST(StubAddressTest, string_round_trip_with_thread_name)
{
    const char* text = "stub::54321::StbSvc::1.2.3::128::StbRole::StbThread::7.0.1";
    const char* next{ nullptr };
    const areg::StubAddress stub = areg::StubAddress::from_path(text, &next);
    EXPECT_EQ(static_cast<uint32_t>(stub), 54321u);
    EXPECT_STREQ(stub.role_name().as_string(), "StbRole");
    EXPECT_STREQ(stub.thread().name().as_string(), "StbThread");
    EXPECT_EQ(stub.service_version(), svc_version());
    EXPECT_EQ(stub.source(), 7u);
    EXPECT_EQ(stub.cookie(), 1u);
    EXPECT_TRUE((next == nullptr) || (*next == '\0'));

    EXPECT_STREQ(areg::StubAddress::to_path(stub).as_string(), text);

    areg::StubAddress again;
    again.from_string(areg::StubAddress::to_path(stub).as_string());
    EXPECT_EQ(again, stub);
    EXPECT_EQ(again.channel(), stub.channel());
}

/**
 * \brief   Text of another kind, empty text or truncated text does not give a valid stub.
 **/
TEST(StubAddressTest, string_empty_and_malformed)
{
    EXPECT_FALSE(areg::StubAddress::from_path("").is_valid());
    EXPECT_FALSE(areg::StubAddress::from_path("garbage").is_valid());
    EXPECT_FALSE(areg::StubAddress::from_path("proxy::12345::S::1.0.0::128::R::T::1.2.3").is_valid());
    EXPECT_FALSE(areg::StubAddress::from_path("stub::12345").is_valid());
    EXPECT_FALSE(areg::StubAddress::from_path("stub::12345::S::1.0.0::128::R::T").is_valid());
}

/**
 * \brief   Endpoint and event header conversion keep the service, role, identity and channel parts.
 **/
TEST(StubAddressTest, endpoint_and_event_round_trip)
{
    areg::StubAddress stub = make_stub();
    stub.set_source(11u);

    areg::RawService raw;
    areg::Endpoint end;
    stub.to_endpoint(raw, end);
    EXPECT_EQ(end.id, areg::COOKIE_LOCAL);
    EXPECT_EQ(end.number, ADDR_NUM);
    EXPECT_EQ(raw.role, ROLE_NUM);
    EXPECT_EQ(raw.service, SVC_NUM);

    const areg::StubAddress fromEnd(raw, end);
    EXPECT_EQ(fromEnd, stub);
    EXPECT_EQ(fromEnd.source(), static_cast<uint32_t>(stub.thread()));

    areg::EventHeader header;
    stub.to_event(header);
    EXPECT_EQ(header.provider.number, ADDR_NUM);
    EXPECT_EQ(header.rawService.role, ROLE_NUM);

    const areg::StubAddress fromHeader(header);
    EXPECT_EQ(fromHeader, stub);

    areg::StubAddress assigned;
    assigned.from_event(header);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(assigned), static_cast<const areg::ServiceAddress&>(stub));
    EXPECT_EQ(assigned.source(), THREAD_NUM);
    EXPECT_EQ(assigned.cookie(), areg::COOKIE_LOCAL);
    areg::StubAddress assigned2;
    assigned2.from_endpoint(raw, end);
    EXPECT_EQ(assigned2.source(), THREAD_NUM);
    EXPECT_EQ(assigned2.cookie(), areg::COOKIE_LOCAL);
}

/**
 * \brief   Reading the endpoint restores the identity number that the endpoint carries.
 **/
TEST(StubAddressTest, from_endpoint_restores_identity)
{
    areg::StubAddress stub = make_stub();
    areg::RawService raw;
    areg::Endpoint end;
    stub.to_endpoint(raw, end);
    const areg::StubAddress built(raw, end);

    areg::StubAddress assigned(make_service("OldSvc", "OldRole"), 0x52u, 0x53u);
    assigned.set_cookie(77u);
    assigned.from_endpoint(raw, end);
    EXPECT_EQ(static_cast<uint32_t>(assigned), ADDR_NUM);
    EXPECT_EQ(assigned, stub);
    EXPECT_EQ(assigned, built);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(assigned), static_cast<const areg::ServiceAddress&>(stub));
    EXPECT_EQ(static_cast<uint32_t>(assigned.service()), SVC_NUM);
    EXPECT_EQ(assigned.service_version(), stub.service_version());
    EXPECT_EQ(assigned.service_type(), stub.service_type());
    EXPECT_EQ(static_cast<uint32_t>(assigned.thread()), THREAD_NUM);
    EXPECT_EQ(assigned.cookie(), stub.cookie());
    EXPECT_EQ(assigned.source(), built.source());
    EXPECT_TRUE(assigned.service_name().is_empty());
    EXPECT_TRUE(assigned.role_name().is_empty());
    EXPECT_EQ(areg::StubAddress::to_path(assigned), areg::StubAddress::to_path(built));

    areg::EventHeader header;
    stub.to_event(header);
    areg::StubAddress fromEvent(make_service("OldSvc", "OldRole"), 0x52u, 0x53u);
    fromEvent.from_event(header);
    EXPECT_EQ(fromEvent, stub);
    EXPECT_EQ(fromEvent, areg::StubAddress(header));
}

/**
 * \brief   A stub of numbers keeps every part, the thread included, through the text.
 **/
TEST(StubAddressTest, string_numeric_round_trip)
{
    const areg::StubAddress stub = make_stub();
    const areg::String text = areg::StubAddress::to_path(stub);
    EXPECT_STREQ(text.as_string(), "stub::16384::#4096::1.2.3::128::#8192::#12288::0.0.1");

    const areg::StubAddress back = areg::StubAddress::from_path(text.as_string());
    EXPECT_EQ(back, stub);
    EXPECT_EQ(static_cast<const areg::ServiceAddress&>(back), static_cast<const areg::ServiceAddress&>(stub));
    EXPECT_EQ(static_cast<uint32_t>(back.service()), SVC_NUM);
    EXPECT_EQ(static_cast<uint32_t>(back.thread()), THREAD_NUM);
    EXPECT_EQ(back.channel(), stub.channel());
    EXPECT_EQ(areg::StubAddress::to_path(back), text);
}

/**
 * \brief   A stub written to a stream is read back equal; a truncated stream is not equal.
 **/
TEST(StubAddressTest, stream_round_trip_and_truncated)
{
    const areg::StubAddress stub = make_stub();
    areg::SharedBuffer buf;
    buf << stub;
    const uint32_t used = buf.size_used();
    EXPECT_GT(used, 0u);

    buf.reset();
    areg::StubAddress back;
    buf >> back;
    EXPECT_EQ(back, stub);
    EXPECT_EQ(back.cookie(), areg::COOKIE_LOCAL);
    EXPECT_EQ(static_cast<uint32_t>(back.thread()), THREAD_NUM);

    areg::SharedBuffer part = cut(buf, used - 1u);
    areg::StubAddress partial;
    part >> partial;
    EXPECT_NE(partial, stub);

    areg::SharedBuffer none;
    areg::StubAddress nothing;
    none >> nothing;
    EXPECT_FALSE(nothing.is_valid());
}

/**
 * \brief   An unknown thread name or address makes the stub not valid and removes its thread.
 **/
TEST(StubAddressTest, set_unknown_thread_invalidates_stub)
{
    areg::StubAddress stub = make_stub();
    ASSERT_TRUE(stub.is_valid());

    stub.set_thread(areg::String("NoSuchDispatcherThread"));
    EXPECT_FALSE(stub.thread().is_valid());
    EXPECT_FALSE(stub.is_valid());

    stub = make_stub();
    stub.set_thread(areg::ThreadAddress(areg::String("NoSuchDispatcherThread")));
    EXPECT_FALSE(stub.thread().is_valid());
    EXPECT_FALSE(stub.is_valid());
}

////////////////////////////////////////////////////////////////////////////
// ThreadAddress text
////////////////////////////////////////////////////////////////////////////

/**
 * \brief   A thread of a number is written with the marker and read back equal, with no name.
 **/
TEST(ThreadAddressTextTest, numeric_round_trip)
{
    const areg::ThreadAddress thread(THREAD_NUM);
    const areg::String text = areg::ThreadAddress::to_path(thread);
    EXPECT_STREQ(text.as_string(), "#12288");

    const areg::ThreadAddress back = areg::ThreadAddress::from_path(text.as_string());
    EXPECT_EQ(back, thread);
    EXPECT_EQ(static_cast<uint32_t>(back), THREAD_NUM);
    EXPECT_TRUE(back.name().is_empty());
}

/**
 * \brief   A thread name of digits is a name and keeps the checksum of the name.
 **/
TEST(ThreadAddressTextTest, digit_name_is_a_name)
{
    const areg::ThreadAddress thread(areg::String("12288"));
    EXPECT_EQ(static_cast<uint32_t>(thread), areg::crc32_calculate("12288"));

    const areg::ThreadAddress back = areg::ThreadAddress::from_path(areg::ThreadAddress::to_path(thread).as_string());
    EXPECT_EQ(back, thread);
    EXPECT_STREQ(back.name().as_string(), "12288");
}

/**
 * \brief   A thread read from a stream or an endpoint has no name, so its text carries the number.
 **/
TEST(ThreadAddressTextTest, stream_and_endpoint_read_text_round_trip)
{
    const areg::ThreadAddress thread(areg::String("Worker"));

    areg::SharedBuffer buf;
    buf << thread;
    buf.reset();
    areg::ThreadAddress streamed(areg::String("Stale"));
    buf >> streamed;
    EXPECT_EQ(streamed, thread);
    EXPECT_TRUE(streamed.name().is_empty());
    EXPECT_EQ(areg::ThreadAddress::from_path(areg::ThreadAddress::to_path(streamed).as_string()), thread);

    areg::Endpoint end;
    thread.to_endpoint(end);
    areg::ThreadAddress fromEnd(areg::String("Stale"));
    fromEnd.from_endpoint(end);
    EXPECT_EQ(areg::ThreadAddress::from_path(areg::ThreadAddress::to_path(fromEnd).as_string()), thread);
}
