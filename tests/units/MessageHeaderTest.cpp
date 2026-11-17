/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/MessageHeaderTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The call type of the event header and the header fields every
 *              service event constructor writes.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/component/EventDefs.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/component/StubAddress.hpp"

#include <cstring>
#include <vector>

namespace
{
    constexpr uint32_t REQUEST_ID   { areg::crc32_calculate("request_header") };
    constexpr uint32_t RESPONSE_ID  { areg::crc32_calculate("response_header") };
    constexpr uint32_t BROADCAST_ID { areg::crc32_calculate("broadcast_header") };
    constexpr uint32_t ATTRIBUTE_ID { areg::crc32_calculate("attribute_Header") };
    constexpr SequenceNumber SEQUENCE{ 0x1234u };

    //!< The header fields at their wire offsets.
    struct HeaderFields
    {
        uint32_t    messageId;
        uint32_t    result;
        uint16_t    eventType;
        uint8_t     callType;
    };

    //!< Reads the fields from the bytes of the header, by offset.
    HeaderFields read_fields(const areg::Event & event)
    {
        const uint8_t * bytes{ reinterpret_cast<const uint8_t *>(event.header()) };
        HeaderFields result{ };
        std::memcpy(&result.messageId, bytes + 76u, sizeof(result.messageId));
        std::memcpy(&result.result,    bytes + 88u, sizeof(result.result));
        std::memcpy(&result.eventType, bytes + 92u, sizeof(result.eventType));
        std::memcpy(&result.callType,  bytes + 94u, sizeof(result.callType));
        return result;
    }

    //!< Checks the fields of the event against the expected values.
    void expect_fields(const areg::Event & event, uint32_t msgId, uint32_t result, areg::EventType eventType, areg::CallType callType)
    {
        ASSERT_NE(event.header(), nullptr);
        const HeaderFields fields{ read_fields(event) };
        EXPECT_EQ(fields.messageId, msgId);
        EXPECT_EQ(fields.result, result);
        EXPECT_EQ(fields.eventType, static_cast<uint16_t>(eventType));
        EXPECT_EQ(fields.callType, static_cast<uint8_t>(callType));
    }

    areg::ProxyAddress proxy_address()
    {
        areg::ProxyAddress address(0x613A2001u, areg::Version(1u, 0u, 0u), areg::ServiceType::Public
                                , 0x613A3001u, 0x613A4001u, 0x613A1001u);
        address.set_channel(areg::Channel(0x613A4001u, 0x613A5001u, 0x613A6001u));
        return address;
    }

    areg::StubAddress stub_address()
    {
        areg::StubAddress address(0x613A2001u, areg::Version(1u, 0u, 0u), areg::ServiceType::Public
                               , 0x613A3001u, 0x613A4002u, 0x613A1002u);
        address.set_channel(areg::Channel(0x613A4002u, 0x613A5002u, 0x613A6002u));
        return address;
    }

    areg::SharedBuffer payload()
    {
        const uint8_t bytes[]{ 1u, 2u, 3u, 4u };
        return areg::SharedBuffer(bytes, static_cast<uint32_t>(sizeof(bytes)));
    }
}

//////////////////////////////////////////////////////////////////////////
// 1. The call type
//////////////////////////////////////////////////////////////////////////

TEST(MessageHeaderTest, call_type_values)
{
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::Undefined),          0x00u);
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::RegisterConsumer),   0x10u);
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::UnregisterConsumer), 0x11u);
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::RegisterProvider),   0x20u);
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::UnregisterProvider), 0x21u);
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::Interface),          0x40u);
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::Request),            0x41u);
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::Response),           0x42u);
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::Broadcast),          0x43u);
    EXPECT_EQ(static_cast<uint8_t>(areg::CallType::Attribute),          0x44u);
    EXPECT_EQ(areg::CALL_INTERFACE_MASK, 0x40u);
}

TEST(MessageHeaderTest, interface_call_is_the_interface_bit_of_every_value)
{
    for (uint32_t value = 0u; value <= 0xFFu; ++value)
    {
        const uint8_t callType{ static_cast<uint8_t>(value) };
        EXPECT_EQ(areg::is_interface_call(callType), (value & 0x40u) != 0u) << value;
        EXPECT_EQ(areg::is_interface_call(static_cast<areg::CallType>(callType)), (value & 0x40u) != 0u) << value;
    }
}

