/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/RemoteMessageTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for RemoteMessage.
 *              Covers: initialization, header field accessors, checksum,
 *              payload streaming, clone, and copy/move semantics.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/RemoteMessage.hpp"

#include "areg/base/SharedBuffer.hpp"

#include <cstring>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// 1. Initialization
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Default-constructed RemoteMessage is valid with zero header fields.
 *          The cursor sits at the start of the payload area (past the header).
 **/
TEST(RemoteMessageTest, default_construction)
{
    areg::RemoteMessage msg;
    EXPECT_FALSE(msg.is_valid());

    // Header fields default to 0
    EXPECT_EQ(msg.source()    , areg::SOURCE_UNKNOWN);
    EXPECT_EQ(msg.target()    , areg::TARGET_UNKNOWN);
    EXPECT_EQ(msg.message_id(), areg::INVALID_MESSAGE_ID);
    EXPECT_EQ(msg.result()    , areg::INVALID_VALUE);
    EXPECT_EQ(msg.sequence()  , areg::SEQUENCE_NUMBER_ANY);

    // Cursor is at position 0 of the payload area (past the internal header)
    EXPECT_EQ(msg.position(), areg::Cursor::INVALID_CURSOR_POSITION);
}

/**
 * \brief   RemoteMessage constructed from raw bytes preserves all bytes and is valid.
 **/
TEST(RemoteMessageTest, raw_bytes_construction)
{
    // Construct a reference message and serialise its bytes
    areg::RemoteMessage ref;
    ref.set_source(111u);
    ref.set_target(222u);
    ref.set_message_id(333u);
    ref << uint32_t(0xDEADBEEFu);
    ref.buffer_completion_fix();

    // Rebuild from raw bytes
    const uint8_t*  raw  = ref.buffer();
    const uint32_t  size = ref.size_used();
    ASSERT_GT(size, 0u);

    areg::RemoteMessage rebuilt(raw, size);
    EXPECT_TRUE(rebuilt.is_valid());
    EXPECT_EQ(rebuilt.size_used(), size);
}

//////////////////////////////////////////////////////////////////////////
// 2. Header field accessors
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   set_source() / source() round-trip.
 **/
TEST(RemoteMessageTest, source_roundtrip)
{
    areg::RemoteMessage msg(true);
    constexpr ITEM_ID id { 42u };
    msg.set_source(id);
    EXPECT_EQ(msg.source(), id);
}

/**
 * \brief   set_target() / target() round-trip.
 **/
TEST(RemoteMessageTest, target_roundtrip)
{
    areg::RemoteMessage msg(true);
    constexpr ITEM_ID id { 99u };
    msg.set_target(id);
    EXPECT_EQ(msg.target(), id);
}

/**
 * \brief   set_message_id() / message_id() round-trip.
 **/
TEST(RemoteMessageTest, message_id_roundtrip)
{
    areg::RemoteMessage msg(true);
    constexpr uint32_t mid { 0x12345678u };
    msg.set_message_id(mid);
    EXPECT_EQ(msg.message_id(), mid);
}

/**
 * \brief   set_result() / result() round-trip.
 **/
TEST(RemoteMessageTest, result_roundtrip)
{
    areg::RemoteMessage msg(true);
    constexpr uint32_t res { 0xABCDu };
    msg.set_result(res);
    EXPECT_EQ(msg.result(), res);
}

/**
 * \brief   set_sequence() / sequence() round-trip.
 **/
TEST(RemoteMessageTest, sequence_roundtrip)
{
    areg::RemoteMessage msg(true);
    constexpr SequenceNumber seq { 0xDEADBEEFu };
    msg.set_sequence(seq);
    EXPECT_EQ(msg.sequence(), seq);
}

/**
 * \brief   All header fields survive an independent round-trip through clone().
 **/
