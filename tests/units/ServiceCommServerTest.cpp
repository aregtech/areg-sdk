/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ServiceCommServerTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the server side communication of a service: a server
 *              on the loopback address with raw clients, in the shared thread mode and in the
 *              pool mode. The clients connect, are told their cookie, send messages to each
 *              other, disconnect, are rejected by the connection policy, and the server restarts.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/private/SocketLiveness.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "aregextend/service/ServiceCommunicationBase.hpp"
#include "units/ServiceTestServer.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <thread>
#include <vector>

#if defined(_POSIX)
    #include <sys/socket.h>
    #include <sys/time.h>
#elif defined(WINDOWS)
    #include <WinSock2.h>
#endif

namespace
{
    using Clock = std::chrono::steady_clock;
    using service_test::Policy;
    using service_test::TestServer;

    //!< Milliseconds the test waits for an outcome before it fails.
    constexpr int64_t   DEADLINE_MS     { 60'000 };
    //!< The pool size of the pool mode.
    constexpr uint32_t  POOL_PAIRS      { 2u };
    //!< Milliseconds a client waits for a reply that must not come.
    constexpr uint32_t  LATE_WAIT_MS    { 5'000u };

    int64_t elapsed_ms(Clock::time_point since)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - since).count();
    }

    //!< Returns a port of the loopback address that nobody listens on, or 0.
    //!< The range is outside the ephemeral ports of all platforms.
    uint16_t free_port()
    {
        std::random_device random;
        const uint16_t first{ static_cast<uint16_t>(21'000u + (random() % 3'000u)) };
        for (uint16_t i = 0u; i < 500u; ++i)
        {
            const uint16_t port{ static_cast<uint16_t>(first + i) };
            const SOCKETHANDLE listener{ areg::server_connect(areg::String("127.0.0.1"), port) };
            if (areg::is_valid_socket(listener) == false)
                continue;

            const bool listening{ areg::server_listen(listener) };
            areg::socket_close(listener);
            if (listening)
                return port;
        }

        return 0u;
    }

    //!< Builds a message from \a from to \a to that the server forwards. The first word of the
    //!< payload is \a seq; the payload has \a bytes bytes in all.
    areg::MessageEnvelope make_data(ITEM_ID from, ITEM_ID to, uint32_t seq, uint32_t bytes)
    {
        areg::MessageEnvelope message;
        uint8_t * data{ message.init_envelope(areg::notify_client_connection(), bytes) };
        if (data != nullptr)
        {
            std::memset(data, 0xA5, bytes);
            std::memcpy(data, &seq, sizeof(seq));
            message.header()->bufHeader.biUsed = bytes;
            message.set_message_id(areg::REQUEST_ID_FIRST);
            message.set_source(static_cast<uint32_t>(from));
            message.set_target(static_cast<uint32_t>(to));
        }

        return message;
    }

    //!< A server on a free port of the loopback address; it is stopped when the object goes away.
    struct Running
    {
        TestServer  server;
        uint16_t    port    { 0u };
        bool        started { false };

        Running(Policy policy, uint32_t pairs)
            : server(policy, pairs)
        {
        }

        bool configure()
        {
            port = free_port();
            if (port == 0u)
                return false;

            server.apply_connection_data(areg::String("127.0.0.1"), port);
            return true;
        }

        bool start()
        {
            started = configure() && server.connect_service_host();
            return started;
        }

        void stop()
        {
            if (started)
            {
                server.disconnect_service_host();
                server.wait_to_complete();
                started = false;
            }
        }

        ~Running()
        {
            stop();
        }
    };

    //!< A raw client of the server.
    struct Peer
    {
        areg::ClientConnection  link;
        ITEM_ID                 cookie  { areg::COOKIE_UNKNOWN };

        explicit Peer(uint16_t port)
            : link(areg::String("127.0.0.1"), port)
        {
        }

        ~Peer()
        {
            link.close_socket();
        }

        bool connect_once()
        {
            if (link.create_socket())
                return true;

            link.close_socket();
            return false;
        }

        bool send(const areg::MessageEnvelope & message)
        {
            return (link.send_message(message) > 0);
        }

        bool receive(areg::MessageEnvelope & message)
        {
            return (link.receive_message(message) > 0);
        }

        //!< Makes a read of the connected socket fail after \a waitMs milliseconds without data.
        bool limit_receive(uint32_t waitMs)
        {
#if defined(WINDOWS)
            const DWORD timeout{ waitMs };
#else
            const timeval timeout{ static_cast<time_t>(waitMs / 1'000u), static_cast<suseconds_t>((waitMs % 1'000u) * 1'000u) };
#endif
            return (::setsockopt(link.live_handle(), SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout), sizeof(timeout)) == 0);
        }

        //!< Connects and says hello. Where the server is not listening yet, or drops the client
        //!< while it restarts, it tries again. The cookie is the one the server tells.
        bool join()
        {
            const Clock::time_point started{ Clock::now() };
            while (elapsed_ms(started) < DEADLINE_MS)
            {
                if (connect_once())
                {
                    areg::MessageEnvelope request{ areg::create_connect_request(areg::COOKIE_REMOTE_SERVICE, areg::COOKIE_ROUTER, areg::MessageSource::SourceTest) };
                    areg::MessageEnvelope reply;
                    if (send(request) && receive(reply) && areg::is_notify_client(reply))
                    {
                        cookie = static_cast<ITEM_ID>(reply.target());
                        return (cookie >= areg::COOKIE_REMOTE_SERVICE);
                    }

                    link.close_socket();
                }

                std::this_thread::yield();
            }

            return false;
        }

        //!< Reads until the server closes the connection. Returns the number of messages read.
        uint32_t drain()
        {
            uint32_t count{ 0u };
            areg::MessageEnvelope message;
            while (receive(message))
            {
                ++count;
            }

            return count;
        }
    };

    using PeerPtr = std::unique_ptr<Peer>;

    PeerPtr make_peer(uint16_t port)
    {
        return std::make_unique<Peer>(port);
    }

    areg::SocketAddress loopback_address(uint16_t port)
    {
        areg::SocketAddress address{ };
        static_cast<void>(address.resolve_address(areg::String("127.0.0.1"), port, false));
        return address;
    }
}