TEST(MessageHeaderTest, interface_kind_is_one_of_the_four_kinds)
{
    for (uint32_t value = 0u; value <= 0xFFu; ++value)
    {
        const areg::CallType callType{ static_cast<areg::CallType>(value) };
        const bool expected{ (callType == areg::CallType::Request) || (callType == areg::CallType::Response)
                          || (callType == areg::CallType::Broadcast) || (callType == areg::CallType::Attribute) };
        EXPECT_EQ(areg::is_interface_kind(callType), expected) << value;
    }

    EXPECT_FALSE(areg::is_interface_kind(areg::CallType::Interface));
    EXPECT_TRUE(areg::is_interface_call(areg::CallType::Interface));
    EXPECT_FALSE(areg::is_interface_call(areg::CallType::RegisterProvider));
}

TEST(MessageHeaderTest, call_type_names)
{
    EXPECT_STREQ(areg::as_string(areg::CallType::Undefined),          "areg::CallType::Undefined");
    EXPECT_STREQ(areg::as_string(areg::CallType::RegisterConsumer),   "areg::CallType::RegisterConsumer");
    EXPECT_STREQ(areg::as_string(areg::CallType::UnregisterConsumer), "areg::CallType::UnregisterConsumer");
    EXPECT_STREQ(areg::as_string(areg::CallType::RegisterProvider),   "areg::CallType::RegisterProvider");
    EXPECT_STREQ(areg::as_string(areg::CallType::UnregisterProvider), "areg::CallType::UnregisterProvider");
    EXPECT_STREQ(areg::as_string(areg::CallType::Interface),          "areg::CallType::Interface");
    EXPECT_STREQ(areg::as_string(areg::CallType::Request),            "areg::CallType::Request");
    EXPECT_STREQ(areg::as_string(areg::CallType::Response),           "areg::CallType::Response");
    EXPECT_STREQ(areg::as_string(areg::CallType::Broadcast),          "areg::CallType::Broadcast");
    EXPECT_STREQ(areg::as_string(areg::CallType::Attribute),          "areg::CallType::Attribute");
    EXPECT_EQ(std::strncmp(areg::as_string(static_cast<areg::CallType>(0x7Fu)), "ERR:", 4), 0);
}

//////////////////////////////////////////////////////////////////////////
// 2. The fields every service event constructor writes
//////////////////////////////////////////////////////////////////////////

TEST(MessageHeaderTest, request_events_write_the_request_kind_and_the_call_function)
{
    const uint32_t callFunction{ static_cast<uint32_t>(areg::RequestType::CallFunction) };
    const areg::LocalRequestEvent localEmpty(proxy_address(), stub_address(), REQUEST_ID, 16u);
    const areg::LocalRequestEvent localData(payload(), proxy_address(), stub_address(), REQUEST_ID);
    const areg::RemoteRequestEvent remoteEmpty(proxy_address(), stub_address(), REQUEST_ID, 16u);
    const areg::RemoteRequestEvent remoteData(payload(), proxy_address(), stub_address(), REQUEST_ID);

    expect_fields(localEmpty,  REQUEST_ID, callFunction, areg::EventType::EventLocalRequest,  areg::CallType::Request);
    expect_fields(localData,   REQUEST_ID, callFunction, areg::EventType::EventLocalRequest,  areg::CallType::Request);
    expect_fields(remoteEmpty, REQUEST_ID, callFunction, areg::EventType::EventRemoteRequest, areg::CallType::Request);
    expect_fields(remoteData,  REQUEST_ID, callFunction, areg::EventType::EventRemoteRequest, areg::CallType::Request);
    EXPECT_EQ(remoteData.call_kind(), areg::CallType::Request);
}

TEST(MessageHeaderTest, notify_request_events_write_the_kind_of_the_subscribed_entry)
{
    struct Row { uint32_t msgId; areg::RequestType type; areg::CallType kind; };
    const Row rows[]
    {
          { ATTRIBUTE_ID, areg::RequestType::StartNotify, areg::CallType::Attribute }
        , { BROADCAST_ID, areg::RequestType::StartNotify, areg::CallType::Broadcast }
        , { RESPONSE_ID,  areg::RequestType::StopNotify,  areg::CallType::Response  }
        , { static_cast<uint32_t>(areg::FuncIdRange::EmptyFunctionId), areg::RequestType::RemoveAllNotify, areg::CallType::Interface }
    };

    for (const Row & row : rows)
    {
        SCOPED_TRACE(areg::as_string(row.kind));
        const areg::LocalNotifyRequestEvent local(proxy_address(), stub_address(), row.msgId, row.type, row.kind);
        const areg::RemoteNotifyRequestEvent remote(proxy_address(), stub_address(), row.msgId, row.type, row.kind);
        expect_fields(local,  row.msgId, static_cast<uint32_t>(row.type), areg::EventType::EventLocalNotifyRequest,  row.kind);
        expect_fields(remote, row.msgId, static_cast<uint32_t>(row.type), areg::EventType::EventRemoteNotifyRequest, row.kind);
        EXPECT_EQ(remote.call_kind(), row.kind);
        EXPECT_EQ(remote.request_type(), row.type);
    }
}

