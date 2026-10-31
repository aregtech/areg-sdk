/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/EventDefsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the event definitions (event types, priorities,
 *              call types and their predicates) and of the service manager event data.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Version.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/EventDefs.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/private/ServiceManagerEvents.hpp"

#include <cstring>
#include <set>

namespace
{
    using areg::EventType;

    //!< Converts an event type to its bits.
    constexpr uint32_t bits(EventType type)
    {
        return static_cast<uint32_t>(type);
    }

    //!< An event type with the masks it is composed of, as documented in EventDefs.hpp.
    struct TypeRow
    {
        EventType   type;
        uint32_t    masks;
        const char* name;
    };

    constexpr uint32_t M_INT   = bits(EventType::EventInternalMask);
    constexpr uint32_t M_EXT   = bits(EventType::EventExternalMask);
    constexpr uint32_t M_LOC   = bits(EventType::EventLocalMask);
    constexpr uint32_t M_REM   = bits(EventType::EventRemoteMask);
    constexpr uint32_t M_PROV  = bits(EventType::EventToProviderMask);
    constexpr uint32_t M_CONS  = bits(EventType::EventToConsumerMask);
    constexpr uint32_t M_SERV  = bits(EventType::EventToServiceMask);
    constexpr uint32_t M_SYS   = bits(EventType::EventToSystemMask);
    constexpr uint32_t M_REQ   = bits(EventType::EventRequestMask);
    constexpr uint32_t M_RESP  = bits(EventType::EventResponseMask);
    constexpr uint32_t M_BCAST = bits(EventType::EventBroadcastMask);
    constexpr uint32_t M_ATTR  = bits(EventType::EventAttributeMask);
    constexpr uint32_t M_NOTE  = bits(EventType::EventNotifyMask);
    constexpr uint32_t M_CONN  = bits(EventType::EventConnectMask);
    constexpr uint32_t M_CUST  = bits(EventType::EventCustomMask);
    constexpr uint32_t M_FAIL  = bits(EventType::EventFailureMask);

