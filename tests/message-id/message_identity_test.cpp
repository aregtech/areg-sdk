/************************************************************************
 * The message ID of a service interface entry is the CRC-32C of its kind and its name:
 * it does not depend on the order of the entries, on other entries, or on the interface.
 * Uses the generated interfaces OrderForward and OrderReverse, which hold the same
 * entries in reversed order.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "tests/message-id/services/OrderForward.hpp"
#include "tests/message-id/services/OrderReverse.hpp"

#include <vector>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "message_id_forward_generated")
    #pragma comment(lib, "message_id_reverse_generated")
#endif // _MSC_VER

namespace
{
    template <typename ENUM>
    constexpr uint32_t id(ENUM value) noexcept
    {
        return static_cast<uint32_t>(value);
    }

    using Fwd = OrderForward::MessageIDs;
    using Rev = OrderReverse::MessageIDs;
}

//////////////////////////////////////////////////////////////////////////
// 1. The hash
//////////////////////////////////////////////////////////////////////////

static_assert(areg::crc32_calculate("123456789") == 0xE3069283u, "CRC-32C check value");
static_assert(areg::crc32_calculate("request_identifier") == 0x71D8A2A1u, "request of example 24");
static_assert(areg::crc32_calculate("request_hello_ublock") == 0x47B40D83u, "request of example 24");
static_assert(areg::crc32_calculate("response_identifier") == 0x8F25F518u, "response of example 24");
static_assert(areg::crc32_calculate("response_hello_unblock") == 0x94B10238u, "response of example 24");
static_assert(areg::crc32_calculate("attribute_HelloServiceState") == 0x664B92D1u, "attribute of example 24");

TEST(MessageIdentityTest, generated_ids_are_the_crc_of_kind_and_name)
{
    EXPECT_EQ(id(Fwd::MsgId_request_alpha),     areg::crc32_calculate("request_alpha"));
    EXPECT_EQ(id(Fwd::MsgId_request_beta),      areg::crc32_calculate("request_beta"));
    EXPECT_EQ(id(Fwd::MsgId_request_gamma),     areg::crc32_calculate("request_gamma"));
    EXPECT_EQ(id(Fwd::MsgId_response_alpha),    areg::crc32_calculate("response_alpha"));
    EXPECT_EQ(id(Fwd::MsgId_response_gamma),    areg::crc32_calculate("response_gamma"));
    EXPECT_EQ(id(Fwd::MsgId_broadcast_alpha),   areg::crc32_calculate("broadcast_alpha"));
    EXPECT_EQ(id(Fwd::MsgId_broadcast_changed), areg::crc32_calculate("broadcast_changed"));
    EXPECT_EQ(id(Fwd::MsgId_Count),             areg::crc32_calculate("attribute_Count"));
    EXPECT_EQ(id(Fwd::MsgId_Level),             areg::crc32_calculate("attribute_Level"));
    EXPECT_EQ(id(Fwd::MsgId_Mode),              areg::crc32_calculate("attribute_Mode"));
    EXPECT_EQ(id(Fwd::MsgId_NotProcessed),      areg::RESPONSE_ID_NONE);
    EXPECT_EQ(id(Fwd::MsgId_Invalid),           areg::INVALID_MESSAGE_ID);
}

//////////////////////////////////////////////////////////////////////////
// 2. Order independence and the name as identity
//////////////////////////////////////////////////////////////////////////

static_assert(id(Fwd::MsgId_request_alpha)     == id(Rev::MsgId_request_alpha),     "the order of the entries changes no ID");
static_assert(id(Fwd::MsgId_request_beta)      == id(Rev::MsgId_request_beta),      "the order of the entries changes no ID");
static_assert(id(Fwd::MsgId_request_gamma)     == id(Rev::MsgId_request_gamma),     "the order of the entries changes no ID");
static_assert(id(Fwd::MsgId_response_alpha)    == id(Rev::MsgId_response_alpha),    "the order of the entries changes no ID");
static_assert(id(Fwd::MsgId_response_gamma)    == id(Rev::MsgId_response_gamma),    "the order of the entries changes no ID");
static_assert(id(Fwd::MsgId_broadcast_alpha)   == id(Rev::MsgId_broadcast_alpha),   "the order of the entries changes no ID");
static_assert(id(Fwd::MsgId_broadcast_changed) == id(Rev::MsgId_broadcast_changed), "the order of the entries changes no ID");
static_assert(id(Fwd::MsgId_Count)             == id(Rev::MsgId_Count),             "the order of the entries changes no ID");
static_assert(id(Fwd::MsgId_Level)             == id(Rev::MsgId_Level),             "the order of the entries changes no ID");
static_assert(id(Fwd::MsgId_Mode)              == id(Rev::MsgId_Mode),              "the order of the entries changes no ID");

static_assert(id(Fwd::MsgId_request_alpha) != id(Fwd::MsgId_response_alpha),  "a request and a response of one name differ");
static_assert(id(Fwd::MsgId_request_alpha) != id(Fwd::MsgId_broadcast_alpha), "a request and a broadcast of one name differ");
static_assert(id(Fwd::MsgId_response_alpha) != id(Fwd::MsgId_broadcast_alpha), "a response and a broadcast of one name differ");
static_assert(areg::crc32_calculate("attribute_Count") != areg::crc32_calculate("attribute_count"), "the name is case sensitive");

TEST(MessageIdentityTest, both_orders_have_the_same_ids_and_their_own_indexes)
{
    const areg::InterfaceData & forward{ OrderForward::interface_data() };
    const areg::InterfaceData & reverse{ OrderReverse::interface_data() };
    ASSERT_EQ(forward.idRequestCount, reverse.idRequestCount);
    ASSERT_EQ(forward.idResponseCount, reverse.idResponseCount);
    ASSERT_EQ(forward.idAttributeCount, reverse.idAttributeCount);

    for (uint32_t i = 0u; i < forward.idRequestCount; ++i)
    {
        const uint32_t msgId{ forward.idRequestList[i] };
        const areg::MessageInfo there{ reverse.idFind(msgId) };
        ASSERT_EQ(there.miKind, areg::CallType::Request);
        EXPECT_EQ(reverse.idRequestList[there.miIndex], msgId);
    }

    for (uint32_t i = 0u; i < forward.idAttributeCount; ++i)
    {
        const uint32_t msgId{ forward.idAttributeList[i] };
        const areg::MessageInfo there{ reverse.idFind(msgId) };
        ASSERT_EQ(there.miKind, areg::CallType::Attribute);
        EXPECT_EQ(reverse.idAttributeList[there.miIndex], msgId);
    }

    EXPECT_NE(OrderForward::find_message(id(Fwd::MsgId_request_alpha)).miIndex, OrderReverse::find_message(id(Rev::MsgId_request_alpha)).miIndex);
}

//////////////////////////////////////////////////////////////////////////
// 3. The lookup of a message ID
//////////////////////////////////////////////////////////////////////////

TEST(MessageIdentityTest, find_message_knows_every_entry)
{
    struct Row { Fwd msgId; areg::CallType kind; uint16_t index; uint8_t params; };
    const Row rows[]
    {
          { Fwd::MsgId_request_alpha,     areg::CallType::Request,   0u, 1u }
        , { Fwd::MsgId_request_beta,      areg::CallType::Request,   1u, 1u }
        , { Fwd::MsgId_request_gamma,     areg::CallType::Request,   2u, 2u }
        , { Fwd::MsgId_response_alpha,    areg::CallType::Response,  0u, 1u }
        , { Fwd::MsgId_response_gamma,    areg::CallType::Response,  1u, 1u }
        , { Fwd::MsgId_broadcast_alpha,   areg::CallType::Broadcast, 2u, 1u }
        , { Fwd::MsgId_broadcast_changed, areg::CallType::Broadcast, 3u, 1u }
        , { Fwd::MsgId_Count,             areg::CallType::Attribute, 0u, 1u }
        , { Fwd::MsgId_Level,             areg::CallType::Attribute, 1u, 1u }
        , { Fwd::MsgId_Mode,              areg::CallType::Attribute, 2u, 1u }
    };

    const areg::InterfaceData & ifData{ OrderForward::interface_data() };
    EXPECT_EQ(ifData.idFind, &OrderForward::find_message);
    for (const Row & row : rows)
    {
        SCOPED_TRACE(OrderForward::as_string(row.msgId));
        const areg::MessageInfo info{ OrderForward::find_message(id(row.msgId)) };
        EXPECT_TRUE(info.is_valid());
        EXPECT_EQ(info.miKind, row.kind);
        EXPECT_EQ(info.miIndex, row.index);
        EXPECT_EQ(info.miParams, row.params);
        EXPECT_TRUE(areg::is_interface_id(id(row.msgId)));

        const uint32_t * list{ row.kind == areg::CallType::Request ? ifData.idRequestList
                             : row.kind == areg::CallType::Attribute ? ifData.idAttributeList
                             : ifData.idResponseList };
        EXPECT_EQ(list[info.miIndex], id(row.msgId));
        if ((row.kind == areg::CallType::Response) || (row.kind == areg::CallType::Broadcast))
        {
            EXPECT_EQ(ifData.idResponseParamCountMap[info.miIndex], static_cast<uint32_t>(row.params));
        }
    }

    EXPECT_EQ(ifData.idRequestToResponseMap[0], id(Fwd::MsgId_response_alpha));
    EXPECT_EQ(ifData.idRequestToResponseMap[1], areg::RESPONSE_ID_NONE);
    EXPECT_EQ(ifData.idRequestToResponseMap[2], id(Fwd::MsgId_response_gamma));
}

TEST(MessageIdentityTest, find_message_knows_no_other_value)
{
    const uint32_t entries[]{ OrderForward::MessageIdList[0], OrderForward::MessageIdList[1], OrderForward::MessageIdList[2]
                            , OrderForward::MessageIdList[3], OrderForward::MessageIdList[4], OrderForward::MessageIdList[5]
                            , OrderForward::MessageIdList[6], OrderForward::MessageIdList[7], OrderForward::MessageIdList[8]
                            , OrderForward::MessageIdList[9] };
    static_assert(sizeof(OrderForward::MessageIdList) == sizeof(entries), "the list holds every entry");

    std::vector<uint32_t> others{ 0u, areg::INVALID_MESSAGE_ID, 0xFFFFFFFFu, areg::SYSTEM_ID_LAST, areg::SYSTEM_ID_LAST + 1u };
    for (uint32_t msgId : entries)
    {
        others.push_back(msgId - 1u);
        others.push_back(msgId + 1u);
    }

    for (uint32_t sysId = static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection); sysId <= static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyDuplicate); ++sysId)
    {
        others.push_back(sysId);
    }

    for (uint32_t other : others)
    {
        if (areg::count_message_id(OrderForward::MessageIdList, other) != 0u)
            continue;

        EXPECT_FALSE(OrderForward::find_message(other).is_valid()) << other;
        EXPECT_EQ(OrderForward::find_message(other).miKind, areg::CallType::Undefined) << other;
    }
}

//////////////////////////////////////////////////////////////////////////
// 4. The reserved values and the uniqueness check
//////////////////////////////////////////////////////////////////////////

TEST(MessageIdentityTest, interface_id_excludes_the_reserved_values)
{
    EXPECT_FALSE(areg::is_interface_id(0u));
    EXPECT_FALSE(areg::is_interface_id(0x0000FFFFu));
    EXPECT_TRUE(areg::is_interface_id(0x00010000u));
    EXPECT_TRUE(areg::is_interface_id(0x7FFFFFFFu));
    EXPECT_FALSE(areg::is_interface_id(0x80000000u));
    EXPECT_TRUE(areg::is_interface_id(0x80000001u));
    EXPECT_TRUE(areg::is_interface_id(0xFFFFFFFEu));
    EXPECT_FALSE(areg::is_interface_id(0xFFFFFFFFu));
    EXPECT_EQ(areg::INVALID_MESSAGE_ID, 0x80000000u);
}

TEST(MessageIdentityTest, unique_interface_id_refuses_a_duplicate_and_a_reserved_value)
{
    constexpr uint32_t a{ areg::crc32_calculate("request_a") };
    constexpr uint32_t b{ areg::crc32_calculate("request_b") };
    constexpr uint32_t unique[]{ a, b };
    constexpr uint32_t duplicate[]{ a, b, a };
    constexpr uint32_t reserved[]{ a, static_cast<uint32_t>(areg::FuncIdRange::SystemServiceConnect) };
    constexpr uint32_t invalid[]{ a, areg::INVALID_MESSAGE_ID };

    static_assert(areg::is_unique_interface_id(unique, a) && areg::is_unique_interface_id(unique, b), "distinct interface ids");
    static_assert(areg::count_message_id(duplicate, a) == 2u, "the duplicate is counted twice");
    EXPECT_FALSE(areg::is_unique_interface_id(duplicate, a));
    EXPECT_TRUE(areg::is_unique_interface_id(duplicate, b));
    EXPECT_FALSE(areg::is_unique_interface_id(reserved, reserved[1]));
    EXPECT_TRUE(areg::is_unique_interface_id(reserved, a));
    EXPECT_FALSE(areg::is_unique_interface_id(invalid, areg::INVALID_MESSAGE_ID));
    EXPECT_FALSE(areg::is_unique_interface_id(unique, 0u));
}

TEST(MessageIdentityTest, a_found_crc_collision_is_detected)
{
    constexpr uint32_t first{ areg::crc32_calculate("request_value_1371838") };
    constexpr uint32_t second{ areg::crc32_calculate("request_value_2000402") };
    constexpr uint32_t pair[]{ first, second };
    static_assert(first == second, "the two names have one CRC-32C");
    static_assert(first == 0x48C92302u, "the CRC-32C of the pair");
    EXPECT_FALSE(areg::is_unique_interface_id(pair, first));

    constexpr uint32_t inSystemRange{ areg::crc32_calculate("broadcast_value_014287") };
    static_assert(inSystemRange == 0x00000836u, "a name whose CRC-32C is a system id");
    EXPECT_FALSE(areg::is_interface_id(inSystemRange));
}
