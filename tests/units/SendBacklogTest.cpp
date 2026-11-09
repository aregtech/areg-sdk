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
 *              While a peer reads nothing the thread sleeps; it moves on once the
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
#include "areg/base/SocketMultiplexer.hpp"
#include "aregextend/service/private/PausedSources.hpp"
#include "aregextend/service/private/SendBacklog.hpp"
#include "aregextend/service/private/ServiceThreadHelper.hpp"
#include "units/LogCapture.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <future>
#include <string>
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
    //!< Milliseconds the test waits for an outcome before it fails.
    constexpr int64_t   DEADLINE_MS     { 60'000 };

    int64_t elapsed_ms(Clock::time_point since)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - since).count();
    }

    //!< Returns true if 'returned' is signaled before the deadline.
    bool returns(const std::future<void> & returned)
    {
        return (returned.wait_for(std::chrono::milliseconds(DEADLINE_MS)) == std::future_status::ready);
    }

    //!< Returns true if 'returned' is not signaled yet.
    bool still_waits(const std::future<void> & returned)
    {
        return (returned.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready);
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

    //!< Pumps until the stall limit, counted from the last bytes that left, has passed.
    void pump_past_stall_limit(SendBacklog & backlog, Owner & owner)
    {
        Clock::time_point progressed{ Clock::now() };
        uint64_t sent{ owner.sent.load() };
        while ((owner.closed.load() == false) && (elapsed_ms(progressed) <= static_cast<int64_t>(SendBacklog::CAP_STALL_MS)))
        {
            static_cast<void>(backlog.pump(owner));
            if (owner.sent.load() != sent)
            {
                sent = owner.sent.load();
                progressed = Clock::now();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        if (owner.closed.load() == false)
        {
            static_cast<void>(backlog.pump(owner));
        }
    }

    //!< Counts the releases it is told of.
    struct Waiter final : public areg::ext::HoldWaiter
    {
        std::atomic<uint32_t>   calls   { 0u };

        void hold_released() noexcept override
        {
            calls.fetch_add(1u, std::memory_order_relaxed);
        }
    };

    //!< The part of a send thread serve_backlog() uses, around a real event queue. Its wait has
    //!< no timeout, so only a writable socket, a queued event or an exit ends it.
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

        void wait_backlog(uint32_t /*timeoutMs*/)
        {
            backlog.reset_wake();
            if (queue.arm_waiter(backlog, true))
            {
                backlog.wait(areg::WAIT_INFINITE);
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

    std::promise<void> done;
    const std::future<void> returned{ done.get_future() };
    std::thread sendThread([&]
    {
        areg::ext::serve_backlog(test.sender, test.backlog, test.owner);
        done.set_value();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    const uint32_t pumpsBefore{ test.owner.pumps.load() };
    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_MS));
    const uint32_t pausedPumps{ test.owner.pumps.load() - pumpsBefore };
    const bool waitedWhilePaused{ still_waits(returned) };

    // The reader resumes: only the socket taking data can end the wait, so the backlog drains.
    std::thread reader([&]
    {
        EXPECT_TRUE(read_all(test.pair, wire_bytes(test.messages)));
    });

    const bool drained{ returns(returned) };
    if (drained == false)
    {
        test.sender.queue.exit_queue(true);
        test.pair.close();
    }

    reader.join();
    test.sender.queue.exit_queue(true);
    sendThread.join();

    EXPECT_LE(pausedPumps, 2u);
    EXPECT_TRUE(waitedWhilePaused);
    EXPECT_TRUE(drained);
    EXPECT_TRUE(test.backlog.is_empty());
    EXPECT_FALSE(test.owner.closed.load());
}

// A message queued while the thread waits for a paused peer ends the wait.
TEST(SendBacklogTest, queued_message_ends_the_wait)
{
    Fixture test;
    ASSERT_TRUE(test.start());

    std::promise<void> done;
    const std::future<void> returned{ done.get_future() };
    std::thread sendThread([&]
    {
        areg::ext::serve_backlog(test.sender, test.backlog, test.owner);
        done.set_value();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_MS));
    const bool waitedBefore{ still_waits(returned) };
    areg::MessageEnvelope wake;
    ASSERT_NE(wake.init_envelope(areg::notify_client_connection(), 0u), nullptr);
    areg::Event evt(std::move(wake));
    ASSERT_TRUE(test.sender.queue.push_event(evt));
    const bool ended{ returns(returned) };
    if (ended == false)
    {
        test.sender.queue.exit_queue(true);
    }

    sendThread.join();

    EXPECT_TRUE(waitedBefore);
    EXPECT_TRUE(ended);
    EXPECT_FALSE(test.backlog.is_empty());
}

// An exit requested while the thread waits for a paused peer ends the wait.
TEST(SendBacklogTest, exit_ends_the_wait)
{
    Fixture test;
    ASSERT_TRUE(test.start());

    std::promise<void> done;
    const std::future<void> returned{ done.get_future() };
    std::thread sendThread([&]
    {
        areg::ext::serve_backlog(test.sender, test.backlog, test.owner);
        done.set_value();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(PAUSE_MS));
    const bool waitedBefore{ still_waits(returned) };
    test.sender.queue.exit_queue(true);
    const bool ended{ returns(returned) };
    if (ended == false)
    {
        test.pair.close();
    }

    sendThread.join();

    EXPECT_TRUE(waitedBefore);
    EXPECT_TRUE(ended);
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
    while ((backlog.is_empty() == false) && (elapsed_ms(started) < DEADLINE_MS))
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

// A held connection does not stop its send thread: a queued message ends the backlog service.
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

    std::promise<void> done;
    const std::future<void> returned{ done.get_future() };
    std::thread sendThread([&]
    {
        areg::ext::serve_backlog(test.sender, test.backlog, test.owner);
        done.set_value();
    });

    const bool ended{ returns(returned) };
    if (ended == false)
    {
        test.sender.queue.exit_queue(true);
    }

    sendThread.join();
    EXPECT_TRUE(ended);
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

        pump_past_stall_limit(backlog, owner);
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

TEST(SendBacklogTest, every_close_reason_has_a_name)
{
    EXPECT_STREQ(SendBacklog::reason_name(SendBacklog::Reason::SocketError), "SocketError");
    EXPECT_STREQ(SendBacklog::reason_name(SendBacklog::Reason::Refused), "Refused");
    EXPECT_STREQ(SendBacklog::reason_name(SendBacklog::Reason::OverCap), "OverCap");
    EXPECT_STREQ(SendBacklog::reason_name(SendBacklog::Reason::Unreachable), "Unreachable");
    EXPECT_STREQ(SendBacklog::reason_name(static_cast<SendBacklog::Reason>(0xFFu)), "Unknown");
}

#if AREG_LOGGING
// Reproduces the defect: the close of a stalled connection left no trace, so a run could not
// tell the reason of a close, the bytes left or how long the peer took nothing.
TEST(SendBacklogTest, close_of_a_stalled_peer_is_logged_with_its_reason_bytes_and_stall)
{
    log_capture::LogCapture capture{ };
    ASSERT_TRUE(capture.start("areg_aregextend_service_SendBacklog.report_close", static_cast<uint32_t>(areg::LogPriority::PrioWarning)));

    constexpr uint64_t CAP{ 2u * MESSAGE_BYTES };
    Pair pair{ };
    ASSERT_TRUE(pair.open());
    areg::SendQueueGate gate{ };
    SendBacklog backlog{ gate };
    Owner owner{ };
    owner.socket = pair.server;
    backlog.set_limits(CAP, SendBacklog::UNLIMITED, 60'000u, true);
    static_cast<void>(backlog.attach(pair.server));
    std::vector<areg::MessageEnvelope> messages{ make_messages() };
    ASSERT_TRUE(write_range(backlog, pair, messages, 0u, 8u));
    pump_past_stall_limit(backlog, owner);
    ASSERT_TRUE(owner.closed.load());
    pair.close();

    const std::string text{ capture.stop() };
    EXPECT_EQ(log_capture::LogCapture::count(text, "row:Closing connection"), 1u) << text;
    const std::size_t row{ text.find("row:Closing connection") };
    ASSERT_NE(row, std::string::npos) << text;

    // Returns the text between the brackets number index of the row.
    const auto field = [&text, row](uint32_t index) -> std::string
    {
        std::size_t begin{ row };
        for (uint32_t i = 0u; (i <= index) && (begin != std::string::npos); ++i)
        {
            begin = text.find("[ ", begin + 1u);
        }

        const std::size_t end{ begin == std::string::npos ? begin : text.find(" ]", begin) };
        return (end == std::string::npos ? std::string() : text.substr(begin + 2u, end - begin - 2u));
    };

    EXPECT_EQ(field(0u), std::to_string(COOKIE)) << text;
    EXPECT_EQ(field(1u), "OverCap") << text;
    EXPECT_EQ(field(2u), std::to_string(owner.kept)) << text;
    EXPECT_GE(std::strtoull(field(3u).c_str(), nullptr, 10), static_cast<unsigned long long>(SendBacklog::CAP_STALL_MS)) << text;
}
#endif  // AREG_LOGGING

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
    while ((backlog.is_empty() == false) && (owner.closed.load() == false) && (elapsed_ms(started) < DEADLINE_MS))
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

//////////////////////////////////////////////////////////////////////////
// Sends larger than the send buffer of the socket
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< The bytes of a stream, many times the send buffer of the pair.
    constexpr uint32_t  STREAM_BYTES    { 8u * 1024u * 1024u };

    //!< The bytes of one read of the peer, small enough to keep the send buffer full.
    constexpr uint32_t  STREAM_BLOCK    { 4u * 1024u };

    //!< The number of messages written in one batch.
    constexpr uint32_t  STREAM_MESSAGES { 128u };

    //!< Returns a stream of \a bytes where every 4 bytes hold their own number.
    std::vector<uint8_t> numbered_stream(uint32_t bytes)
    {
        std::vector<uint8_t> result(bytes);
        for (uint32_t word = 0u; word < bytes / 4u; ++word)
        {
            std::memcpy(result.data() + word * 4u, &word, sizeof(word));
        }

        return result;
    }

    //!< Reads \a expected in small blocks until it ends or differs, then closes the client end.
    //!< Returns the bytes that arrived intact.
    uint64_t read_stream(Pair & pair, const std::vector<uint8_t> & expected)
    {
        std::vector<uint8_t> block(STREAM_BLOCK);
        uint64_t intact{ 0u };
        while (intact < expected.size())
        {
            const uint32_t length{ static_cast<uint32_t>(std::min<uint64_t>(expected.size() - intact, STREAM_BLOCK)) };
            if ((areg::receive_data_window(pair.client, block.data(), length) != static_cast<int32_t>(length))
             || (std::memcmp(block.data(), expected.data() + intact, length) != 0))
            {
                break;
            }

            intact += length;
        }

        areg::socket_close(pair.client);
        pair.client = areg::InvalidSocketHandle;
        return intact;
    }

    //!< Closes the server end, so that a reader of a shorter stream ends, and expects \a stream whole.
    void expect_stream(Pair & pair, std::future<uint64_t> & reader, const std::vector<uint8_t> & stream)
    {
        areg::socket_close(pair.server);
        pair.server = areg::InvalidSocketHandle;
        EXPECT_EQ(reader.get(), stream.size());
        pair.close();
    }

    //!< Sends a numbered stream with \a send while the peer reads it, and expects every byte in order.
    template<typename SendFn>
    void expect_whole_stream(SendFn send)
    {
        Pair pair{ };
        ASSERT_TRUE(pair.open());
        const std::vector<uint8_t> stream{ numbered_stream(STREAM_BYTES) };
        std::future<uint64_t> reader{ std::async(std::launch::async, [&]() { return read_stream(pair, stream); }) };
        const SOCKETHANDLE server{ pair.server };
        EXPECT_EQ(send(server, stream), static_cast<int32_t>(stream.size()));
        expect_stream(pair, reader, stream);
    }
}

// Reproduces the defect: a call larger than the send buffer lost a part of the stream on Cygwin,
// which skips a refused part of the call and still reports the other parts as sent.
TEST(SendBacklogTest, send_data_larger_than_the_send_buffer_keeps_the_stream_whole)
{
    expect_whole_stream([](SOCKETHANDLE server, const std::vector<uint8_t> & stream)
        {
            return areg::send_data(server, stream.data(), static_cast<uint32_t>(stream.size()));
        });
}

TEST(SendBacklogTest, send_data_v_larger_than_the_send_buffer_keeps_the_stream_whole)
{
    expect_whole_stream([](SOCKETHANDLE server, const std::vector<uint8_t> & stream)
        {
            const std::size_t half{ stream.size() / 2u };
            const areg::IoBuffer buffers[]{ { stream.data(), half }, { stream.data() + half, stream.size() - half } };
            return areg::send_data_v(server, buffers, 2u);
        });
}

TEST(SendBacklogTest, try_send_data_v_larger_than_the_send_buffer_keeps_the_stream_whole)
{
    expect_whole_stream([](SOCKETHANDLE server, const std::vector<uint8_t> & stream)
        {
            const std::size_t half{ stream.size() / 2u };
            const areg::IoBuffer buffers[]{ { stream.data(), half }, { stream.data() + half, stream.size() - half } };
            int32_t sent{ 0 };
            const Clock::time_point begin{ Clock::now() };
            while ((sent == 0) && (elapsed_ms(begin) < DEADLINE_MS))
            {
                sent = areg::try_send_data_v(server, buffers, 2u);
            }

            return sent;
        });
}

TEST(SendBacklogTest, write_larger_than_the_send_buffer_keeps_the_stream_whole)
{
    constexpr uint32_t BYTES{ 32u * 1024u };
    Pair pair{ };
    ASSERT_TRUE(pair.open());
    areg::SendQueueGate gate{ };
    SendBacklog backlog{ gate };
    Owner owner{ };
    owner.socket = pair.server;
    backlog.set_limits(STREAM_BYTES, SendBacklog::UNLIMITED, 60'000u);
    static_cast<void>(backlog.attach(pair.server));

    std::vector<areg::MessageEnvelope> messages(STREAM_MESSAGES);
    std::vector<uint8_t> stream;
    uint32_t word{ 0u };
    for (areg::MessageEnvelope & message : messages)
    {
        uint8_t * data{ message.init_envelope(areg::notify_client_connection(), BYTES) };
        ASSERT_NE(data, nullptr);
        for (uint32_t at = 0u; at < BYTES; at += sizeof(word), ++word)
        {
            std::memcpy(data + at, &word, sizeof(word));
        }

        message.header()->bufHeader.biUsed = BYTES;
        const uint8_t * wire{ areg::wire_data(*message.header()) };
        stream.insert(stream.end(), wire, wire + areg::wire_size(*message.header()));
    }

    std::future<uint64_t> reader{ std::async(std::launch::async, [&]() { return read_stream(pair, stream); }) };
    EXPECT_TRUE(write_range(backlog, pair, messages, 0u, STREAM_MESSAGES));
    const Clock::time_point started{ Clock::now() };
    while ((backlog.is_empty() == false) && (owner.closed.load() == false) && (elapsed_ms(started) < DEADLINE_MS))
    {
        static_cast<void>(backlog.pump(owner));
        std::this_thread::yield();
    }

    std::deque<ITEM_ID> cut;
    backlog.release_all(cut);
    expect_stream(pair, reader, stream);
}

//////////////////////////////////////////////////////////////////////////
// Inline writes, failures of the peer, limits and the release of a backlog
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< Builds one message with a payload of \a bytes bytes.
    areg::MessageEnvelope make_message(uint32_t bytes)
    {
        areg::MessageEnvelope message;
        if (message.init_envelope(areg::notify_client_connection(), bytes) != nullptr)
        {
            message.header()->bufHeader.biUsed = bytes;
        }

        return message;
    }

    //!< Closes the client end of the pair; the server end stays open.
    void close_client(Pair & pair)
    {
        if (areg::is_valid_socket(pair.client))
        {
            areg::socket_close(pair.client);
        }

        pair.client = areg::InvalidSocketHandle;
    }
}

// A small message to a free socket leaves whole at once, nothing is kept, and the peer reads it
// byte for byte.
TEST(SendBacklogTest, write_inline_writes_a_small_message_whole)
{
    Pair pair{ };
    ASSERT_TRUE(pair.open());
    areg::SendQueueGate gate{ };
    SendBacklog backlog{ gate };
    areg::MessageEnvelope message{ make_message(100u) };
    ASSERT_NE(message.header(), nullptr);

    int32_t result{ -2 };
    {
        areg::SocketWriteGuard guard{ pair.server };
        result = backlog.write_inline(COOKIE, pair.server, message);
    }

    const uint32_t size{ areg::wire_size(*message.header()) };
    std::vector<uint8_t> received(size);
    const int32_t read{ areg::receive_data_window(pair.client, received.data(), size) };
    EXPECT_EQ(result, 1);
    EXPECT_TRUE(backlog.is_empty());
    EXPECT_TRUE(gate.is_clear());
    ASSERT_EQ(read, static_cast<int32_t>(size));
    EXPECT_EQ(std::memcmp(received.data(), areg::wire_data(*message.header()), size), 0);
    pair.close();
}

// A message the socket takes in part is kept behind the part written: nothing more is written
// for its connection until the backlog is empty, the send queue gate is entered, and the peer
// receives everything in order once it reads.
TEST(SendBacklogTest, write_inline_keeps_the_rest_of_a_large_message)
{
    Pair pair{ };
    ASSERT_TRUE(pair.open());
    areg::SendQueueGate gate{ };
    SendBacklog backlog{ gate };
    Owner owner{ };
    owner.socket = pair.server;
    backlog.set_limits(1024u * 1024u * 1024u, SendBacklog::UNLIMITED, 60'000u);
    static_cast<void>(backlog.attach(pair.server));
    areg::MessageEnvelope message{ make_message(MESSAGE_BYTES) };
    ASSERT_NE(message.header(), nullptr);
    const uint64_t size{ areg::wire_size(*message.header()) };

    int32_t first{ -2 };
    int32_t sameConnection{ -2 };
    {
        areg::SocketWriteGuard guard{ pair.server };
        first = backlog.write_inline(COOKIE, pair.server, message);
        sameConnection = backlog.write_inline(COOKIE, pair.server, message);
    }

    EXPECT_EQ(first, 2);
    EXPECT_EQ(sameConnection, 0);
    EXPECT_FALSE(backlog.is_empty());
    EXPECT_FALSE(gate.is_clear());

    std::thread reader([&]
    {
        EXPECT_TRUE(read_all(pair, size));
    });

    const Clock::time_point started{ Clock::now() };
    while ((backlog.is_empty() == false) && (owner.closed.load() == false) && (elapsed_ms(started) < DEADLINE_MS))
    {
        static_cast<void>(backlog.pump(owner));
        std::this_thread::yield();
    }

    if (backlog.is_empty() == false)
    {
        pair.close();
    }

    reader.join();
    EXPECT_TRUE(backlog.is_empty());
    EXPECT_TRUE(gate.is_clear());
    EXPECT_FALSE(owner.closed.load());
    pair.close();
}

// A peer that closed its end makes the inline write report a failed socket and keep nothing.
TEST(SendBacklogTest, write_inline_reports_the_socket_of_a_closed_peer_as_failed)
{
    Pair pair{ };
    ASSERT_TRUE(pair.open());
    areg::SendQueueGate gate{ };
    SendBacklog backlog{ gate };
    areg::MessageEnvelope message{ make_message(64u) };
    close_client(pair);

    int32_t result{ 1 };
    const Clock::time_point started{ Clock::now() };
    while ((result == 1) && (elapsed_ms(started) < DEADLINE_MS))
    {
        areg::SocketWriteGuard guard{ pair.server };
        result = backlog.write_inline(COOKIE, pair.server, message);
    }

    EXPECT_EQ(result, -1);
    EXPECT_TRUE(backlog.is_empty());
    pair.close();
}

// A peer that closed its end fails the write of a connection that has a backlog.
TEST(SendBacklogTest, write_to_a_backlog_of_a_closed_peer_fails)
{
    Fixture test;
    ASSERT_TRUE(test.start());
    close_client(test.pair);

    bool failed{ false };
    const Clock::time_point started{ Clock::now() };
    while ((failed == false) && (elapsed_ms(started) < DEADLINE_MS))
    {
        failed = (write_range(test.backlog, test.pair, test.messages, 0u, 1u) == false);
    }

    EXPECT_TRUE(failed);
}

// The owner is told to close a connection whose peer closed its end, with the socket error as the
// reason, and the backlog of the connection is released.
TEST(SendBacklogTest, pump_closes_the_connection_of_a_closed_peer_with_a_socket_error)
{
    Fixture test;
    ASSERT_TRUE(test.start());
    close_client(test.pair);

    const Clock::time_point started{ Clock::now() };
    while ((test.owner.closed.load() == false) && (elapsed_ms(started) < DEADLINE_MS))
    {
        static_cast<void>(test.backlog.pump(test.owner));
        std::this_thread::yield();
    }

    EXPECT_TRUE(test.owner.closed.load());
    EXPECT_EQ(test.owner.reason, SendBacklog::Reason::SocketError);
    EXPECT_GT(test.owner.kept, 0u);
    EXPECT_TRUE(test.backlog.is_empty());
}

// A peer that takes nothing for the refusal limit is closed with the refused reason, though its
// backlog stays below the cap.
TEST(SendBacklogTest, pump_closes_a_peer_that_refuses_data_for_the_refusal_limit)
{
    Fixture test;
    ASSERT_TRUE(test.start());
    test.backlog.set_limits(1024u * 1024u * 1024u, 1u, 60'000u);

    const Clock::time_point started{ Clock::now() };
    while ((test.owner.closed.load() == false) && (elapsed_ms(started) < DEADLINE_MS))
    {
        static_cast<void>(test.backlog.pump(test.owner));
        std::this_thread::yield();
    }

    EXPECT_TRUE(test.owner.closed.load());
    EXPECT_EQ(test.owner.reason, SendBacklog::Reason::Refused);
    EXPECT_TRUE(test.backlog.is_empty());
    EXPECT_TRUE(test.gate.is_clear());
}

// Without backlogs the wait is bounded by a fifth of the keepalive time and by the tick; a zero
// limit selects the default.
TEST(SendBacklogTest, prepare_wait_of_an_empty_backlog_follows_the_keepalive_time)
{
    areg::SendQueueGate gate{ };
    SendBacklog backlog{ gate };
    constexpr uint64_t CAP{ 1024u * 1024u };

    backlog.set_limits(CAP, SendBacklog::UNLIMITED, 60'000u);
    EXPECT_EQ(backlog.prepare_wait(), SendBacklog::TICK_MS);

    backlog.set_limits(CAP, SendBacklog::UNLIMITED, 2'000u);
    EXPECT_EQ(backlog.prepare_wait(), 400u);

    backlog.set_limits(CAP, SendBacklog::UNLIMITED, 1u);
    EXPECT_EQ(backlog.prepare_wait(), 1u);

    backlog.set_limits(0u, 0u, 0u);
    const uint32_t expected{ std::max(1u, std::min(SendBacklog::TICK_MS, areg::SOCKET_KEEPALIVE_SEC * 1'000u / 5u)) };
    EXPECT_EQ(backlog.prepare_wait(), expected);
}

// With a backlog the wait ends at the nearest limit: the refusal limit, or the stall limit of a
// backlog above its cap.
TEST(SendBacklogTest, prepare_wait_of_a_backlog_ends_at_the_nearest_limit)
{
    Fixture test;
    ASSERT_TRUE(test.start());

    test.backlog.set_limits(1024u * 1024u * 1024u, 5'000u, 60'000u);
    const uint32_t refusal{ test.backlog.prepare_wait() };
    EXPECT_GE(refusal, 1u);
    EXPECT_LE(refusal, 5'000u);

    test.backlog.set_limits(1u, SendBacklog::UNLIMITED, 60'000u, true);
    const uint32_t stall{ test.backlog.prepare_wait() };
    EXPECT_GE(stall, 1u);
    EXPECT_LE(stall, SendBacklog::CAP_STALL_MS);

    test.backlog.set_limits(1u, SendBacklog::UNLIMITED, 60'000u, false);
    EXPECT_LE(test.backlog.prepare_wait(), SendBacklog::TICK_MS);
}

// A connection whose message was cut is named by release_all(): the peer must be disconnected,
// because it saw the start of a message that never completes.
TEST(SendBacklogTest, release_all_names_the_connection_cut_in_a_message)
{
    Fixture test;
    ASSERT_TRUE(test.start());
    ASSERT_FALSE(test.gate.is_clear());

    std::deque<ITEM_ID> cut;
    test.backlog.release_all(cut);
    ASSERT_EQ(cut.size(), 1u);
    EXPECT_EQ(cut.front(), COOKIE);
    EXPECT_TRUE(test.backlog.is_empty());
    EXPECT_TRUE(test.gate.is_clear());
}

// A waiter is not registered for a connection that is not held, and cancelling a waiter that
// was never registered changes nothing.
TEST(SendBacklogTest, hold_wait_refuses_a_connection_that_is_not_held)
{
    Fixture test;
    ASSERT_TRUE(test.start());
    Waiter waiter{ };

    EXPECT_FALSE(test.backlog.is_held(COOKIE));
    EXPECT_FALSE(test.backlog.hold_wait(COOKIE, waiter));
    test.backlog.hold_cancel(waiter);

    std::deque<ITEM_ID> cut;
    test.backlog.release_all(cut);
    EXPECT_EQ(waiter.calls.load(), 0u);
}

// A source pauses only on a held connection, a dropped source is forgotten, and the others resume
// once their connection is no longer held.
TEST(SendBacklogTest, paused_sources_resume_when_their_connection_is_released)
{
    using areg::ext::PausedSources;
    using areg::ext::ReceivePause;
    constexpr SOCKETHANDLE DROPPED{ static_cast<SOCKETHANDLE>(901) };
    constexpr SOCKETHANDLE KEPT{ static_cast<SOCKETHANDLE>(902) };

    Fixture test;
    ASSERT_TRUE(test.pair.open());
    test.backlog.set_limits(2u * MESSAGE_BYTES, SendBacklog::UNLIMITED, 60'000u);
    test.owner.socket = test.pair.server;
    static_cast<void>(test.backlog.attach(test.pair.server));
    ASSERT_TRUE(write_range(test.backlog, test.pair, test.messages, 0u, 8u));
    ASSERT_TRUE(test.backlog.is_held(COOKIE));

    areg::SocketMultiplexer mux;
    PausedSources paused(mux);
    areg::ext::receive_pause() = ReceivePause{ };
    EXPECT_FALSE(paused.pause(DROPPED));
    areg::ext::receive_pause() = ReceivePause{ &test.backlog, COOKIE + 1u };
    EXPECT_FALSE(paused.pause(DROPPED));
    EXPECT_FALSE(areg::ext::is_receive_paused());

    areg::ext::receive_pause() = ReceivePause{ &test.backlog, COOKIE };
    ASSERT_TRUE(paused.pause(DROPPED));
    EXPECT_FALSE(areg::ext::is_receive_paused());
    areg::ext::receive_pause() = ReceivePause{ &test.backlog, COOKIE };
    ASSERT_TRUE(paused.pause(KEPT));
    paused.drop(static_cast<SOCKETHANDLE>(903));
    paused.drop(DROPPED);

    std::vector<SOCKETHANDLE> resumed;
    paused.resume([&resumed](SOCKETHANDLE hSocket) { resumed.push_back(hSocket); });
    EXPECT_TRUE(resumed.empty());
    EXPECT_FALSE(paused.has_released());

    test.owner.socket = areg::InvalidSocketHandle;
    static_cast<void>(test.backlog.pump(test.owner));
    EXPECT_TRUE(paused.has_released());
    paused.hold_released();
    EXPECT_TRUE(paused.has_released());

    paused.resume([&resumed](SOCKETHANDLE hSocket) { resumed.push_back(hSocket); });
    ASSERT_EQ(resumed.size(), 1u);
    EXPECT_EQ(resumed[0], KEPT);
    EXPECT_FALSE(paused.has_released());
    paused.clear();
}