class ServiceCommServerTest : public testing::Test
{
protected:
    void SetUp() override
    {
        areg::Application::setup(false, false, false, true, false);
    }

    void TearDown() override
    {
        areg::Application::release();
    }

    //!< Many clients connect, are told different cookies, and one says goodbye.
    static void run_handshakes(uint32_t pairs)
    {
        constexpr uint32_t CLIENTS{ 6u };
        Running run(Policy::Accept, pairs);
        ASSERT_TRUE(run.start());

        std::vector<PeerPtr> peers;
        std::set<ITEM_ID> cookies;
        for (uint32_t i = 0u; i < CLIENTS; ++i)
        {
            peers.push_back(make_peer(run.port));
            ASSERT_TRUE(peers.back()->join());
            cookies.insert(peers.back()->cookie);
        }

        EXPECT_EQ(cookies.size(), CLIENTS);
        EXPECT_TRUE(run.server.wait_added(CLIENTS));
        EXPECT_EQ(run.server.instances().size(), CLIENTS);
        EXPECT_EQ(run.server.active_client_pair_count(), pairs);
        EXPECT_TRUE(run.server.is_host_connected());
        EXPECT_FALSE(run.server.is_host_pending());
        EXPECT_TRUE(run.server.is_host_setup());
        EXPECT_FALSE(run.server.is_target_held(peers[0]->cookie));

        if (pairs == 0u)
        {
            // The statistics of the shared threads count every message of the handshakes.
            uint32_t sentMsgs{ 0u };
            uint32_t recvMsgs{ 0u };
            uint64_t sentBytes{ 0u };
            uint64_t recvBytes{ 0u };
            const Clock::time_point started{ Clock::now() };
            while (((sentMsgs < CLIENTS) || (recvMsgs < CLIENTS)) && (elapsed_ms(started) < DEADLINE_MS))
            {
                uint64_t bytes{ 0u };
                uint32_t msgs{ 0u };
                run.server.query_data_sent(bytes, msgs);
                sentBytes += bytes;
                sentMsgs += msgs;
                run.server.data_rate_helper().query_data_received(bytes, msgs);
                recvBytes += bytes;
                recvMsgs += msgs;
                std::this_thread::yield();
            }

            EXPECT_GE(sentMsgs, CLIENTS);
            EXPECT_GE(recvMsgs, CLIENTS);
            EXPECT_GT(sentBytes, 0u);
            EXPECT_GT(recvBytes, 0u);
        }

        // A client that says goodbye is forgotten, and the server closes its connection.
        areg::MessageEnvelope bye{ areg::create_disconnect_request(peers[0]->cookie, areg::COOKIE_ROUTER) };
        ASSERT_TRUE(peers[0]->send(bye));
        static_cast<void>(peers[0]->drain());
        EXPECT_TRUE(run.server.wait_removed(1u));
        EXPECT_TRUE(run.server.wait_disconnect(peers[0]->cookie));
        EXPECT_EQ(run.server.instances().size(), CLIENTS - 1u);

        // The stop of the server closes the connections of the others.
        run.stop();
        for (uint32_t i = 1u; i < CLIENTS; ++i)
        {
            static_cast<void>(peers[i]->drain());
        }
    }