TEST(MessageHeaderTest, response_events_write_the_kind_and_the_result)
{
    struct Row { uint32_t msgId; areg::ResultType result; areg::CallType kind; };
    const Row rows[]
    {
          { RESPONSE_ID,  areg::ResultType::RequestOK,          areg::CallType::Response  }
        , { BROADCAST_ID, areg::ResultType::RequestOK,          areg::CallType::Broadcast }
        , { ATTRIBUTE_ID, areg::ResultType::DataOK,             areg::CallType::Attribute }
        , { ATTRIBUTE_ID, areg::ResultType::ServiceUnavailable, areg::CallType::Attribute }
        , { REQUEST_ID,   areg::ResultType::RequestBusy,        areg::CallType::Request   }
        , { REQUEST_ID,   areg::ResultType::RequestUnsupported, areg::CallType::Request   }
    };

    const areg::ProxyAddress proxy{ proxy_address() };
    areg::Endpoint consumer{ };
    areg::RawService service{ };
    proxy.to_endpoint(service, consumer);
    for (const Row & row : rows)
    {
        SCOPED_TRACE(areg::as_string(row.result));
        const uint32_t result{ static_cast<uint32_t>(row.result) };
        const areg::LocalResponseEvent  local1(proxy, row.result, row.msgId, row.kind, SEQUENCE, 8u);
        const areg::LocalResponseEvent  local2(payload(), proxy, row.result, row.msgId, row.kind, SEQUENCE);
        const areg::LocalResponseEvent  local3(consumer, service, row.result, row.msgId, row.kind, SEQUENCE, 8u);
        const areg::LocalResponseEvent  local4(payload(), consumer, service, row.result, row.msgId, row.kind, SEQUENCE);
        const areg::RemoteResponseEvent remote1(proxy, row.result, row.msgId, row.kind, SEQUENCE, 8u);
        const areg::RemoteResponseEvent remote2(payload(), proxy, row.result, row.msgId, row.kind, SEQUENCE);
        const areg::RemoteResponseEvent remote3(consumer, service, row.result, row.msgId, row.kind, SEQUENCE, 8u);
        const areg::RemoteResponseEvent remote4(payload(), consumer, service, row.result, row.msgId, row.kind, SEQUENCE);

        for (const areg::ServiceResponseEvent * event : { &local1, &local2, &local3, &local4 })
        {
            expect_fields(*event, row.msgId, result, areg::EventType::EventLocalResponse, row.kind);
            EXPECT_EQ(event->call_kind(), row.kind);
        }

        for (const areg::ServiceResponseEvent * event : { &remote1, &remote2, &remote3, &remote4 })
        {
            expect_fields(*event, row.msgId, result, areg::EventType::EventRemoteResponse, row.kind);
            EXPECT_EQ(event->call_kind(), row.kind);
        }
    }
}

TEST(MessageHeaderTest, cloned_response_keeps_the_call_type)
{
    const areg::ProxyAddress proxy{ proxy_address() };
    areg::ProxyAddress other{ proxy_address() };
    other.set_channel(areg::Channel(0x613A4009u, 0x613A5009u, 0x613A6009u));
    areg::Endpoint consumer{ };
    areg::RawService service{ };
    other.to_endpoint(service, consumer);

    for (areg::CallType kind : { areg::CallType::Response, areg::CallType::Broadcast, areg::CallType::Attribute, areg::CallType::Request })
    {
        SCOPED_TRACE(areg::as_string(kind));
        const areg::LocalResponseEvent source(payload(), proxy, areg::ResultType::RequestOK, RESPONSE_ID, kind, SEQUENCE);
        const areg::ServiceResponseEvent byProxy{ source.clone_for_target(other) };
        const areg::ServiceResponseEvent byEndpoint{ source.clone_for_target(consumer, service) };
        expect_fields(byProxy,    RESPONSE_ID, static_cast<uint32_t>(areg::ResultType::RequestOK), areg::EventType::EventLocalResponse, kind);
        expect_fields(byEndpoint, RESPONSE_ID, static_cast<uint32_t>(areg::ResultType::RequestOK), areg::EventType::EventLocalResponse, kind);
    }
}