TEST(RemoteMessageTest, header_fields_survive_clone)
{
    areg::RemoteMessage msg(true);
    msg.set_source(10u);
    msg.set_target(20u);
    msg.set_message_id(30u);
    msg.set_result(40u);
    msg.set_sequence(50u);

    areg::RemoteMessage cloned = msg.clone();
    EXPECT_EQ(cloned.source(),     ITEM_ID{ 10u });
    EXPECT_EQ(cloned.target(),     ITEM_ID{ 20u });
    EXPECT_EQ(cloned.message_id(), 30u);
    EXPECT_EQ(cloned.result(),     40u);
    EXPECT_EQ(cloned.sequence(),   SequenceNumber{ 50u });
}

//////////////////////////////////////////////////////////////////////////
// 3. Checksum
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   buffer_completion_fix() computes a non-zero checksum; is_checksum_valid() returns true afterwards.
 **/
TEST(RemoteMessageTest, checksum_after_completion_fix)
{
    areg::RemoteMessage msg;
    msg.set_source(1u);
    msg.set_message_id(100u);
    msg << uint32_t(0xCAFEu);

    msg.buffer_completion_fix();
    EXPECT_TRUE(msg.is_checksum_valid());
    EXPECT_NE(msg.checksum(), 0u);   // fix should produce a non-zero checksum
}

/**
 * \brief   is_checksum_ignore() returns true when checksum field is areg::CHECKSUM_IGNORE (0).
 **/
TEST(RemoteMessageTest, checksum_ignore_when_zero)
{
    areg::RemoteMessage msg;
    EXPECT_FALSE(msg.is_valid());
    EXPECT_FALSE(msg.is_checksum_ignore());
    EXPECT_TRUE(msg.checksum() == areg::CHECKSUM_INVALID);

    msg.buffer_completion_fix();
    EXPECT_FALSE(msg.is_checksum_valid());
}

//////////////////////////////////////////////////////////////////////////
// 4. Payload streaming
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Payload written via << is readable at the data offset position.
 **/
TEST(RemoteMessageTest, payload_write_read_roundtrip)
{
    areg::RemoteMessage msg;

    constexpr uint32_t val_a { 0xDEAD0001u };
    constexpr uint32_t val_b { 0xBEEF0002u };
    msg << val_a << val_b;

    EXPECT_EQ(msg.size_used(),2u * sizeof(uint32_t));

    // Seek back to payload start and read
    msg.set_position(0u, areg::Cursor::SeekOrigin::Begin);
    uint32_t ra {}, rb {};
    msg >> ra >> rb;
    EXPECT_EQ(ra, val_a);
    EXPECT_EQ(rb, val_b);
}

/**
 * \brief   Writing payload does not corrupt header fields.
 **/
TEST(RemoteMessageTest, payload_write_preserves_header)
{
    areg::RemoteMessage msg(true);
    msg.set_source(77u);
    msg.set_target(88u);
    msg.set_message_id(99u);

    msg << uint64_t(0xFFFFFFFF00000000uLL);   // write 8-byte payload

    EXPECT_EQ(msg.source(),     ITEM_ID{ 77u });
    EXPECT_EQ(msg.target(),     ITEM_ID{ 88u });
    EXPECT_EQ(msg.message_id(), 99u);
}

//////////////////////////////////////////////////////////////////////////
// 5. Clone
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   clone() returns an independent deep copy with the same header and payload.
 **/
TEST(RemoteMessageTest, clone_independent)
{
    areg::RemoteMessage msg(true);
    msg.set_source(10u);
    msg.set_target(20u);
    msg << uint32_t(0xABCDu);
    msg.buffer_completion_fix();

    areg::RemoteMessage cloned = msg.clone();

    EXPECT_FALSE(msg.is_shared());
    EXPECT_FALSE(cloned.is_shared());
    EXPECT_EQ(cloned.source(),  ITEM_ID{ 10u });
    EXPECT_EQ(cloned.target(),  ITEM_ID{ 20u });
    EXPECT_EQ(cloned.checksum(), msg.checksum());
}

/**
 * \brief   clone(source, target) overrides source and target in the returned copy.
 **/
