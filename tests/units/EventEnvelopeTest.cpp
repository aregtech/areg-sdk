/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/EventEnvelopeTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for MessageEnvelope.
 *              Covers: initialization, header bulk access, consumer/provider
 *              endpoint fields, shared service fields, event routing fields,
 *              operations, streaming, copy/move semantics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/MessageEnvelope.hpp"

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/MemoryDefs.hpp"

#include <cstring>
#include <array>
#include <utility>

//////////////////////////////////////////////////////////////////////////
// 1. Initialization
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Default-constructed MessageEnvelope is not initialized.
 **/
TEST(EventEnvelopeTest, default_construction)
{
    areg::MessageEnvelope env;
    EXPECT_EQ(env.header(), nullptr);
}

/**
 * \brief   MessageEnvelope constructed with init=true has an allocated header.
 **/
TEST(EventEnvelopeTest, init_construction)
{
    areg::MessageEnvelope env(true);
    ASSERT_NE(env.header(), nullptr);
    
    // All header fields should be zeroed
    EXPECT_EQ(env.target(), 0u);
    EXPECT_EQ(env.source(), 0u);
    EXPECT_EQ(env.message_id(), 0u);
    EXPECT_EQ(env.result(), 0u);
    EXPECT_EQ(env.event_id(), 0u);
}

/**
 * \brief   MessageEnvelope constructed with reserve size allocates correct buffer.
 **/
TEST(EventEnvelopeTest, reserve_construction)
{
    constexpr uint32_t reserve_bytes { 256u };
    areg::MessageEnvelope env(reserve_bytes, areg::BLOCK_SIZE);
    
    ASSERT_NE(env.header(), nullptr);
    EXPECT_GE(env.size_available(), reserve_bytes);
}

/**
 * \brief   MessageEnvelope constructed from raw bytes stores bytes as payload.
 *          Note: the (raw, size) constructor writes bytes into the payload area,
 *          not into the EventHeader. Header fields are independently managed.
 **/
TEST(EventEnvelopeTest, raw_bytes_construction)
{
    constexpr uint8_t raw[]{ 0x01u, 0x02u, 0x03u, 0x04u,
                              0x05u, 0x06u, 0x07u, 0x08u };
    constexpr uint32_t size{ static_cast<uint32_t>(sizeof(raw)) };

    areg::MessageEnvelope env(raw, size);
    ASSERT_NE(env.header(), nullptr);
    ASSERT_EQ(env.size_used(), size);

    const uint8_t* payload{ env.buffer() };
    ASSERT_NE(payload, nullptr);
    for (uint32_t i{ 0u }; i < size; ++i)
        EXPECT_EQ(payload[i], raw[i]);
}

/**
 * \brief   MessageEnvelope constructed from EventHeader preserves header fields.
 **/
TEST(EventEnvelopeTest, event_header_construction)
{
    areg::EventHeader hdr{};
    hdr.target = 42u;
    hdr.source = 99u;
    hdr.messageId = 777u;
    
    areg::MessageEnvelope env(hdr, 128u);
    ASSERT_NE(env.header(), nullptr);
    EXPECT_EQ(env.target(), 42u);
    EXPECT_EQ(env.source(), 99u);
    EXPECT_EQ(env.message_id(), 777u);
}

/**
 * \brief   MessageEnvelope constructed with eventType and priority stores correct values.
 **/
TEST(EventEnvelopeTest, event_type_priority_construction)
{
    constexpr uint16_t event_type { 5u };
    constexpr uint8_t  priority   { 3u };
    
    areg::MessageEnvelope env(event_type, priority, 64u);
    ASSERT_NE(env.header(), nullptr);
    EXPECT_EQ(env.event_type(), event_type);
    EXPECT_EQ(env.priority(), priority);
}

//////////////////////////////////////////////////////////////////////////
// 2. Header bulk access
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   raw_envelope() returns valid pointer when buffer is allocated.
 **/
TEST(EventEnvelopeTest, raw_envelope_access)
{
    areg::MessageEnvelope env(true);
    const areg::RawEnvelope* raw = env.raw_envelope();
    ASSERT_NE(raw, nullptr);
}

/**
 * \brief   consumer() and set_consumer() round-trip correctly.
 **/
TEST(EventEnvelopeTest, consumer_bulk_roundtrip)
{
    areg::MessageEnvelope env(true);
    
    areg::Endpoint ep{};
    ep.id      = 100u;
    ep.number  = 200u;
    ep.thread  = 300u;
    ep.major   = 1u;
    ep.minor   = 2u;
    ep.patch   = 3u;
    ep.type    = 4u;
    
    env.set_consumer(ep);
    const areg::Endpoint& result = env.consumer();
    
    EXPECT_EQ(result.id,      100u);
    EXPECT_EQ(result.number,  200u);
    EXPECT_EQ(result.thread,  300u);
    EXPECT_EQ(result.major, 1u);
    EXPECT_EQ(result.minor, 2u);
    EXPECT_EQ(result.patch, 3u);
    EXPECT_EQ(result.type,    4u);
}

