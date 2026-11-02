/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SocketAddressTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of SocketAddress: an address that is given or accepted
 *              as an IP keeps the IP as its host name, and a host name stays as given.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketClient.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/base/SocketServer.hpp"
#include "areg/base/String.hpp"

#include <array>
#include <utility>

#if defined(_POSIX)
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <sys/socket.h>
#elif defined(WINDOWS)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#endif

namespace
{
    //!< The loopback address of every test.
    constexpr std::string_view  LOOPBACK    { "127.0.0.1" };

    //!< The port given to the addresses that are not connected.
    constexpr uint16_t          ANY_PORT    { 41'800u };

    //!< Accepts one loopback connection and returns the address the accept filled in.
    bool accept_one(areg::SocketAddress & accepted)
    {
        for (uint16_t port = 41'801u; port < 41'901u; ++port)
        {
            const SOCKETHANDLE listener{ areg::server_connect(areg::String(LOOPBACK), port) };
            if (areg::is_valid_socket(listener) == false)
                continue;

            SOCKETHANDLE client{ areg::InvalidSocketHandle };
            SOCKETHANDLE server{ areg::InvalidSocketHandle };
            if (areg::server_listen(listener))
            {
                areg::SocketAddress address{ };
                client = address.resolve_address(areg::String(LOOPBACK), port, false) ? areg::socket_create() : areg::InvalidSocketHandle;
                if (areg::is_valid_socket(client) && areg::client_connect_fd(client, address))
                {
                    const SOCKETHANDLE none{ areg::InvalidSocketHandle };
                    server = areg::server_accept(listener, &none, 0, &accepted);
                }
            }

            const bool result{ areg::is_valid_socket(server) };
            if (result)
                areg::socket_close(server);
            if (areg::is_valid_socket(client))
                areg::socket_close(client);
            areg::socket_close(listener);
            if (result)
                return true;
        }

        return false;
    }
}

/**
 * \brief   An accepted connection has the IP of the peer as its host name.
 **/
TEST(SocketAddressTest, AcceptedAddressNamesTheIp)
{
    areg::SocketAddress accepted{ };
    ASSERT_TRUE(accept_one(accepted));
    EXPECT_EQ(accepted.host_address(), areg::String(LOOPBACK));
    EXPECT_EQ(accepted.host_name(), accepted.host_address());
}

/**
 * \brief   An address resolved from an IP has that IP as its host name.
 **/
TEST(SocketAddressTest, ResolvedIpNamesTheIp)
{
    areg::SocketAddress address{ };
    ASSERT_TRUE(address.resolve_address(areg::String(LOOPBACK), ANY_PORT, false));
    EXPECT_EQ(address.host_address(), areg::String(LOOPBACK));
    EXPECT_EQ(address.host_name(), areg::String(LOOPBACK));
}

/**
 * \brief   An address constructed from an IP has that IP as its host name.
 **/
TEST(SocketAddressTest, ConstructedIpNamesTheIp)
{
    const areg::SocketAddress address{ areg::String(LOOPBACK), ANY_PORT };
    EXPECT_EQ(address.host_address(), areg::String(LOOPBACK));
    EXPECT_EQ(address.host_name(), areg::String(LOOPBACK));
}

/**
 * \brief   An address resolved from a host name keeps that name and gets its IP.
 **/
TEST(SocketAddressTest, ResolvedNameStaysTheName)
{
    areg::SocketAddress address{ };
    ASSERT_TRUE(address.resolve_address(areg::String("localhost"), ANY_PORT, false));
    EXPECT_EQ(address.host_name(), areg::String("localhost"));
    EXPECT_FALSE(address.host_address().is_empty());
    EXPECT_TRUE(address.is_equal(areg::String("localhost"), ANY_PORT));
    EXPECT_TRUE(address.is_equal(address.host_address(), ANY_PORT));
}

namespace
{
    using OwnedSocket = areg::Socket::SocketHandle;

    constexpr int32_t SOCKET_DEADLINE_MS{ 30'000 };

    bool set_io_deadline(SOCKETHANDLE socket)
    {
#if defined(WINDOWS)
        const DWORD timeout{ static_cast<DWORD>(SOCKET_DEADLINE_MS) };
#else
        const timeval timeout{ SOCKET_DEADLINE_MS / 1000, 0 };
#endif
        return areg::set_send_timeout(socket, SOCKET_DEADLINE_MS)
            && (::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout)) == 0);
    }

    class SocketLoopbackTest : public testing::Test
    {
    protected:
        void SetUp() override
        {
            ASSERT_TRUE(areg::socket_initialize());
        }

        void TearDown() override
        {
            areg::socket_release();
        }
    };