    //!< Every event type except the two without a name (see the disabled test).
    const TypeRow gNamedTypes[] =
    {
          { EventType::EventUnknown,               0u,  "areg::EventType::EventUnknown" }
        , { EventType::EventInternalMask,          M_INT, "areg::EventType::EventInternalMask" }
        , { EventType::EventExternalMask,          M_EXT, "areg::EventType::EventExternalMask" }
        , { EventType::EventLocalMask,             M_LOC, "areg::EventType::EventLocalMask" }
        , { EventType::EventRemoteMask,            M_REM, "areg::EventType::EventRemoteMask" }
        , { EventType::EventToProviderMask,        M_PROV,"areg::EventType::EventToProviderMask" }
        , { EventType::EventToConsumerMask,        M_CONS,"areg::EventType::EventToConsumerMask" }
        , { EventType::EventToServiceMask,         M_SERV,"areg::EventType::EventToServiceMask" }
        , { EventType::EventToSystemMask,          M_SYS, "areg::EventType::EventToSystemMask" }
        , { EventType::EventRequestMask,           M_REQ, "areg::EventType::EventRequestMask" }
        , { EventType::EventResponseMask,          M_RESP,"areg::EventType::EventResponseMask" }
        , { EventType::EventBroadcastMask,         M_BCAST,"areg::EventType::EventBroadcastMask" }
        , { EventType::EventAttributeMask,         M_ATTR,"areg::EventType::EventAttributeMask" }
        , { EventType::EventNotifyMask,            M_NOTE,"areg::EventType::EventNotifyMask" }
        , { EventType::EventConnectMask,           M_CONN,"areg::EventType::EventConnectMask" }
        , { EventType::EventCustomMask,            M_CUST,"areg::EventType::EventCustomMask" }
        , { EventType::EventFailureMask,           M_FAIL,"areg::EventType::EventFailureMask" }
        , { EventType::EventNotifyClient,          M_INT | M_LOC | M_NOTE, "areg::EventType::EventNotifyClient" }
        , { EventType::EventLocalRequest,          M_EXT | M_LOC | M_PROV | M_REQ, "areg::EventType::EventLocalRequest" }
        , { EventType::EventRemoteRequest,         M_EXT | M_REM | M_PROV | M_REQ, "areg::EventType::EventRemoteRequest" }
        , { EventType::EventLocalNotifyRequest,    M_EXT | M_LOC | M_PROV | M_REQ | M_NOTE, "areg::EventType::EventLocalNotifyRequest" }
        , { EventType::EventRemoteNotifyRequest,   M_EXT | M_REM | M_PROV | M_REQ | M_NOTE, "areg::EventType::EventRemoteNotifyRequest" }
        , { EventType::EventLocalNotifyResponse,   M_EXT | M_LOC | M_PROV | M_REQ | M_RESP | M_NOTE, "areg::EventType::EventLocalNotifyResponse" }
        , { EventType::EventRemoteNotifyResponse,  M_EXT | M_REM | M_PROV | M_REQ | M_RESP | M_NOTE, "areg::EventType::EventRemoteNotifyResponse" }
        , { EventType::EventLocalNotifyBroadcast,  M_EXT | M_LOC | M_PROV | M_REQ | M_BCAST | M_NOTE, "areg::EventType::EventLocalNotifyBroadcast" }
        , { EventType::EventRemoteNotifyBroadcast, M_EXT | M_REM | M_PROV | M_REQ | M_BCAST | M_NOTE, "areg::EventType::EventRemoteNotifyBroadcast" }
        , { EventType::EventLocalNotifyAttribute,  M_EXT | M_LOC | M_PROV | M_REQ | M_ATTR | M_NOTE, "areg::EventType::EventLocalNotifyAttribute" }
        , { EventType::EventRemoteNotifyAttribute, M_EXT | M_REM | M_PROV | M_REQ | M_ATTR | M_NOTE, "areg::EventType::EventRemoteNotifyAttribute" }
        , { EventType::EventLocalResponse,         M_EXT | M_LOC | M_CONS | M_RESP, "areg::EventType::EventLocalResponse" }
        , { EventType::EventRemoteResponse,        M_EXT | M_REM | M_CONS | M_RESP, "areg::EventType::EventRemoteResponse" }
        , { EventType::EventLocalBroadcast,        M_EXT | M_LOC | M_CONS | M_BCAST, "areg::EventType::EventLocalBroadcast" }
        , { EventType::EventRemoteBroadcast,       M_EXT | M_REM | M_CONS | M_BCAST, "areg::EventType::EventRemoteBroadcast" }
        , { EventType::EventLocalAttribute,        M_EXT | M_LOC | M_CONS | M_ATTR, "areg::EventType::EventLocalAttribute" }
        , { EventType::EventRemoteAttribute,       M_EXT | M_REM | M_CONS | M_ATTR, "areg::EventType::EventRemoteAttribute" }
        , { EventType::EventLocalProviderConnect,  M_EXT | M_LOC | M_PROV | M_NOTE | M_CONN, "areg::EventType::EventLocalProviderConnect" }
        , { EventType::EventRemoteProviderConnect, M_EXT | M_REM | M_PROV | M_NOTE | M_CONN, "areg::EventType::EventRemoteProviderConnect" }
        , { EventType::EventLocalConsumerConnect,  M_EXT | M_LOC | M_CONS | M_NOTE | M_CONN, "areg::EventType::EventLocalConsumerConnect" }
        , { EventType::EventRemoteConsumerConnect, M_EXT | M_REM | M_CONS | M_NOTE | M_CONN, "areg::EventType::EventRemoteConsumerConnect" }
        , { EventType::EventCustomInternal,        M_INT | M_LOC | M_CUST, "areg::EventType::EventCustomInternal" }
        , { EventType::EventCustomExternal,        M_EXT | M_LOC | M_CUST, "areg::EventType::EventCustomExternal" }
        , { EventType::EventLocalRequestFailed,    M_EXT | M_LOC | M_CONS | M_REQ | M_FAIL, "areg::EventType::EventLocalRequestFailed" }
        , { EventType::EventRemoteRequestFailed,   M_EXT | M_REM | M_CONS | M_REQ | M_FAIL, "areg::EventType::EventRemoteRequestFailed" }
    };

    //!< The connection event types, composed as documented.
    const TypeRow gConnectionTypes[] =
    {
          { EventType::EventLocalConnection,  M_EXT | M_LOC | M_CONS | M_PROV | M_SERV | M_NOTE | M_CONN, "areg::EventType::EventLocalConnection" }
        , { EventType::EventRemoteConnection, M_EXT | M_REM | M_CONS | M_PROV | M_SERV | M_NOTE | M_CONN, "areg::EventType::EventRemoteConnection" }
    };