/**
 * \brief   provider() and set_provider() round-trip correctly.
 **/
TEST(EventEnvelopeTest, provider_bulk_roundtrip)
{
    areg::MessageEnvelope env(true);
    
    areg::Endpoint ep{};
    ep.id      = 500u;
    ep.number  = 600u;
    ep.thread  = 700u;
    ep.major   = 5u;
    ep.minor   = 6u;
    ep.patch   = 7u;
    ep.type    = 8u;
    
    env.set_provider(ep);
    const areg::Endpoint& result = env.provider();
    
    EXPECT_EQ(result.id,      500u);
    EXPECT_EQ(result.number,  600u);
    EXPECT_EQ(result.thread,  700u);
    EXPECT_EQ(result.major, 5u);
    EXPECT_EQ(result.minor, 6u);
    EXPECT_EQ(result.patch, 7u);
    EXPECT_EQ(result.type,    8u);
}

/**
 * \brief   raw_service() and set_raw_service() round-trip correctly.
 **/
TEST(EventEnvelopeTest, raw_service_roundtrip)
{
    areg::MessageEnvelope env(true);
    
    areg::RawService svc{};
    svc.role    = 0xABCDu;
    svc.service = 0x1234u;
    
    env.set_raw_service(svc);
    const areg::RawService& result = env.raw_service();
    
    EXPECT_EQ(result.role,    0xABCDu);
    EXPECT_EQ(result.service, 0x1234u);
}

//////////////////////////////////////////////////////////////////////////
// 3. Consumer endpoint fields
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   consumer_id() and set_consumer_id() round-trip.
 **/
TEST(EventEnvelopeTest, consumer_id_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t id { 0x12345678u };
    env.set_consumer_id(id);
    EXPECT_EQ(env.consumer_id(), id);
}

/**
 * \brief   consumer_number() and set_consumer_number() round-trip.
 **/
TEST(EventEnvelopeTest, consumer_number_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t num { 0xABCDEF00u };
    env.set_consumer_number(num);
    EXPECT_EQ(env.consumer_number(), num);
}

/**
 * \brief   consumer_thread() and set_consumer_thread() round-trip.
 **/
TEST(EventEnvelopeTest, consumer_thread_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t thread { 0x99887766u };
    env.set_consumer_thread(thread);
    EXPECT_EQ(env.consumer_thread(), thread);
}

/**
 * \brief   consumer_version_major() and set_consumer_version_major() round-trip.
 **/
TEST(EventEnvelopeTest, consumer_version_major_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint16_t ver { 10u };
    env.set_consumer_version_major(ver);
    EXPECT_EQ(env.consumer_version_major(), ver);
}

/**
 * \brief   consumer_version_minor() and set_consumer_version_minor() round-trip.
 **/
TEST(EventEnvelopeTest, consumer_version_minor_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint16_t ver { 20u };
    env.set_consumer_version_minor(ver);
    EXPECT_EQ(env.consumer_version_minor(), ver);
}

/**
 * \brief   consumer_version_patch() and set_consumer_version_patch() round-trip.
 **/
TEST(EventEnvelopeTest, consumer_version_patch_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint16_t ver { 30u };
    env.set_consumer_version_patch(ver);
    EXPECT_EQ(env.consumer_version_patch(), ver);
}

/**
 * \brief   consumer_type() and set_consumer_type() round-trip.
 **/
TEST(EventEnvelopeTest, consumer_type_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint16_t type { 7u };
    env.set_consumer_type(type);
    EXPECT_EQ(env.consumer_type(), type);
}

//////////////////////////////////////////////////////////////////////////
// 4. Provider endpoint fields
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   provider_id() and set_provider_id() round-trip.
 **/
TEST(EventEnvelopeTest, provider_id_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t id { 0xFEDCBA98u };
    env.set_provider_id(id);
    EXPECT_EQ(env.provider_id(), id);
}

/**
 * \brief   provider_number() and set_provider_number() round-trip.
 **/
TEST(EventEnvelopeTest, provider_number_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t num { 0x55555555u };
    env.set_provider_number(num);
    EXPECT_EQ(env.provider_number(), num);
}

/**
 * \brief   provider_thread() and set_provider_thread() round-trip.
 **/
TEST(EventEnvelopeTest, provider_thread_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t thread { 0x11223344u };
    env.set_provider_thread(thread);
    EXPECT_EQ(env.provider_thread(), thread);
}

/**
 * \brief   provider_version_major() and set_provider_version_major() round-trip.
 **/
TEST(EventEnvelopeTest, provider_version_major_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint16_t ver { 100u };
    env.set_provider_version_major(ver);
    EXPECT_EQ(env.provider_version_major(), ver);
}