    class ReceiveModeGuard
    {
    public:
        explicit ReceiveModeGuard(areg::ReceiveMode mode)
            : mSaved(areg::receive_mode())
        {
            areg::set_receive_mode(mode);
        }

        ~ReceiveModeGuard()
        {
            areg::set_receive_mode(mSaved);
        }

    private:
        areg::ReceiveMode mSaved;
    };

    bool bind_loopback(OwnedSocket& listener, areg::SocketAddress& address, bool listen = true)
    {
        listener = OwnedSocket(areg::socket_create());
        if (!areg::is_valid_socket(listener.value()))
            return false;

        sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        local.sin_port = 0;
        if (::bind(listener.value(), reinterpret_cast<sockaddr*>(&local), sizeof(local)) != 0)
            return false;

#if defined(WINDOWS)
        int length{ static_cast<int>(sizeof(local)) };
#else
        socklen_t length{ sizeof(local) };
#endif
        if (::getsockname(listener.value(), reinterpret_cast<sockaddr*>(&local), &length) != 0)
            return false;

        address.from_sockaddr(local);
        return !listen || areg::server_listen(listener.value(), 8);
    }

    SOCKETHANDLE accept_pending(SOCKETHANDLE listener, areg::SocketAddress* peer = nullptr)
    {
        areg::SocketMultiplexer mux;
        if (!mux.register_socket(listener, true))
            return areg::InvalidSocketHandle;

        const SOCKETHANDLE accepted{ areg::server_accept(mux, listener, peer, SOCKET_DEADLINE_MS) };
        if (areg::is_valid_socket(accepted) && !set_io_deadline(accepted))
        {
            areg::socket_close(accepted);
            return areg::InvalidSocketHandle;
        }

        return accepted;
    }
}

// IPv4 validation accepts valid octets and rejects malformed addresses.
TEST(SocketAddressCoverageTest, rejects_malformed_ipv4_text)
{
    for (const char* valid : { "0.0.0.0", "127.0.0.1", "255.255.255.255" })
        EXPECT_TRUE(areg::is_ip_address(areg::String(valid))) << valid;

    for (const char* invalid : { "", "1.2.3", ".1.2.3.4", "1..2.3.4", "1.2.3.4.5"
                              , "256.0.0.1", "127.0.0.999", "127.0.0.1.", "127.0.a.1"
                              , "1234.1234.1234.1234" })
        EXPECT_FALSE(areg::is_ip_address(areg::String(invalid))) << invalid;

    EXPECT_TRUE(areg::is_local_address(areg::String("localhost")));
    EXPECT_TRUE(areg::is_local_address(areg::String("127.0.0.1")));
    EXPECT_FALSE(areg::is_local_address(areg::String("192.0.2.1")));
}

// Port limits retain their meaning when converted to and from a native address.
TEST(SocketAddressCoverageTest, port_limits_and_native_address_round_trip)
{
    areg::SocketAddress address;
    EXPECT_FALSE(address.is_valid());
    sockaddr_in native{};
    EXPECT_FALSE(address.to_sockaddr(native));
    ASSERT_TRUE(address.resolve_address(areg::String(LOOPBACK), 0u, true));
    EXPECT_EQ(address.host_port(), 0u);
    EXPECT_FALSE(address.is_valid());
    EXPECT_FALSE(address.to_sockaddr(native));

    ASSERT_TRUE(address.resolve_address(areg::String(LOOPBACK), 65535u, false));
    ASSERT_TRUE(address.to_sockaddr(native));
    EXPECT_EQ(native.sin_family, AF_INET);
    EXPECT_EQ(areg::extract_ip_address(native), areg::String(LOOPBACK));
    EXPECT_EQ(areg::extract_port_number(native), 65535u);
    areg::SocketAddress restored;
    restored.from_sockaddr(native);
    EXPECT_EQ(restored, address);
    EXPECT_EQ(restored.host_name(), address.host_address());
}

// Copying, moving, comparing, and resetting an endpoint preserve its value semantics.
TEST(SocketAddressCoverageTest, copies_moves_compares_and_resets_endpoints)
{
    const areg::SocketAddress original{ areg::String(LOOPBACK), 65535u };
    areg::SocketAddress copy(original);
    EXPECT_EQ(copy, original);
    EXPECT_TRUE(copy == copy);
    EXPECT_FALSE(copy != copy);
    EXPECT_NE(copy, areg::SocketAddress(areg::String("127.0.0.2"), 65535u));
    EXPECT_NE(copy, areg::SocketAddress(areg::String(LOOPBACK), 1u));
    EXPECT_FALSE(copy.is_equal(areg::String(LOOPBACK), 1u));
    EXPECT_FALSE(copy.is_equal(areg::String("other"), 65535u));

    areg::SocketAddress moved(std::move(copy));
    copy = original;
    areg::SocketAddress assigned;
    assigned = std::move(moved);
    EXPECT_EQ(assigned, original);
    EXPECT_EQ(copy, original);
    assigned.reset();
    EXPECT_FALSE(assigned.is_valid());
    EXPECT_TRUE(assigned.host_address().is_empty());
    EXPECT_TRUE(assigned.host_name().is_empty());
    EXPECT_EQ(assigned.host_port(), areg::InvalidPort);
}

