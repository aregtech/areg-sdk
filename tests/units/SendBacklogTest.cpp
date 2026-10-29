/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SendBacklogTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the backlog wait of a service send thread.
 *              While a peer reads nothing the thread sleeps; it moves on as soon as the
 *              peer reads, a message is queued or an exit is requested. A connection above
 *              its cap is held without stopping the thread.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/component/private/EventQueue.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "aregextend/service/private/SendBacklog.hpp"
#include "aregextend/service/private/ServiceThreadHelper.hpp"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace
{
    using Clock = std::chrono::steady_clock;
    using areg::ext::SendBacklog;

    //!< The connection of the test.
    constexpr ITEM_ID   COOKIE          { 7u };
    //!< Bytes of one message payload.
    constexpr uint32_t  MESSAGE_BYTES   { 1024u * 1024u };
    //!< Messages written at once; more than both socket buffers hold.
    constexpr uint32_t  MESSAGE_COUNT   { 64u };
    //!< Milliseconds the reader stays paused.
    constexpr uint32_t  PAUSE_MS        { 500u };
    //!< Milliseconds within which a waiting send thread must react to a message or an exit.
    constexpr int64_t   PROMPT_MS       { 50 };
    //!< Milliseconds within which the first bytes leave after the peer reads again; the
    //!< liveness tick of the test is 1 s.
    constexpr int64_t   RESUME_MS       { 250 };

    int64_t elapsed_ms(Clock::time_point since)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - since).count();
    }

    //!< A connected TCP loopback pair; 'server' is the accepted end the backlog writes.
    struct Pair
    {
        SOCKETHANDLE    client{ areg::InvalidSocketHandle };
        SOCKETHANDLE    server{ areg::InvalidSocketHandle };

        bool open()
        {
            for (uint16_t port = 41'731u; port < 41'831u; ++port)
            {
                const SOCKETHANDLE listener{ areg::server_connect(areg::String("127.0.0.1"), port) };
                if (areg::is_valid_socket(listener) == false)
                    continue;

                if (areg::server_listen(listener))
                {
                    // A receive buffer fixed before connect keeps the window small: the peer
                    // takes nothing while it does not read.
                    areg::SocketAddress address{ };
                    client = address.resolve_address(areg::String("127.0.0.1"), port, false) ? areg::socket_create() : areg::InvalidSocketHandle;
                    if (areg::is_valid_socket(client))
                    {
                        static_cast<void>(areg::set_recv_size(client, 64u * 1024u));
                        if (areg::client_connect_fd(client, address) == false)
                        {
                            areg::socket_close(client);
                            client = areg::InvalidSocketHandle;
                        }
                    }

                    const SOCKETHANDLE none{ areg::InvalidSocketHandle };
                    server = areg::is_valid_socket(client) ? areg::server_accept(listener, &none, 0) : areg::InvalidSocketHandle;
                }

                areg::socket_close(listener);
                if (areg::is_valid_socket(server))
                {
                    static_cast<void>(areg::set_send_size(server, 64u * 1024u));
                    return true;
                }

                close();
            }

            return false;
        }

        void close()
        {
            if (areg::is_valid_socket(client))
                areg::socket_close(client);
            if (areg::is_valid_socket(server))
                areg::socket_close(server);

            client = areg::InvalidSocketHandle;
            server = areg::InvalidSocketHandle;
        }
    };

    //!< The owner of the backlog: counts the pumps and the bytes that left.
    struct Owner final : public SendBacklog::Owner
    {
        SOCKETHANDLE                socket  { areg::InvalidSocketHandle };
        std::atomic<uint32_t>       pumps   { 0u };
        std::atomic<uint64_t>       sent    { 0u };
        std::atomic<bool>           closed  { false };
        SendBacklog::Reason         reason  { SendBacklog::Reason::SocketError };
        uint64_t                    kept    { 0u };

        SOCKETHANDLE backlog_socket(ITEM_ID /*cookie*/) override
        {
            pumps.fetch_add(1u, std::memory_order_relaxed);
            return socket;
        }

        void backlog_sent(uint64_t bytes, uint32_t /*msgs*/) override
        {
            sent.fetch_add(bytes, std::memory_order_relaxed);
        }

        void backlog_close(const SendBacklog::Entry & entry, SendBacklog::Reason why) override
        {
            reason  = why;
            kept    = entry.bytes;
            closed.store(true, std::memory_order_relaxed);
        }
    };

    //!< Counts the releases it is told of.
    struct Waiter final : public areg::ext::HoldWaiter
    {
        std::atomic<uint32_t>   calls   { 0u };

        void hold_released() noexcept override
        {
            calls.fetch_add(1u, std::memory_order_relaxed);
        }
    };

    //!< The part of a send thread serve_backlog() uses, around a real event queue.
    struct Sender
    {
        areg::EventQueue    queue;
        SendBacklog &       backlog;

        explicit Sender(SendBacklog & backlog_)
            : queue     ( 0u )
            , backlog   ( backlog_ )
        {
            queue.acquire_lanes();
        }

        bool has_queued_events() const noexcept
        {
            return queue.has_pending();
        }

        bool is_exit_requested() const noexcept
        {
            return queue.is_exit_triggered();
        }

        void wait_backlog(uint32_t timeoutMs)
        {
            backlog.reset_wake();
            if (queue.arm_waiter(backlog, true))
            {
                backlog.wait(timeoutMs);
            }

            queue.disarm_waiter();
        }
    };

    //!< Builds the messages the backlog writes.
    std::vector<areg::MessageEnvelope> make_messages()
    {
        std::vector<areg::MessageEnvelope> result(MESSAGE_COUNT);
        for (areg::MessageEnvelope & message : result)
        {
            uint8_t * data{ message.init_envelope(areg::notify_client_connection(), MESSAGE_BYTES) };
            if (data != nullptr)
            {
                message.header()->bufHeader.biUsed = MESSAGE_BYTES;
            }
        }

        return result;
    }

    //!< The bytes of the messages on the wire.
    uint64_t wire_bytes(const std::vector<areg::MessageEnvelope> & messages)
    {
        uint64_t result{ 0u };
        for (const areg::MessageEnvelope & message : messages)
        {
            result += areg::wire_size(*message.header());
        }

        return result;
    }

    //!< Writes the messages into the backlog as a send thread does. Returns the bytes kept.
    uint64_t write_all(SendBacklog & backlog, const Pair & pair, const std::vector<areg::MessageEnvelope> & messages)
    {
        std::vector<const areg::MessageEnvelope *> pointers;
        for (const areg::MessageEnvelope & message : messages)
        {
            pointers.push_back(&message);
        }

        uint64_t written{ 0u };
        areg::SocketWriteGuard guard{ pair.server };
        for (uint32_t i = 0u; i < MESSAGE_COUNT; i += areg::DEFAULT_DRAIN_LIMIT)
        {
            const uint32_t count{ std::min(areg::DEFAULT_DRAIN_LIMIT, MESSAGE_COUNT - i) };
            uint64_t bytesDone{ 0u };
            uint32_t msgsDone{ 0u };
            if (backlog.write(COOKIE, pair.server, pointers.data() + i, count, bytesDone, msgsDone) == SendBacklog::Result::Failed)
                return 0u;

            written += bytesDone;
        }

        return wire_bytes(messages) - written;
    }

    //!< Writes the messages [first, first + count) into the backlog, and the bytes that left into
    //!< \a moved if given. Returns false on a failure.
    bool write_range(SendBacklog & backlog, const Pair & pair, const std::vector<areg::MessageEnvelope> & messages, uint32_t first, uint32_t count, uint64_t * moved = nullptr)
    {
        std::vector<const areg::MessageEnvelope *> pointers;
        for (uint32_t i = first; i < first + count; ++i)
        {
            pointers.push_back(&messages[i]);
        }

        uint64_t bytesDone{ 0u };
        uint32_t msgsDone{ 0u };
        areg::SocketWriteGuard guard{ pair.server };
        const bool result{ backlog.write(COOKIE, pair.server, pointers.data(), count, bytesDone, msgsDone) != SendBacklog::Result::Failed };
        if (moved != nullptr)
        {
            *moved = bytesDone;
        }

        return result;
    }

    //!< Reads \a total bytes from the client end.
    bool read_all(const Pair & pair, uint64_t total)
    {
        std::vector<uint8_t> buffer(64u * 1024u);
        while (total != 0u)
        {
            const uint32_t length{ static_cast<uint32_t>(std::min<uint64_t>(total, buffer.size())) };
            if (areg::receive_data_window(pair.client, buffer.data(), length) != static_cast<int32_t>(length))
                return false;

            total -= length;
        }

        return true;
    }

    //!< One send thread with a backlog of a paused peer.
    struct Fixture
    {
        Pair                                pair    { };
        areg::SendQueueGate                 gate    { };
        SendBacklog                         backlog { gate };
        Owner                               owner   { };
        Sender                              sender  { backlog };
        std::vector<areg::MessageEnvelope>  messages{ make_messages() };
        uint64_t                            kept    { 0u };

        bool start()
        {
            if (pair.open() == false)
                return false;

            // A large cap keeps the backlog below it; no refusal limit, a long keepalive.
            backlog.set_limits(1024u * 1024u * 1024u, SendBacklog::UNLIMITED, 60'000u);
            owner.socket = pair.server;
            static_cast<void>(backlog.attach(pair.server));
            kept = write_all(backlog, pair, messages);
            return (kept != 0u);
        }

        ~Fixture()
        {
            std::deque<ITEM_ID> cut;
            backlog.release_all(cut);
            pair.close();
        }
    };
}

