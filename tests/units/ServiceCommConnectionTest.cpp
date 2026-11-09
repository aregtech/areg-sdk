/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ServiceCommConnectionTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the server connection of a service: its addresses,
 *              the accepted clients and their cookies, the rejection of a client, the close of
 *              one connection or of all, and the hand-back of the closed sockets to their owner.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/String.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"
#include "aregextend/service/private/SocketHandover.hpp"

#include <algorithm>
#include <mutex>
#include <thread>
#include <random>
#include <vector>

namespace
{
    //!< Returns a port of the loopback address that nobody listens on, or 0.
    //!< The range is outside the ephemeral ports of all platforms.
    uint16_t free_port()
    {
        std::random_device random;
        const uint16_t first{ static_cast<uint16_t>(26'000u + (random() % 3'000u)) };
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

    //!< The owner of the sockets; keeps what the connection hands back.
    class Released final : public areg::ext::SocketRelease
    {
    public:
        void release_socket(ITEM_ID cookie, areg::SocketAccepted && socket) override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            mCookies.push_back(cookie);
            mSockets.push_back(std::move(socket));
        }

        std::vector<ITEM_ID> cookies()
        {
            std::lock_guard<std::mutex> guard(mMutex);
            return mCookies;
        }

    private:
        std::mutex                          mMutex;
        std::vector<ITEM_ID>                mCookies;
        std::vector<areg::SocketAccepted>   mSockets;
    };

    //!< A client of the connection, connected to its listening socket.
    struct Client
    {
        areg::ClientConnection  link;

        explicit Client(uint16_t port)
            : link(areg::String("127.0.0.1"), port)
        {
        }

        ~Client()
        {
            link.close_socket();
        }

        bool connect()
        {
            return link.create_socket();
        }
    };

    //!< A listening server connection and the sockets it accepted.
    struct Listening
    {
        uint16_t                    port    { free_port() };
        areg::ext::ServerConnection server  { areg::COOKIE_ROUTER, "127.0.0.1", port };
        Released                    released{ };

        bool open()
        {
            return ((port != 0u) && server.create_socket() && server.server_listen());
        }

        //!< Takes the next client waiting to connect. Returns an invalid socket on failure.
        areg::SocketAccepted take()
        {
            areg::SocketAddress address{ };
            const SOCKETHANDLE handle{ server.wait_connection(address) };
            return (areg::is_valid_socket(handle) ? areg::SocketAccepted(handle, address) : areg::SocketAccepted());
        }

        ~Listening()
        {
            server.close_all_connections();
            server.close_socket();
        }
    };

    //!< Reads the state a connection notification of the server carries.
    bool read_state(Client & client, areg::ServiceConnectionState & state, ITEM_ID & cookie)
    {
        areg::MessageEnvelope message;
        if ((client.link.receive_message(message) <= 0) || (areg::is_notify_client(message) == false))
            return false;

        message >> cookie;
        message >> state;
        return true;
    }
}

// A connection keeps the channel and the address it is created with, and is not valid before its
// socket is created.
TEST(ServiceCommConnectionTest, connection_keeps_its_channel_and_address)
{
    areg::ext::ServerConnection byName(areg::COOKIE_ROUTER, "127.0.0.1", 4242u);
    EXPECT_EQ(byName.channel_id(), areg::COOKIE_ROUTER);
    EXPECT_EQ(byName.address().host_port(), 4242u);
    EXPECT_FALSE(byName.is_valid());

    areg::SocketAddress address{ };
    ASSERT_TRUE(address.resolve_address(areg::String("127.0.0.1"), 4243u, true));
    areg::ext::ServerConnection byAddress(areg::COOKIE_LOGGER, address);
    EXPECT_EQ(byAddress.channel_id(), areg::COOKIE_LOGGER);
    EXPECT_EQ(byAddress.address().host_port(), 4243u);
    EXPECT_FALSE(byAddress.is_valid());

    areg::ext::ServerConnection byChannel(areg::COOKIE_ROUTER);
    EXPECT_FALSE(byChannel.address().is_valid());
}