/**
 * \brief   provider_version_minor() and set_provider_version_minor() round-trip.
 **/
TEST(EventEnvelopeTest, provider_version_minor_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint16_t ver { 200u };
    env.set_provider_version_minor(ver);
    EXPECT_EQ(env.provider_version_minor(), ver);
}

/**
 * \brief   provider_version_patch() and set_provider_version_patch() round-trip.
 **/
TEST(EventEnvelopeTest, provider_version_patch_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint16_t ver { 300u };
    env.set_provider_version_patch(ver);
    EXPECT_EQ(env.provider_version_patch(), ver);
}

/**
 * \brief   provider_type() and set_provider_type() round-trip.
 **/
TEST(EventEnvelopeTest, provider_type_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint16_t type { 9u };
    env.set_provider_type(type);
    EXPECT_EQ(env.provider_type(), type);
}

//////////////////////////////////////////////////////////////////////////
// 5. Shared service fields
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   service_role() and set_service_role() round-trip.
 **/
TEST(EventEnvelopeTest, service_role_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t role { 0xDEADBEEFu };
    env.set_service_role(role);
    EXPECT_EQ(env.service_role(), role);
}

/**
 * \brief   service_item() and set_service_item() round-trip.
 **/
TEST(EventEnvelopeTest, service_item_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t item { 0xCAFEBABEu };
    env.set_service_item(item);
    EXPECT_EQ(env.service_item(), item);
}

//////////////////////////////////////////////////////////////////////////
// 6. Event routing fields
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   target() and set_target() round-trip.
 **/
TEST(EventEnvelopeTest, target_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t tgt { 0x11111111u };
    env.set_target(tgt);
    EXPECT_EQ(env.target(), tgt);
}

/**
 * \brief   source() and set_source() round-trip.
 **/
TEST(EventEnvelopeTest, source_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t src { 0x22222222u };
    env.set_source(src);
    EXPECT_EQ(env.source(), src);
}

/**
 * \brief   channel() and set_channel() round-trip.
 **/
TEST(EventEnvelopeTest, channel_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t ch { 0x33333333u };
    env.set_channel(ch);
    EXPECT_EQ(env.channel(), ch);
}

/**
 * \brief   message_id() and set_message_id() round-trip.
 **/
TEST(EventEnvelopeTest, message_id_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t mid { 0x44444444u };
    env.set_message_id(mid);
    EXPECT_EQ(env.message_id(), mid);
}

/**
 * \brief   sequence() and set_sequence() round-trip.
 **/
TEST(EventEnvelopeTest, sequence_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint64_t seq { 0xDEADBEEFu };
    env.set_sequence(seq);
    EXPECT_EQ(env.sequence(), seq);
}

/**
 * \brief   result() and set_result() round-trip.
 **/
TEST(EventEnvelopeTest, result_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t res { 0xABCDu };
    env.set_result(res);
    EXPECT_EQ(env.result(), res);
}

/**
 * \brief   event_type() and set_event_type() round-trip.
 **/
TEST(EventEnvelopeTest, event_type_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint16_t evt { 7u };
    env.set_event_type(evt);
    EXPECT_EQ(env.event_type(), evt);
}

/**
 * \brief   call_type() and set_call_type() round-trip.
 **/
TEST(EventEnvelopeTest, call_type_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint8_t ct { 3u };
    env.set_call_type(ct);
    EXPECT_EQ(env.call_type(), ct);
}

/**
 * \brief   priority() and set_priority() round-trip.
 **/
TEST(EventEnvelopeTest, priority_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint8_t prio { 5u };
    env.set_priority(prio);
    EXPECT_EQ(env.priority(), prio);
}

/**
 * \brief   event_id() and set_event_id() round-trip.
 **/
TEST(EventEnvelopeTest, event_id_roundtrip)
{
    areg::MessageEnvelope env(true);
    constexpr uint32_t cid { 0x99999999u };
    env.set_event_id(cid);
    EXPECT_EQ(env.event_id(), cid);
}

//////////////////////////////////////////////////////////////////////////
// 7. Operations
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   clone() creates an independent copy with same header fields.
 **/
TEST(EventEnvelopeTest, clone_independent)
{
    areg::MessageEnvelope env(true);
    env.set_target(111u);
    env.set_source(222u);
    env.set_message_id(333u);
    
    areg::MessageEnvelope cloned = env.clone();
    
    ASSERT_NE(cloned.header(), nullptr);
    EXPECT_EQ(cloned.target(), 111u);
    EXPECT_EQ(cloned.source(), 222u);
    EXPECT_EQ(cloned.message_id(), 333u);
    EXPECT_FALSE(cloned.is_shared());
}

/**
 * \brief   clone() with consumer override updates consumer endpoint.
 **/
