/************************************************************************
 * An interface that gained entries keeps the IDs of the entries it had. The extended
 * OrderForward holds the entries of OrderReverse plus a request, a response, a
 * broadcast and an attribute; only its header is used.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "tests/message-id/services/extended/OrderForward.hpp"
#include "tests/message-id/services/OrderReverse.hpp"

namespace
{
    template <typename ENUM>
    constexpr uint32_t id(ENUM value) noexcept
    {
        return static_cast<uint32_t>(value);
    }

    using Ext = OrderForward::MessageIDs;
    using Rev = OrderReverse::MessageIDs;
}

static_assert(id(Ext::MsgId_request_alpha)     == id(Rev::MsgId_request_alpha),     "an added entry changes no other ID");
static_assert(id(Ext::MsgId_request_beta)      == id(Rev::MsgId_request_beta),      "an added entry changes no other ID");
static_assert(id(Ext::MsgId_request_gamma)     == id(Rev::MsgId_request_gamma),     "an added entry changes no other ID");
static_assert(id(Ext::MsgId_response_alpha)    == id(Rev::MsgId_response_alpha),    "an added entry changes no other ID");
static_assert(id(Ext::MsgId_response_gamma)    == id(Rev::MsgId_response_gamma),    "an added entry changes no other ID");
static_assert(id(Ext::MsgId_broadcast_alpha)   == id(Rev::MsgId_broadcast_alpha),   "an added entry changes no other ID");
static_assert(id(Ext::MsgId_broadcast_changed) == id(Rev::MsgId_broadcast_changed), "an added entry changes no other ID");
static_assert(id(Ext::MsgId_Count)             == id(Rev::MsgId_Count),             "an added entry changes no other ID");
static_assert(id(Ext::MsgId_Level)             == id(Rev::MsgId_Level),             "an added entry changes no other ID");
static_assert(id(Ext::MsgId_Mode)              == id(Rev::MsgId_Mode),              "an added entry changes no other ID");

TEST(MessageIdentityTest, added_entries_have_ids_of_their_own_names)
{
    EXPECT_EQ(id(Ext::MsgId_request_delta),   areg::crc32_calculate("request_delta"));
    EXPECT_EQ(id(Ext::MsgId_response_delta),  areg::crc32_calculate("response_delta"));
    EXPECT_EQ(id(Ext::MsgId_broadcast_added), areg::crc32_calculate("broadcast_added"));
    EXPECT_EQ(id(Ext::MsgId_Extra),           areg::crc32_calculate("attribute_Extra"));
    EXPECT_EQ(sizeof(OrderForward::MessageIdList) / sizeof(uint32_t), 14u);
}