    //!< Checks the type is in the set, the set holds the types the predicate must accept.
    bool contains(const std::set<EventType>& set, EventType type)
    {
        return (set.find(type) != set.end());
    }

    //!< Checks a predicate over every event type against the set of types it accepts.
    template <typename PRED>
    void check_predicate(const char* name, PRED pred, const std::set<EventType>& accepted)
    {
        for (const TypeRow& row : gNamedTypes)
        {
            EXPECT_EQ(pred(row.type), contains(accepted, row.type)) << name << " of " << row.name;
        }

        for (const TypeRow& row : gConnectionTypes)
        {
            EXPECT_EQ(pred(row.type), contains(accepted, row.type)) << name << " of " << row.name;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 1. Event types
//////////////////////////////////////////////////////////////////////////

TEST(EventDefsTest, event_type_values_follow_the_documented_masks)
{
    for (const TypeRow& row : gNamedTypes)
    {
        EXPECT_EQ(bits(row.type), row.masks) << row.name;
    }

    for (const TypeRow& row : gConnectionTypes)
    {
        EXPECT_EQ(bits(row.type), row.masks) << row.name;
    }
}

TEST(EventDefsTest, event_type_names)
{
    for (const TypeRow& row : gNamedTypes)
    {
        EXPECT_STREQ(areg::as_string(row.type), row.name);
    }
}

TEST(EventDefsTest, connection_event_type_names)
{
    for (const TypeRow& row : gConnectionTypes)
    {
        EXPECT_STREQ(areg::as_string(row.type), row.name);
    }
}

#ifndef DEBUG
TEST(EventDefsTest, undefined_event_type_name)
{
    EXPECT_EQ(std::strncmp(areg::as_string(static_cast<EventType>(0x7FFFFFFFu)), "ERR:", 4), 0);
}
#endif

TEST(EventDefsTest, event_type_is_valid)
{
    EXPECT_FALSE(areg::is_valid(EventType::EventUnknown));
    for (const TypeRow& row : gNamedTypes)
    {
        EXPECT_EQ(areg::is_valid(row.type), row.type != EventType::EventUnknown) << row.name;
    }
}

TEST(EventDefsTest, mask_predicates_follow_the_masks)
{
    struct MaskPredicate
    {
        const char* name;
        bool (*pred)(EventType);
        uint32_t    mask;
    };

    static const MaskPredicate predicates[] =
    {
          { "is_internal",    [](EventType t) { return areg::is_internal(t); },    M_INT }
        , { "is_external",    [](EventType t) { return areg::is_external(t); },    M_EXT }
        , { "is_local",       [](EventType t) { return areg::is_local(t); },       M_LOC }
        , { "is_remote",      [](EventType t) { return areg::is_remote(t); },      M_REM }
        , { "is_to_provider", [](EventType t) { return areg::is_to_provider(t); }, M_PROV }
        , { "is_to_consumer", [](EventType t) { return areg::is_to_consumer(t); }, M_CONS }
        , { "is_to_service",  [](EventType t) { return areg::is_to_service(t); },  M_SERV }
        , { "is_to_system",   [](EventType t) { return areg::is_to_system(t); },   M_SYS }
        , { "is_request",     [](EventType t) { return areg::is_request(t); },     M_REQ }
        , { "is_response",    [](EventType t) { return areg::is_response(t); },    M_RESP }
        , { "is_broadcast",   [](EventType t) { return areg::is_broadcast(t); },   M_BCAST }
        , { "is_attribute",   [](EventType t) { return areg::is_attribute(t); },   M_ATTR }
        , { "is_notify",      [](EventType t) { return areg::is_notify(t); },      M_NOTE }
        , { "is_connect",     [](EventType t) { return areg::is_connect(t); },     M_CONN }
        , { "is_custom",      [](EventType t) { return areg::is_custom(t); },      M_CUST }
        , { "is_failure",     [](EventType t) { return areg::is_failure(t); },     M_FAIL }
    };

    for (const MaskPredicate& predicate : predicates)
    {
        for (const TypeRow& row : gNamedTypes)
        {
            EXPECT_EQ(predicate.pred(row.type), (row.masks & predicate.mask) != 0u) << predicate.name << " of " << row.name;
        }

        for (const TypeRow& row : gConnectionTypes)
        {
            EXPECT_EQ(predicate.pred(row.type), (row.masks & predicate.mask) != 0u) << predicate.name << " of " << row.name;
        }
    }
}

TEST(EventDefsTest, notify_client_predicate)
{
    check_predicate("is_notify_client", [](EventType t) { return areg::is_notify_client(t); }
                  , { EventType::EventNotifyClient });
}

TEST(EventDefsTest, notify_response_predicate)
{
    check_predicate("is_notify_response", [](EventType t) { return areg::is_notify_response(t); }
                  , { EventType::EventLocalNotifyResponse, EventType::EventRemoteNotifyResponse });
}

TEST(EventDefsTest, notify_broadcast_predicate)
{
    check_predicate("is_notify_broadcast", [](EventType t) { return areg::is_notify_broadcast(t); }
                  , { EventType::EventLocalNotifyBroadcast, EventType::EventRemoteNotifyBroadcast });
}

TEST(EventDefsTest, notify_attribute_predicate)
{
    check_predicate("is_notify_attribute", [](EventType t) { return areg::is_notify_attribute(t); }
                  , { EventType::EventLocalNotifyAttribute, EventType::EventRemoteNotifyAttribute });
}

TEST(EventDefsTest, request_call_predicate)
{
    check_predicate("is_request_call", [](EventType t) { return areg::is_request_call(t); }
                  , { EventType::EventLocalRequest, EventType::EventRemoteRequest
                    , EventType::EventLocalNotifyRequest, EventType::EventRemoteNotifyRequest
                    , EventType::EventLocalNotifyResponse, EventType::EventRemoteNotifyResponse
                    , EventType::EventLocalNotifyBroadcast, EventType::EventRemoteNotifyBroadcast
                    , EventType::EventLocalNotifyAttribute, EventType::EventRemoteNotifyAttribute });
}

TEST(EventDefsTest, response_call_predicate)
{
    check_predicate("is_response_call", [](EventType t) { return areg::is_response_call(t); }
                  , { EventType::EventLocalResponse, EventType::EventRemoteResponse });
}

TEST(EventDefsTest, broadcast_call_predicate)
{
    check_predicate("is_broadcast_call", [](EventType t) { return areg::is_broadcast_call(t); }
                  , { EventType::EventLocalBroadcast, EventType::EventRemoteBroadcast });
}

TEST(EventDefsTest, attribute_call_predicate)
{
    check_predicate("is_attribute_call", [](EventType t) { return areg::is_attribute_call(t); }
                  , { EventType::EventLocalAttribute, EventType::EventRemoteAttribute });
}

TEST(EventDefsTest, provider_connect_predicate)
{
    check_predicate("is_provider_connect", [](EventType t) { return areg::is_provider_connect(t); }
                  , { EventType::EventLocalProviderConnect, EventType::EventRemoteProviderConnect
                    , EventType::EventLocalConnection, EventType::EventRemoteConnection });
}

TEST(EventDefsTest, consumer_connect_predicate)
{
    check_predicate("is_consumer_connect", [](EventType t) { return areg::is_consumer_connect(t); }
                  , { EventType::EventLocalConsumerConnect, EventType::EventRemoteConsumerConnect
                    , EventType::EventLocalConnection, EventType::EventRemoteConnection });
}

TEST(EventDefsTest, request_failure_predicate)
{
    check_predicate("is_request_failure", [](EventType t) { return areg::is_request_failure(t); }
                  , { EventType::EventLocalRequestFailed, EventType::EventRemoteRequestFailed });
}

TEST(EventDefsTest, matching_any_and_exact)
{
    const uint32_t localRequest{ bits(EventType::EventLocalRequest) };

    EXPECT_TRUE(areg::is_matching_any(localRequest, EventType::EventLocalMask));
    EXPECT_TRUE(areg::is_matching_any(localRequest, EventType::EventRequestMask));
    EXPECT_FALSE(areg::is_matching_any(localRequest, EventType::EventRemoteMask));
    EXPECT_FALSE(areg::is_matching_any(0u, EventType::EventLocalMask));
    EXPECT_FALSE(areg::is_matching_any(localRequest, EventType::EventUnknown));

    EXPECT_TRUE(areg::is_matching_exact(localRequest, bits(EventType::EventLocalMask) | bits(EventType::EventRequestMask)));
    EXPECT_TRUE(areg::is_matching_exact(localRequest, localRequest));
    EXPECT_FALSE(areg::is_matching_exact(localRequest, bits(EventType::EventLocalMask) | bits(EventType::EventRemoteMask)));
    EXPECT_FALSE(areg::is_matching_exact(bits(EventType::EventLocalMask), localRequest));
    EXPECT_TRUE(areg::is_matching_exact(localRequest, 0u));
}

//////////////////////////////////////////////////////////////////////////
// 2. Event priority and call type
//////////////////////////////////////////////////////////////////////////

TEST(EventDefsTest, event_priority_names_and_order)
{
    EXPECT_STREQ(areg::as_string(areg::EventPriority::NormalPrio), "areg::EventPriority::NormalPrio");
    EXPECT_STREQ(areg::as_string(areg::EventPriority::HighPrio), "areg::EventPriority::HighPrio");
    EXPECT_STREQ(areg::as_string(areg::EventPriority::CriticalPrio), "areg::EventPriority::CriticalPrio");
    EXPECT_STREQ(areg::as_string(areg::EventPriority::ExitPrio), "areg::EventPriority::ExitPrio");

    EXPECT_LT(static_cast<uint32_t>(areg::EventPriority::NormalPrio), static_cast<uint32_t>(areg::EventPriority::HighPrio));
    EXPECT_LT(static_cast<uint32_t>(areg::EventPriority::HighPrio), static_cast<uint32_t>(areg::EventPriority::CriticalPrio));
    EXPECT_LT(static_cast<uint32_t>(areg::EventPriority::CriticalPrio), static_cast<uint32_t>(areg::EventPriority::ExitPrio));
    EXPECT_EQ(areg::DefaultPriority, areg::EventPriority::NormalPrio);

#ifndef DEBUG
    EXPECT_EQ(std::strncmp(areg::as_string(static_cast<areg::EventPriority>(3u)), "ERR:", 4), 0);
#endif
}

TEST(EventDefsTest, event_call_type_names)
{
    EXPECT_STREQ(areg::as_string(areg::EventCallType::Undefined), "areg::EventCallType::Undefined");
    EXPECT_STREQ(areg::as_string(areg::EventCallType::CallLocal), "areg::EventCallType::CallLocal");
    EXPECT_STREQ(areg::as_string(areg::EventCallType::CallRemote), "areg::EventCallType::CallRemote");
    EXPECT_STREQ(areg::as_string(areg::EventCallType::CallCustom), "areg::EventCallType::CallCustom");
    EXPECT_STREQ(areg::as_string(areg::EventCallType::CallService), "areg::EventCallType::CallService");
    EXPECT_STREQ(areg::as_string(areg::EventCallType::CallServiceLocal), "areg::EventCallType::CallServiceLocal");
    EXPECT_STREQ(areg::as_string(areg::EventCallType::CallServiceRemote), "areg::EventCallType::CallServiceRemote");

#ifndef DEBUG
    EXPECT_EQ(std::strncmp(areg::as_string(static_cast<areg::EventCallType>(0x7Fu)), "ERR:", 4), 0);
#endif
}

//////////////////////////////////////////////////////////////////////////
// 3. Service manager event data
//////////////////////////////////////////////////////////////////////////

namespace
{
    using areg::ServiceManagerEventData;
    using Command = areg::ServiceManagerEventData::ServiceManagerCommand;

    //!< Returns a copy of the data, which is readable from its first byte.
    ServiceManagerEventData readable(const ServiceManagerEventData& data)
    {
        return ServiceManagerEventData(data);
    }

    areg::StubAddress make_stub()
    {
        areg::StubAddress stub("EventDefsService", areg::Version(1, 0, 0), areg::ServiceType::Public, "eventRole", "eventThread");
        stub.set_channel(areg::Channel(11u, 22u, 33u));
        return stub;
    }

    areg::ProxyAddress make_proxy()
    {
        areg::ProxyAddress proxy("EventDefsService", areg::Version(1, 0, 0), areg::ServiceType::Public, "eventRole", "eventThread");
        proxy.set_channel(areg::Channel(44u, 55u, 66u));
        return proxy;
    }
}

TEST(ServiceManagerEventsTest, command_names)
{
    static const struct
    {
        Command     command;
        const char* name;
    } table[] =
    {
          { Command::CMD_StopRoutingClient,    "ServiceManagerEventData::ServiceManagerCommand::CMD_StopRoutingClient" }
        , { Command::CMD_RegisterProxy,        "ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProxy" }
        , { Command::CMD_UnregisterProxy,      "ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProxy" }
        , { Command::CMD_RegisterProvider,     "ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterProvider" }
        , { Command::CMD_UnregisterProvider,   "ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterProvider" }
        , { Command::CMD_ConfigureConnection,  "ServiceManagerEventData::ServiceManagerCommand::CMD_ConfigureConnection" }
        , { Command::CMD_StartConnection,      "ServiceManagerEventData::ServiceManagerCommand::CMD_StartConnection" }
        , { Command::CMD_StartNetConnection,   "ServiceManagerEventData::ServiceManagerCommand::CMD_StartNetConnection" }
        , { Command::CMD_StopConnection,       "ServiceManagerEventData::ServiceManagerCommand::CMD_StopConnection" }
        , { Command::CMD_RegisterConnection,   "ServiceManagerEventData::ServiceManagerCommand::CMD_RegisterConnection" }
        , { Command::CMD_UnregisterConnection, "ServiceManagerEventData::ServiceManagerCommand::CMD_UnregisterConnection" }
        , { Command::CMD_LostConnection,       "ServiceManagerEventData::ServiceManagerCommand::CMD_LostConnection" }
        , { Command::CMD_ShutdownService,      "ServiceManagerEventData::ServiceManagerCommand::CMD_ShutdownService" }
        , { Command::CMD_LoadModel,            "ServiceManagerEventData::ServiceManagerCommand::CMD_LoadModel" }
        , { Command::CMD_UnloadModel,          "ServiceManagerEventData::ServiceManagerCommand::CMD_UnloadModel" }
        , { Command::CMD_ThreadRunning,        "ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadRunning" }
        , { Command::CMD_ThreadStopped,        "ServiceManagerEventData::ServiceManagerCommand::CMD_ThreadStopped" }
        , { Command::CMD_WatchdogExpired,      "ServiceManagerEventData::ServiceManagerCommand::CMD_WatchdogExpired" }
        , { Command::CMD_DeadlineExpired,      "ServiceManagerEventData::ServiceManagerCommand::CMD_DeadlineExpired" }
        , { Command::CMD_UserState,            "ServiceManagerEventData::ServiceManagerCommand::CMD_UserState" }
    };

    for (const auto& row : table)
    {
        EXPECT_STREQ(ServiceManagerEventData::as_string(row.command), row.name);
    }

    EXPECT_EQ(std::strncmp(ServiceManagerEventData::as_string(static_cast<Command>(0x7777u)), "ERR:", 4), 0);
}

TEST(ServiceManagerEventsTest, commands_without_payload)
{
    ServiceManagerEventData stopRouter = ServiceManagerEventData::stop_router_client();
    ServiceManagerEventData shutdown = ServiceManagerEventData::shutdown_service_manager();
    ServiceManagerEventData stop = ServiceManagerEventData::stop_connection();
    ServiceManagerEventData deadline = ServiceManagerEventData::deadline_expired();

    EXPECT_EQ(stopRouter.command(), Command::CMD_StopRoutingClient);
    EXPECT_EQ(shutdown.command(), Command::CMD_ShutdownService);
    EXPECT_EQ(stop.command(), Command::CMD_StopConnection);
    EXPECT_EQ(deadline.command(), Command::CMD_DeadlineExpired);
}

TEST(ServiceManagerEventsTest, register_and_unregister_proxy)
{
    const areg::ProxyAddress proxy = make_proxy();

    ServiceManagerEventData reg = ServiceManagerEventData::register_proxy(proxy);
    EXPECT_EQ(reg.command(), Command::CMD_RegisterProxy);
    {
        areg::ProxyAddress addr;
        areg::Channel channel;
        readable(reg).read_stream() >> addr >> channel;
        EXPECT_TRUE(addr == proxy);
        EXPECT_EQ(channel, proxy.channel());
    }

    ServiceManagerEventData unreg = ServiceManagerEventData::unregister_proxy(proxy, areg::DisconnectReason::ConsumerLost);
    EXPECT_EQ(unreg.command(), Command::CMD_UnregisterProxy);
    {
        areg::ProxyAddress addr;
        areg::Channel channel;
        areg::DisconnectReason reason{ areg::DisconnectReason::UndefinedReason };
        readable(unreg).read_stream() >> addr >> channel >> reason;
        EXPECT_TRUE(addr == proxy);
        EXPECT_EQ(channel, proxy.channel());
        EXPECT_EQ(reason, areg::DisconnectReason::ConsumerLost);
    }
}

TEST(ServiceManagerEventsTest, register_and_unregister_stub)
{
    const areg::StubAddress stub = make_stub();

    ServiceManagerEventData reg = ServiceManagerEventData::register_stub(stub);
    EXPECT_EQ(reg.command(), Command::CMD_RegisterProvider);
    {
        areg::StubAddress addr;
        areg::Channel channel;
        readable(reg).read_stream() >> addr >> channel;
        EXPECT_TRUE(addr == stub);
        EXPECT_EQ(channel, stub.channel());
    }

    ServiceManagerEventData unreg = ServiceManagerEventData::unregister_stub(stub, areg::DisconnectReason::ProviderRejected);
    EXPECT_EQ(unreg.command(), Command::CMD_UnregisterProvider);
    {
        areg::StubAddress addr;
        areg::Channel channel;
        areg::DisconnectReason reason{ areg::DisconnectReason::UndefinedReason };
        readable(unreg).read_stream() >> addr >> channel >> reason;
        EXPECT_TRUE(addr == stub);
        EXPECT_EQ(channel, stub.channel());
        EXPECT_EQ(reason, areg::DisconnectReason::ProviderRejected);
    }
}

TEST(ServiceManagerEventsTest, configure_and_start_connection)
{
    ServiceManagerEventData configure = ServiceManagerEventData::configure_connection(areg::RemoteServiceKind::Logger, 0x05u);
    EXPECT_EQ(configure.command(), Command::CMD_ConfigureConnection);
    {
        areg::RemoteServiceKind kind{ areg::RemoteServiceKind::Unknown };
        uint32_t types{ 0u };
        readable(configure).read_stream() >> kind >> types;
        EXPECT_EQ(kind, areg::RemoteServiceKind::Logger);
        EXPECT_EQ(types, 0x05u);
    }

    ServiceManagerEventData start = ServiceManagerEventData::start_connection(areg::RemoteServiceKind::Router, 0x03u);
    EXPECT_EQ(start.command(), Command::CMD_StartConnection);
    {
        areg::RemoteServiceKind kind{ areg::RemoteServiceKind::Unknown };
        uint32_t types{ 0u };
        readable(start).read_stream() >> kind >> types;
        EXPECT_EQ(kind, areg::RemoteServiceKind::Router);
        EXPECT_EQ(types, 0x03u);
    }
}

TEST(ServiceManagerEventsTest, start_net_connection)
{
    ServiceManagerEventData data = ServiceManagerEventData::start_net_connection(areg::String("192.168.1.7"), 8181u);

    EXPECT_EQ(data.command(), Command::CMD_StartNetConnection);
    areg::String address;
    uint16_t port{ 0u };
    readable(data).read_stream() >> address >> port;
    EXPECT_EQ(address, areg::String("192.168.1.7"));
    EXPECT_EQ(port, 8181u);
}

TEST(ServiceManagerEventsTest, channel_commands)
{
    const areg::Channel channel(7u, 8u, 9u);
    struct Row
    {
        ServiceManagerEventData data;
        Command                 command;
    };

    const Row rows[] =
    {
          { ServiceManagerEventData::register_connection(channel),   Command::CMD_RegisterConnection }
        , { ServiceManagerEventData::unregister_connection(channel), Command::CMD_UnregisterConnection }
        , { ServiceManagerEventData::lost_connection(channel),       Command::CMD_LostConnection }
    };

    for (const Row& row : rows)
    {
        EXPECT_EQ(row.data.command(), row.command);
        areg::Channel read;
        readable(row.data).read_stream() >> read;
        EXPECT_EQ(read, channel);
    }
}

TEST(ServiceManagerEventsTest, model_request_selects_load_or_unload)
{
    ServiceManagerEventData load = ServiceManagerEventData::model_request(true, areg::String("model-a"), 0x1122334455667788ull);
    EXPECT_EQ(load.command(), Command::CMD_LoadModel);
    {
        areg::String name;
        uint64_t request{ 0u };
        readable(load).read_stream() >> name >> request;
        EXPECT_EQ(name, areg::String("model-a"));
        EXPECT_EQ(request, 0x1122334455667788ull);
    }

    ServiceManagerEventData unload = ServiceManagerEventData::model_request(false, areg::String(), 5u);
    EXPECT_EQ(unload.command(), Command::CMD_UnloadModel);
    {
        areg::String name("not empty");
        uint64_t request{ 0u };
        readable(unload).read_stream() >> name >> request;
        EXPECT_TRUE(name.is_empty());
        EXPECT_EQ(request, 5u);
    }
}

TEST(ServiceManagerEventsTest, thread_report_selects_running_or_stopped)
{
    ServiceManagerEventData running = ServiceManagerEventData::thread_report(true, areg::String("worker"), 42u);
    EXPECT_EQ(running.command(), Command::CMD_ThreadRunning);
    ServiceManagerEventData stopped = ServiceManagerEventData::thread_report(false, areg::String("worker"), 43u);
    EXPECT_EQ(stopped.command(), Command::CMD_ThreadStopped);

    areg::String name;
    uint64_t instance{ 0u };
    readable(stopped).read_stream() >> name >> instance;
    EXPECT_EQ(name, areg::String("worker"));
    EXPECT_EQ(instance, 43u);
}

TEST(ServiceManagerEventsTest, watchdog_expired)
{
    ServiceManagerEventData data = ServiceManagerEventData::watchdog_expired(areg::String("thread"), 99u, areg::String("guard"));

    EXPECT_EQ(data.command(), Command::CMD_WatchdogExpired);
    areg::String thread;
    areg::String guard;
    uint64_t instance{ 0u };
    readable(data).read_stream() >> thread >> instance >> guard;
    EXPECT_EQ(thread, areg::String("thread"));
    EXPECT_EQ(instance, 99u);
    EXPECT_EQ(guard, areg::String("guard"));
}

TEST(ServiceManagerEventsTest, user_state)
{
    ServiceManagerEventData data = ServiceManagerEventData::user_state(0xCAFEu);

    EXPECT_EQ(data.command(), Command::CMD_UserState);
    uint32_t state{ 0u };
    readable(data).read_stream() >> state;
    EXPECT_EQ(state, 0xCAFEu);
}

TEST(ServiceManagerEventsTest, copy_keeps_command_and_payload)
{
    const ServiceManagerEventData source = ServiceManagerEventData::user_state(77u);

    ServiceManagerEventData copy(source);
    EXPECT_EQ(copy.command(), Command::CMD_UserState);
    uint32_t state{ 0u };
    readable(copy).read_stream() >> state;
    EXPECT_EQ(state, 77u);

    ServiceManagerEventData assigned = ServiceManagerEventData::stop_connection();
    assigned = source;
    EXPECT_EQ(assigned.command(), Command::CMD_UserState);
    uint32_t assignedState{ 0u };
    readable(assigned).read_stream() >> assignedState;
    EXPECT_EQ(assignedState, 77u);
}

TEST(ServiceManagerEventsTest, move_transfers_command_and_payload)
{
    ServiceManagerEventData source = ServiceManagerEventData::user_state(88u);

    ServiceManagerEventData moved(std::move(source));
    EXPECT_EQ(moved.command(), Command::CMD_UserState);
    uint32_t state{ 0u };
    readable(moved).read_stream() >> state;
    EXPECT_EQ(state, 88u);

    ServiceManagerEventData target = ServiceManagerEventData::stop_connection();
    ServiceManagerEventData another = ServiceManagerEventData::user_state(99u);
    target = std::move(another);
    EXPECT_EQ(target.command(), Command::CMD_UserState);
    uint32_t targetState{ 0u };
    readable(target).read_stream() >> targetState;
    EXPECT_EQ(targetState, 99u);
}

TEST(ServiceManagerEventsTest, explicit_construction_has_empty_payload)
{
    ServiceManagerEventData data(Command::CMD_UserState);

    EXPECT_EQ(data.command(), Command::CMD_UserState);
    data.write_stream() << static_cast<uint32_t>(5u);
    uint32_t value{ 0u };
    readable(data).read_stream() >> value;
    EXPECT_EQ(value, 5u);
}