// Local names resolve to IPv4 addresses, while unsupported host text is rejected.
TEST_F(SocketLoopbackTest, resolves_local_names_and_rejects_non_ipv4_hosts)
{
    const areg::String name("localhost");
    const areg::String invalid("::1");
    const areg::SocketAddress constructed(name, 65535u);
    EXPECT_TRUE(constructed.is_valid());
    EXPECT_EQ(constructed.host_name(), name);
    EXPECT_TRUE(areg::is_ip_address(constructed.host_address()));
    EXPECT_EQ(areg::host_to_ip(name), constructed.host_address());
    EXPECT_EQ(areg::host_to_ip(invalid), invalid);
    EXPECT_EQ(areg::ip_to_host(invalid), invalid);
    EXPECT_FALSE(areg::ip_to_host(areg::String(LOOPBACK)).is_empty());
    EXPECT_FALSE(areg::hostname().is_empty());
    EXPECT_EQ(&areg::hostname(), &areg::hostname());

    areg::SocketAddress address(constructed);
    ASSERT_TRUE(address.resolve_address(areg::String(), 65535u, true));
    EXPECT_EQ(address.host_name(), name);
    EXPECT_TRUE(areg::is_local_address(address.host_address()));
    EXPECT_FALSE(address.resolve_address(invalid, 65535u, false));
    EXPECT_FALSE(address.is_valid());
    EXPECT_TRUE(address.host_address().is_empty());
    EXPECT_TRUE(address.host_name().is_empty());
}

// Copied and moved credentials retain their values and validity.
TEST(SocketAddressCoverageTest, user_credentials_preserve_value_semantics)
{
    areg::UserData empty;
    EXPECT_FALSE(empty.is_valid());
    EXPECT_TRUE(empty.user().is_empty());
    EXPECT_TRUE(empty.password().is_empty());
    const areg::UserData original(areg::String("reader"), areg::String("secret"));
    areg::UserData copy(original);
    areg::UserData moved(std::move(copy));
    EXPECT_EQ(moved, original);
    copy = original;
    empty = std::move(moved);
    EXPECT_EQ(empty, original);
    EXPECT_EQ(copy, original);
    EXPECT_TRUE(empty.is_valid());
    empty.set_password(areg::String("changed"));
    EXPECT_NE(empty, original);
    empty.set_password(original.password());
    empty.set_user(areg::String("writer"));
    EXPECT_NE(empty, original);
    empty.set_user(areg::String());
    EXPECT_FALSE(empty.is_valid());
}

