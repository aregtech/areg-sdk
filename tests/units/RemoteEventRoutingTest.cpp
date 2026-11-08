#include "units/GUnitTest.hpp"

#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/ProxyEvent.hpp"
#include "areg/component/RemoteEventFactory.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/StubAddress.hpp"

#include <cstring>
#include <utility>
#include <vector>

namespace
{
    constexpr uint32_t REQUEST_ID{ areg::REQUEST_ID_FIRST + 7u };
    constexpr uint32_t RESPONSE_ID{ areg::RESPONSE_ID_FIRST + 9u };
    constexpr uint32_t ATTRIBUTE_ID{ areg::ATTRIBUTE_ID_FIRST + 3u };
    constexpr SequenceNumber SEQUENCE{ 0x12345678u };

    areg::ProxyAddress proxy_address(uint32_t identity = 0x613A1001u)
    {
        areg::ProxyAddress address(0x613A2001u, areg::Version(1u, 2u, 3u), areg::ServiceType::Public
                                , 0x613A3001u, 0x613A4001u, identity);
        address.set_channel(areg::Channel(0x613A4001u, 0x613A5001u, 0x613A6001u));
        return address;
    }

    areg::StubAddress stub_address()
    {
        areg::StubAddress address(0x613A2001u, areg::Version(1u, 2u, 3u), areg::ServiceType::Public
                               , 0x613A3001u, 0x613A4002u, 0x613A1002u);
        address.set_channel(areg::Channel(0x613A4002u, 0x613A5002u, 0x613A6002u));
        return address;
    }

    std::vector<uint8_t> payload_bytes()
    {
        std::vector<uint8_t> bytes(4096u);
        for (uint32_t i = 0u; i < bytes.size(); ++i)
        {
            bytes[i] = static_cast<uint8_t>((i * 37u) & 0xFFu);
        }
        return bytes;
    }

    areg::SharedBuffer payload()
    {
        const auto bytes{ payload_bytes() };
        return areg::SharedBuffer(bytes.data(), static_cast<uint32_t>(bytes.size()));
    }

    void expect_payload(const areg::MessageEnvelope & envelope)
    {
        const auto bytes{ payload_bytes() };
        ASSERT_EQ(envelope.size_used(), bytes.size());
        ASSERT_NE(envelope.buffer(), nullptr);
        EXPECT_EQ(std::memcmp(envelope.buffer(), bytes.data(), bytes.size()), 0);
    }

    class ProxyReceiver final : public areg::ProxyEventConsumer
    {
    public:
        explicit ProxyReceiver(const areg::ProxyAddress & address)
            : areg::ProxyEventConsumer(address)
        {
        }

        std::vector<areg::EventType> mTypes;
        std::vector<uint32_t> mIds;
        std::vector<SequenceNumber> mSequences;
        std::vector<uint32_t> mResults;
        uint32_t mCustom{ 0u };
        uint32_t mGeneric{ 0u };
        uint32_t mProxy{ 0u };
        uint32_t mConnections{ 0u };
        areg::StubAddress mProvider;
        areg::Channel mChannel;
        areg::ServiceConnectionState mStatus{ areg::ServiceConnectionState::Unknown };

    private:
        void process_response_event(areg::ServiceResponseEvent & event) override
        {
            mTypes.push_back(event.event_type());
            mIds.push_back(event.response_id());
            mSequences.push_back(event.sequence_number());
            mResults.push_back(event.envelope().result());
            expect_payload(event.envelope());
        }

        void process_proxy_event(areg::ProxyEvent &) override
        {
            ++mProxy;
        }

        void process_custom_event(areg::Event &) override
        {
            ++mCustom;
        }

        void process_generic_event(areg::Event &) override
        {
            ++mGeneric;
        }

        void service_connection_updated(const areg::StubAddress & provider, const areg::Channel & channel
                                      , areg::ServiceConnectionState status) override
        {
            ++mConnections;
            mProvider = provider;
            mChannel = channel;
            mStatus = status;
        }
    };