// A paused peer: the waiting send thread does not pump, and it moves on once the peer reads.
TEST(SendBacklogTest, wait_sleeps_while_paused_and_resumes_when_read)
{
    Fixture test;
    ASSERT_TRUE(test.start());
    ASSERT_FALSE(test.backlog.is_empty());

    std::atomic<bool> done{ false };
    std::thread sendThread([&]
    {
        areg::ext::serve_backlog(test.sender, test.backlog, test.owner);
        done.store(true, std::memory_order_release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    const uint32_t pumpsBefore{ test.owner.pumps.load() };
    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_MS));
    const uint32_t pausedPumps{ test.owner.pumps.load() - pumpsBefore };
    const uint64_t sentBefore{ test.owner.sent.load() };

    // The reader resumes: the first bytes of the backlog leave promptly.
    const Clock::time_point resumed{ Clock::now() };
    int64_t firstProgressMs{ -1 };
    std::thread reader([&]
    {
        EXPECT_TRUE(read_all(test.pair, wire_bytes(test.messages)));
    });

    while ((elapsed_ms(resumed) < 2'000) && (firstProgressMs < 0))
    {
        if (test.owner.sent.load() != sentBefore)
        {
            firstProgressMs = elapsed_ms(resumed);
        }
        else
        {
            std::this_thread::yield();
        }
    }

    reader.join();
    const Clock::time_point readEnd{ Clock::now() };
    while ((done.load(std::memory_order_acquire) == false) && (elapsed_ms(readEnd) < 2'000))
    {
        std::this_thread::yield();
    }

    test.sender.queue.exit_queue(true);
    sendThread.join();

    EXPECT_LE(pausedPumps, 2u);
    EXPECT_GE(firstProgressMs, 0);
    EXPECT_LE(firstProgressMs, RESUME_MS);
    EXPECT_TRUE(test.backlog.is_empty());
    EXPECT_FALSE(test.owner.closed.load());
}

// A message queued while the thread waits for a paused peer ends the wait at once.
TEST(SendBacklogTest, queued_message_ends_the_wait)
{
    Fixture test;
    ASSERT_TRUE(test.start());

    std::atomic<int64_t> returnedMs{ -1 };
    Clock::time_point posted{ };
    std::thread sendThread([&]
    {
        areg::ext::serve_backlog(test.sender, test.backlog, test.owner);
        returnedMs.store(elapsed_ms(posted), std::memory_order_release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_MS));
    areg::MessageEnvelope wake;
    ASSERT_NE(wake.init_envelope(areg::notify_client_connection(), 0u), nullptr);
    areg::Event evt(std::move(wake));
    posted = Clock::now();
    ASSERT_TRUE(test.sender.queue.push_event(evt));
    sendThread.join();

    EXPECT_GE(returnedMs.load(), 0);
    EXPECT_LE(returnedMs.load(), PROMPT_MS);
    EXPECT_FALSE(test.backlog.is_empty());
}

// An exit requested while the thread waits for a paused peer ends the wait at once.
TEST(SendBacklogTest, exit_ends_the_wait)
{
    Fixture test;
    ASSERT_TRUE(test.start());

    std::atomic<int64_t> returnedMs{ -1 };
    Clock::time_point requested{ };
    std::thread sendThread([&]
    {
        areg::ext::serve_backlog(test.sender, test.backlog, test.owner);
        returnedMs.store(elapsed_ms(requested), std::memory_order_release);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_MS));
    requested = Clock::now();
    test.sender.queue.exit_queue(true);
    sendThread.join();

    EXPECT_GE(returnedMs.load(), 0);
    EXPECT_LE(returnedMs.load(), PROMPT_MS);
}

// A connection above its cap is held, stays held down to half of it, and is released below that.
// The waiter registered while it is held is called once, at the release.
TEST(SendBacklogTest, connection_is_held_above_cap_until_half_drained)
{
    constexpr uint64_t CAP{ 4u * MESSAGE_BYTES };
    Pair pair{ };
    ASSERT_TRUE(pair.open());
    areg::SendQueueGate gate{ };
    SendBacklog backlog{ gate };
    Owner owner{ };
    owner.socket = pair.server;
    std::atomic<uint32_t> heldTotal{ 0u };
    backlog.set_hold_counter(&heldTotal);
    backlog.set_limits(CAP, SendBacklog::UNLIMITED, 60'000u);
    static_cast<void>(backlog.attach(pair.server));
    std::vector<areg::MessageEnvelope> messages{ make_messages() };
    Waiter waiter{ };

    EXPECT_FALSE(backlog.is_held(COOKIE));
    EXPECT_FALSE(backlog.hold_wait(COOKIE, waiter));

    uint64_t moved{ 0u };
    ASSERT_TRUE(write_range(backlog, pair, messages, 0u, 8u, &moved));
    const uint64_t written{ 8u * (areg::WIRE_HEADER_SIZE + MESSAGE_BYTES) };
    ASSERT_GT(written - moved, CAP);
    EXPECT_TRUE(backlog.is_held(COOKIE));
    EXPECT_FALSE(backlog.is_held(COOKIE + 1u));
    EXPECT_EQ(heldTotal.load(), 1u);
    EXPECT_TRUE(backlog.hold_wait(COOKIE, waiter));

    // Drain in steps: held while above half the cap, released once at or below it.
    // The peer reads at most one buffer per step, so no step drains the cap at once.
    bool sawHeldBelowCap{ false };
    uint64_t left{ written - moved };
    uint64_t readBytes{ 0u };
    std::vector<uint8_t> buffer(64u * 1024u);

    const Clock::time_point started{ Clock::now() };
    while ((backlog.is_empty() == false) && (elapsed_ms(started) < 10'000))
    {
        const int32_t received{ areg::receive_data_nowait(pair.client, buffer.data(), static_cast<uint32_t>(buffer.size())) };
        ASSERT_GE(received, 0);
        readBytes += static_cast<uint64_t>(received);
        static_cast<void>(backlog.pump(owner));
        left = written - moved - owner.sent.load();
        const bool held{ backlog.is_held(COOKIE) };
        if ((left <= CAP) && (left > CAP / 2u))
        {
            sawHeldBelowCap = sawHeldBelowCap || held;
            EXPECT_TRUE(held) << "left " << left;
        }
        else if (left <= CAP / 2u)
        {
            EXPECT_FALSE(held) << "left " << left;
        }

        std::this_thread::yield();
    }

    EXPECT_TRUE(read_all(pair, written - readBytes));
    EXPECT_TRUE(backlog.is_empty());
    EXPECT_TRUE(sawHeldBelowCap);
    EXPECT_FALSE(backlog.is_held(COOKIE));
    EXPECT_EQ(waiter.calls.load(), 1u);
    EXPECT_EQ(heldTotal.load(), 0u);
    EXPECT_FALSE(owner.closed.load());

    std::deque<ITEM_ID> cut;
    backlog.release_all(cut);
    pair.close();
}

// A held connection that goes away releases its waiter, and a cancelled waiter is not called.
TEST(SendBacklogTest, held_connection_releases_its_waiter_when_it_goes_away)
{
    constexpr uint64_t CAP{ 2u * MESSAGE_BYTES };
    Fixture test;
    ASSERT_TRUE(test.pair.open());
    test.backlog.set_limits(CAP, SendBacklog::UNLIMITED, 60'000u);
    test.owner.socket = test.pair.server;
    static_cast<void>(test.backlog.attach(test.pair.server));
    ASSERT_TRUE(write_range(test.backlog, test.pair, test.messages, 0u, 8u));
    ASSERT_TRUE(test.backlog.is_held(COOKIE));

    Waiter kept{ };
    Waiter cancelled{ };
    EXPECT_TRUE(test.backlog.hold_wait(COOKIE, kept));
    EXPECT_TRUE(test.backlog.hold_wait(COOKIE, cancelled));
    test.backlog.hold_cancel(cancelled);

    // The connection is closed by someone else: the next pump drops its backlog.
    test.owner.socket = areg::InvalidSocketHandle;
    static_cast<void>(test.backlog.pump(test.owner));

    EXPECT_TRUE(test.backlog.is_empty());
    EXPECT_FALSE(test.backlog.is_held(COOKIE));
    EXPECT_EQ(kept.calls.load(), 1u);
    EXPECT_EQ(cancelled.calls.load(), 0u);
}

// A held connection does not stop its send thread: a queued message ends the backlog service at once.
TEST(SendBacklogTest, held_connection_does_not_stop_the_send_thread)
{
    Fixture test;
    ASSERT_TRUE(test.pair.open());
    test.backlog.set_limits(MESSAGE_BYTES, SendBacklog::UNLIMITED, 60'000u);
    test.owner.socket = test.pair.server;
    static_cast<void>(test.backlog.attach(test.pair.server));
    ASSERT_TRUE(write_range(test.backlog, test.pair, test.messages, 0u, 8u));
    ASSERT_TRUE(test.backlog.is_held(COOKIE));

    areg::MessageEnvelope wake;
    ASSERT_NE(wake.init_envelope(areg::notify_client_connection(), 0u), nullptr);
    areg::Event evt(std::move(wake));
    ASSERT_TRUE(test.sender.queue.push_event(evt));

    std::atomic<int64_t> returnedMs{ -1 };
    const Clock::time_point started{ Clock::now() };
    std::thread sendThread([&]
    {
        areg::ext::serve_backlog(test.sender, test.backlog, test.owner);
        returnedMs.store(elapsed_ms(started), std::memory_order_release);
    });

    const Clock::time_point waited{ Clock::now() };
    while ((returnedMs.load(std::memory_order_acquire) < 0) && (elapsed_ms(waited) < 3'000))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    if (returnedMs.load(std::memory_order_acquire) < 0)
    {
        test.sender.queue.exit_queue(true);
    }

    sendThread.join();
    EXPECT_GE(returnedMs.load(), 0);
    EXPECT_LE(returnedMs.load(), PROMPT_MS);
    EXPECT_TRUE(test.backlog.is_held(COOKIE));
}

// A peer above its cap that takes nothing is closed after the stall limit when stalled peers are
// closed, and kept, still held, when they are not.
TEST(SendBacklogTest, stalled_peer_above_cap_is_closed_only_if_stalled_peers_are_closed)
{
    constexpr uint64_t CAP{ 2u * MESSAGE_BYTES };
    for (const bool closeStalled : { true, false })
    {
        Pair pair{ };
        ASSERT_TRUE(pair.open());
        areg::SendQueueGate gate{ };
        SendBacklog backlog{ gate };
        Owner owner{ };
        owner.socket = pair.server;
        backlog.set_limits(CAP, SendBacklog::UNLIMITED, 60'000u, closeStalled);
        static_cast<void>(backlog.attach(pair.server));
        std::vector<areg::MessageEnvelope> messages{ make_messages() };
        ASSERT_TRUE(write_range(backlog, pair, messages, 0u, 8u));
        ASSERT_TRUE(backlog.is_held(COOKIE));

        const Clock::time_point started{ Clock::now() };
        while ((owner.closed.load() == false) && (elapsed_ms(started) < static_cast<int64_t>(SendBacklog::CAP_STALL_MS) + 1'500))
        {
            static_cast<void>(backlog.pump(owner));
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        if (closeStalled)
        {
            EXPECT_TRUE(owner.closed.load());
            EXPECT_EQ(owner.reason, SendBacklog::Reason::OverCap);
            EXPECT_TRUE(backlog.is_empty());
            EXPECT_FALSE(backlog.is_held(COOKIE));
        }
        else
        {
            EXPECT_FALSE(owner.closed.load());
            EXPECT_TRUE(backlog.is_held(COOKIE));
        }

        std::deque<ITEM_ID> cut;
        backlog.release_all(cut);
        EXPECT_FALSE(backlog.is_held(COOKIE));
        pair.close();
    }
}

// A reading peer is seen to take data by pump() alone and is not closed, however far one batch
// is past the cap.
TEST(SendBacklogTest, reading_peer_is_kept_past_twice_its_cap)
{
    constexpr uint64_t CAP{ MESSAGE_BYTES };
    Pair pair{ };
    ASSERT_TRUE(pair.open());
    areg::SendQueueGate gate{ };
    SendBacklog backlog{ gate };
    Owner owner{ };
    owner.socket = pair.server;
    backlog.set_limits(CAP, SendBacklog::UNLIMITED, 60'000u);
    static_cast<void>(backlog.attach(pair.server));
    std::vector<areg::MessageEnvelope> messages{ make_messages() };

    ASSERT_TRUE(write_range(backlog, pair, messages, 0u, MESSAGE_COUNT));
    static_cast<void>(backlog.pump(owner));
    EXPECT_FALSE(owner.closed.load());

    std::thread reader([&]
    {
        EXPECT_TRUE(read_all(pair, wire_bytes(messages)));
    });

    const Clock::time_point started{ Clock::now() };
    while ((backlog.is_empty() == false) && (owner.closed.load() == false) && (elapsed_ms(started) < 10'000))
    {
        static_cast<void>(backlog.pump(owner));
        std::this_thread::yield();
    }

    if (owner.closed.load())
    {
        pair.close();
    }

    reader.join();
    EXPECT_FALSE(owner.closed.load()) << "reason " << static_cast<int>(owner.reason) << ", sent " << owner.sent.load();
    EXPECT_TRUE(backlog.is_empty());

    std::deque<ITEM_ID> cut;
    backlog.release_all(cut);
    pair.close();
}