// Invalid socket handles and unconfigured socket objects report failure safely.
TEST_F(SocketLoopbackTest, invalid_handles_and_unconfigured_objects_report_failure)
{
    uint8_t byte{};
    const areg::IoBuffer buffer{ &byte, 1u };
    for (const SOCKETHANDLE socket : { areg::InvalidSocketHandle, areg::FailedSocketHandle })
    {
        EXPECT_FALSE(areg::is_valid_socket(socket));
        EXPECT_FALSE(areg::is_socket_alive(socket));
        EXPECT_FALSE(areg::set_send_timeout(socket, 0u));
        EXPECT_FALSE(areg::disable_send(socket));
        EXPECT_FALSE(areg::disable_receive(socket));
        EXPECT_FALSE(areg::client_connect_fd(socket, areg::SocketAddress()));
        EXPECT_EQ(areg::pending_read(socket), 0u);
        EXPECT_LT(areg::send_data(socket, &byte, 1u), 0);
        EXPECT_LT(areg::send_data_v(socket, &buffer, 1u), 0);
        EXPECT_LT(areg::try_send_data_v(socket, &buffer, 1u), 0);
        EXPECT_LT(areg::receive_data(socket, &byte, 1u), 0);
        EXPECT_LT(areg::receive_data_window(socket, &byte, 1u), 0);
        EXPECT_LT(areg::receive_data_nowait(socket, &byte, 1u), 0);
        EXPECT_LT(areg::recv_fill_nowait(socket), 0);
        areg::ThreadCache cache;
        EXPECT_LT(areg::recv_fill_nowait(socket, cache), 0);
        areg::socket_interrupt(socket);
        areg::socket_close(socket);
    }

    areg::SocketClient client;
    EXPECT_FALSE(client.create());
    EXPECT_FALSE(client.create_fd());
    EXPECT_FALSE(client.connect_to());
    EXPECT_LT(client.send(&byte, 1), 0);
    EXPECT_LT(client.receive(&byte, 1), 0);
    EXPECT_EQ(client.pending_read(), -1);
    EXPECT_FALSE(client.disable_send());
    EXPECT_FALSE(client.disable_receive());
    EXPECT_EQ(client.send_packet_size(), areg::PACKET_INVALID_SIZE);
    EXPECT_EQ(client.recv_packet_size(), areg::PACKET_INVALID_SIZE);
    areg::SocketServer server;
    areg::SocketMultiplexer mux;
    areg::SocketAddress peer;
    EXPECT_FALSE(server.create());
    EXPECT_FALSE(server.listen(0));
    EXPECT_EQ(server.wait_connection_event(mux, peer), areg::FailedSocketHandle);
    EXPECT_EQ(server.wait_connection_nowait(mux, peer), areg::FailedSocketHandle);
    EXPECT_EQ(server.wait_connection_event(peer, nullptr, 0), areg::FailedSocketHandle);
    EXPECT_FALSE(areg::server_listen(areg::InvalidSocketHandle));
    EXPECT_EQ(areg::client_connect(peer), areg::InvalidSocketHandle);
    EXPECT_EQ(areg::server_connect(peer), areg::InvalidSocketHandle);
}

// An empty send list does not attempt to write to an unconnected socket.
TEST_F(SocketLoopbackTest, empty_send_lists_do_not_write_to_an_unconnected_socket)
{
    const OwnedSocket socket(areg::socket_create());
    ASSERT_TRUE(areg::is_valid_socket(socket.value()));
    uint8_t byte{};
    const areg::IoBuffer buffer{ &byte, 1u };
    EXPECT_EQ(areg::send_data(socket.value(), nullptr, 1u), 0);
    EXPECT_EQ(areg::send_data(socket.value(), &byte, 0u), 0);
    EXPECT_EQ(areg::send_data_v(socket.value(), nullptr, 1u), 0);
    EXPECT_EQ(areg::send_data_v(socket.value(), &buffer, 0u), 0);
    EXPECT_EQ(areg::try_send_data_v(socket.value(), nullptr, 1u), 0);
    EXPECT_EQ(areg::try_send_data_v(socket.value(), &buffer, 0u), 0);
    EXPECT_FALSE(areg::client_connect_fd(socket.value(), areg::SocketAddress()));

    areg::SocketAddress stale(areg::String(LOOPBACK), 65535u);
    EXPECT_FALSE(stale.resolve_socket(socket.value()));
    EXPECT_FALSE(stale.is_valid());
    EXPECT_TRUE(stale.host_name().is_empty());
    EXPECT_FALSE(stale.resolve_socket(areg::InvalidSocketHandle));
}

// A refused connection can be retried once the listener starts.
TEST_F(SocketLoopbackTest, refused_connection_can_be_retried_after_listening)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address, false));
    EXPECT_EQ(areg::client_connect(address), areg::InvalidSocketHandle);
    areg::SocketAddress output(address);
    EXPECT_EQ(areg::client_connect(areg::String(LOOPBACK), address.host_port(), &output), areg::InvalidSocketHandle);
    EXPECT_FALSE(output.is_valid());

    areg::SocketClient client(address);
    ASSERT_TRUE(client.create_fd());
    const SOCKETHANDLE failedHandle{ client.handle() };
    EXPECT_FALSE(client.connect_to());
    EXPECT_EQ(client.handle(), failedHandle);
    client.close();
    ASSERT_TRUE(areg::server_listen(listener.value()));
    ASSERT_TRUE(client.create_fd());
    ASSERT_TRUE(client.connect_to());
    const OwnedSocket accepted(accept_pending(listener.value()));
    EXPECT_TRUE(areg::is_valid_socket(accepted.value()));
    EXPECT_TRUE(client.is_valid());
    EXPECT_TRUE(client.is_alive());
}