    void verify_response_family(areg::EventType local, areg::EventType remote, uint32_t messageId)
    {
        const areg::ProxyAddress address{ proxy_address() };
        const areg::ProxyAddress other{ proxy_address(0x613A1003u) };
        ProxyReceiver receiver(address);
        for (areg::EventType type : { local, remote })
        {
            areg::ServiceResponseEvent event(address, areg::ResultType::DataOK, messageId, type, SEQUENCE);
            const auto bytes{ payload_bytes() };
            ASSERT_EQ(event.write_stream().write(bytes.data(), static_cast<uint32_t>(bytes.size())), bytes.size());
            event.dispatch_self(&receiver);
            areg::ServiceResponseEvent wrong{ event.clone_for_target(other) };
            wrong.dispatch_self(&receiver);
        }
        EXPECT_EQ(receiver.mTypes, (std::vector<areg::EventType>{ local, remote }));
        EXPECT_EQ(receiver.mIds, std::vector<uint32_t>(2u, messageId));
        EXPECT_EQ(receiver.mSequences, std::vector<SequenceNumber>(2u, SEQUENCE));
        EXPECT_EQ(receiver.mResults, std::vector<uint32_t>(2u, static_cast<uint32_t>(areg::ResultType::DataOK)));
        EXPECT_EQ(receiver.mCustom + receiver.mGeneric + receiver.mProxy + receiver.mConnections, 0u);
    }
}

TEST(RemoteEventRoutingTest, outgoing_request_stamps_process_cookies_without_changing_request_or_payload)
{
    const areg::ProxyAddress proxy{ proxy_address() };
    const areg::StubAddress stub{ stub_address() };
    areg::RemoteRequestEvent event(payload(), proxy, stub, REQUEST_ID);
    event.set_sequence_number(SEQUENCE);
    const areg::EventHeader before{ *event.header() };
    ASSERT_TRUE(areg::RemoteEventFactory::route_outgoing_message(event, areg::Channel(71u, 72u, 73u)));
    EXPECT_EQ(event.header()->source, 73u);
    EXPECT_EQ(event.header()->target, stub.cookie());
    EXPECT_EQ(event.request_id(), REQUEST_ID);
    EXPECT_EQ(event.request_type(), areg::RequestType::CallFunction);
    EXPECT_EQ(event.sequence_number(), SEQUENCE);
    EXPECT_EQ(event.header()->provider.number, before.provider.number);
    EXPECT_EQ(event.header()->consumer.number, before.consumer.number);
    EXPECT_EQ(event.header()->provider.thread, before.provider.thread);
    EXPECT_EQ(event.header()->consumer.thread, before.consumer.thread);
    EXPECT_EQ(event.header()->rawService.service, before.rawService.service);
    EXPECT_EQ(event.header()->rawService.role, before.rawService.role);
    expect_payload(event.envelope());

    areg::RemoteRequestEvent restored(event.envelope());
    EXPECT_EQ(restored.event_source(), proxy);
    EXPECT_EQ(restored.target_stub(), static_cast<uint32_t>(stub));
    EXPECT_EQ(restored.request_id(), REQUEST_ID);
    EXPECT_EQ(restored.sequence_number(), SEQUENCE);
    EXPECT_EQ(restored.event_type(), areg::EventType::EventRemoteRequest);
    expect_payload(restored.envelope());
}

TEST(RemoteEventRoutingTest, outgoing_notification_preserves_start_stop_and_remove_all_requests)
{
    for (areg::RequestType type : { areg::RequestType::StartNotify, areg::RequestType::StopNotify
                                , areg::RequestType::RemoveAllNotify })
    {
        SCOPED_TRACE(static_cast<uint32_t>(type));
        areg::RemoteNotifyRequestEvent event(proxy_address(), stub_address(), ATTRIBUTE_ID, type);
        event.set_sequence_number(SEQUENCE);
        ASSERT_TRUE(areg::RemoteEventFactory::route_outgoing_message(event, areg::Channel(71u, 72u, 73u)));
        EXPECT_EQ(event.header()->source, 73u);
        EXPECT_EQ(event.header()->target, stub_address().cookie());
        areg::RemoteNotifyRequestEvent restored(event.envelope());
        EXPECT_EQ(restored.request_type(), type);
        EXPECT_EQ(restored.request_id(), ATTRIBUTE_ID);
        EXPECT_EQ(restored.sequence_number(), SEQUENCE);
        EXPECT_EQ(restored.event_type(), areg::EventType::EventRemoteNotifyRequest);
        EXPECT_EQ(restored.envelope().size_used(), 0u);
    }
}