TEST(EventEnvelopeTest, clone_with_consumer_override)
{
    areg::MessageEnvelope env(true);
    env.set_consumer_id(100u);
    
    areg::Endpoint new_consumer{};
    new_consumer.id = 999u;
    
    areg::MessageEnvelope cloned = env.clone(&new_consumer, nullptr);
    EXPECT_EQ(cloned.consumer_id(), 999u);
}

/**
 * \brief   clone() with provider override updates provider endpoint.
 **/
TEST(EventEnvelopeTest, clone_with_provider_override)
{
    areg::MessageEnvelope env(true);
    env.set_provider_id(200u);
    
    areg::Endpoint new_provider{};
    new_provider.id = 888u;
    
    areg::MessageEnvelope cloned = env.clone(nullptr, &new_provider);
    EXPECT_EQ(cloned.provider_id(), 888u);
}

/**
 * \brief   init_envelope() reinitializes with new header.
 **/
TEST(EventEnvelopeTest, init_envelope_reuse)
{
    areg::MessageEnvelope env(true);
    env.set_target(100u);
    
    areg::EventHeader new_hdr{};
    new_hdr.target = 500u;
    new_hdr.source = 600u;
    
    uint8_t* payload = env.init_envelope(new_hdr, 64u);
    ASSERT_NE(payload, nullptr);
    EXPECT_EQ(env.target(), 500u);
    EXPECT_EQ(env.source(), 600u);
}

/**
 * \brief   payload_ptr() returns valid pointer after initialization.
 **/
TEST(EventEnvelopeTest, payload_ptr_access)
{
    areg::MessageEnvelope env(true);
    uint8_t* payload = env.payload_ptr();
    ASSERT_NE(payload, nullptr);
    
    // Write some data to payload
    std::memset(payload, 0xAA, 16);
    
    // Read back const
    const uint8_t* const_payload = env.payload_ptr();
    ASSERT_NE(const_payload, nullptr);
    EXPECT_EQ(const_payload[0], 0xAAu);
}

//////////////////////////////////////////////////////////////////////////
// 8. Copy and move semantics
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Copy constructor shares the underlying buffer (reference count increments).
 **/
TEST(EventEnvelopeTest, copy_constructor_shares)
{
    areg::MessageEnvelope env1(true);
    env1.set_target(42u);
    
    areg::MessageEnvelope env2(env1);
    EXPECT_TRUE(env1.is_shared());
    EXPECT_TRUE(env2.is_shared());
    EXPECT_EQ(env2.target(), 42u);
}

/**
 * \brief   Move constructor transfers ownership without incrementing reference count.
 **/
TEST(EventEnvelopeTest, move_constructor_transfers)
{
    areg::MessageEnvelope env1(true);
    env1.set_target(99u);
    EXPECT_FALSE(env1.is_shared());
    
    areg::MessageEnvelope env2(std::move(env1));
    EXPECT_EQ(env2.target(), 99u);
    EXPECT_FALSE(env2.is_shared());
}

/**
 * \brief   Copy assignment shares the buffer.
 **/
TEST(EventEnvelopeTest, copy_assignment_shares)
{
    areg::MessageEnvelope env1(true);
    env1.set_target(111u);
    
    areg::MessageEnvelope env2(true);
    env2 = env1;
    
    EXPECT_TRUE(env1.is_shared());
    EXPECT_TRUE(env2.is_shared());
    EXPECT_EQ(env2.target(), 111u);
}

/**
 * \brief   Move assignment transfers ownership.
 **/
TEST(EventEnvelopeTest, move_assignment_transfers)
{
    areg::MessageEnvelope env1(true);
    env1.set_target(222u);
    
    areg::MessageEnvelope env2(true);
    env2 = std::move(env1);
    
    EXPECT_EQ(env2.target(), 222u);
    EXPECT_FALSE(env2.is_shared());
}

//////////////////////////////////////////////////////////////////////////
// 9. All header fields survive clone
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   All routing and endpoint fields survive an independent clone().
 **/