TEST(RemoteMessageTest, clone_with_override_ids)
{
    areg::RemoteMessage msg(true);
    msg.set_source(10u);
    msg.set_target(20u);
    msg.set_message_id(30u);

    areg::RemoteMessage cloned = msg.clone(ITEM_ID{ 100u }, ITEM_ID{ 200u });

    EXPECT_EQ(cloned.source(),     ITEM_ID{ 100u });
    EXPECT_EQ(cloned.target(),     ITEM_ID{ 200u });
    EXPECT_EQ(cloned.message_id(), 30u);   // unchanged
}

/**
 * \brief   Modifying the original after clone does not affect the clone.
 **/
TEST(RemoteMessageTest, clone_data_independence)
{
    areg::RemoteMessage msg(true);
    msg.set_source(1u);
    const uint32_t data_start = msg.position();
    msg << uint32_t(0xAAAAAAAAu);

    areg::RemoteMessage cloned = msg.clone();

    // Overwrite payload in original
    msg.set_position(static_cast<int32_t>(data_start), areg::Cursor::SeekOrigin::Begin);
    msg << uint32_t(0xCCCCCCCCu);

    // Clone retains original value
    cloned.set_position(static_cast<int32_t>(data_start), areg::Cursor::SeekOrigin::Begin);
    uint32_t cloned_val {};
    cloned >> cloned_val;
    EXPECT_EQ(cloned_val, 0xAAAAAAAAu);
}

//////////////////////////////////////////////////////////////////////////
// 6. Copy / move semantics (inherited from SharedBuffer)
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Copy constructor shares the block; is_shared() is true on both.
 **/
TEST(RemoteMessageTest, copy_shares_block)
{
    areg::RemoteMessage msg;
    msg << uint32_t(0xBBBBu);
    msg.set_source(55u);

    areg::RemoteMessage copy(msg);
    EXPECT_TRUE(msg.is_shared());
    EXPECT_TRUE(copy.is_shared());
    EXPECT_EQ(copy.source(), ITEM_ID{ 55u });
    EXPECT_EQ(copy.position(), 0u);   // cursor reset to 0 on copy
}

/**
 * \brief   Move constructor transfers ownership; source is left invalid.
 **/
TEST(RemoteMessageTest, move_transfers_ownership)
{
    areg::RemoteMessage msg;
    msg << uint32_t(0xCCCCu);
    msg.set_source(66u);
    const uint32_t size_before = msg.size_used();

    areg::RemoteMessage moved(std::move(msg));
    EXPECT_EQ(moved.size_used(), size_before);
    EXPECT_EQ(moved.source(), ITEM_ID{ 66u });
    EXPECT_FALSE(msg.is_valid());
}

/**
 * \brief   is_shared() is false after move (the moved-from object is gone; moved-into has use_count=1).
 **/
TEST(RemoteMessageTest, is_shared_false_after_move)
{
    areg::RemoteMessage msg;
    msg << uint32_t(0x1u);

    areg::RemoteMessage moved(std::move(msg));
    EXPECT_FALSE(moved.is_shared());
}

//////////////////////////////////////////////////////////////////////////
// 7. Streaming the whole RemoteMessage
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   RemoteMessage can be serialised into a SharedBuffer and deserialised back.
 **/
TEST(RemoteMessageTest, serialize_deserialize_roundtrip)
{
    areg::RemoteMessage original(true);
    original.set_source(7u);
    original.set_target(8u);
    original.set_message_id(9u);
    original << uint32_t(0xFEEDu);
    original.buffer_completion_fix();

    // Serialise into a plain SharedBuffer
    areg::SharedBuffer stream;
    stream << original;

    // Deserialise back
    stream.reset();
    areg::RemoteMessage restored;
    stream >> restored;

    EXPECT_EQ(restored.source(),     ITEM_ID{ 7u });
    EXPECT_EQ(restored.target(),     ITEM_ID{ 8u });
    EXPECT_EQ(restored.message_id(), 9u);
    EXPECT_EQ(restored.size_used(),  original.size_used());
}