// A second listener cannot bind a port that is already in use.
TEST_F(SocketLoopbackTest, second_server_cannot_bind_an_occupied_listening_port)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    EXPECT_EQ(areg::server_connect(address), areg::InvalidSocketHandle);
    areg::SocketAddress output(address);
    EXPECT_EQ(areg::server_connect(areg::String(LOOPBACK), address.host_port(), &output), areg::InvalidSocketHandle);
    EXPECT_FALSE(output.is_valid());

    areg::SocketServer byAddress(address);
    areg::SocketServer byString(address.host_address(), address.host_port());
    areg::SocketServer byText(address.host_address().as_string(), address.host_port());
    EXPECT_EQ(byAddress.address(), address);
    EXPECT_EQ(byString.address(), address);
    EXPECT_EQ(byText.address(), address);
    EXPECT_FALSE(byAddress.create());
    EXPECT_FALSE(byString.create(address.host_address(), address.host_port()));
    EXPECT_FALSE(byText.create(address.host_address().as_string(), address.host_port()));
    EXPECT_FALSE(byAddress.is_valid());
}

// Connection overloads resolve the peer and deliver the exact payload.
TEST_F(SocketLoopbackTest, connection_overloads_resolve_peer_and_deliver_exact_bytes)
{
    ReceiveModeGuard mode(areg::ReceiveMode::NoCache);
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    areg::SocketAddress output;
    const OwnedSocket client(areg::client_connect(areg::String(), address.host_port(), &output));
    ASSERT_TRUE(areg::is_valid_socket(client.value()));
    ASSERT_TRUE(set_io_deadline(client.value()));
    EXPECT_EQ(output, address);
    areg::SocketAddress peer;
    const OwnedSocket accepted(accept_pending(listener.value(), &peer));
    ASSERT_TRUE(areg::is_valid_socket(accepted.value()));
    EXPECT_EQ(peer.host_address(), areg::String(LOOPBACK));
    EXPECT_NE(peer.host_port(), 0u);
    areg::SocketAddress resolved;
    ASSERT_TRUE(resolved.resolve_socket(client.value()));
    EXPECT_EQ(resolved, address);
    ASSERT_TRUE(resolved.resolve_socket(accepted.value()));
    EXPECT_EQ(resolved, peer);

    const std::array<uint8_t, 9> sent{ 0u, 1u, 2u, 0u, 255u, 4u, 5u, 6u, 7u };
    ASSERT_EQ(areg::send_data(client.value(), sent.data(), 3u), 3);
    ASSERT_EQ(areg::send_data(client.value(), sent.data() + 3u, 6u), 6);
    areg::SocketMultiplexer mux;
    ASSERT_TRUE(mux.register_socket(accepted.value(), true));
    ASSERT_EQ(mux.wait(SOCKET_DEADLINE_MS), accepted.value());
    EXPECT_GT(areg::pending_read(accepted.value()), 0u);
    std::array<uint8_t, 9> received{};
    ASSERT_EQ(areg::receive_data(accepted.value(), received.data(), 2u), 2);
    ASSERT_EQ(areg::receive_data(accepted.value(), received.data() + 2u, 7u), 7);
    EXPECT_EQ(received, sent);
    EXPECT_EQ(areg::pending_read(accepted.value()), 0u);
}

// Copied and moved accepted sockets keep the connection alive until its last owner closes.
TEST_F(SocketLoopbackTest, accepted_copies_and_moves_keep_the_connection_until_last_close)
{
    ReceiveModeGuard mode(areg::ReceiveMode::NoCache);
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    areg::SocketClient client(address.host_address().as_string(), address.host_port());
    ASSERT_TRUE(client.create());
    ASSERT_TRUE(set_io_deadline(client.handle()));
    areg::SocketAddress peer;
    areg::SocketAccepted accepted(accept_pending(listener.value(), &peer), std::move(peer));
    ASSERT_TRUE(accepted.is_valid());
    const SOCKETHANDLE handle{ accepted.handle() };
    areg::SocketAccepted copy(accepted);
    areg::SocketAccepted moved(std::move(copy));
    EXPECT_FALSE(copy.is_valid());
    EXPECT_TRUE(accepted == moved);
    EXPECT_TRUE(accepted == accepted);
    areg::SocketAccepted assigned;
    assigned = accepted;
    copy = std::move(assigned);
    EXPECT_FALSE(assigned.is_valid());
    EXPECT_FALSE(assigned == accepted);
    areg::Socket& base{ moved };
    EXPECT_TRUE(base.create());
    EXPECT_TRUE(base.create(areg::String("localhost"), 1u));
    EXPECT_TRUE(base.create("localhost", 1u));
    EXPECT_EQ(moved.handle(), handle);
    accepted.close();
    moved.close();
    uint8_t sent{ 0xA5u };
    ASSERT_EQ(client.send(&sent, 1), 1);
    uint8_t received{};
    ASSERT_EQ(copy.receive(&received, 1), 1);
    EXPECT_EQ(received, sent);
    ASSERT_EQ(copy.send(&received, 1), 1);
    ASSERT_EQ(client.receive(&received, 1), 1);
    EXPECT_EQ(received, sent);
    copy.close();
    EXPECT_FALSE(copy.is_valid());
}