TEST(RemoteEventRoutingTest, outgoing_response_keeps_result_sequence_and_serialized_data)
{
    for (areg::ResultType result : { areg::ResultType::RequestOK, areg::ResultType::DataInvalid
                                 , areg::ResultType::MessageUndelivered, areg::ResultType::ServiceUnavailable })
    {
        SCOPED_TRACE(static_cast<uint32_t>(result));
        areg::RemoteResponseEvent event(payload(), proxy_address(), result, RESPONSE_ID, SEQUENCE);
        ASSERT_TRUE(areg::RemoteEventFactory::route_outgoing_message(event, areg::Channel(81u, 82u, 83u)));
        EXPECT_EQ(event.header()->source, 83u);
        EXPECT_EQ(event.header()->target, proxy_address().cookie());
        EXPECT_EQ(event.envelope().result(), static_cast<uint32_t>(result));
        areg::RemoteResponseEvent restored(event.envelope());
        EXPECT_EQ(restored.response_id(), RESPONSE_ID);
        EXPECT_EQ(restored.sequence_number(), SEQUENCE);
        EXPECT_EQ(restored.envelope().result(), static_cast<uint32_t>(result));
        EXPECT_EQ(restored.target_proxy(), static_cast<uint32_t>(proxy_address()));
        EXPECT_EQ(restored.target_cookie(), proxy_address().cookie());
        expect_payload(restored.envelope());
    }
}

TEST(RemoteEventRoutingTest, outgoing_empty_and_connection_events_are_not_serialized)
{
    areg::Event empty(areg::MessageEnvelope{});
    EXPECT_FALSE(areg::RemoteEventFactory::route_outgoing_message(empty, areg::Channel(1u, 2u, 3u)));
    EXPECT_FALSE(empty.is_valid());
    for (areg::EventType type : { areg::EventType::EventRemoteProviderConnect, areg::EventType::EventRemoteConsumerConnect })
    {
        areg::Event event(type);
        const areg::EventHeader before{ *event.header() };
        EXPECT_FALSE(areg::RemoteEventFactory::route_outgoing_message(event, areg::Channel(1u, 2u, 3u)));
        EXPECT_EQ(std::memcmp(event.header(), &before, sizeof(before)), 0);
    }
}

TEST(RemoteEventRoutingTest, incoming_empty_and_unknown_event_types_report_failure_without_mutation)
{
    areg::MessageEnvelope empty;
    EXPECT_FALSE(areg::RemoteEventFactory::route_incoming_message(empty, areg::Channel()));
    EXPECT_FALSE(empty.is_valid());
    for (areg::EventType type : { areg::EventType::EventUnknown, areg::EventType::EventCustomExternal
                               , static_cast<areg::EventType>(0xFFFFu) })
    {
        areg::MessageEnvelope wire(true);
        wire.header()->eventType = static_cast<uint16_t>(type);
        wire.set_message_id(0xDEADBEEFu);
        wire.set_sequence(SEQUENCE);
        const areg::EventHeader before{ *wire.header() };
        EXPECT_FALSE(areg::RemoteEventFactory::route_incoming_message(wire, areg::Channel(1u, 2u, 3u)));
        ASSERT_NE(wire.header(), nullptr);
        EXPECT_EQ(std::memcmp(wire.header(), &before, sizeof(before)), 0);
    }
}