//////////////////////////////////////////////////////////////////////////
// 7. Checking position of RemoteMessage
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   In RemoteMessage the cursor can be moved to any position and the data can be manipulated.
 **/
TEST(RemoteMessageTest, check_cursor_position)
{
    areg::RemoteMessage src, dst;
    src.set_source(1u);
    src.set_target(2u);
    src.set_message_id(3u);


    uint32_t one{ 1u }, two{ 2u }, three{ 3 }, four{ 4 }, five{ 5 };
    uint32_t nums[2]{0};

    src << one;     // `1`
    dst = src;
    dst >> nums[0];
    EXPECT_EQ(one, nums[0]);

    uint32_t pos = src.position();
    src << two;     // `1`, `2`
    dst = src;
    dst >> nums[0] >> nums[1];
    EXPECT_EQ(one, nums[0]);
    EXPECT_EQ(two, nums[1]);

    src.set_position(static_cast<int>(pos), areg::Cursor::SeekOrigin::Begin);
    src << three;   // `1`, `3`
    dst = src;
    dst >> nums[0] >> nums[1];
    EXPECT_EQ(one, nums[0]);
    EXPECT_EQ(three, nums[1]);

    src.set_position(-1 * static_cast<int32_t>(sizeof(three)), areg::Cursor::SeekOrigin::Current);
    src << four;    // `1`, `4`
    dst = src;
    dst >> nums[0] >> nums[1];
    EXPECT_EQ(one, nums[0]);
    EXPECT_EQ(four, nums[1]);

    src.set_position(-2 * static_cast<int32_t>(sizeof(uint32_t)), areg::Cursor::SeekOrigin::End);
    src << five;    // `5`, `4`
    dst = src;
    dst >> nums[0] >> nums[1];
    EXPECT_EQ(five, nums[0]);
    EXPECT_EQ(four, nums[1]);
}

//////////////////////////////////////////////////////////////////////////
// 8. Simulated TCP receive — init_message + memcpy, no real socket
//////////////////////////////////////////////////////////////////////////

namespace {

    /**
     * \brief   Build a minimal areg::MessageHeader for an incoming message with
     *          the given payload size and addressing fields.
     *          biOffset and biLength are left at zero; init_message() ignores them.
     **/
    inline areg::MessageHeader make_recv_header(
            uint32_t payloadSize,
            ITEM_ID  src   = 10u,
            ITEM_ID  tgt   = 20u,
            uint32_t msgId = 99u) noexcept
    {
        areg::MessageHeader hdr{};
        hdr.rbhBufHeader.biUsed = payloadSize;
        hdr.rbhSource           = src;
        hdr.rbhTarget           = tgt;
        hdr.rbhMessageId        = msgId;
        hdr.rbhChecksum         = areg::CHECKSUM_INVALID;
        return hdr;
    }

} // namespace

/**
 * \brief   (a) Simulate receiving a single TCP packet: init_message() returns a
 *          writable payload pointer; memcpy fills it; the message is then readable.
 **/
TEST(SimTcpReceive, init_and_copy_payload)
{
    constexpr uint32_t kPayload = 16u;
    const uint8_t incoming[kPayload] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };

    areg::RemoteMessage msg;
    areg::MessageHeader hdr = make_recv_header(kPayload);

    uint8_t* dst = msg.init_message(hdr, kPayload);
    ASSERT_NE(dst, nullptr);

    std::memcpy(dst, incoming, kPayload);
    msg.set_size_used(kPayload);
    msg.move_to_begin();

    // Header fields come from the header we passed in
    EXPECT_EQ(msg.source(),     ITEM_ID{ 10u });
    EXPECT_EQ(msg.target(),     ITEM_ID{ 20u });
    EXPECT_EQ(msg.message_id(), 99u);
    EXPECT_EQ(msg.size_used(),  kPayload);

    // Payload round-trips correctly
    uint8_t readback[kPayload]{};
    EXPECT_EQ(msg.read(readback, kPayload), kPayload);
    EXPECT_EQ(std::memcmp(readback, incoming, kPayload), 0);
}