// Changing a client's address releases only that client's shared handle.
TEST_F(SocketLoopbackTest, changing_client_address_releases_only_that_handle)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    areg::SocketClient client(address.host_address(), address.host_port());
    ASSERT_TRUE(client.create(address.host_address(), address.host_port()));
    const OwnedSocket accepted(accept_pending(listener.value()));
    ASSERT_TRUE(areg::is_valid_socket(accepted.value()));
    const SOCKETHANDLE handle{ client.handle() };
    ASSERT_TRUE(client.set_address(address.host_address(), address.host_port(), false));
    EXPECT_EQ(client.handle(), handle);
    const uint16_t differentPort{ static_cast<uint16_t>(address.host_port() == 65535u ? 1u : address.host_port() + 1u) };
    ASSERT_TRUE(client.set_address(address.host_address(), differentPort, false));
    EXPECT_FALSE(client.is_valid());
    EXPECT_EQ(client.address().host_port(), differentPort);
    ASSERT_TRUE(client.create(address.host_address().as_string(), address.host_port()));
    const OwnedSocket reaccepted(accept_pending(listener.value()));
    EXPECT_TRUE(areg::is_valid_socket(reaccepted.value()));
}

// Closing the send direction still allows data to arrive in the reverse direction.
TEST_F(SocketLoopbackTest, send_half_close_preserves_the_reverse_direction)
{
    ReceiveModeGuard mode(areg::ReceiveMode::NoCache);
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    const OwnedSocket client(areg::client_connect(address));
    ASSERT_TRUE(areg::is_valid_socket(client.value()));
    ASSERT_TRUE(set_io_deadline(client.value()));
    const OwnedSocket accepted(accept_pending(listener.value()));
    ASSERT_TRUE(areg::is_valid_socket(accepted.value()));
    uint8_t byte{ 0x7Fu };
    ASSERT_EQ(areg::send_data(client.value(), &byte, 1u), 1);
    ASSERT_TRUE(areg::disable_send(client.value()));
    uint8_t received{};
    ASSERT_EQ(areg::receive_data(accepted.value(), &received, 1u), 1);
    EXPECT_EQ(received, byte);
    EXPECT_LT(areg::receive_data_window(accepted.value(), &received, 1u), 0);
    EXPECT_LT(areg::send_data(client.value(), &byte, 1u), 0);
    ASSERT_EQ(areg::send_data(accepted.value(), &byte, 1u), 1);
    ASSERT_EQ(areg::receive_data(client.value(), &received, 1u), 1);
    EXPECT_EQ(received, byte);
    ASSERT_TRUE(areg::disable_receive(client.value()));
    EXPECT_LT(areg::receive_data_nowait(client.value(), &received, 1u), 0);
}

// Connected-socket options and buffer sizes match the operating system's read-back values.
TEST_F(SocketLoopbackTest, connected_options_and_buffer_sizes_match_native_readback)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    const OwnedSocket client(areg::client_connect(address));
    ASSERT_TRUE(areg::is_valid_socket(client.value()));
    const OwnedSocket accepted(accept_pending(listener.value()));
    ASSERT_TRUE(areg::is_valid_socket(accepted.value()));
    for (const SOCKETHANDLE socket : { client.value(), accepted.value() })
    {
        int noDelay{};
        int keepAlive{};
#if defined(WINDOWS)
        int length{ static_cast<int>(sizeof(int)) };
#else
        socklen_t length{ sizeof(int) };
#endif
        ASSERT_EQ(::getsockopt(socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&noDelay), &length), 0);
        length = sizeof(int);
        ASSERT_EQ(::getsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&keepAlive), &length), 0);
        EXPECT_EQ(noDelay, 1);
        EXPECT_EQ(keepAlive, 1);
        for (const uint32_t size : { 0u, 1u, areg::PACKET_MAX_SIZE })
        {
            (void)areg::set_send_size(socket, size);
            (void)areg::set_recv_size(socket, size);
            int sendSize{};
            int recvSize{};
            length = sizeof(int);
            ASSERT_EQ(::getsockopt(socket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&sendSize), &length), 0);
            length = sizeof(int);
            ASSERT_EQ(::getsockopt(socket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&recvSize), &length), 0);
            EXPECT_EQ(areg::max_send_size(socket), static_cast<uint32_t>(sendSize));
            EXPECT_EQ(areg::max_receive_size(socket), static_cast<uint32_t>(recvSize));
            EXPECT_GE(sendSize, static_cast<int32_t>(areg::PACKET_MIN_SIZE));
            EXPECT_GE(recvSize, static_cast<int32_t>(areg::PACKET_MIN_SIZE));
        }

        for (const uint32_t milliseconds : { 1234u, 0u })
        {
            ASSERT_TRUE(areg::set_send_timeout(socket, milliseconds));
#if defined(WINDOWS)
            DWORD timeout{};
            length = sizeof(timeout);
            ASSERT_EQ(::getsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout), &length), 0);
            EXPECT_EQ(timeout, milliseconds);