TEST(RemoteEventRoutingTest, connection_messages_are_acknowledged_outside_the_executable_path)
{
    for (areg::EventType type : { areg::EventType::EventRemoteProviderConnect, areg::EventType::EventRemoteConsumerConnect })
    {
        areg::MessageEnvelope wire(true);
        wire.header()->eventType = static_cast<uint16_t>(type);
        wire.set_message_id(0xDEADBEEFu);
        const areg::EventHeader before{ *wire.header() };
        EXPECT_TRUE(areg::RemoteEventFactory::route_incoming_message(wire, areg::Channel(1u, 2u, 3u)));
        ASSERT_NE(wire.header(), nullptr);
        EXPECT_EQ(std::memcmp(wire.header(), &before, sizeof(before)), 0);
    }
}

TEST(RemoteEventRoutingTest, requests_and_responses_for_absent_endpoints_report_undeliverable)
{
    for (areg::EventType type : { areg::EventType::EventRemoteRequest, areg::EventType::EventRemoteNotifyRequest
                               , areg::EventType::EventRemoteResponse })
    {
        areg::MessageEnvelope wire(true);
        wire.header()->eventType = static_cast<uint16_t>(type);
        wire.header()->provider.number = 0x613AFFFFu;
        wire.header()->consumer.number = 0x613AFFFEu;
        wire.set_message_id(REQUEST_ID);
        EXPECT_FALSE(areg::RemoteEventFactory::route_incoming_message(wire, areg::Channel(1u, 2u, 3u)));
    }
}

TEST(RemoteEventRoutingTest, failure_factory_returns_invalid_without_a_local_request_proxy)
{
    areg::MessageEnvelope empty;
    EXPECT_FALSE(areg::RemoteEventFactory::create_request_failed_event(empty, areg::Channel()).is_valid());
    for (areg::EventType type : { areg::EventType::EventRemoteRequest, areg::EventType::EventRemoteNotifyRequest
                               , areg::EventType::EventRemoteResponse, areg::EventType::EventRemoteProviderConnect
                               , areg::EventType::EventRemoteConsumerConnect })
    {
        areg::MessageEnvelope wire(true);
        wire.header()->eventType = static_cast<uint16_t>(type);
        wire.header()->consumer.number = 0x613AFFFEu;
        wire.set_message_id(REQUEST_ID);
        wire.set_sequence(SEQUENCE);
        const areg::EventHeader before{ *wire.header() };
        EXPECT_FALSE(areg::RemoteEventFactory::create_request_failed_event(wire, areg::Channel()).is_valid());
        EXPECT_EQ(std::memcmp(wire.header(), &before, sizeof(before)), 0);
    }
}

TEST(ServiceEventValueTest, local_request_and_subscription_round_trip_their_envelope)
{
    const areg::ProxyAddress proxy{ proxy_address() };
    const areg::StubAddress stub{ stub_address() };
    areg::LocalRequestEvent event(payload(), proxy, stub, REQUEST_ID);
    event.set_sequence_number(SEQUENCE);
    areg::LocalRequestEvent restored(event.envelope());
    EXPECT_EQ(restored.request_type(), areg::RequestType::CallFunction);
    EXPECT_EQ(restored.request_id(), REQUEST_ID);
    EXPECT_EQ(restored.sequence_number(), SEQUENCE);
    EXPECT_EQ(restored.event_source(), proxy);
    EXPECT_EQ(restored.target_stub(), static_cast<uint32_t>(stub));
    EXPECT_EQ(restored.event_type(), areg::EventType::EventLocalRequest);
    expect_payload(restored.envelope());

    areg::LocalNotifyRequestEvent notify(proxy, stub, ATTRIBUTE_ID, areg::RequestType::StopNotify);
    areg::LocalNotifyRequestEvent notifyRestored(notify.envelope());
    EXPECT_EQ(notifyRestored.request_type(), areg::RequestType::StopNotify);
    EXPECT_EQ(notifyRestored.request_id(), ATTRIBUTE_ID);
    EXPECT_EQ(notifyRestored.event_type(), areg::EventType::EventLocalNotifyRequest);
    EXPECT_EQ(notifyRestored.event_source(), proxy);
}