TEST(EventEnvelopeTest, all_fields_survive_clone)
{
    areg::MessageEnvelope env(true);
    
    // Set all routing fields
    env.set_target(10u);
    env.set_source(20u);
    env.set_channel(30u);
    env.set_message_id(40u);
    env.set_sequence(50u);
    env.set_result(60u);
    env.set_event_type(7u);
    env.set_call_type(3u);
    env.set_priority(5u);
    env.set_event_id(0x12345678u);
    
    // Set consumer endpoint
    env.set_consumer_id(100u);
    env.set_consumer_number(200u);
    env.set_consumer_thread(300u);
    env.set_consumer_version_major(1u);
    env.set_consumer_version_minor(2u);
    env.set_consumer_version_patch(3u);
    env.set_consumer_type(4u);
    
    // Set provider endpoint
    env.set_provider_id(500u);
    env.set_provider_number(600u);
    env.set_provider_thread(700u);
    env.set_provider_version_major(5u);
    env.set_provider_version_minor(6u);
    env.set_provider_version_patch(7u);
    env.set_provider_type(8u);
    
    // Set service fields
    env.set_service_role(0xABCDu);
    env.set_service_item(0x1234u);
    
    // Clone and verify all fields
    areg::MessageEnvelope cloned = env.clone();
    
    EXPECT_EQ(cloned.target(),      10u);
    EXPECT_EQ(cloned.source(),      20u);
    EXPECT_EQ(cloned.channel(),     30u);
    EXPECT_EQ(cloned.message_id(),  40u);
    EXPECT_EQ(cloned.sequence(),    50u);
    EXPECT_EQ(cloned.result(),      60u);
    EXPECT_EQ(cloned.event_type(),  7u);
    EXPECT_EQ(cloned.call_type(),   3u);
    EXPECT_EQ(cloned.priority(),    5u);
    EXPECT_EQ(cloned.event_id(),    0x12345678u);
    
    EXPECT_EQ(cloned.consumer_id(),             100u);
    EXPECT_EQ(cloned.consumer_number(),         200u);
    EXPECT_EQ(cloned.consumer_thread(),         300u);
    EXPECT_EQ(cloned.consumer_version_major(),  1u);
    EXPECT_EQ(cloned.consumer_version_minor(),  2u);
    EXPECT_EQ(cloned.consumer_version_patch(),  3u);
    EXPECT_EQ(cloned.consumer_type(),           4u);
    
    EXPECT_EQ(cloned.provider_id(),             500u);
    EXPECT_EQ(cloned.provider_number(),         600u);
    EXPECT_EQ(cloned.provider_thread(),         700u);
    EXPECT_EQ(cloned.provider_version_major(),  5u);
    EXPECT_EQ(cloned.provider_version_minor(),  6u);
    EXPECT_EQ(cloned.provider_version_patch(),  7u);
    EXPECT_EQ(cloned.provider_type(),           8u);
    
    EXPECT_EQ(cloned.service_role(), 0xABCDu);
    EXPECT_EQ(cloned.service_item(), 0x1234u);
}

// Reading an empty envelope returns the documented sentinel values.
TEST(EventEnvelopeTest, empty_header_accessors_return_sentinels)
{
    const areg::MessageEnvelope envelope;
    EXPECT_EQ(envelope.header(), nullptr);
    EXPECT_EQ(envelope.raw_envelope(), nullptr);
    EXPECT_EQ(envelope.payload_ptr(), nullptr);
    EXPECT_EQ(envelope.consumer().id, 0u);
    EXPECT_EQ(envelope.consumer().number, 0u);
    EXPECT_EQ(envelope.consumer().thread, 0u);
    EXPECT_EQ(envelope.consumer().major, 0u);
    EXPECT_EQ(envelope.consumer().minor, 0u);
    EXPECT_EQ(envelope.consumer().patch, 0u);
    EXPECT_EQ(envelope.consumer().type, 0u);
    EXPECT_EQ(envelope.provider().id, 0u);
    EXPECT_EQ(envelope.provider().number, 0u);
    EXPECT_EQ(envelope.provider().thread, 0u);
    EXPECT_EQ(envelope.provider().major, 0u);
    EXPECT_EQ(envelope.provider().minor, 0u);
    EXPECT_EQ(envelope.provider().patch, 0u);
    EXPECT_EQ(envelope.provider().type, 0u);
    EXPECT_EQ(envelope.raw_service().role, 0u);
    EXPECT_EQ(envelope.raw_service().service, 0u);
    EXPECT_EQ(envelope.consumer_id(), 0u);
    EXPECT_EQ(envelope.consumer_number(), 0u);
    EXPECT_EQ(envelope.consumer_thread(), 0u);
    EXPECT_EQ(envelope.consumer_version_major(), 0u);
    EXPECT_EQ(envelope.consumer_version_minor(), 0u);
    EXPECT_EQ(envelope.consumer_version_patch(), 0u);
    EXPECT_EQ(envelope.consumer_type(), 0u);
    EXPECT_EQ(envelope.provider_id(), 0u);
    EXPECT_EQ(envelope.provider_number(), 0u);
    EXPECT_EQ(envelope.provider_thread(), 0u);
    EXPECT_EQ(envelope.provider_version_major(), 0u);
    EXPECT_EQ(envelope.provider_version_minor(), 0u);
    EXPECT_EQ(envelope.provider_version_patch(), 0u);
    EXPECT_EQ(envelope.provider_type(), 0u);
    EXPECT_EQ(envelope.service_role(), 0u);
    EXPECT_EQ(envelope.service_item(), 0u);
    EXPECT_EQ(envelope.target(), 0u);
    EXPECT_EQ(envelope.source(), 0u);
    EXPECT_EQ(envelope.channel(), 0u);
    EXPECT_EQ(envelope.message_id(), areg::INVALID_MESSAGE_ID);
    EXPECT_EQ(envelope.sequence(), areg::SEQUENCE_NUMBER_ANY);
    EXPECT_EQ(envelope.result(), areg::INVALID_VALUE);
    EXPECT_EQ(envelope.event_type(), 0u);
    EXPECT_EQ(envelope.call_type(), 0u);
    EXPECT_EQ(envelope.priority(), 0u);
    EXPECT_EQ(envelope.event_id(), areg::CHECKSUM_IGNORE);
    EXPECT_FALSE(envelope.clone().is_valid());
}