#else
            timeval timeout{};
            length = sizeof(timeout);
            ASSERT_EQ(::getsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, &length), 0);
            const int64_t actual{ static_cast<int64_t>(timeout.tv_sec) * 1000 + timeout.tv_usec / 1000 };
            EXPECT_GE(actual, milliseconds);
            EXPECT_LT(actual, milliseconds + 1000u);
#endif
        }
    }
}

// An orderly peer close must return zero in every receive mode.
TEST_F(SocketLoopbackTest, orderly_peer_close_returns_zero_in_every_receive_mode)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    for (const auto mode : { areg::ReceiveMode::NoCache, areg::ReceiveMode::MonoCache, areg::ReceiveMode::MultiCache })
    {
        ReceiveModeGuard guard(mode);
        const OwnedSocket client(areg::client_connect(address));
        ASSERT_TRUE(areg::is_valid_socket(client.value()));
        const OwnedSocket accepted(accept_pending(listener.value()));
        ASSERT_TRUE(areg::is_valid_socket(accepted.value()));
        ASSERT_TRUE(areg::disable_send(client.value()));
        uint8_t byte{};
        EXPECT_EQ(areg::receive_data(accepted.value(), &byte, 1u), 0) << static_cast<int32_t>(mode);
    }
}

// A peer close in the middle of a requested block is an error in every receive mode.
TEST_F(SocketLoopbackTest, peer_close_inside_a_block_is_an_error_in_every_receive_mode)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    for (const auto mode : { areg::ReceiveMode::NoCache, areg::ReceiveMode::MonoCache, areg::ReceiveMode::MultiCache })
    {
        ReceiveModeGuard guard(mode);
        const OwnedSocket client(areg::client_connect(address));
        ASSERT_TRUE(areg::is_valid_socket(client.value()));
        const OwnedSocket accepted(accept_pending(listener.value()));
        ASSERT_TRUE(areg::is_valid_socket(accepted.value()));
        const std::array<uint8_t, 3> sent{ 1u, 2u, 3u };
        ASSERT_EQ(areg::send_data(client.value(), sent.data(), 3u), 3);
        ASSERT_TRUE(areg::disable_send(client.value()));
        std::array<uint8_t, 8> received{};
        EXPECT_LT(areg::receive_data(accepted.value(), received.data(), 8u), 0) << static_cast<int32_t>(mode);
        areg::thread_rx_cache_release(accepted.value());
    }
}

// Legacy accept must allow a null pointer when there are no client sockets.
TEST_F(SocketLoopbackTest, legacy_accept_allows_null_for_an_empty_client_list)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    const OwnedSocket client(areg::client_connect(address));
    ASSERT_TRUE(areg::is_valid_socket(client.value()));
    areg::SocketAddress peer;
    EXPECT_EQ(areg::server_accept(listener.value(), nullptr, 1, &peer), areg::FailedSocketHandle);
    EXPECT_EQ(areg::server_accept(listener.value(), nullptr, -1, &peer), areg::FailedSocketHandle);
    EXPECT_FALSE(peer.is_valid());
    const OwnedSocket accepted(areg::server_accept(listener.value(), nullptr, 0, &peer));
    EXPECT_TRUE(areg::is_valid_socket(accepted.value()));
    EXPECT_TRUE(peer.is_valid());
    EXPECT_EQ(areg::SocketMultiplexer{}.wait(listener.value(), nullptr, 2, 0), areg::FailedSocketHandle);
}

// Accepting on an invalid listener must report a failed handle, not a timeout.
TEST_F(SocketLoopbackTest, accept_reports_invalid_listener_as_failed_handle)
{
    areg::SocketMultiplexer mux;
    areg::SocketAddress peer(areg::String(LOOPBACK), 1u);
    EXPECT_EQ(areg::server_accept(mux, areg::InvalidSocketHandle, &peer, 0), areg::FailedSocketHandle);
    EXPECT_FALSE(peer.is_valid());
    const SOCKETHANDLE none{ areg::InvalidSocketHandle };
    EXPECT_EQ(areg::server_accept(areg::InvalidSocketHandle, &none, 0), areg::FailedSocketHandle);
}