TEST(ServiceEventValueTest, response_clone_has_independent_header_and_payload_for_each_target)
{
    const areg::ProxyAddress first{ proxy_address() };
    const areg::ProxyAddress second{ proxy_address(0x613A1003u) };
    areg::LocalResponseEvent source(payload(), first, areg::ResultType::DataOK, RESPONSE_ID, SEQUENCE);
    areg::ServiceResponseEvent clone{ source.clone_for_target(second) };
    ASSERT_NE(clone.header(), source.header());
    EXPECT_EQ(source.target_proxy(), static_cast<uint32_t>(first));
    EXPECT_EQ(clone.target_proxy(), static_cast<uint32_t>(second));
    EXPECT_EQ(clone.response_id(), RESPONSE_ID);
    EXPECT_EQ(clone.sequence_number(), SEQUENCE);
    expect_payload(source.envelope());
    expect_payload(clone.envelope());
    clone.set_sequence_number(SEQUENCE + 1u);
    clone.header()->result = static_cast<uint32_t>(areg::ResultType::DataInvalid);
    EXPECT_EQ(source.sequence_number(), SEQUENCE);
    EXPECT_EQ(source.envelope().result(), static_cast<uint32_t>(areg::ResultType::DataOK));
    EXPECT_EQ(clone.sequence_number(), SEQUENCE + 1u);
}

TEST(ProxyEventRoutingTest, local_and_remote_responses_filter_the_target_identity)
{
    verify_response_family(areg::EventType::EventLocalResponse, areg::EventType::EventRemoteResponse, RESPONSE_ID);
}

TEST(ProxyEventRoutingTest, local_and_remote_broadcasts_filter_the_target_identity)
{
    verify_response_family(areg::EventType::EventLocalBroadcast, areg::EventType::EventRemoteBroadcast, RESPONSE_ID);
}

TEST(ProxyEventRoutingTest, local_and_remote_attributes_filter_the_target_identity)
{
    verify_response_family(areg::EventType::EventLocalAttribute, areg::EventType::EventRemoteAttribute, ATTRIBUTE_ID);
}

TEST(ProxyEventRoutingTest, local_and_remote_failures_filter_the_target_identity)
{
    verify_response_family(areg::EventType::EventLocalRequestFailed, areg::EventType::EventRemoteRequestFailed, REQUEST_ID);
}

TEST(ProxyEventRoutingTest, connection_events_reconstruct_local_and_remote_return_channels)
{
    const areg::ProxyAddress proxy{ proxy_address() };
    const areg::StubAddress stub{ stub_address() };
    ProxyReceiver receiver(proxy);
    for (areg::EventType type : { areg::EventType::EventLocalConsumerConnect, areg::EventType::EventRemoteConsumerConnect })
    {
        areg::MessageEnvelope wire(true);
        proxy.to_event(*wire.header());
        stub.to_event(*wire.header());
        wire.header()->eventType = static_cast<uint16_t>(type);
        wire.header()->channel = 99u;
        wire.header()->result = static_cast<uint32_t>(areg::ServiceConnectionState::Connected);
        areg::Event event(wire);
        event.dispatch_self(&receiver);
        EXPECT_EQ(receiver.mProvider, stub);
        EXPECT_EQ(receiver.mChannel.source(), proxy.channel().source());
        EXPECT_EQ(receiver.mChannel.target(), areg::is_local(type) ? stub.channel().source() : 99u);
        EXPECT_EQ(receiver.mChannel.cookie(), proxy.cookie());
        EXPECT_EQ(receiver.mStatus, areg::ServiceConnectionState::Connected);
    }
    EXPECT_EQ(receiver.mConnections, 2u);
    EXPECT_TRUE(receiver.mTypes.empty());
}