/**
 * \brief   (b) Re-init with a payload large enough to overflow the current buffer
 *          capacity must trigger invalidate() (slow path): the RawMessage pointer
 *          changes after the call.
 *
 *          Strategy: first init with a 4-byte payload to establish a small
 *          capacity; then re-init with a ≥129-byte payload that exceeds it.
 **/
TEST(SimTcpReceive, reinit_larger_triggers_slow_path)
{
    constexpr std::string_view _before{ "before" };
    constexpr std::string_view _after{ "before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_before_after_" };

    // First receive: 4-byte payload --> establishes a small buffer
    constexpr uint32_t kSmall = 4u;
    areg::RemoteMessage msg;
    uint8_t* dst = msg.init_message(make_recv_header(kSmall, 1u, 2u, 10u), kSmall);
    ASSERT_NE(dst, nullptr);
    msg.set_size_used(kSmall);

    // Capacity (biLength) of the just-allocated buffer
    const uint32_t capacity = msg.size_available();
    msg << _before;
    ASSERT_GT(capacity, 0u);

    const areg::RawMessage* ptr_before = msg.remote_message();
    ASSERT_NE(ptr_before, nullptr);

    msg.move_to_begin();
    // Second receive: payload > capacity, guaranteed ≥ 129 bytes
    const uint32_t kLarge = std::max(capacity + 1u, 256u);
    dst = msg.init_message(make_recv_header(kLarge, 3u, 4u, 20u), kLarge);
    ASSERT_NE(dst, nullptr);

    msg << _after;
    const areg::RawMessage* ptr_after = msg.remote_message();

    // Slow path: buffer was reallocated — pointer must differ
    EXPECT_NE(ptr_before, ptr_after);

    // New capacity must accommodate the large payload
    EXPECT_GE(msg.size_available(), kLarge);

    // Header fields from the second receive are active
    EXPECT_EQ(msg.source(),     ITEM_ID{ 3u });
    EXPECT_EQ(msg.target(),     ITEM_ID{ 4u });
    EXPECT_EQ(msg.message_id(), 20u);
}

/**
 * \brief   (c) Re-init with a payload that fits within the existing capacity must
 *          reuse the buffer in place (fast path): the RawMessage pointer is unchanged
 *          after the call.
 *
 *          Strategy: first init with a large payload to establish generous capacity;
 *          then re-init with a smaller payload that fits.
 **/
TEST(SimTcpReceive, reinit_smaller_uses_fast_path)
{
    // First receive: large payload --> establishes generous capacity
    constexpr uint32_t kFirst = 200u;
    areg::RemoteMessage msg;
    uint8_t* dst = msg.init_message(make_recv_header(kFirst, 1u, 2u, 10u), kFirst);
    ASSERT_NE(dst, nullptr);
    msg.set_size_used(kFirst);

    const uint32_t capacity_after_first = msg.size_available();
    ASSERT_GE(capacity_after_first, kFirst);

    // Second receive: smaller payload that fits in existing buffer
    const uint32_t kSecond = kFirst / 2u;

    const areg::RawMessage* ptr_before = msg.remote_message();
    ASSERT_NE(ptr_before, nullptr);

    dst = msg.init_message(make_recv_header(kSecond, 5u, 6u, 30u), kSecond);
    ASSERT_NE(dst, nullptr);

    const areg::RawMessage* ptr_after = msg.remote_message();

    // Fast path: same allocation reused — pointer must be identical
    EXPECT_EQ(ptr_before, ptr_after);

    // Capacity unchanged
    EXPECT_EQ(msg.size_available(), capacity_after_first);

    // Header fields from the second receive are active
    EXPECT_EQ(msg.source(),     ITEM_ID{ 5u });
    EXPECT_EQ(msg.target(),     ITEM_ID{ 6u });
    EXPECT_EQ(msg.message_id(), 30u);
}