// Setting header fields on an empty envelope does not allocate storage.
TEST(EventEnvelopeTest, empty_header_setters_do_not_allocate)
{
    areg::MessageEnvelope envelope;
    areg::Endpoint endpoint{};
    endpoint.id = 99u;
    areg::RawService service{};
    service.role = 88u;
    envelope.set_consumer(endpoint);
    envelope.set_provider(endpoint);
    envelope.set_raw_service(service);
    envelope.set_consumer_id(1u);
    envelope.set_consumer_number(2u);
    envelope.set_consumer_thread(3u);
    envelope.set_consumer_version_major(4u);
    envelope.set_consumer_version_minor(5u);
    envelope.set_consumer_version_patch(6u);
    envelope.set_consumer_type(7u);
    envelope.set_provider_id(8u);
    envelope.set_provider_number(9u);
    envelope.set_provider_thread(10u);
    envelope.set_provider_version_major(11u);
    envelope.set_provider_version_minor(12u);
    envelope.set_provider_version_patch(13u);
    envelope.set_provider_type(14u);
    envelope.set_service_role(15u);
    envelope.set_service_item(16u);
    envelope.set_target(17u);
    envelope.set_source(18u);
    envelope.set_channel(19u);
    envelope.set_message_id(20u);
    envelope.set_sequence(21u);
    envelope.set_result(22u);
    envelope.set_event_type(23u);
    envelope.set_call_type(24u);
    envelope.set_priority(25u);
    envelope.set_event_id(26u);
    EXPECT_EQ(envelope.header(), nullptr);
    EXPECT_EQ(envelope.payload_ptr(), nullptr);
    EXPECT_FALSE(envelope.is_valid());
    EXPECT_EQ(envelope.message_id(), areg::INVALID_MESSAGE_ID);
    EXPECT_EQ(envelope.result(), areg::INVALID_VALUE);
    EXPECT_EQ(envelope.sequence(), areg::SEQUENCE_NUMBER_ANY);
}

// Constructors without initialized storage leave the envelope empty.
TEST(EventEnvelopeTest, uninitialized_constructors_leave_envelope_empty)
{
    areg::MessageEnvelope notInitialized(false);
    areg::MessageEnvelope noType(uint16_t{ 0u }, uint8_t{ 3u });
    areg::MessageEnvelope noReservation(0u, areg::BLOCK_SIZE);
    EXPECT_FALSE(notInitialized.is_valid());
    EXPECT_FALSE(noType.is_valid());
    EXPECT_FALSE(noReservation.is_valid());
    notInitialized.destroy_event();
    EXPECT_FALSE(notInitialized.is_valid());
}