TEST(ProxyEventRoutingTest, custom_generic_and_unknown_consumer_events_have_distinct_handlers)
{
    const areg::ProxyAddress proxy{ proxy_address() };
    ProxyReceiver receiver(proxy);
    for (areg::EventType type : { areg::EventType::EventCustomInternal, areg::EventType::EventCustomExternal
                               , areg::EventType::EventLocalRequest
                               , static_cast<areg::EventType>(static_cast<uint32_t>(areg::EventType::EventToConsumerMask)
                                                           | static_cast<uint32_t>(areg::EventType::EventExternalMask)) })
    {
        areg::Event event(type);
        event.dispatch_self(&receiver);
    }
    EXPECT_EQ(receiver.mCustom, 2u);
    EXPECT_EQ(receiver.mGeneric, 1u);
    EXPECT_EQ(receiver.mProxy, 1u);
    EXPECT_EQ(receiver.mConnections, 0u);
    EXPECT_TRUE(receiver.mTypes.empty());
}

TEST(EventApiTest, type_queries_agree_with_the_event_type_and_an_empty_event_answers_no)
{
    constexpr areg::EventType TYPES[]
    {
          areg::EventType::EventNotifyClient, areg::EventType::EventLocalRequest, areg::EventType::EventRemoteRequest
        , areg::EventType::EventLocalNotifyRequest, areg::EventType::EventRemoteNotifyRequest
        , areg::EventType::EventLocalNotifyResponse, areg::EventType::EventRemoteNotifyResponse
        , areg::EventType::EventLocalNotifyBroadcast, areg::EventType::EventRemoteNotifyBroadcast
        , areg::EventType::EventLocalNotifyAttribute, areg::EventType::EventRemoteNotifyAttribute
        , areg::EventType::EventLocalResponse, areg::EventType::EventRemoteResponse
        , areg::EventType::EventLocalBroadcast, areg::EventType::EventRemoteBroadcast
        , areg::EventType::EventLocalAttribute, areg::EventType::EventRemoteAttribute
        , areg::EventType::EventLocalProviderConnect, areg::EventType::EventRemoteConsumerConnect
        , areg::EventType::EventLocalConnection, areg::EventType::EventRemoteConnection
        , areg::EventType::EventCustomInternal, areg::EventType::EventCustomExternal
        , areg::EventType::EventLocalRequestFailed, areg::EventType::EventRemoteRequestFailed
    };

    areg::Event event(areg::EventType::EventCustomInternal);
    for (const areg::EventType type : TYPES)
    {
        event.set_event_type(type);
        EXPECT_EQ(event.event_type(), type);
        EXPECT_EQ(event.is_internal(), areg::is_internal(type));
        EXPECT_EQ(event.is_external(), areg::is_external(type));
        EXPECT_EQ(event.is_local(), areg::is_local(type));
        EXPECT_EQ(event.is_remote(), areg::is_remote(type));
        EXPECT_EQ(event.is_custom(), areg::is_custom(type));
        EXPECT_EQ(event.is_request_call(), areg::is_request_call(type));
        EXPECT_EQ(event.is_response_call(), areg::is_response_call(type));
        EXPECT_EQ(event.is_broadcast_call(), areg::is_broadcast_call(type));
        EXPECT_EQ(event.is_attribute_call(), areg::is_attribute_call(type));
        EXPECT_EQ(event.is_request_failed_call(), areg::is_request_failure(type));
        EXPECT_EQ(event.is_notify_call(), areg::is_notify(type));
        EXPECT_EQ(event.is_notify_response(), areg::is_notify_response(type));
        EXPECT_EQ(event.is_notify_broadcast(), areg::is_notify_broadcast(type));
        EXPECT_EQ(event.is_notify_attribute(), areg::is_notify_attribute(type));
        EXPECT_EQ(event.is_connect_call(), areg::is_connect(type));
    }

    event.set_event_type(areg::EventType::EventLocalResponse);
    EXPECT_TRUE(event.is_response_call());
    EXPECT_FALSE(event.is_request_call());

    const areg::Event empty;
    EXPECT_FALSE(empty.is_request_call());
    EXPECT_FALSE(empty.is_response_call());
    EXPECT_FALSE(empty.is_broadcast_call());
    EXPECT_FALSE(empty.is_attribute_call());
    EXPECT_FALSE(empty.is_request_failed_call());
    EXPECT_FALSE(empty.is_notify_call());
    EXPECT_FALSE(empty.is_notify_response());
    EXPECT_FALSE(empty.is_notify_broadcast());
    EXPECT_FALSE(empty.is_notify_attribute());
    EXPECT_FALSE(empty.is_connect_call());
    EXPECT_FALSE(empty.is_critical_prio());
    EXPECT_FALSE(empty.is_high_prio());
    EXPECT_FALSE(empty.is_normal_prio());
    EXPECT_FALSE(empty.is_event_id(1u));
    EXPECT_EQ(empty.message_id(), areg::INVALID_MESSAGE_ID);
    EXPECT_EQ(empty.event_consumer(), nullptr);
    EXPECT_EQ(empty.target_dispatcher(), nullptr);
}