// Receiving into an empty buffer must report an error.
TEST_F(SocketLoopbackTest, empty_receive_buffer_reports_an_error)
{
    const OwnedSocket socket(areg::socket_create());
    ASSERT_TRUE(areg::is_valid_socket(socket.value()));
    uint8_t byte{ 0xA5u };
    EXPECT_LT(areg::receive_data(socket.value(), &byte, 0u), 0);
    EXPECT_LT(areg::receive_data(socket.value(), nullptr, 1u), 0);
    EXPECT_EQ(byte, 0xA5u);
}

// A peer that has reached EOF must no longer be reported alive.
TEST_F(SocketLoopbackTest, closed_peer_is_not_reported_alive_after_eof)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    OwnedSocket client(areg::client_connect(address));
    ASSERT_TRUE(areg::is_valid_socket(client.value()));
    const OwnedSocket accepted(accept_pending(listener.value()));
    ASSERT_TRUE(areg::is_valid_socket(accepted.value()));
    EXPECT_TRUE(areg::is_socket_alive(accepted.value()));
    client.reset();
    uint8_t byte{};
    EXPECT_LT(areg::receive_data_window(accepted.value(), &byte, 1u), 0);
    EXPECT_FALSE(areg::is_socket_alive(accepted.value()));
}

// A closed peer stays alive while its unread data waits, and the query consumes none of it.
TEST_F(SocketLoopbackTest, closed_peer_with_unread_data_stays_alive_until_read)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    OwnedSocket client(areg::client_connect(address));
    ASSERT_TRUE(areg::is_valid_socket(client.value()));
    const OwnedSocket accepted(accept_pending(listener.value()));
    ASSERT_TRUE(areg::is_valid_socket(accepted.value()));
    const uint8_t sent{ 0x5Au };
    ASSERT_EQ(areg::send_data(client.value(), &sent, 1u), 1);
    client.reset();
    areg::SocketMultiplexer mux;
    ASSERT_TRUE(mux.register_socket(accepted.value(), true));
    ASSERT_EQ(mux.wait(SOCKET_DEADLINE_MS), accepted.value());
    EXPECT_TRUE(areg::is_socket_alive(accepted.value()));
    uint8_t byte{};
    ASSERT_EQ(areg::receive_data_window(accepted.value(), &byte, 1u), 1);
    EXPECT_EQ(byte, sent);
    EXPECT_EQ(areg::receive_data(accepted.value(), &byte, 1u), 0);
    EXPECT_FALSE(areg::is_socket_alive(accepted.value()));
}

// Readiness on an existing client returns that client and an invalid accept address.
TEST_F(SocketLoopbackTest, existing_client_readiness_invalidates_the_accept_address)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address));
    const OwnedSocket client(areg::client_connect(address));
    ASSERT_TRUE(areg::is_valid_socket(client.value()));
    const OwnedSocket accepted(accept_pending(listener.value()));
    ASSERT_TRUE(areg::is_valid_socket(accepted.value()));
    areg::SocketMultiplexer mux;
    ASSERT_TRUE(mux.register_socket(listener.value(), true));
    ASSERT_TRUE(mux.register_socket(accepted.value(), true));
    const uint8_t byte{ 0xA5u };
    ASSERT_EQ(areg::send_data(client.value(), &byte, 1u), 1);
    areg::SocketAddress output(address);
    EXPECT_EQ(areg::server_accept(mux, listener.value(), &output, SOCKET_DEADLINE_MS), accepted.value());
    EXPECT_FALSE(output.is_valid());
    output = address;
    const SOCKETHANDLE clients[]{ accepted.value() };
    EXPECT_EQ(areg::server_accept(listener.value(), clients, 1, &output), accepted.value());
    EXPECT_FALSE(output.is_valid());
}

#if defined(_POSIX)
// A refused connection must restore the socket's original blocking flags.
TEST_F(SocketLoopbackTest, refused_connect_restores_the_original_blocking_flags)
{
    OwnedSocket listener;
    areg::SocketAddress address;
    ASSERT_TRUE(bind_loopback(listener, address, false));
    const OwnedSocket client(areg::socket_create());
    ASSERT_TRUE(areg::is_valid_socket(client.value()));
    const int flags{ ::fcntl(client.value(), F_GETFL, 0) };
    ASSERT_NE(flags, -1);
    ASSERT_EQ(flags & O_NONBLOCK, 0);
    EXPECT_FALSE(areg::client_connect_fd(client.value(), address));
    EXPECT_EQ(::fcntl(client.value(), F_GETFL, 0), flags);
}
#endif
