/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SendBacklogTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the backlog wait of a service send thread.
 *              While a peer reads nothing the thread sleeps; it moves on as soon as the
 *              peer reads, a message is queued or an exit is requested.
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

        SOCKETHANDLE backlog_socket(ITEM_ID /*cookie*/) override
        {
            pumps.fetch_add(1u, std::memory_order_relaxed);
            return socket;
        }

        void backlog_sent(uint64_t bytes, uint32_t /*msgs*/) override
        {
            sent.fetch_add(bytes, std::memory_order_relaxed);
        }

        void backlog_close(const SendBacklog::Entry & /*entry*/, SendBacklog::Reason /*reason*/) override
        {
            closed.store(true, std::memory_order_relaxed);
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

        void wait_backlog(bool takeEvents, uint32_t timeoutMs)
        {
            backlog.reset_wake();
            if (queue.arm_waiter(backlog, takeEvents))
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
            result += sizeof(areg::EventHeader) + message.header()->bufHeader.biUsed;
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
            std::deque<SOCKETHANDLE> cut;
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