/**
 * \brief   (d) Re-init on a shared buffer (use_count > 1) with a payload ≥ 129 bytes
 *          always takes the slow path regardless of capacity: the RawMessage pointer
 *          on the re-initialized object must change; the copy retains the old buffer.
 **/
TEST(SimTcpReceive, shared_buffer_reinit_larger_triggers_slow_path)
{
    // First receive: large payload to ensure the second init would normally be "small enough"
    constexpr uint32_t kFirst = 200u;
    areg::RemoteMessage msg;
    uint8_t* dst = msg.init_message(make_recv_header(kFirst, 1u, 2u, 10u), kFirst);
    ASSERT_NE(dst, nullptr);
    msg.set_size_used(kFirst);

    // Create a shared copy — use_count becomes 2
    areg::RemoteMessage copy(msg);
    ASSERT_TRUE(msg.is_shared());
    ASSERT_TRUE(copy.is_shared());

    const uint32_t capacity = msg.size_available();
    const uint32_t kLarge   = std::max(capacity + 1u, 129u);

    const areg::RawMessage* msg_ptr_before  = msg.remote_message();
    const areg::RawMessage* copy_ptr_before = copy.remote_message();
    EXPECT_EQ(msg_ptr_before, copy_ptr_before);  // same block

    // Re-init msg with large payload while shared --> must invalidate
    dst = msg.init_message(make_recv_header(kLarge, 7u, 8u, 40u), kLarge);
    ASSERT_NE(dst, nullptr);

    const areg::RawMessage* msg_ptr_after  = msg.remote_message();
    const areg::RawMessage* copy_ptr_after = copy.remote_message();

    // msg got a new buffer
    EXPECT_NE(msg_ptr_before,  msg_ptr_after);
    // copy still points at the original block
    EXPECT_EQ(copy_ptr_before, copy_ptr_after);
    // Neither is shared any more
    EXPECT_FALSE(msg.is_shared());
    EXPECT_FALSE(copy.is_shared());

    EXPECT_EQ(msg.source(),     ITEM_ID{ 7u });
    EXPECT_EQ(msg.target(),     ITEM_ID{ 8u });
    EXPECT_EQ(msg.message_id(), 40u);
}

/**
 * \brief   (e) Re-init on a shared buffer (use_count > 1) with a payload that would
 *          fit in the current capacity still takes the slow path (because use_count > 1).
 *          This contrasts with case (c) where sole ownership allows reuse.
 **/
TEST(SimTcpReceive, shared_buffer_reinit_smaller_still_triggers_slow_path)
{
    // First receive: large payload
    constexpr uint32_t kFirst = 200u;
    areg::RemoteMessage msg;
    uint8_t* dst = msg.init_message(make_recv_header(kFirst, 1u, 2u, 10u), kFirst);
    ASSERT_NE(dst, nullptr);
    msg.set_size_used(kFirst);

    // Share the buffer
    areg::RemoteMessage copy(msg);
    ASSERT_TRUE(msg.is_shared());

    const uint32_t capacity = msg.size_available();

    // Second payload is SMALLER than capacity — would be fast path if unshared
    const uint32_t kSecond = kFirst / 4u;
    ASSERT_LT(kSecond, capacity);

    const areg::RawMessage* ptr_before = msg.remote_message();

    // Re-init on shared buffer --> always slow path due to use_count > 1
    dst = msg.init_message(make_recv_header(kSecond, 9u, 11u, 50u), kSecond);
    ASSERT_NE(dst, nullptr);

    const areg::RawMessage* ptr_after = msg.remote_message();

    // Pointer must have changed even though payload was small
    EXPECT_NE(ptr_before, ptr_after);

    EXPECT_FALSE(msg.is_shared());
    EXPECT_FALSE(copy.is_shared());

    EXPECT_EQ(msg.source(),     ITEM_ID{ 9u });
    EXPECT_EQ(msg.target(),     ITEM_ID{ 11u });
    EXPECT_EQ(msg.message_id(), 50u);
}