TEST(EventApiTest, priority_identity_and_equality_follow_the_shared_envelope)
{
    areg::Event critical(areg::EventType::EventCustomInternal, areg::EventPriority::CriticalPrio);
    EXPECT_TRUE(critical.is_critical_prio());
    EXPECT_FALSE(critical.is_high_prio());
    EXPECT_FALSE(critical.is_normal_prio());
    critical.set_event_priority(areg::EventPriority::HighPrio);
    EXPECT_TRUE(critical.is_high_prio());
    critical.set_event_priority(areg::EventPriority::NormalPrio);
    EXPECT_TRUE(critical.is_normal_prio());

    const areg::Event copy{ critical };
    const areg::Event other(areg::EventType::EventCustomInternal, areg::EventPriority::NormalPrio);
    EXPECT_TRUE(copy == critical);
    EXPECT_FALSE(copy != critical);
    EXPECT_FALSE(other == critical);
    EXPECT_TRUE(other != critical);

    areg::Event moved{ areg::Event(critical) };
    EXPECT_TRUE(moved == critical);
    constexpr areg::RuntimeClassID classId{ "EventApiTest" };
    areg::MessageEnvelope envelope{ critical.envelope() };
    envelope.set_event_id(classId.magic());
    const areg::Event identified(envelope);
    EXPECT_TRUE(identified.is_event_id(classId));
    EXPECT_FALSE(other.is_event_id(classId));
}

TEST(ServiceEventValueTest, request_copies_and_moves_keep_request_and_payload)
{
    const areg::ProxyAddress proxy{ proxy_address() };
    const areg::StubAddress stub{ stub_address() };

    areg::RemoteRequestEvent request(payload(), proxy, stub, REQUEST_ID);
    areg::RemoteRequestEvent copied{ request };
    areg::RemoteRequestEvent moved{ std::move(copied) };
    EXPECT_EQ(moved.request_id(), REQUEST_ID);
    EXPECT_EQ(moved.event_type(), areg::EventType::EventRemoteRequest);
    expect_payload(moved.envelope());

    areg::RemoteNotifyRequestEvent notify(proxy, stub, ATTRIBUTE_ID, areg::RequestType::StartNotify);
    areg::RemoteNotifyRequestEvent notifyCopied{ notify };
    areg::RemoteNotifyRequestEvent notifyMoved{ std::move(notifyCopied) };
    EXPECT_EQ(notifyMoved.request_type(), areg::RequestType::StartNotify);
    EXPECT_EQ(notifyMoved.event_type(), areg::EventType::EventRemoteNotifyRequest);

    areg::LocalRequestEvent local(proxy, stub, REQUEST_ID, 16u);
    areg::LocalRequestEvent localCopied{ local };
    areg::LocalRequestEvent localMoved{ std::move(localCopied) };
    EXPECT_EQ(localMoved.request_id(), REQUEST_ID);
    areg::LocalNotifyRequestEvent localNotify(proxy, stub, ATTRIBUTE_ID, areg::RequestType::StopNotify);
    areg::LocalNotifyRequestEvent localNotifyCopied{ localNotify };
    areg::LocalNotifyRequestEvent localNotifyMoved{ std::move(localNotifyCopied) };
    EXPECT_EQ(localNotifyMoved.request_type(), areg::RequestType::StopNotify);
}