// An accepted client gets its own cookie, is found by it, is listed by the batch lookup, and is
// handed back to its owner with the cookie when its connection is closed.
TEST(ServiceCommConnectionTest, accepted_client_is_found_by_cookie_and_handed_back_when_closed)
{
    Listening listening;
    ASSERT_TRUE(listening.open());
    Client client(listening.port);
    ASSERT_TRUE(client.connect());
    areg::SocketAccepted accepted{ listening.take() };
    ASSERT_TRUE(accepted.is_valid());

    ASSERT_TRUE(listening.server.accept_connection(accepted, listening.released));
    const ITEM_ID cookie{ listening.server.cookie(accepted) };
    EXPECT_GE(cookie, areg::COOKIE_REMOTE_SERVICE);
    EXPECT_TRUE(listening.server.cookie_exist(cookie));
    EXPECT_TRUE(listening.server.handle_exist(accepted.handle()));
    EXPECT_EQ(listening.server.handle_by_cookie(cookie), accepted.handle());

    const ITEM_ID unknown{ cookie + 1'000u };
    const ITEM_ID cookies[2]{ cookie, unknown };
    SOCKETHANDLE handles[2]{ };
    areg::ext::HeldSocket holds[2]{ };
    uint32_t holdCount{ 9u };
    listening.server.batch_handles_by_cookies(cookies, handles, 2u, holds, holdCount);
    EXPECT_EQ(handles[0], accepted.handle());
    EXPECT_FALSE(areg::is_valid_socket(handles[1]));
    EXPECT_EQ(holdCount, 1u);
    EXPECT_EQ(holds[0].socket, accepted.handle());

    // A socket that was not accepted cannot be moved, and an unknown cookie closes nothing.
    const areg::SocketAccepted stranger{ };
    bool moved{ false };
    EXPECT_FALSE(listening.server.move_serving(stranger, listening.released, [&moved]() { moved = true; }));
    EXPECT_FALSE(moved);
    listening.server.close_connection(unknown);
    EXPECT_TRUE(listening.released.cookies().empty());

    listening.server.close_connection(cookie);
    ASSERT_EQ(listening.released.cookies().size(), 1u);
    EXPECT_EQ(listening.released.cookies().front(), cookie);
    EXPECT_FALSE(listening.server.cookie_exist(cookie));
    EXPECT_FALSE(listening.server.handle_exist(accepted.handle()));
}

// Reproduces the defect: closing a connection from a thread that does not own the multiplexer unregistered the socket there.
TEST(ServiceCommConnectionTest, connection_closed_by_another_thread_stays_registered_until_its_owner_releases_it)
{
    Listening listening;
    ASSERT_TRUE(listening.open());
    for (const bool byCookie : { true, false })
    {
        SCOPED_TRACE(byCookie);
        Client client(listening.port);
        ASSERT_TRUE(client.connect());
        areg::SocketAccepted accepted{ listening.take() };
        ASSERT_TRUE(accepted.is_valid());
        ASSERT_TRUE(listening.server.accept_connection(accepted, listening.released));
        const ITEM_ID cookie{ listening.server.cookie(accepted) };
        const SOCKETHANDLE handle{ accepted.handle() };
        areg::SocketMultiplexer & mux{ listening.server.multiplexer() };
        ASSERT_TRUE(mux.is_registered(handle));

        std::thread closer([&listening, &accepted, cookie, byCookie]()
        {
            if (byCookie)
                listening.server.close_connection(cookie);
            else
                listening.server.close_connection(accepted);
        });
        closer.join();

        EXPECT_FALSE(listening.server.cookie_exist(cookie));
        ASSERT_FALSE(listening.released.cookies().empty());
        EXPECT_EQ(listening.released.cookies().back(), cookie);
        EXPECT_TRUE(mux.is_registered(handle));
        EXPECT_TRUE(mux.unregister_socket(handle));
        EXPECT_FALSE(mux.is_registered(handle));
    }
}

// An accepted client can be moved to another owner, which gets the socket when it is closed.
TEST(ServiceCommConnectionTest, moved_client_is_handed_back_to_its_new_owner)
{
    Listening listening;
    Released other;
    ASSERT_TRUE(listening.open());
    Client client(listening.port);
    ASSERT_TRUE(client.connect());
    areg::SocketAccepted accepted{ listening.take() };
    ASSERT_TRUE(accepted.is_valid());
    ASSERT_TRUE(listening.server.accept_connection(accepted, listening.released));
    const ITEM_ID cookie{ listening.server.cookie(accepted) };

    bool moved{ false };
    EXPECT_TRUE(listening.server.move_serving(accepted, other, [&moved]() { moved = true; }));
    EXPECT_TRUE(moved);

    listening.server.close_connection(accepted);
    EXPECT_TRUE(listening.released.cookies().empty());
    ASSERT_EQ(other.cookies().size(), 1u);
    EXPECT_EQ(other.cookies().front(), cookie);
}

// A client that is not accepted is told that it is rejected, and nothing is handed back.
TEST(ServiceCommConnectionTest, rejected_client_is_told_so_and_not_handed_back)
{
    Listening listening;
    ASSERT_TRUE(listening.open());
    Client client(listening.port);
    ASSERT_TRUE(client.connect());
    areg::SocketAccepted pending{ listening.take() };
    ASSERT_TRUE(pending.is_valid());

    listening.server.reject_connection(pending);
    ITEM_ID cookie{ areg::COOKIE_REMOTE_SERVICE };
    areg::ServiceConnectionState state{ areg::ServiceConnectionState::Connected };
    ASSERT_TRUE(read_state(client, state, cookie));
    EXPECT_EQ(state, areg::ServiceConnectionState::Rejected);
    EXPECT_TRUE(listening.released.cookies().empty());

    pending.close();
    areg::MessageEnvelope rest;
    EXPECT_LE(client.link.receive_message(rest), 0);
}

// The close of all connections tells every client that it is disconnected, hands every socket
// back with its cookie, and leaves no client in the maps.
TEST(ServiceCommConnectionTest, closing_all_connections_tells_every_client)
{
    Listening listening;
    ASSERT_TRUE(listening.open());
    Client first(listening.port);
    ASSERT_TRUE(first.connect());
    areg::SocketAccepted firstAccepted{ listening.take() };
    ASSERT_TRUE(firstAccepted.is_valid());
    ASSERT_TRUE(listening.server.accept_connection(firstAccepted, listening.released));
    Client second(listening.port);
    ASSERT_TRUE(second.connect());
    areg::SocketAccepted secondAccepted{ listening.take() };
    ASSERT_TRUE(secondAccepted.is_valid());
    ASSERT_TRUE(listening.server.accept_connection(secondAccepted, listening.released));
    const ITEM_ID firstCookie{ listening.server.cookie(firstAccepted) };
    const ITEM_ID secondCookie{ listening.server.cookie(secondAccepted) };
    EXPECT_NE(firstCookie, secondCookie);

    listening.server.close_all_connections();

    ITEM_ID told{ areg::COOKIE_UNKNOWN };
    areg::ServiceConnectionState state{ areg::ServiceConnectionState::Connected };
    ASSERT_TRUE(read_state(first, state, told));
    EXPECT_EQ(state, areg::ServiceConnectionState::Disconnected);
    EXPECT_EQ(told, firstCookie);
    ASSERT_TRUE(read_state(second, state, told));
    EXPECT_EQ(state, areg::ServiceConnectionState::Disconnected);
    EXPECT_EQ(told, secondCookie);

    std::vector<ITEM_ID> handedBack{ listening.released.cookies() };
    std::sort(handedBack.begin(), handedBack.end());
    std::vector<ITEM_ID> expected{ firstCookie, secondCookie };
    std::sort(expected.begin(), expected.end());
    EXPECT_EQ(handedBack, expected);
    EXPECT_FALSE(listening.server.cookie_exist(firstCookie));
    EXPECT_FALSE(listening.server.cookie_exist(secondCookie));
}

// A message reaches the client it is sent to; a message without a header sends nothing.
TEST(ServiceCommConnectionTest, message_reaches_its_client_and_a_message_without_header_sends_nothing)
{
    Listening listening;
    ASSERT_TRUE(listening.open());
    Client client(listening.port);
    ASSERT_TRUE(client.connect());
    areg::SocketAccepted accepted{ listening.take() };
    ASSERT_TRUE(accepted.is_valid());
    ASSERT_TRUE(listening.server.accept_connection(accepted, listening.released));
    const ITEM_ID cookie{ listening.server.cookie(accepted) };

    const areg::MessageEnvelope empty;
    EXPECT_EQ(listening.server.send_message(empty, accepted.handle()), 0);

    const areg::MessageEnvelope message{ areg::create_connect_notify(areg::COOKIE_ROUTER, cookie) };
    EXPECT_GT(listening.server.send_message(message, accepted), 0);
    areg::MessageEnvelope received;
    ASSERT_GT(client.link.receive_message(received), 0);
    EXPECT_EQ(received.target(), static_cast<uint32_t>(cookie));
    EXPECT_EQ(received.message_id(), message.message_id());
}