/**
 * \brief   (f1) Complex flow: multiple sequential receives each with increasing
 *          payload size.  Verifies that after each grow-or-reuse cycle the payload
 *          is intact, no heap corruption occurs, and header fields are updated.
 **/
TEST(SimTcpReceive, complex_sequential_receives_growing_payload)
{
    areg::RemoteMessage msg;

    constexpr uint32_t kSizes[] = { 8u, 32u, 128u, 512u, 64u };
    constexpr uint32_t kSteps   = static_cast<uint32_t>(sizeof(kSizes) / sizeof(kSizes[0]));

    for (uint32_t i = 0u; i < kSteps; ++i)
    {
        const uint32_t payloadSize = kSizes[i];

        // Build synthetic payload: each byte = low byte of (i * 16 + byteIndex)
        std::vector<uint8_t> outgoing(payloadSize);
        for (uint32_t j = 0u; j < payloadSize; ++j)
        {
            outgoing[j] = static_cast<uint8_t>((i * 16u + j) & 0xFFu);
        }

        uint8_t* dst = msg.init_message(
                make_recv_header(payloadSize, ITEM_ID(i + 1u), ITEM_ID(i + 100u), i * 10u),
                payloadSize);
        ASSERT_NE(dst, nullptr) << "Step " << i;

        std::memcpy(dst, outgoing.data(), payloadSize);
        msg.set_size_used(payloadSize);
        msg.move_to_begin();

        EXPECT_EQ(msg.source(),     ITEM_ID(i + 1u));
        EXPECT_EQ(msg.target(),     ITEM_ID(i + 100u));
        EXPECT_EQ(msg.message_id(), i * 10u);
        EXPECT_EQ(msg.size_used(),  payloadSize);

        // Verify every byte of the payload
        std::vector<uint8_t> readback(payloadSize);
        EXPECT_EQ(msg.read(readback.data(), payloadSize), payloadSize) << "Step " << i;
        EXPECT_EQ(std::memcmp(readback.data(), outgoing.data(), payloadSize), 0) << "Step " << i;
    }
}

/**
 * \brief   (f2) Complex flow: receive --> share --> re-receive on the original.
 *          The shared copy retains the old payload while the original takes a
 *          fresh buffer via slow path.  After the operation both messages are
 *          independently readable with their respective data.
 **/
TEST(SimTcpReceive, complex_receive_share_rereceive)
{
    // First receive
    constexpr uint32_t kPayload1 = 32u;
    areg::RemoteMessage msg;
    uint8_t* dst = msg.init_message(make_recv_header(kPayload1, 1u, 2u, 11u), kPayload1);
    ASSERT_NE(dst, nullptr);
    std::memset(dst, 0xAA, kPayload1);
    msg.set_size_used(kPayload1);

    // Share the buffer; 'snap' holds the old receive
    areg::RemoteMessage snap(msg);
    ASSERT_TRUE(snap.is_shared());

    // Second receive on msg — payload > capacity forces slow path
    const uint32_t kPayload2 = msg.size_available() + 1u;
    dst = msg.init_message(make_recv_header(kPayload2, 3u, 4u, 22u), kPayload2);
    ASSERT_NE(dst, nullptr);
    std::memset(dst, 0xBB, kPayload2);
    msg.set_size_used(kPayload2);

    // msg has new data
    EXPECT_EQ(msg.source(),     ITEM_ID{ 3u });
    EXPECT_EQ(msg.target(),     ITEM_ID{ 4u });
    EXPECT_EQ(msg.message_id(), 22u);
    EXPECT_EQ(msg.size_used(),  kPayload2);
    EXPECT_FALSE(msg.is_shared());

    msg.move_to_begin();
    std::vector<uint8_t> rb2(kPayload2);
    ASSERT_EQ(msg.read(rb2.data(), kPayload2), kPayload2);
    for (uint32_t i = 0u; i < kPayload2; ++i)
        EXPECT_EQ(rb2[i], uint8_t{0xBBu}) << "index " << i;

    // snap still holds the first receive, unmodified
    EXPECT_EQ(snap.source(),     ITEM_ID{ 1u });
    EXPECT_EQ(snap.target(),     ITEM_ID{ 2u });
    EXPECT_EQ(snap.message_id(), 11u);
    EXPECT_EQ(snap.size_used(),  kPayload1);
    EXPECT_FALSE(snap.is_shared());

    snap.move_to_begin();
    std::vector<uint8_t> rb1(kPayload1);
    ASSERT_EQ(snap.read(rb1.data(), kPayload1), kPayload1);
    for (uint32_t i = 0u; i < kPayload1; ++i)
        EXPECT_EQ(rb1[i], uint8_t{0xAAu}) << "index " << i;
}