    //!< One client sends many messages to another one, a message in two parts among them, and a
    //!< message to a client that does not exist; the other client gets every message in order.
    static void run_forwarding(uint32_t pairs)
    {
        constexpr uint32_t MESSAGES     { 400u };
        constexpr uint32_t SMALL_BYTES  { 96u };
        constexpr uint32_t BIG_BYTES    { 200u * 1024u };
        constexpr uint32_t MARKER       { 777'777u };

        Running run(Policy::Accept, pairs);
        ASSERT_TRUE(run.start());
        PeerPtr from{ make_peer(run.port) };
        PeerPtr to{ make_peer(run.port) };
        PeerPtr third{ make_peer(run.port) };
        ASSERT_TRUE(from->join());
        ASSERT_TRUE(to->join());
        ASSERT_TRUE(third->join());

        const ITEM_ID nobody{ areg::COOKIE_REMOTE_SERVICE + 1'000'000u };
        for (uint32_t seq = 0u; seq < MESSAGES; ++seq)
        {
            ASSERT_TRUE(from->send(make_data(from->cookie, to->cookie, seq, SMALL_BYTES)));
            if ((seq % 100u) == 50u)
            {
                ASSERT_TRUE(from->send(make_data(from->cookie, nobody, seq, SMALL_BYTES)));
            }
        }

        for (uint32_t seq = 0u; seq < MESSAGES; ++seq)
        {
            areg::MessageEnvelope received;
            ASSERT_TRUE(to->receive(received));
            uint32_t value{ 0u };
            received >> value;
            EXPECT_EQ(value, seq);
            EXPECT_EQ(received.source(), static_cast<uint32_t>(from->cookie));
            EXPECT_EQ(received.target(), static_cast<uint32_t>(to->cookie));
        }

        // The first part of a big message is on the wire, then a message of another client passes,
        // then the rest of the big message follows.
        areg::MessageEnvelope big{ make_data(from->cookie, to->cookie, 5u, BIG_BYTES) };
        ASSERT_NE(big.header(), nullptr);
        const uint8_t * wire{ areg::wire_data(*big.header()) };
        const uint32_t wireSize{ areg::wire_size(*big.header()) };
        const uint32_t firstPart{ static_cast<uint32_t>(areg::WIRE_HEADER_SIZE) + 1'000u };
        const areg::IoBuffer head{ wire, firstPart };
        const areg::IoBuffer tail{ wire + firstPart, static_cast<std::size_t>(wireSize - firstPart) };
        ASSERT_GT(from->link.send_messages_batch(&head, 1u, firstPart), 0);
        ASSERT_TRUE(third->send(make_data(third->cookie, to->cookie, MARKER, SMALL_BYTES)));

        areg::MessageEnvelope passed;
        ASSERT_TRUE(to->receive(passed));
        uint32_t marker{ 0u };
        passed >> marker;
        EXPECT_EQ(marker, MARKER);
        EXPECT_EQ(passed.source(), static_cast<uint32_t>(third->cookie));

        ASSERT_GT(from->link.send_messages_batch(&tail, 1u, wireSize - firstPart), 0);
        areg::MessageEnvelope whole;
        ASSERT_TRUE(to->receive(whole));
        uint32_t first{ 0u };
        whole >> first;
        EXPECT_EQ(first, 5u);
        EXPECT_EQ(whole.header()->bufHeader.biUsed, BIG_BYTES);
        EXPECT_EQ(whole.source(), static_cast<uint32_t>(from->cookie));
    }

    //!< Clients that vanish are forgotten, a message to a vanished client is dropped, and the
    //!< clients that remain still talk to each other.
    static void run_vanished_clients(uint32_t pairs)
    {
        Running run(Policy::Accept, pairs);
        ASSERT_TRUE(run.start());
        PeerPtr first{ make_peer(run.port) };
        PeerPtr gone{ make_peer(run.port) };
        PeerPtr last{ make_peer(run.port) };
        ASSERT_TRUE(first->join());
        ASSERT_TRUE(gone->join());
        ASSERT_TRUE(last->join());
        ASSERT_TRUE(run.server.wait_added(3u));

        const ITEM_ID goneCookie{ gone->cookie };
        gone->link.close_socket();
        EXPECT_TRUE(run.server.wait_removed(1u));
        EXPECT_TRUE(run.server.wait_disconnect(goneCookie));
        EXPECT_EQ(run.server.instances().size(), 2u);

        for (uint32_t seq = 0u; seq < 20u; ++seq)
        {
            ASSERT_TRUE(first->send(make_data(first->cookie, gone->cookie, seq, 64u)));
        }

        ASSERT_TRUE(first->send(make_data(first->cookie, last->cookie, 4242u, 64u)));
        areg::MessageEnvelope received;
        ASSERT_TRUE(last->receive(received));
        uint32_t value{ 0u };
        received >> value;
        EXPECT_EQ(value, 4242u);

        // Reproduces the defect: the list was a reference to a map the receive thread changes, so a reader raced with a client leaving.
        const areg::MapInstances before{ run.server.instances() };
        last->link.close_socket();
        EXPECT_TRUE(run.server.wait_instances(1u));
        EXPECT_EQ(before.size(), 2u);
    }

    //!< The server sends one message again and again to a client that reads only afterwards.
    static void run_message_sent_again(uint32_t pairs)
    {
        constexpr uint32_t COPIES{ 400u };
        constexpr uint32_t BYTES{ 128u * 1024u };

        Running run(Policy::Accept, pairs);
        run.server.set_close_stalled(false);
        ASSERT_TRUE(run.start());
        PeerPtr reader{ make_peer(run.port) };
        ASSERT_TRUE(reader->join());

        areg::MessageEnvelope message{ make_data(areg::COOKIE_ROUTER, reader->cookie, 7u, BYTES) };
        for (uint32_t i = 0u; i != COPIES; ++i)
        {
            ASSERT_TRUE(run.server.send_message(message));
        }

        EXPECT_EQ(message.header()->internal1, 0u);
        EXPECT_EQ(message.header()->internal2, 0u);
        for (uint32_t i = 0u; i != COPIES; ++i)
        {
            areg::MessageEnvelope received;
            ASSERT_TRUE(reader->receive(received)) << "copy " << i;
            ASSERT_EQ(received.size_used(), BYTES);
        }
    }

    //!< A client that reads nothing is held by the server, the sender of its messages is not
    //!< read while it is held, and no message is lost or reordered once the client reads.
    static void run_slow_reader(uint32_t pairs, uint32_t messageBytes)
    {
        constexpr uint64_t MAX_BYTES{ 1024ull * 1024ull * 1024ull };
        constexpr uint32_t SENTINEL{ 0xFFFF'FFFFu };
        const uint32_t limit{ static_cast<uint32_t>(MAX_BYTES / messageBytes) };

        Running run(Policy::Accept, pairs);
        run.server.set_close_stalled(false);
        ASSERT_TRUE(run.start());
        PeerPtr sender{ make_peer(run.port) };
        PeerPtr reader{ make_peer(run.port) };
        reader->link.set_socket_buffers(0u, 64u * 1024u);
        ASSERT_TRUE(sender->join());
        ASSERT_TRUE(reader->join());
        areg::set_receive_recheck(reader->link.socket().handle(), static_cast<uint32_t>(DEADLINE_MS));

        // The producer sends until the test saw the reader held, then a last message that ends the stream.
        std::atomic<bool> stop{ false };
        std::atomic<bool> failed{ false };
        std::atomic<uint32_t> sent{ 0u };
        std::thread producer([&]
        {
            for (uint32_t seq = 0u; (seq < limit) && (stop.load(std::memory_order_acquire) == false); ++seq)
            {
                if (sender->send(make_data(sender->cookie, reader->cookie, seq, messageBytes)) == false)
                {
                    failed.store(true, std::memory_order_release);
                    return;
                }

                sent.fetch_add(1u, std::memory_order_relaxed);
            }

            if (sender->send(make_data(sender->cookie, reader->cookie, SENTINEL, 64u)) == false)
            {
                failed.store(true, std::memory_order_release);
            }
        });

        bool sawHeld{ false };
        const Clock::time_point started{ Clock::now() };
        while ((sawHeld == false) && (failed.load(std::memory_order_acquire) == false) && (elapsed_ms(started) < DEADLINE_MS))
        {
            sawHeld = run.server.is_target_held(reader->cookie);
            std::this_thread::yield();
        }

        stop.store(true, std::memory_order_release);

        uint32_t received{ 0u };
        bool inOrder{ true };
        bool ended{ false };
        while ((ended == false) && (failed.load(std::memory_order_acquire) == false))
        {
            areg::MessageEnvelope message;
            if (reader->receive(message) == false)
                break;

            uint32_t value{ 0u };
            message >> value;
            if (value == SENTINEL)
            {
                ended = true;
            }
            else
            {
                inOrder = inOrder && (value == received);
                ++received;
            }
        }

        producer.join();
        EXPECT_FALSE(failed.load());
        EXPECT_TRUE(sawHeld);
        EXPECT_TRUE(ended);
        EXPECT_EQ(received, sent.load());
        EXPECT_TRUE(inOrder);

        const Clock::time_point drained{ Clock::now() };
        while (run.server.is_target_held(reader->cookie) && (elapsed_ms(drained) < DEADLINE_MS))
        {
            std::this_thread::yield();
        }

        EXPECT_FALSE(run.server.is_target_held(reader->cookie));
    }

    //!< Checks that every send thread of the running server uses \a closeStalled and the refusal time of the server.
    static void expect_send_policy(Running & run, uint32_t pairs, bool closeStalled)
    {
        const uint32_t refusal{ areg::send_refusal_ms(0u, areg::SOCKET_KEEPALIVE_SEC) };
        EXPECT_EQ(run.server.shared_backlog().closes_stalled(), closeStalled);
        EXPECT_EQ(run.server.shared_backlog().refusal_ms(), refusal);
        ASSERT_EQ(run.server.active_client_pair_count(), pairs);
        for (uint32_t i = 0u; i != pairs; ++i)
        {
            EXPECT_EQ(run.server.pool_backlog(i).closes_stalled(), closeStalled) << "pair " << i;
            EXPECT_EQ(run.server.pool_backlog(i).refusal_ms(), refusal) << "pair " << i;
        }
    }

    //!< A server started with an address only applies its send policy at the start and at a restart.
    static void run_send_policy(uint32_t pairs)
    {
        for (const bool closeStalled : { false, true })
        {
            Running run(Policy::Accept, pairs);
            run.server.set_close_stalled(closeStalled);
            ASSERT_TRUE(run.start());
            Peer before(run.port);
            ASSERT_TRUE(before.join());
            expect_send_policy(run, pairs, closeStalled);

            run.server.set_close_stalled(!closeStalled);
            ASSERT_TRUE(run.server.reconnect_service_host());
            static_cast<void>(before.drain());
            Peer after(run.port);
            ASSERT_TRUE(after.join());
            expect_send_policy(run, pairs, !closeStalled);
        }
    }

    //!< A client that connects while the connection stops is closed and is not told a cookie.
    static void run_client_while_stopping(uint32_t pairs)
    {
        Running run(Policy::Accept, pairs);
        ASSERT_TRUE(run.start());
        Peer first(run.port);
        ASSERT_TRUE(first.join());
        ASSERT_TRUE(run.server.wait_added(1u));

        run.server.set_stopping(true);
        Peer late(run.port);
        ASSERT_TRUE(late.connect_once());
        ASSERT_TRUE(late.limit_receive(LATE_WAIT_MS));
        areg::MessageEnvelope request{ areg::create_connect_request(areg::COOKIE_REMOTE_SERVICE, areg::COOKIE_ROUTER, areg::MessageSource::SourceTest) };
        static_cast<void>(late.send(request));
        areg::MessageEnvelope reply;
        EXPECT_FALSE(late.receive(reply));
        run.server.set_stopping(false);

        EXPECT_EQ(run.server.instances().size(), 1u);
        EXPECT_EQ(run.server.active_client_pair_count(), pairs);

        Peer after(run.port);
        ASSERT_TRUE(after.join());
        EXPECT_NE(after.cookie, first.cookie);
        EXPECT_TRUE(run.server.wait_instances(2u));
    }
};

TEST_F(ServiceCommServerTest, clients_are_told_their_cookies_in_the_shared_thread_mode)
{
    run_handshakes(0u);
}

TEST_F(ServiceCommServerTest, clients_are_told_their_cookies_in_the_pool_mode)
{
    run_handshakes(POOL_PAIRS);
}

TEST_F(ServiceCommServerTest, messages_between_clients_arrive_whole_and_in_order_in_the_shared_thread_mode)
{
    run_forwarding(0u);
}

TEST_F(ServiceCommServerTest, messages_between_clients_arrive_whole_and_in_order_in_the_pool_mode)
{
    run_forwarding(POOL_PAIRS);
}

TEST_F(ServiceCommServerTest, vanished_clients_are_forgotten_in_the_shared_thread_mode)
{
    run_vanished_clients(0u);
}

TEST_F(ServiceCommServerTest, vanished_clients_are_forgotten_in_the_pool_mode)
{
    run_vanished_clients(POOL_PAIRS);
}

// Reproduces the defect: sending a message again wrote routing fields into the block the send
// thread was using, so a queued copy could lose its consumer and pointers could reach the wire.
TEST_F(ServiceCommServerTest, message_sent_again_arrives_every_time_in_the_shared_thread_mode)
{
    run_message_sent_again(0u);
}

TEST_F(ServiceCommServerTest, message_sent_again_arrives_every_time_in_the_pool_mode)
{
    run_message_sent_again(POOL_PAIRS);
}

TEST_F(ServiceCommServerTest, slow_reader_is_held_without_loss_in_the_shared_thread_mode)
{
    run_slow_reader(0u, 48u * 1024u);
}

TEST_F(ServiceCommServerTest, slow_reader_is_held_without_loss_in_the_pool_mode)
{
    run_slow_reader(POOL_PAIRS, 48u * 1024u);
}

TEST_F(ServiceCommServerTest, slow_reader_gets_big_messages_through_the_send_thread)
{
    run_slow_reader(POOL_PAIRS, 256u * 1024u);
}

// Reproduces the defect: a server started with an address only kept the default send policy in
// its shared send thread, so set_close_stalled(false) was ignored there.
TEST_F(ServiceCommServerTest, send_policy_reaches_every_send_thread_in_the_shared_thread_mode)
{
    run_send_policy(0u);
}

TEST_F(ServiceCommServerTest, send_policy_reaches_every_send_thread_in_the_pool_mode)
{
    run_send_policy(POOL_PAIRS);
}

// Reproduces the defect: while the server restarted, a client that connected after the old
// clients were dropped was still told a cookie by the stopping server, and dropped right after.
TEST_F(ServiceCommServerTest, client_connecting_while_the_server_stops_is_closed_in_the_shared_thread_mode)
{
    run_client_while_stopping(0u);
}

TEST_F(ServiceCommServerTest, client_connecting_while_the_server_stops_is_closed_in_the_pool_mode)
{
    run_client_while_stopping(POOL_PAIRS);
}

// A client of a host that is not allowed is told that it is rejected, and the server closes the
// connection. The host is not allowed if the policy rejects hosts and the host is not in the white
// list, or the policy accepts hosts and the host is in the black list.
TEST_F(ServiceCommServerTest, client_of_a_host_that_is_not_allowed_is_rejected)
{
    for (const bool blacklisted : { false, true })
    {
        Running run(blacklisted ? Policy::Accept : Policy::Reject, 0u);
        ASSERT_TRUE(run.configure());
        if (blacklisted)
        {
            run.server.add_black_list(loopback_address(run.port));
        }

        run.started = run.server.connect_service_host();
        ASSERT_TRUE(run.started);

        Peer peer(run.port);
        const Clock::time_point started{ Clock::now() };
        while ((peer.connect_once() == false) && (elapsed_ms(started) < DEADLINE_MS))
        {
            std::this_thread::yield();
        }

        areg::MessageEnvelope reply;
        ASSERT_TRUE(peer.receive(reply));
        EXPECT_TRUE(areg::is_notify_client(reply));
        ITEM_ID cookie{ areg::COOKIE_REMOTE_SERVICE };
        areg::ServiceConnectionState state{ areg::ServiceConnectionState::Connected };
        reply >> cookie;
        reply >> state;
        EXPECT_EQ(state, areg::ServiceConnectionState::Rejected);
        EXPECT_EQ(peer.drain(), 0u);
        EXPECT_EQ(run.server.instances().size(), 0u);
    }
}

// A host in the white list is served although the policy rejects hosts.
TEST_F(ServiceCommServerTest, white_listed_host_is_served_under_the_rejecting_policy)
{
    Running run(Policy::Reject, 0u);
    ASSERT_TRUE(run.configure());
    run.server.add_white_list(loopback_address(run.port));
    run.started = run.server.connect_service_host();
    ASSERT_TRUE(run.started);

    Peer peer(run.port);
    EXPECT_TRUE(peer.join());
    EXPECT_TRUE(run.server.wait_added(1u));
}

// The white list and the black list exclude each other, and an address is removed from the list
// it is in.
TEST_F(ServiceCommServerTest, white_list_and_black_list_exclude_each_other)
{
    TestServer server(Policy::Accept, 0u);
    const areg::SocketAddress address{ loopback_address(1u) };
    ASSERT_TRUE(address.is_valid());

    EXPECT_FALSE(server.is_address_in_white_list(address));
    EXPECT_FALSE(server.is_address_in_black_list(address));

    server.add_white_list(address);
    EXPECT_TRUE(server.is_address_in_white_list(address));
    EXPECT_FALSE(server.is_address_in_black_list(address));

    server.add_black_list(address);
    EXPECT_FALSE(server.is_address_in_white_list(address));
    EXPECT_TRUE(server.is_address_in_black_list(address));

    server.add_white_list(address);
    EXPECT_TRUE(server.is_address_in_white_list(address));
    EXPECT_FALSE(server.is_address_in_black_list(address));

    server.remove_white_list(address);
    EXPECT_FALSE(server.is_address_in_white_list(address));
    server.add_black_list(address);
    server.remove_black_list(address);
    EXPECT_FALSE(server.is_address_in_black_list(address));
}

// The server is not set up, not connected and not pending before its address is set; it is set
// up after, and connected once it runs. A second start of a running server changes nothing.
TEST_F(ServiceCommServerTest, server_state_follows_its_address_and_its_start)
{
    Running run(Policy::Accept, 0u);
    EXPECT_FALSE(run.server.is_host_setup());
    EXPECT_FALSE(run.server.is_host_connected());
    EXPECT_FALSE(run.server.is_host_pending());
    EXPECT_EQ(run.server.active_client_pair_count(), 0u);

    ASSERT_TRUE(run.configure());
    EXPECT_TRUE(run.server.is_host_setup());
    EXPECT_FALSE(run.server.is_host_connected());

    run.started = run.server.connect_service_host();
    ASSERT_TRUE(run.started);
    Peer peer(run.port);
    ASSERT_TRUE(peer.join());
    EXPECT_TRUE(run.server.is_host_connected());
    EXPECT_FALSE(run.server.is_host_pending());
    EXPECT_TRUE(run.server.connect_service_host());
    EXPECT_TRUE(run.server.is_host_connected());

    run.stop();
    EXPECT_FALSE(run.server.is_host_connected());
}

// The server takes its address only from an enabled TCP/IP entry of its own service in the configuration.
TEST_F(ServiceCommServerTest, server_setup_follows_the_configuration)
{
    constexpr uint32_t TCPIP{ static_cast<uint32_t>(areg::ConnectionType::Tcpip) };
    constexpr uint32_t UDP{ static_cast<uint32_t>(areg::ConnectionType::Udp) };
    areg::ConfigManager & manager{ areg::Application::config_manager() };
    const bool configured{ manager.is_configured() };
    const areg::ListProperties readonly{ manager.readonly_properties() };
    const areg::ListProperties writable{ manager.module_properties() };
    manager.set_configuration(areg::ListProperties(), areg::ListProperties());

    TestServer server(Policy::Accept, 0u);
    EXPECT_FALSE(server.setup_connection_data(areg::RemoteServiceKind::Logger, TCPIP));
    EXPECT_FALSE(server.setup_connection_data(areg::RemoteServiceKind::Router, 0u));
    TestServer udp(Policy::Accept, 0u, UDP);
    EXPECT_FALSE(udp.setup_connection_data(areg::RemoteServiceKind::Router, UDP));

    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    config.set_connection_enable(false);
    EXPECT_FALSE(server.setup_connection_data(areg::RemoteServiceKind::Router, TCPIP));
    EXPECT_FALSE(server.is_host_setup());

    config.set_connection_enable(true);
    config.set_connection_data(areg::String("127.0.0.1"), 23456u);
    EXPECT_TRUE(server.setup_connection_data(areg::RemoteServiceKind::Router, TCPIP));
    EXPECT_TRUE(server.is_host_setup());
    EXPECT_FALSE(server.is_host_connected());

    manager.set_configuration(readonly, writable);
    if (configured == false)
    {
        manager.release_properties();
    }
}

// A reconnect of a server that does not run starts it, and clients join it.
TEST_F(ServiceCommServerTest, reconnect_starts_a_server_that_does_not_run)
{
    for (const uint32_t pairs : { 0u, POOL_PAIRS })
    {
        Running run(Policy::Accept, pairs);
        ASSERT_TRUE(run.configure());
        ASSERT_TRUE(run.server.reconnect_service_host());
        run.started = true;
        Peer peer(run.port);
        ASSERT_TRUE(peer.join());
        EXPECT_TRUE(run.server.is_host_connected());
        EXPECT_EQ(run.server.active_client_pair_count(), pairs);
    }
}

// A connection failure restarts a connected server and changes nothing on a server that does not run.
TEST_F(ServiceCommServerTest, connection_failure_restarts_only_a_connected_server)
{
    for (const uint32_t pairs : { 0u, POOL_PAIRS })
    {
        Running run(Policy::Accept, pairs);
        ASSERT_TRUE(run.configure());
        run.server.connection_failure();
        EXPECT_FALSE(run.server.is_host_pending());
        EXPECT_FALSE(run.server.is_host_connected());

        run.started = run.server.connect_service_host();
        ASSERT_TRUE(run.started);
        Peer before(run.port);
        ASSERT_TRUE(before.join());
        run.server.connection_failure();
        static_cast<void>(before.drain());

        Peer after(run.port);
        ASSERT_TRUE(after.join());
        EXPECT_TRUE(run.server.is_host_connected());
    }
}

// A restart drops the connected clients and the server accepts new clients again.
TEST_F(ServiceCommServerTest, restart_drops_the_clients_and_accepts_new_ones)
{
    for (const uint32_t pairs : { 0u, POOL_PAIRS })
    {
        Running run(Policy::Accept, pairs);
        ASSERT_TRUE(run.start());
        Peer before(run.port);
        ASSERT_TRUE(before.join());

        EXPECT_TRUE(run.server.reconnect_service_host());
        static_cast<void>(before.drain());

        Peer after(run.port);
        ASSERT_TRUE(after.join());
        EXPECT_TRUE(run.server.is_host_connected());
        EXPECT_EQ(run.server.active_client_pair_count(), pairs);
    }
}

// The verbose flag of the data rate helper switches the statistics of the server, and the helper
// reports the flag it set.
TEST_F(ServiceCommServerTest, data_rate_helper_switches_the_statistics)
{
    TestServer server(Policy::Accept, 0u);
    EXPECT_TRUE(server.data_rate_helper().is_verbose());
    EXPECT_TRUE(server.is_data_rate_enabled());

    server.data_rate_helper().set_verbose(false);
    EXPECT_FALSE(server.data_rate_helper().is_verbose());
    EXPECT_FALSE(server.is_data_rate_enabled());

    server.enable_data_rate(true);
    EXPECT_TRUE(server.data_rate_helper().is_verbose());
    EXPECT_TRUE(server.is_data_rate_enabled());

    uint64_t bytes{ 5u };
    uint32_t msgs{ 5u };
    server.data_rate_helper().query_data_sent(bytes, msgs);
    EXPECT_EQ(bytes, 0u);
    EXPECT_EQ(msgs, 0u);
    server.data_rate_helper().query_data_received(bytes, msgs);
    EXPECT_EQ(bytes, 0u);
    EXPECT_EQ(msgs, 0u);
}

// The server builds the connected notification for a client and the disconnected one, from the
// channel to the client, and the connected one names the kind of the source.
TEST_F(ServiceCommServerTest, server_builds_the_connect_and_the_disconnect_notifications)
{
    TestServer server(Policy::Accept, 0u);
    const ITEM_ID client{ areg::COOKIE_REMOTE_SERVICE };

    areg::MessageEnvelope connect{ server.connect_message(areg::COOKIE_ROUTER, client, areg::MessageSource::SourceService) };
    ASSERT_NE(connect.header(), nullptr);
    EXPECT_TRUE(areg::is_notify_client(connect));
    EXPECT_EQ(connect.target(), static_cast<uint32_t>(client));
    ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
    areg::ServiceConnectionState state{ areg::ServiceConnectionState::Disconnected };
    areg::MessageSource source{ areg::MessageSource::SourceTest };
    connect.move_to_begin();
    connect >> cookie;
    connect >> state;
    connect >> source;
    EXPECT_EQ(cookie, client);
    EXPECT_EQ(state, areg::ServiceConnectionState::Connected);
    EXPECT_EQ(source, areg::MessageSource::SourceService);

    areg::MessageEnvelope disconnect{ server.disconnect_message(areg::COOKIE_ROUTER, client) };
    ASSERT_NE(disconnect.header(), nullptr);
    EXPECT_TRUE(areg::is_notify_client(disconnect));
    EXPECT_EQ(disconnect.target(), static_cast<uint32_t>(client));
    disconnect.move_to_begin();
    disconnect >> cookie;
    disconnect >> state;
    EXPECT_EQ(cookie, client);
    EXPECT_EQ(state, areg::ServiceConnectionState::Disconnected);
}