// Growing an envelope keeps its header fields and payload intact.
TEST(EventEnvelopeTest, growth_preserves_header_and_payload)
{
    areg::EventHeader header{};
    header.target = 17u;
    header.source = 29u;
    header.messageId = 41u;
    header.eventId = 53u;
    areg::MessageEnvelope envelope(header, 64u);
    constexpr uint8_t DATA[]{ 3u, 5u, 7u };
    ASSERT_EQ(envelope.write(DATA, sizeof(DATA)), sizeof(DATA));
    const uint32_t capacity{ envelope.size_available() };
    EXPECT_GE(envelope.reserve(capacity + 1u, true), capacity + 1u - sizeof(DATA));
    ASSERT_NE(envelope.header(), nullptr);
    EXPECT_EQ(envelope.target(), header.target);
    EXPECT_EQ(envelope.source(), header.source);
    EXPECT_EQ(envelope.message_id(), header.messageId);
    EXPECT_EQ(envelope.event_id(), header.eventId);
    EXPECT_EQ(envelope.size_used(), sizeof(DATA));
    EXPECT_EQ(envelope.position(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(std::as_const(envelope).payload_ptr(), DATA, sizeof(DATA)), 0);
    EXPECT_EQ(envelope.header()->bufHeader.biOffset, sizeof(areg::EventHeader));
    EXPECT_EQ(envelope.header()->bufHeader.biLength, envelope.size_available());
}

// Reinitializing a shared envelope leaves the other owner's data unchanged.
TEST(EventEnvelopeTest, reinitialize_shared_envelope_keeps_old_owner_intact)
{
    areg::EventHeader first{};
    first.target = 10u;
    areg::MessageEnvelope envelope(first, 64u);
    constexpr uint8_t DATA[]{ 8u, 6u, 4u, 2u };
    ASSERT_EQ(envelope.write(DATA, sizeof(DATA)), sizeof(DATA));
    areg::MessageEnvelope retained(envelope);
    const uint8_t* const allocation{ retained.buffer() };
    areg::EventHeader second{};
    second.target = 20u;
    second.source = 30u;
    ASSERT_NE(envelope.init_envelope(second, 64u), nullptr);
    EXPECT_NE(envelope.buffer(), allocation);
    EXPECT_EQ(envelope.target(), 20u);
    EXPECT_EQ(envelope.source(), 30u);
    EXPECT_EQ(envelope.size_used(), 0u);
    EXPECT_EQ(envelope.position(), 0u);
    EXPECT_TRUE(envelope.is_unique());
    EXPECT_TRUE(retained.is_unique());
    EXPECT_EQ(retained.target(), 10u);
    EXPECT_EQ(retained.size_used(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(retained.buffer(), DATA, sizeof(DATA)), 0);
}

// Reinitialization reuses a unique block when it fits and grows it when needed.
TEST(EventEnvelopeTest, reinitialize_reuses_unique_block_or_grows)
{
    areg::EventHeader header{};
    areg::MessageEnvelope envelope(header, 64u);
    const uint8_t* const allocation{ envelope.buffer() };
    const uint32_t capacity{ envelope.size_available() };
    header.target = 42u;
    ASSERT_EQ(envelope.init_envelope(header, 1u), allocation);
    EXPECT_EQ(envelope.target(), 42u);
    EXPECT_EQ(envelope.size_available(), capacity);
    EXPECT_TRUE(envelope.is_unique());
    header.target = 99u;
    ASSERT_NE(envelope.init_envelope(header, capacity + 1u), nullptr);
    EXPECT_GT(envelope.size_available(), capacity);
    EXPECT_EQ(envelope.target(), 99u);
    EXPECT_EQ(envelope.size_used(), 0u);
    EXPECT_EQ(envelope.position(), 0u);
}

// Cloning replaces both endpoints and gives the payload independent storage.
TEST(EventEnvelopeTest, payload_clone_overrides_both_endpoints_without_aliasing)
{
    areg::MessageEnvelope envelope(true);
    constexpr uint8_t DATA[]{ 2u, 4u, 6u, 8u };
    ASSERT_EQ(envelope.write(DATA, sizeof(DATA)), sizeof(DATA));
    envelope.set_consumer_id(11u);
    envelope.set_provider_id(22u);
    areg::Endpoint consumer{};
    consumer.id = 33u;
    areg::Endpoint provider{};
    provider.id = 44u;
    areg::MessageEnvelope cloned{ envelope.clone(&consumer, &provider) };
    EXPECT_EQ(cloned.consumer_id(), 33u);
    EXPECT_EQ(cloned.provider_id(), 44u);
    EXPECT_EQ(envelope.consumer_id(), 11u);
    EXPECT_EQ(envelope.provider_id(), 22u);
    EXPECT_EQ(cloned.size_used(), sizeof(DATA));
    EXPECT_EQ(cloned.position(), 0u);
    EXPECT_NE(cloned.buffer(), envelope.buffer());
    EXPECT_EQ(std::memcmp(cloned.buffer(), DATA, sizeof(DATA)), 0);
    cloned.buffer()[0] = 99u;
    EXPECT_EQ(envelope.buffer()[0], DATA[0]);
}

// Payload cleanup runs only when the last envelope owner is destroyed.
TEST(EventEnvelopeTest, destroy_event_cleans_payload_only_for_last_owner)
{
    uint32_t calls{ 0u };
    const auto cleanup = +[](void* payload) noexcept
    {
        uint32_t* counter{ nullptr };
        std::memcpy(&counter, payload, sizeof(counter));
        ++*counter;
    };
    areg::MessageEnvelope envelope(true);
    uint32_t* const counter{ &calls };
    ASSERT_EQ(envelope.write(reinterpret_cast<const uint8_t*>(&counter), sizeof(counter)), sizeof(counter));
    static_assert(sizeof(cleanup) <= sizeof(envelope.header()->custom));
    std::memcpy(&envelope.header()->custom, &cleanup, sizeof(cleanup));
    areg::MessageEnvelope shared(envelope);
    envelope.destroy_event();
    EXPECT_FALSE(envelope.is_valid());
    EXPECT_EQ(calls, 0u);
    shared.destroy_event();
    EXPECT_FALSE(shared.is_valid());
    EXPECT_EQ(calls, 1u);
    shared.destroy_event();
    EXPECT_EQ(calls, 1u);
}

// A truncated payload cannot be read beyond the bytes that remain.
TEST(EventEnvelopeTest, truncated_payload_read_is_bounded)
{
    constexpr uint8_t DATA[]{ 0x10u, 0x20u, 0x30u };
    areg::MessageEnvelope envelope(DATA, sizeof(DATA));
    envelope.reset();
    std::array<uint8_t, 5> output{ 9u, 9u, 9u, 9u, 9u };
    EXPECT_EQ(envelope.read(output.data(), static_cast<uint32_t>(output.size())), sizeof(DATA));
    EXPECT_EQ(output, (std::array<uint8_t, 5>{ 0x10u, 0x20u, 0x30u, 9u, 9u }));
    EXPECT_EQ(envelope.read(output.data(), 1u), 0u);
    EXPECT_EQ(envelope.position(), sizeof(DATA));
}

// Inserting a payload writes the 32-bit length, then the bytes.
TEST(EventEnvelopeTest, payload_insertion_writes_length_prefix)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::SharedBuffer source(DATA, sizeof(DATA));
    areg::MessageEnvelope envelope(true);
    envelope << source;
    ASSERT_EQ(envelope.size_used(), sizeof(uint32_t) + sizeof(DATA));
    uint32_t length{ 0u };
    std::memcpy(&length, envelope.buffer(), sizeof(length));
    EXPECT_EQ(length, sizeof(DATA));
    EXPECT_EQ(std::memcmp(envelope.buffer() + sizeof(uint32_t), DATA, sizeof(DATA)), 0);
}

// The explicit envelope operator writes the same bytes as the generic stream operator.
TEST(EventEnvelopeTest, payload_insertion_matches_generic_stream)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::SharedBuffer source(DATA, sizeof(DATA));
    const areg::SharedBuffer invalid;
    areg::MessageEnvelope direct(true);
    direct << source << invalid;
    areg::MessageEnvelope generic(true);
    areg::OutStream& out{ generic };
    out << source << invalid;
    ASSERT_EQ(direct.size_used(), generic.size_used());
    EXPECT_EQ(std::memcmp(direct.buffer(), generic.buffer(), direct.size_used()), 0);
}