/**
 * \brief   Verify the MessageHeader constructor that accepts a BufferHeader as its first
 *          argument, enabling brace-init with a nested BufferHeader sub-aggregate.
 *          Uses init_message() to simulate TCP receive and verifies all header fields
 *          and payload round-trip through the buffer.
 **/
TEST(RemoteMessageTest, aggregate_header_init_message)
{
    constexpr std::string_view _data{ "aggregate_header_construction_test_payload_data_for_areg_sdk_unit_test_123456789" };
    constexpr uint32_t payload_size = static_cast<uint32_t>(_data.length() + 1u);

    // Brace-init using MessageHeader(const BufferHeader&, target, checksum, source, msgId, result, seq)
    constexpr areg::MessageHeader hdr1
    {
        {
              payload_size
            , static_cast<uint32_t>(sizeof(areg::MessageHeader))
            , areg::BufferType::Remote
            , payload_size
        }
        , static_cast<ITEM_ID>(1u)          // target
        , areg::CHECKSUM_IGNORE             // checksum
        , static_cast<ITEM_ID>(2u)          // source
        , static_cast<uint32_t>(3u)         // msgId
        , static_cast<uint32_t>(4u)         // result
        , static_cast<SequenceNumber>(5u)   // seq
    };

    // Verify compile-time values
    static_assert(hdr1.rbhTarget     == static_cast<ITEM_ID>(1u),      "target mismatch");
    static_assert(hdr1.rbhSource     == static_cast<ITEM_ID>(2u),      "source mismatch");
    static_assert(hdr1.rbhMessageId  == 3u,                             "msgId mismatch");
    static_assert(hdr1.rbhResult     == 4u,                             "result mismatch");
    static_assert(hdr1.rbhSequenceNr == SequenceNumber{ 5u },           "seq mismatch");
    static_assert(hdr1.rbhChecksum   == areg::CHECKSUM_IGNORE,          "checksum mismatch");

    // Simulate TCP receive: init_message allocates buffer, then payload is memcpy'd in
    areg::RemoteMessage msg1;
    uint8_t* buf = msg1.init_message(hdr1);
    ASSERT_NE(buf, nullptr);

    areg::mem_copy(buf, payload_size, _data.data(), static_cast<uint32_t>(_data.size() + 1u));
    msg1.set_size_used(hdr1.rbhBufHeader.biUsed);
    msg1.move_to_begin();

    EXPECT_TRUE(msg1.is_valid());
    EXPECT_EQ(msg1.target(),     ITEM_ID{ 1u });
    EXPECT_EQ(msg1.source(),     ITEM_ID{ 2u });
    EXPECT_EQ(msg1.message_id(), 3u);
    EXPECT_EQ(msg1.result(),     4u);
    EXPECT_EQ(msg1.sequence(),   SequenceNumber{ 5u });
    EXPECT_EQ(msg1.size_used(),  payload_size);

    std::vector<char> readback(payload_size, '\0');
    ASSERT_EQ(msg1.read(reinterpret_cast<uint8_t*>(readback.data()), payload_size), payload_size);
    EXPECT_STREQ(readback.data(), _data.data());
}