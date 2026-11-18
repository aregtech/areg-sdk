/************************************************************************
 * The message ID of a service interface entry is the CRC-32C of its kind and its name:
 * it does not depend on the order of the entries, on other entries, or on the interface.
 * Inside the process an entry is named by its kind and its dense index in that kind.
 * Uses the generated interfaces OrderForward and OrderReverse, which hold the same
 * entries in reversed order.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/TestMessageTable.hpp"
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
    constexpr uint32_t idx(ENUM value) noexcept
    {
        return static_cast<uint32_t>(value);
    }

    namespace Fwd = OrderForward;
    namespace Rev = OrderReverse;

    constexpr areg::CallType KINDS[]{ areg::CallType::Request, areg::CallType::Response, areg::CallType::Broadcast, areg::CallType::Attribute };
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
    EXPECT_EQ(Fwd::message_id(Fwd::RequestIDs::MsgId_request_alpha),       areg::crc32_calculate("request_alpha"));
    EXPECT_EQ(Fwd::message_id(Fwd::RequestIDs::MsgId_request_beta),        areg::crc32_calculate("request_beta"));
    EXPECT_EQ(Fwd::message_id(Fwd::RequestIDs::MsgId_request_gamma),       areg::crc32_calculate("request_gamma"));
    EXPECT_EQ(Fwd::message_id(Fwd::ResponseIDs::MsgId_response_alpha),     areg::crc32_calculate("response_alpha"));
    EXPECT_EQ(Fwd::message_id(Fwd::ResponseIDs::MsgId_response_gamma),     areg::crc32_calculate("response_gamma"));
    EXPECT_EQ(Fwd::message_id(Fwd::BroadcastIDs::MsgId_broadcast_alpha),   areg::crc32_calculate("broadcast_alpha"));
    EXPECT_EQ(Fwd::message_id(Fwd::BroadcastIDs::MsgId_broadcast_changed), areg::crc32_calculate("broadcast_changed"));
    EXPECT_EQ(Fwd::message_id(Fwd::AttributeIDs::MsgId_Count),             areg::crc32_calculate("attribute_Count"));
    EXPECT_EQ(Fwd::message_id(Fwd::AttributeIDs::MsgId_Level),             areg::crc32_calculate("attribute_Level"));
    EXPECT_EQ(Fwd::message_id(Fwd::AttributeIDs::MsgId_Mode),              areg::crc32_calculate("attribute_Mode"));
}

//////////////////////////////////////////////////////////////////////////
// 2. Order independence and the name as identity
//////////////////////////////////////////////////////////////////////////

static_assert(Fwd::message_id(Fwd::RequestIDs::MsgId_request_alpha)       == Rev::message_id(Rev::RequestIDs::MsgId_request_alpha),       "the order of the entries changes no ID");
static_assert(Fwd::message_id(Fwd::RequestIDs::MsgId_request_beta)        == Rev::message_id(Rev::RequestIDs::MsgId_request_beta),        "the order of the entries changes no ID");
static_assert(Fwd::message_id(Fwd::RequestIDs::MsgId_request_gamma)       == Rev::message_id(Rev::RequestIDs::MsgId_request_gamma),       "the order of the entries changes no ID");
static_assert(Fwd::message_id(Fwd::ResponseIDs::MsgId_response_alpha)     == Rev::message_id(Rev::ResponseIDs::MsgId_response_alpha),     "the order of the entries changes no ID");
static_assert(Fwd::message_id(Fwd::ResponseIDs::MsgId_response_gamma)     == Rev::message_id(Rev::ResponseIDs::MsgId_response_gamma),     "the order of the entries changes no ID");
static_assert(Fwd::message_id(Fwd::BroadcastIDs::MsgId_broadcast_alpha)   == Rev::message_id(Rev::BroadcastIDs::MsgId_broadcast_alpha),   "the order of the entries changes no ID");
static_assert(Fwd::message_id(Fwd::BroadcastIDs::MsgId_broadcast_changed) == Rev::message_id(Rev::BroadcastIDs::MsgId_broadcast_changed), "the order of the entries changes no ID");
static_assert(Fwd::message_id(Fwd::AttributeIDs::MsgId_Count)             == Rev::message_id(Rev::AttributeIDs::MsgId_Count),             "the order of the entries changes no ID");
static_assert(Fwd::message_id(Fwd::AttributeIDs::MsgId_Level)             == Rev::message_id(Rev::AttributeIDs::MsgId_Level),             "the order of the entries changes no ID");
static_assert(Fwd::message_id(Fwd::AttributeIDs::MsgId_Mode)              == Rev::message_id(Rev::AttributeIDs::MsgId_Mode),              "the order of the entries changes no ID");

static_assert(Fwd::message_id(Fwd::RequestIDs::MsgId_request_alpha) != Fwd::message_id(Fwd::ResponseIDs::MsgId_response_alpha),    "a request and a response of one name differ");
static_assert(Fwd::message_id(Fwd::RequestIDs::MsgId_request_alpha) != Fwd::message_id(Fwd::BroadcastIDs::MsgId_broadcast_alpha),  "a request and a broadcast of one name differ");
static_assert(Fwd::message_id(Fwd::ResponseIDs::MsgId_response_alpha) != Fwd::message_id(Fwd::BroadcastIDs::MsgId_broadcast_alpha), "a response and a broadcast of one name differ");
static_assert(areg::crc32_calculate("attribute_Count") != areg::crc32_calculate("attribute_count"), "the name is case sensitive");

//////////////////////////////////////////////////////////////////////////
// 3. The dense index of every kind
//////////////////////////////////////////////////////////////////////////

static_assert((idx(Fwd::RequestIDs::MsgId_request_alpha) == 0u) && (idx(Fwd::RequestIDs::MsgId_request_beta) == 1u) && (idx(Fwd::RequestIDs::MsgId_request_gamma) == 2u), "requests in document order");
static_assert((idx(Fwd::ResponseIDs::MsgId_response_alpha) == 0u) && (idx(Fwd::ResponseIDs::MsgId_response_gamma) == 1u), "responses in document order");
static_assert((idx(Fwd::BroadcastIDs::MsgId_broadcast_alpha) == 0u) && (idx(Fwd::BroadcastIDs::MsgId_broadcast_changed) == 1u), "broadcasts have their own index space");
static_assert((idx(Fwd::AttributeIDs::MsgId_Count) == 0u) && (idx(Fwd::AttributeIDs::MsgId_Level) == 1u) && (idx(Fwd::AttributeIDs::MsgId_Mode) == 2u), "attributes in document order");
static_assert((idx(Rev::RequestIDs::MsgId_request_alpha) == 2u) && (idx(Rev::AttributeIDs::MsgId_Count) == 2u), "the reversed order reverses the indexes");

TEST(MessageIdentityTest, both_orders_have_the_same_ids_and_their_own_indexes)
{
    const areg::InterfaceData & forward{ OrderForward::interface_data() };
    const areg::InterfaceData & reverse{ OrderReverse::interface_data() };
    for (areg::CallType kind : KINDS)
    {
        ASSERT_EQ(forward.count(kind), reverse.count(kind));
        for (uint32_t i = 0u; i < forward.count(kind); ++i)
        {
            const uint32_t msgId{ forward.message_id(kind, i) };
            const uint32_t there{ reverse.find(kind, msgId) };
            ASSERT_NE(there, areg::INVALID_MESSAGE_INDEX);
            EXPECT_EQ(reverse.message_id(kind, there), msgId);
            EXPECT_EQ(there, forward.count(kind) - 1u - i);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 4. The lookup of a message ID
//////////////////////////////////////////////////////////////////////////

TEST(MessageIdentityTest, the_table_of_every_kind_finds_every_entry)
{
    const areg::InterfaceData & ifData{ OrderForward::interface_data() };
    EXPECT_EQ(ifData.count(areg::CallType::Request), 3u);
    EXPECT_EQ(ifData.count(areg::CallType::Response), 2u);
    EXPECT_EQ(ifData.count(areg::CallType::Broadcast), 2u);
    EXPECT_EQ(ifData.count(areg::CallType::Attribute), 3u);

    for (const areg::MessageTable & table : ifData.idTables)
    {
        EXPECT_TRUE(table.finds_every_entry());
    }

    // The lists have internal linkage: the table holds a copy of the same values.
    for (uint32_t i = 0u; i < 3u; ++i)
    {
        EXPECT_EQ(ifData.requests().mtIds[i], OrderForward::RequestIdList[i]);
        EXPECT_EQ(ifData.attributes().mtIds[i], OrderForward::AttributeIdList[i]);
    }

    for (uint32_t i = 0u; i < 2u; ++i)
    {
        EXPECT_EQ(ifData.responses().mtIds[i], OrderForward::ResponseIdList[i]);
        EXPECT_EQ(ifData.broadcasts().mtIds[i], OrderForward::BroadcastIdList[i]);
    }

    EXPECT_EQ(ifData.response_index(idx(Fwd::RequestIDs::MsgId_request_alpha)), idx(Fwd::ResponseIDs::MsgId_response_alpha));
    EXPECT_EQ(ifData.response_index(idx(Fwd::RequestIDs::MsgId_request_beta)), areg::RESPONSE_INDEX_NONE);
    EXPECT_EQ(ifData.response_index(idx(Fwd::RequestIDs::MsgId_request_gamma)), idx(Fwd::ResponseIDs::MsgId_response_gamma));

    // The parameter counts: the responses, then the broadcasts.
    const uint32_t params[]{ 1u, 1u, 1u, 1u };
    for (uint32_t i = 0u; i < 4u; ++i)
    {
        EXPECT_EQ(ifData.idParamCountMap[i], params[i]) << i;
    }
}

TEST(MessageIdentityTest, the_table_of_a_kind_refuses_every_other_value)
{
    const areg::InterfaceData & ifData{ OrderForward::interface_data() };
    std::vector<uint32_t> others{ 0u, 0x80000000u, 0xFFFFFFFFu, areg::SYSTEM_ID_LAST, areg::SYSTEM_ID_LAST + 1u };
    for (areg::CallType kind : KINDS)
    {
        for (uint32_t i = 0u; i < ifData.count(kind); ++i)
        {
            const uint32_t msgId{ ifData.message_id(kind, i) };
            others.push_back(msgId);
            others.push_back(msgId - 1u);
            others.push_back(msgId + 1u);
        }
    }

    for (uint32_t sysId = static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection); sysId <= static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyDuplicate); ++sysId)
    {
        others.push_back(sysId);
    }

    for (areg::CallType kind : KINDS)
    {
        const areg::MessageTable & table{ ifData.idTables[areg::kind_index(kind)] };
        for (uint32_t other : others)
        {
            bool known{ false };
            for (uint32_t i = 0u; i < table.mtCount; ++i)
            {
                known = known || (table.mtIds[i] == other);
            }

            if (known == false)
            {
                EXPECT_EQ(ifData.find(kind, other), areg::INVALID_MESSAGE_INDEX) << "kind " << static_cast<uint32_t>(kind) << " id " << other;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 5. No reserved value, and the uniqueness check within a kind
//////////////////////////////////////////////////////////////////////////

TEST(MessageIdentityTest, a_found_crc_collision_is_detected)
{
    constexpr uint32_t first{ areg::crc32_calculate("request_value_1371838") };
    constexpr uint32_t second{ areg::crc32_calculate("request_value_2000402") };
    constexpr uint32_t pair[]{ first, second };
    static_assert(first == second, "the two names have one CRC-32C");
    static_assert(first == 0x48C92302u, "the CRC-32C of the pair");
    static_assert(areg::count_message_id(pair, first) == 2u, "the generated check refuses the pair in one kind");
}

TEST(MessageIdentityTest, a_value_of_the_system_range_is_an_ordinary_id)
{
    // Told apart from a system message by the call type, an ID below 0x10000 is found like any other.
    constexpr uint32_t inSystemRange{ areg::crc32_calculate("broadcast_value_014287") };
    static_assert(inSystemRange == 0x00000836u, "a name whose CRC-32C is a system id");

    constexpr uint32_t ids[]{ inSystemRange, 0u, 0x80000000u, 0xFFFFFFFFu };
    const areg_test::TestMessageTable built{ ids, 4u };
    const areg::MessageTable table{ built.table() };
    EXPECT_TRUE(table.finds_every_entry());
    EXPECT_EQ(table.find(inSystemRange), 0u);
    EXPECT_EQ(table.find(0u), 1u);
    EXPECT_EQ(table.find(0x80000000u), 2u);
    EXPECT_EQ(table.find(0xFFFFFFFFu), 3u);
    EXPECT_EQ(table.find(1u), areg::INVALID_MESSAGE_INDEX);
}