TEST(ServiceEventValueTest, responses_built_from_an_endpoint_or_a_copy_keep_result_and_payload)
{
    const areg::ProxyAddress first{ proxy_address() };
    const areg::ProxyAddress second{ proxy_address(0x613A1003u) };
    areg::Endpoint consumer{ };
    consumer.id     = 0x613A4001u;
    consumer.number = 0x613A1004u;
    consumer.thread = 0x613A3001u;
    const areg::RawService service{ 0x613A2001u, 0x613A2002u };

    areg::LocalResponseEvent localEmpty(consumer, service, areg::ResultType::DataOK, RESPONSE_ID, SEQUENCE, 32u);
    EXPECT_EQ(localEmpty.response_id(), RESPONSE_ID);
    EXPECT_EQ(localEmpty.event_type(), areg::EventType::EventLocalResponse);
    areg::LocalResponseEvent local(payload(), consumer, service, areg::ResultType::DataOK, RESPONSE_ID, SEQUENCE);
    expect_payload(local.envelope());
    areg::LocalResponseEvent localForOther(second, local);
    EXPECT_EQ(localForOther.target_proxy(), static_cast<uint32_t>(second));
    expect_payload(localForOther.envelope());
    areg::LocalResponseEvent localForEndpoint(consumer, service, local);
    EXPECT_EQ(localForEndpoint.sequence_number(), SEQUENCE);
    expect_payload(localForEndpoint.envelope());
    areg::LocalResponseEvent localRestored(local.envelope());
    EXPECT_EQ(localRestored.response_id(), RESPONSE_ID);
    areg::LocalResponseEvent localCopied{ local };
    areg::LocalResponseEvent localMoved{ std::move(localCopied) };
    EXPECT_EQ(localMoved.response_id(), RESPONSE_ID);
    areg::LocalResponseEvent localNoData(areg::SharedBuffer(), first, areg::ResultType::DataOK, RESPONSE_ID, SEQUENCE);
    EXPECT_EQ(localNoData.envelope().size_used(), 0u);

    areg::RemoteResponseEvent remoteEmpty(consumer, service, areg::ResultType::DataInvalid, RESPONSE_ID, SEQUENCE, 0u);
    EXPECT_EQ(remoteEmpty.envelope().result(), static_cast<uint32_t>(areg::ResultType::DataInvalid));
    EXPECT_EQ(remoteEmpty.event_type(), areg::EventType::EventRemoteResponse);
    areg::RemoteResponseEvent remote(payload(), consumer, service, areg::ResultType::DataOK, RESPONSE_ID, SEQUENCE);
    expect_payload(remote.envelope());
    areg::RemoteResponseEvent remoteData(payload(), first, areg::ResultType::DataOK, RESPONSE_ID, SEQUENCE);
    expect_payload(remoteData.envelope());
    areg::RemoteResponseEvent remoteForOther(second, remoteData);
    EXPECT_EQ(remoteForOther.target_proxy(), static_cast<uint32_t>(second));
    areg::RemoteResponseEvent remoteForEndpoint(consumer, service, remoteData);
    expect_payload(remoteForEndpoint.envelope());
    areg::RemoteResponseEvent remoteNoData(areg::SharedBuffer(), consumer, service, areg::ResultType::DataOK, RESPONSE_ID, SEQUENCE);
    EXPECT_EQ(remoteNoData.envelope().size_used(), 0u);

    areg::RemoteResponseEvent remoteCopied{ remote };
    areg::RemoteResponseEvent remoteMoved{ std::move(remoteCopied) };
    EXPECT_EQ(remoteMoved.response_id(), RESPONSE_ID);
    expect_payload(remoteMoved.envelope());
}
