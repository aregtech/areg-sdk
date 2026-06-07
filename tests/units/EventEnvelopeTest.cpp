/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
    EXPECT_EQ(env.checksum(), 0u);
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
 * \brief   MessageEnvelope constructed from raw bytes preserves data.
 **/
TEST(EventEnvelopeTest, raw_bytes_construction)
{
    // Create reference envelope
    areg::MessageEnvelope ref(true);
    ref.set_target(111u);
    ref.set_source(222u);
    ref.set_message_id(333u);
    ref.buffer_completion_fix();
    
    const uint8_t*  raw = ref.buffer();
    const uint32_t  size = ref.size_used();
    ASSERT_GT(size, 0u);
    
    // Reconstruct from raw bytes
    areg::MessageEnvelope rebuilt(raw, size);
    ASSERT_NE(rebuilt.header(), nullptr);
    EXPECT_EQ(rebuilt.target(), 111u);
    EXPECT_EQ(rebuilt.source(), 222u);
    EXPECT_EQ(rebuilt.message_id(), 333u);
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

/**
 * \brief   checksum() returns correct value after buffer_completion_fix().
 **/
TEST(EventEnvelopeTest, checksum_after_completion)
{
    areg::MessageEnvelope env(true);
    env.set_message_id(100u);
    env.set_target(200u);
    
    env.buffer_completion_fix();
    EXPECT_TRUE(env.is_checksum_valid() || env.is_checksum_ignore());
}

/**
 * \brief   is_checksum_ignore() returns true when checksum is areg::CHECKSUM_IGNORE.
 **/
TEST(EventEnvelopeTest, checksum_ignore_flag)
{
    areg::MessageEnvelope env(true);
    // Default constructed should have CHECKSUM_INVALID
    EXPECT_FALSE(env.is_checksum_ignore());
    
    env.buffer_completion_fix();
    // After fix with no payload, checksum should be computable or ignored
    EXPECT_TRUE(env.is_checksum_valid() || env.is_checksum_ignore());
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