// Extracting a payload gives the destination its own copy.
TEST(EventEnvelopeTest, payload_extraction_copies_payload)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::SharedBuffer source(DATA, sizeof(DATA));
    areg::MessageEnvelope envelope(true);
    envelope << source;
    envelope.reset();
    areg::SharedBuffer destination;
    envelope >> destination;
    EXPECT_FALSE(destination.is_view());
    EXPECT_NE(destination.share_buffer().get(), envelope.share_buffer().get());
    ASSERT_EQ(destination.size_used(), sizeof(DATA));
    EXPECT_EQ(destination.position(), 0u);
    EXPECT_EQ(std::memcmp(destination.buffer(), DATA, sizeof(DATA)), 0);
    EXPECT_EQ(envelope.position(), sizeof(uint32_t) + sizeof(DATA));
}

// An empty buffer round-trips as empty and replaces stale destination content.
TEST(EventEnvelopeTest, empty_payload_round_trip_clears_destination)
{
    const areg::SharedBuffer empty;
    areg::MessageEnvelope envelope(true);
    envelope << empty;
    EXPECT_EQ(envelope.size_used(), sizeof(uint32_t));
    envelope.reset();
    constexpr uint8_t STALE[]{ 4u, 5u };
    areg::SharedBuffer destination(STALE, sizeof(STALE));
    envelope >> destination;
    EXPECT_FALSE(destination.is_valid());
    EXPECT_EQ(envelope.position(), sizeof(uint32_t));
}

// A length beyond the payload leaves the destination and the envelope cursor unchanged.
TEST(EventEnvelopeTest, truncated_payload_extraction_keeps_destination)
{
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::SharedBuffer source(DATA, sizeof(DATA));
    areg::MessageEnvelope envelope(true);
    envelope << source;
    envelope.set_size_used(envelope.size_used() - 1u);
    envelope.reset();
    constexpr uint8_t STALE[]{ 4u, 5u };
    areg::SharedBuffer destination(STALE, sizeof(STALE));
    envelope >> destination;
    EXPECT_EQ(envelope.position(), 0u);
    ASSERT_EQ(destination.size_used(), sizeof(STALE));
    EXPECT_EQ(std::memcmp(destination.buffer(), STALE, sizeof(STALE)), 0);
}

// Extracting from an envelope without payload leaves the destination unchanged.
TEST(EventEnvelopeTest, empty_payload_extraction_keeps_destination)
{
    const areg::MessageEnvelope empty;
    constexpr uint8_t DATA[]{ 1u, 2u, 3u };
    areg::SharedBuffer destination(DATA, sizeof(DATA));
    const uint8_t* const allocation{ destination.buffer() };
    empty >> destination;
    EXPECT_EQ(destination.buffer(), allocation);
    EXPECT_EQ(destination.size_used(), sizeof(DATA));
    EXPECT_EQ(destination.position(), sizeof(DATA));
    EXPECT_EQ(std::memcmp(destination.buffer(), DATA, sizeof(DATA)), 0);
}